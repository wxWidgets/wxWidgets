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

#ifdef __GNUG__
#pragma implementation "custompropertydialog.h"
#endif

#include <wx/wx.h>
#include <wx/cshelp.h>
#include <wx/statline.h>
#include <wx/splitter.h>
#include <wx/scrolwin.h>
#include <wx/spinctrl.h>
#include <wx/spinbutt.h>
#include <wx/valgen.h>

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

ctCustomPropertyDialog::ctCustomPropertyDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    m_type = wxT("string");

    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
}

/*!
 * Control creation for ctCustomPropertyDialog
 */

void ctCustomPropertyDialog::CreateControls()
{    
////@begin ctCustomPropertyDialog content construction

    ctCustomPropertyDialog* item1 = this;

    wxBoxSizer* item2 = new wxBoxSizer(wxVERTICAL);
    item1->SetSizer(item2);
    item1->SetAutoLayout(TRUE);

    wxBoxSizer* item3 = new wxBoxSizer(wxVERTICAL);
    item2->Add(item3, 1, wxGROW|wxALL, 5);

    wxStaticText* item4 = new wxStaticText(item1, wxID_STATIC, _("&Enter name, type and description for your custom property."), wxDefaultPosition, wxDefaultSize, 0);
    item3->Add(item4, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticText* item5 = new wxStaticText(item1, wxID_STATIC, _("&Name:"), wxDefaultPosition, wxDefaultSize, 0);
    item3->Add(item5, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* item6 = new wxTextCtrl(item1, ID_CUSTOMPROPERTYNAME, _(""), wxDefaultPosition, wxDefaultSize, 0);
    item3->Add(item6, 0, wxGROW|wxALL, 5);

    wxBoxSizer* item7 = new wxBoxSizer(wxHORIZONTAL);
    item3->Add(item7, 0, wxGROW, 5);

    wxBoxSizer* item8 = new wxBoxSizer(wxVERTICAL);
    item7->Add(item8, 1, wxGROW, 5);

    wxStaticText* item9 = new wxStaticText(item1, wxID_STATIC, _("&Data type:"), wxDefaultPosition, wxDefaultSize, 0);
    item8->Add(item9, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    wxString item10Strings[] = {
        _("string"),
        _("bool"),
        _("double"),
        _("long")
    };
    wxChoice* item10 = new wxChoice(item1, ID_CUSTOMPROPERTYTYPE, wxDefaultPosition, wxDefaultSize, 4, item10Strings, 0);
    item10->SetStringSelection(_("string"));
    item8->Add(item10, 1, wxGROW|wxALL, 5);

    wxBoxSizer* item11 = new wxBoxSizer(wxVERTICAL);
    item7->Add(item11, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* item12 = new wxStaticText(item1, wxID_STATIC, _("&Editor type:"), wxDefaultPosition, wxDefaultSize, 0);
    item11->Add(item12, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    wxString item13Strings[] = {
        _("string"),
        _("choice"),
        _("bool"),
        _("float"),
        _("integer"),
        _("configitems")
    };
    wxChoice* item13 = new wxChoice(item1, ID_CUSTOMPROPERTYEDITORTYPE, wxDefaultPosition, wxDefaultSize, 6, item13Strings, 0);
    item13->SetStringSelection(_("string"));
    item11->Add(item13, 1, wxGROW|wxALL, 5);

    wxStaticBox* item14Static = new wxStaticBox(item1, -1, _("Choices"));
    wxStaticBoxSizer* item14 = new wxStaticBoxSizer(item14Static, wxHORIZONTAL);
    item3->Add(item14, 0, wxGROW|wxALL, 5);

    wxString* item15Strings = NULL;
    wxListBox* item15 = new wxListBox(item1, ID_PROPERTY_CHOICES, wxDefaultPosition, wxDefaultSize, 0, item15Strings, wxLB_SINGLE);
    item14->Add(item15, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* item16 = new wxBoxSizer(wxVERTICAL);
    item14->Add(item16, 0, wxALIGN_CENTER_VERTICAL, 5);

    wxButton* item17 = new wxButton(item1, ID_PROPERTY_CHOICE_ADD, _("&Add..."), wxDefaultPosition, wxDefaultSize, 0);
    item16->Add(item17, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* item18 = new wxButton(item1, ID_PROPERTY_CHOICE_REMOVE, _("&Remove"), wxDefaultPosition, wxDefaultSize, 0);
    item16->Add(item18, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* item19 = new wxStaticText(item1, wxID_STATIC, _("&Description:"), wxDefaultPosition, wxDefaultSize, 0);
    item3->Add(item19, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    wxTextCtrl* item20 = new wxTextCtrl(item1, ID_CUSTOMPROPERTYDESCRIPTION, _(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH);
    item3->Add(item20, 1, wxGROW|wxALL, 5);

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
    FindWindow(ID_CUSTOMPROPERTYNAME)->SetValidator(wxGenericValidator(& m_name));
    FindWindow(ID_CUSTOMPROPERTYTYPE)->SetValidator(wxGenericValidator(& m_type));
    FindWindow(ID_CUSTOMPROPERTYEDITORTYPE)->SetValidator(wxGenericValidator(& m_editorType));
    FindWindow(ID_CUSTOMPROPERTYDESCRIPTION)->SetValidator(wxGenericValidator(& m_description));
}

/*!
 * Should we show tooltips?
 */

bool ctCustomPropertyDialog::ShowToolTips()
{
  return TRUE;
}

/*!
 * Update event handler for ID_PROPERTY_CHOICES
 */

void ctCustomPropertyDialog::OnUpdatePropertyChoices( wxUpdateUIEvent& event )
{
    wxChoice* choice = (wxChoice* ) FindWindow(ID_CUSTOMPROPERTYTYPE);
    event.Enable( choice->GetSelection() > -1 && choice->GetStringSelection() == wxT("choice") );
}

/*!
 * Event handler for ID_PROPERTY_CHOICE_ADD
 */

void ctCustomPropertyDialog::OnPropertyChoiceAdd( wxCommandEvent& event )
{
    wxChoice* choice = (wxChoice* ) FindWindow(ID_CUSTOMPROPERTYTYPE);
    if ( choice->GetSelection() > -1 && choice->GetStringSelection() == wxT("choice") )
    {
        wxString str = wxGetTextFromUser(_T("New choice"), _("Add choice"));
        if (!str.IsEmpty())
        {
            wxListBox* listBox = (wxListBox* ) FindWindow(ID_PROPERTY_CHOICES);
            listBox->Append(str);
            m_choices.Add(str);
        }
    }
}

/*!
 * Update event handler for ID_PROPERTY_CHOICE_ADD
 */

void ctCustomPropertyDialog::OnUpdatePropertyChoiceAdd( wxUpdateUIEvent& event )
{
    wxChoice* choice = (wxChoice* ) FindWindow(ID_CUSTOMPROPERTYEDITORTYPE);
    event.Enable( choice->GetSelection() > -1 && choice->GetStringSelection() == wxT("choice") );
}

/*!
 * Event handler for ID_PROPERTY_CHOICE_REMOVE
 */

void ctCustomPropertyDialog::OnPropertyChoiceRemove( wxCommandEvent& event )
{
    wxChoice* choice = (wxChoice* ) FindWindow(ID_CUSTOMPROPERTYEDITORTYPE);
    wxListBox* listBox = (wxListBox* ) FindWindow(ID_PROPERTY_CHOICES);
    if (listBox->GetSelection() > -1)
    {
        listBox->Delete(listBox->GetSelection());
        m_choices.Remove(listBox->GetSelection());
    }
}

/*!
 * Update event handler for ID_PROPERTY_CHOICE_REMOVE
 */

void ctCustomPropertyDialog::OnUpdatePropertyChoiceRemove( wxUpdateUIEvent& event )
{
    wxChoice* choice = (wxChoice* ) FindWindow(ID_CUSTOMPROPERTYEDITORTYPE);
    wxListBox* listBox = (wxListBox* ) FindWindow(ID_PROPERTY_CHOICES);
    event.Enable( choice->GetSelection() > -1 && choice->GetStringSelection() == wxT("choice") &&
                  listBox->GetSelection() > -1 );
}

void ctCustomPropertyDialog::SetChoices(const wxArrayString& choices)
{
    wxListBox* listBox = (wxListBox* ) FindWindow(ID_PROPERTY_CHOICES);
    size_t i, len = choices.GetCount();
    for (i = 0; i < len; i++)
        listBox->Append(m_choices[i]);
}
