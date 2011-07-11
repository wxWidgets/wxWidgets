/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/iphone/viewcontroller.mm
// Purpose:     wxViewController implementation
// Author:      Linas Valiukas
// Modified by:
// Created:     2011-06-30
// RCS-ID:      $Id$
// Copyright:   (c) Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/app.h"
#include "wx/utils.h"
#include "wx/dc.h"
#include "wx/dcclient.h"
#include "wx/settings.h"
#endif // WX_PRECOMP

#include "wx/viewcontroller.h"
#include "wx/osx/core/cfstring.h"
#include "wx/osx/private.h"
#import <UIKit/UIKit.h>



#pragma mark -
#pragma mark wxViewController implementation

IMPLEMENT_DYNAMIC_CLASS(wxViewController, wxViewControllerBase)

wxViewController::wxViewController(const wxString& title, bool autoDelete)
{
    Init();
    SetTitle(title);
    
    m_autoDelete = autoDelete;
}

wxViewController::~wxViewController()
{
    delete m_item;
    if (m_ownsWindow && m_window) {
        delete m_window;
        m_window = NULL;
    }
    
    UIViewController *controller = (UIViewController *)m_uiviewcontroller;
    [controller release];
    m_uiviewcontroller = NULL;
}

void wxViewController::Init()
{
    m_uiviewcontroller = [[UIViewController alloc] init];
    
    m_item = new wxNavigationItem;
    m_item->SetViewController(this);
    m_autoDelete = true;
    m_ownsWindow = true;
    
    m_window = NULL;
}

// Sets the view title
void wxViewController::SetTitle(const wxString& title) {
    if (m_item) {
        m_item->SetTitle(title);
    }
    
    UIViewController *controller = (UIViewController *)m_uiviewcontroller;
    [controller setTitle:wxCFStringRef(title).AsNSString()];
}

// Gets the view title
wxString wxViewController::GetTitle() const
{
    if (m_item) {
        return m_item->GetTitle();
    } else {
        return wxEmptyString;
    }
}

/// Sets the associated window
void wxViewController::SetWindow(wxWindow* window)
{
    m_window = window;
    
    UIViewController *viewController = (UIViewController *)m_uiviewcontroller;
    wxOSXWidgetImpl *windowPeer = window->GetPeer();
    wxASSERT_MSG(windowPeer, "No window peer");
    UIView *windowUIView = windowPeer->GetWXWidget();
    wxASSERT_MSG(windowUIView, "No window peer UIView");
    [viewController setView:windowUIView];
}

/// Gets the navigation item
wxWindow* wxViewController::GetWindow() const
{
    return m_window;
}
