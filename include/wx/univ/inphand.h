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

#endif // _WX_UNIV_INPHAND_H_
