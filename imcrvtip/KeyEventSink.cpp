
#include "imcrvtip.h"
#include "TextService.h"
#include "CandidateList.h"
#include "LanguageBar.h"
#include "mozc/win32/base/deleter.h"
#include "mozc/win32/base/keyboard.h"

static LPCWSTR c_PreservedKeyOnOffDesc = L"OnOff";
static LPCWSTR c_PreservedKeyOnDesc = L"On";
static LPCWSTR c_PreservedKeyOffDesc = L"Off";

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
	if(vk_ctrl)
	{
		postbuf.clear();
		return FALSE;
	}
	//ASCIIモード、かなキーロック
	if(inputmode == im_ascii && !vk_kana)
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
	int eaten = _IsKeyEaten(pic, wParam, lParam, TRUE, TRUE);
	if(eaten == -1)
	{
		*pfEaten = TRUE;
		return S_OK;
	}
	*pfEaten = (eaten == TRUE);

	if(_pCandidateList == NULL || !_pCandidateList->_IsShowCandidateWindow())
	{
		//ASCIIモード
		if(inputmode == im_ascii)
		{
			WCHAR ch = _GetCh((BYTE)wParam);
			//無効
			if(_IsKeyVoid(ch, (BYTE)wParam))
			{
				_UpdateLanguageBar();
			}
			else if(ch != L'\0')
			{
				_ClearComposition();
			}
		}
	}
	return S_OK;
}

STDAPI CTextService::OnKeyDown(ITfContext *pic, WPARAM wParam, LPARAM lParam, BOOL *pfEaten)
{
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
	if(IsEqualGUID(rguid, c_guidPreservedKeyOnOff))
	{
		BOOL fOpen = _IsKeyboardOpen();
		if(!fOpen)
		{
			inputmode = im_disable;
		}
		else
		{
			_ClearComposition();
			postbuf.clear();
		}
		_SetKeyboardOpen(fOpen ? FALSE : TRUE);
		*pfEaten = TRUE;
	}
	else if(IsEqualGUID(rguid, c_guidPreservedKeyOn))
	{
		BOOL fOpen = _IsKeyboardOpen();
		inputmode = im_disable;
		_SetKeyboardOpen(TRUE);
		*pfEaten = TRUE;
	}
	else if(IsEqualGUID(rguid, c_guidPreservedKeyOff))
	{
		BOOL fOpen = _IsKeyboardOpen();
		if(fOpen)
		{
			_ClearComposition();
			postbuf.clear();
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
	ITfKeystrokeMgr *pKeystrokeMgr;
	HRESULT hr = E_FAIL;

	if(_pThreadMgr->QueryInterface(IID_PPV_ARGS(&pKeystrokeMgr)) == S_OK)
	{
		hr = pKeystrokeMgr->AdviseKeyEventSink(_ClientId, (ITfKeyEventSink *)this, TRUE);
		pKeystrokeMgr->Release();
	}

	return (hr == S_OK);
}

void CTextService::_UninitKeyEventSink()
{
	ITfKeystrokeMgr *pKeystrokeMgr;

	if(_pThreadMgr->QueryInterface(IID_PPV_ARGS(&pKeystrokeMgr)) == S_OK)
	{
		pKeystrokeMgr->UnadviseKeyEventSink(_ClientId);
		pKeystrokeMgr->Release();
	}
}

#define _PRESERVE_KEY(preservedkey, c_guidPreservedKey, c_PreservedKeyDesc) \
do \
{ \
	for(i = 0; i < MAX_PRESERVEDKEY; i++) \
	{ \
		if(preservedkey[i].uVKey == 0 && preservedkey[i].uModifiers == 0) \
		{ \
			break; \
		} \
		hr = pKeystrokeMgr->PreserveKey(_ClientId, c_guidPreservedKey, \
			&preservedkey[i], c_PreservedKeyDesc, (ULONG)wcslen(c_PreservedKeyDesc)); \
	} \
} while(0)

BOOL CTextService::_InitPreservedKey()
{
	ITfKeystrokeMgr *pKeystrokeMgr;
	HRESULT hr = E_FAIL;
	int i;

	if(_pThreadMgr->QueryInterface(IID_PPV_ARGS(&pKeystrokeMgr)) == S_OK)
	{
		_PRESERVE_KEY(preservedkeyon, c_guidPreservedKeyOn, c_PreservedKeyOnDesc);
		_PRESERVE_KEY(preservedkeyoff, c_guidPreservedKeyOff, c_PreservedKeyOffDesc);
		_PRESERVE_KEY(preservedkeyonoff, c_guidPreservedKeyOnOff, c_PreservedKeyOnOffDesc);

		pKeystrokeMgr->Release();
	}

	return (hr == S_OK);
}

#define _UNPRESERVE_KEY(preservedkey, c_guidPreservedKey) \
do \
{ \
	for(i = 0; i < MAX_PRESERVEDKEY; i++) \
	{ \
		if(preservedkey[i].uVKey == 0 && preservedkey[i].uModifiers == 0) \
		{ \
			break; \
		} \
		pKeystrokeMgr->UnpreserveKey(c_guidPreservedKey, &preservedkey[i]); \
	} \
} while(0)

void CTextService::_UninitPreservedKey()
{
	ITfKeystrokeMgr *pKeystrokeMgr;
	int i;

	if(_pThreadMgr->QueryInterface(IID_PPV_ARGS(&pKeystrokeMgr)) == S_OK)
	{
		_UNPRESERVE_KEY(preservedkeyon, c_guidPreservedKeyOn);
		_UNPRESERVE_KEY(preservedkeyoff, c_guidPreservedKeyOff);
		_UNPRESERVE_KEY(preservedkeyonoff, c_guidPreservedKeyOnOff);

		pKeystrokeMgr->Release();
	}
}
