/////////////////////////////////////////////////////////////////////////////
// Name:        wallctrldefaultplanenavigation.cpp
// Purpose:     A concrete navigation object for plane surface
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     14/06/2008
// Copyright:   (c) Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#include "wx/wallctrl/WallCtrlDefaultPlaneNavigation.h"

// TODO: What's wrong here?
IMPLEMENT_CLASS(wxWallCtrlDefaultPlaneNavigation, wxWallCtrlNavigation)

// Declare the event table
BEGIN_EVENT_TABLE(wxWallCtrlDefaultPlaneNavigation, wxWallCtrlNavigation)
	//    EVT_SIZE(wxWallCtrl::OnSize)
	//	EVT_PAINT(wxWallCtrl::OnPaint)
	//   EVT_ERASE_BACKGROUND(wxWallCtrl::OnEraseBackground)
	EVT_KEY_DOWN( wxWallCtrlDefaultPlaneNavigation::OnKeyDown )
	//EVT_CHAR( wxWallCtrlDefaultPlaneNavigation::OnKeyDown )
	//    EVT_KEY_UP( wxWallCtrl::OnKeyUp )
	//	EVT_ENTER_WINDOW( wxWallCtrl::OnEnterWindow )
END_EVENT_TABLE()



void wxWallCtrlDefaultPlaneNavigation::OnKeyDown( wxKeyEvent &event )
{
	//wxWallCtrlPlaneSurface * temp = wxDynamicCast(m_surface, wxWallCtrlPlaneSurface);
	//wxWallCtrlPlaneSurface * m_surface = dynamic_cast<wxWallCtrlPlaneSurface *> (m_surface);
	if (!m_surface)
	{
		// TODO: See if we need a different error signaling approach
		return;
	}
	switch (event.GetKeyCode())
	{
	case WXK_LEFT:
		m_surface->MoveLeft(0.1);
		break;
	case WXK_RIGHT:
		m_surface->MoveRight(0.1);
		break;
	case WXK_UP:
		m_surface->MoveIn(0.1);
		break;
	case WXK_DOWN:
		m_surface->MoveOut(0.1);
		break;
	}
}
wxWallCtrlDefaultPlaneNavigation::wxWallCtrlDefaultPlaneNavigation()// wxWallCtrlPlaneSurface * surface ) :m_surface(surface)
{
	
}

wxWallCtrlDefaultPlaneNavigation::~wxWallCtrlDefaultPlaneNavigation(void)
{
}
