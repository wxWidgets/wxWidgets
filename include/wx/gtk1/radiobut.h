/////////////////////////////////////////////////////////////////////////////
// Name:        radiobut.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKRADIOBUTTONH__
#define __GTKRADIOBUTTONH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/list.h"
#include "wx/control.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxRadioButton;

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern const char* wxRadioButtonNameStr;

//-----------------------------------------------------------------------------
// wxRadioButton
//-----------------------------------------------------------------------------

class wxRadioButton: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxRadioButton)
  
  public:
    inline wxRadioButton(void) {}
    inline wxRadioButton( wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxRadioButtonNameStr )
    {
      Create( parent, id, label, pos, size, style, validator, name );
    }
    bool Create(wxWindow *parent, wxWindowID id,
           const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxRadioButtonNameStr );
    virtual void SetLabel(const wxString& label);
    virtual void SetValue(bool val);
    virtual bool GetValue(void) const;
};

#endif // __GTKRADIOBUTTONH__
