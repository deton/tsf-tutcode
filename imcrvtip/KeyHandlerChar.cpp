
#include "imcrvtip.h"
#include "TextService.h"
#include "CandidateList.h"
#include "mozc/win32/tip/tip_surrounding_text.h"
#include "mozc/win32/base/input_state.h"

HRESULT CTextService::_HandleChar(TfEditCookie ec, ITfContext *pContext, std::wstring &composition, WCHAR ch, WCHAR chO)
{
	ROMAN_KANA_CONV rkc;
	ASCII_JLATIN_CONV ajc;
	HRESULT ret = S_OK;
	std::wstring roman_conv;

	if(showentry)
	{
		_Update(ec, pContext, composition, TRUE);
		if(pContext == NULL)	//辞書登録用
		{
			composition.clear();
		}
		_ResetStatus();
		_HandleCharReturn(ec, pContext);
	}

	if(accompidx != 0 && accompidx == kana.size() && chO != L'\0')
	{
		kana.insert(cursoridx, 1, chO);
		cursoridx++;
	}

	switch(inputmode)
	{
	case im_hiragana:
	case im_katakana:
		if(abbrevmode)
		{
			_HandleCharTerminate(ec, pContext, composition);
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
					default:
						break;
					}

					_HandleCharTerminate(ec, pContext, composition);
					_Update(ec, pContext);
					break;
				}

				if(rkc.func)	//機能
				{
					_HandleFunc(ec, pContext, rkc, ch, composition);
					break;
				}

				switch(inputmode)
				{
				case im_hiragana:
					kana.insert(cursoridx, rkc.hiragana);
					if(accompidx != 0 && cursoridx < accompidx)
					{
						accompidx += wcslen(rkc.hiragana);
					}
					cursoridx += wcslen(rkc.hiragana);
					break;
				case im_katakana:
					kana.insert(cursoridx, rkc.katakana);
					if(accompidx != 0 && cursoridx < accompidx)
					{
						accompidx += wcslen(rkc.katakana);
					}
					cursoridx += wcslen(rkc.katakana);
					break;
				default:
					break;
				}

				roman.clear();

				if(!inputkey)
				{
					_HandleCharTerminate(ec, pContext, composition);	//候補＋仮名
					if(composition.empty())
					{
						_HandleCharReturn(ec, pContext);	//仮名のみ
					}
					kana.clear();
					cursoridx = 0;
					if(rkc.soku)
					{
						roman.push_back(ch);
						_Update(ec, pContext);
					}
				}
				else
				{
					_HandleCharTerminate(ec, pContext, composition);
					if(!kana.empty() && accompidx != 0 && !rkc.soku && !c_nookuriconv && !rkc.wait)
					{
						showentry = TRUE;
						_StartConv();
					}
					else if(rkc.soku)
					{
						roman.push_back(ch);
					}
					_Update(ec, pContext);
				}
				break;
			
			case E_PENDING:	//途中まで一致
				_HandleCharTerminate(ec, pContext, composition);
				roman.push_back(ch);
				if(c_showromancomp)
				{
					_Update(ec, pContext);
				}
				break;
			
			case E_ABORT:	//不一致
				_HandleCharTerminate(ec, pContext, composition);
				roman.clear();
				if(accompidx != 0 && accompidx + 1 == cursoridx)
				{
					kana.erase(cursoridx - 1, 1);	//送りローマ字削除
					cursoridx--;
					if(accompidx != kana.size())
					{
						accompidx = 0;
					}
				}
				_Update(ec, pContext);
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
		}
		break;

	case im_ascii:	//かなキーロックONのときのみ
		kana.push_back(ch);
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
	_Update(ec, pContext, TRUE, back);
	_TerminateComposition(ec, pContext);
	_ResetStatus();

	return S_OK;
}

HRESULT CTextService::_HandleCharTerminate(TfEditCookie ec, ITfContext *pContext, std::wstring &composition)
{
	if(!composition.empty())
	{
		_Update(ec, pContext, composition, TRUE);
		_TerminateComposition(ec, pContext);
	}

	return S_OK;
}

void CTextService::_HandleFunc(TfEditCookie ec, ITfContext *pContext, const ROMAN_KANA_CONV &rkc, WCHAR ch, std::wstring &composition)
{
	_PrepareForFunc(ec, pContext, composition);
	//前置型交ぜ書き変換
	if(wcsncmp(rkc.hiragana, L"maze", 4) == 0)
	{
		_HandleConvPoint(ec, pContext, ch);
		return;
	}
	//後置型交ぜ書き変換
	else if(wcsncmp(rkc.hiragana, L"Maze", 4) == 0)
	{
		int count = _wtoi(rkc.hiragana + 4);
		_HandlePostMaze(ec, pContext, count);
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
			_HandlePostKataShrink(ec, pContext, count);
		}
		else
		{
			_HandlePostKata(ec, pContext, count);
		}
		return;
	}
	//後置型部首合成変換
	else if(wcsncmp(rkc.hiragana, L"Bushu", 5) == 0)
	{
		_HandlePostBushu(ec, pContext);
		return;
	}
	_HandleCharReturn(ec, pContext);
}

//入力シーケンスに割り当てられた「機能」の実行前に、composition表示等をクリア
void CTextService::_PrepareForFunc(TfEditCookie ec, ITfContext *pContext, std::wstring &composition)
{
	//wordpadやWord2010だとcomposition表示をクリアしないとうまく動かず
	_ResetStatus();
	_HandleCharReturn(ec, pContext);
}

//後置型交ぜ書き変換
HRESULT CTextService::_HandlePostMaze(TfEditCookie ec, ITfContext *pContext, int count)
{
	//カーソル直前の文字列を取得
	std::wstring text;
	_AcquirePrecedingText(pContext, &text);
	int size = text.size();
	if(size == 0)
	{
		_HandleCharReturn(ec, pContext);
		return S_OK;
	}
	if(size < count)
	{
		count = size;
	}
	//TODO:サロゲートペアや結合文字等の考慮
	kana.insert(cursoridx, text.substr(size - count));
	cursoridx += kana.size();
	return _ReplacePrecedingText(ec, pContext, count, true);
}

//後置型カタカナ変換
HRESULT CTextService::_HandlePostKata(TfEditCookie ec, ITfContext *pContext, int count)
{
	//カーソル直前の文字列を取得
	std::wstring text;
	_AcquirePrecedingText(pContext, &text);
	int size = text.size();
	if(size == 0)
	{
		_HandleCharReturn(ec, pContext);
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
		kana.insert(cursoridx, kata);
		cursoridx += kata.size();
		prevkata = kata;
		_ReplacePrecedingText(ec, pContext, cnt);
	}
	else
	{
		_HandleCharReturn(ec, pContext);
	}

	return S_OK;
}

//直前の後置型カタカナ変換を縮める
//例: 「例えばあぷりけーしょん」ひらがなが続く間カタカナに変換
//	→「例エバアプリケーション」2文字縮める
//	→「例えばアプリケーション」
HRESULT CTextService::_HandlePostKataShrink(TfEditCookie ec, ITfContext *pContext, int count)
{
	if(prevkata.empty())
	{
		_HandleCharReturn(ec, pContext);
		return S_OK;
	}

	//カーソル直前の文字列を取得
	std::wstring text;
	_AcquirePrecedingText(pContext, &text);

	int prevsize = prevkata.size();
	int size = text.size();
	if(size == 0 || size < prevsize || prevkata != text.substr(size - prevsize))
	{
		_HandleCharReturn(ec, pContext);
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
	kana.insert(cursoridx, hira);
	cursoridx += hira.size();
	if(kataLen > 0)
	{
		//カタカナのままにする文字列
		//繰り返しShrinkできるように、prevkataを縮める
		prevkata = prevkata.substr(prevsize - kataLen);
		kana.insert(cursoridx, prevkata);
		cursoridx += kataLen;
	}

	_ReplacePrecedingText(ec, pContext, prevsize);
	return S_OK;
}

//後置型部首合成変換
HRESULT CTextService::_HandlePostBushu(TfEditCookie ec, ITfContext *pContext)
{
	//カーソル直前の文字列を取得
	std::wstring text;
	_AcquirePrecedingText(pContext, &text);

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
			kana.insert(cursoridx, 1, kanji);
			cursoridx++;

			_ReplacePrecedingText(ec, pContext, 2);
			return S_OK;
		}
	}
	_HandleCharReturn(ec, pContext);

	return S_OK;
}

//カーソル直前の文字列を取得
HRESULT CTextService::_AcquirePrecedingText(ITfContext *pContext, std::wstring *text)
{
	text->clear();
	mozc::win32::tsf::TipSurroundingTextInfo info;
	if(mozc::win32::tsf::TipSurroundingText::Get(this, pContext, &info))
	{
		text->append(info.preceding_text);
	}
	else
	{
		text->append(postbuf);
	}
	return S_OK;
}

//カーソル直前の文字列を、kanaに置換
HRESULT CTextService::_ReplacePrecedingText(TfEditCookie ec, ITfContext *pContext, int delete_count, bool startMaze)
{
	if(!mozc::win32::tsf::TipSurroundingText::DeletePrecedingText(this, pContext, delete_count))
	{
		return _ReplacePrecedingTextIMM32(ec, pContext, delete_count, startMaze);
	}
	if(startMaze)
	{
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
	else
	{
		_HandleCharReturn(ec, pContext);
	}
	return S_OK;
}

//カーソル直前文字列をBackspaceを送って消した後、置換文字列を確定する。
HRESULT CTextService::_ReplacePrecedingTextIMM32(TfEditCookie ec, ITfContext *pContext, int delete_count, bool startMaze)
{
	mozc::commands::Output pending;
	mozc::win32::InputState dummy;
	pending.kana = kana;
	pending.maze = startMaze;
	_ResetStatus();
	_HandleCharReturn(ec, pContext);
	deleter.BeginDeletion(delete_count, pending, dummy);
	return E_PENDING;
}
