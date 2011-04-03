///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/buttontest.cpp
// Purpose:     wxButton unit test
// Author:      Steven Lamerton
// Created:     2010-06-21
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_BUTTON

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/button.h"
#endif // WX_PRECOMP

#include "testableframe.h"
#include "wx/uiaction.h"
#include "wx/artprov.h"
//For CPPUNIT_ASSERT_EQUAL to work a class must have a stream output function
//for those classes which do not have them by default we define them in
//asserthelper.h so they can be reused
#include "asserthelper.h"

class ButtonTestCase : public CppUnit::TestCase
{
public:
    ButtonTestCase() { }

    void setUp();
    void tearDown();

private:
    CPPUNIT_TEST_SUITE( ButtonTestCase );
        //We add tests that use wxUIActionSimulator with WXUISIM_TEST so they
        //are not run on platofrms were wxUIActionSimulator isn't supported
        WXUISIM_TEST( Click );
        WXUISIM_TEST( Disabled );
        CPPUNIT_TEST( Auth );
        CPPUNIT_TEST( BitmapMargins );
        CPPUNIT_TEST( Bitmap );
    CPPUNIT_TEST_SUITE_END();

    void Click();
    void Disabled();
    void Auth();
    void BitmapMargins();
    void Bitmap();

    wxButton* m_button;

    DECLARE_NO_COPY_CLASS(ButtonTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ButtonTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ButtonTestCase, "ButtonTestCase" );

void ButtonTestCase::setUp()
{
    //We use wxTheApp->GetTopWindow() as there is only a single testable frame
    //so it will always be returned
    m_button = new wxButton(wxTheApp->GetTopWindow(), wxID_ANY, "wxButton");
}

void ButtonTestCase::tearDown()
{
    wxDELETE(m_button);
}

#if wxUSE_UIACTIONSIMULATOR

void ButtonTestCase::Click()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    //We use the internal class EventCounter which handles connecting and
    //disconnecting the control to the wxTestableFrame
    EventCounter count(m_button, wxEVT_COMMAND_BUTTON_CLICKED);

    wxUIActionSimulator sim;

    //We move in slightly to account for window decorations, we need to yield
    //after every wxUIActionSimulator action to keep everything working in GTK
    sim.MouseMove(m_button->GetScreenPosition() + wxPoint(10, 10));
    wxYield();

    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL( 1, frame->GetEventCount() );
}

void ButtonTestCase::Disabled()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_button, wxEVT_COMMAND_BUTTON_CLICKED);

    wxUIActionSimulator sim;

    //In this test we disable the button and check events are not sent
    m_button->Disable();

    sim.MouseMove(m_button->GetScreenPosition() + wxPoint(10, 10));
    wxYield();

    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL( 0, frame->GetEventCount() );
}

#endif // wxUSE_UIACTIONSIMULATOR

void ButtonTestCase::Auth()
{
    //Some functions only work on specific operating system versions, for
    //this we need a runtime check
    int major = 0;

    if(wxGetOsVersion(&major) != wxOS_WINDOWS_NT || major < 6)
        return;

    //We are running Windows Vista or newer
    CPPUNIT_ASSERT(!m_button->GetAuthNeeded());

    m_button->SetAuthNeeded();

    CPPUNIT_ASSERT(m_button->GetAuthNeeded());

    //We test both states
    m_button->SetAuthNeeded(false);

    CPPUNIT_ASSERT(!m_button->GetAuthNeeded());
}

void ButtonTestCase::BitmapMargins()
{
    //Some functions only work on specific platforms in which case we can use
    //a preprocessor check
#ifdef __WXMSW__
    //We must set a bitmap before we can set its margins, when writing unit
    //tests it is easiest to use an image from wxArtProvider
    m_button->SetBitmap(wxArtProvider::GetIcon(wxART_INFORMATION, wxART_OTHER,
                                               wxSize(32, 32)));

    m_button->SetBitmapMargins(15, 15);

    CPPUNIT_ASSERT_EQUAL(wxSize(15, 15), m_button->GetBitmapMargins());

    m_button->SetBitmapMargins(wxSize(20, 20));

    CPPUNIT_ASSERT_EQUAL(wxSize(20, 20), m_button->GetBitmapMargins());
#endif
}

void ButtonTestCase::Bitmap()
{
    //We start with no bitmaps
    CPPUNIT_ASSERT(!m_button->GetBitmap().IsOk());


    m_button->SetBitmap(wxArtProvider::GetIcon(wxART_INFORMATION, 
                                               wxART_OTHER,
                                               wxSize(32, 32)));

    CPPUNIT_ASSERT(m_button->GetBitmap().IsOk());
}

#endif //wxUSE_BUTTON
