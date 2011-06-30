/////////////////////////////////////////////////////////////////////////////
// Name:        src/moile/generic/mo_viewcontroller_n.mm
// Purpose:     wxMoViewController class
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

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/dc.h"
    #include "wx/dcclient.h"
    #include "wx/settings.h"
#endif // WX_PRECOMP

#include "wx/osx/core/cfstring.h"
#include "wx/mobile/native/viewcontroller.h"
#include "wx/osx/private.h"

#import <UIKit/UIKit.h>

IMPLEMENT_DYNAMIC_CLASS(wxMoViewController, wxEvtHandler)


wxMoViewController::wxMoViewController(const wxString& title, bool autoDelete)
{
    Init();
    SetTitle(title);
    
    m_autoDelete = autoDelete;
}

wxMoViewController::~wxMoViewController()
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

void wxMoViewController::Init()
{
    m_uiviewcontroller = [[UIViewController alloc] init];
    
    m_item = new wxMoNavigationItem;
    m_item->SetViewController(this);
    m_autoDelete = true;
    m_ownsWindow = true;
    
    m_window = NULL;
}

// Sets the view title
void wxMoViewController::SetTitle(const wxString& title) {
    if (m_item) {
        m_item->SetTitle(title);
    }
    
    UIViewController *controller = (UIViewController *)m_uiviewcontroller;
    [controller setTitle:wxCFStringRef(title).AsNSString()];
}

// Gets the view title
wxString wxMoViewController::GetTitle() const
{
    if (m_item) {
        return m_item->GetTitle();
    } else {
        return wxEmptyString;
    }
}

/// Sets the associated window
void wxMoViewController::SetWindow(wxWindow* window)
{
    m_window = window;
    
    UIViewController *viewController = (UIViewController *)m_uiviewcontroller;
    [viewController setView:window->GetPeer()->GetWXWidget()];
}

/// Gets the navigation item
wxWindow* wxMoViewController::GetWindow() const
{
    return m_window;
}
