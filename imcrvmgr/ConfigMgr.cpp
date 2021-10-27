﻿
#include "configxml.h"
#include "utf8.h"
#include "imcrvmgr.h"

LPCWSTR TextServiceDesc = TEXTSERVICE_DESC;
LPCWSTR DictionaryManagerClass = TEXTSERVICE_NAME L"DictionaryManager";

// ファイルパス
WCHAR pathconfigxml[MAX_PATH];	//設定
WCHAR pathuserdic[MAX_PATH];	//ユーザー辞書
WCHAR pathskkdic[MAX_PATH];		//取込SKK辞書
WCHAR pathinitlua[MAX_PATH];	//init.lua
WCHAR pathbackup[MAX_PATH];		//ユーザー辞書バックアップレフィックス

WCHAR krnlobjsddl[MAX_SECURITYDESC];	//SDDL
WCHAR mgrpipename[MAX_PIPENAME];	//名前付きパイプ
WCHAR mgrmutexname[MAX_PATH];		//ミューテックス

// 辞書サーバー設定
BOOL serv = FALSE;		//SKK辞書サーバーを使用する
WCHAR host[MAX_SKKSERVER_HOST] = {};	//ホスト
WCHAR port[MAX_SKKSERVER_PORT] = {};	//ポート
DWORD encoding = 0;		//エンコーディング
DWORD timeout = 1000;	//タイムアウト

INT generation = 0;		//ユーザー辞書バックアップ世代数

BOOL precedeokuri = FALSE;	//送り仮名が一致した候補を優先する
BOOL compincback = FALSE;	//前方一致と後方一致で補完する

const luaL_Reg luaFuncs[] =
{
	{u8"search_skk_dictionary", lua_search_skk_dictionary},
	{u8"search_user_dictionary", lua_search_user_dictionary},
	{u8"search_skk_server", lua_search_skk_server},
	{u8"search_skk_server_info", lua_search_skk_server_info},
	{u8"search_unicode", lua_search_unicode},
	{u8"search_jisx0213", lua_search_jisx0213},
	{u8"search_jisx0208", lua_search_jisx0208},
	{u8"search_character_code", lua_search_character_code},
	{u8"complement", lua_complement},
	{u8"add", lua_add},
	{u8"delete", lua_delete},
	{u8"save", lua_save},
	{nullptr, nullptr}
};

void CreateConfigPath()
{
	PWSTR knownfolderpath = nullptr;

	ZeroMemory(pathconfigxml, sizeof(pathconfigxml));
	ZeroMemory(pathuserdic, sizeof(pathuserdic));
	ZeroMemory(pathskkdic, sizeof(pathskkdic));
	ZeroMemory(pathinitlua, sizeof(pathinitlua));

	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DONT_VERIFY, nullptr, &knownfolderpath)))
	{
		WCHAR appdir[MAX_PATH];

		_snwprintf_s(appdir, _TRUNCATE, L"%s\\%s", knownfolderpath, TextServiceDesc);

		CoTaskMemFree(knownfolderpath);

		CreateDirectoryW(appdir, nullptr);
		SetCurrentDirectoryW(appdir);

		_snwprintf_s(pathconfigxml, _TRUNCATE, L"%s\\%s", appdir, fnconfigxml);
		_snwprintf_s(pathuserdic, _TRUNCATE, L"%s\\%s", appdir, fnuserdic);
		_snwprintf_s(pathskkdic, _TRUNCATE, L"%s\\%s", appdir, fnskkdic);
		_snwprintf_s(pathinitlua, _TRUNCATE, L"%s\\%s", appdir, fninitlua);

		//for compatibility
		if (GetFileAttributesW(pathskkdic) == INVALID_FILE_ATTRIBUTES)
		{
			WCHAR skkdict[MAX_PATH];
			_snwprintf_s(skkdict, _TRUNCATE, L"%s\\%s", appdir, L"skkdict.dic");
			MoveFileW(skkdict, pathskkdic);
			_snwprintf_s(skkdict, _TRUNCATE, L"%s\\%s", appdir, L"skkdict.idx");
			DeleteFileW(skkdict);
		}
	}
}

void UpdateConfigPath()
{
	PWSTR knownfolderpath = nullptr;

	//%APPDATA%\\CorvusSKK\\config.xml
	//%APPDATA%\\CorvusSKK\\skkdict.txt
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_DONT_VERIFY, nullptr, &knownfolderpath)))
	{
		_snwprintf_s(pathconfigxml, _TRUNCATE, L"%s\\%s\\%s", knownfolderpath, TextServiceDesc, fnconfigxml);
		_snwprintf_s(pathskkdic, _TRUNCATE, L"%s\\%s\\%s", knownfolderpath, TextServiceDesc, fnskkdic);

		CoTaskMemFree(knownfolderpath);
	}

	if (GetFileAttributesW(pathconfigxml) == INVALID_FILE_ATTRIBUTES)
	{
#ifdef _DEBUG
		//<module directory>\\config.xml
		if (GetModuleFileNameW(hInst, pathconfigxml, _countof(pathconfigxml)) != 0)
		{
			WCHAR *pdir = wcsrchr(pathconfigxml, L'\\');
			if (pdir != nullptr)
			{
				*(pdir + 1) = L'\0';
				wcsncat_s(pathconfigxml, fnconfigxml, _TRUNCATE);
			}
		}
#else
		//%SystemRoot%\\IME\\IMCRVSKK\\config.xml
		if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Windows, KF_FLAG_DONT_VERIFY, nullptr, &knownfolderpath)))
		{
			_snwprintf_s(pathconfigxml, _TRUNCATE, L"%s\\%s\\%s\\%s", knownfolderpath, SYSTEMROOT_IME_DIR, TEXTSERVICE_DIR, fnconfigxml);

			CoTaskMemFree(knownfolderpath);
		}
#endif
	}

	if (GetFileAttributesW(pathskkdic) == INVALID_FILE_ATTRIBUTES)
	{
#ifdef _DEBUG
		//<module directory>\\skkdict.txt
		if (GetModuleFileNameW(hInst, pathskkdic, _countof(pathskkdic)) != 0)
		{
			WCHAR *pdir = wcsrchr(pathskkdic, L'\\');
			if (pdir != nullptr)
			{
				*(pdir + 1) = L'\0';
				wcsncat_s(pathskkdic, fnskkdic, _TRUNCATE);
			}
		}
#else
		//%SystemRoot%\\IME\\IMCRVSKK\\skkdict.txt
		if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Windows, KF_FLAG_DONT_VERIFY, nullptr, &knownfolderpath)))
		{
			_snwprintf_s(pathskkdic, _TRUNCATE, L"%s\\%s\\%s\\%s", knownfolderpath, SYSTEMROOT_IME_DIR, TEXTSERVICE_DIR, fnskkdic);

			CoTaskMemFree(knownfolderpath);
		}
#endif
	}
}

void CreateIpcName()
{
	ZeroMemory(krnlobjsddl, sizeof(krnlobjsddl));
	ZeroMemory(mgrpipename, sizeof(mgrpipename));
	ZeroMemory(mgrmutexname, sizeof(mgrmutexname));

	LPWSTR pszUserSid = nullptr;

	if (GetUserSid(&pszUserSid))
	{
		// SDDL_ALL_APP_PACKAGES / SDDL_RESTRICTED_CODE / SDDL_LOCAL_SYSTEM / SDDL_BUILTIN_ADMINISTRATORS / User SID
		_snwprintf_s(krnlobjsddl, _TRUNCATE, L"D:%s(A;;GA;;;RC)(A;;GA;;;SY)(A;;GA;;;BA)(A;;GA;;;%s)",
			(IsWindowsVersion62OrLater() ? L"(A;;GA;;;AC)" : L""), pszUserSid);

		// (SDDL_MANDATORY_LABEL, SDDL_NO_WRITE_UP, SDDL_ML_LOW)
		wcsncat_s(krnlobjsddl, L"S:(ML;;NW;;;LW)", _TRUNCATE);

		LocalFree(pszUserSid);
	}

	LPWSTR pszUserUUID = nullptr;

	if (GetUserUUID(&pszUserUUID))
	{
		_snwprintf_s(mgrpipename, _TRUNCATE, L"%s%s", IMCRVMGRPIPE, pszUserUUID);
		_snwprintf_s(mgrmutexname, _TRUNCATE, L"%s%s", IMCRVMGRMUTEX, pszUserUUID);

		LocalFree(pszUserUUID);
	}
}

void LoadConfig()
{
	WCHAR path[MAX_PATH];
	BOOL servtmp;
	WCHAR hosttmp[MAX_SKKSERVER_HOST];	//ホスト
	WCHAR porttmp[MAX_SKKSERVER_PORT];	//ポート
	DWORD encodingtmp;
	DWORD timeouttmp;
	std::wstring strxmlval;

	ReadValue(pathconfigxml, SectionServer, ValueServerServ, strxmlval);
	servtmp = _wtoi(strxmlval.c_str());
	if (servtmp != TRUE && servtmp != FALSE)
	{
		servtmp = FALSE;
	}

	ReadValue(pathconfigxml, SectionServer, ValueServerHost, strxmlval);
	wcsncpy_s(hosttmp, strxmlval.c_str(), _TRUNCATE);

	ReadValue(pathconfigxml, SectionServer, ValueServerPort, strxmlval);
	wcsncpy_s(porttmp, strxmlval.c_str(), _TRUNCATE);

	ReadValue(pathconfigxml, SectionServer, ValueServerEncoding, strxmlval);
	encodingtmp = _wtoi(strxmlval.c_str());
	if (encodingtmp != 1)
	{
		encodingtmp = 0;
	}

	ReadValue(pathconfigxml, SectionServer, ValueServerTimeOut, strxmlval);
	timeouttmp = _wtoi(strxmlval.c_str());
	if (timeouttmp > 60000)
	{
		timeouttmp = 1000;
	}

	//変更があったら接続し直す
	if (servtmp != serv || wcscmp(hosttmp, host) != 0 || wcscmp(porttmp, port) != 0 ||
		encodingtmp != encoding || timeouttmp != timeout)
	{
		serv = servtmp;
		wcsncpy_s(host, hosttmp, _TRUNCATE);
		wcsncpy_s(port, porttmp, _TRUNCATE);
		encoding = encodingtmp;
		timeout = timeouttmp;

		DisconnectSKKServer();

		if (serv)
		{
			StartConnectSKKServer();
		}
	}

	ReadValue(pathconfigxml, SectionDictionary, ValueDictionaryBackupDir, strxmlval);
	if (strxmlval.empty())
	{
		strxmlval = L"%APPDATA%\\" TEXTSERVICE_DESC;
	}
	ExpandEnvironmentStringsW(strxmlval.c_str(), path, _countof(path));
	_snwprintf_s(pathbackup, _TRUNCATE, L"%s\\%s", path, fnskkdic);

	ReadValue(pathconfigxml, SectionDictionary, ValueDictionaryBackupGen, strxmlval);
	INT g = strxmlval.empty() ? -1 : _wtoi(strxmlval.c_str());
	if (g < 0)
	{
		g = DEF_BACKUPGENS;
	}
	else if (g > MAX_BACKUPGENS)
	{
		g = MAX_BACKUPGENS;
	}
	generation = g;

	ReadValue(pathconfigxml, SectionBehavior, ValuePrecedeOkuri, strxmlval);
	precedeokuri = _wtoi(strxmlval.c_str());
	if (precedeokuri != TRUE && precedeokuri != FALSE)
	{
		precedeokuri = FALSE;
	}

	ReadValue(pathconfigxml, SectionBehavior, ValueCompIncBack, strxmlval);
	compincback = _wtoi(strxmlval.c_str());
	if (compincback != TRUE && compincback != FALSE)
	{
		compincback = FALSE;
	}
}

BOOL IsFileModified(LPCWSTR path, FILETIME *ft)
{
	BOOL ret = FALSE;
	HANDLE hFile;
	FILETIME ftn;

	if (path != nullptr && ft != nullptr)
	{
		hFile = CreateFileW(path, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			if (GetFileTime(hFile, nullptr, nullptr, &ftn))
			{
				if (((ULARGE_INTEGER *)ft)->QuadPart != ((ULARGE_INTEGER *)&ftn)->QuadPart)
				{
					*ft = ftn;
					ret = TRUE;
				}
			}
			CloseHandle(hFile);
		}
	}

	return ret;
}

void InitLua()
{
	CHAR version[64];

	lua = luaL_newstate();
	if (lua == nullptr)
	{
		return;
	}

	luaL_openlibs(lua);

	luaL_newlib(lua, luaFuncs);
	lua_setglobal(lua, u8"crvmgr");

	//skk-version
	_snprintf_s(version, _TRUNCATE, "%s", WCTOU8(TEXTSERVICE_NAME L" " TEXTSERVICE_VER));
	lua_pushstring(lua, version);
	lua_setglobal(lua, u8"SKK_VERSION");

	//%APPDATA%\\CorvusSKK\\init.lua
	if (luaL_dofile(lua, WCTOU8(pathinitlua)) == LUA_OK)
	{
		return;
	}

	ZeroMemory(pathinitlua, sizeof(pathinitlua));

#ifdef _DEBUG
	//<module directory>\\init.lua
	if (GetModuleFileNameW(nullptr, pathinitlua, _countof(pathinitlua)) != 0)
	{
		WCHAR *pdir = wcsrchr(pathinitlua, L'\\');
		if (pdir != nullptr)
		{
			*(pdir + 1) = L'\0';
			wcsncat_s(pathinitlua, fninitlua, _TRUNCATE);
		}
	}
#else
	PWSTR knownfolderpath = nullptr;

	//%SystemRoot%\\IME\\IMCRVSKK\\init.lua
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Windows, KF_FLAG_DONT_VERIFY, nullptr, &knownfolderpath)))
	{
		_snwprintf_s(pathinitlua, _TRUNCATE, L"%s\\%s\\%s\\%s", knownfolderpath, SYSTEMROOT_IME_DIR, TEXTSERVICE_DIR, fninitlua);

		CoTaskMemFree(knownfolderpath);
	}
#endif

	if (luaL_dofile(lua, WCTOU8(pathinitlua)) == LUA_OK)
	{
		return;
	}

	UninitLua();
}

void UninitLua()
{
	if (lua != nullptr)
	{
		lua_close(lua);
		lua = nullptr;
	}
}
