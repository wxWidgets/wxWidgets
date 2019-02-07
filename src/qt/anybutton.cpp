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
    virtual bool eventFilter(QObject *watched, QEvent *event) wxOVERRIDE;
    wxQtPushButton( wxWindow *parent, wxAnyButton *handler);

private:
    void clicked(bool);
    static bool checkIcon(wxAnyButton *wxbutton, int state);
    static void setIcon(QPushButton *button, wxAnyButton *wxbutton, int state);
};

wxQtPushButton::wxQtPushButton(wxWindow *parent, wxAnyButton *handler)
    : wxQtEventSignalHandler< QPushButton, wxAnyButton >( parent, handler )
{
    installEventFilter( this );
    connect(this, &QPushButton::clicked, this, &wxQtPushButton::clicked);
}

bool wxQtPushButton::eventFilter(QObject *watched, QEvent *event)
{
    if( watched != this )
        return false;
    QPushButton *button = qobject_cast<QPushButton *>( watched );
    wxAnyButton *wxbutton = GetHandler();
    QIcon icon = button->icon();

    switch ( event->type() )
    {
        case QEvent::HoverMove:
            if( button->isEnabled() && checkIcon(wxbutton, 1) )
                setIcon(button, wxbutton, 1);
            break;

        case QEvent::MouseButtonPress:
            if( button->isEnabled() && checkIcon(wxbutton, 2) )
                setIcon(button, wxbutton, 2);
            break;

        case QEvent::FocusIn:
            if( button->isEnabled() && checkIcon(wxbutton, 4) )
                setIcon(button, wxbutton, 4);
            break;

        case QEvent::EnabledChange:
            if( button->isEnabled() && checkIcon(wxbutton, 3) )
                setIcon(button, wxbutton, 3);
            else
                if( button->hasFocus() && checkIcon(wxbutton, 4) )
                    setIcon(button, wxbutton, 4);
                else if( checkIcon(wxbutton, 0) )
                    setIcon(button, wxbutton, 0);
            break;

        case QEvent::HoverLeave:
            if( button->hasFocus() && checkIcon(wxbutton, 4) )
                setIcon(button, wxbutton, 4);
            else
                if( !button->isEnabled() && checkIcon(wxbutton, 3) )
                    setIcon(button, wxbutton, 3);
                else if( checkIcon(wxbutton, 0) )
                    setIcon(button, wxbutton, 0);
            break;

        case QEvent::FocusOut:
            if( button->isEnabled() && checkIcon(wxbutton, 3) )
                setIcon(button, wxbutton, 3);
            else if( checkIcon(wxbutton, 0) )
                setIcon(button, wxbutton, 0);
            break;

        case QEvent::MouseButtonRelease:
            if( !button->isEnabled() && checkIcon(wxbutton, 3) )
                setIcon(button, wxbutton, 3);
            else if ( checkIcon(wxbutton, 0) )
                setIcon(button, wxbutton, 0);
            break;
    }
    return false;
}

bool wxQtPushButton::checkIcon(wxAnyButton *wxbutton, int state)
{
    return wxbutton->GetStateBitmaps()[state].IsOk();
}

void wxQtPushButton::setIcon(QPushButton *button, wxAnyButton *wxbutton, int state)
{
        button->setIcon( QIcon( *( wxbutton->GetStateBitmaps()[state].GetHandle() ) ) );
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

void wxAnyButton::DoSetBitmap(State which)
{
    QtSetBitmap(DoGetBitmap(which));
    InvalidateBestSize();
}

wxBitmap wxAnyButton::DoGetBitmap(State state) const
{
    return IsStateValid(state) ? m_bitmaps[state] : wxNullBitmap;
}

bool wxAnyButton::IsStateValid(State state)
{
    return state >= State_Normal && state < State_Max;
}

#endif // wxHAS_ANY_BUTTON
