/////////////////////////////////////////////////////////////////////////////
// Name:        wallctrlnavigationevent.cpp
// Purpose:     Represents wxWallCtrl navigation event
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     16/06/2008
// Copyright:   (c) Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#include "wx/wallctrl/wallctrlnavigationevent.h"

// TODO: What's wrong here?
IMPLEMENT_CLASS(wxWallCtrlNavigationEvent, wxNotifyEvent)


DEFINE_EVENT_TYPE( wxEVT_WALLCTRL_SELECTION_CHANGED )


wxWallCtrlNavigationEvent::wxWallCtrlNavigationEvent( wxEventType commandType /*= wxEVT_NULL*/, int id /*= 0 */ )
{

}
wxWallCtrlNavigationEvent::~wxWallCtrlNavigationEvent( void )
{

}

/*

// user code intercepting the event

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
  EVT_PLOT  (ID_MY_WINDOW,  MyFrame::OnPlot)
END_EVENT_TABLE()

void MyFrame::OnPlot( wxPlotEvent &event )
{
    wxPlotCurve *curve = event.GetCurve();
}
*/