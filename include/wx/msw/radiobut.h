/////////////////////////////////////////////////////////////////////////////
// Name:        radiobut.h
// Purpose:     wxRadioButton class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RADIOBUT_H_
#define _WX_RADIOBUT_H_

#ifdef __GNUG__
#pragma interface "radiobut.h"
#endif

class WXDLLEXPORT wxRadioButton: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxRadioButton)
 protected:
 public:
  inline wxRadioButton(void) {}
  inline wxRadioButton(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxRadioButtonNameStr)
  {
      Create(parent, id, label, pos, size, style, validator, name);
  }

  bool Create(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxRadioButtonNameStr);

  virtual void SetLabel(const wxString& label);
  virtual void SetValue(bool val);
  virtual bool GetValue(void) const ;

  bool MSWCommand(WXUINT param, WXWORD id);
  virtual WXHBRUSH OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
            WXUINT message, WXWPARAM wParam, WXLPARAM lParam);

  void Command(wxCommandEvent& event);
};

#endif
    // _WX_RADIOBUT_H_
