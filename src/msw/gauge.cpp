/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/gauge.cpp
// Purpose:     wxGauge class
// Author:      Julian Smart
// Created:     01/02/97
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


#if wxUSE_GAUGE

#include "wx/gauge.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"

    #include "wx/msw/wrapcctl.h" // include <commctrl.h> "properly"
#endif

#include "wx/appprogress.h"
#include "wx/msw/private.h"
#include "wx/msw/private/darkmode.h"
#include "wx/msw/private/winstyle.h"
#include "wx/msw/uxtheme.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#ifndef PBM_SETBARCOLOR
    #define PBM_SETBARCOLOR         (WM_USER+9)
#endif

#ifndef PBM_SETBKCOLOR
    #define PBM_SETBKCOLOR          0x2001
#endif

#ifndef PBS_MARQUEE
    #define PBS_MARQUEE             0x08
#endif

#ifndef PBM_SETMARQUEE
    #define PBM_SETMARQUEE          (WM_USER+10)
#endif

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

// ============================================================================
// wxGauge implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxGauge creation
// ----------------------------------------------------------------------------

bool wxGauge::Create(wxWindow *parent,
                     wxWindowID id,
                     int range,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,
                     const wxValidator& validator,
                     const wxString& name)
{
    if ( !CreateControl(parent, id, pos, size, style, validator, name) )
        return false;

    if ( !MSWCreateControl(PROGRESS_CLASS, wxEmptyString, pos, size) )
        return false;

    // in case we need to emulate indeterminate mode...
    m_nDirection = wxRIGHT;

    SetRange(range);

    InitProgressIndicatorIfNeeded();

    return true;
}

wxGauge::~wxGauge()
{
}

void wxGauge::MSWGetDarkModeSupport(MSWDarkModeSupport& support) const
{
    if ( wxMSWDarkMode::HasDarkTheme() )
    {
        support.themeName = L"DarkMode_DarkTheme";
        support.themeId = L"Progress";
        support.isLightModeThemed = false;
    }
    else
        wxGaugeBase::MSWGetDarkModeSupport(support);
}

void wxGauge::MSWSetDarkOrLightMode(SetMode setmode)
{
    wxGaugeBase::MSWSetDarkOrLightMode(setmode);

    // Adjust colours unless we use DarkMode_DarkTheme in
    // MSWGetDarkModeSupport().
    if ( !wxMSWDarkMode::HasDarkTheme() )
    {
        // Disable visual styles so colour messages take effect.
        ::SetWindowTheme(m_hWnd, L"", L"");
        if ( wxMSWDarkMode::IsActive() )
        {
            // Colours taken from a progress bar with DarkMode_DarkTheme.
            ::SendMessage(m_hWnd, PBM_SETBKCOLOR, 0, 0x131313);
            ::SendMessage(m_hWnd, PBM_SETBARCOLOR, 0, 0x5fcb6c);
        }
        else
        {
            // The control does not go back into themed mode, so we must
            // explicitly set the colours.
            ::SendMessage(m_hWnd, PBM_SETBKCOLOR, 0, CLR_DEFAULT);
            // For the foreground, CLR_DEFAULT apparently sets the unthemed
            // colour, which is blue instead of green. Use the actual observed
            // theme colour.
            ::SendMessage(m_hWnd, PBM_SETBARCOLOR, 0, 0x25b006);
        }
    }

    // Adjust the border unless a border was specified.
    if ( (GetWindowStyleFlag() & wxBORDER_MASK) == wxBORDER_DEFAULT )
    {
        auto style = ::GetWindowLong(m_hWnd, GWL_STYLE);
        auto exStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
        if ( wxMSWDarkMode::IsActive() )
        {
            // The default border looks bad. Use simple border.
            style |= WS_BORDER;
            exStyle &= ~WS_EX_STATICEDGE;
        }
        else
        {
            // Restore default border.
            style &= ~WS_BORDER;
            exStyle |= WS_EX_STATICEDGE;
        }
        ::SetWindowLong(m_hWnd, GWL_STYLE, style);
        ::SetWindowLong(m_hWnd, GWL_EXSTYLE, exStyle);
        ::SetWindowPos(m_hWnd, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED |
            SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
    }

    // Restore the position shown by the control.
    ::SendMessage(m_hWnd, PBM_SETPOS, GetValue(), 0);
}

WXDWORD wxGauge::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    WXDWORD msStyle = wxControl::MSWGetStyle(style, exstyle);

    if ( style & wxGA_VERTICAL )
        msStyle |= PBS_VERTICAL;

    if ( style & wxGA_SMOOTH )
        msStyle |= PBS_SMOOTH;

    return msStyle;
}

// ----------------------------------------------------------------------------
// wxGauge geometry
// ----------------------------------------------------------------------------

wxSize wxGauge::DoGetBestSize() const
{
    // Windows HIG (http://msdn.microsoft.com/en-us/library/aa511279.aspx)
    // suggest progress bar size of "107 or 237 x 8 dialog units". Let's use
    // the smaller one.

    if (HasFlag(wxGA_VERTICAL))
        return ConvertDialogToPixels(wxSize(8, 107));
    else
        return ConvertDialogToPixels(wxSize(107, 8));
}

// ----------------------------------------------------------------------------
// wxGauge setters
// ----------------------------------------------------------------------------

void wxGauge::SetRange(int r)
{
    // Changing range implicitly means we're using determinate mode.
    if ( IsInIndeterminateMode() )
        SetDeterminateMode();

    wxGaugeBase::SetRange(r);

#ifdef PBM_SETRANGE32
    ::SendMessage(GetHwnd(), PBM_SETRANGE32, 0, r);
#else // !PBM_SETRANGE32
    // fall back to PBM_SETRANGE (limited to 16 bits)
    ::SendMessage(GetHwnd(), PBM_SETRANGE, 0, MAKELPARAM(0, r));
#endif // PBM_SETRANGE32/!PBM_SETRANGE32
}

void wxGauge::SetValue(int pos)
{
    // Setting the value implicitly means that we're using determinate mode.
    if ( IsInIndeterminateMode() )
        SetDeterminateMode();

    if ( GetValue() != pos )
    {
        wxGaugeBase::SetValue(pos);

        ::SendMessage(GetHwnd(), PBM_SETPOS, pos, 0);
    }
}

bool wxGauge::SetForegroundColour(const wxColour& col)
{
    if ( !wxControl::SetForegroundColour(col) )
        return false;

    ::SendMessage(GetHwnd(), PBM_SETBARCOLOR, 0, (LPARAM)wxColourToRGB(col));

    return true;
}

bool wxGauge::SetBackgroundColour(const wxColour& col)
{
    if ( !wxControl::SetBackgroundColour(col) )
        return false;

    ::SendMessage(GetHwnd(), PBM_SETBKCOLOR, 0, (LPARAM)wxColourToRGB(col));

    return true;
}

bool wxGauge::IsInIndeterminateMode() const
{
    return (::GetWindowLong(GetHwnd(), GWL_STYLE) & PBS_MARQUEE) != 0;
}

void wxGauge::SetIndeterminateMode()
{
    // Switch the control into indeterminate mode if necessary.
    if ( !IsInIndeterminateMode() )
    {
        wxMSWWinStyleUpdater(GetHwnd()).TurnOn(PBS_MARQUEE);
        ::SendMessage(GetHwnd(), PBM_SETMARQUEE, TRUE, 0);
    }
}

void wxGauge::SetDeterminateMode()
{
    if ( IsInIndeterminateMode() )
    {
        ::SendMessage(GetHwnd(), PBM_SETMARQUEE, FALSE, 0);
        wxMSWWinStyleUpdater(GetHwnd()).TurnOff(PBS_MARQUEE);
    }
}

void wxGauge::Pulse()
{
    if (wxApp::GetComCtl32Version() >= 600)
    {
        // switch to indeterminate mode if required
        SetIndeterminateMode();

        SendMessage(GetHwnd(), PBM_STEPIT, 0, 0);

        if ( m_appProgressIndicator )
            m_appProgressIndicator->Pulse();
    }
    else
    {
        // emulate indeterminate mode
        wxGaugeBase::Pulse();
    }
}

#endif // wxUSE_GAUGE
