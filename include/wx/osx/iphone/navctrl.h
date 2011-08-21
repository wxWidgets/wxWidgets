/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/iphone/navctrl.h
// Purpose:     wxbile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_NAVCTRL_H_
#define _WX_NAVCTRL_H_

#include "wx/bitmap.h"
#include "wx/control.h"

#include "wx/navbar.h"
#include "wx/viewcontroller.h"


/**
 @class wxNavigationCtrl
 @category{wxmobile}
 */

class WXDLLEXPORT wxNavigationCtrl: public wxNavigationCtrlBase
{
public:
    /// Default constructor.
    wxNavigationCtrl();
    
    /// Constructor.
    wxNavigationCtrl(wxWindow *parent,
                     wxWindowID id = wxID_ANY,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = 0,
                     const wxValidator& validator = wxDefaultValidator,
                     const wxString& name = wxNavigationCtrlNameStr);
    
    /// Creation function.
    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxNavigationCtrlNameStr);
    
    virtual ~wxNavigationCtrl();
    
    /// Pushes an item onto the stack.
    bool PushController(wxViewController* controller);
    
    /// Pops a view controller off the stack. The controller must then be deleted or stored by the application.
    wxViewController* PopController();
    
    /// Returns the top controller.
    wxViewController* GetTopController() const;
    
    /// Returns the back controller (the controller below the top controller).
    wxViewController* GetBackController() const;
    
    /// Returns the root controller (the controller which is first in the queue).
    wxViewController* GetRootController() const;
        
    /// Sets the controller stack.
    void SetControllers(const wxViewControllerArray& controllers);
    
    /// Clears the controller stack, deleting the controllers if signalled by auto-delete for each controller.
    void ClearControllers();
    
    /// Returns the navigation bar
    wxNavigationBar* GetNavigationBar() const { return m_navBar; }
        
    /// Returns true if the control is 'frozen', i.e. suppresses display updates and resizes.
    bool IsFrozen() const { return m_freezeCount > 0; }
    
    
    // Implementation: get Cocoa's wxUINavigationController
    void* GetNativeNavigationController();
        
protected:
    
    void Init();
    
private:
    
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxNavigationCtrl)
    DECLARE_EVENT_TABLE()
    
};

#endif
    // _WX_NAVCTRL_H_
