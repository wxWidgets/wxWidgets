#pragma once
#include "wx\event.h"

class wxWallCtrlCommandEvent :
	public wxNotifyEvent
{
public:
	wxWallCtrlCommandEvent(void);
public:
	virtual ~wxWallCtrlCommandEvent(void);
};
