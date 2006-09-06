/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dfb/toplevel.h
// Purpose:     Top level window, abstraction of wxFrame and wxDialog
// Author:      Vaclav Slavik
// Created:     2006-08-10
// RCS-ID:      $Id$
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DFB_TOPLEVEL_H_
#define _WX_DFB_TOPLEVEL_H_

#include "wx/dfb/dfbptr.h"

wxDFB_DECLARE_INTERFACE(IDirectFBWindow);

class wxDfbQueuedPaintRequests;
struct wxDFBWindowEvent;

//-----------------------------------------------------------------------------
// wxTopLevelWindowDFB
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxTopLevelWindowDFB : public wxTopLevelWindowBase
{
public:
    // construction
    wxTopLevelWindowDFB() { Init(); }
    wxTopLevelWindowDFB(wxWindow *parent,
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

    virtual ~wxTopLevelWindowDFB();

    // implement base class pure virtuals
    virtual void Maximize(bool maximize = true);
    virtual bool IsMaximized() const;
    virtual void Iconize(bool iconize = true);
    virtual bool IsIconized() const;
    virtual void Restore();

    virtual bool ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL);
    virtual bool IsFullScreen() const { return m_fsIsShowing; }

    virtual bool Show(bool show = true);

    virtual bool CanSetTransparent() { return true; }
    virtual bool SetTransparent(wxByte alpha);

    virtual void SetTitle(const wxString &title) { m_title = title; }
    virtual wxString GetTitle() const { return m_title; }

    virtual void Update();

    // implementation from now on
    // --------------------------

    void OnInternalIdle();

    wxIDirectFBWindowPtr GetDirectFBWindow() const { return m_dfbwin; }

protected:
    // common part of all ctors
    void Init();

    virtual wxIDirectFBSurfacePtr ObtainDfbSurface() const;

    // overriden wxWindow methods
    virtual void DoGetPosition(int *x, int *y) const;
    virtual void DoGetSize(int *width, int *height) const;
    virtual void DoMoveWindow(int x, int y, int width, int height);

    virtual void DoRefreshRect(const wxRect& rect, bool eraseBack = true);

private:
    // do queued painting in idle time
    void HandleQueuedPaintRequests();

    // DirectFB events handling
    static void HandleDFBWindowEvent(const wxDFBWindowEvent& event_);

protected:
    wxString      m_title;

    bool          m_fsIsShowing:1;         /* full screen */
    long          m_fsSaveStyle;
    long          m_fsSaveFlag;
    wxRect        m_fsSaveFrame;

    // is the frame currently maximized?
    bool          m_isMaximized:1;
    wxRect        m_savedFrame;

    // did we sent wxSizeEvent at least once?
    bool          m_sizeSet:1;

    // window's opacity (0: transparent, 255: opaque)
    wxByte        m_opacity;

    // interface to the underlying DirectFB window
    wxIDirectFBWindowPtr m_dfbwin;

private:
    wxDfbQueuedPaintRequests *m_toPaint;

    friend class wxEventLoop; // for HandleDFBWindowEvent
};

#endif // _WX_DFB_TOPLEVEL_H_
