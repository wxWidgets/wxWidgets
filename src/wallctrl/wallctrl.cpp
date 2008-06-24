/////////////////////////////////////////////////////////////////////////////
// Name:        wallctrl.cpp
// Purpose:     A wall control allows easier 3D navigation of data
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     02/06/2008
// Copyright:   (c) Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#include "wx/wx.h"

#if !wxUSE_GLCANVAS
	#error "OpenGL required: set wxUSE_GLCANVAS to 1 and rebuild the library"
#endif


#include "wx/Wallctrl/WallCtrl.h"

// TODO: What's wrong here?
IMPLEMENT_CLASS(wxWallCtrl, wxGLCanvas)

// Declare the event table
BEGIN_EVENT_TABLE(wxWallCtrl, wxGLCanvas)
    EVT_SIZE(wxWallCtrl::OnSize)
EVT_PAINT(wxWallCtrl::OnPaint)
   EVT_ERASE_BACKGROUND(wxWallCtrl::OnEraseBackground)
//	EVT_KEY_DOWN( wxWallCtrl::OnKeyDown )
//    EVT_KEY_UP( wxWallCtrl::OnKeyUp )
	EVT_ENTER_WINDOW( wxWallCtrl::OnEnterWindow )
	EVT_TIMER(TIMER_ID, wxWallCtrl::OnTimer)
	EVT_IDLE(wxWallCtrl::OnIdle)
END_EVENT_TABLE()

	wxWallCtrl::wxWallCtrl( wxWindow *parent,  wxWindowID id /*= wxID_ANY*/, const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/, long style /*= 0*/, const wxString & name /*= _T("WallCtrl") */ ) :wxGLCanvas(parent, (wxGLCanvas*) NULL, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE , name ), m_timer(this, TIMER_ID)
{
	//m_init= false;
	m_timer.Start(16);		// 60 FPS

	m_canRender = false;
	navigationSet = false;
	m_surface = NULL;
	m_dataSource = NULL;
	m_navigator = NULL;
}

wxWallCtrl::wxWallCtrl( wxWindow *parent, const wxWallCtrl *other, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxGLCanvas(parent, other->GetContext(), id, pos, size, style|wxFULL_REPAINT_ON_RESIZE , name), m_timer(this, TIMER_ID)
{
	//m_init=false;
	m_timer.Start(16);		// 60 FPS

	m_canRender = false;
	navigationSet = false;
	m_surface = NULL;
	m_dataSource = NULL;
	m_navigator = NULL;
}

void wxWallCtrl::Render()
{
	// Only render when we have a valid surface
	if (m_surface)
	{
		SetCurrent();
		m_surface->Render(GetSize());
		SwapBuffers();
	}
}

void wxWallCtrl::SetNavigator( wxWallCtrlNavigation * navigator /*= NULL*/)
{
	if (navigationSet)
	{
		// TODO: Check if this frees the memory
		PopEventHandler(true);
		m_navigator = NULL;
	}	

	if (navigator == NULL)
	{
		return;
	}

	// Push the navigation as the current event handler
	PushEventHandler(navigator);

	// Keep a local pointer to this new navigation
	m_navigator = navigator;

	// Give our navigator a surface
	//SetSurface(navigation->GetSurface());
	navigator->SetSurface(m_surface);

	// Flag that we have set a navigation
	navigationSet = true;
}

void wxWallCtrl::OnEnterWindow( wxMouseEvent& WXUNUSED(event) )
{
	SetFocus();
}

void wxWallCtrl::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
	wxPaintDC dc(this);
	Render();
}

void wxWallCtrl::OnSize( wxSizeEvent& event )
{
	// this is also necessary to update the context on some platforms
	wxGLCanvas::OnSize(event);
}

void wxWallCtrl::OnEraseBackground( wxEraseEvent& WXUNUSED(event) )
{
	// Do nothing, to avoid flashing.
}

void wxWallCtrl::SetSurface( wxWallCtrlSurface * surface = NULL)
{
	// Delete the old surface if any
	if (m_surface)
	{
		delete m_surface;
		m_surface = surface;
	}

	// Set the surface
	m_surface = surface;

	// If we have a data source, send it to the surface
	if (m_dataSource)
	{
		m_surface->SetDataSource(m_dataSource);
	}


}

wxWallCtrl::~wxWallCtrl()
{
	if ((m_ownDataSource) && (m_dataSource))
	{
		delete m_dataSource;
	}

	if (m_surface)
	{
		delete m_surface;
	}

	if (m_navigator)
	{
		SetNavigator(NULL);
		delete m_navigator;
	}
}

void wxWallCtrl::OnTimer( wxTimerEvent& event )
{
	m_canRender = true;
	wxWakeUpIdle();
}

void wxWallCtrl::OnIdle( wxIdleEvent& event )
{
	if (m_canRender)
	{
		Refresh();
		//Render();
	}

	// We only want to render when explicitly asked to
	m_canRender = false;
}

void wxWallCtrl::SetDataSource( wxWallCtrlDataSource * dataSource )
{
	// If we have a current data source that we manage, then delete it
	if (m_dataSource && m_ownDataSource)
	{
		delete m_dataSource;
	}
	// Set this data source and flag that we manage it
	m_dataSource = dataSource;
	m_ownDataSource = true;

	// If we have a surface already set, then set its data source
	if (m_surface)
	{
		// Delete the old one if it exists
		if (m_surface->GetDataSource())
		{
			delete (m_surface->GetDataSource());
		}
		m_surface->SetDataSource(m_dataSource);
	}
}