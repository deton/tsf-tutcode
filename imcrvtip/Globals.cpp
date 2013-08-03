
#include "imcrvtip.h"

HINSTANCE g_hInst;

LPCWSTR TextServiceDesc = TEXTSERVICE_DESC;
LPCWSTR LangbarItemDesc = L"ver. " TEXTSERVICE_VER;
LPCWSTR LangbarFuncDesc = TEXTSERVICE_DESC L" " TEXTSERVICE_VER;

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

// {62B9900F-263C-4EA3-9B8B-C9796F6E903A}
const GUID c_guidDisplayAttributeInput = 
{ 0x62b9900f, 0x263c, 0x4ea3, { 0x9b, 0x8b, 0xc9, 0x79, 0x6f, 0x6e, 0x90, 0x3a } };

// {A65CB46F-2FDC-44A3-B215-F02A72305C7F}
const GUID c_guidDisplayAttributeCandidate = 
{ 0xa65cb46f, 0x2fdc, 0x44a3, { 0xb2, 0x15, 0xf0, 0x2a, 0x72, 0x30, 0x5c, 0x7f } };

// {D003A448-7318-426A-A8BF-2F0B3D86738E}
const GUID c_guidDisplayAttributeAnnotation = 
{ 0xd003a448, 0x7318, 0x426a, { 0xa8, 0xbf, 0x2f, 0xb, 0x3d, 0x86, 0x73, 0x8e } };

// {2090B43D-E245-4BC9-A12E-D2005078ACFC}
const GUID c_guidCandidateListUIElement = 
{ 0x2090b43d, 0xe245, 0x4bc9, { 0xa1, 0x2e, 0xd2, 0x0, 0x50, 0x78, 0xac, 0xfc } };

#else

// {4D97960C-1D59-4466-BEFE-4C1328D2550D}
const GUID c_clsidTextService = 
{ 0x4d97960c, 0x1d59, 0x4466, { 0xbe, 0xfe, 0x4c, 0x13, 0x28, 0xd2, 0x55, 0x0d } };

// {820E9894-024B-4bd1-98AF-3942B772CFF1}
const GUID c_guidProfile = 
{ 0x820e9894, 0x024b, 0x4bd1, { 0x98, 0xaf, 0x39, 0x42, 0xb7, 0x72, 0xcf, 0xf1 } };

// {D1930150-790A-437b-88B5-EB3E9FB9165F}
const GUID c_guidPreservedKeyOnOff = 
{ 0xd1930150, 0x790a, 0x437b, { 0x88, 0xb5, 0xeb, 0x3e, 0x9f, 0xb9, 0x16, 0x5f } };

// {F4BF0D3C-D4CE-456f-837E-FE6712C6A8C3}
const GUID c_guidLangBarItemButton = 
{ 0xf4bf0d3c, 0xd4ce, 0x456f, { 0x83, 0x7e, 0xfe, 0x67, 0x12, 0xc6, 0xa8, 0xc3 } };

// {6F99E3F1-36AC-4015-B334-211CFFCB3262}
const GUID c_guidDisplayAttributeInput = 
{ 0x6f99e3f1, 0x36ac, 0x4015, { 0xb3, 0x34, 0x21, 0x1c, 0xff, 0xcb, 0x32, 0x62 } };

// {6877D302-1C51-4ba4-9329-2F80B5E3A4E7}
const GUID c_guidDisplayAttributeCandidate = 
{ 0x6877d302, 0x1c51, 0x4ba4, { 0x93, 0x29, 0x2f, 0x80, 0xb5, 0xe3, 0xa4, 0xe7 } };

// {C6040719-6FF3-4b92-A589-36E93BFD53EC}
const GUID c_guidDisplayAttributeAnnotation = 
{ 0xc6040719, 0x6ff3, 0x4b92, { 0xa5, 0x89, 0x36, 0xe9, 0x3b, 0xfd, 0x53, 0xec } };

// {25A6388F-D3CB-4866-A2C3-94E00970BF45}
const GUID c_guidCandidateListUIElement = 
{ 0x25a6388f, 0xd3cb, 0x4866, { 0xa2, 0xc3, 0x94, 0xe0, 0x09, 0x70, 0xbf, 0x45 } };

#endif

const TF_DISPLAYATTRIBUTE c_daDisplayAttributeInput =
{
	{TF_CT_NONE, 0},			// TF_DA_COLOR crText;
	{TF_CT_NONE, 0},			// TF_DA_COLOR crBk;
	TF_LS_DOT,					// TF_DA_LINESTYLE lsStyle;
	FALSE,						// BOOL fBoldLine;
	{ TF_CT_NONE, 0},			// TF_DA_COLOR crLine;
	TF_ATTR_INPUT				// TF_DA_ATTR_INFO bAttr;
};

const TF_DISPLAYATTRIBUTE c_daDisplayAttributeCandidate =
{
	{TF_CT_NONE, 0},			// TF_DA_COLOR crText;
	{TF_CT_NONE, 0},			// TF_DA_COLOR crBk;
	TF_LS_SOLID,				// TF_DA_LINESTYLE lsStyle;
	FALSE,						// BOOL fBoldLine;
	{TF_CT_NONE, 0},			// TF_DA_COLOR crLine;
	TF_ATTR_TARGET_CONVERTED	// TF_DA_ATTR_INFO bAttr;
};

const TF_DISPLAYATTRIBUTE c_daDisplayAttributeAnnotation =
{
	{TF_CT_NONE, 0},			// TF_DA_COLOR crText;
	{TF_CT_NONE, 0},			// TF_DA_COLOR crBk;
	TF_LS_DASH,					// TF_DA_LINESTYLE lsStyle;
	FALSE,						// BOOL fBoldLine;
	{TF_CT_NONE, 0},			// TF_DA_COLOR crLine;
	TF_ATTR_CONVERTED			// TF_DA_ATTR_INFO bAttr;
};
