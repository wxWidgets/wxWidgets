/////////////////////////////////////////////////////////////////////////////
// Name:        stattext.h
// Purpose:     wxStaticText class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATTEXT_H_
#define _WX_STATTEXT_H_

#ifdef __GNUG__
#pragma interface "stattext.h"
#endif

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxStaticTextNameStr;

class WXDLLEXPORT wxStaticText: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxStaticText)
 public:
  inline wxStaticText() { }

  inline wxStaticText(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = wxStaticTextNameStr)
  {
    Create(parent, id, label, pos, size, style, name);
  }

  bool Create(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = wxStaticTextNameStr);

  // accessors
  void SetLabel(const wxString& , bool resize );
  void SetLabel( const wxString &str ) { SetLabel( str , true ) ; }
  // operations
  virtual void Command(wxCommandEvent& WXUNUSED(event)) {};
  virtual void ProcessCommand(wxCommandEvent& WXUNUSED(event)) {};
  // events
  void OnPaint( wxPaintEvent &event ) ;
  wxSize DoGetBestSize() const ;
 private :
 	wxString	m_label ;

   DECLARE_EVENT_TABLE()
};

#endif
    // _WX_STATTEXT_H_
