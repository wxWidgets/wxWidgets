/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/spinbutt.cpp
// Purpose:     wxSpinButton
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


#ifndef WX_PRECOMP
    #include "wx/msw/wrapcctl.h" // include <commctrl.h> "properly"
    #include "wx/app.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
#endif

#if wxUSE_SPINBTN

#include "wx/spinbutt.h"

#include "wx/msw/dc.h"
#include "wx/msw/private.h"
#include "wx/msw/private/darkmode.h"

#ifndef UDM_SETRANGE32
    #define UDM_SETRANGE32 (WM_USER+111)
#endif

#ifndef UDM_SETPOS32
    #define UDM_SETPOS32 (WM_USER+113)
    #define UDM_GETPOS32 (WM_USER+114)
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// wxSpinButton
// ----------------------------------------------------------------------------

bool wxSpinButton::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxString& name)
{
    // basic initialization
    m_windowId = (id == wxID_ANY) ? NewControlId() : id;

    SetName(name);

    int x = pos.x;
    int y = pos.y;
    int width = size.x;
    int height = size.y;

    m_windowStyle = style;

    SetParent(parent);

    // get the right size for the control
    if ( width <= 0 || height <= 0 )
    {
        wxSize bestSize = DoGetBestSize();
        if ( width <= 0 )
            width = bestSize.x;
        if ( height <= 0 )
            height = bestSize.y;
    }

    if ( x < 0 )
        x = 0;
    if ( y < 0 )
        y = 0;

    // translate the styles
    DWORD wstyle = WS_VISIBLE | WS_CHILD | WS_TABSTOP | /*  WS_CLIPSIBLINGS | */
                   UDS_NOTHOUSANDS | // never useful, sometimes harmful
                   UDS_ALIGNRIGHT  | // these styles are effectively used only
                   UDS_SETBUDDYINT;  //  by wxSpinCtrl but do no harm otherwise

    if ( m_windowStyle & wxCLIP_SIBLINGS )
        wstyle |= WS_CLIPSIBLINGS;
    if ( m_windowStyle & wxSP_HORIZONTAL )
        wstyle |= UDS_HORZ;
    if ( m_windowStyle & wxSP_ARROW_KEYS )
        wstyle |= UDS_ARROWKEYS;
    if ( m_windowStyle & wxSP_WRAP )
        wstyle |= UDS_WRAP;

    // create the UpDown control.
    m_hWnd = (WXHWND)CreateUpDownControl
                     (
                       wstyle,
                       x, y, width, height,
                       GetHwndOf(parent),
                       m_windowId,
                       wxGetInstance(),
                       nullptr, // no buddy
                       m_max, m_min,
                       m_min // initial position
                     );

    if ( !m_hWnd )
    {
        wxLogLastError(wxT("CreateUpDownControl"));

        return false;
    }

    if ( parent )
    {
        parent->AddChild(this);
    }

    SubclassWin(m_hWnd);

    Bind(wxEVT_PAINT, &wxSpinButton::OnPaint, this);

    SetInitialSize(size);

    return true;
}

wxSpinButton::~wxSpinButton()
{
}

// ----------------------------------------------------------------------------
// size calculation
// ----------------------------------------------------------------------------

wxSize wxSpinButton::DoGetBestSize() const
{
    const bool vert = HasFlag(wxSP_VERTICAL);

    wxSize bestSize(wxGetSystemMetrics(vert ? SM_CXVSCROLL : SM_CXHSCROLL, m_parent),
                    wxGetSystemMetrics(vert ? SM_CYVSCROLL : SM_CYHSCROLL, m_parent));

    if ( vert )
        bestSize.y *= 2;
    else
        bestSize.x *= 2;

    return bestSize;
}

// ----------------------------------------------------------------------------
// painting
// ----------------------------------------------------------------------------

void wxSpinButton::OnPaint(wxPaintEvent& event)
{
    if ( wxMSWDarkMode::IsActive() )
    {
        // Unfortunately PaintIfNecessary() can't be used here as we need to
        // handle the extra border below, so duplicate what it does here.
        const RECT rc = wxGetClientRect(GetHwnd());
        const wxSize size{rc.right - rc.left, rc.bottom - rc.top};

        if ( size == wxSize() )
            return;

        wxBitmap bmp(size);
        {
            wxMemoryDC mdc(bmp);

            ::CallWindowProc(m_oldWndProc,
                             GetHwnd(), WM_PAINT, (WPARAM)GetHdcOf(mdc), 0);
        }

#if wxUSE_IMAGE
        // When using a buddy control, the spin button tries to mimic being a
        // part of it by adding an extra border, not used for standalone
        // controls. This doesn't work very well even in light mode in modern
        // Windows (it was apparently done for the classic 3D appearance and
        // never updated since then), but looks completely horrible in dark
        // mode, so we must get rid of this border by overdrawing it.
        const bool drawBorder = ::SendMessage(GetHwnd(), UDM_GETBUDDY, 0, 0);
        wxImage::RGBValue border;
        if ( drawBorder )
        {
            const auto col = wxMSWDarkMode::GetBorderPen().GetColour();
            border.red = col.GetRed();
            border.green = col.GetGreen();
            border.blue = col.GetBlue();
        }

        wxImage image = bmp.ConvertToImage();

        const int width = image.GetWidth();
        const int height = image.GetHeight();
        unsigned char *data = image.GetData();
        unsigned char *alpha = image.GetAlpha();
        for ( int y = 0; y < height; ++y )
        {
            for ( int x = 0; x < width; ++x )
            {
                wxImage::RGBValue rgb(data[0], data[1], data[2]);

                if ( drawBorder &&
                        (y == 0 || y == height - 1 || x == width - 1) )
                {
                    rgb = border;
                    if ( alpha )
                        *alpha = wxALPHA_OPAQUE;
                }
                else
                {
                    // This uses a slightly different formula than the one in
                    // InvertBitmapPixel() because the one there results in the
                    // lines being too bright.
                    auto hsv = wxImage::RGBtoHSV(rgb);
                    hsv.value = 1.0 - hsv.value;
                    rgb = wxImage::HSVtoRGB(hsv);
                }

                data[0] = rgb.red;
                data[1] = rgb.green;
                data[2] = rgb.blue;
                data += 3;

                if ( alpha )
                    alpha++;
            }
        }

        bmp = wxBitmap(image);
#endif // wxUSE_IMAGE

        PAINTSTRUCT ps;
        wxDCTemp dc(::BeginPaint(GetHwnd(), &ps), size);
        dc.DrawBitmap(bmp, 0, 0);
        ::EndPaint(GetHwnd(), &ps);
    }
    else
    {
        // We need to always paint this control explicitly instead of letting
        // DefWndProc() do it, as this avoids whichever optimization the latter
        // function does when WS_EX_COMPOSITED is on that result in not drawing
        // parts of the control at all (see #23656).
        wxPaintDC dc(this);

        wxSpinButtonBase::OnPaint(event);
    }
}

// ----------------------------------------------------------------------------
// value and range
// ----------------------------------------------------------------------------

int wxSpinButton::GetValue() const
{
    int n;
#ifdef UDM_GETPOS32
    // use the full 32 bit range if available
    n = ::SendMessage(GetHwnd(), UDM_GETPOS32, 0, 0);
#else
    // we're limited to 16 bit
    n = (short)LOWORD(::SendMessage(GetHwnd(), UDM_GETPOS, 0, 0));
#endif // UDM_GETPOS32

    if (n < m_min) n = m_min;
    if (n > m_max) n = m_max;

    return n;
}

void wxSpinButton::SetValue(int val)
{
    // wxSpinButtonBase::SetValue(val); -- no, it is pure virtual

#ifdef UDM_SETPOS32
    // use the full 32 bit range if available
    ::SendMessage(GetHwnd(), UDM_SETPOS32, 0, val);
#else
    ::SendMessage(GetHwnd(), UDM_SETPOS, 0, MAKELONG((short) val, 0));
#endif // UDM_SETPOS32
}

void wxSpinButton::NormalizeValue()
{
    SetValue( GetValue() );
}

void wxSpinButton::SetRange(int minVal, int maxVal)
{
    const bool hadRange = m_min < m_max;

    wxSpinButtonBase::SetRange(minVal, maxVal);

#ifdef UDM_SETRANGE32
    // use the full 32 bit range if available
    ::SendMessage(GetHwnd(), UDM_SETRANGE32, minVal, maxVal);
#else
    // we're limited to 16 bit
    ::SendMessage(GetHwnd(), UDM_SETRANGE, 0,
                  (LPARAM) MAKELONG((short)maxVal, (short)minVal));
#endif // UDM_SETRANGE32

    // the current value might be out of the new range, force it to be in it
    NormalizeValue();

    // if range was valid but becomes degenerated (min == max) now or vice
    // versa then the spin buttons are automatically disabled/enabled back
    // but don't update themselves for some reason, so do it manually
    if ( hadRange != (m_min < m_max) )
    {
        // update the visual state of the button
        Refresh();
    }
}

// ----------------------------------------------------------------------------
// event generation
// ----------------------------------------------------------------------------

bool wxSpinButton::MSWOnScroll(int WXUNUSED(orientation), WXWORD wParam,
                               WXWORD WXUNUSED(pos), WXHWND control)
{
    wxCHECK_MSG( control, false, wxT("scrolling what?") );

    if ( wParam != SB_THUMBPOSITION )
    {
        // probable SB_ENDSCROLL - we don't react to it
        return false;
    }

    wxSpinEvent event(wxEVT_SCROLL_THUMBTRACK, m_windowId);
    // We can't use 16 bit position provided in this message for spin buttons
    // using 32 bit range.
    event.SetPosition(GetValue());
    event.SetEventObject(this);

    return HandleWindowEvent(event);
}

bool wxSpinButton::MSWOnNotify(int WXUNUSED(idCtrl), WXLPARAM lParam, WXLPARAM *result)
{
    NM_UPDOWN *lpnmud = (NM_UPDOWN *)lParam;

    if ( lpnmud->hdr.hwndFrom != GetHwnd() || // make sure it is the right control
         lpnmud->hdr.code != UDN_DELTAPOS )   // and the right notification 
        return false;

    int newVal = lpnmud->iPos + lpnmud->iDelta;
    if ( newVal < m_min )
    {
        newVal = HasFlag(wxSP_WRAP) ? m_max : m_min;
    }
    else if ( newVal > m_max )
    {
        newVal = HasFlag(wxSP_WRAP) ? m_min : m_max;
    }

    // Don't send an event if the value hasn't actually changed (for compatibility with wxGTK and wxOSX).
    if ( newVal == lpnmud->iPos )
    {
        *result = 1;
        return true;
    }

    wxSpinEvent event(lpnmud->iDelta > 0 ? wxEVT_SCROLL_LINEUP
                                         : wxEVT_SCROLL_LINEDOWN,
                      m_windowId);
    event.SetPosition(newVal);
    event.SetEventObject(this);

    bool processed = HandleWindowEvent(event);

    *result = event.IsAllowed() ? 0 : 1;

    return processed;
}

// ----------------------------------------------------------------------------
// increment
// ----------------------------------------------------------------------------

void wxSpinButton::SetIncrement(int value)
{
    UDACCEL accel;
    accel.nSec = 0;
    accel.nInc = value;
    ::SendMessage(GetHwnd(), UDM_SETACCEL, 1, (LPARAM) &accel);
}

int  wxSpinButton::GetIncrement() const
{
    UDACCEL accel;

    // If the message is unsupported, this default value won't be modified and
    // will be returned below.
    accel.nInc = 1;

    ::SendMessage(GetHwnd(), UDM_GETACCEL, 1, (LPARAM) &accel);

    return accel.nInc;
}
#endif // wxUSE_SPINBTN
