
#include "imcrvtip.h"
#include "TextService.h"
#include "CandidateList.h"
#include "moji.h"

HRESULT CTextService::_HandleChar(TfEditCookie ec, ITfContext *pContext, WPARAM wParam, WCHAR ch, WCHAR chO)
{
	ROMAN_KANA_CONV rkc = {};
	ASCII_JLATIN_CONV ajc = {};
	HRESULT ret = S_OK;

	if (showentry)
	{
		//後置型交ぜ書き変換の候補表示時
		if (postmazeContext.IsActive())
		{
			if (ch == L'>') //読みを縮める操作が行われた
			{
				std::wstring yomi;
				if (postmazeContext.Shrink(&yomi))
				{
					_StartConvWithYomi(ec, pContext, yomi);
				}
				return S_OK;
			}
			else if (ch == L'<') //読みを伸ばす操作が行われた
			{
				std::wstring yomi;
				if (postmazeContext.Extend(&yomi))
				{
					_StartConvWithYomi(ec, pContext, yomi);
				}
				return S_OK;
			}
		}

		_HandleCharShift(ec, pContext);
	}

	if ((okuriidx != 0) && (okuriidx + 1 == cursoridx))
	{
		if (chO != L'\0')
		{
			kana.replace(okuriidx, 1, 1, chO);	//送りローマ字
		}
	}

	switch (inputmode)
	{
	case im_hiragana:
	case im_katakana:
	case im_katakana_ank:
		if (abbrevmode)
		{
			kana.insert(cursoridx, 1, ch);
			cursoridx++;

			if (cx_dynamiccomp || cx_dyncompmulti)
			{
				_DynamicComp(ec, pContext);
			}
			else
			{
				_Update(ec, pContext);
			}
		}
		else
		{
			ret = _ConvRomanKanaWithWait(ch, &roman, &rkc);

			//sent from touch-optimized keyboard
			if (ret == E_ABORT && wParam == VK_PACKET && ch != TKB_NEXT_PAGE && ch != TKB_PREV_PAGE)
			{
				rkc.roman[0] = rkc.hiragana[0] = rkc.katakana[0] = rkc.katakana_ank[0] = ch;
				rkc.roman[1] = rkc.hiragana[1] = rkc.katakana[1] = rkc.katakana_ank[1] = L'\0';
				rkc.soku = FALSE;
				rkc.wait = FALSE;
				ret = S_OK;
			}

			switch (ret)
			{
			case S_OK:	//一致
				if (rkc.roman[0] != L'\0' && rkc.wait)	//待機
				{
					_HandleCharShift(ec, pContext);

					switch (inputmode)
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

					_Update(ec, pContext);
				}
				else if (rkc.func)	//機能
				{
					_HandleFunc(ec, pContext, rkc, ch);
				}
				else
				{
					std::wstring kana_ins;
					switch (inputmode)
					{
					case im_hiragana:
						kana_ins = rkc.hiragana;
						break;
					case im_katakana:
						kana_ins = rkc.katakana;
						break;
					case im_katakana_ank:
						kana_ins = rkc.katakana_ank;
						break;
					default:
						break;
					}

					if (!kana_ins.empty())
					{
						kana.insert(cursoridx, kana_ins);
						if (okuriidx != 0 && cursoridx <= okuriidx)
						{
							okuriidx += kana_ins.size();
						}
						cursoridx += kana_ins.size();
					}

					roman.clear();

					_ConvOkuriRoman();

					if (inputkey)
					{
						_HandleCharShift(ec, pContext);

						if (cx_begincvokuri && !hintmode &&
							!kana.empty() && okuriidx != 0 && !rkc.soku && !rkc.wait)
						{
							cursoridx = kana.size();
							showentry = TRUE;
							_StartConv(ec, pContext);
							_Update(ec, pContext);
							break;
						}

						if (cx_dynamiccomp || cx_dyncompmulti)
						{
							_DynamicComp(ec, pContext);

							if (rkc.soku)
							{
								roman.push_back(ch);
								_Update(ec, pContext);
							}
						}
						else
						{
							if (rkc.soku)
							{
								roman.push_back(ch);
							}
							_Update(ec, pContext);
						}
					}
					else
					{
						if (rkc.soku)
						{
							_HandleCharShift(ec, pContext);
							roman.push_back(ch);
							_Update(ec, pContext);
						}
						else
						{
							if (pContext != nullptr)
							{
								cursoridx = kana.size();
								_Update(ec, pContext, TRUE);
							}
							_HandleCharReturn(ec, pContext);
						}
					}
				}
				break;

			case E_PENDING:	//途中まで一致
				_HandleCharShift(ec, pContext);

				if (rkc.roman[0] != L'\0' && rkc.wait)	//待機
				{
					switch (inputmode)
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
					_Update(ec, pContext);
				}
				else
				{
					roman.push_back(ch);
					if (cx_showromancomp)
					{
						_Update(ec, pContext);
					}
					else
					{
						_RedrawVKeyboardWindow();
						if (!inputkey)
						{
							//OnEndEdit()とOnCompositionTerminated()から
							//_ResetStatus()が呼ばれてroman.clear()されるのを回避する
							_TerminateComposition(ec, pContext);
						}
					}
				}
				break;

			case E_ABORT:	//不一致
				_HandleCharShift(ec, pContext);
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

		if (ret == E_ABORT && wParam == VK_PACKET && ch != TKB_NEXT_PAGE && ch != TKB_PREV_PAGE)
		{
			ajc.jlatin[0] = ch;
			ajc.jlatin[1] = L'\0';
			ret = S_OK;
		}

		switch (ret)
		{
		case S_OK:		//一致
			kana.assign(ajc.jlatin);
			if (pContext != nullptr)
			{
				cursoridx = kana.size();
				_Update(ec, pContext, TRUE);
			}
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
		if (pContext != nullptr)
		{
			cursoridx = kana.size();
			_Update(ec, pContext, TRUE);
		}
		_HandleCharReturn(ec, pContext);
		break;

	default:
		break;
	}

	return ret;
}

HRESULT CTextService::_HandleCharReturn(TfEditCookie ec, ITfContext *pContext, BOOL back)
{
	std::wstring fixedtext;
	_EndCompletionList(ec, pContext);

	//terminate composition
	cursoridx = kana.size();
	_Update(ec, pContext, fixedtext, TRUE, back);
	_RedrawVKeyboardWindow();
	if (pContext != nullptr)
	{
		_AddToPostBuf(fixedtext);
	}

	_TerminateComposition(ec, pContext);
	_ResetStatus();

	return S_OK;
}

HRESULT CTextService::_HandleCharShift(TfEditCookie ec, ITfContext *pContext)
{
	if (showentry || (!inputkey && !kana.empty() && roman.empty()))
	{
		_EndCompletionList(ec, pContext);

		//leave composition
		cursoridx = kana.size();
		_Update(ec, pContext, TRUE);

		if (pContext != nullptr)
		{
			if (_IsComposing())
			{
				CComPtr<ITfRange> pRange;
				if (SUCCEEDED(_pComposition->GetRange(&pRange)) && (pRange != nullptr))
				{
					pRange->Collapse(ec, TF_ANCHOR_END);
					_pComposition->ShiftStart(ec, pRange);
				}
			}
		}

		_ResetStatus();
	}

	return S_OK;
}

bool CTextService::_IsPostConvFunc(WCHAR ch)
{
	if (!(inputmode == im_hiragana || inputmode == im_katakana || inputmode == im_katakana_ank))
	{
		return false;
	}
	if (abbrevmode)
	{
		return false;
	}
	std::wstring emptyroman;
	ROMAN_KANA_CONV rkc;
	HRESULT ret = _ConvRomanKanaWithWait(ch, &emptyroman, &rkc);
	return(ret == S_OK && rkc.func && isupper(rkc.hiragana[0]));
}

HRESULT CTextService::_ConvRomanKanaWithWait(WCHAR ch, std::wstring *pRoman, ROMAN_KANA_CONV *pRkc)
{
	ROMAN_KANA_CONV rkc;
	HRESULT ret = S_OK;

	//ローマ字仮名変換 待機処理
	// 「ﾞ」(JIS X 0201 濁点) → 「゛」(JIS X 0208 濁点)
	// 「か」(ka) + 「゛」(濁点) → 「か゛」(か+濁点)
	if (!roman.empty())
	{
		rkc.roman[0] = ch;
		rkc.roman[1] = L'\0';
		ret = _ConvRomanKana(&rkc);
		switch (ret)
		{
		case S_OK:	//一致
		case E_PENDING:	//途中まで一致
			if (rkc.roman[0] != L'\0' && rkc.wait)	//待機
			{
				std::wstring roman_conv = *pRoman;
				switch (inputmode)
				{
				case im_hiragana:
					roman_conv.append(rkc.hiragana);
					break;
				case im_katakana:
					roman_conv.append(rkc.katakana);
					break;
				case im_katakana_ank:
					roman_conv.append(rkc.katakana_ank);
					break;
				default:
					break;
				}

				ROMAN_KANA_CONV rkcn;
				wcsncpy_s(rkcn.roman, roman_conv.c_str(), _TRUNCATE);
				ret = _ConvRomanKana(&rkcn);
				switch (ret)
				{
				case S_OK:	//一致
				case E_PENDING:	//途中まで一致
					if (rkcn.roman[0] != L'\0')	//待機あり/なし
					{
						ch = L'\0';
						switch (inputmode)
						{
						case im_hiragana:
							pRoman->append(rkc.hiragana);
							break;
						case im_katakana:
							pRoman->append(rkc.katakana);
							break;
						case im_katakana_ank:
							pRoman->append(rkc.katakana_ank);
							break;
						default:
							break;
						}
					}
					break;
				default:
					break;
				}
			}
			break;
		default:
			break;
		}
	}

	//ローマ字仮名変換
	std::wstring roman_conv = *pRoman;
	if (ch != L'\0')
	{
		roman_conv.push_back(ch);
	}
	wcsncpy_s(rkc.roman, roman_conv.c_str(), _TRUNCATE);
	ret = _ConvRomanKana(&rkc);
	*pRkc = rkc;
	return ret;
}
