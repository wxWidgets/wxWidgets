/////////////////////////////////////////////////////////////////////////////
// Name:        configbrowser.cpp
// Purpose:     Configuration browser
// Author:      Julian Smart
// Modified by: 
// Created:     2003-08-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP

#include "wx/wx.h"
#include "wx/splitter.h"
#include "wx/treectrl.h"

#endif

#include "configbrowser.h"

////@begin XPM images
////@end XPM images

/*!
 * ctConfigurationBrowserWindow type definition
 */

IMPLEMENT_CLASS( ctConfigurationBrowserWindow, wxPanel )

/*!
 * ctConfigurationBrowserWindow event table definition
 */

BEGIN_EVENT_TABLE( ctConfigurationBrowserWindow, wxPanel )

////@begin ctConfigurationBrowserWindow event table entries
    EVT_TREE_SEL_CHANGED( ID_CONFIGURATION_BROWSER_TREECTRL, ctConfigurationBrowserWindow::OnConfigurationBrowserTreectrl )

////@end ctConfigurationBrowserWindow event table entries

END_EVENT_TABLE()

/*!
 * ctConfigurationBrowserWindow constructor
 */

ctConfigurationBrowserWindow::ctConfigurationBrowserWindow( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin ctConfigurationBrowserWindow member initialisation
////@end ctConfigurationBrowserWindow member initialisation

    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
}

/*!
 * Control creation for ctConfigurationBrowserWindow
 */

void ctConfigurationBrowserWindow::CreateControls()
{
////@begin ctConfigurationBrowserWindow content construction

    ctConfigurationBrowserWindow* item1 = this;

    wxBoxSizer* item2 = new wxBoxSizer(wxVERTICAL);
    item1->SetSizer(item2);

    wxSplitterWindow* item3 = new wxSplitterWindow(item1, ID_CONFIGBROWSER_SPLITTERWINDOW, wxDefaultPosition, wxSize(400, 400), wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER|wxNO_FULL_REPAINT_ON_RESIZE);
    wxTreeCtrl* item4 = new wxTreeCtrl(item3, ID_CONFIGURATION_BROWSER_TREECTRL, wxDefaultPosition, wxSize(100, 100), wxTR_SINGLE|wxNO_BORDER);
    ctConfigurationBrowserControlPanel* item5 = new ctConfigurationBrowserControlPanel(item3, ID_PANEL, wxDefaultPosition, wxSize(100, 80), wxNO_BORDER|wxTAB_TRAVERSAL);
    item3->SplitVertically(item4, item5, 200);
    item2->Add(item3, 1, wxGROW, 5);

////@end ctConfigurationBrowserWindow content construction
}

/*!
 * Event handler for ID_CONFIGURATION_BROWSER_TREECTRL
 */

void ctConfigurationBrowserWindow::OnConfigurationBrowserTreectrl( wxTreeEvent& event )
{
    // Replace with custom code
    event.Skip();
}

/*!
 * Should we show tooltips?
 */

bool ctConfigurationBrowserWindow::ShowToolTips()
{
  return true;
}

/*!
 * ctConfigurationBrowserControlPanel type definition
 */

IMPLEMENT_CLASS( ctConfigurationBrowserControlPanel, wxPanel )

/*!
 * ctConfigurationBrowserControlPanel event table definition
 */

BEGIN_EVENT_TABLE( ctConfigurationBrowserControlPanel, wxPanel )

////@begin ctConfigurationBrowserControlPanel event table entries
    EVT_BUTTON( ID_ADD_CONFIGURATION, ctConfigurationBrowserControlPanel::OnAddConfiguration )
    EVT_UPDATE_UI( ID_ADD_CONFIGURATION, ctConfigurationBrowserControlPanel::OnUpdateAddConfiguration )

    EVT_BUTTON( ID_REMOVE_CONFIGURATION, ctConfigurationBrowserControlPanel::OnRemoveConfiguration )
    EVT_UPDATE_UI( ID_REMOVE_CONFIGURATION, ctConfigurationBrowserControlPanel::OnUpdateRemoveConfiguration )

    EVT_BUTTON( ID_RENAME_CONFIGURATION, ctConfigurationBrowserControlPanel::OnRenameConfiguration )
    EVT_UPDATE_UI( ID_RENAME_CONFIGURATION, ctConfigurationBrowserControlPanel::OnUpdateRenameConfiguration )

////@end ctConfigurationBrowserControlPanel event table entries

END_EVENT_TABLE()

/*!
 * ctConfigurationBrowserControlPanel constructor
 */

ctConfigurationBrowserControlPanel::ctConfigurationBrowserControlPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin ctConfigurationBrowserControlPanel member initialisation
////@end ctConfigurationBrowserControlPanel member initialisation

////@begin ctConfigurationBrowserControlPanel creation
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
////@end ctConfigurationBrowserControlPanel creation
}

/*!
 * Control creation for ctConfigurationBrowserControlPanel
 */

void ctConfigurationBrowserControlPanel::CreateControls()
{
////@begin ctConfigurationBrowserControlPanel content construction

    ctConfigurationBrowserControlPanel* item5 = this;

    wxBoxSizer* item6 = new wxBoxSizer(wxVERTICAL);
    item5->SetSizer(item6);

    wxStaticText* item7 = new wxStaticText(item5, wxID_STATIC, _("Browse, add and remove configurations"), wxDefaultPosition, wxDefaultSize, 0);
    item6->Add(item7, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxADJUST_MINSIZE, 5);

    item6->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* item9 = new wxButton(item5, ID_ADD_CONFIGURATION, _("&Add..."), wxDefaultPosition, wxDefaultSize, 0);
    item6->Add(item9, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* item10 = new wxButton(item5, ID_REMOVE_CONFIGURATION, _("&Remove..."), wxDefaultPosition, wxDefaultSize, 0);
    item6->Add(item10, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxButton* item11 = new wxButton(item5, ID_RENAME_CONFIGURATION, _("&Rename..."), wxDefaultPosition, wxDefaultSize, 0);
    item6->Add(item11, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    item6->Add(5, 5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* item13 = new wxStaticText(item5, ID_CONFIGURATION_NAME, _("Configuration:"), wxDefaultPosition, wxDefaultSize, 0);
    item6->Add(item13, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP|wxADJUST_MINSIZE, 5);

    wxTextCtrl* item14 = new wxTextCtrl(item5, ID_CONFIGURATION_DESCRIPTION, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH);
    item6->Add(item14, 1, wxGROW|wxALL, 5);

////@end ctConfigurationBrowserControlPanel content construction
}

/*!
 * Event handler for ID_ADD_CONFIGURATION
 */

void ctConfigurationBrowserControlPanel::OnAddConfiguration( wxCommandEvent& event )
{
    // Replace with custom code
    event.Skip();
}

/*!
 * Update event handler for ID_ADD_CONFIGURATION
 */

void ctConfigurationBrowserControlPanel::OnUpdateAddConfiguration( wxUpdateUIEvent& event )
{
    // Replace with custom code
    event.Skip();
}

/*!
 * Event handler for ID_REMOVE_CONFIGURATION
 */

void ctConfigurationBrowserControlPanel::OnRemoveConfiguration( wxCommandEvent& event )
{
    // Replace with custom code
    event.Skip();
}

/*!
 * Update event handler for ID_REMOVE_CONFIGURATION
 */

void ctConfigurationBrowserControlPanel::OnUpdateRemoveConfiguration( wxUpdateUIEvent& event )
{
    // Replace with custom code
    event.Skip();
}

/*!
 * Event handler for ID_RENAME_CONFIGURATION
 */

void ctConfigurationBrowserControlPanel::OnRenameConfiguration( wxCommandEvent& event )
{
    // Replace with custom code
    event.Skip();
}

/*!
 * Update event handler for ID_RENAME_CONFIGURATION
 */

void ctConfigurationBrowserControlPanel::OnUpdateRenameConfiguration( wxUpdateUIEvent& event )
{
    // Replace with custom code
    event.Skip();
}

/*!
 * Should we show tooltips?
 */

bool ctConfigurationBrowserControlPanel::ShowToolTips()
{
  return true;
}
