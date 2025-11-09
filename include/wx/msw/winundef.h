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
// windows.h #defines the following identifiers which are also used in wxWin so
// we replace these symbols with the corresponding inline functions and
// undefine the macro.
//
// This looks quite ugly here but allows us to write clear (and correct!) code
// elsewhere because the functions, unlike the macros, respect the scope.
// ----------------------------------------------------------------------------

#ifdef CreateDialog
    #undef CreateDialog

    inline HWND CreateDialog(HINSTANCE hInstance,
                             LPCTSTR pTemplate,
                             HWND hwndParent,
                             DLGPROC pDlgProc)
    {
        return CreateDialogW(hInstance, pTemplate, hwndParent, pDlgProc);
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
                            LPCTSTR facename)
    {
        return CreateFontW(height, width, escapement, orientation,
                           weight, italic, underline, strikeout, charset,
                           outprecision, clipprecision, quality,
                           family, facename);
    }
#endif

#ifdef CreateWindow
    #undef CreateWindow

    inline HWND CreateWindow(LPCTSTR lpClassName,
                             LPCTSTR lpWndClass,
                             DWORD dwStyle,
                             int x, int y, int w, int h,
                             HWND hWndParent,
                             HMENU hMenu,
                             HINSTANCE hInstance,
                             LPVOID lpParam)
    {
        return CreateWindowW(lpClassName, lpWndClass, dwStyle, x, y, w, h,
                             hWndParent, hMenu, hInstance, lpParam);
    }
#endif

#ifdef LoadMenu
    #undef LoadMenu

    inline HMENU LoadMenu(HINSTANCE instance, LPCTSTR name)
    {
        return LoadMenuW(instance, name);
    }
#endif

#ifdef FindText
    #undef FindText

    inline HWND APIENTRY FindText(LPFINDREPLACE lpfindreplace)
    {
        return FindTextW(lpfindreplace);
    }
#endif

#ifdef GetCharWidth
    #undef GetCharWidth

    inline BOOL GetCharWidth(HDC dc, UINT first, UINT last, LPINT buffer)
    {
        return GetCharWidthW(dc, first, last, buffer);
    }
#endif

#ifdef FindWindow
    #undef FindWindow

    inline HWND FindWindow(LPCWSTR classname, LPCWSTR windowname)
    {
        return FindWindowW(classname, windowname);
    }
#endif

#ifdef PlaySound
    #undef PlaySound

    inline BOOL PlaySound(LPCWSTR pszSound, HMODULE hMod, DWORD fdwSound)
    {
        return PlaySoundW(pszSound, hMod, fdwSound);
    }
#endif

#ifdef GetClassName
    #undef GetClassName

    inline int GetClassName(HWND h, LPWSTR classname, int maxcount)
    {
        return GetClassNameW(h, classname, maxcount);
    }
#endif

#ifdef GetClassInfo
    #undef GetClassInfo

    inline BOOL GetClassInfo(HINSTANCE h, LPCWSTR name, LPWNDCLASSW winclass)
    {
        return GetClassInfoW(h, name, winclass);
    }
#endif

#ifdef LoadAccelerators
    #undef LoadAccelerators

    inline HACCEL LoadAccelerators(HINSTANCE h, LPCWSTR name)
    {
        return LoadAcceleratorsW(h, name);
    }
#endif

#ifdef DrawText
    #undef DrawText

    inline int DrawText(HDC h, LPCWSTR str, int count, LPRECT rect, UINT format)
    {
        return DrawTextW(h, str, count, rect, format);
    }
#endif

#ifdef StartDoc
#undef StartDoc

    inline int StartDoc(HDC h, CONST DOCINFOW* info)
    {
        return StartDocW(h, info);
    }
#endif

#ifdef GetObject
    #undef GetObject

    inline int GetObject(HGDIOBJ h, int i, LPVOID buffer)
    {
        return GetObjectW(h, i, buffer);
    }
#endif

#ifdef GetMessage
    #undef GetMessage

    inline int GetMessage(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
    {
        return GetMessageW(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
    }
#endif

#ifdef LoadIcon
    #undef LoadIcon

    inline HICON LoadIcon(HINSTANCE hInstance, LPCTSTR lpIconName)
    {
        return LoadIconW(hInstance, lpIconName);
    }
#endif

#ifdef LoadBitmap
    #undef LoadBitmap

    inline HBITMAP LoadBitmap(HINSTANCE hInstance, LPCTSTR lpBitmapName)
    {
        return LoadBitmapW(hInstance, lpBitmapName);
    }
#endif

#ifdef LoadLibrary
    #undef LoadLibrary

    inline HINSTANCE LoadLibrary(LPCWSTR lpLibFileName)
    {
        return LoadLibraryW(lpLibFileName);
    }
#endif

#ifdef FindResource
    #undef FindResource

    inline HRSRC FindResource(HMODULE hModule, LPCWSTR lpName, LPCWSTR lpType)
    {
        return FindResourceW(hModule, lpName, lpType);
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
