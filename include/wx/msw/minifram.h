/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/minifram.h
// Purpose:     wxMiniFrame class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MINIFRAM_H_
#define _WX_MINIFRAM_H_

#ifdef __GNUG__
#pragma interface "minifram.h"
#endif

#include "wx/frame.h"

#ifdef __WIN32__

class WXDLLEXPORT wxMiniFrame : public wxFrame
{
public:
  wxMiniFrame() { }
  wxMiniFrame(wxWindow *parent,
              wxWindowID id,
              const wxString& title,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxCAPTION | wxCLIP_CHILDREN | wxRESIZE_BORDER,
              const wxString& name = wxFrameNameStr)
  {
      Create(parent, id, title, pos, size, style | wxFRAME_TOOL_WINDOW | wxFRAME_FLOAT_ON_PARENT, name);
  }

protected:
  DECLARE_DYNAMIC_CLASS(wxMiniFrame)
};


#else // !Win32

class WXDLLEXPORT wxMiniFrame : public wxFrame
{
public:
  wxMiniFrame() { }
  wxMiniFrame(wxWindow *parent,
              wxWindowID id,
              const wxString& title,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxDEFAULT_FRAME_STYLE|wxTINY_CAPTION_HORIZ,
              const wxString& name = wxFrameNameStr)
  {
      Create(parent, id, title, pos, size, style, name);
  }

  virtual ~wxMiniFrame();

  virtual long MSWDefWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);

  DECLARE_DYNAMIC_CLASS(wxMiniFrame)
};

#endif // Win32/!Win32

#endif
    // _WX_MINIFRAM_H_
