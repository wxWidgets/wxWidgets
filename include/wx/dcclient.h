/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dcclient.h
// Purpose:     wxClientDC base header
// Author:      Julian Smart
// Modified by:
// Created:
// Copyright:   (c) Julian Smart
// RCS-ID:      $Id$
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCCLIENT_H_BASE_
#define _WX_DCCLIENT_H_BASE_

#include "wx/dc.h"

//-----------------------------------------------------------------------------
// wxWindowDC
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxWindowDC : public wxDC
{
public:
    wxWindowDC();
    wxWindowDC( wxWindow *win );

private:
    DECLARE_DYNAMIC_CLASS(wxWindowDC)
};

//-----------------------------------------------------------------------------
// wxClientDC
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxClientDC : public wxWindowDC
{
public:
    wxClientDC();
    wxClientDC( wxWindow *win );

private:
    DECLARE_DYNAMIC_CLASS(wxClientDC)
};

//-----------------------------------------------------------------------------
// wxPaintDC
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxPaintDC : public wxClientDC
{
public:
    wxPaintDC();
    wxPaintDC( wxWindow *win );

private:
    DECLARE_DYNAMIC_CLASS(wxPaintDC)
};

#endif
    // _WX_DCCLIENT_H_BASE_
