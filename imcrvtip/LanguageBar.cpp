﻿
#include "imcrvtip.h"
#include "TextService.h"
#include "EditSession.h"
#include "CandidateList.h"
#include "LanguageBar.h"
#include "InputModeWindow.h"
#include "resource.h"

#define TEXTSERVICE_LANGBARITEMSINK_COOKIE 0x54ab516b

// langbar menu items
static const struct {
	int inputmode;
	UINT id;
	DWORD flag;
	LPCWSTR text;
} menuItems[] =
{
	{im_disable,		IDM_CAPSLOCK,		0, L"CAPS"},
	{im_disable,		IDM_KANALOCK,		0, L"KANA"},
	{im_disable,		IDM_NONE,			TF_LBMENUF_SEPARATOR, L""},
	{im_hiragana,		IDM_HIRAGANA,		0, L"［かな］"},
	{im_katakana,		IDM_KATAKANA,		0, L"［カナ］"},
	{im_katakana_ank,	IDM_KATAKANA_ANK,	0, L"［－ｶﾅ］"},
	{im_jlatin,			IDM_JLATIN,			0, L"［全英］"},
	{im_ascii,			IDM_ASCII,			0, L"［SKK］"},
	{im_direct,			IDM_DIRECT,			0, L"［－－］"},
	{im_disable,		IDM_NONE,			TF_LBMENUF_SEPARATOR, L""},
	{im_disable,		IDM_CONFIG,			0, L"設定"},
	{im_disable,		IDM_NONE,			TF_LBMENUF_SEPARATOR, L""},
	{im_disable,		IDM_NONE,			0, L"キャンセル"}
};

// 16 colors (black only) icons
static const WORD iconIDX[] =
{
	IDI_X_DEFAULT, IDI_X_HIRAGANA, IDI_X_KATAKANA, IDI_X_KATAKANA1, IDI_X_JLATIN, IDI_X_ASCII
};
// png icons
static const WORD iconIDZ[] =
{
	IDI_Z_DEFAULT, IDI_Z_HIRAGANA, IDI_Z_KATAKANA, IDI_Z_KATAKANA1, IDI_Z_JLATIN, IDI_Z_ASCII
};

CLangBarItemButton::CLangBarItemButton(CTextService *pTextService, REFGUID guid)
{
	DllAddRef();

	_cRef = 1;

	_LangBarItemInfo.clsidService = c_clsidTextService;
	_LangBarItemInfo.guidItem = guid;
	// GDI handle leak occurs when GetIcon function returns monochrome icon if TF_LBI_STYLE_TEXTCOLORICON flag is set.
	// Use 16 colors (black only) icon instead of monochrome icon.
	_LangBarItemInfo.dwStyle = TF_LBI_STYLE_SHOWNINTRAY |
		(IsEqualGUID(_LangBarItemInfo.guidItem, GUID_LBI_INPUTMODE) ? TF_LBI_STYLE_BTN_BUTTON : TF_LBI_STYLE_BTN_MENU) |
		(IsWindowsVersion62OrLater() ? 0 : TF_LBI_STYLE_TEXTCOLORICON);	//16 colors (black only) icon used on earlier than Windows 8
	_LangBarItemInfo.ulSort = 1;
	wcsncpy_s(_LangBarItemInfo.szDescription, LangbarItemDesc, _TRUNCATE);

	_pLangBarItemSink = nullptr;

	_pTextService = pTextService;
}

CLangBarItemButton::~CLangBarItemButton()
{
	_pTextService.Release();

	DllRelease();
}

STDAPI CLangBarItemButton::QueryInterface(REFIID riid, void **ppvObj)
{
	if(ppvObj == nullptr)
	{
		return E_INVALIDARG;
	}

	*ppvObj = nullptr;

	if(IsEqualIID(riid, IID_IUnknown) ||
		IsEqualIID(riid, IID_ITfLangBarItem) ||
		IsEqualIID(riid, IID_ITfLangBarItemButton))
	{
		*ppvObj = (ITfLangBarItemButton *)this;
	}
	else if(IsEqualIID(riid, IID_ITfSource))
	{
		*ppvObj = (ITfSource *)this;
	}

	if(*ppvObj)
	{
		AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

STDAPI_(ULONG) CLangBarItemButton::AddRef()
{
	return ++_cRef;
}

STDAPI_(ULONG) CLangBarItemButton::Release()
{
	if(--_cRef == 0)
	{
		delete this;
		return 0;
	}

	return _cRef;
}

STDAPI CLangBarItemButton::GetInfo(TF_LANGBARITEMINFO *pInfo)
{
	if(pInfo == nullptr)
	{
		return E_INVALIDARG;
	}

	*pInfo = _LangBarItemInfo;

	return S_OK;
}

STDAPI CLangBarItemButton::GetStatus(DWORD *pdwStatus)
{
	if(pdwStatus == nullptr)
	{
		return E_INVALIDARG;
	}

	if(_pTextService->_IsKeyboardDisabled())
	{
		*pdwStatus = TF_LBI_STATUS_DISABLED;
	}
	else
	{
		*pdwStatus = 0;
	}

	return S_OK;
}

STDAPI CLangBarItemButton::Show(BOOL fShow)
{
	if(_pLangBarItemSink == nullptr)
	{
		return E_FAIL;
	}

	return _pLangBarItemSink->OnUpdate(TF_LBI_STATUS);
}

STDAPI CLangBarItemButton::GetTooltipString(BSTR *pbstrToolTip)
{
	BSTR bstrToolTip = nullptr;

	if(pbstrToolTip == nullptr)
	{
		return E_INVALIDARG;
	}

	*pbstrToolTip = nullptr;

	bstrToolTip = SysAllocString(LangbarItemDesc);

	if(bstrToolTip == nullptr)
	{
		return E_OUTOFMEMORY;
	}

	*pbstrToolTip = bstrToolTip;

	return S_OK;
}

STDAPI CLangBarItemButton::OnClick(TfLBIClick click, POINT pt, const RECT *prcArea)
{
	if(IsEqualGUID(_LangBarItemInfo.guidItem, GUID_LBI_INPUTMODE))
	{
		switch(click)
		{
		case TF_LBI_CLK_RIGHT:
			{
				HMENU hMenu = LoadMenuW(g_hInst, MAKEINTRESOURCEW(IDR_SYSTRAY_MENU));
				if(hMenu)
				{
					UINT check = IDM_DIRECT;
					for(int i = 0; i < _countof(menuItems); i++)
					{
						if(_pTextService->inputmode == menuItems[i].inputmode)
						{
							check = menuItems[i].id;
							break;
						}
					}
					CheckMenuRadioItem(hMenu, IDM_HIRAGANA, IDM_DIRECT, check, MF_BYCOMMAND);
					CheckMenuItem(hMenu, IDM_CAPSLOCK,
						MF_BYCOMMAND | ((GetKeyState(VK_CAPITAL) & 1) == 1 ? MF_CHECKED : MF_UNCHECKED));
					CheckMenuItem(hMenu, IDM_KANALOCK,
						MF_BYCOMMAND | ((GetKeyState(VK_KANA) & 1) == 1 ? MF_CHECKED : MF_UNCHECKED));
					HMENU hSubMenu = GetSubMenu(hMenu, 0);
					if(hSubMenu)
					{
						TPMPARAMS tpm;
						TPMPARAMS *ptpm = nullptr;
						if(prcArea != nullptr)
						{
							tpm.cbSize = sizeof(tpm);
							tpm.rcExclude = *prcArea;
							ptpm = &tpm;
						}
						BOOL bRet = TrackPopupMenuEx(hSubMenu,
							TPM_LEFTALIGN | TPM_TOPALIGN | TPM_NONOTIFY | TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_VERTICAL,
							pt.x, pt.y, GetFocus(), ptpm);
						this->OnMenuSelect(bRet);
					}
					DestroyMenu(hMenu);
				}
			}
			break;
		case TF_LBI_CLK_LEFT:
			{
				BOOL fOpen = _pTextService->_IsKeyboardOpen();

				if(fOpen)
				{
					_pTextService->_ClearComposition();
				}
				else
				{
					_pTextService->inputmode = im_disable;
				}

				_pTextService->_SetKeyboardOpen(fOpen ? FALSE : TRUE);
			}
			break;
		default:
			break;
		}
	}

	return S_OK;
}

STDAPI CLangBarItemButton::InitMenu(ITfMenu *pMenu)
{
	if(pMenu == nullptr)
	{
		return E_INVALIDARG;
	}

	for(int i = 0; i < _countof(menuItems); i++)
	{
		switch(menuItems[i].id)
		{
		case IDM_CAPSLOCK:
			pMenu->AddMenuItem(menuItems[i].id, menuItems[i].flag |
				((GetKeyState(VK_CAPITAL) & 1) == 1 ? TF_LBMENUF_CHECKED : 0),
				nullptr, nullptr, menuItems[i].text, (ULONG)wcslen(menuItems[i].text), nullptr);
			break;
		case IDM_KANALOCK:
			pMenu->AddMenuItem(menuItems[i].id, menuItems[i].flag |
				((GetKeyState(VK_KANA) & 1) == 1 ? TF_LBMENUF_CHECKED : 0),
				nullptr, nullptr, menuItems[i].text, (ULONG)wcslen(menuItems[i].text), nullptr);
			break;
		case IDM_HIRAGANA:
		case IDM_KATAKANA:
		case IDM_KATAKANA_ANK:
		case IDM_JLATIN:
		case IDM_ASCII:
		case IDM_DIRECT:
		default:
			pMenu->AddMenuItem(menuItems[i].id, menuItems[i].flag |
				(_pTextService->inputmode == menuItems[i].inputmode ? TF_LBMENUF_RADIOCHECKED : 0),
				nullptr, nullptr, menuItems[i].text, (ULONG)wcslen(menuItems[i].text), nullptr);
			break;
		}
	}

	return S_OK;
}

STDAPI CLangBarItemButton::OnMenuSelect(UINT wID)
{
	switch(wID)
	{
	case IDM_CAPSLOCK:
	case IDM_KANALOCK:
		switch(wID)
		{
		case IDM_CAPSLOCK:
			_pTextService->_CommandDic(REQ_CAPS_LOCK);
			break;
		case IDM_KANALOCK:
			_pTextService->_CommandDic(REQ_KANA_LOCK);
			break;
		default:
			break;
		}
		break;
	case IDM_CONFIG:
		_pTextService->_StartConfigure();
		break;
	case IDM_HIRAGANA:
	case IDM_KATAKANA:
	case IDM_KATAKANA_ANK:
	case IDM_JLATIN:
	case IDM_ASCII:
		for(int i = 0; i < _countof(menuItems); i++)
		{
			if(wID == menuItems[i].id)
			{
				if(_pTextService->_IsKeyboardOpen())
				{
					_pTextService->_ClearComposition();
				}
				else
				{
					_pTextService->inputmode = im_disable;
					_pTextService->_SetKeyboardOpen(TRUE);
				}

				_pTextService->inputmode = menuItems[i].inputmode;
				_pTextService->_UpdateLanguageBar();
				break;
			}
		}
		break;
	case IDM_DIRECT:
		if(_pTextService->_IsKeyboardOpen())
		{
			_pTextService->_ClearComposition();

			_pTextService->inputmode = im_direct;
			_pTextService->_SetKeyboardOpen(FALSE);
		}
		break;
	default:
		break;
	}

	return S_OK;
}

STDAPI CLangBarItemButton::GetIcon(HICON *phIcon)
{
	if(phIcon == nullptr)
	{
		return E_INVALIDARG;
	}

	HDC hdc = GetDC(nullptr);
	int size = MulDiv(16, GetDeviceCaps(hdc, LOGPIXELSY), C_USER_DEFAULT_SCREEN_DPI);
	ReleaseDC(nullptr, hdc);

	_GetIcon(phIcon, size, IsWindowsVersion62OrLater());

	return (*phIcon != nullptr) ? S_OK : E_FAIL;
}

STDAPI CLangBarItemButton::GetText(BSTR *pbstrText)
{
	BSTR bstrText = nullptr;

	if(pbstrText == nullptr)
	{
		return E_INVALIDARG;
	}

	*pbstrText = nullptr;

	bstrText = SysAllocString(LangbarItemDesc);

	if(bstrText == nullptr)
	{
		return E_OUTOFMEMORY;
	}

	*pbstrText = bstrText;

	return S_OK;
}

STDAPI CLangBarItemButton::AdviseSink(REFIID riid, IUnknown *punk, DWORD *pdwCookie)
{
	if(!IsEqualIID(IID_ITfLangBarItemSink, riid))
	{
		return CONNECT_E_CANNOTCONNECT;
	}

	if(_pLangBarItemSink != nullptr)
	{
		return CONNECT_E_ADVISELIMIT;
	}

	if(FAILED(punk->QueryInterface(IID_PPV_ARGS(&_pLangBarItemSink))))
	{
		_pLangBarItemSink.Release();
		return E_NOINTERFACE;
	}

	*pdwCookie = TEXTSERVICE_LANGBARITEMSINK_COOKIE;

	return S_OK;
}

STDAPI CLangBarItemButton::UnadviseSink(DWORD dwCookie)
{
	if(dwCookie != TEXTSERVICE_LANGBARITEMSINK_COOKIE)
	{
		return CONNECT_E_NOCONNECTION;
	}

	if(_pLangBarItemSink == nullptr)
	{
		return CONNECT_E_NOCONNECTION;
	}

	_pLangBarItemSink.Release();

	return S_OK;
}

HRESULT CLangBarItemButton::_Update()
{
	CComVariant var;

	V_VT(&var) = VT_I4;

	V_I4(&var) = TF_SENTENCEMODE_PHRASEPREDICT;
	_pTextService->_SetCompartment(GUID_COMPARTMENT_KEYBOARD_INPUTMODE_SENTENCE, &var);

	if(!_pTextService->_IsKeyboardDisabled() && _pTextService->_IsKeyboardOpen())
	{
		switch(_pTextService->inputmode)
		{
		case im_hiragana:
			V_I4(&var) = TF_CONVERSIONMODE_NATIVE | TF_CONVERSIONMODE_FULLSHAPE |
				TF_CONVERSIONMODE_ROMAN;
			_pTextService->_SetCompartment(GUID_COMPARTMENT_KEYBOARD_INPUTMODE_CONVERSION, &var);
			break;
		case im_katakana:
			V_I4(&var) = TF_CONVERSIONMODE_NATIVE | TF_CONVERSIONMODE_FULLSHAPE |
				TF_CONVERSIONMODE_ROMAN | TF_CONVERSIONMODE_KATAKANA;
			_pTextService->_SetCompartment(GUID_COMPARTMENT_KEYBOARD_INPUTMODE_CONVERSION, &var);
			break;
		case im_katakana_ank:
			V_I4(&var) = TF_CONVERSIONMODE_NATIVE |
				TF_CONVERSIONMODE_ROMAN | TF_CONVERSIONMODE_KATAKANA;
			_pTextService->_SetCompartment(GUID_COMPARTMENT_KEYBOARD_INPUTMODE_CONVERSION, &var);
			break;
		case im_jlatin:
			V_I4(&var) = TF_CONVERSIONMODE_ALPHANUMERIC | TF_CONVERSIONMODE_FULLSHAPE;
			_pTextService->_SetCompartment(GUID_COMPARTMENT_KEYBOARD_INPUTMODE_CONVERSION, &var);
			break;
		case im_ascii:
			V_I4(&var) = TF_CONVERSIONMODE_ALPHANUMERIC;
			_pTextService->_SetCompartment(GUID_COMPARTMENT_KEYBOARD_INPUTMODE_CONVERSION, &var);
			break;
		default:
			break;
		}
	}

	if(_pLangBarItemSink == nullptr)
	{
		return E_FAIL;
	}

	return _pLangBarItemSink->OnUpdate(TF_LBI_ICON | TF_LBI_STATUS);
}

HRESULT CLangBarItemButton::_GetIcon(HICON *phIcon, INT size, BOOL bNT62)
{
	size_t iconindex = 0;
	WORD iconid = 0;

	if(!_pTextService->_IsKeyboardDisabled() && _pTextService->_IsKeyboardOpen())
	{
		switch(_pTextService->inputmode)
		{
		case im_hiragana:
			iconindex = 1;
			break;
		case im_katakana:
			iconindex = 2;
			break;
		case im_katakana_ank:
			iconindex = 3;
			break;
		case im_jlatin:
			iconindex = 4;
			break;
		case im_ascii:
			iconindex = 5;
			break;
		default:
			break;
		}
	}

	if(bNT62)
	{
		if(iconindex < _countof(iconIDZ))
		{
			iconid = iconIDZ[iconindex];
		}
	}
	else
	{
		if(iconindex < _countof(iconIDX))
		{
			iconid = iconIDX[iconindex];
		}
	}

	*phIcon = (HICON)LoadImageW(g_hInst, MAKEINTRESOURCEW(iconid), IMAGE_ICON, size, size, LR_SHARED);

	return (*phIcon != nullptr) ? S_OK : E_FAIL;
}

BOOL CTextService::_InitLanguageBar()
{
	BOOL fRet = FALSE;
	BOOL fRetI = FALSE;

	_pLangBarItem = nullptr;
	_pLangBarItemI = nullptr;

	CComPtr<ITfLangBarItemMgr> pLangBarItemMgr;
	if(SUCCEEDED(_pThreadMgr->QueryInterface(IID_PPV_ARGS(&pLangBarItemMgr))) && (pLangBarItemMgr != nullptr))
	{
		try
		{
			_pLangBarItem.Attach(new CLangBarItemButton(this, c_guidLangBarItemButton));
			if(SUCCEEDED(pLangBarItemMgr->AddItem(_pLangBarItem)))
			{
				fRet = TRUE;
			}
			else
			{
				_pLangBarItem.Release();
			}
		}
		catch(...)
		{
		}

		if(IsWindowsVersion62OrLater())
		{
			try
			{
				_pLangBarItemI.Attach(new CLangBarItemButton(this, GUID_LBI_INPUTMODE));
				if(SUCCEEDED(pLangBarItemMgr->AddItem(_pLangBarItemI)))
				{
					fRetI = TRUE;
				}
				else
				{
					_pLangBarItemI.Release();
				}
			}
			catch(...)
			{
			}
		}
		else
		{
			fRetI = TRUE;
		}
	}

	return (fRet && fRetI);
}

void CTextService::_UninitLanguageBar()
{
	CComPtr<ITfLangBarItemMgr> pLangBarItemMgr;
	if(SUCCEEDED(_pThreadMgr->QueryInterface(IID_PPV_ARGS(&pLangBarItemMgr))) && (pLangBarItemMgr != nullptr))
	{
		if(_pLangBarItem != nullptr)
		{
			pLangBarItemMgr->RemoveItem(_pLangBarItem);
		}

		if(_pLangBarItemI != nullptr)
		{
			pLangBarItemMgr->RemoveItem(_pLangBarItemI);
		}
	}

	_pLangBarItem.Release();
	_pLangBarItemI.Release();
}

void CTextService::_UpdateLanguageBar(BOOL showinputmode)
{
	if(_pLangBarItem != nullptr)
	{
		_pLangBarItem->_Update();
	}

	if(_pLangBarItemI != nullptr)
	{
		_pLangBarItemI->_Update();
	}

	if(showinputmode && _ShowInputMode && !_IsComposing())
	{
		_StartInputModeWindow();
	}
	else
	{
		_EndInputModeWindow();
	}
}

void CTextService::_GetIcon(HICON *phIcon, INT size)
{
	if(_pLangBarItem != nullptr)
	{
		_pLangBarItem->_GetIcon(phIcon, size, FALSE);
	}
}
