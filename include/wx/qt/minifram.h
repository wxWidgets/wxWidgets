/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/minifram.h
// Purpose:     wxMiniFrame class
// Author:      Mariano Reingart
// Copyright:   (c) 2014 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MINIFRAM_H_
#define _WX_MINIFRAM_H_

#include "wx/frame.h"

class WXDLLIMPEXP_CORE wxMiniFrame : public wxFrame
{
public:
  wxMiniFrame() { }

  bool Create(wxWindow *parent,
              wxWindowID id,
              const wxString& title,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxCAPTION | wxCLIP_CHILDREN | wxRESIZE_BORDER,
              const wxString& name = wxFrameNameStr)
  {
      return wxFrame::Create(parent, id, title, pos, size,
                             style | wxFRAME_TOOL_WINDOW | wxFRAME_NO_TASKBAR, 
                             name);
  }

  wxMiniFrame(wxWindow *parent,
              wxWindowID id,
              const wxString& title,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxCAPTION | wxCLIP_CHILDREN | wxRESIZE_BORDER,
              const wxString& name = wxFrameNameStr)
  {
      Create(parent, id, title, pos, size, style, name);
  }

protected:
  wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxMiniFrame);
};

#endif
    // _WX_MINIFRAM_H_
