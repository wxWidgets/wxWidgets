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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/combobox.h"
#endif // WX_PRECOMP

#include "textentrytest.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class ComboBoxTestCase : public TextEntryTestCase
{
public:
    ComboBoxTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    virtual wxTextEntry *GetTestEntry() const { return m_combo; }
    virtual wxWindow *GetTestWindow() const { return m_combo; }

    virtual void CheckStringSelection(const char * WXUNUSED(sel))
    {
        // do nothing here, as explained in TextEntryTestCase comment, our
        // GetStringSelection() is the wxChoice, not wxTextEntry, one and there
        // is no way to return the selection contents directly
    }

    CPPUNIT_TEST_SUITE( ComboBoxTestCase );
        wxTEXT_ENTRY_TESTS();

        CPPUNIT_TEST( Size );
        CPPUNIT_TEST( Append );
        CPPUNIT_TEST( Insert );
        CPPUNIT_TEST( Count );
        CPPUNIT_TEST( ItemSelect );
        CPPUNIT_TEST( FindString );
    CPPUNIT_TEST_SUITE_END();

    void Size();
    void Append();
    void Insert();
    void Count();
    void ItemSelect();
    void FindString();

    wxComboBox *m_combo;

    DECLARE_NO_COPY_CLASS(ComboBoxTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ComboBoxTestCase );

// also include in it's own registry so that these tests can be run alone
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

//---------------------------------------------------------------------
//Please also update tests in ItemContainerTestCase when updating below
//---------------------------------------------------------------------

void ComboBoxTestCase::Append()
{
    m_combo->Append("item 0");

    CPPUNIT_ASSERT_EQUAL("item 0", m_combo->GetString(0));

    wxArrayString testitems;
    testitems.Add("item 1");
    testitems.Add("item 2");

    m_combo->Append(testitems);

    CPPUNIT_ASSERT_EQUAL("item 1", m_combo->GetString(1));
    CPPUNIT_ASSERT_EQUAL("item 2", m_combo->GetString(2));

    wxString arritems[] = { "item 3", "item 4" };

    m_combo->Append(2, arritems);

    CPPUNIT_ASSERT_EQUAL("item 3", m_combo->GetString(3));
    CPPUNIT_ASSERT_EQUAL("item 4", m_combo->GetString(4));

    m_combo->Clear();
}

void ComboBoxTestCase::Insert()
{
    m_combo->Insert("item 0", 0);

    CPPUNIT_ASSERT_EQUAL("item 0", m_combo->GetString(0));

    wxArrayString testitems;
    testitems.Add("item 1");
    testitems.Add("item 2");

    m_combo->Insert(testitems, 0);

    CPPUNIT_ASSERT_EQUAL("item 1", m_combo->GetString(0));
    CPPUNIT_ASSERT_EQUAL("item 2", m_combo->GetString(1));

    wxString arritems[] = { "item 3", "item 4" };

    m_combo->Insert(2, arritems, 1);

    CPPUNIT_ASSERT_EQUAL("item 3", m_combo->GetString(1));
    CPPUNIT_ASSERT_EQUAL("item 4", m_combo->GetString(2));

    m_combo->Clear();
}

void ComboBoxTestCase::Count()
{
    wxArrayString testitems;
    testitems.Add("item 0");
    testitems.Add("item 1");
    testitems.Add("item 2");
    testitems.Add("item 3");

    m_combo->Append(testitems);

    CPPUNIT_ASSERT_EQUAL(4, m_combo->GetCount());

    m_combo->Delete(0);

    CPPUNIT_ASSERT_EQUAL(3, m_combo->GetCount());

    m_combo->Delete(0);
    m_combo->Delete(0);

    CPPUNIT_ASSERT_EQUAL(1, m_combo->GetCount());

    m_combo->Insert(testitems, 1);

    CPPUNIT_ASSERT_EQUAL(5, m_combo->GetCount());

    m_combo->Clear();
}

void ComboBoxTestCase::ItemSelect()
{
    wxArrayString testitems;
    testitems.Add("item 0");
    testitems.Add("item 1");
    testitems.Add("item 2");
    testitems.Add("item 3");

    m_combo->Append(testitems);

    CPPUNIT_ASSERT_EQUAL(wxNOT_FOUND, m_combo->GetSelection());
    CPPUNIT_ASSERT_EQUAL("", m_combo->GetStringSelection());

    m_combo->SetSelection(1);

    CPPUNIT_ASSERT_EQUAL(1, m_combo->GetSelection());
    CPPUNIT_ASSERT_EQUAL("item 1", m_combo->GetStringSelection());

    m_combo->SetStringSelection("item 2");

    CPPUNIT_ASSERT_EQUAL(2, m_combo->GetSelection());
    CPPUNIT_ASSERT_EQUAL("item 2", m_combo->GetStringSelection());

    m_combo->Clear();
}

void ComboBoxTestCase::FindString()
{
    wxArrayString testitems;
    testitems.Add("item 0");
    testitems.Add("item 1");
    testitems.Add("item 2");
    testitems.Add("item 3");

    m_combo->Append(testitems);

    CPPUNIT_ASSERT_EQUAL(1, m_combo->FindString("item 1"));
    CPPUNIT_ASSERT_EQUAL(1, m_combo->FindString("ITEM 1"));
    CPPUNIT_ASSERT_EQUAL(wxNOT_FOUND, m_combo->FindString("ITEM 1", true));

    m_combo->Clear();
}

