/*
 *  tests_segctrl.h
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-12.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

//
// TESTS:
// 
// events:
//   + wxEVT_COMMAND_TAB_SEL_CHANGING
//   + wxEVT_COMMAND_TAB_SEL_CHANGED
// - wxMoSegmentedCtrl (wxWindow *parent, wxWindowID id, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxTAB_TEXT|wxTAB_BITMAP, const wxString &name=wxT("segCtrl"))
// + int     GetSelection () const 
// - int     GetCurFocus () const 
// - wxImageList *     GetImageList () const 
// + int     GetItemCount () const 
// - bool     GetItemRect (int item, wxRect &rect) const 
// - wxString     GetItemText (int item) const 
// - int     GetItemImage (int item) const 
// + int     SetSelection (int item)
// - int     ChangeSelection (int item)
// - void     SetImageList (wxImageList *imageList)
// - void     AssignImageList (wxImageList *imageList)
// - bool     SetItemText (int item, const wxString &text)
// - bool     SetItemImage (int item, int image)
// - void     SetItemSize (const wxSize &size)
// - void     SetPadding (const wxSize &padding)
// - bool     DeleteAllItems ()
// - bool     DeleteItem (int item)
// - bool     InsertItem (int item, const wxString &text, int imageId=-1)
// - bool     InsertItem (int item, const wxString &text, const wxBitmap &bitmap)
// + bool     AddItem (const wxString &text, int imageId=-1)
// - bool     AddItem (const wxString &text, const wxBitmap &bitmap)
// - bool     SetBadge (int item, const wxString &badge)
// - wxString     GetBadge (int item) const 
// - virtual void     SetBorderColour (const wxColour &colour)
// - virtual wxColour     GetBorderColour () const 
// + virtual bool     SetButtonBackgroundColour (const wxColour &colour)
// + virtual wxColour     GetButtonBackgroundColour () const

#ifndef testios_tests_segctrl_h
#define testios_tests_segctrl_h

#include "testpanel.h"
#include "wx/segctrl.h"

class MobileTestsWxSegmentedCtrlPanel : public MobileTestsWxPanel {
    
public:
    bool CreateWithControls(wxWindow* parent,
                            wxWindowID id = wxID_ANY,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = wxTAB_TRAVERSAL,
                            const wxString& name = _("wxSegmentedCtrl Mobile"));
    
    // Event tests
    void OnChanging(wxCommandEvent& WXUNUSED(event));
    void OnChanged(wxCommandEvent& WXUNUSED(event));
    
protected:
    bool CreateControls();
    
    DECLARE_EVENT_TABLE()
    
private:
    wxSegmentedCtrl* m_segCtrl;
};

#endif
