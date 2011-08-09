//
//  tests_navctrl.h
//  testios
//
//  Created by Linas Valiukas on 2011-07-27.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

//
// Tests:
// - wxNavigationCtrl ()
// + wxNavigationCtrl (wxWindow *parent, wxWindowID id=wxID_ANY, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=0, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxNavigationCtrlNameStr)
// + bool   Create (wxWindow *parent, wxWindowID id=wxID_ANY, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=0, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxNavigationCtrlNameStr)
// + virtual bool                   PushController (wxMoViewController *controller)
// + virtual wxMoViewController*    PopController ()
// + virtual wxMoViewController*    GetTopController () const 
// + virtual wxMoViewController*    GetBackController () const 
// + const wxMoViewControllerArray& GetControllers () const 
// - void                           SetControllers (const wxMoViewControllerArray &controllers)
// - void                           ClearControllers ()
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
    
protected:
    bool CreateControls();
    
    DECLARE_EVENT_TABLE()
    
private:
    
    wxNavigationCtrl *m_navCtrl;
};


// Sample view controller
class MobileTestswxNavCtrlViewController : public wxViewController {
    
public:
    MobileTestswxNavCtrlViewController(const wxString& title, wxNavigationCtrl* ctrl, int viewNumber);
    
    void OnPushVC(wxCommandEvent& WXUNUSED(event));
    void OnPopVC(wxCommandEvent& WXUNUSED(event));
    
private:
    wxNavigationCtrl*   m_navCtrl;
    int                 m_viewNumber;
};

#endif
