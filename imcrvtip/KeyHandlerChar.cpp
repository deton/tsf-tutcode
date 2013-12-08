
#include "imcrvtip.h"
#include "TextService.h"
#include "CandidateList.h"
#include "mozc/win32/tip/tip_surrounding_text.h"
#include "mozc/win32/base/input_state.h"

HRESULT CTextService::_HandleChar(TfEditCookie ec, ITfContext *pContext, std::wstring &composition, WPARAM wParam, WCHAR ch, WCHAR chO)
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
	case im_katakana_ank:
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
				case im_katakana_ank:
					kana.insert(cursoridx, rkc.katakana_ank);
					if(accompidx != 0 && cursoridx < accompidx)
					{
						accompidx += wcslen(rkc.katakana_ank);
					}
					cursoridx += wcslen(rkc.katakana_ank);
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
					if(!kana.empty() && accompidx != 0 && !rkc.soku && cx_begincvokuri && !hintmode && !rkc.wait)
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
				break;
			
			case E_PENDING:	//途中まで一致
				_HandleCharTerminate(ec, pContext, composition);
				roman.push_back(ch);
				if(cx_showromancomp)
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
	BOOL incomp = _PrepareForFunc(ec, pContext, composition);
	//前置型交ぜ書き変換
	if(wcsncmp(rkc.hiragana, L"maze", 4) == 0)
	{
		if(!incomp)
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
		int count = _wtoi(rkc.hiragana + 4);
		if(!incomp)
		{
			//前置型交ぜ書き変換で入力中の読みの一部に対する後置型交ぜ書き変換
			//は未対応。候補表示等の制御が面倒なので。
			_HandlePostMaze(ec, pContext, count);
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
			_HandlePostKataShrink(ec, pContext, count, incomp);
		}
		else
		{
			_HandlePostKata(ec, pContext, count, incomp);
		}
		return;
	}
	//後置型部首合成変換
	else if(wcsncmp(rkc.hiragana, L"Bushu", 5) == 0)
	{
		_HandlePostBushu(ec, pContext, incomp);
		return;
	}
	else
	{
		if(!incomp)
		{
			kana.clear();
			cursoridx = 0;
		}
	}
	if(!incomp)
	{
		_HandleCharReturn(ec, pContext);
	}
	else
	{
		_Update(ec, pContext);
	}
}

//入力シーケンスに割り当てられた「機能」の実行前に、composition表示等をクリア
BOOL CTextService::_PrepareForFunc(TfEditCookie ec, ITfContext *pContext, std::wstring &composition)
{
	roman.clear();
	if(inputkey && !kana.empty())
	{
		return TRUE;
	}
	else
	{
		//wordpadやWord2010だとcomposition表示をクリアしないとうまく動かず
		_ResetStatus();
		_HandleCharReturn(ec, pContext);
		return FALSE;
	}
}

//後置型交ぜ書き変換
HRESULT CTextService::_HandlePostMaze(TfEditCookie ec, ITfContext *pContext, int count)
{
	//カーソル直前の文字列を取得
	std::wstring text;
	_AcquirePrecedingText(pContext, FALSE, &text);
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
	return _ReplacePrecedingText(ec, pContext, count, text.substr(size - count), FALSE, TRUE);
}

//後置型カタカナ変換
HRESULT CTextService::_HandlePostKata(TfEditCookie ec, ITfContext *pContext, int count, BOOL incomp)
{
	//カーソル直前の文字列を取得
	std::wstring text;
	_AcquirePrecedingText(pContext, incomp, &text);
	int size = text.size();
	if(size == 0)
	{
		if(!incomp)
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
		_ReplacePrecedingText(ec, pContext, cnt, kata, incomp);
	}
	else
	{
		if(!incomp)
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
HRESULT CTextService::_HandlePostKataShrink(TfEditCookie ec, ITfContext *pContext, int count, BOOL incomp)
{
	if(prevkata.empty())
	{
		if(!incomp)
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
	_AcquirePrecedingText(pContext, incomp, &text);

	int prevsize = prevkata.size();
	int size = text.size();
	if(size < prevsize || text.compare(size - prevsize, prevsize, prevkata) != 0)
	{
		if(!incomp)
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

	_ReplacePrecedingText(ec, pContext, prevsize, hira, incomp);
	return S_OK;
}

//後置型部首合成変換
HRESULT CTextService::_HandlePostBushu(TfEditCookie ec, ITfContext *pContext, BOOL incomp)
{
	//カーソル直前の文字列を取得
	std::wstring text;
	_AcquirePrecedingText(pContext, incomp, &text);

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
			_ReplacePrecedingText(ec, pContext, 2, kanjistr, incomp);
			_ShowAutoHelp(kanjistr, L"");
			return S_OK;
		}
	}
	if(!incomp)
	{
		_HandleCharReturn(ec, pContext);
	}
	else
	{
		_Update(ec, pContext);
	}

	return S_OK;
}

//カーソル直前の文字列を取得
HRESULT CTextService::_AcquirePrecedingText(ITfContext *pContext, BOOL incomp, std::wstring *text)
{
	text->clear();
	//前置型交ぜ書き変換の読み入力中の場合は、入力済みの読みを対象にする
	if(incomp)
	{
		text->append(kana.substr(0, cursoridx));
		return S_OK;
	}
	kana.clear();
	cursoridx = 0;

	mozc::win32::tsf::TipSurroundingTextInfo info;
	if(mozc::win32::tsf::TipSurroundingText::Get(this, pContext, &info))
	{
		if(info.preceding_text.size() > 0)
		{
			text->append(info.preceding_text);
		}
		else
		{
			text->append(postbuf);
		}
	}
	else
	{
		text->append(postbuf);
	}
	return S_OK;
}

//カーソル直前の文字列を、kanaに置換
HRESULT CTextService::_ReplacePrecedingText(TfEditCookie ec, ITfContext *pContext, int delete_count, const std::wstring &replstr, BOOL incomp, BOOL startMaze)
{
	if(incomp)
	{
		kana.erase(cursoridx - delete_count, delete_count);
		cursoridx -= delete_count;
		kana.insert(cursoridx, replstr);
		cursoridx += replstr.size();
		_Update(ec, pContext);
		return S_OK;
	}

	if(!mozc::win32::tsf::TipSurroundingText::DeletePrecedingText(this, pContext, delete_count))
	{
		return _ReplacePrecedingTextIMM32(ec, pContext, delete_count, replstr, startMaze);
	}
	kana = replstr;
	cursoridx = kana.size();
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
HRESULT CTextService::_ReplacePrecedingTextIMM32(TfEditCookie ec, ITfContext *pContext, int delete_count, const std::wstring &replstr, BOOL startMaze)
{
	mozc::commands::Output pending;
	mozc::win32::InputState dummy;
	pending.kana = replstr;
	pending.maze = startMaze;
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
		}
	}
	return S_OK;
}
