/////////////////////////////////////////////////////////////////////////////
// Name:        dcscreen.h
// Purpose:     wxScreenDC class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __DCSCREENH__
#define __DCSCREENH__

#ifdef __GNUG__
#pragma interface "dcscreen.h"
#endif

#include "wx/dc.h"

class WXDLLEXPORT wxScreenDC: public wxDC
{
  DECLARE_DYNAMIC_CLASS(wxScreenDC)

 public:
  // Create a DC representing the whole screen
  wxScreenDC(void);
  ~wxScreenDC(void);

  // Compatibility with X's requirements for
  // drawing on top of all windows
  static bool StartDrawingOnTop(wxWindow *window) { return TRUE; }
  static bool StartDrawingOnTop(wxRectangle *rect = NULL) { return TRUE; }
  static bool EndDrawingOnTop(void) { return TRUE; }
};

#endif
    // __DCSCREENH__

