/////////////////////////////////////////////////////////////////////////////
// Name:        configitemselector.cpp
// Purpose:     Selector for one or more config items
// Author:      Julian Smart
// Modified by:
// Created:     2003-06-04
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "configitemselector.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP

#include "wx/statline.h"
#include "wx/splitter.h"
#include "wx/scrolwin.h"
#include "wx/spinctrl.h"
#include "wx/spinbutt.h"

#endif

#include "wx/cshelp.h"
#include "wx/notebook.h"
#include "wx/valgen.h"
#include "configitemselector.h"
#include "configtooldoc.h"
#include "configtoolview.h"
#include "configitem.h"
#include "mainframe.h"
#include "wxconfigtool.h"

////@begin XPM images
////@end XPM images

/*!
 * ctConfigItemsSelector type definition
 */

IMPLEMENT_CLASS( ctConfigItemsSelector, wxDialog )

/*!
 * ctConfigItemsSelector event table definition
 */

BEGIN_EVENT_TABLE( ctConfigItemsSelector, wxDialog )

////@begin ctConfigItemsSelector event table entries
    EVT_BUTTON( ID_CONFIG_ADD, ctConfigItemsSelector::OnConfigAdd )
    EVT_UPDATE_UI( ID_CONFIG_ADD, ctConfigItemsSelector::OnUpdateConfigAdd )

    EVT_BUTTON( ID_CONFIG_REMOVE, ctConfigItemsSelector::OnConfigRemove )
    EVT_UPDATE_UI( ID_CONFIG_REMOVE, ctConfigItemsSelector::OnUpdateConfigRemove )

    EVT_BUTTON( wxID_OK, ctConfigItemsSelector::OnOk )

////@end ctConfigItemsSelector event table entries

END_EVENT_TABLE()

/*!
 * ctConfigItemsSelector constructor
 */

ctConfigItemsSelector::ctConfigItemsSelector( wxWindow* parent, wxWindowID id, const wxString& caption)
{
    wxDialog::Create( parent, id, caption);

    CreateControls();
    InitSourceConfigList();
}

/*!
 * Control creation for ctConfigItemsSelector
 */

void ctConfigItemsSelector::CreateControls()
{
////@begin ctConfigItemsSelector content construction

    ctConfigItemsSelector* item1 = this;

    wxBoxSizer* item2 = new wxBoxSizer(wxVERTICAL);
    item1->SetSizer(item2);

    wxBoxSizer* item3 = new wxBoxSizer(wxVERTICAL);
    item2->Add(item3, 1, wxGROW|wxALL, 5);

    wxStaticText* item4 = new wxStaticText(item1, wxID_STATIC, _("Please edit the list of configuration items by selecting from the\nlist below."), wxDefaultPosition, wxDefaultSize, 0);
    item3->Add(item4, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticText* item5 = new wxStaticText(item1, wxID_STATIC, _("&Available items:"), wxDefaultPosition, wxDefaultSize, 0);
    item3->Add(item5, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    wxString* item6Strings = NULL;
    wxListBox* item6 = new wxListBox(item1, ID_AVAILABLE_CONFIG_ITEMS, wxDefaultPosition, wxSize(wxDefaultCoord, 150), 0, item6Strings, wxLB_SINGLE|wxLB_SORT);
    item3->Add(item6, 1, wxGROW|wxALL, 5);

    wxStaticText* item7 = new wxStaticText(item1, wxID_STATIC, _("&List of configuration items:"), wxDefaultPosition, wxDefaultSize, 0);
    item3->Add(item7, 0, wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE, 5);

    wxBoxSizer* item8 = new wxBoxSizer(wxHORIZONTAL);
    item3->Add(item8, 0, wxGROW, 5);

    wxString* item9Strings = NULL;
    wxListBox* item9 = new wxListBox(item1, ID_CONFIG_ITEMS, wxDefaultPosition, wxSize(wxDefaultCoord, 100), 0, item9Strings, wxLB_SINGLE);
    item8->Add(item9, 1, wxGROW|wxALL, 5);

    wxBoxSizer* item10 = new wxBoxSizer(wxVERTICAL);
    item8->Add(item10, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* item11 = new wxButton(item1, ID_CONFIG_ADD, _("A&dd"), wxDefaultPosition, wxDefaultSize, 0);
    item10->Add(item11, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* item12 = new wxButton(item1, ID_CONFIG_REMOVE, _("&Remove"), wxDefaultPosition, wxDefaultSize, 0);
    item10->Add(item12, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxBoxSizer* item13 = new wxBoxSizer(wxHORIZONTAL);
    item3->Add(item13, 0, wxGROW, 5);

    item13->Add(5, 5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* item15 = new wxButton(item1, wxID_OK);
    item15->SetDefault();
    item13->Add(item15, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* item16 = new wxButton(item1, wxID_CANCEL);
    item13->Add(item16, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);
    Centre();
////@end ctConfigItemsSelector content construction
}

/*!
 * Event handler for ID_CONFIG_ADD
 */

void ctConfigItemsSelector::OnConfigAdd( wxCommandEvent& WXUNUSED(event) )
{
    wxListBox* masterList = wxDynamicCast(FindWindow(ID_AVAILABLE_CONFIG_ITEMS), wxListBox);
    wxListBox* listBox = wxDynamicCast(FindWindow(ID_CONFIG_ITEMS), wxListBox);
    if (masterList)
    {
        if (masterList->GetSelection() > -1)
        {
            wxString str = masterList->GetStringSelection();
            if (m_configItems.Index(str) == wxNOT_FOUND)
            {
                listBox->Append(str);
                m_configItems.Add(str);
            }
        }
    }
}

/*!
 * Event handler for ID_CONFIG_REMOVE
 */

void ctConfigItemsSelector::OnConfigRemove( wxCommandEvent& WXUNUSED(event) )
{
    wxListBox* listBox = wxDynamicCast(FindWindow(ID_CONFIG_ITEMS), wxListBox);
    if (listBox)
    {
        if (listBox->GetSelection() > -1)
        {
            wxString str = listBox->GetStringSelection();
            listBox->Delete(listBox->GetSelection());
            m_configItems.Remove(str);
        }
    }
}

/*!
 * Event handler for wxID_OK
 */

void ctConfigItemsSelector::OnOk( wxCommandEvent& event )
{
    // Replace with custom code
    event.Skip();
}

/*!
 * Should we show tooltips?
 */

bool ctConfigItemsSelector::ShowToolTips()
{
  return true;
}
/*!
 * Update event handler for ID_CONFIG_ADD
 */

void ctConfigItemsSelector::OnUpdateConfigAdd( wxUpdateUIEvent& event )
{
    wxListBox* masterList = wxDynamicCast(FindWindow(ID_AVAILABLE_CONFIG_ITEMS), wxListBox);
    event.Enable(masterList && masterList->GetSelection() != -1);
}

/*!
 * Update event handler for ID_CONFIG_REMOVE
 */

void ctConfigItemsSelector::OnUpdateConfigRemove( wxUpdateUIEvent& event )
{
    wxListBox* listBox = wxDynamicCast(FindWindow(ID_CONFIG_ITEMS), wxListBox);
    event.Enable(listBox && listBox->GetSelection() != -1);
}

/// Initialise the master list
void ctConfigItemsSelector::InitSourceConfigList(ctConfigItem* item)
{
    wxListBox* masterList = wxDynamicCast(FindWindow(ID_AVAILABLE_CONFIG_ITEMS), wxListBox);
    if (!item)
        item = wxGetApp().GetMainFrame()->GetDocument()->GetTopItem();

    if (!item)
        return;

    bool addIt = false;

    if (item->GetType() == ctTypeGroup)
        addIt = false;
    else if (item->GetType() == ctTypeCheckGroup)
        addIt = true;
    else if (item->GetType() == ctTypeRadioGroup)
        addIt = true;
    else if (item->GetType() == ctTypeString)
        addIt = true;
    else if (item->GetType() == ctTypeBoolCheck)
        addIt = true;
    else if (item->GetType() == ctTypeBoolRadio)
        addIt = true;
    else if (item->GetType() == ctTypeInteger)
        addIt = true;
    if (addIt)
    {
        masterList->Append(item->GetName());
    }

    wxObjectList::compatibility_iterator node = item->GetChildren().GetFirst();
    while (node)
    {
        ctConfigItem* child = (ctConfigItem*) node->GetData();
        InitSourceConfigList(child);

        node = node->GetNext();
    }
}

/// Set the initial list
void ctConfigItemsSelector::SetConfigList(const wxArrayString& items)
{
    m_configItems = items;
    wxListBox* listBox = wxDynamicCast(FindWindow(ID_CONFIG_ITEMS), wxListBox);
    listBox->Clear();

    size_t i;
    for (i = 0; i < items.GetCount(); i++)
        listBox->Append(items[i]);
}

