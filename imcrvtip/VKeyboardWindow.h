﻿
#pragma once

#include "TextService.h"
#include "EditSession.h"

class CVKeyboardWindow : public ITfTextLayoutSink
{
public:
	CVKeyboardWindow();
	~CVKeyboardWindow();

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObj);
	STDMETHODIMP_(ULONG) AddRef(void);
	STDMETHODIMP_(ULONG) Release(void);

	// ITfTextLayoutSink
	STDMETHODIMP OnLayoutChange(ITfContext *pContext, TfLayoutCode lcode, ITfContextView *pContextView);

	BOOL _Create(CTextService *pTextService, ITfContext *pContext, BOOL bCandidateWindow, HWND hWnd);
	void _Destroy();
	static BOOL _InitClass();
	static void _UninitClass();
	static LRESULT CALLBACK _WindowPreProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK _WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void _Move(int x, int y);
	void _Show(BOOL bShow);
	void _Redraw();
	void _GetRect(LPRECT lpRect);
	void _CalcWindowRect(LPRECT lpRect);
	BOOL _IsHide() { return _bHide; }

private:
	LONG _cRef;

	HRESULT _AdviseTextLayoutSink();
	HRESULT _UnadviseTextLayoutSink();

	void _WindowProcPaint(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void _InitFont();
	void _UninitFont();

	ITfContext *_pContext;

	DWORD _dwCookieTextLayoutSink;

	CTextService *_pTextService;
	HWND _hwndParent;
	HWND _hwnd;
	HFONT hFont;
	int _dpi;

	std::wstring _vkb;
	LONG _fontHeight;
	BOOL _bHide;
};
