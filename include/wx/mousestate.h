///////////////////////////////////////////////////////////////////////////////
// Name:        wx/mousestate.h
// Purpose:     Declaration of wxMouseState class
// Author:      Vadim Zeitlin
// Created:     2008-09-19 (extracted from wx/utils.h)
// RCS-ID:      $Id$
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOUSESTATE_H_
#define _WX_MOUSESTATE_H_

#include "wx/kbdstate.h"

// ----------------------------------------------------------------------------
// wxMouseState contains the information about mouse position, buttons and also
// key modifiers
// ----------------------------------------------------------------------------

// wxMouseState is used to hold information about button and modifier state
// and is what is returned from wxGetMouseState.
class WXDLLIMPEXP_CORE wxMouseState : public wxKeyboardState
{
public:
    wxMouseState()
        : m_leftDown(false), m_middleDown(false), m_rightDown(false),
          m_aux1Down(false), m_aux2Down(false),
          m_x(0), m_y(0)
    {
    }

    // default copy ctor, assignment operator and dtor are ok


    // accessors for the mouse position
    wxCoord GetX() const { return m_x; }
    wxCoord GetY() const { return m_y; }
    wxPoint GetPosition() const { return wxPoint(m_x, m_y); }

    // accessors for the pressed buttons
    bool LeftDown()    const { return m_leftDown; }
    bool MiddleDown()  const { return m_middleDown; }
    bool RightDown()   const { return m_rightDown; }
    bool Aux1Down()    const { return m_aux1Down; }
    bool Aux2Down()    const { return m_aux2Down; }

    // these functions are mostly used by wxWidgets itself
    void SetX(wxCoord x) { m_x = x; }
    void SetY(wxCoord y) { m_y = y; }

    void SetLeftDown(bool down)   { m_leftDown = down; }
    void SetMiddleDown(bool down) { m_middleDown = down; }
    void SetRightDown(bool down)  { m_rightDown = down; }
    void SetAux1Down(bool down)   { m_aux1Down = down; }
    void SetAux2Down(bool down)   { m_aux2Down = down; }

private:
    bool m_leftDown   : 1;
    bool m_middleDown : 1;
    bool m_rightDown  : 1;
    bool m_aux1Down   : 1;
    bool m_aux2Down   : 1;

    wxCoord m_x,
            m_y;
};

#endif // _WX_MOUSESTATE_H_

