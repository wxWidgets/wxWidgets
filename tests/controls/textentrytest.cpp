///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/textentrytest.cpp
// Purpose:     TestEntryTestCase implementation
// Author:      Vadim Zeitlin
// Created:     2008-09-19 (extracted from textctrltest.cpp)
// Copyright:   (c) 2007, 2008 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/dialog.h"
    #include "wx/event.h"
    #include "wx/sizer.h"
    #include "wx/textctrl.h"
    #include "wx/textentry.h"
    #include "wx/timer.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "textentrytest.h"
#include "testableframe.h"

#include "wx/uiaction.h"

#include <memory>

void TextEntryTestCase::SetValue()
{
    wxTextEntry * const entry = GetTestEntry();

    CHECK( entry->IsEmpty() );

    entry->SetValue("foo");
    CHECK( entry->GetValue() == "foo" );

    entry->SetValue("");
    CHECK( entry->IsEmpty() );

    entry->SetValue("hi");
    CHECK( entry->GetValue() == "hi" );

    entry->SetValue("bye");
    CHECK( entry->GetValue() == "bye" );
}

void TextEntryTestCase::TextChangeEvents()
{
    EventCounter updated(GetTestWindow(), wxEVT_TEXT);

    wxTextEntry * const entry = GetTestEntry();

    // notice that SetValue() generates an event even if the text didn't change
    entry->SetValue("");
    CHECK( updated.GetCount() == 1 );
    updated.Clear();

    entry->SetValue("foo");
    CHECK( updated.GetCount() == 1 );
    updated.Clear();

    entry->SetValue("foo");
    CHECK( updated.GetCount() == 1 );
    updated.Clear();

    entry->SetValue("");
    CHECK( updated.GetCount() == 1 );
    updated.Clear();

    entry->ChangeValue("bar");
    CHECK( updated.GetCount() == 0 );

    entry->AppendText("bar");
    CHECK( updated.GetCount() == 1 );
    updated.Clear();

    entry->Replace(3, 6, "baz");
    CHECK( updated.GetCount() == 1 );
    updated.Clear();

    entry->Remove(0, 3);
    CHECK( updated.GetCount() == 1 );
    updated.Clear();

    entry->WriteText("foo");
    CHECK( updated.GetCount() == 1 );
    updated.Clear();

    entry->Clear();
    CHECK( updated.GetCount() == 1 );
    updated.Clear();

    entry->ChangeValue("");
    CHECK( updated.GetCount() == 0 );
    updated.Clear();

    entry->ChangeValue("non-empty");
    CHECK( updated.GetCount() == 0 );
    updated.Clear();

    entry->ChangeValue("");
    CHECK( updated.GetCount() == 0 );
    updated.Clear();
}

void TextEntryTestCase::CheckStringSelection(const char *sel)
{
    CHECK( GetTestEntry()->GetStringSelection() == sel );
}

void TextEntryTestCase::AssertSelection(int from, int to, const char *sel)
{
    wxTextEntry * const entry = GetTestEntry();

    CHECK( entry->HasSelection() );

    long fromReal,
         toReal;
    entry->GetSelection(&fromReal, &toReal);
    CHECK( fromReal == from );
    CHECK( toReal == to );

    CHECK( entry->GetInsertionPoint() == from );

    CheckStringSelection(sel);
}

void TextEntryTestCase::Selection()
{
    wxTextEntry * const entry = GetTestEntry();

    entry->SetValue("0123456789");

    entry->SetSelection(2, 4);
    AssertSelection(2, 4, "23"); // not "234"!

    entry->SetSelection(3, -1);
    AssertSelection(3, 10, "3456789");

    entry->SelectAll();
    AssertSelection(0, 10, "0123456789");

    entry->SetSelection(0, 0);
    CHECK( !entry->HasSelection() );
}

void TextEntryTestCase::InsertionPoint()
{
    wxTextEntry * const entry = GetTestEntry();

    CHECK( entry->GetLastPosition() == 0 );
    CHECK( entry->GetInsertionPoint() == 0 );

    entry->SetValue("0"); // should put the insertion point in front
    CHECK( entry->GetLastPosition() == 1 );
    CHECK( entry->GetInsertionPoint() == 0 );

    entry->AppendText("12"); // should update the insertion point position
    CHECK( entry->GetLastPosition() == 3 );
    CHECK( entry->GetInsertionPoint() == 3 );

    entry->SetInsertionPoint(1);
    CHECK( entry->GetLastPosition() == 3 );
    CHECK( entry->GetInsertionPoint() == 1 );

    entry->SetValue("012"); // shouldn't change the position if no real change
    CHECK( entry->GetInsertionPoint() == 1 );

    entry->ChangeValue("012"); // same as for SetValue()
    CHECK( entry->GetInsertionPoint() == 1 );

    entry->SetInsertionPointEnd();
    CHECK( entry->GetInsertionPoint() == 3 );

    entry->SetInsertionPoint(0);
    entry->WriteText("-"); // should move it after the written text
    CHECK( entry->GetLastPosition() == 4 );
    CHECK( entry->GetInsertionPoint() == 1 );

    entry->SetValue("something different"); // should still reset the caret
    CHECK( entry->GetInsertionPoint() == 0 );
}

void TextEntryTestCase::Replace()
{
    wxTextEntry * const entry = GetTestEntry();

    entry->SetValue("Hello replace!"
                    "0123456789012");
    entry->SetInsertionPoint(0);

    entry->Replace(6, 13, "changed");

    CHECK(entry->GetValue() == "Hello changed!"
                         "0123456789012");
    CHECK(entry->GetInsertionPoint() == 13);

    entry->Replace(13, -1, "");
    CHECK(entry->GetValue() == "Hello changed");
    CHECK(entry->GetInsertionPoint() == 13);

    entry->Replace(0, 6, "Un");
    CHECK(entry->GetValue() == "Unchanged");
    CHECK(entry->GetInsertionPoint() == 2);
}

void TextEntryTestCase::WriteText()
{
    wxTextEntry * const entry = GetTestEntry();

    entry->SetValue("foo");
    entry->SetInsertionPoint(3);
    entry->WriteText("bar");
    CHECK( entry->GetValue() == "foobar" );

    entry->SetValue("foo");
    entry->SetInsertionPoint(0);
    entry->WriteText("bar");
    CHECK( entry->GetValue() == "barfoo" );

    entry->SetValue("abxxxhi");
    entry->SetSelection(2, 5);
    entry->WriteText("cdefg");
    CHECK( entry->GetValue() == "abcdefghi" );
    CHECK( entry->GetInsertionPoint() == 7 );
    CHECK( entry->HasSelection() == false );
}

#if wxUSE_UIACTIONSIMULATOR

class TextEventHandler
{
public:
    explicit TextEventHandler(wxWindow* win)
        : m_win(win)
    {
        m_win->Bind(wxEVT_TEXT, &TextEventHandler::OnText, this);
    }

    ~TextEventHandler()
    {
        m_win->Unbind(wxEVT_TEXT, &TextEventHandler::OnText, this);
    }

    const wxString& GetLastString() const
    {
        return m_string;
    }

private:
    void OnText(wxCommandEvent& event)
    {
        m_string = event.GetString();
    }

    wxWindow* const m_win;

    wxString m_string;
};

void TextEntryTestCase::Editable()
{
    if ( !EnableUITests() )
        return;

    wxTextEntry * const entry = GetTestEntry();
    wxWindow * const window = GetTestWindow();

    EventCounter updated(window, wxEVT_TEXT);

    window->SetFocus();
    wxYield();

#ifdef __WXGTK__
    // For some reason, wxBitmapComboBox doesn't appear on the screen without
    // this (due to wxTLW size hacks perhaps?). It would be nice to avoid doing
    // this, but without this hack the test often (although not always) fails.
    wxMilliSleep(50);
#endif // __WGTK__

    // Check that we get the expected number of events.
    wxUIActionSimulator sim;
    sim.Text("abcdef");
    wxYield();

    CHECK(entry->GetValue() == "abcdef");
    CHECK(updated.GetCount() == 6);

    wxYield();

    // And that the event carries the right value.
    TextEventHandler handler(window);

    sim.Text("g");
    wxYield();

    CHECK(handler.GetLastString() == "abcdefg");

    // ... even if we generate the event programmatically and whether it uses
    // the same value as the control has right now
    entry->SetValue("abcdefg");
    CHECK(handler.GetLastString() == "abcdefg");

    // ... or not
    entry->SetValue("abcdef");
    CHECK(handler.GetLastString() == "abcdef");

    // Check that making the control not editable does indeed prevent it from
    // being edited.
    updated.Clear();

    entry->SetEditable(false);
    sim.Text("gh");
    wxYield();

    CHECK(entry->GetValue() == "abcdef");
    CHECK(updated.GetCount() == 0);
}

#endif // wxUSE_UIACTIONSIMULATOR

void TextEntryTestCase::Hint()
{
    GetTestEntry()->SetHint("This is a hint");
    CHECK(GetTestEntry()->GetValue() == "");
}

void TextEntryTestCase::CopyPaste()
{
#ifndef __WXOSX__
    wxTextEntry * const entry = GetTestEntry();

    entry->AppendText("sometext");
    entry->SelectAll();

    if(entry->CanCopy() && entry->CanPaste())
    {
        entry->Copy();
        entry->Clear();
        CHECK(entry->IsEmpty());

        wxYield();

        entry->Paste();
        CHECK(entry->GetValue() == "sometext");
    }
#endif
}

void TextEntryTestCase::UndoRedo()
{
    wxTextEntry * const entry = GetTestEntry();

    entry->AppendText("sometext");

    if(entry->CanUndo())
    {
        entry->Undo();
        CHECK(entry->IsEmpty());

        if(entry->CanRedo())
        {
            entry->Redo();
            CHECK(entry->GetValue() == "sometext");
        }
    }
}

#if wxUSE_UIACTIONSIMULATOR

namespace
{

enum ProcessEnter
{
    ProcessEnter_No,
    ProcessEnter_ButSkip,
    ProcessEnter_WithoutSkipping
};

class TestDialog : public wxDialog
{
public:
    explicit TestDialog(const TextLikeControlCreator& controlCreator,
                        ProcessEnter processEnter)
        : wxDialog(wxTheApp->GetTopWindow(), wxID_ANY, "Test dialog"),
          m_control
          (
              controlCreator.Create
              (
               this,
               processEnter == ProcessEnter_No ? 0 : wxTE_PROCESS_ENTER
              )
          ),
          m_processEnter(processEnter),
          m_gotEnter(false)
    {
        wxSizer* const sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(m_control, wxSizerFlags().Expand());
        sizer->Add(CreateStdDialogButtonSizer(wxOK));
        SetSizerAndFit(sizer);

        CallAfter(&TestDialog::SimulateEnter);

        m_timer.Bind(wxEVT_TIMER, &TestDialog::OnTimeOut, this);
        m_timer.StartOnce(2000);
    }

    bool GotEnter() const { return m_gotEnter; }

private:
    void OnTextEnter(wxCommandEvent& e)
    {
        m_gotEnter = true;

        switch ( m_processEnter )
        {
            case ProcessEnter_No:
                FAIL("Shouldn't be getting wxEVT_TEXT_ENTER at all");
                break;

            case ProcessEnter_ButSkip:
                e.Skip();
                break;

            case ProcessEnter_WithoutSkipping:
                // Close the dialog with a different exit code than what
                // pressing the OK button would have generated.
                EndModal(wxID_APPLY);
                break;
        }
    }

    void OnText(wxCommandEvent& WXUNUSED(e))
    {
        // This should only happen for the multiline text controls.
        switch ( m_processEnter )
        {
            case ProcessEnter_No:
            case ProcessEnter_ButSkip:
                // We consider that the text succeeded, but in a different way,
                // so use a different ID to be able to distinguish between this
                // scenario and Enter activating the default button.
                EndModal(wxID_CLOSE);
                break;

            case ProcessEnter_WithoutSkipping:
                FAIL("Shouldn't be getting wxEVT_TEXT if handled");
                break;
        }
    }

    void OnTimeOut(wxTimerEvent&)
    {
        EndModal(wxID_CANCEL);
    }

    void SimulateEnter()
    {
        wxUIActionSimulator sim;

        // Calling SetFocus() is somehow not enough to give the focus to this
        // window when running this test with wxGTK, apparently because the
        // dialog itself needs to be raised to the front first, so simulate a
        // click doing this.
        sim.MouseMove(m_control->ClientToScreen(
                          wxPoint(5, m_control->GetClientSize().y / 2)));
        wxYield();
        sim.MouseClick();
        wxYield();

        // Note that clicking it is still not enough to give it focus with
        // wxGTK either, so we still need to call SetFocus() nevertheless: but
        // now it works.
        m_control->SetFocus();

        sim.Char(WXK_RETURN);
    }

    wxControl* const m_control;
    const ProcessEnter m_processEnter;
    wxTimer m_timer;
    bool m_gotEnter;

    wxDECLARE_EVENT_TABLE();
};

// Note that we must use event table macros here instead of Bind() because
// binding wxEVT_TEXT_ENTER handler for a control without wxTE_PROCESS_ENTER
// style would fail with an assertion failure, due to wx helpfully complaining
// about it.
wxBEGIN_EVENT_TABLE(TestDialog, wxDialog)
    EVT_TEXT(wxID_ANY, TestDialog::OnText)
    EVT_TEXT_ENTER(wxID_ANY, TestDialog::OnTextEnter)
wxEND_EVENT_TABLE()

} // anonymous namespace

void TestProcessEnter(const TextLikeControlCreator& controlCreator)
{
    if ( !EnableUITests() )
    {
        WARN("Skipping wxTE_PROCESS_ENTER tests: wxUIActionSimulator use disabled");
        return;
    }

    SECTION("Without wxTE_PROCESS_ENTER")
    {
        TestDialog dlg(controlCreator, ProcessEnter_No);
        REQUIRE( dlg.ShowModal() == wxID_OK );
        CHECK( !dlg.GotEnter() );
    }

    SECTION("With wxTE_PROCESS_ENTER but skipping")
    {
        TestDialog dlgProcessEnter(controlCreator, ProcessEnter_ButSkip);
        REQUIRE( dlgProcessEnter.ShowModal() == wxID_OK );
        CHECK( dlgProcessEnter.GotEnter() );
    }

    SECTION("With wxTE_PROCESS_ENTER without skipping")
    {
        TestDialog dlgProcessEnter(controlCreator, ProcessEnter_WithoutSkipping);
        REQUIRE( dlgProcessEnter.ShowModal() == wxID_APPLY );
        CHECK( dlgProcessEnter.GotEnter() );
    }

    SECTION("Without wxTE_PROCESS_ENTER but with wxTE_MULTILINE")
    {
        std::unique_ptr<TextLikeControlCreator>
            multiLineCreator(controlCreator.CloneAsMultiLine());
        if ( !multiLineCreator )
            return;

        TestDialog dlg(*multiLineCreator, ProcessEnter_No);
        REQUIRE( dlg.ShowModal() == wxID_CLOSE );
        CHECK( !dlg.GotEnter() );
    }

    SECTION("With wxTE_PROCESS_ENTER and wxTE_MULTILINE but skipping")
    {
        std::unique_ptr<TextLikeControlCreator>
            multiLineCreator(controlCreator.CloneAsMultiLine());
        if ( !multiLineCreator )
            return;

        TestDialog dlg(*multiLineCreator, ProcessEnter_ButSkip);
        REQUIRE( dlg.ShowModal() == wxID_CLOSE );
        CHECK( dlg.GotEnter() );
    }

    SECTION("With wxTE_PROCESS_ENTER and wxTE_MULTILINE without skipping")
    {
        std::unique_ptr<TextLikeControlCreator>
            multiLineCreator(controlCreator.CloneAsMultiLine());
        if ( !multiLineCreator )
            return;

        TestDialog dlg(*multiLineCreator, ProcessEnter_WithoutSkipping);
        REQUIRE( dlg.ShowModal() == wxID_APPLY );
        CHECK( dlg.GotEnter() );
    }
}

#else // !wxUSE_UIACTIONSIMULATOR

void TestProcessEnter(const TextLikeControlCreator& WXUNUSED(controlCreator))
{
    WARN("Skipping wxTE_PROCESS_ENTER tests: wxUIActionSimulator not available");
}

#endif // wxUSE_UIACTIONSIMULATOR/!wxUSE_UIACTIONSIMULATOR
