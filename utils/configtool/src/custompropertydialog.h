/////////////////////////////////////////////////////////////////////////////
// Name:        custompropertydialog.h
// Purpose:     Custom property dialog
// Author:      Julian Smart
// Modified by:
// Created:     2003-06-04
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#ifndef _CUSTOMPROPERTYDIALOG_H_
#define _CUSTOMPROPERTYDIALOG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "custompropertydialog.cpp"
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
#define ID_CUSTOMPROPERTYDIALOG 10000
#define ID_PROPERTY_CHOICES 10001
#define ID_PROPERTY_CHOICE_ADD 10005
#define ID_PROPERTY_CHOICE_REMOVE 10006
////@end control identifiers

/*!
 * ctCustomPropertyDialog class declaration
 */

class ctCustomPropertyDialog: public wxDialog
{    
public:
    /// Constructor
    ctCustomPropertyDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& caption = _("Edit Custom Property"));

    /// Creates the controls and sizers
    void CreateControls();

////@begin ctCustomPropertyDialog event handler declarations

    /// Update event handler for ID_PROPERTY_CHOICES
    void OnUpdatePropertyChoices( wxUpdateUIEvent& event );

    /// Event handler for ID_PROPERTY_CHOICE_ADD
    void OnPropertyChoiceAdd( wxCommandEvent& event );

    /// Update event handler for ID_PROPERTY_CHOICE_ADD
    void OnUpdatePropertyChoiceAdd( wxUpdateUIEvent& event );

    /// Event handler for ID_PROPERTY_CHOICE_REMOVE
    void OnPropertyChoiceRemove( wxCommandEvent& event );

    /// Update event handler for ID_PROPERTY_CHOICE_REMOVE
    void OnUpdatePropertyChoiceRemove( wxUpdateUIEvent& event );

////@end ctCustomPropertyDialog event handler declarations

////@begin ctCustomPropertyDialog member function declarations

////@end ctCustomPropertyDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

    void SetPropertyName(const wxString& name) { m_name = name; }
    wxString GetPropertyName() { return m_name; }

    void SetPropertyType(const wxString& type) { m_type = type; }
    wxString GetPropertyType() { return m_type; }

    void SetEditorType(const wxString& type) { m_editorType = type; }
    wxString GetEditorType() { return m_editorType; }

    void SetPropertyDescription(const wxString& descr) { m_description = descr; }
    wxString GetPropertyDescription() { return m_description; }

    void SetChoices(const wxArrayString& choices) ;
    wxArrayString GetChoices() { return m_choices; }

    DECLARE_CLASS( ctCustomPropertyDialog )
    DECLARE_EVENT_TABLE()

protected:
    wxString      m_name;
    wxString      m_type;
    wxString      m_description;
    wxString      m_editorType;
    wxArrayString m_choices;

    // Dialog controls
    wxTextCtrl*   m_customPropertyName;
    wxTextCtrl*   m_customPropertyDescription;
    wxChoice*     m_customPrototype;
    wxChoice*     m_customPropertyEditorType;
    wxListBox*    m_propertyChoices;
};

#endif
    // _CUSTOMPROPERTYDIALOG_H_
