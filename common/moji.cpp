
#include "eucjis2004table.h"

/*!
 * JIS X 0213に含まれる結合文字かどうかを返す
 * \param m1 文字1
 * \param m2 文字2
 * \return TRUE: 結合文字の場合
 */
BOOL _IsJISCombiningMoji(WCHAR m1, WCHAR m2)
{
    for(int i = 0; i < CMBCHARNUM; i++)
    {
        if(m1 == euccmb[i].ucp[0] && m2 == euccmb[i].ucp[1])
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*!
 * サロゲートペア等を考慮して、count数前方の文字にidxを進める
 *
 * sの末尾に到達してcount数に満たない文字しか進められない場合あり。
 * JIS X 0213に含まれる結合文字は1文字とみなす。
 * \param s 対象文字列
 * \param idx s内のインデックス
 * \param count 進める文字数
 * \param ucs4 結合文字は考慮しない。
 * UCS4の文字単位でのみ扱う。サロゲートペアのみ考慮。
 * \return 新しいidxの値。
 * sの末尾のため進められなかった場合は指定されたidx値のまま
 */
//TODO:異体字セレクタの考慮
size_t _ForwardMoji(const std::wstring &s, size_t idx, size_t count, BOOL ucs4 = FALSE)
{
	while(count-- > 0)
	{
		if(idx >= s.size())
		{
			idx = s.size();
			break;
		}
		if(idx + 1 < s.size()
                && (IS_SURROGATE_PAIR(s[idx], s[idx + 1])
                    || !ucs4 && _IsJISCombiningMoji(s[idx], s[idx + 1])))
		{
			//CharNext()は結合文字対応らしいが、異体字セレクタのサロゲートペア
			//の間に進んだりしていまいち
			idx += 2;
		}
		else
		{
			idx += 1;
		}
	}
	return idx;
}

/*!
 * サロゲートペア等を考慮して、count数後方の文字にidxを戻す
 *
 * sの先頭に到達してcount数に満たない文字しか戻せない場合あり。
 * JIS X 0213に含まれる結合文字は1文字とみなす。
 * \param s 対象文字列
 * \param idx s内のインデックス
 * \param count 戻す文字数
 * \return 新しいidxの値。sの先頭のため戻せなかった場合は指定されたidx値のまま
 */
//TODO:異体字セレクタの考慮
size_t _BackwardMoji(const std::wstring &s, size_t idx, size_t count)
{
	if(idx > s.size())
	{
		idx = s.size();
	}
	while(count-- > 0)
	{
		if(idx == 0)
		{
			break;
		}
		if(idx >= 2
				&& (IS_SURROGATE_PAIR(s[idx - 2], s[idx - 1])
					|| _IsJISCombiningMoji(s[idx - 2], s[idx - 1])))
		{
			idx -= 2;
		}
		else if(idx >= 1)
		{
			idx -= 1;
		}
	}
	return idx;
}

/*!
 * サロゲートペア等を考慮してidx位置の1文字をコピー
 * \param s 対象文字列
 * \param idx s内のインデックス
 * \param [out] target コピー先
 * \return 1文字コピー後のインデックス。
 * 0: sの末尾に到達してコピーできなかった場合
 */
size_t _Copy1Moji(const std::wstring &s, size_t idx, std::wstring *target)
{
	target->clear();
	size_t ed = _ForwardMoji(s, idx, 1);
	if(ed == idx)
	{
		return 0;
	}
	target->append(s.substr(idx, ed - idx));
	return ed;
}

/*!
 * サロゲートペア等を考慮してidx位置の1文字を取得
 * \param s 対象文字列
 * \param idx s内のインデックス
 * \param 取得した1文字(新規文字列)。
 * sの末尾のため取得できなかった場合は空文字列(empty)
 */
std::wstring _Get1Moji(const std::wstring &s, size_t idx)
{
	std::wstring m;
	_Copy1Moji(s, idx, &m);
	return m;
}

/*!
 * サロゲートペア等を1文字とみなして、文字数を返す
 * \param s 対象文字列
 * \return 文字数
 */
size_t _CountMoji(const std::wstring &s)
{
	size_t count = 0;
	size_t previdx = 0;
	size_t idx;
	while((idx = _ForwardMoji(s, previdx, 1)) > previdx)
	{
		count++;
		previdx = idx;
	}
	return count;
}

/*!
 * サロゲートペアを1文字とみなして、UCS4での文字数を返す
 * \param s 対象文字列
 * \return 文字数
 */
size_t _CountMojiInUcs4(const std::wstring &s)
{
	size_t count = 0;
	size_t previdx = 0;
	size_t idx;
	while((idx = _ForwardMoji(s, previdx, 1, TRUE)) > previdx)
	{
		count++;
		previdx = idx;
	}
	return count;
}
