/////////////////////////////////////////////////////////////////////////////
// Name:        dcclient.h
// Purpose:     wxClientDC, wxPaintDC and wxWindowDC classes
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

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxPaintDC;
class WXDLLEXPORT wxWindow;

class WXDLLEXPORT wxWindowDC: public wxDC
{
  DECLARE_DYNAMIC_CLASS(wxWindowDC)

 public:
  wxWindowDC(void);

  // Create a DC corresponding to a canvas
  wxWindowDC(wxWindow *win);

  ~wxWindowDC(void);
};


class WXDLLEXPORT wxClientDC: public wxWindowDC
{
  DECLARE_DYNAMIC_CLASS(wxClientDC)

 public:
  wxClientDC(void);

  // Create a DC corresponding to a canvas
  wxClientDC(wxWindow *win);

  ~wxClientDC(void);
};

class WXDLLEXPORT wxPaintDC: public wxWindowDC
{
  DECLARE_DYNAMIC_CLASS(wxPaintDC)

 public:
  wxPaintDC(void);

  // Create a DC corresponding to a canvas
  wxPaintDC(wxWindow *win);

  ~wxPaintDC(void);
};

#endif
    // _WX_DCCLIENT_H_
