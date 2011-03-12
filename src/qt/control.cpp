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
    Create( parent, id, pos, size, style, validator, name );
}


bool wxControl::Create(wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size, long style,
        const wxValidator& validator,
        const wxString& name )
{
    bool isCreated = wxWindow::Create(parent, id, pos, size, style, name);

#if wxUSE_VALIDATORS
    SetValidator(validator);
#endif

    return isCreated;
}

bool wxControl::QtCreateControl( wxWindow *parent, wxWindowID id,
    const wxPoint &pos, const wxSize &size, long style,
    const wxValidator &validator, const wxString &name )
{
    // The Qt widget has been created without a position/size so move/resize it:

    wxSize bestSize = GetBestSize();
    int width = ( size.GetWidth() == wxDefaultCoord ) ? bestSize.GetWidth() : size.GetWidth();
    int height = ( size.GetHeight() == wxDefaultCoord ) ? bestSize.GetHeight() : size.GetHeight();
    DoMoveWindow( pos.x, pos.y, width, height );

    return CreateControl( parent, id, pos, size, style, validator, name );
}
