/////////////////////////////////////////////////////////////////////////////
// Name:        dcclient.h
// Purpose:
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_DCCLIENT_H__
#define __WX_DCCLIENT_H__

#ifdef __GNUG__
#pragma interface "dcclient.h"
#endif

#include "wx/dc.h"
#include "wx/window.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxWindowDC;
class WXDLLEXPORT wxPaintDC;
class WXDLLEXPORT wxClientDC;

//-----------------------------------------------------------------------------
// wxWindowDC
//-----------------------------------------------------------------------------

//FIXME_MGL
class WXDLLEXPORT wxWindowDC : public wxDC
{
public:
    wxWindowDC() {}
    wxWindowDC( wxWindow *win ) {}

private:
    DECLARE_DYNAMIC_CLASS(wxWindowDC)
};

//-----------------------------------------------------------------------------
// wxClientDC
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxClientDC : public wxWindowDC
{
public:
    wxClientDC() {}
    wxClientDC( wxWindow *win ) {}

private:
    DECLARE_DYNAMIC_CLASS(wxClientDC)
};

//-----------------------------------------------------------------------------
// wxPaintDC
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxPaintDC : public wxClientDC
{
public:
    wxPaintDC() { }
    wxPaintDC( wxWindow *win ) {}

private:
    DECLARE_DYNAMIC_CLASS(wxPaintDC)
};

#endif // __WX_DCCLIENT_H__
