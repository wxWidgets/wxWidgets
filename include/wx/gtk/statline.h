/////////////////////////////////////////////////////////////////////////////
// Name:        statline.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKSTATICLINEH__
#define __GTKSTATICLINEH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"

#if wxUSE_STATLINE

#include "wx/object.h"
#include "wx/list.h"
#include "wx/control.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxStaticLine;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const char *wxStaticTextNameStr;

//-----------------------------------------------------------------------------
// wxStaticLine
//-----------------------------------------------------------------------------

class wxStaticLine: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxStaticLine)

  public:

    wxStaticLine(void);
    wxStaticLine( wxWindow *parent, wxWindowID id,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      long style = wxLI_HORIZONTAL, const wxString &name = wxStaticTextNameStr );
    bool Create(  wxWindow *parent, wxWindowID id,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      long style = wxLI_HORIZONTAL, const wxString &name = wxStaticTextNameStr );
    
};

#endif

#endif // __GTKSTATICLINEH__
