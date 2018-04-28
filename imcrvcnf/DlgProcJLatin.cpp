﻿
#include "configxml.h"
#include "imcrvcnf.h"
#include "convtable.h"
#include "resource.h"

ASCII_JLATIN_CONV ascii_jlatin_conv[ASCII_JLATIN_TBL_NUM];

void LoadJLatin(HWND hDlg);

INT_PTR CALLBACK DlgProcJLatin(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hWndListView;
	LV_COLUMNW lvc;
	LVITEMW item;
	int index, count;
	ASCII_JLATIN_CONV ajc;
	ASCII_JLATIN_CONV ajcBak;
	NMLISTVIEW *pListView;

	switch(message)
	{
	case WM_INITDIALOG:
		hWndListView = GetDlgItem(hDlg, IDC_LIST_JLATTBL);
		ListView_SetExtendedListViewStyle(hWndListView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvc.fmt = LVCFMT_CENTER;

		lvc.iSubItem = 0;
		lvc.cx = GetScaledSizeX(hDlg, 60);
		lvc.pszText = L"ASCII";
		ListView_InsertColumn(hWndListView, 0, &lvc);
		lvc.iSubItem = 1;
		lvc.cx = GetScaledSizeX(hDlg, 60);
		lvc.pszText = L"全英";
		ListView_InsertColumn(hWndListView, 1, &lvc);

		SetDlgItemTextW(hDlg, IDC_EDIT_JLATTBL_A, L"");
		SetDlgItemTextW(hDlg, IDC_EDIT_JLATTBL_J, L"");

		LoadJLatin(hDlg);

		return TRUE;

	case WM_DPICHANGED_AFTERPARENT:
		hWndListView = GetDlgItem(hDlg, IDC_LIST_JLATTBL);

		ListView_SetColumnWidth(hWndListView, 0, GetScaledSizeX(hDlg, 60));
		ListView_SetColumnWidth(hWndListView, 1, GetScaledSizeX(hDlg, 60));

		return TRUE;

	case WM_COMMAND:
		hWndListView = GetDlgItem(hDlg, IDC_LIST_JLATTBL);
		switch(LOWORD(wParam))
		{
		case IDC_BUTTON_JLATTBL_W:
			index = ListView_GetNextItem(hWndListView, -1, LVNI_SELECTED);
			count = ListView_GetItemCount(hWndListView);
			if(index >= 0)
			{
				PropSheet_Changed(GetParent(hDlg), hDlg);

				GetDlgItemTextW(hDlg, IDC_EDIT_JLATTBL_A, ajc.ascii, _countof(ajc.ascii));
				GetDlgItemTextW(hDlg, IDC_EDIT_JLATTBL_J, ajc.jlatin, _countof(ajc.jlatin));
				SetDlgItemTextW(hDlg, IDC_EDIT_JLATTBL_A, ajc.ascii);
				SetDlgItemTextW(hDlg, IDC_EDIT_JLATTBL_J, ajc.jlatin);
				ListView_SetItemText(hWndListView, index, 0, ajc.ascii);
				ListView_SetItemText(hWndListView, index, 1, ajc.jlatin);
			}
			else if(count < ASCII_JLATIN_TBL_NUM)
			{
				PropSheet_Changed(GetParent(hDlg), hDlg);

				GetDlgItemTextW(hDlg, IDC_EDIT_JLATTBL_A, ajc.ascii, _countof(ajc.ascii));
				GetDlgItemTextW(hDlg, IDC_EDIT_JLATTBL_J, ajc.jlatin, _countof(ajc.jlatin));
				SetDlgItemTextW(hDlg, IDC_EDIT_JLATTBL_A, ajc.ascii);
				SetDlgItemTextW(hDlg, IDC_EDIT_JLATTBL_J, ajc.jlatin);
				item.mask = LVIF_TEXT;
				item.pszText = ajc.ascii;
				item.iItem = count;
				item.iSubItem = 0;
				ListView_InsertItem(hWndListView, &item);
				item.pszText = ajc.jlatin;
				item.iItem = count;
				item.iSubItem = 1;
				ListView_SetItem(hWndListView, &item);
			}
			return TRUE;

		case IDC_BUTTON_JLATTBL_D:
			index = ListView_GetNextItem(hWndListView, -1, LVNI_SELECTED);
			if(index != -1)
			{
				PropSheet_Changed(GetParent(hDlg), hDlg);

				ListView_DeleteItem(hWndListView, index);
			}
			return TRUE;

		case IDC_BUTTON_JLATTBL_UP:
			index = ListView_GetNextItem(hWndListView, -1, LVNI_SELECTED);
			if(index > 0)
			{
				PropSheet_Changed(GetParent(hDlg), hDlg);

				ListView_GetItemText(hWndListView, index - 1, 0, ajcBak.ascii, _countof(ajcBak.ascii));
				ListView_GetItemText(hWndListView, index - 1, 1, ajcBak.jlatin, _countof(ajcBak.jlatin));
				ListView_GetItemText(hWndListView, index, 0, ajc.ascii, _countof(ajc.ascii));
				ListView_GetItemText(hWndListView, index, 1, ajc.jlatin, _countof(ajc.jlatin));
				ListView_SetItemText(hWndListView, index - 1, 0, ajc.ascii);
				ListView_SetItemText(hWndListView, index - 1, 1, ajc.jlatin);
				ListView_SetItemText(hWndListView, index, 0, ajcBak.ascii);
				ListView_SetItemText(hWndListView, index, 1, ajcBak.jlatin);
				ListView_SetItemState(hWndListView, index - 1, LVIS_FOCUSED | LVIS_SELECTED, 0x000F);
			}
			return TRUE;

		case IDC_BUTTON_JLATTBL_DOWN:
			index = ListView_GetNextItem(hWndListView, -1, LVNI_SELECTED);
			count = ListView_GetItemCount(hWndListView);
			if(index >= 0 && index < count - 1)
			{
				PropSheet_Changed(GetParent(hDlg), hDlg);

				ListView_GetItemText(hWndListView, index + 1, 0, ajcBak.ascii, _countof(ajcBak.ascii));
				ListView_GetItemText(hWndListView, index + 1, 1, ajcBak.jlatin, _countof(ajcBak.jlatin));
				ListView_GetItemText(hWndListView, index, 0, ajc.ascii, _countof(ajc.ascii));
				ListView_GetItemText(hWndListView, index, 1, ajc.jlatin, _countof(ajc.jlatin));
				ListView_SetItemText(hWndListView, index + 1, 0, ajc.ascii);
				ListView_SetItemText(hWndListView, index + 1, 1, ajc.jlatin);
				ListView_SetItemText(hWndListView, index, 0, ajcBak.ascii);
				ListView_SetItemText(hWndListView, index, 1, ajcBak.jlatin);
				ListView_SetItemState(hWndListView, index + 1, LVIS_FOCUSED | LVIS_SELECTED, 0x000F);
			}
			return TRUE;

		default:
			break;
		}
		break;

	case WM_NOTIFY:
		switch(((LPNMHDR)lParam)->code)
		{
		case LVN_ITEMCHANGED:
			pListView = (NMLISTVIEW*)((LPNMHDR)lParam);
			if(pListView->uChanged & LVIF_STATE)
			{
				hWndListView = ((LPNMHDR)lParam)->hwndFrom;
				index = ListView_GetNextItem(hWndListView, -1, LVNI_SELECTED);
				if(index == -1)
				{
					SetDlgItemTextW(hDlg, IDC_EDIT_JLATTBL_A, L"");
					SetDlgItemTextW(hDlg, IDC_EDIT_JLATTBL_J, L"");
				}
				else
				{
					ListView_GetItemText(hWndListView, index, 0, ajc.ascii, _countof(ajc.ascii));
					ListView_GetItemText(hWndListView, index, 1, ajc.jlatin, _countof(ajc.jlatin));
					SetDlgItemTextW(hDlg, IDC_EDIT_JLATTBL_A, ajc.ascii);
					SetDlgItemTextW(hDlg, IDC_EDIT_JLATTBL_J, ajc.jlatin);
				}
				return TRUE;
			}
			break;

		default:
			break;
		}
		break;
	}
	return FALSE;
}

void LoadConfigJLatin()
{
	APPDATAXMLLIST list;

	ZeroMemory(ascii_jlatin_conv, sizeof(ascii_jlatin_conv));

	HRESULT hr = ReadList(pathconfigxml, SectionJLatin, list);

	if(SUCCEEDED(hr) && list.size() != 0)
	{
		int i = 0;
		FORWARD_ITERATION_I(l_itr, list)
		{
			if(i >= ASCII_JLATIN_TBL_NUM)
			{
				break;
			}

			FORWARD_ITERATION_I(r_itr, *l_itr)
			{
				WCHAR *pszb = nullptr;
				size_t blen = 0;

				if(r_itr->first == AttributeLatin)
				{
					pszb = ascii_jlatin_conv[i].ascii;
					blen = _countof(ascii_jlatin_conv[i].ascii);
				}
				else if(r_itr->first == AttributeJLatin)
				{
					pszb = ascii_jlatin_conv[i].jlatin;
					blen = _countof(ascii_jlatin_conv[i].jlatin);
				}

				if(pszb != nullptr)
				{
					wcsncpy_s(pszb, blen, r_itr->second.c_str(), _TRUNCATE);
				}
			}

			i++;
		}
	}
	else if(FAILED(hr))
	{
		memcpy_s(ascii_jlatin_conv, sizeof(ascii_jlatin_conv),
			ascii_jlatin_conv_default, sizeof(ascii_jlatin_conv_default));
	}
}

void LoadJLatin(HWND hDlg)
{
	LVITEMW item;

	LoadConfigJLatin();

	HWND hWndListView = GetDlgItem(hDlg, IDC_LIST_JLATTBL);

	for(int i = 0; i < ASCII_JLATIN_TBL_NUM; i++)
	{
		if(ascii_jlatin_conv[i].ascii[0] == L'\0' &&
			ascii_jlatin_conv[i].jlatin[0] == L'\0')
		{
			break;
		}

		item.mask = LVIF_TEXT;
		item.pszText = ascii_jlatin_conv[i].ascii;
		item.iItem = i;
		item.iSubItem = 0;
		ListView_InsertItem(hWndListView, &item);
		item.pszText = ascii_jlatin_conv[i].jlatin;
		item.iItem = i;
		item.iSubItem = 1;
		ListView_SetItem(hWndListView, &item);
	}
}

void SaveJLatin(IXmlWriter *pWriter, HWND hDlg)
{
	APPDATAXMLLIST list;
	APPDATAXMLROW row;
	APPDATAXMLATTR attr;
	ASCII_JLATIN_CONV ajc;

	HWND hWndListView = GetDlgItem(hDlg, IDC_LIST_JLATTBL);
	int count = ListView_GetItemCount(hWndListView);

	for(int i = 0; i < count && i < ASCII_JLATIN_TBL_NUM; i++)
	{
		ListView_GetItemText(hWndListView, i, 0, ajc.ascii, _countof(ajc.ascii));
		ListView_GetItemText(hWndListView, i, 1, ajc.jlatin, _countof(ajc.jlatin));
		ascii_jlatin_conv[i] = ajc;
	}
	if(count < ASCII_JLATIN_TBL_NUM)
	{
		ascii_jlatin_conv[count].ascii[0] = L'\0';
		ascii_jlatin_conv[count].jlatin[0] = L'\0';
	}

	for(int i = 0; i < ASCII_JLATIN_TBL_NUM; i++)
	{
		if(ascii_jlatin_conv[i].ascii[0] == L'\0' &&
			ascii_jlatin_conv[i].jlatin[0] == L'\0')
		{
			break;
		}

		attr.first = AttributeLatin;
		attr.second = ascii_jlatin_conv[i].ascii;
		row.push_back(attr);

		attr.first = AttributeJLatin;
		attr.second = ascii_jlatin_conv[i].jlatin;
		row.push_back(attr);

		list.push_back(row);
		row.clear();
	}

	WriterList(pWriter, list);
}
