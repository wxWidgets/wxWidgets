/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/navctrl.cpp
// Purpose:     Common wxNavigationCtrl parts
// Author:      Julian Smart, Linas Valiukas
// Created:     2011-08-19
// RCS-ID:      $Id$
// Copyright:   (c) 2011 Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/dc.h"
#include "wx/sizer.h"
#endif

#include "wx/navctrl.h"
#include "wx/arrimpl.cpp"


extern WXDLLEXPORT_DATA(const wxChar) wxNavigationCtrlNameStr[] = wxT("NavigationCtrl");

IMPLEMENT_DYNAMIC_CLASS(wxNavigationCtrlEvent, wxNotifyEvent)

DEFINE_EVENT_TYPE(wxEVT_COMMAND_NAVCTRL_POPPED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NAVCTRL_POPPING)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NAVCTRL_PUSHED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NAVCTRL_PUSHING)


// Utility to push a table onto the navigation controller
void wxNavigationCtrlBase::PushTable(wxTableCtrl* tableCtrl,
                                     const wxString& title)
{
    wxViewController* controller = new wxViewController(title);
    tableCtrl->Show(false);
    controller->SetWindow(tableCtrl);
    
    PushController(controller);
    tableCtrl->Show(true);
    
    tableCtrl->ReloadData();
}

// Utility to push a window onto the navigation controller
void wxNavigationCtrlBase::PushWindow(wxWindow* win, const wxString& title)
{
    wxViewController* controller = new wxViewController(title);
    win->Show(false);
    controller->SetWindow(win);
    PushController(controller);
    win->Show(true);
}

// Sets the item stack.
void wxNavigationCtrl::SetControllers(const wxViewControllerArray& controllers)
{
    ClearControllers();
    
    size_t i;
    for (i = 0; i < controllers.GetCount(); i++) {
        PushController(controllers[i]);
    }
}

// Returns the top item.
wxViewController* wxNavigationCtrl::GetTopController() const
{
    if (m_controllers.GetCount() > 0) {
        wxViewController* controller = m_controllers[m_controllers.GetCount()-1];
        return controller;
    } else {
        return NULL;
    }
}

// Returns the back item.
wxViewController* wxNavigationCtrl::GetBackController() const
{
    if (m_controllers.GetCount() > 1) {
        wxViewController* controller = m_controllers[m_controllers.GetCount()-2];
        return controller;
    } else {
        return NULL;
    }
}

// Returns the root item.
wxViewController* wxNavigationCtrl::GetRootController() const
{
    if (m_controllers.GetCount() > 1) {
        wxViewController* controller = m_controllers[0];
        return controller;
    } else {
        return NULL;
    }
}
