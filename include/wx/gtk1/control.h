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
  // construction
  wxControl();
  wxControl( wxWindow *parent, wxWindowID id, 
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      long style = 0, const wxString &name = wxPanelNameStr  );
  
  // overridables
  virtual void Command( wxCommandEvent &event );

  // accessors
    // this function will filter out '&' characters and will put the accelerator
    // char (the one immediately after '&') into m_chAccel (@@ not yet)
  virtual void SetLabel( const wxString &label );
  virtual wxString GetLabel() const;

protected:
  wxString   m_label;
  // when we implement keyboard interface we will make use of this, but not yet
  //char       m_chAccel;
};

#endif // __GTKCONTROLH__
