
#include "imcrvtip.h"
#include "TextService.h"
#include "CandidateList.h"
#include "mozc/win32/tip/tip_surrounding_text.h"
#include "mozc/win32/base/input_state.h"

HRESULT CTextService::_HandleChar(TfEditCookie ec, ITfContext *pContext, std::wstring &comptext, WPARAM wParam, WCHAR ch, WCHAR chO)
{
	ROMAN_KANA_CONV rkc;
	ASCII_JLATIN_CONV ajc;
	HRESULT ret = S_OK;
	std::wstring roman_conv;

	if(showentry)
	{
		_Update(ec, pContext, comptext, TRUE);
		if(pContext == NULL)	//辞書登録用
		{
			comptext.clear();
		}
		_ResetStatus();
	}

	if(okuriidx != 0 && okuriidx == kana.size() && chO != L'\0')
	{
		kana.insert(cursoridx, 1, chO);
		cursoridx++;
	}

	switch(inputmode)
	{
	case im_hiragana:
	case im_katakana:
	case im_katakana_ank:
		if(abbrevmode)
		{
			_HandleCharShift(ec, pContext, comptext);
			roman.clear();
			kana.insert(cursoridx, 1, ch);
			cursoridx++;
			_Update(ec, pContext);
		}
		else
		{
			//ローマ字仮名変換 待機処理
			rkc.roman[0] = ch;
			rkc.roman[1] = L'\0';
			ret = _ConvRomanKana(&rkc);
			switch(ret)
			{
			case S_OK:	//一致
				if(rkc.wait)	//待機
				{
					ch = L'\0';
					switch(inputmode)
					{
					case im_hiragana:
						roman.append(rkc.hiragana);
						break;
					case im_katakana:
						roman.append(rkc.katakana);
						break;
					case im_katakana_ank:
						roman.append(rkc.katakana_ank);
						break;
					default:
						break;
					}
				}
				break;
			default:
				break;
			}

			//ローマ字仮名変換
			roman_conv = roman;
			if(ch != L'\0')
			{
				roman_conv.push_back(ch);
			}
			wcsncpy_s(rkc.roman, roman_conv.c_str(), _TRUNCATE);
			ret = _ConvRomanKana(&rkc);

			if(wParam == VK_PACKET && ret == E_ABORT && ch != TKB_NEXT_PAGE && ch != TKB_PREV_PAGE)
			{
				rkc.hiragana[0] = rkc.katakana[0] = rkc.katakana_ank[0] = ch;
				rkc.hiragana[1] = rkc.katakana[1] = rkc.katakana_ank[1] = L'\0';
				rkc.soku = FALSE;
				rkc.wait = FALSE;
				ret = S_OK;
			}

			switch(ret)
			{
			case S_OK:	//一致
				if(rkc.wait)	//待機
				{
					switch(inputmode)
					{
					case im_hiragana:
						roman.assign(rkc.hiragana);
						break;
					case im_katakana:
						roman.assign(rkc.katakana);
						break;
					case im_katakana_ank:
						roman.assign(rkc.katakana_ank);
						break;
					default:
						break;
					}

					_HandleCharShift(ec, pContext, comptext);
					_Update(ec, pContext);
					break;
				}

				if(rkc.func)	//機能
				{
					_HandleFunc(ec, pContext, rkc, ch);
					break;
				}

				switch(inputmode)
				{
				case im_hiragana:
					kana.insert(cursoridx, rkc.hiragana);
					if(okuriidx != 0 && cursoridx < okuriidx)
					{
						okuriidx += wcslen(rkc.hiragana);
					}
					cursoridx += wcslen(rkc.hiragana);
					break;
				case im_katakana:
					kana.insert(cursoridx, rkc.katakana);
					if(okuriidx != 0 && cursoridx < okuriidx)
					{
						okuriidx += wcslen(rkc.katakana);
					}
					cursoridx += wcslen(rkc.katakana);
					break;
				case im_katakana_ank:
					kana.insert(cursoridx, rkc.katakana_ank);
					if(okuriidx != 0 && cursoridx < okuriidx)
					{
						okuriidx += wcslen(rkc.katakana_ank);
					}
					cursoridx += wcslen(rkc.katakana_ank);
					break;
				default:
					break;
				}

				roman.clear();

				if(inputkey)
				{
					_HandleCharShift(ec, pContext, comptext);
					if(!kana.empty() && okuriidx != 0 && !rkc.soku && cx_begincvokuri && !hintmode && !rkc.wait)
					{
						cursoridx = kana.size();
						showentry = TRUE;
						_StartConv();
					}
					else if(rkc.soku)
					{
						roman.push_back(ch);
					}
					_Update(ec, pContext);
				}
				else
				{
					_HandleCharShift(ec, pContext, comptext);	//候補＋仮名
					if(comptext.empty())
					{
						_HandleCharShift(ec, pContext);	//仮名のみ
					}
					kana.clear();
					cursoridx = 0;
					if(rkc.soku)
					{
						roman.push_back(ch);
						_Update(ec, pContext);
					}
					else
					{
						_HandleCharReturn(ec, pContext);
					}
				}
				break;
			
			case E_PENDING:	//途中まで一致
				_HandleCharShift(ec, pContext, comptext);
				roman.push_back(ch);
				if(cx_showromancomp)
				{
					_Update(ec, pContext);
				}
				break;
			
			case E_ABORT:	//不一致
				_HandleCharShift(ec, pContext, comptext);
				roman.clear();
				if(okuriidx != 0 && okuriidx + 1 == cursoridx)
				{
					kana.erase(cursoridx - 1, 1);	//送りローマ字削除
					cursoridx--;
					if(okuriidx != kana.size())
					{
						okuriidx = 0;
					}
				}
				_Update(ec, pContext);
				if(!inputkey)
				{
					//OnCompositionTerminatedを呼ばないアプリの為にコンポジションを終了
					_HandleCharReturn(ec, pContext);
				}
				break;
			default:
				break;
			}
			break;
		}
		break;

	case im_jlatin:
		//ASCII全英変換
		roman.push_back(ch);
		wcsncpy_s(ajc.ascii, roman.c_str(), _TRUNCATE);
		ret = _ConvAsciiJLatin(&ajc);

		if(wParam == VK_PACKET && ret == E_ABORT && ch != TKB_NEXT_PAGE && ch != TKB_PREV_PAGE)
		{
			ajc.jlatin[0] = ch;
			ajc.jlatin[1] = L'\0';
			ret = S_OK;
		}

		switch(ret)
		{
		case S_OK:		//一致
			kana.assign(ajc.jlatin);
			cursoridx = kana.size();
			_HandleCharReturn(ec, pContext);
			break;
		case E_PENDING:	//途中まで一致
		case E_ABORT:	//不一致
			roman.clear();
			_HandleCharReturn(ec, pContext);
			break;
		default:
			break;
		}
		break;

	case im_ascii:	//かなキーロックONのときのみ
		ajc.ascii[0] = ch;
		ajc.ascii[1] = L'\0';
		kana.assign(ajc.ascii);
		cursoridx = kana.size();
		_HandleCharReturn(ec, pContext);
		break;

	default:
		break;
	}

	return ret;
}

HRESULT CTextService::_HandleCharReturn(TfEditCookie ec, ITfContext *pContext, BOOL back)
{
	//terminate composition
	_Update(ec, pContext, TRUE, back);
	_TerminateComposition(ec, pContext);
	_ResetStatus();

	return S_OK;
}

HRESULT CTextService::_HandleCharShift(TfEditCookie ec, ITfContext *pContext)
{
	std::wstring comptext;
	_Update(ec, pContext, comptext, TRUE);
	_ResetStatus();
	if(pContext != NULL)
	{
		_HandleCharShift(ec, pContext, comptext);
	}

	return S_OK;
}

HRESULT CTextService::_HandleCharShift(TfEditCookie ec, ITfContext *pContext, std::wstring &comptext)
{
	ITfRange * pRange;
	//leave composition
	if(!comptext.empty())
	{
		_Update(ec, pContext, comptext, TRUE);
		if(_IsComposing() && _pComposition->GetRange(&pRange) == S_OK)
		{
			pRange->Collapse(ec, TF_ANCHOR_END);
			_pComposition->ShiftStart(ec, pRange);
			pRange->Release();
		}
	}

	return S_OK;
}

void CTextService::_HandleFunc(TfEditCookie ec, ITfContext *pContext, const ROMAN_KANA_CONV &rkc, WCHAR ch)
{
	PostConvContext postconvctx = _PrepareForFunc(ec, pContext);
	//前置型交ぜ書き変換
	if(wcsncmp(rkc.hiragana, L"maze", 4) == 0)
	{
		if(postconvctx != PCC_COMPOSITION)
		{
			_HandleConvPoint(ec, pContext, ch);
		}
		else
		{
			_Update(ec, pContext);
		}
		return;
	}
	//後置型交ぜ書き変換
	else if(wcsncmp(rkc.hiragana, L"Maze", 4) == 0)
	{
		if(postconvctx != PCC_COMPOSITION)
		{
			//前置型交ぜ書き変換で入力中の読みの一部に対する後置型交ぜ書き変換
			//は未対応。候補表示等の制御が面倒なので。
			int count = _wtoi(rkc.hiragana + 4);
			if(count <= 0)
			{
				count = 1; //TODO:count=0の場合、なるべく長く読みとみなす
			}
			_HandlePostMaze(ec, pContext, count, postconvctx);
		}
		else
		{
			_Update(ec, pContext);
		}
		return;
	}
	//後置型カタカナ変換
	else if(wcsncmp(rkc.hiragana, L"Kata", 4) == 0)
	{
		int offset = 4;
		int isShrink = 0;
		if(rkc.hiragana[4] == L'>')
		{
			offset = 5;
			isShrink = 1;
		}
		int count = _wtoi(rkc.hiragana + offset);
		if(isShrink)
		{
			_HandlePostKataShrink(ec, pContext, count, postconvctx);
		}
		else
		{
			_HandlePostKata(ec, pContext, count, postconvctx);
		}
		return;
	}
	//後置型部首合成変換
	else if(wcsncmp(rkc.hiragana, L"Bushu", 5) == 0)
	{
		_HandlePostBushu(ec, pContext, postconvctx);
		return;
	}
	//後置型入力シーケンス→漢字変換
	//("Seq2Kanji"だとKANA_NUM(8)を越えるので"S2K")
	else if(wcsncmp(rkc.hiragana, L"S2K", 3) == 0)
	{
		int count = _wtoi(rkc.hiragana + 3);
		_HandlePostSeq2Kanji(ec, pContext, count, postconvctx);
		return;
	}
	//後置型漢字→入力シーケンス変換
	else if(wcsncmp(rkc.hiragana, L"K2S", 3) == 0)
	{
		int count = _wtoi(rkc.hiragana + 3);
		_HandlePostKanji2Seq(ec, pContext, count, postconvctx);
		return;
	}
	//打鍵ヘルプ
	else if(wcsncmp(rkc.hiragana, L"Help", 4) == 0)
	{
		int count = _wtoi(rkc.hiragana + 4);
		if(count <= 0)
		{
			count = 1;
		}
		_HandlePostHelp(ec, pContext, postconvctx, count);
		return;
	}
	else
	{
		if(postconvctx != PCC_COMPOSITION)
		{
			kana.clear();
			cursoridx = 0;
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
}

//入力シーケンスに割り当てられた「機能」の実行前に、composition表示等をクリア
CTextService::PostConvContext CTextService::_PrepareForFunc(TfEditCookie ec, ITfContext *pContext)
{
	roman.clear();
	if(inputkey && !kana.empty())
	{
		return PCC_COMPOSITION; //前置型交ぜ書き入力の読み入力中
	}
	else if (pContext == NULL)
	{
		return PCC_REGWORD; //辞書登録用編集中
	}
	else
	{
		_ResetStatus();
		if(cx_showromancomp)
		{
			//wordpadやWord2010だとcomposition表示をクリアしないとうまく動かず
			_HandleCharReturn(ec, pContext);
		}
		return PCC_APP;
	}
}

//後置型交ぜ書き変換
HRESULT CTextService::_HandlePostMaze(TfEditCookie ec, ITfContext *pContext, int count, PostConvContext postconvctx)
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
	return _ReplacePrecedingText(ec, pContext, count, text.substr(size - count), postconvctx, TRUE);
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
		_ConvKanaToKana(kata, im_katakana, text.substr(st), im_hiragana);
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
	_ConvKanaToKana(hira, im_hiragana, prevkata.substr(0, count), im_katakana);
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
	if(size >= 2)
	{
		//TODO:サロゲートペアや結合文字等の考慮
		WCHAR bushu1 = text[size - 2];
		WCHAR bushu2 = text[size - 1];
		//部首合成変換
		WCHAR kanji = _SearchBushuDic(bushu1, bushu2);
		if(kanji != 0)
		{
			//カーソル直前の文字列を置換
			std::wstring kanjistr(1, kanji);
			_ReplacePrecedingText(ec, pContext, 2, kanjistr, postconvctx);
			_ShowAutoHelp(kanjistr, L"");
			return S_OK;
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
		ROMAN_KANA_CONV rkc;
		std::wstring kanji;
		int i = 0;
		ZeroMemory(&rkc, sizeof(rkc));
		for(; st < size; st++)
		{
			rkc.roman[i] = text[st];
			HRESULT ret = _ConvRomanKana(&rkc);
			switch(ret)
			{
			case S_OK:	//一致
				//TODO: 後置型部首合成変換等のfunc対応
				kanji.append(rkc.hiragana);
				i = 0;
				ZeroMemory(&rkc, sizeof(rkc));
				break;
			case E_PENDING:	//途中まで一致
				++i;
				break;
			case E_ABORT:	//一致する可能性なし
			default:
				kanji.append(rkc.roman);
				i = 0;
				ZeroMemory(&rkc, sizeof(rkc));
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
		if(_pCandidateList != NULL)
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
		if(_pCandidateList != NULL)
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
	_StartConv();
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
