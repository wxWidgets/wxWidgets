/////////////////////////////////////////////////////////////////////////////
// Name:        custompropertydialog.cpp
// Purpose:     Custom property dialog
// Author:      Julian Smart
// Modified by:
// Created:     2003-06-04
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "custompropertydialog.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP

#include "wx/wx.h"
#include "wx/statline.h"
#include "wx/splitter.h"
#include "wx/scrolwin.h"
#include "wx/spinctrl.h"
#include "wx/spinbutt.h"

#endif

#include "wx/cshelp.h"
#include "wx/valgen.h"
#include "custompropertydialog.h"

////@begin XPM images
////@end XPM images

/*!
 * ctCustomPropertyDialog type definition
 */

IMPLEMENT_CLASS( ctCustomPropertyDialog, wxDialog )

/*!
 * ctCustomPropertyDialog event table definition
 */

BEGIN_EVENT_TABLE( ctCustomPropertyDialog, wxDialog )

////@begin ctCustomPropertyDialog event table entries
    EVT_UPDATE_UI( ID_PROPERTY_CHOICES, ctCustomPropertyDialog::OnUpdatePropertyChoices )

    EVT_BUTTON( ID_PROPERTY_CHOICE_ADD, ctCustomPropertyDialog::OnPropertyChoiceAdd )
    EVT_UPDATE_UI( ID_PROPERTY_CHOICE_ADD, ctCustomPropertyDialog::OnUpdatePropertyChoiceAdd )

    EVT_BUTTON( ID_PROPERTY_CHOICE_REMOVE, ctCustomPropertyDialog::OnPropertyChoiceRemove )
    EVT_UPDATE_UI( ID_PROPERTY_CHOICE_REMOVE, ctCustomPropertyDialog::OnUpdatePropertyChoiceRemove )

////@end ctCustomPropertyDialog event table entries

END_EVENT_TABLE()

/*!
 * ctCustomPropertyDialog constructor
 */

ctCustomPropertyDialog::ctCustomPropertyDialog( wxWindow* parent, wxWindowID id, const wxString& caption)
{
    m_type = wxT("string");

    wxDialog::Create( parent, id, caption);

    CreateControls();
}

/*!
 * Control creation for ctCustomPropertyDialog
 */

void ctCustomPropertyDialog::CreateControls()
{
////@begin ctCustomPropertyDialog content construction

    wxArrayString items;

    ctCustomPropertyDialog* item1 = this;

    wxBoxSizer* item2 = new wxBoxSizer(wxVERTICAL);
    item1->SetSizer(item2);

    wxBoxSizer* item3 = new wxBoxSizer(wxVERTICAL);
    item2->Add(item3, 1, wxGROW|wxALL, 5);

    wxStaticText* item4 = new wxStaticText(item1, wxID_STATIC, _("&Enter name, type and description for your custom property."), wxDefaultPosition, wxDefaultSize, 0);
    item3->Add(item4, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticText* item5 = new wxStaticText(item1, wxID_STATIC, _("&Name:"), wxDefaultPosition, wxDefaultSize, 0);
    item3->Add(item5, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    m_customPropertyName = new wxTextCtrl(item1, wxID_ANY);
    item3->Add(m_customPropertyName, 0, wxGROW|wxALL, 5);

    wxBoxSizer* item7 = new wxBoxSizer(wxHORIZONTAL);
    item3->Add(item7, 0, wxGROW, 5);

    wxBoxSizer* item8 = new wxBoxSizer(wxVERTICAL);
    item7->Add(item8, 1, wxGROW, 5);

    wxStaticText* item9 = new wxStaticText(item1, wxID_STATIC, _("&Data type:"), wxDefaultPosition, wxDefaultSize, 0);
    item8->Add(item9, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    items.Empty();
    items.Add(_("string"));
    items.Add(_("bool"));
    items.Add(_("double"));
    items.Add(_("long"));
    m_customPrototype = new wxChoice(item1, wxID_ANY, wxDefaultPosition, wxDefaultSize, items);
    m_customPrototype->SetStringSelection(_("string"));
    item8->Add(m_customPrototype, 1, wxGROW|wxALL, 5);

    wxBoxSizer* item11 = new wxBoxSizer(wxVERTICAL);
    item7->Add(item11, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* item12 = new wxStaticText(item1, wxID_STATIC, _("&Editor type:"), wxDefaultPosition, wxDefaultSize, 0);
    item11->Add(item12, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    items.Empty();
    items.Add(_("string"));
    items.Add(_("choice"));
    items.Add(_("bool"));
    items.Add(_("float"));
    items.Add(_("integer"));
    items.Add(_("configitems"));
    m_customPropertyEditorType = new wxChoice(item1, wxID_ANY, wxDefaultPosition, wxDefaultSize, items);
    m_customPropertyEditorType->SetStringSelection(_("string"));
    item11->Add(m_customPropertyEditorType, 1, wxGROW|wxALL, 5);

    wxStaticBox* item14Static = new wxStaticBox(item1, wxID_ANY, _("Choices"));
    wxStaticBoxSizer* item14 = new wxStaticBoxSizer(item14Static, wxHORIZONTAL);
    item3->Add(item14, 0, wxGROW|wxALL, 5);

    wxString* item15Strings = NULL;
    m_propertyChoices = new wxListBox(item1, ID_PROPERTY_CHOICES, wxDefaultPosition, wxDefaultSize, 0, item15Strings, wxLB_SINGLE);
    item14->Add(m_propertyChoices, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* item16 = new wxBoxSizer(wxVERTICAL);
    item14->Add(item16, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxButton* item17 = new wxButton(item1, ID_PROPERTY_CHOICE_ADD, _("&Add..."), wxDefaultPosition, wxDefaultSize, 0);
    item16->Add(item17, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* item18 = new wxButton(item1, ID_PROPERTY_CHOICE_REMOVE, _("&Remove"), wxDefaultPosition, wxDefaultSize, 0);
    item16->Add(item18, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* item19 = new wxStaticText(item1, wxID_STATIC, _("&Description:"), wxDefaultPosition, wxDefaultSize, 0);
    item3->Add(item19, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    m_customPropertyDescription = new wxTextCtrl(item1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH);
    item3->Add(m_customPropertyDescription, 1, wxGROW|wxALL, 5);

    wxBoxSizer* item21 = new wxBoxSizer(wxHORIZONTAL);
    item3->Add(item21, 0, wxGROW|wxALL, 5);

    item21->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* item23 = new wxButton(item1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0);
    item21->Add(item23, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* item24 = new wxButton(item1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0);
    item21->Add(item24, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* item25 = new wxButton(item1, wxID_HELP, _("&Help"), wxDefaultPosition, wxDefaultSize, 0);
    item21->Add(item25, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end ctCustomPropertyDialog content construction

    // Add validators
    m_customPropertyName->SetValidator(wxGenericValidator(& m_name));
    m_customPrototype->SetValidator(wxGenericValidator(& m_type));
    m_customPropertyEditorType->SetValidator(wxGenericValidator(& m_editorType));
    m_customPropertyDescription->SetValidator(wxGenericValidator(& m_description));
}

/*!
 * Should we show tooltips?
 */

bool ctCustomPropertyDialog::ShowToolTips()
{
  return true;
}

/*!
 * Update event handler for ID_PROPERTY_CHOICES
 */

void ctCustomPropertyDialog::OnUpdatePropertyChoices( wxUpdateUIEvent& event )
{
    if(m_customPrototype)
        event.Enable( m_customPrototype->GetSelection() > -1 && m_customPrototype->GetStringSelection() == wxT("choice") );
}

/*!
 * Event handler for ID_PROPERTY_CHOICE_ADD
 */

void ctCustomPropertyDialog::OnPropertyChoiceAdd( wxCommandEvent& WXUNUSED(event) )
{
    if(m_customPrototype)
    {
        if ( m_customPropertyEditorType->GetSelection() > -1 && m_customPropertyEditorType->GetStringSelection() == wxT("choice") )
        {
            wxString str = wxGetTextFromUser(_T("New choice"), _("Add choice"));
            if (!str.IsEmpty() && m_propertyChoices)
            {
                m_propertyChoices->Append(str);
                m_choices.Add(str);
            }
        }
    }
}

/*!
 * Update event handler for ID_PROPERTY_CHOICE_ADD
 */

void ctCustomPropertyDialog::OnUpdatePropertyChoiceAdd( wxUpdateUIEvent& event )
{
    if(m_customPropertyEditorType)
        event.Enable( m_customPropertyEditorType->GetSelection() > -1 && 
                      m_customPropertyEditorType->GetStringSelection() == wxT("choice") );
}

/*!
 * Event handler for ID_PROPERTY_CHOICE_REMOVE
 */

void ctCustomPropertyDialog::OnPropertyChoiceRemove( wxCommandEvent& WXUNUSED(event) )
{
    if (m_propertyChoices && m_propertyChoices->GetSelection() > -1)
    {
        m_propertyChoices->Delete(m_propertyChoices->GetSelection());
        m_choices.RemoveAt(m_propertyChoices->GetSelection());
    }
}

/*!
 * Update event handler for ID_PROPERTY_CHOICE_REMOVE
 */

void ctCustomPropertyDialog::OnUpdatePropertyChoiceRemove( wxUpdateUIEvent& event )
{
    if (m_customPropertyEditorType && m_propertyChoices)
        event.Enable( m_customPropertyEditorType->GetSelection() > -1 && 
                      m_customPropertyEditorType->GetStringSelection() == wxT("choice") &&
                      m_propertyChoices->GetSelection() > -1 );
}

void ctCustomPropertyDialog::SetChoices(const wxArrayString& choices)
{
    size_t i, len = choices.GetCount();
    if (m_propertyChoices)
        for (i = 0; i < len; i++)
            m_propertyChoices->Append(m_choices[i]);
}
