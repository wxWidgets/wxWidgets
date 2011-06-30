/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/native/viewcontroller.h
// Purpose:     wxMobile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_NATIVE_VIEWCONTROLLER_H_
#define _WX_MOBILE_NATIVE_VIEWCONTROLLER_H_

#include "wx/dynarray.h"
#include "wx/mobile/native/navitem.h"

/**
    @class wxMoViewController

    The view controller manages a window, usually in conjunction with the wxMoNavigationCtrl
    class. For more details, see wxMoNavigationCtrl.

    @category{wxMobile}
*/

class WXDLLEXPORT wxMoViewController: public wxEvtHandler
{
public:
    /// Constructor.
    wxMoViewController(const wxString& title = wxEmptyString, bool autoDelete = true);

    ~wxMoViewController();

    void Init();

// Overridable functions

    /// Called before deletion, cancelling delete if false is returned.
    virtual bool OnDelete() { return GetAutoDelete(); }

// Accessors

    /// Sets the navigation item
    void SetNavigationItem(wxMoNavigationItem* item) { m_item = item; }

    /// Gets the navigation item
    wxMoNavigationItem* GetNavigationItem() const { return m_item; }

    /// Sets the associated window
    void SetWindow(wxWindow* window);

    /// Gets the navigation item
    wxWindow* GetWindow() const;

    /// Sets the auto-delete flag. If true, the controller will be deleted when
    /// popped off a navigation stack.
    void SetAutoDelete(bool autoDelete) { m_autoDelete = autoDelete; }

    /// Gets the auto-delete flag
    bool GetAutoDelete() const { return m_autoDelete; }

    /// Sets the view title
    void SetTitle(const wxString& title);

    /// Gets the view title
    wxString GetTitle() const;

    /// Pass true if the window should be deleted when the controller
    /// is deleted. By default this is true.
    void SetOwnsWindow(bool owns) { m_ownsWindow = owns; }

    /// Returns true if the window will be deleted when the controller
    /// is deleted. By default this is true.
    bool GetOwnsWindow() const { return m_ownsWindow; }
    
    
    // Returns a pointer to UIViewController
    void* GetUIViewController() const { return m_uiviewcontroller; }

protected:

    wxMoNavigationItem*     m_item;
    bool                    m_autoDelete;
    bool                    m_ownsWindow;
    wxWindow*               m_window;
    
    // Pointer to UIViewController
    void*                   m_uiviewcontroller;

    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoViewController)
};

WX_DEFINE_ARRAY_PTR(wxMoViewController*, wxMoViewControllerArray);

#endif
    // _WX_MOBILE_NATIVE_VIEWCONTROLLER_H_
