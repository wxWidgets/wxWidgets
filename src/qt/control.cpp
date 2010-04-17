/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/control.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/control.h"

IMPLEMENT_DYNAMIC_CLASS( wxControl, wxControlBase )

wxControl::wxControl()
{
}

wxControl::wxControl(wxWindow *parent, wxWindowID id,
         const wxPoint& pos,
         const wxSize& size, long style,
         const wxValidator& validator,
         const wxString& name )
{
}


bool wxControl::Create(wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size, long style,
        const wxValidator& validator,
        const wxString& name )
{
    return false;
}


QWidget *wxControl::GetHandle() const
{
    return NULL;
}

