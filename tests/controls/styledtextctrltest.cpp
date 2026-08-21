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

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/stc/stc.h"
#include "wx/uiaction.h"

#if defined(__WXOSX_COCOA__) || defined(__WXGTK__)
    #include "wx/private/textinput.h"
#endif

#ifdef __WXOSX_COCOA__
    #include <objc/message.h>
    #include <objc/runtime.h>
#endif

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

#if defined(__WXOSX_COCOA__) || defined(__WXGTK__)

class StcTextInputTestCase
{
public:
    StcTextInputTestCase()
        : m_stc(new wxStyledTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY))
    {
    }

    ~StcTextInputTestCase()
    {
        delete m_stc;
    }

protected:
    wxTextInputClient* GetTextInputClient() const
    {
        return wxFindTextInputClient(m_stc);
    }

    wxStyledTextCtrl* const m_stc;
};

#ifdef __WXGTK__

TEST_CASE_METHOD(StcTextInputTestCase,
                 "wxStyledTextCtrl::GTKTextInput",
                 "[wxStyledTextCtrl][ime]")
{
    wxTextInputClient* const client = GetTextInputClient();
    REQUIRE( client );
    CHECK( client->IsTextInputEnabled() );

    const wxString hiragana =
        wxString::FromUTF8("\xe3\x81\xab");
    const wxString kanji =
        wxString::FromUTF8("\xe6\x97\xa5\xe6\x9c\xac");

    m_stc->SetText("AB");
    m_stc->EmptyUndoBuffer();

    m_stc->SetSelection(0, 2);
    REQUIRE( client->UpdateComposition("", 0) );
    CHECK( m_stc->GetText() == "AB" );
    CHECK( m_stc->GetSelectionStart() == 0 );
    CHECK( m_stc->GetSelectionEnd() == 2 );
    CHECK_FALSE( client->HasActiveComposition() );
    CHECK_FALSE( client->CommitComposition(kanji) );

    m_stc->SetEmptySelection(1);

    REQUIRE( client->UpdateComposition(hiragana, 1) );
    CHECK( m_stc->GetText() == "A" + hiragana + "B" );

    client->CancelComposition();
    CHECK( m_stc->GetText() == "AB" );

    REQUIRE( client->UpdateComposition(hiragana, 1) );
    REQUIRE( client->CommitComposition(kanji) );
    CHECK( m_stc->GetText() == "A" + kanji + "B" );
    CHECK_FALSE( client->CommitComposition(kanji) );

    m_stc->Undo();
    CHECK( m_stc->GetText() == "AB" );

    m_stc->SetEmptySelection(1);
    REQUIRE( client->UpdateComposition(hiragana, 1) );
    m_stc->SetIMEInteraction(wxSTC_IME_WINDOWED);
    CHECK( m_stc->GetText() == "AB" );
    CHECK_FALSE( client->IsTextInputEnabled() );
    m_stc->SetIMEInteraction(wxSTC_IME_INLINE);
    CHECK( client->IsTextInputEnabled() );
}

#endif // __WXGTK__

#ifdef __WXOSX_COCOA__

TEST_CASE_METHOD(StcTextInputTestCase,
                 "wxStyledTextCtrl::CocoaTextInput",
                 "[wxStyledTextCtrl][ime]")
{
    wxTextInputClient* const client = GetTextInputClient();
    REQUIRE( client );
    CHECK( client->IsTextInputEnabled() );

    const wxString hiragana =
        wxString::FromUTF8("\xe3\x81\xab");
    const wxString kanji =
        wxString::FromUTF8("\xe6\x97\xa5\xe6\x9c\xac");
    const wxString original =
        wxString::FromUTF8("A\xf0\x9f\x98\x80" "B");

    m_stc->SetText(original);
    m_stc->EmptyUndoBuffer();
    m_stc->SetSelection(1, 5);

    REQUIRE( client->SetMarkedText(
        hiragana, 1, 0, wxTextInputClient::NoPosition, 0) );
    CHECK( m_stc->GetText() == "A" + hiragana + "B" );
    CHECK( client->HasMarkedText() );

    long start, length;
    REQUIRE( client->GetMarkedTextRange(&start, &length) );
    CHECK( start == 1 );
    CHECK( length == 1 );
    REQUIRE( client->GetSelectedTextRange(&start, &length) );
    CHECK( start == 2 );
    CHECK( length == 0 );

    REQUIRE( client->SetMarkedText(
        kanji, 2, 0, wxTextInputClient::NoPosition, 0) );
    CHECK( m_stc->GetText() == "A" + kanji + "B" );
    REQUIRE( client->InsertText(
        kanji, wxTextInputClient::NoPosition, 0) );
    CHECK_FALSE( client->HasMarkedText() );
    CHECK( m_stc->GetText() == "A" + kanji + "B" );

    m_stc->Undo();
    CHECK( m_stc->GetText() == "AB" );
    m_stc->Undo();
    CHECK( m_stc->GetText() == original );

    // After choosing a candidate, some input methods send insertNewline: to
    // accept it instead of calling insertText:. It must commit the marked
    // text without inserting a newline.
    m_stc->SetSelection(1, 5);
    REQUIRE( client->SetMarkedText(
        kanji, 2, 0, wxTextInputClient::NoPosition, 0) );
    using SendSelector = void (*)(void*, SEL, SEL);
    reinterpret_cast<SendSelector>(objc_msgSend)(
        m_stc->GetHandle(),
        sel_registerName("doCommandBySelector:"),
        sel_registerName("insertNewline:"));
    CHECK_FALSE( client->HasMarkedText() );
    CHECK( m_stc->GetText() == "A" + kanji + "B" );

    m_stc->Undo();
    CHECK( m_stc->GetText() == "AB" );
    m_stc->Undo();
    CHECK( m_stc->GetText() == original );

    wxString text;
    long actualStart, actualLength;
    REQUIRE( client->GetTextInRange(0, 4, &text,
                                    &actualStart, &actualLength) );
    CHECK( text == original );
    CHECK( actualStart == 0 );
    CHECK( actualLength == 4 );

    REQUIRE( client->InsertText("X", 1, 2) );
    CHECK( m_stc->GetText() == "AXB" );
    REQUIRE( client->InsertText(
        "ignored", wxTextInputClient::InvalidPosition, 0) );
    CHECK( m_stc->GetText() == "AXB" );

    m_stc->SetEmptySelection(m_stc->GetTextLength());
    REQUIRE( client->SetMarkedText(
        hiragana, 1, 0, wxTextInputClient::NoPosition, 0) );
    CHECK( client->HasMarkedText() );
    m_stc->SetIMEInteraction(wxSTC_IME_WINDOWED);
    CHECK( m_stc->GetText() == "AXB" );
    CHECK_FALSE( client->HasMarkedText() );
    CHECK_FALSE( client->IsTextInputEnabled() );
    m_stc->SetIMEInteraction(wxSTC_IME_INLINE);
    CHECK( client->IsTextInputEnabled() );
}

#endif // __WXOSX_COCOA__

#endif // __WXOSX_COCOA__ || __WXGTK__

#endif // defined(__WXOSX_COCOA__) || defined(__WXMSW__) || defined(__WXGTK__)

#endif // wxUSE_STC

