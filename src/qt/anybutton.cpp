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
    wxQtPushButton( wxWindow *parent, wxAnyButton *handler);

private:
    virtual bool event(QEvent* e) override;
    void action(); // press, release
    void clicked(bool);
};

wxQtPushButton::wxQtPushButton(wxWindow *parent, wxAnyButton *handler)
    : wxQtEventSignalHandler< QPushButton, wxAnyButton >( parent, handler )
{
    connect(this, &QPushButton::clicked, this, &wxQtPushButton::clicked);
    connect(this, &QPushButton::pressed, this, &wxQtPushButton::action);
    connect(this, &QPushButton::released, this, &wxQtPushButton::action);
}

void wxQtPushButton::clicked(bool checked)
{
    wxAnyButton *handler = GetHandler();
    if ( handler )
    {
        wxCommandEvent event( handler->QtGetEventType(), handler->GetId() );
        if ( isCheckable() ) // toggle buttons
        {
            event.SetInt(checked);
        }
        EmitEvent( event );
    }
}

void wxQtPushButton::action()
{
    GetHandler()->QtUpdateState();
}

bool wxQtPushButton::event(QEvent* e)
{
    switch ( e->type() )
    {
    case QEvent::EnabledChange:
    case QEvent::Enter:
    case QEvent::Leave:
    case QEvent::FocusIn:
    case QEvent::FocusOut:
        GetHandler()->QtUpdateState();
        break;
    default:
        break;
    }

    return QPushButton::event(e);
}

wxAnyButton::wxAnyButton() :
    m_qtPushButton(nullptr)
{
}


void wxAnyButton::QtCreate(wxWindow *parent)
{
    // create the basic push button (used in button and bmp button)
    m_qtPushButton = new wxQtPushButton(parent, this);
    m_qtPushButton->setAutoDefault(false);
}

void wxAnyButton::QtSetBitmap( const wxBitmapBundle &bitmapBundle )
{
    wxCHECK_RET(m_qtPushButton, "Invalid button.");

    if ( !bitmapBundle.IsOk() )
        return;

    wxBitmap bitmap = bitmapBundle.GetBitmap(bitmapBundle.GetDefaultSize()*GetDPIScaleFactor());

    // load the bitmap and resize the button:
    QPixmap *pixmap = bitmap.GetHandle();
    if ( pixmap != nullptr )
    {
        m_qtPushButton->setIcon(QIcon(*pixmap));
        m_qtPushButton->setIconSize(pixmap->rect().size());

        InvalidateBestSize();
    }
}

void wxAnyButton::SetLabel( const wxString &label )
{
    m_qtPushButton->setText( wxQtConvertString( label ));
}

QWidget *wxAnyButton::GetHandle() const
{
    return m_qtPushButton;
}

wxBitmap wxAnyButton::DoGetBitmap(State state) const
{
    return state < State_Max ? m_bitmaps[state].GetBitmap(wxDefaultSize) : wxNullBitmap;
}

void wxAnyButton::DoSetBitmap(const wxBitmapBundle& bitmap, State which)
{
    wxCHECK_RET(which < State_Max, "Invalid state");

    // Cache the bitmap.
    m_bitmaps[which] = bitmap;

    // Replace current bitmap only if the button is in the same state.
    if ( which == QtGetCurrentState() )
    {
        QtUpdateState();
    }
}

wxAnyButton::State wxAnyButton::QtGetCurrentState() const
{
    wxCHECK_MSG(m_qtPushButton, State_Normal, "Invalid button.");

    if ( !m_qtPushButton->isEnabled() )
    {
        return State_Disabled;
    }

    if ( m_qtPushButton->isChecked() || m_qtPushButton->isDown() )
    {
        return State_Pressed;
    }

    if ( HasCapture() || m_qtPushButton->hasMouseTracking() || m_qtPushButton->underMouse() )
    {
        return State_Current;
    }

    if ( m_qtPushButton->hasFocus() )
    {
        return State_Focused;
    }

    return State_Normal;
}

void wxAnyButton::QtUpdateState()
{
    State state = QtGetCurrentState();

    // Update the bitmap
    const wxBitmapBundle& bmp = m_bitmaps[state];
    QtSetBitmap(bmp.IsOk() ? bmp : m_bitmaps[State_Normal]);
}

#endif // wxHAS_ANY_BUTTON
