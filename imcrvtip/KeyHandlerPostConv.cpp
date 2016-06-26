
#include "imcrvtip.h"
#include "TextService.h"
#include "CandidateList.h"
#include "mozc/win32/tip/tip_surrounding_text.h"
#include "mozc/win32/base/input_state.h"

//サロゲートペアを考慮して、offset位置より前のcount文字を取得する。
//XXX:結合文字の考慮
//\return 新しいoffsetの値
static size_t _GetCharsBack(const std::wstring &s, std::wstring *res, size_t offset, size_t count)
{
	res->clear();
	if(offset <= 0)
	{
		return offset;
	}
	size_t st = offset;
	size_t ed = offset;
	while (count-- > 0)
	{
		if(s.size() >= 2 && st >= 2 && IS_SURROGATE_PAIR(s[st - 2], s[st - 1]))
		{
			st -= 2;
		}
		else if(s.size() >= 1)
		{
			st -= 1;
		}
		else
		{
			break;
		}
	}
	res->append(s.substr(st, ed - st));
	return st;
}

//後置型交ぜ書き変換
HRESULT CTextService::_HandlePostMaze(TfEditCookie ec, ITfContext *pContext, int count, PostConvContext postconvctx, BOOL isKatuyo)
{
	//カーソル直前の文字列を取得
	std::wstring text;
	_AcquirePrecedingText(pContext, postconvctx, &text);
	int size = text.size();
	if(size == 0)
	{
		if(postconvctx == PCC_APP)
		{
			_HandleCharReturn(ec, pContext);
		}
		else
		{
			_Update(ec, pContext);
		}
		return S_OK;
	}
	if(size < count)
	{
		count = size;
	}
	//TODO:サロゲートペアや結合文字等の考慮
	std::wstring yomi(text.substr(size - count));
	if(isKatuyo)
	{
		//TODO:読みに含まれる語尾を―に置き換えて変換
		yomi.append(L"―");
	}
	return _ReplacePrecedingText(ec, pContext, count, yomi, postconvctx, TRUE);
}

//後置型カタカナ変換
HRESULT CTextService::_HandlePostKata(TfEditCookie ec, ITfContext *pContext, int count, PostConvContext postconvctx)
{
	//カーソル直前の文字列を取得
	std::wstring text;
	_AcquirePrecedingText(pContext, postconvctx, &text);
	int size = text.size();
	if(size == 0)
	{
		if(postconvctx == PCC_APP)
		{
			_HandleCharReturn(ec, pContext);
		}
		else
		{
			_Update(ec, pContext);
		}
		return S_OK;
	}

	//ひらがなをカタカナに変換
	std::wstring kata;
	int st = size - count;
	if(st < 0)
	{
		st = 0;
	}
	if(count <= 0) //0: ひらがなが続く間、負: ひらがなとして残す文字数指定
	{
		//TODO:サロゲートペアや結合文字等の考慮
		for(st = size - 1; 0 <= st; st--)
		{
//TRUEの文字が続く間、後置型カタカナ変換対象とする(ひらがな、「ー」)
#define TYOON(m) ((m) == 0x30FC)
#define IN_KATARANGE(m) (0x3041 <= (m) && (m) <= 0x309F || TYOON(m))
			WCHAR m = text[st];
			if(!IN_KATARANGE(m))
			{
				// 「キーとばりゅー」に対し1文字残してカタカナ変換で
				// 「キーとバリュー」になるように「ー」は除く
				while(st < size - 1)
				{
					m = text[st + 1];
					if(TYOON(m))
					{
						st++;
					}
					else
					{
						break;
					}
				}
				break;
			}
		}
		st++;
		if(count < 0)
		{
			st += -count; // 指定文字数を除いてカタカナに変換
		}
	}
	int cnt = size - st;
	if(cnt > 0)
	{
		_ConvKanaToKana(text.substr(st), im_hiragana, kata, im_katakana);
		//カーソル直前の文字列を置換
		prevkata = kata;
		_ReplacePrecedingText(ec, pContext, cnt, kata, postconvctx);
	}
	else
	{
		if(postconvctx == PCC_APP)
		{
			_HandleCharReturn(ec, pContext);
		}
		else
		{
			_Update(ec, pContext);
		}
	}

	return S_OK;
}

//直前の後置型カタカナ変換を縮める
//例: 「例えばあぷりけーしょん」ひらがなが続く間カタカナに変換
//	→「例エバアプリケーション」2文字縮める
//	→「例えばアプリケーション」
HRESULT CTextService::_HandlePostKataShrink(TfEditCookie ec, ITfContext *pContext, int count, PostConvContext postconvctx)
{
	if(prevkata.empty())
	{
		if(postconvctx == PCC_APP)
		{
			kana.clear();
			cursoridx = 0;
			_HandleCharReturn(ec, pContext);
		}
		else
		{
			_Update(ec, pContext);
		}
		return S_OK;
	}

	//カーソル直前の文字列を取得
	std::wstring text;
	_AcquirePrecedingText(pContext, postconvctx, &text);

	int prevsize = prevkata.size();
	int size = text.size();
	if(size < prevsize || text.compare(size - prevsize, prevsize, prevkata) != 0)
	{
		if(postconvctx == PCC_APP)
		{
			_HandleCharReturn(ec, pContext);
		}
		else
		{
			_Update(ec, pContext);
		}
		return S_OK;
	}
	//countぶん縮める部分をひらがなにする
	int kataLen = prevsize - count;
	if(kataLen < 0)
	{
		kataLen = 0;
		count = prevsize;
	}
	//縮めることでひらがなになる文字列
	std::wstring hira;
	_ConvKanaToKana(prevkata.substr(0, count), im_katakana, hira, im_hiragana);
	if(kataLen > 0)
	{
		//カタカナのままにする文字列
		//繰り返しShrinkできるように、prevkataを縮める
		prevkata.erase(0, count);
		hira.append(prevkata);
	}

	_ReplacePrecedingText(ec, pContext, prevsize, hira, postconvctx);
	return S_OK;
}

//後置型部首合成変換
HRESULT CTextService::_HandlePostBushu(TfEditCookie ec, ITfContext *pContext, PostConvContext postconvctx)
{
	//カーソル直前の文字列を取得
	std::wstring text;
	_AcquirePrecedingText(pContext, postconvctx, &text);

	size_t size = text.size();
	std::wstring bushu2;
	size_t idx = _GetCharsBack(text, &bushu2, size, 1);
	if(idx < size)
	{
		std::wstring bushu1;
		size_t j = _GetCharsBack(text, &bushu1, idx, 1);
		if(j < idx)
		{
			std::wstring kanji;
			_SearchBushuDic(bushu1, bushu2, &kanji);
			if(!kanji.empty())
			{
				//カーソル直前の文字列を置換
				_ReplacePrecedingText(ec, pContext, 2, kanji, postconvctx);
				_ShowAutoHelp(kanji, L"");
				return S_OK;
			}
		}
	}
	if(postconvctx == PCC_APP)
	{
		_HandleCharReturn(ec, pContext);
	}
	else
	{
		_Update(ec, pContext);
	}

	return S_OK;
}

//roman_kana_convのromanで使用される文字かどうか
BOOL CTextService::isroman(WCHAR ch)
{
	if(ch > ISROMAN_TBL_SIZE)
	{
		return FALSE;
	}
	return isroman_tbl[ch];
}

//後置型入力シーケンス→漢字変換
HRESULT CTextService::_HandlePostSeq2Kanji(TfEditCookie ec, ITfContext *pContext, int count, PostConvContext postconvctx)
{
	//カーソル直前の文字列を取得
	std::wstring text;
	_AcquirePrecedingText(pContext, postconvctx, &text);
	int size = text.size();
	if(size == 0)
	{
		if(postconvctx == PCC_APP)
		{
			_HandleCharReturn(ec, pContext);
		}
		else
		{
			_Update(ec, pContext);
		}
		return S_OK;
	}

	//対象入力シーケンス文字列を特定
	int st = size - count;
	if(st < 0)
	{
		st = 0;
	}
	if(count <= 0) //0: 英字が続く間、負: そのまま残す文字数指定
	{
		//TODO:サロゲートペアや結合文字等の考慮
		for(st = size - 1; 0 <= st; st--)
		{
			WCHAR m = text[st];
			if(!isroman(m))
			{
				break;
			}
		}
		st++;
		if(count < 0)
		{
			st += -count; //指定文字数を除いて漢字に変換
		}
	}
	int cnt = size - st;
	if(cnt > 0)
	{
		//入力シーケンスを漢字に変換
		WCHAR seq[ROMAN_NUM];
		std::wstring kanji;
		int i = 0;
		for(; st < size; st++)
		{
			seq[i] = text[st];
			seq[i+1] = L'\0';
			ROMAN_KANA_CONV rkc; //_ConvRomanKana()で変更されるので呼出毎に生成
			wcscpy_s(rkc.roman, seq);
			HRESULT ret = _ConvRomanKana(&rkc);
			switch(ret)
			{
			case S_OK:	//一致
				//TODO: 後置型部首合成変換等のfunc対応
				kanji.append(rkc.hiragana);
				i = 0;
				break;
			case E_PENDING:	//途中まで一致。(rkcは変更されている)
				++i;
				break;
			case E_ABORT:	//一致する可能性なし
			default:
				kanji.append(seq); // rkc.romanは消去されている
				i = 0;
				break;
			}
		}
		//カーソル直前の文字列を置換
		_ReplacePrecedingText(ec, pContext, cnt, kanji, postconvctx);
	}
	else
	{
		if(postconvctx == PCC_APP)
		{
			_HandleCharReturn(ec, pContext);
		}
		else
		{
			_Update(ec, pContext);
		}
	}

	return S_OK;
}

//後置型漢字→入力シーケンス変換
HRESULT CTextService::_HandlePostKanji2Seq(TfEditCookie ec, ITfContext *pContext, int count, PostConvContext postconvctx)
{
	//カーソル直前の文字列を取得
	std::wstring text;
	_AcquirePrecedingText(pContext, postconvctx, &text);
	int size = text.size();
	if(size == 0)
	{
		if(postconvctx == PCC_APP)
		{
			_HandleCharReturn(ec, pContext);
		}
		else
		{
			_Update(ec, pContext);
		}
		return S_OK;
	}

	//対象入力シーケンス文字列を特定
	int st = size - count;
	if(st < 0)
	{
		st = 0;
	}
	if(count <= 0) //0: 改行やタブまで
	{
		//TODO:サロゲートペアや結合文字等の考慮
		for(st = size - 1; 0 <= st; st--)
		{
			WCHAR m = text[st];
			if(m == L'\n' || m == L'\t')
			{
				break;
			}
			//最後の' 'は無視。途中打鍵を確定するために入力したものの可能性
			if(m == L' ' && st != size - 1)
			{
				st--; //最初の' 'は含める。区切り用に入力したものを削るため
				break;
			}
		}
		st++;
		if(count < 0)
		{
			st += -count; //指定文字数を除いて漢字に変換
		}
	}
	int cnt = size - st;
	if(cnt > 0)
	{
		//漢字を入力シーケンスに変換
		std::wstring seq;
		_ConvKanaToRoman(seq, text.substr(st), im_hiragana);
		//最後の連続する' 'は削除。途中打鍵を確定するために入力したもの
		int seqsize = seq.size();
		int trim = seqsize;
		while(trim > 0 && seq[trim - 1] == L' ')
		{
			--trim;
		}
		if(trim < seqsize)
		{
			seq.erase(trim, seqsize - trim);
		}
		//区切り用に入力された最初のスペースは削る
		if(seq[0] == L' ')
		{
			seq.erase(0, 1);
		}
		//カーソル直前の文字列を置換
		_ReplacePrecedingText(ec, pContext, cnt, seq, postconvctx);
	}
	else
	{
		if(postconvctx == PCC_APP)
		{
			_HandleCharReturn(ec, pContext);
		}
		else
		{
			_Update(ec, pContext);
		}
	}

	return S_OK;
}

//打鍵ヘルプ
HRESULT CTextService::_HandlePostHelp(TfEditCookie ec, ITfContext *pContext, PostConvContext postconvctx, int count)
{
	std::wstring text;
	AcquiredFrom from = _AcquirePrecedingText(pContext, postconvctx, &text, TRUE);

	size_t size = text.size();
	if(size > 0)
	{
		if(from != AF_SELECTION && size - count > 0)
		{
			_ShowAutoHelp(text.substr(size - count), L"");
		}
		else
		{
			_ShowAutoHelp(text, L"");
		}
	}

	return S_OK;
}

//カーソル直前の文字列を取得
CTextService::AcquiredFrom CTextService::_AcquirePrecedingText(ITfContext *pContext, PostConvContext postconvctx, std::wstring *text, BOOL useSelectedText)
{
	text->clear();
	//前置型交ぜ書き変換の読み入力中の場合は、入力済みの読みを対象にする
	if(postconvctx == PCC_COMPOSITION)
	{
		text->append(kana.substr(0, cursoridx));
		return AF_COMPOSITION;
	}
	// 辞書登録用エントリ編集中は、編集中文字列を対象にする
	else if(postconvctx == PCC_REGWORD)
	{
		if(_pCandidateList != nullptr)
		{
			_pCandidateList->_GetPrecedingText(text);
		}
		return AF_REGWORD;
	}
	kana.clear();
	cursoridx = 0;

	mozc::win32::tsf::TipSurroundingTextInfo info;
	if(mozc::win32::tsf::TipSurroundingText::Get(this, pContext, &info))
	{
		if(useSelectedText && info.selected_text.size() > 0)
		{
			text->append(info.selected_text);
			return AF_SELECTION;
		}
		else if(info.preceding_text.size() > 0)
		{
			text->append(info.preceding_text);
			return AF_PRECEDING;
		}
		else
		{
			text->append(postbuf);
			return AF_POSTBUF;
		}
	}
	else
	{
		text->append(postbuf);
		return AF_POSTBUF;
	}
}

void CTextService::_AddToPostBuf(const std::wstring &text)
{
	postbuf.append(text);
#define MAX_POSTBUF 10
	if(postbuf.size() > MAX_POSTBUF)
	{
		postbuf.erase(0, postbuf.size() - MAX_POSTBUF);
	}
}

//カーソル直前の文字列を、kanaに置換
HRESULT CTextService::_ReplacePrecedingText(TfEditCookie ec, ITfContext *pContext, int delete_count, const std::wstring &replstr, PostConvContext postconvctx, BOOL startMaze)
{
	if(postconvctx == PCC_COMPOSITION)
	{
		kana.erase(cursoridx - delete_count, delete_count);
		cursoridx -= delete_count;
		kana.insert(cursoridx, replstr);
		cursoridx += replstr.size();
		_Update(ec, pContext);
		return S_OK;
	}
	else if(postconvctx == PCC_REGWORD)
	{
		if(_pCandidateList != nullptr)
		{
			_pCandidateList->_DeletePrecedingText(delete_count);
			if(startMaze)
			{
				_StartConvWithYomi(ec, pContext, replstr);
			}
			else
			{
				_pCandidateList->_SetText(replstr, TRUE, FALSE, FALSE);
			}
		}
		return S_OK;
	}

	if(!mozc::win32::tsf::TipSurroundingText::DeletePrecedingText(this, pContext, delete_count))
	{
		return _ReplacePrecedingTextIMM32(ec, pContext, delete_count, replstr, startMaze);
	}
	if(startMaze)
	{
		_StartConvWithYomi(ec, pContext, replstr);
	}
	else
	{
		kana = replstr;
		cursoridx = kana.size();
		_HandleCharReturn(ec, pContext);
	}
	return S_OK;
}

//(後置型交ぜ書き変換開始時に)指定した読み文字列で交ぜ書き変換を開始する
void CTextService::_StartConvWithYomi(TfEditCookie ec, ITfContext *pContext, const std::wstring &yomi)
{
	kana = yomi;
	cursoridx = kana.size();
	//(候補無し時、登録に入るため。でないと読みが削除されただけの状態)
	if(!_IsComposing())
	{
		_StartComposition(pContext);
	}
	//交ぜ書き変換候補表示開始
	showentry = TRUE;
	inputkey = TRUE;
	_StartConv(ec, pContext);
	_Update(ec, pContext);
	//TODO:cancel時は前置型読み入力モードでなく後置型開始前の状態に
}

//カーソル直前文字列をBackspaceを送って消した後、置換文字列を確定する。
HRESULT CTextService::_ReplacePrecedingTextIMM32(TfEditCookie ec, ITfContext *pContext, int delete_count, const std::wstring &replstr, BOOL startMaze)
{
	mozc::commands::Output pending;
	mozc::win32::InputState dummy;
	pending.kana = replstr;
	pending.maze = startMaze ? true : false;
	_ResetStatus();
	_HandleCharReturn(ec, pContext);
	deleter.BeginDeletion(delete_count, pending, dummy);
	return E_PENDING;
}

//打鍵ヘルプ表示: 漢索窓が起動されていれば、そこに表示
HRESULT CTextService::_ShowAutoHelp(const std::wstring &kanji, const std::wstring &yomi)
{
	HWND hwnd = FindWindow(L"kansaku", NULL);
	if(hwnd == NULL)
	{
		return E_FAIL;
	}

	std::wstring str;
	//ヘルプ表示不要(読みとして入力した文字/重複する文字)かどうか
	class skiphelp
	{
		const std::wstring _yomi;
		const std::wstring _helpstr;
	public:
		skiphelp(const std::wstring& yomi, const std::wstring& helpstr):
			_yomi(yomi), _helpstr(helpstr)
		{
		}
		bool operator()(wchar_t c)
		{
			return _yomi.find(c) != std::wstring::npos
				|| _helpstr.find(c) != std::wstring::npos;
		}
	};
	remove_copy_if(kanji.begin(), kanji.end(), back_inserter(str), skiphelp(yomi, str));

	//XXX:クリップボード内容を上書きされるのはユーザにはうれしくない
	if(OpenClipboard(NULL))
	{
		size_t len = str.size() + 1;
		size_t size = len * sizeof(WCHAR);
		HGLOBAL hMem = GlobalAlloc(GMEM_FIXED, size);
		if(hMem != NULL)
		{
			LPWSTR pMem = (LPWSTR)hMem;
			wcsncpy_s(pMem, len, str.c_str(), _TRUNCATE);
			EmptyClipboard();
			SetClipboardData(CF_UNICODETEXT, hMem);
		}
		CloseClipboard();
		if(hMem != NULL)
		{
			PostMessage(hwnd, WM_LBUTTONDBLCLK, 0, 0);
			//漢索窓を最前面に表示させる
			HWND foreWin = GetForegroundWindow();
			DWORD foreThread = GetWindowThreadProcessId(foreWin, NULL);
			DWORD selfThread = GetCurrentThreadId();
			AttachThreadInput(selfThread, foreThread, TRUE);
			SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
				SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_ASYNCWINDOWPOS);
			//最前面に出たままになって邪魔にならないように
			SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0,
				SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW | SWP_ASYNCWINDOWPOS);
			AttachThreadInput(selfThread, foreThread, FALSE);
		}
	}
	return S_OK;
}
