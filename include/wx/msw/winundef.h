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
#endif

inline BOOL  GetCharWidth(HDC dc, UINT first, UINT last, LPINT buffer)
{
#ifdef _UNICODE
   return GetCharWidthW(dc, first, last, buffer);
#else
   return GetCharWidthA(dc, first, last, buffer);
#endif
}

// FindWindow

#ifdef FindWindow
    #undef FindWindow
#endif

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

// GetClassName

#ifdef GetClassName
    #undef GetClassName
#endif

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

// GetClassInfo

#ifdef GetClassInfo
    #undef GetClassInfo
#endif

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

// LoadAccelerators

#ifdef LoadAccelerators
    #undef LoadAccelerators
#endif

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


// GetWindowProc... this isn't a Windows API function?!?!
//ifdef GetWindowProc
//   #undef GetWindowProc
//endif


// DrawText

#ifdef DrawText
    #undef DrawText
#endif

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

// StartDoc

#ifdef StartDoc
    #undef StartDoc
#endif

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

// GetFirstChild... not a Windows API Function!?!?!
//ifdef GetFirstChild
//   #undef GetFirstChild
//endif

//ifdef GetNextChild
//    #undef GetNextChild
//endif

//ifdef GetNextSibling
//    #undef GetNextSibling
//endif

// GetObject

#ifdef GetObject
    #undef GetObject
#endif

inline int GetObject(HGDIOBJ h, int i, LPVOID buffer)
{
#ifdef _UNICODE
   return GetObjectW(h, i, buffer);
#else
   return GetObjectA(h, i, buffer);
#endif   
}

#endif // _WX_WINUNDEF_H_
