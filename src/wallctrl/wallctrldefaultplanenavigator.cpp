/////////////////////////////////////////////////////////////////////////////
// Name:        wallctrldefaultplanenavigation.cpp
// Purpose:     A concrete navigation object for plane surface
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     14/06/2008
// Copyright:   (c) Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#include "wx/wallctrl/WallCtrlDefaultPlaneNavigator.h"

// TODO: What's wrong here?
IMPLEMENT_CLASS(wxWallCtrlDefaultPlaneNavigator, wxWallCtrlNavigator)

// Declare the event table
BEGIN_EVENT_TABLE(wxWallCtrlDefaultPlaneNavigator, wxWallCtrlNavigator)
	//    EVT_SIZE(wxWallCtrl::OnSize)
	//	EVT_PAINT(wxWallCtrl::OnPaint)
	//   EVT_ERASE_BACKGROUND(wxWallCtrl::OnEraseBackground)
	EVT_KEY_DOWN( wxWallCtrlDefaultPlaneNavigator::OnKeyDown )
	//EVT_CHAR( wxWallCtrlDefaultPlaneNavigation::OnKeyDown )
	//    EVT_KEY_UP( wxWallCtrl::OnKeyUp )
	//	EVT_ENTER_WINDOW( wxWallCtrl::OnEnterWindow )
END_EVENT_TABLE()



void wxWallCtrlDefaultPlaneNavigator::OnKeyDown( wxKeyEvent &event )
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
	case WXK_HOME:
		// TODO: Should go to first element, not necessarily 0
		m_surface->Seek(0);
		break;
	case WXK_LEFT:
		m_surface->SeekLeft();
		//m_surface->MoveLeft(0.1);
		break;
	case WXK_RIGHT:
		m_surface->SeekRight();
		//m_surface->MoveRight(0.1);
		break;
	case WXK_UP:
		m_surface->SeekUp();
		//m_surface->MoveIn(0.1);
		break;
	case WXK_DOWN:
		m_surface->SeekDown();
		//m_surface->MoveOut(0.1);
		break;
	case WXK_END:
		m_surface->LoadNextLayerItemTexture();
		break;
	}
}
wxWallCtrlDefaultPlaneNavigator::wxWallCtrlDefaultPlaneNavigator()// wxWallCtrlPlaneSurface * surface ) :m_surface(surface)
{
	
}

wxWallCtrlDefaultPlaneNavigator::~wxWallCtrlDefaultPlaneNavigator(void)
{
}

void wxWallCtrlDefaultPlaneNavigator::ChangeSelection()
{
	//wxWallCtrlNavigationEvent event( wxEVT_WALLCTRL_SELECTION_CHANGED, GetId() );
	wxWallCtrlNavigatorEvent event( wxEVT_WALLCTRL_SELECTION_CHANGED, 0 );
	event.SetEventObject( this );
	// Set event details here
	ProcessEvent( event );
	
}