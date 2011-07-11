/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/iphone/viewcontroller.h
// Purpose:     wxbile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_VIEWCONTROLLER_H_
#define _WX_VIEWCONTROLLER_H_

#include "wx/dynarray.h"
#include "wx/navitem.h"

/**
    @class wxViewController

    The view controller manages a window, usually in conjunction with the wxNavigationCtrl
    class. For more details, see wxNavigationCtrl.

    @category{wxbile}
*/

class WXDLLEXPORT wxViewController: public wxViewControllerBase
{
public:
    /// Constructor.
    wxViewController(const wxString& title = wxEmptyString, bool autoDelete = true);

    ~wxViewController();

// Overridable functions

    /// Called before deletion, cancelling delete if false is returned.
    virtual bool OnDelete() { return GetAutoDelete(); }

// Accessors

    /// Sets the associated window
    void SetWindow(wxWindow* window);

    /// Gets the navigation item
    wxWindow* GetWindow() const;

    /// Sets the view title
    void SetTitle(const wxString& title);

    /// Gets the view title
    wxString GetTitle() const;
    
        
    // Returns a pointer to UIViewController
    void* GetUIViewController() const { return m_uiviewcontroller; }

protected:

    void Init();

    // Pointer to UIViewController
    void*                   m_uiviewcontroller;

    DECLARE_DYNAMIC_CLASS_NO_COPY(wxViewController)
};

#endif
    // _WX_VIEWCONTROLLER_H_
