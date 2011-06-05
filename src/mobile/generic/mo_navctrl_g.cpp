/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_navctrl_g.cpp
// Purpose:     wxMoNavigationCtrl class
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/dcclient.h"
#include "wx/dcscreen.h"

#include "wx/mobile/generic/navctrl.h"
#include "wx/mobile/generic/utils.h"

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
    if ( !wxControl::Create(parent, id, pos, size, style|wxBORDER_NONE, validator, name) )
        return false;

    SetBackgroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_NAVBAR_NORMAL_BG));
    SetForegroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_NAVBAR_TEXT));

    long barStyle = 0;
    if (style & wxNAVCTRL_BLACK_OPAQUE_BG)
        barStyle |= wxNAVBAR_BLACK_OPAQUE_BG;
    if (style & wxNAVCTRL_BLACK_TRANSLUCENT_BG)
        barStyle |= wxNAVBAR_BLACK_TRANSLUCENT_BG;

    m_navBar = new wxMoNavigationBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, barStyle);

    m_navBar->Connect(wxID_BACK, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(wxMoNavigationCtrl::OnBack), NULL, this);

    SetInitialSize(size);

    return true;
}

wxMoNavigationCtrl::~wxMoNavigationCtrl()
{
    ClearControllers();
}

void wxMoNavigationCtrl::Init()
{
    m_navBar = NULL;
    m_freezeCount = 0;
}

wxSize wxMoNavigationCtrl::DoGetBestSize() const
{
    wxClientDC dc(wx_const_cast(wxMoNavigationCtrl *, this));
    dc.SetFont(GetFont());

    // Pixels between labels and edges
    // int spacing = 5;

    // TODO
    int width = 100;
    int height = 44; // normal navigation bar height
    
    wxSize best(width, height);
    CacheBestSize(best);
    return best;
}

bool wxMoNavigationCtrl::SetBackgroundColour(const wxColour &colour)
{
    if ( !wxControl::SetBackgroundColour(colour) )
    {
        // nothing to do
        return false;
    }
    Refresh();
    return true;
}

bool wxMoNavigationCtrl::SetForegroundColour(const wxColour &colour)
{
    if ( !wxControl::SetForegroundColour(colour) )
    {
        // nothing to do
        return false;
    }

    Refresh();
    return true;
}

bool wxMoNavigationCtrl::Enable(bool enable)
{
    wxControl::Enable(enable);
    Refresh();
    return true;
}

bool wxMoNavigationCtrl::SetFont(const wxFont& font)
{
    wxControl::SetFont(font);
    Refresh();

    return true;
}

void wxMoNavigationCtrl::OnBack(wxCommandEvent& WXUNUSED(event))
{
    wxMoViewController* backController = GetBackController();
    if (backController)
    {
        wxMoViewController* oldController = PopController();
        if (oldController && oldController->OnDelete())
        {
            delete oldController;
        }
    }
}

void wxMoNavigationCtrl::OnSize(wxSizeEvent& WXUNUSED(event))
{
    PositionItems();
    Refresh();
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

    if (GetTopController() && GetTopController()->GetWindow())
        GetTopController()->GetWindow()->Show(false);

    if (controller->GetNavigationItem())
        m_navBar->PushItem(controller->GetNavigationItem());

    m_controllers.Add(controller);

    if (!IsFrozen())
    {
        PositionItems();

        if (GetTopController() && GetTopController()->GetWindow())
            GetTopController()->GetWindow()->Show(true);
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
    if (m_controllers.GetCount() > 0)
    {
        wxMoViewController* backItem = GetBackController();
        wxMoViewController* topItem = GetTopController();
        if (topItem && backItem)
        {
            {
                wxNavigationCtrlEvent navEvent(wxEVT_COMMAND_NAVCTRL_POPPING,
                    GetId(), GetTopController(), backItem);
                navEvent.SetEventObject(this);
                navEvent.Allow();
                if (GetEventHandler()->ProcessEvent(navEvent))
                {
                    if (!navEvent.IsAllowed())
                        return NULL;
                }
            }

            if (topItem->GetWindow())
                topItem->GetWindow()->Show(false);

            if (topItem->GetNavigationItem())
            {
                (void) m_navBar->PopItem();
            }

            m_controllers.RemoveAt(m_controllers.GetCount()-1);

            if (!IsFrozen())
            {
                PositionItems();
            
                if (backItem->GetWindow())
                    backItem->GetWindow()->Show(true);
            }
            
            {
                wxNavigationCtrlEvent navEvent(wxEVT_COMMAND_NAVCTRL_POPPED,
                    GetId(), topItem, backItem);
                navEvent.SetEventObject(this);
                GetEventHandler()->ProcessEvent(navEvent);
            }
            
            return topItem;
        }
    }
    return NULL;
}

// Returns the top item.
wxMoViewController* wxMoNavigationCtrl::GetTopController() const
{
    if (m_controllers.GetCount() > 0)
    {
        wxMoViewController* controller = m_controllers[m_controllers.GetCount()-1];
        return controller;
    }
    else
        return NULL;
}

// Returns the back item.
wxMoViewController* wxMoNavigationCtrl::GetBackController() const
{
    if (m_controllers.GetCount() > 1)
    {
        wxMoViewController* controller = m_controllers[m_controllers.GetCount()-2];
        return controller;
    }
    else
        return NULL;
}

// Sets the item stack.
void wxMoNavigationCtrl::SetControllers(const wxMoViewControllerArray& controllers)
{
    Freeze();

    ClearControllers();

    size_t i;
    for (i = 0; i < controllers.GetCount(); i++)
    {
        PushController(controllers[i]);
    }

    Thaw();
}

// Clears the item stack, deleting the items.
void wxMoNavigationCtrl::ClearControllers()
{
    m_navBar->ClearItems(false);

    size_t i;
    size_t count = m_controllers.GetCount();
    for (i = 0; i < count; i++)
    {
        wxMoViewController* controller = m_controllers[0];
        m_controllers.RemoveAt(0);
        if (controller->OnDelete())
            delete controller;
    }
}

void wxMoNavigationCtrl::PositionItems()
{
    wxSize sz = GetClientSize();
    wxSize availableSize = sz;

    if (m_navBar)
    {
        m_navBar->SetSize(0, 0, availableSize.x, m_navBar->GetSize().y);
        availableSize.y -= m_navBar->GetSize().y;
    }

    if (GetTopController() && GetTopController()->GetWindow())
    {
        GetTopController()->GetWindow()->SetSize(0, sz.y - availableSize.y, availableSize.x, availableSize.y);
    }
}

// Restores the display
void wxMoNavigationCtrl::Thaw()
{
    if (IsFrozen())
    {
        m_freezeCount --;
    }

    if (m_freezeCount == 0)
    {
        PositionItems();
        Refresh();
    }
}


