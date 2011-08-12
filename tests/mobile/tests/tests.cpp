/////////////////////////////////////////////////////////////////////////////
// Name:        tests.cpp
// Purpose:     Control list initializer (part of an attempt to test implementations of wxiOS controls)
// Author:      Linas Valiukas
// Modified by: 
// Created:     2011-07-27
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "tests.h"

#include "tests_button.h"
#include "tests_notebook.h"
#include "tests_navctrl.h"
#include "tests_stattext.h"
#include "tests_textctrl.h"
#include "tests_webctrl.h"
#include "tests_checkbox.h"
#include "tests_toolbar.h"
#include "tests_gauge.h"
#include "tests_pagectrl.h"
#include "tests_segctrl.h"
#include "tests_scrolwin.h"


#pragma mark -
#pragma mark Tests table data source

BEGIN_EVENT_TABLE(MobileTestsDataSource, wxTableDataSource)
    EVT_TABLE_ROW_SELECTED(wxID_ANY, MobileTestsDataSource::OnSelectRow)
END_EVENT_TABLE()

MobileTestsDataSource::MobileTestsDataSource(wxNavigationCtrl* ctrl)
{
    m_navCtrl = ctrl;
    
    //
    // Tests
    //
            
    // wxButton
    m_testNames.Add(_("wxButton"));
    m_testDescriptions.Add(_("This shows a text button."));
    m_testPanels.Add(new MobileTestsWxButtonPanel());
    
    // wxNotebook
    m_testNames.Add(_("wxNotebook"));
    m_testDescriptions.Add(_("This is an implementation of the standard wxNotebook class."));
    m_testPanels.Add(new MobileTestsWxNotebookPanel());

    // wxNavigationCtrl
    m_testNames.Add(_("wxNavigationCtrl"));
    m_testDescriptions.Add(_("This class makes it easy to implement a style of interface similar to a web browser, where an action in the currently displayed window causes another window to replace it, and allowing a return to the previously displayed window using the Back button."));
    m_testPanels.Add(new MobileTestsWxNavCtrlPanel());

    // wxLabel
    m_testNames.Add(_("wxStaticText"));
    m_testDescriptions.Add(_("A control showing a text string."));
    m_testPanels.Add(new MobileTestsWxStaticTextPanel());

    // wxTextCtrl
    m_testNames.Add(_("wxTextCtrl"));
    m_testDescriptions.Add(_("A single or multi-line text control."));
    m_testPanels.Add(new MobileTestsWxTextCtrlPanel());

    // wxWebKitCtrl
    m_testNames.Add(_("wxWebKitCtrl"));
    m_testDescriptions.Add(_("A control for displaying remote web pages."));
    m_testPanels.Add(new MobileTestsWxWebCtrlPanel());

    // wxCheckBox
    m_testNames.Add(_("wxCheckBox"));
    m_testDescriptions.Add(_("A checkbox is a labelled box which by default is either on (checkmark is visible) or off (no checkmark)."));
    m_testPanels.Add(new MobileTestsWxCheckBoxPanel());
    
    // wxToolBar
    m_testNames.Add(_("wxToolBar"));
    m_testDescriptions.Add(_("A horizontal toolbar containing text and/or bitmap buttons."));
    m_testPanels.Add(new MobileTestsWxToolBarPanel());

    // wxGauge
    m_testNames.Add(_("wxGauge"));
    m_testDescriptions.Add(_("A gauge class, representing a range and current value."));
    m_testPanels.Add(new MobileTestsWxGaugePanel());

    // wxPageCtrl
    m_testNames.Add(_("wxPageCtrl"));
    m_testDescriptions.Add(_("A control representing available pages as a row of dots, with one dot selected to represent the current page."));
    m_testPanels.Add(new MobileTestsWxPageCtrlPanel());

    // wxSegmentedCtrl
    m_testNames.Add(_("wxSegmentedCtrl"));
    m_testDescriptions.Add(_("A class with the same API as wxMoTabCtrl, but displaying a space-efficient row of buttons."));
    m_testPanels.Add(new MobileTestsWxSegmentedCtrlPanel());

    // wxScrolledWindow
    m_testNames.Add(_("wxScrolledWindow"));
    m_testDescriptions.Add(_("A container that scrolls its content."));
    m_testPanels.Add(new MobileTestsWxScrolledWindowPanel());
    
    //
    // --
    //
    
    // Integrity check
    wxASSERT_MSG(m_testNames.Count() == m_testDescriptions.Count(), "Test names count doesn't match test descriptions count.");
    wxASSERT_MSG(m_testNames.Count() == m_testPanels.Count(), "Test names count doesn't match test panels count.");
}

// Returns a table cell for the give location.
wxTableCell* MobileTestsDataSource::GetCell(wxTableCtrl* ctrl, const wxTablePath& path)
{
    int section = path.GetSection();
    int row = path.GetRow();
    
    wxTableCell* cell = ctrl->GetReusableCell(wxT("TestsCell"));
    if (!cell) {
        cell = new wxTableCell(ctrl, wxT("TestsCell"), wxTableCell::CellStyleSubtitle);
    }
    
    cell->SetText(m_testNames[row]);
    cell->SetDetailText(m_testDescriptions[row]);
    
    return cell;
}

// Returns the number of sections in the table.
int MobileTestsDataSource::GetSectionCount(wxTableCtrl* WXUNUSED(ctrl)) const
{
    return 1;
}

// Returns the number of rows in a section.
int MobileTestsDataSource::GetRowCount(wxTableCtrl* WXUNUSED(ctrl), int section) const
{
    return m_testNames.Count();
}

// Returns the number of rows in a section.
wxArrayString MobileTestsDataSource::GetSectionTitles(wxTableCtrl* WXUNUSED(ctrl)) const
{
    wxArrayString titles;
    return titles;
}

/// Returns an array of strings, each one corresponding to an index title to be shown
/// on the right hand side of a plain list.
wxArrayString MobileTestsDataSource::GetIndexTitles(wxTableCtrl* WXUNUSED(ctrl)) const
{
    wxArrayString index;
    return index;
}

void MobileTestsDataSource::OnSelectRow(wxTableCtrlEvent& event)
{
    wxTablePath path = event.GetPath();
    
    wxTableCtrl* tableCtrl = event.GetTableCtrl();
    if (tableCtrl) {
        tableCtrl->Deselect(event.GetPath());
    }
    
    wxASSERT_MSG(path.GetRow() < m_testNames.Count(), "Unknown test selected");
    
    MobileTestsWxPanel *testPanel = m_testPanels[path.GetRow()];
    testPanel->CreateWithControls(m_navCtrl, wxID_ANY, wxPoint(-200, -200), wxSize(10, 10), 0);
    testPanel->SetBackgroundColour(wxColour(wxT("#E3E4FF")));

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    testPanel->SetSizer(sizer);

    PushWindow(testPanel, wxEmptyString);
}

// Utility to push a table onto the navigation controller
void MobileTestsDataSource::PushTable(wxTableCtrl* tableCtrl, wxTableDataSource* dataSource, const wxString& title)
{
    wxViewController* controller = new wxViewController(title);
    tableCtrl->Show(false);
    controller->SetWindow(tableCtrl);
    
    tableCtrl->SetDataSource(dataSource, true /* own this data source */);    
    m_navCtrl->PushController(controller);
    tableCtrl->Show(true);
    
    tableCtrl->ReloadData();
}

// Utility to push a window onto the navigation controller
void MobileTestsDataSource::PushWindow(wxWindow* win, const wxString& title)
{
    wxViewController* controller = new wxViewController(title);
    win->Show(false);
    controller->SetWindow(win);
    m_navCtrl->PushController(controller);
    win->Show(true);
}


#pragma mark -
#pragma mark Tests table controller

MobileTestsTableController::MobileTestsTableController(wxNavigationCtrl* ctrl): wxViewController("wxiOS tests")
{
    m_navCtrl = ctrl;
    
    // Create off-screen to avoid flicker
    wxTableCtrl* tableCtrl = new wxTableCtrl(ctrl, wxID_ANY, wxPoint(-100, -100), wxSize(10, 10), wxTC_GROUPED);
    MobileTestsDataSource *rootDS = new MobileTestsDataSource(ctrl);
    tableCtrl->SetDataSource(rootDS, true);
    tableCtrl->ReloadData();
    SetWindow(tableCtrl);
}



#pragma mark -
#pragma mark wxFrame

//IMPLEMENT_CLASS( MobileTestsFrame, wxFrame )

BEGIN_EVENT_TABLE( MobileTestsFrame, wxFrame )
END_EVENT_TABLE()

/*
 * DemoFrame constructors
 */

MobileTestsFrame::MobileTestsFrame() : wxFrame(NULL, wxID_ANY, wxEmptyString)
{
    Init();
}

MobileTestsFrame::MobileTestsFrame( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create( parent, id, caption, pos, size, style );
}


/*
 * DemoFrame creator
 */

bool MobileTestsFrame::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    if ( !wxFrame::Create( parent, id, caption, pos, size, style )) {
        return false;
    }
    
    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(topSizer);

    // Create table view of control list
    wxNavigationCtrl* navCtrl = new wxNavigationCtrl(this, wxID_ANY);
    topSizer->Add(navCtrl);

    MobileTestsTableController *tableController = new MobileTestsTableController(navCtrl);
    navCtrl->PushController(tableController);
    
    return true;
}


/*
 * DemoFrame destructor
 */

MobileTestsFrame::~MobileTestsFrame()
{

}


/*
 * Member initialisation
 */

void MobileTestsFrame::Init()
{

}



#pragma mark -
#pragma mark wxApp

IMPLEMENT_APP( MobileTestsApp )

IMPLEMENT_CLASS( MobileTestsApp, wxApp )

BEGIN_EVENT_TABLE( MobileTestsApp, wxApp )
END_EVENT_TABLE()


MobileTestsApp::MobileTestsApp()
{
    Init();
}

void MobileTestsApp::Init()
{
    // Log everything
    wxLog::SetLogLevel(wxLOG_Max);
    wxLog::SetVerbose();
    
    // Log to STDOUT
    wxLog *logger_cout = new wxLogStream(&std::cout);
    wxLog::SetActiveTarget(logger_cout);
}

bool MobileTestsApp::OnInit()
{
    wxApp::OnInit();
    
    // PNG handler added in OnInit already
#if wxUSE_XPM
    wxImage::AddHandler(new wxXPMHandler);
#endif
#if wxUSE_LIBJPEG
    wxImage::AddHandler(new wxJPEGHandler);
#endif
#if wxUSE_GIF
    wxImage::AddHandler(new wxGIFHandler);
#endif
#if wxUSE_LIBPNG
    wxImage::AddHandler(new wxPNGHandler);
#endif
    MobileTestsFrame* mainWindow = new MobileTestsFrame( NULL );
    mainWindow->Show(true);
    
    return true;
}

int MobileTestsApp::OnExit()
{    
    return wxApp::OnExit();
}
