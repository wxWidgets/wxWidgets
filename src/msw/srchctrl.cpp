/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/srchctrl.cpp
// Purpose:     wxSearchCtrl implementation - native
// Author:      Kettab Ali
// Created:     2019-12-23
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_SEARCHCTRL && wxUSE_GRAPHICS_CONTEXT

#include "wx/srchctrl.h"

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
    #include "wx/dc.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
    #include "wx/log.h"
    #include "wx/menu.h"
    #include "wx/settings.h"
#endif //WX_PRECOMP

#include "wx/msw/private.h"

#include <windowsx.h> // For GET_X_LPARAM and GET_Y_LPARAM macros

#include "wx/dcgraph.h"
#include "wx/graphics.h"
#include "wx/utils.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the margin between the text control and the search/cancel buttons
static const int MARGIN = 2;

// ============================================================================
// wxSearchCtrl implementation
// ============================================================================
wxBEGIN_EVENT_TABLE(wxSearchCtrl, wxTextCtrl)
    EVT_TEXT(wxID_ANY, wxSearchCtrl::OnText)
    EVT_TEXT_ENTER(wxID_ANY, wxSearchCtrl::OnTextEnter)
wxEND_EVENT_TABLE()

wxIMPLEMENT_DYNAMIC_CLASS(wxSearchCtrl, wxTextCtrl);

// ----------------------------------------------------------------------------
//  creation/destruction
// ----------------------------------------------------------------------------

// destruction
// -----------
wxSearchCtrl::~wxSearchCtrl()
{
#if wxUSE_MENUS
    delete m_menu;
#endif // wxUSE_MENUS
}

// creation
// --------

void wxSearchCtrl::Init()
{
    m_isCancelButtonShown = false;
    m_mouseInCancelButton = false;
#if wxUSE_MENUS
    m_menu = NULL;
#endif // wxUSE_MENUS
}

bool wxSearchCtrl::Create(wxWindow *parent, wxWindowID id,
            const wxString& value,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    if ( !wxTextCtrl::Create(parent, id, value, pos, size, style | wxTE_PROCESS_ENTER,
                             validator, name) )
    {
        wxFAIL_MSG( "wxSearchCtrl creation failed" );
        return false;
    }

    SetHint(_("Search"));

    return true;
}


bool
wxSearchCtrl::MSWHandleMessage(WXLRESULT *rc,
                               WXUINT nMsg,
                               WXWPARAM wParam,
                               WXLPARAM lParam)
{
    bool processed = false;
    const int bitmapWidth = GetSize().GetHeight();

    switch ( nMsg )
    {
        case WM_NCCALCSIZE:
            {
                if ( wParam )
                {
                    NCCALCSIZE_PARAMS* csparam = (NCCALCSIZE_PARAMS *)lParam;

                    // Where to draw the buttons depending on the layout,
                    //    LTR: search button (left), cancel button (right).
                    //    RTL: search button (right), cancel button (left).

                    if ( GetLayoutDirection() == wxLayout_LeftToRight )
                    {
                        csparam->rgrc [0].left  += bitmapWidth - MARGIN;

                        if ( IsCancelButtonVisible() )
                        {
                            csparam->rgrc [0].right -= bitmapWidth - MARGIN;
                        }
                    }
                    else // wxLayout_RightToLeft
                    {
                        csparam->rgrc [0].right -= bitmapWidth - MARGIN;

                        if ( IsCancelButtonVisible() )
                        {
                            csparam->rgrc [0].left  += bitmapWidth - MARGIN;
                        }
                    }
                }
            }
            break;

        case WM_NCPAINT:
            {
                const RECT rcWindow = wxGetWindowRect(GetHwnd());
                RECT rcClient = wxGetClientRect(GetHwnd());
                ::MapWindowPoints(GetHwnd(), NULL, reinterpret_cast<LPPOINT>(&rcClient), 2);

                RECT leftRect, rightRect;
                RECT *searchButtonRect, *cancelButtonRect;

                if ( GetLayoutDirection() == wxLayout_LeftToRight )
                {
                    searchButtonRect = &leftRect;
                    cancelButtonRect = &rightRect;
                }
                else // wxLayout_RightToLeft
                {
                    searchButtonRect = &rightRect;
                    cancelButtonRect = &leftRect;
                }

                // In RTL layout, we can skip leftRect (cancel button) calculation unless it's visible.
                if ( GetLayoutDirection() == wxLayout_LeftToRight || IsCancelButtonVisible() )
                {
                    // Calculate leftRect
                    leftRect = rcWindow;
                    leftRect.right = leftRect.left + bitmapWidth;

                    leftRect.top += rcClient.top - rcWindow.top;
                    leftRect.bottom -= rcWindow.bottom - rcClient.bottom;
                    ::OffsetRect(&leftRect, -rcWindow.left, -rcWindow.top);
                }
                
                // In LTR layout, we can skip rightRect (cancel button) calculation unless it's visible.
                if ( GetLayoutDirection() == wxLayout_RightToLeft || IsCancelButtonVisible() )
                {
                    // Calculate rightRect
                    rightRect = rcWindow;
                    rightRect.left = rightRect.right - bitmapWidth;

                    ::OffsetRect(&rightRect, rcClient.right + bitmapWidth - rcWindow.right, 0);
                    rightRect.top += rcClient.top - rcWindow.top;
                    rightRect.bottom -= rcWindow.bottom - rcClient.bottom;
                    ::OffsetRect(&rightRect, -rcWindow.left, -rcWindow.top);
                }

                // copy results
                wxCopyRECTToRect(*searchButtonRect, m_searchButtonRect);
                wxCopyRECTToRect(*cancelButtonRect, m_cancelButtonRect);

                DrawButtons(bitmapWidth);
            }
            break;

        case WM_NCHITTEST:
            {
                POINT pt;
                pt.x = GET_X_LPARAM(lParam);
                pt.y = GET_Y_LPARAM(lParam);
                ::MapWindowPoints(NULL, GetHwnd(), &pt, 1);

                const wxPoint point = wxPoint(pt.x + bitmapWidth, pt.y);

                if ( IsCancelButtonVisible() && m_cancelButtonRect.Contains(point) )
                {
                    m_mouseInCancelButton = true;
                    *rc = HTCAPTION;
                    processed = true;

                    TRACKMOUSEEVENT tme = { sizeof(tme), TME_NONCLIENT | TME_LEAVE, GetHwnd() };
                    TrackMouseEvent(&tme);
                }
                else if ( m_searchButtonRect.Contains(point) )
                {
                    *rc = HasMenu() ? HTCAPTION : HTCLIENT;
                    processed = true;
                }
            }
            break;

        case WM_NCLBUTTONDOWN:
            {
                ClickButton();
                processed = true;
            }
            break;

        case WM_NCMOUSELEAVE:
            {
                m_mouseInCancelButton = false;
            }
            wxFALLTHROUGH;

        case WM_NCMOUSEMOVE:
            {
                // if this causes a noticeable flicker, we better off handling
                // WM_NCMOUSEHOVER instead.
                ::RedrawWindow(GetHwnd(), NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
            }
            break;

            // Reduce noticeable flicker by processing WM_MOUSEMOVE here
            // by doing nothing if it just a mouse move only.
        case WM_MOUSEMOVE:
            {
                // ...text can be selected though.
                if ( wParam & MK_LBUTTON )
                    break;
            }
            wxFALLTHROUGH;

        case WM_NCLBUTTONDBLCLK:
            {
                *rc = 0;
                processed = true;
            }
            break;
    }

    return processed || wxTextCtrl::MSWHandleMessage(rc, nMsg, wParam, lParam);
}

void wxSearchCtrl::DrawButtons(int width)
{
    // For better results, the drawing is made with a larger dimension
    // (here we use 80) with an appropriate scale applied.
    const double xWidth = 80.;
    const double radius = xWidth / 4. + 5;
    const double scale  = 0.6 * (width / xWidth);
    const double shift  = (width / 4.) - 1;

    const wxColour bg = GetBackgroundColour();
    const wxColour fg = GetForegroundColour().ChangeLightness(130);

    wxWindowDC winDC(this);

    {
        wxGCDC gdc(winDC);
        wxDCClipper clip(gdc, m_searchButtonRect);

        gdc.SetDeviceOrigin(m_searchButtonRect.x + shift, shift);

        gdc.SetBackground(bg);
        gdc.Clear();

        gdc.SetUserScale(scale, scale);

        gdc.SetPen(wxPen(fg, 8));
        gdc.SetBrush(*wxTRANSPARENT_BRUSH);

        gdc.DrawCircle(xWidth - radius, radius, radius);
        gdc.DrawLine(0, xWidth, xWidth/2. - 5, xWidth/2. + 5);
    }

    if ( IsCancelButtonVisible() )
    {
        wxGCDC gdc(winDC);
        wxDCClipper clip(gdc, m_cancelButtonRect);

        // _shift_ value is calculated to work well with the calculated _scale_
        // above, but since we are applying an altered value of the _scale_ below,
        // a little adjustment to the _shift_ value seems to be necessary for better
        // positioning of the cancel button.
        const double adj = GetLayoutDirection() == wxLayout_LeftToRight ? 1 : 2;
        gdc.SetDeviceOrigin(m_cancelButtonRect.x + shift + adj, shift + adj);

        const wxColour& highlightColour = m_mouseInCancelButton ?
            wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT) : bg;
        gdc.SetBackground(highlightColour);
        gdc.Clear();

        gdc.SetUserScale(0.75 * scale, 0.75 * scale);

        gdc.SetPen(wxPen(fg, 10));
        gdc.SetBrush(*wxTRANSPARENT_BRUSH);

        gdc.DrawLine(0, 0, xWidth, xWidth);
        gdc.DrawLine(0, xWidth, xWidth, 0);
    }
}

// search control specific interfaces
// ----------------------------------
void wxSearchCtrl::ClickButton()
{
    if ( m_mouseInCancelButton )
    {
        Clear();
        ToggleCancelButtonVisibility();

        wxCommandEvent event(wxEVT_SEARCH_CANCEL, GetId());
        event.SetEventObject(this);
        HandleWindowEvent(event);
    }
#if wxUSE_MENUS
    else if ( HasMenu() )
    {
        if ( m_menu )
        {
            const wxSize& size = GetSize();
            PopupMenu(m_menu, 0, size.y);
        }
    }
#endif // wxUSE_MENUS
}

#if wxUSE_MENUS

void wxSearchCtrl::SetMenu(wxMenu* menu)
{
    if ( menu != m_menu )
    {
        delete m_menu;
        m_menu = menu;
    }
}

#endif // wxUSE_MENUS

void wxSearchCtrl::ShowSearchButton(bool WXUNUSED(show))
{
    // Search button is always shown.
}

bool wxSearchCtrl::IsSearchButtonVisible() const
{
    // Search button is always shown.
    return true;
}

void wxSearchCtrl::ShowCancelButton(bool WXUNUSED(show))
{
    // Cancel button is only shown if the control is not empty.
}

bool wxSearchCtrl::IsCancelButtonVisible() const
{
    return !IsEmpty();
}

void wxSearchCtrl::SetDescriptiveText(const wxString& text)
{
    wxTextEntry::SetHint(text);
}

wxString wxSearchCtrl::GetDescriptiveText() const
{
    return wxTextEntry::GetHint();
}


// Events
// ----------

void wxSearchCtrl::OnText(wxCommandEvent& event)
{
    event.Skip();
    ToggleCancelButtonVisibility();
}

void wxSearchCtrl::OnTextEnter(wxCommandEvent& WXUNUSED(event))
{
    if ( !IsEmpty() )
    {
        wxCommandEvent evt(wxEVT_SEARCH, GetId());
        evt.SetEventObject(this);
        evt.SetString(GetValue());

        ProcessWindowEvent(evt);
    }
}


void wxSearchCtrl::ToggleCancelButtonVisibility()
{
    if ( m_isCancelButtonShown != IsCancelButtonVisible() )
    {
        m_isCancelButtonShown = IsCancelButtonVisible();

        // Calling RedrawWindow() is of no help here, because to show/hide
        // the cancel button, WM_NCCALCSIZE must be sent to redo the needed
        // calculations. so we have to call SetWindowPos() instead.

        ::SetWindowPos(GetHwnd(), NULL, 0, 0, 0, 0,
                   SWP_FRAMECHANGED |
                   SWP_NOSIZE |
                   SWP_NOMOVE |
                   SWP_NOZORDER |
                   SWP_NOACTIVATE |
                   SWP_NOCOPYBITS |
                   SWP_NOOWNERZORDER |
                   SWP_NOSENDCHANGING);
    }
}

#endif // wxUSE_SEARCHCTRL
