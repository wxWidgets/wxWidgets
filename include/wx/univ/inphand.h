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

#include "wx/control.h" // for wxControlAction

// ----------------------------------------------------------------------------
// wxInputHandler: maps the events to the actions
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxInputHandler
{
public:
    // map a keyboard event to an action (pressed == TRUE if the key was
    // pressed, FALSE if released)
    virtual wxControlAction Map(const wxKeyEvent& event, bool pressed) = 0;

    // map a mouse event to an action
    virtual wxControlAction Map(const wxMouseEvent& event) = 0;

    // virtual dtor for any base class
    virtual ~wxInputHandler();
};

// ----------------------------------------------------------------------------
// the control names which can be passed to wxTheme::GetInputHandler()
// ----------------------------------------------------------------------------

#define wxCONTROL_DEFAULT   _T("control")
#define wxCONTROL_BUTTON    _T("button")

#endif // _WX_UNIV_INPHAND_H_
