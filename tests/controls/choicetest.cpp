///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/choice.cpp
// Purpose:     wxChoice unit test
// Author:      Steven Lamerton
// Created:     2010-06-29
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_CHOICE

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/choice.h"
#endif // WX_PRECOMP

#include "itemcontainertest.h"

class ChoiceTestCase : public ItemContainerTestCase, public CppUnit::TestCase
{
public:
    ChoiceTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    virtual wxItemContainer *GetContainer() const { return m_choice; }
    virtual wxWindow *GetContainerWindow() const { return m_choice; }

    CPPUNIT_TEST_SUITE( ChoiceTestCase );
        wxITEM_CONTAINER_TESTS();
        CPPUNIT_TEST( Sort );
    CPPUNIT_TEST_SUITE_END();

    void Sort();

    wxChoice* m_choice;

    DECLARE_NO_COPY_CLASS(ChoiceTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ChoiceTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ChoiceTestCase, "ChoiceTestCase" );

void ChoiceTestCase::setUp()
{
    m_choice = new wxChoice(wxTheApp->GetTopWindow(), wxID_ANY);
}

void ChoiceTestCase::tearDown()
{
    wxDELETE(m_choice);
}

void ChoiceTestCase::Sort()
{
#if !defined(__WXOSX__)
    wxDELETE(m_choice);
    m_choice = new wxChoice(wxTheApp->GetTopWindow(), wxID_ANY,
                            wxDefaultPosition, wxDefaultSize, 0, 0,
                            wxCB_SORT);

    wxArrayString testitems;
    testitems.Add("aaa");
    testitems.Add("Aaa");
    testitems.Add("aba");
    testitems.Add("aaab");
    testitems.Add("aab");
    testitems.Add("AAA");

    m_choice->Append(testitems);

    CPPUNIT_ASSERT_EQUAL("AAA", m_choice->GetString(0));
    CPPUNIT_ASSERT_EQUAL("Aaa", m_choice->GetString(1));
    CPPUNIT_ASSERT_EQUAL("aaa", m_choice->GetString(2));
    CPPUNIT_ASSERT_EQUAL("aaab", m_choice->GetString(3));
    CPPUNIT_ASSERT_EQUAL("aab", m_choice->GetString(4));
    CPPUNIT_ASSERT_EQUAL("aba", m_choice->GetString(5));

    m_choice->Append("a");

    CPPUNIT_ASSERT_EQUAL("a", m_choice->GetString(0));
#endif
}

#endif //wxUSE_CHOICE
