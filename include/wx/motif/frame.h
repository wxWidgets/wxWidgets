/////////////////////////////////////////////////////////////////////////////
// Name:        wx/motif/frame.h
// Purpose:     wxFrame class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOTIF_FRAME_H_
#define _WX_MOTIF_FRAME_H_

#ifdef __GNUG__
    #pragma interface "frame.h"
#endif

class WXDLLEXPORT wxFrame : public wxFrameBase
{
public:
    wxFrame() { Init(); }
    wxFrame(wxWindow *parent,
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

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);

    virtual ~wxFrame();

    bool Show(bool show);

    // Set menu bar
    void SetMenuBar(wxMenuBar *menu_bar);

    // Set title
    void SetTitle(const wxString& title);
    wxString GetTitle() const { return m_title; }

    // Set icon
    virtual void SetIcon(const wxIcon& icon);

#if wxUSE_STATUSBAR
    virtual void PositionStatusBar();
#endif // wxUSE_STATUSBAR

    // Create toolbar
#if wxUSE_TOOLBAR
    virtual wxToolBar* CreateToolBar(long style = wxNO_BORDER|wxTB_HORIZONTAL, wxWindowID id = -1, const wxString& name = wxToolBarNameStr);
    virtual void PositionToolBar();
#endif // wxUSE_TOOLBAR

    // Iconize
    virtual void Iconize(bool iconize);

    virtual bool IsIconized() const;

    // Is the frame maximized? Returns FALSE under Motif (but TRUE for
    // wxMDIChildFrame due to the tabbed implementation).
    virtual bool IsMaximized() const;

    virtual void Maximize(bool maximize);

    virtual void Raise();
    virtual void Lower();

    virtual void Restore();

    // Implementation only from now on
    // -------------------------------

    void OnSysColourChanged(wxSysColourChangedEvent& event);
    void OnActivate(wxActivateEvent& event);

    virtual void ChangeFont(bool keepOriginalSize = TRUE);
    virtual void ChangeBackgroundColour();
    virtual void ChangeForegroundColour();
    WXWidget GetMenuBarWidget() const;
    WXWidget GetShellWidget() const { return m_frameShell; }
    WXWidget GetWorkAreaWidget() const { return m_workArea; }
    WXWidget GetClientAreaWidget() const { return m_clientArea; }
    WXWidget GetTopWidget() const { return m_frameShell; }

    virtual WXWidget GetMainWidget() const { return m_frameWidget; }

    // The widget that can have children on it
    WXWidget GetClientWidget() const;
    bool GetVisibleStatus() const { return m_visibleStatus; }

    bool PreResize();

protected:
    // common part of all ctors
    void Init();

    //// Motif-specific
    WXWidget              m_frameShell;
    WXWidget              m_frameWidget;
    WXWidget              m_workArea;
    WXWidget              m_clientArea;
    wxString              m_title;
    bool                  m_visibleStatus;
    bool                  m_iconized;

    virtual void DoGetClientSize(int *width, int *height) const;
    virtual void DoGetSize(int *width, int *height) const;
    virtual void DoGetPosition(int *x, int *y) const;
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);
    virtual void DoSetClientSize(int width, int height);

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxFrame)
};

#endif
    // _WX_MOTIF_FRAME_H_
