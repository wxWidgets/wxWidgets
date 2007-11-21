/////////////////////////////////////////////////////////////////////////////
// Name:        dcmemory.h
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

class WXDLLIMPEXP_CORE wxMemoryDC : public wxWindowDC, public wxMemoryDCBase
{
public:
    wxMemoryDC() { Init(); }
    wxMemoryDC(wxBitmap& bitmap) { Init(); SelectObject(bitmap); }
    wxMemoryDC( wxDC *dc ); // Create compatible DC
    virtual ~wxMemoryDC();

    // implementation
    wxBitmap  m_selected;

protected:
    virtual void DoGetSize( int *width, int *height ) const;
    virtual void DoSelect(const wxBitmap& bitmap);

private:
    void Init();

    DECLARE_DYNAMIC_CLASS(wxMemoryDC)
};

#endif
// _WX_DCMEMORY_H_
