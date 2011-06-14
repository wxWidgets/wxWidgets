/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_navctrl_n.mm
// Purpose:     wxMoNavigationCtrl class
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/dcclient.h"
#include "wx/dcscreen.h"

#include "wx/mobile/native/navctrl.h"
#include "wx/mobile/native/utils.h"

extern WXDLLEXPORT_DATA(const wxChar) wxNavigationCtrlNameStr[] = wxT("NavigationCtrl");

IMPLEMENT_DYNAMIC_CLASS(wxMoNavigationCtrl, wxControl)

BEGIN_EVENT_TABLE(wxMoNavigationCtrl, wxControl)
    EVT_SIZE(wxMoNavigationCtrl::OnSize)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxNavigationCtrlEvent, wxNotifyEvent)

DEFINE_EVENT_TYPE(wxEVT_COMMAND_NAVCTRL_POPPED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NAVCTRL_POPPING)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NAVCTRL_PUSHED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NAVCTRL_PUSHING)

bool wxMoNavigationCtrl::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    // FIXME stub

    return true;
}

wxMoNavigationCtrl::~wxMoNavigationCtrl()
{
    // FIXME stub
}

void wxMoNavigationCtrl::Init()
{
    // FIXME stub
}

wxSize wxMoNavigationCtrl::DoGetBestSize() const
{
    // FIXME stub

	wxSize empty(1, 1);
	return empty;
}

bool wxMoNavigationCtrl::SetBackgroundColour(const wxColour &colour)
{
    // FIXME stub

    return true;
}

bool wxMoNavigationCtrl::SetForegroundColour(const wxColour &colour)
{
    // FIXME stub

    return true;
}

bool wxMoNavigationCtrl::Enable(bool enable)
{
    // FIXME stub

    return true;
}

bool wxMoNavigationCtrl::SetFont(const wxFont& font)
{
    // FIXME stub

    return true;
}

void wxMoNavigationCtrl::OnBack(wxCommandEvent& WXUNUSED(event))
{
    // FIXME stub
}

void wxMoNavigationCtrl::OnSize(wxSizeEvent& WXUNUSED(event))
{
    // FIXME stub
}

// Pushes a controller onto the stack.
bool wxMoNavigationCtrl::PushController(wxMoViewController* controller)
{
    // FIXME stub
    
    return true;
}

// Pops an item off the stack. The item must then be deleted or stored by the application.
wxMoViewController* wxMoNavigationCtrl::PopController()
{
    // FIXME stub

    return NULL;
}

// Returns the top item.
wxMoViewController* wxMoNavigationCtrl::GetTopController() const
{
    // FIXME stub

	return NULL;
}

// Returns the back item.
wxMoViewController* wxMoNavigationCtrl::GetBackController() const
{
    // FIXME stub

	return NULL;
}

// Sets the item stack.
void wxMoNavigationCtrl::SetControllers(const wxMoViewControllerArray& controllers)
{
    // FIXME stub
}

// Clears the item stack, deleting the items.
void wxMoNavigationCtrl::ClearControllers()
{
    // FIXME stub
}

void wxMoNavigationCtrl::PositionItems()
{
    // FIXME stub
}

// Restores the display
void wxMoNavigationCtrl::Thaw()
{
    // FIXME stub
}
