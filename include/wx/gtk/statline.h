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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);
    
};

#endif 
  // wxUSE_STATLINE
  
#endif 
  // __GTKSTATICLINEH__
