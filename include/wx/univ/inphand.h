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

#include "wx/control.h" // for wxControlAction(s)

// ----------------------------------------------------------------------------
// wxInputHandler: maps the events to the actions
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxInputHandler
{
public:
    // map a keyboard event to one or more actions (pressed == TRUE if the key
    // was pressed, FALSE if released)
    virtual wxControlActions Map(wxControl *control,
                                 const wxKeyEvent& event,
                                 bool pressed) = 0;

    // map a mouse (click) event to one or more actions
    virtual wxControlActions Map(wxControl *control,
                                 const wxMouseEvent& event) = 0;

    // do something with mouse move/enter/leave: unlike the Map() functions,
    // this doesn't translate the event into an action but, normally, uses the
    // renderer directly to change the controls appearance as needed
    //
    // this is faster than using Map() which is important for mouse move
    // events as they occur often and in a quick succession
    //
    // return TRUE to refresh the control, FALSE otherwise
    virtual bool OnMouseMove(wxControl *control, const wxMouseEvent& event);

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

    virtual wxControlActions Map(wxControl *control,
                                 const wxKeyEvent& event,
                                 bool pressed)
        { return m_handler->Map(control, event, pressed); }
    virtual wxControlActions Map(wxControl *control,
                                 const wxMouseEvent& event)
        { return m_handler->Map(control, event); }
    virtual bool OnMouseMove(wxControl *control, const wxMouseEvent& event)
        { return m_handler->OnMouseMove(control, event); }

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

    virtual wxControlActions Map(wxControl *control,
                                 const wxKeyEvent& event,
                                 bool pressed);
    virtual wxControlActions Map(wxControl *control,
                                 const wxMouseEvent& event);
    virtual bool OnMouseMove(wxControl *control, const wxMouseEvent& event);

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

    virtual wxControlActions Map(wxControl *control,
                                 const wxKeyEvent& event,
                                 bool pressed);
    virtual wxControlActions Map(wxControl *control,
                                 const wxMouseEvent& event);
    virtual bool OnMouseMove(wxControl *control, const wxMouseEvent& event);

protected:
    // the methods which must be overridden in the derived class

    // return TRUE to stop scrolling when the mouse leaves the scrollbar (Win32
    // behaviour) or FALSE to continue (GTK+)
    virtual bool OnMouseLeave() = 0;

    // return TRUE if the mouse button can be used to activate scrollbar, FALSE
    // if not (only left mouse button can do it under Windows, any button under
    // GTK+)
    virtual bool IsAllowedButton(int button) = 0;

    // set or clear the specified flag on the scrollbar element corresponding
    // to m_htLast
    void SetElementState(wxScrollBar *scrollbar, int flag, bool doIt);

    // [un]highlight the scrollbar element corresponding to m_htLast
    void Highlight(wxScrollBar *scrollbar, bool doIt)
        { SetElementState(scrollbar, wxCONTROL_CURRENT, doIt); }

    // [un]press the scrollbar element corresponding to m_htLast
    void Press(wxScrollBar *scrollbar, bool doIt)
        { SetElementState(scrollbar, wxCONTROL_PRESSED, doIt); }

    // the window (scrollbar) which has capture or NULL and the flag telling if
    // the mouse is inside the element which captured it or not
    wxWindow *m_winCapture;
    bool      m_winHasMouse;
    int       m_btnCapture;  // the mouse button which has captured mouse

    // one of wxHT_SCROLLBAR_XXX value: where has the mouse been last time?
    wxHitTest m_htLast;

    // the renderer (we use it only for hit testing)
    wxRenderer *m_renderer;
};

#endif // _WX_UNIV_INPHAND_H_
