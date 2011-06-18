///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/checklistlistbox.cpp
// Purpose:     wxCheckListBox unit test
// Author:      Steven Lamerton
// Created:     2010-06-30
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_CHECKLISTBOX

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/checklst.h"
#endif // WX_PRECOMP

#include "itemcontainertest.h"
#include "testableframe.h"

class CheckListBoxTestCase : public ItemContainerTestCase, public CppUnit::TestCase
{
public:
    CheckListBoxTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    virtual wxItemContainer *GetContainer() const { return m_check; }
    virtual wxWindow *GetContainerWindow() const { return m_check; }

    CPPUNIT_TEST_SUITE( CheckListBoxTestCase );
        wxITEM_CONTAINER_TESTS();
        CPPUNIT_TEST( Check );
    CPPUNIT_TEST_SUITE_END();

    void Check();

    wxCheckListBox* m_check;

    DECLARE_NO_COPY_CLASS(CheckListBoxTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( CheckListBoxTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( CheckListBoxTestCase, "CheckListBoxTestCase" );

void CheckListBoxTestCase::setUp()
{
    m_check = new wxCheckListBox(wxTheApp->GetTopWindow(), wxID_ANY);
}

void CheckListBoxTestCase::tearDown()
{
    wxDELETE(m_check);
}

void CheckListBoxTestCase::Check()
{
   wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_check, wxEVT_COMMAND_CHECKLISTBOX_TOGGLED);

    wxArrayString testitems;
    testitems.Add("item 0");
    testitems.Add("item 1");
    testitems.Add("item 2");
    testitems.Add("item 3");

    m_check->Append(testitems);

    m_check->Check(0);
    m_check->Check(1);
    m_check->Check(1, false);

    //We should not get any events when changing this from code
    CPPUNIT_ASSERT_EQUAL(0, frame->GetEventCount());
    CPPUNIT_ASSERT_EQUAL(true, m_check->IsChecked(0));
    CPPUNIT_ASSERT_EQUAL(false, m_check->IsChecked(1));

    //Make sure a double check of an items doesn't deselect it
    m_check->Check(0);

    CPPUNIT_ASSERT_EQUAL(true, m_check->IsChecked(0));
}

#endif // wxUSE_CHECKLISTBOX
