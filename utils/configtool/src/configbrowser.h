/////////////////////////////////////////////////////////////////////////////
// Name:        configbrowser.h
// Purpose:     Configuration browser
// Author:      Julian Smart
// Modified by: 
// Created:     2003-08-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _CONFIGBROWSER_H_
#define _CONFIGBROWSER_H_

/*!
 * Includes
 */

////@begin includes
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class ctConfigurationBrowserControlPanel;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG 2000
#define ID_CONFIGBROWSER_SPLITTERWINDOW 2001
#define ID_CONFIGURATION_BROWSER_TREECTRL 2002
#define ID_PANEL 2003
#define ID_ADD_CONFIGURATION 2004
#define ID_REMOVE_CONFIGURATION 2005
#define ID_RENAME_CONFIGURATION 2006
#define ID_CONFIGURATION_NAME 2007
#define ID_CONFIGURATION_DESCRIPTION 2008
////@end control identifiers

/*!
 * ctConfigurationBrowserWindow class declaration
 */

class ctConfigurationBrowserWindow: public wxPanel
{    
    DECLARE_CLASS( ctConfigurationBrowserWindow )
    DECLARE_EVENT_TABLE()

public:
    /// Constructor
    ctConfigurationBrowserWindow( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxNO_FULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN  );

    /// Creates the controls and sizers
    void CreateControls();

////@begin ctConfigurationBrowserWindow event handler declarations

    /// Event handler for ID_CONFIGURATION_BROWSER_TREECTRL
    void OnConfigurationBrowserTreectrl( wxTreeEvent& event );

////@end ctConfigurationBrowserWindow event handler declarations

////@begin ctConfigurationBrowserWindow member function declarations

////@end ctConfigurationBrowserWindow member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin ctConfigurationBrowserWindow member variables
////@end ctConfigurationBrowserWindow member variables
};

/*!
 * ctConfigurationBrowserControlPanel class declaration
 */

class ctConfigurationBrowserControlPanel: public wxPanel
{    
    DECLARE_CLASS( ctConfigurationBrowserControlPanel )
    DECLARE_EVENT_TABLE()

public:
    /// Constructor
    ctConfigurationBrowserControlPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER|wxTAB_TRAVERSAL );

    /// Creates the controls and sizers
    void CreateControls();

////@begin ctConfigurationBrowserControlPanel event handler declarations

    /// Event handler for ID_ADD_CONFIGURATION
    void OnAddConfiguration( wxCommandEvent& event );

    /// Update event handler for ID_ADD_CONFIGURATION
    void OnUpdateAddConfiguration( wxUpdateUIEvent& event );

    /// Event handler for ID_REMOVE_CONFIGURATION
    void OnRemoveConfiguration( wxCommandEvent& event );

    /// Update event handler for ID_REMOVE_CONFIGURATION
    void OnUpdateRemoveConfiguration( wxUpdateUIEvent& event );

    /// Event handler for ID_RENAME_CONFIGURATION
    void OnRenameConfiguration( wxCommandEvent& event );

    /// Update event handler for ID_RENAME_CONFIGURATION
    void OnUpdateRenameConfiguration( wxUpdateUIEvent& event );

////@end ctConfigurationBrowserControlPanel event handler declarations

////@begin ctConfigurationBrowserControlPanel member function declarations

////@end ctConfigurationBrowserControlPanel member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin ctConfigurationBrowserControlPanel member variables
////@end ctConfigurationBrowserControlPanel member variables
};

#endif
    // _CONFIGBROWSER_H_
