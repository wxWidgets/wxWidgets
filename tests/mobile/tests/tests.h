/////////////////////////////////////////////////////////////////////////////
// Name:        tests.h
// Purpose:     Control list initializer (part of an attempt to test implementations of wxiOS controls)
// Author:      Linas Valiukas
// Modified by: 
// Created:     2011-07-27
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef testios_tests_h
#define testios_tests_h

#include "wx/app.h"
#include "wx/frame.h"
#include "wx/viewcontroller.h"
#include "wx/navctrl.h"
#include "wx/tablectrl.h"
#include "wx/image.h"
#include "testpanel.h"


#pragma mark Tests table controller

// A navigation controller for Table tab, starting off with the root data source
class MobileTestsTableController: public wxViewController
{
public:
    MobileTestsTableController(wxNavigationCtrl* ctrl);
    
protected:
    wxNavigationCtrl* m_navCtrl;
};


#pragma mark Tests table data source

WX_DEFINE_ARRAY_PTR(MobileTestsWxPanel *, wxArrayMobileTestsWxPanel);

class MobileTestsDataSource: public wxTableDataSource
{
public:
    MobileTestsDataSource(wxNavigationCtrl* ctrl);
    
    /// Returns a table cell for the given location.
    virtual wxTableCell* GetCell(wxTableCtrl* ctrl, const wxTablePath& path);
    
    /// Returns the number of sections in the table.
    virtual int GetSectionCount(wxTableCtrl* ctrl) const;
    
    /// Returns the number of rows in a section.
    virtual int GetRowCount(wxTableCtrl* ctrl, int section) const;
    
    /// Returns the number of rows in a section.
    virtual wxArrayString GetSectionTitles(wxTableCtrl* ctrl) const;
    
    /// Returns an array of strings, each one corresponding to an index title to be shown
    /// on the right hand side of a plain list.
    virtual wxArrayString GetIndexTitles(wxTableCtrl* WXUNUSED(ctrl)) const;
    
    bool OnSelectRow(wxTableCtrl* ctrl,
                     wxTablePath path);
    
    DECLARE_EVENT_TABLE()
protected:
    wxNavigationCtrl* m_navCtrl;
    
private:
    wxArrayString              m_testNames;
    wxArrayString              m_testDescriptions;
    wxArrayMobileTestsWxPanel  m_testPanels;
};



#pragma mark wxFrame

class MobileTestsFrame: public wxFrame
{
public:
    /// Constructors
    MobileTestsFrame();
    MobileTestsFrame(wxWindow* parent,
              wxWindowID id = 10000,
              const wxString& caption = _("Mobile tests"),
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxSize(400, 300),
              long style = 0 );
    
    bool Create(wxWindow* parent,
                wxWindowID id = 10000,
                const wxString& caption = _("Mobile tests"),
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxSize(400, 300),
                long style = 0 );
    
    /// Destructor
    ~MobileTestsFrame();
    
    /// Initialises member variables
    void Init();
        
private:
    DECLARE_EVENT_TABLE()
};



#pragma mark wxApp

class MobileTestsApp: public wxApp
{    
    DECLARE_CLASS( MobileTestsApp )
    DECLARE_EVENT_TABLE()
    
public:
    /// Constructor
    MobileTestsApp();
    
    void Init();
    
    /// Initialises the application
    virtual bool OnInit();
    
    /// Called on exit
    virtual int OnExit();    
};

#endif
