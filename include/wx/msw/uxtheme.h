///////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/msw/uxtheme.h
// Purpose:     wxUxThemeEngine class: support for XP themes
// Author:
// Modified by:
// Created:     2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 wxWidgets Dev-Team
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UXTHEME_H_
#define _WX_UXTHEME_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
  #pragma interface "uxtheme.h"
#endif

#include "wx/defs.h"

#include "wx/msw/wrapwin.h"

#if wxUSE_UNICODE
typedef struct _WXUOSVERSIONINFOEXW {
    WXDWORD        dwOSVersionInfoSize;
    WXDWORD        dwMajorVersion;
    WXDWORD        dwMinorVersion;
    WXDWORD        dwBuildNumber;
    WXDWORD        dwPlatformId;
    wchar_t        szCSDVersion[ 128 ];     // Maintenance string for PSS usage
    unsigned short wServicePackMajor;
    unsigned short wServicePackMinor;
    unsigned short wSuiteMask;
    unsigned char  wProductType;
    unsigned char  wReserved;
} WXUOSVERSIONINFOEXW, *PWXUOSVERSIONINFOEXW, *LPWXUOSVERSIONINFOEXW;
#define WXUOSVERSIONINFOEX WXUOSVERSIONINFOEXW
#define PWXUOSVERSIONINFOEX PWXUOSVERSIONINFOEXW
#define LPWXUOSVERSIONINFOEX LPWXUOSVERSIONINFOEXW
#else
typedef struct _WXUOSVERSIONINFOEXA {
    WXDWORD        dwOSVersionInfoSize;
    WXDWORD        dwMajorVersion;
    WXDWORD        dwMinorVersion;
    WXDWORD        dwBuildNumber;
    WXDWORD        dwPlatformId;
    char           szCSDVersion [128];     // Maintenance string for PSS usage
    unsigned short wServicePackMajor;
    unsigned short wServicePackMinor;
    unsigned short wSuiteMask;
    unsigned char  wProductType;
    unsigned char  wReserved;
} WXUOSVERSIONINFOEXA, *PWXUOSVERSIONINFOEXA, *LPWXUOSVERSIONINFOEXA;
#define WXUOSVERSIONINFOEX WXUOSVERSIONINFOEXA
#define PWXUOSVERSIONINFOEX PWXUOSVERSIONINFOEXA
#define LPWXUOSVERSIONINFOEX LPWXUOSVERSIONINFOEXA
#endif

typedef void* WXHTHEME;
typedef long WXUHRESULT;
typedef WXHTHEME (__stdcall *PFNWXUOPENTHEMEDATA)(WXHWND, const wchar_t *);
typedef WXUHRESULT (__stdcall *PFNWXUCLOSETHEMEDATA)(WXHTHEME);
typedef WXUHRESULT (__stdcall *PFNWXUDRAWTHEMEBACKGROUND)(WXHTHEME, WXHDC,
    int, int, const RECT *, const RECT *);
#define WXU_DTT_GRAYED 0x1
typedef WXUHRESULT (__stdcall *PFNWXUDRAWTHEMETEXT)(WXHTHEME, WXHDC, int,
    int, const wchar_t *, int, DWORD,
    DWORD, const RECT *);
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEBACKGROUNDCONTENTRECT)(WXHTHEME, WXHDC,
    int, int, const RECT *, RECT *);
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEBACKGROUNDEXTENT)(WXHTHEME, WXHDC, int, int,
    const RECT *, RECT *);

enum WXUTHEMESIZE
{
    WXU_TS_MIN,             // minimum size
    WXU_TS_TRUE,            // size without stretching
    WXU_TS_DRAW             // size that theme mgr will use to draw part
};

typedef struct tagWXUSIZE
{
    long        cx;
    long        cy;
} WXUSIZE;

typedef struct tagWXUTEXTMETRIC
{
    long          tmHeight;
    long          tmAscent;
    long          tmDescent;
    long          tmInternalLeading;
    long          tmExternalLeading;
    long          tmAveCharWidth;
    long          tmMaxCharWidth;
    long          tmWeight;
    long          tmOverhang;
    long          tmDigitizedAspectX;
    long          tmDigitizedAspectY;
    wchar_t       tmFirstChar;
    wchar_t       tmLastChar;
    wchar_t       tmDefaultChar;
    wchar_t       tmBreakChar;
    unsigned char tmItalic;
    unsigned char tmUnderlined;
    unsigned char tmStruckOut;
    unsigned char tmPitchAndFamily;
    unsigned char tmCharSet;
} WXUTEXTMETRIC;

typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEPARTSIZE)(WXHTHEME, WXHDC, int, int, const RECT *,
    enum WXUTHEMESIZE, WXUSIZE *);
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMETEXTEXTENT)(WXHTHEME, WXHDC, int, int, const wchar_t *,
    int, WXDWORD, const RECT *, RECT *);
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMETEXTMETRICS)(WXHTHEME, WXHDC,
    int, int, WXUTEXTMETRIC*);
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEBACKGROUNDREGION)(WXHTHEME, WXHDC,
    int, int, const RECT *, WXHRGN *);

#define WXU_HTTB_BACKGROUNDSEG          0x0000
#define WXU_HTTB_FIXEDBORDER            0x0002
#define WXU_HTTB_CAPTION                0x0004
#define WXU_HTTB_RESIZINGBORDER_LEFT    0x0010
#define WXU_HTTB_RESIZINGBORDER_TOP     0x0020
#define WXU_HTTB_RESIZINGBORDER_RIGHT   0x0040
#define WXU_HTTB_RESIZINGBORDER_BOTTOM  0x0080
#define WXU_HTTB_RESIZINGBORDER         0x00F0
#define WXU_HTTB_SIZINGTEMPLATE         0x0100
#define WXU_HTTB_SYSTEMSIZINGMARGINS    0x0200

typedef struct tagWXUPOINT
{
    long x;
    long y;
} WXUPOINT;

typedef WXUHRESULT (__stdcall *PFNWXUHITTESTTHEMEBACKGROUND)(WXHTHEME, WXHDC, int,
    int, WXDWORD, const RECT *, WXHRGN,
    WXUPOINT, unsigned short *);
typedef WXUHRESULT (__stdcall *PFNWXUDRAWTHEMEEDGE)(WXHTHEME, WXHDC, int, int,
    const RECT *, unsigned int, unsigned int, RECT *);
typedef WXUHRESULT (__stdcall *PFNWXUDRAWTHEMEICON)(WXHTHEME, WXHDC, int, int,
    const RECT *, WXHIMAGELIST, int);
typedef BOOL (__stdcall *PFNWXUISTHEMEPARTDEFINED)(WXHTHEME, int, int);
typedef BOOL (__stdcall *PFNWXUISTHEMEBACKGROUNDPARTIALLYTRANSPARENT)(WXHTHEME, int, int);
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMECOLOR)(WXHTHEME, int, int, int, WXCOLORREF*);
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEMETRIC)(WXHTHEME, WXHDC, int,
    int, int, int *);
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMESTRING)(WXHTHEME, int,
    int, int, wchar_t *, int);
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEBOOL)(WXHTHEME, int, int, int, BOOL *);
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEINT)(WXHTHEME, int, int, int, int *);
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEENUMVALUE)(WXHTHEME, int, int, int, int *);
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEPOSITION)(WXHTHEME, int, int, int, WXUPOINT *);
typedef struct tagWXULOGFONT
{
    long          lfHeight;
    long          lfWidth;
    long          lfEscapement;
    long          lfOrientation;
    long          lfWeight;
    unsigned char lfItalic;
    unsigned char lfUnderline;
    unsigned char lfStrikeOut;
    unsigned char lfCharSet;
    unsigned char lfOutPrecision;
    unsigned char lfClipPrecision;
    unsigned char lfQuality;
    unsigned char lfPitchAndFamily;
    wchar_t       lfFaceName[32];
} WXULOGFONT;
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEFONT)(WXHTHEME, WXHDC, int, int, int, WXULOGFONT *);
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMERECT)(WXHTHEME, int, int, int, RECT *);
typedef struct _WXUMARGINS
{
    int cxLeftWidth;      // width of left border that retains its size
    int cxRightWidth;     // width of right border that retains its size
    int cyTopHeight;      // height of top border that retains its size
    int cyBottomHeight;   // height of bottom border that retains its size
} WXUMARGINS, *PWXUMARGINS;
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEMARGINS)(WXHTHEME, WXHDC, int,
    int, int, RECT *, WXUMARGINS *);

#define WXU_MAX_INTLIST_COUNT 10
typedef struct _WXUINTLIST
{
    int iValueCount;      // number of values in iValues
    int iValues[WXU_MAX_INTLIST_COUNT];
} WXUINTLIST, *PWXUINTLIST;
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEINTLIST)(WXHTHEME, int,
    int, int, WXUINTLIST*);

enum WXUPROPERTYORIGIN
{
    WXU_PO_STATE,           // property was found in the state section
    WXU_PO_PART,            // property was found in the part section
    WXU_PO_CLASS,           // property was found in the class section
    WXU_PO_GLOBAL,          // property was found in [globals] section
    WXU_PO_NOTFOUND         // property was not found
};

typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEPROPERTYORIGIN)(WXHTHEME, int,
    int, int, enum WXUPROPERTYORIGIN *);
typedef WXUHRESULT (__stdcall *PFNWXUSETWINDOWTHEME)(WXHWND, const wchar_t*, const wchar_t *);
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEFILENAME)(WXHTHEME, int, int, int, wchar_t *, int);
typedef WXCOLORREF (__stdcall *PFNWXUGETTHEMESYSCOLOR)(WXHTHEME, int);
typedef WXHBRUSH (__stdcall *PFNWXUGETTHEMESYSCOLORBRUSH)(WXHTHEME, int);
typedef BOOL (__stdcall *PFNWXUGETTHEMESYSBOOL)(WXHTHEME, int);
typedef int (__stdcall *PFNWXUGETTHEMESYSSIZE)(WXHTHEME, int);
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMESYSFONT)(WXHTHEME, int, WXULOGFONT *);
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMESYSSTRING)(WXHTHEME, int, wchar_t *, int);
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMESYSINT)(WXHTHEME, int, int *);
typedef BOOL (__stdcall *PFNWXUISTHEMEACTIVE)();
typedef BOOL (__stdcall *PFNWXUISAPPTHEMED)();
typedef WXHTHEME (__stdcall *PFNWXUGETWINDOWTHEME)(WXHWND);

#define WXU_ETDT_DISABLE        0x00000001
#define WXU_ETDT_ENABLE         0x00000002
#define WXU_ETDT_USETABTEXTURE  0x00000004
#define WXU_ETDT_ENABLETAB      0x00000006

typedef WXUHRESULT (__stdcall *PFNWXUENABLETHEMEDIALOGTEXTURE)(WXHWND, WXDWORD);
typedef BOOL (__stdcall *PFNWXUISTHEMEDIALOGTEXTUREENABLED)(WXHWND);

#define WXU_STAP_ALLOW_NONCLIENT    1
#define WXU_STAP_ALLOW_CONTROLS     2
#define WXU_STAP_ALLOW_WEBCONTENT   4

typedef WXDWORD (__stdcall *PFNWXUGETTHEMEAPPPROPERTIES)();
typedef void (__stdcall *PFNWXUSETTHEMEAPPPROPERTIES)(WXDWORD);
typedef WXUHRESULT (__stdcall *PFNWXUGETCURRENTTHEMENAME)(wchar_t *, int,
    wchar_t *, int, wchar_t *, int);

#define WXU_SZ_THDOCPROP_DISPLAYNAME                L"DisplayName"
#define WXU_SZ_THDOCPROP_CANONICALNAME              L"ThemeName"
#define WXU_SZ_THDOCPROP_TOOLTIP                    L"ToolTip"
#define WXU_SZ_THDOCPROP_AUTHOR                     L"author"

typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEDOCUMENTATIONPROPERTY)(const wchar_t *,
    const wchar_t *, wchar_t *, int);
typedef WXUHRESULT (__stdcall *PFNWXUDRAWTHEMEPARENTBACKGROUND)(WXHWND, WXHDC, RECT *);
typedef WXUHRESULT (__stdcall *PFNWXUENABLETHEMING)(BOOL);

// ----------------------------------------------------------------------------
// wxUxThemeEngine: provides all theme functions from uxtheme.dll
// ----------------------------------------------------------------------------

// we always define this class, even if wxUSE_UXTHEME == 0, but we just make it
// empty in this case -- this allows to use it elsewhere without any #ifdefs
#if wxUSE_UXTHEME
    #include "wx/dynload.h"

    #define wxUX_THEME_DECLARE(type, func) type func;
#else
    #define wxUX_THEME_DECLARE(type, func) type func(...) { return 0; }
#endif

class WXDLLEXPORT wxUxThemeEngine
{
public:
    // get the theme engine or NULL if themes are not available
    static wxUxThemeEngine *Get();

    // get the theme enging or NULL if themes are not available or not used for
    // this application
    static wxUxThemeEngine *GetIfActive();

    // all uxtheme.dll functions
    wxUX_THEME_DECLARE(PFNWXUOPENTHEMEDATA, OpenThemeData)
    wxUX_THEME_DECLARE(PFNWXUCLOSETHEMEDATA, CloseThemeData)
    wxUX_THEME_DECLARE(PFNWXUDRAWTHEMEBACKGROUND, DrawThemeBackground)
    wxUX_THEME_DECLARE(PFNWXUDRAWTHEMETEXT, DrawThemeText)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMEBACKGROUNDCONTENTRECT, GetThemeBackgroundContentRect)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMEBACKGROUNDEXTENT, GetThemeBackgroundExtent)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMEPARTSIZE, GetThemePartSize)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMETEXTEXTENT, GetThemeTextExtent)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMETEXTMETRICS, GetThemeTextMetrics)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMEBACKGROUNDREGION, GetThemeBackgroundRegion)
    wxUX_THEME_DECLARE(PFNWXUHITTESTTHEMEBACKGROUND, HitTestThemeBackground)
    wxUX_THEME_DECLARE(PFNWXUDRAWTHEMEEDGE, DrawThemeEdge)
    wxUX_THEME_DECLARE(PFNWXUDRAWTHEMEICON, DrawThemeIcon)
    wxUX_THEME_DECLARE(PFNWXUISTHEMEPARTDEFINED, IsThemePartDefined)
    wxUX_THEME_DECLARE(PFNWXUISTHEMEBACKGROUNDPARTIALLYTRANSPARENT, IsThemeBackgroundPartiallyTransparent)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMECOLOR, GetThemeColor)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMEMETRIC, GetThemeMetric)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMESTRING, GetThemeString)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMEBOOL, GetThemeBool)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMEINT, GetThemeInt)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMEENUMVALUE, GetThemeEnumValue)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMEPOSITION, GetThemePosition)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMEFONT, GetThemeFont)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMERECT, GetThemeRect)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMEMARGINS, GetThemeMargins)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMEINTLIST, GetThemeIntList)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMEPROPERTYORIGIN, GetThemePropertyOrigin)
    wxUX_THEME_DECLARE(PFNWXUSETWINDOWTHEME, SetWindowTheme)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMEFILENAME, GetThemeFilename)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMESYSCOLOR, GetThemeSysColor)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMESYSCOLORBRUSH, GetThemeSysColorBrush)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMESYSBOOL, GetThemeSysBool)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMESYSSIZE, GetThemeSysSize)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMESYSFONT, GetThemeSysFont)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMESYSSTRING, GetThemeSysString)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMESYSINT, GetThemeSysInt)
    wxUX_THEME_DECLARE(PFNWXUISTHEMEACTIVE, IsThemeActive)
    wxUX_THEME_DECLARE(PFNWXUISAPPTHEMED, IsAppThemed)
    wxUX_THEME_DECLARE(PFNWXUGETWINDOWTHEME, GetWindowTheme)
    wxUX_THEME_DECLARE(PFNWXUENABLETHEMEDIALOGTEXTURE, EnableThemeDialogTexture)
    wxUX_THEME_DECLARE(PFNWXUISTHEMEDIALOGTEXTUREENABLED, IsThemeDialogTextureEnabled)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMEAPPPROPERTIES, GetThemeAppProperties)
    wxUX_THEME_DECLARE(PFNWXUSETTHEMEAPPPROPERTIES, SetThemeAppProperties)
    wxUX_THEME_DECLARE(PFNWXUGETCURRENTTHEMENAME, GetCurrentThemeName)
    wxUX_THEME_DECLARE(PFNWXUGETTHEMEDOCUMENTATIONPROPERTY, GetThemeDocumentationProperty)
    wxUX_THEME_DECLARE(PFNWXUDRAWTHEMEPARENTBACKGROUND, DrawThemeParentBackground)
    wxUX_THEME_DECLARE(PFNWXUENABLETHEMING, EnableTheming)

private:
    // construcor is private as only Get() can create us and is also trivial as
    // everything really happens in Initialize()
    wxUxThemeEngine() { }

    // destructor is private as only Get() and wxUxThemeModule delete us, it is
    // not virtual as we're not supposed to be derived from
    ~wxUxThemeEngine() { }

#if wxUSE_UXTHEME
    // initialize the theme engine: load the DLL, resolve the functions
    //
    // return true if we can be used, false if themes are not available
    bool Initialize();


    // uxtheme.dll
    wxDynamicLibrary m_dllUxTheme;


    // the one and only theme engine, initially NULL
    static wxUxThemeEngine *ms_themeEngine;

    // this is a bool which initially has the value -1 meaning "unknown"
    static int ms_isThemeEngineAvailable;

    // it must be able to delete us
    friend class wxUxThemeModule;
#endif // wxUSE_UXTHEME

    DECLARE_NO_COPY_CLASS(wxUxThemeEngine)
};

#if wxUSE_UXTHEME

/* static */ inline wxUxThemeEngine *wxUxThemeEngine::GetIfActive()
{
    wxUxThemeEngine *engine = Get();
    return engine && engine->IsAppThemed() && engine->IsThemeActive()
                ? engine
                : NULL;
}

#else // !wxUSE_UXTHEME

/* static */ inline wxUxThemeEngine *wxUxThemeEngine::Get()
{
    return NULL;
}

/* static */ inline wxUxThemeEngine *wxUxThemeEngine::GetIfActive()
{
    return NULL;
}

#endif // wxUSE_UXTHEME/!wxUSE_UXTHEME

// ----------------------------------------------------------------------------
// wxUxThemeHandle: encapsulates ::Open/CloseThemeData()
// ----------------------------------------------------------------------------

class wxUxThemeHandle
{
public:
    wxUxThemeHandle(wxWindow *win, const wchar_t *classes)
    {
        wxUxThemeEngine *engine = wxUxThemeEngine::Get();

        m_hTheme =
            engine ? (WXHTHEME)engine->OpenThemeData(win->GetHWND(), classes)
                   : NULL;
    }

    operator WXHTHEME() const { return m_hTheme; }

    ~wxUxThemeHandle()
    {
        if ( m_hTheme )
        {
            wxUxThemeEngine::Get()->CloseThemeData(m_hTheme);
        }
    }

private:
    WXHTHEME m_hTheme;

    DECLARE_NO_COPY_CLASS(wxUxThemeHandle)
};

#endif // _WX_UXTHEME_H_

