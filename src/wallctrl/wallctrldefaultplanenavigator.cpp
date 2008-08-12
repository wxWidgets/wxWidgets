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
	if (!m_surface)
	{
		wxLogError(wxT("Navigator does not have a valid surface"));
		return;
	}
	// Switch on the pressed key and seek accordingly
	switch (event.GetKeyCode())
	{
	case WXK_HOME:
		m_surface->Seek(m_surface->GetDataSource()->GetFirstItem());
		ChangeSelection();
		break;
	case WXK_LEFT:
		m_surface->SeekLeft();
		ChangeSelection();
		break;
	case WXK_RIGHT:
		m_surface->SeekRight();
		ChangeSelection();
		break;
	case WXK_UP:
		m_surface->SeekUp();
		ChangeSelection();
		break;
	case WXK_DOWN:
		m_surface->SeekDown();
		ChangeSelection();
		break;
	}
}
wxWallCtrlDefaultPlaneNavigator::wxWallCtrlDefaultPlaneNavigator()// wxWallCtrlPlaneSurface * surface ) :m_surface(surface)
{
	m_surface = NULL;
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
	if (!ProcessEvent( event ))
		if (!m_surface->ProcessEvent( event))
			m_surface->GetParent()->ProcessEvent(event);
}

wxWallCtrlSurface * wxWallCtrlDefaultPlaneNavigator::GetSurface() const
{
	return m_surface;
}

void wxWallCtrlDefaultPlaneNavigator::SetSurface( wxWallCtrlSurface * surface )
{

	// If we have a previous surface
	if (m_surface)
	{
		// TODO: We need to see if we should delete the old surface as well
//		PopEventHandler();
		m_surface = NULL;
	}

	// TODO: What should we do if the surface is invalid? Probably nothing
	m_surface = wxDynamicCast(surface, wxWallCtrlPlaneSurface);

	if (m_surface)
	{
//		PushEventHandler(m_surface);
	}
}