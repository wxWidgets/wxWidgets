/////////////////////////////////////////////////////////////////////////////
// Name:        wallctrl.h
// Purpose:     A wall control that allows easier 3D navigation of visual data
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     02/06/2008
// Copyright:   (c) Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WALLCTRL_H
#define WX_WALLCTRL_H

#include "wx/glcanvas.h"

#include "wx/string.h"
#include "wx/Wallctrl/WallCtrlDataSource.h"
#include "wx/Wallctrl/WallCtrlSurface.h"

#include "wx/wallctrl/WallCtrlNavigator.h"

// TODO: Find out what ID should we use? (Maybe auto-numbered?)
#define TIMER_ID 1

/*
Usage:
	Create wxWallCtrl
	Set a data source for it
	Set a surface for it (automatically gets the data source)
	Set a navigator for it (automatically gets the surface)
*/
class WXDLLEXPORT wxWallCtrl : public wxGLCanvas
{
	DECLARE_DYNAMIC_CLASS(wxWallCtrl)
public:
	// Constructors
	wxWallCtrl(wxWindow *parent,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = 0, const wxString & name = _T("WallCtrl") );

	wxWallCtrl(wxWindow *parent, const wxWallCtrl *other,
		wxWindowID id, const wxPoint& pos, const wxSize& size, long style,
		const wxString& name );

	~wxWallCtrl();

	// Render the control (and all its items)
	void Render();

	// Sets a navigator to control the wxWallCtrl
	void SetNavigator (wxWallCtrlNavigator * navigation = NULL);

	// Sets the data source for the control
	void SetDataSource(wxWallCtrlDataSource * dataSource);

	// Sets the surface for the control
	void SetSurface(wxWallCtrlSurface * surface);

	// Basic event handlers
	void OnEnterWindow( wxMouseEvent& WXUNUSED(event) );
	void OnPaint( wxPaintEvent& WXUNUSED(event) );
	void OnSize(wxSizeEvent& event);
	void OnEraseBackground(wxEraseEvent& WXUNUSED(event));

	// The timer handler, we wake up the idle handler here
	void OnTimer(wxTimerEvent& event);

	// The idle handler, we do our rendering here when needed
	void OnIdle(wxIdleEvent& event);

private:	
	// This is the data source that will supply the bitmaps. Mainly used for clean up
	wxWallCtrlDataSource * m_dataSource;

	// If true, we will delete the data source
	bool m_ownDataSource;

	// A pointer to our navigator used to update its surface
	wxWallCtrlNavigator * m_navigator;

	// This will point to the single surface that is currently selected.
	wxWallCtrlSurface * m_surface;

	// Timer used for rendering
	wxTimer m_timer;

	// Specifies whether we can render when idle or not
	bool m_canRender;	

	// True when a navigation has been set
	bool navigationSet;

	DECLARE_EVENT_TABLE();
};
#endif