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

void
wxRendererMSW::DrawComboBoxDropButton(wxWindow * WXUNUSED(win),
                                      wxDC& dc,
                                      const wxRect& rect,
                                      int flags)
{
    RECT r;
    r.left = rect.GetLeft();
    r.top = rect.GetTop();
    r.bottom = rect.GetBottom();
    r.right = rect.GetRight();

    int style = DFCS_SCROLLCOMBOBOX;
    if ( flags & wxCONTROL_DISABLED )
        style |= DFCS_INACTIVE;
    if ( flags & wxCONTROL_PRESSED )
        style |= DFCS_PUSHED;

    ::DrawFrameControl(GetHdcOf(dc), &r, DFC_SCROLL, style);
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
    if ( !win->HasFlag(wxSP_NO_XP_THEME) )
    {
        wxUxThemeHandle hTheme(win, L"WINDOW");
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

