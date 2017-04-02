
#include "imcrvtip.h"
#include "TextService.h"
#include "CandidateList.h"
#include "mozc/win32/tip/tip_surrounding_text.h"
#include "mozc/win32/base/input_state.h"
#include "moji.h"

/**
 * 後置型交ぜ書き変換を開始する
 * @param isKatuyo 活用する語として変換を開始するか
 * @param resizeWithInflection 活用しない語を縮めた時に、活用する語としての変換を試みるか
 */
HRESULT CTextService::_HandlePostMaze(TfEditCookie ec, ITfContext *pContext, int count, PostConvContext postconvctx, bool isKatuyo, bool resizeWithInflection)
{
	postmazeContext.Deactivate();
	//カーソル直前の文字列を取得
	std::wstring text;
	_AcquirePrecedingYomi(pContext, postconvctx, &text, count);
	size_t size = text.size();
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
	//count==0:文字数指定無しの場合は縮めながら変換
	postmazeContext.Activate(text, isKatuyo, (count == 0), resizeWithInflection);
	std::wstring yomi;
	postmazeContext.GetYomi(true, &yomi);
	return _ReplacePrecedingText(ec, pContext, text, yomi, postconvctx, TRUE);
}

//後置型交ぜ書き変換用の読みをカーソル直前の文字列から取得する。
void CTextService::_AcquirePrecedingYomi(ITfContext *pContext, PostConvContext postconvctx, std::wstring *text, size_t count)
{
	text->clear();
	std::wstring s;
	_AcquirePrecedingText(pContext, postconvctx, &s);
	size_t size = s.size();
	if(size == 0)
	{
		return;
	}
	if(count > 0) //countが指定されている時は"。"等も含める
	{
		size_t st = BackwardMoji(s, size, count);
		text->assign(s.substr(st));
		return;
	}

	//count=0の場合、なるべく長く「読み」とみなす。文字列末尾から見ていく。
	//(a)"、"や"。"以前の文字は読みに含めない。
	size_t stch = s.find_last_of(L"\n\t 、。，．・「」（）");
	if(stch != std::wstring::npos)
	{
		stch = ForwardMoji(s, stch, 1);
	}
	else
	{
		stch = 0;
	}

	//(b)日本語文字とLatin文字の境目があれば、そこまでを取得する。
//Basic Latin + Latin-1 Supplement (XXX:とりあえず)
#define ISLATIN(m) ((m)[0] <= 0xFF)
	bool bLastLatin = false;
	size_t st = 0;
	size_t prevst = size;
	while((st = BackwardMoji(s, prevst, 1)) < prevst && st >= stch)
	{
		bool bLatin = ISLATIN(Get1Moji(s, st));
		if(prevst == size) //末尾の文字?
		{
			bLastLatin = bLatin;
		}
		else if(bLatin != bLastLatin)
		{
			st = prevst;
			break;
		}
		prevst = st;
	}

	//(a)と(b)で、先に止まったところまで
	if(stch > st)
	{
		st = stch;
	}
	text->assign(s.substr(st));
}

//文字列末尾にある連続するひらがなの最初の位置を返す
static size_t _BackwardWhileKana(const std::wstring &text)
{
	size_t st;
	size_t prevst = text.size();
	while((st = BackwardMoji(text, prevst, 1)) < prevst)
	{
		prevst = st;
//TRUEの文字が続く間、後置型カタカナ変換対象とする(ひらがな、「ー」)
#define TYOON(m) ((m[0]) == 0x30FC)
#define IN_HIRARANGE(m) (0x3041 <= (m[0]) && (m[0]) <= 0x309F || TYOON(m))
		if(!IN_HIRARANGE(Get1Moji(text, st)))
		{
			// 「キーとばりゅー」に対し1文字残してカタカナ変換で
			// 「キーとバリュー」になるように、先頭の「ー」は外す
			while((st = ForwardMoji(text, prevst, 1)) > prevst)
			{
				prevst = st;
				if(!TYOON(Get1Moji(text, st)))
				{
					break;
				}
			}
			break;
		}
	}
	return st;
}

/**
 * 後置型かな漢字変換を開始する
 * @param isKatuyo 活用する語として変換を開始するか
 * @param resizeWithInflection 活用しない語を縮めた時に、活用する語としての変換を試みるか
 */
HRESULT CTextService::_HandlePostKanaKan(TfEditCookie ec, ITfContext *pContext, PostConvContext postconvctx, bool isKatuyo, bool resizeWithInflection)
{
	postmazeContext.Deactivate();
	//カーソル直前の文字列を取得
	std::wstring text;
	_AcquirePrecedingText(pContext, postconvctx, &text);
	size_t st = _BackwardWhileKana(text);
	if(st >= text.size())
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
	std::wstring todel(text.substr(st));
	postmazeContext.Activate(todel, isKatuyo, true, resizeWithInflection);
	std::wstring yomi;
	postmazeContext.GetYomi(true, &yomi);
	return _ReplacePrecedingText(ec, pContext, todel, yomi, postconvctx, TRUE);
}

//後置型カタカナ変換
HRESULT CTextService::_HandlePostKata(TfEditCookie ec, ITfContext *pContext, int count, PostConvContext postconvctx)
{
	//カーソル直前の文字列を取得
	std::wstring text;
	_AcquirePrecedingText(pContext, postconvctx, &text);
	size_t size = text.size();
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
	size_t st;
	if(count > 0)
	{
		st = BackwardMoji(text, size, count);
	}
	else //count==0: ひらがなが続く間、負: ひらがなとして残す文字数指定
	{
		st = _BackwardWhileKana(text);
		if(count < 0) // 指定文字数を除いてカタカナに変換
		{
			st = ForwardMoji(text, st, -count);
		}
	}
	if(size > st)
	{
		std::wstring todel(text.substr(st));
		_ConvKanaToKana(todel, im_hiragana, kata, im_katakana);
		//カーソル直前の文字列を置換
		prevkata = kata;
		_ReplacePrecedingText(ec, pContext, todel, kata, postconvctx);
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

	size_t prevsize = prevkata.size();
	size_t size = text.size();
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
	size_t st = ForwardMoji(prevkata, 0, count);
	//縮めることでひらがなになる文字列
	std::wstring hira;
	_ConvKanaToKana(prevkata.substr(0, st), im_katakana, hira, im_hiragana);
	std::wstring todel(prevkata);
	if(st < prevsize)
	{
		//カタカナのままにする文字列
		//繰り返しShrinkできるように、prevkataを縮める
		prevkata.erase(0, st);
		hira.append(prevkata);
	}

	_ReplacePrecedingText(ec, pContext, todel, hira, postconvctx);
	return S_OK;
}

//後置型部首合成変換
HRESULT CTextService::_HandlePostBushu(TfEditCookie ec, ITfContext *pContext, PostConvContext postconvctx)
{
	//カーソル直前の文字列を取得
	std::wstring text;
	_AcquirePrecedingText(pContext, postconvctx, &text);

	size_t size = text.size();
	size_t b2st = BackwardMoji(text, size, 1);
	if(b2st < size)
	{
		std::wstring bushu2 = Get1Moji(text, b2st);
		size_t b1st = BackwardMoji(text, b2st, 1);
		if(b1st < b2st)
		{
			std::wstring bushu1 = Get1Moji(text, b1st);
			std::wstring kanji;
			_SearchBushuDic(bushu1, bushu2, &kanji);
			if(!kanji.empty())
			{
				//カーソル直前の文字列を置換
				_ReplacePrecedingText(ec, pContext, text.substr(b1st), kanji, postconvctx);
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
BOOL CTextService::isroman(std::wstring &ch)
{
	WCHAR c = ch[0];
	if(c > ISROMAN_TBL_SIZE)
	{
		return FALSE;
	}
	return isroman_tbl[c];
}

//後置型入力シーケンス→漢字変換
HRESULT CTextService::_HandlePostSeq2Kanji(TfEditCookie ec, ITfContext *pContext, int count, PostConvContext postconvctx)
{
	//カーソル直前の文字列を取得
	std::wstring text;
	_AcquirePrecedingText(pContext, postconvctx, &text);
	size_t size = text.size();
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
	size_t st;
	if(count > 0)
	{
		st = BackwardMoji(text, size, count);
	}
	else //count==0: 英字が続く間、負: そのまま残す文字数指定
	{
		size_t prevst = size;
		while((st = BackwardMoji(text, prevst, 1)) < prevst)
		{
			prevst = st;
			if(!isroman(Get1Moji(text, st)))
			{
				st = ForwardMoji(text, st, 1);
				break;
			}
		}
		if(count < 0) //指定文字数を除いて漢字に変換
		{
			st = ForwardMoji(text, st, -count);
		}
	}
	if(st < size)
	{
		std::wstring todel(text.substr(st));
		//入力シーケンスを漢字に変換
		WCHAR seq[ROMAN_NUM];
		std::wstring kanji;
		size_t i = 0;
		size_t prevst = st;
		do {
			prevst = st;
			std::wstring ch = Get1Moji(text, st);
			seq[i] = ch[0];
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
		} while((st = ForwardMoji(text, prevst, 1)) > prevst);
		//カーソル直前の文字列を置換
		_ReplacePrecedingText(ec, pContext, todel, kanji, postconvctx);
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
	size_t size = text.size();
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
	size_t st;
	if(count > 0)
	{
		st = BackwardMoji(text, size, count);
	}
	else //count==0: 改行やタブまで
	{
		size_t prevst = size;
		while((st = BackwardMoji(text, prevst, 1)) < prevst)
		{
			prevst = st;
			std::wstring m = Get1Moji(text, st);
			if(m[0] == L'\n' || m[0] == L'\t')
			{
				st = ForwardMoji(text, st, 1);
				break;
			}
			//最後の' 'は無視。途中打鍵を確定するために入力したものの可能性
			if(m[0] == L' ' && st != size - 1)
			{
				//最初の' 'は含める。区切り用に入力したものを削るため
				break;
			}
		}
		if(count < 0) //指定文字数を除いて入力シーケンスに変換
		{
			st = ForwardMoji(text, st, -count);
		}
	}
	if(st < size)
	{
		std::wstring todel(text.substr(st));
		//漢字を入力シーケンスに変換
		std::wstring seq;
		_ConvKanaToRoman(seq, todel, im_hiragana);
		//最後の連続する' 'は削除。途中打鍵を確定するために入力したもの
		size_t seqsize = seq.size();
		size_t trim = seqsize;
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
		_ReplacePrecedingText(ec, pContext, todel, seq, postconvctx);
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
		if(from != AF_SELECTION && count > 0)
		{
			size_t st = BackwardMoji(text, size, count);
			if(st < size)
			{
				_ShowAutoHelp(text.substr(st), L"");
				return S_OK;
			}
		}
		_ShowAutoHelp(text, L"");
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
		text->append(kana, 0, cursoridx);
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
		size_t st = BackwardMoji(postbuf, postbuf.size(), MAX_POSTBUF);
		postbuf.erase(0, st);
	}
}

//カーソル直前の文字列を、kanaに置換
HRESULT CTextService::_ReplacePrecedingText(TfEditCookie ec, ITfContext *pContext, const std::wstring &delstr, const std::wstring &replstr, PostConvContext postconvctx, BOOL startMaze)
{
	size_t delete_count = delstr.size();
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
				_pCandidateList->_SetText(replstr, TRUE, wm_none);
			}
		}
		return S_OK;
	}

	if(!mozc::win32::tsf::TipSurroundingText::DeletePrecedingText(this, pContext, CountMoji(delstr, MOJIMB_NONE)))
	{
		return _ReplacePrecedingTextIMM32(ec, pContext, CountMoji(delstr, MOJIMB_IVS), replstr, startMaze);
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
}

//カーソル直前文字列をBackspaceを送って消した後、置換文字列を確定する。
HRESULT CTextService::_ReplacePrecedingTextIMM32(TfEditCookie ec, ITfContext *pContext, size_t delete_count, const std::wstring &replstr, BOOL startMaze)
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
