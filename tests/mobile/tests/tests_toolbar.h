/*
 *  tests_toolbar.h
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-12.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

//
// TESTS:
// 
// styles:
//   - wxTB_NORMAL_BG
//   + wxTB_BLACK_OPAQUE_BG
//   - wxTB_BLACK_TRANSLUCENT_BG
// events:
//   + EVT_TOOL (wxEVT_COMMAND_TOOL_CLICKED)
// + wxMoToolBar (wxWindow *parent, wxWindowID id, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxNO_BORDER|wxTB_HORIZONTAL, const wxString &name=wxToolBarNameStr)
// - bool               Create (wxWindow *parent, wxWindowID id, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxNO_BORDER|wxTB_HORIZONTAL, const wxString &name=wxToolBarNameStr)
// - wxToolBarToolBase* AddTool (int toolid, const wxString &label, const wxBitmap &bitmap, const wxBitmap &bmpDisabled, wxItemKind kind=wxITEM_NORMAL, const wxString &shortHelp=wxEmptyString, const wxString &longHelp=wxEmptyString, wxObject *data=NULL)
// + wxToolBarToolBase* AddTool (int toolid, const wxString &label, const wxBitmap &bitmap, const wxString &shortHelp=wxEmptyString, wxItemKind kind=wxITEM_NORMAL)
// - wxToolBarToolBase* AddCheckTool (int toolid, const wxString &label, const wxBitmap &bitmap, const wxBitmap &bmpDisabled=wxNullBitmap, const wxString &shortHelp=wxEmptyString, const wxString &longHelp=wxEmptyString, wxObject *data=NULL)
// - wxToolBarToolBase* AddRadioTool (int toolid, const wxString &label, const wxBitmap &bitmap, const wxBitmap &bmpDisabled=wxNullBitmap, const wxString &shortHelp=wxEmptyString, const wxString &longHelp=wxEmptyString, wxObject *data=NULL)
// + virtual wxToolBarToolBase* AddSeparator ()
// - virtual wxToolBarToolBase* AddFlexibleSeparator ()
// - virtual wxToolBarToolBase* InsertFlexibleSeparator (size_t pos)
// - virtual wxToolBarToolBase* AddFixedSeparator (int width)
// - virtual wxToolBarToolBase* InsertFixedSeparator (size_t pos, int width)
// + virtual bool               Realize ()

#ifndef testios_tests_toolbar_h
#define testios_tests_toolbar_h

#include "testpanel.h"
#include "wx/toolbar.h"

class MobileTestsWxToolBarPanel : public MobileTestsWxPanel {
    
public:
    bool CreateWithControls(wxWindow* parent,
                            wxWindowID id = wxID_ANY,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = wxTAB_TRAVERSAL,
                            const wxString& name = _("wxToolBar Mobile"));
    
    // Event tests
    void OnToolBarToolClicked(wxCommandEvent& WXUNUSED(event));
        
protected:
    bool CreateControls();
    
    DECLARE_EVENT_TABLE()
    
private:
    wxToolBar* m_toolbar;
};

#endif
