
#include "imcrvmgr.h"
#include "kw/bushu_dic.h"
#include "moji.h"

static BushuDic bushudic;
static std::map< std::wstring, std::wstring > userbushudic;

std::wstring ConvBushu(const std::wstring &bushu1, const std::wstring &bushu2)
{
	auto userdic_itr = userbushudic.find(bushu1 + bushu2);
	if (userdic_itr != userbushudic.end())
	{
		return userdic_itr->second;
	}

	userdic_itr = userbushudic.find(bushu2 + bushu1);
	if (userdic_itr != userbushudic.end())
	{
		return userdic_itr->second;
	}

	std::wstring ret;
	WCHAR r = bushudic.look(bushu1[0], bushu2[0], TC_BUSHU_ALGO_YAMANOBE);
	if (r != 0)
	{
		ret += r;
	}
	return ret;
}

//逆順の部首1と部首2があるか調べる
static size_t HasReverseCompose(const std::wstring &dec, const std::wstring bushu1, const std::wstring bushu2)
{
	if (bushu2 == bushu1)
	{
		return std::wstring::npos;
	}
	size_t idx = 0;
	while ((idx = dec.find(bushu2, idx)) != std::wstring::npos)
	{
		std::wstring nextch;
		idx = ForwardMoji(dec, idx, 1); // skip bushu2
		idx = Copy1Moji(dec, idx, &nextch);
		if (nextch == bushu1)
		{
			return idx;
		}
	}
	return idx;
}

//漢字を部首合成変換で入力する方法を示すヘルプ表示用文字列を返す。
// \return "<部首1><部首2>[*][ ][部首1][部首2][*][ ][部首1][部首2][*]..."
std::wstring BushuHelp(const std::wstring &kanji)
{
	std::wstring ret;
	FORWARD_ITERATION_I(itr, userbushudic)
	{
		if (itr->second != kanji)
		{
			continue;
		}
		std::wstring bushu1;
		size_t idx = Copy1Moji(itr->first, 0, &bushu1);
		if (idx == 0)
		{
			continue;
		}
		std::wstring bushu2;
		idx = Copy1Moji(itr->first, idx, &bushu2);
		if (idx == 0)
		{
			continue;
		}

		//逆順の部首1と部首2がある場合は"*"を付けたものだけにする
		idx = HasReverseCompose(ret, bushu1, bushu2);
		if (idx == std::wstring::npos)
		{
			if (!ret.empty())
			{
				ret += L" ";
			}
			ret += bushu1;
			ret += bushu2;
		}
		else
		{
			ret.insert(idx, L"*");
		}
	}
	if (!ret.empty())
	{
		return ret;
	}

	WCHAR b1, b2;
	bushudic.decompose(kanji[0], b1, b2);
	if (b1 == MOJI_BUSHU_NL || b2 == MOJI_BUSHU_NL)
	{
		return ret;
	}
	ret += b1;
	ret += b2;
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
	if (idx == 0)
	{
		return;
	}

	std::wstring nextch;
	do {
		std::wstring bushu1;
		idx = Copy1Moji(s, idx, &bushu1);
		if (idx == 0)
		{
			return;
		}
		std::wstring bushu2;
		idx = Copy1Moji(s, idx, &bushu2);
		if (idx == 0)
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
	FILE *fp = nullptr;
	_wfopen_s(&fp, pathbushudic, modeRccsUTF8);
	if (fp == nullptr)
	{
		return FALSE;
	}

	WCHAR wbuf[READBUFSIZE];
	std::wstring wsbuf;
	while (fgetws(wbuf, _countof(wbuf), fp) != nullptr)
	{
		wsbuf += wbuf;
		if (!wsbuf.empty() && wsbuf.back() == L'\n')
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
