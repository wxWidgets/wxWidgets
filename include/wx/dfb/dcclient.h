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
    wxWindowDC() {}
    wxWindowDC(wxWindow *win);

protected:
    void InitForWin(wxWindow *win);

    DECLARE_DYNAMIC_CLASS(wxWindowDC)
    DECLARE_NO_COPY_CLASS(wxWindowDC)
};

//-----------------------------------------------------------------------------
// base class for wxClientDC and wxPaintDC
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxClientDCBase : public wxWindowDC
{
public:
    wxClientDCBase() {}
    wxClientDCBase(wxWindow *win);
};

//-----------------------------------------------------------------------------
// wxClientDC
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxClientDC : public wxClientDCBase
{
public:
    wxClientDC() {}
    wxClientDC(wxWindow *win) : wxClientDCBase(win) {}
    ~wxClientDC();

    DECLARE_DYNAMIC_CLASS(wxClientDC)
    DECLARE_NO_COPY_CLASS(wxClientDC)
};


//-----------------------------------------------------------------------------
// wxPaintDC
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxPaintDC : public wxClientDCBase
{
public:
    wxPaintDC() {}
    wxPaintDC(wxWindow *win) : wxClientDCBase(win) {}
    ~wxPaintDC();

    DECLARE_DYNAMIC_CLASS(wxPaintDC)
    DECLARE_NO_COPY_CLASS(wxPaintDC)
};

#endif // _WX_DFB_DCCLIENT_H_
