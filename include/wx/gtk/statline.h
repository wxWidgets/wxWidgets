/////////////////////////////////////////////////////////////////////////////
// Name:        statline.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKSTATICLINEH__
#define __GTKSTATICLINEH__

#ifdef __GNUG__
    #pragma interface
#endif

#if !wxUSE_STATLINE
    #error "This file should only be included if wxUSE_STATLINE == 1"
#endif

#include "wx/object.h"
#include "wx/list.h"
#include "wx/control.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxStaticLine;

//-----------------------------------------------------------------------------
// wxStaticLine
//-----------------------------------------------------------------------------

class wxStaticLine : public wxStaticLineBase
{
    DECLARE_DYNAMIC_CLASS(wxStaticLine)

public:
    wxStaticLine();
    wxStaticLine( wxWindow *parent, wxWindowID id,
            const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
            long style = wxLI_HORIZONTAL, const wxString &name = wxStaticTextNameStr );
    bool Create(  wxWindow *parent, wxWindowID id,
            const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
            long style = wxLI_HORIZONTAL, const wxString &name = wxStaticTextNameStr );
};

#endif // __GTKSTATICLINEH__
