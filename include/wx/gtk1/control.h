/////////////////////////////////////////////////////////////////////////////
// Name:        control.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart
// Licence:     wxWindows licence
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

class wxControl : public wxControlBase
{
public:
    wxControl();
    wxControl( wxWindow *parent,
               wxWindowID id,
               const wxPoint &pos = wxDefaultPosition,
               const wxSize &size = wxDefaultSize,
               long style = 0,
               const wxString &name = wxPanelNameStr );

    // this function will filter out '&' characters and will put the accelerator
    // char (the one immediately after '&') into m_chAccel (TODO not yet)
    virtual void SetLabel( const wxString &label );
    virtual wxString GetLabel() const;

protected:
    wxString   m_label;
    char       m_chAccel;  // enabled to avoid breaking binary compatibility later on
    
private:
    DECLARE_DYNAMIC_CLASS(wxControl)
};

#endif // __GTKCONTROLH__
