/////////////////////////////////////////////////////////////////////////////
// Name:        dcmemory.h
// Purpose:     wxMemoryDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCMEMORY_H_
#define _WX_DCMEMORY_H_

#ifdef __GNUG__
#pragma interface "dcmemory.h"
#endif

#include "wx/dcclient.h"

class WXDLLEXPORT wxMemoryDC: public wxDC
{
  DECLARE_DYNAMIC_CLASS(wxMemoryDC)

 public:
  wxMemoryDC(void);
  wxMemoryDC(wxDC *dc); // Create compatible DC

  ~wxMemoryDC(void);
  virtual void SelectObject(const wxBitmap& bitmap);
  virtual void GetSize(int* width, int* height) const;
};

#endif
    // _WX_DCMEMORY_H_
