///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/renderer.cpp
// Purpose:     implementation of wxRendererNative for Windows
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.07.2003
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/window.h"
    #include "wx/control.h"     // for wxControl::Ellipsize()
    #include "wx/dc.h"
    #include "wx/dcmemory.h"
    #include "wx/settings.h"
#endif //WX_PRECOMP

#include "wx/dcgraph.h"
#include "wx/math.h"
#include "wx/scopeguard.h"
#include "wx/splitter.h"
#include "wx/renderer.h"
#include "wx/msw/private.h"
#include "wx/msw/uxtheme.h"
#include "wx/msw/wrapcctl.h"
#include "wx/dynlib.h"

#include "wx/msw/private/darkmode.h"

// ----------------------------------------------------------------------------
// methods common to wxRendererMSW and wxRendererXP
// ----------------------------------------------------------------------------

class wxRendererMSWBase : public wxDelegateRendererNative
{
public:
    wxRendererMSWBase() { }
    wxRendererMSWBase(wxRendererNative& rendererNative)
        : wxDelegateRendererNative(rendererNative) { }

    void DrawFocusRect(wxWindow * win,
                        wxDC& dc,
                        const wxRect& rect,
                        int flags = 0) override;

    void DrawItemSelectionRect(wxWindow *win,
                                wxDC& dc,
                                const wxRect& rect,
                                int flags = 0) override;

    void DrawChoice(wxWindow* win,
                     wxDC& dc,
                     const wxRect& rect,
                     int flags = 0) override;

    void DrawComboBox(wxWindow* win,
                       wxDC& dc,
                       const wxRect& rect,
                       int flags = 0) override;

    virtual void DrawComboBoxDropButton(wxWindow *win,
                                         wxDC& dc,
                                         const wxRect& rect,
                                         int flags = 0) override = 0;

protected:
    // Helper function returning the MSW RECT corresponding to the wxRect
    // adjusted for the given wxDC.
    static RECT ConvertToRECT(wxDC& dc, const wxRect& rect)
    {
        // Theme API doesn't know anything about GDI+ transforms, so apply them
        // manually.
        wxRect rectDevice = dc.GetImpl()->MSWApplyGDIPlusTransform(rect);

        // We also need to handle the origin offset manually as we don't use
        // Windows support for this, see wxDC code.
        rectDevice.Offset(dc.GetDeviceOrigin());

        RECT rc;
        wxCopyRectToRECT(rectDevice, rc);
        return rc;
    }
};

// ----------------------------------------------------------------------------
// wxRendererMSW: wxRendererNative implementation for "old" Win32 systems
// ----------------------------------------------------------------------------

class wxRendererMSW : public wxRendererMSWBase
{
public:
    wxRendererMSW() { }

    static wxRendererNative& Get();

    virtual void DrawComboBoxDropButton(wxWindow *win,
                                        wxDC& dc,
                                        const wxRect& rect,
                                        int flags = 0) override;

    virtual void DrawCheckBox(wxWindow *win,
                              wxDC& dc,
                              const wxRect& rect,
                              int flags = 0) override
    {
        DoDrawButton(DFCS_BUTTONCHECK, win, dc, rect, flags);
    }

    virtual void DrawCheckMark(wxWindow *win,
                               wxDC& dc,
                               const wxRect& rect,
                               int flags = 0) override
    {
        DoDrawFrameControl(DFC_MENU, DFCS_MENUCHECK, win, dc, rect, flags);
    }

    virtual void DrawPushButton(wxWindow *win,
                                wxDC& dc,
                                const wxRect& rect,
                                int flags = 0) override;

    virtual void DrawRadioBitmap(wxWindow* win,
                                 wxDC& dc,
                                 const wxRect& rect,
                                 int flags = 0) override
    {
        DoDrawButton(DFCS_BUTTONRADIO, win, dc, rect, flags);
    }

    virtual void DrawTitleBarBitmap(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    wxTitleBarButton button,
                                    int flags = 0) override;

    virtual wxSize GetCheckBoxSize(wxWindow *win, int flags = 0) override;

    virtual int GetHeaderButtonHeight(wxWindow *win) override;

    virtual int GetHeaderButtonMargin(wxWindow *win) override;

private:
    // wrapper of DrawFrameControl()
    void DoDrawFrameControl(UINT type,
                            UINT kind,
                            wxWindow *win,
                            wxDC& dc,
                            const wxRect& rect,
                            int flags);

    // common part of Draw{PushButton,CheckBox,RadioBitmap}(): wraps
    // DrawFrameControl(DFC_BUTTON)
    void DoDrawButton(UINT kind,
                      wxWindow *win,
                      wxDC& dc,
                      const wxRect& rect,
                      int flags)
    {
        DoDrawFrameControl(DFC_BUTTON, kind, win, dc, rect, flags);
    }

    wxDECLARE_NO_COPY_CLASS(wxRendererMSW);
};

// ----------------------------------------------------------------------------
// wxRendererXP: wxRendererNative implementation for Windows XP and later
// ----------------------------------------------------------------------------

#if wxUSE_UXTHEME

class wxRendererXP : public wxRendererMSWBase
{
public:
    wxRendererXP() : wxRendererMSWBase(wxRendererMSW::Get()) { }

    static wxRendererNative& Get();

    virtual int DrawHeaderButton(wxWindow *win,
                                  wxDC& dc,
                                  const wxRect& rect,
                                  int flags = 0,
                                  wxHeaderSortIconType sortArrow = wxHDR_SORT_ICON_NONE,
                                  wxHeaderButtonParams* params = nullptr) override;

    virtual void DrawTreeItemButton(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int flags = 0) override;
    virtual void DrawSplitterBorder(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int flags = 0) override;
    virtual void DrawSplitterSash(wxWindow *win,
                                  wxDC& dc,
                                  const wxSize& size,
                                  wxCoord position,
                                  wxOrientation orient,
                                  int flags = 0) override;
    virtual void DrawComboBoxDropButton(wxWindow *win,
                                        wxDC& dc,
                                        const wxRect& rect,
                                        int flags = 0) override;
    virtual void DrawCheckBox(wxWindow *win,
                              wxDC& dc,
                              const wxRect& rect,
                              int flags = 0) override
    {
        if ( !DoDrawXPButton(BP_CHECKBOX, win, dc, rect, flags) )
            m_rendererNative.DrawCheckBox(win, dc, rect, flags);
    }

    virtual void DrawCheckMark(wxWindow *win,
                               wxDC& dc,
                               const wxRect& rect,
                               int flags = 0) override
    {
        if ( !DoDrawCheckMark(MENU_POPUPCHECK, win, dc, rect, flags) )
            m_rendererNative.DrawCheckMark(win, dc, rect, flags);
    }

    virtual void DrawPushButton(wxWindow *win,
                                wxDC& dc,
                                const wxRect& rect,
                                int flags = 0) override
    {
        if ( !DoDrawXPButton(BP_PUSHBUTTON, win, dc, rect, flags) )
            m_rendererNative.DrawPushButton(win, dc, rect, flags);
    }

    virtual void DrawCollapseButton(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int flags = 0) override;

    virtual wxSize GetCollapseButtonSize(wxWindow *win, wxDC& dc) override;

    virtual void DrawItemSelectionRect(wxWindow *win,
                                       wxDC& dc,
                                       const wxRect& rect,
                                       int flags = 0) override;

    virtual void DrawTextCtrl(wxWindow* win,
                              wxDC& dc,
                              const wxRect& rect,
                              int flags = 0) override;

    virtual void DrawRadioBitmap(wxWindow *win,
                                 wxDC& dc,
                                 const wxRect& rect,
                                 int flags = 0) override
    {
        if ( !DoDrawXPButton(BP_RADIOBUTTON, win, dc, rect, flags) )
            m_rendererNative.DrawRadioBitmap(win, dc, rect, flags);
    }

    virtual void DrawTitleBarBitmap(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    wxTitleBarButton button,
                                    int flags = 0) override;

    virtual wxSize GetCheckBoxSize(wxWindow *win, int flags = 0) override;

    virtual wxSize GetCheckMarkSize(wxWindow* win) override;

    virtual wxSize GetExpanderSize(wxWindow *win) override;

    virtual void DrawGauge(wxWindow* win,
                           wxDC& dc,
                           const wxRect& rect,
                           int value,
                           int max,
                           int flags = 0) override;

    virtual void DrawItemText(wxWindow* win,
                              wxDC& dc,
                              const wxString& text,
                              const wxRect& rect,
                              int align = wxALIGN_LEFT | wxALIGN_TOP,
                              int flags = 0,
                              wxEllipsizeMode ellipsizeMode = wxELLIPSIZE_END) override;

    virtual wxSplitterRenderParams GetSplitterParams(const wxWindow *win) override;

private:
    // wrapper around DrawThemeBackground() translating flags to NORMAL/HOT/
    // PUSHED/DISABLED states (and so suitable for drawing anything
    // button-like)
    void DoDrawButtonLike(HTHEME htheme,
                          int part,
                          wxDC& dc,
                          const wxRect& rect,
                          int flags);

    // common part of DrawCheckBox(), DrawPushButton() and DrawRadioBitmap()
    bool DoDrawXPButton(int kind,
                        wxWindow *win,
                        wxDC& dc,
                        const wxRect& rect,
                        int flags);

    bool DoDrawCheckMark(int kind,
                         wxWindow *win,
                         wxDC& dc,
                         const wxRect& rect,
                         int flags);

    wxDECLARE_NO_COPY_CLASS(wxRendererXP);
};

#endif // wxUSE_UXTHEME


// ============================================================================
// wxRendererMSWBase implementation
// ============================================================================

void wxRendererMSWBase::DrawFocusRect(wxWindow * WXUNUSED(win),
                                      wxDC& dc,
                                      const wxRect& rect,
                                      int WXUNUSED(flags))
{
    RECT rc = ConvertToRECT(dc, rect);

    ::DrawFocusRect(GetHdcOf(dc.GetTempHDC()), &rc);
}

void wxRendererMSWBase::DrawItemSelectionRect(wxWindow *win,
                                              wxDC& dc,
                                              const wxRect& rect,
                                              int flags)
{
    if ( flags & wxCONTROL_CELL )
    {
        m_rendererNative.DrawItemSelectionRect(win, dc, rect, flags);
        return;
    }

    if ( flags & wxCONTROL_SELECTED )
    {
        wxColour color(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
        if ((flags & wxCONTROL_FOCUSED) == 0)
        {
            // Use wxSYS_COLOUR_BTNFACE for unfocused selection, but only if it
            // has enough contrast with wxSYS_COLOUR_HIGHLIGHTTEXT, as otherwise
            // the text will be unreadable
            const wxColour btnface(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
            const wxColour highlightText(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
            if (fabs(btnface.GetLuminance() - highlightText.GetLuminance()) > 0.5)
                color = btnface;
        }

        wxDCBrushChanger setBrush(dc, wxBrush(color));
        wxDCPenChanger setPen(dc, *wxTRANSPARENT_PEN);
        dc.DrawRectangle(rect);
    }

    if ((flags & wxCONTROL_FOCUSED) && (flags & wxCONTROL_CURRENT))
        DrawFocusRect( win, dc, rect, flags );
}

void wxRendererMSWBase::DrawChoice(wxWindow* win,
                                   wxDC& dc,
                                   const wxRect& rect,
                                   int flags)
{
    DrawComboBox(win, dc, rect, flags);
}

void wxRendererMSWBase::DrawComboBox(wxWindow* win,
                                     wxDC& dc,
                                     const wxRect& rect,
                                     int flags)
{
    // Draw the main part of the control same as TextCtrl
    DrawTextCtrl(win, dc, rect, flags);

    // Draw the button inside the border, on the right side
    wxRect br(rect);
    br.height -= 2;
    br.x += br.width - br.height - 1;
    br.width = br.height;
    br.y += 1;

    DrawComboBoxDropButton(win, dc, br, flags);
}

// ============================================================================
// wxRendererNative and wxRendererMSW implementation
// ============================================================================

/* static */
wxRendererNative& wxRendererNative::GetDefault()
{
#if wxUSE_UXTHEME
    if ( wxUxThemeIsActive() )
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

void
wxRendererMSW::DrawComboBoxDropButton(wxWindow * WXUNUSED(win),
                                      wxDC& dc,
                                      const wxRect& rect,
                                      int flags)
{
    wxCHECK_RET( dc.GetImpl(), wxT("Invalid wxDC") );

    RECT r = ConvertToRECT(dc, rect);

    int style = DFCS_SCROLLCOMBOBOX;
    if ( flags & wxCONTROL_DISABLED )
        style |= DFCS_INACTIVE;
    if ( flags & wxCONTROL_PRESSED )
        style |= DFCS_PUSHED | DFCS_FLAT;

    ::DrawFrameControl(GetHdcOf(dc.GetTempHDC()), &r, DFC_SCROLL, style);
}

void
wxRendererMSW::DoDrawFrameControl(UINT type,
                                  UINT kind,
                                  wxWindow * WXUNUSED(win),
                                  wxDC& dc,
                                  const wxRect& rect,
                                  int flags)
{
    wxCHECK_RET( dc.GetImpl(), wxT("Invalid wxDC") );

    RECT r = ConvertToRECT(dc, rect);

    int style = kind;
    if ( flags & wxCONTROL_CHECKED )
        style |= DFCS_CHECKED;
    if ( flags & wxCONTROL_DISABLED )
        style |= DFCS_INACTIVE;
    if ( flags & wxCONTROL_FLAT )
        style |= DFCS_MONO;
    if ( flags & wxCONTROL_PRESSED )
        style |= DFCS_PUSHED;
    if ( flags & wxCONTROL_CURRENT )
        style |= DFCS_HOT;
    if ( flags & wxCONTROL_UNDETERMINED )
        // Using DFCS_BUTTON3STATE here doesn't work (as might be expected),
        // use the following two styles to get the same look of a check box
        // in the undetermined state.
        style |= DFCS_INACTIVE | DFCS_CHECKED;

    ::DrawFrameControl(GetHdcOf(dc.GetTempHDC()), &r, type, style);
}

void
wxRendererMSW::DrawPushButton(wxWindow *win,
                              wxDC& dc,
                              const wxRect& rectOrig,
                              int flags)
{
    wxRect rect(rectOrig);
    if ( flags & wxCONTROL_ISDEFAULT )
    {
        // DrawFrameControl() doesn't seem to support default buttons so we
        // have to draw the border ourselves
        wxDCPenChanger pen(dc, *wxBLACK_PEN);
        wxDCBrushChanger brush(dc, *wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(rect);
        rect.Deflate(1);
    }

    DoDrawButton(DFCS_BUTTONPUSH, win, dc, rect, flags);
}

void
wxRendererMSW::DrawTitleBarBitmap(wxWindow *win,
                                  wxDC& dc,
                                  const wxRect& rect,
                                  wxTitleBarButton button,
                                  int flags)
{
    UINT kind;
    switch ( button )
    {
        case wxTITLEBAR_BUTTON_CLOSE:
            kind = DFCS_CAPTIONCLOSE;
            break;

        case wxTITLEBAR_BUTTON_MAXIMIZE:
            kind = DFCS_CAPTIONMAX;
            break;

        case wxTITLEBAR_BUTTON_ICONIZE:
            kind = DFCS_CAPTIONMIN;
            break;

        case wxTITLEBAR_BUTTON_RESTORE:
            kind = DFCS_CAPTIONRESTORE;
            break;

        case wxTITLEBAR_BUTTON_HELP:
            kind = DFCS_CAPTIONHELP;
            break;

        default:
            wxFAIL_MSG( "unsupported title bar button" );
            return;
    }

    DoDrawFrameControl(DFC_CAPTION, kind, win, dc, rect, flags);
}

wxSize wxRendererMSW::GetCheckBoxSize(wxWindow* win, int WXUNUSED(flags))
{
    // We must have a valid window in order to return the size which is correct
    // for the display this window is on.
    wxCHECK_MSG( win, wxSize(0, 0), "Must have a valid window" );

    return wxSize(wxGetSystemMetrics(SM_CXMENUCHECK, win),
                  wxGetSystemMetrics(SM_CYMENUCHECK, win));
}

int wxRendererMSW::GetHeaderButtonHeight(wxWindow * win)
{
    // some "reasonable" value returned in case of error, it doesn't really
    // correspond to anything but it's better than returning 0
    static const int DEFAULT_HEIGHT = wxWindow::FromDIP(20, win);


    // create a temporary header window just to get its geometry
    HWND hwndHeader = ::CreateWindow(WC_HEADER, nullptr, 0,
                                     0, 0, 0, 0, nullptr, nullptr, nullptr, nullptr);
    if ( !hwndHeader )
        return DEFAULT_HEIGHT;

    wxON_BLOCK_EXIT1( ::DestroyWindow, hwndHeader );

    // Set the font, even if it's the default one, before measuring the window.
    wxFont font;
    if ( win )
        font = win->GetFont();
    if ( !font.IsOk() )
        wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);

    wxSetWindowFont(hwndHeader, font);

    // initialize the struct filled with the values by Header_Layout()
    RECT parentRect = { 0, 0, 100, 100 };
    WINDOWPOS wp = { 0, 0, 0, 0, 0, 0, 0 };
    HDLAYOUT hdl = { &parentRect, &wp };

    return Header_Layout(hwndHeader, &hdl) ? wp.cy : DEFAULT_HEIGHT;
}

int wxRendererMSW::GetHeaderButtonMargin(wxWindow *win)
{
    // The native control seems to use 3*SM_CXEDGE margins on each size.
    return 6*wxGetSystemMetrics(SM_CXEDGE, win);
}

// ============================================================================
// wxRendererXP implementation
// ============================================================================

#if wxUSE_UXTHEME

namespace
{

int GetListItemState(int flags)
{
    int itemState = (flags & wxCONTROL_CURRENT) ? LISS_HOT : LISS_NORMAL;
    if ( flags & wxCONTROL_SELECTED )
    {
        itemState = (flags & wxCONTROL_CURRENT) ? LISS_HOTSELECTED : LISS_SELECTED;
        if ( !(flags & wxCONTROL_FOCUSED) )
            itemState = LISS_SELECTEDNOTFOCUS;
    }

    if ( flags & wxCONTROL_DISABLED )
        itemState = LISS_DISABLED;

    return itemState;
}

} // anonymous namespace

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
    if ( !hTheme )
    {
        m_rendererNative.DrawComboBoxDropButton(win, dc, rect, flags);
        return;
    }

    wxCHECK_RET( dc.GetImpl(), wxT("Invalid wxDC") );

    RECT r = ConvertToRECT(dc, rect);

    int state;
    if ( flags & wxCONTROL_PRESSED )
        state = CBXS_PRESSED;
    else if ( flags & wxCONTROL_CURRENT )
        state = CBXS_HOT;
    else if ( flags & wxCONTROL_DISABLED )
        state = CBXS_DISABLED;
    else
        state = CBXS_NORMAL;

    ::DrawThemeBackground
                            (
                                hTheme,
                                GetHdcOf(dc.GetTempHDC()),
                                CP_DROPDOWNBUTTON,
                                state,
                                &r,
                                nullptr
                            );

}

int
wxRendererXP::DrawHeaderButton(wxWindow *win,
                               wxDC& dc,
                               const wxRect& rect,
                               int flags,
                               wxHeaderSortIconType sortArrow,
                               wxHeaderButtonParams* params)
{
    wxUxThemeHandle hTheme(win, L"HEADER");
    if ( !hTheme )
    {
        return m_rendererNative.DrawHeaderButton(win, dc, rect, flags, sortArrow, params);
    }

    wxCHECK_MSG( dc.GetImpl(), -1, wxT("Invalid wxDC") );

    RECT r = ConvertToRECT(dc, rect);

    int state;
    if ( flags & wxCONTROL_PRESSED )
        state = HIS_PRESSED;
    else if ( flags & wxCONTROL_CURRENT )
        state = HIS_HOT;
    else
        state = HIS_NORMAL;
    ::DrawThemeBackground
                            (
                                hTheme,
                                GetHdcOf(dc.GetTempHDC()),
                                HP_HEADERITEM,
                                state,
                                &r,
                                nullptr
                            );

    // NOTE: Using the theme to draw HP_HEADERSORTARROW doesn't do anything.
    // Why?  If this can be fixed then draw the sort arrows using the theme
    // and then clear those flags before calling DrawHeaderButtonContents.

    // Add any extras that are specified in flags and params
    return DrawHeaderButtonContents(win, dc, rect, flags, sortArrow, params);
}


void
wxRendererXP::DrawTreeItemButton(wxWindow *win,
                                 wxDC& dc,
                                 const wxRect& rect,
                                 int flags)
{
    wxUxThemeHandle hTheme(win, L"TREEVIEW");
    if ( !hTheme )
    {
        m_rendererNative.DrawTreeItemButton(win, dc, rect, flags);
        return;
    }

    wxCHECK_RET( dc.GetImpl(), wxT("Invalid wxDC") );

    RECT r = ConvertToRECT(dc, rect);

    int state = flags & wxCONTROL_EXPANDED ? GLPS_OPENED : GLPS_CLOSED;
    ::DrawThemeBackground
                            (
                                hTheme,
                                GetHdcOf(dc.GetTempHDC()),
                                TVP_GLYPH,
                                state,
                                &r,
                                nullptr
                            );
}

bool
wxRendererXP::DoDrawXPButton(int kind,
                             wxWindow *win,
                             wxDC& dc,
                             const wxRect& rect,
                             int flags)
{
    wxUxThemeHandle hTheme(win, L"BUTTON");
    if ( !hTheme )
        return false;

    DoDrawButtonLike(hTheme, kind, dc, rect, flags);

    return true;
}

bool
wxRendererXP::DoDrawCheckMark(int kind,
                              wxWindow *win,
                              wxDC& dc,
                              const wxRect& rect,
                              int flags)
{
    wxUxThemeHandle hTheme(win, L"MENU");
    if ( !hTheme )
        return false;

    wxCHECK_MSG( dc.GetImpl(), false, wxT("Invalid wxDC") );

    RECT r = ConvertToRECT(dc, rect);

    int state = MC_CHECKMARKNORMAL;
    if ( flags & wxCONTROL_DISABLED )
        state = MC_CHECKMARKDISABLED;

    ::DrawThemeBackground
                            (
                                hTheme,
                                GetHdcOf(dc.GetTempHDC()),
                                kind,
                                state,
                                &r,
                                nullptr
                            );

    return true;
}

void
wxRendererXP::DoDrawButtonLike(HTHEME htheme,
                               int part,
                               wxDC& dc,
                               const wxRect& rect,
                               int flags)
{
    wxCHECK_RET( dc.GetImpl(), wxT("Invalid wxDC") );

    RECT r = ConvertToRECT(dc, rect);

    // the base state is always 1, whether it is PBS_NORMAL,
    // {CBS,RBS}_UNCHECKEDNORMAL or CBS_NORMAL
    int state = 1;

    // XBS_XXX is followed by XBX_XXXHOT, then XBS_XXXPRESSED and DISABLED
    enum
    {
        NORMAL_OFFSET,
        HOT_OFFSET,
        PRESSED_OFFSET,
        DISABLED_OFFSET,
        STATES_COUNT
    };

    // in both RBS_ and CBS_ enums CHECKED elements are offset by 4 from base
    // (UNCHECKED) ones and MIXED are offset by 4 again as there are all states
    // from the above enum in between them
    if ( flags & wxCONTROL_CHECKED )
        state += STATES_COUNT;
    else if ( flags & wxCONTROL_UNDETERMINED )
        state += 2*STATES_COUNT;

    if ( flags & wxCONTROL_DISABLED )
        state += DISABLED_OFFSET;
    else if ( flags & wxCONTROL_PRESSED )
        state += PRESSED_OFFSET;
    else if ( flags & wxCONTROL_CURRENT )
        state += HOT_OFFSET;
    // wxCONTROL_ISDEFAULT flag is only valid for push buttons
    else if ( part == BP_PUSHBUTTON && (flags & wxCONTROL_ISDEFAULT) )
        state = PBS_DEFAULTED;

    ::DrawThemeBackground
                            (
                                htheme,
                                GetHdcOf(dc.GetTempHDC()),
                                part,
                                state,
                                &r,
                                nullptr
                            );
}

void
wxRendererXP::DrawTitleBarBitmap(wxWindow *win,
                                 wxDC& dc,
                                 const wxRect& rect,
                                 wxTitleBarButton button,
                                 int flags)
{
    wxUxThemeHandle hTheme(win, L"WINDOW");
    if ( !hTheme )
    {
        m_rendererNative.DrawTitleBarBitmap(win, dc, rect, button, flags);
        return;
    }

    int part;
    switch ( button )
    {
        case wxTITLEBAR_BUTTON_CLOSE:
            part = WP_CLOSEBUTTON;
            break;

        case wxTITLEBAR_BUTTON_MAXIMIZE:
            part = WP_MAXBUTTON;
            break;

        case wxTITLEBAR_BUTTON_ICONIZE:
            part = WP_MINBUTTON;
            break;

        case wxTITLEBAR_BUTTON_RESTORE:
            part = WP_RESTOREBUTTON;
            break;

        case wxTITLEBAR_BUTTON_HELP:
            part = WP_HELPBUTTON;
            break;

        default:
            wxFAIL_MSG( "unsupported title bar button" );
            return;
    }

    DoDrawButtonLike(hTheme, part, dc, rect, flags);
}

wxSize wxRendererXP::GetCheckBoxSize(wxWindow* win, int flags)
{
    wxCHECK_MSG( win, wxSize(0, 0), "Must have a valid window" );

    wxUxThemeHandle hTheme(win, L"BUTTON");
    if (hTheme)
    {
        if (::IsThemePartDefined(hTheme, BP_CHECKBOX, 0))
        {
            SIZE checkSize;
            if (::GetThemePartSize(hTheme, nullptr, BP_CHECKBOX, CBS_UNCHECKEDNORMAL, nullptr, TS_DRAW, &checkSize) == S_OK)
                return wxSize(checkSize.cx, checkSize.cy);
        }
    }
    return m_rendererNative.GetCheckBoxSize(win, flags);
}

wxSize wxRendererXP::GetCheckMarkSize(wxWindow* win)
{
    wxCHECK_MSG(win, wxSize(0, 0), "Must have a valid window");

    wxUxThemeHandle hTheme(win, L"MENU");
    if (hTheme)
    {
        if (::IsThemePartDefined(hTheme, MENU_POPUPCHECK, 0))
        {
            SIZE checkSize;
            if (::GetThemePartSize(hTheme, nullptr, MENU_POPUPCHECK, MC_CHECKMARKNORMAL, nullptr, TS_DRAW, &checkSize) == S_OK)
                return wxSize(checkSize.cx, checkSize.cy);
        }
    }
    return m_rendererNative.GetCheckMarkSize(win);
}

wxSize wxRendererXP::GetExpanderSize(wxWindow* win)
{
    wxCHECK_MSG( win, wxSize(0, 0), "Must have a valid window" );

    wxUxThemeHandle hTheme(win, L"TREEVIEW");
    if ( hTheme )
    {
        if ( ::IsThemePartDefined(hTheme, TVP_GLYPH, 0) )
        {
            SIZE expSize;
            if (::GetThemePartSize(hTheme, nullptr, TVP_GLYPH, GLPS_CLOSED, nullptr,
                                   TS_DRAW, &expSize) == S_OK)
                return wxSize(expSize.cx, expSize.cy);

        }
    }

    return m_rendererNative.GetExpanderSize(win);
}

static bool
DoDrawCollapseButton(wxWindow* win, HDC hdc, RECT r, int flags)
{
    wxUxThemeHandle hTheme(win, L"TASKDIALOG");

    if ( ::IsThemePartDefined(hTheme, TDLG_EXPANDOBUTTON, 0) )
    {
        int state;
        if (flags & wxCONTROL_PRESSED)
            state = TDLGEBS_PRESSED;
        else if (flags & wxCONTROL_CURRENT)
            state = TDLGEBS_HOVER;
        else
            state = TDLGEBS_NORMAL;

        if ( flags & wxCONTROL_EXPANDED )
            state += 3;

        ::DrawThemeBackground
            (
            hTheme,
            hdc,
            TDLG_EXPANDOBUTTON,
            state,
            &r,
            nullptr
            );
        return true;
    }

    return false;
}

void
wxRendererXP::DrawCollapseButton(wxWindow *win,
                                 wxDC& dc,
                                 const wxRect& rect,
                                 int flags)
{
    RECT r = ConvertToRECT(dc, rect);

    // Default theme draws the button on light background which looks very out
    // of place when using dark mode, so invert it if necessary and fall back
    // on the generic version if this fails.
    //
    // Ideal would be to find the theme drawing the version appropriate for the
    // dark mode, but it's unknown if there is one providing this.
    if ( wxMSWDarkMode::IsActive() )
    {
        wxBitmap bmp(rect.GetSize());

        bool ok;
        {
            wxMemoryDC mdc(bmp);
            ok = DoDrawCollapseButton(win, GetHdcOf(mdc), r, flags);
        }

        if ( ok )
        {
            wxBitmap bmpInv = wxMSWDarkMode::InvertBitmap(bmp);
            if ( bmpInv.IsOk() )
            {
                dc.DrawBitmap(bmpInv, rect.GetPosition());
                return;
            }
        }
    }
    else
    {
        if ( DoDrawCollapseButton(win, GetHdcOf(dc.GetTempHDC()), r, flags) )
            return;
    }

    m_rendererNative.DrawCollapseButton(win, dc, rect, flags);
}

wxSize wxRendererXP::GetCollapseButtonSize(wxWindow *win, wxDC& dc)
{
    wxUxThemeHandle hTheme(win, L"TASKDIALOG");

    // EXPANDOBUTTON scales ugly if not using the correct size, get size from theme

    if ( ::IsThemePartDefined(hTheme, TDLG_EXPANDOBUTTON, 0) )
    {
        SIZE s;
        ::GetThemePartSize(hTheme,
            GetHdcOf(dc.GetTempHDC()),
            TDLG_EXPANDOBUTTON,
            TDLGEBS_NORMAL,
            nullptr,
            TS_TRUE,
            &s);

        return wxSize(s.cx, s.cy);
    }
    else
        return m_rendererNative.GetCollapseButtonSize(win, dc);
}

void
wxRendererXP::DrawItemSelectionRect(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int flags)
{
    wxUxThemeHandle hTheme(win, L"EXPLORER::LISTVIEW;LISTVIEW");

    const int itemState = GetListItemState(flags);

    if ( ::IsThemePartDefined(hTheme, LVP_LISTITEM, 0) )
    {
        RECT rc = ConvertToRECT(dc, rect);

        if ( ::IsThemeBackgroundPartiallyTransparent(hTheme, LVP_LISTITEM, itemState) )
            ::DrawThemeParentBackground(GetHwndOf(win), GetHdcOf(dc.GetTempHDC()), &rc);

        ::DrawThemeBackground(hTheme, GetHdcOf(dc.GetTempHDC()), LVP_LISTITEM, itemState, &rc, 0);
    }
    else
    {
        m_rendererNative.DrawItemSelectionRect(win, dc, rect, flags);
    }
}

void wxRendererXP::DrawItemText(wxWindow* win,
                                wxDC& dc,
                                const wxString& text,
                                const wxRect& rect,
                                int align,
                                int flags,
                                wxEllipsizeMode ellipsizeMode)
{
    wxUxThemeHandle hTheme(win, L"EXPLORER::LISTVIEW;LISTVIEW");

    const int itemState = GetListItemState(flags);

    if ( ::IsThemePartDefined(hTheme, LVP_LISTITEM, 0) )
    {
        RECT rc = ConvertToRECT(dc, rect);

        DTTOPTS textOpts;
        textOpts.dwSize = sizeof(textOpts);
        textOpts.dwFlags = DTT_STATEID;
        textOpts.iStateId = itemState;

        wxColour textColour = dc.GetTextForeground();
        if (flags & wxCONTROL_SELECTED)
        {
            textColour = wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOXTEXT);
        }
        else if (flags & wxCONTROL_DISABLED)
        {
            textColour = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);
        }

        if (textColour.IsOk()) {
            textOpts.dwFlags |= DTT_TEXTCOLOR;
            textOpts.crText = textColour.GetPixel();
        }

        const DWORD defTextFlags = DT_NOPREFIX;
        DWORD textFlags = defTextFlags;

        // Always use DT_* flags for horizontal alignment.
        if ( align & wxALIGN_CENTER_HORIZONTAL )
            textFlags |= DT_CENTER;
        else if ( align & wxALIGN_RIGHT )
            textFlags |= DT_RIGHT;
        else
            textFlags |= DT_LEFT;

        /*
        Bottom (DT_BOTTOM) and centered vertical (DT_VCENTER) alignment
        are documented to be only used with the DT_SINGLELINE flag which
        doesn't handle multi-lines. In case of drawing multi-lines with
        such alignment use DT_TOP (0), which does work for multi-lines,
        and deal with the actual desired vertical alignment ourselves with
        the help of GetThemeTextExtent().

        [TODO] Ideally text measurement should only be needed for the above
        mentioned situations but because there can be a difference between
        the extent from GetThemeTextExtent() and the rect received by this
        function could have involved other text measurements (e.g. with wxDVC,
        see #18487), use it in all cases for now.
        */
        bool useTopDrawing = false;

        /*
        Get the actual text extent using GetThemeTextExtent() and adjust
        drawing rect if needed.

        Note that DrawThemeTextEx() in combination with DT_CALCRECT
        and DTT_CALCRECT can also be used to get the text extent.
        This seems to always result in the exact same extent (checked
        with an assert) as using GetThemeTextExtent(), despite having
        an additional DTTOPTS argument for various effects.
        Some effects have been tried (DTT_BORDERSIZE, DTT_SHADOWTYPE
        and DTT_SHADOWOFFSET) and while rendered correctly with effects
        the returned extent remains the same as without effects.

        Official docs don't seem to prefer one method over the other
        though a possibly outdated note for DrawThemeText() recommends
        using GetThemeTextExtent(). Because Wine as of writing doesn't
        support DT_CALCRECT with DrawThemeTextEx() while it does support
        GetThemeTextExtent(), opt to use the latter.
        */

        /*
        It's important for the dwTextFlags parameter passed to
        GetThemeTextExtent() not to have some DT_* flags because they
        influence the extent size in unwanted ways: Using
        DT_SINGLELINE combined with either DT_VCENTER or DT_BOTTOM
        results in a height that can't be used (either halved or 0),
        and having DT_END_ELLIPSIS ends up always ellipsizing.
        Passing a non-null rect solves these problems but is not
        really a good option as it doesn't make the rectangle extent
        a tight fit and calculations would have to be done with large
        numbers needlessly (provided the passed rect is set to
        something like {0, 0, LONG_MAX, LONG_MAX} ).
        */
        RECT rcExtent;
        HRESULT hr = ::GetThemeTextExtent(hTheme, dc.GetHDC(),
            LVP_LISTITEM, itemState, text.wchar_str(), -1,
            defTextFlags, nullptr, &rcExtent);
        if ( SUCCEEDED(hr) )
        {
            /*
            Compensate for rare cases where the horizontal extents differ
            slightly. Don't use the width of the passed rect here to deal
            with horizontal alignment as it results in the text always
            fitting and ellipsization then can't occur. Instead check for
            width differences by comparing with the extent as calculated
            by wxDC.
            */
            const int textWidthDc = dc.GetMultiLineTextExtent(text).x;
            const int widthDiff = textWidthDc - rcExtent.right;
            if ( widthDiff )
            {
                if ( align & wxALIGN_CENTRE_HORIZONTAL )
                {
                    const int widthOffset = widthDiff / 2;
                    rc.left += widthOffset;
                    rc.right -= widthOffset;
                }
                else if ( align & wxALIGN_RIGHT )
                    rc.left += widthDiff;
                else // left aligned
                    rc.right -= widthDiff;
            }

            /*
            For height compare with the height of the passed rect and use
            the difference for handling vertical alignment. This has
            consequences for particularly multi-line text: it will now
            always try to fit vertically while a rect received from wxDVC
            may have its extent based on calculations for a single line
            only and therefore couldn't show more than one line. This is
            consistent with other major platforms where no clipping to
            the rect takes places either, including non-themed MSW.
            */
            if ( text.Contains(wxS('\n')) )
            {
                useTopDrawing = true;

                const int heightDiff = rect.GetHeight() - rcExtent.bottom;
                if ( align & wxALIGN_CENTRE_VERTICAL )
                {
                    const int heightOffset = heightDiff / 2;
                    rc.top += heightOffset;
                    rc.bottom -= heightOffset;
                }
                else if ( align & wxALIGN_BOTTOM )
                    rc.top += heightDiff;
                else // top aligned
                    rc.bottom -= heightDiff;
            }
        }

        if ( !useTopDrawing )
        {
            if ( align & wxALIGN_BOTTOM )
                textFlags |= DT_BOTTOM | DT_SINGLELINE;
            else if ( align & wxALIGN_CENTER_VERTICAL )
                textFlags |= DT_VCENTER | DT_SINGLELINE;
            else
                textFlags |= DT_TOP;
        }

        const wxString* drawText = &text;
        wxString ellipsizedText;
        switch ( ellipsizeMode )
        {
            case wxELLIPSIZE_NONE:
                // no flag required
                break;

            case wxELLIPSIZE_START:
            case wxELLIPSIZE_MIDDLE:
                // no native support for this ellipsize modes, use wxWidgets
                // implementation (may not be 100% accurate because per
                // definition the theme defines the font but should be close
                // enough with current windows themes)
                drawText = &ellipsizedText;
                ellipsizedText = wxControl::Ellipsize(text, dc, ellipsizeMode,
                                                      rect.width,
                                                      wxELLIPSIZE_FLAGS_NONE);
                break;

            case wxELLIPSIZE_END:
                textFlags |= DT_END_ELLIPSIS;
                break;
        }

        ::DrawThemeTextEx(hTheme, dc.GetHDC(), LVP_LISTITEM, itemState,
                            drawText->wchar_str(), -1, textFlags, &rc, &textOpts);
    }
    else
    {
        m_rendererNative.DrawItemText(win, dc, text, rect, align, flags, ellipsizeMode);
    }
}

// Uses the theme to draw the border and fill for something like a wxTextCtrl
void wxRendererXP::DrawTextCtrl(wxWindow* win,
                                wxDC& dc,
                                const wxRect& rect,
                                int flags)
{
    wxUxThemeHandle hTheme(win, L"EDIT");
    if ( !hTheme )
    {
        m_rendererNative.DrawTextCtrl(win,dc,rect,flags);
        return;
    }

    wxColour fill = hTheme.GetColour(EP_EDITTEXT, TMT_FILLCOLOR, ETS_NORMAL);
    wxColour bdr = hTheme.GetColour(EP_EDITTEXT, TMT_BORDERCOLOR,
                                    flags & wxCONTROL_DISABLED
                                        ? ETS_DISABLED
                                        : ETS_NORMAL);

    wxDCPenChanger setPen(dc, bdr);
    wxDCBrushChanger setBrush(dc, fill);
    dc.DrawRectangle(rect);
}

void wxRendererXP::DrawGauge(wxWindow* win,
    wxDC& dc,
    const wxRect& rect,
    int value,
    int max,
    int flags)
{
    wxUxThemeHandle hTheme(win, L"PROGRESS");
    if ( !hTheme )
    {
        m_rendererNative.DrawGauge(win, dc, rect, value, max, flags);
        return;
    }

    RECT r = ConvertToRECT(dc, rect);

    ::DrawThemeBackground(
        hTheme,
        GetHdcOf(dc.GetTempHDC()),
        flags & wxCONTROL_SPECIAL ? PP_BARVERT : PP_BAR,
        0,
        &r,
        nullptr);

    RECT contentRect;
    ::GetThemeBackgroundContentRect(
        hTheme,
        GetHdcOf(dc.GetTempHDC()),
        flags & wxCONTROL_SPECIAL ? PP_BARVERT : PP_BAR,
        0,
        &r,
        &contentRect);

    if ( flags & wxCONTROL_SPECIAL )
    {
        // For a vertical gauge, the value grows from the bottom to the top.
        contentRect.top = contentRect.bottom -
                          wxMulDivInt32(contentRect.bottom - contentRect.top,
                                        value,
                                        max);
    }
    else // Horizontal.
    {
        contentRect.right = contentRect.left +
                            wxMulDivInt32(contentRect.right - contentRect.left,
                                          value,
                                          max);
    }

    ::DrawThemeBackground(
        hTheme,
        GetHdcOf(dc.GetTempHDC()),
        flags & wxCONTROL_SPECIAL ? PP_CHUNKVERT : PP_CHUNK,
        0,
        &contentRect,
        nullptr);
}

// ----------------------------------------------------------------------------
// splitter drawing
// ----------------------------------------------------------------------------

// the width of the sash: this is the same as used by Explorer...
static const wxCoord SASH_WIDTH = 4;

wxSplitterRenderParams
wxRendererXP::GetSplitterParams(const wxWindow * win)
{
    if ( win->HasFlag(wxSP_NO_XP_THEME) )
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
    if ( win->HasFlag(wxSP_NO_XP_THEME) )
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
        wxDCPenChanger setPen(dc, *wxTRANSPARENT_PEN);
        wxDCBrushChanger setBrush(dc, wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE)));
        if ( orient == wxVERTICAL )
        {
            dc.DrawRectangle(position, 0, SASH_WIDTH, size.y);
        }
        else // wxHORIZONTAL
        {
            dc.DrawRectangle(0, position, size.x, SASH_WIDTH);
        }

        return;
    }

    m_rendererNative.DrawSplitterSash(win, dc, size, position, orient, flags);
}

#endif // wxUSE_UXTHEME
