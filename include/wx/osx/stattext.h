/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/stattext.h
// Purpose:     wxStaticText class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATTEXT_H_
#define _WX_STATTEXT_H_

class WXDLLIMPEXP_CORE wxStaticText: public wxStaticTextBase
{
public:
    wxStaticText() { }

    wxStaticText(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = wxASCII_STR(wxStaticTextNameStr))
  {
    Create(parent, id, label, pos, size, style, name);
  }

  bool Create(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = wxASCII_STR(wxStaticTextNameStr));

  // accessors
  void SetLabel( const wxString &str ) wxOVERRIDE;
  bool SetFont( const wxFont &font ) wxOVERRIDE;

    virtual bool AcceptsFocus() const wxOVERRIDE { return false; }

protected :

    virtual wxString WXGetVisibleLabel() const wxOVERRIDE;
    virtual void WXSetVisibleLabel(const wxString& str) wxOVERRIDE;

  virtual wxSize DoGetBestSize() const wxOVERRIDE;

#if wxUSE_MARKUP && wxOSX_USE_COCOA
    virtual bool DoSetLabelMarkup(const wxString& markup) wxOVERRIDE;
#endif // wxUSE_MARKUP && wxOSX_USE_COCOA

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxStaticText);
};

#endif
    // _WX_STATTEXT_H_
