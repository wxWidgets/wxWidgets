/////////////////////////////////////////////////////////////////////////////
// Name:        minifram.h
// Purpose:     wxMiniFrame class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __MINIFRAMH__
#define __MINIFRAMH__

#ifdef __GNUG__
#pragma interface "minifram.h"
#endif

#include "wx/frame.h"

class WXDLLEXPORT wxMiniFrame: public wxFrame {

  DECLARE_DYNAMIC_CLASS(wxMiniFrame)

public:
  inline wxMiniFrame(void) {}
  inline wxMiniFrame(wxWindow *parent,
           const wxWindowID id,
           const wxString& title,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           const long style = wxDEFAULT_FRAME|wxTINY_CAPTION_HORIZ,
           const wxString& name = wxFrameNameStr)
  {
      Create(parent, id, title, pos, size, style, name);
  }

  ~wxMiniFrame(void);

  long MSWDefWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

protected:
};

#endif
    // __MINIFRAMH__
