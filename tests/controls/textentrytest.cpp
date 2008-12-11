///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/textentrytest.cpp
// Purpose:     TestEntryTestCase implementation
// Author:      Vadim Zeitlin
// Created:     2008-09-19 (extracted from textctrltest.cpp)
// RCS-ID:      $Id$
// Copyright:   (c) 2007, 2008 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/textentry.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "textentrytest.h"

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
    class TextTestEventHandler : public wxEvtHandler
    {
    public:
        TextTestEventHandler() { m_events = 0; }

        // calling this automatically resets the events counter
        int GetEvents()
        {
            const int events = m_events;
            m_events = 0;
            return events;
        }

        void OnText(wxCommandEvent& WXUNUSED(event)) { m_events++; }

    private:
        int m_events;
    } handler;

    GetTestWindow()->Connect
                     (
                        wxEVT_COMMAND_TEXT_UPDATED,
                        wxCommandEventHandler(TextTestEventHandler::OnText),
                        NULL,
                        &handler
                     );

    wxTextEntry * const entry = GetTestEntry();

    // notice that SetValue() generates an event even if the text didn't change
    entry->SetValue("");
    CPPUNIT_ASSERT_EQUAL( 1, handler.GetEvents() );

    entry->SetValue("foo");
    CPPUNIT_ASSERT_EQUAL( 1, handler.GetEvents() );

    entry->SetValue("foo");
    CPPUNIT_ASSERT_EQUAL( 1, handler.GetEvents() );

    entry->ChangeValue("bar");
    CPPUNIT_ASSERT_EQUAL( 0, handler.GetEvents() );

    entry->AppendText("bar");
    CPPUNIT_ASSERT_EQUAL( 1, handler.GetEvents() );

    entry->Replace(3, 6, "baz");
    CPPUNIT_ASSERT_EQUAL( 1, handler.GetEvents() );

    entry->Remove(0, 3);
    CPPUNIT_ASSERT_EQUAL( 1, handler.GetEvents() );

    entry->WriteText("foo");
    CPPUNIT_ASSERT_EQUAL( 1, handler.GetEvents() );

    entry->Clear();
    CPPUNIT_ASSERT_EQUAL( 1, handler.GetEvents() );
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

    entry->SetInsertionPointEnd();
    CPPUNIT_ASSERT_EQUAL( 3, entry->GetInsertionPoint() );

    entry->SetInsertionPoint(0);
    entry->WriteText("-"); // should move it after the written text
    CPPUNIT_ASSERT_EQUAL( 4, entry->GetLastPosition() );
    CPPUNIT_ASSERT_EQUAL( 1, entry->GetInsertionPoint() );
}

void TextEntryTestCase::Replace()
{
    wxTextEntry * const entry = GetTestEntry();

    entry->SetValue("Hello replace\n"
                    "0123456789012");
    entry->SetInsertionPoint(0);

    entry->Replace(6, 13, "changed");

    CPPUNIT_ASSERT_EQUAL("Hello changed\n"
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

