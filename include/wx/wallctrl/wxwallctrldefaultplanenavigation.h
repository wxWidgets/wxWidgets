#pragma once
#include "wx\wallctrl\wxWallCtrlNavigation.h"
#include "wx\wallctrl\wallctrlplanesurface.h"

class wxWallCtrlDefaultPlaneNavigation :
	public wxWallCtrlNavigation
{
public:
	wxWallCtrlDefaultPlaneNavigation(wxWallCtrlPlaneSurface * surface):m_surface(surface)
	{
	}
	virtual ~wxWallCtrlDefaultPlaneNavigation(void);

	void OnKeyDown(wxKeyEvent &event)
	{
		// TODO: wxDynamicCast should be used instead here
		//wxWallCtrlPlaneSurface * temp = wxDynamicCast(m_surface, wxWallCtrlPlaneSurface);
		//wxWallCtrlPlaneSurface * m_surface = dynamic_cast<wxWallCtrlPlaneSurface *> (m_surface);
		if (!m_surface)
		{
			// TODO: See if we need a different error signaling approach
			return;
		}
		switch (event.GetKeyCode())
		{
		case 'A'://WXK_LEFT:
			m_surface->MoveLeft(0.1);
			break;
		case 'D':
			m_surface->MoveRight(0.1);
			break;
		case 'W':
			m_surface->MoveIn(0.1);
			break;
		case 'S':
			m_surface->MoveOut(0.1);
			break;
		}
	}

	DECLARE_EVENT_TABLE();

protected:
	wxWallCtrlPlaneSurface * m_surface;
};
