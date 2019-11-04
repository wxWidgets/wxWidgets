///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/choice.cpp
// Purpose:     wxChoice unit test
// Author:      Steven Lamerton
// Created:     2010-06-29
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

    virtual void setUp() wxOVERRIDE;
    virtual void tearDown() wxOVERRIDE;

private:
    virtual wxItemContainer *GetContainer() const wxOVERRIDE { return m_choice; }
    virtual wxWindow *GetContainerWindow() const wxOVERRIDE { return m_choice; }

    CPPUNIT_TEST_SUITE( ChoiceTestCase );
        wxITEM_CONTAINER_TESTS();
        CPPUNIT_TEST( Sort );
        CPPUNIT_TEST( GetBestSize );
    CPPUNIT_TEST_SUITE_END();

    void Sort();
    void GetBestSize();

    wxChoice* m_choice;

    wxDECLARE_NO_COPY_CLASS(ChoiceTestCase);
};

wxREGISTER_UNIT_TEST_WITH_TAGS(ChoiceTestCase,
                               "[ChoiceTestCase][item-container]");

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

void ChoiceTestCase::GetBestSize()
{
    wxArrayString testitems;
    testitems.Add("1");
    testitems.Add("11");
    m_choice->Append(testitems);

    SECTION("Normal best size")
    {
        // nothing to do here
    }

    // Ensure that the hidden control return a valid best size too.
    SECTION("Hidden best size")
    {
        m_choice->Hide();
    }

    wxYield();

    m_choice->InvalidateBestSize();
    const wxSize bestSize = m_choice->GetBestSize();

    CHECK(bestSize.GetWidth() > m_choice->FromDIP(30));
    CHECK(bestSize.GetWidth() < m_choice->FromDIP(120));
    CHECK(bestSize.GetHeight() > m_choice->FromDIP(15));
    CHECK(bestSize.GetHeight() < m_choice->FromDIP(35));
}

#endif //wxUSE_CHOICE
