///////////////////////////////////////////////////////////////////////////////
// Name:        msw/renderer.cpp
// Purpose:     implementation of wxRendererNative for Windows
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.07.2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// License:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/window.h"
    #include "wx/dc.h"
#endif //WX_PRECOMP

#include "wx/splitter.h"
#include "wx/renderer.h"
#include "wx/msw/uxtheme.h"
#include "wx/msw/private.h"

// tmschema.h is in Win32 Platform SDK and might not be available with earlier
// compilers
#ifndef CP_DROPDOWNBUTTON
    #define CP_DROPDOWNBUTTON  1

    #define CBXS_NORMAL        1
    #define CBXS_HOT           2
    #define CBXS_PRESSED       3
    #define CBXS_DISABLED      4
#endif

// ----------------------------------------------------------------------------
// wxRendererMSW: wxRendererNative implementation for "old" Win32 systems
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxRendererMSW : public wxDelegateRendererNative
{
public:
    wxRendererMSW() { }

    static wxRendererNative& Get();

    virtual void DrawComboBoxDropButton(wxWindow *win,
                                        wxDC& dc,
                                        const wxRect& rect,
                                        int flags = 0);

private:
    DECLARE_NO_COPY_CLASS(wxRendererMSW)
};

// ----------------------------------------------------------------------------
// wxRendererXP: wxRendererNative implementation for Windows XP and later
// ----------------------------------------------------------------------------

#if wxUSE_UXTHEME

class WXDLLEXPORT wxRendererXP : public wxDelegateRendererNative
{
public:
    wxRendererXP() : wxDelegateRendererNative(wxRendererMSW::Get()) { }

    static wxRendererNative& Get();

    virtual void DrawSplitterBorder(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int flags = 0);
    virtual void DrawSplitterSash(wxWindow *win,
                                  wxDC& dc,
                                  const wxSize& size,
                                  wxCoord position,
                                  wxOrientation orient,
                                  int flags = 0);

    virtual wxSplitterRenderParams GetSplitterParams(const wxWindow *win);

    virtual void DrawComboBoxDropButton(wxWindow *win,
                                        wxDC& dc,
                                        const wxRect& rect,
                                        int flags = 0);
private:
    DECLARE_NO_COPY_CLASS(wxRendererXP)
};

#endif // wxUSE_UXTHEME

// ============================================================================
// wxRendererNative and wxRendererMSW implementation
// ============================================================================

/* static */
wxRendererNative& wxRendererNative::GetDefault()
{
#if wxUSE_UXTHEME
    wxUxThemeEngine *themeEngine = wxUxThemeEngine::Get();
    if ( themeEngine && themeEngine->IsAppThemed() )
        return wxRendererXP::Get();
#endif // wxUSE_UXTHEME

    return wxRendererMSW::Get();
}

/* static */
wxRendererNative& wxRendererMSW::Get()
{
    static wxRendererMSW s_rendererMSW;

    return s_rendererMSW;
}

#if defined(__WXWINCE__) && !defined(DFCS_FLAT)
#define DFCS_FLAT 0
#endif

void
wxRendererMSW::DrawComboBoxDropButton(wxWindow * WXUNUSED(win),
                                      wxDC& dc,
                                      const wxRect& rect,
                                      int flags)
{
    RECT r;
    r.left = rect.GetLeft();
    r.top = rect.GetTop();
    r.bottom = rect.y + rect.height;
    r.right = rect.x + rect.width;

    int style = DFCS_SCROLLCOMBOBOX;
    if ( flags & wxCONTROL_DISABLED )
        style |= DFCS_INACTIVE;
    if ( flags & wxCONTROL_PRESSED )
        style |= DFCS_PUSHED | DFCS_FLAT;

    ::DrawFrameControl(GetHdcOf(dc), &r, DFC_SCROLL, style);
}

// ============================================================================
// wxRendererXP implementation
// ============================================================================

#if wxUSE_UXTHEME

/* static */
wxRendererNative& wxRendererXP::Get()
{
    static wxRendererXP s_rendererXP;

    return s_rendererXP;
}

// NOTE: There is no guarantee that the button drawn fills the entire rect (XP
// default theme, for example), so the caller should have cleared button's
// background before this call. This is quite likely a wxMSW-specific thing.
void
wxRendererXP::DrawComboBoxDropButton(wxWindow * win,
                                      wxDC& dc,
                                      const wxRect& rect,
                                      int flags)
{
    wxUxThemeHandle hTheme(win, L"COMBOBOX");
    if ( hTheme )
    {
        RECT r;
        r.left = rect.x;
        r.top = rect.y;
        r.right = rect.x + rect.width;
        r.bottom = rect.y + rect.height;

        int state;
        if ( flags & wxCONTROL_PRESSED )
            state = CBXS_PRESSED;
        else if ( flags & wxCONTROL_CURRENT )
            state = CBXS_HOT;
        else if ( flags & wxCONTROL_DISABLED )
            state = CBXS_DISABLED;
        else
            state = CBXS_NORMAL;

        wxUxThemeEngine::Get()->DrawThemeBackground
                                (
                                    hTheme,
                                    (HDC) dc.GetHDC(),
                                    CP_DROPDOWNBUTTON,
                                    state,
                                    &r,
                                    NULL
                                );

    }
}

// ----------------------------------------------------------------------------
// splitter drawing
// ----------------------------------------------------------------------------

// the width of the sash: this is the same as used by Explorer...
static const wxCoord SASH_WIDTH = 4;

wxSplitterRenderParams
wxRendererXP::GetSplitterParams(const wxWindow * win)
{
    if (win->GetWindowStyle() & wxSP_NO_XP_THEME)
        return m_rendererNative.GetSplitterParams(win);
    else
        return wxSplitterRenderParams(SASH_WIDTH, 0, false);
}

void
wxRendererXP::DrawSplitterBorder(wxWindow * win,
                                 wxDC& dc,
                                 const wxRect& rect,
                                 int flags)
{
    if (win->GetWindowStyle() & wxSP_NO_XP_THEME)
    {
        m_rendererNative.DrawSplitterBorder(win, dc, rect, flags);
    }
}

void
wxRendererXP::DrawSplitterSash(wxWindow *win,
                               wxDC& dc,
                               const wxSize& size,
                               wxCoord position,
                               wxOrientation orient,
                               int flags)
{
    if ( !win->HasFlag(wxSP_NO_XP_THEME) )
    {
        wxUxThemeHandle hTheme(win, L"REBAR");
        if ( hTheme )
        {
            RECT rect;
            if ( orient == wxVERTICAL )
            {
                rect.left = position;
                rect.right = position + SASH_WIDTH;
                rect.top = 0;
                rect.bottom = size.y;
            }
            else // wxHORIZONTAL
            {
                rect.left = 0;
                rect.right = size.x;
                rect.top = position;
                rect.bottom = position + SASH_WIDTH;
            }

            wxUxThemeEngine::Get()->DrawThemeBackground
                                    (
                                        hTheme,
                                        (HDC) dc.GetHDC(),
                                        29, // WP_DIALOG: dlg background
                                        0, // no particular state
                                        &rect,
                                        NULL
                                    );
            return;
        }
    }

    m_rendererNative.DrawSplitterSash(win, dc, size, position, orient, flags);
}

#endif // wxUSE_UXTHEME

