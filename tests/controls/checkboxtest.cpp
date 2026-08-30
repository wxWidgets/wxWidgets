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

#include <memory>

class CheckBoxTestCase
{
public:
    CheckBoxTestCase();

protected:
    // Initialize m_check.get() with a new checkbox with the specified style
    //
    // This function always returns false just to make it more convenient to
    // use inside WX_ASSERT_FAILS_WITH_ASSERT(), its return value doesn't have
    // any meaning otherwise.
    bool CreateCheckBox(long style)
    {
        m_check = make_unique<wxCheckBox>(wxTheApp->GetTopWindow(), wxID_ANY,
                                          "Check box",
                                          wxDefaultPosition, wxDefaultSize,
                                          style);
        return false;
    }


    std::unique_ptr<wxCheckBox> m_check;

    wxDECLARE_NO_COPY_CLASS(CheckBoxTestCase);
};

CheckBoxTestCase::CheckBoxTestCase()
{
    m_check = make_unique<wxCheckBox>(wxTheApp->GetTopWindow(), wxID_ANY,
                                      "Check box");
}


TEST_CASE_METHOD(CheckBoxTestCase, "CheckBox::Check", "[checkbox]")
{
    EventCounter clicked(m_check.get(), wxEVT_CHECKBOX);

    //We should be unchecked by default
    CHECK(!m_check->IsChecked());

    m_check->SetValue(true);

    CHECK(m_check->IsChecked());

    m_check->SetValue(false);

    CHECK(!m_check->IsChecked());

    m_check->Set3StateValue(wxCHK_CHECKED);

    CHECK(m_check->IsChecked());

    m_check->Set3StateValue(wxCHK_UNCHECKED);

    CHECK(!m_check->IsChecked());

    //None of these should send events
    CHECK(clicked.GetCount() == 0);
}

#ifdef wxHAS_3STATE_CHECKBOX
TEST_CASE_METHOD(CheckBoxTestCase, "CheckBox::ThirdState", "[checkbox]")
{
    CreateCheckBox(wxCHK_3STATE);

    CHECK(m_check->Get3StateValue() == wxCHK_UNCHECKED);
    CHECK(m_check->Is3State());
    CHECK(!m_check->Is3rdStateAllowedForUser());

    m_check->SetValue(true);

    CHECK(m_check->Get3StateValue() == wxCHK_CHECKED);

    m_check->Set3StateValue(wxCHK_UNDETERMINED);

    CHECK(m_check->Get3StateValue() == wxCHK_UNDETERMINED);
}

TEST_CASE_METHOD(CheckBoxTestCase, "CheckBox::ThirdStateUser", "[checkbox]")
{
    CreateCheckBox(wxCHK_3STATE | wxCHK_ALLOW_3RD_STATE_FOR_USER);

    CHECK(m_check->Get3StateValue() == wxCHK_UNCHECKED);
    CHECK(m_check->Is3State());
    CHECK(m_check->Is3rdStateAllowedForUser());

    m_check->SetValue(true);

    CHECK(m_check->Get3StateValue() == wxCHK_CHECKED);

    m_check->Set3StateValue(wxCHK_UNDETERMINED);

    CHECK(m_check->Get3StateValue() == wxCHK_UNDETERMINED);

    m_check->SetValue(true);
    CHECK(m_check->Get3StateValue() == wxCHK_CHECKED);
}

TEST_CASE_METHOD(CheckBoxTestCase, "CheckBox::InvalidStyles", "[checkbox]")
{
    // Check that using incompatible styles doesn't work.
    WX_ASSERT_FAILS_WITH_ASSERT( CreateCheckBox(wxCHK_2STATE | wxCHK_3STATE) );
#if !wxDEBUG_LEVEL
    CHECK( !m_check->Is3State() );
    CHECK( !m_check->Is3rdStateAllowedForUser() );
#endif

    WX_ASSERT_FAILS_WITH_ASSERT(
        CreateCheckBox(wxCHK_2STATE | wxCHK_ALLOW_3RD_STATE_FOR_USER) );
#if !wxDEBUG_LEVEL
    CHECK( !m_check->Is3State() );
    CHECK( !m_check->Is3rdStateAllowedForUser() );
#endif

    // wxCHK_ALLOW_3RD_STATE_FOR_USER without wxCHK_3STATE doesn't work.
    WX_ASSERT_FAILS_WITH_ASSERT( CreateCheckBox(wxCHK_ALLOW_3RD_STATE_FOR_USER) );
}

#endif // wxHAS_3STATE_CHECKBOX

#endif // wxUSE_CHECKBOX
