/////////////////////////////////////////////////////////////////////////////
// Name:        frame.h
// Purpose:     wxFrame class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FRAME_H_
#define _WX_FRAME_H_

#ifdef __GNUG__
    #pragma interface "frame.h"
#endif

#include "wx/window.h"
#include "wx/toolbar.h"
#include "wx/msw/accel.h"
#include "wx/icon.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxFrameNameStr;
WXDLLEXPORT_DATA(extern const wxChar*) wxToolBarNameStr;
WXDLLEXPORT_DATA(extern const wxChar*) wxStatusLineNameStr;

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

    virtual void ClientToScreen(int *x, int *y) const;
    virtual void ScreenToClient(int *x, int *y) const;

    void OnSize(wxSizeEvent& event);
    void OnMenuHighlight(wxMenuEvent& event);
    void OnActivate(wxActivateEvent& event);
    void OnIdle(wxIdleEvent& event);
    void OnCloseWindow(wxCloseEvent& event);

    bool Show(bool show);

    // Set menu bar
    void SetMenuBar(wxMenuBar *menu_bar);
    virtual wxMenuBar *GetMenuBar() const;

    // Call this to simulate a menu command
    bool Command(int id) { return ProcessCommand(id); }

    // process menu command: returns TRUE if processed
    bool ProcessCommand(int id);

    // Set icon
    virtual void SetIcon(const wxIcon& icon);

    // Toolbar
    virtual wxToolBar* CreateToolBar(long style = wxNO_BORDER | wxTB_HORIZONTAL | wxTB_FLAT,
                                     wxWindowID id = -1,
                                     const wxString& name = wxToolBarNameStr);

    virtual wxToolBar *OnCreateToolBar(long style, wxWindowID id, const wxString& name);

    virtual void SetToolBar(wxToolBar *toolbar) { m_frameToolBar = toolbar; }
    virtual wxToolBar *GetToolBar() const { return m_frameToolBar; }

    virtual void PositionToolBar();

    // Status bar
    virtual wxStatusBar* CreateStatusBar(int number = 1,
                                         long style = wxST_SIZEGRIP,
                                         wxWindowID id = 0,
                                         const wxString& name = wxStatusLineNameStr);

    wxStatusBar *GetStatusBar() const { return m_frameStatusBar; }
    void SetStatusBar(wxStatusBar *statusBar) { m_frameStatusBar = statusBar; }

    virtual void PositionStatusBar();
    virtual wxStatusBar *OnCreateStatusBar(int number,
                                           long style,
                                           wxWindowID id,
                                           const wxString& name);

    // Set status line text
    virtual void SetStatusText(const wxString& text, int number = 0);

    // Set status line widths
    virtual void SetStatusWidths(int n, const int widths_field[]);

    // Hint to tell framework which status bar to use
    // TODO: should this go into a wxFrameworkSettings class perhaps?
    static void UseNativeStatusBar(bool useNative) { m_useNativeStatusBar = useNative; };
    static bool UsesNativeStatusBar() { return m_useNativeStatusBar; };

    // Iconize
    virtual void Iconize(bool iconize);

    virtual bool IsIconized() const;

    // Is it maximized?
    virtual bool IsMaximized() const;

    // Compatibility
    bool Iconized() const { return IsIconized(); }

    virtual void Maximize(bool maximize);
    //  virtual bool LoadAccelerators(const wxString& table);

    // Responds to colour changes
    void OnSysColourChanged(wxSysColourChangedEvent& event);

    // Query app for menu item updates (called from OnIdle)
    void DoMenuUpdates();
    void DoMenuUpdates(wxMenu* menu, wxWindow* focusWin);

    WXHMENU GetWinMenu() const { return m_hMenu; }

    // Returns the origin of client area (may be different from (0,0) if the
    // frame has a toolbar)
    virtual wxPoint GetClientAreaOrigin() const;

    // Implementation only from here
        // event handlers
    bool HandlePaint();
    bool HandleSize(int x, int y, WXUINT flag);
    bool HandleCommand(WXWORD id, WXWORD cmd, WXHWND control);
    bool HandleMenuSelect(WXWORD nItem, WXWORD nFlags, WXHMENU hMenu);

    bool MSWCreate(int id, wxWindow *parent, const char *wclass,
                   wxWindow *wx_win, const char *title,
                   int x, int y, int width, int height, long style);

  // tooltip management
#if wxUSE_TOOLTIPS
    WXHWND GetToolTipCtrl() const { return m_hwndToolTip; }
    void SetToolTipCtrl(WXHWND hwndTT) { m_hwndToolTip = hwndTT; }
#endif // tooltips

protected:
    // override base class virtuals
    virtual void DoGetClientSize(int *width, int *height) const;
    virtual void DoGetSize(int *width, int *height) const;
    virtual void DoGetPosition(int *x, int *y) const;

    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);
    virtual void DoSetClientSize(int width, int height);

    // a plug in for MDI frame classes which need to do something special when
    // the menubar is set
    virtual void InternalSetMenuBar();

    // propagate our state change to all child frames
    void IconizeChildFrames(bool bIconize);

    // we add menu bar accel processing
    bool MSWTranslateMessage(WXMSG* pMsg);

    // window proc for the frames
    long MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam);

    wxMenuBar *           m_frameMenuBar;
    wxStatusBar *         m_frameStatusBar;
    wxIcon                m_icon;
    bool                  m_iconized;
    WXHICON               m_defaultIcon;
    wxToolBar *           m_frameToolBar;

    static bool           m_useNativeStatusBar;

private:
#if wxUSE_TOOLTIPS
    WXHWND                m_hwndToolTip;
#endif // tooltips

    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_FRAME_H_
