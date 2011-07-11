/////////////////////////////////////////////////////////////////////////////
// Name:        wx/viewcontroller.h
// Purpose:     wxbile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_VIEWCONTROLLER_H_BASE_
#define _WX_VIEWCONTROLLER_H_BASE_

#include "wx/dynarray.h"
#include "wx/navitem.h"

/**
    @class wxViewController

    The view controller manages a window, usually in conjunction with the wxNavigationCtrl
    class. For more details, see wxNavigationCtrl.

    @category{wxbile}
*/

class WXDLLEXPORT wxViewControllerBase: public wxEvtHandler
{
public:
    /// Constructor.
    wxViewControllerBase(const wxString& title = wxEmptyString, bool autoDelete = true) { }

    virtual ~wxViewControllerBase();

// Overridable functions

    /// Called before deletion, cancelling delete if false is returned.
    virtual bool OnDelete() { return GetAutoDelete(); }

// Accessors

    /// Sets the navigation item
    void SetNavigationItem(wxNavigationItem* item) { m_item = item; }

    /// Gets the navigation item
    wxNavigationItem* GetNavigationItem() const { return m_item; }

    /// Sets the associated window
    virtual void SetWindow(wxWindow* window) = 0;

    /// Gets the navigation item
    virtual wxWindow* GetWindow() const = 0;

    /// Sets the auto-delete flag. If true, the controller will be deleted when
    /// popped off a navigation stack.
    void SetAutoDelete(bool autoDelete) { m_autoDelete = autoDelete; }

    /// Gets the auto-delete flag
    bool GetAutoDelete() const { return m_autoDelete; }

    /// Sets the view title
    virtual void SetTitle(const wxString& title) = 0;

    /// Gets the view title
    virtual wxString GetTitle() const = 0;

    /// Pass true if the window should be deleted when the controller
    /// is deleted. By default this is true.
    void SetOwnsWindow(bool owns) { m_ownsWindow = owns; }

    /// Returns true if the window will be deleted when the controller
    /// is deleted. By default this is true.
    bool GetOwnsWindow() const { return m_ownsWindow; }
    
protected:

    wxNavigationItem*     m_item;
    bool                  m_autoDelete;
    bool                  m_ownsWindow;
    wxWindow*             m_window;
    
    wxDECLARE_NO_COPY_CLASS(wxViewControllerBase);
    
};

WX_DEFINE_ARRAY_PTR(wxViewController*, wxViewControllerArray);


// ----------------------------------------------------------------------------
// wxViewController class itself
// ----------------------------------------------------------------------------

#if defined(__WXOSX_IPHONE__)  // iPhone-only
    #include  "wx/osx/iphone/viewcontroller.h"
#endif


#endif
    // _WX_VIEWCONTROLLER_H_BASE_
