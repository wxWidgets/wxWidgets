/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/tglbtn.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/tglbtn.h"
#include "wx/bitmap.h"
#include "wx/qt/converter.h"

wxDEFINE_EVENT( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEvent );

IMPLEMENT_DYNAMIC_CLASS( wxBitmapToggleButton, wxToggleButtonBase )

wxBitmapToggleButton::wxBitmapToggleButton()
{
}

wxBitmapToggleButton::wxBitmapToggleButton(wxWindow *parent,
               wxWindowID id,
               const wxBitmap& label,
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
            const wxBitmap& label,
            const wxPoint& pos,
            const wxSize& size, long style,
            const wxValidator& validator,
            const wxString& name)
{
    m_qtPushButton = new wxQtPushButton( parent, this, wxEVT_COMMAND_TOGGLEBUTTON_CLICKED);
    // this button is toggleable and has a bitmap label:
    m_qtPushButton->setCheckable( true );
    QtSetBitmap( label );

    return QtCreateControl( parent, id, pos, size, style, validator, name );
}

void wxBitmapToggleButton::SetValue(bool state)
{
    m_qtPushButton->setChecked( state );
}

bool wxBitmapToggleButton::GetValue() const
{
    return m_qtPushButton->isChecked();
}

QPushButton *wxBitmapToggleButton::GetHandle() const
{
    return m_qtPushButton;
}

//##############################################################################

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
    m_qtPushButton = new wxQtPushButton( parent, this, wxEVT_COMMAND_TOGGLEBUTTON_CLICKED );
    // this button is toggleable and has a text label
    m_qtPushButton->setCheckable( true );
    SetLabel( wxIsStockID( id ) ? wxGetStockLabel( id ) : label );

    return QtCreateControl( parent, id, pos, size, style, validator, name );
}

void wxToggleButton::SetValue(bool state)
{
    m_qtPushButton->setChecked( state );
}

bool wxToggleButton::GetValue() const
{
    return m_qtPushButton->isChecked();
}

QPushButton *wxToggleButton::GetHandle() const
{
    return m_qtPushButton;
}
