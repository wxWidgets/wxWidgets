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

// ----------------------------------------------------------------------------
// wxRendererMSW: wxRendererNative implementation for "old" Win32 systems
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxRendererMSW : public wxDelegateRendererNative
{
public:
    wxRendererMSW() { }

    static wxRendererNative& Get();

private:
    DECLARE_NO_COPY_CLASS(wxRendererMSW)
};

// ----------------------------------------------------------------------------
// wxRendererXP: wxRendererNative implementation for Windows XP and later
// ----------------------------------------------------------------------------

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

private:
    DECLARE_NO_COPY_CLASS(wxRendererXP)
};

// ============================================================================
// wxRendererNative and wxRendererMSW implementation
// ============================================================================

/* static */
wxRendererNative& wxRendererNative::GetDefault()
{
    wxUxThemeEngine *themeEngine = wxUxThemeEngine::Get();
    return themeEngine && themeEngine->IsAppThemed() ? wxRendererXP::Get()
                                                     : wxRendererMSW::Get();
}

/* static */
wxRendererNative& wxRendererMSW::Get()
{
    static wxRendererMSW s_rendererMSW;

    return s_rendererMSW;
}

// ============================================================================
// wxRendererXP implementation
// ============================================================================

/* static */
wxRendererNative& wxRendererXP::Get()
{
    static wxRendererXP s_rendererXP;

    return s_rendererXP;
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
    if (win->GetWindowStyle() & wxSP_NO_XP_THEME)
    {
        m_rendererNative.DrawSplitterSash(
                   win, dc, size, position, orient, flags);
        return;
    }

    // I don't know if it is correct to use the rebar background for the
    // splitter but it least this works ok in the default theme
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
                                    (WXHTHEME)hTheme,
                                    dc.GetHDC(),
                                    3 /* RP_BAND */,
                                    0 /* no state */ ,
                                    &rect,
                                    NULL
                                );
    }
}

