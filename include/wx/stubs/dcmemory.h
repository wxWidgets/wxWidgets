/////////////////////////////////////////////////////////////////////////////
// Name:        dcmemory.h
// Purpose:     wxMemoryDC class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
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
  wxMemoryDC();
  wxMemoryDC(wxDC *dc); // Create compatible DC

  ~wxMemoryDC();
  virtual void SelectObject(const wxBitmap& bitmap);
  virtual void GetSize(int* width, int* height) const;
};

#endif
    // _WX_DCMEMORY_H_
