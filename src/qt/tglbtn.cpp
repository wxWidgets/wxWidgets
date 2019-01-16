/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/tglbtn.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
#endif // WX_PRECOMP

#include "wx/stockitem.h"
#include "wx/tglbtn.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QPushButton>

class wxQtToggleButton : public wxQtEventSignalHandler< QPushButton, wxAnyButton >
{

public:
    wxQtToggleButton( wxWindow *parent, wxAnyButton *handler);

private:
    void clicked( bool checked );
};

wxQtToggleButton::wxQtToggleButton(wxWindow *parent, wxAnyButton *handler)
    : wxQtEventSignalHandler< QPushButton, wxAnyButton >( parent, handler )
{
    setCheckable( true );
    connect(this, &QPushButton::clicked, this, &wxQtToggleButton::clicked);
}

void wxQtToggleButton::clicked( bool checked )
{
    wxAnyButton *handler = GetHandler();
    if ( handler )
    {
        // for toggle buttons, send the checked state in the wx event:
        wxCommandEvent event( wxEVT_TOGGLEBUTTON, handler->GetId() );
        event.SetInt( checked );
        EmitEvent( event );
    }
}

wxDEFINE_EVENT( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEvent );

wxIMPLEMENT_DYNAMIC_CLASS(wxBitmapToggleButton, wxControl);

wxBitmapToggleButton::wxBitmapToggleButton()
{
}

wxBitmapToggleButton::wxBitmapToggleButton(wxWindow *parent,
               wxWindowID id,
               const wxBitmap& label,
               const wxPoint& pos,
               const wxSize& size,
               long style,
               const wxValidator& validator,
               const wxString& name)
{
    Create( parent, id, label, pos, size, style, validator, name );
}

bool wxBitmapToggleButton::Create(wxWindow *parent,
            wxWindowID id,
            const wxBitmap& label,
            const wxPoint& pos,
            const wxSize& size, long style,
            const wxValidator& validator,
            const wxString& name)
{
    // this button is toggleable and has a bitmap label:
    QtSetBitmap( label );

    return QtCreateControl( parent, id, pos, size, style, validator, name );
}

void wxBitmapToggleButton::SetValue(bool state)
{
    m_qtPushButton->setChecked( state );
}

bool wxBitmapToggleButton::GetValue() const
{
    return m_qtPushButton->isChecked();
}

QWidget *wxBitmapToggleButton::GetHandle() const
{
    return m_qtPushButton;
}

//##############################################################################

wxIMPLEMENT_DYNAMIC_CLASS(wxToggleButton, wxControl);

wxToggleButton::wxToggleButton()
{
}

wxToggleButton::wxToggleButton(wxWindow *parent,
               wxWindowID id,
               const wxString& label,
               const wxPoint& pos,
               const wxSize& size,
               long style,
               const wxValidator& validator,
               const wxString& name)
{
    Create( parent, id, label, pos, size, style, validator, name );
}

bool wxToggleButton::Create(wxWindow *parent,
            wxWindowID id,
            const wxString& label,
            const wxPoint& pos,
            const wxSize& size, long style,
            const wxValidator& validator,
            const wxString& name)
{
    // create a checkable push button
    m_qtPushButton = new wxQtToggleButton( parent, this );

    // this button is toggleable and has a text label
    SetLabel( wxIsStockID( id ) ? wxGetStockLabel( id ) : label );

    return QtCreateControl( parent, id, pos, size, style, validator, name );
}

void wxToggleButton::SetValue(bool state)
{
    m_qtPushButton->setChecked( state );
}

bool wxToggleButton::GetValue() const
{
    return m_qtPushButton->isChecked();
}

QWidget *wxToggleButton::GetHandle() const
{
    return m_qtPushButton;
}
