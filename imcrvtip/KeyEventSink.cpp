﻿
#include "imcrvtip.h"
#include "TextService.h"
#include "CandidateList.h"
#include "mozc/win32/base/deleter.h"
#include "mozc/win32/base/keyboard.h"

static LPCWSTR c_PreservedKeyDesc[PRESERVEDKEY_NUM] = {L"ON", L"OFF"};
static const GUID c_guidPreservedKeyOnOff[PRESERVEDKEY_NUM] = {c_guidPreservedKeyOn, c_guidPreservedKeyOff};

int CTextService::_IsKeyEaten(ITfContext *pContext, WPARAM wParam, LPARAM lParam, bool isKeyDown, bool isTest)
{
	if(_IsKeyboardDisabled())
	{
		return FALSE;
	}

	if(!_IsKeyboardOpen())
	{
		return FALSE;
	}

	//カーソル直前文字列削除をBSを送って消す場合のDeleter用処理。
	//(TSFによるカーソル直前文字列削除ができなかった場合用。)
	//mozcのwin32/tip/tip_keyevent_handler.ccから。
	bool is_key_down = isKeyDown;
	if(isTest)
	{
		const mozc::win32::LParamKeyInfo key_info(lParam);
		is_key_down = key_info.IsKeyDownInImeProcessKey();
	}
	const mozc::win32::VKBackBasedDeleter::ClientAction vk_back_action =
		deleter.OnKeyEvent(wParam, is_key_down, isTest);

	switch(vk_back_action)
	{
	case mozc::win32::VKBackBasedDeleter::DO_DEFAULT_ACTION:
		// do nothing.
		break;
	case mozc::win32::VKBackBasedDeleter::CALL_END_DELETION_THEN_DO_DEFAULT_ACTION:
		deleter.EndDeletion();
		break;
	case mozc::win32::VKBackBasedDeleter::SEND_KEY_TO_APPLICATION:
		if(is_key_down && isTest && !postbuf.empty())
		{
			postbuf.pop_back();
		}
		return FALSE; // Do not consume this key.
	case mozc::win32::VKBackBasedDeleter::CONSUME_KEY_BUT_NEVER_SEND_TO_SERVER:
		return -1; // Consume this key but do not send this key to server.
	case mozc::win32::VKBackBasedDeleter::CALL_END_DELETION_BUT_NEVER_SEND_TO_SERVER:
		if(!isTest)
		{
			deleter.EndDeletion();
			return -1;
		}
		else
		{
			return FALSE;
		}
	case mozc::win32::VKBackBasedDeleter::APPLY_PENDING_STATUS:
		if(!isTest)
		{
			_InvokeKeyHandler(pContext, wParam, lParam, SKK_AFTER_DELETER);
			return -1;
		}
		else
		{
			return FALSE;
		}
	default:
		break;
	}

	if(_pCandidateList && _pCandidateList->_IsContextCandidateWindow(pContext))
	{
		return FALSE;
	}

	if(_IsComposing() || !cx_showromancomp && !roman.empty())
	{
		if(inputmode != im_ascii)
		{
			return TRUE;
		}
	}

	SHORT vk_ctrl = GetKeyState(VK_CONTROL) & 0x8000;
	SHORT vk_kana = GetKeyState(VK_KANA) & 0x0001;

	WCHAR ch = _GetCh((BYTE)wParam);
	BYTE sf = _GetSf((BYTE)wParam, ch);

	//確定状態で処理する機能
	switch(inputmode)
	{
	case im_jlatin:
	case im_ascii:
		switch(sf)
		{
		case SKK_JMODE:
		case SKK_OTHERIME:
			return TRUE;
			break;
		default:
			break;
		}
		break;
	case im_hiragana:
	case im_katakana:
		switch(sf)
		{
		case SKK_CONV_POINT:
		case SKK_KANA:
		case SKK_CONV_CHAR:
		case SKK_JLATIN:
		case SKK_ASCII:
		case SKK_ABBREV:
		case SKK_OTHERIME:
			return TRUE;
			break;
		case SKK_DIRECT:
			if(cx_setbydirect && !inputkey && roman.empty())
			{
				return FALSE;
			}
			break;
		case SKK_VIESC:
			inputmode = im_ascii;
			_UpdateLanguageBar();
			postbuf.clear();
			return FALSE;
			break;
		default:
			break;
		}
		break;
	case im_katakana_ank:
		switch(sf)
		{
		case SKK_KANA:
		case SKK_CONV_CHAR:
		case SKK_JLATIN:
		case SKK_ASCII:
		case SKK_OTHERIME:
			return TRUE;
			break;
		case SKK_DIRECT:
			if(cx_setbydirect && !inputkey && roman.empty())
			{
				return FALSE;
			}
			break;
		case SKK_VIESC:
			inputmode = im_ascii;
			_UpdateLanguageBar();
			postbuf.clear();
			return FALSE;
		default:
			break;
		}
		break;
	default:
		break;
	}

	//無効
	if(_IsKeyVoid(ch, (BYTE)wParam))
	{
		return TRUE;
	}

	//処理しないCtrlキー
	if(vk_ctrl != 0)
	{
		postbuf.clear();
		return FALSE;
	}

	//ASCIIモード、かなキーロックOFF
	if(inputmode == im_ascii && vk_kana == 0)
	{
		return FALSE;
	}

	if(ch >= L'\x20')
	{
		return TRUE;
	}

	if(!postbuf.empty())
	{
		if((wParam == VK_BACK || wParam == VK_LEFT) && is_key_down && isTest)
		{
			postbuf.pop_back();
		}
		else
		{
			switch(wParam)
			{
			//case VK_LEFT:
			case VK_RIGHT:
			case VK_UP:
			case VK_DOWN:
			case VK_HOME:
			case VK_END:
			case VK_PRIOR:
			case VK_NEXT:
				if(is_key_down && isTest)
				{
					postbuf.clear();
				}
				break;
			default:
				break;
			}
		}
	}

	return FALSE;
}

STDAPI CTextService::OnSetFocus(BOOL fForeground)
{
	return S_OK;
}

STDAPI CTextService::OnTestKeyDown(ITfContext *pic, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	if(pfEaten == nullptr)
	{
		return E_INVALIDARG;
	}

	int eaten = _IsKeyEaten(pic, wParam, lParam, TRUE, TRUE);
	if(eaten == -1)
	{
		*pfEaten = TRUE;
		return S_OK;
	}
	*pfEaten = (eaten == TRUE);

	_EndInputModeWindow();
	//_EndVKeyboardWindow();

	if(!_IsKeyboardDisabled() && _IsKeyboardOpen() && !_IsComposing())
	{
		WCHAR ch = _GetCh((BYTE)wParam);
		if(_IsKeyVoid(ch, (BYTE)wParam))
		{
			_GetActiveFlags();
			_UpdateLanguageBar();
		}
	}

	return S_OK;
}

STDAPI CTextService::OnKeyDown(ITfContext *pic, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	if(pfEaten == nullptr)
	{
		return E_INVALIDARG;
	}

	int eaten = _IsKeyEaten(pic, wParam, lParam, TRUE, FALSE);
	if(eaten == -1)
	{
		*pfEaten = TRUE;
		return S_OK;
	}
	*pfEaten = (eaten == TRUE);

	if(*pfEaten)
	{
		_InvokeKeyHandler(pic, wParam, lParam, SKK_NULL);
	}

	return S_OK;
}

STDAPI CTextService::OnTestKeyUp(ITfContext *pic, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	if(pfEaten == nullptr)
	{
		return E_INVALIDARG;
	}

	int eaten = _IsKeyEaten(pic, wParam, lParam, FALSE, TRUE);
	if(eaten == -1)
	{
		*pfEaten = TRUE;
		return S_OK;
	}
	*pfEaten = (eaten == TRUE);

	return S_OK;
}

STDAPI CTextService::OnKeyUp(ITfContext *pic, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
	if(pfEaten == nullptr)
	{
		return E_INVALIDARG;
	}

	int eaten = _IsKeyEaten(pic, wParam, lParam, FALSE, FALSE);
	if(eaten == -1)
	{
		*pfEaten = TRUE;
		return S_OK;
	}
	*pfEaten = (eaten == TRUE);

	return S_OK;
}

STDAPI CTextService::OnPreservedKey(ITfContext *pic, REFGUID rguid, BOOL *pfEaten)
{
	if(pic == nullptr || pfEaten == nullptr)
	{
		return E_INVALIDARG;
	}

	BOOL fOpen = _IsKeyboardOpen();

	if(IsEqualGUID(rguid, c_guidPreservedKeyOn))
	{
		if(fOpen)
		{
			//入力途中のシーケンスはそのまま確定
			_InvokeKeyHandler(pic, 0, 0, SKK_ENTER);
			_ClearComposition();
			postbuf.clear();
		}
		else
		{
			_UpdateLanguageBar();
		}
		inputmode = im_disable;
		_SetKeyboardOpen(TRUE);
		*pfEaten = TRUE;
	}
	else if(IsEqualGUID(rguid, c_guidPreservedKeyOff))
	{
		if(fOpen)
		{
			_InvokeKeyHandler(pic, 0, 0, SKK_ENTER);
			_ClearComposition();
			postbuf.clear();
		}
		else
		{
			_UpdateLanguageBar();
		}
		_SetKeyboardOpen(FALSE);
		*pfEaten = TRUE;
	}
	else
	{
		*pfEaten = FALSE;
	}

	return S_OK;
}

BOOL CTextService::_InitKeyEventSink()
{
	HRESULT hr = E_FAIL;

	ITfKeystrokeMgr *pKeystrokeMgr = nullptr;
	if(SUCCEEDED(_pThreadMgr->QueryInterface(IID_PPV_ARGS(&pKeystrokeMgr))) && (pKeystrokeMgr != nullptr))
	{
		hr = pKeystrokeMgr->AdviseKeyEventSink(_ClientId, (ITfKeyEventSink *)this, TRUE);

		SafeRelease(&pKeystrokeMgr);
	}

	return SUCCEEDED(hr);
}

void CTextService::_UninitKeyEventSink()
{
	ITfKeystrokeMgr *pKeystrokeMgr = nullptr;
	if(SUCCEEDED(_pThreadMgr->QueryInterface(IID_PPV_ARGS(&pKeystrokeMgr))) && (pKeystrokeMgr != nullptr))
	{
		pKeystrokeMgr->UnadviseKeyEventSink(_ClientId);

		SafeRelease(&pKeystrokeMgr);
	}
}

BOOL CTextService::_InitPreservedKey(int onoff)
{
	BOOL fRet = TRUE;
	HRESULT hr;

	if(onoff != 0 && onoff != 1)
	{
		return FALSE;
	}

	ITfKeystrokeMgr *pKeystrokeMgr = nullptr;
	if(SUCCEEDED(_pThreadMgr->QueryInterface(IID_PPV_ARGS(&pKeystrokeMgr))) && (pKeystrokeMgr != nullptr))
	{
		for(int i = 0; i < MAX_PRESERVEDKEY; i++)
		{
			if(preservedkey[onoff][i].uVKey == 0 && preservedkey[onoff][i].uModifiers == 0)
			{
				break;
			}

			hr = pKeystrokeMgr->PreserveKey(_ClientId, c_guidPreservedKeyOnOff[onoff],
				&preservedkey[onoff][i], c_PreservedKeyDesc[onoff], (ULONG)wcslen(c_PreservedKeyDesc[onoff]));

			if(FAILED(hr))
			{
				fRet = FALSE;
			}
		}

		SafeRelease(&pKeystrokeMgr);
	}
	else
	{
		fRet = FALSE;
	}

	return fRet;
}

void CTextService::_UninitPreservedKey(int onoff)
{
	HRESULT hr;

	if(onoff != 0 && onoff != 1)
	{
		return;
	}

	ITfKeystrokeMgr *pKeystrokeMgr = nullptr;
	if(SUCCEEDED(_pThreadMgr->QueryInterface(IID_PPV_ARGS(&pKeystrokeMgr))) && (pKeystrokeMgr != nullptr))
	{
		for(int i = 0; i < MAX_PRESERVEDKEY; i++)
		{
			if(preservedkey[onoff][i].uVKey == 0 && preservedkey[onoff][i].uModifiers == 0)
			{
				break;
			}

			hr = pKeystrokeMgr->UnpreserveKey(c_guidPreservedKeyOnOff[onoff], &preservedkey[onoff][i]);
		}

		SafeRelease(&pKeystrokeMgr);
	}
}
