/*
 *  tests_tablectrl.h
 *  testios
 *
 *  Created by Linas Valiukas on 2011-08-20.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

//
// TESTS:
// - test this being a scrolled window
// events:
//   + wxEVT_COMMAND_TABLE_ROW_SELECTED
//   - wxEVT_COMMAND_TABLE_ACCESSORY_CLICKED
//   - wxEVT_COMMAND_TABLE_ADD_CLICKED
//   - wxEVT_COMMAND_TABLE_DELETE_CLICKED
//   - wxEVT_COMMAND_TABLE_CONFIRM_DELETE_CLICKED
//   - wxEVT_COMMAND_TABLE_MOVE_DRAGGED


#ifndef testios_tests_tablectrl_h
#define testios_tests_tablectrl_h

#include "testpanel.h"
#include "wx/tablectrl.h"

class MobileTestsWxTableCtrlPanel : public MobileTestsWxPanel {
    
public:
    MobileTestsWxTableCtrlPanel(wxNavigationCtrl* navCtrl) : MobileTestsWxPanel(navCtrl) { }

    bool CreateWithControls(wxWindow* parent,
                            wxWindowID id = wxID_ANY,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = wxTAB_TRAVERSAL,
                            const wxString& name = _("wxTableCtrl Mobile"));
        
    // Plain/grouped tables
    void OnTableRowSelected(wxTableCtrlEvent& event);
    
protected:
    bool CreateControls();
    
    DECLARE_EVENT_TABLE()
    
private:
    wxTableCtrl* m_tableCtrl;
};


#pragma mark Root table data source

class MobileTestsWxTableCtrlRootDataSource: public wxTableDataSource {
    
public:
    /// Returns a table cell for the given location.
    virtual wxTableCell* GetCell(wxTableCtrl* ctrl, const wxTablePath& path);
    
    /// Returns the number of sections in the table.
    virtual int GetSectionCount(wxTableCtrl* ctrl) const;
    
    /// Returns the number of rows in a section.
    virtual int GetRowCount(wxTableCtrl* ctrl, int section) const;
            
protected:
    DECLARE_EVENT_TABLE()
};


#pragma mark Main (tests) table data source

class MobileTestsWxTableCtrlTestsDataSource: public wxTableDataSource {
    
public:
    /// Returns a table cell for the given location.
    virtual wxTableCell* GetCell(wxTableCtrl* ctrl, const wxTablePath& path);
    
    /// Returns the number of sections in the table.
    virtual int GetSectionCount(wxTableCtrl* ctrl) const;
    
    /// Returns the number of rows in a section.
    virtual int GetRowCount(wxTableCtrl* ctrl, int section) const;
    
protected:
    DECLARE_EVENT_TABLE()
};


#endif
