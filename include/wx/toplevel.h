///////////////////////////////////////////////////////////////////////////////
// Name:        wx/toplevel.h
// Purpose:     declares wxTopLevelWindow class, the base class for all
//              top level windows (such as frames and dialogs)
// Author:      Vadim Zeitlin, Vaclav Slavik
// Modified by:
// Created:     06.08.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
//                       Vaclav Slavik <vaclav@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TOPLEVEL_BASE_H_
#define _WX_TOPLEVEL_BASE_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma interface "toplevelbase.h"
#endif

#include "wx/window.h"
#include "wx/icon.h"

// the default names for various classs
WXDLLEXPORT_DATA(extern const wxChar*) wxFrameNameStr;

class WXDLLEXPORT wxTopLevelWindowBase;

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// Dialogs are created in a special way
#define wxTOPLEVEL_EX_DIALOG        0x00000008

// Styles for ShowFullScreen
// (note that wxTopLevelWindow only handles wxFULLSCREEN_NOBORDER and
//  wxFULLSCREEN_NOCAPTION; the rest is handled by wxTopLevelWindow)
enum
{
    wxFULLSCREEN_NOMENUBAR   = 0x0001,
    wxFULLSCREEN_NOTOOLBAR   = 0x0002,
    wxFULLSCREEN_NOSTATUSBAR = 0x0004,
    wxFULLSCREEN_NOBORDER    = 0x0008,
    wxFULLSCREEN_NOCAPTION   = 0x0010,
    
    wxFULLSCREEN_ALL         = wxFULLSCREEN_NOMENUBAR | wxFULLSCREEN_NOTOOLBAR | 
                               wxFULLSCREEN_NOSTATUSBAR | wxFULLSCREEN_NOBORDER | 
                               wxFULLSCREEN_NOCAPTION
};

// Flags for interactive frame manipulation functions (only in wxUniversal):
enum
{
    wxINTERACTIVE_MOVE           = 0x00000001,
    wxINTERACTIVE_RESIZE         = 0x00000002,
    wxINTERACTIVE_RESIZE_S       = 0x00000010,
    wxINTERACTIVE_RESIZE_N       = 0x00000020,
    wxINTERACTIVE_RESIZE_W       = 0x00000040,
    wxINTERACTIVE_RESIZE_E       = 0x00000080,
    wxINTERACTIVE_WAIT_FOR_INPUT = 0x10000000
};

// ----------------------------------------------------------------------------
// wxTopLevelWindow: a top level (as opposed to child) window
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxTopLevelWindowBase : public wxWindow
{
public:
    // construction
    wxTopLevelWindowBase();

#ifdef __DARWIN__
    virtual ~wxTopLevelWindowBase() {}
#endif

    // top level wnd state
    // --------------------

    // maximize = TRUE => maximize, otherwise - restore
    virtual void Maximize(bool maximize = TRUE) = 0;

    // undo Maximize() or Iconize()
    virtual void Restore() = 0;

    // iconize = TRUE => iconize, otherwise - restore
    virtual void Iconize(bool iconize = TRUE) = 0;

    // return TRUE if the frame is maximized
    virtual bool IsMaximized() const = 0;

    // return TRUE if the frame is iconized
    virtual bool IsIconized() const = 0;

    // get the frame icon
    const wxIcon& GetIcon() const { return m_icon; }

    // set the frame icon
    virtual void SetIcon(const wxIcon& icon) { m_icon = icon; }

    // maximize the window to cover entire screen
    virtual bool ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL) = 0;
    
    // return TRUE if the frame is in fullscreen mode
    virtual bool IsFullScreen() const = 0;

    /*
       for now we already have them in wxWindow, but this is wrong: these
       methods really only make sense for wxTopLevelWindow!

    virtual void SetTitle(const wxString& title) = 0;
    virtual wxString GetTitle() const = 0;
     */

    // old functions, use the new ones instead!
#if WXWIN_COMPATIBILITY_2
    bool Iconized() const { return IsIconized(); }
#endif // WXWIN_COMPATIBILITY_2

    
#ifdef __WXUNIVERSAL__
    // move/resize the frame interactively, i.e. let the user do it
    virtual void InteractiveMove(int flags = wxINTERACTIVE_MOVE);
#endif

    // implementation only from now on
    // -------------------------------

    // override some base class virtuals
    virtual bool Destroy();
    virtual bool IsTopLevel() const { return TRUE; }

    // event handlers
    void OnCloseWindow(wxCloseEvent& event);
    void OnSize(wxSizeEvent& event);

    // this should go away, but for now it's called from docview.cpp,
    // so should be there for all platforms
    void OnActivate(wxActivateEvent &WXUNUSED(event)) { }

protected:
    // the frame client to screen translation should take account of the
    // toolbar which may shift the origin of the client area
    virtual void DoClientToScreen(int *x, int *y) const;
    virtual void DoScreenToClient(int *x, int *y) const;

    // send the iconize event, return TRUE if processed
    bool SendIconizeEvent(bool iconized = TRUE);

    // the frame icon
    wxIcon m_icon;

    // test whether this window makes part of the frame
    // (menubar, toolbar and statusbar are excluded from automatic layout)
    virtual bool IsOneOfBars(const wxWindow *WXUNUSED(win)) const { return FALSE; }

    DECLARE_EVENT_TABLE()
};


// include the real class declaration
#if defined(__WXMSW__)
    #include "wx/msw/toplevel.h"
    #define wxTopLevelWindowNative wxTopLevelWindowMSW
#elif defined(__WXGTK__)
    #include "wx/gtk/toplevel.h"
    #define wxTopLevelWindowNative wxTopLevelWindowGTK
#elif defined(__WXMGL__)
    #include "wx/mgl/toplevel.h"
    #define wxTopLevelWindowNative wxTopLevelWindowMGL
#elif defined(__WXMAC__)
    #include "wx/mac/toplevel.h"
    #define wxTopLevelWindowNative wxTopLevelWindowMac
#endif

#ifdef __WXUNIVERSAL__
    #include "wx/univ/toplevel.h"
#else // !__WXUNIVERSAL__
    #ifdef wxTopLevelWindowNative
        class WXDLLEXPORT wxTopLevelWindow : public wxTopLevelWindowNative
        {
        public:
            // construction
            wxTopLevelWindow() { Init(); }
            wxTopLevelWindow(wxWindow *parent,
                       wxWindowID id,
                       const wxString& title,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = wxDEFAULT_FRAME_STYLE,
                       const wxString& name = wxFrameNameStr)
            {
                Init();
                Create(parent, id, title, pos, size, style, name);
            }

            DECLARE_DYNAMIC_CLASS(wxTopLevelWindow)
        };
    #endif // wxTopLevelWindowNative
#endif // __WXUNIVERSAL__/!__WXUNIVERSAL__


#endif // _WX_TOPLEVEL_BASE_H_
