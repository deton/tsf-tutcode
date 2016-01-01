
#include <initguid.h>

#include "configxml.h"
#include "imcrvtip.h"

HINSTANCE g_hInst;

LPCWSTR TextServiceDesc = TEXTSERVICE_DESC;
LPCWSTR LangbarItemDesc = L"ver. " TEXTSERVICE_VER;
LPCWSTR CandidateWindowClass = TEXTSERVICE_NAME L"CandidateWindow";
LPCWSTR InputModeWindowClass = TEXTSERVICE_NAME L"InputModeWindow";

#ifndef _DEBUG

// {7EFCBAE8-A0EF-4156-BB52-C6FE737BC2D7}
const GUID c_clsidTextService = 
{ 0x7efcbae8, 0xa0ef, 0x4156, { 0xbb, 0x52, 0xc6, 0xfe, 0x73, 0x7b, 0xc2, 0xd7 } };

// {30689481-5946-448C-870F-DCFE17B9BF71}
const GUID c_guidProfile = 
{ 0x30689481, 0x5946, 0x448c, { 0x87, 0xf, 0xdc, 0xfe, 0x17, 0xb9, 0xbf, 0x71 } };

// {19AC53F3-32DF-41B7-800A-37039CEDD9B9}
const GUID c_guidPreservedKeyOn = 
{ 0x19ac53f3, 0x32df, 0x41b7, { 0x80, 0xa, 0x37, 0x3, 0x9c, 0xed, 0xd9, 0xb9 } };

// {494B00AC-16D0-4D34-BFD8-7E885B4117DE}
const GUID c_guidPreservedKeyOff = 
{ 0x494b00ac, 0x16d0, 0x4d34, { 0xbf, 0xd8, 0x7e, 0x88, 0x5b, 0x41, 0x17, 0xde } };

// {88334C7C-D702-42D2-8FCF-AA58D52A05D4}
const GUID c_guidPreservedKeyOnOff = 
{ 0x88334c7c, 0xd702, 0x42d2, { 0x8f, 0xcf, 0xaa, 0x58, 0xd5, 0x2a, 0x5, 0xd4 } };

// {0C105AF2-E6F5-4178-9452-FBC927013029}
const GUID c_guidLangBarItemButton = 
{ 0xc105af2, 0xe6f5, 0x4178, { 0x94, 0x52, 0xfb, 0xc9, 0x27, 0x1, 0x30, 0x29 } };

// {2090B43D-E245-4BC9-A12E-D2005078ACFC}
const GUID c_guidCandidateListUIElement = 
{ 0x2090b43d, 0xe245, 0x4bc9, { 0xa1, 0x2e, 0xd2, 0x0, 0x50, 0x78, 0xac, 0xfc } };

// {E4AF1C77-6D3E-4A3C-B2B7-3ACF7E40AB2C}
const GUID c_guidDisplayAttributeInputMark = 
{ 0xe4af1c77, 0x6d3e, 0x4a3c, { 0xb2, 0xb7, 0x3a, 0xcf, 0x7e, 0x40, 0xab, 0x2c } };

// {62B9900F-263C-4EA3-9B8B-C9796F6E903A}
const GUID c_guidDisplayAttributeInputText = 
{ 0x62b9900f, 0x263c, 0x4ea3, { 0x9b, 0x8b, 0xc9, 0x79, 0x6f, 0x6e, 0x90, 0x3a } };

// {DCDB6260-4352-473A-B64E-EE143914DBE0}
const GUID c_guidDisplayAttributeInputOkuri = 
{ 0xdcdb6260, 0x4352, 0x473a, { 0xb6, 0x4e, 0xee, 0x14, 0x39, 0x14, 0xdb, 0xe0 } };

// {187B4EF1-CC96-4BC7-83F7-BCA5AF1571DD}
const GUID c_guidDisplayAttributeConvMark = 
{ 0x187b4ef1, 0xcc96, 0x4bc7, { 0x83, 0xf7, 0xbc, 0xa5, 0xaf, 0x15, 0x71, 0xdd } };

// {A65CB46F-2FDC-44A3-B215-F02A72305C7F}
const GUID c_guidDisplayAttributeConvText = 
{ 0xa65cb46f, 0x2fdc, 0x44a3, { 0xb2, 0x15, 0xf0, 0x2a, 0x72, 0x30, 0x5c, 0x7f } };

// {59178231-5AB8-4F2F-9B64-48D470B2DC79}
const GUID c_guidDisplayAttributeConvOkuri = 
{ 0x59178231, 0x5ab8, 0x4f2f, { 0x9b, 0x64, 0x48, 0xd4, 0x70, 0xb2, 0xdc, 0x79 } };

// {D003A448-7318-426A-A8BF-2F0B3D86738E}
const GUID c_guidDisplayAttributeConvAnnot = 
{ 0xd003a448, 0x7318, 0x426a, { 0xa8, 0xbf, 0x2f, 0xb, 0x3d, 0x86, 0x73, 0x8e } };

#else

// {4D97960C-1D59-4466-BEFE-4C1328D2550D}
const GUID c_clsidTextService =
{0x4d97960c, 0x1d59, 0x4466, {0xbe, 0xfe, 0x4c, 0x13, 0x28, 0xd2, 0x55, 0x0d}};

// {820E9894-024B-4bd1-98AF-3942B772CFF1}
const GUID c_guidProfile =
{0x820e9894, 0x024b, 0x4bd1, {0x98, 0xaf, 0x39, 0x42, 0xb7, 0x72, 0xcf, 0xf1}};

// {CFDFABF4-892E-483A-865E-C00451417C1C}
const GUID c_guidPreservedKeyOn = 
{ 0xcfdfabf4, 0x892e, 0x483a, { 0x86, 0x5e, 0xc0, 0x4, 0x51, 0x41, 0x7c, 0x1c } };

// {58B06D02-56CE-4F45-B45C-1B6B0323A090}
const GUID c_guidPreservedKeyOff = 
{ 0x58b06d02, 0x56ce, 0x4f45, { 0xb4, 0x5c, 0x1b, 0x6b, 0x3, 0x23, 0xa0, 0x90 } };

// {D1930150-790A-437b-88B5-EB3E9FB9165F}
const GUID c_guidPreservedKeyOnOff =
{0xd1930150, 0x790a, 0x437b, {0x88, 0xb5, 0xeb, 0x3e, 0x9f, 0xb9, 0x16, 0x5f}};

// {F4BF0D3C-D4CE-456f-837E-FE6712C6A8C3}
const GUID c_guidLangBarItemButton =
{0xf4bf0d3c, 0xd4ce, 0x456f, {0x83, 0x7e, 0xfe, 0x67, 0x12, 0xc6, 0xa8, 0xc3}};

// {25A6388F-D3CB-4866-A2C3-94E00970BF45}
const GUID c_guidCandidateListUIElement =
{0x25a6388f, 0xd3cb, 0x4866, {0xa2, 0xc3, 0x94, 0xe0, 0x09, 0x70, 0xbf, 0x45}};

// {CB22C53A-AD57-485A-A6CF-20390A0D5098}
const GUID c_guidDisplayAttributeInputMark =
{0xcb22c53a, 0xad57, 0x485a, {0xa6, 0xcf, 0x20, 0x39, 0x0a, 0x0d, 0x50, 0x98}};

// {6F99E3F1-36AC-4015-B334-211CFFCB3262}
const GUID c_guidDisplayAttributeInputText =
{0x6f99e3f1, 0x36ac, 0x4015, {0xb3, 0x34, 0x21, 0x1c, 0xff, 0xcb, 0x32, 0x62}};

// {D2176C6C-8758-40C6-9612-5832FA315879}
const GUID c_guidDisplayAttributeInputOkuri =
{0xd2176c6c, 0x8758, 0x40c6, {0x96, 0x12, 0x58, 0x32, 0xfa, 0x31, 0x58, 0x79}};

// {B564E740-166D-45B1-AF44-7CCC7F75A807}
const GUID c_guidDisplayAttributeConvMark =
{0xb564e740, 0x166d, 0x45b1, {0xaf, 0x44, 0x7c, 0xcc, 0x7f, 0x75, 0xa8, 0x07}};

// {6877D302-1C51-4ba4-9329-2F80B5E3A4E7}
const GUID c_guidDisplayAttributeConvText =
{0x6877d302, 0x1c51, 0x4ba4, {0x93, 0x29, 0x2f, 0x80, 0xb5, 0xe3, 0xa4, 0xe7}};

// {F99304F1-9F91-439E-8446-6FE0F8A98EDC}
const GUID c_guidDisplayAttributeConvOkuri =
{0xf99304f1, 0x9f91, 0x439e, {0x84, 0x46, 0x6f, 0xe0, 0xf8, 0xa9, 0x8e, 0xdc}};

// {C6040719-6FF3-4b92-A589-36E93BFD53EC}
const GUID c_guidDisplayAttributeConvAnnot =
{0xc6040719, 0x6ff3, 0x4b92, {0xa5, 0x89, 0x36, 0xe9, 0x3b, 0xfd, 0x53, 0xec}};

#endif

LPCWSTR markNo = L":";
LPCWSTR markAnnotation = L";";
LPCWSTR markCandEnd = L"\u3000";
LPCWSTR markCursor = L"|";
LPCWSTR markReg = L"登録";
LPCWSTR markRegL = L"[";
LPCWSTR markRegR = L"]";
LPCWSTR markRegKeyEnd = L"：";
LPCWSTR markSP = L"\x20";
LPCWSTR markNBSP = L"\u00A0";
LPCWSTR markMidashi = L"▽";
LPCWSTR markHenkan = L"▼";
LPCWSTR markOkuri = L"*";

const DISPLAYATTRIBUTE_INFO c_gdDisplayAttributeInfo[DISPLAYATTRIBUTE_INFO_NUM] =
{
	{
		ValueDisplayAttrInputMark, c_guidDisplayAttributeInputMark,
		c_daDisplayAttributeSeries[0], c_daDisplayAttributeInputMark
	},
	{
		ValueDisplayAttrInputText, c_guidDisplayAttributeInputText,
		c_daDisplayAttributeSeries[1], c_daDisplayAttributeInputText
	},
	{
		ValueDisplayAttrInputOkuri, c_guidDisplayAttributeInputOkuri,
		c_daDisplayAttributeSeries[2], c_daDisplayAttributeInputOkuri
	},
	{
		ValueDisplayAttrConvMark, c_guidDisplayAttributeConvMark,
		c_daDisplayAttributeSeries[3], c_daDisplayAttributeConvMark
	},
	{
		ValueDisplayAttrConvText, c_guidDisplayAttributeConvText,
		c_daDisplayAttributeSeries[4], c_daDisplayAttributeConvText
	},
	{
		ValueDisplayAttrConvOkuri, c_guidDisplayAttributeConvOkuri,
		c_daDisplayAttributeSeries[5], c_daDisplayAttributeConvOkuri
	},
	{
		ValueDisplayAttrConvAnnot, c_guidDisplayAttributeConvAnnot,
		c_daDisplayAttributeSeries[6], c_daDisplayAttributeConvAnnot
	}
};

// added in Windows 8 SDK
#ifndef _WIN32_WINNT_WIN8

const GUID GUID_TFCAT_TIPCAP_IMMERSIVESUPPORT =
{0x13A016DF, 0x560B, 0x46CD, {0x94, 0x7A, 0x4C, 0x3A, 0xF1, 0xE0, 0xE3, 0x5D}};

const GUID GUID_TFCAT_TIPCAP_SYSTRAYSUPPORT =
{0x25504FB4, 0x7BAB, 0x4BC1, {0x9C, 0x69, 0xCF, 0x81, 0x89, 0x0F, 0x0E, 0xF5}};

const GUID GUID_LBI_INPUTMODE =
{0x2C77A81E, 0x41CC, 0x4178, {0xA3, 0xA7, 0x5F, 0x8A, 0x98, 0x75, 0x68, 0xE6}};

const IID IID_ITfFnGetPreferredTouchKeyboardLayout =
{0x5F309A41, 0x590A, 0x4ACC, {0xA9, 0x7F, 0xD8, 0xEF, 0xFF, 0x13, 0xFD, 0xFC}};

#endif //_WIN32_WINNT_WIN8
