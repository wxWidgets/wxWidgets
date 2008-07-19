/////////////////////////////////////////////////////////////////////////////
// Name:        wallctrlnavigationevent.h
// Purpose:     Represents wxWallCtrl navigation event
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     16/06/2008
// Copyright:   (c) Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WALLCTRLNAVIGATIONEVENT_H
#define WX_WALLCTRLNAVIGATIONEVENT_H

#include "wx/event.h"

class wxWallCtrlNavigationEvent :
	public wxNotifyEvent
{
	DECLARE_DYNAMIC_CLASS(wxWallCtrlNavigationEvent)
public:
	wxWallCtrlNavigationEvent( wxEventType commandType = wxEVT_NULL, int id = 0 );
	virtual ~wxWallCtrlNavigationEvent(void);

	// required for sending with wxPostEvent(). We'll see about that later
	//virtual wxEvent *Clone() const;

};

DECLARE_EVENT_TYPE( wxEVT_WALLCTRL_SELECTION_CHANGED, -1 )

typedef void (wxEvtHandler::*wxSelectionChangedEventFunction)(wxWallCtrlNavigationEvent&);

#define EVT_SELECT(id, fn) \
	DECLARE_EVENT_TABLE_ENTRY( wxEVT_WALLCTRL_SELECTION_CHANGED, id, -1, \
	(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
	wxStaticCastEvent( wxSelectionChangedEventFunction, & fn ), (wxObject *) NULL ),


#endif


/*
// code defining event

class wxPlotEvent: public wxNotifyEvent
{
public:
wxPlotEvent( wxEventType commandType = wxEVT_NULL, int id = 0 );

// accessors
wxPlotCurve *GetCurve()
{ return m_curve; }

// required for sending with wxPostEvent()
virtual wxEvent *Clone() const;

private:
wxPlotCurve   *m_curve;
};

DECLARE_EVENT_TYPE( wxEVT_PLOT_ACTION, -1 )

typedef void (wxEvtHandler::*wxPlotEventFunction)(wxPlotEvent&);

#define EVT_PLOT(id, fn) \
DECLARE_EVENT_TABLE_ENTRY( wxEVT_PLOT_ACTION, id, -1, \
(wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction) (wxNotifyEventFunction) \
wxStaticCastEvent( wxPlotEventFunction, & fn ), (wxObject *) NULL ),


// code implementing the event type and the event class

DEFINE_EVENT_TYPE( wxEVT_PLOT_ACTION )

wxPlotEvent::wxPlotEvent( ...


// user code intercepting the event

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_PLOT  (ID_MY_WINDOW,  MyFrame::OnPlot)
END_EVENT_TABLE()

void MyFrame::OnPlot( wxPlotEvent &event )
{
wxPlotCurve *curve = event.GetCurve();
}


// user code sending the event

void MyWindow::SendEvent()
{
wxPlotEvent event( wxEVT_PLOT_ACTION, GetId() );
event.SetEventObject( this );
event.SetCurve( m_curve );
GetEventHandler()->ProcessEvent( event );
}

*/