/////////////////////////////////////////////////////////////////////////////
// Name:        statbmp.h
// Purpose:     wxStaticBitmap class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATBMP_H_
#define _WX_STATBMP_H_

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

  inline wxStaticBitmap(wxWindow *parent, wxWindowID id,
           const wxBitmap& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = wxStaticBitmapNameStr)
  {
    Create(parent, id, label, pos, size, style, name);
  }

  bool Create(wxWindow *parent, wxWindowID id,
           const wxBitmap& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = wxStaticBitmapNameStr);

  virtual void SetBitmap(const wxBitmap& bitmap);

  virtual void Command(wxCommandEvent& WXUNUSED(event)) {};
  virtual void ProcessCommand(wxCommandEvent& WXUNUSED(event)) {};

  void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
  void SetSize(const wxRect& rect, int sizeFlags = wxSIZE_AUTO)
    { wxWindow::SetSize(rect, sizeFlags); }
  void SetSize(const wxSize& size) { wxWindow::SetSize(size); }

  inline wxBitmap& GetBitmap(void) const { return (wxBitmap&) m_messageBitmap; }

  // overriden base class virtuals
  virtual bool AcceptsFocus() const { return FALSE; }

  // Implementation
  virtual bool MSWOnDraw(WXDRAWITEMSTRUCT *item);
  virtual long MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
 protected:
  wxBitmap m_messageBitmap;

};

#endif
    // _WX_STATBMP_H_
