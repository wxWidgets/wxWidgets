/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/dcclient.h
// Purpose:     wxClientDC, wxPaintDC and wxWindowDC classes
// Author:      Stefan Csomor
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
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

    virtual void DoGetSize( int *width, int *height ) const override;
    virtual wxBitmap DoGetAsBitmap(const wxRect *subrect) const override;
    virtual void DestroyClippingRegion() override;

protected:
#if WXWIN_COMPATIBILITY_3_2
    wxDEPRECATED_MSG("Don't use OSXGetOrigin()")
    virtual wxPoint OSXGetOrigin() const override;
#endif // WXWIN_COMPATIBILITY_3_2

    bool m_release;
    int m_width;
    int m_height;
    wxPoint m_origin;

    wxDECLARE_CLASS(wxWindowDCImpl);
    wxDECLARE_NO_COPY_CLASS(wxWindowDCImpl);
};


class WXDLLIMPEXP_CORE wxClientDCImpl: public wxWindowDCImpl
{
public:
    wxClientDCImpl( wxDC *owner );
    wxClientDCImpl( wxDC *owner, wxWindow *window );
    virtual ~wxClientDCImpl();

private:
    wxDECLARE_CLASS(wxClientDCImpl);
    wxDECLARE_NO_COPY_CLASS(wxClientDCImpl);
};


class WXDLLIMPEXP_CORE wxPaintDCImpl: public wxWindowDCImpl
{
public:
    wxPaintDCImpl( wxDC *owner );
    wxPaintDCImpl( wxDC *owner, wxWindow *win );
    virtual ~wxPaintDCImpl();

protected:
    wxDECLARE_CLASS(wxPaintDCImpl);
    wxDECLARE_NO_COPY_CLASS(wxPaintDCImpl);
};


#endif
    // _WX_DCCLIENT_H_
