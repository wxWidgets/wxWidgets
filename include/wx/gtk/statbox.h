/////////////////////////////////////////////////////////////////////////////
// Name:        stabox.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKSTATICBOXH__
#define __GTKSTATICBOXH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"

#if wxUSE_STATBOX

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

  // implementation
    
    void ApplyWidgetStyle();
};

#endif

#endif // __GTKSTATICBOXH__
