/*
 *  tests_sheetdlg.h
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-12.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

//
// TESTS:
// 

#ifndef testios_tests_sheetdlg_h
#define testios_tests_sheetdlg_h

#include "testpanel.h"
#include "wx/sheetdlg.h"

class MobileTestsWxSheetDialogPanel : public MobileTestsWxPanel {
    
public:
    bool CreateWithControls(wxWindow* parent,
                            wxWindowID id = wxID_ANY,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = wxTAB_TRAVERSAL,
                            const wxString& name = _("wxActionSheetDialog, wxAlertSheetDialog Mobile"));
    
protected:
    bool CreateControls();
    
    // Showing sheets
    void OnShowActionSheet(wxCommandEvent& WXUNUSED(event));
    void OnShowAlertSheet(wxCommandEvent& WXUNUSED(event));
    
    // Test action sheet events
    void OnActionSheetButtonClicked(wxSheetDialogEvent& WXUNUSED(event));
    void OnAlertSheetButtonClicked(wxSheetDialogEvent& WXUNUSED(event));
    
    DECLARE_EVENT_TABLE()
    
private:
    
    wxButton* m_showActionSheetButton;
    wxButton* m_showAlertSheetButton;
    
    wxActionSheetDialog*    m_actionSheetDlg;
    wxAlertSheetDialog*     m_alertSheetDlg;
};

#endif
