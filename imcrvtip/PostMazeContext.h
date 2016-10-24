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

#pragma once

/**
 * 後置型交ぜ書き変換の状態
 */
class CPostMazeContext
{
public:
	CPostMazeContext();
	~CPostMazeContext();
	//XXX:コピー代入演算子は自動生成されるものでOK
	//CPostMazeContext& operator=(const CPostMazeContext& from);

	void Activate(const std::wstring& yomi, bool isInflection, bool startResizing, bool resizeWithInflection);
	void Deactivate();
	bool IsActive();
	void EndResizing();
	bool GetYomi(bool withInflection, std::wstring *yomi);
	bool GetGobi(std::wstring *gobi);
	bool GetExcluded(std::wstring *excluded);
	void EraseExcluded();
	bool Resize(std::wstring *yomi);
	bool Shrink(std::wstring *yomi);
	bool Extend(std::wstring *yomi);

private:
	bool IsYomiInflection();

	std::wstring postyomi;	//文字数指定無し後置型交ぜ書き変換中の読み。縮め用
	size_t postyomist;		//postyomi中で、対象となる読みの開始インデックス
	size_t postyomied;		//postyomi中で、対象となる読みの終了インデックス
	enum PostYomiResizing
	{
		PYR_NO,
		PYR_SHRINKING,
		PYR_EXTENDING,
	};
	PostYomiResizing postyomiResizing;	//読みを縮め/伸ばしながらの変換試行中かどうか
	/** 活用しない語を縮めた時に、活用する語としての変換を試みる(逆も) */
	bool resizeWithInflection;
	bool isInflection; //活用する語としての変換中かどうか
};
