/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/spinbutt.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/spinbutt.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QSpinBox>

class wxQtSpinButton : public wxQtEventSignalHandler< QSpinBox, wxSpinButton  >
{
public:
    wxQtSpinButton( wxWindow *parent, wxSpinButton *handler );

private:
    void valueChanged(int value);
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
        wxCommandEvent event( wxEVT_SPIN, handler->GetId() );
        event.SetInt( value );
        EmitEvent( event );
    }
}


wxSpinButton::wxSpinButton() :
    m_qtSpinBox(NULL)
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

    // Modify the size so that the text field is not visible.
    // TODO: Find out the width of the buttons i.e. take the style into account (QStyleOptionSpinBox).
    wxSize newSize( size );
    newSize.SetWidth( 18 );

    return QtCreateControl( parent, id, pos, newSize, style, wxDefaultValidator, name );
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
