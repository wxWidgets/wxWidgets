///////////////////////////////////////////////////////////////////////////////
// Name:        caret.h
// Purpose:     wxCaretBase class - the interface of wxCaret
// Author:      Vadim Zeitlin
// Modified by:
// Created:     23.05.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CARET_H_BASE_
#define _WX_CARET_H_BASE_

#ifdef __GNUG__
#pragma interface "caret.h"
#endif

// ---------------------------------------------------------------------------
// forward declarations
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxWindow;
class WXDLLEXPORT wxWindowBase;

// ----------------------------------------------------------------------------
// headers we have to include
// ----------------------------------------------------------------------------

#include "wx/gdicmn.h"  // for wxPoint, wxSize

// ----------------------------------------------------------------------------
// A caret is a blinking cursor showing the position where the typed text will
// appear. It can be either a solid block or a custom bitmap (TODO)
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxCaretBase
{
public:
    // ctors
    // -----
        // default - use Create
    wxCaretBase() { Init(); }
        // create the caret of given (in pixels) width and height and associate
        // with the given window
    wxCaretBase(wxWindowBase *window, int width, int height)
    {
        Init();

        (void)Create(window, width, height);
    }
        // same as above
    wxCaretBase(wxWindowBase *window, const wxSize& size)
    {
        Init();

        (void)Create(window, size);
    }

    // Create() functions - same as ctor but returns the success code
    // --------------------------------------------------------------

        // same as ctor
    bool Create(wxWindowBase *window, int width, int height)
        { return DoCreate(window, width, height); }
        // same as ctor
    bool Create(wxWindowBase *window, const wxSize& size)
        { return DoCreate(window, size.x, size.y); }

    // accessors
    // ---------

        // is the caret valid?
    bool IsOk() const { return m_width != 0 && m_height != 0; }

        // is the caret currently shown?
    bool IsVisible() const { return m_countVisible > 0; }

        // get the caret position
    void GetPosition(int *x, int *y) const
    {
        if ( x ) *x = m_x;
        if ( y ) *y = m_y;
    }
    wxPoint GetPosition() const { return wxPoint(m_x, m_y); }

        // get the caret size
    void GetSize(int *width, int *height) const
    {
        if ( width ) *width = m_width;
        if ( height ) *height = m_height;
    }
    wxSize GetSize() const { return wxSize(m_width, m_height); }

        // get the window we're associated with
    wxWindow *GetWindow() const { return (wxWindow *)m_window; }

    // operations
    // ----------

        // move the caret to given position (in logical coords)
    void Move(int x, int y) { m_x = x; m_y = y; DoMove(); }
    void Move(const wxPoint& pt) { m_x = pt.x; m_y = pt.y; DoMove(); }

        // show/hide the caret (should be called by wxWindow when needed):
        // Show() must be called as many times as Hide() + 1 to make the caret
        // visible
    virtual void Show(bool show = TRUE)
        {
            if ( show )
            {
                if ( m_countVisible++ == 0 )
                    DoShow();
            }
            else
            {
                if ( --m_countVisible == 0 )
                    DoHide();
            }
        }
    virtual void Hide() { Show(FALSE); }

        // blink time is measured in milliseconds and is the time elapsed
        // between 2 inversions of the caret (blink time of the caret is common
        // to all carets in the Universe, so these functions are static)
    static int GetBlinkTime();
    static void SetBlinkTime(int milliseconds);

    // implementation from now on
    // --------------------------

    // these functions should be called by wxWindow when the window gets/loses
    // the focus - we create/show and hide/destroy the caret here
    virtual void OnSetFocus() { }
    virtual void OnKillFocus() { }

protected:
    // these functions may be overriden in the derived classes, but they
    // should call the base class version first
    virtual bool DoCreate(wxWindowBase *window, int width, int height)
    {
        m_window = window;
        m_width = width;
        m_height = height;

        return TRUE;
    }

    // pure virtuals to implement in the derived class
    virtual void DoShow() = 0;
    virtual void DoHide() = 0;
    virtual void DoMove() = 0;

    // the common initialization
    void Init()
    {
        m_window = (wxWindowBase *)NULL;
        m_x = m_y = 0;
        m_width = m_height = 0;
        m_countVisible = 0;
    }

    // the size of the caret
    int m_width, m_height;

    // the position of the caret
    int m_x, m_y;

    // the window we're associated with
    wxWindowBase *m_window;

    // visibility count: the caret is visible only if it's positive
    int m_countVisible;

private:
    DECLARE_NO_COPY_CLASS(wxCaretBase);
};

// ---------------------------------------------------------------------------
// now include the real thing
// ---------------------------------------------------------------------------

#if defined(__WXMSW__)
    #include "wx/msw/caret.h"
#elif  defined(__WXPM__)
    #include "wx/os2/caret.h"
#else
    #include "wx/generic/caret.h"
#endif // platform

#endif // _WX_CARET_H_BASE_

