/////////////////////////////////////////////////////////////////////////////
// Name:        dcclient.h
// Purpose:     wxClientDC class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCCLIENT_H_
#define _WX_DCCLIENT_H_

#ifdef __GNUG__
#pragma interface "dcclient.h"
#endif

#include "wx/dc.h"

class WXDLLEXPORT wxClientDC: public wxDC
{
  DECLARE_DYNAMIC_CLASS(wxClientDC)

 public:
  wxClientDC();

  // Create a DC corresponding to a canvas
  wxClientDC(wxWindow *win);

  ~wxClientDC();
};

class WXDLLEXPORT wxWindowDC: public wxDC
{
  DECLARE_DYNAMIC_CLASS(wxWindowDC)

 public:
  wxWindowDC();

  // Create a DC corresponding to a canvas
  wxWindowDC(wxWindow *win);

  ~wxWindowDC();
};

class WXDLLEXPORT wxPaintDC: public wxDC
{
  DECLARE_DYNAMIC_CLASS(wxPaintDC)

 public:
  wxPaintDC();

  // Create a DC corresponding to a canvas
  wxPaintDC(wxWindow *win);

  ~wxPaintDC();
};

#endif
    // _WX_DCCLIENT_H_
