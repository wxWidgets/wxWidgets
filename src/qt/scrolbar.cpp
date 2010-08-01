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

IMPLEMENT_DYNAMIC_CLASS( wxScrollBar, wxScrollBarBase )

wxScrollBar::wxScrollBar()
{
}

wxScrollBar::~wxScrollBar()
{
    delete m_qtScrollBar;
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
    QWidget *qtParent = NULL;
    if ( parent != NULL ) {
        qtParent = parent->QtGetContainer();
        parent->AddChild(this);
    }
    m_qtScrollBar = new wxQtScrollBar( this, wxQtConvertOrientation( style, wxSB_HORIZONTAL ),
            qtParent );
    
    return wxControl::Create( parent, id, pos, size, style, validator, name );
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

WXWidget wxScrollBar::QtGetScrollBarsContainer() const
{
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// wxQtScrollBar
/////////////////////////////////////////////////////////////////////////////

wxQtScrollBar::wxQtScrollBar( wxScrollBar *scrollBar, Qt::Orientation orient, QWidget *parent )
    : QScrollBar( orient, parent ),
      wxQtSignalForwarder< wxScrollBar >( scrollBar )
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
    
    wxScrollBar *scrollBar = GetSignalHandler();
    wxScrollEvent e( eventType, scrollBar->GetId(), sliderPosition(),
            wxQtConvertOrientation( orientation() ));
                        
    scrollBar->ProcessWindowEvent(e);
}

void wxQtScrollBar::OnSliderReleased()
{
    wxScrollBar *scrollBar = GetSignalHandler();
    wxScrollEvent e( wxEVT_SCROLL_THUMBRELEASE, scrollBar->GetId(), sliderPosition(),
            wxQtConvertOrientation( orientation() ));
                        
    scrollBar->ProcessWindowEvent(e);
}

void wxQtScrollBar::OnValueChanged( int position )
{
    wxScrollBar *scrollBar = GetSignalHandler();
    wxScrollEvent e( wxEVT_SCROLL_CHANGED, scrollBar->GetId(), position,
            wxQtConvertOrientation( orientation() ));
                     
    scrollBar->ProcessWindowEvent(e);
}
