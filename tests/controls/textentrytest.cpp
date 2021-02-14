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

#include "wx/scopedptr.h"
#include "wx/uiaction.h"

void TextEntryTestCase::SetValue()
{
    wxTextEntry * const entry = GetTestEntry();

    CPPUNIT_ASSERT( entry->IsEmpty() );

    entry->SetValue("foo");
    CPPUNIT_ASSERT_EQUAL( "foo", entry->GetValue() );

    entry->SetValue("");
    CPPUNIT_ASSERT( entry->IsEmpty() );

    entry->SetValue("hi");
    CPPUNIT_ASSERT_EQUAL( "hi", entry->GetValue() );

    entry->SetValue("bye");
    CPPUNIT_ASSERT_EQUAL( "bye", entry->GetValue() );
}

void TextEntryTestCase::TextChangeEvents()
{
    EventCounter updated(GetTestWindow(), wxEVT_TEXT);

    wxTextEntry * const entry = GetTestEntry();

    // notice that SetValue() generates an event even if the text didn't change
#ifndef __WXQT__
    entry->SetValue("");
    CPPUNIT_ASSERT_EQUAL( 1, updated.GetCount() );
    updated.Clear();
#else
    WARN("Events are only sent when text changes in WxQt");
#endif

    entry->SetValue("foo");
    CPPUNIT_ASSERT_EQUAL( 1, updated.GetCount() );
    updated.Clear();

#ifndef __WXQT__
    entry->SetValue("foo");
    CPPUNIT_ASSERT_EQUAL( 1, updated.GetCount() );
    updated.Clear();
#else
    WARN("Events are only sent when text changes in WxQt");
#endif

    entry->SetValue("");
    CPPUNIT_ASSERT_EQUAL( 1, updated.GetCount() );
    updated.Clear();

    entry->ChangeValue("bar");
    CPPUNIT_ASSERT_EQUAL( 0, updated.GetCount() );

    entry->AppendText("bar");
    CPPUNIT_ASSERT_EQUAL( 1, updated.GetCount() );
    updated.Clear();

    entry->Replace(3, 6, "baz");
    CPPUNIT_ASSERT_EQUAL( 1, updated.GetCount() );
    updated.Clear();

    entry->Remove(0, 3);
    CPPUNIT_ASSERT_EQUAL( 1, updated.GetCount() );
    updated.Clear();

    entry->WriteText("foo");
    CPPUNIT_ASSERT_EQUAL( 1, updated.GetCount() );
    updated.Clear();

    entry->Clear();
    CPPUNIT_ASSERT_EQUAL( 1, updated.GetCount() );
    updated.Clear();

    entry->ChangeValue("");
    CPPUNIT_ASSERT_EQUAL( 0, updated.GetCount() );
    updated.Clear();

    entry->ChangeValue("non-empty");
    CPPUNIT_ASSERT_EQUAL( 0, updated.GetCount() );
    updated.Clear();

    entry->ChangeValue("");
    CPPUNIT_ASSERT_EQUAL( 0, updated.GetCount() );
    updated.Clear();
}

void TextEntryTestCase::CheckStringSelection(const char *sel)
{
    CPPUNIT_ASSERT_EQUAL( sel, GetTestEntry()->GetStringSelection() );
}

void TextEntryTestCase::AssertSelection(int from, int to, const char *sel)
{
    wxTextEntry * const entry = GetTestEntry();

    CPPUNIT_ASSERT( entry->HasSelection() );

    long fromReal,
         toReal;
    entry->GetSelection(&fromReal, &toReal);
    CPPUNIT_ASSERT_EQUAL( from, fromReal );
    CPPUNIT_ASSERT_EQUAL( to, toReal );

    CPPUNIT_ASSERT_EQUAL( from, entry->GetInsertionPoint() );

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
    CPPUNIT_ASSERT( !entry->HasSelection() );
}

void TextEntryTestCase::InsertionPoint()
{
    wxTextEntry * const entry = GetTestEntry();

    CPPUNIT_ASSERT_EQUAL( 0, entry->GetLastPosition() );
    CPPUNIT_ASSERT_EQUAL( 0, entry->GetInsertionPoint() );

    entry->SetValue("0"); // should put the insertion point in front
    CPPUNIT_ASSERT_EQUAL( 1, entry->GetLastPosition() );
    CPPUNIT_ASSERT_EQUAL( 0, entry->GetInsertionPoint() );

    entry->AppendText("12"); // should update the insertion point position
    CPPUNIT_ASSERT_EQUAL( 3, entry->GetLastPosition() );
    CPPUNIT_ASSERT_EQUAL( 3, entry->GetInsertionPoint() );

    entry->SetInsertionPoint(1);
    CPPUNIT_ASSERT_EQUAL( 3, entry->GetLastPosition() );
    CPPUNIT_ASSERT_EQUAL( 1, entry->GetInsertionPoint() );

    entry->SetValue("012"); // shouldn't change the position if no real change
    CPPUNIT_ASSERT_EQUAL( 1, entry->GetInsertionPoint() );

    entry->ChangeValue("012"); // same as for SetValue()
    CPPUNIT_ASSERT_EQUAL( 1, entry->GetInsertionPoint() );

    entry->SetInsertionPointEnd();
    CPPUNIT_ASSERT_EQUAL( 3, entry->GetInsertionPoint() );

    entry->SetInsertionPoint(0);
    entry->WriteText("-"); // should move it after the written text
    CPPUNIT_ASSERT_EQUAL( 4, entry->GetLastPosition() );
    CPPUNIT_ASSERT_EQUAL( 1, entry->GetInsertionPoint() );

    entry->SetValue("something different"); // should still reset the caret
    CPPUNIT_ASSERT_EQUAL( 0, entry->GetInsertionPoint() );
}

void TextEntryTestCase::Replace()
{
    wxTextEntry * const entry = GetTestEntry();

    entry->SetValue("Hello replace!"
                    "0123456789012");
    entry->SetInsertionPoint(0);

    entry->Replace(6, 13, "changed");

    CPPUNIT_ASSERT_EQUAL("Hello changed!"
                         "0123456789012",
                         entry->GetValue());
    CPPUNIT_ASSERT_EQUAL(13, entry->GetInsertionPoint());

    entry->Replace(13, -1, "");
    CPPUNIT_ASSERT_EQUAL("Hello changed", entry->GetValue());
    CPPUNIT_ASSERT_EQUAL(13, entry->GetInsertionPoint());

    entry->Replace(0, 6, "Un");
    CPPUNIT_ASSERT_EQUAL("Unchanged", entry->GetValue());
    CPPUNIT_ASSERT_EQUAL(2, entry->GetInsertionPoint());
}

void TextEntryTestCase::WriteText()
{
    wxTextEntry * const entry = GetTestEntry();

    entry->SetValue("foo");
    entry->SetInsertionPoint(3);
    entry->WriteText("bar");
    CPPUNIT_ASSERT_EQUAL( "foobar", entry->GetValue() );

    entry->SetValue("foo");
    entry->SetInsertionPoint(0);
    entry->WriteText("bar");
    CPPUNIT_ASSERT_EQUAL( "barfoo", entry->GetValue() );

    entry->SetValue("abxxxhi");
    entry->SetSelection(2, 5);
    entry->WriteText("cdefg");
    CPPUNIT_ASSERT_EQUAL( "abcdefghi", entry->GetValue() );
    CPPUNIT_ASSERT_EQUAL( 7, entry->GetInsertionPoint() );
    CPPUNIT_ASSERT_EQUAL( false, entry->HasSelection() );
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

    CPPUNIT_ASSERT_EQUAL("abcdef", entry->GetValue());
    CPPUNIT_ASSERT_EQUAL(6, updated.GetCount());

    wxYield();

    // And that the event carries the right value.
    TextEventHandler handler(window);

    sim.Text("g");
    wxYield();

    CPPUNIT_ASSERT_EQUAL("abcdefg", handler.GetLastString());

    // ... even if we generate the event programmatically and whether it uses
    // the same value as the control has right now
    entry->SetValue("abcdefg");
    CPPUNIT_ASSERT_EQUAL("abcdefg", handler.GetLastString());

    // ... or not
    entry->SetValue("abcdef");
    CPPUNIT_ASSERT_EQUAL("abcdef", handler.GetLastString());

    // Check that making the control not editable does indeed prevent it from
    // being edited.
    updated.Clear();

    entry->SetEditable(false);
    sim.Text("gh");
    wxYield();

    CPPUNIT_ASSERT_EQUAL("abcdef", entry->GetValue());
    CPPUNIT_ASSERT_EQUAL(0, updated.GetCount());
}

#endif // wxUSE_UIACTIONSIMULATOR

void TextEntryTestCase::Hint()
{
    GetTestEntry()->SetHint("This is a hint");
    CPPUNIT_ASSERT_EQUAL("", GetTestEntry()->GetValue());
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
        CPPUNIT_ASSERT(entry->IsEmpty());

        wxYield();

        entry->Paste();
        CPPUNIT_ASSERT_EQUAL("sometext", entry->GetValue());
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
        CPPUNIT_ASSERT(entry->IsEmpty());

        if(entry->CanRedo())
        {
            entry->Redo();
            CPPUNIT_ASSERT_EQUAL("sometext", entry->GetValue());
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
        wxScopedPtr<TextLikeControlCreator>
            multiLineCreator(controlCreator.CloneAsMultiLine());
        if ( !multiLineCreator )
            return;

        TestDialog dlg(*multiLineCreator, ProcessEnter_No);
        REQUIRE( dlg.ShowModal() == wxID_CLOSE );
        CHECK( !dlg.GotEnter() );
    }

    SECTION("With wxTE_PROCESS_ENTER and wxTE_MULTILINE but skipping")
    {
        wxScopedPtr<TextLikeControlCreator>
            multiLineCreator(controlCreator.CloneAsMultiLine());
        if ( !multiLineCreator )
            return;

        TestDialog dlg(*multiLineCreator, ProcessEnter_ButSkip);
        REQUIRE( dlg.ShowModal() == wxID_CLOSE );
        CHECK( dlg.GotEnter() );
    }

    SECTION("With wxTE_PROCESS_ENTER and wxTE_MULTILINE without skipping")
    {
        wxScopedPtr<TextLikeControlCreator>
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
