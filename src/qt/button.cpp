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
#include "wx/qt/utils.h"

wxQtPushButton::wxQtPushButton( wxWindow *parent, wxControl *handler )
    : wxQtEventSignalHandler< QPushButton, wxControl >( parent, handler )
{
    connect(this, &QPushButton::clicked, this, &wxQtPushButton::OnButtonClicked);
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
    // load the bitmap and resize the button:
    QPixmap *pixmap = bitmap.GetHandle();
    setIcon( QIcon( *pixmap  ));
    setIconSize( pixmap->rect().size() );
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

void wxButton::DoSetBitmap(const wxBitmap& bitmap, State which)
{
    switch ( which )
    {
        case State_Normal:
            m_qtPushButton->SetBitmap(bitmap);
            InvalidateBestSize();
            break;

        case State_Pressed:
            wxMISSING_IMPLEMENTATION( wxSTRINGIZE( State_Pressed ));
            break;

        case State_Current:
            wxMISSING_IMPLEMENTATION( wxSTRINGIZE( State_Current ));
            break;

        case State_Focused:
            wxMISSING_IMPLEMENTATION( wxSTRINGIZE( State_Focused ));
            break;

        default:
            ;
    }
}

