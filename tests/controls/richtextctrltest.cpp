///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/richtextctrltest.cpp
// Purpose:     wxRichTextCtrl unit test
// Author:      Steven Lamerton
// Created:     2010-07-07
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_RICHTEXT


#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/richtext/richtextctrl.h"
#include "wx/richtext/richtextstyles.h"
#include "wx/uiaction.h"

#if wxUSE_CLIPBOARD && wxUSE_DATAOBJ && !defined(__WXOSX__)
    #include "wx/clipbrd.h"
    #include "wx/dataobj.h"
#endif // wxUSE_CLIPBOARD && wxUSE_DATAOBJ && !defined(__WXOSX__)

#include "testableframe.h"
#include "asserthelper.h"
#include "waitfor.h"

#include <memory>

class RichTextCtrlTestCase
{
public:
    RichTextCtrlTestCase();

protected:
    std::unique_ptr<wxRichTextCtrl> m_rich;

    wxDECLARE_NO_COPY_CLASS(RichTextCtrlTestCase);
};

#if wxUSE_CLIPBOARD && wxUSE_DATAOBJ && !defined(__WXOSX__)

namespace
{

bool SetClipboardText(const wxString &text)
{
    wxClipboardLocker lock;

    if ( !lock )
        return false;

    wxTheClipboard->Clear();
    return wxTheClipboard->SetData(new wxTextDataObject(text));
}

bool ClipboardContainsText(const wxString &text)
{
    wxClipboardLocker lock;

    if ( !lock )
        return false;

    wxTextDataObject data;
    return wxTheClipboard->GetData(data) && data.GetText() == text;
}

} // anonymous namespace

#endif // wxUSE_CLIPBOARD && wxUSE_DATAOBJ && !defined(__WXOSX__)

RichTextCtrlTestCase::RichTextCtrlTestCase()
{
    m_rich = make_unique<wxRichTextCtrl>(
        wxTheApp->GetTopWindow(), wxID_ANY, "", wxDefaultPosition, wxSize(400,
        200), wxWANTS_CHARS);
}


TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::IsModified",
                 "[richtextctrl]")
{
    CHECK( m_rich->IsModified() == false );
    m_rich->WriteText("abcdef");
    CHECK( m_rich->IsModified() == true );
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::CharacterEvent",
                 "[richtextctrl]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;


    EventCounter character(m_rich.get(), wxEVT_RICHTEXT_CHARACTER);
    EventCounter content(m_rich.get(), wxEVT_RICHTEXT_CONTENT_INSERTED);

    m_rich->SetFocus();

    wxUIActionSimulator sim;
    sim.Text("abcdef");
    wxYield();

    CHECK(character.GetCount() == 6);
    CHECK(content.GetCount() == 6);

    character.Clear();
    content.Clear();

    //As these are not characters they shouldn't count
    sim.Char(WXK_RETURN);
    sim.Char(WXK_SHIFT);
    wxYield();

    CHECK(character.GetCount() == 0);
    CHECK(content.GetCount() == 1);
#endif
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::DeleteEvent",
                 "[richtextctrl]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;


    EventCounter deleteevent(m_rich.get(), wxEVT_RICHTEXT_DELETE);
    EventCounter contentdelete(m_rich.get(), wxEVT_RICHTEXT_CONTENT_DELETED);

    m_rich->SetFocus();

    wxUIActionSimulator sim;
    sim.Text("abcdef");
    sim.Char(WXK_BACK);
    sim.Char(WXK_DELETE);
    wxYield();

    CHECK(deleteevent.GetCount() == 2);
    //Only one as the delete doesn't delete anthing
    CHECK(contentdelete.GetCount() == 1);
#endif
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::ReturnEvent",
                 "[richtextctrl]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;


    EventCounter returnevent(m_rich.get(), wxEVT_RICHTEXT_RETURN);

    m_rich->SetFocus();

    wxUIActionSimulator sim;
    sim.Char(WXK_RETURN);
    wxYield();

    CHECK(returnevent.GetCount() == 1);
#endif
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::StyleEvent",
                 "[richtextctrl]")
{
    EventCounter stylechanged(m_rich.get(), wxEVT_RICHTEXT_STYLE_CHANGED);

    m_rich->SetValue("Sometext");
    m_rich->SetStyle(0, 8, wxTextAttr(*wxRED, *wxWHITE));

    CHECK(stylechanged.GetCount() == 1);
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::BufferResetEvent",
                 "[richtextctrl]")
{
    EventCounter reset(m_rich.get(), wxEVT_RICHTEXT_BUFFER_RESET);

    m_rich->AppendText("more text!");
    m_rich->SetValue("");

    CHECK(reset.GetCount() == 1);

    reset.Clear();
    m_rich->AppendText("more text!");
    m_rich->Clear();

    CHECK(reset.GetCount() == 1);

    reset.Clear();

    //We expect a buffer reset here as setvalue clears the existing text
    m_rich->SetValue("replace");
    CHECK(reset.GetCount() == 1);
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::UrlEvent",
                 "[richtextctrl]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;


    EventCounter url(m_rich.get(), wxEVT_TEXT_URL);

    m_rich->BeginURL("http://www.wxwidgets.org");
    m_rich->WriteText("http://www.wxwidgets.org");
    m_rich->EndURL();

    wxUIActionSimulator sim;
    sim.MouseMove(m_rich->ClientToScreen(wxPoint(10, 10)));
    wxYield();

    sim.MouseClick();
    wxYield();

    CHECK(url.GetCount() == 1);
#endif
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::TextEvent",
                 "[richtextctrl]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;

    EventCounter updated(m_rich.get(), wxEVT_TEXT);

    m_rich->SetFocus();

    wxUIActionSimulator sim;
    sim.Text("abcdef");
    wxYield();

    CHECK(m_rich->GetValue() == "abcdef");
    CHECK(updated.GetCount() == 6);
#endif
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::CutCopyPaste",
                 "[richtextctrl]")
{
#if wxUSE_CLIPBOARD && wxUSE_DATAOBJ && !defined(__WXOSX__)
    const wxString text("sometext");
    const wxString sentinel("not sometext");

    auto waitForPaste = [&]()
    {
        return WaitFor("wxRichTextCtrl paste clipboard update",
                       [&]()
                       {
                           if ( m_rich->GetValue() == text )
                               return true;

                           m_rich->Paste();
                           return m_rich->GetValue() == text;
                       });
    };

    m_rich->AppendText(text);
    m_rich->SelectAll();

    REQUIRE(WaitFor("wxRichTextCtrl clipboard setup",
                    [&]() { return SetClipboardText(sentinel); }));

    if ( m_rich->CanCut() )
    {
        REQUIRE(WaitFor("wxRichTextCtrl cut clipboard update",
                        [&]()
                        {
                            m_rich->Cut();
                            return m_rich->IsEmpty() &&
                                   ClipboardContainsText(text);
                        }));
        CHECK(m_rich->IsEmpty());

        REQUIRE(waitForPaste());
        CHECK(m_rich->GetValue() == text);
    }

    m_rich->SelectAll();

    REQUIRE(WaitFor("wxRichTextCtrl clipboard setup",
                    [&]() { return SetClipboardText(sentinel); }));

    if ( m_rich->CanCopy() )
    {
        REQUIRE(WaitFor("wxRichTextCtrl copy clipboard update",
                        [&]()
                        {
                            m_rich->Copy();
                            return ClipboardContainsText(text);
                        }));
        m_rich->Clear();
        CHECK(m_rich->IsEmpty());

        REQUIRE(waitForPaste());
        CHECK(m_rich->GetValue() == text);
    }
#endif
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::UndoRedo",
                 "[richtextctrl]")
{
    m_rich->AppendText("sometext");

    CHECK(m_rich->CanUndo());

    m_rich->Undo();

    CHECK(m_rich->IsEmpty());
    CHECK(m_rich->CanRedo());

    m_rich->Redo();

    CHECK(m_rich->GetValue() == "sometext");

    m_rich->AppendText("Batch undo");
    m_rich->SelectAll();

    //Also test batch operations
    m_rich->BeginBatchUndo("batchtest");

    m_rich->ApplyBoldToSelection();
    m_rich->ApplyItalicToSelection();

    m_rich->EndBatchUndo();

    CHECK(m_rich->CanUndo());

    m_rich->Undo();

    CHECK(!m_rich->IsSelectionBold());
    CHECK(!m_rich->IsSelectionItalics());
    CHECK(m_rich->CanRedo());

    m_rich->Redo();

    CHECK(m_rich->IsSelectionBold());
    CHECK(m_rich->IsSelectionItalics());

    //And surpressing undo
    m_rich->BeginSuppressUndo();

    m_rich->AppendText("Can't undo this");

    CHECK(m_rich->CanUndo());

    m_rich->EndSuppressUndo();
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::CaretPosition",
                 "[richtextctrl]")
{
    m_rich->AddParagraph("This is paragraph one");
    m_rich->AddParagraph("Paragraph two\n has \nlots of\n lines");

    m_rich->SetInsertionPoint(2);

    CHECK(m_rich->GetCaretPosition() == 1);

    m_rich->MoveToParagraphStart();

    CHECK(m_rich->GetCaretPosition() == 0);

    m_rich->MoveRight();
    m_rich->MoveRight(2);
    m_rich->MoveLeft(1);
    m_rich->MoveLeft(0);

    CHECK(m_rich->GetCaretPosition() == 2);

    m_rich->MoveToParagraphEnd();

    CHECK(m_rich->GetCaretPosition() == 21);

    m_rich->MoveToLineStart();

    CHECK(m_rich->GetCaretPosition() == 0);

    m_rich->MoveToLineEnd();

    CHECK(m_rich->GetCaretPosition() == 21);
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::Selection",
                 "[richtextctrl]")
{
    m_rich->SetValue("some more text");

    m_rich->SelectAll();

    CHECK(m_rich->GetStringSelection() == "some more text");

    m_rich->SelectNone();

    CHECK(m_rich->GetStringSelection() == "");

    m_rich->SelectWord(1);

    CHECK(m_rich->GetStringSelection() == "some");

    m_rich->SetSelection(5, 14);

    CHECK(m_rich->GetStringSelection() == "more text");

    wxRichTextRange range(5, 9);

    m_rich->SetSelectionRange(range);

    CHECK(m_rich->GetStringSelection() == "more");
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::Editable",
                 "[richtextctrl]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;

    EventCounter updated(m_rich.get(), wxEVT_TEXT);

    m_rich->SetFocus();

    wxUIActionSimulator sim;
    sim.Text("abcdef");
    wxYield();

    CHECK(m_rich->GetValue() == "abcdef");
    CHECK(updated.GetCount() == 6);
    updated.Clear();

    m_rich->SetEditable(false);
    sim.Text("gh");
    wxYield();

    CHECK(m_rich->GetValue() == "abcdef");
    CHECK(updated.GetCount() == 0);
#endif
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::Range", "[richtextctrl]")
{
    wxRichTextRange range(0, 10);

    CHECK(range.GetStart() == 0);
    CHECK(range.GetEnd() == 10);
    CHECK(range.GetLength() == 11);
    CHECK(range.Contains(5));

    wxRichTextRange outside(12, 14);

    CHECK(outside.IsOutside(range));

    wxRichTextRange inside(6, 7);

    CHECK(inside.IsWithin(range));

    range.LimitTo(inside);

    CHECK(inside == range);
    CHECK(inside + range == outside);
    CHECK(outside - range == inside);

    range.SetStart(4);
    range.SetEnd(6);

    CHECK(range.GetStart() == 4);
    CHECK(range.GetEnd() == 6);
    CHECK(range.GetLength() == 3);

    inside.SetRange(6, 4);
    inside.Swap();

    CHECK(inside == range);
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::Alignment",
                 "[richtextctrl]")
{
    m_rich->SetValue("text to align");
    m_rich->SelectAll();

    m_rich->ApplyAlignmentToSelection(wxTEXT_ALIGNMENT_RIGHT);

    CHECK(m_rich->IsSelectionAligned(wxTEXT_ALIGNMENT_RIGHT));

    m_rich->BeginAlignment(wxTEXT_ALIGNMENT_CENTRE);
    m_rich->AddParagraph("middle aligned");
    m_rich->EndAlignment();

    m_rich->SetSelection(20, 25);

    CHECK(m_rich->IsSelectionAligned(wxTEXT_ALIGNMENT_CENTRE));
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::Bold", "[richtextctrl]")
{
    m_rich->SetValue("text to bold");
    m_rich->SelectAll();
    m_rich->ApplyBoldToSelection();

    CHECK(m_rich->IsSelectionBold());

    m_rich->BeginBold();
    m_rich->AddParagraph("bold paragraph");
    m_rich->EndBold();
    m_rich->AddParagraph("not bold paragraph");

    m_rich->SetSelection(15, 20);

    CHECK(m_rich->IsSelectionBold());

    m_rich->SetSelection(30, 35);

    CHECK(!m_rich->IsSelectionBold());
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::Italic",
                 "[richtextctrl]")
{
    m_rich->SetValue("text to italic");
    m_rich->SelectAll();
    m_rich->ApplyItalicToSelection();

    CHECK(m_rich->IsSelectionItalics());

    m_rich->BeginItalic();
    m_rich->AddParagraph("italic paragraph");
    m_rich->EndItalic();
    m_rich->AddParagraph("not italic paragraph");

    m_rich->SetSelection(20, 25);

    CHECK(m_rich->IsSelectionItalics());

    m_rich->SetSelection(35, 40);

    CHECK(!m_rich->IsSelectionItalics());
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::Underline",
                 "[richtextctrl]")
{
    m_rich->SetValue("text to underline");
    m_rich->SelectAll();
    m_rich->ApplyUnderlineToSelection();

    CHECK(m_rich->IsSelectionUnderlined());

    m_rich->BeginUnderline();
    m_rich->AddParagraph("underline paragraph");
    m_rich->EndUnderline();
    m_rich->AddParagraph("not underline paragraph");

    m_rich->SetSelection(20, 25);

    CHECK(m_rich->IsSelectionUnderlined());

    m_rich->SetSelection(40, 45);

    CHECK(!m_rich->IsSelectionUnderlined());
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::Indent",
                 "[richtextctrl]")
{
    m_rich->BeginLeftIndent(12, -5);
    m_rich->BeginRightIndent(14);
    m_rich->AddParagraph("A paragraph with indents");
    m_rich->EndLeftIndent();
    m_rich->EndRightIndent();
    m_rich->AddParagraph("No more indent");

    wxTextAttr indent;
    m_rich->GetStyle(5, indent);

    CHECK(indent.GetLeftIndent() == 12);
    CHECK(indent.GetLeftSubIndent() == -5);
    CHECK(indent.GetRightIndent() == 14);

    m_rich->GetStyle(35, indent);

    CHECK(indent.GetLeftIndent() == 0);
    CHECK(indent.GetLeftSubIndent() == 0);
    CHECK(indent.GetRightIndent() == 0);
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::LineSpacing",
                 "[richtextctrl]")
{
    m_rich->BeginLineSpacing(20);
    m_rich->AddParagraph("double spaced");
    m_rich->EndLineSpacing();
    m_rich->BeginLineSpacing(wxTEXT_ATTR_LINE_SPACING_HALF);
    m_rich->AddParagraph("1.5 spaced");
    m_rich->EndLineSpacing();
    m_rich->AddParagraph("normally spaced");

    wxTextAttr spacing;
    m_rich->GetStyle(5, spacing);

    CHECK(spacing.GetLineSpacing() == 20);

    m_rich->GetStyle(20, spacing);

    CHECK(spacing.GetLineSpacing() == 15);

    m_rich->GetStyle(30, spacing);

    CHECK(spacing.GetLineSpacing() == 10);
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::ParagraphSpacing",
                 "[richtextctrl]")
{
    m_rich->BeginParagraphSpacing(15, 20);
    m_rich->AddParagraph("spaced paragraph");
    m_rich->EndParagraphSpacing();
    m_rich->AddParagraph("non-spaced paragraph");

    wxTextAttr spacing;
    m_rich->GetStyle(5, spacing);

    CHECK(spacing.GetParagraphSpacingBefore() == 15);
    CHECK(spacing.GetParagraphSpacingAfter() == 20);

    m_rich->GetStyle(25, spacing);

    //Make sure we test against the defaults
    CHECK(spacing.GetParagraphSpacingBefore() == m_rich->GetBasicStyle().GetParagraphSpacingBefore());
    CHECK(spacing.GetParagraphSpacingAfter() == m_rich->GetBasicStyle().GetParagraphSpacingAfter());
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::TextColour",
                 "[richtextctrl]")
{
    m_rich->BeginTextColour(*wxRED);
    m_rich->AddParagraph("red paragraph");
    m_rich->EndTextColour();
    m_rich->AddParagraph("default paragraph");

    wxTextAttr colour;
    m_rich->GetStyle(5, colour);

    CHECK(colour.GetTextColour() == *wxRED);

    m_rich->GetStyle(25, colour);

    CHECK(colour.GetTextColour() == m_rich->GetBasicStyle().GetTextColour());
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::NumberedBullet",
                 "[richtextctrl]")
{
    m_rich->BeginNumberedBullet(1, 15, 20);
    m_rich->AddParagraph("bullet one");
    m_rich->EndNumberedBullet();
    m_rich->BeginNumberedBullet(2, 25, -5);
    m_rich->AddParagraph("bullet two");
    m_rich->EndNumberedBullet();

    wxTextAttr bullet;
    m_rich->GetStyle(5, bullet);

    CHECK(bullet.HasBulletStyle());
    CHECK(bullet.HasBulletNumber());
    CHECK(bullet.GetBulletNumber() == 1);
    CHECK(bullet.GetLeftIndent() == 15);
    CHECK(bullet.GetLeftSubIndent() == 20);

    m_rich->GetStyle(15, bullet);

    CHECK(bullet.HasBulletStyle());
    CHECK(bullet.HasBulletNumber());
    CHECK(bullet.GetBulletNumber() == 2);
    CHECK(bullet.GetLeftIndent() == 25);
    CHECK(bullet.GetLeftSubIndent() == -5);
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::SymbolBullet",
                 "[richtextctrl]")
{
    m_rich->BeginSymbolBullet("*", 15, 20);
    m_rich->AddParagraph("bullet one");
    m_rich->EndSymbolBullet();
    m_rich->BeginSymbolBullet("%", 25, -5);
    m_rich->AddParagraph("bullet two");
    m_rich->EndSymbolBullet();

    wxTextAttr bullet;
    m_rich->GetStyle(5, bullet);

    CHECK(bullet.HasBulletStyle());
    CHECK(bullet.HasBulletText());
    CHECK(bullet.GetBulletText() == "*");
    CHECK(bullet.GetLeftIndent() == 15);
    CHECK(bullet.GetLeftSubIndent() == 20);

    m_rich->GetStyle(15, bullet);

    CHECK(bullet.HasBulletStyle());
    CHECK(bullet.HasBulletText());
    CHECK(bullet.GetBulletText() == "%");
    CHECK(bullet.GetLeftIndent() == 25);
    CHECK(bullet.GetLeftSubIndent() == -5);
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::FontSize",
                 "[richtextctrl]")
{
    m_rich->BeginFontSize(24);
    m_rich->AddParagraph("Large text");
    m_rich->EndFontSize();

    wxTextAttr size;
    m_rich->GetStyle(5, size);

    CHECK(size.HasFontSize());
    CHECK(size.GetFontSize() == 24);
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::Font", "[richtextctrl]")
{
    wxFont font(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    m_rich->BeginFont(font);
    m_rich->AddParagraph("paragraph with font");
    m_rich->EndFont();

    wxTextAttr fontstyle;
    m_rich->GetStyle(5, fontstyle);

    CHECK(fontstyle.GetFont() == font);
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::Delete",
                 "[richtextctrl]")
{
    m_rich->AddParagraph("here is a long long line in a paragraph");
    m_rich->SetSelection(0, 6);

    CHECK(m_rich->CanDeleteSelection());

    m_rich->DeleteSelection();

    CHECK(m_rich->GetValue() == "is a long long line in a paragraph");

    m_rich->SetSelection(0, 5);

    CHECK(m_rich->CanDeleteSelection());

    m_rich->DeleteSelectedContent();

    CHECK(m_rich->GetValue() == "long long line in a paragraph");

    m_rich->Delete(wxRichTextRange(14, 29));

    CHECK(m_rich->GetValue() == "long long line");
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::Url", "[richtextctrl]")
{
    m_rich->BeginURL("http://www.wxwidgets.org");
    m_rich->WriteText("http://www.wxwidgets.org");
    m_rich->EndURL();

    wxTextAttr url;
    m_rich->GetStyle(5, url);

    CHECK(url.HasURL());
    CHECK(url.GetURL() == "http://www.wxwidgets.org");
}

    // Helper function for ::Table()
wxRichTextTable* GetCurrentTableInstance(wxRichTextParagraph* para)
{
    wxRichTextTable* table = wxDynamicCast(para->FindObjectAtPosition(0), wxRichTextTable);
    CHECK(table);
    return table;
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::Table", "[richtextctrl]")
{
    m_rich->BeginSuppressUndo();
    wxRichTextTable* table = m_rich->WriteTable(1, 1);
    m_rich->EndSuppressUndo();
    CHECK(table);
    CHECK(m_rich->CanUndo() == false);

    // Run the tests twice: first for the original table, then for a contained one
    for (int t = 0; t < 2; ++t)
    {
        // Undo() and Redo() switch table instances, so invalidating 'table'
        // The containing paragraph isn't altered, and so can be used to find the current object
        wxRichTextParagraph* para = wxDynamicCast(table->GetParent(), wxRichTextParagraph);
        CHECK(para);

        CHECK(table->GetColumnCount() == 1);
        CHECK(table->GetRowCount() == 1);

        // Test adding columns and rows
        for (size_t n = 0; n < 3; ++n)
        {
            m_rich->BeginBatchUndo("Add col and row");

            table->AddColumns(0, 1);
            table->AddRows(0, 1);

            m_rich->EndBatchUndo();
        }
        CHECK(table->GetColumnCount() == 4);
        CHECK(table->GetRowCount() == 4);

        // Test deleting columns and rows
        for (size_t n = 0; n < 3; ++n)
        {
            m_rich->BeginBatchUndo("Delete col and row");

            table->DeleteColumns(table->GetColumnCount() - 1, 1);
            table->DeleteRows(table->GetRowCount() - 1, 1);

            m_rich->EndBatchUndo();
        }
        CHECK(table->GetColumnCount() == 1);
        CHECK(table->GetRowCount() == 1);

        // Test undo, first of the deletions...
        CHECK(m_rich->CanUndo());
        for (size_t n = 0; n < 3; ++n)
        {
            m_rich->Undo();
        }
        table = GetCurrentTableInstance(para);
        CHECK(table->GetColumnCount() == 4);
        CHECK(table->GetRowCount() == 4);

        // ...then the additions
        for (size_t n = 0; n < 3; ++n)
        {
            m_rich->Undo();
        }
        table = GetCurrentTableInstance(para);
        CHECK(table->GetColumnCount() == 1);
        CHECK(table->GetRowCount() == 1);
        CHECK(m_rich->CanUndo() == false);

        // Similarly test redo. Additions:
        CHECK(m_rich->CanRedo());
        for (size_t n = 0; n < 3; ++n)
        {
            m_rich->Redo();
        }
        table = GetCurrentTableInstance(para);
        CHECK(table->GetColumnCount() == 4);
        CHECK(table->GetRowCount() == 4);

        // Deletions:
        for (size_t n = 0; n < 3; ++n)
        {
            m_rich->Redo();
        }
        table = GetCurrentTableInstance(para);
        CHECK(table->GetColumnCount() == 1);
        CHECK(table->GetRowCount() == 1);
        CHECK(m_rich->CanRedo() == false);

        // Now test multiple addition and deletion, and also suppression
        m_rich->BeginSuppressUndo();
        table->AddColumns(0, 3);
        table->AddRows(0, 3);
        CHECK(table->GetColumnCount() == 4);
        CHECK(table->GetRowCount() == 4);

        // Only delete 2 of these. This makes it easy to be sure we're dealing with the child table when we loop
        table->DeleteColumns(0, 2);
        table->DeleteRows(0, 2);
        CHECK(table->GetColumnCount() == 2);
        CHECK(table->GetRowCount() == 2);
        m_rich->EndSuppressUndo();

        m_rich->GetCommandProcessor()->ClearCommands(); // otherwise the command-history from this loop will cause the checks in the next one to fail

        if (t == 0)
        {
            // For round 2, re-run the tests on another table inside the last cell of the first one
            wxRichTextCell* cell = table->GetCell(table->GetRowCount() - 1, table->GetColumnCount() - 1);
            CHECK(cell);
            m_rich->SetFocusObject(cell);
            m_rich->BeginSuppressUndo();
            table = m_rich->WriteTable(1, 1);
            m_rich->EndSuppressUndo();
            CHECK(table);
        }
    }

    // Test ClearTable()
    table->ClearTable();
    CHECK(table->GetCells().GetCount() == 0);
    CHECK(table->GetColumnCount() == 0);
    CHECK(table->GetRowCount() == 0);

    m_rich->Clear();
    m_rich->SetFocusObject(nullptr);
}

#endif //wxUSE_RICHTEXT
