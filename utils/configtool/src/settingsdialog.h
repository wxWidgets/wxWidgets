/////////////////////////////////////////////////////////////////////////////
// Name:        settingsdialog.h
// Purpose:     Settings dialog
// Author:      Julian Smart
// Modified by:
// Created:     2002-09-04
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#ifndef _SETTINGSDIALOG_H_
#define _SETTINGSDIALOG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "settingsdialog.cpp"
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
class ctGeneralSettingsDialog;
class ctLocationSettingsDialog;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_SETTINGS_DIALOG 10000
#define ID_NOTEBOOK 10001
#define ID_GENERAL_SETTINGS_DIALOG 10005
#define ID_LOAD_LAST_DOCUMENT 10006
#define ID_SHOW_TOOLTIPS 10007
#define ID_DEFAULT_FILE_KIND 10003
#define ID_LOCATION_SETTINGS_DIALOG 10008
#define ID_WXWIN_HIERARCHY 10010
#define ID_CHOOSE_WXWIN_HIERARCHY 10011
#define ID_USE_WXWIN 10012
////@end control identifiers

/*!
 * ctSettingsDialog class declaration
 */

class ctSettingsDialog: public wxDialog
{
public:
    /// Constructor
    ctSettingsDialog( wxWindow* parent );

    /// Creates the controls and sizers
    void CreateControls();

////@begin ctSettingsDialog event handler declarations

    /// Event handler for wxID_OK
    void OnOk( wxCommandEvent& event );

    /// Event handler for wxID_CANCEL
    void OnCancel( wxCommandEvent& event );

    /// Event handler for wxID_HELP
    void OnHelp( wxCommandEvent& event );

////@end ctSettingsDialog event handler declarations

////@begin ctSettingsDialog member function declarations

////@end ctSettingsDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

    DECLARE_CLASS( ctSettingsDialog )
    DECLARE_EVENT_TABLE()

protected:

    // Dialog controls.
    wxNotebook* m_notebook;
};

/*!
 * ctGeneralSettingsDialog class declaration
 */

class ctGeneralSettingsDialog: public wxPanel
{
public:
    /// Constructor
    ctGeneralSettingsDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER|wxTAB_TRAVERSAL );

    /// Creates the controls and sizers
    void CreateControls();

////@begin ctGeneralSettingsDialog event handler declarations

////@end ctGeneralSettingsDialog event handler declarations

////@begin ctGeneralSettingsDialog member function declarations

////@end ctGeneralSettingsDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

    DECLARE_CLASS( ctGeneralSettingsDialog )
    DECLARE_EVENT_TABLE()
};

/*!
 * ctLocationSettingsDialog class declaration
 */

class ctLocationSettingsDialog: public wxPanel
{
public:
    /// Constructor
    ctLocationSettingsDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER|wxTAB_TRAVERSAL );

    /// Creates the controls and sizers
    void CreateControls();

////@begin ctLocationSettingsDialog event handler declarations

    /// Update event handler for ID_WXWIN_HIERARCHY
    void OnUpdateWxwinHierarchy( wxUpdateUIEvent& event );

    /// Event handler for ID_CHOOSE_WXWIN_HIERARCHY
    void OnChooseWxwinHierarchy( wxCommandEvent& event );

    /// Update event handler for ID_CHOOSE_WXWIN_HIERARCHY
    void OnUpdateChooseWxwinHierarchy( wxUpdateUIEvent& event );

////@end ctLocationSettingsDialog event handler declarations

////@begin ctLocationSettingsDialog member function declarations

////@end ctLocationSettingsDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

    DECLARE_CLASS( ctLocationSettingsDialog )
    DECLARE_EVENT_TABLE()

protected:

    // Dialog controls.
    wxTextCtrl* m_wxWinHierarchy;
    wxCheckBox* m_wxWinUse;
};

#endif
    // _SETTINGSDIALOG_H_
