///////////////////////////////////////////////////////////////////////////////
// Name:        wx/renderer.h
// Purpose:     wxRendererNative class declaration
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.07.2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/*
   Renderers are used in wxWindows for two similar but different things:
    (a) wxUniversal uses them to draw everything, i.e. all the control
    (b) all the native ports use them to draw generic controls only

   wxUniversal needs more functionality than what is included in the base class
   as it needs to draw stuff like scrollbars which are never going to be
   generic. So we put the bare minimum needed by the native ports here and the
   full wxRenderer class is declared in wx/univ/renderer.h and is only used by
   wxUniveral (although note that native ports can load wxRenderer objects from
   theme DLLs and use them as wxRendererNative ones, of course).
 */

#ifndef _WX_RENDERER_H_
#define _WX_RENDERER_H_

class WXDLLEXPORT wxDC;
class WXDLLEXPORT wxWindow;

#include "wx/gdicmn.h" // for wxPoint

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control state flags used in wxRenderer and wxColourScheme
enum
{
    wxCONTROL_DISABLED   = 0x00000001,  // control is disabled
    wxCONTROL_FOCUSED    = 0x00000002,  // currently has keyboard focus
    wxCONTROL_PRESSED    = 0x00000004,  // (button) is pressed
    wxCONTROL_ISDEFAULT  = 0x00000008,  // only applies to the buttons
    wxCONTROL_ISSUBMENU  = wxCONTROL_ISDEFAULT, // only for menu items
    wxCONTROL_EXPANDED   = wxCONTROL_ISDEFAULT, // only for the tree items
    wxCONTROL_CURRENT    = 0x00000010,  // mouse is currently over the control
    wxCONTROL_SELECTED   = 0x00000020,  // selected item in e.g. listbox
    wxCONTROL_CHECKED    = 0x00000040,  // (check/radio button) is checked
    wxCONTROL_CHECKABLE  = 0x00000080,  // (menu) item can be checked

    wxCONTROL_FLAGS_MASK = 0x000000ff,

    // this is a pseudo flag not used directly by wxRenderer but rather by some
    // controls internally
    wxCONTROL_DIRTY      = 0x80000000
};

// ----------------------------------------------------------------------------
// wxRendererNative: abstracts drawing methods needed by the native controls
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxRendererNative
{
public:
    virtual ~wxRendererNative() { } // stop GCC warning

    // drawing functions
    // -----------------

    // draw the header control button (used by wxListCtrl)
    virtual void DrawHeaderButton(wxWindow *win,
                                  wxDC& dc,
                                  const wxRect& rect,
                                  int flags = 0) = 0;

    // draw the expanded/collapsed icon for a tree control item
    virtual void DrawTreeItemButton(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int flags = 0) = 0;

    // draw the border for sash window: this border must be such that the sash
    // drawn by DrawSash() blends into it well
    virtual void DrawSplitterBorder(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect) = 0;

    // draw a (vertical) sash
    virtual void DrawSplitterSash(wxWindow *win,
                                  wxDC& dc,
                                  const wxSize& size,
                                  wxCoord position) = 0;


    // geometry functions
    // ------------------

    // get the splitter parameters: the x field of the returned point is the
    // sash width and the y field is the border width
    virtual wxPoint GetSplitterSashAndBorder(const wxWindow *win) = 0;


    // pseudo constructors
    // -------------------

    // return the currently used renderer
    static wxRendererNative& Get();

    // return the generic implementation of the renderer
    static wxRendererNative& GetGeneric();
};

// ----------------------------------------------------------------------------
// wxDelegateRendererNative: allows reuse of renderers code
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxDelegateRendererNative : public wxRendererNative
{
public:
    wxDelegateRendererNative()
        : m_rendererNative(GetGeneric()) { }

    wxDelegateRendererNative(wxRendererNative& rendererNative)
        : m_rendererNative(rendererNative) { }


    virtual void DrawHeaderButton(wxWindow *win,
                                  wxDC& dc,
                                  const wxRect& rect,
                                  int flags = 0)
        { m_rendererNative.DrawHeaderButton(win, dc, rect, flags); }

    virtual void DrawTreeItemButton(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int flags = 0)
        { m_rendererNative.DrawTreeItemButton(win, dc, rect, flags); }

    virtual void DrawSplitterBorder(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect)
        { m_rendererNative.DrawSplitterBorder(win, dc, rect); }

    virtual void DrawSplitterSash(wxWindow *win,
                                  wxDC& dc,
                                  const wxSize& size,
                                  wxCoord position)
        { m_rendererNative.DrawSplitterSash(win, dc, size, position); }


    virtual wxPoint GetSplitterSashAndBorder(const wxWindow *win)
        { return m_rendererNative.GetSplitterSashAndBorder(win); }

protected:
    wxRendererNative& m_rendererNative;

    DECLARE_NO_COPY_CLASS(wxDelegateRendererNative)
};

#endif // _WX_RENDERER_H_

