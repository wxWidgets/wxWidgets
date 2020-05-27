///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/radiobuttontest.cpp
// Purpose:     wxRadioButton unit test
// Author:      Steven Lamerton
// Created:     2010-07-30
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_RADIOBTN

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/button.h"
    #include "wx/panel.h"
    #include "wx/radiobut.h"
    #include "wx/sizer.h"
    #include "wx/stattext.h"
#endif // WX_PRECOMP

#include "wx/uiaction.h"
#include "testableframe.h"
#include "testwindow.h"

class RadioButtonTestCase : public CppUnit::TestCase
{
public:
    RadioButtonTestCase() { }

    void setUp() wxOVERRIDE;
    void tearDown() wxOVERRIDE;

private:
    CPPUNIT_TEST_SUITE( RadioButtonTestCase );
        WXUISIM_TEST( Click );
        CPPUNIT_TEST( Value );
        CPPUNIT_TEST( Group );
        CPPUNIT_TEST( Single );
    CPPUNIT_TEST_SUITE_END();

    void Click();
    void Value();
    void Group();
    void Single();

    wxRadioButton* m_radio;

    wxDECLARE_NO_COPY_CLASS(RadioButtonTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( RadioButtonTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( RadioButtonTestCase,
                                      "RadioButtonTestCase" );

void RadioButtonTestCase::setUp()
{
    m_radio = new wxRadioButton(wxTheApp->GetTopWindow(), wxID_ANY,
                                "wxRadioButton");
    m_radio->Update();
    m_radio->Refresh();
}

void RadioButtonTestCase::tearDown()
{
    wxDELETE(m_radio);
}

void RadioButtonTestCase::Click()
{
    // OS X doesn't support selecting a single radio button
#if wxUSE_UIACTIONSIMULATOR && !defined(__WXOSX__)
    EventCounter selected(m_radio, wxEVT_RADIOBUTTON);

    wxUIActionSimulator sim;
    wxYield();

    sim.MouseMove(m_radio->GetScreenPosition() + wxPoint(10, 10));
    sim.MouseClick();

    wxYield();

    CPPUNIT_ASSERT_EQUAL( 1, selected.GetCount() );
#endif
}

void RadioButtonTestCase::Value()
{
#ifndef __WXGTK__
    EventCounter selected(m_radio, wxEVT_RADIOBUTTON);

    m_radio->SetValue(true);

    CPPUNIT_ASSERT(m_radio->GetValue());

    m_radio->SetValue(false);

    CPPUNIT_ASSERT(!m_radio->GetValue());

    CPPUNIT_ASSERT_EQUAL(0, selected.GetCount());
#endif
}

void RadioButtonTestCase::Group()
{
    wxWindow* const parent = wxTheApp->GetTopWindow();

    // Create two different radio groups.
    wxRadioButton* g1radio0 = new wxRadioButton(parent, wxID_ANY, "radio 1.0",
                                                wxDefaultPosition, wxDefaultSize,
                                                wxRB_GROUP);

    wxRadioButton* g1radio1 = new wxRadioButton(parent, wxID_ANY, "radio 1.1");

    wxRadioButton* g2radio0 = new wxRadioButton(parent, wxID_ANY, "radio 2.0",
                                                wxDefaultPosition, wxDefaultSize,
                                                wxRB_GROUP);

    wxRadioButton* g2radio1 = new wxRadioButton(parent, wxID_ANY, "radio 2.1");

    // Check that having another control between radio buttons doesn't break
    // grouping.
    wxStaticText* text = new wxStaticText(parent, wxID_ANY, "Label");
    wxRadioButton* g2radio2 = new wxRadioButton(parent, wxID_ANY, "radio 2.1");

    g1radio0->SetValue(true);
    g2radio0->SetValue(true);

    CPPUNIT_ASSERT(g1radio0->GetValue());
    CPPUNIT_ASSERT(!g1radio1->GetValue());
    CPPUNIT_ASSERT(g2radio0->GetValue());
    CPPUNIT_ASSERT(!g2radio1->GetValue());

    g1radio1->SetValue(true);
    g2radio1->SetValue(true);

    CPPUNIT_ASSERT(!g1radio0->GetValue());
    CPPUNIT_ASSERT(g1radio1->GetValue());
    CPPUNIT_ASSERT(!g2radio0->GetValue());
    CPPUNIT_ASSERT(g2radio1->GetValue());

    g2radio2->SetValue(true);
    CPPUNIT_ASSERT(!g2radio0->GetValue());
    CPPUNIT_ASSERT(!g2radio1->GetValue());
    CPPUNIT_ASSERT(g2radio2->GetValue());

    g1radio0->SetValue(true);
    g2radio0->SetValue(true);

    CPPUNIT_ASSERT(g1radio0->GetValue());
    CPPUNIT_ASSERT(!g1radio1->GetValue());
    CPPUNIT_ASSERT(g2radio0->GetValue());
    CPPUNIT_ASSERT(!g2radio1->GetValue());

    wxDELETE(g1radio0);
    wxDELETE(g1radio1);
    wxDELETE(g2radio0);
    wxDELETE(g2radio1);
    wxDELETE(g2radio2);
    wxDELETE(text);
}

void RadioButtonTestCase::Single()
{
    //Create a group of 2 buttons, having second button selected
    wxScopedPtr<wxRadioButton> gradio0(new wxRadioButton(wxTheApp->GetTopWindow(),
        wxID_ANY, "wxRadioButton",
        wxDefaultPosition,
        wxDefaultSize, wxRB_GROUP));

    wxScopedPtr<wxRadioButton> gradio1(new wxRadioButton(wxTheApp->GetTopWindow(),
        wxID_ANY, "wxRadioButton"));

    gradio1->SetValue(true);

    //Create a "single" button (by default it will not be selected)
    wxScopedPtr<wxRadioButton> sradio(new wxRadioButton(wxTheApp->GetTopWindow(),
        wxID_ANY, "wxRadioButton",
        wxDefaultPosition,
        wxDefaultSize, wxRB_SINGLE));

    //Create a non-grouped button and select it
    wxScopedPtr<wxRadioButton> ngradio(new wxRadioButton(wxTheApp->GetTopWindow(),
        wxID_ANY, "wxRadioButton"));

    ngradio->SetValue(true);

    //Select the "single" button
    sradio->SetValue(true);

    CHECK(gradio1->GetValue());
    CHECK(ngradio->GetValue());
}

TEST_CASE("wxRadioButton::Focus", "[radiobutton][focus]")
{
    // Create a container panel just to be able to destroy all the windows
    // created here at once by simply destroying it.
    wxWindow* const tlw = wxTheApp->GetTopWindow();
    wxScopedPtr<wxPanel> parentPanel(new wxPanel(tlw));

    // Create a panel containing 2 radio buttons and another control outside
    // this panel, so that we could give focus to something different and then
    // return it back to the panel.
    wxPanel* const radioPanel = new wxPanel(parentPanel.get());
    wxRadioButton* const radio1 = new wxRadioButton(radioPanel, wxID_ANY, "1");
    wxRadioButton* const radio2 = new wxRadioButton(radioPanel, wxID_ANY, "2");
    wxSizer* const radioSizer = new wxBoxSizer(wxHORIZONTAL);
    radioSizer->Add(radio1);
    radioSizer->Add(radio2);
    radioPanel->SetSizer(radioSizer);

    wxButton* const dummyButton = new wxButton(parentPanel.get(), wxID_OK);

    wxSizer* const sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(radioPanel, wxSizerFlags(1).Expand());
    sizer->Add(dummyButton, wxSizerFlags().Expand());
    parentPanel->SetSizer(sizer);

    parentPanel->SetSize(tlw->GetClientSize());
    parentPanel->Layout();

    // Initially the first radio button should be checked.
    radio1->SetFocus();
    CHECK(radio1->GetValue());
    CHECK_FOCUS_IS(radio1);

    // Switching focus from it shouldn't change this.
    dummyButton->SetFocus();
    CHECK(radio1->GetValue());

    // Checking another radio button should make it checked and uncheck the
    // first one.
    radio2->SetValue(true);
    CHECK(!radio1->GetValue());
    CHECK(radio2->GetValue());

    // While not changing focus.
    CHECK_FOCUS_IS(dummyButton);

    // And giving the focus to the panel shouldn't change radio button
    // selection.
    radioPanel->SetFocus();

    // Under MSW, focus is always on the selected button, but in the other
    // ports this is not necessarily the case, i.e. under wxGTK this check
    // would fail because focus gets set to the first button -- even though the
    // second one remains checked.
#ifdef __WXMSW__
    CHECK_FOCUS_IS(radio2);
#endif

    CHECK(!radio1->GetValue());
    CHECK(radio2->GetValue());
}

#endif //wxUSE_RADIOBTN
