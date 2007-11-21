/////////////////////////////////////////////////////////////////////////////
// Name:        dcmemory.h
// Purpose:     wxMemoryDC class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCMEMORY_H_
#define _WX_DCMEMORY_H_

#include "wx/dcclient.h"

class WXDLLEXPORT wxMemoryDC: public wxPaintDC, public wxMemoryDCBase
{
  DECLARE_DYNAMIC_CLASS(wxMemoryDC)

public:
    wxMemoryDC() { Init(); }
    wxMemoryDC(wxBitmap& bitmap) { Init(); SelectObject(bitmap); }
    wxMemoryDC( wxDC *dc ); // Create compatible DC
    virtual ~wxMemoryDC(void);

    const wxBitmap& GetSelectedBitmap() const { return m_selected; }
    wxBitmap    GetSelectedBitmap() { return m_selected; }

	wxBitmap    GetSelectedObject() { return GetSelectedBitmap() ; }

protected:
    virtual void DoGetSize( int *width, int *height ) const;
    virtual wxBitmap DoGetAsBitmap(const wxRect *subrect) const 
    { return subrect == NULL ? GetSelectedBitmap() : GetSelectedBitmap().GetSubBitmap(*subrect); }
    virtual void DoSelect(const wxBitmap& bitmap);

private:
    void Init();

    wxBitmap  m_selected;
};

#endif
    // _WX_DCMEMORY_H_
