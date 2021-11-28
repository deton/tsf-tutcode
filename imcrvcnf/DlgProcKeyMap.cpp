﻿
#include "configxml.h"
#include "imcrvcnf.h"
#include "resource.h"

static const struct {
	int idd;
	LPCWSTR keyName;
	LPCWSTR defaultValue;
} KeyMap[2][27] =
{
	{
		{IDC_EDIT_KANA,			ValueKeyMapKana,		L"q"},
		{IDC_EDIT_CONV_CHAR,	ValueKeyMapConvChar,	L"\\cq"},
		{IDC_EDIT_JLATIN,		ValueKeyMapJLatin,		L"L"},
		{IDC_EDIT_ASCII,		ValueKeyMapAscii,		L"l"},
		{IDC_EDIT_JMODE,		ValueKeyMapJMode,		L""},
		{IDC_EDIT_ABBREV,		ValueKeyMapAbbrev,		L"/"},
		{IDC_EDIT_AFFIX,		ValueKeyMapAffix,		L"<|>"},
		{IDC_EDIT_NEXT_CAND,	ValueKeyMapNextCand,	L"\\x20|\\cn"},
		{IDC_EDIT_PREV_CAND,	ValueKeyMapPrevCand,	L"x|\\cp"},
		{IDC_EDIT_PURGE_DIC,	ValueKeyMapPurgeDic,	L"X|\\cx"},
		{IDC_EDIT_NEXT_COMP,	ValueKeyMapNextComp,	L"\\ci"},
		{IDC_EDIT_PREV_COMP,	ValueKeyMapPrevComp,	L"\\cu"},
		{IDC_EDIT_COMP_CAND,	ValueKeyMapCompCand,	L"\\cc"},
		{IDC_EDIT_HINT,			ValueKeyMapHint,		L""},
		{IDC_EDIT_CONV_POINT,	ValueKeyMapConvPoint,	L""},
		{IDC_EDIT_DIRECT,		ValueKeyMapDirect,		L"[0-9]"},
		{IDC_EDIT_ENTER,		ValueKeyMapEnter,		L"\\cm|\\cj"},
		{IDC_EDIT_CANCEL,		ValueKeyMapCancel,		L"\\cg|\\x1B"},
		{IDC_EDIT_BACK,			ValueKeyMapBack,		L"\\ch"},
		{IDC_EDIT_DELETE,		ValueKeyMapDelete,		L"\\cd|\\x7F"},
		{IDC_EDIT_VOID,			ValueKeyMapVoid,		L""},
		{IDC_EDIT_LEFT,			ValueKeyMapLeft,		L"\\cb"},
		{IDC_EDIT_UP,			ValueKeyMapUp,			L"\\ca"},
		{IDC_EDIT_RIGHT,		ValueKeyMapRight,		L"\\cf"},
		{IDC_EDIT_DOWN,			ValueKeyMapDown,		L"\\ce"},
		{IDC_EDIT_PASTE,		ValueKeyMapPaste,		L"\\cy|\\cv"},
		{IDC_EDIT_RECONVERT,	ValueKeyMapReconvert,	L""}
	},
	{
		{IDC_EDIT_KANA,			ValueKeyMapKana,		L""},
		{IDC_EDIT_CONV_CHAR,	ValueKeyMapConvChar,	L""},
		{IDC_EDIT_JLATIN,		ValueKeyMapJLatin,		L""},
		{IDC_EDIT_ASCII,		ValueKeyMapAscii,		L""},
		{IDC_EDIT_JMODE,		ValueKeyMapJMode,		L"C\\x4A"},
		{IDC_EDIT_ABBREV,		ValueKeyMapAbbrev,		L""},
		{IDC_EDIT_AFFIX,		ValueKeyMapAffix,		L""},
		{IDC_EDIT_NEXT_CAND,	ValueKeyMapNextCand,	L"\\x22"},
		{IDC_EDIT_PREV_CAND,	ValueKeyMapPrevCand,	L"\\x21"},
		{IDC_EDIT_PURGE_DIC,	ValueKeyMapPurgeDic,	L""},
		{IDC_EDIT_NEXT_COMP,	ValueKeyMapNextComp,	L""},
		{IDC_EDIT_PREV_COMP,	ValueKeyMapPrevComp,	L"S\\x09"},
		{IDC_EDIT_COMP_CAND,	ValueKeyMapCompCand,	L""},
		{IDC_EDIT_HINT,			ValueKeyMapHint,		L""},
		{IDC_EDIT_CONV_POINT,	ValueKeyMapConvPoint,	L""},
		{IDC_EDIT_DIRECT,		ValueKeyMapDirect,		L""},
		{IDC_EDIT_ENTER,		ValueKeyMapEnter,		L""},
		{IDC_EDIT_CANCEL,		ValueKeyMapCancel,		L""},
		{IDC_EDIT_BACK,			ValueKeyMapBack,		L""},
		{IDC_EDIT_DELETE,		ValueKeyMapDelete,		L"\\x2E"},
		{IDC_EDIT_VOID,			ValueKeyMapVoid,		L"C\\x4A"},
		{IDC_EDIT_LEFT,			ValueKeyMapLeft,		L"\\x25"},
		{IDC_EDIT_UP,			ValueKeyMapUp,			L"\\x26"},
		{IDC_EDIT_RIGHT,		ValueKeyMapRight,		L"\\x27"},
		{IDC_EDIT_DOWN,			ValueKeyMapDown,		L"\\x28"},
		{IDC_EDIT_PASTE,		ValueKeyMapPaste,		L""},
		{IDC_EDIT_RECONVERT,	ValueKeyMapReconvert,	L""}
	}
};

static LPCWSTR SectionName[2] = {SectionKeyMap, SectionVKeyMap};

INT_PTR CALLBACK DlgProcKeyMap(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam, int no);
void LoadKeyMap(HWND hDlg, int nIDDlgItem, LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault);

INT_PTR CALLBACK DlgProcKeyMap1(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DlgProcKeyMap(hDlg, message, wParam, lParam, 0);
}

INT_PTR CALLBACK DlgProcKeyMap2(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DlgProcKeyMap(hDlg, message, wParam, lParam, 1);
}

INT_PTR CALLBACK DlgProcKeyMap(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam, int no)
{
	switch (message)
	{
	case WM_INITDIALOG:
		for (int i = 0; i < _countof(KeyMap[no]); i++)
		{
			LoadKeyMap(hDlg, KeyMap[no][i].idd, SectionName[no], KeyMap[no][i].keyName, KeyMap[no][i].defaultValue);
		}
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_EDIT_KANA:
		case IDC_EDIT_CONV_CHAR:
		case IDC_EDIT_JLATIN:
		case IDC_EDIT_ASCII:
		case IDC_EDIT_JMODE:
		case IDC_EDIT_ABBREV:
		case IDC_EDIT_AFFIX:
		case IDC_EDIT_NEXT_CAND:
		case IDC_EDIT_PREV_CAND:
		case IDC_EDIT_PURGE_DIC:
		case IDC_EDIT_NEXT_COMP:
		case IDC_EDIT_PREV_COMP:
		case IDC_EDIT_COMP_CAND:
		case IDC_EDIT_HINT:
		case IDC_EDIT_CONV_POINT:
		case IDC_EDIT_DIRECT:
		case IDC_EDIT_ENTER:
		case IDC_EDIT_CANCEL:
		case IDC_EDIT_BACK:
		case IDC_EDIT_DELETE:
		case IDC_EDIT_VOID:
		case IDC_EDIT_LEFT:
		case IDC_EDIT_UP:
		case IDC_EDIT_RIGHT:
		case IDC_EDIT_DOWN:
		case IDC_EDIT_PASTE:
		case IDC_EDIT_RECONVERT:
			switch (HIWORD(wParam))
			{
			case EN_CHANGE:
				PropSheet_Changed(GetParent(hDlg), hDlg);
				return TRUE;
			default:
				break;
			}
			break;
		default:
			break;
		}
		break;

	case WM_NOTIFY:
		if (lParam == NULL) break;
		switch (((LPNMHDR)lParam)->code)
		{
		case PSN_TRANSLATEACCELERATOR:
		{
			WCHAR vkeytext[8];
			LPMSG lpMsg = (LPMSG)((LPPSHNOTIFY)lParam)->lParam;
			if (lpMsg == NULL) break;
			switch (lpMsg->message)
			{
			case WM_KEYDOWN:
			case WM_SYSKEYDOWN:
				switch (GetDlgCtrlID(lpMsg->hwnd))
				{
				case IDC_EDIT_KEYMAP_TEST_VKEY:
					_snwprintf_s(vkeytext, _TRUNCATE, L"\\x%02X", (BYTE)lpMsg->wParam);
					SetDlgItemTextW(hDlg, IDC_EDIT_KEYMAP_TEST_VKEY, vkeytext);
					SendDlgItemMessageW(hDlg, IDC_EDIT_KEYMAP_TEST_VKEY, EM_SETSEL, 4, 4);
					SetWindowLongPtrW(hDlg, DWLP_MSGRESULT, PSNRET_MESSAGEHANDLED);
					return TRUE;
				default:
					break;
				}
				break;
			default:
				break;
			}
		}
		break;
		default:
			break;
		}
		break;

	default:
		break;
	}

	return FALSE;
}

void LoadKeyMap(HWND hDlg, int nIDDlgItem, LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault)
{
	std::wstring strxmlval;
	LPCWSTR lpDefVal = L"\uFFFF";

	ReadValue(pathconfigxml, lpAppName, lpKeyName, strxmlval, lpDefVal);
	if (strxmlval == lpDefVal) strxmlval = lpDefault;
	SetDlgItemTextW(hDlg, nIDDlgItem, strxmlval.c_str());
}

void SaveKeyMap(IXmlWriter *pWriter, HWND hDlg, int nIDDlgItem, LPCWSTR lpKeyName)
{
	WCHAR keyre[MAX_KEYRE];

	GetDlgItemTextW(hDlg, nIDDlgItem, keyre, _countof(keyre));
	WriterKey(pWriter, lpKeyName, keyre);
}

void SaveCKeyMap(IXmlWriter *pWriter, HWND hDlg)
{
	for (int i = 0; i < _countof(KeyMap[0]); i++)
	{
		SaveKeyMap(pWriter, hDlg, KeyMap[0][i].idd, KeyMap[0][i].keyName);
	}
}

void SaveVKeyMap(IXmlWriter *pWriter, HWND hDlg)
{
	for (int i = 0; i < _countof(KeyMap[1]); i++)
	{
		SaveKeyMap(pWriter, hDlg, KeyMap[1][i].idd, KeyMap[1][i].keyName);
	}
}
