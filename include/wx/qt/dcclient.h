/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/dcclient.h
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_DCCLIENT_H_
#define _WX_QT_DCCLIENT_H_

#include "wx/qt/dc.h"

#include <memory>

class QPicture;

class WXDLLIMPEXP_CORE wxWindowDCImpl : public wxQtDCImpl
{
public:
    wxWindowDCImpl( wxDC *owner );
    wxWindowDCImpl( wxDC *owner, wxWindow *win );

    ~wxWindowDCImpl();

protected:
    wxWindow *m_window;

private:
    wxDECLARE_CLASS(wxWindowDCImpl);
    wxDECLARE_NO_COPY_CLASS(wxWindowDCImpl);
};


class WXDLLIMPEXP_CORE wxClientDCImpl : public wxWindowDCImpl
{
public:
    wxClientDCImpl( wxDC *owner );
    wxClientDCImpl( wxDC *owner, wxWindow *win );

    ~wxClientDCImpl();
private:
    std::unique_ptr<QPicture> m_pict;

    wxDECLARE_CLASS(wxClientDCImpl);
    wxDECLARE_NO_COPY_CLASS(wxClientDCImpl);
};


class WXDLLIMPEXP_CORE wxPaintDCImpl : public wxWindowDCImpl
{
public:
    wxPaintDCImpl( wxDC *owner );
    wxPaintDCImpl( wxDC *owner, wxWindow *win );
private:
    wxDECLARE_CLASS(wxPaintDCImpl);
    wxDECLARE_NO_COPY_CLASS(wxPaintDCImpl);
};

#endif // _WX_QT_DCCLIENT_H_
