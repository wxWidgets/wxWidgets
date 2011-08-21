//
//  tests_button.h
//  testios
//
//  Created by Linas Valiukas on 2011-07-27.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

//
// TESTS:
// 
// styles:
//   + wxBU_ROUNDED_RECTANGLE
//   + wxBU_DISCLOSURE
//   + wxBU_INFO_LIGHT
//   + wxBU_INFO_DARK
//   + wxBU_CONTACT_ADD
// - SetLabel()
//

#ifndef testios_tests_button_h
#define testios_tests_button_h

#include "testpanel.h"
#include "wx/button.h"

class MobileTestsWxButtonPanel : public MobileTestsWxPanel {
    
public:
    bool CreateWithControls(wxWindow* parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL,
                const wxString& name = _("wxButton Mobile"));
    
protected:
    bool CreateControls();
    
    void OnButtonRoundedRectangleClicked(wxCommandEvent& WXUNUSED(event));
    void OnButtonDisclosureClicked(wxCommandEvent& WXUNUSED(event));
    void OnButtonInfoLightClicked(wxCommandEvent& WXUNUSED(event));
    void OnButtonInfoDarkClicked(wxCommandEvent& WXUNUSED(event));
    void OnButtonContactAddClicked(wxCommandEvent& WXUNUSED(event));
    void OnButtonBitmapClicked(wxCommandEvent& WXUNUSED(event));
    
    DECLARE_EVENT_TABLE()

private:
    wxButton* m_buttonRoundedRectangle;
    wxButton* m_buttonDisclosure;
    wxButton* m_buttonInfoLight;
    wxButton* m_buttonInfoDark;
    wxButton* m_buttonContactAdd;
    wxBitmapButton* m_buttonBitmap;
};

#endif
