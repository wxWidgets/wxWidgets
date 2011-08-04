//
//  tests_navctrl.cpp
//  testios
//
//  Created by Linas Valiukas on 2011-07-27.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "tests_navctrl.h"


#pragma mark -
#pragma mark wxNavigationCtrl

BEGIN_EVENT_TABLE(MobileTestsWxNavCtrlPanel, wxPanel)
    //EVT_BUTTON(MobileTestsWxButtonClickedID, MobileTestsWxButtonPanel::OnButtonClicked)
END_EVENT_TABLE()

bool MobileTestsWxNavCtrlPanel::CreateWithControls(wxWindow* parent,
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

bool MobileTestsWxNavCtrlPanel::CreateControls()
{
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    this->SetSizer(sizer);

    m_navCtrl = new wxNavigationCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNAVCTRL_BLACK_OPAQUE_BG);
    sizer->Add(m_navCtrl, 1, wxEXPAND, 0);
    
    // Test getters
    wxASSERT_MSG(!m_navCtrl->GetTopController(), "Top controller should not be present at the moment");
    wxASSERT_MSG(!m_navCtrl->GetBackController(), "Back controller should not be present at the moment");
    
    MobileTestswxNavCtrlViewController* firstController = new MobileTestswxNavCtrlViewController("First VC", m_navCtrl, 1);
    m_navCtrl->PushController(firstController);
    
    // Test getters again
    wxASSERT_MSG(m_navCtrl->GetTopController() == firstController, "Top controller should be the first controller");
    
    return true;
}


#pragma mark -
#pragma mark wxNavigationCtrl test view controller

MobileTestswxNavCtrlViewController::MobileTestswxNavCtrlViewController(const wxString& title, wxNavigationCtrl* ctrl, int viewNumber) : wxViewController(title)
{
    m_navCtrl = ctrl;
    m_viewNumber = viewNumber;

    // Panel
    wxPanel* panel = new wxPanel(ctrl, wxID_ANY, wxPoint(-100, -100), wxSize(10, 10));
    panel->SetBackgroundColour(wxColour(wxT("#E3E4FF")));
    SetWindow(panel);

    // "Push new VC" button
    wxString label(wxString::Format(_("Create Test VC %d"), viewNumber+1));
    wxButton* button = new wxButton(panel, wxID_ANY, label, wxPoint(0, 45));
    panel->Connect(button->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MobileTestswxNavCtrlViewController::OnPushVC), NULL, this);
}

void MobileTestswxNavCtrlViewController::OnPushVC(wxCommandEvent& WXUNUSED(event))
{
    // Test current conditions
    wxViewControllerArray controllers = m_navCtrl->GetControllers();
    wxASSERT_MSG(controllers.Count() == m_viewNumber, "Incorrect number of controllers");
    
    
    wxLogMessage("Pushing a new view controller");
    
    wxASSERT_MSG(m_navCtrl->PushController(new MobileTestswxNavCtrlViewController(wxString::Format(_("Test VC %d"), m_viewNumber+1), m_navCtrl, m_viewNumber+1)), "Unable to push a new view controller");
}

void MobileTestswxNavCtrlViewController::OnPopVC(wxCommandEvent& WXUNUSED(event))
{
    wxLogMessage("Pulling a view controller");
    
    m_navCtrl->PopController();
}
