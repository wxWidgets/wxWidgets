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

class HyperlinkCtrlTestCase
{
public:
    HyperlinkCtrlTestCase()
    {
        m_hyperlink = new wxHyperlinkCtrl(wxTheApp->GetTopWindow(), wxID_ANY,
                                         "wxWidgets", "http://wxwidgets.org");
    }

    ~HyperlinkCtrlTestCase()
    {
        delete m_hyperlink;
    }

protected:
    wxHyperlinkCtrl* m_hyperlink;

    wxDECLARE_NO_COPY_CLASS(HyperlinkCtrlTestCase);
};

TEST_CASE_METHOD(HyperlinkCtrlTestCase, "wxHyperlinkCtrl::Colour",
                 "[hyperlinkctrl]")
{
#ifndef __WXGTK__
    CHECK(m_hyperlink->GetHoverColour().IsOk());
    CHECK(m_hyperlink->GetNormalColour().IsOk());
    CHECK(m_hyperlink->GetVisitedColour().IsOk());

    // Changing hover colour doesn't work in wxMSW and Wine doesn't seem to
    // implement either LM_SETITEM or LM_GETITEM correctly, so skip this there.
#ifdef __WXMSW__
    if ( wxIsRunningUnderWine() )
    {
        WARN("Skipping testing wxHyperlinkCtrl colours under Wine.");
        return;
    }
#else // __WXMSW__
    m_hyperlink->SetHoverColour(*wxGREEN);
    CHECK( m_hyperlink->GetHoverColour() == *wxGREEN );
#endif // __WXMSW__/!__WXMSW__

    m_hyperlink->SetNormalColour(*wxRED);
    CHECK( m_hyperlink->GetNormalColour() == *wxRED );

    m_hyperlink->SetVisitedColour(*wxBLUE);
    CHECK( m_hyperlink->GetVisitedColour() == *wxBLUE );
#endif
}

TEST_CASE_METHOD(HyperlinkCtrlTestCase, "wxHyperlinkCtrl::Url",
                 "[hyperlinkctrl]")
{
    CHECK( m_hyperlink->GetURL() == "http://wxwidgets.org" );

    m_hyperlink->SetURL("http://google.com");

    CHECK( m_hyperlink->GetURL() == "http://google.com" );
}

TEST_CASE_METHOD(HyperlinkCtrlTestCase, "wxHyperlinkCtrl::Click",
                 "[hyperlinkctrl]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;

    EventCounter hyperlink(m_hyperlink, wxEVT_HYPERLINK);

    wxUIActionSimulator sim;

    sim.MouseMove(m_hyperlink->GetScreenPosition() + wxPoint(10, 10));
    wxYield();

    sim.MouseClick();
    wxYield();

    CHECK( hyperlink.GetCount() == 1 );
#endif
}

#endif //wxUSE_HYPERLINKCTRL
