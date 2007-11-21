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

    // Returns true if some invalidated area of the TLW is currently being
    // painted
    bool IsPainting() const { return m_isPainting; }

protected:
    // common part of all ctors
    void Init();

    virtual wxIDirectFBSurfacePtr ObtainDfbSurface() const;

    // overriden wxWindow methods
    virtual void DoGetPosition(int *x, int *y) const;
    virtual void DoGetSize(int *width, int *height) const;
    virtual void DoMoveWindow(int x, int y, int width, int height);

    virtual void DoRefreshRect(const wxRect& rect);

    // sets DirectFB keyboard focus to this toplevel window (note that DFB
    // focus is different from wx: only shown TLWs can have it and not any
    // wxWindows as in wx
    void SetDfbFocus();

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
    // invalidated areas of the TLW that need repainting
    wxDfbQueuedPaintRequests *m_toPaint;
    // are we currently painting some area of this TLW?
    bool m_isPainting;

    friend class wxEventLoop; // for HandleDFBWindowEvent
    friend class wxWindowDFB; // for SetDfbFocus
};

#endif // _WX_DFB_TOPLEVEL_H_
