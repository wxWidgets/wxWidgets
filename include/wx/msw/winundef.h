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

// CreateDialog

#if defined(CreateDialog)
    #undef CreateDialog

    inline HWND CreateDialog(HINSTANCE hInstance,
                             LPCTSTR pTemplate,
                             HWND hwndParent,
                             DLGPROC pDlgProc)
    {
        return CreateDialogW(hInstance, pTemplate, hwndParent, pDlgProc);
    }
#endif

// CreateFont

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
#endif // CreateFont

// CreateWindow

#if defined(CreateWindow)
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

// LoadMenu

#ifdef LoadMenu
    #undef LoadMenu

    inline HMENU LoadMenu(HINSTANCE instance, LPCTSTR name)
    {
        return LoadMenuW(instance, name);
    }
#endif

// FindText

#ifdef FindText
    #undef FindText

    inline HWND APIENTRY FindText(LPFINDREPLACE lpfindreplace)
    {
        return FindTextW(lpfindreplace);
    }
#endif

// GetCharWidth

#ifdef GetCharWidth
   #undef GetCharWidth

   inline BOOL GetCharWidth(HDC dc, UINT first, UINT last, LPINT buffer)
   {
      return GetCharWidthW(dc, first, last, buffer);
   }
#endif

// FindWindow

#ifdef FindWindow
   #undef FindWindow

   inline HWND FindWindow(LPCWSTR classname, LPCWSTR windowname)
   {
      return FindWindowW(classname, windowname);
   }
#endif

// PlaySound

#ifdef PlaySound
   #undef PlaySound

   inline BOOL PlaySound(LPCWSTR pszSound, HMODULE hMod, DWORD fdwSound)
   {
      return PlaySoundW(pszSound, hMod, fdwSound);
   }
#endif

// GetClassName

#ifdef GetClassName
   #undef GetClassName

   inline int GetClassName(HWND h, LPWSTR classname, int maxcount)
   {
      return GetClassNameW(h, classname, maxcount);
   }
#endif

// GetClassInfo

#ifdef GetClassInfo
   #undef GetClassInfo

   inline BOOL GetClassInfo(HINSTANCE h, LPCWSTR name, LPWNDCLASSW winclass)
   {
      return GetClassInfoW(h, name, winclass);
   }
#endif

// LoadAccelerators

#ifdef LoadAccelerators
   #undef LoadAccelerators

   inline HACCEL LoadAccelerators(HINSTANCE h, LPCWSTR name)
   {
      return LoadAcceleratorsW(h, name);
   }
#endif

// DrawText

#ifdef DrawText
   #undef DrawText

   inline int DrawText(HDC h, LPCWSTR str, int count, LPRECT rect, UINT format)
   {
      return DrawTextW(h, str, count, rect, format);
   }
#endif


// StartDoc

#ifdef StartDoc
   #undef StartDoc

   inline int StartDoc(HDC h, CONST DOCINFOW* info)
   {
      return StartDocW(h, const_cast<DOCINFOW*>(info));
   }
#endif

// GetObject

#ifdef GetObject
   #undef GetObject
   inline int GetObject(HGDIOBJ h, int i, LPVOID buffer)
   {
      return GetObjectW(h, i, buffer);
   }
#endif

// GetMessage

#ifdef GetMessage
   #undef GetMessage
   inline int GetMessage(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
   {
      return GetMessageW(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);
   }
#endif

// LoadIcon
#ifdef LoadIcon
    #undef LoadIcon
    inline HICON LoadIcon(HINSTANCE hInstance, LPCTSTR lpIconName)
    {
        return LoadIconW(hInstance, lpIconName);
    }
#endif // LoadIcon

// LoadBitmap
#ifdef LoadBitmap
    #undef LoadBitmap
    inline HBITMAP LoadBitmap(HINSTANCE hInstance, LPCTSTR lpBitmapName)
    {
        return LoadBitmapW(hInstance, lpBitmapName);
    }
#endif // LoadBitmap

// LoadLibrary

#ifdef LoadLibrary
    #undef LoadLibrary

    inline HINSTANCE LoadLibrary(LPCWSTR lpLibFileName)
    {
        return LoadLibraryW(lpLibFileName);
    }
#endif

// FindResource
#ifdef FindResource
    #undef FindResource

    inline HRSRC FindResource(HMODULE hModule, LPCWSTR lpName, LPCWSTR lpType)
    {
        return FindResourceW(hModule, lpName, lpType);
    }
#endif

// IsMaximized

#ifdef IsMaximized
    #undef IsMaximized
    inline BOOL IsMaximized(HWND hwnd)
    {
        return IsZoomed(hwnd);
    }
#endif

// GetFirstChild

#ifdef GetFirstChild
    #undef GetFirstChild
    inline HWND GetFirstChild(HWND hwnd)
    {
        return GetTopWindow(hwnd);
    }
#endif

// GetFirstSibling

#ifdef GetFirstSibling
    #undef GetFirstSibling
    inline HWND GetFirstSibling(HWND hwnd)
    {
        return GetWindow(hwnd,GW_HWNDFIRST);
    }
#endif

// GetLastSibling

#ifdef GetLastSibling
    #undef GetLastSibling
    inline HWND GetLastSibling(HWND hwnd)
    {
        return GetWindow(hwnd,GW_HWNDLAST);
    }
#endif

// GetPrevSibling

#ifdef GetPrevSibling
    #undef GetPrevSibling
    inline HWND GetPrevSibling(HWND hwnd)
    {
        return GetWindow(hwnd,GW_HWNDPREV);
    }
#endif

// GetNextSibling

#ifdef GetNextSibling
    #undef GetNextSibling
    inline HWND GetNextSibling(HWND hwnd)
    {
        return GetWindow(hwnd,GW_HWNDNEXT);
    }
#endif

// For WINE

#if defined(GetWindowStyle)
  #undef GetWindowStyle
#endif

// For ming and cygwin

#ifdef Yield
    #undef Yield
#endif


// GetWindowProc
//ifdef GetWindowProc
//   #undef GetWindowProc
//endif
//ifdef GetNextChild
//    #undef GetNextChild
//endif

// #endif // _WX_WINUNDEF_H_

