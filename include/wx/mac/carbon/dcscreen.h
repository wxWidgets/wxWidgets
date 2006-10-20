/////////////////////////////////////////////////////////////////////////////
// Name:        dcscreen.h
// Purpose:     wxScreenDC class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DCSCREEN_H_
#define _WX_DCSCREEN_H_

#include "wx/dcclient.h"

class WXDLLEXPORT wxScreenDC: public wxWindowDC
{
  DECLARE_DYNAMIC_CLASS(wxScreenDC)

 public:
  // Create a DC representing the whole screen
  wxScreenDC();
  virtual ~wxScreenDC();

  // Compatibility with X's requirements for
  // drawing on top of all windows
  static bool StartDrawingOnTop(wxWindow* WXUNUSED(window)) { return TRUE; }
  static bool StartDrawingOnTop(wxRect* WXUNUSED(rect) = NULL) { return TRUE; }
  static bool EndDrawingOnTop() { return TRUE; }
 private:
#if wxMAC_USE_CORE_GRAPHICS
	wxUint32 m_displayId;
#endif
};

#endif
    // _WX_DCSCREEN_H_

