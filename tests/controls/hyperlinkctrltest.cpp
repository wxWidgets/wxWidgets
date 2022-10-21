///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/hyperlinkctrltest.cpp
// Purpose:     wxHyperlinkCtrl unit test
// Author:      Steven Lamerton
// Created:     2010-08-05
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_HYPERLINKCTRL


#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/hyperlink.h"
#include "wx/uiaction.h"
#include "testableframe.h"
#include "asserthelper.h"

class HyperlinkCtrlTestCase : public CppUnit::TestCase
{
public:
    HyperlinkCtrlTestCase() { }

    void setUp() override;
    void tearDown() override;

private:
    CPPUNIT_TEST_SUITE( HyperlinkCtrlTestCase );
        CPPUNIT_TEST( Colour );
        CPPUNIT_TEST( Url );
        WXUISIM_TEST( Click );
    CPPUNIT_TEST_SUITE_END();

    void Colour();
    void Url();
    void Click();

    wxHyperlinkCtrl* m_hyperlink;

    wxDECLARE_NO_COPY_CLASS(HyperlinkCtrlTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( HyperlinkCtrlTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( HyperlinkCtrlTestCase, "HyperlinkCtrlTestCase" );

void HyperlinkCtrlTestCase::setUp()
{
    m_hyperlink = new wxHyperlinkCtrl(wxTheApp->GetTopWindow(), wxID_ANY,
                                     "wxWidgets", "http://wxwidgets.org");
}

void HyperlinkCtrlTestCase::tearDown()
{
    wxDELETE(m_hyperlink);
}

void HyperlinkCtrlTestCase::Colour()
{
#ifndef __WXGTK__
    CPPUNIT_ASSERT(m_hyperlink->GetHoverColour().IsOk());
    CPPUNIT_ASSERT(m_hyperlink->GetNormalColour().IsOk());
    CPPUNIT_ASSERT(m_hyperlink->GetVisitedColour().IsOk());

    m_hyperlink->SetHoverColour(*wxGREEN);
    m_hyperlink->SetNormalColour(*wxRED);
    m_hyperlink->SetVisitedColour(*wxBLUE);

    CPPUNIT_ASSERT_EQUAL(*wxGREEN, m_hyperlink->GetHoverColour());
    CPPUNIT_ASSERT_EQUAL(*wxRED, m_hyperlink->GetNormalColour());
    CPPUNIT_ASSERT_EQUAL(*wxBLUE, m_hyperlink->GetVisitedColour());
#endif
}

void HyperlinkCtrlTestCase::Url()
{
    CPPUNIT_ASSERT_EQUAL("http://wxwidgets.org", m_hyperlink->GetURL());

    m_hyperlink->SetURL("http://google.com");

    CPPUNIT_ASSERT_EQUAL("http://google.com", m_hyperlink->GetURL());
}

void HyperlinkCtrlTestCase::Click()
{
#if wxUSE_UIACTIONSIMULATOR
    EventCounter hyperlink(m_hyperlink, wxEVT_HYPERLINK);

    wxUIActionSimulator sim;

    sim.MouseMove(m_hyperlink->GetScreenPosition() + wxPoint(10, 10));
    wxYield();

    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, hyperlink.GetCount());
#endif
}

#endif //wxUSE_HYPERLINKCTRL
