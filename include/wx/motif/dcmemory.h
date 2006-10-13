/////////////////////////////////////////////////////////////////////////////
// Name:        wx/motif/dcmemory.h
// Purpose:     wxMemoryDC class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCMEMORY_H_
#define _WX_DCMEMORY_H_

#include "wx/dcclient.h"

class WXDLLIMPEXP_CORE wxMemoryDC : public wxWindowDC
{
    DECLARE_DYNAMIC_CLASS(wxMemoryDC)

public:
    wxMemoryDC( const wxBitmap& bitmap = wxNullBitmap );
    wxMemoryDC( wxDC *dc ); // Create compatible DC
    virtual ~wxMemoryDC();

    virtual void SelectObject( const wxBitmap& bitmap );

    void DoGetSize( int *width, int *height ) const;

    wxBitmap& GetBitmap() const { return (wxBitmap&) m_bitmap; }

private:
    friend class wxPaintDC;

    wxBitmap  m_bitmap;
};

#endif
// _WX_DCMEMORY_H_
