/////////////////////////////////////////////////////////////////////////////
// Name:        dcclient.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id: dcclient.h 61724 2009-08-21 10:41:26Z VZ $
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCCLIENT_H_
#define _WX_DCCLIENT_H_

#include "wx/dc.h"
#include "wx/dcgraph.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_CORE wxPaintDC;
class WXDLLIMPEXP_FWD_CORE wxWindow;

class WXDLLIMPEXP_CORE wxWindowDCImpl: public wxGCDCImpl
{
public:
    wxWindowDCImpl( wxDC *owner );
    wxWindowDCImpl( wxDC *owner, wxWindow *window );
    virtual ~wxWindowDCImpl();

    virtual void DoGetSize( int *width, int *height ) const;
    virtual wxBitmap DoGetAsBitmap(const wxRect *subrect) const;

protected:
    bool m_release;
    int m_width;
    int m_height;

    DECLARE_CLASS(wxWindowDCImpl)
    wxDECLARE_NO_COPY_CLASS(wxWindowDCImpl);
};


class WXDLLIMPEXP_CORE wxClientDCImpl: public wxWindowDCImpl
{
public:
    wxClientDCImpl( wxDC *owner );
    wxClientDCImpl( wxDC *owner, wxWindow *window );
    virtual ~wxClientDCImpl();

private:
    DECLARE_CLASS(wxClientDCImpl)
    wxDECLARE_NO_COPY_CLASS(wxClientDCImpl);
};


class WXDLLIMPEXP_CORE wxPaintDCImpl: public wxWindowDCImpl
{
public:
    wxPaintDCImpl( wxDC *owner );
    wxPaintDCImpl( wxDC *owner, wxWindow *win );
    virtual ~wxPaintDCImpl();

protected:
    DECLARE_CLASS(wxPaintDCImpl)
    wxDECLARE_NO_COPY_CLASS(wxPaintDCImpl);
};


#endif
    // _WX_DCCLIENT_H_
