/////////////////////////////////////////////////////////////////////////////
// Name:        dcmemory.h
// Purpose:
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKDCMEMORYH__
#define __GTKDCMEMORYH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/dcclient.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxMemoryDC;

//-----------------------------------------------------------------------------
// wxMemoryDC
//-----------------------------------------------------------------------------

class wxMemoryDC: public wxWindowDC
{
  DECLARE_DYNAMIC_CLASS(wxMemoryDC)

public:
  wxMemoryDC();
  wxMemoryDC( wxDC *dc ); // Create compatible DC
  ~wxMemoryDC();
  virtual void SelectObject( const wxBitmap& bitmap );
  void GetSize( int *width, int *height ) const;

  // implementation      
    
  wxBitmap  m_selected;
};

#endif
    // __GTKDCMEMORYH__

