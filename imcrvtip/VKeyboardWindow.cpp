
#include "imcrvtip.h"
#include "TextService.h"
#include "VKeyboardWindow.h"
#include "moji.h"
#include <sstream>

#define IM_MERGIN_X 2
#define IM_MERGIN_Y 2

class CVKeyboardWindowGetTextExtEditSession : public CEditSessionBase
{
public:
	CVKeyboardWindowGetTextExtEditSession(CTextService *pTextService, ITfContext *pContext,
		ITfContextView *pContextView, CVKeyboardWindow *pVKeyboardWindow) : CEditSessionBase(pTextService, pContext)
	{
		_pVKeyboardWindow = pVKeyboardWindow;
		_pVKeyboardWindow->AddRef();
		_pContextView = pContextView;
		_pContextView->AddRef();
	}

	~CVKeyboardWindowGetTextExtEditSession()
	{
		SafeRelease(&_pVKeyboardWindow);
		SafeRelease(&_pContextView);
	}

	// ITfEditSession
	STDMETHODIMP DoEditSession(TfEditCookie ec)
	{
		if(_pVKeyboardWindow->_IsHide())
		{
			return S_OK;
		}

		TF_SELECTION tfSelection = {};
		ULONG cFetched = 0;
		if(_pContext->GetSelection(ec, TF_DEFAULT_SELECTION, 1, &tfSelection, &cFetched) != S_OK)
		{
			return E_FAIL;
		}

		if(cFetched != 1)
		{
			SafeRelease(&tfSelection.range);
			return E_FAIL;
		}

		RECT rc = {};
		BOOL fClipped;
		if(_pContextView->GetTextExt(ec, tfSelection.range, &rc, &fClipped) != S_OK)
		{
			SafeRelease(&tfSelection.range);
			return E_FAIL;
		}

		//ignore abnormal position (from CUAS ?)
		if((rc.top == rc.bottom) && ((rc.right - rc.left) == 1))
		{
			SafeRelease(&tfSelection.range);
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
		_pVKeyboardWindow->_GetRect(&rw);
		LONG height = rw.bottom - rw.top;
		LONG width = rw.right - rw.left;

		if(rc.left < mi.rcWork.left)
		{
			rc.left = mi.rcWork.left;
		}

		if(mi.rcWork.right < rc.right)
		{
			rc.left = mi.rcWork.right - width;
		}

		if(mi.rcWork.bottom < rc.top)
		{
			rc.bottom = mi.rcWork.bottom - height - IM_MERGIN_Y;
		}
		else if(mi.rcWork.bottom < (rc.bottom + height + IM_MERGIN_Y))
		{
			rc.bottom = rc.top - height - IM_MERGIN_Y * 2;
		}

		if(rc.bottom < mi.rcWork.top)
		{
			rc.bottom = mi.rcWork.top - IM_MERGIN_Y;
		}

		_pVKeyboardWindow->_Move(rc.left, rc.bottom + IM_MERGIN_Y);
		_pVKeyboardWindow->_Show(TRUE);

		SafeRelease(&tfSelection.range);

		return S_OK;
	}

private:
	ITfContextView *_pContextView;
	CVKeyboardWindow *_pVKeyboardWindow;
};

CVKeyboardWindow::CVKeyboardWindow(): _bHide(FALSE)
{
	DllAddRef();

	_cRef = 1;

	_hwnd = nullptr;
	_hwndParent = nullptr;
	_pTextService = nullptr;
	_pContext = nullptr;
}

CVKeyboardWindow::~CVKeyboardWindow()
{
	_Destroy();

	DllRelease();
}

STDAPI CVKeyboardWindow::QueryInterface(REFIID riid, void **ppvObj)
{
	if(ppvObj == nullptr)
	{
		return E_INVALIDARG;
	}

	*ppvObj = nullptr;

	if(IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_ITfTextLayoutSink))
	{
		*ppvObj = (ITfTextLayoutSink *)this;
	}

	if(*ppvObj)
	{
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDAPI_(ULONG) CVKeyboardWindow::AddRef()
{
	return ++_cRef;
}

STDAPI_(ULONG) CVKeyboardWindow::Release()
{
	if(--_cRef == 0)
	{
		delete this;
		return 0;
	}

	return _cRef;
}

STDAPI CVKeyboardWindow::OnLayoutChange(ITfContext *pContext, TfLayoutCode lcode, ITfContextView *pContextView)
{
	HRESULT hr;

	if(pContext != _pContext)
	{
		return S_OK;
	}

	switch(lcode)
	{
	case TF_LC_CREATE:
		break;

	case TF_LC_CHANGE:
		try
		{
			CVKeyboardWindowGetTextExtEditSession *pEditSession =
				new CVKeyboardWindowGetTextExtEditSession(_pTextService, pContext, pContextView, this);
			pContext->RequestEditSession(_pTextService->_GetClientId(), pEditSession, TF_ES_SYNC | TF_ES_READ, &hr);
			SafeRelease(&pEditSession);
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

HRESULT CVKeyboardWindow::_AdviseTextLayoutSink()
{
	HRESULT hr = E_FAIL;

	ITfSource *pSource;
	if(_pContext->QueryInterface(IID_PPV_ARGS(&pSource)) == S_OK)
	{
		hr = pSource->AdviseSink(IID_IUNK_ARGS((ITfTextLayoutSink *)this), &_dwCookieTextLayoutSink);
		SafeRelease(&pSource);
	}

	return hr;
}

HRESULT CVKeyboardWindow::_UnadviseTextLayoutSink()
{
	HRESULT hr = E_FAIL;

	if(_pContext != nullptr)
	{
		ITfSource *pSource;
		if(_pContext->QueryInterface(IID_PPV_ARGS(&pSource)) == S_OK)
		{
			hr = pSource->UnadviseSink(_dwCookieTextLayoutSink);
			SafeRelease(&pSource);
		}
	}

	return hr;
}

BOOL CVKeyboardWindow::_Create(CTextService *pTextService, ITfContext *pContext, BOOL bCandidateWindow, HWND hWnd)
{
	POINT pt = {};

	if(pContext != nullptr)
	{
		_pContext = pContext;
		_pContext->AddRef();
		if(_AdviseTextLayoutSink() != S_OK)
		{
			return FALSE;
		}
	}

	if(!bCandidateWindow && _pContext == nullptr)
	{
		return FALSE;
	}

	_pTextService = pTextService;
	_pTextService->AddRef();
	_vkb = _pTextService->_MakeVkbTable();

	if(bCandidateWindow)
	{
		_hwndParent = hWnd;
	}
	else
	{
		ITfContextView *pContextView;
		if(_pContext->GetActiveView(&pContextView) == S_OK)
		{
			if(FAILED(pContextView->GetWnd(&_hwndParent)) || _hwndParent == nullptr)
			{
				_hwndParent = GetFocus();
			}
			SafeRelease(&pContextView);
		}
	}

	_hwnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
		VKeyboardWindowClass, L"", WS_POPUP,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		_hwndParent, nullptr, g_hInst, this);

	if(_hwnd == nullptr)
	{
		return FALSE;
	}

	RECT rw = {};
	_CalcWindowRect(&rw);

	if(bCandidateWindow)
	{
		RECT r = {};
		GetClientRect(_hwndParent, &r);
		pt.x = r.left;
		pt.y = r.bottom;
		ClientToScreen(_hwndParent, &pt);
	}

	SetWindowPos(_hwnd, HWND_TOPMOST, pt.x, pt.y + IM_MERGIN_Y, rw.right, rw.bottom, SWP_NOSIZE | SWP_NOACTIVATE);

	return TRUE;
}

BOOL CVKeyboardWindow::_InitClass()
{
	WNDCLASSEXW wcex = {};

	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_VREDRAW | CS_HREDRAW;
	wcex.lpfnWndProc = CVKeyboardWindow::_WindowPreProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = g_hInst;
	wcex.hIcon = nullptr;
	wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = VKeyboardWindowClass;
	wcex.hIconSm = nullptr;

	ATOM atom = RegisterClassExW(&wcex);

	return (atom != 0);
}

void CVKeyboardWindow::_UninitClass()
{
	UnregisterClassW(VKeyboardWindowClass, g_hInst);
}

LRESULT CALLBACK CVKeyboardWindow::_WindowPreProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CVKeyboardWindow *pVKeyboardWindow = nullptr;

	switch(uMsg)
	{
	case WM_NCCREATE:
		pVKeyboardWindow = (CVKeyboardWindow *)((LPCREATESTRUCTW)lParam)->lpCreateParams;
		SetWindowLongPtrW(hWnd, GWLP_USERDATA, (LONG_PTR)pVKeyboardWindow);
		break;
	default:
		pVKeyboardWindow = (CVKeyboardWindow *)GetWindowLongPtrW(hWnd, GWLP_USERDATA);
		break;
	}

	if(pVKeyboardWindow != nullptr)
	{
		return pVKeyboardWindow->_WindowProc(hWnd, uMsg, wParam, lParam);
	}

	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CVKeyboardWindow::_WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	HDC hmemdc;
	HBITMAP hmembmp;
	HPEN npen;
	HBRUSH nbrush;
	HGDIOBJ bmp, pen, brush, font;
	RECT r = {};

	switch(uMsg)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		GetClientRect(hWnd, &r);

		hmemdc = CreateCompatibleDC(hdc);
		hmembmp = CreateCompatibleBitmap(hdc, r.right, r.bottom);
		bmp = SelectObject(hmemdc, hmembmp);

		npen = CreatePen(PS_SOLID, 1, RGB(0x00, 0x00, 0x00));
		pen = SelectObject(hmemdc, npen);
		nbrush = CreateSolidBrush(RGB(0xFF, 0xFF, 0xFF));
		brush = SelectObject(hmemdc, nbrush);

		Rectangle(hmemdc, 0, 0, r.right, r.bottom);

		font = SelectObject(hmemdc, _pTextService->hFont);
		if(!_vkb.empty())
		{
			int i = 0;
			std::wistringstream iss(_vkb);
			for(std::wstring s; getline(iss, s);)
			{
				int y = IM_MERGIN_Y + i * _fontHeight;
				TextOut(hmemdc, IM_MERGIN_X, y, s.c_str(), s.size());
				i++;
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
		break;
	case WM_MOUSEACTIVATE:
		return MA_NOACTIVATE;
	default:
		return DefWindowProcW(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

void CVKeyboardWindow::_Destroy()
{
	if(_hwnd != nullptr)
	{
		DestroyWindow(_hwnd);
		_hwnd = nullptr;
	}

	_UnadviseTextLayoutSink();
	SafeRelease(&_pContext);

	SafeRelease(&_pTextService);
}

void CVKeyboardWindow::_Move(int x, int y)
{
	if(_hwnd != nullptr)
	{
		SetWindowPos(_hwnd, HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
	}
}

void CVKeyboardWindow::_Show(BOOL bShow)
{
	_bHide = !bShow;

	//辞書登録時用の仮想鍵盤を表示しようとしてる場合、大元の仮想鍵盤は非表示に。
	//再帰的な辞書登録をキャンセルした時の、2重の仮想鍵盤表示を回避するため
	if(bShow && _pContext == nullptr)
	{
		_pTextService->_HideVKeyboardWindow();
	}

	if(_hwnd != nullptr)
	{
		if(bShow && _vkb.empty())
		{
			bShow = FALSE;
		}
		SetWindowPos(_hwnd, HWND_TOPMOST, 0, 0, 0, 0,
			SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | (bShow ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));
	}
}

void CVKeyboardWindow::_Redraw()
{
	if(_bHide)
	{
		return;
	}
	if(_hwnd != nullptr)
	{
		_vkb = _pTextService->_MakeVkbTable();
		if(_vkb.empty())
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

void CVKeyboardWindow::_GetRect(LPRECT lpRect)
{
	if(lpRect != nullptr)
	{
		SetRectEmpty(lpRect);
		if(_hwnd != nullptr)
		{
			GetClientRect(_hwnd, lpRect);
		}
	}
}

void CVKeyboardWindow::_CalcWindowRect(LPRECT lpRect)
{
	SetRectEmpty(lpRect);

	HDC hdc = GetDC(_hwnd);
	HGDIOBJ fontSave = SelectObject(hdc, _pTextService->hFont);
	TEXTMETRICW tm;
	GetTextMetricsW(hdc, &tm);
	_fontHeight = tm.tmHeight;
	lpRect->bottom = IM_MERGIN_Y * 2 + _fontHeight * 4;

	RECT r = {0, 0, 0, 0};
	//XXX:漢字は固定幅と想定。表示文字列ごとに計算するのは面倒なので
	DrawTextW(hdc, L"並態両乗専│興口洋船久", -1, &r, DT_CALCRECT);
	lpRect->right = IM_MERGIN_X * 2 + r.right;

	SetWindowPos(_hwnd, HWND_TOPMOST, 0, 0, lpRect->right, lpRect->bottom, SWP_NOMOVE | SWP_NOACTIVATE);

	SelectObject(hdc, fontSave);
	ReleaseDC(_hwnd, hdc);
}

class CVKeyboardWindowEditSession : public CEditSessionBase
{
public:
	CVKeyboardWindowEditSession(CTextService *pTextService, ITfContext *pContext,
		CVKeyboardWindow *pVKeyboardWindow) : CEditSessionBase(pTextService, pContext)
	{
		_pVKeyboardWindow = pVKeyboardWindow;
		_pVKeyboardWindow->AddRef();
	}

	~CVKeyboardWindowEditSession()
	{
		SafeRelease(&_pVKeyboardWindow);
	}

	// ITfEditSession
	STDMETHODIMP DoEditSession(TfEditCookie ec)
	{
		HRESULT hr;

		ITfContextView *pContextView;
		if(_pContext->GetActiveView(&pContextView) == S_OK)
		{
			try
			{
				CVKeyboardWindowGetTextExtEditSession *pEditSession =
					new CVKeyboardWindowGetTextExtEditSession(_pTextService, _pContext, pContextView, _pVKeyboardWindow);
				_pContext->RequestEditSession(_pTextService->_GetClientId(), pEditSession, TF_ES_SYNC | TF_ES_READ, &hr);
				SafeRelease(&pEditSession);
			}
			catch(...)
			{
			}

			SafeRelease(&pContextView);
		}

		return S_OK;
	}

private:
	CVKeyboardWindow *_pVKeyboardWindow;
};

void CTextService::_StartVKeyboardWindow()
{
	if(_pVKeyboardWindow != nullptr)
	{
		_pVKeyboardWindow->_Show(TRUE);
		_pVKeyboardWindow->_Redraw();
		return;
	}

	if(!_IsRomanKanaStatus())
	{
		return;
	}

	ITfDocumentMgr *pDocumentMgr;
	if((_pThreadMgr->GetFocus(&pDocumentMgr) == S_OK) && (pDocumentMgr != nullptr))
	{
		ITfContext *pContext;
		if((pDocumentMgr->GetTop(&pContext) == S_OK) && (pContext != nullptr))
		{
			try
			{
				_pVKeyboardWindow = new CVKeyboardWindow();

				if(_pVKeyboardWindow->_Create(this, pContext, FALSE, nullptr))
				{
					HRESULT hr = E_FAIL;
					HRESULT hrSession = E_FAIL;

					try
					{
						CVKeyboardWindowEditSession *pEditSession = new CVKeyboardWindowEditSession(this, pContext, _pVKeyboardWindow);
						// Asynchronous, read-only
						hr = pContext->RequestEditSession(_ClientId, pEditSession, TF_ES_ASYNC | TF_ES_READ, &hrSession);
						SafeRelease(&pEditSession);

						// It is possible that asynchronous requests are treated as synchronous requests.
						if(hr != S_OK || (hrSession != TF_S_ASYNC && hrSession != S_OK))
						{
							_EndVKeyboardWindow();
						}
					}
					catch(...)
					{
					}
				}
			}
			catch(...)
			{
			}

			SafeRelease(&pContext);
		}

		SafeRelease(&pDocumentMgr);
	}
}

void CTextService::_HideVKeyboardWindow()
{
	if(_pVKeyboardWindow != nullptr)
	{
		_pVKeyboardWindow->_Show(FALSE);
	}
}

void CTextService::_EndVKeyboardWindow()
{
	if(_pVKeyboardWindow != nullptr)
	{
		_pVKeyboardWindow->_Destroy();
	}
	SafeRelease(&_pVKeyboardWindow);
}

void CTextService::_RedrawVKeyboardWindow()
{
	if(_pVKeyboardWindow != nullptr)
	{
		_pVKeyboardWindow->_Redraw();
	}
}

BOOL CTextService::_IsRomanKanaStatus()
{
	if((inputmode == im_hiragana || inputmode == im_katakana || inputmode == im_katakana_ank)
			&& !abbrevmode && !showcandlist)
	{
		return TRUE;
	}
	return FALSE;
}

std::wstring CTextService::_MakeVkbTable()
{
	if(!_IsRomanKanaStatus())
	{
		return L"";
	}
	if(roman.empty())
	{
		return cx_vkbdtop; //初期状態の仮想鍵盤を表示
	}
	ROMAN_KANA_CONV rkc;
	wcsncpy_s(rkc.roman, roman.c_str(), _TRUNCATE);
	HRESULT ret = _ConvRomanKana(&rkc);
	switch(ret)
	{
	case S_OK:	//一致
		return cx_vkbdtop; //初期状態の仮想鍵盤を表示
	case E_PENDING:	//途中まで一致
		break;
	case E_ABORT:	//一致する可能性なし
	default:
		return cx_vkbdtop;
	}
	std::wstring vkb;
	FORWARD_ITERATION_I(itr, cx_vkbdlayout)
	{
		if(*itr == L'\n')
		{
			vkb.append(L"\n");
			continue;
		}
		if(*itr == L'│') //左手ブロックと右手ブロックの区切り
		{
			vkb.append(L"│");
			continue;
		}
		std::wstring seq(roman);
		seq.push_back(*itr);
		ROMAN_KANA_CONV rkc;
		wcsncpy_s(rkc.roman, seq.c_str(), _TRUNCATE);
		HRESULT ret = _ConvRomanKana(&rkc);
		switch(ret)
		{
		case S_OK:	//一致
			if(rkc.func)
			{
				vkb.append(L"・");
			}
			else
			{
				std::wstring s;
				switch(inputmode)
				{
				case im_katakana:
				case im_katakana_ank: //XXX:半角幅だと列位置がずれるので
					s = rkc.katakana;
					break;
				case im_hiragana:
				default:
					s = rkc.hiragana;
					break;
				}
				//XXX:「じぇ」等、複数文字は未対応。ほとんどの場合は1文字なので
				vkb.append(Get1Moji(s, 0));
			}
			break;
		case E_PENDING:	//途中まで一致
			vkb.append(L"□");
			break;
		case E_ABORT:	//一致する可能性なし
		default:
			vkb.append(L"　");
			break;
		}
	}
	return vkb;
}
