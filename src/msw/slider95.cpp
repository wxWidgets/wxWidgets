/////////////////////////////////////////////////////////////////////////////
// Name:        msw/slider95.cpp
// Purpose:     wxSlider95, using the Win95 trackbar control
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart 1998
//                  Vadim Zeitlin 2004
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "slider95.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_SLIDER

#ifndef WX_PRECOMP
    #include "wx/brush.h"
#endif

#include "wx/slider.h"
#include "wx/msw/subwin.h"

#if !(defined(__GNUWIN32_OLD__) && !defined(__CYGWIN10__))
    #include <commctrl.h>
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// indices of labels in wxSlider95::m_labels
enum
{
    SliderLabel_Min,
    SliderLabel_Max,
    SliderLabel_Value,
    SliderLabel_Last
};

// the gap between the slider and the labels, in pixels
static const int HGAP = 5;

// ----------------------------------------------------------------------------
// XTI
// ----------------------------------------------------------------------------

#if wxUSE_EXTENDED_RTTI
WX_DEFINE_FLAGS( wxSliderStyle )

wxBEGIN_FLAGS( wxSliderStyle )
    // new style border flags, we put them first to
    // use them for streaming out
    wxFLAGS_MEMBER(wxBORDER_SIMPLE)
    wxFLAGS_MEMBER(wxBORDER_SUNKEN)
    wxFLAGS_MEMBER(wxBORDER_DOUBLE)
    wxFLAGS_MEMBER(wxBORDER_RAISED)
    wxFLAGS_MEMBER(wxBORDER_STATIC)
    wxFLAGS_MEMBER(wxBORDER_NONE)

    // old style border flags
    wxFLAGS_MEMBER(wxSIMPLE_BORDER)
    wxFLAGS_MEMBER(wxSUNKEN_BORDER)
    wxFLAGS_MEMBER(wxDOUBLE_BORDER)
    wxFLAGS_MEMBER(wxRAISED_BORDER)
    wxFLAGS_MEMBER(wxSTATIC_BORDER)
    wxFLAGS_MEMBER(wxBORDER)

    // standard window styles
    wxFLAGS_MEMBER(wxTAB_TRAVERSAL)
    wxFLAGS_MEMBER(wxCLIP_CHILDREN)
    wxFLAGS_MEMBER(wxTRANSPARENT_WINDOW)
    wxFLAGS_MEMBER(wxWANTS_CHARS)
    wxFLAGS_MEMBER(wxFULL_REPAINT_ON_RESIZE)
    wxFLAGS_MEMBER(wxALWAYS_SHOW_SB )
    wxFLAGS_MEMBER(wxVSCROLL)
    wxFLAGS_MEMBER(wxHSCROLL)

    wxFLAGS_MEMBER(wxSL_HORIZONTAL)
    wxFLAGS_MEMBER(wxSL_VERTICAL)
    wxFLAGS_MEMBER(wxSL_AUTOTICKS)
    wxFLAGS_MEMBER(wxSL_LABELS)
    wxFLAGS_MEMBER(wxSL_LEFT)
    wxFLAGS_MEMBER(wxSL_TOP)
    wxFLAGS_MEMBER(wxSL_RIGHT)
    wxFLAGS_MEMBER(wxSL_BOTTOM)
    wxFLAGS_MEMBER(wxSL_BOTH)
    wxFLAGS_MEMBER(wxSL_SELRANGE)

wxEND_FLAGS( wxSliderStyle )

IMPLEMENT_DYNAMIC_CLASS_XTI(wxSlider95, wxControl,"wx/scrolbar.h")

wxBEGIN_PROPERTIES_TABLE(wxSlider95)
    wxEVENT_RANGE_PROPERTY( Scroll , wxEVT_SCROLL_TOP , wxEVT_SCROLL_ENDSCROLL , wxScrollEvent )
    wxEVENT_PROPERTY( Updated , wxEVT_COMMAND_SLIDER_UPDATED , wxCommandEvent )

    wxPROPERTY( Value , int , SetValue, GetValue , 0, 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY( Minimum , int , SetMin, GetMin, 0 , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY( Maximum , int , SetMax, GetMax, 0 , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY( PageSize , int , SetPageSize, GetLineSize, 1 , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY( LineSize , int , SetLineSize, GetLineSize, 1 , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY( ThumbLength , int , SetThumbLength, GetThumbLength, 1 , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY_FLAGS( WindowStyle , wxSliderStyle , long , SetWindowStyleFlag , GetWindowStyleFlag , EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // style
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxSlider95)
wxEND_HANDLERS_TABLE()

wxCONSTRUCTOR_8( wxSlider95 , wxWindow* , Parent , wxWindowID , Id , int , Value , int , Minimum , int , Maximum , wxPoint , Position , wxSize , Size , long , WindowStyle )
#else
IMPLEMENT_DYNAMIC_CLASS(wxSlider95, wxControl)
#endif

// ============================================================================
// wxSlider95 implementation
// ============================================================================

// ----------------------------------------------------------------------------
// construction
// ----------------------------------------------------------------------------

void wxSlider95::Init()
{
    m_labels = NULL;

    m_pageSize = 1;
    m_lineSize = 1;
    m_rangeMax = 0;
    m_rangeMin = 0;
    m_tickFreq = 0;
}

bool
wxSlider95::Create(wxWindow *parent,
                   wxWindowID id,
                   int value,
                   int minValue,
                   int maxValue,
                   const wxPoint& pos,
                   const wxSize& size,
                   long style,
                   const wxValidator& validator,
                   const wxString& name)
{
    // initialize everything
    if ( !CreateControl(parent, id, pos, size, style, validator, name) )
        return false;

    // ensure that we have correct values for GetLabelsSize()
    m_rangeMin = minValue;
    m_rangeMax = maxValue;

    // create the labels first, so that our DoGetBestSize() could take them
    // into account
    //
    // note that we could simply create 3 wxStaticTexts here but it could
    // result in some observable side effects at wx level (e.g. the parent of
    // wxSlider would have 3 more children than expected) and so we prefer not
    // to do it like this
    if ( m_windowStyle & wxSL_LABELS )
    {
        m_labels = new wxSubwindows(SliderLabel_Last);

        HWND hwndParent = GetHwndOf(parent);
        for ( size_t n = 0; n < SliderLabel_Last; n++ )
        {
            (*m_labels)[n] = ::CreateWindow
                               (
                                    wxT("STATIC"),
                                    NULL,
                                    WS_CHILD | WS_VISIBLE | SS_CENTER,
                                    0, 0, 0, 0,
                                    hwndParent,
                                    (HMENU)NewControlId(),
                                    wxGetInstance(),
                                    NULL
                               );
        }

        m_labels->SetFont(GetFont());
    }

    // now create the main control too
    if ( !MSWCreateControl(TRACKBAR_CLASS, wxEmptyString, pos, size) )
        return false;

    // and initialize everything
    SetRange(minValue, maxValue);
    SetValue(value);
    SetPageSize((maxValue - minValue)/10);

    // we need to position the labels correctly if we have them and if
    // SetSize() hadn't been called before (when best size was determined by
    // MSWCreateControl()) as in this case they haven't been put in place yet
    if ( m_labels && size.x != wxDefaultCoord && size.y != wxDefaultCoord )
    {
        SetSize(size);
    }

    return true;
}

WXDWORD wxSlider95::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    WXDWORD msStyle = wxControl::MSWGetStyle(style, exstyle);

    // TBS_HORZ is 0 anyhow, but do mention it explicitly for clarity
    msStyle |= style & wxSL_VERTICAL ? TBS_VERT : TBS_HORZ;

    if ( style & wxSL_AUTOTICKS )
        msStyle |= TBS_AUTOTICKS ;

    // again, TBS_RIGHT is 0 but do include it for clarity
    if ( style & wxSL_LEFT )
        msStyle |= TBS_LEFT;
    else if ( style & wxSL_RIGHT )
        msStyle |= TBS_RIGHT;
    else if ( style & wxSL_TOP )
        msStyle |= TBS_TOP;
    else if ( style & wxSL_BOTTOM )
        msStyle |= TBS_BOTTOM;
    else if ( style & wxSL_BOTH )
        msStyle |= TBS_BOTH;
    else if ( !(style & wxSL_AUTOTICKS) )
        msStyle |= TBS_NOTICKS;

    if ( style & wxSL_SELRANGE )
        msStyle |= TBS_ENABLESELRANGE;

    return msStyle;
}

wxSlider95::~wxSlider95()
{
    delete m_labels;
}

// ----------------------------------------------------------------------------
// event handling
// ----------------------------------------------------------------------------

bool wxSlider95::MSWOnScroll(int WXUNUSED(orientation),
                             WXWORD wParam,
                             WXWORD WXUNUSED(pos),
                             WXHWND control)
{
    wxEventType scrollEvent;
    switch ( wParam )
    {
        case SB_TOP:
            scrollEvent = wxEVT_SCROLL_TOP;
            break;

        case SB_BOTTOM:
            scrollEvent = wxEVT_SCROLL_BOTTOM;
            break;

        case SB_LINEUP:
            scrollEvent = wxEVT_SCROLL_LINEUP;
            break;

        case SB_LINEDOWN:
            scrollEvent = wxEVT_SCROLL_LINEDOWN;
            break;

        case SB_PAGEUP:
            scrollEvent = wxEVT_SCROLL_PAGEUP;
            break;

        case SB_PAGEDOWN:
            scrollEvent = wxEVT_SCROLL_PAGEDOWN;
            break;

        case SB_THUMBTRACK:
            scrollEvent = wxEVT_SCROLL_THUMBTRACK;
            break;

        case SB_THUMBPOSITION:
            scrollEvent = wxEVT_SCROLL_THUMBRELEASE;
            break;

        case SB_ENDSCROLL:
            scrollEvent = wxEVT_SCROLL_ENDSCROLL;
            break;

        default:
            // unknown scroll event?
            return false;
    }

    int newPos = (int) ::SendMessage((HWND) control, TBM_GETPOS, 0, 0);
    if ( (newPos < GetMin()) || (newPos > GetMax()) )
    {
        // out of range - but we did process it
        return true;
    }

    SetValue(newPos);

    wxScrollEvent event(scrollEvent, m_windowId);
    event.SetPosition(newPos);
    event.SetEventObject( this );
    GetEventHandler()->ProcessEvent(event);

    wxCommandEvent cevent( wxEVT_COMMAND_SLIDER_UPDATED, GetId() );
    cevent.SetInt( newPos );
    cevent.SetEventObject( this );

    return GetEventHandler()->ProcessEvent( cevent );
}

void wxSlider95::Command (wxCommandEvent & event)
{
    SetValue (event.GetInt());
    ProcessCommand (event);
}

// ----------------------------------------------------------------------------
// geometry stuff
// ----------------------------------------------------------------------------

wxRect wxSlider95::GetBoundingBox() const
{
    // take care not to call our own functions which would call us recursively
    int x, y, w, h;
    wxSliderBase::DoGetPosition(&x, &y);
    wxSliderBase::DoGetSize(&w, &h);

    wxRect rect(x, y, w, h);
    if ( m_labels )
    {
        wxRect lrect = m_labels->GetBoundingBox();
        GetParent()->ScreenToClient(&lrect.x, &lrect.y);
        rect.Union(lrect);
    }

    return rect;
}

void wxSlider95::DoGetSize(int *width, int *height) const
{
    wxRect rect = GetBoundingBox();

    if ( width )
        *width = rect.width;
    if ( height )
        *height = rect.height;
}

void wxSlider95::DoGetPosition(int *x, int *y) const
{
    wxRect rect = GetBoundingBox();

    if ( x )
        *x = rect.x;
    if ( y )
        *y = rect.y;
}

int wxSlider95::GetLabelsSize(int *width) const
{
    int cy;

    if ( width )
    {
        // find the max label width
        int wLabelMin, wLabelMax;
        GetTextExtent(Format(m_rangeMin), &wLabelMin, &cy);
        GetTextExtent(Format(m_rangeMax), &wLabelMax, &cy);

        *width = wxMax(wLabelMin, wLabelMax);
    }
    else
    {
        cy = GetCharHeight();
    }

    return EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy);
}

void wxSlider95::DoMoveWindow(int x, int y, int width, int height)
{
    // all complications below are because we need to position the labels,
    // without them everything is easy
    if ( !m_labels )
    {
        wxSliderBase::DoMoveWindow(x, y, width, height);
        return;
    }

    // be careful to position the slider itself after moving the labels as
    // otherwise our GetBoundingBox(), which is called from WM_SIZE handler,
    // would return a wrong result and wrong size would be cached internally
    if ( HasFlag(wxSL_VERTICAL) )
    {
        int wLabel;
        int hLabel = GetLabelsSize(&wLabel);

        int xLabel = HasFlag(wxSL_LEFT) ? x + width - wLabel : x;

        // position all labels: min at the top, value in the middle and max at
        // the bottom
        ::MoveWindow((*m_labels)[SliderLabel_Min],
                     xLabel, y, wLabel, hLabel, TRUE);

        ::MoveWindow((*m_labels)[SliderLabel_Value],
                     xLabel, y + (height - hLabel)/2, wLabel, hLabel, TRUE);

        ::MoveWindow((*m_labels)[SliderLabel_Max],
                     xLabel, y + height - hLabel, wLabel, hLabel, TRUE);

        // position the slider itself along the left/right edge
        ::MoveWindow(GetHwnd(),
                     HasFlag(wxSL_LEFT) ? x : x + wLabel + HGAP,
                     y + hLabel/2,
                     width - wLabel - HGAP,
                     height - hLabel,
                     TRUE);
    }
    else // horizontal
    {
        int wLabel;
        int hLabel = GetLabelsSize(&wLabel);

        int yLabel = HasFlag(wxSL_TOP) ? y + height - hLabel : y;

        // position all labels: min on the left, value in the middle and max to
        // the right
        ::MoveWindow((*m_labels)[SliderLabel_Min],
                     x, yLabel, wLabel, hLabel, TRUE);

        ::MoveWindow((*m_labels)[SliderLabel_Value],
                     x + (width - wLabel)/2, yLabel, wLabel, hLabel, TRUE);

        ::MoveWindow((*m_labels)[SliderLabel_Max],
                     x + width - wLabel, yLabel, wLabel, hLabel, TRUE);

        // position the slider itself along the top/bottom edge
        ::MoveWindow(GetHwnd(),
                     x,
                     HasFlag(wxSL_TOP) ? y : y + hLabel,
                     width,
                     height - hLabel,
                     TRUE);
    }
}

wxSize wxSlider95::DoGetBestSize() const
{
    // these values are arbitrary
    static const int length = 100;
    static const int thickness = 26;

    wxSize size;
    if ( HasFlag(wxSL_VERTICAL) )
    {
        size.x = thickness;
        size.y = length;

        if ( m_labels )
        {
            int wLabel;
            int hLabel = GetLabelsSize(&wLabel);

            // account for the labels
            size.x += HGAP + wLabel;

            // labels are indented relative to the slider itself
            size.y += hLabel;
        }
    }
    else // horizontal
    {
        size.x = length;
        size.y = thickness;

        if ( m_labels )
        {
            // labels add extra height
            size.y += GetLabelsSize();
        }
    }

    return size;
}

// ----------------------------------------------------------------------------
// slider-specific methods
// ----------------------------------------------------------------------------

int wxSlider95::GetValue() const
{
    return ::SendMessage(GetHwnd(), TBM_GETPOS, 0, 0);
}

void wxSlider95::SetValue(int value)
{
    ::SendMessage(GetHwnd(), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)value);

    if ( m_labels )
    {
        ::SetWindowText((*m_labels)[SliderLabel_Value], Format(value));
    }
}

void wxSlider95::SetRange(int minValue, int maxValue)
{
    m_rangeMin = minValue;
    m_rangeMax = maxValue;

    ::SendMessage(GetHwnd(), TBM_SETRANGE, TRUE, MAKELONG(minValue, maxValue));

    if ( m_labels )
    {
        ::SetWindowText((*m_labels)[SliderLabel_Min], Format(m_rangeMin));
        ::SetWindowText((*m_labels)[SliderLabel_Max], Format(m_rangeMax));
    }
}

void wxSlider95::SetTickFreq(int n, int pos)
{
    m_tickFreq = n;
    ::SendMessage( GetHwnd(), TBM_SETTICFREQ, (WPARAM) n, (LPARAM) pos );
}

void wxSlider95::SetPageSize(int pageSize)
{
    ::SendMessage( GetHwnd(), TBM_SETPAGESIZE, (WPARAM) 0, (LPARAM) pageSize );
    m_pageSize = pageSize;
}

int wxSlider95::GetPageSize() const
{
    return m_pageSize;
}

void wxSlider95::ClearSel()
{
    ::SendMessage(GetHwnd(), TBM_CLEARSEL, (WPARAM) TRUE, (LPARAM) 0);
}

void wxSlider95::ClearTicks()
{
    ::SendMessage(GetHwnd(), TBM_CLEARTICS, (WPARAM) TRUE, (LPARAM) 0);
}

void wxSlider95::SetLineSize(int lineSize)
{
    m_lineSize = lineSize;
    ::SendMessage(GetHwnd(), TBM_SETLINESIZE, (WPARAM) 0, (LPARAM) lineSize);
}

int wxSlider95::GetLineSize() const
{
    return (int)::SendMessage(GetHwnd(), TBM_GETLINESIZE, 0, 0);
}

int wxSlider95::GetSelEnd() const
{
    return (int)::SendMessage(GetHwnd(), TBM_SETSELEND, 0, 0);
}

int wxSlider95::GetSelStart() const
{
    return (int)::SendMessage(GetHwnd(), TBM_GETSELSTART, 0, 0);
}

void wxSlider95::SetSelection(int minPos, int maxPos)
{
    ::SendMessage(GetHwnd(), TBM_SETSEL,
                  (WPARAM) TRUE /* redraw */,
                  (LPARAM) MAKELONG( minPos, maxPos) );
}

void wxSlider95::SetThumbLength(int len)
{
    ::SendMessage(GetHwnd(), TBM_SETTHUMBLENGTH, (WPARAM) len, (LPARAM) 0);
}

int wxSlider95::GetThumbLength() const
{
  return (int)::SendMessage( GetHwnd(), TBM_GETTHUMBLENGTH, 0, 0);
}

void wxSlider95::SetTick(int tickPos)
{
  ::SendMessage( GetHwnd(), TBM_SETTIC, (WPARAM) 0, (LPARAM) tickPos );
}

// ----------------------------------------------------------------------------
// composite control methods
// ----------------------------------------------------------------------------

WXHWND wxSlider95::GetStaticMin() const
{
    return m_labels ? (WXHWND)(*m_labels)[SliderLabel_Min] : NULL;
}

WXHWND wxSlider95::GetStaticMax() const
{
    return m_labels ? (WXHWND)(*m_labels)[SliderLabel_Max] : NULL;
}

WXHWND wxSlider95::GetEditValue() const
{
    return m_labels ? (WXHWND)(*m_labels)[SliderLabel_Value] : NULL;
}

WX_FORWARD_STD_METHODS_TO_SUBWINDOWS(wxSlider95, wxSliderBase, m_labels)

#endif // wxUSE_SLIDER
