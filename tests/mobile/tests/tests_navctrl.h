//
//  tests_navctrl.h
//  testios
//
//  Created by Linas Valiukas on 2011-07-27.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

//
// TESTS:
// 
// styles:
//   - wxNAVCTRL_NORMAL_BG
//   - wxNAVCTRL_BLACK_OPAQUE_BG
//   - wxNAVCTRL_BLACK_TRANSLUCENT_BG
// events:
//   + wxEVT_COMMAND_NAVCTRL_POPPED
//   + wxEVT_COMMAND_NAVCTRL_PUSHED
//   + wxEVT_COMMAND_NAVCTRL_POPPING
//   + wxEVT_COMMAND_NAVCTRL_PUSHING
// - wxNavigationCtrl ()
// + wxNavigationCtrl (wxWindow *parent, wxWindowID id=wxID_ANY, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=0, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxNavigationCtrlNameStr)
// + bool   Create (wxWindow *parent, wxWindowID id=wxID_ANY, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=0, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxNavigationCtrlNameStr)
// + virtual bool                   PushController (wxMoViewController *controller)
// + virtual wxMoViewController*    PopController ()
// + virtual wxMoViewController*    GetTopController () const 
// + virtual wxMoViewController*    GetBackController () const 
// + const wxMoViewControllerArray& GetControllers () const 
// + void                           SetControllers (const wxMoViewControllerArray &controllers)
// + void                           ClearControllers ()
// - wxMoNavigationBar *            GetNavigationBar () const 
// - void                           Freeze ()
// - void                           Thaw ()
// - bool                           IsFrozen () const 

#ifndef testios_tests_navctrl_h
#define testios_tests_navctrl_h

#include "testpanel.h"
#include "wx/navctrl.h"
#include "wx/viewcontroller.h"


class MobileTestsWxNavCtrlPanel : public MobileTestsWxPanel {
    
public:
    bool CreateWithControls(wxWindow* parent,
                            wxWindowID id = wxID_ANY,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = wxTAB_TRAVERSAL,
                            const wxString& name = _("wxButton Mobile"));
    
    // Event tests
    void OnNavCtrlPopped(wxCommandEvent& WXUNUSED(event));
    void OnNavCtrlPushed(wxCommandEvent& WXUNUSED(event));
    void OnNavCtrlPopping(wxCommandEvent& WXUNUSED(event));
    void OnNavCtrlPushing(wxCommandEvent& WXUNUSED(event));
    
protected:
    bool CreateControls();
    
    DECLARE_EVENT_TABLE()
    
private:
    
    wxNavigationCtrl *m_navCtrl;
};


// Sample view controller
class MobileTestswxNavCtrlViewController : public wxViewController {
    
public:
    MobileTestswxNavCtrlViewController(const wxString& title,
                                       wxNavigationCtrl* ctrl,
                                       int viewNumber);
    
    // Button presses
    void OnPushVC(wxCommandEvent& WXUNUSED(event));
    void OnPopVC(wxCommandEvent& WXUNUSED(event));
    void OnClearVCs(wxCommandEvent& WXUNUSED(event));
    
private:
    wxNavigationCtrl*   m_navCtrl;
    int                 m_viewNumber;
};

#endif
