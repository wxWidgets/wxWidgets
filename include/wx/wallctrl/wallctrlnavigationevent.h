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
	wxWallCtrlNavigationEvent(void);
	virtual ~wxWallCtrlNavigationEvent(void);
};

#endif