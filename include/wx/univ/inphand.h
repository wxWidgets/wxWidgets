///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/inphand.h
// Purpose:     wxInputHandler class maps the keyboard and mouse events to the
//              actions which then are performed by the control
// Author:      Vadim Zeitlin
// Modified by:
// Created:     18.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_INPHAND_H_
#define _WX_UNIV_INPHAND_H_

#ifdef __GNUG__
    #pragma interface "inphand.h"
#endif

#include "wx/control.h"         // for wxControlAction(s)

#include "wx/univ/renderer.h"   // for wxHitTest

class WXDLLEXPORT wxListBox;
class WXDLLEXPORT wxRenderer;
class WXDLLEXPORT wxScrollBar;

// ----------------------------------------------------------------------------
// wxInputHandler: maps the events to the actions
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxInputHandler
{
public:
    // map a keyboard event to one or more actions (pressed == TRUE if the key
    // was pressed, FALSE if released), returns TRUE if something was done
    virtual bool HandleKey(wxControl *control,
                           const wxKeyEvent& event,
                           bool pressed) = 0;

    // map a mouse (click) event to one or more actions
    virtual bool HandleMouse(wxControl *control,
                             const wxMouseEvent& event) = 0;

    // handle mouse movement (or enter/leave) event: it is separated from
    // HandleMouse() for convenience as many controls don't care about mouse
    // movements at all
    virtual bool HandleMouseMove(wxControl *control,
                                 const wxMouseEvent& event);

    // do something with focus set/kill event: this is different from
    // HandleMouseMove() as the mouse maybe over the control without it having
    // focus
    //
    // return TRUE to refresh the control, FALSE otherwise
    virtual bool HandleFocus(wxControl *control, const wxFocusEvent& event);

    // virtual dtor for any base class
    virtual ~wxInputHandler();
};

// ============================================================================
// The classes below provide the standard input handling for some controls so
// that the code can be reused in different themes. All of these classes chain
// to an existing input handler (which must be given to the ctor) and so allow
// custom processing of the events which they don't handle themselves.
// ============================================================================

// ----------------------------------------------------------------------------
// wxStdInputHandler is just a base class for all other "standard" handlers
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxStdInputHandler : public wxInputHandler
{
public:
    wxStdInputHandler(wxInputHandler *handler) : m_handler(handler) { }

    virtual bool HandleKey(wxControl *control,
                           const wxKeyEvent& event,
                           bool pressed)
        { return m_handler->HandleKey(control, event, pressed); }
    virtual bool HandleMouse(wxControl *control,
                             const wxMouseEvent& event)
        { return m_handler->HandleMouse(control, event); }
    virtual bool HandleMouseMove(wxControl *control, const wxMouseEvent& event)
        { return m_handler->HandleMouseMove(control, event); }
    virtual bool HandleFocus(wxControl *control, const wxFocusEvent& event)
        { return m_handler->HandleFocus(control, event); }

private:
    wxInputHandler *m_handler;
};

// ----------------------------------------------------------------------------
// wxStdButtonInputHandler: translates SPACE and ENTER keys and the left mouse
// click into button press/release actions
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxStdButtonInputHandler : public wxStdInputHandler
{
public:
    wxStdButtonInputHandler(wxInputHandler *inphand);

    virtual bool HandleKey(wxControl *control,
                           const wxKeyEvent& event,
                           bool pressed);
    virtual bool HandleMouse(wxControl *control,
                             const wxMouseEvent& event);
    virtual bool HandleMouseMove(wxControl *control, const wxMouseEvent& event);
    virtual bool HandleFocus(wxControl *control, const wxFocusEvent& event);

private:
    // the window (button) which has capture or NULL and the flag telling if
    // the mouse is inside the button which captured it or not
    wxWindow *m_winCapture;
    bool      m_winHasMouse;
};

// ----------------------------------------------------------------------------
// common scrollbar input handler: it manages clicks on the standard scrollbar
// elements (line arrows, bar, thumb)
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxStdScrollBarInputHandler : public wxStdInputHandler
{
public:
    // constructor takes a renderer (used for scrollbar hit testing) and the
    // base handler to which all unhandled events are forwarded
    wxStdScrollBarInputHandler(wxRenderer *renderer,
                               wxInputHandler *inphand);

    virtual bool HandleKey(wxControl *control,
                           const wxKeyEvent& event,
                           bool pressed);
    virtual bool HandleMouse(wxControl *control,
                             const wxMouseEvent& event);
    virtual bool HandleMouseMove(wxControl *control, const wxMouseEvent& event);

    virtual ~wxStdScrollBarInputHandler();

    // this method is called by wxScrollBarTimer only and may be overridden
    //
    // return TRUE to continue scrolling, FALSE to stop the timer
    virtual bool OnScrollTimer(wxScrollBar *scrollbar,
                               const wxControlAction& action);

protected:
    // the methods which must be overridden in the derived class

    // return TRUE if the mouse button can be used to activate scrollbar, FALSE
    // if not (only left mouse button can do it under Windows, any button under
    // GTK+)
    virtual bool IsAllowedButton(int button) = 0;

    // set or clear the specified flag on the scrollbar element corresponding
    // to m_htLast
    void SetElementState(wxScrollBar *scrollbar, int flag, bool doIt);

    // [un]highlight the scrollbar element corresponding to m_htLast
    virtual void Highlight(wxScrollBar *scrollbar, bool doIt)
        { SetElementState(scrollbar, wxCONTROL_CURRENT, doIt); }

    // [un]press the scrollbar element corresponding to m_htLast
    virtual void Press(wxScrollBar *scrollbar, bool doIt)
        { SetElementState(scrollbar, wxCONTROL_PRESSED, doIt); }

    // stop scrolling because we reached the end point
    void StopScrolling(wxScrollBar *scrollbar);

    // get the mouse coordinates in the scrollbar direction from the event
    wxCoord GetMouseCoord(const wxScrollBar *scrollbar,
                          const wxMouseEvent& event) const;

    // generate a "thumb move" action for this mouse event
    void HandleThumbMove(wxScrollBar *scrollbar, const wxMouseEvent& event);

    // the window (scrollbar) which has capture or NULL and the flag telling if
    // the mouse is inside the element which captured it or not
    wxWindow *m_winCapture;
    bool      m_winHasMouse;
    int       m_btnCapture;  // the mouse button which has captured mouse

    // the position where we started scrolling by page
    wxPoint m_ptStartScrolling;

    // one of wxHT_SCROLLBAR_XXX value: where has the mouse been last time?
    wxHitTest m_htLast;

    // the renderer (we use it only for hit testing)
    wxRenderer *m_renderer;

    // the offset of the top/left of the scrollbar relative to the mouse to
    // keep during the thumb drag
    int m_ofsMouse;

    // the timer for generating scroll events when the mouse stays pressed on
    // a scrollbar
    class wxTimer *m_timerScroll;
};

// ----------------------------------------------------------------------------
// wxStdListboxInputHandler: handles mouse and kbd in a single or multi
// selection listbox
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxStdListboxInputHandler : public wxStdInputHandler
{
public:
    wxStdListboxInputHandler(wxInputHandler *inphand);

    virtual bool HandleKey(wxControl *control,
                           const wxKeyEvent& event,
                           bool pressed);
    virtual bool HandleMouse(wxControl *control,
                             const wxMouseEvent& event);
    virtual bool HandleMouseMove(wxControl *control,
                                 const wxMouseEvent& event);

protected:
    // get the listbox item under mouse and return -1 if there is none
    int HitTest(const wxListBox *listbox, const wxMouseEvent& event);

    wxRenderer *m_renderer;

    wxWindow *m_winCapture;
};

#endif // _WX_UNIV_INPHAND_H_
