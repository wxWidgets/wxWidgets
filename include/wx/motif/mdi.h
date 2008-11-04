/////////////////////////////////////////////////////////////////////////////
// Name:        wx/motif/mdi.h
// Purpose:     MDI (Multiple Document Interface) classes.
// Author:      Julian Smart
// Modified by: 2008-10-31 Vadim Zeitlin: derive from the base classes
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
//              (c) 2008 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOTIF_MDI_H_
#define _WX_MOTIF_MDI_H_

class WXDLLIMPEXP_CORE wxMDIParentFrame : public wxMDIParentFrameBase
{
public:
    wxMDIParentFrame() { Init(); }
    wxMDIParentFrame(wxWindow *parent,
                     wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
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
                long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                const wxString& name = wxFrameNameStr);

    virtual ~wxMDIParentFrame();

    // implement base class pure virtuals
    // ----------------------------------

    static bool IsTDI() { return true; }

    virtual void ActivateNext() { /* TODO */ }
    virtual void ActivatePrevious() { /* TODO */ }


    // Implementation

    // Set the child's menubar into the parent frame
    void SetChildMenuBar(wxMDIChildFrame* frame);

    wxMenuBar* GetActiveMenuBar() const { return m_activeMenuBar; }

    // Redirect events to active child first
    virtual bool ProcessEvent(wxEvent& event);

    void OnSize(wxSizeEvent& event);
    void OnActivate(wxActivateEvent& event);
    void OnSysColourChanged(wxSysColourChangedEvent& event);
    void OnMenuHighlight(wxMenuEvent& event);

    void SetMenuBar(wxMenuBar *menu_bar);

protected:
    wxMenuBar *m_activeMenuBar;

private:
    void Init();

    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxMDIParentFrame)
};

class WXDLLIMPEXP_CORE wxMDIChildFrame : public wxTDIChildFrame
{
public:
    wxMDIChildFrame() { }
    wxMDIChildFrame(wxMDIParentFrame *parent,
                    wxWindowID id,
                    const wxString& title,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxDEFAULT_FRAME_STYLE,
                    const wxString& name = wxFrameNameStr)
    {
        Create(parent, id, title, pos, size, style, name);
    }

    bool Create(wxMDIParentFrame *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);

    virtual ~wxMDIChildFrame();


    // Set menu bar
    void SetMenuBar(wxMenuBar *menu_bar);
    void SetTitle(const wxString& title);

    // Set icon
    virtual void SetIcon(const wxIcon& icon);
    virtual void SetIcons(const wxIconBundle& icons );

    // Override wxFrame operations
    void CaptureMouse();
    void ReleaseMouse();
    void Raise();
    void Lower(void);

    // MDI operations
    virtual void Maximize();
    virtual void Maximize(bool WXUNUSED(maximize)) { };
    inline void Minimize() { Iconize(true); };
    virtual void Iconize(bool iconize);
    virtual void Restore();
    virtual void Activate();
    virtual bool IsIconized() const ;

    virtual bool IsTopLevel() const { return false; }

    // Is the frame maximized? Returns true for
    // wxMDIChildFrame due to the tabbed implementation.
    virtual bool IsMaximized(void) const ;

    bool Show(bool show);

    WXWidget GetMainWidget() const { return m_mainWidget; };
    WXWidget GetTopWidget() const { return m_mainWidget; };
    WXWidget GetClientWidget() const { return m_mainWidget; };

protected:
    DECLARE_DYNAMIC_CLASS(wxMDIChildFrame)
};

class WXDLLIMPEXP_CORE wxMDIClientWindow : public wxMDIClientWindowBase
{
public:
    wxMDIClientWindow() { }
    virtual ~wxMDIClientWindow();

    virtual bool CreateClient(wxMDIParentFrame *parent,
                              long style = wxVSCROLL | wxHSCROLL);

    // Implementation
    void OnPageChanged(wxBookCtrlEvent& event);

    int FindPage(const wxNotebookPage* page);

protected:
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);
    virtual void DoSetClientSize(int width, int height);

    virtual void DoGetClientSize(int *width, int *height) const;
    virtual void DoGetSize(int *width, int *height) const ;
    virtual void DoGetPosition(int *x, int *y) const ;

    wxNotebook *m_notebook;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxMDIClientWindow)
};

#endif // _WX_MOTIF_MDI_H_
