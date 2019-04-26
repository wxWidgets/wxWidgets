/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/dcmemory.h
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_DCMEMORY_H_
#define _WX_QT_DCMEMORY_H_

#include "wx/qt/dcclient.h"

class WXDLLIMPEXP_CORE wxMemoryDCImpl : public wxQtDCImpl
{
public:
    wxMemoryDCImpl( wxMemoryDC *owner );
    wxMemoryDCImpl( wxMemoryDC *owner, wxBitmap& bitmap );
    wxMemoryDCImpl( wxMemoryDC *owner, wxDC *dc );
    ~wxMemoryDCImpl();

    virtual wxBitmap DoGetAsBitmap(const wxRect *subrect) const wxOVERRIDE;
    virtual void DoSelect(const wxBitmap& bitmap) wxOVERRIDE;

    virtual const wxBitmap& GetSelectedBitmap() const wxOVERRIDE;
    virtual wxBitmap& GetSelectedBitmap() wxOVERRIDE;

private:
    wxBitmap m_selected;

    DECLARE_CLASS(wxMemoryDCImpl);
    DECLARE_NO_COPY_CLASS(wxMemoryDCImpl);
};

#endif // _WX_QT_DCMEMORY_H_
