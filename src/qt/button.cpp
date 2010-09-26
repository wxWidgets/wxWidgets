/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/button.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/button.h"

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
    m_qtPushButton = new wxQtButton( this, wxQtConvertString( label ), parent->GetHandle() );

    return wxButtonBase::Create( parent, id, pos, size, style, validator, name );
}

wxWindow *wxButton::SetDefault()
{
    wxWindow *oldDefault = wxButtonBase::SetDefault();

    m_qtPushButton->setDefault( true );

    return oldDefault;

}
QPushButton *wxButton::GetHandle() const
{
    return m_qtPushButton;
}

//=============================================================================

wxQtButton::wxQtButton( wxButton *button, const QString &text, QWidget *parent )
    : QPushButton( text, parent ),
      wxQtSignalForwarder< wxButton >( button )
{
    connect( this, SIGNAL( clicked( bool )), this, SLOT( OnButtonClicked( bool )));
}

void wxQtButton::OnButtonClicked( bool WXUNUSED( checked ))
{
    wxButton *button = GetSignalHandler();
    wxCommandEvent event( wxEVT_COMMAND_BUTTON_CLICKED, button->GetId() );
    event.SetEventObject( button );
    button->HandleWindowEvent( event );
}
