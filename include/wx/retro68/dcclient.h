/////////////////////////////////////////////////////////////////////////////
// Name:        wx/retro68/dcclient.h
// Purpose:     wxWindowDCImpl, wxClientDCImpl and wxPaintDCImpl
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RETRO68_DCCLIENT_H_
#define _WX_RETRO68_DCCLIENT_H_

#include "wx/retro68/dc.h"

class WXDLLIMPEXP_FWD_CORE wxWindow;

//-----------------------------------------------------------------------------
// wxWindowDCImpl
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxWindowDCImpl : public wxRetro68DCImpl
{
public:
    explicit wxWindowDCImpl(wxDC *owner) : wxRetro68DCImpl(owner) { }
    wxWindowDCImpl(wxDC *owner, wxWindow *win);

protected:
    // initialize the DC for drawing on the given part (in window coordinates)
    // of the window, or the entire window if rect is null
    void InitForWin(wxWindow *win, const wxRect *rect);

    wxDECLARE_DYNAMIC_CLASS(wxWindowDCImpl);
    wxDECLARE_NO_COPY_CLASS(wxWindowDCImpl);
};

//-----------------------------------------------------------------------------
// wxClientDCImpl
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxClientDCImpl : public wxWindowDCImpl
{
public:
    explicit wxClientDCImpl(wxDC *owner) : wxWindowDCImpl(owner) { }
    wxClientDCImpl(wxDC *owner, wxWindow *win);

    static bool
    CanBeUsedForDrawing(const wxWindow* WXUNUSED(window)) { return true; }

    wxDECLARE_DYNAMIC_CLASS(wxClientDCImpl);
    wxDECLARE_NO_COPY_CLASS(wxClientDCImpl);
};

//-----------------------------------------------------------------------------
// wxPaintDCImpl
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxPaintDCImpl : public wxClientDCImpl
{
public:
    explicit wxPaintDCImpl(wxDC *owner) : wxClientDCImpl(owner) { }
    wxPaintDCImpl(wxDC *owner, wxWindow *win) : wxClientDCImpl(owner, win) { }

    wxDECLARE_DYNAMIC_CLASS(wxPaintDCImpl);
    wxDECLARE_NO_COPY_CLASS(wxPaintDCImpl);
};

#endif // _WX_RETRO68_DCCLIENT_H_
