/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/button_qt.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/qt/button_qt.h"
#include "wx/bitmap.h"

wxQtPushButton::wxQtPushButton( wxWindow *parent, wxControl *handler )
    : wxQtEventSignalHandler< QPushButton, wxControl >( parent, handler )
{
    connect( this, SIGNAL( clicked( bool )), this, SLOT( OnButtonClicked( bool )));
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
