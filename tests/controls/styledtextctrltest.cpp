///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/styledtextctrltest.cpp
// Purpose:     wxStyledTextCtrl unit test
// Author:      New Pagodi
// Created:     2019-03-10
// Copyright:   (c) 2019 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_STC

#if defined(WXUSINGDLL)

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/stc/stc.h"
#include "wx/uiaction.h"

#include "testwindow.h"

#if defined(__WXOSX_COCOA__) || defined(__WXMSW__) || defined(__WXGTK__)

class StcPopupWindowsTestCase
{
public:
    StcPopupWindowsTestCase()
        : m_stc(new wxStyledTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY))
    {
        m_focusAlwaysRetained=true;
        m_calltipClickReceived=false;

        m_stc->Bind(wxEVT_KILL_FOCUS,
                    &StcPopupWindowsTestCase::OnKillSTCFocus, this);
        m_stc->Bind(wxEVT_STC_CALLTIP_CLICK,
                    &StcPopupWindowsTestCase::OnCallTipClick, this);
    }

    ~StcPopupWindowsTestCase()
    {
        delete m_stc;
    }

    void OnKillSTCFocus(wxFocusEvent& WXUNUSED(event))
    {
        m_focusAlwaysRetained=false;
    }

    void OnCallTipClick(wxStyledTextEvent& WXUNUSED(event))
    {
        m_calltipClickReceived=true;
    }

protected:
    wxStyledTextCtrl* const m_stc;
    bool m_focusAlwaysRetained;
    bool m_calltipClickReceived;
};

// This set of tests is used to verify that an autocompletion popup does not
// take focus from its parent styled text control.
TEST_CASE_METHOD(StcPopupWindowsTestCase,
                 "wxStyledTextCtrl::AutoComp",
                 "[wxStyledTextCtrl][focus]")
{
    m_stc->SetFocus();
    m_focusAlwaysRetained = true;
    m_stc->AutoCompShow(0,"ability able about above abroad absence absent");

#if wxUSE_UIACTIONSIMULATOR
    // Pressing the tab key should cause the current entry in the list to be
    // entered into the styled text control. However with GTK+, characters sent
    // with the UI simulator seem to arrive too late, so select the current
    // entry with a double click instead.

    wxUIActionSimulator sim;

#ifdef __WXGTK__
    wxPoint zeroPosition = m_stc->PointFromPosition(0);
    int textHt = m_stc->TextHeight(0);
    int textWd = m_stc->TextWidth(0,"ability");
    wxPoint autoCompPoint(zeroPosition.x + textWd/2,
                            zeroPosition.y + textHt + textHt/2);
    wxPoint scrnPoint = m_stc->ClientToScreen(autoCompPoint);
    sim.MouseMove(scrnPoint);
    sim.MouseDblClick();
#else
    sim.Char(WXK_TAB);
#endif // __WXGTK__
    ::wxYield();
    CHECK( m_stc->GetText() == "ability" );
#endif //wxUSE_UIACTIONSIMULATOR

    if ( m_stc->AutoCompActive() )
        m_stc->AutoCompCancel();

    CHECK_FOCUS_IS( m_stc );

    // Unfortunately under GTK we do get focus loss events, at least sometimes
    // (and actually more often than not, especially with GTK2, but this
    // happens with GTK3 too).
#ifndef __WXGTK__
    CHECK( m_focusAlwaysRetained );
#endif // !__WXGTK__
}

// This test is used to verify that a call tip receives mouse clicks. However
// the clicks do sent with the UI simulator do not seem to be received on 
// cocoa for some reason, so skip the test there for now.
#if !defined(__WXOSX_COCOA__)
TEST_CASE_METHOD(StcPopupWindowsTestCase,
                 "wxStyledTextCtrl::Calltip",
                 "[wxStyledTextCtrl][focus]")
{
    m_stc->SetFocus();
    m_calltipClickReceived = false;
    m_focusAlwaysRetained = true;

    wxString calltipText = "This is a calltip.";
    m_stc->CallTipShow(0,calltipText);

#if wxUSE_UIACTIONSIMULATOR
    wxUIActionSimulator sim;
    wxPoint zeroPosition = m_stc->PointFromPosition(0);
    int textHt = m_stc->TextHeight(0);
    int textWd = m_stc->TextWidth(0,calltipText);

    // zeroPosition is the top left of position 0 and the call tip should have
    // roughly the same height as textHt (there seems to be some extra padding
    // that makes it a little taller, but it's roughly the same height),
    // so (zeroPosition.x+textWd/2,zeroPosition.y+textHt+textHt/2) should
    // be the middle of the calltip.
    wxPoint calltipMidPoint(zeroPosition.x + textWd/2,
                            zeroPosition.y + textHt + textHt/2);
    wxPoint scrnPoint = m_stc->ClientToScreen(calltipMidPoint);
    sim.MouseMove(scrnPoint);
    sim.MouseClick();
    ::wxYield();

    CHECK( m_calltipClickReceived );
#endif // wxUSE_UIACTIONSIMULATOR

    if ( m_stc->CallTipActive() )
        m_stc->CallTipCancel();

    // Verify that clicking the call tip did not take focus from the STC.
    //
    // Unfortunately this test fails for unknown reasons under Xvfb (but only
    // there).
    if ( !IsRunningUnderXVFB() )
        CHECK_FOCUS_IS( m_stc );

    // With wxGTK there is the same problem here as in the test above.
#ifndef __WXGTK__
    CHECK( m_focusAlwaysRetained );
#endif // !__WXGTK__
}

#endif // !defined(__WXOSX_COCOA__)

#endif // defined(__WXOSX_COCOA__) || defined(__WXMSW__) || defined(__WXGTK__)

#endif // WXUSINGDLL

#endif // wxUSE_STC

