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
    #include "wx/event.h"
    #include "wx/textentry.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "textentrytest.h"
#include "testableframe.h"
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
    entry->SetValue("");
    CPPUNIT_ASSERT_EQUAL( 1, updated.GetCount() );
    updated.Clear();

    entry->SetValue("foo");
    CPPUNIT_ASSERT_EQUAL( 1, updated.GetCount() );
    updated.Clear();

    entry->SetValue("foo");
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

void TextEntryTestCase::Editable()
{
#if wxUSE_UIACTIONSIMULATOR

#ifdef __WXGTK__
    // FIXME: For some reason this test regularly (although not always) fails
    //        in wxGTK build bot builds when testing wxBitmapComboBox, but I
    //        can't reproduce the failure locally. For now, disable this check
    //        to let the entire test suite pass in automatic tests instead of
    //        failing sporadically.
    if ( wxStrcmp(GetTestWindow()->GetClassInfo()->GetClassName(),
                  "wxBitmapComboBox") == 0 &&
           IsAutomaticTest() )
    {
        return;
    }
#endif // __WGTK__

    wxTextEntry * const entry = GetTestEntry();
    wxWindow * const window = GetTestWindow();

    EventCounter updated(window, wxEVT_TEXT);

    window->SetFocus();
    wxYield();

    wxUIActionSimulator sim;
    sim.Text("abcdef");
    wxYield();

    CPPUNIT_ASSERT_EQUAL("abcdef", entry->GetValue());
    CPPUNIT_ASSERT_EQUAL(6, updated.GetCount());

    updated.Clear();

    entry->SetEditable(false);
    sim.Text("gh");
    wxYield();

    CPPUNIT_ASSERT_EQUAL("abcdef", entry->GetValue());
    CPPUNIT_ASSERT_EQUAL(0, updated.GetCount());
#endif
}

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
