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
#include "wx/qt/private/utils.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QSpinBox>

template< typename T, typename Widget >
wxSpinCtrlQt< T, Widget >::wxSpinCtrlQt() :
    m_qtSpinBox(NULL)
{
}

template< typename T, typename Widget >
wxSpinCtrlQt< T, Widget >::wxSpinCtrlQt( wxWindow *WXUNUSED(WXUNUSED(parent)), wxWindowID WXUNUSED(WXUNUSED(id)),
    const wxString& WXUNUSED(WXUNUSED(value)), const wxPoint& WXUNUSED(WXUNUSED(pos)), const wxSize& WXUNUSED(WXUNUSED(size)), long WXUNUSED(WXUNUSED(style)),
    T WXUNUSED(WXUNUSED(min)), T WXUNUSED(WXUNUSED(max)), T WXUNUSED(WXUNUSED(initial)), T WXUNUSED(WXUNUSED(inc)), const wxString& WXUNUSED(WXUNUSED(name)) )
{
}

template< typename T, typename Widget >
bool wxSpinCtrlQt< T, Widget >::Create( wxWindow *parent, wxWindowID id,
    const wxString& value, const wxPoint& pos, const wxSize& size, long style,
    T min, T max, T initial, T inc, const wxString& name )
{
    if ( !(style & wxSP_ARROW_KEYS) )
        m_qtSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);

    if ( style & wxSP_WRAP )
        m_qtSpinBox->setWrapping(true);

    if ( style & wxALIGN_CENTRE_HORIZONTAL )
        m_qtSpinBox->setAlignment(Qt::AlignHCenter);
    else if ( style & wxALIGN_RIGHT )
        m_qtSpinBox->setAlignment(Qt::AlignRight);

    m_qtSpinBox->setAccelerated(true); // to match gtk behavior

    SetRange( min, max );
    SetValue( initial );
    SetIncrement( inc );

    if ( !value.IsEmpty() )
        SetValue( value );

    return QtCreateControl( parent, id, pos, size, style, wxDefaultValidator, name );
}

template< typename T, typename Widget >
wxString wxSpinCtrlQt< T, Widget >::GetTextValue() const
{
    return wxQtConvertString(m_qtSpinBox->text());
}

template< typename T, typename Widget >
void wxSpinCtrlQt< T, Widget >::SetValue( T val )
{
    m_qtSpinBox->blockSignals(true);
    m_qtSpinBox->setValue( val );
    m_qtSpinBox->blockSignals(false);
}

template< typename T, typename Widget >
void wxSpinCtrlQt< T, Widget >::SetRange( T min, T max )
{
    m_qtSpinBox->blockSignals(true);
    m_qtSpinBox->setRange( min, max );
    m_qtSpinBox->blockSignals(false);
}

template< typename T, typename Widget >
void wxSpinCtrlQt< T, Widget >::SetIncrement( T inc )
{
    m_qtSpinBox->setSingleStep( inc );
}

template< typename T, typename Widget >
T wxSpinCtrlQt< T, Widget >::GetValue() const
{
    return m_qtSpinBox->value();
}

template< typename T, typename Widget >
T wxSpinCtrlQt< T, Widget >::GetMin() const
{
    return m_qtSpinBox->minimum();
}

template< typename T, typename Widget >
T wxSpinCtrlQt< T, Widget >::GetMax() const
{
    return m_qtSpinBox->maximum();
}

template< typename T, typename Widget >
T wxSpinCtrlQt< T, Widget >::GetIncrement() const
{
    return m_qtSpinBox->singleStep();
}

template< typename T, typename Widget >
void wxSpinCtrlQt< T, Widget >::SetSnapToTicks(bool WXUNUSED(WXUNUSED(snap_to_ticks)))
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
void wxSpinCtrlQt< T, Widget >::SetSelection(long WXUNUSED(WXUNUSED(from)), long WXUNUSED(WXUNUSED(to)))
{
    wxMISSING_FUNCTION();
}

template< typename T, typename Widget >
QWidget *wxSpinCtrlQt< T, Widget >::GetHandle() const
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
    void valueChanged(int value)
    {
        if ( wxControl *handler = GetHandler() )
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
    wxQtDoubleSpinBox( wxWindow *parent, wxControl *handler )
        : wxQtSpinBoxBase< QDoubleSpinBox >( parent, handler )
    {
        connect(this, static_cast<void (QDoubleSpinBox::*)(double value)>(&QDoubleSpinBox::valueChanged),
                this, &wxQtDoubleSpinBox::valueChanged);
    }
private:
    void valueChanged(double value)
    {
        if ( wxControl *handler = GetHandler() )
        {
            wxSpinDoubleEvent event( wxEVT_SPINCTRLDOUBLE, handler->GetId() );
            event.SetValue(value);
            EmitEvent( event );
        }
    }
};


//##############################################################################

template class wxSpinCtrlQt< int, QSpinBox >;

wxSpinCtrl::wxSpinCtrl()
{
    Init();
}

wxSpinCtrl::wxSpinCtrl(wxWindow *parent, wxWindowID id, const wxString& value,
    const wxPoint& pos, const wxSize& size, long style,
    int min, int max, int initial,
    const wxString& name )

: wxSpinCtrlQt< int, QSpinBox >( parent, id, value, pos, size, style,
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
    return wxSpinCtrlQt< int, QSpinBox >::Create( parent, id, value,
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

//##############################################################################

template class wxSpinCtrlQt< double, QDoubleSpinBox >;

wxIMPLEMENT_DYNAMIC_CLASS( wxSpinCtrlDouble, wxSpinCtrlBase );

wxSpinCtrlDouble::wxSpinCtrlDouble()
{
}

wxSpinCtrlDouble::wxSpinCtrlDouble(wxWindow *parent, wxWindowID id, const wxString& value,
    const wxPoint& pos, const wxSize& size, long style,
    double min, double max, double initial, double inc,
    const wxString& name )

: wxSpinCtrlQt< double, QDoubleSpinBox >( parent, id, value, pos, size, style,
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
    return wxSpinCtrlQt< double, QDoubleSpinBox >::Create( parent, id, value,
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
