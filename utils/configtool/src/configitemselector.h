/////////////////////////////////////////////////////////////////////////////
// Name:        configitemselector.h
// Purpose:     Selector for one or more config items
// Author:      Julian Smart
// Modified by:
// Created:     2003-06-04
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#ifndef _CONFIGITEMSELECTOR_H_
#define _CONFIGITEMSELECTOR_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "configitemselector.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG 10000
#define ID_AVAILABLE_CONFIG_ITEMS 10002
#define ID_CONFIG_ITEMS 10005
#define ID_CONFIG_ADD 10006
#define ID_CONFIG_REMOVE 10007
////@end control identifiers

class ctConfigItem;

/*!
 * ctConfigItemsSelector class declaration
 */

class ctConfigItemsSelector: public wxDialog
{
public:
    /// Constructor
    ctConfigItemsSelector( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& caption = _("Configuration Items Selector"));

    /// Creates the controls and sizers
    void CreateControls();

////@begin ctConfigItemsSelector event handler declarations

    /// Event handler for ID_CONFIG_ADD
    void OnConfigAdd( wxCommandEvent& event );

    /// Update event handler for ID_CONFIG_ADD
    void OnUpdateConfigAdd( wxUpdateUIEvent& event );

    /// Event handler for ID_CONFIG_REMOVE
    void OnConfigRemove( wxCommandEvent& event );

    /// Update event handler for ID_CONFIG_REMOVE
    void OnUpdateConfigRemove( wxUpdateUIEvent& event );

    /// Event handler for wxID_OK
    void OnOk( wxCommandEvent& event );

////@end ctConfigItemsSelector event handler declarations

////@begin ctConfigItemsSelector member function declarations

////@end ctConfigItemsSelector member function declarations

    /// Initialise the master list
    void InitSourceConfigList(ctConfigItem* item = NULL);

    /// Set the initial list
    void SetConfigList(const wxArrayString& items);

    /// Get the list
    const wxArrayString& GetConfigList() const { return m_configItems; }

    /// Should we show tooltips?
    static bool ShowToolTips();

    DECLARE_CLASS( ctConfigItemsSelector )
    DECLARE_EVENT_TABLE()

protected:
    wxArrayString   m_configItems;

};

#endif
    // _CONFIGITEMSELECTOR_H_
