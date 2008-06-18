#pragma once
#include "wx/event.h"

class wxWallCtrlNavigationEvent :
	public wxNotifyEvent
{
public:
	wxWallCtrlNavigationEvent(void);
	virtual ~wxWallCtrlNavigationEvent(void);
};
