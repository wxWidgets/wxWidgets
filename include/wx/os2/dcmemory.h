/////////////////////////////////////////////////////////////////////////////
// Name:        dcmemory.h
// Purpose:     wxMemoryDC class
// Author:      David Webster
// Modified by:
// Created:     09/09/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCMEMORY_H_
#define _WX_DCMEMORY_H_

#ifdef __GNUG__
#pragma interface "dcmemory.h"
#endif

#include "wx/dcclient.h"

class WXDLLEXPORT wxMemoryDC: public wxPaintDC
{
  DECLARE_DYNAMIC_CLASS(wxMemoryDC)

  public:
    wxMemoryDC(void);
    wxMemoryDC( wxDC *dc ); // Create compatible DC

    ~wxMemoryDC(void);
    virtual void SelectObject( const wxBitmap& bitmap );
    void GetSize( int *width, int *height ) const;

  private:
    friend wxPaintDC;
    wxBitmap  m_selected;
};

#endif
    // _WX_DCMEMORY_H_
