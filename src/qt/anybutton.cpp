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

#include <QtWidgets/QPushButton>

#include "wx/bitmap.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/winevent.h"

class wxQtPushButton : public wxQtEventSignalHandler< QPushButton, wxAnyButton >
{

public:
    virtual bool eventFilter(QObject *watched, QEvent *event);
    wxQtPushButton( wxWindow *parent, wxAnyButton *handler);

private:
    void clicked(bool);
    wxAnyButton *m_button;
};

wxQtPushButton::wxQtPushButton(wxWindow *parent, wxAnyButton *handler)
    : wxQtEventSignalHandler< QPushButton, wxAnyButton >( parent, handler )
{
    m_button = handler;
    installEventFilter( this );
    connect(this, &QPushButton::clicked, this, &wxQtPushButton::clicked);
}

bool wxQtPushButton::eventFilter(QObject *watched, QEvent *event)
{
    QPushButton *button = qobject_cast<QPushButton *>( watched );
    if( !button )
        return false;
    QIcon icon = button->icon();
    if( event->type() == QEvent::HoverMove && button->isEnabled() )
    {
        if( m_button->GetStateBitmaps()[1].IsOk() )
        {
            button->setIcon( QIcon( *( m_button->GetStateBitmaps()[1].GetHandle() ) ) );
        }
    }
    else if( event->type() == QEvent::MouseButtonPress && button->isEnabled() )
    {
        if( m_button->GetStateBitmaps()[2].IsOk() )
        {
            button->setIcon( QIcon( *( m_button->GetStateBitmaps()[2].GetHandle() ) ) );
        }
    }
    else if( event->type() == QEvent::FocusIn && button->isEnabled() )
    {
        if( m_button->GetStateBitmaps()[4].IsOk() )
            button->setIcon( QIcon( *( m_button->GetStateBitmaps()[4].GetHandle() ) ) );
    }
    else if( event->type() == QEvent::EnabledChange )
    {
        if( !button->isEnabled() && m_button->GetStateBitmaps()[3].IsOk() )
            button->setIcon( QIcon( *( m_button->GetStateBitmaps()[3].GetHandle() ) ) );
        else
        {
            if( button->hasFocus() && m_button->GetStateBitmaps()[4].IsOk() )
                button->setIcon( QIcon( *( m_button->GetStateBitmaps()[4].GetHandle() ) ) );
            if( !button->hasFocus() && m_button->GetStateBitmaps()[0].IsOk() )
                button->setIcon( QIcon( *( m_button->GetStateBitmaps()[0].GetHandle() ) ) );
        }
    }
    else if( event->type() == QEvent::HoverLeave )
    {
        if( button->hasFocus() && m_button->GetStateBitmaps()[4].IsOk() )
            button->setIcon( QIcon( *( m_button->GetStateBitmaps()[4].GetHandle() ) ) );
        else if( !button->isEnabled() && m_button->GetStateBitmaps()[3].IsOk() )
            button->setIcon( QIcon( *( m_button->GetStateBitmaps()[3].GetHandle() ) ) );
        else if( m_button->GetStateBitmaps()[0].IsOk() )
            button->setIcon( QIcon( *( m_button->GetStateBitmaps()[0].GetHandle() ) ) );
    }
    else if( event->type() == QEvent::FocusOut )
    {
        if( !button->isEnabled() && m_button->GetStateBitmaps()[3].IsOk() )
            button->setIcon( QIcon( *( m_button->GetStateBitmaps()[3].GetHandle() ) ) );
        else if( m_button->GetStateBitmaps()[0].IsOk() )
            button->setIcon( QIcon( *( m_button->GetStateBitmaps()[0].GetHandle() ) ) );
    }
    else if( event->type() == QEvent::MouseButtonRelease )
    {
        if( !button->isEnabled() && m_button->GetStateBitmaps()[3].IsOk() )
            button->setIcon( QIcon( *( m_button->GetStateBitmaps()[3].GetHandle() ) ) );
        else if( m_button->GetStateBitmaps()[0].IsOk() )
            button->setIcon( QIcon( *( m_button->GetStateBitmaps()[0].GetHandle() ) ) );
    }
    return false;
}

void wxQtPushButton::clicked( bool WXUNUSED(checked) )
{
    wxAnyButton *handler = GetHandler();
    if ( handler )
    {
        wxCommandEvent event( wxEVT_BUTTON, handler->GetId() );
        EmitEvent( event );
    }
}

void wxAnyButton::QtCreate(wxWindow *parent)
{
    // create the default push button (used in button and bmp button)
    m_qtPushButton = new wxQtPushButton( parent, this );
}

void wxAnyButton::QtSetBitmap( const wxBitmap &bitmap )
{
    // load the bitmap and resize the button:
    QPixmap *pixmap = bitmap.GetHandle();
    m_qtPushButton->setIcon( QIcon( *pixmap  ));
    m_qtPushButton->setIconSize( pixmap->rect().size() );
}

void wxAnyButton::SetLabel( const wxString &label )
{
    m_qtPushButton->setText( wxQtConvertString( label ));
}

QWidget *wxAnyButton::GetHandle() const
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
            break;

        case State_Current:
            break;

        case State_Focused:
            break;

        case State_Disabled:
            break;

        case State_Max:
            break;
    }
    m_bitmaps[which] = bitmap;
}

#endif // wxHAS_ANY_BUTTON
