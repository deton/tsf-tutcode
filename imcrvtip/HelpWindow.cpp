
#include "imcrvtip.h"
#include "TextService.h"
#include "HelpWindow.h"
#include "moji.h"
#include <sstream>

#define IM_MARGIN_X 2
#define IM_MARGIN_Y 2

class CHelpWindowGetTextExtEditSession : public CEditSessionBase
{
public:
	CHelpWindowGetTextExtEditSession(CTextService *pTextService, ITfContext *pContext,
		ITfContextView *pContextView, CHelpWindow *pHelpWindow) : CEditSessionBase(pTextService, pContext)
	{
		_pHelpWindow = pHelpWindow;
		_pContextView = pContextView;
	}

	~CHelpWindowGetTextExtEditSession()
	{
		_pHelpWindow.Release();
		_pContextView.Release();
	}

	// ITfEditSession
	STDMETHODIMP DoEditSession(TfEditCookie ec)
	{
		if (_pHelpWindow->_IsHide())
		{
			return S_OK;
		}

		TF_SELECTION tfSelection = {};
		ULONG cFetched = 0;
		if (FAILED(_pContext->GetSelection(ec, TF_DEFAULT_SELECTION, 1, &tfSelection, &cFetched)))
		{
			return E_FAIL;
		}

		CComPtr<ITfRange> pRangeSelection;
		pRangeSelection.Attach(tfSelection.range);

		if (cFetched != 1)
		{
			return E_FAIL;
		}

		RECT rc = {};
		BOOL fClipped;
		if (FAILED(_pContextView->GetTextExt(ec, tfSelection.range, &rc, &fClipped)))
		{
			return E_FAIL;
		}

		//ignore abnormal position (from CUAS ?)
		if ((rc.top == rc.bottom) && ((rc.right - rc.left) == 1))
		{
			return E_FAIL;
		}

		POINT pt = {};
		pt.x = rc.left;
		pt.y = rc.bottom;
		HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);

		MONITORINFO mi = {};
		mi.cbSize = sizeof(mi);
		GetMonitorInfoW(hMonitor, &mi);

		RECT rw = {};
		_pHelpWindow->_GetRect(&rw);
		LONG height = rw.bottom - rw.top;
		LONG width = rw.right - rw.left;

		if (rc.left < mi.rcWork.left)
		{
			rc.left = mi.rcWork.left;
		}

		if (mi.rcWork.right < rc.right)
		{
			rc.left = mi.rcWork.right - width;
		}

		if (mi.rcWork.bottom < rc.top)
		{
			rc.bottom = mi.rcWork.bottom - height - IM_MARGIN_Y;
		}
		else if (mi.rcWork.bottom < (rc.bottom + height + IM_MARGIN_Y))
		{
			rc.bottom = rc.top - height - IM_MARGIN_Y * 2;
		}

		if (rc.bottom < mi.rcWork.top)
		{
			rc.bottom = mi.rcWork.top - IM_MARGIN_Y;
		}

		_pHelpWindow->_Move(rc.left, rc.bottom + IM_MARGIN_Y);
		_pHelpWindow->_Show(TRUE);

		return S_OK;
	}

private:
	CComPtr<ITfContextView> _pContextView;
	CComPtr<CHelpWindow> _pHelpWindow;
};

CHelpWindow::CHelpWindow(): _bHide(FALSE)
{
	DllAddRef();

	_cRef = 1;

	_pContext = nullptr;
	_dwCookieTextLayoutSink= TF_INVALID_COOKIE;
	_pTextService = nullptr;
	_hwndParent = nullptr;
	_hwnd = nullptr;

	HDC hdc = GetDC(nullptr);
	_dpi = GetDeviceCaps(hdc, LOGPIXELSY);
	ReleaseDC(nullptr, hdc);
}

CHelpWindow::~CHelpWindow()
{
	_Destroy();

	DllRelease();
}

STDAPI CHelpWindow::QueryInterface(REFIID riid, void **ppvObj)
{
	if (ppvObj == nullptr)
	{
		return E_INVALIDARG;
	}

	*ppvObj = nullptr;

	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_ITfTextLayoutSink))
	{
		*ppvObj = static_cast<ITfTextLayoutSink *>(this);
	}

	if (*ppvObj)
	{
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDAPI_(ULONG) CHelpWindow::AddRef()
{
	return ++_cRef;
}

STDAPI_(ULONG) CHelpWindow::Release()
{
	if (--_cRef == 0)
	{
		delete this;
		return 0;
	}

	return _cRef;
}

STDAPI CHelpWindow::OnLayoutChange(ITfContext *pContext, TfLayoutCode lcode, ITfContextView *pContextView)
{
	HRESULT hr;

	if (pContext != _pContext)
	{
		return S_OK;
	}

	switch (lcode)
	{
	case TF_LC_CREATE:
		break;

	case TF_LC_CHANGE:
		try
		{
			CComPtr<ITfEditSession> pEditSession;
			pEditSession.Attach(
				new CHelpWindowGetTextExtEditSession(_pTextService, pContext, pContextView, this));
			pContext->RequestEditSession(_pTextService->_GetClientId(), pEditSession, TF_ES_SYNC | TF_ES_READ, &hr);
		}
		catch(...)
		{
		}
		break;

	case TF_LC_DESTROY:
		_Destroy();
		break;

	default:
		break;
	}

	return S_OK;
}

HRESULT CHelpWindow::_AdviseTextLayoutSink()
{
	HRESULT hr = E_FAIL;

	CComPtr<ITfSource> pSource;
	if (SUCCEEDED(_pContext->QueryInterface(IID_PPV_ARGS(&pSource))) && (pSource != nullptr))
	{
		hr = pSource->AdviseSink(IID_IUNK_ARGS(static_cast<ITfTextLayoutSink *>(this)), &_dwCookieTextLayoutSink);
	}

	return hr;
}

HRESULT CHelpWindow::_UnadviseTextLayoutSink()
{
	HRESULT hr = E_FAIL;

	if (_pContext != nullptr)
	{
		CComPtr<ITfSource> pSource;
		if (SUCCEEDED(_pContext->QueryInterface(IID_PPV_ARGS(&pSource))) && (pSource != nullptr))
		{
			hr = pSource->UnadviseSink(_dwCookieTextLayoutSink);
		}
	}

	return hr;
}

BOOL CHelpWindow::_Create(CTextService *pTextService, ITfContext *pContext, BOOL bCandidateWindow, HWND hWnd)
{
	POINT pt = {};

	if (pContext != nullptr)
	{
		_pContext = pContext;
		if (FAILED(_AdviseTextLayoutSink()))
		{
			return FALSE;
		}
	}

	if (!bCandidateWindow && _pContext == nullptr)
	{
		return FALSE;
	}

	_pTextService = pTextService;
	_pTextService->_MakeHelpTable(_kanji, &_helptables);

	if (bCandidateWindow)
	{
		_hwndParent = hWnd;
	}
	else
	{
		CComPtr<ITfContextView> pContextView;
		if (SUCCEEDED(_pContext->GetActiveView(&pContextView)) && (pContextView != nullptr))
		{
			if (FAILED(pContextView->GetWnd(&_hwndParent)) || _hwndParent == nullptr)
			{
				_hwndParent = GetFocus();
			}
		}
	}

	_hwnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
		HelpWindowClass, L"", WS_POPUP,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		_hwndParent, nullptr, g_hInst, this);

	if (_hwnd == nullptr)
	{
		return FALSE;
	}

	_InitFont();

	RECT rw = {};
	_CalcWindowRect(&rw);

	if (bCandidateWindow)
	{
		RECT r = {};
		GetClientRect(_hwndParent, &r);
		pt.x = r.left;
		pt.y = r.bottom;
		ClientToScreen(_hwndParent, &pt);
	}

	SetWindowPos(_hwnd, HWND_TOPMOST, pt.x, pt.y + IM_MARGIN_Y,
		rw.right, rw.bottom, SWP_NOSIZE | SWP_NOACTIVATE);

	return TRUE;
}

BOOL CHelpWindow::_InitClass()
{
	WNDCLASSEXW wcex = {};

	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_VREDRAW | CS_HREDRAW;
	wcex.lpfnWndProc = CHelpWindow::_WindowPreProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = g_hInst;
	wcex.hIcon = nullptr;
	wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = HelpWindowClass;
	wcex.hIconSm = nullptr;

	ATOM atom = RegisterClassExW(&wcex);

	return (atom != 0);
}

void CHelpWindow::_UninitClass()
{
	UnregisterClassW(HelpWindowClass, g_hInst);
}

LRESULT CALLBACK CHelpWindow::_WindowPreProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CHelpWindow *pHelpWindow = nullptr;

	switch (uMsg)
	{
	case WM_NCCREATE:
		pHelpWindow = (CHelpWindow *)((LPCREATESTRUCTW)lParam)->lpCreateParams;
		SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)pHelpWindow);
		break;
	default:
		pHelpWindow = (CHelpWindow *)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
		break;
	}

	if (pHelpWindow != nullptr)
	{
		return pHelpWindow->_WindowProc(hWnd, uMsg, wParam, lParam);
	}

	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CHelpWindow::_WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_PAINT:
		_WindowProcPaint(hWnd, uMsg, wParam, lParam);
		break;
	case WM_ERASEBKGND:
		break;
	case WM_MOUSEACTIVATE:
		return MA_NOACTIVATE;
	default:
		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

void CHelpWindow::_Destroy()
{
	if (_hwnd != nullptr)
	{
		DestroyWindow(_hwnd);
		_hwnd = nullptr;
	}

	_UninitFont();

	_UnadviseTextLayoutSink();
	_pContext.Release();

	_pTextService.Release();
}

void CHelpWindow::_Move(int x, int y)
{
	if (_hwnd != nullptr)
	{
		SetWindowPos(_hwnd, HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	}
}

void CHelpWindow::_Show(BOOL bShow)
{
	_bHide = !bShow;

	//辞書登録時用のヘルプを表示しようとしてる場合、大元のヘルプは非表示に。
	//再帰的な辞書登録をキャンセルした時の、2重のヘルプ表示を回避するため
	if (bShow && _pContext == nullptr && _pTextService != nullptr)
	{
		_pTextService->_HideHelpWindow();
	}

	if (_hwnd != nullptr)
	{
		if (bShow && _helptables.empty())
		{
			bShow = FALSE;
		}
		SetWindowPos(_hwnd, HWND_TOPMOST, 0, 0, 0, 0,
			SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | (bShow ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));
	}
}

void CHelpWindow::_Redraw()
{
	if (_bHide)
	{
		return;
	}
	if (_hwnd != nullptr)
	{
		_pTextService->_MakeHelpTable(_kanji, &_helptables);
		RECT rw = {};
		_CalcWindowRect(&rw);
		if (_helptables.empty())
		{
			ShowWindow(_hwnd, SW_HIDE);
		}
		else
		{
			ShowWindow(_hwnd, SW_SHOWNA);
			InvalidateRect(_hwnd, nullptr, FALSE);
			UpdateWindow(_hwnd);
		}
	}
}

void CHelpWindow::_GetRect(LPRECT lpRect)
{
	if (lpRect != nullptr)
	{
		SetRectEmpty(lpRect);
		if (_hwnd != nullptr)
		{
			GetClientRect(_hwnd, lpRect);
		}
	}
}

void CHelpWindow::_CalcWindowRect(LPRECT lpRect)
{
	SetRectEmpty(lpRect);

	HDC hdc = GetDC(_hwnd);
	HFONT fontSave = (HFONT)SelectObject(hdc, hFont);
	TEXTMETRICW tm = {};
	GetTextMetricsW(hdc, &tm);
	_fontHeight = tm.tmHeight;
	LONG nallrows = 0;
	FORWARD_ITERATION_I(itr, _helptables)
	{
		std::wstring tbl = (*itr).showtable;
		nallrows += (LONG)std::count(tbl.begin(), tbl.end(), L'\n') + 1;
	}
	LONG ntables = (LONG)_helptables.size();
	LONG nmargins_y = 2 + ntables - 1; //top, bottom, between tables
	lpRect->bottom = IM_MARGIN_Y * nmargins_y + _fontHeight * nallrows;

	RECT r = {};
	//XXX:漢字は固定幅と想定
	DrawTextW(hdc, L"並態両乗専│興口洋船久　漢", -1, &r, DT_CALCRECT);
	lpRect->right = IM_MARGIN_X * 2 + r.right;
	_fontWidth = (LONG)ceil(r.right / 13);

	SetWindowPos(_hwnd, HWND_TOPMOST, 0, 0, lpRect->right, lpRect->bottom, SWP_NOMOVE | SWP_NOACTIVATE);

	SelectObject(hdc, fontSave);
	ReleaseDC(_hwnd, hdc);
}

void CHelpWindow::_InitFont()
{
	LOGFONTW lf = {};
	lf.lfHeight = -MulDiv(_pTextService->cx_fontpoint, _dpi, C_FONT_LOGICAL_HEIGHT_PPI);
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = _pTextService->cx_fontweight;
	lf.lfItalic = _pTextService->cx_fontitalic;
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = SHIFTJIS_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = PROOF_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH;
	wcscpy_s(lf.lfFaceName, _pTextService->cx_fontname);

	hFont = CreateFontIndirectW(&lf);
}

void CHelpWindow::_UninitFont()
{
	if (hFont != nullptr)
	{
		DeleteObject(hFont);
		hFont = nullptr;
	}
}

void CHelpWindow::_WindowProcPaint(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	HDC hmemdc;
	HBITMAP hmembmp, bmp;
	HPEN pen, npen;
	HBRUSH brush, nbrush;
	HFONT font;
	RECT r = {};

	hdc = BeginPaint(hWnd, &ps);

	GetClientRect(hWnd, &r);

	hmemdc = CreateCompatibleDC(hdc);
	hmembmp = CreateCompatibleBitmap(hdc, r.right, r.bottom);
	bmp = (HBITMAP)SelectObject(hmemdc, hmembmp);

	npen = CreatePen(PS_SOLID, 1, RGB(0x00, 0x00, 0x00));
	pen = (HPEN)SelectObject(hmemdc, npen);
	nbrush = CreateSolidBrush(RGB(0xFF, 0xFF, 0xFF));
	brush = (HBRUSH)SelectObject(hmemdc, nbrush);

	Rectangle(hmemdc, 0, 0, r.right, r.bottom);

	font = (HFONT)SelectObject(hmemdc, hFont);
	int y = IM_MARGIN_Y;
	FORWARD_ITERATION_I(itr, _helptables)
	{
		if ((*itr).colormarker.empty()) //部首合成ヘルプ
		{
			std::wstring s = (*itr).showtable;
			SetBkMode(hmemdc, TRANSPARENT);
			TextOutW(hmemdc, IM_MARGIN_X, y, s.c_str(), (int)s.size());
			y += _fontHeight;
		}
		else //打鍵ヘルプ表(ドット表または漢字表)
		{
			bool firstline = true;
			std::wistringstream iss((*itr).showtable);
			std::wistringstream issbkcol((*itr).colormarker); //背景色特定用
			for (std::wstring s, bk; getline(iss, s) && getline(issbkcol, bk);)
			{
				int x = IM_MARGIN_X;
				std::wstring m;
				std::wstring mbk;
				size_t idx = 0;
				size_t idxbk = 0;
				while ((idx = Copy1Moji(s, idx, &m)) != 0)
				{
					//何打鍵目かを示す背景色を付ける
					idxbk = Copy1Moji(bk, idxbk, &mbk);
					if (mbk.empty())
					{
						SetBkMode(hmemdc, TRANSPARENT);
						idxbk = idx; //idxbk=0になってるので最初に戻るのを回避
					}
					else
					{
						int bkmode = OPAQUE;
						switch (mbk[0])
						{
						case L'●':	//第1打鍵
							SetBkColor(hmemdc, RGB(0xff, 0xc0, 0xc0));//red
							break;
						case L'○':	//第2打鍵
							SetBkColor(hmemdc, RGB(0xc0, 0xff, 0xc0));//green
							break;
						case L'△':	//第3打鍵
							SetBkColor(hmemdc, RGB(0xff, 0xff, 0xc0));//yellow
							break;
						case L'◇':	//第4打鍵以降
							SetBkColor(hmemdc, RGB(0xc0, 0xc0, 0xc0));//gray
							break;
						case L'◎':	//二重打鍵
							SetBkColor(hmemdc, RGB(0xc0, 0xc0, 0xff));//blue
							break;
						case L'☆':	//二重打鍵(その2)
							SetBkColor(hmemdc, RGB(0xc0, 0xff, 0xff));//cyan
							break;
						default:
							bkmode = TRANSPARENT;
							break;
						}
						SetBkMode(hmemdc, bkmode);
						if (m == L"・") //特に表示内容が無ければ'△'等に置換
						{
							m = mbk;
						}
					}
					TextOutW(hmemdc, x, y, m.c_str(), (int)m.size());
					x += _fontWidth;
				}
				//1行目に間隔をあけてヘルプ対象の漢字を表示
				if (firstline)
				{
					x += _fontWidth;
					std::wstring k = (*itr).kanji;
					TextOutW(hmemdc, x, y, k.c_str(), (int)k.size());
					firstline = false;
				}
				y += _fontHeight;
			}
		}
		//ドット表の間に、区切り線を引く
		POINT ptmw[2] = { {0, y}, {r.right, y} };
		y += IM_MARGIN_Y;
		if (y < r.bottom) //表の間でなくbottomの場合は引かない
		{
			Polyline(hmemdc, ptmw, 2);
		}
	}

	SelectObject(hmemdc, font);
	SelectObject(hmemdc, pen);
	SelectObject(hmemdc, brush);
	DeleteObject(npen);
	DeleteObject(nbrush);

	BitBlt(hdc, 0, 0, r.right, r.bottom, hmemdc, 0, 0, SRCCOPY);

	SelectObject(hmemdc, bmp);

	DeleteObject(hmembmp);
	DeleteObject(hmemdc);

	EndPaint(hWnd, &ps);
}

class CHelpWindowEditSession : public CEditSessionBase
{
public:
	CHelpWindowEditSession(CTextService *pTextService, ITfContext *pContext,
		CHelpWindow *pHelpWindow) : CEditSessionBase(pTextService, pContext)
	{
		_pHelpWindow = pHelpWindow;
	}

	~CHelpWindowEditSession()
	{
		_pHelpWindow.Release();
	}

	// ITfEditSession
	STDMETHODIMP DoEditSession(TfEditCookie ec)
	{
		HRESULT hr;

		CComPtr<ITfContextView> pContextView;
		if (SUCCEEDED(_pContext->GetActiveView(&pContextView)) && (pContextView != nullptr))
		{
			try
			{
				CComPtr<ITfEditSession> pEditSession;
				pEditSession.Attach(
					new CHelpWindowGetTextExtEditSession(_pTextService, _pContext, pContextView, _pHelpWindow));
				_pContext->RequestEditSession(_pTextService->_GetClientId(), pEditSession, TF_ES_SYNC | TF_ES_READ, &hr);
			}
			catch(...)
			{
			}
		}

		return S_OK;
	}

private:
	CComPtr<CHelpWindow> _pHelpWindow;
};

HRESULT CTextService::_StartHelpWindow(const std::wstring &kanji)
{
	if (_pHelpWindow != nullptr)
	{
		_pHelpWindow->_SetKanji(kanji);
		_pHelpWindow->_Show(TRUE);
		_pHelpWindow->_Redraw();
		return S_OK;
	}

	CComPtr<ITfDocumentMgr> pDocumentMgr;
	if (SUCCEEDED(_pThreadMgr->GetFocus(&pDocumentMgr)) && (pDocumentMgr != nullptr))
	{
		CComPtr<ITfContext> pContext;
		if (SUCCEEDED(pDocumentMgr->GetTop(&pContext)) && (pContext != nullptr))
		{
			try
			{
				_pHelpWindow.Attach(new CHelpWindow());
				_pHelpWindow->_SetKanji(kanji);
				if (_pHelpWindow->_Create(this, pContext, FALSE, nullptr))
				{
					HRESULT hr = E_FAIL;
					HRESULT hrSession = E_FAIL;

					CComPtr<ITfEditSession> pEditSession;
					pEditSession.Attach(
						new CHelpWindowEditSession(this, pContext, _pHelpWindow));
					// Asynchronous, read-only
					hr = pContext->RequestEditSession(_ClientId, pEditSession, TF_ES_ASYNC | TF_ES_READ, &hrSession);

					// It is possible that asynchronous requests are treated as synchronous requests.
					if (FAILED(hr) || (hrSession != TF_S_ASYNC && FAILED(hrSession)))
					{
						_EndHelpWindow();
						return E_FAIL;
					}
				}
				else
				{
					return E_FAIL;
				}
			}
			catch(...)
			{
				_EndHelpWindow();
				return E_FAIL;
			}
		}
		else
		{
			return E_FAIL;
		}
	}
	else
	{
		return E_FAIL;
	}
	return S_OK;
}

void CTextService::_HideHelpWindow()
{
	if (_pHelpWindow != nullptr)
	{
		_pHelpWindow->_Show(FALSE);
	}
}

void CTextService::_EndHelpWindow()
{
	if (_pHelpWindow != nullptr)
	{
		_pHelpWindow->_Destroy();
	}
	_pHelpWindow.Release();
}

void CTextService::_MakeHelpTable(const std::wstring &kanji, HELPTABLES *helptables)
{
	helptables->clear();
	//漢字を1文字ずつ入力シーケンスに変換
	std::wstring k1;
	size_t idx = 0;
	while ((idx = Copy1Moji(kanji, idx, &k1)) != 0)
	{
		if (iswascii(k1[0])) //直接入力できる文字はヘルプ表示しない
		{
			continue;
		}
		std::wstring seq;
		_ConvKanaToRoman(seq, k1, im_hiragana);
		//入力シーケンスが無い文字(seq==k1)に対しては部首合成方法を示す
		if (seq == k1)
		{
			std::wstring bushuhelp;
			_BushuHelp(k1, &bushuhelp);
			if (!bushuhelp.empty())
			{
				bushuhelp.insert(0, k1);
				HELPTABLE tbl = {k1, bushuhelp, L""};
				helptables->push_back(tbl);
			}
			continue;
		}

		std::wstring dothyo = _MakeHelpTableDotHyo(seq);
		HELPTABLE tbl = {k1, dothyo, dothyo};
		if (cx_showhelp == SH_KANJIHYO)
		{
			std::wstring kanjihyo = _MakeHelpTableKanjiHyo(seq);
			tbl.showtable = kanjihyo;
		}
		helptables->push_back(tbl);
	}
}

std::wstring CTextService::_MakeHelpTableDotHyo(const std::wstring &seq)
{
	std::wstring vkb;
	FORWARD_ITERATION_I(itr, cx_vkbdlayout)
	{
		//改行 || 直接キー入力できない文字(左右ブロック区切り'│'等)
		if (*itr == L'\n' || !iswascii(*itr))
		{
			vkb += *itr;
		}
		else
		{
			vkb += L'・';
		}
	}
	bool needX = false;
	for (size_t th = 0; th < seq.size(); th++)
	{
		size_t k = cx_vkbdlayout.find(seq[th]);
		if (k == std::wstring::npos)
		{
			continue;
		}
		switch (th)
		{
		case 0:                 // 1st stroke
			vkb[k] = L'●';
			break;
		case 1:                 // 2nd stroke
			if (vkb[k] == L'・')
			{
				vkb[k] = L'○';
				break;
			}
			vkb[k] = L'◎'; // 二重打鍵
			needX = true;
			break;
		case 2:                 // 3rd stroke
			if (vkb[k] == L'・')
			{
				vkb[k] = L'△';
				break;
			}
			if (!needX)
			{
				vkb[k] = L'◎'; // 二重打鍵
				needX = true;
				break;
			}
			vkb[k] = L'☆'; // 二重打鍵 (その 2)
			break;
		default:                // forth stroke(s)
			if (vkb[k] == L'・')
			{
				vkb[k] = L'◇';
				break;
			}
			if (!needX)
			{
				vkb[k] = L'◎'; // 二重打鍵
				needX = true;
				break;
			}
			vkb[k] = L'☆'; // 二重打鍵 (その 2)
			break;
		}
	}
	return vkb;
}

//tcvimeの自動ヘルプ同様の漢字表作成
std::wstring CTextService::_MakeHelpTableKanjiHyo(const std::wstring &baseseq)
{
	std::wstring vkb;
	FORWARD_ITERATION_I(itr, cx_vkbdlayout)
	{
		//改行 || 直接キー入力できない文字(左右ブロック区切り'│'等)
		if (*itr == L'\n' || !iswascii(*itr))
		{
			vkb += *itr;
		}
		else
		{
			//*itrで始まりseqの末尾で終わる各入力シーケンスを漢字に変換
			ROMAN_KANA_CONV rkc; //_ConvRomanKana()で変更されるので呼出毎に生成
			wcsncpy_s(rkc.roman, baseseq.c_str(), _TRUNCATE);
			rkc.roman[0] = *itr; //シーケンス最初をvkbdlayoutの各キーで上書き
			HRESULT ret = _ConvRomanKana(&rkc);
			switch (ret)
			{
			case S_OK:	//一致
				if (rkc.func)
				{
					vkb += L'＠';
				}
				else
				{
					//XXX:複数文字は未対応。ほとんどの場合は1文字なので
					vkb += Get1Moji(rkc.hiragana, 0);
				}
				break;
			case E_PENDING:	//途中まで一致
				vkb += L'□';
				break;
			case E_ABORT:	//一致する可能性なし
			default:
				vkb += L'・';
				break;
			}
		}
	}
	return vkb;
}
