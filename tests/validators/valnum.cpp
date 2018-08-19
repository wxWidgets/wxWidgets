///////////////////////////////////////////////////////////////////////////////
// Name:        tests/validators/valnum.cpp
// Purpose:     Unit tests for numeric validators.
// Author:      Vadim Zeitlin
// Created:     2011-01-18
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/intl.h"
    #include "wx/textctrl.h"
    #include "wx/validate.h"
#endif // WX_PRECOMP

#include "wx/valnum.h"

#include "asserthelper.h"
#include "testableframe.h"
#include "wx/uiaction.h"

class NumValidatorTestCase : public CppUnit::TestCase
{
public:
    NumValidatorTestCase() { }

    void setUp() wxOVERRIDE;
    void tearDown() wxOVERRIDE;

private:
    CPPUNIT_TEST_SUITE( NumValidatorTestCase );
        CPPUNIT_TEST( TransferInt );
        CPPUNIT_TEST( TransferUnsigned );
        CPPUNIT_TEST( TransferFloat );
        CPPUNIT_TEST( ZeroAsBlank );
        CPPUNIT_TEST( NoTrailingZeroes );
        WXUISIM_TEST( Interactive );
    CPPUNIT_TEST_SUITE_END();

    void TransferInt();
    void TransferUnsigned();
    void TransferFloat();
    void ZeroAsBlank();
    void NoTrailingZeroes();
#if wxUSE_UIACTIONSIMULATOR
    void Interactive();
#endif // wxUSE_UIACTIONSIMULATOR

    wxTextCtrl *m_text;

    wxDECLARE_NO_COPY_CLASS(NumValidatorTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( NumValidatorTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( NumValidatorTestCase, "NumValidatorTestCase" );

void NumValidatorTestCase::setUp()
{
    m_text = new wxTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY);
}

void NumValidatorTestCase::tearDown()
{
    wxTheApp->GetTopWindow()->DestroyChildren();
}

void NumValidatorTestCase::TransferInt()
{
    int value = 0;
    wxIntegerValidator<int> valInt(&value);
    valInt.SetWindow(m_text);

    CPPUNIT_ASSERT( valInt.TransferToWindow() );
    CPPUNIT_ASSERT_EQUAL( "0", m_text->GetValue() );

    value = 17;
    CPPUNIT_ASSERT( valInt.TransferToWindow() );
    CPPUNIT_ASSERT_EQUAL( "17", m_text->GetValue() );


    m_text->ChangeValue("foobar");
    CPPUNIT_ASSERT( !valInt.TransferFromWindow() );

    m_text->ChangeValue("-234");
    CPPUNIT_ASSERT( valInt.TransferFromWindow() );
    CPPUNIT_ASSERT_EQUAL( -234, value );

    m_text->ChangeValue("9223372036854775808"); // == LLONG_MAX + 1
    CPPUNIT_ASSERT( !valInt.TransferFromWindow() );

    m_text->Clear();
    CPPUNIT_ASSERT( !valInt.TransferFromWindow() );
}

void NumValidatorTestCase::TransferUnsigned()
{
    unsigned value = 0;
    wxIntegerValidator<unsigned> valUnsigned(&value);
    valUnsigned.SetWindow(m_text);

    CPPUNIT_ASSERT( valUnsigned.TransferToWindow() );
    CPPUNIT_ASSERT_EQUAL( "0", m_text->GetValue() );

    value = 17;
    CPPUNIT_ASSERT( valUnsigned.TransferToWindow() );
    CPPUNIT_ASSERT_EQUAL( "17", m_text->GetValue() );


    m_text->ChangeValue("foobar");
    CPPUNIT_ASSERT( !valUnsigned.TransferFromWindow() );

    m_text->ChangeValue("-234");
    CPPUNIT_ASSERT( !valUnsigned.TransferFromWindow() );

    m_text->ChangeValue("234");
    CPPUNIT_ASSERT( valUnsigned.TransferFromWindow() );
    CPPUNIT_ASSERT_EQUAL( 234, value );

    m_text->ChangeValue("18446744073709551616"); // == ULLONG_MAX + 1
    CPPUNIT_ASSERT( !valUnsigned.TransferFromWindow() );

    m_text->Clear();
    CPPUNIT_ASSERT( !valUnsigned.TransferFromWindow() );
}

void NumValidatorTestCase::TransferFloat()
{
    // We need a locale with point as decimal separator.
    wxLocale loc(wxLANGUAGE_ENGLISH_UK, wxLOCALE_DONT_LOAD_DEFAULT);

    float value = 0;
    wxFloatingPointValidator<float> valFloat(3, &value);
    valFloat.SetWindow(m_text);

    CPPUNIT_ASSERT( valFloat.TransferToWindow() );
    CPPUNIT_ASSERT_EQUAL( "0.000", m_text->GetValue() );

    value = 1.234f;
    CPPUNIT_ASSERT( valFloat.TransferToWindow() );
    CPPUNIT_ASSERT_EQUAL( "1.234", m_text->GetValue() );

    value = 1.2345678f;
    CPPUNIT_ASSERT( valFloat.TransferToWindow() );
    CPPUNIT_ASSERT_EQUAL( "1.235", m_text->GetValue() );


    m_text->ChangeValue("foobar");
    CPPUNIT_ASSERT( !valFloat.TransferFromWindow() );

    m_text->ChangeValue("-234.567");
    CPPUNIT_ASSERT( valFloat.TransferFromWindow() );
    CPPUNIT_ASSERT_EQUAL( -234.567f, value );

    m_text->Clear();
    CPPUNIT_ASSERT( !valFloat.TransferFromWindow() );
}

void NumValidatorTestCase::ZeroAsBlank()
{
    long value = 0;
    m_text->SetValidator(
        wxMakeIntegerValidator(&value, wxNUM_VAL_ZERO_AS_BLANK));

    wxValidator * const val = m_text->GetValidator();

    CPPUNIT_ASSERT( val->TransferToWindow() );
    CPPUNIT_ASSERT_EQUAL( "", m_text->GetValue() );

    value++;
    CPPUNIT_ASSERT( val->TransferFromWindow() );
    CPPUNIT_ASSERT_EQUAL( 0, value );
}

void NumValidatorTestCase::NoTrailingZeroes()
{
    // We need a locale with point as decimal separator.
    wxLocale loc(wxLANGUAGE_ENGLISH_UK, wxLOCALE_DONT_LOAD_DEFAULT);

    double value = 1.2;
    m_text->SetValidator(
        wxMakeFloatingPointValidator(3, &value, wxNUM_VAL_NO_TRAILING_ZEROES));

    wxValidator * const val = m_text->GetValidator();

    CPPUNIT_ASSERT( val->TransferToWindow() );
    CPPUNIT_ASSERT_EQUAL( "1.2", m_text->GetValue() );

    value = 1.234;
    CPPUNIT_ASSERT( val->TransferToWindow() );
    CPPUNIT_ASSERT_EQUAL( "1.234", m_text->GetValue() );
}

#if wxUSE_UIACTIONSIMULATOR

void NumValidatorTestCase::Interactive()
{
#ifdef __WXMSW__
    // FIXME: This test fails on MSW buildbot slaves although works fine on
    //        development machine, no idea why. It seems to be a problem with
    //        wxUIActionSimulator rather the wxListCtrl control itself however.
    if ( IsAutomaticTest() )
        return;
#endif // __WXMSW__

    // Set a locale using comma as thousands separator character.
    wxLocale loc(wxLANGUAGE_ENGLISH_UK, wxLOCALE_DONT_LOAD_DEFAULT);

    m_text->SetValidator(
        wxIntegerValidator<unsigned>(NULL, wxNUM_VAL_THOUSANDS_SEPARATOR));

    // Create a sibling text control to be able to switch focus and thus
    // trigger the control validation/normalization.
    wxTextCtrl * const text2 = new wxTextCtrl(m_text->GetParent(), wxID_ANY);
    text2->Move(10, 80); // Just to see it better while debugging...
    wxFloatingPointValidator<float> valFloat(3);
    valFloat.SetRange(-10., 10.);
    text2->SetValidator(valFloat);

    wxUIActionSimulator sim;

    // Entering '-' in a control with positive range is not allowed.
    m_text->SetFocus();
    sim.Char('-');
    wxYield();
    CPPUNIT_ASSERT_EQUAL( "", m_text->GetValue() );

    // Neither is entering '.' or any non-digit character.
    sim.Text(".a+/");
    wxYield();
    CPPUNIT_ASSERT_EQUAL( "", m_text->GetValue() );

    // Entering digits should work though and after leaving the control the
    // contents should be normalized.
    sim.Text("1234567");
    wxYield();
    text2->SetFocus();
    wxYield();
    if ( loc.IsOk() )
        CPPUNIT_ASSERT_EQUAL( "1,234,567", m_text->GetValue() );
    else
        CPPUNIT_ASSERT_EQUAL( "1234567", m_text->GetValue() );


    // Entering both '-' and '.' in this control should work but only in the
    // correct order.
    sim.Char('-');
    wxYield();
    CPPUNIT_ASSERT_EQUAL( "-", text2->GetValue() );

    text2->SetInsertionPoint(0);
    sim.Char('.');
    wxYield();
    CPPUNIT_ASSERT_EQUAL( "-", text2->GetValue() );

    text2->SetInsertionPointEnd();
    sim.Char('.');
    wxYield();
    CPPUNIT_ASSERT_EQUAL( "-.", text2->GetValue() );

    // Adding up to three digits after the point should work.
    sim.Text("987");
    wxYield();
    CPPUNIT_ASSERT_EQUAL( "-.987", text2->GetValue() );

    // But no more.
    sim.Text("654");
    wxYield();
    CPPUNIT_ASSERT_EQUAL( "-.987", text2->GetValue() );

    // We can remove one digit and another one though.
    sim.Char(WXK_BACK);
    sim.Char(WXK_BACK);
    sim.Char('6');
    wxYield();
    CPPUNIT_ASSERT_EQUAL( "-.96", text2->GetValue() );


    // Also test the range constraint.
    text2->Clear();

    sim.Char('9');
    wxYield();
    CPPUNIT_ASSERT_EQUAL( "9", text2->GetValue() );

    sim.Char('9');
    wxYield();
    CPPUNIT_ASSERT_EQUAL( "9", text2->GetValue() );
}

#endif // wxUSE_UIACTIONSIMULATOR
