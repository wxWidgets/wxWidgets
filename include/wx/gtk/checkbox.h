/////////////////////////////////////////////////////////////////////////////
// Name:        checkbox.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKCHECKBOXH__
#define __GTKCHECKBOXH__

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

class wxCheckBox;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const char *wxCheckBoxNameStr;

//-----------------------------------------------------------------------------
// wxCheckBox
//-----------------------------------------------------------------------------

class wxCheckBox: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxCheckBox)

  public:

    wxCheckBox(void);
    inline wxCheckBox( wxWindow *parent, wxWindowID id, const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxCheckBoxNameStr)
    {
      Create(parent, id, label, pos, size, style, validator, name);
    }
    bool Create( wxWindow *parent, wxWindowID id, const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxCheckBoxNameStr );
    void SetValue( bool state );
    bool GetValue(void) const;
};

#endif // __GTKCHECKBOXH__
