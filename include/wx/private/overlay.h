/////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/overlay.h
// Purpose:     wxOverlayImpl declaration
// Author:      Stefan Csomor
// Modified by:
// Created:     2006-10-20
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_OVERLAY_H_
#define _WX_PRIVATE_OVERLAY_H_

#include "wx/bitmap.h"

class wxOverlayImpl
{
public:
    wxOverlayImpl() { }
    virtual ~wxOverlayImpl() { }

    static wxOverlayImpl* Create();

    // wxOverlayGenericImpl is the only not native implementation
    // derived from this class.
    virtual bool IsNative() const { return true; }

    void Init(wxDC* dc, int x, int y, int width, int height)
    {
        InitFromDC(dc, x, y, width, height);
    }

    void Init(wxWindow* win, bool fullscreen)
    {
        InitFromWindow(win, fullscreen);
    }

    // returns true if it has been setup
    virtual bool IsOk() = 0;

    // clears the overlay without restoring the former state
    // to be done eg when the window content has been changed and repainted
    virtual void Reset() = 0;

    virtual void Clear(wxDC* dc) = 0;

    virtual void BeginDrawing(wxDC* WXUNUSED(dc)) { }

    virtual void EndDrawing(wxDC* WXUNUSED(dc)) { }

    virtual wxRect GetRect() const { return wxRect(); }

    virtual void SetUpdateRectangle(const wxRect& WXUNUSED(rect)) { }

    wxBitmap& GetBitmap() { return m_bitmap; }

protected:
    virtual void InitFromDC(wxDC* dc, int x, int y, int width, int height) = 0;
    virtual void InitFromWindow(wxWindow* win, bool fullscreen) = 0;

private:
    wxBitmap m_bitmap;

    wxDECLARE_NO_COPY_CLASS(wxOverlayImpl);
};

#endif // _WX_PRIVATE_OVERLAY_H_
