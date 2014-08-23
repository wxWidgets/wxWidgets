/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/anybutton.cpp
// Author:      Mariano Reingart
// Copyright:   (c) 2014 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef wxHAS_ANY_BUTTON

#ifndef WX_PRECOMP
    #include "wx/anybutton.h"
#endif

#include "wx/bitmap.h"
#include "wx/qt/utils.h"


wxQtPushButton::wxQtPushButton( wxWindow *parent, wxAnyButton *handler, wxEventType eventType )
    : wxQtEventSignalHandler< QPushButton, wxAnyButton >( parent, handler )
{
    m_eventType = eventType;
    connect(this, &QPushButton::clicked, this, &wxQtPushButton::OnButtonClicked);
}

void wxAnyButton::QtSetBitmap( const wxBitmap &bitmap )
{
    // load the bitmap and resize the button:
    QPixmap *pixmap = bitmap.GetHandle();
    m_qtPushButton->setIcon( QIcon( *pixmap  ));
    m_qtPushButton->setIconSize( pixmap->rect().size() );
}

void wxQtPushButton::OnButtonClicked( bool WXUNUSED( checked ))
{
    wxCommandEvent event( m_eventType, GetHandler()->GetId() );
    // for toggle buttons, send the checked state in the wx event:
    if ( isCheckable() )
    {
        event.SetInt( isChecked() );
    }
    EmitEvent( event );
}

void wxAnyButton::SetLabel( const wxString &label )
{
    m_qtPushButton->setText( wxQtConvertString( label ));
}

QPushButton *wxAnyButton::GetHandle() const
{
    return m_qtPushButton;
}

void wxAnyButton::DoSetBitmap(const wxBitmap& bitmap, State which)
{
    switch ( which )
    {
        case State_Normal:
            QtSetBitmap(bitmap);
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

#endif // wxHAS_ANY_BUTTON
