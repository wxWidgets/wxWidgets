/////////////////////////////////////////////////////////////////////////////
// Name:        msw/settings.cpp
// Purpose:     wxSystemSettingsNative implementation for MSW
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/gdicmn.h"
#endif

#include "wx/settings.h"

#include "wx/msw/private.h"

#include "wx/module.h"
#include "wx/fontutil.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// the module which is used to clean up wxSystemSettingsNative data (this is a
// singleton class so it can't be done in the dtor)
class wxSystemSettingsModule : public wxModule
{
public:
    virtual bool OnInit();
    virtual void OnExit();

private:
    DECLARE_DYNAMIC_CLASS(wxSystemSettingsModule)
};

// ----------------------------------------------------------------------------
// global data
// ----------------------------------------------------------------------------

// the font returned by GetFont(wxSYS_DEFAULT_GUI_FONT): it is created when
// GetFont() is called for the first time and deleted by wxSystemSettingsModule
static wxFont *gs_fontDefault = NULL;

// ============================================================================
// implementation
// ============================================================================

// TODO: see ::SystemParametersInfo for all sorts of Windows settings.
// Different args are required depending on the id. How does this differ
// from GetSystemMetric, and should it? Perhaps call it GetSystemParameter
// and pass an optional void* arg to get further info.
// Should also have SetSystemParameter.
// Also implement WM_WININICHANGE (NT) / WM_SETTINGCHANGE (Win95)

// ----------------------------------------------------------------------------
// wxSystemSettingsModule
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxSystemSettingsModule, wxModule)

bool wxSystemSettingsModule::OnInit()
{
    return TRUE;
}

void wxSystemSettingsModule::OnExit()
{
    delete gs_fontDefault;
    gs_fontDefault = NULL;
}

// ----------------------------------------------------------------------------
// wxSystemSettingsNative
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// colours
// ----------------------------------------------------------------------------

wxColour wxSystemSettingsNative::GetColour(wxSystemColour index)
{
    wxColour col;
    wxRGBToColour(col, ::GetSysColor(index));
    return col;
}

// ----------------------------------------------------------------------------
// fonts
// ----------------------------------------------------------------------------

wxFont wxCreateFontFromStockObject(int index)
{
    wxFont font;

    HFONT hFont = (HFONT) ::GetStockObject(index);
    if ( hFont )
    {
        LOGFONT lf;
        if ( ::GetObject(hFont, sizeof(LOGFONT), &lf) != 0 )
        {
            wxNativeFontInfo info;
            info.lf = lf;
            // Under MicroWindows we pass the HFONT as well
            // because it's hard to convert HFONT -> LOGFONT -> HFONT
            // It's OK to delete stock objects, the delete will be ignored.
#ifdef __WXMICROWIN__
            font.Create(info, (WXHFONT) hFont);
#else
            font.Create(info);
#endif
        }
        else
        {
            wxFAIL_MSG( _T("failed to get LOGFONT") );
        }
    }
    else // GetStockObject() failed
    {
        wxFAIL_MSG( _T("stock font not found") );
    }

    return font;
}

wxFont wxSystemSettingsNative::GetFont(wxSystemFont index)
{
    // wxWindow ctor calls GetSystemFont(wxSYS_DEFAULT_GUI_FONT) so we're
    // called fairly often - this is why we cache this particular font
    bool isDefaultRequested = index == wxSYS_DEFAULT_GUI_FONT;
    if ( isDefaultRequested && gs_fontDefault )
    {
        return *gs_fontDefault;
    }

    wxFont font = wxCreateFontFromStockObject(index);

    if ( isDefaultRequested )
    {
        // if we got here it means we hadn't cached it yet - do now
        gs_fontDefault = new wxFont(font);
    }

    return font;
}

// ----------------------------------------------------------------------------
// system metrics/features
// ----------------------------------------------------------------------------

// TODO: some of the "metrics" clearly should be features now that we have
//       HasFeature()!

// the conversion table from wxSystemMetric enum to GetSystemMetrics() param
//
// if the constant is not defined, put -1 in the table to indicate that it is
// unknown
static const int gs_metricsMap[] =
{
    -1,  // wxSystemMetric enums start at 1, so give a dummy value for pos 0.
#ifdef __WIN32__
    SM_CMOUSEBUTTONS,
#else
    -1,
#endif

    SM_CXBORDER,
    SM_CYBORDER,
    SM_CXCURSOR,
    SM_CYCURSOR,
    SM_CXDOUBLECLK,
    SM_CYDOUBLECLK,
#if defined(__WIN32__) && defined(SM_CXDRAG)
    SM_CXDRAG,
    SM_CYDRAG,
    SM_CXEDGE,
    SM_CYEDGE,
#else
    -1, -1, -1, -1
#endif
    SM_CXHSCROLL,
    SM_CYHSCROLL,
    SM_CXHTHUMB,
    SM_CXICON,
    SM_CYICON,
    SM_CXICONSPACING,
    SM_CYICONSPACING,
    SM_CXMIN,
    SM_CYMIN,
    SM_CXSCREEN,
    SM_CYSCREEN,

#if defined(__WIN32__) && defined(SM_CXSIZEFRAME)
    SM_CXSIZEFRAME,
    SM_CYSIZEFRAME,
    SM_CXSMICON,
    SM_CYSMICON,
#else
    -1, -1, -1, -1
#endif
    SM_CYHSCROLL,
    SM_CXVSCROLL,
    SM_CXVSCROLL,
    SM_CYVSCROLL,
    SM_CYVTHUMB,
    SM_CYCAPTION,
    SM_CYMENU,
#if defined(__WIN32__) && defined(SM_NETWORK)
    SM_NETWORK,
#else
    -1,
#endif
    SM_PENWINDOWS,
#if defined(__WIN32__) && defined(SM_SHOWSOUNDS)
    SM_SHOWSOUNDS,
#else
    -1,
#endif
    SM_SWAPBUTTON,
};

// Get a system metric, e.g. scrollbar size
int wxSystemSettingsNative::GetMetric(wxSystemMetric index)
{
#ifdef __WXMICROWIN__
    // TODO: probably use wxUniv themes functionality
    return 0;
#else // !__WXMICROWIN__
    wxCHECK_MSG( index < WXSIZEOF(gs_metricsMap), 0, _T("invalid metric") );

    int indexMSW = gs_metricsMap[index];
    if ( indexMSW == -1 )
    {
        // not supported under current system
        return 0;
    }

    int rc = ::GetSystemMetrics(indexMSW);
    if ( index == wxSYS_NETWORK_PRESENT )
    {
        // only the last bit is significant according to the MSDN
        rc &= 1;
    }

    return rc;
#endif // __WXMICROWIN__/!__WXMICROWIN__
}

bool wxSystemSettingsNative::HasFeature(wxSystemFeature index)
{
    switch ( index )
    {
        case wxSYS_CAN_ICONIZE_FRAME:
        case wxSYS_CAN_DRAW_FRAME_DECORATIONS:
            return TRUE;

        default:
            wxFAIL_MSG( _T("unknown system feature") );

            return FALSE;
    }
}
