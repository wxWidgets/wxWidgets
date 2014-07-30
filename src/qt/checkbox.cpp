/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/checkbox.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/checkbox.h"
#include "wx/qt/utils.h"
#include "wx/qt/converter.h"
#include "wx/qt/private/winevent.h"

class wxQtCheckBox : public wxQtEventSignalHandler< QCheckBox, wxCheckBox >
{
public:
    wxQtCheckBox( wxWindow *parent, wxCheckBox *handler );

private:
    void clicked( bool checked );
};

wxQtCheckBox::wxQtCheckBox( wxWindow *parent, wxCheckBox *handler )
    : wxQtEventSignalHandler< QCheckBox, wxCheckBox >( parent, handler )
{
    connect(this, &QCheckBox::clicked, this, &wxQtCheckBox::clicked);
}

void wxQtCheckBox::clicked( bool checked )
{
    wxCheckBox *handler = GetHandler();
    if ( handler )
    {
        wxCommandEvent event( wxEVT_CHECKBOX, handler->GetId() );
        event.SetInt( checked );
        EmitEvent( event );
    }
}


wxCheckBox::wxCheckBox()
{
}

wxCheckBox::wxCheckBox( wxWindow *parent, wxWindowID id, const wxString& label,
        const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator,
        const wxString& name )
{
    Create( parent, id, label, pos, size, style, validator, name );
}

bool wxCheckBox::Create(wxWindow *parent, wxWindowID id, const wxString& label,
            const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator,
            const wxString& name )
{
    m_qtWindow = m_qtCheckBox = new wxQtCheckBox( parent, this );
    m_qtCheckBox->setText( wxQtConvertString( label ) );

    if ( style & wxCHK_2STATE )
        m_qtCheckBox->setTristate( false );
    else if ( style & wxCHK_3STATE )
    {
        m_qtCheckBox->setTristate( true );

        if ( style & wxCHK_ALLOW_3RD_STATE_FOR_USER )
            wxMISSING_IMPLEMENTATION( wxSTRINGIZE( wxCHK_ALLOW_3RD_STATE_FOR_USER ));
    }
    if ( style & wxALIGN_RIGHT )
        m_qtCheckBox->setLayoutDirection( Qt::RightToLeft );

    return QtCreateControl( parent, id, pos, size, style, validator, name );
}


void wxCheckBox::SetValue(bool value)
{
    m_qtCheckBox->setChecked( value );
}

bool wxCheckBox::GetValue() const
{
    return m_qtCheckBox->isChecked();
}

QCheckBox *wxCheckBox::GetHandle() const
{
    return m_qtCheckBox;
}
