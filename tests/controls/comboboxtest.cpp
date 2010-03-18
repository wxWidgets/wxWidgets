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
    CPPUNIT_TEST_SUITE_END();

    void Size();

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

