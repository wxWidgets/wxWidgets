/////////////////////////////////////////////////////////////////////////////
// Name:        button.h
// Purpose:     wxButton class
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BUTTON_H_
#define _WX_BUTTON_H_

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxButtonNameStr;

// Pushbutton
class WXDLLEXPORT wxButton: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxButton)
 public:
  inline wxButton() {}
  inline wxButton(wxWindow *parent, wxWindowID id, const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
#if wxUSE_VALIDATORS
#  if defined(__VISAGECPP__)
           const wxValidator* validator = wxDefaultValidator,
#  else
           const wxValidator& validator = wxDefaultValidator,
#  endif
#endif
           const wxString& name = wxButtonNameStr)
  {
      Create(parent, id, label, pos, size, style, validator, name);
  }

  bool Create(wxWindow *parent, wxWindowID id, const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
#if wxUSE_VALIDATORS
#  if defined(__VISAGECPP__)
           const wxValidator* validator = wxDefaultValidator,
#  else
           const wxValidator& validator = wxDefaultValidator,
#  endif
#endif
           const wxString& name = wxButtonNameStr);

  virtual ~wxButton();

  virtual void SetDefault();

  static wxSize GetDefaultSize();

  virtual void Command(wxCommandEvent& event);
  virtual bool OS2Command(WXUINT param, WXWORD id);
  virtual WXHBRUSH OnCtlColor(WXHDC pDC,
                              WXHWND pWnd,
                              WXUINT nCtlColor,
                              WXUINT message,
                              WXWPARAM wParam,
                              WXLPARAM lParam);
protected:
    // send a notification event, return TRUE if processed
    bool SendClickEvent();

    virtual wxSize DoGetBestSize();
};

#endif
    // _WX_BUTTON_H_
