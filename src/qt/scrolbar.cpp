/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/scrolbar.cpp
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SCROLLBAR

#include "wx/scrolbar.h"
#include "wx/qt/private/utils.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QScrollBar>

class wxQtScrollBar : public wxQtEventSignalHandler< QScrollBar, wxScrollBar >
{

    public:
        wxQtScrollBar( wxWindow *parent, wxScrollBar *handler );

    private:
        void actionTriggered( int action );
        void sliderReleased();
        void valueChanged( int position );
};


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
    m_qtWindow = new wxQtScrollBar( parent, this );

    GetQScrollBar()->setOrientation( wxQtConvertOrientation( style, wxSB_HORIZONTAL ));

    return wxScrollBarBase::Create( parent, id, pos, size, style, validator, name );
}

QScrollBar* wxScrollBar::GetQScrollBar() const
{
    return static_cast<QScrollBar*>(m_qtWindow);
}

int wxScrollBar::GetThumbPosition() const
{
    wxCHECK_MSG( GetHandle(), 0, "Invalid QScrollbar" );

    return GetQScrollBar()->value();
}

int wxScrollBar::GetThumbSize() const
{
    wxCHECK_MSG( GetHandle(), 0, "Invalid QScrollbar" );

    return GetQScrollBar()->pageStep();
}

int wxScrollBar::GetPageSize() const
{
    wxCHECK_MSG( GetHandle(), 0, "Invalid QScrollbar" );

    return GetQScrollBar()->pageStep();
}

int wxScrollBar::GetRange() const
{
    wxCHECK_MSG( GetHandle(), 0, "Invalid QScrollbar" );

    return GetQScrollBar()->maximum();
}

void wxScrollBar::SetThumbPosition(int viewStart)
{
    wxCHECK_RET( GetHandle(), "Invalid QScrollbar" );

    GetQScrollBar()->setValue( viewStart );
}

void wxScrollBar::SetScrollbar(int position, int WXUNUSED(thumbSize),
                          int range, int pageSize,
                          bool WXUNUSED(refresh))
{
    wxCHECK_RET( GetHandle(), "Invalid QScrollbar" );

    // Configure the scrollbar
    if (range != 0 )
    {
        GetQScrollBar()->setRange( 0, range - pageSize );
        GetQScrollBar()->setPageStep( pageSize );
        {
            wxQtEnsureSignalsBlocked blocker(GetQScrollBar());
            GetQScrollBar()->setValue( position );
        }
        GetQScrollBar()->show();
    }
    else
    {
        // If range is zero, hide it
        GetQScrollBar()->hide();
    }
}

/////////////////////////////////////////////////////////////////////////////
// wxQtScrollBar
/////////////////////////////////////////////////////////////////////////////

wxQtScrollBar::wxQtScrollBar( wxWindow *parent, wxScrollBar *handler )
    : wxQtEventSignalHandler< QScrollBar, wxScrollBar >( parent, handler )
{
    connect( this, &QScrollBar::actionTriggered, this, &wxQtScrollBar::actionTriggered );
    connect( this, &QScrollBar::sliderReleased, this, &wxQtScrollBar::sliderReleased );
    connect( this, &QScrollBar::valueChanged, this, &wxQtScrollBar::valueChanged );
}


void wxQtScrollBar::actionTriggered( int action )
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
    if ( handler )
    {
        wxScrollEvent e( eventType, handler->GetId(), sliderPosition(),
                wxQtConvertOrientation( orientation() ));
        EmitEvent( e );
    }
}

void wxQtScrollBar::sliderReleased()
{
    wxScrollBar *handler = GetHandler();
    if ( handler )
    {
        wxScrollEvent e( wxEVT_SCROLL_THUMBRELEASE, handler->GetId(), sliderPosition(),
                wxQtConvertOrientation( orientation() ));
        EmitEvent( e );
    }
}

void wxQtScrollBar::valueChanged( int position )
{
    wxScrollBar *handler = GetHandler();
    if ( handler )
    {
        wxScrollEvent e( wxEVT_SCROLL_CHANGED, handler->GetId(), position,
                wxQtConvertOrientation( orientation() ));
        EmitEvent( e );
    }
}

#endif // wxUSE_SCROLLBAR
