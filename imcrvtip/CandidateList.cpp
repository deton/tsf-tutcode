﻿
#include "imcrvtip.h"
#include "TextService.h"
#include "EditSession.h"
#include "CandidateList.h"
#include "CandidateWindow.h"

class CCandidateListGetTextExtEditSession : public CEditSessionBase
{
public:
	CCandidateListGetTextExtEditSession(CTextService *pTextService, ITfContext *pContext,
		ITfContextView *pContextView, ITfRange *pRange, CCandidateWindow *pCandidateWindow) : CEditSessionBase(pTextService, pContext)
	{
		_pCandidateWindow = pCandidateWindow;
		_pCandidateWindow->AddRef();
		_pRangeComposition = pRange;
		_pRangeComposition->AddRef();
		_pContextView = pContextView;
		_pContextView->AddRef();
	}

	~CCandidateListGetTextExtEditSession()
	{
		SafeRelease(&_pCandidateWindow);
		SafeRelease(&_pRangeComposition);
		SafeRelease(&_pContextView);
	}

	// ITfEditSession
	STDMETHODIMP DoEditSession(TfEditCookie ec)
	{
		RECT rc = {};
		BOOL fClipped;
		if(SUCCEEDED(_pContextView->GetTextExt(ec, _pRangeComposition, &rc, &fClipped)))
		{
			_pCandidateWindow->_Move(&rc, ec, _pContext);
		}

		return S_OK;
	}

private:
	ITfContextView *_pContextView;
	ITfRange *_pRangeComposition;
	CCandidateWindow *_pCandidateWindow;
};

CCandidateList::CCandidateList(CTextService *pTextService)
{
	DllAddRef();

	_cRef = 1;

	_pTextService = pTextService;
	_pTextService->AddRef();

	_pCandidateWindow = nullptr;
	_pRangeComposition = nullptr;
	_pContextCandidateWindow = nullptr;
	_pContextDocument = nullptr;
	_pDocumentMgr = nullptr;

	_dwCookieContextKeyEventSink = TF_INVALID_COOKIE;
	_dwCookieTextLayoutSink = TF_INVALID_COOKIE;

	_ec = TF_INVALID_EDIT_COOKIE;
}

CCandidateList::~CCandidateList()
{
	_EndCandidateList();

	SafeRelease(&_pTextService);

	DllRelease();
}

STDAPI CCandidateList::QueryInterface(REFIID riid, void **ppvObj)
{
	if(ppvObj == nullptr)
	{
		return E_INVALIDARG;
	}

	*ppvObj = nullptr;

	if(IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_ITfContextKeyEventSink))
	{
		*ppvObj = (ITfContextKeyEventSink *)this;
	}
	else if(IsEqualIID(riid, IID_ITfTextLayoutSink))
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

STDAPI_(ULONG) CCandidateList::AddRef()
{
	return ++_cRef;
}

STDAPI_(ULONG) CCandidateList::Release()
{
	if(--_cRef == 0)
	{
		delete this;
		return 0;
	}

	return _cRef;
}

STDAPI CCandidateList::OnKeyDown(WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	if(pfEaten == nullptr)
	{
		return E_INVALIDARG;
	}

	*pfEaten = TRUE;
#ifdef _DEBUG
	_pCandidateWindow->_OnKeyDown((UINT)wParam);
#else
	__try
	{
		_pCandidateWindow->_OnKeyDown((UINT)wParam);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		_pTextService->_ResetStatus();
		_pTextService->_ClearComposition();
	}

#endif
	return S_OK;
}

STDAPI CCandidateList::OnKeyUp(WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	if(pfEaten == nullptr)
	{
		return E_INVALIDARG;
	}

	*pfEaten = TRUE;

	return S_OK;
}

STDAPI CCandidateList::OnTestKeyDown(WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	if(pfEaten == nullptr)
	{
		return E_INVALIDARG;
	}

	*pfEaten = TRUE;

	return S_OK;
}

STDAPI CCandidateList::OnTestKeyUp(WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	if(pfEaten == nullptr)
	{
		return E_INVALIDARG;
	}

	*pfEaten = TRUE;

	return S_OK;
}

STDAPI CCandidateList::OnLayoutChange(ITfContext *pContext, TfLayoutCode lcode, ITfContextView *pContextView)
{
	HRESULT hr;

	if(pContext != _pContextDocument)
	{
		return S_OK;
	}

	switch(lcode)
	{
	case TF_LC_CREATE:
		break;

	case TF_LC_CHANGE:
		if(_pCandidateWindow != nullptr)
		{
			try
			{
				CCandidateListGetTextExtEditSession *pEditSession =
					new CCandidateListGetTextExtEditSession(_pTextService, pContext, pContextView, _pRangeComposition, _pCandidateWindow);
				pContext->RequestEditSession(_pTextService->_GetClientId(), pEditSession, TF_ES_SYNC | TF_ES_READ, &hr);
				SafeRelease(&pEditSession);
			}
			catch(...)
			{
			}
		}
		break;

	case TF_LC_DESTROY:
		_EndCandidateList();
		break;

	default:
		break;
	}

	return S_OK;
}

class CCandidateWindowEditSession : public CEditSessionBase
{
public:
	CCandidateWindowEditSession(CTextService *pTextService, ITfContext *pContext,
		ITfRange *pRange, CCandidateWindow *pCandidateWindow) : CEditSessionBase(pTextService, pContext)
	{
		_pCandidateWindow = pCandidateWindow;
		_pCandidateWindow->AddRef();
		_pRangeComposition = pRange;
		_pRangeComposition->AddRef();
	}

	~CCandidateWindowEditSession()
	{
		SafeRelease(&_pCandidateWindow);
		SafeRelease(&_pRangeComposition);
	}

	// ITfEditSession
	STDMETHODIMP DoEditSession(TfEditCookie ec)
	{
		ITfContextView *pContextView = nullptr;
		if(SUCCEEDED(_pContext->GetActiveView(&pContextView)) && (pContextView != nullptr))
		{
			RECT rc = {};
			BOOL fClipped;
			if(SUCCEEDED(pContextView->GetTextExt(ec, _pRangeComposition, &rc, &fClipped)))
			{
				_pCandidateWindow->_Move(&rc, ec, _pContext);
			}

			SafeRelease(&pContextView);
		}

		_pCandidateWindow->_BeginUIElement();
		_pCandidateWindow->_Redraw();

		return S_OK;
	}

private:
	ITfRange *_pRangeComposition;
	CCandidateWindow *_pCandidateWindow;
};

HRESULT CCandidateList::_StartCandidateList(TfClientId tfClientId, ITfDocumentMgr *pDocumentMgr,
	ITfContext *pContext, TfEditCookie ec, ITfRange *pRange, int mode)
{
	HRESULT hrRet = E_FAIL;
	TfEditCookie ecTextStore;

	_EndCandidateList();

	if(FAILED(pDocumentMgr->CreateContext(tfClientId, 0, nullptr, &_pContextCandidateWindow, &ecTextStore)))
	{
		return E_FAIL;
	}

	if(FAILED(pDocumentMgr->Push(_pContextCandidateWindow)))
	{
		goto exit;
	}

	_pDocumentMgr = pDocumentMgr;
	_pDocumentMgr->AddRef();

	_pContextDocument = pContext;
	_pContextDocument->AddRef();

	_pRangeComposition = pRange;
	_pRangeComposition->AddRef();

	_ec = ec;

	if(FAILED(_AdviseContextKeyEventSink()))
	{
		goto exit;
	}

	if(FAILED(_AdviseTextLayoutSink()))
	{
		goto exit;
	}

	try
	{
		_pCandidateWindow = new CCandidateWindow(_pTextService, this);

		HWND hwnd = nullptr;
		ITfContextView *pContextView = nullptr;
		if(SUCCEEDED(pContext->GetActiveView(&pContextView)) && (pContextView != nullptr))
		{
			if(!_pTextService->_UILessMode && _pCandidateWindow->_CanShowUIElement())
			{
				if(FAILED(pContextView->GetWnd(&hwnd)) || hwnd == nullptr)
				{
					hwnd = GetFocus();
				}
			}

			SafeRelease(&pContextView);
		}

		if(!_pCandidateWindow->_Create(hwnd, nullptr, 0, 0, mode))
		{
			goto exit;
		}
	}
	catch(...)
	{
		goto exit;
	}

	HRESULT hr = E_FAIL;
	HRESULT hrSession = E_FAIL;

	try
	{
		CCandidateWindowEditSession *pEditSession =
			new CCandidateWindowEditSession(_pTextService, _pContextDocument, _pRangeComposition, _pCandidateWindow);
		// Asynchronous, read-only
		hr = pContext->RequestEditSession(ec, pEditSession, TF_ES_ASYNC | TF_ES_READ, &hrSession);
		SafeRelease(&pEditSession);

		// It is possible that asynchronous requests are treated as synchronous requests.
		if(FAILED(hr) || (hrSession != TF_S_ASYNC && FAILED(hrSession)))
		{
			goto exit;
		}
	}
	catch(...)
	{
		goto exit;
	}

	hrRet = S_OK;

exit:
	if(FAILED(hrRet))
	{
		_EndCandidateList();
	}
	return hrRet;
}

void CCandidateList::_InvokeKeyHandler(WPARAM key)
{
	if(_pTextService != nullptr && _pContextDocument != nullptr)
	{
		_pTextService->_InvokeKeyHandler(_pContextDocument, (WPARAM)key, (LPARAM)0, 0);
	}
}

void CCandidateList::_InvokeSfHandler(BYTE sf)
{
	if(_pTextService != nullptr && _pContextDocument != nullptr)
	{
		_pTextService->_InvokeKeyHandler(_pContextDocument, (WPARAM)0, (LPARAM)0, sf);
	}
}

void CCandidateList::_EndCandidateList()
{
	_UnadviseTextLayoutSink();

	_UnadviseContextKeyEventSink();

	if(_pDocumentMgr != nullptr)
	{
		_pDocumentMgr->Pop(0);
	}

	SafeRelease(&_pContextCandidateWindow);
	SafeRelease(&_pDocumentMgr);

	if(_pCandidateWindow != nullptr)
	{
		_pCandidateWindow->_EndUIElement();
		_pCandidateWindow->_Destroy();
	}
	SafeRelease(&_pCandidateWindow);

	SafeRelease(&_pRangeComposition);
	SafeRelease(&_pContextDocument);
}

BOOL CCandidateList::_IsShowCandidateWindow()
{
	return (_pCandidateWindow != nullptr) ? TRUE : FALSE;
}

BOOL CCandidateList::_IsContextCandidateWindow(ITfContext *pContext)
{
	return (_pContextCandidateWindow == pContext) ? TRUE : FALSE;
}

HRESULT CCandidateList::_AdviseContextKeyEventSink()
{
	HRESULT hr = E_FAIL;

	ITfSource *pSource = nullptr;
	if(SUCCEEDED(_pContextCandidateWindow->QueryInterface(IID_PPV_ARGS(&pSource))) && (pSource != nullptr))
	{
		hr = pSource->AdviseSink(IID_IUNK_ARGS((ITfContextKeyEventSink *)this), &_dwCookieContextKeyEventSink);
		SafeRelease(&pSource);
	}

	return hr;
}

HRESULT CCandidateList::_UnadviseContextKeyEventSink()
{
	HRESULT hr = E_FAIL;

	if(_pContextCandidateWindow != nullptr)
	{
		ITfSource *pSource = nullptr;
		if(SUCCEEDED(_pContextCandidateWindow->QueryInterface(IID_PPV_ARGS(&pSource))) && (pSource != nullptr))
		{
			hr = pSource->UnadviseSink(_dwCookieContextKeyEventSink);
			SafeRelease(&pSource);
		}
	}

	return hr;
}

HRESULT CCandidateList::_AdviseTextLayoutSink()
{
	HRESULT hr = E_FAIL;

	ITfSource *pSource = nullptr;
	if(SUCCEEDED(_pContextDocument->QueryInterface(IID_PPV_ARGS(&pSource))) && (pSource != nullptr))
	{
		hr = pSource->AdviseSink(IID_IUNK_ARGS((ITfTextLayoutSink *)this), &_dwCookieTextLayoutSink);
		SafeRelease(&pSource);
	}

	return hr;
}

HRESULT CCandidateList::_UnadviseTextLayoutSink()
{
	HRESULT hr = E_FAIL;

	if(_pContextDocument != nullptr)
	{
		ITfSource *pSource = nullptr;
		if(SUCCEEDED(_pContextDocument->QueryInterface(IID_PPV_ARGS(&pSource))) && (pSource != nullptr))
		{
			hr = pSource->UnadviseSink(_dwCookieTextLayoutSink);
			SafeRelease(&pSource);
		}
	}

	return hr;
}

void CCandidateList::_Show(BOOL bShow)
{
	if(_pCandidateWindow != nullptr)
	{
		_pCandidateWindow->Show(bShow);
	}
}

void CCandidateList::_SetText(const std::wstring &text, BOOL fixed, int mode)
{
	if(_pCandidateWindow != nullptr)
	{
		_pCandidateWindow->_SetText(text, fixed, mode);
	}
}

//辞書登録時後置型交ぜ書き変換で読みから外した部分をセット。表示用
void CCandidateList::_SetTextExcludedPostyomi(const std::wstring &text)
{
	if(_pCandidateWindow != nullptr)
	{
		_pCandidateWindow->_SetTextExcludedPostyomi(text);
	}
}

void CCandidateList::_GetPrecedingText(std::wstring *text)
{
	if(_pCandidateWindow != nullptr)
	{
		_pCandidateWindow->_GetPrecedingText(text);
	}
	else
	{
		text->clear();
	}
}

void CCandidateList::_DeletePrecedingText(size_t delete_count)
{
	if(_pCandidateWindow != nullptr)
	{
		_pCandidateWindow->_DeletePrecedingText(delete_count);
	}
}

void CCandidateList::_Move(LPRECT lpr, TfEditCookie ec, ITfContext *pContext)
{
	if(_pCandidateWindow != nullptr)
	{
		_pCandidateWindow->_Move(lpr, ec, pContext);
	}
}

void CCandidateList::_UpdateComp()
{
	if(_pCandidateWindow != nullptr)
	{
		_pCandidateWindow->_UpdateComp();
	}
}
