/////////////////////////////////////////////////////////////////////////////
// Name:        wx/frame.h
// Purpose:     wxFrame class interface
// Author:      Vadim Zeitlin
// Modified by:
// Created:     15.11.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FRAME_H_BASE_
#define _WX_FRAME_H_BASE_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma interface "framebase.h"
#endif

#include "wx/window.h"      // the base class
#include "wx/icon.h"        // for m_icon

// the default names for various classs
WXDLLEXPORT_DATA(extern const wxChar*) wxFrameNameStr;
WXDLLEXPORT_DATA(extern const wxChar*) wxStatusLineNameStr;
WXDLLEXPORT_DATA(extern const wxChar*) wxToolBarNameStr;
WXDLLEXPORT_DATA(extern wxWindow*) wxWndHook;

class WXDLLEXPORT wxMenuBar;
class WXDLLEXPORT wxStatusBar;
class WXDLLEXPORT wxToolBar;

// Styles for ShowFullScreen
#define wxFULLSCREEN_NOMENUBAR      0x01
#define wxFULLSCREEN_NOTOOLBAR      0x02
#define wxFULLSCREEN_NOSTATUSBAR    0x04
#define wxFULLSCREEN_NOBORDER       0x08
#define wxFULLSCREEN_NOCAPTION      0x10
#define wxFULLSCREEN_ALL            (wxFULLSCREEN_NOMENUBAR | wxFULLSCREEN_NOTOOLBAR | wxFULLSCREEN_NOSTATUSBAR | wxFULLSCREEN_NOBORDER | wxFULLSCREEN_NOCAPTION)

// ----------------------------------------------------------------------------
// wxFrame is a top-level window with optional menubar, statusbar and toolbar
//
// For each of *bars, a frame may have several of them, but only one is
// managed by the frame, i.e. resized/moved when the frame is and whose size
// is accounted for in client size calculations - all others should be taken
// care of manually. The CreateXXXBar() functions create this, main, XXXBar,
// but the actual creation is done in OnCreateXXXBar() functions which may be
// overridden to create custom objects instead of standard ones when
// CreateXXXBar() is called.
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFrameBase : public wxWindow
{
public:
    // construction
    wxFrameBase();

    wxFrame *New(wxWindow *parent,
                 wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxDEFAULT_FRAME_STYLE,
                 const wxString& name = wxFrameNameStr);

    // frame state
    // -----------

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

    // make the window modal (all other windows unresponsive)
    virtual void MakeModal(bool modal = TRUE);

    // get the origin of the client area (which may be different from (0, 0)
    // if the frame has a toolbar) in client coordinates
    virtual wxPoint GetClientAreaOrigin() const;

    // menu bar functions
    // ------------------

#if wxUSE_MENUS
    virtual void SetMenuBar(wxMenuBar *menubar) = 0;
    virtual wxMenuBar *GetMenuBar() const { return m_frameMenuBar; }
#endif // wxUSE_MENUS

    // call this to simulate a menu command
    bool Command(int id) { return ProcessCommand(id); }

    // process menu command: returns TRUE if processed
    bool ProcessCommand(int id);

    // status bar functions
    // --------------------
#if wxUSE_STATUSBAR
    // create the main status bar by calling OnCreateStatusBar()
    virtual wxStatusBar* CreateStatusBar(int number = 1,
                                         long style = wxST_SIZEGRIP,
                                         wxWindowID id = 0,
                                         const wxString& name =
                                            wxStatusLineNameStr);
    // return a new status bar
    virtual wxStatusBar *OnCreateStatusBar(int number,
                                           long style,
                                           wxWindowID id,
                                           const wxString& name);
    // get the main status bar
    virtual wxStatusBar *GetStatusBar() const { return m_frameStatusBar; }

    // sets the main status bar
    void SetStatusBar(wxStatusBar *statBar) { m_frameStatusBar = statBar; }

    // forward these to status bar
    virtual void SetStatusText(const wxString &text, int number = 0);
    virtual void SetStatusWidths(int n, const int widths_field[]);
#endif // wxUSE_STATUSBAR

    // toolbar functions
    // -----------------
#if wxUSE_TOOLBAR
    // create main toolbar bycalling OnCreateToolBar()
    virtual wxToolBar* CreateToolBar(long style = wxNO_BORDER|wxTB_HORIZONTAL,
                                     wxWindowID id = -1,
                                     const wxString& name = wxToolBarNameStr);
    // return a new toolbar
    virtual wxToolBar *OnCreateToolBar(long style,
                                       wxWindowID id,
                                       const wxString& name );

    // get/set the main toolbar
    virtual wxToolBar *GetToolBar() const { return m_frameToolBar; }
    virtual void SetToolBar(wxToolBar *toolbar) { m_frameToolBar = toolbar; }
#endif // wxUSE_TOOLBAR

    // old functions, use the new ones instead!
#if WXWIN_COMPATIBILITY_2
    bool Iconized() const { return IsIconized(); }
#endif // WXWIN_COMPATIBILITY_2

    // implementation only from now on
    // -------------------------------

    // override some base class virtuals
    virtual bool Destroy();
    virtual bool IsTopLevel() const { return TRUE; }

    // event handlers
    void OnIdle(wxIdleEvent& event);
    void OnCloseWindow(wxCloseEvent& event);
    void OnMenuHighlight(wxMenuEvent& event);
    void OnSize(wxSizeEvent& event);
    // this should go away, but for now it's called from docview.cpp,
    // so should be there for all platforms
    void OnActivate(wxActivateEvent &WXUNUSED(event)) { }

#if wxUSE_MENUS
    // send wxUpdateUIEvents for all menu items (called from OnIdle())
    void DoMenuUpdates();
    void DoMenuUpdates(wxMenu* menu, wxWindow* focusWin);
#endif // wxUSE_MENUS

protected:
    // the frame main menu/status/tool bars
    // ------------------------------------

    // this (non virtual!) function should be called from dtor to delete the
    // main menubar, statusbar and toolbar (if any)
    void DeleteAllBars();

#if wxUSE_MENUS
    wxMenuBar *m_frameMenuBar;
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // override to update status bar position (or anything else) when
    // something changes
    virtual void PositionStatusBar() { }

    // show the help string for this menu item in the given status bar: the
    // status bar pointer can be NULL; return TRUE if help was shown
    bool ShowMenuHelp(wxStatusBar *statbar, int id);

    wxStatusBar *m_frameStatusBar;
#endif // wxUSE_STATUSBAR

#if wxUSE_TOOLBAR
    // override to update status bar position (or anything else) when
    // something changes
    virtual void PositionToolBar() { }

    wxToolBar *m_frameToolBar;
#endif // wxUSE_TOOLBAR

    // the frame client to screen translation should take account of the
    // toolbar which may shift the origin of the client area
    virtual void DoClientToScreen(int *x, int *y) const;
    virtual void DoScreenToClient(int *x, int *y) const;

    // the frame icon
    wxIcon m_icon;

    DECLARE_EVENT_TABLE()
};

// include the real class declaration
#if defined(__WXMSW__)
    #include "wx/msw/frame.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/frame.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/frame.h"
#elif defined(__WXQT__)
    #include "wx/qt/frame.h"
#elif defined(__WXMAC__)
    #include "wx/mac/frame.h"
#elif defined(__WXPM__)
    #include "wx/os2/frame.h"
#elif defined(__WXSTUBS__)
    #include "wx/stubs/frame.h"
#endif

#endif
    // _WX_FRAME_H_BASE_
