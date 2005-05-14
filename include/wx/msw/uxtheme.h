///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/uxtheme.h
// Purpose:     wxUxThemeEngine class: support for XP themes
// Author:      John Platts, Vadim Zeitlin
// Modified by:
// Created:     2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 John Platts, Vadim Zeitlin
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UXTHEME_H_
#define _WX_UXTHEME_H_

#ifdef __GNUG__
  #pragma interface "uxtheme.cpp"
#endif

#include "wx/defs.h"

#include "wx/msw/uxthemep.h"

// Use wxModule approach to initialization.
#define WXU_USE_WXMODULE 1

typedef HTHEME  (__stdcall *PFNWXUOPENTHEMEDATA)(HWND, const wchar_t *);
typedef HRESULT (__stdcall *PFNWXUCLOSETHEMEDATA)(HTHEME);
typedef HRESULT (__stdcall *PFNWXUDRAWTHEMEBACKGROUND)(HTHEME, HDC, int, int, const RECT *, const RECT *);
typedef HRESULT (__stdcall *PFNWXUDRAWTHEMETEXT)(HTHEME, HDC, int, int, const wchar_t *, int, DWORD, DWORD, const RECT *);
typedef HRESULT (__stdcall *PFNWXUGETTHEMEBACKGROUNDCONTENTRECT)(HTHEME, HDC, int, int, const RECT *, RECT *);
typedef HRESULT (__stdcall *PFNWXUGETTHEMEBACKGROUNDEXTENT)(HTHEME, HDC, int, int, const RECT *, RECT *);
typedef HRESULT (__stdcall *PFNWXUGETTHEMEPARTSIZE)(HTHEME, HDC, int, int, const RECT *, /* enum */ THEMESIZE, SIZE *);
typedef HRESULT (__stdcall *PFNWXUGETTHEMETEXTEXTENT)(HTHEME, HDC, int, int, const wchar_t *, int, DWORD, const RECT *, RECT *);
typedef HRESULT (__stdcall *PFNWXUGETTHEMETEXTMETRICS)(HTHEME, HDC, int, int, TEXTMETRIC*);
typedef HRESULT (__stdcall *PFNWXUGETTHEMEBACKGROUNDREGION)(HTHEME, HDC, int, int, const RECT *, HRGN *);
typedef HRESULT (__stdcall *PFNWXUHITTESTTHEMEBACKGROUND)(HTHEME, HDC, int, int, DWORD, const RECT *, HRGN, POINT, unsigned short *);
typedef HRESULT (__stdcall *PFNWXUDRAWTHEMEEDGE)(HTHEME, HDC, int, int, const RECT *, unsigned int, unsigned int, RECT *);
typedef HRESULT (__stdcall *PFNWXUDRAWTHEMEICON)(HTHEME, HDC, int, int, const RECT *, HIMAGELIST, int);
typedef BOOL    (__stdcall *PFNWXUISTHEMEPARTDEFINED)(HTHEME, int, int);
typedef BOOL    (__stdcall *PFNWXUISTHEMEBACKGROUNDPARTIALLYTRANSPARENT)(HTHEME, int, int);
typedef HRESULT (__stdcall *PFNWXUGETTHEMECOLOR)(HTHEME, int, int, int, COLORREF*);
typedef HRESULT (__stdcall *PFNWXUGETTHEMEMETRIC)(HTHEME, HDC, int, int, int, int *);
typedef HRESULT (__stdcall *PFNWXUGETTHEMESTRING)(HTHEME, int, int, int, wchar_t *, int);
typedef HRESULT (__stdcall *PFNWXUGETTHEMEBOOL)(HTHEME, int, int, int, BOOL *);
typedef HRESULT (__stdcall *PFNWXUGETTHEMEINT)(HTHEME, int, int, int, int *);
typedef HRESULT (__stdcall *PFNWXUGETTHEMEENUMVALUE)(HTHEME, int, int, int, int *);
typedef HRESULT (__stdcall *PFNWXUGETTHEMEPOSITION)(HTHEME, int, int, int, POINT *);
typedef HRESULT (__stdcall *PFNWXUGETTHEMEFONT)(HTHEME, HDC, int, int, int, LOGFONT *);
typedef HRESULT (__stdcall *PFNWXUGETTHEMERECT)(HTHEME, int, int, int, RECT *);
typedef HRESULT (__stdcall *PFNWXUGETTHEMEMARGINS)(HTHEME, HDC, int, int, int, RECT *, MARGINS *);
typedef HRESULT (__stdcall *PFNWXUGETTHEMEINTLIST)(HTHEME, int, int, int, INTLIST*);
typedef HRESULT (__stdcall *PFNWXUGETTHEMEPROPERTYORIGIN)(HTHEME, int, int, int, /* enum */ PROPERTYORIGIN *);
typedef HRESULT (__stdcall *PFNWXUSETWINDOWTHEME)(HWND, const wchar_t*, const wchar_t *);
typedef HRESULT (__stdcall *PFNWXUGETTHEMEFILENAME)(HTHEME, int, int, int, wchar_t *, int);
typedef COLORREF(__stdcall *PFNWXUGETTHEMESYSCOLOR)(HTHEME, int);
typedef HBRUSH  (__stdcall *PFNWXUGETTHEMESYSCOLORBRUSH)(HTHEME, int);
typedef BOOL    (__stdcall *PFNWXUGETTHEMESYSBOOL)(HTHEME, int);
typedef int     (__stdcall *PFNWXUGETTHEMESYSSIZE)(HTHEME, int);
typedef HRESULT (__stdcall *PFNWXUGETTHEMESYSFONT)(HTHEME, int, LOGFONT *);
typedef HRESULT (__stdcall *PFNWXUGETTHEMESYSSTRING)(HTHEME, int, wchar_t *, int);
typedef HRESULT (__stdcall *PFNWXUGETTHEMESYSINT)(HTHEME, int, int *);
typedef BOOL    (__stdcall *PFNWXUISTHEMEACTIVE)();
typedef BOOL    (__stdcall *PFNWXUISAPPTHEMED)();
typedef HTHEME  (__stdcall *PFNWXUGETWINDOWTHEME)(HWND);
typedef HRESULT (__stdcall *PFNWXUENABLETHEMEDIALOGTEXTURE)(HWND, DWORD);
typedef BOOL    (__stdcall *PFNWXUISTHEMEDIALOGTEXTUREENABLED)(HWND);
typedef DWORD   (__stdcall *PFNWXUGETTHEMEAPPPROPERTIES)();
typedef void    (__stdcall *PFNWXUSETTHEMEAPPPROPERTIES)(DWORD);
typedef HRESULT (__stdcall *PFNWXUGETCURRENTTHEMENAME)(wchar_t *, int, wchar_t *, int, wchar_t *, int);
typedef HRESULT (__stdcall *PFNWXUGETTHEMEDOCUMENTATIONPROPERTY)(const wchar_t *, const wchar_t *, wchar_t *, int);
typedef HRESULT (__stdcall *PFNWXUDRAWTHEMEPARENTBACKGROUND)(HWND, HDC, RECT *);
typedef HRESULT (__stdcall *PFNWXUENABLETHEMING)(BOOL);

class WXDLLEXPORT wxUxThemeEngine
{
private:
    wxUxThemeEngine() ;

public:
    ~wxUxThemeEngine() ;
    bool IsInitialized() const { return m_bIsInitialized ; }

private:
    bool m_bIsInitialized ;
    HINSTANCE m_hInstUXThemeDLL ;

public:
    static wxUxThemeEngine* wxInitUxThemeEngine() ;
    static wxUxThemeEngine* Get() ;
protected:
    void ResetFunctionPointers() ;
public:
    PFNWXUOPENTHEMEDATA m_pfnOpenThemeData ;
    PFNWXUCLOSETHEMEDATA m_pfnCloseThemeData ;
    PFNWXUDRAWTHEMEBACKGROUND m_pfnDrawThemeBackground ;
    PFNWXUDRAWTHEMETEXT m_pfnDrawThemeText ;
    PFNWXUGETTHEMEBACKGROUNDCONTENTRECT m_pfnGetThemeBackgroundContentRect ;
    PFNWXUGETTHEMEBACKGROUNDEXTENT m_pfnGetThemeBackgroundExtent ;
    PFNWXUGETTHEMEPARTSIZE m_pfnGetThemePartSize ;
    PFNWXUGETTHEMETEXTEXTENT m_pfnGetThemeTextExtent ;
    PFNWXUGETTHEMETEXTMETRICS m_pfnGetThemeTextMetrics ;
    PFNWXUGETTHEMEBACKGROUNDREGION m_pfnGetThemeBackgroundRegion ;
    PFNWXUHITTESTTHEMEBACKGROUND m_pfnHitTestThemeBackground ;
    PFNWXUDRAWTHEMEEDGE m_pfnDrawThemeEdge ;
    PFNWXUDRAWTHEMEICON m_pfnDrawThemeIcon ;
    PFNWXUISTHEMEPARTDEFINED m_pfnIsThemePartDefined ;
    PFNWXUISTHEMEBACKGROUNDPARTIALLYTRANSPARENT m_pfnIsThemeBackgroundPartiallyTransparent ;
    PFNWXUGETTHEMECOLOR m_pfnGetThemeColor ;
    PFNWXUGETTHEMEMETRIC m_pfnGetThemeMetric ;
    PFNWXUGETTHEMESTRING m_pfnGetThemeString ;
    PFNWXUGETTHEMEBOOL m_pfnGetThemeBool ;
    PFNWXUGETTHEMEINT m_pfnGetThemeInt ;
    PFNWXUGETTHEMEENUMVALUE m_pfnGetThemeEnumValue ;
    PFNWXUGETTHEMEPOSITION m_pfnGetThemePosition ;
    PFNWXUGETTHEMEFONT m_pfnGetThemeFont ;
    PFNWXUGETTHEMERECT m_pfnGetThemeRect ;
    PFNWXUGETTHEMEMARGINS m_pfnGetThemeMargins ;
    PFNWXUGETTHEMEINTLIST m_pfnGetThemeIntList ;
    PFNWXUGETTHEMEPROPERTYORIGIN m_pfnGetThemePropertyOrigin ;
    PFNWXUSETWINDOWTHEME m_pfnSetWindowTheme ;
    PFNWXUGETTHEMEFILENAME m_pfnGetThemeFilename ;
    PFNWXUGETTHEMESYSCOLOR m_pfnGetThemeSysColor ;
    PFNWXUGETTHEMESYSCOLORBRUSH m_pfnGetThemeSysColorBrush ;
    PFNWXUGETTHEMESYSBOOL m_pfnGetThemeSysBool ;
    PFNWXUGETTHEMESYSSIZE m_pfnGetThemeSysSize ;
    PFNWXUGETTHEMESYSFONT m_pfnGetThemeSysFont ;
    PFNWXUGETTHEMESYSSTRING m_pfnGetThemeSysString ;
    PFNWXUGETTHEMESYSINT m_pfnGetThemeSysInt ;
    PFNWXUISTHEMEACTIVE m_pfnIsThemeActive ;
    PFNWXUISAPPTHEMED m_pfnIsAppThemed ;
    PFNWXUGETWINDOWTHEME m_pfnGetWindowTheme ;
    PFNWXUENABLETHEMEDIALOGTEXTURE m_pfnEnableThemeDialogTexture ;
    PFNWXUISTHEMEDIALOGTEXTUREENABLED m_pfnIsThemeDialogTextureEnabled ;
    PFNWXUGETTHEMEAPPPROPERTIES m_pfnGetThemeAppProperties ;
    PFNWXUSETTHEMEAPPPROPERTIES m_pfnSetThemeAppProperties ;
    PFNWXUGETCURRENTTHEMENAME m_pfnGetCurrentThemeName ;
    PFNWXUGETTHEMEDOCUMENTATIONPROPERTY m_pfnGetThemeDocumentationProperty ;
    PFNWXUDRAWTHEMEPARENTBACKGROUND m_pfnDrawThemeParentBackground ;
    PFNWXUENABLETHEMING m_pfnEnableTheming ;
};

WXDLLEXPORT_DATA(extern wxUxThemeEngine*) g_pThemeEngine ;

WXDLLEXPORT BOOL wxCanUseInitThemeEngine() ;

#if !defined(WXU_USE_WXMODULE)
WXDLLEXPORT BOOL wxUxInitThemeEngine() ;
#endif

#endif

