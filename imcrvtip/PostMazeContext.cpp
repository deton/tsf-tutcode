
/* MIT License
Copyright (c) 2016 KIHARA, Hideto

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "PostMazeContext.h"
#include "moji.h"

const int MAX_SUFFIX = 4; //活用語尾の最大文字数 TODO:imtutcnfで設定可能にする
/**
 * 活用する語の読みに付加する文字。
 * 交ぜ書き変換辞書の読みには、活用する語はこの文字付きで登録されている。
 */
const std::wstring STR_INFLECTION_MARK = L"―";
const wchar_t     CHAR_INFLECTION_MARK = L'―';

CPostMazeContext::CPostMazeContext()
{
	Deactivate();
}

CPostMazeContext::~CPostMazeContext()
{
}

/**
 * 後置型交ぜ書き変換状態を終了
 */
void CPostMazeContext::Deactivate()
{
	postyomi.clear();
	postyomist = 0;
	postyomied = 0;
	postyomiResizing = PYR_NO;
	resizeWithInflection = false;
	isInflection = false;
}

/**
 * 後置型交ぜ書き変換状態を有効化
 * @param yomi 読み
 * @param _isInflection 活用する語として変換を開始するか
 * @param startResizing yomiが変換できなかった場合、縮めながらの変換を試みるか
 * @param _resizeWithInflection 活用しない語を縮めた時に、活用する語としての変換を試みるか
 */
void CPostMazeContext::Activate(const std::wstring& yomi, bool _isInflection, bool startResizing, bool _resizeWithInflection)
{
	postyomi.assign(yomi);
	postyomist = 0;
	postyomied = postyomi.size();
	isInflection = _isInflection;
	if(startResizing) 
	{
		postyomiResizing = PYR_SHRINKING;
	}
	resizeWithInflection = _resizeWithInflection;
}

/**
 * 後置型交ぜ書き変換中かどうか
 */
bool CPostMazeContext::IsActive()
{
	return !postyomi.empty();
}

/**
 * 読みの縮め/伸ばし状態を終了させる
 */
void CPostMazeContext::EndResizing()
{
	postyomiResizing = PYR_NO;
}

/**
 * 変換対象読みが活用する語かどうかを返す
 * @return true:活用する語, false:活用しない語
 */
bool CPostMazeContext::IsYomiInflection()
{
	return (IsActive() && isInflection);
}

/**
 * 読みを取得する
 * @param withInflection 活用する語の場合、活用する語を示す「―」を付けるかどうか
 * @param [out] yomi 読み
 * @return true:読みがあった(=IsActive())場合。false:読みが無い場合
 */
bool CPostMazeContext::GetYomi(bool withInflection, std::wstring *yomi)
{
	yomi->clear();
	if(IsActive())
	{
		yomi->assign(postyomi);
		if(withInflection && isInflection)
		{
			yomi->append(STR_INFLECTION_MARK);
		}
		return true;
	}
	return false;
}

/**
 * 活用する語の語尾を取得する
 * @param [out] gobi 語尾
 * @return true:語尾があった場合。false:語尾が無い場合
 */
bool CPostMazeContext::GetGobi(std::wstring *gobi)
{
	gobi->clear();
	if(IsYomiInflection() && postyomied < postyomi.size())
	{
		gobi->assign(postyomi, postyomied, postyomi.size() - postyomied);
		return true;
	}
	return false;
}

/**
 * (読みの縮め/伸ばしにより)変換対象の読みから外された形になっている部分を取得する
 * @param [out] excluded 読みから外されている部分
 * @return true:外されている部分がある場合。false:外されている部分が無い場合
 */
bool CPostMazeContext::GetExcluded(std::wstring *excluded)
{
	excluded->clear();
	if(postyomist > 0)
	{
		excluded->assign(postyomi, 0, postyomist);
		return true;
	}
	return false;
}

/**
 * 変換対象の読みから外された形になっている部分を消去する
 */
void CPostMazeContext::EraseExcluded()
{
	postyomi.erase(0, postyomist);
	postyomist = 0;
	postyomied = postyomi.size();
}

/**
 * 読みを縮め/伸ばしながらの変換中の場合には、読みを縮め/伸ばす
 * @param [out] yomi 縮め/伸ばした後の、変換対象となる読み
 * @return true:縮め/伸ばした場合。false:縮め/伸ばさなかった場合
 */
bool CPostMazeContext::Resize(std::wstring *yomi)
{
	yomi->clear();
	if(postyomiResizing == PYR_SHRINKING)
	{
		return Shrink(yomi);
	}
	else if(postyomiResizing == PYR_EXTENDING)
	{
		return Extend(yomi);
	}
	return false;
}

/**
 * 後置型交ぜ書き変換で、読みを縮める
 * @param [out] yomi 縮めた読み
 * @return true:縮めることができた場合。false:縮められなかった場合
 */
bool CPostMazeContext::Shrink(std::wstring *yomi)
{
	yomi->clear();
	if(postyomist >= postyomied)
	{
		return false;
	}
	if(IsYomiInflection()) //活用する語
	{
		//語幹の長さは保持したまま読みを縮める。対象読みを右にずらしたものに
		//例: 「あおい」に対し、「あお」→「おい」
		size_t ed = ForwardMoji(postyomi, postyomied, 1);
		if(ed > postyomied)
		{
			size_t st = ForwardMoji(postyomi, postyomist, 1);
			if(st > postyomist)
			{
				postyomist = st;
				postyomied = ed;
				yomi->assign(postyomi, st, ed - st);
				yomi->append(STR_INFLECTION_MARK);
				postyomiResizing = PYR_SHRINKING;
				return true;
			}
		}
		size_t curlen = CountMoji(postyomi.substr(postyomist, postyomied - postyomist));
		//ずらせない場合、語幹を縮めて、postyomiの最初から試行
		//例: 「あおい」に対し、「おい」→「あ」
		if(curlen > 1)
		{
			size_t alllen = CountMoji(postyomi);
			size_t newlen = curlen - 1;
			size_t st = 0;
			//語尾が長くなりすぎて、余分な候補が表示されるのを回避
			if(alllen - newlen > MAX_SUFFIX)
			{
				size_t n = alllen - newlen - MAX_SUFFIX;
				st = ForwardMoji(postyomi, st, n);
			}
			ed = ForwardMoji(postyomi, st, newlen);
			if(ed > st)
			{
				postyomist = st;
				postyomied = ed;
				yomi->assign(postyomi, st, ed - st);
				yomi->append(STR_INFLECTION_MARK);
				postyomiResizing = PYR_SHRINKING;
				return true;
			}
		}
	}
	else //活用しない語:読みを縮める。例:「あおい」に対して「おい」
	{
		size_t st = ForwardMoji(postyomi, postyomist, 1);
		if(st > postyomist && st < postyomied)
		{
			std::wstring s(postyomi.substr(st));
			if(s != STR_INFLECTION_MARK) //活用する語を示すマーカだけでない?
			{
				postyomist = st;
				yomi->assign(s);
				postyomiResizing = PYR_SHRINKING;
				return true;
			}
		}
		//活用しない語として変換できなかったので、活用する語として変換を試みる。
		//(ユーザが入力した'―'がある場合は、
		//既に活用する語として変換試行済。2重に'―'は付けない)
		if(resizeWithInflection && postyomi[postyomi.size() - 1] != CHAR_INFLECTION_MARK)
		{
			postyomist = 0;
			isInflection = true;
			yomi->assign(postyomi);
			yomi->append(STR_INFLECTION_MARK);
			postyomiResizing = PYR_SHRINKING;
			return true;
		}
	}
	return false;
}

/**
 * 後置型交ぜ書き変換で、読みを伸ばす
 * @param [out] yomi 伸ばした読み
 * @return true:伸ばすことができた場合。false:伸ばせなかった場合
 */
bool CPostMazeContext::Extend(std::wstring *yomi)
{
	yomi->clear();
	if(IsYomiInflection()) //活用する語
	{
		size_t suffixlen = CountMoji(postyomi.substr(postyomied));
		//語尾を長くしすぎて、余分な候補が表示されるのを回避
		if(suffixlen < MAX_SUFFIX)
		{
			//語幹の長さは保持したまま読みを伸ばす。対象読みを左にずらしたものに
			//例: 「あおい」に対し、「おい」→「あお」
			size_t st = BackwardMoji(postyomi, postyomist, 1);
			if(st < postyomist)
			{
				size_t ed = BackwardMoji(postyomi, postyomied, 1);
				if(ed < postyomied)
				{
					postyomist = st;
					postyomied = ed;
					yomi->assign(postyomi, st, ed - st);
					yomi->append(STR_INFLECTION_MARK);
					postyomiResizing = PYR_EXTENDING;
					return true;
				}
			}
		}
		size_t alllen = CountMoji(postyomi);
		size_t curlen = CountMoji(postyomi.substr(postyomist, postyomied - postyomist));
		//ずらせない場合、語幹を伸ばして、postyomiの末尾から試行
		//例: 「あおい」に対し、「あ」→「おい」
		if(curlen < alllen)
		{
			size_t ed = postyomi.size();
			size_t st = BackwardMoji(postyomi, ed, curlen + 1);
			if(st < ed)
			{
				postyomist = st;
				postyomied = ed;
				yomi->assign(postyomi, st, ed - st);
				yomi->append(STR_INFLECTION_MARK);
				postyomiResizing = PYR_EXTENDING;
				return true;
			}
		}
		if(resizeWithInflection)
		{
			//さらに伸ばす場合、活用しない語として変換を試みる
			isInflection = false;
			postyomied = postyomi.size();
			size_t st = BackwardMoji(postyomi, postyomied, 1);
			if(st < postyomied)
			{
				postyomist = st;
				yomi->assign(postyomi, st, postyomied - st);
				postyomiResizing = PYR_EXTENDING;
				return true;
			}
		}
	}
	else //活用しない語:読みを伸ばす。例:「あおい」に対して、「い」→「おい」
	{
		if(postyomist == 0)
		{
			return false;
		}
		size_t st = BackwardMoji(postyomi, postyomist, 1);
		if(st < postyomist)
		{
			postyomist = st;
			yomi->assign(postyomi.substr(st));
			postyomiResizing = PYR_EXTENDING;
			return true;
		}
	}
	return false;
}
