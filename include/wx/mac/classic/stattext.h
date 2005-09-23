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

class WXDLLEXPORT wxStaticText: public wxStaticTextBase
{
  DECLARE_DYNAMIC_CLASS(wxStaticText)
 public:
  inline wxStaticText() : m_label() { }

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
  void SetLabel( const wxString &str ) ;
  bool SetFont( const wxFont &font );

  // operations
  virtual void Command(wxCommandEvent& WXUNUSED(event)) {};
  virtual void ProcessCommand(wxCommandEvent& WXUNUSED(event)) {};
  // events
  void DrawParagraph(wxDC &dc, wxString paragraph, int &y);
  void OnPaint( wxPaintEvent &event ) ;
  void OnDraw( wxDC &dc ) ;
  virtual wxSize DoGetBestSize() const ;
  virtual bool AcceptsFocus() const { return FALSE; }
 private :
     wxString    m_label ;

   DECLARE_EVENT_TABLE()
};

#endif
    // _WX_STATTEXT_H_
