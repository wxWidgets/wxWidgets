/////////////////////////////////////////////////////////////////////////////
// Name:        wallctrldefaultplanenavigation.h
// Purpose:     A concrete navigation object for plane surface
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     14/06/2008
// Copyright:   (c) Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WALLCTRLDEFAULTPLANENAVIGATOR_H
#define WX_WALLCTRLDEFAULTPLANENAVIGATOR_H

#include "wx/wallctrl/WallCtrlNavigator.h"
#include "wx/wallctrl/wallctrlplanesurface.h"
#include "wx/wallctrl/wallctrlnavigatorevent.h"

class wxWallCtrlDefaultPlaneNavigator :
	public wxWallCtrlNavigator
{
	DECLARE_DYNAMIC_CLASS(wxWallCtrlDefaultPlaneNavigator);
public:
	wxWallCtrlDefaultPlaneNavigator();
	virtual ~wxWallCtrlDefaultPlaneNavigator(void);

	void OnKeyDown(wxKeyEvent &event);

	// Raises an activation event
	void ChangeSelection();


	virtual wxWallCtrlSurface * GetSurface() const;

	virtual void SetSurface(wxWallCtrlSurface * surface);

	DECLARE_EVENT_TABLE();

protected:
	wxWallCtrlPlaneSurface * m_surface;
};

#endif