/////////////////////////////////////////////////////////////////////////////
// Name:        wallctrlnavigatorevent.cpp
// Purpose:     Represents wxWallCtrl navigation event
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     16/06/2008
// Copyright:   (c) Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#include "wx/wallctrl/wallctrlnavigatorevent.h"

IMPLEMENT_CLASS(wxWallCtrlNavigatorEvent, wxNotifyEvent)


DEFINE_EVENT_TYPE( wxEVT_WALLCTRL_SELECTION_CHANGED )


wxWallCtrlNavigatorEvent::wxWallCtrlNavigatorEvent( wxEventType commandType /*= wxEVT_NULL*/, int id /*= 0 */ ):
  wxNotifyEvent(commandType, id)
{
	m_index = 0;
}
wxWallCtrlNavigatorEvent::~wxWallCtrlNavigatorEvent( void )
{

}

unsigned wxWallCtrlNavigatorEvent::GetSelectedIndex() const
{
	return m_index;
}

void wxWallCtrlNavigatorEvent::SetSelectedIndex( unsigned index )
{
	m_index = index;
}