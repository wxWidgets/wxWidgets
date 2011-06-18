///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/label.cpp
// Purpose:     wxControl and wxStaticText label tests
// Author:      Francesco Montorsi
// Created:     2010-3-21
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Francesco Montorsi
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
#endif // WX_PRECOMP

#include "wx/control.h"
#include "wx/stattext.h"
#include "wx/checkbox.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class LabelTestCase : public CppUnit::TestCase
{
public:
    LabelTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    CPPUNIT_TEST_SUITE( LabelTestCase );
        CPPUNIT_TEST( GetLabel );
        CPPUNIT_TEST( GetLabelText );
        CPPUNIT_TEST( Statics );
    CPPUNIT_TEST_SUITE_END();

    void GetLabel();
    void GetLabelText();
    void Statics();

    wxStaticText *m_st;

    // we cannot test wxControl directly (it's abstract) so we rather test wxCheckBox
    wxCheckBox *m_cb;

    DECLARE_NO_COPY_CLASS(LabelTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( LabelTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( LabelTestCase, "LabelTestCase" );

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

#define ORIGINAL_LABEL      "original label"

void LabelTestCase::setUp()
{
    m_st = new wxStaticText(wxTheApp->GetTopWindow(), wxID_ANY, ORIGINAL_LABEL);

    m_cb = new wxCheckBox(wxTheApp->GetTopWindow(), wxID_ANY, ORIGINAL_LABEL);

    CPPUNIT_ASSERT_EQUAL( ORIGINAL_LABEL, m_st->GetLabel() );
    CPPUNIT_ASSERT_EQUAL( ORIGINAL_LABEL, m_cb->GetLabel() );
}

void LabelTestCase::tearDown()
{
    wxDELETE(m_st);
    wxDELETE(m_cb);
}

// ----------------------------------------------------------------------------
// the tests themselves
// ----------------------------------------------------------------------------

#define SET_LABEL(str)                              \
        m_st->SetLabel(str);                        \
        m_cb->SetLabel(str);

#define SET_LABEL_TEXT(str)                         \
        m_st->SetLabelText(str);                    \
        m_cb->SetLabelText(str);

void LabelTestCase::GetLabel()
{
    const wxString testLabelArray[] = {
        "label without mnemonics and markup",
        "label with &mnemonic",
        "label with <span foreground='blue'>some</span> <b>markup</b>",
        "label with <span foreground='blue'>some</span> <b>markup</b> and &mnemonic",
    };

    // test calls to SetLabel() and then to GetLabel()

    for ( unsigned int s = 0; s < WXSIZEOF(testLabelArray); s++ )
    {
        SET_LABEL(testLabelArray[s]);

        // GetLabel() should always return the string passed to SetLabel()
        CPPUNIT_ASSERT_EQUAL( testLabelArray[s], m_st->GetLabel() );
        CPPUNIT_ASSERT_EQUAL( testLabelArray[s], m_cb->GetLabel() );
    }


    // test calls to SetLabelText() and then to GetLabel()

    const wxString& testLabel = "label without mnemonics and markup";
    SET_LABEL_TEXT(testLabel);
    CPPUNIT_ASSERT_EQUAL( testLabel, m_st->GetLabel() );
    CPPUNIT_ASSERT_EQUAL( testLabel, m_cb->GetLabel() );

    const wxString& testLabel2 = "label with &mnemonic";
    const wxString& testLabelText2 = "label with &&mnemonic";
    SET_LABEL_TEXT(testLabel2);
    CPPUNIT_ASSERT_EQUAL( testLabelText2, m_st->GetLabel() );
    CPPUNIT_ASSERT_EQUAL( testLabelText2, m_cb->GetLabel() );

    const wxString& testLabel3 = "label with <span foreground='blue'>some</span> <b>markup</b>";
    SET_LABEL_TEXT(testLabel3);
    CPPUNIT_ASSERT_EQUAL( testLabel3, m_st->GetLabel() );
    CPPUNIT_ASSERT_EQUAL( testLabel3, m_cb->GetLabel() );

    const wxString& testLabel4 = "label with <span foreground='blue'>some</span> <b>markup</b> and &mnemonic";
    const wxString& testLabelText4 = "label with <span foreground='blue'>some</span> <b>markup</b> and &&mnemonic";
    SET_LABEL_TEXT(testLabel4);
    CPPUNIT_ASSERT_EQUAL( testLabelText4, m_st->GetLabel() );
    CPPUNIT_ASSERT_EQUAL( testLabelText4, m_cb->GetLabel() );
}

void LabelTestCase::GetLabelText()
{
    // test calls to SetLabel() and then to GetLabelText()

    const wxString& testLabel = "label without mnemonics and markup";
    SET_LABEL(testLabel);
    CPPUNIT_ASSERT_EQUAL( testLabel, m_st->GetLabelText() );
    CPPUNIT_ASSERT_EQUAL( testLabel, m_cb->GetLabelText() );

    const wxString& testLabel2 = "label with &mnemonic";
    const wxString& testLabelText2 = "label with mnemonic";
    SET_LABEL(testLabel2);
    CPPUNIT_ASSERT_EQUAL( testLabelText2, m_st->GetLabelText() );
    CPPUNIT_ASSERT_EQUAL( testLabelText2, m_cb->GetLabelText() );

    const wxString& testLabel3 = "label with <span foreground='blue'>some</span> <b>markup</b>";
    SET_LABEL(testLabel3);
    CPPUNIT_ASSERT_EQUAL( testLabel3, m_st->GetLabelText() );
    CPPUNIT_ASSERT_EQUAL( testLabel3, m_cb->GetLabelText() );

    const wxString& testLabel4 = "label with <span foreground='blue'>some</span> <b>markup</b> and &mnemonic";
    const wxString& testLabelText4 = "label with <span foreground='blue'>some</span> <b>markup</b> and mnemonic";
    SET_LABEL(testLabel4);
    CPPUNIT_ASSERT_EQUAL( testLabelText4, m_st->GetLabelText() );
    CPPUNIT_ASSERT_EQUAL( testLabelText4, m_cb->GetLabelText() );


    const wxString testLabelArray[] = {
        "label without mnemonics and markup",
        "label with &mnemonic",
        "label with <span foreground='blue'>some</span> <b>markup</b>",
        "label with <span foreground='blue'>some</span> <b>markup</b> and &mnemonic",
    };

    // test calls to SetLabelText() and then to GetLabelText()

    for ( unsigned int s = 0; s < WXSIZEOF(testLabelArray); s++ )
    {
        SET_LABEL_TEXT(testLabelArray[s]);

        // GetLabelText() should always return the string passed to SetLabelText()
        CPPUNIT_ASSERT_EQUAL( testLabelArray[s], m_st->GetLabelText() );
        CPPUNIT_ASSERT_EQUAL( testLabelArray[s], m_cb->GetLabelText() );
    }
}

void LabelTestCase::Statics()
{
    CPPUNIT_ASSERT_EQUAL( "mnemonic", wxControl::RemoveMnemonics("&mnemonic") );
    CPPUNIT_ASSERT_EQUAL( "&mnemonic", wxControl::RemoveMnemonics("&&mnemonic") );
    CPPUNIT_ASSERT_EQUAL( "&mnemonic", wxControl::RemoveMnemonics("&&&mnemonic") );
}
