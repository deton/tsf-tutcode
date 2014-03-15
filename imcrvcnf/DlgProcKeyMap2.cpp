
#include "configxml.h"
#include "imcrvcnf.h"
#include "resource.h"

static const struct {
	LPCWSTR label;
	LPCWSTR name;
} VKeyMap[] =
{
	{L"かな／カナ",	ValueKeyMapKana},
	{L"ｶﾅ全英変換",	ValueKeyMapConvChar},
	{L"全英",		ValueKeyMapJLatin},
	{L"ASCII",		ValueKeyMapAscii},
	{L"ひらがな",	ValueKeyMapJMode},
	{L"abbrev",		ValueKeyMapAbbrev},
	{L"接辞",		ValueKeyMapAffix},
	{L"次候補",		ValueKeyMapNextCand},
	{L"前候補",		ValueKeyMapPrevCand},
	{L"辞書削除",	ValueKeyMapPurgeDic},
	{L"次補完",		ValueKeyMapNextComp},
	{L"前補完",		ValueKeyMapPrevComp},
	{L"絞り込み",	ValueKeyMapHint},
	{L"変換位置",	ValueKeyMapConvPoint},
	{L"直接入力",	ValueKeyMapDirect},
	{L"確定",		ValueKeyMapEnter},
	{L"取消",		ValueKeyMapCancel},
	{L"後退",		ValueKeyMapBack},
	{L"削除",		ValueKeyMapDelete},
	{L"*無効*",		ValueKeyMapVoid},
	{L"左移動",		ValueKeyMapLeft},
	{L"先頭移動",	ValueKeyMapUp},
	{L"右移動",		ValueKeyMapRight},
	{L"末尾移動",	ValueKeyMapDown},
	{L"貼付",		ValueKeyMapPaste},
	{L"他IME切替",	ValueKeyMapOtherIme},
	{L"Vi Esc",		ValueKeyMapViEsc},
	{NULL,	NULL},
};

static void InitListView(HWND hDlg, HWND hWndListView)
{
	LV_COLUMNW lvc;
	ListView_SetExtendedListViewStyle(hWndListView, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_CENTER;

	lvc.iSubItem = 0;
	lvc.cx = GetScaledSizeX(hDlg, 60);
	lvc.pszText = L"仮想ｷｰ";
	ListView_InsertColumn(hWndListView, 0, &lvc);
	lvc.iSubItem = 1;
	lvc.cx = GetScaledSizeX(hDlg, 60);
	lvc.pszText = L"ALT";
	ListView_InsertColumn(hWndListView, 1, &lvc);
	lvc.iSubItem = 2;
	lvc.cx = GetScaledSizeX(hDlg, 60);
	lvc.pszText = L"CTRL";
	ListView_InsertColumn(hWndListView, 2, &lvc);
	lvc.iSubItem = 3;
	lvc.cx = GetScaledSizeX(hDlg, 60);
	lvc.pszText = L"SHIFT";
	ListView_InsertColumn(hWndListView, 3, &lvc);
	lvc.iSubItem = 4;
	lvc.cx = GetScaledSizeX(hDlg, 80);
	lvc.pszText = L"機能";
	ListView_InsertColumn(hWndListView, 4, &lvc);
}

static int FindVKeyMap(LPCWSTR name)
{
	for(int i = 0; VKeyMap[i].label != NULL; i++)
	{
		if(wcscmp(VKeyMap[i].name, name) == 0)
		{
			return i;
		}
	}
	return -1;
}

static void AddVKeyMapRow(HWND hWndList, int index, BYTE vk, BYTE modifiers, LPCWSTR funcname)
{
	LVITEMW item;
	WCHAR num[8];

	int skkfunc = FindVKeyMap(funcname);
	if(skkfunc == -1)
	{
		return;
	}
	LPCWSTR label = VKeyMap[skkfunc].label;

	item.mask = LVIF_TEXT;
	_snwprintf_s(num, _TRUNCATE, L"0x%02X", vk);
	item.pszText = num;
	item.iItem = index;
	item.iSubItem = 0;
	ListView_InsertItem(hWndList, &item);
	_snwprintf_s(num, _TRUNCATE, L"%d", modifiers & TF_MOD_ALT ? 1 : 0);
	item.pszText = num;
	item.iItem = index;
	item.iSubItem = 1;
	ListView_SetItem(hWndList, &item);
	_snwprintf_s(num, _TRUNCATE, L"%d", modifiers & TF_MOD_CONTROL ? 1 : 0);
	item.pszText = num;
	item.iItem = index;
	item.iSubItem = 2;
	ListView_SetItem(hWndList, &item);
	_snwprintf_s(num, _TRUNCATE, L"%d", modifiers & TF_MOD_SHIFT ? 1 : 0);
	item.pszText = num;
	item.iItem = index;
	item.iSubItem = 3;
	ListView_SetItem(hWndList, &item);
	_snwprintf_s(num, _TRUNCATE, L"%s", label);
	item.pszText = num;
	item.iItem = index;
	item.iSubItem = 4;
	ListView_SetItem(hWndList, &item);
}

static void LoadVKeyMap(HWND hwnd)
{
	APPDATAXMLLIST list;
	APPDATAXMLLIST::iterator l_itr;
	APPDATAXMLROW::iterator r_itr;
	HRESULT hr;
	int i = 0;
	HWND hWndList = GetDlgItem(hwnd, IDC_LIST_VKEYMAP);

	hr = ReadList(pathconfigxml, SectionVKeyMap, list);
	if(hr == S_OK && list.size() != 0)
	{
		for(l_itr = list.begin(); l_itr != list.end(); l_itr++)
		{
			BYTE vk = 0;
			BYTE m = 0;
			LPCWSTR funcname;
			for(r_itr = l_itr->begin(); r_itr != l_itr->end(); r_itr++)
			{
				if(r_itr->first == AttributeVKey)
				{
					vk = wcstoul(r_itr->second.c_str(), NULL, 0);
				}
				else if(r_itr->first == AttributeMKey)
				{
					m = wcstoul(r_itr->second.c_str(), NULL, 0) & (TF_MOD_ALT | TF_MOD_CONTROL | TF_MOD_SHIFT);
				}
				else if(r_itr->first == AttributeName)
				{
					funcname = r_itr->second.c_str();
				}
			}

			if(vk != 0)
			{
				AddVKeyMapRow(hWndList, i, vk, m, funcname);
				i++;
			}
		}
	}
	else
	{
		AddVKeyMapRow(hWndList, i++, VK_DELETE, 0, ValueKeyMapDelete);
		AddVKeyMapRow(hWndList, i++, VK_LEFT, 0, ValueKeyMapLeft);
		AddVKeyMapRow(hWndList, i++, VK_UP, 0, ValueKeyMapUp);
		AddVKeyMapRow(hWndList, i++, VK_RIGHT, 0, ValueKeyMapRight);
		AddVKeyMapRow(hWndList, i++, VK_DOWN, 0, ValueKeyMapDown);
	}
}

static void SaveVKeyMap(HWND hwnd)
{
	HWND hWndListView = GetDlgItem(hwnd, IDC_LIST_VKEYMAP);
	int i, count;
	WCHAR key[8];
	APPDATAXMLATTR attr;
	APPDATAXMLROW row;
	APPDATAXMLLIST list;

	WriterStartSection(pXmlWriter, SectionVKeyMap);	//Start of SectionVKeyMap

	count = ListView_GetItemCount(hWndListView);
	for(i = 0; i < count; i++)
	{
		BYTE vk;
		BYTE m = 0;
		ListView_GetItemText(hWndListView, i, 0, key, _countof(key));
		vk = wcstoul(key, NULL, 0);
		ListView_GetItemText(hWndListView, i, 1, key, _countof(key));
		if(key[0] == L'1')
		{
			m |= TF_MOD_ALT;
		}
		ListView_GetItemText(hWndListView, i, 2, key, _countof(key));
		if(key[0] == L'1')
		{
			m |= TF_MOD_CONTROL;
		}
		ListView_GetItemText(hWndListView, i, 3, key, _countof(key));
		if(key[0] == L'1')
		{
			m |= TF_MOD_SHIFT;
		}

		ListView_GetItemText(hWndListView, i, 4, key, _countof(key));
		int sel = 0;
		int j;
		for(j = 0; VKeyMap[j].label != NULL; j++)
		{
			if(wcsncmp(VKeyMap[j].label, key, _countof(key)) == 0)
			{
				sel = j;
				break;
			}
		}
		if(VKeyMap[j].label == NULL)
		{
			sel = 0;
		}

		attr.first = AttributeVKey;
		_snwprintf_s(key, _TRUNCATE, L"0x%02X", vk);
		attr.second = key;
		row.push_back(attr);

		attr.first = AttributeMKey;
		_snwprintf_s(key, _TRUNCATE, L"%X", m);
		attr.second = key;
		row.push_back(attr);

		attr.first = AttributeName;
		attr.second = VKeyMap[sel].name;
		row.push_back(attr);

		list.push_back(row);
		row.clear();
	}

	WriterList(pXmlWriter, list);

	WriterEndSection(pXmlWriter);	//End of SectionVKeyMap
}

INT_PTR CALLBACK DlgProcKeyMap2(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hWndListView;
	HWND hwnd;
	LVITEMW item;
	int index, count;
	size_t i;
	WCHAR key[8];
	WCHAR keyBak[8];
	NMLISTVIEW *pListView;

	switch(message)
	{
	case WM_INITDIALOG:
		hWndListView = GetDlgItem(hDlg, IDC_LIST_VKEYMAP);
		InitListView(hDlg, hWndListView);

		SetDlgItemText(hDlg, IDC_EDIT_VKEYMAP_VKEY, L"");
		CheckDlgButton(hDlg, IDC_CHECKBOX_VKEYMAP_MKEY_ALT, BST_UNCHECKED);
		CheckDlgButton(hDlg, IDC_CHECKBOX_VKEYMAP_MKEY_CTRL, BST_UNCHECKED);
		CheckDlgButton(hDlg, IDC_CHECKBOX_VKEYMAP_MKEY_SHIFT, BST_UNCHECKED);

		hwnd = GetDlgItem(hDlg, IDC_COMBO_SKKFUNCLIST);
		for(i = 0; VKeyMap[i].label != NULL; i++)
		{
			SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)VKeyMap[i].label);
		}
		SendMessage(hwnd, CB_SETCURSEL, 0, 0);

		LoadVKeyMap(hDlg);

		return TRUE;

	case WM_COMMAND:
		hWndListView = GetDlgItem(hDlg, IDC_LIST_VKEYMAP);
		switch(LOWORD(wParam))
		{
		case IDC_BUTTON_VKEYMAP_W:
			index = ListView_GetNextItem(hWndListView, -1, LVNI_SELECTED);
			count = ListView_GetItemCount(hWndListView);
			if(index >= 0)
			{
				PropSheet_Changed(GetParent(hDlg), hDlg);

				GetDlgItemText(hDlg, IDC_EDIT_VKEYMAP_VKEY, key, _countof(key));
				_snwprintf_s(key, _TRUNCATE, L"0x%02X", wcstoul(key, NULL, 0));
				SetDlgItemText(hDlg, IDC_EDIT_VKEYMAP_VKEY, key);
				ListView_SetItemText(hWndListView, index, 0, key);
				ListView_SetItemText(hWndListView, index, 1,
					IsDlgButtonChecked(hDlg, IDC_CHECKBOX_VKEYMAP_MKEY_ALT) == BST_CHECKED ? L"1" : L"0");
				ListView_SetItemText(hWndListView, index, 2,
					IsDlgButtonChecked(hDlg, IDC_CHECKBOX_VKEYMAP_MKEY_CTRL) == BST_CHECKED ? L"1" : L"0");
				ListView_SetItemText(hWndListView, index, 3,
					IsDlgButtonChecked(hDlg, IDC_CHECKBOX_VKEYMAP_MKEY_SHIFT) == BST_CHECKED ? L"1" : L"0");

				hwnd = GetDlgItem(hDlg, IDC_COMBO_SKKFUNCLIST);
				int sel = SendMessage(hwnd, CB_GETCURSEL, 0, 0);
				_snwprintf_s(key, _TRUNCATE, L"%s", VKeyMap[sel].label);
				ListView_SetItemText(hWndListView, index, 4, key);
			}
			else
			{
				PropSheet_Changed(GetParent(hDlg), hDlg);

				item.mask = LVIF_TEXT;
				GetDlgItemText(hDlg, IDC_EDIT_VKEYMAP_VKEY, key, _countof(key));
				_snwprintf_s(key, _TRUNCATE, L"0x%02X", wcstoul(key, NULL, 0));
				SetDlgItemText(hDlg, IDC_EDIT_VKEYMAP_VKEY, key);
				item.pszText = key;
				item.iItem = count;
				item.iSubItem = 0;
				ListView_InsertItem(hWndListView, &item);
				item.pszText = IsDlgButtonChecked(hDlg, IDC_CHECKBOX_VKEYMAP_MKEY_ALT) == BST_CHECKED ? L"1" : L"0";
				item.iItem = count;
				item.iSubItem = 1;
				ListView_SetItem(hWndListView, &item);
				item.pszText = IsDlgButtonChecked(hDlg, IDC_CHECKBOX_VKEYMAP_MKEY_CTRL) == BST_CHECKED ? L"1" : L"0";
				item.iItem = count;
				item.iSubItem = 2;
				ListView_SetItem(hWndListView, &item);
				item.pszText = IsDlgButtonChecked(hDlg, IDC_CHECKBOX_VKEYMAP_MKEY_SHIFT) == BST_CHECKED ? L"1" : L"0";
				item.iItem = count;
				item.iSubItem = 3;
				ListView_SetItem(hWndListView, &item);

				hwnd = GetDlgItem(hDlg, IDC_COMBO_SKKFUNCLIST);
				int sel = SendMessage(hwnd, CB_GETCURSEL, 0, 0);
				_snwprintf_s(key, _TRUNCATE, L"%s", VKeyMap[sel].label);
				item.pszText = key;
				item.iItem = count;
				item.iSubItem = 4;
				ListView_SetItem(hWndListView, &item);
			}
			return TRUE;

		case IDC_BUTTON_VKEYMAP_D:
			index = ListView_GetNextItem(hWndListView, -1, LVNI_SELECTED);
			if(index != -1)
			{
				PropSheet_Changed(GetParent(hDlg), hDlg);

				ListView_DeleteItem(hWndListView, index);
			}
			return TRUE;

		case IDC_BUTTON_VKEYMAP_UP:
			index = ListView_GetNextItem(hWndListView, -1, LVNI_SELECTED);
			if(index > 0)
			{
				PropSheet_Changed(GetParent(hDlg), hDlg);

				ListView_GetItemText(hWndListView, index - 1, 0, keyBak, _countof(keyBak));
				ListView_GetItemText(hWndListView, index, 0, key, _countof(key));
				ListView_SetItemText(hWndListView, index - 1, 0, key);
				ListView_SetItemText(hWndListView, index, 0, keyBak);

				ListView_GetItemText(hWndListView, index - 1, 1, keyBak, _countof(keyBak));
				ListView_GetItemText(hWndListView, index, 1, key, _countof(key));
				ListView_SetItemText(hWndListView, index - 1, 1, key);
				ListView_SetItemText(hWndListView, index, 1, keyBak);

				ListView_GetItemText(hWndListView, index - 1, 2, keyBak, _countof(keyBak));
				ListView_GetItemText(hWndListView, index, 2, key, _countof(key));
				ListView_SetItemText(hWndListView, index - 1, 2, key);
				ListView_SetItemText(hWndListView, index, 2, keyBak);

				ListView_GetItemText(hWndListView, index - 1, 3, keyBak, _countof(keyBak));
				ListView_GetItemText(hWndListView, index, 3, key, _countof(key));
				ListView_SetItemText(hWndListView, index - 1, 3, key);
				ListView_SetItemText(hWndListView, index, 3, keyBak);
				ListView_GetItemText(hWndListView, index - 1, 4, keyBak, _countof(keyBak));
				ListView_GetItemText(hWndListView, index, 4, key, _countof(key));
				ListView_SetItemText(hWndListView, index - 1, 4, key);
				ListView_SetItemText(hWndListView, index, 4, keyBak);

				ListView_SetItemState(hWndListView, index - 1, LVIS_FOCUSED | LVIS_SELECTED, 0x000F);
			}
			return TRUE;

		case IDC_BUTTON_VKEYMAP_DOWN:
			index = ListView_GetNextItem(hWndListView, -1, LVNI_SELECTED);
			count = ListView_GetItemCount(hWndListView);
			if(index >= 0 && index < count - 1)
			{
				PropSheet_Changed(GetParent(hDlg), hDlg);

				ListView_GetItemText(hWndListView, index + 1, 0, keyBak, _countof(keyBak));
				ListView_GetItemText(hWndListView, index, 0, key, _countof(key));
				ListView_SetItemText(hWndListView, index + 1, 0, key);
				ListView_SetItemText(hWndListView, index, 0, keyBak);

				ListView_GetItemText(hWndListView, index + 1, 1, keyBak, _countof(keyBak));
				ListView_GetItemText(hWndListView, index, 1, key, _countof(key));
				ListView_SetItemText(hWndListView, index + 1, 1, key);
				ListView_SetItemText(hWndListView, index, 1, keyBak);

				ListView_GetItemText(hWndListView, index + 1, 2, keyBak, _countof(keyBak));
				ListView_GetItemText(hWndListView, index, 2, key, _countof(key));
				ListView_SetItemText(hWndListView, index + 1, 2, key);
				ListView_SetItemText(hWndListView, index, 2, keyBak);

				ListView_GetItemText(hWndListView, index + 1, 3, keyBak, _countof(keyBak));
				ListView_GetItemText(hWndListView, index, 3, key, _countof(key));
				ListView_SetItemText(hWndListView, index + 1, 3, key);
				ListView_SetItemText(hWndListView, index, 3, keyBak);
				ListView_GetItemText(hWndListView, index + 1, 4, keyBak, _countof(keyBak));
				ListView_GetItemText(hWndListView, index, 4, key, _countof(key));
				ListView_SetItemText(hWndListView, index + 1, 4, key);
				ListView_SetItemText(hWndListView, index, 4, keyBak);

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
				hwnd = GetDlgItem(hDlg, IDC_COMBO_SKKFUNCLIST);
				if(index == -1)
				{
					SetDlgItemText(hDlg, IDC_EDIT_VKEYMAP_VKEY, L"");
					CheckDlgButton(hDlg, IDC_CHECKBOX_VKEYMAP_MKEY_ALT, BST_UNCHECKED);
					CheckDlgButton(hDlg, IDC_CHECKBOX_VKEYMAP_MKEY_CTRL, BST_UNCHECKED);
					CheckDlgButton(hDlg, IDC_CHECKBOX_VKEYMAP_MKEY_SHIFT, BST_UNCHECKED);
					SendMessage(hwnd, CB_SETCURSEL, 0, 0);
				}
				else
				{
					ListView_GetItemText(hWndListView, index, 0, key, _countof(key));
					SetDlgItemText(hDlg, IDC_EDIT_VKEYMAP_VKEY, key);
					ListView_GetItemText(hWndListView, index, 1, key, _countof(key));
					CheckDlgButton(hDlg, IDC_CHECKBOX_VKEYMAP_MKEY_ALT, key[0] == L'1' ? BST_CHECKED : BST_UNCHECKED);
					ListView_GetItemText(hWndListView, index, 2, key, _countof(key));
					CheckDlgButton(hDlg, IDC_CHECKBOX_VKEYMAP_MKEY_CTRL, key[0] == L'1' ? BST_CHECKED : BST_UNCHECKED);
					ListView_GetItemText(hWndListView, index, 3, key, _countof(key));
					CheckDlgButton(hDlg, IDC_CHECKBOX_VKEYMAP_MKEY_SHIFT, key[0] == L'1' ? BST_CHECKED : BST_UNCHECKED);					

					ListView_GetItemText(hWndListView, index, 4, key, _countof(key));
					for(i = 0; VKeyMap[i].label != NULL; i++)
					{
						if(wcsncmp(VKeyMap[i].label, key, _countof(key)) == 0)
						{
							SendMessage(hwnd, CB_SETCURSEL, i, 0);
							break;
						}
					}
					if(VKeyMap[i].label == NULL)
					{
						SendMessage(hwnd, CB_SETCURSEL, 0, 0);
					}
				}
				return TRUE;
			}
			break;

		case PSN_APPLY:
			SaveVKeyMap(hDlg);
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
