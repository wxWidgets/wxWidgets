/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/mdi.h
// Purpose:     MDI (Multiple Document Interface) classes
// Author:      Julian Smart
// Modified by: 2008-10-31 Vadim Zeitlin: derive from the base classes
// Created:     01/02/97
// Copyright:   (c) 1997 Julian Smart
//              (c) 2008 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_MDI_H_
#define _WX_MSW_MDI_H_

#include "wx/frame.h"

class WXDLLIMPEXP_FWD_CORE wxAcceleratorTable;

// ---------------------------------------------------------------------------
// wxMDIParentFrame
// ---------------------------------------------------------------------------

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
                     const wxString& name = wxASCII_STR(wxFrameNameStr))
    {
        Init();

        Create(parent, id, title, pos, size, style, name);
    }

    virtual ~wxMDIParentFrame();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                const wxString& name = wxASCII_STR(wxFrameNameStr));

    // override/implement base class [pure] virtual methods
    // ----------------------------------------------------

    static bool IsTDI() { return false; }

    // we don't store the active child in m_currentChild so override this
    // function to find it dynamically
    virtual wxMDIChildFrame *GetActiveChild() const wxOVERRIDE;

    virtual void Cascade() wxOVERRIDE;
    virtual void Tile(wxOrientation orient = wxHORIZONTAL) wxOVERRIDE;
    virtual void ArrangeIcons() wxOVERRIDE;
    virtual void ActivateNext() wxOVERRIDE;
    virtual void ActivatePrevious() wxOVERRIDE;

#if wxUSE_MENUS
    virtual void SetWindowMenu(wxMenu* menu) wxOVERRIDE;

    virtual void DoMenuUpdates(wxMenu* menu = NULL) wxOVERRIDE;

    // return the active child menu, if any
    virtual WXHMENU MSWGetActiveMenu() const wxOVERRIDE;
#endif // wxUSE_MENUS


    // implementation only from now on

    // MDI helpers
    // -----------

#if wxUSE_MENUS
    // called by wxMDIChildFrame after it was successfully created
    virtual void AddMDIChild(wxMDIChildFrame *child);

    // called by wxMDIChildFrame just before it is destroyed
    virtual void RemoveMDIChild(wxMDIChildFrame *child);
#endif // wxUSE_MENUS

    // Retrieve the current window menu label: it can be different from
    // "Window" when using non-English translations and can also be different
    // from wxGetTranslation("Window") if the locale has changed since the
    // "Window" menu was added.
    const wxString& MSWGetCurrentWindowMenuLabel() const
        { return m_currentWindowMenuLabel; }

    // handlers
    // --------

    // Responds to colour changes
    void OnSysColourChanged(wxSysColourChangedEvent& event);

    void OnActivate(wxActivateEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnIconized(wxIconizeEvent& event);

    bool HandleActivate(int state, bool minimized, WXHWND activate);

    // override window proc for MDI-specific message processing
    virtual WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam) wxOVERRIDE;

    virtual WXLRESULT MSWDefWindowProc(WXUINT, WXWPARAM, WXLPARAM) wxOVERRIDE;
    virtual bool MSWTranslateMessage(WXMSG* msg) wxOVERRIDE;

#if wxUSE_MENUS
    // override the menu-relayed methods to also look in the active child menu
    // bar and the "Window" menu
    virtual wxMenuItem *FindItemInMenuBar(int menuId) const wxOVERRIDE;
    virtual wxMenu* MSWFindMenuFromHMENU(WXHMENU hMenu) wxOVERRIDE;
#endif // wxUSE_MENUS

protected:
#if wxUSE_MENUS_NATIVE
    virtual void InternalSetMenuBar() wxOVERRIDE;
#endif // wxUSE_MENUS_NATIVE

    virtual WXHICON GetDefaultIcon() const wxOVERRIDE;

    // set the size of the MDI client window to match the frame size
    void UpdateClientSize();

private:
    // common part of all ctors
    void Init();

#if wxUSE_MENUS
    // "Window" menu commands event handlers
    void OnMDICommand(wxCommandEvent& event);
    void OnMDIChild(wxCommandEvent& event);


    // add/remove window menu if we have it (i.e. m_windowMenu != NULL)
    void AddWindowMenu();
    void RemoveWindowMenu();

    // update the window menu (if we have it) to enable or disable the commands
    // which only make sense when we have more than one child
    void UpdateWindowMenu(bool enable);

#if wxUSE_ACCEL
    wxAcceleratorTable *m_accelWindowMenu;
#endif // wxUSE_ACCEL
#endif // wxUSE_MENUS

    // return the number of child frames we currently have (maybe 0)
    int GetChildFramesCount() const;

    // if true, indicates whether the event wasn't really processed even though
    // it was "handled", see OnActivate() and HandleActivate()
    bool m_activationNotHandled;

    // holds the current translation for the window menu label
    wxString m_currentWindowMenuLabel;


    friend class WXDLLIMPEXP_FWD_CORE wxMDIChildFrame;

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS(wxMDIParentFrame);
    wxDECLARE_NO_COPY_CLASS(wxMDIParentFrame);
};

// ---------------------------------------------------------------------------
// wxMDIChildFrame
// ---------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMDIChildFrame : public wxMDIChildFrameBase
{
public:
    wxMDIChildFrame() { Init(); }
    wxMDIChildFrame(wxMDIParentFrame *parent,
                    wxWindowID id,
                    const wxString& title,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxDEFAULT_FRAME_STYLE,
                    const wxString& name = wxASCII_STR(wxFrameNameStr))
    {
        Init();

        Create(parent, id, title, pos, size, style, name);
    }

    bool Create(wxMDIParentFrame *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxASCII_STR(wxFrameNameStr));

    virtual ~wxMDIChildFrame();

    // implement MDI operations
    virtual void Activate() wxOVERRIDE;

    // Override some frame operations too
    virtual void Maximize(bool maximize = true) wxOVERRIDE;
    virtual void Restore() wxOVERRIDE;

    virtual bool Show(bool show = true) wxOVERRIDE;

    // Implementation only from now on
    // -------------------------------

    // Handlers
    bool HandleMDIActivate(long bActivate, WXHWND, WXHWND);
    bool HandleWindowPosChanging(void *lpPos);
    bool HandleGetMinMaxInfo(void *mmInfo);

    virtual WXLRESULT MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam) wxOVERRIDE;
    virtual WXLRESULT MSWDefWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam) wxOVERRIDE;
    virtual bool MSWTranslateMessage(WXMSG *msg) wxOVERRIDE;

    virtual void MSWDestroyWindow() wxOVERRIDE;

    bool ResetWindowStyle(void *vrect);

    void OnIdle(wxIdleEvent& event);

protected:
    virtual void DoGetScreenPosition(int *x, int *y) const wxOVERRIDE;
    virtual void DoGetPosition(int *x, int *y) const wxOVERRIDE;
    virtual void DoSetSize(int x, int y, int width, int height, int sizeFlags) wxOVERRIDE;
    virtual void DoSetClientSize(int width, int height) wxOVERRIDE;
    virtual void InternalSetMenuBar() wxOVERRIDE;
    virtual bool IsMDIChild() const wxOVERRIDE { return true; }
    virtual void DetachMenuBar() wxOVERRIDE;

    virtual WXHICON GetDefaultIcon() const wxOVERRIDE;

    // common part of all ctors
    void Init();

private:
    bool m_needsResize; // flag which tells us to artificially resize the frame

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxMDIChildFrame);
};

// ---------------------------------------------------------------------------
// wxMDIClientWindow
// ---------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMDIClientWindow : public wxMDIClientWindowBase
{
public:
    wxMDIClientWindow() { Init(); }

    // Note: this is virtual, to allow overridden behaviour.
    virtual bool CreateClient(wxMDIParentFrame *parent,
                              long style = wxVSCROLL | wxHSCROLL) wxOVERRIDE;

    // Explicitly call default scroll behaviour
    void OnScroll(wxScrollEvent& event);

protected:
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO) wxOVERRIDE;

    void Init() { m_scrollX = m_scrollY = 0; }

    int m_scrollX, m_scrollY;

private:
    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxMDIClientWindow);
};

#endif // _WX_MSW_MDI_H_
