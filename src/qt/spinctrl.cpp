/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/spinctrl.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SPINCTRL

#include "wx/spinctrl.h"
#include "wx/qt/utils.h"
#include "wx/qt/converter.h"
#include "wx/qt/private/winevent.h"

template< typename T, typename Widget >
wxQtSpinCtrlBase< T, Widget >::wxQtSpinCtrlBase()
{
}

template< typename T, typename Widget >
wxQtSpinCtrlBase< T, Widget >::wxQtSpinCtrlBase( wxWindow *parent, wxWindowID id,
    const wxString& value, const wxPoint& pos, const wxSize& size, long style,
    T min, T max, T initial, T inc, const wxString& name )
{
}

template< typename T, typename Widget >
bool wxQtSpinCtrlBase< T, Widget >::Create( wxWindow *parent, wxWindowID id,
    const wxString& value, const wxPoint& pos, const wxSize& size, long style,
    T min, T max, T initial, T inc, const wxString& name )
{
    wxMISSING_IMPLEMENTATION( wxSTRINGIZE( style ));
    SetValue( initial );
    SetRange( min, max );
    SetIncrement( inc );

    if ( !value.IsEmpty() )
        SetValue( value );

    return QtCreateControl( parent, id, pos, size, style, wxDefaultValidator, name );
}

template< typename T, typename Widget >
void wxQtSpinCtrlBase< T, Widget >::SetValue( T val )
{
    m_qtSpinBox->setValue( val );
}

template< typename T, typename Widget >
void wxQtSpinCtrlBase< T, Widget >::SetRange( T min, T max )
{
    m_qtSpinBox->setRange( min, max );
}

template< typename T, typename Widget >
void wxQtSpinCtrlBase< T, Widget >::SetIncrement( T inc )
{
    m_qtSpinBox->setSingleStep( inc );
}

template< typename T, typename Widget >
T wxQtSpinCtrlBase< T, Widget >::GetValue() const
{
    return m_qtSpinBox->value();
}

template< typename T, typename Widget >
T wxQtSpinCtrlBase< T, Widget >::GetMin() const
{
    return m_qtSpinBox->minimum();
}

template< typename T, typename Widget >
T wxQtSpinCtrlBase< T, Widget >::GetMax() const
{
    return m_qtSpinBox->maximum();
}

template< typename T, typename Widget >
T wxQtSpinCtrlBase< T, Widget >::GetIncrement() const
{
    return m_qtSpinBox->singleStep();
}

template< typename T, typename Widget >
void wxQtSpinCtrlBase< T, Widget >::SetSnapToTicks(bool snap_to_ticks)
{
    wxMISSING_FUNCTION();
}

template< typename T, typename Widget >
bool wxQtSpinCtrlBase< T, Widget >::GetSnapToTicks() const
{
    wxMISSING_FUNCTION();

    return false;
}

template< typename T, typename Widget >
void wxQtSpinCtrlBase< T, Widget >::SetSelection(long from, long to)
{
    wxMISSING_FUNCTION();
}

template< typename T, typename Widget >
Widget *wxQtSpinCtrlBase< T, Widget >::GetHandle() const
{
    return m_qtSpinBox;
}


// Define a derived helper class to get access to valueFromText:

template < typename Widget >
class wxQtSpinBoxBase : public wxQtEventSignalHandler< Widget, wxControl >
{
public:
    wxQtSpinBoxBase( wxWindow *parent, wxControl *handler )
        : wxQtEventSignalHandler< Widget, wxControl >( parent, handler )
    { }

    using Widget::valueFromText;
};

class wxQtSpinBox : public wxQtSpinBoxBase< QSpinBox >
{
public:
    wxQtSpinBox( wxWindow *parent, wxControl *handler )
        : wxQtSpinBoxBase< QSpinBox >( parent, handler )
    {
        connect(this, static_cast<void (QSpinBox::*)(int index)>(&QSpinBox::valueChanged),
                this, &wxQtSpinBox::valueChanged);
    }
private:
    void valueChanged(int value);
};

class wxQtDoubleSpinBox : public wxQtSpinBoxBase< QDoubleSpinBox >
{
public:
    wxQtDoubleSpinBox( wxWindow *parent, wxControl *handler )
        : wxQtSpinBoxBase< QDoubleSpinBox >( parent, handler )
    { }
};


//##############################################################################

template class wxQtSpinCtrlBase< int, QSpinBox >;

wxSpinCtrl::wxSpinCtrl()
{
    Init(); 
}

wxSpinCtrl::wxSpinCtrl(wxWindow *parent, wxWindowID id, const wxString& value,
    const wxPoint& pos, const wxSize& size, long style,
    int min, int max, int initial,
    const wxString& name )

: wxQtSpinCtrlBase< int, QSpinBox >( parent, id, value, pos, size, style,
     min, max, initial, 1, name )
{
    Init();
    Create( parent, id, value, pos, size, style, min, max, initial, name );
}

bool wxSpinCtrl::Create( wxWindow *parent, wxWindowID id, const wxString& value,
    const wxPoint& pos, const wxSize& size, long style,
    int min, int max, int initial,
    const wxString& name )
{
    m_qtSpinBox = new wxQtSpinBox( parent, this );
    return wxQtSpinCtrlBase< int, QSpinBox >::Create( parent, id, value,
        pos, size, style, min, max, initial, 1, name );
}


bool wxSpinCtrl::SetBase(int base)
{
    // Currently we only support base 10.
    if ( base != 10 )
        return false;

    if ( base == m_base )
        return true;

    m_base = base;

    return true;
}

void wxSpinCtrl::SetValue( const wxString &value )
{
    // valueFromText can be called if m_qtSpinBox is an instance of the helper class
    wxQtSpinBox * qtSpinBox = dynamic_cast<wxQtSpinBox *> ((QSpinBox *) m_qtSpinBox);
    if (qtSpinBox != NULL)
        qtSpinBox->setValue( qtSpinBox->valueFromText( wxQtConvertString( value )));
}

void wxQtSpinBox::valueChanged(int value)
{
    wxControl *handler = GetHandler();
    if ( handler )
    {
        wxCommandEvent event( wxEVT_SPINCTRL, handler->GetId() );
        event.SetInt( value );
        EmitEvent( event );
    }
}

//##############################################################################

template class wxQtSpinCtrlBase< double, QDoubleSpinBox >;

wxIMPLEMENT_DYNAMIC_CLASS( wxSpinCtrlDouble, wxSpinCtrlBase );

wxSpinCtrlDouble::wxSpinCtrlDouble()
{
}

wxSpinCtrlDouble::wxSpinCtrlDouble(wxWindow *parent, wxWindowID id, const wxString& value,
    const wxPoint& pos, const wxSize& size, long style,
    double min, double max, double initial, double inc,
    const wxString& name )

: wxQtSpinCtrlBase< double, QDoubleSpinBox >( parent, id, value, pos, size, style,
        min, max, initial, inc, name )
{
    Create( parent, id, value, pos, size, style, min, max, initial, inc, name );
}

bool wxSpinCtrlDouble::Create(wxWindow *parent, wxWindowID id, const wxString& value,
    const wxPoint& pos, const wxSize& size, long style,
    double min, double max, double initial, double inc,
    const wxString& name )
{
    m_qtSpinBox = new wxQtDoubleSpinBox( parent, this );
    return wxQtSpinCtrlBase< double, QDoubleSpinBox >::Create( parent, id, value,
        pos, size, style, min, max, initial, inc, name );
}

void wxSpinCtrlDouble::SetDigits(unsigned digits)
{
    m_qtSpinBox->setDecimals( digits );
}

unsigned wxSpinCtrlDouble::GetDigits() const
{
    return m_qtSpinBox->decimals();
}

void wxSpinCtrlDouble::SetValue( const wxString &value )
{
    // valueFromText can be called if m_qtSpinBox is an instance of the helper class
    wxQtDoubleSpinBox * qtSpinBox = dynamic_cast<wxQtDoubleSpinBox *> ((QDoubleSpinBox *) m_qtSpinBox);
    if (qtSpinBox != NULL)
        qtSpinBox->setValue( qtSpinBox->valueFromText( wxQtConvertString( value )));
}


#endif // wxUSE_SPINCTRL
