/////////////////////////////////////////////////////////////////////////////
// Name:        stattext.h
// Purpose:     wxStaticText class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATTEXT_H_
#define _WX_STATTEXT_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "stattext.h"
#endif

class WXDLLEXPORT wxStaticText: public wxStaticTextBase
{
public:
    wxStaticText() { }

    wxStaticText(wxWindow *parent, wxWindowID id,
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
  void SetLabel( const wxString &str ) ;
  bool SetFont( const wxFont &font );

protected :

  virtual wxSize DoGetBestSize() const ;
  virtual bool AcceptsFocus() const { return FALSE; }

    DECLARE_DYNAMIC_CLASS_NO_COPY(wxStaticText)
};

#endif
    // _WX_STATTEXT_H_
