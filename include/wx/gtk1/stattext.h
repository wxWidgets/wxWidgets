/////////////////////////////////////////////////////////////////////////////
// Name:        stattext.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKSTATICTEXTH__
#define __GTKSTATICTEXTH__

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

class wxStaticText;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const char *wxStaticTextNameStr;

//-----------------------------------------------------------------------------
// wxStaticText
//-----------------------------------------------------------------------------

class wxStaticText: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxStaticText)

  public:

    wxStaticText(void);
    wxStaticText( wxWindow *parent, wxWindowID id, const wxString &label,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      const long style = 0, const wxString &name = wxStaticTextNameStr );
    bool Create(  wxWindow *parent, wxWindowID id, const wxString &label,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      const long style = 0, const wxString &name = wxStaticTextNameStr );
    wxString GetLabel(void) const;
    void SetLabel( const wxString &label );
};

#endif // __GTKSTATICTEXTH__
