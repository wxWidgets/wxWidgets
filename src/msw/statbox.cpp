/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/statbox.cpp
// Purpose:     wxStaticBox
// Author:      Julian Smart
// Modified by:
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


#if wxUSE_STATBOX

#include "wx/statbox.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
    #include "wx/image.h"
    #include "wx/sizer.h"
#endif

#include "wx/notebook.h"
#include "wx/sysopt.h"

#include "wx/msw/uxtheme.h"

#include <windowsx.h> // needed by GET_X_LPARAM and GET_Y_LPARAM macros

#include "wx/msw/private.h"
#include "wx/msw/missing.h"
#include "wx/msw/dc.h"
#include "wx/msw/private/darkmode.h"
#include "wx/msw/private/winstyle.h"

namespace
{

// Offset of the first pixel of the label from the box left border.
//
// FIXME: value is hardcoded as this is what it is on my system, no idea if
//        it's true everywhere
const int LABEL_HORZ_OFFSET = 9;

// Extra borders around the label on left/right and bottom sides.
const int LABEL_HORZ_BORDER = 2;
const int LABEL_VERT_BORDER = 2;

// Offset of the box contents from left/right/bottom edge (top one is
// different, see GetBordersForSizer()). This one is completely arbitrary.
const int CHILDREN_OFFSET = 5;

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxStaticBox
// ----------------------------------------------------------------------------

bool wxStaticBox::Create(wxWindow *parent,
                         wxWindowID id,
                         const wxString& label,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
{
    if ( !CreateControl(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    if ( !MSWCreateControl(wxT("BUTTON"), label, pos, size) )
        return false;

    if ( ShouldUseCustomPaint() )
        UseCustomPaint();

    return true;
}

bool wxStaticBox::MSWGetDarkModeSupport(MSWDarkModeSupport& support) const
{
    // Static boxes don't seem to have any dark mode support, so just set the
    // foreground colour contrasting with the dark background for them.
    support.setForeground = true;

    return true;
}

bool wxStaticBox::ShouldUseCustomPaint() const
{
    // When not using double buffering, we paint the box ourselves by default
    // because using the standard control default WM_PAINT handler results in
    // awful flicker. However this can be disabled by setting a system option
    // which can be useful if the application paints on the box itself (which
    // should be avoided, but some existing code does it).
    return !IsDoubleBuffered() &&
            !wxSystemOptions::IsFalse(wxT("msw.staticbox.optimized-paint"));
}

void wxStaticBox::UseCustomPaint()
{
    // If background style is already set to this value, we must have been
    // already called -- and as we currently never unbind the handler, this
    // means we don't need to do anything.
    if ( GetBackgroundStyle() != wxBG_STYLE_PAINT )
    {
        wxMSWWinExStyleUpdater(GetHwnd()).TurnOff(WS_EX_TRANSPARENT);

        Bind(wxEVT_PAINT, &wxStaticBox::OnPaint, this);

        // Our OnPaint() completely erases our background, so don't do it in
        // WM_ERASEBKGND too to avoid flicker.
        SetBackgroundStyle(wxBG_STYLE_PAINT);
    }
}

void wxStaticBox::MSWOnDisabledComposited()
{
    // We need to enable custom painting if we're not using compositing any
    // longer, as otherwise the window is not drawn correctly due to it using
    // WS_EX_TRANSPARENT and thus not redrawing its background.
    UseCustomPaint();
}

bool wxStaticBox::Create(wxWindow* parent,
                         wxWindowID id,
                         wxWindow* labelWin,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
{
    wxCHECK_MSG( labelWin, false, wxS("Label window can't be null") );

    if ( !Create(parent, id, wxString(), pos, size, style, name) )
        return false;

    m_labelWin = labelWin;
    m_labelWin->Reparent(this);

    PositionLabelWindow();

    return true;
}

void wxStaticBox::PositionLabelWindow()
{
    m_labelWin->SetSize(m_labelWin->GetBestSize());

    // Note that we intentionally don't use FromDIP() with the label offset
    // here, see comment in PaintForeground() where it is also used.
    m_labelWin->Move(LABEL_HORZ_OFFSET, 0);
}

wxWindowList wxStaticBox::GetCompositeWindowParts() const
{
    wxWindowList parts;
    if ( m_labelWin )
        parts.push_back(m_labelWin);
    return parts;
}

WXDWORD wxStaticBox::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    long styleWin = wxStaticBoxBase::MSWGetStyle(style, exstyle);

    // no need for it anymore, must be removed for wxRadioBox child
    // buttons to be able to repaint themselves
    styleWin &= ~WS_CLIPCHILDREN;

    if ( exstyle )
    {
        // We may have children inside this static box, so use this style for
        // TAB navigation to work if we ever use IsDialogMessage() to implement
        // it (currently we don't because it's too buggy and implement TAB
        // navigation ourselves, but this could change in the future).
        *exstyle |= WS_EX_CONTROLPARENT;

        if ( !ShouldUseCustomPaint() )
            *exstyle |= WS_EX_TRANSPARENT;
    }

    styleWin |= BS_GROUPBOX;

    return styleWin;
}

wxSize wxStaticBox::DoGetBestSize() const
{
    wxSize best;

    // Calculate the size needed by the label
    int cx, cy;
    wxGetCharSize(GetHWND(), &cx, &cy, GetFont());

    int wBox;
    GetTextExtent(GetLabelText(wxGetWindowText(m_hWnd)), &wBox, &cy);

    wBox += 3*cx;
    int hBox = EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy);

    // If there is a sizer then the base best size is the sizer's minimum
    if (GetSizer() != nullptr)
    {
        wxSize cm(GetSizer()->CalcMin());
        best = ClientToWindowSize(cm);
        // adjust for a long label if needed
        best.x = wxMax(best.x, wBox);
    }
    // otherwise the best size falls back to the label size
    else
    {
        best = wxSize(wBox, hBox);
    }
    return best;
}

void wxStaticBox::GetBordersForSizer(int *borderTop, int *borderOther) const
{
    // Base class version doesn't leave enough space at the top when the label
    // is empty, so we can't use it here, even though the code is pretty
    // similar.
    if ( m_labelWin )
    {
        *borderTop = m_labelWin->GetSize().y;
    }
    else if ( !GetLabel().empty() )
    {
        *borderTop = GetCharHeight();
    }
    else // No label window nor text.
    {
        // This is completely arbitrary, but using the full char height in
        // this case too seems bad as it leaves too much space at the top
        // (although it does have the advantage of aligning the controls
        // inside static boxes with and without labels vertically).
        *borderTop = 2*FromDIP(CHILDREN_OFFSET);
    }

    // Intentionally don't scale this one by DPI, as it's not scaled when it is
    // actually used in the drawing code, see comments there.
    *borderTop += LABEL_VERT_BORDER;

    *borderOther = FromDIP(CHILDREN_OFFSET);
}

bool wxStaticBox::SetBackgroundColour(const wxColour& colour)
{
    // Do _not_ call the immediate base class method, we don't need to set the
    // label window (which is the only sub-window of this composite window)
    // background explicitly because it will almost always be a wxCheckBox or
    // wxRadioButton which inherits its background from the box anyhow, so
    // setting it would be at best useless.
    return wxStaticBoxBase::SetBackgroundColour(colour);
}

bool wxStaticBox::SetForegroundColour(const wxColour& colour)
{
    if ( !base_type::SetForegroundColour(colour) )
        return false;

    if ( colour.IsOk() && !m_labelWin )
    {
        // We need to be using our custom paint handler to support non-default
        // colours.
        UseCustomPaint();
    }

    return true;
}

bool wxStaticBox::SetFont(const wxFont& font)
{
    if ( !base_type::SetFont(font) )
        return false;

    // We need to reposition the label as its size may depend on the font.
    if ( m_labelWin )
    {
        PositionLabelWindow();
    }

    return true;
}

WXLRESULT wxStaticBox::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    if ( nMsg == WM_NCHITTEST )
    {
        // This code breaks some other processing such as enter/leave tracking
        // so it's off by default.

        static int s_useHTClient = -1;
        if (s_useHTClient == -1)
            s_useHTClient = wxSystemOptions::GetOptionInt(wxT("msw.staticbox.htclient"));
        if (s_useHTClient == 1)
        {
            int xPos = GET_X_LPARAM(lParam);
            int yPos = GET_Y_LPARAM(lParam);

            ScreenToClient(&xPos, &yPos);

            // Make sure you can drag by the top of the groupbox, but let
            // other (enclosed) controls get mouse events also
            if ( yPos < 10 )
                return (long)HTCLIENT;
        }
    }

    if ( nMsg == WM_PRINTCLIENT )
    {
        // we have to process WM_PRINTCLIENT ourselves as otherwise child
        // windows' background (eg buttons in radio box) would never be drawn
        // unless we have a parent with non default background

        // so check first if we have one
        if ( !HandlePrintClient((WXHDC)wParam) )
        {
            // no, we don't, erase the background ourselves
            RECT rc;
            ::GetClientRect(GetHwnd(), &rc);
            wxDCTemp dc((WXHDC)wParam);
            PaintBackground(dc, rc);
        }

        return 0;
    }

    if ( nMsg == WM_UPDATEUISTATE )
    {
        // DefWindowProc() redraws just the static box text when it gets this
        // message and it does it using the standard (blue in standard theme)
        // colour and not our own label colour that we use in PaintForeground()
        // resulting in the label mysteriously changing the colour when e.g.
        // "Alt" is pressed anywhere in the window, see #12497.
        //
        // To avoid this we simply refresh the window forcing our own code
        // redrawing the label in the correct colour to be called. This is
        // inefficient but there doesn't seem to be anything else we can do.
        //
        // Notice that the problem is XP-specific and doesn't arise under later
        // systems.
        if ( m_hasFgCol && wxGetWinVersion() == wxWinVersion_XP )
            Refresh();
    }

    return wxControl::MSWWindowProc(nMsg, wParam, lParam);
}

void wxStaticBox::MSWBeforeDPIChangedEvent(const wxDPIChangedEvent& WXUNUSED(event))
{
    if ( m_labelWin )
        PositionLabelWindow();
}

// ----------------------------------------------------------------------------
// static box drawing
// ----------------------------------------------------------------------------

/*
   We draw the static box ourselves because it's the only way to prevent it
   from flickering horribly on resize (because everything inside the box is
   erased twice: once when the box itself is repainted and second time when
   the control inside it is repainted) without using WS_EX_TRANSPARENT style as
   we used to do and which resulted in other problems.
 */

// MSWGetRegionWithoutSelf helper: removes the given rectangle from region
static inline void
SubtractRectFromRgn(HRGN hrgn, int left, int top, int right, int bottom)
{
    AutoHRGN hrgnRect(::CreateRectRgn(left, top, right, bottom));
    if ( !hrgnRect )
    {
        wxLogLastError(wxT("CreateRectRgn()"));
        return;
    }

    ::CombineRgn(hrgn, hrgn, hrgnRect, RGN_DIFF);
}

void wxStaticBox::MSWGetRegionWithoutSelf(WXHRGN hRgn, int w, int h)
{
    HRGN hrgn = (HRGN)hRgn;

    // remove the area occupied by the static box borders from the region
    int borderTop, border;
    GetBordersForSizer(&borderTop, &border);

    // top
    if ( m_labelWin )
    {
        // Don't exclude the entire rectangle at the top, we do need to paint
        // the background of the gap between the label window and the box
        // frame.
        const wxRect labelRect = m_labelWin->GetRect();
        const int gap = LABEL_HORZ_BORDER;

        SubtractRectFromRgn(hrgn, 0, 0, labelRect.GetLeft() - gap, borderTop);
        SubtractRectFromRgn(hrgn, labelRect.GetRight() + gap, 0, w, borderTop);
    }
    else
    {
        SubtractRectFromRgn(hrgn, 0, 0, w, borderTop);
    }

    // bottom
    SubtractRectFromRgn(hrgn, 0, h - border, w, h);

    // left
    SubtractRectFromRgn(hrgn, 0, 0, border, h);

    // right
    SubtractRectFromRgn(hrgn, w - border, 0, w, h);
}

namespace {
RECT AdjustRectForRtl(wxLayoutDirection dir, RECT const& childRect, RECT const& boxRect) {
    RECT ret = childRect;
    if( dir == wxLayout_RightToLeft ) {
        // The clipping region too is mirrored in RTL layout.
        // We need to mirror screen coordinates relative to static box window priot to
        // intersecting with region.
        ret.right = boxRect.right - (childRect.left - boxRect.left);
        ret.left = boxRect.left + (boxRect.right - childRect.right);
    }

    return ret;
}
}

WXHRGN wxStaticBox::MSWGetRegionWithoutChildren()
{
    RECT boxRc;
    ::GetWindowRect(GetHwnd(), &boxRc);
    HRGN hrgn = ::CreateRectRgn(boxRc.left, boxRc.top, boxRc.right + 1, boxRc.bottom + 1);
    bool foundThis = false;

    // Iterate over all sibling windows as in the old wxWidgets API the
    // controls appearing inside the static box were created as its siblings
    // and not children. This is now deprecated but should still work.
    //
    // Also notice that we must iterate over all windows, not just all
    // wxWindows, as there may be composite windows etc.
    HWND child;
    for ( child = ::GetWindow(GetHwndOf(GetParent()), GW_CHILD);
          child;
          child = ::GetWindow(child, GW_HWNDNEXT) )
    {
        if ( ! ::IsWindowVisible(child) )
        {
            // if the window isn't visible then it doesn't need clipped
            continue;
        }

        wxMSWWinStyleUpdater updateStyle(child);
        wxString str(wxGetWindowClass(child));
        str.MakeUpper();
        if ( str == wxT("BUTTON") && updateStyle.IsOn(BS_GROUPBOX) )
        {
            if ( child == GetHwnd() )
                foundThis = true;

            // Any static boxes below this one in the Z-order can't be clipped
            // since if we have the case where a static box with a low Z-order
            // is nested inside another static box with a high Z-order then the
            // nested static box would be painted over. Doing it this way
            // unfortunately results in flicker if the Z-order of nested static
            // boxes is not inside (lowest) to outside (highest) but at least
            // they are still shown.
            if ( foundThis )
                continue;
        }

        RECT rc;
        ::GetWindowRect(child, &rc);
        rc = AdjustRectForRtl(GetLayoutDirection(), rc, boxRc );
        if ( ::RectInRegion(hrgn, &rc) )
        {
            // need to remove WS_CLIPSIBLINGS from all sibling windows
            // that are within this staticbox if set
            if ( updateStyle.IsOn(WS_CLIPSIBLINGS) )
            {
                updateStyle.TurnOff(WS_CLIPSIBLINGS).Apply();

                // MSDN: "If you have changed certain window data using
                // SetWindowLong, you must call SetWindowPos to have the
                // changes take effect."
                ::SetWindowPos(child, nullptr, 0, 0, 0, 0,
                               SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                               SWP_FRAMECHANGED);
            }

            AutoHRGN hrgnChild(::CreateRectRgnIndirect(&rc));
            ::CombineRgn(hrgn, hrgn, hrgnChild, RGN_DIFF);
        }
    }

    // Also iterate over all children of the static box, we need to clip them
    // out as well.
    for ( child = ::GetWindow(GetHwnd(), GW_CHILD);
          child;
          child = ::GetWindow(child, GW_HWNDNEXT) )
    {
        if ( !::IsWindowVisible(child) )
        {
            // if the window isn't visible then it doesn't need clipped
            continue;
        }

        RECT rc;
        ::GetWindowRect(child, &rc);
        rc = AdjustRectForRtl(GetLayoutDirection(), rc, boxRc );
        AutoHRGN hrgnChild(::CreateRectRgnIndirect(&rc));
        ::CombineRgn(hrgn, hrgn, hrgnChild, RGN_DIFF);
    }

    return (WXHRGN)hrgn;
}

// helper for OnPaint(): really erase the background, i.e. do it even if we
// don't have any non default brush for doing it (DoEraseBackground() doesn't
// do anything in such case)
void wxStaticBox::PaintBackground(wxDC& dc, const RECT& rc)
{
    wxMSWDCImpl *impl = (wxMSWDCImpl*) dc.GetImpl();
    HBRUSH hbr = MSWGetBgBrush(impl->GetHDC());

    // if there is no special brush for painting this control, just use the
    // solid background colour
    wxBrush brush;
    if ( !hbr )
    {
        brush = wxBrush(GetParent()->GetBackgroundColour());
        hbr = GetHbrushOf(brush);
    }

    ::FillRect(GetHdcOf(*impl), &rc, hbr);
}

void wxStaticBox::PaintForeground(wxDC& dc, const RECT&)
{
    wxMSWDCImpl *impl = (wxMSWDCImpl*) dc.GetImpl();

    // Optionally use this pen to draw a border which has less contrast in dark
    // mode than the default white box which is "too shiny"
    wxPen penBorder;
    if ( wxMSWDarkMode::IsActive() )
    {
        penBorder = wxMSWDarkMode::GetBorderPen();
    }

    if ( penBorder.IsOk() )
    {
        const wxRect clientRect = GetClientRect();
        wxRect rect = clientRect;
        wxDCBrushChanger brushChanger(dc, *wxTRANSPARENT_BRUSH);
        wxDCPenChanger penChanger(dc, penBorder);

        // Note that we want to to do this even if our label is empty because
        // this ensures that the border appears at the same position for the
        // boxes with and without labels.
        if ( !m_labelWin )
        {
            // if the control has a font, use it
            wxDCFontChanger fontChanger(dc);
            if ( GetFont().IsOk() )
            {
                dc.SetFont(GetFont());
            }

            // Make sure that the label is vertically aligned with the border
            //
            // Use "Tp" as our sampling text to get the
            // maximum height from the current font
            const wxCoord height = dc.GetTextExtent(L"Tp").y;

            // adjust the border height & Y coordinate
            const int offsetFromTop = height / 2;
            rect.SetTop(offsetFromTop);
            rect.SetHeight(rect.GetHeight() - offsetFromTop);
        }

        dc.DrawRectangle(rect);
    }
    else
    {
        MSWDefWindowProc(WM_PAINT, (WPARAM)GetHdcOf(*impl), 0);
    }

#if wxUSE_UXTHEME
    // when using XP themes, neither setting the text colour nor transparent
    // background mode changes anything: the static box def window proc
    // still draws the label in its own colours, so we need to redraw the text
    // ourselves if we have a non default fg colour
    if ( m_hasFgCol && wxUxThemeIsActive() && !m_labelWin && !GetLabel().empty() )
    {
        // draw over the text in default colour in our colour
        HDC hdc = GetHdcOf(*impl);
        ::SetTextColor(hdc, GetForegroundColour().GetPixel());

        // Get dimensions of the label
        const wxString label = GetLabel();

        // choose the correct font
        AutoHFONT font;
        SelectInHDC selFont;
        if ( m_hasFont )
        {
            selFont.Init(hdc, GetHfontOf(GetFont()));
        }
        else // no font set, use the one set by the theme
        {
            wxUxThemeHandle hTheme(this, L"BUTTON");
            if ( hTheme )
            {
                LOGFONTW themeFont;
                if ( ::GetThemeFont
                                             (
                                                hTheme,
                                                hdc,
                                                BP_GROUPBOX,
                                                GBS_NORMAL,
                                                TMT_FONT,
                                                &themeFont
                                             ) == S_OK )
                {
                    font.Init(themeFont);
                    if ( font )
                        selFont.Init(hdc, font);
                }
            }
        }

        // Get the font extent
        int width, height;
        dc.GetTextExtent(wxStripMenuCodes(label, wxStrip_Mnemonics),
                         &width, &height);

        // first we need to correctly paint the background of the label
        // as Windows ignores the brush offset when doing it
        // NOTE: Border intentionally does not use DIPs in order to match native look
        const int x = LABEL_HORZ_OFFSET;
        RECT dimensions = { x, 0, 0, height };
        dimensions.left = x;
        dimensions.right = x + width;

        // need to adjust the rectangle to cover all the label background
        dimensions.left -= LABEL_HORZ_BORDER;
        dimensions.right += LABEL_HORZ_BORDER;
        dimensions.bottom += LABEL_VERT_BORDER;

        if ( UseBgCol() )
        {
            // our own background colour should be used for the background of
            // the label: this is consistent with the behaviour under pre-XP
            // systems (i.e. without visual themes) and generally makes sense
            wxBrush brush = wxBrush(GetBackgroundColour());
            ::FillRect(hdc, &dimensions, GetHbrushOf(brush));
        }
        else // paint parent background
        {
            PaintBackground(dc, dimensions);
        }

        UINT drawTextFlags = DT_SINGLELINE | DT_VCENTER;

        // determine the state of UI queues to draw the text correctly under XP
        // and later systems
        static const bool isXPorLater = wxGetWinVersion() >= wxWinVersion_XP;
        if ( isXPorLater )
        {
            if ( ::SendMessage(GetHwnd(), WM_QUERYUISTATE, 0, 0) &
                    UISF_HIDEACCEL )
            {
                drawTextFlags |= DT_HIDEPREFIX;
            }
        }

        // now draw the text
        RECT rc2 = { x, 0, x + width, height };
        ::DrawText(hdc, label.t_str(), label.length(), &rc2,
                   drawTextFlags);
    }
#endif // wxUSE_UXTHEME
}

void wxStaticBox::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    RECT rc;
    ::GetClientRect(GetHwnd(), &rc);
    wxPaintDC dc(this);

    // No need to do anything if the client rectangle is empty and, worse,
    // doing it would result in an assert when creating the bitmap below.
    if ( !rc.right || !rc.bottom )
        return;

    // draw the entire box in a memory DC
    wxMemoryDC memdc(&dc);

    const double scale = dc.GetContentScaleFactor();
    wxBitmap bitmap;
    bitmap.CreateWithDIPSize(rc.right / scale, rc.bottom / scale, scale);
    memdc.SelectObject(bitmap);

    PaintBackground(memdc, rc);
    PaintForeground(memdc, rc);

    // now only blit the static box border itself, not the interior, to avoid
    // flicker when background is drawn below
    //
    // note that it seems to be faster to do 4 small blits here and then paint
    // directly into wxPaintDC than painting background in wxMemoryDC and then
    // blitting everything at once to wxPaintDC, this is why we do it like this
    int borderTop, border;
    GetBordersForSizer(&borderTop, &border);

    // top
    if ( m_labelWin )
    {
        // We also have to exclude the area taken by the label window,
        // otherwise there would be flicker when it draws itself on top of it.
        const wxRect labelRect = m_labelWin->GetRect();

        // We also leave a small border around label window to make it appear
        // more similarly to a plain text label.
        const int gap = FromDIP(LABEL_HORZ_BORDER);

        dc.Blit(border, 0,
                labelRect.GetLeft() - gap - border,
                borderTop,
                &memdc, border, 0);

        const int xStart = labelRect.GetRight() + gap;
        dc.Blit(xStart, 0, rc.right - xStart, borderTop,
                &memdc, xStart, 0);
    }
    else
    {
        dc.Blit(border, 0, rc.right - border, borderTop,
                &memdc, border, 0);
    }

    // bottom
    dc.Blit(border, rc.bottom - border, rc.right - border, border,
            &memdc, border, rc.bottom - border);
    // left
    dc.Blit(0, 0, border, rc.bottom,
            &memdc, 0, 0);
    // right (note that upper and bottom right corners were already part of the
    // first two blits so we shouldn't overwrite them here to avoi flicker)
    dc.Blit(rc.right - border, borderTop,
            border, rc.bottom - borderTop - border,
            &memdc, rc.right - border, borderTop);


    // create the region excluding box children
    AutoHRGN hrgn((HRGN)MSWGetRegionWithoutChildren());
    RECT rcWin;
    ::GetWindowRect(GetHwnd(), &rcWin);
    ::OffsetRgn(hrgn, -rcWin.left, -rcWin.top);

    // and also the box itself
    MSWGetRegionWithoutSelf((WXHRGN) hrgn, rc.right, rc.bottom);
    wxMSWDCImpl *impl = (wxMSWDCImpl*) dc.GetImpl();
    HDCClipper clipToBg(GetHdcOf(*impl), hrgn);

    // paint the inside of the box (excluding box itself and child controls)
    PaintBackground(dc, rc);
}

#endif // wxUSE_STATBOX
