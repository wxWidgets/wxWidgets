///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/checkboxtest.cpp
// Purpose:     wCheckBox unit test
// Author:      Steven Lamerton
// Created:     2010-07-14
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_CHECKBOX


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/checkbox.h"
#endif // WX_PRECOMP

#include "testableframe.h"

class CheckBoxTestCase : public CppUnit::TestCase
{
public:
    CheckBoxTestCase() { }

    void setUp() override;
    void tearDown() override;

private:
    CPPUNIT_TEST_SUITE( CheckBoxTestCase );
        CPPUNIT_TEST( Check );
#ifdef wxHAS_3STATE_CHECKBOX
        CPPUNIT_TEST( ThirdState );
        CPPUNIT_TEST( ThirdStateUser );
        CPPUNIT_TEST( InvalidStyles );
#endif // wxHAS_3STATE_CHECKBOX
    CPPUNIT_TEST_SUITE_END();

    void Check();
#ifdef wxHAS_3STATE_CHECKBOX
    void ThirdState();
    void ThirdStateUser();
    void InvalidStyles();
#endif // wxHAS_3STATE_CHECKBOX

    // Initialize m_check with a new checkbox with the specified style
    //
    // This function always returns false just to make it more convenient to
    // use inside WX_ASSERT_FAILS_WITH_ASSERT(), its return value doesn't have
    // any meaning otherwise.
    bool CreateCheckBox(long style)
    {
        wxDELETE( m_check );
        m_check = new wxCheckBox(wxTheApp->GetTopWindow(), wxID_ANY, "Check box",
                                 wxDefaultPosition, wxDefaultSize, style);
        return false;
    }


    wxCheckBox* m_check;

    wxDECLARE_NO_COPY_CLASS(CheckBoxTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( CheckBoxTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( CheckBoxTestCase, "CheckBoxTestCase" );

void CheckBoxTestCase::setUp()
{
    m_check = new wxCheckBox(wxTheApp->GetTopWindow(), wxID_ANY, "Check box");
}

void CheckBoxTestCase::tearDown()
{
    delete m_check;
}

void CheckBoxTestCase::Check()
{
    EventCounter clicked(m_check, wxEVT_CHECKBOX);

    //We should be unchecked by default
    CPPUNIT_ASSERT(!m_check->IsChecked());

    m_check->SetValue(true);

    CPPUNIT_ASSERT(m_check->IsChecked());

    m_check->SetValue(false);

    CPPUNIT_ASSERT(!m_check->IsChecked());

    m_check->Set3StateValue(wxCHK_CHECKED);

    CPPUNIT_ASSERT(m_check->IsChecked());

    m_check->Set3StateValue(wxCHK_UNCHECKED);

    CPPUNIT_ASSERT(!m_check->IsChecked());

    //None of these should send events
    CPPUNIT_ASSERT_EQUAL(0, clicked.GetCount());
}

#ifdef wxHAS_3STATE_CHECKBOX
void CheckBoxTestCase::ThirdState()
{
    CreateCheckBox(wxCHK_3STATE);

    CPPUNIT_ASSERT_EQUAL(wxCHK_UNCHECKED, m_check->Get3StateValue());
    CPPUNIT_ASSERT(m_check->Is3State());
    CPPUNIT_ASSERT(!m_check->Is3rdStateAllowedForUser());

    m_check->SetValue(true);

    CPPUNIT_ASSERT_EQUAL(wxCHK_CHECKED, m_check->Get3StateValue());

    m_check->Set3StateValue(wxCHK_UNDETERMINED);

    CPPUNIT_ASSERT_EQUAL(wxCHK_UNDETERMINED, m_check->Get3StateValue());
}

void CheckBoxTestCase::ThirdStateUser()
{
    CreateCheckBox(wxCHK_3STATE | wxCHK_ALLOW_3RD_STATE_FOR_USER);

    CPPUNIT_ASSERT_EQUAL(wxCHK_UNCHECKED, m_check->Get3StateValue());
    CPPUNIT_ASSERT(m_check->Is3State());
    CPPUNIT_ASSERT(m_check->Is3rdStateAllowedForUser());

    m_check->SetValue(true);

    CPPUNIT_ASSERT_EQUAL(wxCHK_CHECKED, m_check->Get3StateValue());

    m_check->Set3StateValue(wxCHK_UNDETERMINED);

    CPPUNIT_ASSERT_EQUAL(wxCHK_UNDETERMINED, m_check->Get3StateValue());
}

void CheckBoxTestCase::InvalidStyles()
{
    // Check that using incompatible styles doesn't work.
    WX_ASSERT_FAILS_WITH_ASSERT( CreateCheckBox(wxCHK_2STATE | wxCHK_3STATE) );
#if !wxDEBUG_LEVEL
    CPPUNIT_ASSERT( !m_check->Is3State() );
    CPPUNIT_ASSERT( !m_check->Is3rdStateAllowedForUser() );
#endif

    WX_ASSERT_FAILS_WITH_ASSERT(
        CreateCheckBox(wxCHK_2STATE | wxCHK_ALLOW_3RD_STATE_FOR_USER) );
#if !wxDEBUG_LEVEL
    CPPUNIT_ASSERT( !m_check->Is3State() );
    CPPUNIT_ASSERT( !m_check->Is3rdStateAllowedForUser() );
#endif

    // wxCHK_ALLOW_3RD_STATE_FOR_USER without wxCHK_3STATE doesn't work.
    WX_ASSERT_FAILS_WITH_ASSERT( CreateCheckBox(wxCHK_ALLOW_3RD_STATE_FOR_USER) );
}

#endif // wxHAS_3STATE_CHECKBOX

#endif // wxUSE_CHECKBOX
