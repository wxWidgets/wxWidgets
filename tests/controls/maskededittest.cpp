///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/maskededittest.cpp
// Purpose:     wxMaskedEdit controls unit test
// Author:      Manuel Martin
// Created:     2012-07-30
// RCS-ID:      $Id:
// Copyright:   (c)
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_MASKED_EDIT

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/maskededit.h"

#include "testableframe.h"
#include "wx/uiaction.h"

class MaskedEditTestCase : public CppUnit::TestCase
{
public:
    MaskedEditTestCase() { }

    void setUp() wxOVERRIDE;
    void tearDown() wxOVERRIDE;

private:
    CPPUNIT_TEST_SUITE( MaskedEditTestCase );
    CPPUNIT_TEST( TestSet );
    WXUISIM_TEST( TestUI );
    CPPUNIT_TEST( TestFunc );
    CPPUNIT_TEST_SUITE_END();

    void TestSet();
#if wxUSE_UIACTIONSIMULATOR
    void TestUI();
#endif
    void TestFunc();

    wxMaskedEditText* m_editTx;

    DECLARE_NO_COPY_CLASS(MaskedEditTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( MaskedEditTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( MaskedEditTestCase, "MaskedEditTestCase" );

void MaskedEditTestCase::setUp()
{
    m_editTx = new wxMaskedEditText(wxTheApp->GetTopWindow(), wxID_ANY);
}

void MaskedEditTestCase::tearDown()
{
    wxTheApp->GetTopWindow()->DestroyChildren();
}

void MaskedEditTestCase::TestSet()
{
    m_editTx->SetMask("=a>A^-###++xx.#{4}");
    // test empty value
    CPPUNIT_ASSERT_EQUAL( "=  -   ++  .    ", m_editTx->GetValue() );

    m_editTx->SetValue("=bc- 12++  .9876");
    // 'c' should be changed to 'C' and '12' should be left re-aligned
    CPPUNIT_ASSERT_EQUAL( "=bC-12 ++  .9876", m_editTx->GetValue() );

    m_editTx->SetFieldFlags(3, wxEditFieldFlags(wxALIGN_RIGHT));
    m_editTx->SetPlainValue("fg56");
    m_editTx->SetFieldValue(3, "987");
    CPPUNIT_ASSERT_EQUAL( "=fG-56 ++  . 987", m_editTx->GetValue() );
}

#if wxUSE_UIACTIONSIMULATOR
void MaskedEditTestCase::TestUI()
{
    wxUIActionSimulator sim;

    m_editTx->SetMask("=a>A^-###++xx.#{4}");
    m_editTx->SetValue( "=fG-56 ++  . 987" );

    m_editTx->SetFocus();
    sim.Char(WXK_END);
    sim.Char(WXK_BACK);
    wxYield();
    CPPUNIT_ASSERT_EQUAL( "=fG-56 ++  .  98", m_editTx->GetValue() );

    sim.Char(WXK_LEFT);
    sim.Char(WXK_LEFT);
    sim.Char('d');
    wxYield();
    CPPUNIT_ASSERT_EQUAL( "=fG-56 ++d .  98", m_editTx->GetValue() );

    m_editTx->SetInsertionPoint(6);
    sim.Char(WXK_RIGHT);
    sim.Char('2');
    wxYield();
    CPPUNIT_ASSERT_EQUAL( "=fG-56 ++2d.  98", m_editTx->GetValue() );
}
#endif


void MaskedEditTestCase::TestFunc()
{
    // Should fail because not all fields have all required chars
    m_editTx->SetMask("=a>A^-###++xx.#{4}");
    m_editTx->SetValue( "=fG-56 ++2d.  98" );

    CPPUNIT_ASSERT( !m_editTx->IsValid() );

    m_editTx->ChangeValue("=aB-123++  .5670");
    CPPUNIT_ASSERT( m_editTx->IsValid() );

    wxRangeParams parms;
    parms.rmin = 70;
    parms.rmax = 100;
    m_editTx->SetFieldFunction(1, &wxMaskedRangeCheck, &parms);
    // Should fail because 123 > 100
    CPPUNIT_ASSERT( !m_editTx->IsValid() );
}

#endif // wxUSE_MASKED_EDIT
