/////////////////////////////////////////////////////////////////////////////
// Name:        frame.h
// Purpose:     wxFrame class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FRAME_H_
#define _WX_FRAME_H_

#ifdef __GNUG__
#pragma interface "frame.h"
#endif

#include "wx/window.h"
#include "wx/toolbar.h"
#include "wx/accel.h"

WXDLLEXPORT_DATA(extern const char*) wxFrameNameStr;
WXDLLEXPORT_DATA(extern const char*) wxToolBarNameStr;

class WXDLLEXPORT wxMenuBar;
class WXDLLEXPORT wxStatusBar;

class WXDLLEXPORT wxFrame : public wxWindow
{
DECLARE_DYNAMIC_CLASS(wxFrame)

public:
    wxFrame();
    wxFrame(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE,
            const wxString& name = wxFrameNameStr)
    {
        Create(parent, id, title, pos, size, style, name);
    }

    ~wxFrame();

    bool Create(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE,
            const wxString& name = wxFrameNameStr);

    virtual bool Destroy();

    void GetClientSize(int *width, int *height) const;
    void GetSize(int *width, int *height) const ;
    void GetPosition(int *x, int *y) const ;

    void ClientToScreen(int *x, int *y) const;
    wxPoint ClientToScreen(const wxPoint& pt) const { return wxWindow::ClientToScreen(pt); }

    void ScreenToClient(int *x, int *y) const;
    wxPoint ScreenToClient(const wxPoint& pt) const { return wxWindow::ScreenToClient(pt); }

    void OnSize(wxSizeEvent& event);
    void OnMenuHighlight(wxMenuEvent& event);
    void OnActivate(wxActivateEvent& event);
    void OnIdle(wxIdleEvent& event);
    void OnCloseWindow(wxCloseEvent& event);

    bool Show(bool show);

    // Set menu bar
    void SetMenuBar(wxMenuBar *menu_bar);
    virtual wxMenuBar *GetMenuBar() const ;

    // Set title
    void SetTitle(const wxString& title);
    wxString GetTitle() const { return m_title; }

    void Centre(int direction = wxBOTH);

    // Call this to simulate a menu command
    virtual void Command(int id);
    virtual void ProcessCommand(int id);

    // Set icon
    virtual void SetIcon(const wxIcon& icon);

    // Create status line
    virtual wxStatusBar* CreateStatusBar(int number=1, long style = wxST_SIZEGRIP, wxWindowID id = 0,
            const wxString& name = "statusBar");
    wxStatusBar *GetStatusBar() const { return m_frameStatusBar; }
    virtual void PositionStatusBar();
    virtual wxStatusBar *OnCreateStatusBar(int number, long style, wxWindowID id,
            const wxString& name);

    // Create toolbar
#if wxUSE_TOOLBAR
    virtual wxToolBar* CreateToolBar(long style = wxNO_BORDER|wxTB_HORIZONTAL, wxWindowID id = -1, const wxString& name = wxToolBarNameStr);
    virtual wxToolBar *OnCreateToolBar(long style, wxWindowID id, const wxString& name);
    // If made known to the frame, the frame will manage it automatically.
    virtual void SetToolBar(wxToolBar *toolbar) ;
    virtual wxToolBar *GetToolBar() const ;
    virtual void PositionToolBar();
#endif // wxUSE_TOOLBAR

    // Set status line text
    virtual void SetStatusText(const wxString& text, int number = 0);

    // Set status line widths
    virtual void SetStatusWidths(int n, const int widths_field[]);

    // Hint to tell framework which status bar to use
    // TODO: should this go into a wxFrameworkSettings class perhaps?
    static void UseNativeStatusBar(bool useNative) { m_useNativeStatusBar = useNative; };
    static bool UsesNativeStatusBar() { return m_useNativeStatusBar; };

    // Fit frame around subwindows
    virtual void Fit();

    // Iconize
    virtual void Iconize(bool iconize);

    virtual bool IsIconized() const ;

    // Compatibility
    bool Iconized() const { return IsIconized(); }

    // Is the frame maximized? Returns FALSE under Motif (but TRUE for
    // wxMDIChildFrame due to the tabbed implementation).
    virtual bool IsMaximized(void) const ;

    virtual void Maximize(bool maximize);

    // Responds to colour changes
    void OnSysColourChanged(wxSysColourChangedEvent& event);

    // Query app for menu item updates (called from OnIdle)
    void DoMenuUpdates();
    void DoMenuUpdates(wxMenu* menu);

    // Checks if there is a toolbar, and returns the first free client position
    virtual wxPoint GetClientAreaOrigin() const;

    virtual void Raise();
    virtual void Lower();

    virtual void CaptureMouse();
    virtual void ReleaseMouse();

    // Implementation
    virtual void ChangeFont(bool keepOriginalSize = TRUE);
    virtual void ChangeBackgroundColour();
    virtual void ChangeForegroundColour();
    WXWidget GetMenuBarWidget() const ;
    WXWidget GetShellWidget() const { return m_frameShell; }
    WXWidget GetWorkAreaWidget() const { return m_workArea; }
    WXWidget GetClientAreaWidget() const { return m_clientArea; }
    WXWidget GetTopWidget() const { return m_frameShell; }
    WXWidget GetMainWindowWidget() const { return m_frameWidget; }

    // The widget that can have children on it
    WXWidget GetClientWidget() const;
    bool GetVisibleStatus() const { return m_visibleStatus; }

    bool PreResize();

protected:
    wxMenuBar *           m_frameMenuBar;
    wxStatusBar *         m_frameStatusBar;
    wxIcon                m_icon;
    bool                  m_iconized;
    static bool           m_useNativeStatusBar;

#if wxUSE_TOOLBAR
    wxToolBar *           m_frameToolBar ;
#endif // wxUSE_TOOLBAR

    //// Motif-specific

    WXWidget              m_frameShell;
    WXWidget              m_frameWidget;
    WXWidget              m_workArea;
    WXWidget              m_clientArea;
    //  WXWidget              m_menuBarWidget;
    bool                  m_visibleStatus;
    wxString              m_title;

    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);

    virtual void DoSetClientSize(int width, int height);

private:
    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_FRAME_H_
