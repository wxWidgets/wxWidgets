/////////////////////////////////////////////////////////////////////////////
// Name:        dcclient.h
// Purpose:     wxClientDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __DCCLIENTH__
#define __DCCLIENTH__

#ifdef __GNUG__
#pragma interface "dcclient.h"
#endif

#include "wx/dc.h"

class WXDLLEXPORT wxClientDC: public wxDC
{
  DECLARE_DYNAMIC_CLASS(wxClientDC)

 public:
  wxClientDC(void);

  // Create a DC corresponding to a canvas
  wxClientDC(wxWindow *win);

  ~wxClientDC(void);
};

class WXDLLEXPORT wxWindowDC: public wxDC
{
  DECLARE_DYNAMIC_CLASS(wxWindowDC)

 public:
  wxWindowDC(void);

  // Create a DC corresponding to a canvas
  wxWindowDC(wxWindow *win);

  ~wxWindowDC(void);
};

class WXDLLEXPORT wxPaintDC: public wxDC
{
  DECLARE_DYNAMIC_CLASS(wxPaintDC)

 public:
  wxPaintDC(void);

  // Create a DC corresponding to a canvas
  wxPaintDC(wxWindow *win);

  ~wxPaintDC(void);

 protected:
    static WXHDC ms_PaintHDC;
    static uint  ms_PaintCount;
};

#endif
    // __DCCLIENTH__
