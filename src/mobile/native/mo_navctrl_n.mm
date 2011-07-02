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

#include "wx/osx/private.h"

#include "wx/osx/iphone/private/navctrlimpl.h"
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


/// Default constructor.
wxMoNavigationCtrl::wxMoNavigationCtrl()
{
    Init();
}

/// Constructor.
wxMoNavigationCtrl::wxMoNavigationCtrl(wxWindow *parent,
                                       wxWindowID id,
                                       const wxPoint& pos,
                                       const wxSize& size,
                                       long style,
                                       const wxValidator& validator,
                                       const wxString& name)
{
    Init();
    Create(parent, id, pos, size, style, validator, name);
}

wxMoNavigationCtrl::~wxMoNavigationCtrl()
{
    ClearControllers();
}

void wxMoNavigationCtrl::Init()
{
    m_navBar = NULL;
}

/// Creation function.
bool wxMoNavigationCtrl::Create(wxWindow *parent,
                                wxWindowID id,
                                const wxPoint& pos,
                                const wxSize& size,
                                long style,
                                const wxValidator& validator,
                                const wxString& name)
{
    DontCreatePeer();
    
    if ( !wxControl::Create( parent, id, pos, size, style, validator, name )) {
        return false;
    }
    
    SetPeer(wxWidgetImpl::CreateNavigationController( this, parent, id, pos, size, style, GetExtraStyle() ));
    
    MacPostControlCreate( pos, size );
    
    return true;    
}

// Pushes a controller onto the stack.
bool wxMoNavigationCtrl::PushController(wxMoViewController* controller)
{
    {
        wxNavigationCtrlEvent navEvent(wxEVT_COMMAND_NAVCTRL_PUSHING,
                                       GetId(), GetTopController(), controller);
        navEvent.SetEventObject(this);
        navEvent.Allow();
        if (GetEventHandler()->ProcessEvent(navEvent))
        {
            if (!navEvent.IsAllowed())
                return false;
        }
    }
    
    m_controllers.Add(controller);
    wxNavigationCtrlIPhoneImpl *peer = (wxNavigationCtrlIPhoneImpl *)GetPeer();
    if (! peer->PushViewController(controller)) {
        return false;
    }
    
    {
        wxNavigationCtrlEvent navEvent(wxEVT_COMMAND_NAVCTRL_PUSHED,
                                       GetId(), GetTopController(), controller);
        navEvent.SetEventObject(this);
        GetEventHandler()->ProcessEvent(navEvent);
    }
    
    return true;
}

// Pops an item off the stack. The item must then be deleted or stored by the application.
wxMoViewController* wxMoNavigationCtrl::PopController()
{
    if (m_controllers.GetCount() == 0) {
        return NULL;
    }
        
    wxMoViewController* backItem = GetBackController();
    wxMoViewController* topItem = GetTopController();
    
    if (! (topItem && backItem)) {
        return NULL;
    }
    
    {
        wxNavigationCtrlEvent navEvent(wxEVT_COMMAND_NAVCTRL_POPPING,
                                       GetId(), GetTopController(), backItem);
        navEvent.SetEventObject(this);
        navEvent.Allow();
        if (GetEventHandler()->ProcessEvent(navEvent)) {
            if (!navEvent.IsAllowed()) {
                return NULL;
            }
        }
    }
    
    if (topItem->GetNavigationItem()) {
        (void) m_navBar->PopItem();
    }
    
    m_controllers.RemoveAt(m_controllers.GetCount()-1);
    wxNavigationCtrlIPhoneImpl *peer = (wxNavigationCtrlIPhoneImpl *)GetPeer();
    if (! peer->PopViewController()) {
        return NULL;
    }
    
    {
        wxNavigationCtrlEvent navEvent(wxEVT_COMMAND_NAVCTRL_POPPED,
                                       GetId(), topItem, backItem);
        navEvent.SetEventObject(this);
        GetEventHandler()->ProcessEvent(navEvent);
    }
    
    return topItem;
}

// Sets the item stack.
void wxMoNavigationCtrl::SetControllers(const wxMoViewControllerArray& controllers)
{
    ClearControllers();
    
    size_t i;
    for (i = 0; i < controllers.GetCount(); i++) {
        PushController(controllers[i]);
    }
}

const wxMoViewControllerArray& wxMoNavigationCtrl::GetControllers() const
{
    return m_controllers;
}

// Returns the top item.
wxMoViewController* wxMoNavigationCtrl::GetTopController() const
{
    if (m_controllers.GetCount() > 0) {
        wxMoViewController* controller = m_controllers[m_controllers.GetCount()-1];
        return controller;
    } else {
        return NULL;
    }
}

// Returns the back item.
wxMoViewController* wxMoNavigationCtrl::GetBackController() const
{
    if (m_controllers.GetCount() > 1) {
        wxMoViewController* controller = m_controllers[m_controllers.GetCount()-2];
        return controller;
    } else {
        return NULL;
    }
}

// Returns the root item.
wxMoViewController* wxMoNavigationCtrl::GetRootController() const
{
    if (m_controllers.GetCount() > 1) {
        wxMoViewController* controller = m_controllers[0];
        return controller;
    } else {
        return NULL;
    }
}

// Clears the item stack, deleting the items.
void wxMoNavigationCtrl::ClearControllers()
{
    m_navBar->ClearItems(false);
    
    size_t i;
    size_t count = m_controllers.GetCount();

    wxNavigationCtrlIPhoneImpl *peer = (wxNavigationCtrlIPhoneImpl *)GetPeer();

    for (i = 0; i < count; i++) {
        wxMoViewController* controller = m_controllers[0];

        peer->PopViewController();
        m_controllers.RemoveAt(0);
        
        if (controller->OnDelete()) {
            delete controller;
        }
    }
}

#pragma mark Events

void wxMoNavigationCtrl::OnBack(wxCommandEvent& WXUNUSED(event))
{
    // FIXME stub
}

void wxMoNavigationCtrl::OnSize(wxSizeEvent& WXUNUSED(event))
{
    // FIXME stub
}



#if 0
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

void wxMoNavigationCtrl::PositionItems()
{
    // FIXME stub
}

// Restores the display
void wxMoNavigationCtrl::Thaw()
{
    // FIXME stub
}

#endif  // 0
