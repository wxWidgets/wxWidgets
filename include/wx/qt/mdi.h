/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/mdi.h
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_MDI_H_
#define _WX_QT_MDI_H_

class QMdiArea;
class QMdiSubWindow;

class WXDLLIMPEXP_CORE wxMDIParentFrame : public wxMDIParentFrameBase
{
public:
    wxMDIParentFrame() = default;

    wxMDIParentFrame(wxWindow *parent,
                     wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                     const wxString& name = wxASCII_STR(wxFrameNameStr));

    ~wxMDIParentFrame();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                const wxString& name = wxASCII_STR(wxFrameNameStr));

    QMdiArea* GetQtMdiArea() const;

    enum class Layout
    {
        MDI,
        Tabbed
    };

    void QtSetPreferredDILayout(Layout layout);

    // override/implement base class [pure] virtual methods
    // ----------------------------------------------------

    // The default is to return what wxMDIParentFrame::IsTDI() is supposed to
    // return under the target platform. i.e. wxMSW and wxOSX return false,
    // while wxGTK returns true. Use QtSetPreferredDILayout() to change that.
    static bool IsTDI() { return ms_layout == Layout::Tabbed; }

    virtual void Cascade() override;
    virtual void Tile(wxOrientation orient = wxHORIZONTAL) override;
    virtual void ActivateNext() override;
    virtual void ActivatePrevious() override;

    // implementation only from now on

    // MDI helpers
    // -----------

#if wxUSE_MENUS
    // called by wxMDIChildFrame after it was successfully created
    void AddMDIChild(wxMDIChildFrame* child);

    // called by wxMDIChildFrame just before it is destroyed
    void RemoveMDIChild(wxMDIChildFrame* child);
#endif // wxUSE_MENUS

private:
    void OnMDICommand(wxCommandEvent& event);

    // add/remove window menu if we have it (i.e. m_windowMenu != nullptr)
    void AddWindowMenu();
    void RemoveWindowMenu();

    // update the window menu (if we have it) to enable or disable the commands
    // which only make sense when we have more than one child
    void UpdateWindowMenu(bool enable);

    // return the number of child frames we currently have (maybe 0)
    int GetChildFramesCount() const;

    // TDI=true, MDI=false
    // Default to false under Windows, true otherwise.
    static Layout ms_layout;

    wxDECLARE_DYNAMIC_CLASS(wxMDIParentFrame);
};



class WXDLLIMPEXP_CORE wxMDIChildFrame : public wxMDIChildFrameBase
{
public:
    wxMDIChildFrame() = default;

    wxMDIChildFrame(wxMDIParentFrame *parent,
                    wxWindowID id,
                    const wxString& title,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxDEFAULT_FRAME_STYLE,
                    const wxString& name = wxASCII_STR(wxFrameNameStr));

    ~wxMDIChildFrame();

    bool Create(wxMDIParentFrame *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxASCII_STR(wxFrameNameStr));

    virtual void Activate() override;

    virtual void SetMenuBar(wxMenuBar* menubar) override;

    // This function is responsible for attaching/detaching this frame's menubar
    // to m_mdiParent. i.e. the menubar is attached when this frame becomes active
    // Otherwise, it will be detached and the m_mdiParent's menubar will be restored.
    void InternalSetMenuBar();

    // wxMDIChildFrame doesn't have toolbar nor statusbar
    // --------------------------------------------------

#if wxUSE_STATUSBAR
    virtual void SetStatusBar(wxStatusBar* WXUNUSED(statusBar)) override {}
#endif // wxUSE_STATUSBAR

#if wxUSE_TOOLBAR
    virtual void SetToolBar(wxToolBar* WXUNUSED(toolbar)) override {}
#endif // wxUSE_TOOLBAR

    virtual void SetWindowStyleFlag( long style ) override;

private:
    void AttachWindowMenuTo(wxMenuBar* attachedMenuBar, wxMenuBar* detachedMenuBar);

    wxMenuBar* m_menuBar = nullptr;

    QMdiSubWindow* m_qtSubWindow = nullptr;

    wxDECLARE_DYNAMIC_CLASS(wxMDIChildFrame);
};



class WXDLLIMPEXP_CORE wxMDIClientWindow : public wxMDIClientWindowBase
{
public:
    wxMDIClientWindow() = default;

    virtual bool CreateClient(wxMDIParentFrame *parent, long style = wxVSCROLL | wxHSCROLL) override;

    wxDECLARE_DYNAMIC_CLASS(wxMDIClientWindow);
};

#endif // _WX_QT_MDI_H_
