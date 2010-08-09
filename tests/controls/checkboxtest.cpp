///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/checkboxtest.cpp
// Purpose:     wCheckBox unit test
// Author:      Steven Lamerton
// Created:     2010-07-14
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_CHECKBOX

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/checkbox.h"
#endif // WX_PRECOMP

#include "testableframe.h"

class CheckBoxTestCase : public CppUnit::TestCase
{
public:
    CheckBoxTestCase() { }

    void setUp();
    void tearDown();

private:
    CPPUNIT_TEST_SUITE( CheckBoxTestCase );
        CPPUNIT_TEST( Check );
        CPPUNIT_TEST( ThirdState );
        CPPUNIT_TEST( ThirdStateUser );
    CPPUNIT_TEST_SUITE_END();

    void Check();
    void ThirdState();
    void ThirdStateUser();

    wxCheckBox* m_check;

    DECLARE_NO_COPY_CLASS(CheckBoxTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( CheckBoxTestCase );

// also include in it's own registry so that these tests can be run alone
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
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_check, wxEVT_COMMAND_CHECKBOX_CLICKED);

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
    CPPUNIT_ASSERT_EQUAL(0, frame->GetEventCount());
}

void CheckBoxTestCase::ThirdState()
{
#if !defined(__WXMGL__) && !defined(__WXPM__) && !defined(__WXGTK12__)
    wxDELETE(m_check);
    m_check = new wxCheckBox(wxTheApp->GetTopWindow(), wxID_ANY, "Check box", 
                             wxDefaultPosition, wxDefaultSize, wxCHK_3STATE);

    CPPUNIT_ASSERT_EQUAL(wxCHK_UNCHECKED, m_check->Get3StateValue());
    CPPUNIT_ASSERT(m_check->Is3State());
    CPPUNIT_ASSERT(!m_check->Is3rdStateAllowedForUser());

    m_check->SetValue(true);

    CPPUNIT_ASSERT_EQUAL(wxCHK_CHECKED, m_check->Get3StateValue());

    m_check->Set3StateValue(wxCHK_UNDETERMINED);

    CPPUNIT_ASSERT_EQUAL(wxCHK_UNDETERMINED, m_check->Get3StateValue());
#endif
}

void CheckBoxTestCase::ThirdStateUser()
{
#if !defined(__WXMGL__) && !defined(__WXPM__) && !defined(__WXGTK12__)
    wxDELETE(m_check);
    m_check = new wxCheckBox(wxTheApp->GetTopWindow(), wxID_ANY, "Check box", 
                             wxDefaultPosition, wxDefaultSize, 
                             wxCHK_3STATE | wxCHK_ALLOW_3RD_STATE_FOR_USER);

    CPPUNIT_ASSERT_EQUAL(wxCHK_UNCHECKED, m_check->Get3StateValue());
    CPPUNIT_ASSERT(m_check->Is3State());
    CPPUNIT_ASSERT(m_check->Is3rdStateAllowedForUser());

    m_check->SetValue(true);

    CPPUNIT_ASSERT_EQUAL(wxCHK_CHECKED, m_check->Get3StateValue());

    m_check->Set3StateValue(wxCHK_UNDETERMINED);

    CPPUNIT_ASSERT_EQUAL(wxCHK_UNDETERMINED, m_check->Get3StateValue());
#endif
}

#endif //wxUSE_CHECKBOX
