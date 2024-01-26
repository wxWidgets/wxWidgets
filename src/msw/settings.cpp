/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/settings.cpp
// Purpose:     wxSystemSettingsNative implementation for MSW
// Author:      Julian Smart
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/settings.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/utils.h"
    #include "wx/gdicmn.h"
    #include "wx/module.h"
#endif

#include "wx/msw/private.h"
#include "wx/msw/missing.h" // for SM_CXCURSOR, SM_CYCURSOR, SM_TABLETPC
#include "wx/msw/private/darkmode.h"
#include "wx/msw/private/metrics.h"
#include "wx/msw/registry.h"

#include "wx/fontutil.h"
#include "wx/fontenum.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// the module which is used to clean up wxSystemSettingsNative data (this is a
// singleton class so it can't be done in the dtor)
class wxSystemSettingsModule : public wxModule
{
public:
    virtual bool OnInit() override;
    virtual void OnExit() override;

private:
    wxDECLARE_DYNAMIC_CLASS(wxSystemSettingsModule);
};

// ----------------------------------------------------------------------------
// global data
// ----------------------------------------------------------------------------

// the font returned by GetFont(wxSYS_DEFAULT_GUI_FONT): it is created when
// GetFont() is called for the first time and deleted by wxSystemSettingsModule
static wxFont *gs_fontDefault = nullptr;

// ============================================================================
// implementation
// ============================================================================

// TODO: see ::SystemParametersInfo for all sorts of Windows settings.
// Different args are required depending on the id. How does this differ
// from GetSystemMetric, and should it? Perhaps call it GetSystemParameter
// and pass an optional void* arg to get further info.
// Should also have SetSystemParameter.
// Also implement WM_WININICHANGE

// ----------------------------------------------------------------------------
// wxSystemSettingsModule
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxSystemSettingsModule, wxModule);

bool wxSystemSettingsModule::OnInit()
{
    return true;
}

void wxSystemSettingsModule::OnExit()
{
    wxDELETE(gs_fontDefault);
}

// ----------------------------------------------------------------------------
// wxSystemSettingsNative
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// colours
// ----------------------------------------------------------------------------

wxColour wxSystemSettingsNative::GetColour(wxSystemColour index)
{
    // As GetSysColor() doesn't support dark mode, check for it before using it.
    if ( wxMSWDarkMode::IsActive() )
    {
        const wxColour colDark = wxMSWDarkMode::GetColour(index);
        if ( colDark.IsOk() )
            return colDark;
    }

    if ( index == wxSYS_COLOUR_LISTBOXTEXT)
    {
        // there is no standard colour with this index, map to another one
        index = wxSYS_COLOUR_WINDOWTEXT;
    }
    else if ( index == wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT)
    {
        // there is no standard colour with this index, map to another one
        index = wxSYS_COLOUR_HIGHLIGHTTEXT;
    }
    else if ( index == wxSYS_COLOUR_LISTBOX )
    {
        // there is no standard colour with this index, map to another one
        index = wxSYS_COLOUR_WINDOW;
    }
    else if ( index > wxSYS_COLOUR_BTNHIGHLIGHT )
    {
        // Determine if we are using flat menus, only then allow wxSYS_COLOUR_MENUBAR
        if ( index == wxSYS_COLOUR_MENUBAR )
        {
            BOOL isFlat ;
            if ( SystemParametersInfo( SPI_GETFLATMENU , 0 ,&isFlat, 0 ) )
            {
                if ( !isFlat )
                    index = wxSYS_COLOUR_MENU ;
            }
        }
    }

    COLORREF colSys = ::GetSysColor(index);

    wxColour ret = wxRGBToColour(colSys);
    wxASSERT(ret.IsOk());
    return ret;
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
            wxNativeFontInfo info(lf, nullptr);
            font.Create(info);
        }
        else
        {
            wxFAIL_MSG( wxT("failed to get LOGFONT") );
        }
    }
    else // GetStockObject() failed
    {
        wxFAIL_MSG( wxT("stock font not found") );
    }

    return font;
}

wxFont wxSystemSettingsNative::GetFont(wxSystemFont index)
{
    // wxWindow ctor calls GetFont(wxSYS_DEFAULT_GUI_FONT) so we're
    // called fairly often -- this is why we cache this particular font
    if ( index == wxSYS_DEFAULT_GUI_FONT )
    {
        if ( !gs_fontDefault )
        {
            // http://blogs.msdn.com/oldnewthing/archive/2005/07/07/436435.aspx
            // explains why neither SYSTEM_FONT nor DEFAULT_GUI_FONT should be
            // used here
            //
            // the message box font seems to be the one which should be used
            // for most (simple) controls, e.g. buttons and such but other
            // controls may prefer to use lfStatusFont or lfCaptionFont if it
            // is more appropriate for them
            const wxWindow* win = wxApp::GetMainTopWindow();
            const wxNativeFontInfo
                info(wxMSWImpl::GetNonClientMetrics(win).lfMessageFont, win);

            gs_fontDefault = new wxFont(info);
        }

        return *gs_fontDefault;
    }

    wxFont font = wxCreateFontFromStockObject(index);

    wxASSERT(font.IsOk());

#if wxUSE_FONTENUM
    wxASSERT(wxFontEnumerator::IsValidFacename(font.GetFaceName()));
#endif // wxUSE_FONTENUM

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
    SM_CMOUSEBUTTONS,

    SM_CXBORDER,
    SM_CYBORDER,
    SM_CXCURSOR,
    SM_CYCURSOR,
    SM_CXDOUBLECLK,
    SM_CYDOUBLECLK,
    SM_CXDRAG,
    SM_CYDRAG,
    SM_CXEDGE,
    SM_CYEDGE,
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

    SM_CXSIZEFRAME,
    SM_CYSIZEFRAME,
    SM_CXSMICON,
    SM_CYSMICON,
    SM_CYHSCROLL,
    SM_CXHSCROLL,
    SM_CXVSCROLL,
    SM_CYVSCROLL,
    SM_CYVTHUMB,
    SM_CYCAPTION,
    SM_CYMENU,
    SM_NETWORK,
    SM_PENWINDOWS,
    SM_SHOWSOUNDS,
    SM_SWAPBUTTON,
    -1,   // wxSYS_DCLICK_MSEC - not available as system metric
    -1,   // wxSYS_CARET_ON_MSEC - not available as system metric
    -1,   // wxSYS_CARET_OFF_MSEC - not available as system metric
    -1    // wxSYS_CARET_TIMEOUT_MSEC - not available as system metric
};

// Get a system metric, e.g. scrollbar size
int wxSystemSettingsNative::GetMetric(wxSystemMetric index, const wxWindow* win)
{
    wxCHECK_MSG( index > 0 && (size_t)index < WXSIZEOF(gs_metricsMap), 0,
                 wxT("invalid metric") );

    if ( index == wxSYS_DCLICK_MSEC )
    {
        // This one is not a Win32 system metric
        return ::GetDoubleClickTime();
    }

    // return the caret blink time for both
    // wxSYS_CARET_ON_MSEC and wxSYS_CARET_OFF_MSEC
    if ( index == wxSYS_CARET_ON_MSEC || index == wxSYS_CARET_OFF_MSEC )
    {
        const UINT blinkTime = ::GetCaretBlinkTime();

        if ( blinkTime == 0 ) // error
        {
            return -1;
        }
        
        if ( blinkTime == INFINITE ) // caret does not blink
        {
            return 0;
        }

        return blinkTime;
    }

    int indexMSW = gs_metricsMap[index];
    if ( indexMSW == -1 )
    {
        // not supported under current system
        return -1;
    }

    int rc = wxGetSystemMetrics(indexMSW, win);
    if ( index == wxSYS_NETWORK_PRESENT )
    {
        // only the last bit is significant according to the MSDN
        rc &= 1;
    }

    return rc;
}

bool wxSystemSettingsNative::HasFeature(wxSystemFeature index)
{
    switch ( index )
    {
        case wxSYS_CAN_ICONIZE_FRAME:
        case wxSYS_CAN_DRAW_FRAME_DECORATIONS:
            return true;

        case wxSYS_TABLET_PRESENT:
            return ::GetSystemMetrics(SM_TABLETPC) != 0;

        default:
            wxFAIL_MSG( wxT("unknown system feature") );

            return false;
    }
}

// ----------------------------------------------------------------------------
// function from wx/msw/wrapcctl.h: there is really no other place for it...
// ----------------------------------------------------------------------------

#if wxUSE_LISTCTRL || wxUSE_TREECTRL

extern wxFont wxGetCCDefaultFont()
{
    // the default font used for the common controls seems to be the desktop
    // font which is also used for the icon titles and not the stock default
    // GUI font
    LOGFONT lf;
    const wxWindow* win = wxApp::GetMainTopWindow();
    if ( wxSystemParametersInfo
           (
                SPI_GETICONTITLELOGFONT,
                sizeof(lf),
                &lf,
                0,
                win
           ) )
    {
        return wxFont(wxNativeFontInfo(lf, win));
    }
    else
    {
        wxLogLastError(wxT("SystemParametersInfo(SPI_GETICONTITLELOGFONT"));
    }

    // fall back to the default font for the normal controls
    return wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
}

#endif // wxUSE_LISTCTRL || wxUSE_TREECTRL

// There is no official API for determining whether dark mode is being used,
// but HKCU\Software\Microsoft\Windows\CurrentVersion\Themes\Personalize
// contains AppsUseLightTheme and SystemUsesLightTheme values determining
// whether the applications/system use light or dark mode, so use them.
//
// Adapted from https://stackoverflow.com/a/51336913/15275 ("How to detect
// Windows 10 light/dark mode in Win32 application?").
namespace
{

// Return false unless we are sure we're using the dark mode.
bool IsUsingDarkTheme(const wxString& forWhat)
{
    wxRegKey rk(wxRegKey::HKCU, "Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize");
    if ( rk.Exists() && rk.HasValue(forWhat) )
    {
        long value = -1;
        if ( rk.QueryValue(forWhat, &value) )
            return value <= 0;
    }

    return false;
}

} // anonymous namespace

bool wxSystemAppearance::IsDark() const
{
    // If the application opted in using dark mode, use the undocumented API
    // which we use for dark mode support directly.
    if ( wxMSWDarkMode::IsActive() )
        return true;

    // Note that we should _not_ check if the system is configured to use the
    // dark mode for the other applications here, what matters is whether this
    // application itself uses dark colour schema or not.
    return IsUsingDarkBackground();
}

bool wxSystemAppearance::AreAppsDark() const
{
    return IsUsingDarkTheme("AppsUseLightTheme");
}

bool wxSystemAppearance::IsSystemDark() const
{
    return IsUsingDarkTheme("SystemUsesLightTheme");
}
