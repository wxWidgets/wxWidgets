/////////////////////////////////////////////////////////////////////////////
// Name:        dcscreen.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKDCSCREENH__
#define __GTKDCSCREENH__

#include "wx/dcclient.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxScreenDC;

//-----------------------------------------------------------------------------
// wxScreenDC
//-----------------------------------------------------------------------------

class wxScreenDC: public wxPaintDC
{
  DECLARE_DYNAMIC_CLASS(wxScreenDC)

public:
  wxScreenDC();
  ~wxScreenDC();

  static bool StartDrawingOnTop( wxWindow *window );
  static bool StartDrawingOnTop( wxRect *rect = (wxRect *) NULL );
  static bool EndDrawingOnTop();

  // implementation

  static GdkWindow  *sm_overlayWindow;
  static int         sm_overlayWindowX;
  static int         sm_overlayWindowY;
};

#endif

    // __GTKDCSCREENH__

