/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/spinbutt.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/spinbutt.h"


class wxQtSpinBox : public wxQtEventSignalHandler< QSpinBox, wxSpinButton  >
{
public:
    wxQtSpinBox( wxWindow *parent, wxSpinButton *handler );

private:
    void valueChanged(int value);
};

wxQtSpinBox::wxQtSpinBox( wxWindow *parent, wxSpinButton *handler )
    : wxQtEventSignalHandler< QSpinBox, wxSpinButton >( parent, handler )
{
    connect(this, static_cast<void (QSpinBox::*)(int index)>(&QSpinBox::valueChanged),
            this, &wxQtSpinBox::valueChanged);
}

void wxQtSpinBox::valueChanged(int value)
{
    wxCommandEvent event( wxEVT_SPIN, GetHandler()->GetId() );
    event.SetInt( value );
    EmitEvent( event );
}


wxSpinButton::wxSpinButton()
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
    m_qtSpinBox = new wxQtSpinBox( parent, this );

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

QSpinBox *wxSpinButton::GetHandle() const
{
    return m_qtSpinBox;
}
