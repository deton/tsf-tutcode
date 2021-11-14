
#include "configxml.h"
#include "imcrvcnf.h"
#include "resource.h"

#define MAX_VKBDTOP 256 // (5*2(surrogate)+1(│)+5*2+2('\\','n')=23)*4

static struct {
	int id;
	LPCWSTR value;
	COLORREF color;
} displayModeColor[DISPLAY_MODE_COLOR_NUM] =
{
	{IDC_COL_MODE_MC, ValueColorMC, RGB(0xFF, 0xFF, 0xFF)},
	{IDC_COL_MODE_MF, ValueColorMF, RGB(0x00, 0x00, 0x00)},
	{IDC_COL_MODE_HR, ValueColorHR, RGB(0xC0, 0x00, 0x00)},
	{IDC_COL_MODE_KT, ValueColorKT, RGB(0x00, 0xC0, 0x00)},
	{IDC_COL_MODE_KA, ValueColorKA, RGB(0x80, 0x00, 0xC0)},
	{IDC_COL_MODE_JL, ValueColorJL, RGB(0x00, 0x00, 0xC0)},
	{IDC_COL_MODE_AC, ValueColorAC, RGB(0x00, 0x80, 0xC0)},
	{IDC_COL_MODE_DR, ValueColorDR, RGB(0x80, 0x80, 0x80)}
};

//打鍵ヘルプ表示タイミング
static const LPCWSTR cbAutoHelpText[] =
{
	L"なし", L"Help機能キー入力時", L"変換確定時も"
};

static const LPCWSTR cbAutoHelpValue[] =
{
	ValueAutoHelpOff, ValueAutoHelpOnKey, ValueAutoHelpOnConv
};

//打鍵ヘルプ表示方法
static const LPCWSTR cbShowHelpText[] =
{
	L"漢索窓", L"ドット表", L"漢字表"
};

static const LPCWSTR cbShowHelpValue[] =
{
	ValueShowHelpKansaku, ValueShowHelpDotHyo, ValueShowHelpKanjiHyo
};

INT_PTR CALLBACK DlgProcDisplay2(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	WCHAR num[16];
	WCHAR vkbdlayout[MAX_VKBDTOP] =
		L"12345│67890\\n"
		 "qwert│yuiop\\n"
		 "asdfg│hjkl;\\n"
		 "zxcvb│nm,./";
	WCHAR vkbdtop[MAX_VKBDTOP];
	int n;
	std::wstring strxmlval;
	CHOOSECOLORW cc = {};
	static COLORREF customColor[16];
	HWND hCombo;

	switch (message)
	{
	case WM_INITDIALOG:
		LoadCheckButton(hDlg, IDC_CHECKBOX_SHOWMODEINL, SectionDisplay, ValueShowModeInl, L"1");
		ReadValue(pathconfigxml, SectionDisplay, ValueShowModeInlTm, strxmlval);
		if (!strxmlval.empty())
		{
			n = _wtoi(strxmlval.c_str());
		}
		else
		{
			//for compatibility
			ReadValue(pathconfigxml, SectionDisplay, ValueShowModeSec, strxmlval);
			n = strxmlval.empty() ? -1 : _wtoi(strxmlval.c_str()) * 1000;
		}

		if (n > 60000 || n <= 0)
		{
			n = SHOWMODEINLTM_DEF;
		}
		_snwprintf_s(num, _TRUNCATE, L"%d", n);
		SetDlgItemTextW(hDlg, IDC_EDIT_SHOWMODEINLTM, num);

		for (int i = 0; i < _countof(customColor); i++)
		{
			customColor[i] = RGB(0xFF, 0xFF, 0xFF);
		}

		for (int i = 0; i < _countof(displayModeColor); i++)
		{
			ReadValue(pathconfigxml, SectionDisplay, displayModeColor[i].value, strxmlval);
			if (!strxmlval.empty())
			{
				displayModeColor[i].color = wcstoul(strxmlval.c_str(), nullptr, 0);
			}
		}

		LoadCheckButton(hDlg, IDC_CHECKBOX_SHOWVKBD, SectionDisplay, ValueShowVkbd, L"0");

		ReadValue(pathconfigxml, SectionDisplay, ValueVkbdLayout, strxmlval, L"");
		if (!strxmlval.empty())
		{
			wcsncpy_s(vkbdlayout, strxmlval.c_str(), _TRUNCATE);
		}
		SetDlgItemTextW(hDlg, IDC_EDIT_VKBDLAYOUT, vkbdlayout);
		ReadValue(pathconfigxml, SectionDisplay, ValueVkbdTop, strxmlval);
		wcsncpy_s(vkbdtop, strxmlval.c_str(), _TRUNCATE);
		SetDlgItemTextW(hDlg, IDC_EDIT_VKBDTOP, vkbdtop);

		hCombo = GetDlgItem(hDlg, IDC_COMBO_AUTOHELP);
		for (int i = 0; i < _countof(cbAutoHelpText); i++)
		{
			SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)cbAutoHelpText[i]);
		}
		ReadValue(pathconfigxml, SectionDisplay, ValueAutoHelp, strxmlval);
		n = 0;
		if (!strxmlval.empty())
		{
			for (int i = 0; i < _countof(cbAutoHelpValue); i++)
			{
				if (wcscmp(cbAutoHelpValue[i], strxmlval.c_str()) == 0)
				{
					n = i;
					break;
				}
			}
		}
		SendMessageW(hCombo, CB_SETCURSEL, (WPARAM)n, 0);

		hCombo = GetDlgItem(hDlg, IDC_COMBO_SHOWHELP);
		for (int i = 0; i < _countof(cbShowHelpText); i++)
		{
			SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)cbShowHelpText[i]);
		}
		ReadValue(pathconfigxml, SectionDisplay, ValueShowHelp, strxmlval);
		n = 1;
		if (!strxmlval.empty())
		{
			for (int i = 0; i < _countof(cbShowHelpValue); i++)
			{
				if (wcscmp(cbShowHelpValue[i], strxmlval.c_str()) == 0)
				{
					n = i;
					break;
				}
			}
		}
		SendMessageW(hCombo, CB_SETCURSEL, (WPARAM)n, 0);

		return TRUE;

	case WM_DPICHANGED_AFTERPARENT:

		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_EDIT_SHOWMODEINLTM:
		case IDC_EDIT_VKBDLAYOUT:
		case IDC_EDIT_VKBDTOP:
			switch (HIWORD(wParam))
			{
			case EN_CHANGE:
				PropSheet_Changed(GetParent(hDlg), hDlg);
				return TRUE;
			default:
				break;
			}
			break;

		case IDC_COMBO_AUTOHELP:
			switch (HIWORD(wParam))
			{
			case CBN_SELCHANGE:
				PropSheet_Changed(GetParent(hDlg), hDlg);
				return TRUE;
			default:
				break;
			}
			break;

		case IDC_CHECKBOX_SHOWMODEINL:
		case IDC_CHECKBOX_SHOWVKBD:
			PropSheet_Changed(GetParent(hDlg), hDlg);
			return TRUE;

		case IDC_COL_MODE_MC:
		case IDC_COL_MODE_MF:
		case IDC_COL_MODE_HR:
		case IDC_COL_MODE_KT:
		case IDC_COL_MODE_KA:
		case IDC_COL_MODE_JL:
		case IDC_COL_MODE_AC:
		case IDC_COL_MODE_DR:
			switch (HIWORD(wParam))
			{
			case STN_CLICKED:
			case STN_DBLCLK:
				for (int i = 0; i < _countof(displayModeColor); i++)
				{
					if (LOWORD(wParam) == displayModeColor[i].id)
					{
						cc.lStructSize = sizeof(cc);
						cc.hwndOwner = hDlg;
						cc.hInstance = nullptr;
						cc.rgbResult = displayModeColor[i].color;
						cc.lpCustColors = customColor;
						cc.Flags = CC_FULLOPEN | CC_RGBINIT;
						cc.lCustData = 0;
						cc.lpfnHook = nullptr;
						cc.lpTemplateName = nullptr;

						if (ChooseColorW(&cc))
						{
							DrawSelectColor(hDlg, displayModeColor[i].id, cc.rgbResult);
							displayModeColor[i].color = cc.rgbResult;
							PropSheet_Changed(GetParent(hDlg), hDlg);
						}
						return TRUE;
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
		break;

	case WM_PAINT:
		hdc = BeginPaint(hDlg, &ps);
		for (int i = 0; i < _countof(displayModeColor); i++)
		{
			DrawSelectColor(hDlg, displayModeColor[i].id, displayModeColor[i].color);
		}
		EndPaint(hDlg, &ps);

		return TRUE;

	case WM_DESTROY:
		PostQuitMessage(0);
		return TRUE;

	default:
		break;
	}

	return FALSE;
}

void SaveDisplay2(IXmlWriter *pWriter, HWND hDlg)
{
	WCHAR num[16];
	WCHAR vkbdlayout[MAX_VKBDTOP], vkbdtop[MAX_VKBDTOP];
	int count;
	HWND hwnd;
	int sel;

	SaveCheckButton(pWriter, hDlg, IDC_CHECKBOX_SHOWMODEINL, ValueShowModeInl);
	GetDlgItemTextW(hDlg, IDC_EDIT_SHOWMODEINLTM, num, _countof(num));
	count = _wtoi(num);
	if (count <= 0 || count > 60000)
	{
		count = SHOWMODEINLTM_DEF;
	}
	_snwprintf_s(num, _TRUNCATE, L"%d", count);
	SetDlgItemTextW(hDlg, IDC_EDIT_SHOWMODEINLTM, num);
	WriterKey(pWriter, ValueShowModeInlTm, num);

	for (int i = 0; i < _countof(displayModeColor); i++)
	{
		_snwprintf_s(num, _TRUNCATE, L"0x%06X", displayModeColor[i].color);
		WriterKey(pWriter, displayModeColor[i].value, num);
	}

	SaveCheckButton(pWriter, hDlg, IDC_CHECKBOX_SHOWVKBD, ValueShowVkbd);
	GetDlgItemTextW(hDlg, IDC_EDIT_VKBDLAYOUT, vkbdlayout, _countof(vkbdlayout));
	WriterKey(pWriter, ValueVkbdLayout, vkbdlayout);
	GetDlgItemTextW(hDlg, IDC_EDIT_VKBDTOP, vkbdtop, _countof(vkbdtop));
	WriterKey(pWriter, ValueVkbdTop, vkbdtop);

	hwnd = GetDlgItem(hDlg, IDC_COMBO_AUTOHELP);
	sel = (int)SendMessageW(hwnd, CB_GETCURSEL, 0, 0);
	WriterKey(pWriter, ValueAutoHelp, cbAutoHelpValue[sel]);

	hwnd = GetDlgItem(hDlg, IDC_COMBO_SHOWHELP);
	sel = (int)SendMessageW(hwnd, CB_GETCURSEL, 0, 0);
	WriterKey(pWriter, ValueShowHelp, cbShowHelpValue[sel]);
}
