/////////////////////////////////////////////////////////////////////////////
// Name:        univ/scrolbar.cpp
// Purpose:     wxScrollBar implementation
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "univscrolbar.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_SCROLLBAR

#ifndef WX_PRECOMP
    #include "wx/timer.h"

    #include "wx/dcclient.h"
    #include "wx/scrolbar.h"
    #include "wx/validate.h"
#endif

#include "wx/univ/renderer.h"
#include "wx/univ/inphand.h"
#include "wx/univ/theme.h"

// ----------------------------------------------------------------------------
// wxScrollBarTimer: this class is used to repeatedly scroll the scrollbar
// when the mouse is help pressed on the arrow or on the bar. It generates the
// given scroll action command periodically.
// ----------------------------------------------------------------------------

class wxScrollBarTimer : public wxTimer
{
public:
    wxScrollBarTimer(wxStdScrollBarInputHandler *handler,
                     const wxControlAction& action,
                     wxScrollBar *control);

    virtual void Notify();

private:
    wxStdScrollBarInputHandler *m_handler;
    wxControlAction m_action;
    wxScrollBar    *m_control;
};

// ============================================================================
// implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxScrollBar, wxControl)

BEGIN_EVENT_TABLE(wxScrollBar, wxScrollBarBase)
    EVT_IDLE(wxScrollBar::OnIdle)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

void wxScrollBar::Init()
{
    m_range =
    m_thumbSize =
    m_thumbPos =
    m_pageSize = 0;

    m_thumbPosOld = -1;

    for ( size_t n = 0; n < WXSIZEOF(m_elementsState); n++ )
    {
        m_elementsState[n] = 0;
    }

    m_dirty = FALSE;
}

bool wxScrollBar::Create(wxWindow *parent,
                         wxWindowID id,
                         const wxPoint &pos,
                         const wxSize &size,
                         long style,
                         const wxValidator& validator,
                         const wxString &name)
{
    // the scrollbars always have the standard border so far
    style &= ~wxBORDER_MASK;
    style |= wxBORDER_SUNKEN;

    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

    SetBestSize(size);

    return TRUE;
}

wxScrollBar::~wxScrollBar()
{
}

// ----------------------------------------------------------------------------
// scrollbar API
// ----------------------------------------------------------------------------

void wxScrollBar::DoSetThumb(int pos)
{
    // don't do checks here, we're a private function
    if ( pos < 0 )
    {
        pos = 0;
    }
    else if ( pos > m_range - m_thumbSize )
    {
        pos = m_range - m_thumbSize;
    }

    if ( m_thumbPosOld == -1 )
    {
        // remember the old thumb position
        m_thumbPosOld = m_thumbPos;
    }

    m_thumbPos = pos;

    // we have to refresh the part of the bar which was under the thumb and the
    // thumb itself
    m_elementsState[Element_Thumb] |= wxCONTROL_DIRTY;
    m_elementsState[m_thumbPos > m_thumbPosOld
                        ? Element_Bar_1 : Element_Bar_2] |= wxCONTROL_DIRTY;
    m_dirty = TRUE;
}

int wxScrollBar::GetThumbPosition() const
{
    return m_thumbPos;
}

int wxScrollBar::GetThumbSize() const
{
    return m_thumbSize;
}

int wxScrollBar::GetPageSize() const
{
    return m_pageSize;
}

int wxScrollBar::GetRange() const
{
    return m_range;
}

void wxScrollBar::SetThumbPosition(int pos)
{
    wxCHECK_RET( pos >= 0 && pos <= m_range, _T("thumb position out of range") );

    DoSetThumb(pos);
}

void wxScrollBar::SetScrollbar(int position, int thumbSize,
                               int range, int pageSize,
                               bool refresh)
{
    // set all parameters
    m_range = range;
    m_thumbSize = thumbSize;
    SetThumbPosition(position);
    m_pageSize = pageSize;

    if ( refresh )
    {
        // and update the window
        Refresh();
    }
}

// ----------------------------------------------------------------------------
// size management
// ----------------------------------------------------------------------------

wxSize wxScrollBar::DoGetBestClientSize() const
{
    // completely arbitrary
    return wxSize(140, 140);
}

// ----------------------------------------------------------------------------
// drawing
// ----------------------------------------------------------------------------

void wxScrollBar::OnIdle(wxIdleEvent& event)
{
    if ( m_dirty )
    {
        for ( size_t n = 0; n < WXSIZEOF(m_elementsState); n++ )
        {
            if ( m_elementsState[n] & wxCONTROL_DIRTY )
            {
                wxRect rect = GetRenderer()->GetScrollbarRect(this, (Element)n);
 
                if ( rect.width && rect.height )
                {
                    // don't refresh the background when refreshing the shaft
                    Refresh(TRUE, //(n != Element_Bar_1) && (n != Element_Bar_2)
                            &rect);
                }

                m_elementsState[n] &= ~wxCONTROL_DIRTY;
            }
        }

        m_dirty = FALSE;
    }
}

void wxScrollBar::DoDraw(wxControlRenderer *renderer)
{
    renderer->DrawScrollbar(this, m_thumbPosOld);

    // clear all dirty flags
    m_dirty = FALSE;
    m_thumbPosOld = -1;
}

// ----------------------------------------------------------------------------
// input processing
// ----------------------------------------------------------------------------

void wxScrollBar::SetState(Element which, int flags)
{
    if ( (m_elementsState[which] & ~wxCONTROL_DIRTY) != flags )
    {
        m_elementsState[which] = flags | wxCONTROL_DIRTY;

        m_dirty = TRUE;
    }
}

bool wxScrollBar::PerformAction(const wxControlAction& action,
                                long numArg,
                                const wxString& strArg)
{
    int thumbOld = m_thumbPos;

    bool notify = FALSE; // send an event about the change?

    wxEventType scrollType;

    // test for thumb move first as these events happen in quick succession
    if ( action == wxACTION_SCROLL_THUMB_MOVE )
    {
        DoSetThumb(numArg);

        scrollType = wxEVT_SCROLLWIN_THUMBTRACK;
    }
    else if ( action == wxACTION_SCROLL_LINE_UP )
    {
        scrollType = wxEVT_SCROLLWIN_LINEUP;
        ScrollLines(-1);
    }
    else if ( action == wxACTION_SCROLL_LINE_DOWN )
    {
        scrollType = wxEVT_SCROLLWIN_LINEDOWN;
        ScrollLines(1);
    }
    else if ( action == wxACTION_SCROLL_PAGE_UP )
    {
        scrollType = wxEVT_SCROLLWIN_PAGEUP;
        ScrollPages(-1);
    }
    else if ( action == wxACTION_SCROLL_PAGE_DOWN )
    {
        scrollType = wxEVT_SCROLLWIN_PAGEDOWN;
        ScrollPages(1);
    }
    else if ( action == wxACTION_SCROLL_START )
    {
        scrollType = wxEVT_SCROLLWIN_THUMBRELEASE; // anything better?
        ScrollToStart();
    }
    else if ( action == wxACTION_SCROLL_END )
    {
        scrollType = wxEVT_SCROLLWIN_THUMBRELEASE; // anything better?
        ScrollToEnd();
    }
    else if ( action == wxACTION_SCROLL_THUMB_DRAG )
    {
        // we won't use it but this line suppresses the compiler
        // warning about "variable may be used without having been
        // initialized"
        scrollType = wxEVT_NULL;
    }
    else if ( action == wxACTION_SCROLL_THUMB_RELEASE )
    {
        // always notify about this
        notify = TRUE;
        scrollType = wxEVT_SCROLLWIN_THUMBRELEASE;
    }
    else
        return wxControl::PerformAction(action, numArg, strArg);

    // has scrollbar position changed?
    bool changed = m_thumbPos != thumbOld;
    if ( notify || changed )
    {
        wxScrollWinEvent event(scrollType, m_thumbPos,
                               IsVertical() ? wxVERTICAL : wxHORIZONTAL);
        event.SetEventObject(this);
        GetParent()->GetEventHandler()->ProcessEvent(event);
    }

    return TRUE;
}

void wxScrollBar::ScrollToStart()
{
    DoSetThumb(0);
}

void wxScrollBar::ScrollToEnd()
{
    DoSetThumb(m_range - m_thumbSize);
}

void wxScrollBar::ScrollLines(int nLines)
{
    DoSetThumb(m_thumbPos + nLines);
}

void wxScrollBar::ScrollPages(int nPages)
{
    DoSetThumb(m_thumbPos + nPages*m_pageSize);
}

// ============================================================================
// scroll bar input handler
// ============================================================================

// ----------------------------------------------------------------------------
// wxScrollBarTimer
// ----------------------------------------------------------------------------

wxScrollBarTimer::wxScrollBarTimer(wxStdScrollBarInputHandler *handler,
                                   const wxControlAction& action,
                                   wxScrollBar *control)
{
    m_handler = handler;
    m_action = action;
    m_control = control;
}

void wxScrollBarTimer::Notify()
{
    m_handler->OnScrollTimer(m_control, m_action);
}

// ----------------------------------------------------------------------------
// wxStdScrollBarInputHandler
// ----------------------------------------------------------------------------

wxStdScrollBarInputHandler::wxStdScrollBarInputHandler(wxRenderer *renderer,
                                                       wxInputHandler *handler)
                          : wxStdInputHandler(handler)
{
    m_renderer = renderer;
    m_winCapture = NULL;
    m_htLast = wxHT_NOWHERE;
    m_timerScroll = NULL;
}

wxStdScrollBarInputHandler::~wxStdScrollBarInputHandler()
{
    // normally, it's NULL by now but just in case the user somehow managed to
    // keep the mouse captured until now...
    delete m_timerScroll;
}

void wxStdScrollBarInputHandler::SetElementState(wxScrollBar *control,
                                                 int flag,
                                                 bool doIt)
{
    if ( m_htLast > wxHT_SCROLLBAR_FIRST && m_htLast < wxHT_SCROLLBAR_LAST )
    {
        wxScrollBar::Element
            elem = (wxScrollBar::Element)(m_htLast - wxHT_SCROLLBAR_FIRST - 1);

        int flags = control->GetState(elem);
        if ( doIt )
            flags |= flag;
        else
            flags &= ~flag;
        control->SetState(elem, flags);
    }
}

bool wxStdScrollBarInputHandler::OnScrollTimer(wxScrollBar *scrollbar,
                                               const wxControlAction& action)
{
    int oldThumbPos = scrollbar->GetThumbPosition();
    scrollbar->PerformAction(action);
    if ( scrollbar->GetThumbPosition() != oldThumbPos )
        return TRUE;

    // we scrolled till the end
    m_timerScroll->Stop();

    return FALSE;
}

void wxStdScrollBarInputHandler::StopScrolling(wxScrollBar *control)
{
    // return everything to the normal state
    if ( m_winCapture )
    {
        m_winCapture->ReleaseMouse();
        m_winCapture = NULL;
    }

    m_btnCapture = -1;

    if ( m_timerScroll )
    {
        delete m_timerScroll;
        m_timerScroll = NULL;
    }

    // unpress the arrow and highlight the current element
    Press(control, FALSE);
}

wxCoord
wxStdScrollBarInputHandler::GetMouseCoord(const wxScrollBar *scrollbar,
                                          const wxMouseEvent& event) const
{
    wxPoint pt = event.GetPosition();
    return scrollbar->GetWindowStyle() & wxVERTICAL ? pt.y : pt.x;
}

void wxStdScrollBarInputHandler::HandleThumbMove(wxScrollBar *scrollbar,
                                                 const wxMouseEvent& event)
{
    int thumbPos = GetMouseCoord(scrollbar, event) - m_ofsMouse;
    thumbPos = m_renderer->PixelToScrollbar(scrollbar, thumbPos);
    scrollbar->PerformAction(wxACTION_SCROLL_THUMB_MOVE, thumbPos);
}

bool wxStdScrollBarInputHandler::HandleKey(wxControl *control,
                                           const wxKeyEvent& event,
                                           bool pressed)
{
    // we only react to the key presses here
    if ( pressed )
    {
        wxControlAction action;
        switch ( event.GetKeyCode() )
        {
            case WXK_DOWN:
            case WXK_RIGHT:     action = wxACTION_SCROLL_LINE_DOWN; break;
            case WXK_UP:
            case WXK_LEFT:      action = wxACTION_SCROLL_LINE_UP;   break;
            case WXK_HOME:      action = wxACTION_SCROLL_START;     break;
            case WXK_END:       action = wxACTION_SCROLL_END;       break;
            case WXK_PRIOR:     action = wxACTION_SCROLL_PAGE_UP;   break;
            case WXK_NEXT:      action = wxACTION_SCROLL_PAGE_DOWN; break;
        }

        if ( !!action )
        {
            control->PerformAction(action);

            return TRUE;
        }
    }

    return wxStdInputHandler::HandleKey(control, event, pressed);
}

bool wxStdScrollBarInputHandler::HandleMouse(wxControl *control,
                                             const wxMouseEvent& event)
{
    // is this a click event from an acceptable button?
    int btn = -1;
    if ( event.IsButton() )
    {
        for ( int i = 1; i <= 3; i++ )
        {
            if ( event.Button(i) )
            {
                btn = i;
                break;
            }
        }

        wxASSERT_MSG( btn != -1, _T("unknown mouse button") );
    }

    if ( (btn != -1) && IsAllowedButton(btn) )
    {
        // determine which part of the window mouse is in
        wxScrollBar *scrollbar = wxStaticCast(control, wxScrollBar);
        wxHitTest ht = m_renderer->HitTestScrollbar
                                   (
                                    scrollbar,
                                    event.GetPosition()
                                   );

        // when the mouse is pressed on any scrollbar element, we capture it
        // and hold capture until the same mouse button is released
        if ( event.ButtonDown() || event.ButtonDClick() )
        {
            if ( !m_winCapture )
            {
                m_btnCapture = btn;
                m_winCapture = control;
                m_winCapture->CaptureMouse();

                // generate the command
                bool hasAction = TRUE;
                wxControlAction action;
                switch ( ht )
                {
                    case wxHT_SCROLLBAR_ARROW_LINE_1:
                        action = wxACTION_SCROLL_LINE_UP;
                        break;

                    case wxHT_SCROLLBAR_ARROW_LINE_2:
                        action = wxACTION_SCROLL_LINE_DOWN;
                        break;

                    case wxHT_SCROLLBAR_BAR_1:
                        action = wxACTION_SCROLL_PAGE_UP;
                        m_ptStartScrolling = event.GetPosition();
                        break;

                    case wxHT_SCROLLBAR_BAR_2:
                        action = wxACTION_SCROLL_PAGE_DOWN;
                        m_ptStartScrolling = event.GetPosition();
                        break;

                    case wxHT_SCROLLBAR_THUMB:
                        control->PerformAction(wxACTION_SCROLL_THUMB_DRAG);
                        m_ofsMouse = GetMouseCoord(scrollbar, event) -
                                     m_renderer->ScrollbarToPixel(scrollbar);

                        // fall through: there is no immediate action

                    default:
                        hasAction = FALSE;
                }

                // remove highlighting
                Highlight(scrollbar, FALSE);
                m_htLast = ht;

                // and press the arrow or highlight thumb now instead
                if ( m_htLast == wxHT_SCROLLBAR_THUMB )
                    Highlight(scrollbar, TRUE);
                else
                    Press(scrollbar, TRUE);

                // start dragging
                if ( hasAction )
                {
                    m_timerScroll = new wxScrollBarTimer(this,
                                                         action,
                                                         scrollbar);
                    // start scrolling immediately
                    m_timerScroll->Notify();

                    // and continue it later
                    m_timerScroll->Start(50); // FIXME hardcoded delay
                }
                //else: no (immediate) action

            }
            //else: mouse already captured, nothing to do
        }
        // release mouse if the *same* button went up
        else if ( btn == m_btnCapture )
        {
            if ( m_winCapture )
            {
                StopScrolling(scrollbar);

                // if we were dragging the thumb, send the last event
                if ( m_htLast == wxHT_SCROLLBAR_THUMB )
                {
                    scrollbar->PerformAction(wxACTION_SCROLL_THUMB_RELEASE);
                }

                m_htLast = ht;
                Highlight(scrollbar, TRUE);
            }
            else
            {
                // this is not supposed to happen as the button can't go up
                // without going down previously and then we'd have
                // m_winCapture by now
                wxFAIL_MSG( _T("logic error in mouse capturing code") );
            }
        }
    }

    return wxStdInputHandler::HandleMouse(control, event);
}

bool wxStdScrollBarInputHandler::HandleMouseMove(wxControl *control,
                                                 const wxMouseEvent& event)
{
    wxScrollBar *scrollbar = wxStaticCast(control, wxScrollBar);

    if ( m_winCapture )
    {
        if ( (m_htLast == wxHT_SCROLLBAR_THUMB) && event.Moving() )
        {
            // make the thumb follow the mouse by keeping the same offset
            // between the mouse position and the top/left of the thumb
            HandleThumbMove(scrollbar, event);

            return TRUE;
        }

        // no other changes are possible while the mouse is captured
        return FALSE;
    }

    if ( event.Moving() )
    {
        wxHitTest ht = m_renderer->HitTestScrollbar
                                   (
                                    scrollbar,
                                    event.GetPosition()
                                   );
        if ( ht == m_htLast )
        {
            // nothing changed
            return FALSE;
        }

#ifdef DEBUG_MOUSE
        wxLogDebug("Scrollbar::OnMouseMove: ht = %d", ht);
#endif // DEBUG_MOUSE

        Highlight(scrollbar, FALSE);
        m_htLast = ht;
        Highlight(scrollbar, TRUE);
    }
    else if ( event.Leaving() )
    {
        Highlight(scrollbar, FALSE);
        m_htLast = wxHT_NOWHERE;
    }
    else
    {
        // we don't process this event
        return FALSE;
    }

    // we did something
    return TRUE;
}

#endif // wxUSE_SCROLLBAR

