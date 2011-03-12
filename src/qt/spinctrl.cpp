/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/spinctrl.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SPINCTRL

#include "wx/spinctrl.h"
#include "wx/qt/utils.h"
#include "wx/qt/converter.h"

template< typename T, typename Widget >
wxQtSpinCtrlBase< T, Widget >::wxQtSpinCtrlBase()
{
}

template< typename T, typename Widget >
wxQtSpinCtrlBase< T, Widget >::wxQtSpinCtrlBase( wxWindow *parent, wxWindowID id,
    const wxString& value, const wxPoint& pos, const wxSize& size, long style,
    T min, T max, T initial, T inc, const wxString& name )
{
    Create( parent, id, value, pos, size, style, min, max, initial, inc, name );
}

template< typename T, typename Widget >
bool wxQtSpinCtrlBase< T, Widget >::Create( wxWindow *parent, wxWindowID id,
    const wxString& value, const wxPoint& pos, const wxSize& size, long style,
    T min, T max, T initial, T inc, const wxString& name )
{
    m_qtSpinBox = new Widget( parent->GetHandle() );
    wxMISSING_IMPLEMENTATION( wxSTRINGIZE( style ));
    SetValue( initial );
    SetRange( min, max );
    SetIncrement( inc );

    if ( !value.IsEmpty() )
        SetValue( value );

    return QtCreateControl( parent, id, pos, size, style, wxDefaultValidator, name );
}

template< typename T, typename Widget >
void wxQtSpinCtrlBase< T, Widget >::SetValue( const wxString &value )
{
    m_qtSpinBox->setValue( m_qtSpinBox->valueFromText( wxQtConvertString( value )));
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


//##############################################################################

template class wxQtSpinCtrlBase< int, wxQtSpinBox >;

wxSpinCtrl::wxSpinCtrl()
{
}

wxSpinCtrl::wxSpinCtrl(wxWindow *parent, wxWindowID id, const wxString& value,
    const wxPoint& pos, const wxSize& size, long style,
    int min, int max, int initial,
    const wxString& name )

: wxQtSpinCtrlBase< int, wxQtSpinBox >( parent, id, value, pos, size, style,
     min, max, initial, 1, name )
{
}

bool wxSpinCtrl::Create( wxWindow *parent, wxWindowID id, const wxString& value,
    const wxPoint& pos, const wxSize& size, long style,
    int min, int max, int initial,
    const wxString& name )
{
    return wxQtSpinCtrlBase< int, wxQtSpinBox >::Create( parent, id, value,
        pos, size, style, min, max, initial, 1, name );
}


//##############################################################################

template class wxQtSpinCtrlBase< double, wxQtDoubleSpinBox >;

wxIMPLEMENT_DYNAMIC_CLASS( wxSpinCtrlDouble, wxSpinCtrlBase );

wxSpinCtrlDouble::wxSpinCtrlDouble()
{
}

wxSpinCtrlDouble::wxSpinCtrlDouble(wxWindow *parent, wxWindowID id, const wxString& value,
    const wxPoint& pos, const wxSize& size, long style,
    double min, double max, double initial, double inc,
    const wxString& name )

: wxQtSpinCtrlBase< double, wxQtDoubleSpinBox >( parent, id, value, pos, size, style,
        min, max, initial, inc, name )
{
}

bool wxSpinCtrlDouble::Create(wxWindow *parent, wxWindowID id, const wxString& value,
    const wxPoint& pos, const wxSize& size, long style,
    double min, double max, double initial, double inc,
    const wxString& name )
{
    return wxQtSpinCtrlBase< double, wxQtDoubleSpinBox >::Create( parent, id, value,
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


#endif // wxUSE_SPINCTRL
