/////////////////////////////////////////////////////////////////////////////
// Name:        minifram.h
// Purpose:     wxMiniFrame class. A small frame for e.g. floating toolbars.
//              If there is no equivalent on your platform, just make it a
//              normal frame.
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MINIFRAM_H_
#define _WX_MINIFRAM_H_

#ifdef __GNUG__
#pragma interface "minifram.h"
#endif

#include "wx/frame.h"

class WXDLLEXPORT wxMiniFrame: public wxFrame {

  DECLARE_DYNAMIC_CLASS(wxMiniFrame)

public:
  inline wxMiniFrame() {}
  inline wxMiniFrame(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxDEFAULT_FRAME_STYLE|wxTINY_CAPTION_HORIZ,
           const wxString& name = wxFrameNameStr)
  {
      Create(parent, id, title, pos, size, style, name);
  }

  ~wxMiniFrame();
protected:
};

#endif
    // _WX_MINIFRAM_H_
