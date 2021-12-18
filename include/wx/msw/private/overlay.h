/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/overlay.h
// Purpose:     wxOverlayImpl declaration
// Author:      Kettab Ali
// Created:     2021-07-12
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_OVERLAY_H_
#define _WX_MSW_PRIVATE_OVERLAY_H_

#include "wx/private/overlay.h"
#include "wx/timer.h"

class WXDLLIMPEXP_FWD_CORE wxWindow;
class WXDLLIMPEXP_FWD_CORE wxDC;

class wxOverlayMSWImpl : public wxOverlayImpl
{
public:
    wxOverlayMSWImpl();
    ~wxOverlayMSWImpl();

    virtual void Reset() wxOVERRIDE;
    virtual bool IsOk() wxOVERRIDE;
    virtual void InitFromDC(wxDC* dc, int x , int y , int width , int height) wxOVERRIDE;
    virtual void InitFromWindow(wxWindow* win, wxOverlay::Target target) wxOVERRIDE;
    virtual void BeginDrawing(wxDC* dc) wxOVERRIDE;
    virtual void EndDrawing(wxDC* dc) wxOVERRIDE;
    virtual void Clear(wxDC* dc) wxOVERRIDE;

    virtual void SetUpdateRectangle(const wxRect& rect) wxOVERRIDE;

    wxWindow* GetWindow() const { return m_window; }

public:
    void DoReset();

    void OnMoveStart(wxMoveEvent& event);
    void OnMoveEnd(wxMoveEvent& event);
    void OnResize(wxSizeEvent& event);
    void OnIconize(wxIconizeEvent& event);

    class OverlayTimer : public wxTimer
    {
    public:
        OverlayTimer(wxOverlayMSWImpl* owner) : m_owner(owner) { }

        virtual void Notify() wxOVERRIDE;

    private:
        wxOverlayMSWImpl* const m_owner;

    } m_timer;

    // window the overlay is associated with
    wxWindow* m_window;
    // the overlay window itself
    wxWindow* m_overlayWindow;
    // rectangle to be refreshed/updated within the overlay,
    // in m_window's window coordinates if not fullScreen.
    wxRect m_rect;
    wxRect m_oldRect;

    bool m_fullscreen;

    wxDECLARE_NO_COPY_CLASS(wxOverlayMSWImpl);
};

#endif // _WX_MSW_PRIVATE_OVERLAY_H_
