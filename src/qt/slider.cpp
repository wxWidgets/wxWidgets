/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/slider.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SLIDER

#include "wx/slider.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QSlider>

class wxQtSlider : public wxQtEventSignalHandler< QSlider, wxSlider >
{
public:
    wxQtSlider( wxWindow *parent, wxSlider *handler );

private:
    void valueChanged(int position);
    void actionTriggered(int action);

    void sliderPressed();
    void sliderReleased();
};

wxQtSlider::wxQtSlider( wxWindow *parent, wxSlider *handler )
    : wxQtEventSignalHandler< QSlider, wxSlider >( parent, handler )
{
    connect(this, &QSlider::valueChanged, this, &wxQtSlider::valueChanged);
    connect(this, &QSlider::actionTriggered, this, &wxQtSlider::actionTriggered);

    connect(this, &QSlider::sliderPressed, this, &wxQtSlider::sliderPressed);
    connect(this, &QSlider::sliderReleased, this, &wxQtSlider::sliderReleased);
}

void wxQtSlider::valueChanged(int position)
{
    wxSlider *handler = GetHandler();
    if ( handler )
    {
        wxScrollEvent e(wxEVT_SCROLL_CHANGED, handler->GetId(), position,
                        wxQtConvertOrientation( orientation( ) ));
        EmitEvent( e );

        // and also generate a command event for compatibility
        wxCommandEvent event( wxEVT_SLIDER, handler->GetId() );
        event.SetInt( position );
        EmitEvent( event );
    }
}

void wxQtSlider::actionTriggered(int action)
{
    wxEventType eventType = wxEVT_NULL;
    switch( action )
    {
        case QAbstractSlider::SliderSingleStepAdd:
            eventType = wxEVT_SCROLL_LINEDOWN;
            break;
        case QAbstractSlider::SliderSingleStepSub:
            eventType = wxEVT_SCROLL_LINEUP;
            break;
        case QAbstractSlider::SliderPageStepAdd:
            eventType = wxEVT_SCROLL_PAGEDOWN;
            break;
        case QAbstractSlider::SliderPageStepSub:
            eventType = wxEVT_SCROLL_PAGEUP;
            break;
        case QAbstractSlider::SliderToMinimum:
            eventType = wxEVT_SCROLL_TOP;
            break;
        case QAbstractSlider::SliderToMaximum:
            eventType = wxEVT_SCROLL_BOTTOM;
            break;
        /*
        case QAbstractSlider::SliderMove:
            eventType = wxEVT_SCROLL_CHANGED;
            break;
        */
        default:
            return;
    }

    wxSlider *handler = GetHandler();
    if ( handler )
    {
        const int  newPosition = sliderPosition();
        const bool hasValueChanged = handler->GetValue() != newPosition;

        // Event handlers expect the wxSlider to return the new position;
        // and because valueChanged() signal is not emitted yet, it will
        // return the old position not the new one. So we need to update
        // the value here and call the valueChanged() signal manually
        // (because it won't be emitted after we call SetValue()) with
        // the new position if it was really changed,
        handler->SetValue(newPosition);

        wxScrollEvent e( eventType, handler->GetId(), newPosition,
                wxQtConvertOrientation( orientation() ));
        EmitEvent( e );

        if ( hasValueChanged )
        {
            valueChanged(newPosition);
        }
    }
}

void wxQtSlider::sliderPressed()
{
    wxSlider *handler = GetHandler();
    if ( handler )
    {
        wxScrollEvent e( wxEVT_SCROLL_THUMBTRACK,
                         handler->GetId(), sliderPosition(),
                         wxQtConvertOrientation( orientation() ));
        EmitEvent( e );
    }
}

void wxQtSlider::sliderReleased()
{
    wxSlider *handler = GetHandler();
    if ( handler )
    {
        wxScrollEvent e( wxEVT_SCROLL_THUMBRELEASE,
                         handler->GetId(), sliderPosition(),
                         wxQtConvertOrientation( orientation() ));
        EmitEvent( e );
    }
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
    m_qtWindow = new wxQtSlider( parent, this );

    GetQSlider()->setOrientation( wxQtConvertOrientation( style, wxSL_HORIZONTAL ) );

    GetQSlider()->setInvertedAppearance( style & wxSL_INVERSE );

    // For compatibility with the other ports, pressing PageUp should move value
    // towards the slider's minimum.
    GetQSlider()->setInvertedControls(true);

    wxQtEnsureSignalsBlocked blocker(GetQSlider());
    SetRange( minValue, maxValue );
    SetValue( value );
    SetPageSize(wxMax(1, (maxValue - minValue) / 10));

    if ( !wxSliderBase::Create( parent, id, pos, size, style, validator, name ) )
        return false;

    // SetTick() needs the window style which is normally set after wxSliderBase::Create()
    // is called. Pass 0 as tickPos parameter is not used by Qt anyhow.
    SetTick( 0 );

    return true;
}

QSlider* wxSlider::GetQSlider() const
{
    return static_cast<QSlider*>(m_qtWindow);
}

int wxSlider::GetValue() const
{
    return GetQSlider()->value();
}

void wxSlider::SetValue(int value)
{
    wxQtEnsureSignalsBlocked blocker(GetQSlider());
    GetQSlider()->setValue( value );
}

void wxSlider::SetRange(int minValue, int maxValue)
{
    wxQtEnsureSignalsBlocked blocker(GetQSlider());
    GetQSlider()->setRange( minValue, maxValue );
}

int wxSlider::GetMin() const
{
    return GetQSlider()->minimum();
}

int wxSlider::GetMax() const
{
    return GetQSlider()->maximum();
}

void wxSlider::DoSetTickFreq(int freq)
{
    GetQSlider()->setTickInterval(freq);
}

int wxSlider::GetTickFreq() const
{
    return GetQSlider()->tickInterval();
}

void wxSlider::ClearTicks()
{
    GetQSlider()->setTickPosition(QSlider::NoTicks);
}

void wxSlider::SetTick(int WXUNUSED(tickPos))
{
    QSlider::TickPosition posTicks;

    const int style = GetWindowStyle();

    if ( !(style & wxSL_TICKS) &&
         !(style & (wxSL_TOP|wxSL_BOTTOM|wxSL_LEFT|wxSL_RIGHT|wxSL_BOTH)) )
    {
        posTicks = QSlider::NoTicks;
    }
    else if ( style & wxSL_BOTH )
    {
        posTicks = QSlider::TicksBothSides;
    }
    else if ( style & (wxSL_TOP|wxSL_LEFT) )
    {
        // TicksAbove is the same as TicksLeft for vertical slider
        posTicks = QSlider::TicksAbove;
    }
    else // if ( style & (wxSL_BOTTOM|wxSL_RIGHT) )
    {
        // This the default, below if horizontal, right if vertical
        // TicksBelow is the same as TicksRight for vertical slider
        posTicks = QSlider::TicksBelow;
    }

    // Draw ticks marks if posTicks != QSlider::NoTicks. remove them otherwise.
    GetQSlider()->setTickPosition( posTicks );
}

void wxSlider::SetLineSize(int lineSize)
{
    GetQSlider()->setSingleStep(lineSize);
}

void wxSlider::SetPageSize(int pageSize)
{
    GetQSlider()->setPageStep(pageSize);
}

int wxSlider::GetLineSize() const
{
    return GetQSlider()->singleStep();
}

int wxSlider::GetPageSize() const
{
    return GetQSlider()->pageStep();
}

void wxSlider::SetThumbLength(int WXUNUSED(lenPixels))
{
}

int wxSlider::GetThumbLength() const
{
    return 0;
}

#endif // wxUSE_SLIDER
