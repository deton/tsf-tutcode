﻿
#include "imcrvtip.h"
#include "TextService.h"

STDAPI CTextService::OnInitDocumentMgr(ITfDocumentMgr *pdim)
{
	return S_OK;
}

STDAPI CTextService::OnUninitDocumentMgr(ITfDocumentMgr *pdim)
{
	return S_OK;
}

STDAPI CTextService::OnSetFocus(ITfDocumentMgr *pdim, ITfDocumentMgr *pdimPrevFocus)
{
	_InitTextEditSink(pdim);

	_UpdateLanguageBar(FALSE);

	return S_OK;
}

STDAPI CTextService::OnPushContext(ITfContext *pic)
{
	return S_OK;
}

STDAPI CTextService::OnPopContext(ITfContext *pic)
{
	return S_OK;
}

BOOL CTextService::_InitThreadMgrEventSink()
{
	BOOL fRet = FALSE;

	ITfSource *pSource = nullptr;
	if(SUCCEEDED(_pThreadMgr->QueryInterface(IID_PPV_ARGS(&pSource))) && (pSource != nullptr))
	{
		if(SUCCEEDED(pSource->AdviseSink(IID_IUNK_ARGS((ITfThreadMgrEventSink *)this), &_dwThreadMgrEventSinkCookie)))
		{
			fRet = TRUE;
		}
		else
		{
			_dwThreadMgrEventSinkCookie = TF_INVALID_COOKIE;
		}
		SafeRelease(&pSource);
	}

	return fRet;
}

void CTextService::_UninitThreadMgrEventSink()
{
	HRESULT hr;

	if(_dwThreadMgrEventSinkCookie != TF_INVALID_COOKIE)
	{
		ITfSource *pSource = nullptr;
		if(SUCCEEDED(_pThreadMgr->QueryInterface(IID_PPV_ARGS(&pSource))) && (pSource != nullptr))
		{
			hr = pSource->UnadviseSink(_dwThreadMgrEventSinkCookie);
			SafeRelease(&pSource);
		}
		_dwThreadMgrEventSinkCookie = TF_INVALID_COOKIE;
	}
}
