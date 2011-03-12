/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/scrolbar.cpp
// Author:      Peter Most, Javier Torres
// Id:          $Id$
// Copyright:   (c) Peter Most, Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/scrolbar.h"
#include "wx/qt/utils.h"

wxScrollBar::wxScrollBar()
{
}

wxScrollBar::wxScrollBar( wxWindow *parent, wxWindowID id,
       const wxPoint& pos,
       const wxSize& size,
       long style,
       const wxValidator& validator,
       const wxString& name)
{
    Create( parent, id, pos, size, style, validator, name );
}

bool wxScrollBar::Create( wxWindow *parent, wxWindowID id,
       const wxPoint& pos,
       const wxSize& size,
       long style,
       const wxValidator& validator,
       const wxString& name)
{
    m_qtScrollBar = new wxQtScrollBar( parent, this );
    m_qtScrollBar->setOrientation( wxQtConvertOrientation( style, wxSB_HORIZONTAL ));

    return QtCreateControl( parent, id, pos, size, style, validator, name );
}

int wxScrollBar::GetThumbPosition() const
{
    wxCHECK_MSG( m_qtScrollBar, 0, "Invalid QScrollbar" );

    return m_qtScrollBar->value();
}

int wxScrollBar::GetThumbSize() const
{
    wxCHECK_MSG( m_qtScrollBar, 0, "Invalid QScrollbar" );

    return m_qtScrollBar->pageStep();
}

int wxScrollBar::GetPageSize() const
{
    wxCHECK_MSG( m_qtScrollBar, 0, "Invalid QScrollbar" );

    return m_qtScrollBar->pageStep();
}

int wxScrollBar::GetRange() const
{
    wxCHECK_MSG( m_qtScrollBar, 0, "Invalid QScrollbar" );

    return m_qtScrollBar->maximum();
}

void wxScrollBar::SetThumbPosition(int viewStart)
{
    wxCHECK_RET( m_qtScrollBar, "Invalid QScrollbar" );

    m_qtScrollBar->setValue( viewStart );
}

void wxScrollBar::SetScrollbar(int position, int thumbSize,
                          int range, int pageSize,
                          bool refresh)
{
    wxCHECK_RET( m_qtScrollBar, "Invalid QScrollbar" );

    // Configure the scrollbar
    if (range != 0 )
    {
        m_qtScrollBar->setRange( 0, range - pageSize );
        m_qtScrollBar->setPageStep( pageSize );
        m_qtScrollBar->setValue( position );
        m_qtScrollBar->show();
    }
    else
    {
        // If range is zero, hide it
        m_qtScrollBar->hide();
    }
}

QScrollBar *wxScrollBar::GetHandle() const
{
    return m_qtScrollBar;
}

/////////////////////////////////////////////////////////////////////////////
// wxQtScrollBar
/////////////////////////////////////////////////////////////////////////////

wxQtScrollBar::wxQtScrollBar( wxWindow *parent, wxScrollBar *handler )
    : wxQtEventSignalHandler< QScrollBar, wxScrollBar >( parent, handler )
{
    connect( this, SIGNAL( actionTriggered(int) ), this, SLOT( OnActionTriggered(int) ) );
    connect( this, SIGNAL( sliderReleased() ), this, SLOT( OnSliderReleased() ) );
    connect( this, SIGNAL( valueChanged(int) ), this, SLOT( OnValueChanged(int) ) );
}

void wxQtScrollBar::OnActionTriggered( int action )
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
        case QAbstractSlider::SliderMove:
            eventType = wxEVT_SCROLL_THUMBTRACK;
            break;
        default:
            return;
    }
    
    wxScrollBar *handler = GetHandler();
    wxScrollEvent e( eventType, handler->GetId(), sliderPosition(),
            wxQtConvertOrientation( orientation() ));
                        
    EmitEvent( e );
}

void wxQtScrollBar::OnSliderReleased()
{
    wxScrollBar *handler = GetHandler();
    wxScrollEvent e( wxEVT_SCROLL_THUMBRELEASE, handler->GetId(), sliderPosition(),
            wxQtConvertOrientation( orientation() ));
                        
    EmitEvent( e );
}

void wxQtScrollBar::OnValueChanged( int position )
{
    wxScrollBar *handler = GetHandler();
    wxScrollEvent e( wxEVT_SCROLL_CHANGED, handler->GetId(), position,
            wxQtConvertOrientation( orientation() ));
                     
    EmitEvent( e );
}
