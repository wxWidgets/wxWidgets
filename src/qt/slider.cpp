/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/slider.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/slider.h"
#include "wx/qt/converter.h"
#include "wx/qt/private/winevent.h"


class wxQtSlider : public wxQtEventSignalHandler< QSlider, wxSlider >
{
public:
    wxQtSlider( wxWindow *parent, wxSlider *handler );

private:
    void valueChanged(int position);
};

wxQtSlider::wxQtSlider( wxWindow *parent, wxSlider *handler )
    : wxQtEventSignalHandler< QSlider, wxSlider >( parent, handler )
{
    connect(this, &QSlider::valueChanged, this, &wxQtSlider::valueChanged);
}

void wxQtSlider::valueChanged(int position)
{
    wxCommandEvent event( wxEVT_SLIDER, GetHandler()->GetId() );
    event.SetInt( position );
    EmitEvent( event );
}


wxSlider::wxSlider()
{
}

wxSlider::wxSlider(wxWindow *parent,
         wxWindowID id,
         int value, int minValue, int maxValue,
         const wxPoint& pos,
         const wxSize& size,
         long style,
         const wxValidator& validator,
         const wxString& name)
{
    Create( parent, id, value, minValue, maxValue, pos, size, style, validator, name );
}

bool wxSlider::Create(wxWindow *parent,
            wxWindowID id,
            int value, int minValue, int maxValue,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    m_qtWindow = m_qtSlider = new wxQtSlider( parent, this );
    m_qtSlider->setOrientation( wxQtConvertOrientation( style, wxSL_HORIZONTAL ) );
    SetRange( minValue, maxValue );
    // draw ticks marks (default bellow if horizontal, right if vertical):
    if ( style & wxSL_VERTICAL )
    {
        m_qtSlider->setTickPosition( style & wxSL_LEFT ? QSlider::TicksLeft :
                                                         QSlider::TicksRight );
    }
    else // horizontal slider
    {
        m_qtSlider->setTickPosition( style & wxSL_TOP ? QSlider::TicksAbove :
                                                        QSlider::TicksBelow );
    }
    return QtCreateControl( parent, id, pos, size, style, validator, name );
}

int wxSlider::GetValue() const
{
    return m_qtSlider->value();
}

void wxSlider::SetValue(int value)
{
    m_qtSlider->setValue( value );
}

void wxSlider::SetRange(int minValue, int maxValue)
{
    m_qtSlider->setRange( minValue, maxValue );
}

int wxSlider::GetMin() const
{
    return m_qtSlider->minimum();
}

int wxSlider::GetMax() const
{
    return m_qtSlider->maximum();
}

void wxSlider::DoSetTickFreq(int freq)
{
    m_qtSlider->setTickInterval(freq);
}

int wxSlider::GetTickFreq() const
{
    return m_qtSlider->tickInterval();
}

void wxSlider::SetLineSize(int lineSize)
{
}

void wxSlider::SetPageSize(int pageSize)
{
    m_qtSlider->setPageStep(pageSize);
}

int wxSlider::GetLineSize() const
{
    return 0;
}

int wxSlider::GetPageSize() const
{
    return m_qtSlider->pageStep();
}

void wxSlider::SetThumbLength(int lenPixels)
{
}

int wxSlider::GetThumbLength() const
{
    return 0;
}


QSlider *wxSlider::GetHandle() const
{
    return m_qtSlider;
}

