///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/bitmaptogglebuttontest.cpp
// Purpose:     wxBitmapToggleButton unit test
// Author:      Steven Lamerton
// Created:     2010-07-17
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_TOGGLEBTN


#include "wx/tglbtn.h"

#ifdef wxHAS_BITMAPTOGGLEBUTTON

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "testableframe.h"
#include "wx/uiaction.h"
#include "wx/artprov.h"

class BitmapToggleButtonTestCase : public CppUnit::TestCase
{
public:
    BitmapToggleButtonTestCase() { }

    void setUp() wxOVERRIDE;
    void tearDown() wxOVERRIDE;

private:
    CPPUNIT_TEST_SUITE( BitmapToggleButtonTestCase );
        WXUISIM_TEST( Click );
        CPPUNIT_TEST( Value );
    CPPUNIT_TEST_SUITE_END();

    void Click();
    void Value();

    wxBitmapToggleButton* m_button;

    wxDECLARE_NO_COPY_CLASS(BitmapToggleButtonTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( BitmapToggleButtonTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( BitmapToggleButtonTestCase,
                                      "BitmapToggleButtonTestCase" );

void BitmapToggleButtonTestCase::setUp()
{
    m_button = new wxBitmapToggleButton(wxTheApp->GetTopWindow(), wxID_ANY,
                                        wxArtProvider::GetIcon(wxART_INFORMATION,
                                                               wxART_OTHER,
                                                               wxSize(32, 32)));
    m_button->Update();
    m_button->Refresh();
}

void BitmapToggleButtonTestCase::tearDown()
{
    wxDELETE(m_button);
}

void BitmapToggleButtonTestCase::Click()
{
#if wxUSE_UIACTIONSIMULATOR
    EventCounter clicked(m_button, wxEVT_TOGGLEBUTTON);

    wxUIActionSimulator sim;

    //We move in slightly to account for window decorations
    sim.MouseMove(m_button->GetScreenPosition() + wxPoint(10, 10));
    wxYield();

    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, clicked.GetCount());
    CPPUNIT_ASSERT(m_button->GetValue());

    clicked.Clear();

#ifdef __WXMSW__
    wxMilliSleep(1000);
#endif

    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, clicked.GetCount());
    CPPUNIT_ASSERT(!m_button->GetValue());
#endif // wxUSE_UIACTIONSIMULATOR
}

void BitmapToggleButtonTestCase::Value()
{
    EventCounter clicked(m_button, wxEVT_BUTTON);

    m_button->SetValue(true);

    CPPUNIT_ASSERT(m_button->GetValue());

    m_button->SetValue(false);

    CPPUNIT_ASSERT(!m_button->GetValue());

    CPPUNIT_ASSERT_EQUAL( 0, clicked.GetCount() );
}

#endif // wxHAS_BITMAPTOGGLEBUTTON

#endif // wxUSE_TOGGLEBTN
