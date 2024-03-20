/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/tglbtn.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_TOGGLEBTN

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
#endif // WX_PRECOMP

#include "wx/stockitem.h"
#include "wx/tglbtn.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QPushButton>

wxDEFINE_EVENT( wxEVT_TOGGLEBUTTON, wxCommandEvent );

wxIMPLEMENT_DYNAMIC_CLASS(wxBitmapToggleButton, wxToggleButton);

wxBitmapToggleButton::wxBitmapToggleButton()
{
}

wxBitmapToggleButton::wxBitmapToggleButton(wxWindow *parent,
               wxWindowID id,
               const wxBitmapBundle& label,
               const wxPoint& pos,
               const wxSize& size,
               long style,
               const wxValidator& validator,
               const wxString& name)
{
    Create( parent, id, label, pos, size, style, validator, name );
}

bool wxBitmapToggleButton::Create(wxWindow *parent,
            wxWindowID id,
            const wxBitmapBundle& label,
            const wxPoint& pos,
            const wxSize& size, long style,
            const wxValidator& validator,
            const wxString& name)
{
    if ( !wxToggleButton::Create(parent, id, wxString(), pos, size, style, validator, name) )
    {
        return false;
    }

    // this button is toggleable and has a bitmap label:
    if ( label.IsOk() )
    {
        SetBitmapLabel(label);

        // we need to adjust the size after setting the bitmap as it may be too
        // big for the default button size
        SetInitialSize(size);
    }

    return true;
}

//##############################################################################

wxIMPLEMENT_DYNAMIC_CLASS(wxToggleButton, wxControl);

wxToggleButton::wxToggleButton()
{
}

wxToggleButton::wxToggleButton(wxWindow *parent,
               wxWindowID id,
               const wxString& label,
               const wxPoint& pos,
               const wxSize& size,
               long style,
               const wxValidator& validator,
               const wxString& name)
{
    Create( parent, id, label, pos, size, style, validator, name );
}

bool wxToggleButton::Create(wxWindow *parent,
            wxWindowID id,
            const wxString& label,
            const wxPoint& pos,
            const wxSize& size, long style,
            const wxValidator& validator,
            const wxString& name)
{
    // create a checkable push button
    QtCreate(parent);
    m_qtPushButton->setCheckable(true);

    // this button is toggleable and has a text label
    SetLabel( wxIsStockID( id ) ? wxGetStockLabel( id ) : label );

    return wxToggleButtonBase::Create( parent, id, pos, size, style, validator, name );
}

void wxToggleButton::SetValue(bool state)
{
    m_qtPushButton->setChecked( state );
}

bool wxToggleButton::GetValue() const
{
    return m_qtPushButton->isChecked();
}

#endif // wxUSE_TOGGLEBTN
