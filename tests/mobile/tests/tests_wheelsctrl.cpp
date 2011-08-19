/*
 *  tests_wheelsctrl.cpp
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-18.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "tests_wheelsctrl.h"

BEGIN_EVENT_TABLE(MobileTestsWxWheelsCtrlPanel, wxPanel)
END_EVENT_TABLE()


bool MobileTestsWxWheelsCtrlPanel::CreateWithControls(wxWindow* parent,
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

bool MobileTestsWxWheelsCtrlPanel::CreateControls()
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer);
        
    
    //
    // Wheels control with text (string) data source, 2 components
    // 
    
    wxWheelsTextDataSource* stringDataSource = new wxWheelsTextDataSource(2);
    wxArrayString textDataStrings;
    textDataStrings.Add(_("First string"));
    textDataStrings.Add(_("A second string"));
    textDataStrings.Add(_("And here's another string"));
    textDataStrings.Add(_("Fourth string at your service"));
    textDataStrings.Add(_("Five strings are not quite enough"));
    textDataStrings.Add(_("So here's a sixth string"));
    stringDataSource->SetStrings(textDataStrings, 0);
    
    wxArrayString textDataStrings2;
    for (int i = 0; i < 50; i++) {
        textDataStrings2.Add(wxString::Format(wxT("%d"), i));
    }
    stringDataSource->SetStrings(textDataStrings2, 1);
    
    m_textWheelsCtrl = new wxWheelsCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
    sizer->Add(m_textWheelsCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    m_textWheelsCtrl->SetDataSource(stringDataSource, true /* control owns the data source */);
    m_textWheelsCtrl->Connect(wxEVT_COMMAND_WHEEL_SELECTED, wxWheelsCtrlEventHandler(MobileTestsWxWheelsCtrlPanel::OnTextChange), NULL, this);
    
    
    //
    // Wheels control with date-time data source
    // 
    
    // FIXME fix default frame
    wxWheelsDateTimeDataSource* dateTimeDataSource = new wxWheelsDateTimeDataSource();
    
    m_dateTimeWheelsCtrl = new wxWheelsCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
    sizer->Add(m_dateTimeWheelsCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    m_dateTimeWheelsCtrl->SetDataSource(dateTimeDataSource, true /* control owns the data source */);
    m_dateTimeWheelsCtrl->Connect(wxEVT_COMMAND_WHEEL_SELECTED, wxWheelsCtrlEventHandler(MobileTestsWxWheelsCtrlPanel::OnDateTimeChange), NULL, this);

    
    //
    // Wheels control with count down data source
    // 
    
    wxWheelsDateTimeDataSource* countDownDataSource = new wxWheelsDateTimeDataSource();
    countDownDataSource->SetPickerMode(wxWHEELS_DT_COUNTDOWN);
    
    m_countDownWheelsCtrl = new wxWheelsCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
    sizer->Add(m_countDownWheelsCtrl, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    m_countDownWheelsCtrl->SetDataSource(countDownDataSource, true /* control owns the data source */);
    m_countDownWheelsCtrl->Connect(wxEVT_COMMAND_WHEEL_SELECTED, wxWheelsCtrlEventHandler(MobileTestsWxWheelsCtrlPanel::OnCountDownChange), NULL, this);

    return true;
}

void MobileTestsWxWheelsCtrlPanel::OnTextChange(wxWheelsCtrlEvent& WXUNUSED(event))
{
    wxLogMessage("wxEVT_COMMAND_WHEEL_SELECTED (text wheel control)"); 
}

void MobileTestsWxWheelsCtrlPanel::OnDateTimeChange(wxWheelsCtrlEvent& WXUNUSED(event))
{
    wxLogMessage("wxEVT_COMMAND_WHEEL_SELECTED (date-time wheel control");
}

void MobileTestsWxWheelsCtrlPanel::OnCountDownChange(wxWheelsCtrlEvent& WXUNUSED(event))
{
    wxLogMessage("wxEVT_COMMAND_WHEEL_SELECTED (count down wheel control)");
}
