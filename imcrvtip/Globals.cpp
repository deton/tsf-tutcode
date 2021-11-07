
#include "configxml.h"
#include "imcrvtip.h"

HINSTANCE g_hInst;

LPCWSTR TextServiceDesc = TEXTSERVICE_DESC;
LPCWSTR LangbarItemDesc = L"ver. " TEXTSERVICE_VER;
LPCWSTR CandidateWindowClass = TEXTSERVICE_NAME L"CandidateWindow";
LPCWSTR InputModeWindowClass = TEXTSERVICE_NAME L"InputModeWindow";
LPCWSTR VKeyboardWindowClass = TEXTSERVICE_NAME L"VKeyboardWindow";

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

// {3ACDEE0E-0ACE-4343-82A5-989A6785BD37}
const GUID c_guidPrivateModeKeyOn = 
{ 0x3acdee0e, 0xace, 0x4343, { 0x82, 0xa5, 0x98, 0x9a, 0x67, 0x85, 0xbd, 0x37 } };

// {D187A32B-2581-450F-9071-0A7608E3AF05}
const GUID c_guidPrivateModeKeyOff = 
{ 0xd187a32b, 0x2581, 0x450f, { 0x90, 0x71, 0xa, 0x76, 0x8, 0xe3, 0xaf, 0x5 } };

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

// {DC9E1E45-6818-4108-AC41-3A72EB7068F4}
const GUID c_guidPrivateModeKeyOn =
{0xdc9e1e45, 0x6818, 0x4108, {0xac, 0x41, 0x3a, 0x72, 0xeb, 0x70, 0x68, 0xf4}};

// {42D1356F-A252-488A-80F5-29244E830216}
const GUID c_guidPrivateModeKeyOff =
{0x42d1356f, 0xa252, 0x488a, { 0x80, 0xf5, 0x29, 0x24, 0x4e, 0x83, 0x2, 0x16 }};

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
LPCWSTR markCursor = L"|";
LPCWSTR markSqbL = L"[";
LPCWSTR markSqbR = L"]";
LPCWSTR markSP = L"\x20";
LPCWSTR markNBSP = L"\u00A0";
LPCWSTR markHM = L"\x2D";
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
