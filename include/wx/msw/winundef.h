/////////////////////////////////////////////////////////////////////////////
// Name:        winundef.h
// Purpose:     undefine the common symbols #define'd by <windows.h>
// Author:      Vadim Zeitlin
// Modified by:
// Created:     16.05.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUNDEF_H_
#define _WX_WINUNDEF_H_

// windows.h #defines the following identifiers which are also used in wxWin

// GetCharWidth

#ifdef GetCharWidth
   #undef GetCharWidth
   inline BOOL  GetCharWidth(HDC dc, UINT first, UINT last, LPINT buffer)
   {
   #ifdef _UNICODE
      return GetCharWidthW(dc, first, last, buffer);
   #else
      return GetCharWidthA(dc, first, last, buffer);
   #endif
   }
#endif

// FindWindow

#ifdef FindWindow
   #undef FindWindow
   #ifdef _UNICODE
   inline HWND FindWindow(LPCWSTR classname, LPCWSTR windowname)
   {
      return FindWindowW(classname, windowname);
   }
   #else
   inline HWND FindWindow(LPCSTR classname, LPCSTR windowname)
   {
      return FindWindowA(classname, windowname);
   }
   #endif
#endif

// GetClassName

#ifdef GetClassName
   #undef GetClassName
   #ifdef _UNICODE
   inline int GetClassName(HWND h, LPWSTR classname, int maxcount)
   {
      return GetClassNameW(h, classname, maxcount);
   }
   #else
   inline int GetClassName(HWND h, LPSTR classname, int maxcount)
   {
      return GetClassNameA(h, classname, maxcount);
   }
   #endif
#endif

// GetClassInfo

#ifdef GetClassInfo
   #undef GetClassInfo
   #ifdef _UNICODE
   inline BOOL GetClassInfo(HINSTANCE h, LPCWSTR name, LPWNDCLASSW winclass)
   {
      return GetClassInfoW(h, name, winclass);
   }
   #else
   inline BOOL GetClassInfo(HINSTANCE h, LPCSTR name, LPWNDCLASSA winclass)
   {
      return GetClassInfoA(h, name, winclass);
   }
   #endif
#endif

// LoadAccelerators

#ifdef LoadAccelerators
   #undef LoadAccelerators
   #ifdef _UNICODE
   inline HACCEL LoadAccelerators(HINSTANCE h, LPCWSTR name)
   {
      return LoadAcceleratorsW(h, name);
   }
   #else
   inline HACCEL LoadAccelerators(HINSTANCE h, LPCSTR name)
   {
      return LoadAcceleratorsA(h, name);
   }
   #endif
#endif

// DrawText

#ifdef DrawText
   #undef DrawText
   #ifdef _UNICODE
   inline int DrawText(HDC h, LPCWSTR str, int count, LPRECT rect, UINT format)
   {
      return DrawTextW(h, str, count, rect, format);
   }
   #else
   inline int DrawText(HDC h, LPCSTR str, int count, LPRECT rect, UINT format)
   {
      return DrawTextA(h, str, count, rect, format);
   }
   #endif
#endif

// StartDoc

#ifdef StartDoc
   #undef StartDoc
   #ifdef _UNICODE
   inline int StartDoc(HDC h, CONST DOCINFOW* info)
   {
      return StartDocW(h, info);
   }
   #else
   inline int StartDoc(HDC h, CONST DOCINFOA* info)
   {
      return StartDocA(h, info);
   }
   #endif
#endif

// GetObject

#ifdef GetObject
   #undef GetObject
   inline int GetObject(HGDIOBJ h, int i, LPVOID buffer)
   {
   #ifdef _UNICODE
      return GetObjectW(h, i, buffer);
   #else
      return GetObjectA(h, i, buffer);
   #endif   
   }
#endif


// For ming and cygwin

// GetFirstChild
#ifdef GetFirstChild
   #undef GetFirstChild
   inline HWND GetFirstChild(HWND h)
   {
      return GetTopWindow(h);
   }
#endif


// GetNextSibling
#ifdef GetNextSibling
    #undef GetNextSibling
    inline HWND GetNextSibling(HWND h)
    {
      return GetWindow(h, GW_HWNDNEXT);
    }
#endif

// GetWindowProc
//ifdef GetWindowProc
//   #undef GetWindowProc
//endif
//ifdef GetNextChild
//    #undef GetNextChild
//endif

#endif // _WX_WINUNDEF_H_
