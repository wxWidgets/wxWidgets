/////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/mdi.h
// Purpose:     wxMDIParentFrame, wxMDIChildFrame, wxMDIClientWindow
// Author:      David Elliott
// Modified by:
// Created:     2003/09/08
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_COCOA_MDI_H__
#define __WX_COCOA_MDI_H__

#include "wx/frame.h"

DECLARE_WXCOCOA_OBJC_CLASS(wxMDIParentFrameObserver);

class WXDLLEXPORT wxMDIChildFrame;
class WXDLLEXPORT wxMDIClientWindow;

WX_DECLARE_LIST(wxMDIChildFrame, wxCocoaMDIChildFrameList);

// ========================================================================
// wxMDIParentFrame
// ========================================================================
class WXDLLEXPORT wxMDIParentFrame: public wxFrame
{
    friend class WXDLLEXPORT wxMDIChildFrame;
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxMDIParentFrame)
// ------------------------------------------------------------------------
// initialization
// ------------------------------------------------------------------------
public:
    wxMDIParentFrame() { Init(); }
    wxMDIParentFrame(wxWindow *parent,
            wxWindowID winid,
            const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE,
            const wxString& name = wxFrameNameStr)
    {
        Init();
        Create(parent, winid, title, pos, size, style, name);
    }

    virtual ~wxMDIParentFrame();

    bool Create(wxWindow *parent,
                wxWindowID winid,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);
protected:
    void Init();
// ------------------------------------------------------------------------
// Cocoa specifics
// ------------------------------------------------------------------------
public:
    void WindowDidBecomeMain(NSNotification *notification);
protected:
    virtual void CocoaDelegate_windowDidBecomeKey(void);
    virtual void CocoaDelegate_windowDidResignKey(void);
    virtual bool Cocoa_canBecomeMainWindow(bool &canBecome);
    virtual wxMenuBar* GetAppMenuBar(wxCocoaNSWindow *win);

    void AddMDIChild(wxMDIChildFrame *child);
    void RemoveMDIChild(wxMDIChildFrame *child);

    wxMDIParentFrameObserver *m_observer;
// ------------------------------------------------------------------------
// Implementation
// ------------------------------------------------------------------------
public:
    wxMDIChildFrame *GetActiveChild() const;
    void SetActiveChild(wxMDIChildFrame *child);

    wxMDIClientWindow *GetClientWindow() const;
    virtual wxMDIClientWindow *OnCreateClient();

    virtual void Cascade() {}
    virtual void Tile(wxOrientation WXUNUSED(orient) = wxHORIZONTAL) {}
    virtual void ArrangeIcons() {}
    virtual void ActivateNext();
    virtual void ActivatePrevious();
protected:
    wxMDIClientWindow *m_clientWindow;
    wxMDIChildFrame *m_currentChild;
    wxCocoaMDIChildFrameList m_mdiChildren;
};

// ========================================================================
// wxMDIChildFrame
// ========================================================================
class WXDLLEXPORT wxMDIChildFrame: public wxFrame
{
    friend class WXDLLEXPORT wxMDIParentFrame;
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxMDIChildFrame)
// ------------------------------------------------------------------------
// initialization
// ------------------------------------------------------------------------
public:
    wxMDIChildFrame() { Init(); }
    wxMDIChildFrame(wxMDIParentFrame *parent,
            wxWindowID winid,
            const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE,
            const wxString& name = wxFrameNameStr)
    {
        Init();
        Create(parent, winid, title, pos, size, style, name);
    }

    virtual ~wxMDIChildFrame();

    bool Create(wxMDIParentFrame *parent,
                wxWindowID winid,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);
protected:
    void Init();
// ------------------------------------------------------------------------
// Cocoa specifics
// ------------------------------------------------------------------------
public:
protected:
    virtual void CocoaDelegate_windowDidBecomeKey(void);
    virtual void CocoaDelegate_windowDidBecomeMain(void);
    virtual void CocoaDelegate_windowDidResignKey(void);
// ------------------------------------------------------------------------
// Implementation
// ------------------------------------------------------------------------
public:
    virtual void Activate();
    virtual bool Destroy();
protected:
    wxMDIParentFrame *m_mdiParent;
};

// ========================================================================
// wxMDIClientWindow
// ========================================================================
class wxMDIClientWindow: public wxWindow
{
    DECLARE_DYNAMIC_CLASS(wxMDIClientWindow)
public:
    wxMDIClientWindow();
    wxMDIClientWindow( wxMDIParentFrame *parent, long style = 0 );
    virtual ~wxMDIClientWindow();
    virtual bool CreateClient( wxMDIParentFrame *parent, long style = 0 );
};

#endif // __WX_COCOA_MDI_H__
