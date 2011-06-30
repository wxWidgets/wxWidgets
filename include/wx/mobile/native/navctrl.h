/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/native/navctrl.h
// Purpose:     wxMobile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_NATIVE_NAVCTRL_H_
#define _WX_MOBILE_NATIVE_NAVCTRL_H_

#include "wx/bitmap.h"
#include "wx/control.h"

#include "wx/mobile/native/utils.h"
#include "wx/mobile/native/navbar.h"
#include "wx/mobile/native/viewcontroller.h"

extern WXDLLEXPORT_DATA(const wxChar) wxNavigationCtrlNameStr[];

/**
 Bar style
 */

enum {
    wxNAVCTRL_NORMAL_BG =            0x00000000,
    wxNAVCTRL_BLACK_OPAQUE_BG =      0x00010000,
    wxNAVCTRL_BLACK_TRANSLUCENT_BG = 0x00020000
};


/**
 @class wxMoNavigationCtrl
 
 This class makes it easy to implement a style of interface similar to a web
 browser, where an action in the currently displayed window causes another window
 to replace it, and allowing a return to the previously displayed window using the
 Back button. This style of UI helps to make up for the lack of custom dialogs:
 you can switch to another part of the user interface, giving the effect of
 a modal user interface, in the sense of focusing user attention on a particular window.
 The Back button is the equivalent of a Close button on a dialog.
 
 The navigation control manages a wxMoNavigationBar (displayed at the top of the screen),
 and a stack of wxMoViewController objects which each manage a window and its title.
 
 The navigation bar shows a centred title, and a Back button on the left of the bar if
 there is one or more controller under the current controller. Each wxMoViewController
 manages a window and contains a wxMoNavigationItem to use with the wxMoNavigationBar.
 
 The bar can be customised by adding a user-defined wxMoBarButton to the
 left or right of the bar.
 
 Control styles can be:
 
 @li wxNAVCTRL_NORMAL_BG: a normal, opaque background colour for the navigation bar
 @li wxNAVCTRL_BLACK_OPAQUE_BG: an opaque black background for the navigation bar
 @li wxNAVCTRL_BLACK_TRANSLUCENT_BG: a translucent black background for the navigation bar
 
 To use this class, call wxMoNavigationCtrl::PushController to push a
 wxMoViewController object onto the stack. The wxMoViewController should
 be prepared by setting the window that is managed by the controller,
 and the title to show on the navigation bar and Back button. You
 can also specify further left and/or right buttons by retrieving the
 wxMoNavigationItem object from the controller and setting its button
 properties.
 
 The application can call wxMoNavigationCtrl::PopController to pop the top
 controller. The application should delete the returned controller if non-NULL.
 
 By default, the window that a wxMoViewController object manages is deleted
 when the wxMoViewController object is deleted. Also by default, each wxMoViewController
 is destroyed automatically when popped off the stack using the Back button.
 Both of these behaviours can be changed in case you need to store the controller
 and/or the window instead of creating and deleting them each time they are used.
 
 Popping and pushing items, whether handled by wxMoNavigationCtrl or
 initiated by the application, causes wxNavigationCtrlEvent events to be
 generated. Event types are:
 
 @li wxEVT_COMMAND_NAVCTRL_POPPED: a controller was popped
 @li wxEVT_COMMAND_NAVCTRL_PUSHED: a controller was pushed
 @li wxEVT_COMMAND_NAVCTRL_POPPING: a controller is about to be popped. This can be vetoed.
 @li wxEVT_COMMAND_NAVCTRL_PUSHING: a controller is about to be pushed. This can be vetoed.
 
 @category{wxMobile}
 */

class WXDLLEXPORT wxMoNavigationCtrl: public wxControl
{
public:
    /// Default constructor.
    wxMoNavigationCtrl();
    
    /// Constructor.
    wxMoNavigationCtrl(wxWindow *parent,
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
    
    virtual ~wxMoNavigationCtrl();
    
    void Init();
    
    /// Pushes an item onto the stack.
    virtual bool PushController(wxMoViewController* controller);
    
    /// Pops a view controller off the stack. The controller must then be deleted or stored by the application.
    virtual wxMoViewController* PopController();
    
    /// Returns the top controller.
    virtual wxMoViewController* GetTopController() const;
    
    /// Returns the back controller (the controller below the top controller).
    virtual wxMoViewController* GetBackController() const;
    
    /// Returns the root controller (the controller which is first in the queue).
    virtual wxMoViewController* GetRootController() const;
    
    /// Returns the controller stack.
    const wxMoViewControllerArray& GetControllers() const;
    
    /// Sets the controller stack.
    void SetControllers(const wxMoViewControllerArray& controllers);
    
    /// Clears the controller stack, deleting the controllers if signalled by auto-delete for each controller.
    void ClearControllers();
    
    /// Returns the navigation bar
    wxMoNavigationBar* GetNavigationBar() const { return m_navBar; }
    
    /// Freezes (doesn't display or resize during operations)
    void Freeze() { m_freezeCount ++; }
    
    /// Restores the display
    void Thaw();
    
    /// Returns true if the control is 'frozen', i.e. suppresses display updates and resizes.
    bool IsFrozen() const { return m_freezeCount > 0; }
    
    // IMPLEMENTATION
    
#if 0
    // Position the control items
    virtual void PositionItems();
    
    virtual bool SetBackgroundColour(const wxColour &colour);
    virtual bool SetForegroundColour(const wxColour &colour);
    virtual bool SetFont(const wxFont& font);
    virtual bool Enable(bool enable);
#endif
    
protected:
    
#if 0
    // Get the best size
    virtual wxSize DoGetBestSize() const;
#endif
        
    void OnSize(wxSizeEvent& event);
    void OnBack(wxCommandEvent& event);
    
    wxMoViewControllerArray m_controllers;
    wxMoNavigationBar*      m_navBar;
    int                     m_freezeCount;
    
private:
    
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoNavigationCtrl)
    DECLARE_EVENT_TABLE()
    
};

/**
 @class wxMoNavigationCtrlEvent
 
 A navigation control event is sent when a view controller is
 pushed or popped.
 */

class WXDLLEXPORT wxNavigationCtrlEvent: public wxNotifyEvent
{
public:
    wxNavigationCtrlEvent()
    {
        m_oldItem = NULL;
        m_newItem = NULL;
    }
    wxNavigationCtrlEvent(const wxNavigationCtrlEvent& event): wxNotifyEvent(event)
    {
        m_oldItem = event.m_oldItem;
        m_newItem = event.m_newItem;
    }
    wxNavigationCtrlEvent(wxEventType commandType, int id,
                          wxMoViewController* oldItem, wxMoViewController* newItem)
    : wxNotifyEvent(commandType, id)
    {
        m_oldItem = oldItem;
        m_newItem = newItem;
    }
    
    /// Set the new item
    void SetNewController(wxMoViewController* newItem) { m_newItem = newItem; }
    
    /// Get the new item
    wxMoViewController* GetNewController() const { return m_newItem; }
    
    /// Set the old item
    void SetOldController(wxMoViewController* oldItem) { m_oldItem = oldItem; }
    
    /// Get the old item
    wxMoViewController* GetOldController() const { return m_oldItem; }
    
    virtual wxEvent *Clone() const { return new wxNavigationCtrlEvent(*this); }
    
private:
    wxMoViewController* m_oldItem;
    wxMoViewController* m_newItem;
    
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxNavigationCtrlEvent)
};

typedef void (wxEvtHandler::*wxNavigationCtrlEventFunction)(wxNavigationCtrlEvent&);

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(wxEVT_COMMAND_NAVCTRL_POPPED,     850)
DECLARE_EVENT_TYPE(wxEVT_COMMAND_NAVCTRL_PUSHED,     851)
DECLARE_EVENT_TYPE(wxEVT_COMMAND_NAVCTRL_POPPING,    852)
DECLARE_EVENT_TYPE(wxEVT_COMMAND_NAVCTRL_PUSHING,    853)
END_DECLARE_EVENT_TYPES()

#define wxNavigationCtrlEventHandler(func) \
(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxNavigationCtrlEventFunction, &func)

#define EVT_NAVCTRL_POPPED(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_NAVCTRL_POPPED, \
id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxNavigationEventFunction, & fn ), NULL),
#define EVT_NAVCTRL_POPPING(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_NAVCTRL_POPPING, \
id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxNavigationEventFunction, & fn ), NULL),
#define EVT_NAVCTRL_PUSHED(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_NAVCTRL_PUSHED, \
id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxNavigationEventFunction, & fn ), NULL),
#define EVT_NAVCTRL_PUSHING(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_NAVCTRL_PUSHING, \
id, wxID_ANY, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent( wxNavigationEventFunction, & fn ), NULL),

#endif
    // _WX_MOBILE_NATIVE_NAVCTRL_H_
