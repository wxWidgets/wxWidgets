/*
 * wxuxthem.h : wxWindows support for Microsoft Windows XP Theme library
 *                (uxtheme.dll)
 * Modified to work with almost all Win32 compilers
 * Compiler must support Unicode, the __cdecl calling convention, and the
 * __stdcall calling convention
 */

#ifndef _WX_UXTHEME_H_
#define _WX_UXTHEME_H_

#ifdef __GNUG__
  #pragma interface "uxtheme.cpp"
#endif

// Use wxModule approach to initialization.
#define WXU_USE_WXMODULE 1

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

typedef int WXUBOOL ;

typedef struct tagWXURECT
{
    long    left;
    long    top;
    long    right;
    long    bottom;
} WXURECT ;

typedef void* WXHTHEME;
typedef long WXUHRESULT;
typedef WXHTHEME (__stdcall *PFNWXUOPENTHEMEDATA)(WXHWND, const wchar_t *) ; 
typedef WXUHRESULT (__stdcall *PFNWXUCLOSETHEMEDATA)(WXHTHEME) ;
typedef WXUHRESULT (__stdcall *PFNWXUDRAWTHEMEBACKGROUND)(WXHTHEME, WXHDC, 
    int, int, const WXURECT *, const WXURECT *) ;
#define WXU_DTT_GRAYED 0x1
typedef WXUHRESULT (__stdcall *PFNWXUDRAWTHEMETEXT)(WXHTHEME, WXHDC, int, 
    int, const wchar_t *, int, DWORD, 
    DWORD, const WXURECT *) ;
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEBACKGROUNDCONTENTRECT)(WXHTHEME, WXHDC, 
    int, int, const WXURECT *, WXURECT *) ;
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEBACKGROUNDEXTENT)(WXHTHEME, WXHDC, int, int, 
    const WXURECT *, WXURECT *) ;

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
} WXUSIZE ;

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

typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEPARTSIZE)(WXHTHEME, WXHDC, int, int, const WXURECT *, 
    enum WXUTHEMESIZE, WXUSIZE *) ;
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMETEXTEXTENT)(WXHTHEME, WXHDC, int, int, const wchar_t *, 
    int, WXDWORD, const WXURECT *, WXURECT *) ;
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMETEXTMETRICS)(WXHTHEME, WXHDC,
    int, int, WXUTEXTMETRIC*);
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEBACKGROUNDREGION)(WXHTHEME, WXHDC,
    int, int, const WXURECT *, WXHRGN *) ;

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
} WXUPOINT ;

typedef WXUHRESULT (__stdcall *PFNWXUHITTESTTHEMEBACKGROUND)(WXHTHEME, WXHDC, int,
    int, WXDWORD, const WXURECT *, WXHRGN,
    WXUPOINT, unsigned short *) ;
typedef WXUHRESULT (__stdcall *PFNWXUDRAWTHEMEEDGE)(WXHTHEME, WXHDC, int, int,
    const WXURECT *, unsigned int, unsigned int, WXURECT *);
typedef WXUHRESULT (__stdcall *PFNWXUDRAWTHEMEICON)(WXHTHEME, WXHDC, int, int,
    const WXURECT *, WXHIMAGELIST, int) ;
typedef WXUBOOL (__stdcall *PFNWXUISTHEMEPARTDEFINED)(WXHTHEME, int, int) ;
typedef WXUBOOL (__stdcall *PFNWXUISTHEMEBACKGROUNDPARTIALLYTRANSPARENT)(WXHTHEME, int, int) ;
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMECOLOR)(WXHTHEME, int, int, int, WXCOLORREF*) ;
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEMETRIC)(WXHTHEME, WXHDC, int,
    int, int, int *) ;
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMESTRING)(WXHTHEME, int,
    int, int, wchar_t *, int) ;
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEBOOL)(WXHTHEME, int, int, int, WXUBOOL *) ;
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEINT)(WXHTHEME, int, int, int, int *) ;
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEENUMVALUE)(WXHTHEME, int, int, int, int *) ;
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEPOSITION)(WXHTHEME, int, int, int, WXUPOINT *) ;
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
} WXULOGFONT ;
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEFONT)(WXHTHEME, WXHDC, int, int, int, WXULOGFONT *) ;
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMERECT)(WXHTHEME, int, int, int, WXURECT *) ;
typedef struct _WXUMARGINS
{
    int cxLeftWidth;      // width of left border that retains its size
    int cxRightWidth;     // width of right border that retains its size
    int cyTopHeight;      // height of top border that retains its size
    int cyBottomHeight;   // height of bottom border that retains its size
} WXUMARGINS, *PWXUMARGINS;
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEMARGINS)(WXHTHEME, WXHDC, int,
    int, int, WXURECT *, WXUMARGINS *) ;

#define WXU_MAX_INTLIST_COUNT 10
typedef struct _WXUINTLIST
{
    int iValueCount;      // number of values in iValues
    int iValues[WXU_MAX_INTLIST_COUNT];
} WXUINTLIST, *PWXUINTLIST;
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEINTLIST)(WXHTHEME, int,
	int, int, WXUINTLIST*) ;

enum WXUPROPERTYORIGIN
{
    WXU_PO_STATE,           // property was found in the state section
    WXU_PO_PART,            // property was found in the part section
    WXU_PO_CLASS,           // property was found in the class section
    WXU_PO_GLOBAL,          // property was found in [globals] section
    WXU_PO_NOTFOUND         // property was not found
};

typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEPROPERTYORIGIN)(WXHTHEME, int,
    int, int, enum WXUPROPERTYORIGIN *) ;
typedef WXUHRESULT (__stdcall *PFNWXUSETWINDOWTHEME)(WXHWND, const wchar_t*, const wchar_t *) ;
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEFILENAME)(WXHTHEME, int, int, int, wchar_t *, int) ;
typedef WXCOLORREF (__stdcall *PFNWXUGETTHEMESYSCOLOR)(WXHTHEME, int) ;
typedef WXHBRUSH (__stdcall *PFNWXUGETTHEMESYSCOLORBRUSH)(WXHTHEME, int) ;
typedef WXUBOOL (__stdcall *PFNWXUGETTHEMESYSBOOL)(WXHTHEME, int) ;
typedef int (__stdcall *PFNWXUGETTHEMESYSSIZE)(WXHTHEME, int) ;
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMESYSFONT)(WXHTHEME, int, WXULOGFONT *) ;
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMESYSSTRING)(WXHTHEME, int, wchar_t *, int) ;
typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMESYSINT)(WXHTHEME, int, int *) ;
typedef WXUBOOL (__stdcall *PFNWXUISTHEMEACTIVE)() ;
typedef WXUBOOL (__stdcall *PFNWXUISAPPTHEMED)() ;
typedef WXHTHEME (__stdcall *PFNWXUGETWINDOWTHEME)(WXHWND) ;

#define WXU_ETDT_DISABLE        0x00000001
#define WXU_ETDT_ENABLE         0x00000002
#define WXU_ETDT_USETABTEXTURE  0x00000004
#define WXU_ETDT_ENABLETAB      0x00000006

typedef WXUHRESULT (__stdcall *PFNWXUENABLETHEMEDIALOGTEXTURE)(WXHWND, WXDWORD) ;
typedef WXUBOOL (__stdcall *PFNWXUISTHEMEDIALOGTEXTUREENABLED)(WXHWND) ;

#define WXU_STAP_ALLOW_NONCLIENT    1
#define WXU_STAP_ALLOW_CONTROLS     2
#define WXU_STAP_ALLOW_WEBCONTENT   4

typedef WXDWORD (__stdcall *PFNWXUGETTHEMEAPPPROPERTIES)() ;
typedef void (__stdcall *PFNWXUSETTHEMEAPPPROPERTIES)(WXDWORD) ;
typedef WXUHRESULT (__stdcall *PFNWXUGETCURRENTTHEMENAME)(wchar_t *, int, 
    wchar_t *, int, wchar_t *, int) ;

#define WXU_SZ_THDOCPROP_DISPLAYNAME                L"DisplayName"
#define WXU_SZ_THDOCPROP_CANONICALNAME              L"ThemeName"
#define WXU_SZ_THDOCPROP_TOOLTIP                    L"ToolTip"
#define WXU_SZ_THDOCPROP_AUTHOR                     L"author"

typedef WXUHRESULT (__stdcall *PFNWXUGETTHEMEDOCUMENTATIONPROPERTY)(const wchar_t *,
    const wchar_t *, wchar_t *, int) ;
typedef WXUHRESULT (__stdcall *PFNWXUDRAWTHEMEPARENTBACKGROUND)(WXHWND, WXHDC, WXURECT *) ;
typedef WXUHRESULT (__stdcall *PFNWXUENABLETHEMING)(WXUBOOL) ;

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

