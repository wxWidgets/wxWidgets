/////////////////////////////////////////////////////////////////////////////
// Name:        control.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKCONTROLH__
#define __GTKCONTROLH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/list.h"
#include "wx/window.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxControl;

//-----------------------------------------------------------------------------
// wxControl
//-----------------------------------------------------------------------------

class wxControl: public wxWindow
{
DECLARE_DYNAMIC_CLASS(wxControl)

public:
  wxControl();
  wxControl( wxWindow *parent, wxWindowID id, 
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      long style = 0, const wxString &name = wxPanelNameStr  );
  
  virtual void Command( wxCommandEvent &event );

    // this function will filter out '&' characters and will put the accelerator
    // char (the one immediately after '&') into m_chAccel (@@ not yet)
  virtual void SetLabel( const wxString &label );
  virtual wxString GetLabel() const;

protected:
  wxString   m_label;
  char       m_chAccel;  // enabled to avoid breaking binary compatibility later on
};

#endif // __GTKCONTROLH__
