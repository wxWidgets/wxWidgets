/////////////////////////////////////////////////////////////////////////////
// Name:        statbmp.h
// Purpose:     wxStaticBitmap class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __STATBMPH__
#define __STATBMPH__

#ifdef __GNUG__
#pragma interface "statbmp.h"
#endif

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxStaticBitmapNameStr;

class WXDLLEXPORT wxStaticBitmap: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxStaticBitmap)
 public:
  inline wxStaticBitmap(void) { }

  inline wxStaticBitmap(wxWindow *parent, const wxWindowID id,
           const wxBitmap& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           const long style = 0,
           const wxString& name = wxStaticBitmapNameStr)
  {
    Create(parent, id, label, pos, size, style, name);
  }

  bool Create(wxWindow *parent, const wxWindowID id,
           const wxBitmap& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           const long style = 0,
           const wxString& name = wxStaticBitmapNameStr);

  virtual void SetBitmap(const wxBitmap& bitmap);

  virtual void Command(wxCommandEvent& WXUNUSED(event)) {};
  virtual void ProcessCommand(wxCommandEvent& WXUNUSED(event)) {};

  void SetSize(const int x, const int y, const int width, const int height, const int sizeFlags = wxSIZE_AUTO);

  inline wxBitmap& GetBitmap(void) const { return (wxBitmap&) m_messageBitmap; }

  // Implementation
  virtual bool MSWOnDraw(WXDRAWITEMSTRUCT *item);
  virtual long MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
 protected:
  wxBitmap m_messageBitmap;

};

#endif
    // __STATBMPH__
