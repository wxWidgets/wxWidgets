/////////////////////////////////////////////////////////////////////////////
// Name:        winundef.h
// Purpose:     undefine the common symbols #define'd by <windows.h>
// Author:      Vadim Zeitlin
// Created:     16.05.99
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/* THIS SHOULD NOT BE USED since you might include it once e.g. in window.h,
 * then again _AFTER_ you've included windows.h, in which case it won't work
 * a 2nd time -- JACS
#ifndef _WX_WINUNDEF_H_
#define _WX_WINUNDEF_H_
 */

// ----------------------------------------------------------------------------
// windows.h and other win32 headers can #define the following identifiers
// which are also used by wxWidgets so we replace these symbols with the
// corresponding inline functions and undefine the macro.
//
// This looks quite ugly here but allows us to write clear (and correct!) code
// elsewhere because the functions, unlike the macros, respect the scope.
//
// wxWidgets is built using the WIDE win32 API, and the inline functions in
// this file will use the W variant by default.
// Applications that include and use ANSI Windows.h can override this by
// defining wxNO_WIN32_W before including any wxWidgets headers. The inline
// function will then use the A variant.
// ----------------------------------------------------------------------------

#if defined(WXBUILDING) || !defined(wxNO_WIN32_W)
    #define wxFUNC_AW(fn) fn ## W
    #define wxLPCSTR LPCWSTR
    #define wxLPSTR LPWSTR
    #define wxLPWNDCLASS LPWNDCLASSW
    #define wxDOCINFO DOCINFOW
    #define wxLPFINDREPLACE LPFINDREPLACEW
#else
    #define wxFUNC_AW(fn) fn ## A
    #define wxLPCSTR LPCSTR
    #define wxLPSTR LPSTR
    #define wxLPWNDCLASS LPWNDCLASSA
    #define wxDOCINFO DOCINFOA
    #define wxLPFINDREPLACE LPFINDREPLACEA
#endif

#ifdef CreateDialog
    #undef CreateDialog

    inline HWND CreateDialog(HINSTANCE hInstance,
                             wxLPCSTR pTemplate,
                             HWND hwndParent,
                             DLGPROC pDlgProc)
    {
        return wxFUNC_AW(CreateDialog)(hInstance, pTemplate, hwndParent, pDlgProc);
    }
#endif

#ifdef CreateFont
    #undef CreateFont

    inline HFONT CreateFont(int height,
                            int width,
                            int escapement,
                            int orientation,
                            int weight,
                            DWORD italic,
                            DWORD underline,
                            DWORD strikeout,
                            DWORD charset,
                            DWORD outprecision,
                            DWORD clipprecision,
                            DWORD quality,
                            DWORD family,
                            wxLPCSTR facename)
    {
        return wxFUNC_AW(CreateFont)(height, width, escapement, orientation,
                                     weight, italic, underline, strikeout,
                                     charset, outprecision, clipprecision,
                                     quality, family, facename);
    }
#endif

#ifdef CreateWindow
    #undef CreateWindow

    inline HWND CreateWindow(wxLPCSTR lpClassName,
                             wxLPCSTR lpWndClass,
                             DWORD dwStyle,
                             int x, int y, int w, int h,
                             HWND hWndParent,
                             HMENU hMenu,
                             HINSTANCE hInstance,
                             LPVOID lpParam)
    {
        return wxFUNC_AW(CreateWindow)(lpClassName, lpWndClass, dwStyle, x, y, w, h,
                                       hWndParent, hMenu, hInstance, lpParam);
    }
#endif

#ifdef LoadMenu
    #undef LoadMenu

    inline HMENU LoadMenu(HINSTANCE instance, wxLPCSTR name)
    {
        return wxFUNC_AW(LoadMenu)(instance, name);
    }
#endif

#ifdef FindText
    #undef FindText

    inline HWND APIENTRY FindText(wxLPFINDREPLACE lpfindreplace)
    {
        return wxFUNC_AW(FindText)(lpfindreplace);
    }
#endif

#ifdef GetCharWidth
    #undef GetCharWidth

    inline BOOL GetCharWidth(HDC dc, UINT first, UINT last, LPINT buffer)
    {
        return wxFUNC_AW(GetCharWidth)(dc, first, last, buffer);
    }
#endif

#ifdef FindWindow
    #undef FindWindow

    inline HWND FindWindow(wxLPCSTR classname, wxLPCSTR windowname)
    {
        return wxFUNC_AW(FindWindow)(classname, windowname);
    }
#endif

#ifdef PlaySound
    #undef PlaySound

    inline BOOL PlaySound(wxLPCSTR pszSound, HMODULE hMod, DWORD fdwSound)
    {
        return wxFUNC_AW(PlaySound)(pszSound, hMod, fdwSound);
    }
#endif

#ifdef GetClassName
    #undef GetClassName

    inline int GetClassName(HWND h, wxLPSTR classname, int maxcount)
    {
        return wxFUNC_AW(GetClassName)(h, classname, maxcount);
    }
#endif

#ifdef GetClassInfo
    #undef GetClassInfo

    inline BOOL GetClassInfo(HINSTANCE h, wxLPCSTR name, wxLPWNDCLASS winclass)
    {
        return wxFUNC_AW(GetClassInfo)(h, name, winclass);
    }
#endif

#ifdef LoadAccelerators
    #undef LoadAccelerators

    inline HACCEL LoadAccelerators(HINSTANCE h, wxLPCSTR name)
    {
        return wxFUNC_AW(LoadAccelerators)(h, name);
    }
#endif

#ifdef DrawText
    #undef DrawText

    inline int DrawText(HDC h, wxLPCSTR str, int count, LPRECT rect, UINT format)
    {
        return wxFUNC_AW(DrawText)(h, str, count, rect, format);
    }
#endif

#ifdef StartDoc
    #undef StartDoc

    inline int StartDoc(HDC h, CONST wxDOCINFO* info)
    {
        return wxFUNC_AW(StartDoc)(h, info);
    }
#endif

#ifdef GetObject
    #undef GetObject

    inline int GetObject(HGDIOBJ h, int i, LPVOID buffer)
    {
        return wxFUNC_AW(GetObject)(h, i, buffer);
    }
#endif

#ifdef GetMessage
    #undef GetMessage

    inline int GetMessage(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
    {
        return wxFUNC_AW(GetMessage)(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
    }
#endif

#ifdef LoadIcon
    #undef LoadIcon

    inline HICON LoadIcon(HINSTANCE hInstance, wxLPCSTR lpIconName)
    {
        return wxFUNC_AW(LoadIcon)(hInstance, lpIconName);
    }
#endif

#ifdef LoadBitmap
    #undef LoadBitmap

    inline HBITMAP LoadBitmap(HINSTANCE hInstance, wxLPCSTR lpBitmapName)
    {
        return wxFUNC_AW(LoadBitmap)(hInstance, lpBitmapName);
    }
#endif

#ifdef LoadLibrary
    #undef LoadLibrary

    inline HINSTANCE LoadLibrary(wxLPCSTR lpLibFileName)
    {
        return wxFUNC_AW(LoadLibrary)(lpLibFileName);
    }
#endif

#ifdef FindResource
    #undef FindResource

    inline HRSRC FindResource(HMODULE hModule, wxLPCSTR lpName, wxLPCSTR lpType)
    {
        return wxFUNC_AW(FindResource)(hModule, lpName, lpType);
    }
#endif

#ifdef IsMaximized
    #undef IsMaximized

    inline BOOL IsMaximized(HWND hwnd)
    {
        return IsZoomed(hwnd);
    }
#endif

#ifdef GetFirstChild
    #undef GetFirstChild

    inline HWND GetFirstChild(HWND hwnd)
    {
        return GetTopWindow(hwnd);
    }
#endif

#ifdef GetFirstSibling
    #undef GetFirstSibling

    inline HWND GetFirstSibling(HWND hwnd)
    {
        return GetWindow(hwnd, GW_HWNDFIRST);
    }
#endif

#ifdef GetLastSibling
    #undef GetLastSibling

    inline HWND GetLastSibling(HWND hwnd)
    {
        return GetWindow(hwnd, GW_HWNDLAST);
    }
#endif

#ifdef GetPrevSibling
    #undef GetPrevSibling

    inline HWND GetPrevSibling(HWND hwnd)
    {
        return GetWindow(hwnd, GW_HWNDPREV);
    }
#endif

#ifdef GetNextSibling
    #undef GetNextSibling

    inline HWND GetNextSibling(HWND hwnd)
    {
        return GetWindow(hwnd, GW_HWNDNEXT);
    }
#endif

#ifdef Yield
    #undef Yield
#endif

// For WINE

#ifdef GetWindowStyle
    #undef GetWindowStyle
#endif

// #endif // _WX_WINUNDEF_H_
