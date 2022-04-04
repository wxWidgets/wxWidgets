///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/spinctrltest.cpp
// Purpose:     wxSpinCtrl unit test
// Author:      Steven Lamerton
// Created:     2010-07-21
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_SPINCTRL


#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "testableframe.h"
#include "wx/uiaction.h"
#include "wx/spinctrl.h"
#include "wx/textctrl.h"

class SpinCtrlTestCase1
{
public:
    SpinCtrlTestCase1()
        : m_spin(new wxSpinCtrl())
    {
    }

    ~SpinCtrlTestCase1()
    {
        delete m_spin;
    }

protected:
    wxSpinCtrl* m_spin;
};

class SpinCtrlTestCase2
{
public:
    SpinCtrlTestCase2()
        : m_spin(new wxSpinCtrl(wxTheApp->GetTopWindow()))
    {
    }

    ~SpinCtrlTestCase2()
    {
        delete m_spin;
    }

protected:
    wxSpinCtrl* m_spin;
};

class SpinCtrlTestCase3
{
public:
    SpinCtrlTestCase3()
        : m_spin(new wxSpinCtrl(wxTheApp->GetTopWindow()))
    {
        m_spin->Bind(wxEVT_SPINCTRL, &SpinCtrlTestCase3::OnSpinSetValue, this);
    }

    ~SpinCtrlTestCase3()
    {
        delete m_spin;
    }

private:
    void OnSpinSetValue(wxSpinEvent &e)
    {
        // Constrain the value to be in the 1..16 range or 32.
        int newVal = e.GetValue();

        if ( newVal == 31 )
            m_spin->SetValue(16);
        else if ( newVal > 16 )
            m_spin->SetValue(32);
    }

protected:
    wxSpinCtrl* m_spin;
};


TEST_CASE_METHOD(SpinCtrlTestCase2, "SpinCtrl::Init", "[spinctrl]")
{
    // Initial value is defined by "initial" argument which is 0 by default.
    CHECK(m_spin->GetValue() == 0);
}

TEST_CASE_METHOD(SpinCtrlTestCase1, "SpinCtrl::Init2", "[spinctrl]")
{
    m_spin->Create(wxTheApp->GetTopWindow(), wxID_ANY, "",
                   wxDefaultPosition, wxDefaultSize, 0,
                   0, 100, 17);

    // Recreate the control with another "initial" to check this.
    CHECK(m_spin->GetValue() == 17);
}

TEST_CASE_METHOD(SpinCtrlTestCase1, "SpinCtrl::Init3", "[spinctrl]")
{
    m_spin->Create(wxTheApp->GetTopWindow(), wxID_ANY, "",
                   wxDefaultPosition, wxDefaultSize, 0,
                   0, 200, 150);

    // Recreate the control with another "initial" outside of standard spin
    // ctrl range.
    CHECK(m_spin->GetValue() == 150);
}

TEST_CASE_METHOD(SpinCtrlTestCase1, "SpinCtrl::Init4", "[spinctrl]")
{
    m_spin->Create(wxTheApp->GetTopWindow(), wxID_ANY, "99",
                   wxDefaultPosition, wxDefaultSize, 0,
                   0, 100, 17);

    // Recreate the control with another "initial" outside of standard spin
    // ctrl range.
    // But if the text string is specified, it takes precedence.
    CHECK(m_spin->GetValue() == 99);
}

TEST_CASE_METHOD(SpinCtrlTestCase1, "SpinCtrl::InitOutOfRange", "[spinctrl]")
{
    m_spin->Create(wxTheApp->GetTopWindow(), wxID_ANY, "",
                   wxDefaultPosition, wxDefaultSize, 0,
                   10, 20, 0);

    // Recreate the control with another "initial" outside of the valid range:
    // it shouldn't be taken into account.
    CHECK(m_spin->GetValue() == 10);
}

TEST_CASE_METHOD(SpinCtrlTestCase1, "SpinCtrl::NoEventsInCtor", "[spinctrl]")
{
    // Verify that creating the control does not generate any events. This is
    // unexpected and shouldn't happen.
    EventCounter updatedSpin(m_spin, wxEVT_SPINCTRL);
    EventCounter updatedText(m_spin, wxEVT_TEXT);

    m_spin->Create(wxTheApp->GetTopWindow(), wxID_ANY, "",
                   wxDefaultPosition, wxDefaultSize, 0,
                   0, 100, 17);

    CHECK(updatedSpin.GetCount() == 0);
    CHECK(updatedText.GetCount() == 0);
}

TEST_CASE_METHOD(SpinCtrlTestCase2, "SpinCtrl::Arrows", "[spinctrl]")
{
#if wxUSE_UIACTIONSIMULATOR
    EventCounter updated(m_spin, wxEVT_SPINCTRL);

    wxUIActionSimulator sim;

    m_spin->SetFocus();
    wxYield();

    sim.Char(WXK_UP);

    wxYield();

    CHECK(updated.GetCount() == 1);
    CHECK(m_spin->GetValue() == 1);
    updated.Clear();

    sim.Char(WXK_DOWN);

    wxYield();

    CHECK(updated.GetCount() == 1);
    CHECK(m_spin->GetValue() == 0);
#endif
}

TEST_CASE_METHOD(SpinCtrlTestCase1, "SpinCtrl::Wrap", "[spinctrl]")
{
#if wxUSE_UIACTIONSIMULATOR
    m_spin->Create(wxTheApp->GetTopWindow(), wxID_ANY, "",
                   wxDefaultPosition, wxDefaultSize,
                   wxSP_ARROW_KEYS | wxSP_WRAP);

    wxUIActionSimulator sim;

    m_spin->SetFocus();
    wxYield();

    sim.Char(WXK_DOWN);

    wxYield();

    CHECK(m_spin->GetValue() == 100);

    sim.Char(WXK_UP);

    wxYield();

    CHECK(m_spin->GetValue() == 0);
#endif
}

TEST_CASE_METHOD(SpinCtrlTestCase2, "SpinCtrl::Range", "[spinctrl]")
{
    CHECK(m_spin->GetMin() == 0);
    CHECK(m_spin->GetMax() == 100);
    CHECK(m_spin->GetBase() == 10);

    // Test that the value is adjusted to be inside the new valid range but
    // that this doesn't result in any events (as this is not something done by
    // the user).
    {
        EventCounter updatedSpin(m_spin, wxEVT_SPINCTRL);
        EventCounter updatedText(m_spin, wxEVT_TEXT);

        m_spin->SetRange(1, 10);
        CHECK(m_spin->GetValue() == 1);

        CHECK(updatedSpin.GetCount() == 0);
        CHECK(updatedText.GetCount() == 0);
    }

    // Test negative ranges
    m_spin->SetRange(-10, 10);

    CHECK(m_spin->GetMin() == -10);
    CHECK(m_spin->GetMax() == 10);

    // With base 16 only ranges including values >= 0 are allowed
    m_spin->SetRange(0, 10);
    int oldMinVal = m_spin->GetMin();
    int oldMaxVal = m_spin->GetMax();
    CHECK(oldMinVal == 0);
    CHECK(oldMaxVal == 10);

    CHECK(m_spin->SetBase(16) == true);
    CHECK(m_spin->GetBase() == 16);

    // New range should be silently ignored
    m_spin->SetRange(-20, 20);
    CHECK(m_spin->GetMin() == oldMinVal);
    CHECK(m_spin->GetMax() == oldMaxVal);

    // This range should be accepted
    m_spin->SetRange(2, 8);
    CHECK(m_spin->GetMin() == 2);
    CHECK(m_spin->GetMax() == 8);

    CHECK(m_spin->SetBase(10) == true);

    CHECK(m_spin->GetBase() == 10);

    //Test backwards ranges
    m_spin->SetRange(75, 50);

    CHECK(m_spin->GetMin() == 75);
    CHECK(m_spin->GetMax() == 50);
}

TEST_CASE_METHOD(SpinCtrlTestCase2, "SpinCtrl::Value", "[spinctrl]")
{
    EventCounter updatedSpin(m_spin, wxEVT_SPINCTRL);
    EventCounter updatedText(m_spin, wxEVT_TEXT);

    CHECK(m_spin->GetValue() == 0);

    m_spin->SetValue(50);
    CHECK(m_spin->GetValue() == 50);

    m_spin->SetValue(-10);
    CHECK(m_spin->GetValue() == 0);

    m_spin->SetValue(110);
    CHECK(m_spin->GetValue() == 100);

    // Calling SetValue() shouldn't have generated any events.
    CHECK(updatedSpin.GetCount() == 0);
    CHECK(updatedText.GetCount() == 0);

    // Also test that setting the text value works.
    CHECK( m_spin->GetTextValue() == "100" );

    m_spin->SetValue("57");
    CHECK( m_spin->GetTextValue() == "57" );
    CHECK( m_spin->GetValue() == 57 );

    CHECK(updatedSpin.GetCount() == 0);
    CHECK(updatedText.GetCount() == 0);

    m_spin->SetValue("");
    CHECK( m_spin->GetTextValue() == "" );
    CHECK( m_spin->GetValue() == 0 );

    CHECK(updatedSpin.GetCount() == 0);
    CHECK(updatedText.GetCount() == 0);
}

TEST_CASE_METHOD(SpinCtrlTestCase2, "SpinCtrl::Base", "[spinctrl]")
{
    CHECK(m_spin->GetMin() == 0);
    CHECK(m_spin->GetMax() == 100);
    CHECK(m_spin->GetBase() == 10);

    // Only 10 and 16 bases are allowed
    CHECK(m_spin->SetBase(10) == true);
    CHECK(m_spin->GetBase() == 10);

    CHECK_FALSE(m_spin->SetBase(8));
    CHECK(m_spin->GetBase() == 10);

    CHECK_FALSE(m_spin->SetBase(2));
    CHECK(m_spin->GetBase() == 10);

    CHECK(m_spin->SetBase(16) == true);
    CHECK(m_spin->GetBase() == 16);

    CHECK(m_spin->SetBase(10) == true);
    CHECK(m_spin->GetBase() == 10);

    // When range contains negative values only base 10 is allowed
    m_spin->SetRange(-10, 10);
    CHECK(m_spin->GetMin() == -10);
    CHECK(m_spin->GetMax() == 10);

    CHECK_FALSE(m_spin->SetBase(8));
    CHECK(m_spin->GetBase() == 10);

    CHECK_FALSE(m_spin->SetBase(2));
    CHECK(m_spin->GetBase() == 10);

    CHECK_FALSE(m_spin->SetBase(16));
    CHECK(m_spin->GetBase() == 10);

    CHECK(m_spin->SetBase(10) == true);
    CHECK(m_spin->GetBase() == 10);
}

TEST_CASE_METHOD(SpinCtrlTestCase3, "SpinCtrl::SetValueInsideEventHandler", "[spinctrl]")
{
#if wxUSE_UIACTIONSIMULATOR
    // A dummy control with which we change the focus.
    wxTextCtrl* text = new wxTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY);
    text->Move(m_spin->GetSize().x, m_spin->GetSize().y * 3);

    wxUIActionSimulator sim;

    // run multiple times to make sure there are no issues with keeping old value
    for ( size_t i = 0; i < 2; i++ )
    {
        m_spin->SetFocus();
        wxYield();

        sim.Char(WXK_DELETE);
        sim.Char(WXK_DELETE);
        sim.Text("20");
        wxYield();

        text->SetFocus();
        wxYield();

        CHECK(m_spin->GetValue() == 32);
    }

    delete text;
#endif // wxUSE_UIACTIONSIMULATOR
}

TEST_CASE_METHOD(SpinCtrlTestCase1, "SpinCtrl::Increment", "[spinctrl]")
{
#if wxUSE_UIACTIONSIMULATOR
    m_spin->Create(wxTheApp->GetTopWindow(), wxID_ANY, "",
        wxDefaultPosition, wxDefaultSize,
        wxSP_ARROW_KEYS | wxSP_WRAP);

    wxUIActionSimulator sim;

    CHECK( m_spin->GetIncrement() == 1 );

    m_spin->SetFocus();
    wxYield();
    m_spin->SetIncrement( 5 );
    sim.Char(WXK_UP);

    wxYield();

    CHECK(m_spin->GetValue() == 5);

    int increment = m_spin->GetIncrement();

    CHECK( increment == 5 );
#endif
}

#endif
