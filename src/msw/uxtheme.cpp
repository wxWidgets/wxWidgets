/*
 * wxuxthem.h : wxWindows support for Microsoft Windows XP Theme library
 *                (uxtheme.dll)
 * Modified to work with almost all Win32 compilers
 * Compiler must support Unicode, the __cdecl calling convention, and the
 * __stdcall calling convention
 */

#ifdef __GNUG__
#pragma implementation "uxtheme.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/toplevel.h"
    #include "wx/string.h"
    #include "wx/log.h"
#endif //WX_PRECOMP

#include "wx/module.h"

#if wxUSE_UXTHEME

#include <windows.h>

#include "wx/msw/winundef.h"
#include "wx/msw/uxtheme.h"
#include "wx/msw/private.h"
#include "wx/app.h"         // for GetComCtl32Version

wxUxThemeEngine* g_pThemeEngine = ((wxUxThemeEngine*)NULL) ;

BOOL wxCanUseInitThemeEngine()
{
    WXUOSVERSIONINFOEX wxuosex ;
    memset((void*)&wxuosex, 0, sizeof(WXUOSVERSIONINFOEX)) ;
    wxuosex.dwOSVersionInfoSize = sizeof(WXUOSVERSIONINFOEX) ;
    if(!GetVersionEx((LPOSVERSIONINFO)&wxuosex))
    {
        wxuosex.dwOSVersionInfoSize = sizeof(OSVERSIONINFO) ;
        if(!GetVersionEx((LPOSVERSIONINFO)&wxuosex))
        {
            return FALSE ;
        }
    }
    if ((wxuosex.dwMajorVersion == 5) && (wxuosex.dwMinorVersion > 0))
    {
        return (wxTheApp->GetComCtl32Version() >= 600) ;
    }
    else
    {
        return FALSE ;
    }
}

wxUxThemeEngine* wxUxThemeEngine::wxInitUxThemeEngine()
{
    wxUxThemeEngine* pThemeEngine = new wxUxThemeEngine ;
    return pThemeEngine ;
}

wxUxThemeEngine* wxUxThemeEngine::Get()
{
    return g_pThemeEngine;
}

#ifdef WXU_USE_WXMODULE
class wxUxThemeModule : public wxModule
{
	DECLARE_DYNAMIC_CLASS(wxUxThemeModule)
public:
	virtual void OnExit()
	{
		if((wxCanUseInitThemeEngine()) && (g_pThemeEngine))
		{
			delete g_pThemeEngine ;
			g_pThemeEngine = ((wxUxThemeEngine*)NULL) ;
		}
	}

	virtual bool OnInit()
	{
		if(!wxCanUseInitThemeEngine())
		{
			g_pThemeEngine = ((wxUxThemeEngine*)NULL) ;
			return TRUE ;
		}
		g_pThemeEngine = wxUxThemeEngine::wxInitUxThemeEngine() ;
		if(!g_pThemeEngine)
		{
			return FALSE ;
		}
		else if(!g_pThemeEngine->IsInitialized())
		{
			OnExit() ;
			return FALSE ;
		}

		return TRUE ;
	}
};

IMPLEMENT_DYNAMIC_CLASS(wxUxThemeModule, wxModule)
#else
static void __cdecl wxUxThemeEngineOnExit()
{
    if(g_pThemeEngine)
    {
        delete g_pThemeEngine ;
        g_pThemeEngine = ((wxUxThemeEngine*)NULL) ;
    }
}

BOOL wxUxInitThemeEngine()
{
    if(!wxCanUseInitThemeEngine())
    {
        return TRUE ;
    }
    g_pThemeEngine = wxUxThemeEngine::wxInitUxThemeEngine() ;
    if(!g_pThemeEngine)
    {
        return FALSE ;
    }
    else if(!g_pThemeEngine->IsInitialized())
    {
        wxUxThemeEngineOnExit() ;
        return FALSE ;
    }

    if(atexit(wxUxThemeEngineOnExit) != 0)
    {
        wxUxThemeEngineOnExit() ;
        return FALSE ;
    }

    return TRUE ;
}
#endif

#define WXU_GET_FN_PTR(hmod, type, pfunc, funcname) if(hmod != ((HINSTANCE)NULL)) { \
    pfunc = (type)::GetProcAddress(hmod, funcname) ; \
    if(pfunc == ((type)NULL)) \
    {	\
        ::FreeLibrary(hmod) ; \
        hmod = ((HINSTANCE)NULL) ; \
    }	\
}

wxUxThemeEngine::wxUxThemeEngine()
{
    m_hInstUXThemeDLL = ::LoadLibraryA("uxtheme.dll") ;
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUOPENTHEMEDATA, m_pfnOpenThemeData, "OpenThemeData")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUCLOSETHEMEDATA, m_pfnCloseThemeData, "CloseThemeData")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUDRAWTHEMEBACKGROUND, m_pfnDrawThemeBackground, "DrawThemeBackground")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUDRAWTHEMETEXT, m_pfnDrawThemeText, "DrawThemeText")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMEBACKGROUNDCONTENTRECT, m_pfnGetThemeBackgroundContentRect, "GetThemeBackgroundContentRect")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMEBACKGROUNDEXTENT, m_pfnGetThemeBackgroundExtent, "GetThemeBackgroundExtent")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMEPARTSIZE, m_pfnGetThemePartSize, "GetThemePartSize")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMETEXTEXTENT, m_pfnGetThemeTextExtent, "GetThemeTextExtent")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMETEXTMETRICS, m_pfnGetThemeTextMetrics, "GetThemeTextMetrics")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMEBACKGROUNDREGION, m_pfnGetThemeBackgroundRegion, "GetThemeBackgroundRegion")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUHITTESTTHEMEBACKGROUND, m_pfnHitTestThemeBackground, "HitTestThemeBackground")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUDRAWTHEMEEDGE, m_pfnDrawThemeEdge, "DrawThemeEdge")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUDRAWTHEMEICON, m_pfnDrawThemeIcon, "DrawThemeIcon")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUISTHEMEPARTDEFINED, m_pfnIsThemePartDefined, "IsThemePartDefined")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUISTHEMEBACKGROUNDPARTIALLYTRANSPARENT, m_pfnIsThemeBackgroundPartiallyTransparent, "IsThemeBackgroundPartiallyTransparent")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMECOLOR, m_pfnGetThemeColor, "GetThemeColor")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMEMETRIC, m_pfnGetThemeMetric, "GetThemeMetric")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMESTRING, m_pfnGetThemeString, "GetThemeString")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMEBOOL, m_pfnGetThemeBool, "GetThemeBool")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMEINT, m_pfnGetThemeInt, "GetThemeInt")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMEENUMVALUE, m_pfnGetThemeEnumValue, "GetThemeEnumValue")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMEPOSITION, m_pfnGetThemePosition, "GetThemePosition")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMEFONT, m_pfnGetThemeFont, "GetThemeFont")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMERECT, m_pfnGetThemeRect, "GetThemeRect")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMEMARGINS, m_pfnGetThemeMargins, "GetThemeMargins")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMEINTLIST, m_pfnGetThemeIntList, "GetThemeIntList")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMEPROPERTYORIGIN, m_pfnGetThemePropertyOrigin, "GetThemePropertyOrigin")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUSETWINDOWTHEME, m_pfnSetWindowTheme, "SetWindowTheme")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMEFILENAME, m_pfnGetThemeFilename, "GetThemeFilename")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMESYSCOLOR, m_pfnGetThemeSysColor, "GetThemeSysColor")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMESYSCOLORBRUSH, m_pfnGetThemeSysColorBrush, "GetThemeSysColorBrush")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMESYSBOOL, m_pfnGetThemeSysBool, "GetThemeSysBool")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMESYSSIZE, m_pfnGetThemeSysSize, "GetThemeSysSize")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMESYSFONT, m_pfnGetThemeSysFont, "GetThemeSysFont")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMESYSSTRING, m_pfnGetThemeSysString, "GetThemeSysString")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMESYSINT, m_pfnGetThemeSysInt, "GetThemeSysInt")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUISTHEMEACTIVE, m_pfnIsThemeActive, "IsThemeActive")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUISAPPTHEMED, m_pfnIsAppThemed, "IsAppThemed")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETWINDOWTHEME, m_pfnGetWindowTheme, "GetWindowTheme")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUENABLETHEMEDIALOGTEXTURE, m_pfnEnableThemeDialogTexture, "EnableThemeDialogTexture")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUISTHEMEDIALOGTEXTUREENABLED, m_pfnIsThemeDialogTextureEnabled, "IsThemeDialogTextureEnabled")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMEAPPPROPERTIES, m_pfnGetThemeAppProperties, "GetThemeAppProperties")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUSETTHEMEAPPPROPERTIES, m_pfnSetThemeAppProperties, "SetThemeAppProperties")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETCURRENTTHEMENAME, m_pfnGetCurrentThemeName, "GetCurrentThemeName")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUGETTHEMEDOCUMENTATIONPROPERTY, m_pfnGetThemeDocumentationProperty, "GetThemeDocumentationProperty")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUDRAWTHEMEPARENTBACKGROUND, m_pfnDrawThemeParentBackground, "DrawThemeParentBackground")
    WXU_GET_FN_PTR (m_hInstUXThemeDLL, PFNWXUENABLETHEMING, m_pfnEnableTheming, "EnableTheming")
    m_bIsInitialized = (m_hInstUXThemeDLL != ((HINSTANCE)NULL)) ;
    if(!m_bIsInitialized)
    {
        ResetFunctionPointers() ;
    }
}

void wxUxThemeEngine::ResetFunctionPointers()
{
    m_pfnOpenThemeData = ((PFNWXUOPENTHEMEDATA)NULL) ;
    m_pfnCloseThemeData = ((PFNWXUCLOSETHEMEDATA)NULL) ;
    m_pfnDrawThemeBackground = ((PFNWXUDRAWTHEMEBACKGROUND)NULL) ;
    m_pfnDrawThemeText = ((PFNWXUDRAWTHEMETEXT)NULL) ;
    m_pfnGetThemeBackgroundContentRect = ((PFNWXUGETTHEMEBACKGROUNDCONTENTRECT)NULL) ;
    m_pfnGetThemeBackgroundExtent = ((PFNWXUGETTHEMEBACKGROUNDEXTENT)NULL) ;
    m_pfnGetThemePartSize = ((PFNWXUGETTHEMEPARTSIZE)NULL) ;
    m_pfnGetThemeTextExtent = ((PFNWXUGETTHEMETEXTEXTENT)NULL) ;
    m_pfnGetThemeTextMetrics = ((PFNWXUGETTHEMETEXTMETRICS)NULL) ;
    m_pfnGetThemeBackgroundRegion = ((PFNWXUGETTHEMEBACKGROUNDREGION)NULL) ;
    m_pfnHitTestThemeBackground = ((PFNWXUHITTESTTHEMEBACKGROUND)NULL) ;
    m_pfnDrawThemeEdge = ((PFNWXUDRAWTHEMEEDGE)NULL) ;
    m_pfnDrawThemeIcon = ((PFNWXUDRAWTHEMEICON)NULL) ;
    m_pfnIsThemePartDefined = ((PFNWXUISTHEMEPARTDEFINED)NULL) ;
    m_pfnIsThemeBackgroundPartiallyTransparent = ((PFNWXUISTHEMEBACKGROUNDPARTIALLYTRANSPARENT)NULL) ;
    m_pfnGetThemeColor = ((PFNWXUGETTHEMECOLOR)NULL) ;
    m_pfnGetThemeMetric = ((PFNWXUGETTHEMEMETRIC)NULL) ;
    m_pfnGetThemeString = ((PFNWXUGETTHEMESTRING)NULL) ;
    m_pfnGetThemeBool = ((PFNWXUGETTHEMEBOOL)NULL) ;
    m_pfnGetThemeInt = ((PFNWXUGETTHEMEINT)NULL) ;
    m_pfnGetThemeEnumValue = ((PFNWXUGETTHEMEENUMVALUE)NULL) ;
    m_pfnGetThemePosition = ((PFNWXUGETTHEMEPOSITION)NULL) ;
    m_pfnGetThemeFont = ((PFNWXUGETTHEMEFONT)NULL) ;
    m_pfnGetThemeRect = ((PFNWXUGETTHEMERECT)NULL) ;
    m_pfnGetThemeMargins = ((PFNWXUGETTHEMEMARGINS)NULL) ;
    m_pfnGetThemeIntList = ((PFNWXUGETTHEMEINTLIST)NULL) ;
    m_pfnGetThemePropertyOrigin = ((PFNWXUGETTHEMEPROPERTYORIGIN)NULL) ;
    m_pfnSetWindowTheme = ((PFNWXUSETWINDOWTHEME)NULL) ;
    m_pfnGetThemeFilename = ((PFNWXUGETTHEMEFILENAME)NULL) ;
    m_pfnGetThemeSysColor = ((PFNWXUGETTHEMESYSCOLOR)NULL) ;
    m_pfnGetThemeSysColorBrush = ((PFNWXUGETTHEMESYSCOLORBRUSH)NULL) ;
    m_pfnGetThemeSysBool = ((PFNWXUGETTHEMESYSBOOL)NULL) ;
    m_pfnGetThemeSysSize = ((PFNWXUGETTHEMESYSSIZE)NULL) ;
    m_pfnGetThemeSysFont = ((PFNWXUGETTHEMESYSFONT)NULL) ;
    m_pfnGetThemeSysString = ((PFNWXUGETTHEMESYSSTRING)NULL) ;
    m_pfnGetThemeSysInt = ((PFNWXUGETTHEMESYSINT)NULL) ;
    m_pfnIsThemeActive = ((PFNWXUISTHEMEACTIVE)NULL) ;
    m_pfnIsAppThemed = ((PFNWXUISAPPTHEMED)NULL) ;
    m_pfnGetWindowTheme = ((PFNWXUGETWINDOWTHEME)NULL) ;
    m_pfnEnableThemeDialogTexture = ((PFNWXUENABLETHEMEDIALOGTEXTURE)NULL) ;
    m_pfnIsThemeDialogTextureEnabled = ((PFNWXUISTHEMEDIALOGTEXTUREENABLED)NULL) ;
    m_pfnGetThemeAppProperties = ((PFNWXUGETTHEMEAPPPROPERTIES)NULL) ;
    m_pfnSetThemeAppProperties = ((PFNWXUSETTHEMEAPPPROPERTIES)NULL) ;
    m_pfnGetCurrentThemeName = ((PFNWXUGETCURRENTTHEMENAME)NULL) ;
    m_pfnGetThemeDocumentationProperty = ((PFNWXUGETTHEMEDOCUMENTATIONPROPERTY)NULL) ;
    m_pfnDrawThemeParentBackground = ((PFNWXUDRAWTHEMEPARENTBACKGROUND)NULL) ;
    m_pfnEnableTheming = ((PFNWXUENABLETHEMING)NULL) ;
}

wxUxThemeEngine::~wxUxThemeEngine()
{
    if(m_hInstUXThemeDLL != ((HINSTANCE)NULL))
    {
        ::FreeLibrary(m_hInstUXThemeDLL) ;
    }
    ResetFunctionPointers() ;
}

#endif
    // wxUSE_UXTHEME

