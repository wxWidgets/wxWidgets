/////////////////////////////////////////////////////////////////////////////
// Name:        dcclient.h
// Purpose:
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_DCCLIENT_H__
#define __WX_DCCLIENT_H__

#ifdef __GNUG__
#pragma interface "dcclient.h"
#endif

#include "wx/dc.h"

class WXDLLEXPORT wxWindowMGL;

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxWindowDC;
class WXDLLEXPORT wxPaintDC;
class WXDLLEXPORT wxClientDC;

//-----------------------------------------------------------------------------
// wxWindowDC
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxWindowDC : public wxDC
{
public:
    wxWindowDC() {}
    virtual ~wxWindowDC();
    wxWindowDC(wxWindow *win);

protected:
    wxWindow *m_wnd;

private:
    DECLARE_DYNAMIC_CLASS(wxWindowDC)
};

//-----------------------------------------------------------------------------
// wxClientDC
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxClientDC : public wxWindowDC
{
public:
    wxClientDC() : wxWindowDC() {}
    wxClientDC(wxWindow *win);

private:
    wxWindowMGL *m_wnd;
    DECLARE_DYNAMIC_CLASS(wxClientDC)
};

//-----------------------------------------------------------------------------
// wxPaintDC
//-----------------------------------------------------------------------------

// FIXME_MGL
class WXDLLEXPORT wxPaintDC : public wxClientDC
{
public:
    wxPaintDC() { }
    wxPaintDC( wxWindow *win ) {}

private:
    DECLARE_DYNAMIC_CLASS(wxPaintDC)
};

#endif // __WX_DCCLIENT_H__
