/*
 *  tests_textctrl.h
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-10.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */


//
// TESTS:
// 
// styles:
//   + wxTE_PROCESS_ENTER
//   + wxTE_MULTILINE
//   + wxTE_PASSWORD
//   + wxTE_READONLY
//   + wxTE_CAPITALIZE
// events:
//   + wxEVT_TEXT
//   + wxEVT_TEXT_ENTER
//   + wxEVT_TEXT_MAXLEN
// + void SetMaxLength(unsigned long len)

#ifndef testios_tests_textctrl_h
#define testios_tests_textctrl_h

#include "testpanel.h"
#include "wx/textctrl.h"

class MobileTestsWxTextCtrlPanel : public MobileTestsWxPanel {
    
public:
    bool CreateWithControls(wxWindow* parent,
                            wxWindowID id = wxID_ANY,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = wxTAB_TRAVERSAL,
                            const wxString& name = _("wxTextCtrl Mobile"));
    
    // Event tests
    void OnTextCtrlText(wxCommandEvent& WXUNUSED(event));
    void OnTextCtrlTextEnter(wxCommandEvent& WXUNUSED(event));
    void OnTextCtrlTextMaxLen(wxCommandEvent& WXUNUSED(event));
    
protected:
    bool CreateControls();
    
    DECLARE_EVENT_TABLE()
    
private:
    wxTextCtrl* m_textCtrlNormal;
    wxTextCtrl* m_textCtrlPassword;
    wxTextCtrl* m_textCtrlReadOnly;
    wxTextCtrl* m_textCtrlLimitedTo10Chars;
    wxTextCtrl* m_textCtrlMultiLine;
    wxTextCtrl* m_textCtrlProcessEnter;
    wxTextCtrl* m_textCtrlCapitalize;
};

#endif
