///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/buttontest.cpp
// Purpose:     wxButton unit test
// Author:      Steven Lamerton
// Created:     2010-06-21
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_BUTTON


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/button.h"
#endif // WX_PRECOMP

#include "testableframe.h"
#include "wx/uiaction.h"
#include "wx/artprov.h"

// Get operator<<(wxSize) so that wxSize values are shown correctly in case of
// a failure of a CHECK() involving them.
#include "asserthelper.h"
#include "waitfor.h"

class ButtonTestCase
{
public:
    ButtonTestCase();
    ~ButtonTestCase();

protected:
    wxButton* m_button;

    wxDECLARE_NO_COPY_CLASS(ButtonTestCase);
};

ButtonTestCase::ButtonTestCase()
{
    //We use wxTheApp->GetTopWindow() as there is only a single testable frame
    //so it will always be returned
    m_button = new wxButton(wxTheApp->GetTopWindow(), wxID_ANY, "wxButton");
}

ButtonTestCase::~ButtonTestCase()
{
    delete m_button;
}

#if wxUSE_UIACTIONSIMULATOR

TEST_CASE_METHOD(ButtonTestCase, "Button::Click", "[button]")
{
    //We use the internal class EventCounter which handles connecting and
    //disconnecting the control to the wxTestableFrame
    EventCounter clicked(m_button, wxEVT_BUTTON);

    wxUIActionSimulator sim;

    //We move in slightly to account for window decorations, we need to yield
    //after every wxUIActionSimulator action to keep everything working in GTK
    sim.MouseMove(m_button->GetScreenPosition() + wxPoint(10, 10));
    wxYield();

    sim.MouseClick();

    // At least under wxMSW calling wxYield() just once doesn't always work, so
    // try for a while.
    WaitFor("button to be clicked", [&]() { return clicked.GetCount() != 0; });

    CHECK( clicked.GetCount() == 1 );
}

TEST_CASE_METHOD(ButtonTestCase, "Button::Disabled", "[button]")
{
    wxUIActionSimulator sim;

    // In this test we disable the button and check events are not sent and we
    // do it once by disabling the previously enabled button and once by
    // creating the button in the disabled state.
    SECTION("Disable after creation")
    {
        m_button->Disable();
    }

    SECTION("Create disabled")
    {
        delete m_button;
        m_button = new wxButton();
        m_button->Disable();
        m_button->Create(wxTheApp->GetTopWindow(), wxID_ANY, "wxButton");
    }

    EventCounter clicked(m_button, wxEVT_BUTTON);

    sim.MouseMove(m_button->GetScreenPosition() + wxPoint(10, 10));
    wxYield();

    sim.MouseClick();
    wxYield();

    CHECK( clicked.GetCount() == 0 );
}

#endif // wxUSE_UIACTIONSIMULATOR

TEST_CASE_METHOD(ButtonTestCase, "Button::Auth", "[button]")
{
    //Some functions only work on specific operating system versions, for
    //this we need a runtime check
    int major = 0;

    if(wxGetOsVersion(&major) != wxOS_WINDOWS_NT || major < 6)
        return;

    //We are running Windows Vista or newer
    CHECK(!m_button->GetAuthNeeded());

    m_button->SetAuthNeeded();

    CHECK(m_button->GetAuthNeeded());

    //We test both states
    m_button->SetAuthNeeded(false);

    CHECK(!m_button->GetAuthNeeded());
}

TEST_CASE_METHOD(ButtonTestCase, "Button::BitmapMargins", "[button]")
{
    //Some functions only work on specific platforms in which case we can use
    //a preprocessor check
#ifdef __WXMSW__
    //We must set a bitmap before we can set its margins, when writing unit
    //tests it is easiest to use an image from wxArtProvider
    m_button->SetBitmap(wxArtProvider::GetIcon(wxART_INFORMATION, wxART_OTHER,
                                               wxSize(32, 32)));

    m_button->SetBitmapMargins(15, 15);

    CHECK( m_button->GetBitmapMargins() == wxSize(15, 15) );

    m_button->SetBitmapMargins(wxSize(20, 20));

    CHECK( m_button->GetBitmapMargins() == wxSize(20, 20) );
#endif
}

TEST_CASE_METHOD(ButtonTestCase, "Button::Bitmap", "[button]")
{
    //We start with no bitmaps
    CHECK(!m_button->GetBitmap().IsOk());

    // Some bitmap, doesn't really matter which.
    const wxBitmap bmp = wxArtProvider::GetBitmap(wxART_INFORMATION);

    m_button->SetBitmap(bmp);

    CHECK(m_button->GetBitmap().IsOk());

    // The call above shouldn't affect any other bitmaps as returned by the API
    // even though the same (normal) bitmap does appear for all the states.
    CHECK( !m_button->GetBitmapCurrent().IsOk() );
    CHECK( !m_button->GetBitmapDisabled().IsOk() );
    CHECK( !m_button->GetBitmapFocus().IsOk() );
    CHECK( !m_button->GetBitmapPressed().IsOk() );

    // Do set one of the bitmaps now.
    m_button->SetBitmapPressed(wxArtProvider::GetBitmap(wxART_ERROR));
    CHECK( m_button->GetBitmapPressed().IsOk() );

    // Check that resetting the button label doesn't result in problems when
    // updating the bitmap later, as it used to be the case in wxGTK (#18898).
    m_button->SetLabel(wxString());
    CHECK_NOTHROW( m_button->Disable() );

    // Also check that setting an invalid bitmap doesn't do anything untoward,
    // such as crashing, as it used to do in wxOSX (#19257).
    CHECK_NOTHROW( m_button->SetBitmapPressed(wxNullBitmap) );
    CHECK( !m_button->GetBitmapPressed().IsOk() );
}

#endif //wxUSE_BUTTON
