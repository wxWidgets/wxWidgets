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

// ----------------------------------------------------------------------------
// types of the standard input handlers which can be passed to
// wxTheme::GetInputHandler()
// ----------------------------------------------------------------------------

#define wxINP_HANDLER_DEFAULT           _T("")
#define wxINP_HANDLER_BUTTON            _T("button")
#define wxINP_HANDLER_CHECKBOX          _T("checkbox")
#define wxINP_HANDLER_CHECKLISTBOX      _T("checklistbox")
#define wxINP_HANDLER_COMBOBOX          _T("combobox")
#define wxINP_HANDLER_LISTBOX           _T("listbox")
#define wxINP_HANDLER_NOTEBOOK          _T("notebook")
#define wxINP_HANDLER_RADIOBTN          _T("radiobtn")
#define wxINP_HANDLER_SCROLLBAR         _T("scrollbar")
#define wxINP_HANDLER_SLIDER            _T("slider")
#define wxINP_HANDLER_SPINBTN           _T("spinbtn")
#define wxINP_HANDLER_TEXTCTRL          _T("textctrl")

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

    // react to the app getting/losing activation
    //
    // return TRUE to refresh the control, FALSE otherwise
    virtual bool HandleActivation(wxControl *control, bool activated);

    // virtual dtor for any base class
    virtual ~wxInputHandler();
};

// ----------------------------------------------------------------------------
// wxStdInputHandler is just a base class for all other "standard" handlers
// and also provides the way to chain input handlers together
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxStdInputHandler : public wxInputHandler
{
public:
    wxStdInputHandler(wxInputHandler *handler) : m_handler(handler) { }

    virtual bool HandleKey(wxControl *control,
                           const wxKeyEvent& event,
                           bool pressed)
    {
        return m_handler ? m_handler->HandleKey(control, event, pressed)
                         : FALSE;
    }

    virtual bool HandleMouse(wxControl *control,
                             const wxMouseEvent& event)
    {
        return m_handler ? m_handler->HandleMouse(control, event) : FALSE;
    }

    virtual bool HandleMouseMove(wxControl *control, const wxMouseEvent& event)
    {
        return m_handler ? m_handler->HandleMouseMove(control, event) : FALSE;
    }

    virtual bool HandleFocus(wxControl *control, const wxFocusEvent& event)
    {
        return m_handler ? m_handler->HandleFocus(control, event) : FALSE;
    }

private:
    wxInputHandler *m_handler;
};

#endif // _WX_UNIV_INPHAND_H_
