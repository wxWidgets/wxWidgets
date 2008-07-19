/////////////////////////////////////////////////////////////////////////////
// Name:        wallctrldefaultplanenavigation.h
// Purpose:     A concrete navigation object for plane surface
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     14/06/2008
// Copyright:   (c) Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WALLCTRLDEFAULTPLANENAVIGATION_H
#define WX_WALLCTRLDEFAULTPLANENAVIGATION_H

#include "wx/wallctrl/WallCtrlNavigation.h"
#include "wx/wallctrl/wallctrlplanesurface.h"
#include "wx/wallctrl/wallctrlnavigationevent.h"

class wxWallCtrlDefaultPlaneNavigation :
	public wxWallCtrlNavigation
{
	DECLARE_DYNAMIC_CLASS(wxWallCtrlDefaultPlaneNavigation);
public:
	wxWallCtrlDefaultPlaneNavigation();
	virtual ~wxWallCtrlDefaultPlaneNavigation(void);

	void OnKeyDown(wxKeyEvent &event);

	// Raises an activation event
/*	bool Activate()
	{
		wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
		event.SetEventObject(this);

		return ProcessCommand(event);
	}
*/
	void ChangeSelection();


	virtual wxWallCtrlSurface * GetSurface() const
	{
		return m_surface;
	}

	virtual void SetSurface(wxWallCtrlSurface * surface)
	{
		// TODO: What should we do if the surface is invalid? Probably nothing
		m_surface = wxDynamicCast(surface, wxWallCtrlPlaneSurface);
	}

	DECLARE_EVENT_TABLE();

protected:
	wxWallCtrlPlaneSurface * m_surface;
};

#endif