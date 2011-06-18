///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/comboboxtest.cpp
// Purpose:     wxComboBox unit test
// Author:      Vadim Zeitlin
// Created:     2007-09-25
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_COMBOBOX

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/combobox.h"
#endif // WX_PRECOMP

#include "textentrytest.h"
#include "itemcontainertest.h"
#include "testableframe.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class ComboBoxTestCase : public TextEntryTestCase, public ItemContainerTestCase,
                         public CppUnit::TestCase
{
public:
    ComboBoxTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    virtual wxTextEntry *GetTestEntry() const { return m_combo; }
    virtual wxWindow *GetTestWindow() const { return m_combo; }

    virtual wxItemContainer *GetContainer() const { return m_combo; }
    virtual wxWindow *GetContainerWindow() const { return m_combo; }

    virtual void CheckStringSelection(const char * WXUNUSED(sel))
    {
        // do nothing here, as explained in TextEntryTestCase comment, our
        // GetStringSelection() is the wxChoice, not wxTextEntry, one and there
        // is no way to return the selection contents directly
    }

    CPPUNIT_TEST_SUITE( ComboBoxTestCase );
        wxTEXT_ENTRY_TESTS();
        wxITEM_CONTAINER_TESTS();
        CPPUNIT_TEST( Size );
        CPPUNIT_TEST( PopDismiss );
        CPPUNIT_TEST( Sort );
        CPPUNIT_TEST( ReadOnly );
    CPPUNIT_TEST_SUITE_END();

    void Size();
    void PopDismiss();
    void Sort();
    void ReadOnly();

    wxComboBox *m_combo;

    DECLARE_NO_COPY_CLASS(ComboBoxTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ComboBoxTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ComboBoxTestCase, "ComboBoxTestCase" );

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

void ComboBoxTestCase::setUp()
{
    m_combo = new wxComboBox(wxTheApp->GetTopWindow(), wxID_ANY);
}

void ComboBoxTestCase::tearDown()
{
    delete m_combo;
    m_combo = NULL;
}

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

void ComboBoxTestCase::Size()
{
    // under MSW changing combobox size is a non-trivial operation because of
    // confusion between the size of the control with and without dropdown, so
    // check that it does work as expected

    const int heightOrig = m_combo->GetSize().y;

    // check that the height doesn't change if we don't touch it
    m_combo->SetSize(100, -1);
    CPPUNIT_ASSERT_EQUAL( heightOrig, m_combo->GetSize().y );

    // check that setting both big and small (but not too small, there is a
    // limit on how small the control can become under MSW) heights works
    m_combo->SetSize(-1, 50);
    CPPUNIT_ASSERT_EQUAL( 50, m_combo->GetSize().y );

    m_combo->SetSize(-1, 10);
    CPPUNIT_ASSERT_EQUAL( 10, m_combo->GetSize().y );

    // and also that restoring it works (this used to be broken before 2.9.1)
    m_combo->SetSize(-1, heightOrig);
    CPPUNIT_ASSERT_EQUAL( heightOrig, m_combo->GetSize().y );
}

void ComboBoxTestCase::PopDismiss()
{
#if defined(__WXMSW__) || defined(__WXGTK210__)
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_combo, wxEVT_COMMAND_COMBOBOX_DROPDOWN);
    EventCounter count1(m_combo, wxEVT_COMMAND_COMBOBOX_CLOSEUP);

    m_combo->Popup();
    m_combo->Dismiss();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_COMBOBOX_DROPDOWN));
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_COMBOBOX_CLOSEUP));
#endif
}

void ComboBoxTestCase::Sort()
{
#if !defined(__WXOSX__)
    m_combo = new wxComboBox(wxTheApp->GetTopWindow(), wxID_ANY, "",
                             wxDefaultPosition, wxDefaultSize, 0, NULL,
                             wxCB_SORT);

    m_combo->Append("aaa");
    m_combo->Append("Aaa");
    m_combo->Append("aba");
    m_combo->Append("aaab");
    m_combo->Append("aab");
    m_combo->Append("AAA");

    CPPUNIT_ASSERT_EQUAL("AAA", m_combo->GetString(0));
    CPPUNIT_ASSERT_EQUAL("Aaa", m_combo->GetString(1));
    CPPUNIT_ASSERT_EQUAL("aaa", m_combo->GetString(2));
    CPPUNIT_ASSERT_EQUAL("aaab", m_combo->GetString(3));
    CPPUNIT_ASSERT_EQUAL("aab", m_combo->GetString(4));
    CPPUNIT_ASSERT_EQUAL("aba", m_combo->GetString(5));

    m_combo->Append("a");

    CPPUNIT_ASSERT_EQUAL("a", m_combo->GetString(0));
#endif
}

void ComboBoxTestCase::ReadOnly()
{
#ifndef __WXOSX__
    wxArrayString testitems;
    testitems.Add("item 1");
    testitems.Add("item 2");

    m_combo = new wxComboBox(wxTheApp->GetTopWindow(), wxID_ANY, "",
                             wxDefaultPosition, wxDefaultSize, testitems,
                             wxCB_READONLY);

    m_combo->SetValue("item 1");

    CPPUNIT_ASSERT_EQUAL("item 1", m_combo->GetValue());

    m_combo->SetValue("not an item");

    CPPUNIT_ASSERT_EQUAL("item 1", m_combo->GetValue());

    // Since this uses FindString it is case insensitive
    m_combo->SetValue("ITEM 2");

    CPPUNIT_ASSERT_EQUAL("item 2", m_combo->GetValue());
#endif
}

#endif //wxUSE_COMBOBOX
