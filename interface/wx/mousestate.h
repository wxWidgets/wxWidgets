/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mousestate.h
// Purpose:     documentation of wxMouseState
// Author:      wxWidgets team
// Created:     2008-09-19
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


/**
    @class wxMouseState

    Represents the mouse state.

    This class is used as a base class by wxMouseEvent and so its methods may
    be used to obtain information about the mouse state for the mouse events.
    It also inherits from wxKeyboardState and so carries information about the
    keyboard state and not only the mouse one.

    This class is implemented entirely inline in @<wx/mousestate.h@> and thus
    has no linking requirements.

    @nolibrary
    @category{misc}

    @see wxGetMouseState(), wxMouseEvent
 */
class wxMouseState : public wxKeyboardState
{
public:
    /**
        Default constructor.
    */
    wxMouseState();

    /**
        Returns X coordinate of the physical mouse event position.
    */
    wxCoord GetX() const;

    /**
        Returns Y coordinate of the physical mouse event position.
    */
    wxCoord GetY() const;

    /**
        Returns the physical mouse position.
    */
    wxPoint GetPosition() const;

    /**
        Returns @true if the left mouse button changed to down.
    */
    bool LeftDown() const;

    /**
        Returns @true if the middle mouse button changed to down.
    */
    bool MiddleDown() const;

    /**
        Returns @true if the right mouse button changed to down.
    */
    bool RightDown() const;

    /**
        Returns @true if the first extra button mouse button changed to down.
    */
    bool Aux1Down() const;

    /**
        Returns @true if the second extra button mouse button changed to down.
    */
    bool Aux2Down() const;
};


