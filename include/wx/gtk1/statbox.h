/////////////////////////////////////////////////////////////////////////////
// Name:        stabox.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKSTATICBOXH__
#define __GTKSTATICBOXH__

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

class wxStaticBox;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const char *wxStaticBoxNameStr;

//-----------------------------------------------------------------------------
// wxStaticBox
//-----------------------------------------------------------------------------

class wxStaticBox: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxStaticBox)

  public:

    wxStaticBox(void);
    wxStaticBox( wxWindow *parent, wxWindowID id, const wxString &label, 
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      long style = 0, const wxString &name = wxStaticBoxNameStr  );
    bool Create( wxWindow *parent, wxWindowID id, const wxString &label,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      long style = 0, const wxString &name = wxStaticBoxNameStr  );
    void SetLabel( const wxString &label );
};

#endif // __GTKSTATICBOXH__
