/////////////////////////////////////////////////////////////////////////////
// Name:        slider95.cpp
// Purpose:     wxSlider95, using the Win95 trackbar control
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "slider95.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_SLIDER

#ifndef WX_PRECOMP
#include "wx/utils.h"
#include "wx/brush.h"
#include "wx/slider.h"
#endif

#ifdef __WIN95__

#include "wx/msw/slider95.h"
#include "wx/msw/private.h"

#if defined(__WIN95__) && !(defined(__GNUWIN32_OLD__) && !defined(__CYGWIN10__))
    #include <commctrl.h>
#endif

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

// Slider
wxSlider95::wxSlider95()
{
    m_staticValue = (WXHWND) NULL;
    m_staticMin = (WXHWND) NULL;
    m_staticMax = (WXHWND) NULL;
    m_pageSize = 1;
    m_lineSize = 1;
    m_rangeMax = 0;
    m_rangeMin = 0;
    m_tickFreq = 0;
}

bool wxSlider95::Create(wxWindow *parent, wxWindowID id,
           int value, int minValue, int maxValue,
           const wxPoint& pos,
           const wxSize& size, long style,
           const wxValidator& validator,
           const wxString& name)
{
    // default is no border
    if ( (style & wxBORDER_MASK) == wxBORDER_DEFAULT )
        style |= wxBORDER_NONE;

    if ( !CreateBase(parent, id, pos, size, style, validator, name) )
        return false;

    if (parent) parent->AddChild(this);

    InheritAttributes();

    m_staticValue = (WXHWND) NULL;;
    m_staticMin = (WXHWND) NULL;;
    m_staticMax = (WXHWND) NULL;;
    m_pageSize = 1;
    m_lineSize = 1;
    m_windowStyle = style;
    m_tickFreq = 0;

    long msStyle = 0;

    if ( m_windowStyle & wxSL_LABELS )
    {
        msStyle |= SS_CENTER|WS_VISIBLE;

        WXDWORD exStyle = 0;
        long valueStyle = m_windowStyle & ~wxBORDER_MASK;
        valueStyle |= wxBORDER_SUNKEN;
        msStyle |= MSWGetStyle(valueStyle, & exStyle) ;

        m_staticValue = (WXHWND) CreateWindowEx
            (
                exStyle, wxT("STATIC"), NULL,
                msStyle,
                0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)NewControlId(),
                wxGetInstance(), NULL
            );

        // Now create min static control
        wxString minLabel;
        minLabel.Printf(wxT("%d"), minValue);
        long wstyle = STATIC_FLAGS;
        if ( m_windowStyle & wxCLIP_SIBLINGS )
            wstyle |= WS_CLIPSIBLINGS;
        m_staticMin = (WXHWND) CreateWindowEx
            (
                0, wxT("STATIC"), minLabel,
                wstyle,
                0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)NewControlId(),
                wxGetInstance(), NULL
            );
    }

    WXDWORD exStyle = 0;

    msStyle = MSWGetStyle(GetWindowStyle(), & exStyle) ;

    wxUnusedVar(msStyle);

    if (m_windowStyle & wxSL_VERTICAL)
        msStyle = TBS_VERT | WS_CHILD | WS_VISIBLE | WS_TABSTOP ;
    else
        msStyle = TBS_HORZ | WS_CHILD | WS_VISIBLE | WS_TABSTOP ;

    if ( m_windowStyle & wxSL_AUTOTICKS )
        msStyle |= TBS_AUTOTICKS ;

    if ( m_windowStyle & wxSL_LEFT )
        msStyle |= TBS_LEFT;
    else if ( m_windowStyle & wxSL_RIGHT )
        msStyle |= TBS_RIGHT;
    else if ( m_windowStyle & wxSL_TOP )
        msStyle |= TBS_TOP;
    else if ( m_windowStyle & wxSL_BOTTOM )
        msStyle |= TBS_BOTTOM;
    else if ( m_windowStyle & wxSL_BOTH )
        msStyle |= TBS_BOTH;
    else if ( ! (m_windowStyle & wxSL_AUTOTICKS) )
        msStyle |= TBS_NOTICKS;

    if ( m_windowStyle & wxSL_SELRANGE )
        msStyle |= TBS_ENABLESELRANGE;

    HWND scroll_bar = CreateWindowEx
        (
            exStyle, TRACKBAR_CLASS, wxEmptyString,
            msStyle,
            0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)m_windowId,
            wxGetInstance(), NULL
        );

    m_rangeMax = maxValue;
    m_rangeMin = minValue;

    m_pageSize = (int)((maxValue-minValue)/10);

    ::SendMessage(scroll_bar, TBM_SETRANGE, TRUE, MAKELONG(minValue, maxValue));
    ::SendMessage(scroll_bar, TBM_SETPOS, TRUE, (LPARAM)value);
    ::SendMessage(scroll_bar, TBM_SETPAGESIZE, 0, (LPARAM)m_pageSize);

    m_hWnd = (WXHWND)scroll_bar;

    SubclassWin(GetHWND());

    ::SetWindowText((HWND) m_hWnd, wxEmptyString);

    if ( m_windowStyle & wxSL_LABELS )
    {
        // Finally, create max value static item
        wxString maxLabel;
        maxLabel.Printf(wxT("%d"), maxValue);
        long wstyle = STATIC_FLAGS;

        if ( m_windowStyle & wxCLIP_SIBLINGS )
            wstyle |= WS_CLIPSIBLINGS;

        m_staticMax = (WXHWND) CreateWindowEx
            (
                0, wxT("STATIC"), maxLabel,
                wstyle,
                0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)NewControlId(),
                wxGetInstance(), NULL
            );


        if (GetFont().Ok())
        {
            if (GetFont().GetResourceHandle())
            {
                if ( m_staticMin )
                    ::SendMessage((HWND) m_staticMin, WM_SETFONT,
                          (WPARAM) GetFont().GetResourceHandle(), 0L);

                if ( m_staticMax )
                    ::SendMessage((HWND) m_staticMax, WM_SETFONT,
                        (WPARAM) GetFont().GetResourceHandle(), 0L);

                if (m_staticValue)
                    ::SendMessage((HWND) m_staticValue, WM_SETFONT,
                        (WPARAM) GetFont().GetResourceHandle(), 0L);
            }
        }
    }

    SetSize(pos.x, pos.y, size.x, size.y);
    SetValue(value);

    // SetInitialBestSize is not called since we don't call MSWCreateControl
    // for this control, so call SetBestSize here instead.
    SetBestSize(size);

    return true;
}

bool wxSlider95::MSWOnScroll(int WXUNUSED(orientation), WXWORD wParam,
                             WXWORD WXUNUSED(pos), WXHWND control)
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

wxSlider95::~wxSlider95()
{
    if (m_staticMin)
    {
        ::DestroyWindow((HWND) m_staticMin);
        m_staticMin = (WXHWND) NULL;
    }

    if (m_staticMax)
    {
        ::DestroyWindow((HWND) m_staticMax);
        m_staticMax = (WXHWND) NULL;
    }

    if (m_staticValue)
    {
        ::DestroyWindow((HWND) m_staticValue);
        m_staticValue = (WXHWND) NULL;
    }
}

int wxSlider95::GetValue() const
{
    return ::SendMessage(GetHwnd(), TBM_GETPOS, 0, 0);
}

void wxSlider95::SetValue(int value)
{
    ::SendMessage(GetHwnd(), TBM_SETPOS, (WPARAM)TRUE, (LPARAM)value);

    if (m_staticValue)
    {
        wxString str;
        str.Printf(wxT("%d"), value);
        ::SetWindowText((HWND) m_staticValue, str);
    }
}

void wxSlider95::DoGetSize(int *width, int *height) const
{
    RECT rect;
    rect.left = -1; rect.right = -1; rect.top = -1; rect.bottom = -1;

    wxFindMaxSize(GetHWND(), &rect);

    if (m_staticMin)
        wxFindMaxSize(m_staticMin, &rect);

    if (m_staticMax)
        wxFindMaxSize(m_staticMax, &rect);

    if (m_staticValue)
        wxFindMaxSize(m_staticValue, &rect);

    if ( width )
        *width = rect.right - rect.left;
    if ( height )
        *height = rect.bottom - rect.top;
}

void wxSlider95::GetPosition(int *x, int *y) const
{
    wxWindow *parent = GetParent();
    RECT rect;
    rect.left = -1; rect.right = -1; rect.top = -1; rect.bottom = -1;

    wxFindMaxSize(GetHWND(), &rect);

    if (m_staticMin)
        wxFindMaxSize(m_staticMin, &rect);
    if (m_staticMax)
        wxFindMaxSize(m_staticMax, &rect);
    if (m_staticValue)
        wxFindMaxSize(m_staticValue, &rect);

    // Since we now have the absolute screen coords,
    // if there's a parent we must subtract its top left corner
    POINT point;
    point.x = rect.left;
    point.y = rect.top;
    if (parent)
        ::ScreenToClient((HWND) parent->GetHWND(), &point);

    // We may be faking the client origin.
    // So a window that's really at (0, 30) may appear
    // (to wxWin apps) to be at (0, 0).
    if (GetParent())
    {
        wxPoint pt(GetParent()->GetClientAreaOrigin());
        point.x -= pt.x;
        point.y -= pt.y;
    }

    *x = point.x;
    *y = point.y;
}

// TODO one day, make sense of all this horros and replace it with a readable
//      DoGetBestSize()
void wxSlider95::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    int x1 = x;
    int y1 = y;
    int w1 = width;
    int h1 = height;

    int currentX, currentY;
    GetPosition(&currentX, &currentY);
    if (x == wxDefaultCoord && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
       x1 = currentX;
    if (y == wxDefaultCoord && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
        y1 = currentY;

    AdjustForParentClientOrigin(x1, y1, sizeFlags);

    wxChar buf[300];

    int x_offset = x;
    int y_offset = y;

    int cx;     // slider,min,max sizes
    int cy;
    int cyf;

    wxGetCharSize(GetHWND(), &cx, &cy, this->GetFont());

    if ((m_windowStyle & wxSL_VERTICAL) != wxSL_VERTICAL)
    {
        if ( m_windowStyle & wxSL_LABELS )
        {
            int min_len = 0;

            ::GetWindowText((HWND) m_staticMin, buf, 300);
            GetTextExtent(buf, &min_len, &cyf);

            int max_len = 0;

            ::GetWindowText((HWND) m_staticMax, buf, 300);
            GetTextExtent(buf, &max_len, &cyf);
            if (m_staticValue)
            {
                int new_width = (int)(wxMax(min_len, max_len));
                int valueHeight = (int)cyf;
#ifdef __WIN32__
                // For some reason, under Win95, the text edit control has
                // a lot of space before the first character
                new_width += 3*cx;
#endif
                // The height needs to be a bit bigger under Win95 if
                // using native 3D effects.
                valueHeight = (int) (valueHeight * 1.5) ;
                ::MoveWindow((HWND) m_staticValue, x_offset, y_offset,
                    new_width, valueHeight, TRUE);
                x_offset += new_width + cx;
            }

            ::MoveWindow((HWND) m_staticMin, x_offset, y_offset,
                (int) min_len, cy, TRUE);
            x_offset += (int)(min_len + cx);

            // slider_length = (total width available) - (width used so far)
            //                   - (width of max label) - (border)
            int slider_length = (int)(w1 - (x_offset-x) - max_len - cx);

            int slider_height = h1;
            if (slider_height < 0 )
                slider_height = 20;

            // Slider must have a minimum/default length/height
            if (slider_length < 100)
                slider_length = 100;

            ::MoveWindow(GetHwnd(), x_offset, y_offset,
                slider_length, slider_height, TRUE);
            x_offset += slider_length + cx;

            ::MoveWindow((HWND) m_staticMax, x_offset, y_offset,
                (int) max_len, cy, TRUE);
        }
        else
        {
            // No labels
            // If we're prepared to use the existing size, then...
            if
            (
                width == wxDefaultCoord
                && height == wxDefaultCoord
                && ((sizeFlags & wxSIZE_AUTO) != wxSIZE_AUTO)
            )
            {
                GetSize(&w1, &h1);
            }

            if ( w1 < 0 )
                w1 = 100;
            if ( h1 < 0 )
                h1 = 20;

            ::MoveWindow(GetHwnd(), x1, y1, w1, h1, TRUE);
        }
    }
    else
    {
        if ( m_windowStyle & wxSL_LABELS )
        {
            int min_len;
            ::GetWindowText((HWND) m_staticMin, buf, 300);
            GetTextExtent(buf, &min_len, &cyf);

            int max_len;
            ::GetWindowText((HWND) m_staticMax, buf, 300);
            GetTextExtent(buf, &max_len, &cyf);

            if (m_staticValue)
            {
                int new_width = (int)(wxMax(min_len, max_len));
                int valueHeight = (int)cyf;
                new_width += cx;

                // The height needs to be a bit bigger under Win95 if
                // using native 3D effects.
                valueHeight = (int) (valueHeight * 1.5) ;

                ::MoveWindow((HWND) m_staticValue, x_offset, y_offset,
                    new_width, valueHeight, TRUE);
                y_offset += valueHeight;
            }

            ::MoveWindow((HWND) m_staticMin, x_offset, y_offset,
                (int) min_len, cy, TRUE);
            y_offset += cy;

            //  slider_length = (total height available) - (height used so far)
            //                              - (height of max label) - (border)
            int slider_length = (int)(h1 - (y_offset-y) - cy - cy);

            int slider_width = w1;
            if (slider_width < 0 )
                slider_width = 20;

            // Slider must have a minimum/default length
            if (slider_length < 100)
                slider_length = 100;

            ::MoveWindow(GetHwnd(), x_offset, y_offset,
                slider_width, slider_length, TRUE);
            y_offset += slider_length;

            ::MoveWindow((HWND) m_staticMax, x_offset, y_offset,
                (int)max_len, cy, TRUE);
        }
        else
        {
            // No labels
            // If we're prepared to use the existing size, then...
            if
            (
                width == wxDefaultCoord && height == wxDefaultCoord
                && ((sizeFlags & wxSIZE_AUTO) != wxSIZE_AUTO)
            )
            {
                GetSize(&w1, &h1);
            }

            if ( w1 < 0 )
                w1 = 20;
            if ( h1 < 0 )
                h1 = 100;

            ::MoveWindow(GetHwnd(), x1, y1, w1, h1, TRUE);
        }
    }
}


// A reimplementaion of the mess above changed a bit to just determine the min
// size needed.  It would certainly be nice to refactor this and DoSetSize
// somehow.
wxSize wxSlider95::DoGetBestSize() const
{
    wxSize rv;
    wxChar buf[300];
    int cx;
    int cy;
    int cyf;
    int min_len = 0;
    int max_len = 0;

    wxGetCharSize(GetHWND(), &cx, &cy, this->GetFont());

    if ( !HasFlag(wxSL_VERTICAL))
    {
        rv = wxSize(100, 20);  // default size for the slider itself

        if (HasFlag(wxSL_LABELS))  // do we need to add more for the labels?
        {
            ::GetWindowText((HWND) m_staticMin, buf, 300);
            GetTextExtent(buf, &min_len, &cyf);
            rv.x += min_len + cx;

            ::GetWindowText((HWND) m_staticMax, buf, 300);
            GetTextExtent(buf, &max_len, &cyf);
            rv.x += max_len + cx;

            if (m_staticValue)
            {
                int new_width = (int)(wxMax(min_len, max_len));
                int valueHeight = (int)cyf;

#ifdef __WIN32__
                // For some reason, under Win95, the text edit control has
                // a lot of space before the first character
                new_width += 3*cx;
#endif
                // The height needs to be a bit bigger under Win95 if
                // using native 3D effects.
                valueHeight = (int) (valueHeight * 1.5) ;

                rv.x += new_width + cx;
                rv.y = wxMax(valueHeight, rv.y);
            }
        }
    }
    else // ! wxSL_HORIZONTAL
    {
        rv = wxSize(20, 100);  // default size for the slider itself

        if (HasFlag(wxSL_LABELS)) // do we need to add more for the labels?
        {
            ::GetWindowText((HWND) m_staticMin, buf, 300);
            GetTextExtent(buf, &min_len, &cyf);
            rv.y += cy;

            ::GetWindowText((HWND) m_staticMax, buf, 300);
            GetTextExtent(buf, &max_len, &cyf);
            rv.y += cy;

            if (m_staticValue)
            {
                int new_width = (int)(wxMax(min_len, max_len));
                int valueHeight = (int)cyf;
                new_width += cx;

                // The height needs to be a bit bigger under Win95 if
                // using native 3D effects.
                valueHeight = (int) (valueHeight * 1.5) ;
                rv.y += valueHeight;
                rv.x = wxMax(new_width, rv.x);
            }
        }
    }
    return rv;
}


void wxSlider95::SetRange(int minValue, int maxValue)
{
    m_rangeMin = minValue;
    m_rangeMax = maxValue;

    ::SendMessage(GetHwnd(), TBM_SETRANGE, TRUE, MAKELONG(minValue, maxValue));

    wxChar buf[40];
    if ( m_staticMin )
    {
        wxSprintf(buf, wxT("%d"), m_rangeMin);
        ::SetWindowText((HWND) m_staticMin, buf);
    }

    if ( m_staticMax )
    {
        wxSprintf(buf, wxT("%d"), m_rangeMax);
        ::SetWindowText((HWND) m_staticMax, buf);
    }
}

// For trackbars only
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
  ::SendMessage( GetHwnd(), TBM_CLEARSEL, (WPARAM) TRUE, (LPARAM) 0 );
}

void wxSlider95::ClearTicks()
{
  ::SendMessage( GetHwnd(), TBM_CLEARTICS, (WPARAM) TRUE, (LPARAM) 0 );
}

void wxSlider95::SetLineSize(int lineSize)
{
    m_lineSize = lineSize;
    ::SendMessage( GetHwnd(), TBM_SETLINESIZE, (WPARAM) 0, (LPARAM) lineSize );
}

int wxSlider95::GetLineSize() const
{
    return (int) ::SendMessage( GetHwnd(), TBM_GETLINESIZE,
        (WPARAM) 0, (LPARAM) 0 );
}

int wxSlider95::GetSelEnd() const
{
    return (int) ::SendMessage( GetHwnd(), TBM_SETSELEND,
        (WPARAM) 0, (LPARAM) 0 );
}

int wxSlider95::GetSelStart() const
{
    return (int) ::SendMessage( GetHwnd(), TBM_GETSELSTART,
        (WPARAM) 0, (LPARAM) 0 );
}

void wxSlider95::SetSelection(int minPos, int maxPos)
{
    ::SendMessage(GetHwnd(), TBM_SETSEL,
        (WPARAM) TRUE, (LPARAM) MAKELONG( minPos, maxPos) );
}

void wxSlider95::SetThumbLength(int len)
{
    ::SendMessage( GetHwnd(), TBM_SETTHUMBLENGTH, (WPARAM) len, (LPARAM) 0 );
}

int wxSlider95::GetThumbLength() const
{
  return (int) ::SendMessage( GetHwnd(), TBM_GETTHUMBLENGTH,
      (WPARAM) 0, (LPARAM) 0 );
}

void wxSlider95::SetTick(int tickPos)
{
  ::SendMessage( GetHwnd(), TBM_SETTIC, (WPARAM) 0, (LPARAM) tickPos );
}

bool wxSlider95::ContainsHWND(WXHWND hWnd) const
{
    return
    (
        hWnd == GetStaticMin()
        || hWnd == GetStaticMax()
        || hWnd == GetEditValue()
    );
}

void wxSlider95::Command (wxCommandEvent & event)
{
    SetValue (event.GetInt());
    ProcessCommand (event);
}

bool wxSlider95::Show(bool show)
{
    wxWindow::Show(show);

    int cshow = show ? SW_SHOW : SW_HIDE;

    if(m_staticValue)
        ShowWindow((HWND) m_staticValue, cshow);

    if(m_staticMin)
        ShowWindow((HWND) m_staticMin, cshow);

    if(m_staticMax)
        ShowWindow((HWND) m_staticMax, cshow);

    return true;
}

#endif
  // __WIN95__

#endif // wxUSE_SLIDER
