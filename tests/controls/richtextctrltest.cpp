///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/richtextctrltest.cpp
// Purpose:     wxRichTextCtrl unit test
// Author:      Steven Lamerton
// Created:     2010-07-07
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_RICHTEXT

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/richtext/richtextctrl.h"
#include "wx/richtext/richtextstyles.h"
#include "testableframe.h"
#include "asserthelper.h"
#include "wx/uiaction.h"

class RichTextCtrlTestCase : public CppUnit::TestCase
{
public:
    RichTextCtrlTestCase() { }

    void setUp();
    void tearDown();

private:
    CPPUNIT_TEST_SUITE( RichTextCtrlTestCase );
        WXUISIM_TEST( CharacterEvent );
        WXUISIM_TEST( DeleteEvent );
        WXUISIM_TEST( ReturnEvent );
        CPPUNIT_TEST( StyleEvent );
        CPPUNIT_TEST( BufferResetEvent );
        WXUISIM_TEST( UrlEvent );
        WXUISIM_TEST( TextEvent );
        CPPUNIT_TEST( CutCopyPaste );
        CPPUNIT_TEST( UndoRedo );
        CPPUNIT_TEST( CaretPosition );
        CPPUNIT_TEST( Selection );
        WXUISIM_TEST( Editable );
        CPPUNIT_TEST( Range );
        CPPUNIT_TEST( Alignment );
        CPPUNIT_TEST( Bold );
        CPPUNIT_TEST( Italic );
        CPPUNIT_TEST( Underline );
        CPPUNIT_TEST( Indent );
        CPPUNIT_TEST( LineSpacing );
        CPPUNIT_TEST( ParagraphSpacing );
        CPPUNIT_TEST( TextColour );
        CPPUNIT_TEST( NumberedBullet );
        CPPUNIT_TEST( SymbolBullet );
        CPPUNIT_TEST( FontSize );
        CPPUNIT_TEST( Font );
        CPPUNIT_TEST( Delete );
        CPPUNIT_TEST( Url );
    CPPUNIT_TEST_SUITE_END();

    void CharacterEvent();
    void DeleteEvent();
    void ReturnEvent();
    void StyleEvent();
    void BufferResetEvent();
    void UrlEvent();
    void TextEvent();
    void CutCopyPaste();
    void UndoRedo();
    void CaretPosition();
    void Selection();
    void Editable();
    void Range();
    void Alignment();
    void Bold();
    void Italic();
    void Underline();
    void Indent();
    void LineSpacing();
    void ParagraphSpacing();
    void TextColour();
    void NumberedBullet();
    void SymbolBullet();
    void FontSize();
    void Font();
    void Delete();
    void Url();

    wxRichTextCtrl* m_rich;

    DECLARE_NO_COPY_CLASS(RichTextCtrlTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( RichTextCtrlTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( RichTextCtrlTestCase, "RichTextCtrlTestCase" );

void RichTextCtrlTestCase::setUp()
{
    m_rich = new wxRichTextCtrl(wxTheApp->GetTopWindow(), wxID_ANY, "",
                                wxDefaultPosition, wxSize(400, 200));
}

void RichTextCtrlTestCase::tearDown()
{
    wxDELETE(m_rich);
}

void RichTextCtrlTestCase::CharacterEvent()
{
#if wxUSE_UIACTIONSIMULATOR

  // There seems to be an event sequence problem on GTK+ that causes the events
  // to be disconnected before they're processed, generating spurious errors.
#if !defined(__WXGTK__)
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_rich, wxEVT_COMMAND_RICHTEXT_CHARACTER);
    EventCounter count1(m_rich, wxEVT_COMMAND_RICHTEXT_CONTENT_INSERTED);

    m_rich->SetFocus();

    wxUIActionSimulator sim;
    sim.Text("abcdef");
    wxYield();

    CPPUNIT_ASSERT_EQUAL(6, frame->GetEventCount(wxEVT_COMMAND_RICHTEXT_CHARACTER));
    CPPUNIT_ASSERT_EQUAL(6, frame->GetEventCount(wxEVT_COMMAND_RICHTEXT_CONTENT_INSERTED));

    //As these are not characters they shouldn't count
    sim.Char(WXK_RETURN);
    sim.Char(WXK_SHIFT);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(0, frame->GetEventCount(wxEVT_COMMAND_RICHTEXT_CHARACTER));
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_RICHTEXT_CONTENT_INSERTED));
#endif
#endif
}

void RichTextCtrlTestCase::DeleteEvent()
{
#if wxUSE_UIACTIONSIMULATOR
  // There seems to be an event sequence problem on GTK+ that causes the events
  // to be disconnected before they're processed, generating spurious errors.
#if !defined(__WXGTK__)
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_rich, wxEVT_COMMAND_RICHTEXT_DELETE);
    EventCounter count1(m_rich, wxEVT_COMMAND_RICHTEXT_CONTENT_DELETED);

    m_rich->SetFocus();

    wxUIActionSimulator sim;
    sim.Text("abcdef");
    sim.Char(WXK_BACK);
    sim.Char(WXK_DELETE);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(2, frame->GetEventCount(wxEVT_COMMAND_RICHTEXT_DELETE));
    //Only one as the delete doesn't delete anthing
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_RICHTEXT_CONTENT_DELETED));
#endif
#endif
}

void RichTextCtrlTestCase::ReturnEvent()
{
#if wxUSE_UIACTIONSIMULATOR
  // There seems to be an event sequence problem on GTK+ that causes the events
  // to be disconnected before they're processed, generating spurious errors.
#if !defined(__WXGTK__)
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_rich, wxEVT_COMMAND_RICHTEXT_RETURN);

    m_rich->SetFocus();

    wxUIActionSimulator sim;
    sim.Char(WXK_RETURN);
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount());
#endif
#endif
}

void RichTextCtrlTestCase::StyleEvent()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_rich, wxEVT_COMMAND_RICHTEXT_STYLE_CHANGED);

    m_rich->SetValue("Sometext");
    m_rich->SetStyle(0, 8, wxTextAttr(*wxRED, *wxWHITE));

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(wxEVT_COMMAND_RICHTEXT_STYLE_CHANGED));
}

void RichTextCtrlTestCase::BufferResetEvent()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_rich, wxEVT_COMMAND_RICHTEXT_BUFFER_RESET);

    m_rich->AppendText("more text!");
    m_rich->SetValue("");

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount());

    m_rich->AppendText("more text!");
    m_rich->Clear();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount());

    //We expect a buffer reset here as setvalue clears the existing text
    m_rich->SetValue("replace");
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount());
}

void RichTextCtrlTestCase::UrlEvent()
{
#if wxUSE_UIACTIONSIMULATOR
    // Mouse up event not being caught on GTK+
#if !defined(__WXGTK__)
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_rich, wxEVT_COMMAND_TEXT_URL);

    m_rich->BeginURL("http://www.wxwidgets.org");
    m_rich->WriteText("http://www.wxwidgets.org");
    m_rich->EndURL();

    wxUIActionSimulator sim;
    sim.MouseMove(m_rich->ClientToScreen(wxPoint(10, 10)));
    wxYield();

    sim.MouseClick();
    wxYield();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount());
#endif
#endif
}

void RichTextCtrlTestCase::TextEvent()
{
#if wxUSE_UIACTIONSIMULATOR
#if !defined(__WXGTK__)
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_rich, wxEVT_COMMAND_TEXT_UPDATED);

    m_rich->SetFocus();

    wxUIActionSimulator sim;
    sim.Text("abcdef");
    wxYield();

    CPPUNIT_ASSERT_EQUAL("abcdef", m_rich->GetValue());
    CPPUNIT_ASSERT_EQUAL(6, frame->GetEventCount());
#endif
#endif
}

void RichTextCtrlTestCase::CutCopyPaste()
{
#ifndef __WXOSX__
    m_rich->AppendText("sometext");
    m_rich->SelectAll();

    if(m_rich->CanCut() && m_rich->CanPaste())
    {
        m_rich->Cut();
        CPPUNIT_ASSERT(m_rich->IsEmpty());

        wxYield();

        m_rich->Paste();
        CPPUNIT_ASSERT_EQUAL("sometext", m_rich->GetValue());
    }

    m_rich->SelectAll();

    if(m_rich->CanCopy() && m_rich->CanPaste())
    {
        m_rich->Copy();
        m_rich->Clear();
        CPPUNIT_ASSERT(m_rich->IsEmpty());

        wxYield();

        m_rich->Paste();
        CPPUNIT_ASSERT_EQUAL("sometext", m_rich->GetValue());
    }
#endif
}

void RichTextCtrlTestCase::UndoRedo()
{
    m_rich->AppendText("sometext");

    CPPUNIT_ASSERT(m_rich->CanUndo());

    m_rich->Undo();

    CPPUNIT_ASSERT(m_rich->IsEmpty());
    CPPUNIT_ASSERT(m_rich->CanRedo());

    m_rich->Redo();

    CPPUNIT_ASSERT_EQUAL("sometext", m_rich->GetValue());

    m_rich->AppendText("Batch undo");
    m_rich->SelectAll();

    //Also test batch operations
    m_rich->BeginBatchUndo("batchtest");

    m_rich->ApplyBoldToSelection();
    m_rich->ApplyItalicToSelection();

    m_rich->EndBatchUndo();

    CPPUNIT_ASSERT(m_rich->CanUndo());

    m_rich->Undo();

    CPPUNIT_ASSERT(!m_rich->IsSelectionBold());
    CPPUNIT_ASSERT(!m_rich->IsSelectionItalics());
    CPPUNIT_ASSERT(m_rich->CanRedo());

    m_rich->Redo();

    CPPUNIT_ASSERT(m_rich->IsSelectionBold());
    CPPUNIT_ASSERT(m_rich->IsSelectionItalics());

    //And surpressing undo
    m_rich->BeginSuppressUndo();

    m_rich->AppendText("Can't undo this");

    CPPUNIT_ASSERT(m_rich->CanUndo());

    m_rich->EndSuppressUndo();
}

void RichTextCtrlTestCase::CaretPosition()
{
    m_rich->AddParagraph("This is paragraph one");
    m_rich->AddParagraph("Paragraph two\n has \nlots of\n lines");

    m_rich->MoveCaret(1);

    CPPUNIT_ASSERT_EQUAL(1, m_rich->GetCaretPosition());

    m_rich->MoveToParagraphStart();

    CPPUNIT_ASSERT_EQUAL(0, m_rich->GetCaretPosition());

    m_rich->MoveRight();
    m_rich->MoveRight(2);
    m_rich->MoveLeft(1);
    m_rich->MoveLeft(0);

    CPPUNIT_ASSERT_EQUAL(2, m_rich->GetCaretPosition());

    m_rich->MoveToParagraphEnd();

    CPPUNIT_ASSERT_EQUAL(21, m_rich->GetCaretPosition());

    m_rich->MoveToLineStart();

    CPPUNIT_ASSERT_EQUAL(0, m_rich->GetCaretPosition());

    m_rich->MoveToLineEnd();

    CPPUNIT_ASSERT_EQUAL(21, m_rich->GetCaretPosition());
}

void RichTextCtrlTestCase::Selection()
{
    m_rich->SetValue("some more text");

    m_rich->SelectAll();

    CPPUNIT_ASSERT_EQUAL("some more text", m_rich->GetStringSelection());

    m_rich->SelectNone();

    CPPUNIT_ASSERT_EQUAL("", m_rich->GetStringSelection());

    m_rich->SelectWord(1);

    CPPUNIT_ASSERT_EQUAL("some", m_rich->GetStringSelection());

    m_rich->SetSelection(5, 14);

    CPPUNIT_ASSERT_EQUAL("more text", m_rich->GetStringSelection());

    wxRichTextRange range(5, 9);

    m_rich->SetSelectionRange(range);

    CPPUNIT_ASSERT_EQUAL("more", m_rich->GetStringSelection());
}

void RichTextCtrlTestCase::Editable()
{
#if wxUSE_UIACTIONSIMULATOR
#if !defined(__WXGTK__)
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    EventCounter count(m_rich, wxEVT_COMMAND_TEXT_UPDATED);

    m_rich->SetFocus();

    wxUIActionSimulator sim;
    sim.Text("abcdef");
    wxYield();

    CPPUNIT_ASSERT_EQUAL("abcdef", m_rich->GetValue());
    CPPUNIT_ASSERT_EQUAL(6, frame->GetEventCount());

    m_rich->SetEditable(false);
    sim.Text("gh");
    wxYield();

    CPPUNIT_ASSERT_EQUAL("abcdef", m_rich->GetValue());
    CPPUNIT_ASSERT_EQUAL(0, frame->GetEventCount());
#endif
#endif
}

void RichTextCtrlTestCase::Range()
{
    wxRichTextRange range(0, 10);

    CPPUNIT_ASSERT_EQUAL(0, range.GetStart());
    CPPUNIT_ASSERT_EQUAL(10, range.GetEnd());
    CPPUNIT_ASSERT_EQUAL(11, range.GetLength());
    CPPUNIT_ASSERT(range.Contains(5));

    wxRichTextRange outside(12, 14);

    CPPUNIT_ASSERT(outside.IsOutside(range));

    wxRichTextRange inside(6, 7);

    CPPUNIT_ASSERT(inside.IsWithin(range));

    range.LimitTo(inside);

    CPPUNIT_ASSERT(inside == range);
    CPPUNIT_ASSERT(inside + range == outside);
    CPPUNIT_ASSERT(outside - range == inside);

    range.SetStart(4);
    range.SetEnd(6);

    CPPUNIT_ASSERT_EQUAL(4, range.GetStart());
    CPPUNIT_ASSERT_EQUAL(6, range.GetEnd());
    CPPUNIT_ASSERT_EQUAL(3, range.GetLength());

    inside.SetRange(6, 4);
    inside.Swap();

    CPPUNIT_ASSERT(inside == range);
}

void RichTextCtrlTestCase::Alignment()
{
    m_rich->SetValue("text to align");
    m_rich->SelectAll();

    m_rich->ApplyAlignmentToSelection(wxTEXT_ALIGNMENT_RIGHT);

    CPPUNIT_ASSERT(m_rich->IsSelectionAligned(wxTEXT_ALIGNMENT_RIGHT));

    m_rich->BeginAlignment(wxTEXT_ALIGNMENT_CENTRE);
    m_rich->AddParagraph("middle aligned");
    m_rich->EndAlignment();

    m_rich->SetSelection(20, 25);

    CPPUNIT_ASSERT(m_rich->IsSelectionAligned(wxTEXT_ALIGNMENT_CENTRE));
}

void RichTextCtrlTestCase::Bold()
{
    m_rich->SetValue("text to bold");
    m_rich->SelectAll();
    m_rich->ApplyBoldToSelection();

    CPPUNIT_ASSERT(m_rich->IsSelectionBold());

    m_rich->BeginBold();
    m_rich->AddParagraph("bold paragraph");
    m_rich->EndBold();
    m_rich->AddParagraph("not bold paragraph");

    m_rich->SetSelection(15, 20);

    CPPUNIT_ASSERT(m_rich->IsSelectionBold());

    m_rich->SetSelection(30, 35);

    CPPUNIT_ASSERT(!m_rich->IsSelectionBold());
}

void RichTextCtrlTestCase::Italic()
{
    m_rich->SetValue("text to italic");
    m_rich->SelectAll();
    m_rich->ApplyItalicToSelection();

    CPPUNIT_ASSERT(m_rich->IsSelectionItalics());

    m_rich->BeginItalic();
    m_rich->AddParagraph("italic paragraph");
    m_rich->EndItalic();
    m_rich->AddParagraph("not italic paragraph");

    m_rich->SetSelection(20, 25);

    CPPUNIT_ASSERT(m_rich->IsSelectionItalics());

    m_rich->SetSelection(35, 40);

    CPPUNIT_ASSERT(!m_rich->IsSelectionItalics());
}

void RichTextCtrlTestCase::Underline()
{
    m_rich->SetValue("text to underline");
    m_rich->SelectAll();
    m_rich->ApplyUnderlineToSelection();

    CPPUNIT_ASSERT(m_rich->IsSelectionUnderlined());

    m_rich->BeginUnderline();
    m_rich->AddParagraph("underline paragraph");
    m_rich->EndUnderline();
    m_rich->AddParagraph("not underline paragraph");

    m_rich->SetSelection(20, 25);

    CPPUNIT_ASSERT(m_rich->IsSelectionUnderlined());

    m_rich->SetSelection(40, 45);

    CPPUNIT_ASSERT(!m_rich->IsSelectionUnderlined());
}

void RichTextCtrlTestCase::Indent()
{
    m_rich->BeginLeftIndent(12, -5);
    m_rich->BeginRightIndent(14);
    m_rich->AddParagraph("A paragraph with indents");
    m_rich->EndLeftIndent();
    m_rich->EndRightIndent();
    m_rich->AddParagraph("No more indent");

    wxTextAttr indent;
    m_rich->GetStyle(5, indent);

    CPPUNIT_ASSERT_EQUAL(12, indent.GetLeftIndent());
    CPPUNIT_ASSERT_EQUAL(-5, indent.GetLeftSubIndent());
    CPPUNIT_ASSERT_EQUAL(14, indent.GetRightIndent());

    m_rich->GetStyle(35, indent);

    CPPUNIT_ASSERT_EQUAL(0, indent.GetLeftIndent());
    CPPUNIT_ASSERT_EQUAL(0, indent.GetLeftSubIndent());
    CPPUNIT_ASSERT_EQUAL(0, indent.GetRightIndent());
}

void RichTextCtrlTestCase::LineSpacing()
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

    CPPUNIT_ASSERT_EQUAL(20, spacing.GetLineSpacing());

    m_rich->GetStyle(20, spacing);

    CPPUNIT_ASSERT_EQUAL(15, spacing.GetLineSpacing());

    m_rich->GetStyle(30, spacing);

    CPPUNIT_ASSERT_EQUAL(10, spacing.GetLineSpacing());
}

void RichTextCtrlTestCase::ParagraphSpacing()
{
    m_rich->BeginParagraphSpacing(15, 20);
    m_rich->AddParagraph("spaced paragraph");
    m_rich->EndParagraphSpacing();
    m_rich->AddParagraph("non-spaced paragraph");

    wxTextAttr spacing;
    m_rich->GetStyle(5, spacing);

    CPPUNIT_ASSERT_EQUAL(15, spacing.GetParagraphSpacingBefore());
    CPPUNIT_ASSERT_EQUAL(20, spacing.GetParagraphSpacingAfter());

    m_rich->GetStyle(25, spacing);

    //Make sure we test against the defaults
    CPPUNIT_ASSERT_EQUAL(m_rich->GetBasicStyle().GetParagraphSpacingBefore(),
                         spacing.GetParagraphSpacingBefore());
    CPPUNIT_ASSERT_EQUAL(m_rich->GetBasicStyle().GetParagraphSpacingAfter(),
                         spacing.GetParagraphSpacingAfter());
}

void RichTextCtrlTestCase::TextColour()
{
    m_rich->BeginTextColour(*wxRED);
    m_rich->AddParagraph("red paragraph");
    m_rich->EndTextColour();
    m_rich->AddParagraph("default paragraph");

    wxTextAttr colour;
    m_rich->GetStyle(5, colour);

    CPPUNIT_ASSERT_EQUAL(*wxRED, colour.GetTextColour());

    m_rich->GetStyle(25, colour);

    CPPUNIT_ASSERT_EQUAL(m_rich->GetBasicStyle().GetTextColour(),
                         colour.GetTextColour());
}

void RichTextCtrlTestCase::NumberedBullet()
{
    m_rich->BeginNumberedBullet(1, 15, 20);
    m_rich->AddParagraph("bullet one");
    m_rich->EndNumberedBullet();
    m_rich->BeginNumberedBullet(2, 25, -5);
    m_rich->AddParagraph("bullet two");
    m_rich->EndNumberedBullet();

    wxTextAttr bullet;
    m_rich->GetStyle(5, bullet);

    CPPUNIT_ASSERT(bullet.HasBulletStyle());
    CPPUNIT_ASSERT(bullet.HasBulletNumber());
    CPPUNIT_ASSERT_EQUAL(1, bullet.GetBulletNumber());
    CPPUNIT_ASSERT_EQUAL(15, bullet.GetLeftIndent());
    CPPUNIT_ASSERT_EQUAL(20, bullet.GetLeftSubIndent());

    m_rich->GetStyle(15, bullet);

    CPPUNIT_ASSERT(bullet.HasBulletStyle());
    CPPUNIT_ASSERT(bullet.HasBulletNumber());
    CPPUNIT_ASSERT_EQUAL(2, bullet.GetBulletNumber());
    CPPUNIT_ASSERT_EQUAL(25, bullet.GetLeftIndent());
    CPPUNIT_ASSERT_EQUAL(-5, bullet.GetLeftSubIndent());
}

void RichTextCtrlTestCase::SymbolBullet()
{
    m_rich->BeginSymbolBullet("*", 15, 20);
    m_rich->AddParagraph("bullet one");
    m_rich->EndSymbolBullet();
    m_rich->BeginSymbolBullet("%", 25, -5);
    m_rich->AddParagraph("bullet two");
    m_rich->EndSymbolBullet();

    wxTextAttr bullet;
    m_rich->GetStyle(5, bullet);

    CPPUNIT_ASSERT(bullet.HasBulletStyle());
    CPPUNIT_ASSERT(bullet.HasBulletText());
    CPPUNIT_ASSERT_EQUAL("*", bullet.GetBulletText());
    CPPUNIT_ASSERT_EQUAL(15, bullet.GetLeftIndent());
    CPPUNIT_ASSERT_EQUAL(20, bullet.GetLeftSubIndent());

    m_rich->GetStyle(15, bullet);

    CPPUNIT_ASSERT(bullet.HasBulletStyle());
    CPPUNIT_ASSERT(bullet.HasBulletText());
    CPPUNIT_ASSERT_EQUAL("%", bullet.GetBulletText());
    CPPUNIT_ASSERT_EQUAL(25, bullet.GetLeftIndent());
    CPPUNIT_ASSERT_EQUAL(-5, bullet.GetLeftSubIndent());
}

void RichTextCtrlTestCase::FontSize()
{
    m_rich->BeginFontSize(24);
    m_rich->AddParagraph("Large text");
    m_rich->EndFontSize();

    wxTextAttr size;
    m_rich->GetStyle(5, size);

    CPPUNIT_ASSERT(size.HasFontSize());
    CPPUNIT_ASSERT_EQUAL(24, size.GetFontSize());
}

void RichTextCtrlTestCase::Font()
{
    wxFont font(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    m_rich->BeginFont(font);
    m_rich->AddParagraph("paragraph with font");
    m_rich->EndFont();

    wxTextAttr fontstyle;
    m_rich->GetStyle(5, fontstyle);

    CPPUNIT_ASSERT_EQUAL(font, fontstyle.GetFont());
}

void RichTextCtrlTestCase::Delete()
{
    m_rich->AddParagraph("here is a long long line in a paragraph");
    m_rich->SetSelection(0, 6);

    CPPUNIT_ASSERT(m_rich->CanDeleteSelection());

    m_rich->DeleteSelection();

    CPPUNIT_ASSERT_EQUAL("is a long long line in a paragraph", m_rich->GetValue());

    m_rich->SetSelection(0, 5);

    CPPUNIT_ASSERT(m_rich->CanDeleteSelection());

    m_rich->DeleteSelectedContent();

    CPPUNIT_ASSERT_EQUAL("long long line in a paragraph", m_rich->GetValue());

    m_rich->Delete(wxRichTextRange(14, 29));

    CPPUNIT_ASSERT_EQUAL("long long line", m_rich->GetValue());
}

void RichTextCtrlTestCase::Url()
{
    m_rich->BeginURL("http://www.wxwidgets.org");
    m_rich->WriteText("http://www.wxwidgets.org");
    m_rich->EndURL();

    wxTextAttr url;
    m_rich->GetStyle(5, url);

    CPPUNIT_ASSERT(url.HasURL());
    CPPUNIT_ASSERT_EQUAL("http://www.wxwidgets.org", url.GetURL());
}

#endif //wxUSE_RICHTEXT
