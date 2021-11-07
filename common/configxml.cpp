﻿
#include "common.h"
#include "configxml.h"

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "xmllite.lib")

//tag

LPCWSTR TagRoot = L"skk";
LPCWSTR TagSection = L"section";
LPCWSTR TagEntry = L"entry";
LPCWSTR TagKey = L"key";
LPCWSTR TagList = L"list";
LPCWSTR TagRow = L"row";

//attribute

LPCWSTR AttributeName = L"name";
LPCWSTR AttributeValue = L"value";
LPCWSTR AttributeCandidate = L"c";
LPCWSTR AttributeAnnotation = L"a";
LPCWSTR AttributeKey = L"key";
LPCWSTR AttributePath = L"path";
LPCWSTR AttributeEnabled = L"enabled";
LPCWSTR AttributeVKey = L"vkey";
LPCWSTR AttributeMKey = L"mkey";
LPCWSTR AttributeCPStart = L"st";
LPCWSTR AttributeCPAlter = L"al";
LPCWSTR AttributeCPOkuri = L"ok";
LPCWSTR AttributeRoman = L"ro";
LPCWSTR AttributeHiragana = L"hi";
LPCWSTR AttributeKatakana = L"ka";
LPCWSTR AttributeKatakanaAnk = L"an";
LPCWSTR AttributeSpOp = L"so";
LPCWSTR AttributeLatin = L"la";
LPCWSTR AttributeJLatin = L"jl";

//dictionary section

LPCWSTR SectionDictionary = L"dictionary";

//server section

LPCWSTR SectionServer = L"server";

//server keys

LPCWSTR ValueServerServ = L"serv";
LPCWSTR ValueServerHost = L"host";
LPCWSTR ValueServerPort = L"port";
LPCWSTR ValueServerEncoding = L"encoding";
LPCWSTR ValueServerTimeOut = L"timeout";

//userdict section

LPCWSTR SectionUserDict = L"userdict";

//userdict keys

LPCWSTR ValueBackupDir = L"backupdir";
LPCWSTR ValueBackupGen = L"backupgen";
LPCWSTR ValuePrivateOnVKey = L"privateonvkey";
LPCWSTR ValuePrivateOnMKey = L"privateonmkey";
LPCWSTR ValuePrivateOffVKey = L"privateoffvkey";
LPCWSTR ValuePrivateOffMKey = L"privateoffmkey";
LPCWSTR ValuePrivateModeAuto = L"privatemodeauto";

//behavior section

LPCWSTR SectionBehavior = L"behavior";

//behavior keys

LPCWSTR ValueDefaultMode = L"defaultmode";
LPCWSTR ValueDefModeAscii = L"defmodeascii";
LPCWSTR ValueBeginCvOkuri = L"begincvokuri";
LPCWSTR ValuePrecedeOkuri = L"precedeokuri";
LPCWSTR ValueShiftNNOkuri = L"shiftnnokuri";
LPCWSTR ValueSrchAllOkuri = L"srchallokuri";
LPCWSTR ValueDelCvPosCncl = L"delcvposcncl";
LPCWSTR ValueDelOkuriCncl = L"delokuricncl";
LPCWSTR ValueBackIncEnter = L"backincenter";
LPCWSTR ValueAddCandKtkn = L"addcandktkn";
LPCWSTR ValueEnToggleKana = L"entogglekana";
LPCWSTR ValueSetByDirect = L"setbydirect";
LPCWSTR ValueFixMazeCount = L"fixmazecount";

LPCWSTR ValueCompMultiNum = L"compmultinum";
LPCWSTR ValueStaCompMulti = L"stacompmulti";
LPCWSTR ValueDynamicComp = L"dynamiccomp";
LPCWSTR ValueDynCompMulti = L"dyncompmulti";
LPCWSTR ValueCompUserDic = L"compuserdic";
LPCWSTR ValueCompIncBack = L"compincback";

//font section

LPCWSTR SectionFont = L"font";

//font keys

LPCWSTR ValueFontName = L"name";
LPCWSTR ValueFontSize = L"size";
LPCWSTR ValueFontWeight = L"weight";
LPCWSTR ValueFontItalic = L"italic";

//display section

LPCWSTR SectionDisplay = L"display";

//display keys

LPCWSTR ValueMaxWidth = L"maxwidth";
LPCWSTR ValueColorBG = L"colorbg";
LPCWSTR ValueColorFR = L"colorfr";
LPCWSTR ValueColorSE = L"colorse";
LPCWSTR ValueColorCO = L"colorco";
LPCWSTR ValueColorCA = L"colorca";
LPCWSTR ValueColorSC = L"colorsc";
LPCWSTR ValueColorAN = L"coloran";
LPCWSTR ValueColorNO = L"colorno";
LPCWSTR ValueDrawAPI = L"drawapi";
LPCWSTR ValueColorFont = L"colorfont";
LPCWSTR ValueUntilCandList = L"untilcandlist";
LPCWSTR ValueDispCandNo = L"dispcandno";
LPCWSTR ValueVerticalCand = L"verticalcand";
LPCWSTR ValueAnnotation = L"annotation";
LPCWSTR ValueAnnotatLst = L"annotatlst";
LPCWSTR ValueShowModeMark = L"showmodemark";
LPCWSTR ValueShowRoman = L"showroman";
LPCWSTR ValueShowRomanJLat = L"showromanjlat";
LPCWSTR ValueShowRomanComp = L"showromancomp";

LPCWSTR ValueShowModeInl = L"showmodeinl";
LPCWSTR ValueShowModeSec = L"showmodesec";	//for compatibility
LPCWSTR ValueShowModeInlTm = L"showmodeinltm";
LPCWSTR ValueColorMC = L"colormc";
LPCWSTR ValueColorMF = L"colormf";
LPCWSTR ValueColorHR = L"colorhr";
LPCWSTR ValueColorKT = L"colorkt";
LPCWSTR ValueColorKA = L"colorka";
LPCWSTR ValueColorJL = L"colorjl";
LPCWSTR ValueColorAC = L"colorac";
LPCWSTR ValueColorDR = L"colordr";
LPCWSTR ValueShowVkbd = L"showvkbd";
LPCWSTR ValueVkbdLayout = L"vkbdlayout";
LPCWSTR ValueVkbdTop = L"vkbdtop";
LPCWSTR ValueAutoHelp = L"autohelp";
LPCWSTR ValueAutoHelpOff = L"off";
LPCWSTR ValueAutoHelpOnKey = L"onkey";
LPCWSTR ValueAutoHelpOnConv = L"onconv";
LPCWSTR ValueShowHelp = L"showhelp";
LPCWSTR ValueShowHelpKansaku = L"kansaku";
LPCWSTR ValueShowHelpDotHyo = L"dothyo";
LPCWSTR ValueShowHelpKanjiHyo = L"kanjihyo";

//displayattr section

LPCWSTR SectionDisplayAttr = L"displayattr";

//displayattr keys

LPCWSTR ValueDisplayAttrInputMark = L"inputmark";
LPCWSTR ValueDisplayAttrInputText = L"inputtext";
LPCWSTR ValueDisplayAttrInputOkuri = L"inputokuri";
LPCWSTR ValueDisplayAttrConvMark = L"convmark";
LPCWSTR ValueDisplayAttrConvText = L"convtext";
LPCWSTR ValueDisplayAttrConvOkuri = L"convokuri";
LPCWSTR ValueDisplayAttrConvAnnot = L"convannot";

//selkey section

LPCWSTR SectionSelKey = L"selkey";

//preservedkey section

LPCWSTR SectionPreservedKey = L"preservedkey";	//for compatibility
LPCWSTR SectionPreservedKeyON = L"preservedkeyon";
LPCWSTR SectionPreservedKeyOFF = L"preservedkeyoff";

//keymap section

LPCWSTR SectionKeyMap = L"keymap";

//vkeymap section

LPCWSTR SectionVKeyMap = L"vkeymap";

//keymap and vkeymap keys

LPCWSTR ValueKeyMapKana = L"kana";
LPCWSTR ValueKeyMapConvChar = L"convchar";
LPCWSTR ValueKeyMapJLatin = L"jlatin";
LPCWSTR ValueKeyMapAscii = L"ascii";
LPCWSTR ValueKeyMapJMode = L"jmode";
LPCWSTR ValueKeyMapAbbrev = L"abbrev";
LPCWSTR ValueKeyMapAffix = L"affix";
LPCWSTR ValueKeyMapNextCand = L"nextcand";
LPCWSTR ValueKeyMapPrevCand = L"prevcand";
LPCWSTR ValueKeyMapPurgeDic = L"purgedic";
LPCWSTR ValueKeyMapNextComp = L"nextcomp";
LPCWSTR ValueKeyMapPrevComp = L"prevcomp";
LPCWSTR ValueKeyMapCompCand = L"compcand";
LPCWSTR ValueKeyMapHint = L"hint";
LPCWSTR ValueKeyMapConvPoint = L"convpoint";
LPCWSTR ValueKeyMapDirect = L"direct";
LPCWSTR ValueKeyMapEnter = L"enter";
LPCWSTR ValueKeyMapCancel = L"cancel";
LPCWSTR ValueKeyMapBack = L"back";
LPCWSTR ValueKeyMapDelete = L"delete";
LPCWSTR ValueKeyMapVoid = L"void";
LPCWSTR ValueKeyMapLeft = L"left";
LPCWSTR ValueKeyMapUp = L"up";
LPCWSTR ValueKeyMapRight = L"right";
LPCWSTR ValueKeyMapDown = L"down";
LPCWSTR ValueKeyMapPaste = L"paste";
LPCWSTR ValueKeyMapOtherIme = L"otherime";
LPCWSTR ValueKeyMapViEsc = L"viesc";

//convpoint section

LPCWSTR SectionConvPoint = L"convpoint";

//kana section

LPCWSTR SectionKana = L"kana";

//jlatin section

LPCWSTR SectionJLatin = L"jlatin";



HRESULT CreateStreamReader(LPCWSTR path, IXmlReader **ppReader, IStream **ppFileStream)
{
	HRESULT hr = S_FALSE;

	if (ppReader != nullptr && ppFileStream != nullptr)
	{
		hr = SHCreateStreamOnFileW(path, STGM_READ, ppFileStream);
		if (*ppFileStream == nullptr) hr = E_FAIL;
		EXIT_FAILED(hr);

		hr = CreateXmlReader(IID_PPV_ARGS(ppReader), nullptr);
		if (*ppReader == nullptr) hr = E_FAIL;
		EXIT_FAILED(hr);

		hr = (*ppReader)->SetInput(*ppFileStream);
		EXIT_FAILED(hr);
	}

L_FAILED:
	return hr;
}

HRESULT ReadList(LPCWSTR path, LPCWSTR section, APPDATAXMLLIST &list)
{
	HRESULT hr;
	CComPtr<IXmlReader> pReader;
	CComPtr<IStream> pFileStream;
	XmlNodeType nodeType;
	LPCWSTR pwszLocalName;
	LPCWSTR pwszAttributeName;
	LPCWSTR pwszAttributeValue;
	int sequence = 0;

	APPDATAXMLATTR attr;
	APPDATAXMLROW row;

	hr = CreateStreamReader(path, &pReader, &pFileStream);
	EXIT_FAILED(hr);

	while (pReader->Read(&nodeType) == S_OK)
	{
		switch (nodeType)
		{
		case XmlNodeType_Element:
			pwszLocalName = nullptr;
			hr = pReader->GetLocalName(&pwszLocalName, nullptr);
			if (pwszLocalName == nullptr) hr = E_FAIL;
			EXIT_FAILED(hr);

			switch (sequence)
			{
			case 0:
				if (wcscmp(TagRoot, pwszLocalName) == 0)
				{
					sequence = 1;
				}
				break;
			case 1:
				if (wcscmp(TagSection, pwszLocalName) == 0)
				{
					sequence = 2;
				}
				break;
			case 3:
				if (wcscmp(TagList, pwszLocalName) == 0)
				{
					sequence = 4;
				}
				break;
			case 4:
				if (wcscmp(TagRow, pwszLocalName) == 0)
				{
					sequence = 5;
					row.clear();
				}
				break;
			default:
				break;
			}

			for (hr = pReader->MoveToFirstAttribute(); hr == S_OK; hr = pReader->MoveToNextAttribute())
			{
				pwszAttributeName = nullptr;
				hr = pReader->GetLocalName(&pwszAttributeName, nullptr);
				if (pwszAttributeName == nullptr) hr = E_FAIL;
				EXIT_FAILED(hr);

				pwszAttributeValue = nullptr;
				hr = pReader->GetValue(&pwszAttributeValue, nullptr);
				if (pwszAttributeValue == nullptr) hr = E_FAIL;
				EXIT_FAILED(hr);

				switch (sequence)
				{
				case 2:
					if (wcscmp(TagSection, pwszLocalName) == 0 &&
						wcscmp(AttributeName, pwszAttributeName) == 0 && wcscmp(section, pwszAttributeValue) == 0)
					{
						sequence = 3;
					}
					break;
				case 5:
					if (wcscmp(TagRow, pwszLocalName) == 0)
					{
						attr.first = pwszAttributeName;
						attr.second = pwszAttributeValue;
						row.push_back(attr);
					}
					break;
				default:
					break;
				}
			}

			switch (sequence)
			{
			case 2:
				sequence = 1;
				break;
			case 5:
				list.push_back(row);
				row.clear();
				break;
			default:
				break;
			}
			break;

		case XmlNodeType_EndElement:
			pwszLocalName = nullptr;
			hr = pReader->GetLocalName(&pwszLocalName, nullptr);
			if (pwszLocalName == nullptr) hr = E_FAIL;
			EXIT_FAILED(hr);

			switch (sequence)
			{
			case 1:
				if (wcscmp(TagRoot, pwszLocalName) == 0)
				{
					goto L_EXIT;
				}
				break;
			case 2:
				if (wcscmp(TagSection, pwszLocalName) == 0)
				{
					sequence = 1;
				}
				break;
			case 3:
				if (wcscmp(TagSection, pwszLocalName) == 0)
				{
					goto L_EXIT;
				}
				break;
			case 4:
				if (wcscmp(TagSection, pwszLocalName) == 0 || wcscmp(TagList, pwszLocalName) == 0)
				{
					goto L_EXIT;
				}
				break;
			case 5:
				if (wcscmp(TagList, pwszLocalName) == 0)
				{
					goto L_EXIT;
				}
				break;
			default:
				break;
			}
			break;

		default:
			break;
		}
	}

L_FAILED:
L_EXIT:
	return hr;
}

HRESULT ReadValue(LPCWSTR path, LPCWSTR section, LPCWSTR key, std::wstring &strxmlval, LPCWSTR defval)
{
	CComPtr<IXmlReader> pReader;
	CComPtr<IStream> pFileStream;
	HRESULT hr;
	XmlNodeType nodeType;
	LPCWSTR pwszLocalName;
	LPCWSTR pwszAttributeName;
	LPCWSTR pwszAttributeValue;
	int sequence = 0;

	strxmlval = defval;

	hr = CreateStreamReader(path, &pReader, &pFileStream);
	EXIT_FAILED(hr);

	while (pReader->Read(&nodeType) == S_OK)
	{
		switch (nodeType)
		{
		case XmlNodeType_Element:
			pwszLocalName = nullptr;
			hr = pReader->GetLocalName(&pwszLocalName, nullptr);
			if (pwszLocalName == nullptr) hr = E_FAIL;
			EXIT_FAILED(hr);

			switch (sequence)
			{
			case 0:
				if (wcscmp(TagRoot, pwszLocalName) == 0)
				{
					sequence = 1;
				}
				break;
			case 1:
				if (wcscmp(TagSection, pwszLocalName) == 0)
				{
					sequence = 2;
				}
				break;
			case 3:
				if (wcscmp(TagKey, pwszLocalName) == 0)
				{
					sequence = 4;
				}
				break;
			default:
				break;
			}

			for (hr = pReader->MoveToFirstAttribute(); hr == S_OK; hr = pReader->MoveToNextAttribute())
			{
				pwszAttributeName = nullptr;
				hr = pReader->GetLocalName(&pwszAttributeName, nullptr);
				if (pwszAttributeName == nullptr) hr = E_FAIL;
				EXIT_FAILED(hr);

				pwszAttributeValue = nullptr;
				hr = pReader->GetValue(&pwszAttributeValue, nullptr);
				if (pwszAttributeValue == nullptr) hr = E_FAIL;
				EXIT_FAILED(hr);

				switch (sequence)
				{
				case 2:
					if (wcscmp(AttributeName, pwszAttributeName) == 0 && wcscmp(section, pwszAttributeValue) == 0)
					{
						sequence = 3;
					}
					break;
				case 4:
					if (wcscmp(AttributeName, pwszAttributeName) == 0 && wcscmp(key, pwszAttributeValue) == 0)
					{
						sequence = 5;
					}
					break;
				case 5:
					if (wcscmp(AttributeValue, pwszAttributeName) == 0)
					{
						strxmlval.assign(pwszAttributeValue);
						goto L_EXIT;
					}
					break;
				default:
					break;
				}
			}
			break;

		case XmlNodeType_EndElement:
			pwszLocalName = nullptr;
			hr = pReader->GetLocalName(&pwszLocalName, nullptr);
			if (pwszLocalName == nullptr) hr = E_FAIL;
			EXIT_FAILED(hr);

			switch (sequence)
			{
			case 1:
				if (wcscmp(TagRoot, pwszLocalName) == 0)
				{
					goto L_EXIT;
				}
				break;
			case 2:
				if (wcscmp(TagSection, pwszLocalName) == 0)
				{
					sequence = 1;
				}
				break;
			case 3:
			case 4:
			case 5:
				if (wcscmp(TagSection, pwszLocalName) == 0)
				{
					goto L_EXIT;
				}
				break;
			default:
				break;
			}
			break;

		default:
			break;
		}
	}

L_FAILED:
L_EXIT:
	return hr;
}

HRESULT CreateStreamWriter(LPCWSTR path, IXmlWriter **ppWriter, IStream **ppFileStream)
{
	HRESULT hr = S_FALSE;

	if (ppWriter != nullptr && ppFileStream != nullptr)
	{
		hr = SHCreateStreamOnFileW(path, STGM_WRITE | STGM_CREATE, ppFileStream);
		if (*ppFileStream == nullptr) hr = E_FAIL;
		EXIT_FAILED(hr);

		hr = CreateXmlWriter(IID_PPV_ARGS(ppWriter), nullptr);
		if (*ppWriter == nullptr) hr = E_FAIL;
		EXIT_FAILED(hr);

		hr = (*ppWriter)->SetOutput(*ppFileStream);
		EXIT_FAILED(hr);
	}

L_FAILED:
	return hr;
}

HRESULT WriterInit(LPCWSTR path, IXmlWriter **ppWriter, IStream **pFileStream, BOOL indent)
{
	HRESULT hr = S_FALSE;

	if (ppWriter != nullptr && pFileStream != nullptr)
	{
		hr = CreateStreamWriter(path, ppWriter, pFileStream);
		EXIT_FAILED(hr);

		hr = (*ppWriter)->SetProperty(XmlWriterProperty_Indent, indent);
		EXIT_FAILED(hr);

		hr = (*ppWriter)->WriteStartDocument(XmlStandalone_Omit);
		EXIT_FAILED(hr);
	}

L_FAILED:
	return hr;
}

HRESULT WriterFinal(IXmlWriter *pWriter)
{
	HRESULT hr = S_FALSE;

	if (pWriter != nullptr)
	{
		hr = pWriter->WriteEndDocument();
		EXIT_FAILED(hr);

		hr = pWriter->Flush();
		EXIT_FAILED(hr);
	}

L_FAILED:
	return hr;
}

HRESULT WriterNewLine(IXmlWriter *pWriter)
{
	HRESULT hr = S_FALSE;

	if (pWriter != nullptr)
	{
		hr = pWriter->WriteRaw(L"\r\n");
	}

	return hr;
}

HRESULT WriterStartElement(IXmlWriter *pWriter, LPCWSTR element)
{
	HRESULT hr = S_FALSE;

	if (pWriter != nullptr)
	{
		hr = pWriter->WriteStartElement(nullptr, element, nullptr);
	}

	return hr;
}

HRESULT WriterEndElement(IXmlWriter *pWriter)
{
	HRESULT hr = S_FALSE;

	if (pWriter != nullptr)
	{
		hr = pWriter->WriteEndElement();
	}

	return hr;
}

HRESULT WriterAttribute(IXmlWriter *pWriter, LPCWSTR name, LPCWSTR value)
{
	HRESULT hr = S_FALSE;

	if (pWriter != nullptr)
	{
		hr = pWriter->WriteAttributeString(nullptr, name, nullptr, value);
	}

	return hr;
}

HRESULT WriterStartSection(IXmlWriter *pWriter, LPCWSTR name)
{
	HRESULT hr = S_FALSE;

	if (pWriter != nullptr)
	{
		hr = WriterStartElement(pWriter, TagSection);
		EXIT_FAILED(hr);

		hr = WriterAttribute(pWriter, AttributeName, name);
		EXIT_FAILED(hr);
	}

L_FAILED:
	return hr;
}

HRESULT WriterEndSection(IXmlWriter *pWriter)
{
	return WriterEndElement(pWriter);
}

HRESULT WriterKey(IXmlWriter *pWriter, LPCWSTR key, LPCWSTR value)
{
	HRESULT hr = S_FALSE;

	if (pWriter != nullptr)
	{
		hr = WriterStartElement(pWriter, TagKey);
		EXIT_FAILED(hr);

		hr = WriterAttribute(pWriter, AttributeName, key);
		EXIT_FAILED(hr);

		hr = WriterAttribute(pWriter, AttributeValue, value);
		EXIT_FAILED(hr);

		hr = WriterEndElement(pWriter);	//key
		EXIT_FAILED(hr);
	}

L_FAILED:
	return hr;
}

HRESULT WriterRow(IXmlWriter *pWriter, const APPDATAXMLROW &row)
{
	HRESULT hr = S_FALSE;

	if (pWriter != nullptr)
	{
		FORWARD_ITERATION_I(r_itr, row)
		{
			hr = WriterAttribute(pWriter, r_itr->first.c_str(), r_itr->second.c_str());
			EXIT_FAILED(hr);
		}
	}

L_FAILED:
	return hr;
}

HRESULT WriterList(IXmlWriter *pWriter, const APPDATAXMLLIST &list, BOOL newline)
{
	HRESULT hr = S_FALSE;

	if (pWriter != nullptr)
	{
		hr = WriterStartElement(pWriter, TagList);
		EXIT_FAILED(hr);

		if (newline)
		{
			hr = WriterNewLine(pWriter);
			EXIT_FAILED(hr);
		}

		FORWARD_ITERATION_I(l_itr, list)
		{
			hr = WriterStartElement(pWriter, TagRow);
			EXIT_FAILED(hr);

			hr = WriterRow(pWriter, *l_itr);
			EXIT_FAILED(hr);

			hr = WriterEndElement(pWriter);	//row
			EXIT_FAILED(hr);

			if (newline)
			{
				hr = WriterNewLine(pWriter);
				EXIT_FAILED(hr);
			}
		}

		hr = WriterEndElement(pWriter);	//list
		EXIT_FAILED(hr);
	}

L_FAILED:
	return hr;
}
