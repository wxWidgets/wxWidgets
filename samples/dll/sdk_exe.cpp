/////////////////////////////////////////////////////////////////////////////
// Name:        samples/dll/my_exe.cpp
// Purpose:     Sample showing how to use wx DLL from a Win32 application
// Author:      Vadim Zeitlin
// Created:     2009-12-07
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/*
    This program is intentionally as simple as possible and shouldn't be seen
    as an example of how to write a proper Win32 application (why should you
    want to do this anyhow when you have wxWidgets). It's just a test bed for
    the wx DLL which it uses.
 */

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <windows.h>
#include <windowsx.h>

#include <stdio.h>
#include <tchar.h>

#include "my_dll.h"

namespace
{

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

const TCHAR *MAIN_WIN_CLASS_NAME = _TEXT("my_exe_main_win_class");

const INT_PTR IDB_RUN_GUI_FROM_DLL = 100;

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

HINSTANCE g_hInstance;
HWND g_hwndMain;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// callbacks
// ----------------------------------------------------------------------------

void
OnCommand(HWND /* hwnd */, int id, HWND /* hwndCtl */, UINT /* codeNotify */)
{
    if ( id == IDB_RUN_GUI_FROM_DLL )
    {
        run_wx_gui_from_dll("child instance");
    }
}

void OnDestroy(HWND /* hwnd */)
{
    wx_dll_cleanup();

    PostQuitMessage(0);
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch ( msg )
    {
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

// ----------------------------------------------------------------------------
// initialization functions
// ----------------------------------------------------------------------------

bool RegisterMainClass()
{
    WNDCLASS wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.style         = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = MainWndProc;
    wc.hInstance     = g_hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = MAIN_WIN_CLASS_NAME;

    return RegisterClass(&wc) != 0;
}

bool CreateMainWindow()
{
    g_hwndMain = CreateWindow
                 (
                    MAIN_WIN_CLASS_NAME,
                    _TEXT("Main Win32 app"),
                    WS_OVERLAPPEDWINDOW,
                    CW_USEDEFAULT, CW_USEDEFAULT,
                    400, 300,
                    NULL, NULL, g_hInstance, NULL
                 );
    if ( !g_hwndMain )
        return false;

    CreateWindow
    (
        _TEXT("static"),
        _TEXT("Main Win32 application"),
        WS_CHILD | WS_VISIBLE,
        10, 10, 200, 30,
        g_hwndMain, (HMENU)-1, g_hInstance, NULL
    );

    CreateWindow
    (
        _TEXT("button"),
        _TEXT("Run GUI from DLL"),
        WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
        200, 200, 150, 35,
        g_hwndMain, (HMENU)IDB_RUN_GUI_FROM_DLL, g_hInstance, NULL
    );

    return true;
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// entry point
// ----------------------------------------------------------------------------

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    g_hInstance = hInstance;

    if ( !RegisterMainClass() )
        return 1;

    if ( !CreateMainWindow() )
        return 2;

    ShowWindow(g_hwndMain, nCmdShow);

    MSG msg;
    while ( GetMessage(&msg, NULL, 0, 0) )
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
