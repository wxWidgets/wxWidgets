/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/spinbutt.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SPINBTN

#include "wx/spinbutt.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QSpinBox>

class wxQtSpinButton : public wxQtEventSignalHandler< QSpinBox, wxSpinButton  >
{
public:
    wxQtSpinButton( wxWindow *parent, wxSpinButton *handler );

private:
    void valueChanged(int value);
    virtual void stepBy(int steps) override; // see QAbstractSpinBox::stepBy()
};

wxQtSpinButton::wxQtSpinButton( wxWindow *parent, wxSpinButton *handler )
    : wxQtEventSignalHandler< QSpinBox, wxSpinButton >( parent, handler )
{
    connect(this, static_cast<void (QSpinBox::*)(int index)>(&QSpinBox::valueChanged),
            this, &wxQtSpinButton::valueChanged);
}

void wxQtSpinButton::valueChanged(int value)
{
    wxSpinButton *handler = GetHandler();
    if ( handler )
    {
        wxSpinEvent event( wxEVT_SPIN, handler->GetId() );
        event.SetInt( value );
        EmitEvent( event );
    }
}

void wxQtSpinButton::stepBy(int steps)
{
    wxSpinButton* const handler = GetHandler();
    if ( !handler )
        return;

    int eventType = steps < 0 ? wxEVT_SPIN_DOWN : wxEVT_SPIN_UP;
    wxSpinEvent directionEvent(eventType, handler->GetId());
    directionEvent.SetPosition(value());
    directionEvent.SetInt(value() + steps * singleStep());
    directionEvent.SetEventObject(handler);

    if ( !handler->HandleWindowEvent(directionEvent) || directionEvent.IsAllowed() )
    {
        QSpinBox::stepBy(steps);
    }
}


wxSpinButton::wxSpinButton() :
    m_qtSpinBox(nullptr)
{
}

wxSpinButton::wxSpinButton(wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos,
             const wxSize& size,
             long style,
             const wxString& name)
{
    Create( parent, id, pos, size, style, name );
}

bool wxSpinButton::Create(wxWindow *parent,
            wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{
    m_qtSpinBox = new wxQtSpinButton( parent, this );

    m_qtSpinBox->setRange(wxSpinButtonBase::GetMin(), wxSpinButtonBase::GetMax());

    // Modify the size so that the text field is not visible.
    // TODO: Find out the width of the buttons i.e. take the style into account (QStyleOptionSpinBox).
    wxSize newSize( size );
    newSize.SetWidth( 18 );

    return wxSpinButtonBase::Create( parent, id, pos, newSize, style, wxDefaultValidator, name );
}

void wxSpinButton::SetRange(int min, int max)
{
    wxSpinButtonBase::SetRange(min, max); // cache the values

    if ( m_qtSpinBox )
    {
        m_qtSpinBox->setRange(min, max);
    }
}

int wxSpinButton::GetValue() const
{
    return m_qtSpinBox->value();
}

void wxSpinButton::SetValue(int val)
{
    m_qtSpinBox->setValue( val );
}

QWidget *wxSpinButton::GetHandle() const
{
    return m_qtSpinBox;
}

#endif // wxUSE_SPINBTN
