/*
 *  tests_tablectrl.cpp
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-20.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "tests_tablectrl.h"

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
    
    MobileTestsWxTableCtrlTestsDataSource* dataSource = new MobileTestsWxTableCtrlTestsDataSource();
    wxTableCtrl* pushedTableCtrl = NULL;
    wxString title = wxEmptyString;
    
    switch (row) {
        case 0:
            title = _("Plain");
            pushedTableCtrl = new wxTableCtrl(m_navCtrl, wxID_ANY, wxPoint(100, 100), wxSize(400, 400), wxTC_PLAIN);
            break;
        case 1:
            title = _("Grouped");
            pushedTableCtrl = new wxTableCtrl(m_navCtrl, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTC_GROUPED);
            break;
        default:
            wxASSERT_MSG(false, "Unknown row index");
            break;
    };
    
    pushedTableCtrl->SetDataSource(dataSource, true);
    
    m_navCtrl->PushTable(pushedTableCtrl, wxEmptyString);
}


#pragma mark Root table data source

BEGIN_EVENT_TABLE(MobileTestsWxTableCtrlRootDataSource, wxTableDataSource)
END_EVENT_TABLE()
    
wxTableCell* MobileTestsWxTableCtrlRootDataSource::GetCell(wxTableCtrl* ctrl, const wxTablePath& path)
{
    int row = path.GetRow();
    
    wxTableCell* cell = ctrl->GetReusableCell(wxT("TestsCell"));
    if (!cell) {
        cell = new wxTableCell(ctrl, wxT("TestsCell"), wxTableCell::CellStyleDefault);
    }

    cell->SetAccessoryType(wxTableCell::AccessoryTypeDisclosureIndicator);
    
    switch (row) {
        case 0: cell->SetText(_("Plain"));                  break;
        case 1: cell->SetText(_("Grouped"));                break;
        default: wxASSERT_MSG(false, "Unknown row index");  break;
    };
    
    return cell;    
}
    
int MobileTestsWxTableCtrlRootDataSource::GetSectionCount(wxTableCtrl* ctrl) const
{
    return 1;
}
    
int MobileTestsWxTableCtrlRootDataSource::GetRowCount(wxTableCtrl* ctrl, int section) const
{
    return 2 /* plain + grouped */;
}


#pragma mark Main (tests) table data source

BEGIN_EVENT_TABLE(MobileTestsWxTableCtrlTestsDataSource, wxTableDataSource)
END_EVENT_TABLE()

wxTableCell* MobileTestsWxTableCtrlTestsDataSource::GetCell(wxTableCtrl* ctrl, const wxTablePath& path)
{
    int row = path.GetRow();
    
    wxTableCell* cell = ctrl->GetReusableCell(wxT("TestsCell"));
    if (!cell) {
        cell = new wxTableCell(ctrl, wxT("TestsCell"), wxTableCell::CellStyleDefault);
    }
    
    cell->SetAccessoryType(wxTableCell::AccessoryTypeDisclosureIndicator);
    
    switch (row) {
        case 0: cell->SetText(_("X"));                  break;
        case 1: cell->SetText(_("Y"));                break;
        default: wxASSERT_MSG(false, "Unknown row index");  break;
    };
    
    return cell;    
}

int MobileTestsWxTableCtrlTestsDataSource::GetSectionCount(wxTableCtrl* ctrl) const
{
    return 1;
}

int MobileTestsWxTableCtrlTestsDataSource::GetRowCount(wxTableCtrl* ctrl, int section) const
{
    return 2 /* plain + grouped */;
}
