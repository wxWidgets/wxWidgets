/////////////////////////////////////////////////////////////////////////////
// Name:        mdi.h
// Purpose:     MDI (Multiple Document Interface) classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MDI_H_
#define _WX_MDI_H_

#ifdef __GNUG__
    #pragma interface "mdi.h"
#endif

#include "wx/frame.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxFrameNameStr;
WXDLLEXPORT_DATA(extern const wxChar*) wxStatusLineNameStr;

class WXDLLEXPORT wxMDIClientWindow;
class WXDLLEXPORT wxMDIChildFrame;

// ---------------------------------------------------------------------------
// wxMDIParentFrame
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxMDIParentFrame : public wxFrame
{
    DECLARE_DYNAMIC_CLASS(wxMDIParentFrame)

public:
    wxMDIParentFrame();
    wxMDIParentFrame(wxWindow *parent,
                     wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                     const wxString& name = wxFrameNameStr)
    {
        Create(parent, id, title, pos, size, style, name);
    }

    ~wxMDIParentFrame();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                const wxString& name = wxFrameNameStr);

    // accessors
    // ---------

    // Get the active MDI child window (Windows only)
    wxMDIChildFrame *GetActiveChild() const;

    // Get the client window
    wxMDIClientWindow *GetClientWindow() const { return m_clientWindow; }

    // Create the client window class (don't Create the window,
    // just return a new class)
    virtual wxMDIClientWindow *OnCreateClient(void);

    WXHMENU GetWindowMenu() const { return m_windowMenu; }

    // MDI operations
    // --------------
    virtual void Cascade();
    virtual void Tile();
    virtual void ArrangeIcons();
    virtual void ActivateNext();
    virtual void ActivatePrevious();

    // handlers
    // --------

    // Responds to colour changes
    void OnSysColourChanged(wxSysColourChangedEvent& event);

    void OnSize(wxSizeEvent& event);

    bool HandleActivate(int state, bool minimized, WXHWND activate);
    bool HandleCommand(WXWORD id, WXWORD cmd, WXHWND control);

    // override window proc for MDI-specific message processing
    virtual long MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

    virtual long MSWDefWindowProc(WXUINT, WXWPARAM, WXLPARAM);
    virtual bool MSWTranslateMessage(WXMSG* msg);

protected:
    virtual void InternalSetMenuBar();

    wxMDIClientWindow *             m_clientWindow;
    wxMDIChildFrame *               m_currentChild;
    WXHMENU                         m_windowMenu;

    // TRUE if MDI Frame is intercepting commands, not child
    bool m_parentFrameActive;

private:
    friend class WXDLLEXPORT wxMDIChildFrame;

    DECLARE_EVENT_TABLE()
};

// ---------------------------------------------------------------------------
// wxMDIChildFrame
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxMDIChildFrame : public wxFrame
{
    DECLARE_DYNAMIC_CLASS(wxMDIChildFrame)

public:
    wxMDIChildFrame();
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

    ~wxMDIChildFrame();

    bool Create(wxMDIParentFrame *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr);

    // MDI operations
    virtual void Maximize(bool maximize = TRUE);
    virtual void Restore();
    virtual void Activate();

    // Handlers

    bool HandleMDIActivate(long bActivate, WXHWND, WXHWND);
    bool HandleSize(int x, int y, WXUINT);
    bool HandleWindowPosChanging(void *lpPos);
    bool HandleCommand(WXWORD id, WXWORD cmd, WXHWND control);

    virtual long MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam);
    virtual long MSWDefWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam);
    virtual bool MSWTranslateMessage(WXMSG *msg);

    virtual void MSWDestroyWindow();

    // Implementation
    bool ResetWindowStyle(void *vrect);

protected:
    virtual void DoGetPosition(int *x, int *y) const;
    virtual void DoSetClientSize(int width, int height);
    virtual void InternalSetMenuBar();
};

// ---------------------------------------------------------------------------
// wxMDIClientWindow
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxMDIClientWindow : public wxWindow
{
    DECLARE_DYNAMIC_CLASS(wxMDIClientWindow)

public:
    wxMDIClientWindow() { Init(); }
    wxMDIClientWindow(wxMDIParentFrame *parent, long style = 0)
    {
        Init();

        CreateClient(parent, style);
    }

    // Note: this is virtual, to allow overridden behaviour.
    virtual bool CreateClient(wxMDIParentFrame *parent,
                              long style = wxVSCROLL | wxHSCROLL);

    // Explicitly call default scroll behaviour
    void OnScroll(wxScrollEvent& event);

protected:
    void Init() { m_scrollX = m_scrollY = 0; }

    int m_scrollX, m_scrollY;

private:
    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_MDI_H_
