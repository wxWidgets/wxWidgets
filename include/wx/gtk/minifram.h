/////////////////////////////////////////////////////////////////////////////
// Name:        minifram.h
// Purpose:     wxMiniFrame class
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKMINIFRAMEH__
#define __GTKMINIFRAMEH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/frame.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxMiniFrame;

//-----------------------------------------------------------------------------
// wxMiniFrame
//-----------------------------------------------------------------------------

class wxMiniFrame: public wxFrame 
{
  DECLARE_DYNAMIC_CLASS(wxMiniFrame)

public:
  inline wxMiniFrame(void) {}
  inline wxMiniFrame(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxDEFAULT_FRAME|wxTINY_CAPTION_HORIZ,
           const wxString& name = wxFrameNameStr)
  {
      Create(parent, id, title, pos, size, style, name);
  }
  bool Create(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxDEFAULT_FRAME|wxTINY_CAPTION_HORIZ,
           const wxString& name = wxFrameNameStr);
	   
  // implementation	   
	   
  bool   m_isDragging;
  int    m_oldX,m_oldY;
  int    m_diffX,m_diffY;
  
  void DrawFrame( int x, int y );
  void OnPaint( wxPaintEvent &event );
  void OnMouse( wxMouseEvent &event );

  DECLARE_EVENT_TABLE()
};

#endif
  //  __GTKMINIFRAMEH__
