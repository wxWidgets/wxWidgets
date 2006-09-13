/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dfb/dcclient.h
// Purpose:     wxWindowDC, wxClientDC and wxPaintDC
// Author:      Vaclav Slavik
// Created:     2006-08-10
// RCS-ID:      $Id$
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DFB_DCCLIENT_H_
#define _WX_DFB_DCCLIENT_H_

#include "wx/dc.h"

class WXDLLIMPEXP_CORE wxWindow;

//-----------------------------------------------------------------------------
// wxWindowDC
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxWindowDC : public wxDC
{
public:
    wxWindowDC() : m_win(NULL) {}
    wxWindowDC(wxWindow *win);
    virtual ~wxWindowDC();

protected:
    // initializes the DC for painting on given window; if rect!=NULL, then
    // for painting only on the given region of the window
    void InitForWin(wxWindow *win, const wxRect *rect);

private:
    wxWindow *m_win; // the window the DC paints on

    DECLARE_DYNAMIC_CLASS(wxWindowDC)
    DECLARE_NO_COPY_CLASS(wxWindowDC)
};

//-----------------------------------------------------------------------------
// wxClientDC
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxClientDC : public wxWindowDC
{
public:
    wxClientDC() {}
    wxClientDC(wxWindow *win);

    DECLARE_DYNAMIC_CLASS(wxClientDC)
    DECLARE_NO_COPY_CLASS(wxClientDC)
};


//-----------------------------------------------------------------------------
// wxPaintDC
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxPaintDC : public wxClientDC
{
public:
    wxPaintDC() {}
    wxPaintDC(wxWindow *win) : wxClientDC(win) {}

    DECLARE_DYNAMIC_CLASS(wxPaintDC)
    DECLARE_NO_COPY_CLASS(wxPaintDC)
};

#endif // _WX_DFB_DCCLIENT_H_
