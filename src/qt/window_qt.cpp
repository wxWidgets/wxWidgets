/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/window_qt.cpp
// Author:      Javier Torres
// Id:          $Id$
// Copyright:   (c) Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/qt/window_qt.h"

wxQtWidget::wxQtWidget( wxWindow *window, QWidget *parent )
: wxQtEventForwarder< QWidget >( parent )
{
    m_wxWindow = window;
}

wxWindow *wxQtWidget::GetEventReceiver()
{
    return m_wxWindow;
}

wxQtScrollBarEventForwarder::wxQtScrollBarEventForwarder( wxWindow *window,
                                                          Qt::Orientation orient,
                                                          QWidget *parent )
: QScrollBar( orient, parent )
{
    m_wxWindow = window;
    connect( this, SIGNAL( actionTriggered(int) ), this, SLOT( OnActionTriggered(int) ) );
    connect( this, SIGNAL( sliderReleased() ), this, SLOT( OnSliderReleased() ) );
}

void wxQtScrollBarEventForwarder::OnActionTriggered( int action )
{
    wxEventType wxAction = 0;
    switch( action )
    {
        case QAbstractSlider::SliderSingleStepAdd:
            wxAction = wxEVT_SCROLLWIN_LINEDOWN;
            break;
        case QAbstractSlider::SliderSingleStepSub:
            wxAction = wxEVT_SCROLLWIN_LINEUP;
            break;
        case QAbstractSlider::SliderPageStepAdd:
            wxAction = wxEVT_SCROLLWIN_PAGEDOWN;
            break;
        case QAbstractSlider::SliderPageStepSub:
            wxAction = wxEVT_SCROLLWIN_PAGEUP;
            break;
        case QAbstractSlider::SliderToMinimum:
            wxAction = wxEVT_SCROLLWIN_TOP;
            break;
        case QAbstractSlider::SliderToMaximum:
            wxAction = wxEVT_SCROLLWIN_BOTTOM;
            break;
        case QAbstractSlider::SliderMove:
            wxAction = wxEVT_SCROLLWIN_THUMBTRACK;
            break;
        default:
            return;
    }

    wxScrollWinEvent e( wxAction, sliderPosition(),
                        orientation() == Qt::Horizontal ? wxHORIZONTAL : wxVERTICAL );

    m_wxWindow->ProcessWindowEvent(e);
}

void wxQtScrollBarEventForwarder::OnSliderReleased()
{
    wxScrollWinEvent e( wxEVT_SCROLLWIN_THUMBRELEASE, sliderPosition(),
                        orientation() == Qt::Horizontal ? wxHORIZONTAL : wxVERTICAL );

    m_wxWindow->ProcessWindowEvent(e);
}
