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


class wxQtSlider : public wxQtEventSignalHandler< QSlider, wxSlider >
{
public:
    wxQtSlider( wxWindow *parent, wxSlider *handler );

private:
    void sliderMoved(int position);
};

wxQtSlider::wxQtSlider( wxWindow *parent, wxSlider *handler )
    : wxQtEventSignalHandler< QSlider, wxSlider >( parent, handler )
{
    connect(this, &QSlider::sliderMoved, this, &wxQtSlider::sliderMoved);
}

void wxQtSlider::sliderMoved(int position)
{
    // TODO: use SendSelectionChangedEvent(wxEVT_COMBOBOX);
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
    m_qtSlider = new wxQtSlider( parent, this );
    m_qtSlider->setOrientation( wxQtConvertOrientation( style, wxSL_HORIZONTAL ) );

    return QtCreateControl( parent, id, pos, size, style, validator, name );
}

int wxSlider::GetValue() const
{
    return 0;
}

void wxSlider::SetValue(int value)
{
}

void wxSlider::SetRange(int minValue, int maxValue)
{
}

int wxSlider::GetMin() const
{
    return 0;
}

int wxSlider::GetMax() const
{
    return 0;
}

void wxSlider::SetLineSize(int lineSize)
{
}

void wxSlider::SetPageSize(int pageSize)
{
}

int wxSlider::GetLineSize() const
{
    return 0;
}

int wxSlider::GetPageSize() const
{
    return 0;
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

