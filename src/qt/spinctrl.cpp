/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/spinctrl.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SPINCTRL

#include "wx/spinctrl.h"
#include "wx/private/spinctrl.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QSpinBox>

template< typename T, typename Widget >
wxSpinCtrlQt< T, Widget >::wxSpinCtrlQt( wxWindow *WXUNUSED(parent), wxWindowID WXUNUSED(id),
    const wxString& WXUNUSED(value), const wxPoint& WXUNUSED(pos), const wxSize& WXUNUSED(size), long WXUNUSED(style),
    T WXUNUSED(min), T WXUNUSED(max), T WXUNUSED(initial), T WXUNUSED(inc), const wxString& WXUNUSED(name) )
{
}

template< typename T, typename Widget >
bool wxSpinCtrlQt< T, Widget >::Create( wxWindow *parent, wxWindowID id,
    const wxString& value, const wxPoint& pos, const wxSize& size, long style,
    T min, T max, T initial, T inc, const wxString& name )
{
    if ( !(style & wxSP_ARROW_KEYS) )
        GetQtSpinBox()->setButtonSymbols(QAbstractSpinBox::NoButtons);

    if ( style & wxSP_WRAP )
        GetQtSpinBox()->setWrapping(true);

    if ( style & wxALIGN_CENTRE_HORIZONTAL )
        GetQtSpinBox()->setAlignment(Qt::AlignHCenter);
    else if ( style & wxALIGN_RIGHT )
        GetQtSpinBox()->setAlignment(Qt::AlignRight);

    GetQtSpinBox()->setAccelerated(true); // to match gtk behavior

    SetRange( min, max );
    SetValue( initial );
    SetIncrement( inc );

    if ( !value.IsEmpty() )
        SetValue( value );

    return wxSpinCtrlBase::Create( parent, id, pos, size, style, wxDefaultValidator, name );
}

template< typename T, typename Widget >
wxString wxSpinCtrlQt< T, Widget >::GetTextValue() const
{
    return wxQtConvertString(GetQtSpinBox()->text());
}

template< typename T, typename Widget >
void wxSpinCtrlQt< T, Widget >::SetValue( T val )
{
    wxQtEnsureSignalsBlocked blocker(GetQtSpinBox());
    GetQtSpinBox()->setValue( val );
}

template< typename T, typename Widget >
void wxSpinCtrlQt< T, Widget >::SetRange( T min, T max )
{
    wxQtEnsureSignalsBlocked blocker(GetQtSpinBox());
    GetQtSpinBox()->setRange( min, max );
}

template< typename T, typename Widget >
void wxSpinCtrlQt< T, Widget >::SetIncrement( T inc )
{
    GetQtSpinBox()->setSingleStep( inc );
}

template< typename T, typename Widget >
T wxSpinCtrlQt< T, Widget >::GetValue() const
{
    return GetQtSpinBox()->value();
}

template< typename T, typename Widget >
T wxSpinCtrlQt< T, Widget >::GetMin() const
{
    return GetQtSpinBox()->minimum();
}

template< typename T, typename Widget >
T wxSpinCtrlQt< T, Widget >::GetMax() const
{
    return GetQtSpinBox()->maximum();
}

template< typename T, typename Widget >
T wxSpinCtrlQt< T, Widget >::GetIncrement() const
{
    return GetQtSpinBox()->singleStep();
}

template< typename T, typename Widget >
void wxSpinCtrlQt< T, Widget >::SetSnapToTicks(bool WXUNUSED(snap_to_ticks))
{
    wxMISSING_FUNCTION();
}

template< typename T, typename Widget >
bool wxSpinCtrlQt< T, Widget >::GetSnapToTicks() const
{
    wxMISSING_FUNCTION();

    return false;
}

template< typename T, typename Widget >
void wxSpinCtrlQt< T, Widget >::SetSelection(long WXUNUSED(from), long WXUNUSED(to))
{
    wxMISSING_FUNCTION();
}

// Specializations for QSpinBox
template<>
void wxSpinCtrlQt< int, QSpinBox >::SetRange( int min, int max )
{
    // For negative values in the range only base == 10 is allowed
    if ( !wxSpinCtrlImpl::IsBaseCompatibleWithRange(min, max, this->GetBase()) )
        return;

    wxQtEnsureSignalsBlocked blocker(GetQtSpinBox());
    GetQtSpinBox()->setRange( min, max );
}

// Specializations for QDoubleSpinBox
template<>
void wxSpinCtrlQt< double, QDoubleSpinBox >::SetIncrement( double inc )
{
    GetQtSpinBox()->setSingleStep( inc );

    const int digits = wxSpinCtrlImpl::DetermineDigits(inc);

    // Increase the number of digits, if necessary, to show all numbers that
    // can be obtained by using the new increment without loss of precision.
    if ( digits > GetQtSpinBox()->decimals() )
        GetQtSpinBox()->setDecimals( digits );
}

// Define a derived helper class to get access to valueFromText:

template < typename Widget >
class wxQtSpinBoxBase : public wxQtEventSignalHandler< Widget, wxSpinCtrlBase >
{
public:
    wxQtSpinBoxBase( wxWindow *parent, wxSpinCtrlBase *handler )
        : wxQtEventSignalHandler< Widget, wxSpinCtrlBase >( parent, handler )
    { }

    virtual wxString GetValueForProcessEnter() override
    {
        return this->GetHandler()->GetTextValue();
    }

    using Widget::valueFromText;
};

class wxQtSpinBox : public wxQtSpinBoxBase< QSpinBox >
{
public:
    wxQtSpinBox( wxWindow *parent, wxSpinCtrlBase *handler )
        : wxQtSpinBoxBase< QSpinBox >( parent, handler )
    {
        connect(this, static_cast<void (QSpinBox::*)(int index)>(&QSpinBox::valueChanged),
                this, &wxQtSpinBox::valueChanged);
    }
private:
    void valueChanged(int value)
    {
        if ( wxSpinCtrlBase *handler = GetHandler() )
        {
            wxSpinEvent event( wxEVT_SPINCTRL, handler->GetId() );
            event.SetInt( value );
            EmitEvent( event );
        }
    }
};

class wxQtDoubleSpinBox : public wxQtSpinBoxBase< QDoubleSpinBox >
{
public:
    wxQtDoubleSpinBox( wxWindow *parent, wxSpinCtrlBase *handler )
        : wxQtSpinBoxBase< QDoubleSpinBox >( parent, handler )
    {
        connect(this, static_cast<void (QDoubleSpinBox::*)(double value)>(&QDoubleSpinBox::valueChanged),
                this, &wxQtDoubleSpinBox::valueChanged);
    }
private:
    void valueChanged(double value)
    {
        if ( wxSpinCtrlBase *handler = GetHandler() )
        {
            wxSpinDoubleEvent event( wxEVT_SPINCTRLDOUBLE, handler->GetId() );
            event.SetValue(value);
            EmitEvent( event );
        }
    }
};


//##############################################################################

template class wxSpinCtrlQt< int, QSpinBox >;

wxSpinCtrl::wxSpinCtrl(wxWindow *parent, wxWindowID id, const wxString& value,
    const wxPoint& pos, const wxSize& size, long style,
    int min, int max, int initial,
    const wxString& name )

: wxSpinCtrlQt< int, QSpinBox >( parent, id, value, pos, size, style,
     min, max, initial, 1, name )
{
    Create( parent, id, value, pos, size, style, min, max, initial, name );
}

bool wxSpinCtrl::Create( wxWindow *parent, wxWindowID id, const wxString& value,
    const wxPoint& pos, const wxSize& size, long style,
    int min, int max, int initial,
    const wxString& name )
{
    m_qtWindow = new wxQtSpinBox( parent, this );

    return wxSpinCtrlQt< int, QSpinBox >::Create(
        parent, id, value, pos, size, style, min, max, initial, 1, name );
}


bool wxSpinCtrl::SetBase(int base)
{
    if ( base == m_base )
        return true;

#if (QT_VERSION < QT_VERSION_CHECK(5, 2, 0))
    // Currently we only support base 10 for Qt version < 5.2
    if ( base != 10 )
        return false;
#else // Qt5.2+
    if ( base != 10 && base != 16 )
        return false;

    // For negative values in the range only base == 10 is allowed
    if ( !wxSpinCtrlImpl::IsBaseCompatibleWithRange(static_cast<int>(GetMin()),
                                                    static_cast<int>(GetMax()), base) )
        return false;

    GetQtSpinBox()->setDisplayIntegerBase(base);
#endif

    m_base = base;

    return true;
}

void wxSpinCtrl::SetValue( const wxString &value )
{
    // valueFromText can be called if GetQtSpinBox() is an instance of the helper class
    if ( auto qtSpinBox = dynamic_cast<wxQtSpinBox*>(GetQtSpinBox()) )
    {
        BaseType::SetValue( qtSpinBox->valueFromText( wxQtConvertString( value )));
    }
}

//##############################################################################

template class wxSpinCtrlQt< double, QDoubleSpinBox >;

wxIMPLEMENT_DYNAMIC_CLASS( wxSpinCtrlDouble, wxSpinCtrlBase );

wxSpinCtrlDouble::wxSpinCtrlDouble(wxWindow *parent, wxWindowID id, const wxString& value,
    const wxPoint& pos, const wxSize& size, long style,
    double min, double max, double initial, double inc,
    const wxString& name )

: wxSpinCtrlQt< double, QDoubleSpinBox >( parent, id, value, pos, size, style,
                                          min, max, initial, inc, name )
{
    if ( Create( parent, id, value, pos, size, style, min, max, initial, inc, name ) )
    {
        SetDigits(wxSpinCtrlImpl::DetermineDigits(inc));
    }
}

bool wxSpinCtrlDouble::Create(wxWindow *parent, wxWindowID id, const wxString& value,
    const wxPoint& pos, const wxSize& size, long style,
    double min, double max, double initial, double inc,
    const wxString& name )
{
    m_qtWindow = new wxQtDoubleSpinBox( parent, this );

    return wxSpinCtrlQt< double, QDoubleSpinBox >::Create( parent, id, value,
        pos, size, style, min, max, initial, inc, name );
}

void wxSpinCtrlDouble::SetDigits(unsigned digits)
{
    GetQtSpinBox()->setDecimals( digits );
}

unsigned wxSpinCtrlDouble::GetDigits() const
{
    return GetQtSpinBox()->decimals();
}

void wxSpinCtrlDouble::SetValue( const wxString &value )
{
    // valueFromText can be called if GetQtSpinBox() is an instance of the helper class
    if ( auto qtSpinBox = dynamic_cast<wxQtDoubleSpinBox*>(GetQtSpinBox()) )
    {
        BaseType::SetValue( qtSpinBox->valueFromText( wxQtConvertString( value )));
    }
}


#endif // wxUSE_SPINCTRL
