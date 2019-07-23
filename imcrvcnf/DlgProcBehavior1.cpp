﻿
#include "configxml.h"
#include "imcrvcnf.h"
#include "resource.h"

INT_PTR CALLBACK DlgProcBehavior1(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		LoadCheckButton(hDlg, IDC_CHECKBOX_DEFAULTMODE, SectionBehavior, ValueDefaultMode);
		LoadCheckButton(hDlg, IDC_RADIO_DEFMODEASCII, SectionBehavior, ValueDefModeAscii);
		if (!IsDlgButtonChecked(hDlg, IDC_RADIO_DEFMODEASCII))
		{
			CheckDlgButton(hDlg, IDC_RADIO_DEFMODEHIRA, BST_CHECKED);
		}
		LoadCheckButton(hDlg, IDC_CHECKBOX_BEGINCVOKURI, SectionBehavior, ValueBeginCvOkuri, L"1");
		LoadCheckButton(hDlg, IDC_CHECKBOX_PRECEDEOKURI, SectionBehavior, ValuePrecedeOkuri);
		LoadCheckButton(hDlg, IDC_CHECKBOX_SHIFTNNOKURI, SectionBehavior, ValueShiftNNOkuri, L"1");
		LoadCheckButton(hDlg, IDC_CHECKBOX_SRCHALLOKURI, SectionBehavior, ValueSrchAllOkuri);
		LoadCheckButton(hDlg, IDC_CHECKBOX_DELCVPOSCNCL, SectionBehavior, ValueDelCvPosCncl, L"1");
		LoadCheckButton(hDlg, IDC_CHECKBOX_DELOKURICNCL, SectionBehavior, ValueDelOkuriCncl);
		LoadCheckButton(hDlg, IDC_CHECKBOX_BACKINCENTER, SectionBehavior, ValueBackIncEnter, L"1");
		LoadCheckButton(hDlg, IDC_CHECKBOX_ADDCANDKTKN, SectionBehavior, ValueAddCandKtkn);
		LoadCheckButton(hDlg, IDC_CHECKBOX_ENTOGGLEKANA, SectionBehavior, ValueEnToggleKana, L"1");
		LoadCheckButton(hDlg, IDC_CHECKBOX_SETBYDIRECT, SectionBehavior, ValueSetByDirect);

		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_CHECKBOX_DEFAULTMODE:
		case IDC_RADIO_DEFMODEHIRA:
		case IDC_RADIO_DEFMODEASCII:
		case IDC_CHECKBOX_BEGINCVOKURI:
		case IDC_CHECKBOX_PRECEDEOKURI:
		case IDC_CHECKBOX_SHIFTNNOKURI:
		case IDC_CHECKBOX_SRCHALLOKURI:
		case IDC_CHECKBOX_DELCVPOSCNCL:
		case IDC_CHECKBOX_DELOKURICNCL:
		case IDC_CHECKBOX_BACKINCENTER:
		case IDC_CHECKBOX_ADDCANDKTKN:
		case IDC_CHECKBOX_ENTOGGLEKANA:
		case IDC_CHECKBOX_SETBYDIRECT:

			PropSheet_Changed(GetParent(hDlg), hDlg);

			return TRUE;

		default:
			break;
		}
		break;

	default:
		break;
	}

	return FALSE;
}

void SaveBehavior1(IXmlWriter *pWriter, HWND hDlg)
{
	SaveCheckButton(pWriter, hDlg, IDC_CHECKBOX_DEFAULTMODE, ValueDefaultMode);
	SaveCheckButton(pWriter, hDlg, IDC_RADIO_DEFMODEASCII, ValueDefModeAscii);
	SaveCheckButton(pWriter, hDlg, IDC_CHECKBOX_BEGINCVOKURI, ValueBeginCvOkuri);
	SaveCheckButton(pWriter, hDlg, IDC_CHECKBOX_PRECEDEOKURI, ValuePrecedeOkuri);
	SaveCheckButton(pWriter, hDlg, IDC_CHECKBOX_SHIFTNNOKURI, ValueShiftNNOkuri);
	SaveCheckButton(pWriter, hDlg, IDC_CHECKBOX_SRCHALLOKURI, ValueSrchAllOkuri);
	SaveCheckButton(pWriter, hDlg, IDC_CHECKBOX_DELCVPOSCNCL, ValueDelCvPosCncl);
	SaveCheckButton(pWriter, hDlg, IDC_CHECKBOX_DELOKURICNCL, ValueDelOkuriCncl);
	SaveCheckButton(pWriter, hDlg, IDC_CHECKBOX_BACKINCENTER, ValueBackIncEnter);
	SaveCheckButton(pWriter, hDlg, IDC_CHECKBOX_ADDCANDKTKN, ValueAddCandKtkn);
	SaveCheckButton(pWriter, hDlg, IDC_CHECKBOX_ENTOGGLEKANA, ValueEnToggleKana);
	SaveCheckButton(pWriter, hDlg, IDC_CHECKBOX_SETBYDIRECT, ValueSetByDirect);
}
