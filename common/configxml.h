﻿#pragma once

//tag

extern LPCWSTR TagRoot;
extern LPCWSTR TagSection;
extern LPCWSTR TagKey;
extern LPCWSTR TagEntry;
extern LPCWSTR TagList;
extern LPCWSTR TagRow;

//attribute

extern LPCWSTR AttributeName;
extern LPCWSTR AttributeValue;
extern LPCWSTR AttributeCandidate;
extern LPCWSTR AttributeAnnotation;
extern LPCWSTR AttributeKey;
extern LPCWSTR AttributePath;
extern LPCWSTR AttributeEnabled;
extern LPCWSTR AttributeVKey;
extern LPCWSTR AttributeMKey;
extern LPCWSTR AttributeCPStart;
extern LPCWSTR AttributeCPAlter;
extern LPCWSTR AttributeCPOkuri;
extern LPCWSTR AttributeRoman;
extern LPCWSTR AttributeHiragana;
extern LPCWSTR AttributeKatakana;
extern LPCWSTR AttributeKatakanaAnk;
extern LPCWSTR AttributeSpOp;
extern LPCWSTR AttributeLatin;
extern LPCWSTR AttributeJLatin;

//dictionary section

extern LPCWSTR SectionDictionary;

//server section

extern LPCWSTR SectionServer;

//server keys

extern LPCWSTR ValueServerServ;
extern LPCWSTR ValueServerHost;
extern LPCWSTR ValueServerPort;
extern LPCWSTR ValueServerEncoding;
extern LPCWSTR ValueServerTimeOut;

//behavior section

extern LPCWSTR SectionBehavior;

//behavior keys

extern LPCWSTR ValueDefaultMode;
extern LPCWSTR ValueDefModeAscii;
extern LPCWSTR ValueBeginCvOkuri;
extern LPCWSTR ValuePrecedeOkuri;
extern LPCWSTR ValueShiftNNOkuri;
extern LPCWSTR ValueSrchAllOkuri;
extern LPCWSTR ValueDelCvPosCncl;
extern LPCWSTR ValueDelOkuriCncl;
extern LPCWSTR ValueBackIncEnter;
extern LPCWSTR ValueAddCandKtkn;
extern LPCWSTR ValueEnToggleKana;
extern LPCWSTR ValueSetByDirect;
extern LPCWSTR ValueFixMazeCount;

extern LPCWSTR ValueCompMultiNum;
extern LPCWSTR ValueStaCompMulti;
extern LPCWSTR ValueDynamicComp;
extern LPCWSTR ValueDynCompMulti;
extern LPCWSTR ValueCompUserDic;
extern LPCWSTR ValueCompIncBack;

//font section

extern LPCWSTR SectionFont;

//font keys

extern LPCWSTR ValueFontName;
extern LPCWSTR ValueFontSize;
extern LPCWSTR ValueFontWeight;
extern LPCWSTR ValueFontItalic;

//display section

extern LPCWSTR SectionDisplay;

//display keys

extern LPCWSTR ValueMaxWidth;
extern LPCWSTR ValueColorBG;
extern LPCWSTR ValueColorFR;
extern LPCWSTR ValueColorSE;
extern LPCWSTR ValueColorCO;
extern LPCWSTR ValueColorCA;
extern LPCWSTR ValueColorSC;
extern LPCWSTR ValueColorAN;
extern LPCWSTR ValueColorNO;
extern LPCWSTR ValueDrawAPI;
extern LPCWSTR ValueColorFont;
extern LPCWSTR ValueUntilCandList;
extern LPCWSTR ValueDispCandNo;
extern LPCWSTR ValueVerticalCand;
extern LPCWSTR ValueAnnotation;
extern LPCWSTR ValueAnnotatLst;
extern LPCWSTR ValueShowModeMark;
extern LPCWSTR ValueShowRoman;
extern LPCWSTR ValueShowRomanJLat;
extern LPCWSTR ValueShowRomanComp;
extern LPCWSTR ValueShowVkbd;
extern LPCWSTR ValueVkbdLayout;
extern LPCWSTR ValueVkbdTop;

extern LPCWSTR ValueShowModeInl;
extern LPCWSTR ValueShowModeSec;
extern LPCWSTR ValueColorMC;
extern LPCWSTR ValueColorMF;
extern LPCWSTR ValueColorHR;
extern LPCWSTR ValueColorKT;
extern LPCWSTR ValueColorKA;
extern LPCWSTR ValueColorJL;
extern LPCWSTR ValueColorAC;
extern LPCWSTR ValueColorDR;

//displayattr section

extern LPCWSTR SectionDisplayAttr;

//displayattr keys

extern LPCWSTR ValueDisplayAttrInputMark;
extern LPCWSTR ValueDisplayAttrInputText;
extern LPCWSTR ValueDisplayAttrInputOkuri;
extern LPCWSTR ValueDisplayAttrConvMark;
extern LPCWSTR ValueDisplayAttrConvText;
extern LPCWSTR ValueDisplayAttrConvOkuri;
extern LPCWSTR ValueDisplayAttrConvAnnot;

//selkey section

extern LPCWSTR SectionSelKey;

//preservedkey section

extern LPCWSTR SectionPreservedKey;	//for compatibility
extern LPCWSTR SectionPreservedKeyON;
extern LPCWSTR SectionPreservedKeyOFF;

//keymap section

extern LPCWSTR SectionKeyMap;

//vkeymap section

extern LPCWSTR SectionVKeyMap;

//keymap and vkeymap keys

extern LPCWSTR ValueKeyMapKana;
extern LPCWSTR ValueKeyMapConvChar;
extern LPCWSTR ValueKeyMapJLatin;
extern LPCWSTR ValueKeyMapAscii;
extern LPCWSTR ValueKeyMapJMode;
extern LPCWSTR ValueKeyMapAbbrev;
extern LPCWSTR ValueKeyMapAffix;
extern LPCWSTR ValueKeyMapNextCand;
extern LPCWSTR ValueKeyMapPrevCand;
extern LPCWSTR ValueKeyMapPurgeDic;
extern LPCWSTR ValueKeyMapNextComp;
extern LPCWSTR ValueKeyMapPrevComp;
extern LPCWSTR ValueKeyMapCompCand;
extern LPCWSTR ValueKeyMapHint;
extern LPCWSTR ValueKeyMapConvPoint;
extern LPCWSTR ValueKeyMapDirect;
extern LPCWSTR ValueKeyMapEnter;
extern LPCWSTR ValueKeyMapCancel;
extern LPCWSTR ValueKeyMapBack;
extern LPCWSTR ValueKeyMapDelete;
extern LPCWSTR ValueKeyMapVoid;
extern LPCWSTR ValueKeyMapLeft;
extern LPCWSTR ValueKeyMapUp;
extern LPCWSTR ValueKeyMapRight;
extern LPCWSTR ValueKeyMapDown;
extern LPCWSTR ValueKeyMapPaste;
extern LPCWSTR ValueKeyMapOtherIme;
extern LPCWSTR ValueKeyMapViEsc;

//convpoint section

extern LPCWSTR SectionConvPoint;

//kana section

extern LPCWSTR SectionKana;

//jlatin section

extern LPCWSTR SectionJLatin;



#define L_EXIT l_exit
#define L_FAILED l_failed
#define EXIT_FAILED(hr) if(FAILED(hr)) goto L_FAILED

typedef std::pair<std::wstring, std::wstring> APPDATAXMLATTR;
typedef std::vector<APPDATAXMLATTR> APPDATAXMLROW;
typedef std::vector<APPDATAXMLROW> APPDATAXMLLIST;

HRESULT CreateStreamReader(LPCWSTR path, IXmlReader **ppReader, IStream **ppFileStream);

HRESULT ReadList(LPCWSTR path, LPCWSTR section, APPDATAXMLLIST &list);
HRESULT ReadValue(LPCWSTR path, LPCWSTR section, LPCWSTR key, std::wstring &strxmlval, LPCWSTR defval = L"");

HRESULT CreateStreamWriter(LPCWSTR path, IXmlWriter **ppWriter, IStream **ppFileStream);

HRESULT WriterInit(LPCWSTR path, IXmlWriter **ppWriter, IStream **ppFileStream, BOOL indent = TRUE);
HRESULT WriterFinal(IXmlWriter *pWriter);

HRESULT WriterNewLine(IXmlWriter *pWriter);
HRESULT WriterStartElement(IXmlWriter *pWriter, LPCWSTR element);
HRESULT WriterEndElement(IXmlWriter *pWriter);
HRESULT WriterAttribute(IXmlWriter *pWriter, LPCWSTR name, LPCWSTR value);

HRESULT WriterStartSection(IXmlWriter *pWriter, LPCWSTR name);
HRESULT WriterEndSection(IXmlWriter *pWriter);
HRESULT WriterKey(IXmlWriter *pWriter, LPCWSTR key, LPCWSTR value);
HRESULT WriterRow(IXmlWriter *pWriter, const APPDATAXMLROW &row);
HRESULT WriterList(IXmlWriter *pWriter, const APPDATAXMLLIST &list, BOOL newline = FALSE);
