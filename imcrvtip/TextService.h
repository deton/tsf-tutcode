﻿#pragma once

#include "imcrvtip.h"
#include "convtype.h"
#include "parseskkdic.h"
#include "configxml.h"
#include "mozc/win32/base/deleter.h"
#include "PostMazeContext.h"

class CLangBarItemButton;
class CCandidateList;
class CInputModeWindow;
class CVKeyboardWindow;

class CTextService :
	public ITfTextInputProcessorEx,
	public ITfThreadMgrEventSink,
	public ITfThreadFocusSink,
	public ITfCompartmentEventSink,
	public ITfTextEditSink,
	public ITfKeyEventSink,
	public ITfCompositionSink,
	public ITfDisplayAttributeProvider,
	public ITfFunctionProvider,
	public ITfFnConfigure,
	public ITfFnShowHelp,
	public ITfFnReconversion,
	public ITfFnGetPreferredTouchKeyboardLayout
{
public:
	CTextService();
	~CTextService();

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObj);
	STDMETHODIMP_(ULONG) AddRef(void);
	STDMETHODIMP_(ULONG) Release(void);

	// ITfTextInputProcessor
	STDMETHODIMP Activate(ITfThreadMgr *ptim, TfClientId tid);
	STDMETHODIMP Deactivate();

	// ITfTextInputProcessorEx
	STDMETHODIMP ActivateEx(ITfThreadMgr *ptim, TfClientId tid, DWORD dwFlags);

	// ITfThreadMgrEventSink
	STDMETHODIMP OnInitDocumentMgr(ITfDocumentMgr *pdim);
	STDMETHODIMP OnUninitDocumentMgr(ITfDocumentMgr *pdim);
	STDMETHODIMP OnSetFocus(ITfDocumentMgr *pdimFocus, ITfDocumentMgr *pdimPrevFocus);
	STDMETHODIMP OnPushContext(ITfContext *pic);
	STDMETHODIMP OnPopContext(ITfContext *pic);

	// ITfTextEditSink
	STDMETHODIMP OnEndEdit(ITfContext *pic, TfEditCookie ecReadOnly, ITfEditRecord *pEditRecord);

	// ITfThreadFocusSink
	STDMETHODIMP OnSetThreadFocus();
	STDMETHODIMP OnKillThreadFocus();

	// ItfCompartmentEventSink
	STDMETHODIMP OnChange(REFGUID rguid);

	// ITfKeyEventSink
	STDMETHODIMP OnSetFocus(BOOL fForeground);
	STDMETHODIMP OnTestKeyDown(ITfContext *pic, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
	STDMETHODIMP OnKeyDown(ITfContext *pic, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
	STDMETHODIMP OnTestKeyUp(ITfContext *pic, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
	STDMETHODIMP OnKeyUp(ITfContext *pic, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
	STDMETHODIMP OnPreservedKey(ITfContext *pic, REFGUID rguid, BOOL *pfEaten);

	// ITfCompositionSink
	STDMETHODIMP OnCompositionTerminated(TfEditCookie ecWrite, ITfComposition *pComposition);

	// ITfDisplayAttributeProvider
	STDMETHODIMP EnumDisplayAttributeInfo(IEnumTfDisplayAttributeInfo **ppEnum);
	STDMETHODIMP GetDisplayAttributeInfo(REFGUID guid, ITfDisplayAttributeInfo **ppInfo);

	// ITfFunctionProvider
	STDMETHODIMP GetType(GUID *pguid);
	STDMETHODIMP GetDescription(BSTR *pbstrDesc);
	STDMETHODIMP GetFunction(REFGUID rguid, REFIID riid, IUnknown **ppunk);

	// ITfFunction
	STDMETHODIMP GetDisplayName(BSTR *pbstrName);

	// ITfFnConfigure
	STDMETHODIMP Show(HWND hwndParent, LANGID langid, REFGUID rguidProfile);

	// ITfFnShowHelp
	STDMETHODIMP Show(HWND hwndParent);

	// ITfFnReconversion
	STDMETHODIMP QueryRange(ITfRange *pRange, ITfRange **ppNewRange, BOOL *pfConvertable);
	STDMETHODIMP GetReconversion(ITfRange *pRange, ITfCandidateList **ppCandList);
	STDMETHODIMP Reconvert(ITfRange *pRange);

	// ITfFnGetPreferredTouchKeyboardLayout
	STDMETHODIMP GetLayout(TKBLayoutType *pTKBLayoutType, WORD *pwPreferredLayoutId);

	ITfThreadMgr *_GetThreadMgr()
	{
		return _pThreadMgr;
	}
	TfClientId _GetClientId()
	{
		return _ClientId;
	}
	ITfComposition *_GetComposition()
	{
		return _pComposition;
	}

	// Compartment
	HRESULT _SetCompartment(REFGUID rguid, const VARIANT *pvar);
	HRESULT _GetCompartment(REFGUID rguid, VARIANT *pvar);
	BOOL _IsKeyboardDisabled();
	BOOL _IsKeyboardOpen();
	HRESULT _SetKeyboardOpen(BOOL fOpen);

	// Composition
	BOOL _IsComposing();
	void _SetComposition(ITfComposition *pComposition);
	BOOL _IsRangeCovered(TfEditCookie ec, ITfRange *pRangeTest, ITfRange *pRangeCover);
	BOOL _StartComposition(ITfContext *pContext);
	void _TerminateComposition(TfEditCookie ec, ITfContext *pContext);
	void _EndComposition(ITfContext *pContext);
	void _CancelComposition(TfEditCookie ec, ITfContext *pContext);
	void _ClearComposition();

	// LanguageBar
	void _UpdateLanguageBar(BOOL showinputmode = TRUE);
	void _GetIcon(HICON *phIcon, INT size);

	// DisplayAttribureProvider
	void _ClearCompositionDisplayAttributes(TfEditCookie ec, ITfContext *pContext);
	BOOL _SetCompositionDisplayAttributes(TfEditCookie ec, ITfContext *pContext, ITfRange *pRange, TfGuidAtom gaDisplayAttribute);

	// KeyHandler
	HRESULT _InvokeKeyHandler(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BYTE bSf);
	HRESULT _HandleKey(TfEditCookie ec, ITfContext *pContext, WPARAM wParam, BYTE bSf);
	void _KeyboardOpenCloseChanged(BOOL showinputmode = TRUE);
	void _KeyboardInputConversionChanged();
	BOOL _KeyboardSetDefaultMode();
	BOOL _IsKeyVoid(WCHAR ch, BYTE vk);
	void _ResetStatus();
	void _GetActiveFlags();
	void _InitD2D();
	void _UninitD2D();

	// KeyHandlerChar
	HRESULT _HandleChar(TfEditCookie ec, ITfContext *pContext, WPARAM wParam, WCHAR ch, WCHAR chO);
	HRESULT _HandleCharReturn(TfEditCookie ec, ITfContext *pContext, BOOL back = FALSE);
	HRESULT _HandleCharShift(TfEditCookie ec, ITfContext *pContext);
	bool _IsPostConvFunc(WCHAR ch);
	HRESULT _ConvRomanKanaWithWait(WCHAR ch, std::wstring *pRoman, ROMAN_KANA_CONV *pRkc);

	// KeyHandlerCompostion
	HRESULT _Update(TfEditCookie ec, ITfContext *pContext, BOOL fixed = FALSE, BOOL back = FALSE);
	HRESULT _Update(TfEditCookie ec, ITfContext *pContext, std::wstring &comptext, BOOL fixed = FALSE, BOOL back = FALSE);
	HRESULT _SetText(TfEditCookie ec, ITfContext *pContext, const std::wstring &text, LONG cchCursor, LONG cchOkuri, BOOL fixed);
	HRESULT _ShowCandidateList(TfEditCookie ec, ITfContext *pContext, int mode);
	void _EndCandidateList();
	void _EndCompletionList(TfEditCookie ec, ITfContext *pContext);
	BOOL _GetVertical(TfEditCookie ec, ITfContext *pContext);

	// KeyHandlerControl
	HRESULT _HandleControl(TfEditCookie ec, ITfContext *pContext, BYTE sf, WCHAR ch);
	HRESULT _HandleConvPoint(TfEditCookie ec, ITfContext *pContext, WCHAR ch);
	enum PostConvContext
	{
		PCC_COMPOSITION, //交ぜ書き変換の読み入力中 (全てtsf-tutcodeの管理下)
		PCC_REGWORD, 	 //辞書登録用エントリ編集中 (全てtsf-tutcodeの管理下)
		PCC_APP			 //通常時 (TSF等でアプリ等とのやりとりが必要)
	};
	void _HandleFunc(TfEditCookie ec, ITfContext *pContext, const ROMAN_KANA_CONV &rkc, WCHAR ch);
	PostConvContext _PrepareForFunc(TfEditCookie ec, ITfContext *pContext);
	void _HandleVkbdToggle(TfEditCookie ec, ITfContext *pContext, int n);

	// KeyHandlerConv
	WCHAR _GetCh(BYTE vk, BYTE vkoff = 0);
	BYTE _GetSf(BYTE vk, WCHAR ch);
	WORD _GetModifiers();
	HRESULT _ConvRomanKana(ROMAN_KANA_CONV *pconv);
	HRESULT _SearchRomanKanaNode(const ROMAN_KANA_NODE &tree, ROMAN_KANA_CONV *pconv, int depth);
	HRESULT _ConvAsciiJLatin(ASCII_JLATIN_CONV *pconv);
	void _StartConv(TfEditCookie ec, ITfContext *pContext);
	void _StartSubConv(WCHAR command);
	void _NextConv();
	void _PrevConv();
	void _NextComp();
	void _PrevComp();
	void _SetComp(const std::wstring &candidate);
	void _DynamicComp(TfEditCookie ec, ITfContext *pContext, BOOL sel = FALSE);
	void _ConvRoman();
	BOOL _ConvShift(WCHAR ch);
	BOOL _ConvN();
	void _ConvKanaToKana(const std::wstring &src, int srcmode, std::wstring &dst, int dstmode);
	BOOL _SearchKanaByKana(const ROMAN_KANA_NODE &tree, const WCHAR *src, int srcmode, std::wstring &dst, int dstmode);
	void _ConvKanaToRoman(std::wstring &dst, const std::wstring &src, int srcmode);
	BOOL _SearchRomanByKana(const ROMAN_KANA_NODE &tree, int srcmode, const WCHAR *src, std::wstring &dst);

	// KeyHandlerPostConv
	HRESULT _HandlePostMaze(TfEditCookie ec, ITfContext *pContext, int count, PostConvContext postconvctx, const std::wstring &abortedRomanForPostConv, bool isKatuyo, bool resizeWithInflection);
	void _AcquirePrecedingYomi(ITfContext *pContext, PostConvContext postconvctx, const std::wstring &abortedRomanForPostConv, std::wstring *yomi, size_t count);
	HRESULT _HandlePostKanaKan(TfEditCookie ec, ITfContext *pContext, PostConvContext postconvctx, const std::wstring &abortedRomanForPostConv, bool isKatuyo, bool resizeWithInflection);
	HRESULT _HandlePostKata(TfEditCookie ec, ITfContext *pContext, int count, PostConvContext postconvctx, const std::wstring &abortedRomanForPostConv, bool skipKata);
	HRESULT _HandlePostKataShrink(TfEditCookie ec, ITfContext *pContext, int count, PostConvContext postconvctx, const std::wstring &abortedRomanForPostConv);
	HRESULT _HandlePostBushu(TfEditCookie ec, ITfContext *pContext, PostConvContext postconvctx, const std::wstring &abortedRomanForPostConv);
	HRESULT _HandlePostSeq2Kanji(TfEditCookie ec, ITfContext *pContext, int count, PostConvContext postconvctx, const std::wstring &abortedRomanForPostConv);
	HRESULT _HandlePostKanji2Seq(TfEditCookie ec, ITfContext *pContext, int count, PostConvContext postconvctx, const std::wstring &abortedRomanForPostConv);
	HRESULT _HandlePostHelp(TfEditCookie ec, ITfContext *pContext, PostConvContext postconvctx, const std::wstring &abortedRomanForPostConv, int count);
	BOOL isroman(std::wstring &ch);
	enum AcquiredFrom
	{
		AF_COMPOSITION,
		AF_REGWORD,
		AF_POSTBUF,
		AF_PRECEDING,
		AF_SELECTION,
	};
	AcquiredFrom _AcquirePrecedingText(ITfContext *pContext, PostConvContext postconvctx, const std::wstring &abortedRomanForPostConv, std::wstring *text, BOOL useSelectedText = FALSE);
	void _AddToPostBuf(const std::wstring &text);
	HRESULT _ReplacePrecedingText(TfEditCookie ec, ITfContext *pContext, const std::wstring &delstr, const std::wstring &replstr, PostConvContext postconvctx, const std::wstring &abortedRomanForPostConv, BOOL startMaze = false);
	void _StartConvWithYomi(TfEditCookie ec, ITfContext *pContext, const std::wstring &yomi);
	HRESULT _ReplacePrecedingTextIMM32(TfEditCookie ec, ITfContext *pContext, size_t delete_count, const std::wstring &replstr, BOOL startMaze = false);
	HRESULT _ShowAutoHelp(const std::wstring &kanji, const std::wstring &yomi);
	void _CommitStr(TfEditCookie ec, ITfContext *pContext, const std::wstring &s, PostConvContext postconvctx);
	bool _CancelPostConv(TfEditCookie ec, ITfContext *pContext);

	// KeyHandlerDictionary
	void _ConnectDic();
	void _DisconnectDic();
	void _SearchDic(WCHAR command);
	void _SearchBushuDic(const std::wstring &bushu1, const std::wstring &bushu2, std::wstring *kanji);
	void _ConvertWord(WCHAR command, const std::wstring &key, const std::wstring &candidate, const std::wstring &okuri, std::wstring &conv);
	void _AddUserDic(WCHAR command, const std::wstring &key, const std::wstring &candidate, const std::wstring &annotation);
	void _DelUserDic(WCHAR command, const std::wstring &key, const std::wstring &candidate);
	void _SaveUserDic();
	void _CommandDic(WCHAR command);
	void _StartManager();
	void _StartConfigure();

	// FnConfig
	void _CreateConfigPath();
	void _CreateIpcName();
	void _ReadBoolValue(LPCWSTR section, LPCWSTR key, BOOL &value, BOOL defval);
	void _LoadBehavior();
	void _LoadDisplayAttr();
	void _LoadSelKey();
	void _SetPreservedKeyONOFF(int onoff, const APPDATAXMLLIST &list);
	void _LoadPreservedKey();
	void _LoadCKeyMap();
	void _LoadVKeyMap();
	void _LoadConvPoint();
	void _LoadKana();
	BOOL _AddKanaTree(ROMAN_KANA_NODE &tree, ROMAN_KANA_CONV rkc, int depth);
	void _AddKanaTreeItem(ROMAN_KANA_NODE &tree, ROMAN_KANA_CONV rkc, int depth);
	void _LoadJLatin();

	// InputModeWindow
	void _StartInputModeWindow();
	void _EndInputModeWindow();

	// FunctionProvider
	HRESULT _GetRangeText(ITfRange *pRange, std::wstring &text);
	HRESULT _SetResult(const std::wstring &fnsearchkey, const CANDIDATES &fncandidates, UINT index);

	// VKeyboardWindow
	void _StartVKeyboardWindow();
	void _EndVKeyboardWindow();
	void _HideVKeyboardWindow();
	void _RedrawVKeyboardWindow();
	std::wstring _MakeVkbTable();
	BOOL _IsRomanKanaStatus();

private:
	LONG _cRef;

	BOOL _InitThreadMgrEventSink();
	void _UninitThreadMgrEventSink();

	BOOL _InitThreadFocusSink();
	void _UninitThreadFocusSink();

	BOOL _InitCompartmentEventSink();
	void _UninitCompartmentEventSink();

	BOOL _InitTextEditSink(ITfDocumentMgr *pDocumentMgr);

	BOOL _InitKeyEventSink();
	void _UninitKeyEventSink();

	BOOL _InitPreservedKey(int onoff);
	void _UninitPreservedKey(int onoff);

	BOOL _InitLanguageBar();
	void _UninitLanguageBar();

	BOOL _InitDisplayAttributeGuidAtom();

	BOOL _InitFunctionProvider();
	void _UninitFunctionProvider();

	int _IsKeyEaten(ITfContext *pContext, WPARAM wParam, LPARAM lParam, bool isKeyDown, bool isTest);

	CComPtr<ITfThreadMgr> _pThreadMgr;
	TfClientId _ClientId;

	DWORD _dwThreadMgrEventSinkCookie;
	DWORD _dwThreadFocusSinkCookie;
	DWORD _dwCompartmentEventSinkOpenCloseCookie;
	DWORD _dwCompartmentEventSinkInputmodeConversionCookie;

	CComPtr<ITfContext> _pTextEditSinkContext;
	DWORD _dwTextEditSinkCookie;

	CComPtr<ITfComposition> _pComposition;

	CComPtr<CLangBarItemButton> _pLangBarItem;
	CComPtr<CLangBarItemButton> _pLangBarItemI;

	CComPtr<CCandidateList> _pCandidateList;

	CComPtr<CInputModeWindow> _pInputModeWindow;
	CComPtr<CVKeyboardWindow> _pVKeyboardWindow;

	TfGuidAtom _gaDisplayAttributeInputMark;
	TfGuidAtom _gaDisplayAttributeInputText;
	TfGuidAtom _gaDisplayAttributeInputOkuri;
	TfGuidAtom _gaDisplayAttributeConvMark;
	TfGuidAtom _gaDisplayAttributeConvText;
	TfGuidAtom _gaDisplayAttributeConvOkuri;
	TfGuidAtom _gaDisplayAttributeConvAnnot;

	mozc::win32::VKBackBasedDeleter deleter;

private:
	//ファイルパス
	WCHAR pathconfigxml[MAX_PATH];	//設定
	FILETIME ftconfigxml;			//更新時刻

	//imcrvmgr.exe との名前付きパイプ
	WCHAR mgrpipename[MAX_KRNLOBJNAME];
	HANDLE hPipe;
	WCHAR pipebuf[PIPEBUFSIZE];
	//ミューテックス
	WCHAR mgrmutexname[MAX_KRNLOBJNAME];
	WCHAR cnfmutexname[MAX_KRNLOBJNAME];

	//キーマップ
	CKEYMAP ckeymap;		//文字
	VKEYMAP vkeymap;		//仮想キー
	VKEYMAP vkeymap_shift;	//仮想キー(+Shift)
	VKEYMAP vkeymap_ctrl;	//仮想キー(+Ctrl)

	//変換位置指定(0:開始,1:代替,2:送り)
	std::vector<CONV_POINT> conv_point_s;	//開始で昇順ソート
	std::vector<CONV_POINT> conv_point_a;	//代替で昇順ソート

	//ローマ字仮名変換木
	ROMAN_KANA_NODE roman_kana_tree;
	//ASCII全英変換テーブル
	// メンバーasciiで昇順ソート
	std::vector<ASCII_JLATIN_CONV> ascii_jlatin_conv;
	//roman_kana_convのromanで使用される文字かどうか
	BOOL isroman_tbl[ISROMAN_TBL_SIZE];

public:
	CComPtr<ID2D1Factory> _pD2DFactory;
	CComPtr<ID2D1DCRenderTarget> _pD2DDCRT;
	CComPtr<ID2D1SolidColorBrush> _pD2DBrush[DISPLAY_LIST_COLOR_NUM];
	D2D1_DRAW_TEXT_OPTIONS _drawtext_option;
	CComPtr<IDWriteFactory> _pDWFactory;

	DWORD _dwActiveFlags;	//ITfThreadMgrEx::GetActiveFlags()
	BOOL _ImmersiveMode;	//Immersive Mode
	BOOL _UILessMode;		//UILess Mode
	BOOL _ShowInputMode;	//InputModeWindow

	//状態
	INT inputmode;			//入力モード (無し/ひらがな/カタカナ/半角ｶﾀｶﾅ/全英/アスキー)
	BOOL inputkey;			//見出し入力▽モード
	BOOL abbrevmode;		//abbrevモード
	BOOL showentry;			//候補表示▼モード
	BOOL showcandlist;		//候補リスト表示
	BOOL complement;		//補完
	BOOL purgedicmode;		//辞書削除モード
	BOOL hintmode;			//ヒントモード
	BOOL reconversion;		//再変換

	//動作設定
	WCHAR cx_fontname[LF_FACESIZE];	//候補一覧のフォント設定(フォント名)
	INT cx_fontpoint;				//候補一覧のフォント設定(サイズ)
	INT cx_fontweight;				//候補一覧のフォント設定(太さ)
	BOOL cx_fontitalic;				//候補一覧のフォント設定(イタリック)

	LONG cx_maxwidth;			//候補一覧の最大幅
	COLORREF cx_list_colors[DISPLAY_LIST_COLOR_NUM];		//候補一覧の色
	UINT cx_drawapi;			//候補一覧の描画API(0:GDI/1:Direct2D)
	BOOL cx_colorfont;			//候補一覧の描画API 彩色(Direct2Dのときカラーフォントにする)
	UINT cx_untilcandlist;		//候補一覧表示に要する変換回数(0:表示なし/1:1回目...)
	BOOL cx_verticalcand;		//候補一覧を縦に表示する
	BOOL cx_dispcandnum;		//候補一覧表示なしのとき候補数を表示する
	BOOL cx_annotation;			//注釈を表示する
	BOOL cx_annotatlst;			//注釈を表示する(FALSE:全て/TRUE:候補一覧のみ)
	BOOL cx_showmodemark;		//▽▼*マークを表示する
	BOOL cx_showroman;			//ローマ字を表示する
	BOOL cx_showromanjlat;		//ローマ字を表示する(FALSE:ASCII/TRUE:全英)
	BOOL cx_showromancomp;		//入力途中のキーシーケンスを表示する
	BOOL cx_showvkbd;			//入力途中に仮想鍵盤を表示する
	std::wstring cx_vkbdlayout;	//仮想鍵盤のレイアウト(dvorak等)
	std::wstring cx_vkbdtop;	//初期状態の仮想鍵盤に表示する内容

	BOOL cx_showmodeinl;		//入力モードを表示する
	UINT cx_showmodesec;		//入力モードの表示秒数
	COLORREF cx_mode_colors[DISPLAY_MODE_COLOR_NUM];		//入力モードの色

	BOOL cx_begincvokuri;		//送り仮名が決定したとき変換を開始する
	BOOL cx_shiftnnokuri;		//送り仮名で撥音を送り出す
	BOOL cx_srchallokuri;		//送りあり変換で送りなし候補も検索する
	BOOL cx_delcvposcncl;		//取消のとき変換位置を削除する
	BOOL cx_delokuricncl;		//取消のとき送り仮名を削除する
	BOOL cx_backincenter;		//後退に確定を含める
	BOOL cx_addcandktkn;		//候補に片仮名変換を追加する
	BOOL cx_entogglekana;		//かな／カナ入力モードをトグルする
	BOOL cx_setbydirect;		//キー設定「直接入力」を確定入力で使用する
	INT cx_fixmazecount;		//交ぜ書き変換の学習から除外する候補数(-1は学習無し)

	UINT cx_compmultinum;		//複数補完/複数動的補完の表示数
	BOOL cx_stacompmulti;		//複数補完を使用する
	BOOL cx_dynamiccomp;		//動的補完を使用する
	BOOL cx_dyncompmulti;		//複数動的補完を使用する
	BOOL cx_compuserdic;		//補完された見出し語の候補を表示する

	//ローマ字・仮名
	std::wstring roman;		//ローマ字
	std::wstring kana;		//仮名
	size_t okuriidx;		//送り仮名インデックス
	std::wstring reconvsrc;	//再変換元

	//検索用見出し語
	std::wstring searchkey;		//数値変換で数値→#
	std::wstring searchkeyorg;	//オリジナル

	//候補
	CANDIDATES candidates;	//候補
	size_t candidx;			//候補インデックス
	size_t candorgcnt;		//オリジナル見出し語の候補数

	size_t cursoridx;		//カーソルインデックス

	//後置型変換
	std::wstring postbuf;	//直近に確定した文字列
	std::wstring prevkata;	//直前の後置型カタカナ変換で変換した文字列
	CPostMazeContext postmazeContext; //後置型交ぜ書き変換状態

	//候補一覧選択キー
	WCHAR selkey[MAX_SELKEY_C][3][2];

	//preserved key
	TF_PRESERVEDKEY preservedkey[PRESERVEDKEY_NUM][MAX_PRESERVEDKEY];

	//表示属性   別のインスタンスからGetDisplayAttributeInfo()が呼ばれるのでstaticで
	static BOOL display_attribute_series[DISPLAYATTRIBUTE_INFO_NUM];
	static TF_DISPLAYATTRIBUTE display_attribute_info[DISPLAYATTRIBUTE_INFO_NUM];
};
