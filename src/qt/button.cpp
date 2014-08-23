/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/button.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/button.h"
#include "wx/bitmap.h"

wxQtPushButton::wxQtPushButton( wxWindow *parent, wxControl *handler )
    : wxQtEventSignalHandler< QPushButton, wxControl >( parent, handler )
{
    connect( this, SIGNAL( clicked( bool )), this, SLOT( OnButtonClicked( bool )));
}

void wxQtPushButton::SetToggleable()
{
    setCheckable( true );
}

void wxQtPushButton::SetValue( bool state )
{
    setChecked( state );
}

bool wxQtPushButton::GetValue() const
{
    return isChecked();
}

void wxQtPushButton::SetDefault()
{
    setDefault( true );
}

void wxQtPushButton::SetLabel( const wxString &label )
{
    setText( wxQtConvertString( label ));
}

void wxQtPushButton::SetBitmap( const wxBitmap &bitmap )
{
    setIcon( QIcon( *bitmap.GetHandle() ));
}

void wxQtPushButton::OnButtonClicked( bool WXUNUSED( checked ))
{
    wxCommandEvent event( wxEVT_COMMAND_BUTTON_CLICKED, GetHandler()->GetId() );
    EmitEvent( event );
}

wxButton::wxButton()
{
}

wxButton::wxButton(wxWindow *parent, wxWindowID id,
       const wxString& label,
       const wxPoint& pos,
       const wxSize& size, long style,
       const wxValidator& validator,
       const wxString& name )
{
    Create( parent, id, label, pos, size, style, validator, name );
}

bool wxButton::Create(wxWindow *parent, wxWindowID id,
       const wxString& label,
       const wxPoint& pos,
       const wxSize& size, long style,
       const wxValidator& validator,
       const wxString& name )
{
    m_qtPushButton = new wxQtPushButton( parent, this );
    SetLabel( wxIsStockID( id ) ? wxGetStockLabel( id ) : label );

    return QtCreateControl( parent, id, pos, size, style, validator, name );
}

wxWindow *wxButton::SetDefault()
{
    wxWindow *oldDefault = wxButtonBase::SetDefault();

    m_qtPushButton->SetDefault();

    return oldDefault;

}

void wxButton::SetLabel( const wxString &label )
{
    m_qtPushButton->SetLabel( label );
}

QPushButton *wxButton::GetHandle() const
{
    return m_qtPushButton;
}


