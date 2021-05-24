///////////////////////////////////////////////////////////////////////////////
// Name:        tests/validators/valnum.cpp
// Purpose:     Unit tests for numeric validators.
// Author:      Vadim Zeitlin
// Created:     2011-01-18
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/intl.h"
    #include "wx/textctrl.h"
    #include "wx/validate.h"
#endif // WX_PRECOMP

#include "wx/valnum.h"

#include "asserthelper.h"
#include "testableframe.h"

#include "wx/scopeguard.h"
#include "wx/uiaction.h"

class NumValidatorTestCase
{
public:
    NumValidatorTestCase();
    ~NumValidatorTestCase();

    void OnValidate(wxValidationStatusEvent& WXUNUSED(event))
        { /*No one will see the error messages, only the test*/ }

protected:
    wxTextCtrl* const m_text;

    wxDECLARE_NO_COPY_CLASS(NumValidatorTestCase);
};

NumValidatorTestCase::NumValidatorTestCase()
    : m_text(new wxTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY))
{
    m_text->Bind(wxEVT_VALIDATE_ERROR, &NumValidatorTestCase::OnValidate, this);
}

NumValidatorTestCase::~NumValidatorTestCase()
{
    delete m_text;
}

TEST_CASE_METHOD(NumValidatorTestCase, "ValNum::TransferInt", "[valnum]")
{
    int value = 0;
    wxIntegerValidator<int> valInt(&value);
    valInt.SetWindow(m_text);

    CHECK( valInt.TransferToWindow() );
    CHECK( m_text->GetValue() == "0" );

    value = 17;
    CHECK( valInt.TransferToWindow() );
    CHECK( m_text->GetValue() == "17" );


    m_text->ChangeValue("foobar");
    CHECK( !valInt.TransferFromWindow() );

    m_text->ChangeValue("-234");
    CHECK( valInt.TransferFromWindow() );
    CHECK( value == -234 );

    m_text->ChangeValue("9223372036854775808"); // == LLONG_MAX + 1
    CHECK( !valInt.TransferFromWindow() );

    m_text->Clear();
    CHECK( !valInt.TransferFromWindow() );
}

TEST_CASE_METHOD(NumValidatorTestCase, "ValNum::TransferUnsigned", "[valnum]")
{
    unsigned value = 0;
    wxIntegerValidator<unsigned> valUnsigned(&value);
    valUnsigned.SetWindow(m_text);

    CHECK( valUnsigned.TransferToWindow() );
    CHECK( m_text->GetValue() == "0" );

    value = 17;
    CHECK( valUnsigned.TransferToWindow() );
    CHECK( m_text->GetValue() == "17" );


    m_text->ChangeValue("foobar");
    CHECK( !valUnsigned.TransferFromWindow() );

    m_text->ChangeValue("-234");
    CHECK( !valUnsigned.TransferFromWindow() );

    m_text->ChangeValue("234");
    CHECK( valUnsigned.TransferFromWindow() );
    CHECK( value == 234 );

    m_text->ChangeValue("4294967295"); // == ULONG_MAX in 32 bits
    CHECK( valUnsigned.TransferFromWindow() );
    CHECK( value == wxUINT32_MAX );
    CHECK( valUnsigned.TransferToWindow() );
    CHECK( m_text->GetValue() == "4294967295" );

    m_text->ChangeValue("4294967296"); // == ULONG_MAX + 1
    CHECK( !valUnsigned.TransferFromWindow() );

    m_text->ChangeValue("18446744073709551616"); // == ULLONG_MAX + 1
    CHECK( !valUnsigned.TransferFromWindow() );

    m_text->Clear();
    CHECK( !valUnsigned.TransferFromWindow() );
}

TEST_CASE_METHOD(NumValidatorTestCase, "ValNum::TransferULL", "[valnum]")
{
    unsigned long long value = 0;
    wxIntegerValidator<unsigned long long> valULL(&value);
    valULL.SetWindow(m_text);

    SECTION("LLONG_MAX")
    {
        m_text->ChangeValue("9223372036854775807"); // == LLONG_MAX
        REQUIRE( valULL.TransferFromWindow() );
        CHECK( value == static_cast<wxULongLong_t>(wxINT64_MAX) );

        REQUIRE( valULL.TransferToWindow() );
        CHECK( m_text->GetValue() == "9223372036854775807" );
    }

    SECTION("LLONG_MAX+1")
    {
        m_text->ChangeValue("9223372036854775808"); // == LLONG_MAX + 1
        REQUIRE( valULL.TransferFromWindow() );
        CHECK( value == static_cast<wxULongLong_t>(wxINT64_MAX) + 1 );

        REQUIRE( valULL.TransferToWindow() );
        CHECK( m_text->GetValue() == "9223372036854775808" );
    }

    SECTION("ULLONG_MAX")
    {
        m_text->ChangeValue("18446744073709551615"); // == ULLONG_MAX
        REQUIRE( valULL.TransferFromWindow() );
        CHECK( value == wxUINT64_MAX );

        REQUIRE( valULL.TransferToWindow() );
        CHECK( m_text->GetValue() == "18446744073709551615" );
    }

    SECTION("ULLONG_MAX+1")
    {
        m_text->ChangeValue("18446744073709551616"); // == ULLONG_MAX + 1
        CHECK( !valULL.TransferFromWindow() );
    }
}

TEST_CASE_METHOD(NumValidatorTestCase, "ValNum::TransferFloat", "[valnum]")
{
    // We need a locale with point as decimal separator.
    wxLocale loc(wxLANGUAGE_ENGLISH_UK, wxLOCALE_DONT_LOAD_DEFAULT);

    float value = 0;
    wxFloatingPointValidator<float> valFloat(3, &value);
    valFloat.SetWindow(m_text);

    CHECK( valFloat.TransferToWindow() );
    CHECK( m_text->GetValue() == "0.000" );

    value = 1.234f;
    CHECK( valFloat.TransferToWindow() );
    CHECK( m_text->GetValue() == "1.234" );

    value = 1.2345678f;
    CHECK( valFloat.TransferToWindow() );
    CHECK( m_text->GetValue() == "1.235" );


    m_text->ChangeValue("foobar");
    CHECK( !valFloat.TransferFromWindow() );

    m_text->ChangeValue("-234.567");
    CHECK( valFloat.TransferFromWindow() );
    CHECK( value == -234.567f );

    m_text->Clear();
    CHECK( !valFloat.TransferFromWindow() );
}

TEST_CASE_METHOD(NumValidatorTestCase, "ValNum::ZeroAsBlank", "[valnum]")
{
    long value = 0;
    m_text->SetValidator(
        wxMakeIntegerValidator(&value, wxNUM_VAL_ZERO_AS_BLANK));

    wxValidator * const val = m_text->GetValidator();

    CHECK( val->TransferToWindow() );
    CHECK( m_text->GetValue() == "" );

    value++;
    CHECK( val->TransferFromWindow() );
    CHECK( value == 0 );
}

TEST_CASE_METHOD(NumValidatorTestCase, "ValNum::NoTrailingZeroes", "[valnum]")
{
    // We need a locale with point as decimal separator.
    wxLocale loc(wxLANGUAGE_ENGLISH_UK, wxLOCALE_DONT_LOAD_DEFAULT);

    double value = 1.2;
    m_text->SetValidator(
        wxMakeFloatingPointValidator(3, &value, wxNUM_VAL_NO_TRAILING_ZEROES));

    wxValidator * const val = m_text->GetValidator();

    CHECK( val->TransferToWindow() );
    CHECK( m_text->GetValue() == "1.2" );

    value = 1.234;
    CHECK( val->TransferToWindow() );
    CHECK( m_text->GetValue() == "1.234" );
}

TEST_CASE_METHOD(NumValidatorTestCase, "ValNum::ClipboardTest", "[valnum]")
{
    int value = 9;
    wxIntegerValidator<int> valInt(&value);
    valInt.SetRange(0, 999);

    m_text->SetValidator(valInt);

    wxWindow * const parent = m_text->GetParent();
    CHECK( parent != NULL );

    CHECK( parent->TransferDataToWindow() );
    CHECK( parent->Validate() );
    CHECK( m_text->GetValue() == "9" );

    m_text->SelectAll();
    m_text->Cut();

    // The validator is created without wxNUM_VAL_ZERO_AS_BLANK
    // therefore the validation should fail.
    CHECK( m_text->GetValue() == "" );
    CHECK( !parent->Validate() );

    m_text->Paste();
    m_text->Paste();

    CHECK( m_text->GetValue() == "99" );
    CHECK( parent->Validate() );

    m_text->Paste();
    m_text->Paste(); // Should not be allowed: 9999 is out-of-range.

    CHECK( m_text->GetValue() == "999" );
    CHECK( parent->Validate() );

    m_text->SetValue("-1");

    CHECK( m_text->GetValue() == "-1" );
    CHECK( !parent->Validate() );

    m_text->SelectAll();
    m_text->Cut();
    m_text->Paste(); // Should not be allowed: -1 is out-of-range.

    CHECK( m_text->GetValue() == "" );
    CHECK( !parent->Validate() );
}

#if wxUSE_UIACTIONSIMULATOR

TEST_CASE_METHOD(NumValidatorTestCase, "ValNum::Interactive", "[valnum]")
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
    text2->Bind(wxEVT_VALIDATE_ERROR, &NumValidatorTestCase::OnValidate, this);
    wxON_BLOCK_EXIT_OBJ0( *text2, wxWindow::Destroy );
    text2->Move(10, 80); // Just to see it better while debugging...
    wxFloatingPointValidator<float> valFloat(3);
    valFloat.SetRange(-10., 10.);
    text2->SetValidator(valFloat);

    wxUIActionSimulator sim;

    // Entering '-' in a control with positive range is not allowed.
    m_text->SetFocus();
    sim.Char('-');
    wxYield();
    CHECK( m_text->GetValue() == "" );

    // Neither is entering '.' or any non-digit character.
    sim.Text(".a+/");
    wxYield();
    CHECK( m_text->GetValue() == "" );

    // Entering digits should work though and after leaving the control the
    // contents should be normalized.
    sim.Text("1234567");
    wxYield();
    text2->SetFocus();
    wxYield();
    if ( loc.IsOk() )
        CHECK( m_text->GetValue() == "1,234,567" );
    else
        CHECK( m_text->GetValue() == "1234567" );


    // Entering both '-' and '.' in this control should work but only in the
    // correct order.
    sim.Char('-');
    wxYield();
    CHECK( text2->GetValue() == "-" );

    text2->SetInsertionPoint(0);
    sim.Char('.');
    wxYield();
    CHECK( text2->GetValue() == "-" );

    text2->SetInsertionPointEnd();
    sim.Char('.');
    wxYield();
    CHECK( text2->GetValue() == "-." );

    // Adding up to three digits after the point should work.
    sim.Text("987");
    wxYield();
    CHECK( text2->GetValue() == "-.987" );

    // But no more.
    sim.Text("654");
    wxYield();
    CHECK( text2->GetValue() == "-.987" );

    // We can remove one digit and another one though.
    sim.Char(WXK_BACK);
    sim.Char(WXK_BACK);
    sim.Char('6');
    wxYield();
    CHECK( text2->GetValue() == "-.96" );


    // Also test the range constraint.

    SECTION("With range [-10., 10.]")
    {
        text2->Clear();

        sim.Char('9');
        wxYield();
        CHECK( text2->GetValue() == "9" );

        sim.Char('9');
        wxYield();
        CHECK( text2->GetValue() == "9" );

        text2->SetInsertionPoint(0);
        sim.Char('-');
        wxYield();
        CHECK( text2->GetValue() == "-9" );

        sim.Char('9');
        wxYield();
        CHECK( text2->GetValue() == "-9" );
    }

    SECTION("With range [0.2, 1.0]")
    {
        valFloat.SetRange(0.2, 1.0);
        text2->SetValidator(valFloat);

        text2->Clear();
        wxYield();

        sim.Text("0.1");
        wxYield();
        CHECK( text2->GetValue() == "0." ); // the validator should reject '1'
        CHECK( !text2->Validate() );

        sim.Char('5'); // We already have "0." in the control by the previous sim.
        wxYield();
        CHECK( text2->GetValue() == "0.5" );
        CHECK( text2->Validate() );

        text2->Clear();

        sim.Char('2');
        wxYield();
        CHECK( text2->GetValue() == "" );
        CHECK( !text2->Validate() );
    }

    // Now with integer validator...
    SECTION("With ranges [3, 30] and [180, 360]")
    {
        // With range [3, 30]
        wxIntegerValidator<unsigned> valInt;
        valInt.SetRange(3, 30);
        text2->SetValidator(valInt);

        text2->Clear();
        wxYield();

        sim.Char('2');
        wxYield();
        CHECK( text2->GetValue() == "2" );
        // Even if we could enter the first digit of 22, validation should fail.
        CHECK( !text2->Validate() );

        sim.Char('2');
        wxYield();
        CHECK( text2->GetValue() == "22" );
        CHECK( text2->Validate() );

        sim.Char('2');
        wxYield();
        CHECK( text2->GetValue() == "22" );
        CHECK( text2->Validate() );

        // With range [180, 360]
        valInt.SetRange(180, 360);
        text2->SetValidator(valInt);

        text2->Clear();
        wxYield();

        // will be rejected, no possible values starting with 4 in the range (180, 360)
        sim.Char('4');
        wxYield();
        CHECK( text2->GetValue() == "" );

        sim.Char('1');
        wxYield();
        CHECK( text2->GetValue() == "1" );
        CHECK( !text2->Validate() );

        // will be rejected, no possible values of 17x in the range (180, 360)
        sim.Char('7');
        wxYield();
        CHECK( text2->GetValue() == "1" );

        sim.Text("850"); // 0 will be rejected
        wxYield();
        CHECK( text2->GetValue() == "185" );
        CHECK( text2->Validate() );

        text2->Clear();
        wxYield();

        sim.Text("361"); // 1 will be rejected
        wxYield();
        CHECK( text2->GetValue() == "36" );
        CHECK( !text2->Validate() ); // 36 is not in the range (180, 360)

        sim.Char('0');
        wxYield();
        CHECK( text2->GetValue() == "360" );
        CHECK( text2->Validate() );
    }
}

#endif // wxUSE_UIACTIONSIMULATOR
