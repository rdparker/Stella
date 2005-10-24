//============================================================================
//
//   SSSS    tt          lll  lll
//  SS  SS   tt           ll   ll
//  SS     tttttt  eeee   ll   ll   aaaa
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2005 by Bradford W. Mott and the Stella team
//
// See the file "license" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// Windows CE Port by Kostas Nakos
//============================================================================

#include <windows.h>
#include "EventHandler.hxx"
#include "OSystemWinCE.hxx"
#include "SettingsWinCE.hxx"
#include "PropsSet.hxx"
#include "FSNode.hxx"
#include "FrameBufferWinCE.hxx"

#define KEYSCHECK_ASYNC

struct key2event
{
	UINT keycode;
	SDLKey sdlkey;
	uInt32 state;
	SDLKey launcherkey;
};
extern key2event keycodes[2][MAX_KEYS+NUM_MOUSEKEYS];
extern void KeySetup(void);
extern void KeySetMode(int);
bool RequestRefresh = false;

OSystemWinCE* theOSystem = (OSystemWinCE*) NULL;
HWND hWnd;
uInt16 rotkeystate = 0;


void KeyCheck(void)
{
#ifdef KEYSCHECK_ASYNC
	if (GetAsyncKeyState(VK_F3))
	{
		if (rotkeystate == 0)
			KeySetMode( ((FrameBufferWinCE *) (&(theOSystem->frameBuffer())))->rotatedisplay() );
		rotkeystate = 1;
	}
	else
		rotkeystate = 0;

	for (int i=0; i<MAX_KEYS; i++)
		if (GetAsyncKeyState(keycodes[0][i].keycode))
			keycodes[0][i].state = 1;
		else
			keycodes[0][i].state = 0;
#endif
}


void CleanUp(void)
{
	if(theOSystem) delete theOSystem;
	GXCloseDisplay();
	GXCloseInput();
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	static PAINTSTRUCT ps;

	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_F3)
		{
			if (rotkeystate == 0)
				KeySetMode( ((FrameBufferWinCE *) (&(theOSystem->frameBuffer())))->rotatedisplay() );
			rotkeystate = 1;
		}
		else
			rotkeystate = 0;
#ifndef KEYSCHECK_ASYNC
		uInt8 i;
			for (i=0; i<MAX_KEYS; i++)
				if (wParam == keycodes[0][i].keycode)
					keycodes[0][i].state = 1;
#endif
		return 0;

	case WM_KEYUP:
#ifndef KEYSCHECK_ASYNC
		for (i=0; i<MAX_KEYS; i++)
			if (wParam == keycodes[0][i].keycode)
				keycodes[0][i].state = 0;
#endif
		return 0;

	case WM_MOUSEMOVE:
		keycodes[0][M_POS].state = lParam;
		return 0;

	case WM_LBUTTONDOWN:
		keycodes[0][M_BUT].state = lParam | 0x80000000;
		return 0;

	case WM_LBUTTONUP:
		keycodes[0][M_BUT].state = lParam & 0x7FFFFFFF;
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_SETFOCUS:
	case WM_ACTIVATE:
		GXResume();
		return 0;

	case WM_KILLFOCUS:
	case WM_HIBERNATE:
		GXSuspend();
		return 0;

	case WM_PAINT:
		BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		RequestRefresh = true;
		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd )
{
	LPTSTR wndname = _T("PocketStella");
	WNDCLASS wc = { CS_HREDRAW | CS_VREDRAW, WindowProc, 0, 0, hInstance, NULL, NULL, 
		(HBRUSH)GetStockObject(BLACK_BRUSH), NULL, wndname};
	RegisterClass(&wc);
	hWnd = CreateWindow(wndname, wndname, WS_VISIBLE | WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN),
						GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hInstance, NULL);
	if (!hWnd) return 1;
	SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	
	// pump the messages to get the window up
	MSG msg;
	while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	LOGFONT f = {11, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
				 OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, _T("")};
	HFONT hFnt = CreateFontIndirect(&f);
	HDC hDC = GetDC(hWnd);
	SelectObject(hDC, hFnt);
	RECT RWnd;
	GetClipBox(hDC, &RWnd);
	SetTextColor(hDC, 0xA0A0A0);
	SetBkColor(hDC, 0);
	DrawText(hDC, _T("PocketStella Initializing..."), -1, &RWnd, DT_CENTER | DT_VCENTER);
	ReleaseDC(hWnd, hDC);
	DeleteObject(hFnt);
	
	theOSystem = new OSystemWinCE();
	SettingsWinCE theSettings(theOSystem);
	theOSystem->settings().loadConfig();
	theOSystem->settings().validate();
	EventHandler theEventHandler(theOSystem);
	PropertiesSet propertiesSet;
	propertiesSet.load(theOSystem->systemProperties(), false);
	theOSystem->attach(&propertiesSet);

	if ( !GXOpenDisplay(hWnd, GX_FULLSCREEN) || !GXOpenInput() )
	{
		CleanUp();
		return 1;
	}
	KeySetup();

	if(!theOSystem->createFrameBuffer())
	{
		CleanUp();
		return 1;
	}
	theOSystem->createSound();

	string romfile = ((string) getcwd()) + ((string) "\\") + theSettings.getString("GameFilename");
	if (!FilesystemNode::fileExists(romfile))
		theOSystem->createLauncher();
	else
		theOSystem->createConsole(romfile);

	theOSystem->mainLoop();

	CleanUp();

	return 0;
}
