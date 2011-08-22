/*
 *  tests_tablectrl.cpp
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-20.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "tests_tablectrl.h"
#include "tests_bitmap.h"

#include "wx/mstream.h"


BEGIN_EVENT_TABLE(MobileTestsWxTableCtrlPanel, wxPanel)
END_EVENT_TABLE()


bool MobileTestsWxTableCtrlPanel::CreateWithControls(wxWindow* parent,
                                                     wxWindowID id,
                                                     const wxPoint& pos,
                                                     const wxSize& size,
                                                     long style,
                                                     const wxString& name)
{
    if ( !wxPanel::Create(parent, id, pos, size, style, name)) {
        wxFAIL_MSG("Unable to create parent wxPanel");
        return false;
    }
    
    if ( !CreateControls()) {
        wxFAIL_MSG("Unable to create controls for this test panel");
        return false;
    }
    
    return true;
}

bool MobileTestsWxTableCtrlPanel::CreateControls()
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer);
    
    //
    // Table controller
    // 
    
    m_tableCtrl = new wxTableCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTC_GROUPED);
    sizer->Add(m_tableCtrl, 1, wxEXPAND|wxALL, 5);
    m_tableCtrl->Connect(wxEVT_COMMAND_TABLE_ROW_SELECTED, wxTableCtrlEventHandler(MobileTestsWxTableCtrlPanel::OnTableRowSelected), NULL, this);
    m_tableCtrl->SetDataSource(new MobileTestsWxTableCtrlRootDataSource(), true);
    m_tableCtrl->ReloadData();
    
    return true;
}

void MobileTestsWxTableCtrlPanel::OnTableRowSelected(wxTableCtrlEvent& event)
{
    int row = int(event.GetPath().GetRow());
    wxLogMessage("Selected row %d", row);
    
    MobileTestsWxTableCtrlTestsDataSource* dataSource = NULL;
    wxTableCtrl* pushedTableCtrl = NULL;
    wxString title = wxEmptyString;
    
    // Tables
    if (row == 0)
    {
        pushedTableCtrl = new wxTableCtrl(m_navCtrl, wxID_ANY, wxPoint(-200, -200), wxSize(10, 10), wxTC_GROUPED);
        dataSource = new MobileTestsWxTableCtrlTestsDataSource(wxTableCell::CellStyleDefault);
    }
    else if (row == 1)
    {
        pushedTableCtrl = new wxTableCtrl(m_navCtrl, wxID_ANY, wxPoint(-200, -200), wxSize(10, 10), wxTC_GROUPED);
        dataSource = new MobileTestsWxTableCtrlTestsDataSource(wxTableCell::CellStyleValue1);
    }
    else if (row == 2)
    {
        pushedTableCtrl = new wxTableCtrl(m_navCtrl, wxID_ANY, wxPoint(-200, -200), wxSize(10, 10), wxTC_GROUPED);
        dataSource = new MobileTestsWxTableCtrlTestsDataSource(wxTableCell::CellStyleValue2);
    }
    else if (row == 3)
    {
        pushedTableCtrl = new wxTableCtrl(m_navCtrl, wxID_ANY, wxPoint(-200, -200), wxSize(10, 10), wxTC_GROUPED);
        dataSource = new MobileTestsWxTableCtrlTestsDataSource(wxTableCell::CellStyleSubtitle);
    }
    else if (row == 4)
    {
        pushedTableCtrl = new wxTableCtrl(m_navCtrl, wxID_ANY, wxPoint(-200, -200), wxSize(10, 10), wxTC_PLAIN);
        dataSource = new MobileTestsWxTableCtrlTestsDataSource;
    }
    
    pushedTableCtrl->SetDataSource(dataSource, true);
    
    m_navCtrl->PushTable(pushedTableCtrl, wxEmptyString);
}


#pragma mark Root table data source

BEGIN_EVENT_TABLE(MobileTestsWxTableCtrlRootDataSource, wxTableDataSource)
END_EVENT_TABLE()

int MobileTestsWxTableCtrlRootDataSource::GetSectionCount(wxTableCtrl* ctrl) const
{
    return 1;
}

int MobileTestsWxTableCtrlRootDataSource::GetRowCount(wxTableCtrl* ctrl, int section) const
{
    return 6;
}

wxTableCell* MobileTestsWxTableCtrlRootDataSource::GetCell(wxTableCtrl* ctrl, const wxTablePath& path)
{
    int row = path.GetRow();
    
    wxTableCell* cell = ctrl->GetReusableCell(wxT("TestsCell"));
    if (!cell) {
        cell = new wxTableCell(ctrl, wxT("TestsCell"), wxTableCell::CellStyleDefault);
    }
    
    wxArrayString titles;
    titles.Add(_("Grouped table - Default style"));
    titles.Add(_("Grouped table - Value1 style"));
    titles.Add(_("Grouped table - Value2 style"));
    titles.Add(_("Grouped table - Subtitle style"));
    titles.Add(_("Plain table demo"));
    titles.Add(_("Editable table demo"));
    
    cell->SetText(titles[row]);
    cell->SetAccessoryType(wxTableCell::AccessoryTypeDisclosureIndicator);
        
    return cell;    
}


#pragma mark Main (tests) table data source

BEGIN_EVENT_TABLE(MobileTestsWxTableCtrlTestsDataSource, wxTableDataSource)
END_EVENT_TABLE()

// Returns a table cell for the give location.
wxTableCell* MobileTestsWxTableCtrlTestsDataSource::GetCell(wxTableCtrl* ctrl, const wxTablePath& path)
{
    int section = path.GetSection();
    int row = path.GetRow();
    
    wxString rowText(wxString::Format(_("Section %d, row %d"), int(section+1), int(row+1)));
    
    int accessory = section;
    wxString reuseName;
    if (accessory == 3)
        reuseName = wxT("DemoCellWithSwitch");
    else
        reuseName = wxT("DemoCell");
    
    wxTableCell* cell = ctrl->GetReusableCell(reuseName);
    
    if (!cell)
        cell = new wxTableCell(ctrl, reuseName, m_cellStyle);
    cell->SetText(rowText);
    cell->SetDetailText(_("Detail text."));
    
    if (ctrl->HasFlag(wxTC_GROUPED))
    {
        if (accessory == 0)
            cell->SetAccessoryType(wxTableCell::AccessoryTypeDisclosureIndicator);
        else if (accessory == 1)
            cell->SetAccessoryType(wxTableCell::AccessoryTypeDetailDisclosureButton);
        else if (accessory == 2)
            cell->SetAccessoryType(wxTableCell::AccessoryTypeCheckmark);
        else if (accessory == 3)
        {
            cell->SetAccessoryType(wxTableCell::AccessoryTypeNone);
            cell->SetAccessoryWindow(new wxCheckBox(ctrl, wxID_ANY));
        }
    }
    
    if (accessory == 4)
    {
        cell->SetIndentationWidth(20);
        cell->SetIndentationLevel(row);
    }
    else
        cell->SetIndentationLevel(0);
    
    {
        cell->SetBitmap(wxBitmap(tests_bitmap));
    }
    
    return cell;
}

// Returns the number of sections in the table.
int MobileTestsWxTableCtrlTestsDataSource::GetSectionCount(wxTableCtrl* WXUNUSED(ctrl)) const
{
    return 10;
}

// Returns the number of rows in a section.
int MobileTestsWxTableCtrlTestsDataSource::GetRowCount(wxTableCtrl* WXUNUSED(ctrl), int WXUNUSED(section)) const
{
    return 10;
}

// Returns the number of rows in a section.
wxArrayString MobileTestsWxTableCtrlTestsDataSource::GetSectionTitles(wxTableCtrl* WXUNUSED(ctrl)) const
{
    wxArrayString titles;
    size_t i;
    for (i = 0; i < 10; i++)
    {
        wxString sectionText(wxString::Format(_("Welcome to section %d"), int(i+1)));
        titles.Add(sectionText);
    }
    return titles;
}

/// Returns an array of strings, each one corresponding to an index title to be shown
/// on the right hand side of a plain list.
wxArrayString MobileTestsWxTableCtrlTestsDataSource::GetIndexTitles(wxTableCtrl* WXUNUSED(ctrl)) const
{
    wxArrayString index;
    size_t i;
    for (i = 0; i < 10; i++)
    {
        index.Add(wxString::Format(wxT("%c"), (char) 'A' + i));
    }
    return index;
}
