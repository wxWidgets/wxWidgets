/////////////////////////////////////////////////////////////////////////////
// Name:        mdi.cpp
// Purpose:     MDI classes
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "mdi.h"
#endif

#include "wx/mdi.h"

extern wxList wxModelessWindows;

IMPLEMENT_DYNAMIC_CLASS(wxMDIParentFrame, wxFrame)
IMPLEMENT_DYNAMIC_CLASS(wxMDIChildFrame, wxFrame)
IMPLEMENT_DYNAMIC_CLASS(wxMDIClientWindow, wxWindow)

BEGIN_EVENT_TABLE(wxMDIParentFrame, wxFrame)
  EVT_SIZE(wxMDIParentFrame::OnSize)
  EVT_ACTIVATE(wxMDIParentFrame::OnActivate)
  EVT_SYS_COLOUR_CHANGED(wxMDIParentFrame::OnSysColourChanged)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxMDIClientWindow, wxWindow)
  EVT_SCROLL(wxMDIClientWindow::OnScroll)
END_EVENT_TABLE()


// Parent frame

wxMDIParentFrame::wxMDIParentFrame()
{
}

bool wxMDIParentFrame::Create(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    if (!parent)
        wxTopLevelWindows.Append(this);

    SetName(name);
    m_windowStyle = style;

    if (parent) parent->AddChild(this);

    if ( id > -1 )
        m_windowId = id;
    else
        m_windowId = (int)NewControlId();

    // TODO: create MDI parent frame

    wxModelessWindows.Append(this);

    return TRUE;
}

wxMDIParentFrame::~wxMDIParentFrame()
{
}

// Get size *available for subwindows* i.e. excluding menu bar.
void wxMDIParentFrame::GetClientSize(int *x, int *y) const
{
    // TODO
}

void wxMDIParentFrame::SetMenuBar(wxMenuBar *menu_bar)
{
    // TODO
    if (!menu_bar)
    {
        m_frameMenuBar = NULL;
        return;
    }
  
    if (menu_bar->m_menuBarFrame)
	    return;

    m_frameMenuBar = menu_bar;
}

void wxMDIParentFrame::OnSize(wxSizeEvent& event)
{
#if wxUSE_CONSTRAINTS
    if (GetAutoLayout())
      Layout();
#endif
    int x = 0;
    int y = 0;
    int width, height;
    GetClientSize(&width, &height);

    if ( GetClientWindow() )
        GetClientWindow()->SetSize(x, y, width, height);
}

void wxMDIParentFrame::OnActivate(wxActivateEvent& event)
{
	// Do nothing
}

// Returns the active MDI child window
wxMDIChildFrame *wxMDIParentFrame::GetActiveChild() const
{
    // TODO
    return NULL;
}

// Create the client window class (don't Create the window,
// just return a new class)
wxMDIClientWindow *wxMDIParentFrame::OnCreateClient()
{
	return new wxMDIClientWindow ;
}

// Responds to colour changes, and passes event on to children.
void wxMDIParentFrame::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    // TODO

    // Propagate the event to the non-top-level children
    wxFrame::OnSysColourChanged(event);
}

// MDI operations
void wxMDIParentFrame::Cascade()
{
    // TODO
}

void wxMDIParentFrame::Tile()
{
    // TODO
}

void wxMDIParentFrame::ArrangeIcons()
{
    // TODO
}

void wxMDIParentFrame::ActivateNext()
{
    // TODO
}

void wxMDIParentFrame::ActivatePrevious()
{
    // TODO
}

// Child frame

wxMDIChildFrame::wxMDIChildFrame()
{
}

bool wxMDIChildFrame::Create(wxMDIParentFrame *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    SetName(name);

    if ( id > -1 )
        m_windowId = id;
    else
        m_windowId = (int)NewControlId();

    if (parent) parent->AddChild(this);

    // TODO: create child frame

    wxModelessWindows.Append(this);
    return FALSE;
}

wxMDIChildFrame::~wxMDIChildFrame()
{
}

// Set the client size (i.e. leave the calculation of borders etc.
// to wxWindows)
void wxMDIChildFrame::SetClientSize(int width, int height)
{
    // TODO
}

void wxMDIChildFrame::GetPosition(int *x, int *y) const
{
    // TODO
}

void wxMDIChildFrame::SetMenuBar(wxMenuBar *menu_bar)
{
    // TODO
    if (!menu_bar)
    {
        m_frameMenuBar = NULL;
        return;
    }
  
    if (menu_bar->m_menuBarFrame)
	    return;
    m_frameMenuBar = menu_bar;
}

// MDI operations
void wxMDIChildFrame::Maximize()
{
    // TODO
}

void wxMDIChildFrame::Restore()
{
    // TODO
}

void wxMDIChildFrame::Activate()
{
    // TODO
}

// Client window

wxMDIClientWindow::wxMDIClientWindow()
{
}

wxMDIClientWindow::~wxMDIClientWindow()
{
}

bool wxMDIClientWindow::CreateClient(wxMDIParentFrame *parent, long style)
{
    // TODO create client window
    m_backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_APPWORKSPACE);

    return FALSE;
}

// Explicitly call default scroll behaviour
void wxMDIClientWindow::OnScroll(wxScrollEvent& event)
{
    Default(); // Default processing
}

