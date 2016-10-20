
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

	void Activate(const std::wstring& yomi, bool isKatuyo, bool startResizing, bool resizeWithInflection);
	void Deactivate();
	bool IsActive();
	void EndResizing();
	bool GetYomi(std::wstring *yomi);
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
};
