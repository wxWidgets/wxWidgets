/////////////////////////////////////////////////////////////////////////////
// Name:        dcscreen.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
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
  wxScreenDC(void);
  ~wxScreenDC(void);

  static bool StartDrawingOnTop( wxWindow *window );
  static bool StartDrawingOnTop( wxRectangle *rect = (wxRectangle *) NULL );
  static bool EndDrawingOnTop(void);
  
  // implementation
  
  static GdkWindow  *sm_overlayWindow;
  static int         sm_overlayWindowX;
  static int         sm_overlayWindowY;
};

#endif

    // __GTKDCSCREENH__

