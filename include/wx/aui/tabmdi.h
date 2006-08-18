/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/tabmdi.h
// Purpose:     Generic MDI (Multiple Document Interface) classes
// Author:      Hans Van Leemputten
// Modified by: Benjamin I. Williams / Kirix Corporation
// Created:     29/07/2002
// RCS-ID:      $Id$
// Copyright:   (c) Hans Van Leemputten
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_AUITABMDI_H_
#define _WX_AUITABMDI_H_

#if wxUSE_AUI

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/frame.h"
#include "wx/panel.h"
#include "wx/notebook.h"
#include "wx/aui/auibook.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_AUI wxTabMDIParentFrame;
class WXDLLIMPEXP_AUI wxTabMDIClientWindow;
class WXDLLIMPEXP_AUI wxTabMDIChildFrame;

//-----------------------------------------------------------------------------
// wxTabMDIParentFrame
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_AUI wxTabMDIParentFrame : public wxFrame
{
public:
    wxTabMDIParentFrame();
    wxTabMDIParentFrame(wxWindow *parent,
                        wxWindowID winid,
                        const wxString& title,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                        const wxString& name = wxFrameNameStr);

    ~wxTabMDIParentFrame();

    bool Create(wxWindow *parent,
                wxWindowID winid,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                const wxString& name = wxFrameNameStr );

#if wxUSE_MENUS
    wxMenu* GetWindowMenu() const { return m_pWindowMenu; };
    void SetWindowMenu(wxMenu* pMenu);

    virtual void SetMenuBar(wxMenuBar *pMenuBar);
#endif // wxUSE_MENUS

    void SetChildMenuBar(wxTabMDIChildFrame *pChild);

    virtual bool ProcessEvent(wxEvent& event);

    wxTabMDIChildFrame *GetActiveChild() const;
    inline void SetActiveChild(wxTabMDIChildFrame* pChildFrame);

    wxTabMDIClientWindow *GetClientWindow() const;
    virtual wxTabMDIClientWindow *OnCreateClient();

    virtual void Cascade() { /* Has no effect */ }
    virtual void Tile(wxOrientation WXUNUSED(orient) = wxHORIZONTAL) { }
    virtual void ArrangeIcons() { /* Has no effect */ }
    virtual void ActivateNext();
    virtual void ActivatePrevious();

protected:
    wxTabMDIClientWindow   *m_pClientWindow;
    wxTabMDIChildFrame     *m_pActiveChild;

#if wxUSE_MENUS
    wxMenu              *m_pWindowMenu;
    wxMenuBar           *m_pMyMenuBar;
#endif // wxUSE_MENUS

protected:
    void Init();

#if wxUSE_MENUS
    void RemoveWindowMenu(wxMenuBar *pMenuBar);
    void AddWindowMenu(wxMenuBar *pMenuBar);

    void DoHandleMenu(wxCommandEvent &event);
#endif // wxUSE_MENUS

    virtual void DoGetClientSize(int *width, int *height) const;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxTabMDIParentFrame)
};

//-----------------------------------------------------------------------------
// wxTabMDIChildFrame
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_AUI wxTabMDIChildFrame : public wxPanel
{
public:
    wxTabMDIChildFrame();
    wxTabMDIChildFrame(wxTabMDIParentFrame *parent,
                       wxWindowID winid,
                       const wxString& title,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = wxDEFAULT_FRAME_STYLE,
                       const wxString& name = wxFrameNameStr);

    virtual ~wxTabMDIChildFrame();
    bool Create(wxTabMDIParentFrame *parent,
                wxWindowID winid,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);

#if wxUSE_MENUS
    virtual void SetMenuBar(wxMenuBar *menu_bar);
    virtual wxMenuBar *GetMenuBar() const;
#endif // wxUSE_MENUS

    virtual void SetTitle(const wxString& title);
    virtual wxString GetTitle() const;

    virtual void Activate();
    virtual bool Destroy();

#if wxUSE_STATUSBAR
    // no status bars
    virtual wxStatusBar* CreateStatusBar(int WXUNUSED(number) = 1,
                                         long WXUNUSED(style) = 1,
                                         wxWindowID WXUNUSED(winid) = 1,
                                         const wxString& WXUNUSED(name) = wxEmptyString)
      { return (wxStatusBar*)NULL; }

    virtual wxStatusBar *GetStatusBar() const { return (wxStatusBar*)NULL; }
    virtual void SetStatusText( const wxString &WXUNUSED(text), int WXUNUSED(number)=0 ) {}
    virtual void SetStatusWidths( int WXUNUSED(n), const int WXUNUSED(widths_field)[] ) {}
#endif

    // no size hints
    virtual void DoSetSizeHints(int WXUNUSED(minW),
                                int WXUNUSED(minH),
                                int WXUNUSED(maxW) = wxDefaultCoord,
                                int WXUNUSED(maxH) = wxDefaultCoord,
                                int WXUNUSED(incW) = wxDefaultCoord,
                                int WXUNUSED(incH) = wxDefaultCoord) {}
#if wxUSE_TOOLBAR
    // no toolbar bars
    virtual wxToolBar* CreateToolBar(long WXUNUSED(style),
                                     wxWindowID WXUNUSED(winid),
                                     const wxString& WXUNUSED(name))
        { return (wxToolBar*)NULL; }
    virtual wxToolBar *GetToolBar() const { return (wxToolBar*)NULL; }
#endif

    // no icon
    void SetIcon(const wxIcon& WXUNUSED(icon)) { }
    void SetIcons(const wxIconBundle& WXUNUSED(icons)) { }

    // no maximize etc
    virtual void Maximize(bool WXUNUSED(maximize) = true) { /* Has no effect */ }
    virtual void Restore() { /* Has no effect */ }
    virtual void Iconize(bool WXUNUSED(iconize)  = true) { /* Has no effect */ }
    virtual bool IsMaximized() const { return true; }
    virtual bool IsIconized() const { return false; }
    virtual bool ShowFullScreen(bool WXUNUSED(show), long WXUNUSED(style)) { return false; }
    virtual bool IsFullScreen() const { return false; }

    virtual bool IsTopLevel() const { return false; }

    void OnMenuHighlight(wxMenuEvent& evt);
    void OnActivate(wxActivateEvent& evt);
    void OnCloseWindow(wxCloseEvent& evt);

    void SetMDIParentFrame(wxTabMDIParentFrame* parent);
    wxTabMDIParentFrame* GetMDIParentFrame() const;

protected:
    wxTabMDIParentFrame *m_pMDIParentFrame;
    wxRect            m_mdi_newrect;
    wxRect            m_mdi_currect;
    wxString          m_title;

#if wxUSE_MENUS
    wxMenuBar        *m_pMenuBar;
#endif // wxUSE_MENUS

protected:
    void Init();

    virtual bool Show(bool show = true);
    virtual void DoSetSize(int x, int y, int width, int height, int size_flags);
    virtual void DoMoveWindow(int x, int y, int width, int height);

public:
    // This function needs to be called when a size change is confirmed,
    // we needed this function to prevent anybody from the outside
    // changing the panel... it messes the UI layout when we would allow it.
    void ApplyMDIChildFrameRect();
    void DoShow(bool show);

private:
    DECLARE_DYNAMIC_CLASS(wxTabMDIChildFrame)
    DECLARE_EVENT_TABLE()

    friend class wxTabMDIClientWindow;
};

//-----------------------------------------------------------------------------
// wxTabMDIClientWindow
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_AUI wxTabMDIClientWindow : public wxAuiMultiNotebook
{
public:
    wxTabMDIClientWindow();
    wxTabMDIClientWindow(wxTabMDIParentFrame *parent, long style = 0);
    ~wxTabMDIClientWindow();

    virtual bool CreateClient(wxTabMDIParentFrame *parent,
                              long style = wxVSCROLL | wxHSCROLL);

    virtual int SetSelection(size_t page);

protected:

    void PageChanged(int old_selection, int new_selection);
    void OnPageChanged(wxAuiNotebookEvent& event);
    void OnSize(wxSizeEvent& evt);

private:
    DECLARE_DYNAMIC_CLASS(wxTabMDIClientWindow)
    DECLARE_EVENT_TABLE()
};
#endif // wxUSE_AUI

#endif // _WX_AUITABMDI_H_
