/////////////////////////////////////////////////////////////////////////////
// Name:        dcscreen.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKDCSCREENH__
#define __GTKDCSCREENH__

#include "wx/dcclient.h"

class WXDLLEXPORT wxScreenDC: public wxPaintDC
{
  DECLARE_DYNAMIC_CLASS(wxScreenDC)

 public:
  wxScreenDC(void);
  ~wxScreenDC(void);

  static bool StartDrawingOnTop( wxWindow *window );
  static bool StartDrawingOnTop( wxRectangle *rect = NULL );
  static bool EndDrawingOnTop(void);
};

#endif
    // __GTKDCSCREENH__

