///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/scrarrow.h
// Purpose:     wxScrollArrows class
// Author:      Vadim Zeitlin
// Modified by:
// Created:     22.01.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_SCRARROW_H_
#define _WX_UNIV_SCRARROW_H_

#ifdef __GNUG__
    #pragma interface "univscrarrow.h"
#endif

// ----------------------------------------------------------------------------
// wxScrollArrows is not a control but just a class containing the common
// functionality of scroll arrows, whether part of scrollbars, spin ctrls or
// anything else.
//
// To customize its behaviour, wxScrollArrows doesn't use any virtual methods
// but instead a callback pointer to a wxControlWithArrows object which is used
// for all control-dependent stuff. Thus, to use wxScrollArrows, you just need
// to derive from the wxControlWithArrows interface and implement its methods.
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxControlWithArrows;
class WXDLLEXPORT wxDC;
class WXDLLEXPORT wxMouseEvent;
class WXDLLEXPORT wxRect;
class WXDLLEXPORT wxRenderer;

// ----------------------------------------------------------------------------
// wxScrollArrows: an abstraction of scrollbar arrow
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxScrollArrows
{
public:
    enum Arrow
    {
        Arrow_None = -1,
        Arrow_First,        // left or top
        Arrow_Second,       // right or bottom
        Arrow_Max
    };

    // ctor requires a back pointer to wxControlWithArrows
    wxScrollArrows(wxControlWithArrows *control);

    // draws the arrow on the given DC in the given rectangle, uses
    // wxControlWithArrows::GetArrowState() to get its current state
    void DrawArrow(Arrow arrow, wxDC& dc, const wxRect& rect,
                   bool scrollbarLike = FALSE) const;

    // process a mouse move, enter or leave event, possibly calling
    // wxControlWithArrows::SetArrowState() if wxControlWithArrows::HitTest()
    // says that the mosue has left/entered an arrow
    bool HandleMouseMove(const wxMouseEvent& event) const;

    // process a mouse click event
    bool HandleMouse(const wxMouseEvent& event) const;

    // dtor
    ~wxScrollArrows();

private:
    // set or clear the wxCONTROL_CURRENT flag for the arrow
    void UpdateCurrentFlag(Arrow arrow, Arrow arrowCur) const;

    // the main control
    wxControlWithArrows *m_control;

    // the data for the mouse capture
    struct wxScrollArrowCaptureData *m_captureData;
};

// ----------------------------------------------------------------------------
// wxControlWithArrows: interface implemented by controls using wxScrollArrows
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxControlWithArrows
{
public:
    // get the renderer to use for drawing the arrows
    virtual wxRenderer *GetRenderer() const = 0;

    // get the controls window (used for mouse capturing)
    virtual wxWindow *GetWindow() = 0;

    // get the orientation of the arrows (vertical or horizontal)
    virtual bool IsVertical() const = 0;

    // get the state of this arrow as combination of wxCONTROL_XXX flags
    virtual int GetArrowState(wxScrollArrows::Arrow arrow) const = 0;

    // set or clear the specified flag in the arrow state: this function is
    // responsible for refreshing the control
    virtual void SetArrowFlag(wxScrollArrows::Arrow arrow,
                              int flag, bool set = TRUE) = 0;

    // hit testing: return on which arrow the point is (or Arrow_None)
    virtual wxScrollArrows::Arrow HitTest(const wxPoint& pt) const = 0;

    // called when the arrow is pressed, return TRUE to continue scrolling and
    // FALSE to stop it
    virtual bool OnArrow(wxScrollArrows::Arrow arrow) = 0;
};

#endif // _WX_UNIV_SCRARROW_H_
