
/*!
 * サロゲートペアを考慮して、count数前方の文字にidxを進める
 *
 * sの末尾に到達してcount数に満たない文字しか進められない場合あり。
 * \param s 対象文字列
 * \param idx s内のインデックス
 * \param count 進める文字数
 * \return 新しいidxの値。
 * sの末尾のため進められなかった場合は指定されたidx値のまま
 */
//TODO:結合文字、異体字セレクタの考慮
size_t _ForwardMoji(const std::wstring &s, size_t idx, size_t count)
{
	while(count-- > 0)
	{
		if(idx >= s.size())
		{
			idx = s.size();
			break;
		}
		if(idx + 1 < s.size() && IS_SURROGATE_PAIR(s[idx], s[idx + 1]))
		{
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
 * サロゲートペアを考慮して、count数後方の文字にidxを戻す
 *
 * sの先頭に到達してcount数に満たない文字しか戻せない場合あり。
 * \param s 対象文字列
 * \param idx s内のインデックス
 * \param count 戻す文字数
 * \return 新しいidxの値。sの先頭のため戻せなかった場合は指定されたidx値のまま
 */
//TODO:結合文字、異体字セレクタの考慮
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
		if(idx >= 2 && IS_SURROGATE_PAIR(s[idx - 2], s[idx - 1]))
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
 * サロゲートペアを考慮してidx位置の1文字をコピー
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
 * サロゲートペアを考慮してidx位置の1文字を取得
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
 * サロゲートペアを1文字とみなして、文字数を返す
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
