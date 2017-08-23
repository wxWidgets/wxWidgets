///////////////////////////////////////////////////////////////////////////////
// Name:        tests/persistence.cpp
// Purpose:     wxPersistentObjects unit test
// Author:      wxWidgets Team
// Created:     2017-08-23
// Copyright:   (c) 2017 wxWidgets Team
///////////////////////////////////////////////////////////////////////////////

// Note: The wxDataViewCtrl test currently uses the derivative class
// wxDataViewListCtrl for convenience.

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"
 #include <cppunit/extensions/HelperMacros.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/config.h"
    #include "wx/dataview.h"
    #include "wx/frame.h"
    #include "wx/persist/dataview.h"
    #include "wx/persist/toplevel.h"
#endif // WX_PRECOMP

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#define APP_NAME            "cpptest"
#define PO_PREFIX           "/Persistent_Options"
#define DVLC_PREFIX         PO_PREFIX "/DataView/dvlc"
#define DVLC_COL            "Column #"
#define DVLC_COL_PREFIX     DVLC_PREFIX "/Columns/" DVLC_COL
#define DVLC_SORT_PREFIX    DVLC_PREFIX "/Sorting"
#define DVLC_FRAME_PREFIX   PO_PREFIX "/Window/frame"

// --------------------------------------------------------------------------
// test class
// --------------------------------------------------------------------------

class PersistenceTestCase : public CppUnit::TestCase
{
public:
    PersistenceTestCase() 
    {
        suite_setUp();
    }
    
    virtual void setUp();
    virtual void tearDown();
    
    void suite_setUp()
    {
        wxTheApp->SetAppName("PersistTest");
        wxConfig::Get()->DeleteGroup("/Persistent_Options");
        wxConfig::Get()->Flush();
    }

private:
    CPPUNIT_TEST_SUITE( PersistenceTestCase );
        CPPUNIT_TEST( FrameSaveTest );
        CPPUNIT_TEST( FrameRestoreTest );
        CPPUNIT_TEST( DVLCSaveTest );
        CPPUNIT_TEST( DVLCRestoreTest );
    CPPUNIT_TEST_SUITE_END();

    void FrameSaveTest();
    void FrameRestoreTest();
    void DVLCSaveTest();
    void DVLCRestoreTest();

    wxDataViewListCtrl* m_list;
    wxFrame* m_frame;

    wxDECLARE_NO_COPY_CLASS(PersistenceTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( PersistenceTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( PersistenceTestCase, "PersistenceTestCase" );

void PersistenceTestCase::setUp()
{
    // Create the objects to persist.
    m_frame =  new wxFrame(wxTheApp->GetTopWindow(), wxID_ANY,
                                    "Persistence Test",
                                    wxDefaultPosition, wxSize(400,400));
    
    m_list = new wxDataViewListCtrl(m_frame, wxID_ANY, 
                                    wxDefaultPosition, 
                                    wxDLG_UNIT(m_frame, wxSize(-1,-1)), 
                                    wxDV_ROW_LINES|wxDV_SINGLE);
                                    
    // Define names so the objects can be found using wxConfig.
    m_frame->SetName("frame");
    m_list->SetName("dvlc");

    // Add some columns to the DVLC.
    m_list->AppendTextColumn(DVLC_COL "1", 
                                wxDATAVIEW_CELL_INERT, -1, wxALIGN_LEFT, 
                                wxDATAVIEW_COL_RESIZABLE | 
                                wxDATAVIEW_COL_REORDERABLE |
                                wxDATAVIEW_COL_SORTABLE);
    m_list->AppendTextColumn(DVLC_COL "2", 
                                wxDATAVIEW_CELL_INERT, -1, wxALIGN_LEFT, 
                                wxDATAVIEW_COL_RESIZABLE | 
                                wxDATAVIEW_COL_REORDERABLE |
                                wxDATAVIEW_COL_SORTABLE);
    
    // Populate with DVLC data.
    wxVector<wxVariant> data;
    
    data.push_back("AAAA");
    data.push_back("BBBB");
    m_list->AppendItem(data);
    
    data.clear();
    data.push_back("CCCC");
    data.push_back("DDDD");
    m_list->AppendItem(data);   
    
    data.clear();
    data.push_back("EEEE");
    data.push_back("FFFF");
    m_list->AppendItem(data);
}

void PersistenceTestCase::tearDown()
{
    
}

void PersistenceTestCase::FrameSaveTest()
{
    // Adjust the initial settings.
    m_frame->SetPosition(wxPoint(100, 150));
    m_frame->SetSize(wxSize(450, 350));

    // Get default managers.
    wxPersistenceManager& PM = wxPersistenceManager::Get();
    wxConfigBase* conf = wxConfig::Get();

    PM.Register(m_frame);
    
    // Destroy the frame immediately.
    m_frame->wxWindowBase::Destroy();
   
    // Test that the relevant keys have been stored correctly.
    int val;
   
    CPPUNIT_ASSERT(conf->Read(DVLC_FRAME_PREFIX "/x", &val));
    CPPUNIT_ASSERT_EQUAL(100, val);

    CPPUNIT_ASSERT(conf->Read(DVLC_FRAME_PREFIX "/y", &val));
    CPPUNIT_ASSERT_EQUAL(150, val);
    
    CPPUNIT_ASSERT(conf->Read(DVLC_FRAME_PREFIX "/h", &val));
    CPPUNIT_ASSERT_EQUAL(350, val);

    CPPUNIT_ASSERT(conf->Read(DVLC_FRAME_PREFIX "/w", &val));
    CPPUNIT_ASSERT_EQUAL(450, val);

    CPPUNIT_ASSERT(conf->Read(DVLC_FRAME_PREFIX "/Iconized", &val));
    CPPUNIT_ASSERT_EQUAL(0, val);

    CPPUNIT_ASSERT(conf->Read(DVLC_FRAME_PREFIX "/Maximized", &val));
    CPPUNIT_ASSERT_EQUAL(0, val);
}

void PersistenceTestCase::FrameRestoreTest()
{
    // Get default manager.
    wxPersistenceManager& PM = wxPersistenceManager::Get();
    
    // Test that the objects are registered and can be restored.
    CPPUNIT_ASSERT(PM.RegisterAndRestore(m_frame));

    CPPUNIT_ASSERT_EQUAL(100, m_frame->GetPosition().x);
    CPPUNIT_ASSERT_EQUAL(150, m_frame->GetPosition().y);
    CPPUNIT_ASSERT_EQUAL(350, m_frame->GetSize().GetHeight());
    CPPUNIT_ASSERT_EQUAL(450, m_frame->GetSize().GetWidth());
    CPPUNIT_ASSERT(!m_frame->IsMaximized());
    CPPUNIT_ASSERT(!m_frame->IsIconized());
}

void PersistenceTestCase::DVLCSaveTest()
{
    // Adjust the initial settings.
    m_list->GetColumn(0)->SetWidth(150);
    m_list->GetColumn(1)->SetWidth(250);
    m_list->GetColumn(1)->SetSortOrder(false);

    // Get default managers.
    wxPersistenceManager& PM = wxPersistenceManager::Get();
    wxConfigBase* conf = wxConfig::Get();

    PM.Register(m_list);
    
    // Destroy the frame containing the DVLC immediately.
    m_frame->wxWindowBase::Destroy();
   
    // Test that the relevant keys have been stored correctly.
    int val;
    wxString text;
   
    CPPUNIT_ASSERT(conf->Read(DVLC_COL_PREFIX "1/Width", &val));
    CPPUNIT_ASSERT_EQUAL(150, val);

    CPPUNIT_ASSERT(conf->Read(DVLC_COL_PREFIX "2/Width", &val));
    CPPUNIT_ASSERT_EQUAL(250, val);

    CPPUNIT_ASSERT(conf->Read(DVLC_SORT_PREFIX "/Column", &text));
    CPPUNIT_ASSERT_EQUAL("Column #2", text);

    CPPUNIT_ASSERT(conf->Read(DVLC_SORT_PREFIX "/Asc", &val));
    CPPUNIT_ASSERT_EQUAL(0, val);
}

void PersistenceTestCase::DVLCRestoreTest()
{
    // Get default manager.
    wxPersistenceManager& PM = wxPersistenceManager::Get();
    
    // Test that the objects are registered and can be restored.
    CPPUNIT_ASSERT(PM.RegisterAndRestore(m_list));
    
    // Test that the correct values were restored.
    CPPUNIT_ASSERT_EQUAL(150, m_list->GetColumn(0)->GetWidth());
    CPPUNIT_ASSERT_EQUAL(250, m_list->GetColumn(1)->GetWidth());
    CPPUNIT_ASSERT(m_list->GetColumn(1)->IsSortKey());
    CPPUNIT_ASSERT(!m_list->GetColumn(1)->IsSortOrderAscending());
}
