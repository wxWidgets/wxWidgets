/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/control.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2009 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/control.h"
#include "wx/qt/private/converter.h"

#include <QtWidgets/QWidget>

wxIMPLEMENT_DYNAMIC_CLASS(wxControl, wxWindow);

// Defined in src/qt/window.cpp
extern wxSize wxQtGetBestSize(QWidget* qtWidget);

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

wxSize wxControl::DoGetBestSize() const
{
    wxCHECK_MSG(GetHandle(), wxDefaultSize, "Invalid control");

    if ( m_windowSizer )
       return wxControlBase::DoGetBestSize();

    return wxQtGetBestSize( GetHandle() );
}
