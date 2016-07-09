
#include "imcrvmgr.h"
#include "kw/bushu_dic.h"
#include "moji.h"

static BushuDic bushudic;
static std::map< std::wstring, std::wstring > userbushudic;

std::wstring ConvBushu(const std::wstring &bushu1, const std::wstring &bushu2)
{
	auto userdic_itr = userbushudic.find(bushu1 + bushu2);
	if(userdic_itr != userbushudic.end())
	{
		return userdic_itr->second;
	}

	userdic_itr = userbushudic.find(bushu2 + bushu1);
	if(userdic_itr != userbushudic.end())
	{
		return userdic_itr->second;
	}

	std::wstring ret;
	WCHAR r = bushudic.look(bushu1[0], bushu2[0], TC_BUSHU_ALGO_YAMANOBE);
	if(r != 0)
	{
		ret += r;
	}
	return ret;
}

//部首合成変換ユーザー辞書の行を解析して辞書に追加する。
//部首合成変換ユーザー辞書の各行の形式は、
//<漢字><部首1><部首2>[*][ ][部首1][部首2][*][ ][部首1][部首2][*]...
//*がある場合は、<部首2><部首1>の順でも<漢字>の合成可能。
static void AddBushuDicEntries(const std::wstring &s)
{
	std::wstring kanji;
	size_t idx = Copy1Moji(s, 0, &kanji);
	if(idx == 0)
	{
		return;
	}

	std::wstring nextch;
	do {
		std::wstring bushu1;
		idx = Copy1Moji(s, idx, &bushu1);
		if(idx == 0)
		{
			return;
		}
		std::wstring bushu2;
		idx = Copy1Moji(s, idx, &bushu2);
		if(idx == 0)
		{
			return;
		}
		userbushudic[bushu1 + bushu2] = kanji;

		idx = Copy1Moji(s, idx, &nextch);
		if (idx == 0)
		{
			return;
		}
		if (nextch[0] == L'*')
		{
			userbushudic[bushu2 + bushu1] = kanji; //部首が逆順でも合成可
			idx = Copy1Moji(s, idx, &nextch);
			if (idx == 0)
			{
				return;
			}
		}
	} while (nextch[0] == L' ');
}

BOOL LoadBushuConvUserDic()
{
	FILE *fp;
	_wfopen_s(&fp, pathbushudic, RccsUTF8);
	if(fp == nullptr)
	{
		return FALSE;
	}

	WCHAR wbuf[READBUFSIZE];
	std::wstring wsbuf;
	while(fgetws(wbuf, _countof(wbuf), fp) != nullptr)
	{
		wsbuf += wbuf;
		if(!wsbuf.empty() && wsbuf.back() == L'\n')
		{
			std::wstring s(wsbuf);
			std::wregex re(L"[\\x00-\\x19]");
			std::wstring fmt(L"");
			s = std::regex_replace(s, re, fmt);
			AddBushuDicEntries(s);
			wsbuf.clear();
		}
	}

	fclose(fp);
	return TRUE;
}
