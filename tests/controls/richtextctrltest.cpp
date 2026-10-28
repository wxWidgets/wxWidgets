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
#include "wx/richtext/richtextxml.h"
#include "wx/mstream.h"
#include "wx/image.h"
#include "wx/imagpng.h"
#include "wx/uiaction.h"

#if wxUSE_PRINTING_ARCHITECTURE
    #include "wx/cmndata.h"
#endif

#if wxUSE_CLIPBOARD && wxUSE_DATAOBJ && !defined(__WXOSX__)
    #include "wx/clipbrd.h"
    #include "wx/dataobj.h"
#endif // wxUSE_CLIPBOARD && wxUSE_DATAOBJ && !defined(__WXOSX__)

#include "testableframe.h"
#include "asserthelper.h"
#include "waitfor.h"

#include <memory>
#include <limits>

class RefreshCountingRichTextCtrl : public wxRichTextCtrl
{
public:
    RefreshCountingRichTextCtrl(wxWindow *parent)
        : wxRichTextCtrl(parent, wxID_ANY, "", wxDefaultPosition,
                         wxSize(400, 200), wxWANTS_CHARS)
    {
    }

    void Refresh(bool eraseBackground = true,
                 const wxRect *rect = nullptr) override
    {
        m_refreshCount++;
        wxRichTextCtrl::Refresh(eraseBackground, rect);
    }

    int GetRefreshCount() const
    {
        return m_refreshCount;
    }

    void ResetRefreshCount()
    {
        m_refreshCount = 0;
    }
private:
    int m_refreshCount = 0;
};

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

wxDataFormat RTFClipboardFormat()
{
#ifdef __WXMSW__
    return wxDataFormat("Rich Text Format");
#else
    return wxDataFormat("text/rtf");
#endif
}

// Returns the RTF clipboard data, which is NUL-terminated only on Windows.
bool GetClipboardRTF(const wxCustomDataObject& data, wxString& rtf)
{
    size_t size = data.GetSize();
    const char* const bytes = static_cast<const char*>(data.GetData());
    if ( !bytes )
        return false;
#ifdef __WXMSW__
    if ( !size || bytes[size - 1] != '\0' )
        return false;
    --size;
#endif
    rtf = wxString::FromUTF8(bytes, size);
    return true;
}

bool ClipboardContainsRTF(const wxString& text)
{
    wxClipboardLocker lock;

    if ( !lock )
        return false;

    wxCustomDataObject data(RTFClipboardFormat());
    wxString rtf;
    if ( !wxTheClipboard->GetData(data) || !GetClipboardRTF(data, rtf) )
        return false;

    return rtf.StartsWith("{\\rtf1") && rtf.Contains(text);
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
        // Other programs, e.g. a clipboard history, may briefly open the
        // clipboard after each change: retry opening it.
        bool rtfOffered = true;
        REQUIRE(WaitFor("wxRichTextCtrl clipboard formats", [&]()
        {
            wxClipboardLocker lock;
            if ( !lock )
                return false;
            rtfOffered = wxTheClipboard->IsSupported(RTFClipboardFormat());
            return true;
        }));
        CHECK_FALSE(rtfOffered);
        m_rich->Clear();
        CHECK(m_rich->IsEmpty());

        REQUIRE(waitForPaste());
        CHECK(m_rich->GetValue() == text);
    }
#endif
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::EnableRTF",
                 "[richtextctrl][rtf]")
{
    CHECK_FALSE(m_rich->GetRTFEnabled());
    wxRichTextCtrl uncreated;
    CHECK_FALSE(uncreated.GetRTFEnabled());
    uncreated.EnableRTF(true);
    REQUIRE(uncreated.Create(wxTheApp->GetTopWindow(), wxID_ANY));
    CHECK(uncreated.GetRTFEnabled());

    const wxString allSaveFilters = wxRichTextBuffer::GetExtWildcard(false, true);
    const wxString defaultFilters = m_rich->GetExtWildcard(false, true);
    for ( bool enabled : { false, true, false } )
    {
        m_rich->EnableRTF(enabled);
        CHECK(m_rich->GetRTFEnabled() == enabled);
        // Content replacement must not reset a control preference.
        m_rich->SetValue("text");
        CHECK(m_rich->GetRTFEnabled() == enabled);

        wxArrayInt types;
        const wxString filters = m_rich->GetExtWildcard(false, true, &types);
        CHECK(filters.Contains("*.rtf") == enabled);
        CHECK((types.Index(wxRICHTEXT_TYPE_RTF) != wxNOT_FOUND) == enabled);
        CHECK(types.Index(wxRICHTEXT_TYPE_TEXT) != wxNOT_FOUND);
        const wxArrayString parts = wxSplit(filters, '|');
        REQUIRE(parts.size() == 2 * types.size());
        for ( size_t i = 0; i < types.size(); ++i )
        {
            auto handler = wxRichTextBuffer::FindHandler(static_cast<wxRichTextFileType>(types[i]));
            REQUIRE(handler);
            CHECK(parts[2 * i + 1] == "*." + handler->GetExtension());
        }
        CHECK(filters == (enabled ? allSaveFilters : defaultFilters));
        CHECK(m_rich->GetExtWildcard(true, true, &types).Contains("*.rtf") == enabled);
        CHECK(types.empty());
        CHECK_FALSE(m_rich->GetExtWildcard(false, false, &types).Contains("*.rtf"));
        CHECK(types.Index(wxRICHTEXT_TYPE_RTF) == wxNOT_FOUND);
        CHECK_FALSE(m_rich->GetExtWildcard(true, false).Contains("*.rtf"));

        // Neither the other control nor the global handler list is changed.
        CHECK(uncreated.GetRTFEnabled());
        CHECK(uncreated.GetExtWildcard(false, true) == allSaveFilters);
        CHECK(wxRichTextBuffer::GetExtWildcard(false, true) == allSaveFilters);
        m_rich->Clear();
        CHECK(m_rich->GetRTFEnabled() == enabled);
    }
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::RTFClipboardOptIn",
                 "[richtextctrl][rtf][clipboard]")
{
#if wxUSE_CLIPBOARD && wxUSE_DATAOBJ && !defined(__WXOSX__)
#if wxUSE_XML
    // Exercise the native format alongside RTF without leaving a new handler
    // registered for later tests.
    struct XMLHandlerScope
    {
        bool added = false;
        XMLHandlerScope()
        {
            if ( !wxRichTextBuffer::FindHandler(wxRICHTEXT_TYPE_XML) )
            {
                wxRichTextBuffer::AddHandler(new wxRichTextXMLHandler);
                added = true;
            }
        }
        ~XMLHandlerScope()
        {
            if ( added )
                wxRichTextBuffer::RemoveHandler("XML");
        }
    } xmlHandler;
#endif
    bool cut = false;
    SECTION("Copy") { }
    SECTION("Cut") { cut = true; }
    const wxString text("text");
    for ( bool enabled : { false, true, false } )
    {
        m_rich->EnableRTF(enabled);
        m_rich->SetValue(text);
        m_rich->SelectAll();
#if wxUSE_XML
        // The native payload must still be the original CopyFragment() result,
        // without the source container's basic style, whether RTF is on or off.
        auto nativeBuffer = new wxRichTextBuffer;
        wxRichTextBufferDataObject expectedNative(nativeBuffer);
        REQUIRE(m_rich->GetBuffer().CopyFragment(
            m_rich->GetInternalSelectionRange(), *nativeBuffer));
        const size_t nativeSize = expectedNative.GetDataSize();
        REQUIRE(nativeSize > 0);
        wxCharBuffer nativeBytes(nativeSize);
        REQUIRE(expectedNative.GetDataHere(nativeBytes.data()));
#endif
        REQUIRE(WaitFor("RTF opt-in clipboard setup",
                        [&]() { return SetClipboardText("sentinel"); }));
        REQUIRE(WaitFor("RTF opt-in clipboard update", [&]()
        {
            if ( cut )
                m_rich->Cut();
            else
                m_rich->Copy();
            return ClipboardContainsText(text);
        }));
        // Other programs, e.g. a clipboard history, may briefly open the
        // clipboard after each change: retry opening and reading it.
        bool rtfOffered = !enabled;
#if wxUSE_XML
        wxCustomDataObject native{wxDataFormat(
            wxRichTextBufferDataObject::GetRichTextBufferFormatId())};
#endif
        const auto readClipboard = [&]()
        {
            wxClipboardLocker lock;
            if ( !lock )
                return false;
            rtfOffered = wxTheClipboard->IsSupported(RTFClipboardFormat());
#if wxUSE_XML
            return wxTheClipboard->GetData(native);
#else
            return true;
#endif
        };
        REQUIRE(WaitFor("RTF opt-in clipboard contents", readClipboard));
        CHECK(rtfOffered == enabled);
#if wxUSE_XML
        REQUIRE(native.GetSize() == nativeSize);
        CHECK(memcmp(native.GetData(), nativeBytes.data(), nativeSize) == 0);
#endif
        if ( enabled )
        {
            CHECK(WaitFor("RTF clipboard data",
                          [&]() { return ClipboardContainsRTF(text); }));
        }
        CHECK(m_rich->GetValue() == (cut ? wxString() : text));
    }
#endif
}

namespace
{

// Compare complete documents, not isolated control words. The fixed base
// face avoids depending on the platform's default GUI font. All expected
// control words and converted dimensions below are literal test data.
class RTFExportTestCase
{
public:
    RTFExportTestCase()
    {
        wxRichTextAttr basic;
        basic.SetFontFaceName("Arial");
        buffer.SetBasicStyle(basic);
    }

    wxRichTextParagraph* AddText(const wxString& text,
                                const wxRichTextAttr& attr = wxRichTextAttr())
    {
        buffer.AddParagraph(text);
        auto para = wxDynamicCast(buffer.GetChildren().GetLast()->GetData(),
                                  wxRichTextParagraph);
        para->GetChildren().GetFirst()->GetData()->SetAttributes(attr);
        return para;
    }

    // Layout tests use explicit fonts in nested containers too: a cell or
    // text box does not inherit its font from the outer buffer.
    wxRichTextBox* CreateTestBox()
    {
        auto box = new wxRichTextBox;
        box->GetAttributes().SetFontFaceName("Arial");
        return box;
    }

    wxRichTextTable* CreateTestTable(int rows, int cols)
    {
        auto table = new wxRichTextTable;
        REQUIRE(table->CreateTable(rows, cols));
        for ( int row = 0; row < rows; ++row )
            for ( int col = 0; col < cols; ++col )
                table->GetCell(row, col)->GetAttributes().SetFontFaceName("Arial");
        return table;
    }

    // Stores a table or text box in a paragraph of its own, as
    // wxRichTextCtrl::WriteTable() and WriteTextBox() do: the layout of a
    // container only handles paragraphs.
    template <typename T>
    static T* Anchor(wxRichTextParagraphLayoutBox& container, T* object)
    {
        auto para = new wxRichTextParagraph(&container);
        para->AppendChild(object);
        container.AppendChild(para);
        return object;
    }

    void CheckRTF(const wxString& body, const wxString& colours = wxString(),
                  const wxString& fonts = wxString(), const wxString& lists = wxString())
    {
        const wxString expected =
            "{\\rtf1\\ansi\\ansicpg1252\\deff0\\uc1\n"
            "{\\fonttbl{\\f0\\fnil Arial;}" + fonts + "}\n"
            "{\\colortbl;" + colours + "}\n" + lists +
            "\\viewkind4\\deftab567\\dntblnsbdb\\expshrtn\\paperw12240\\paperh15840"
            "\\margl1440\\margr1440\\margt1440\\margb1440\n" + body + "}";
        wxString actual;
        REQUIRE(buffer.GetAsRTF(actual));
        CHECK(actual == expected);
    }

    void CheckParagraph(const wxString& character = wxString(),
                        const wxString& paragraph = wxString(),
                        const wxString& colours = wxString(),
                        const wxString& fonts = wxString(),
                        const wxString& bullet = wxString(), const wxString& lists = wxString())
    {
        CheckRTF("{\\pard\\itap0" + paragraph + " " + bullet +
                 "{\\plain" + character + " text}\\par\n}", colours, fonts, lists);
    }

    void CheckPageRTF(const wxString& page, const wxString& body,
                      const wxPageSetupDialogData* pageSetup = nullptr)
    {
        const wxString expected =
            "{\\rtf1\\ansi\\ansicpg1252\\deff0\\uc1\n"
            "{\\fonttbl{\\f0\\fnil Arial;}}\n{\\colortbl;}\n"
            "\\viewkind4\\deftab567\\dntblnsbdb\\expshrtn" + page + "\n" + body + "}";
        wxString actual;
        REQUIRE(buffer.GetAsRTF(actual, pageSetup));
        CHECK(actual == expected);
    }

    // The marker is followed by a tab (follow 0), or by a space (1) if the
    // list has no hanging indent.
    wxString ListDefinition(int id, int format, int start, const wxString& text,
                            const wxString& positions, const wxString& style,
                            int alignment = 0, int follow = 0)
    {
        return wxString::Format("{\\list\\listtemplateid%d{\\listlevel\\levelnfc%d\\levelnfcn%d"
                                "\\leveljc%d\\leveljcn%d\\levelfollow%d\\levelstartat%d",
                                id, format, format, alignment, alignment, follow, start) +
               "{\\leveltext" + text + ";}{\\levelnumbers" + positions + ";}" +
               style + wxString::Format("}\\listid%d}", id);
    }

    wxString ListTables(const wxString& definitions, int count = 1)
    {
        wxString tables = "{\\*\\listtable" + definitions + "}\n{\\*\\listoverridetable";
        for ( int id = 1; id <= count; ++id )
            tables += wxString::Format("{\\listoverride\\listid%d\\listoverridecount0\\ls%d}", id, id);
        return tables + "}\n";
    }

    // The text box story followed by the in-flow shape result shown by
    // readers without shape support, and the end of the shape group.
    static wxString ShapeText(const wxString& story, const wxString& fallback)
    {
        return "{\\shptxt " + story + "}}{\\shprslt " + fallback + "}}";
    }

    static wxString ShapeText(const wxString& story)
    {
        return ShapeText(story, story);
    }

    // The paragraph of minimal height following a text box written as a
    // block, which it is anchored to. At the end of a cell, it ends the cell.
    static wxString ShapeAnchor(const wxString& end = "\\par", int depth = 0)
    {
        return (depth ? wxString::Format("{\\pard\\intbl\\itap%d ", depth)
                      : wxString("{\\pard\\itap0 ")) +
               "\\plain\\fs1\\sl-1\\slmult0" + end + "}";
    }

    // The data of a picture group: two hexadecimal digits per byte, 32 bytes
    // per line, starting on a new line.
    static wxString PictureData(const wxRichTextImageBlock& block)
    {
        wxString data = "\n";
        const unsigned char* const bytes = block.GetData();
        for ( size_t n = 0; n < block.GetDataSize(); ++n )
        {
            data += wxString::Format("%02x", bytes[n]);
            if ( (n + 1) % 32 == 0 )
                data += "\n";
        }
        return data;
    }

#if wxUSE_LIBPNG && wxUSE_STREAMS
    // A fixed 2x4 red PNG, independent of the platform's PNG encoder.
    static wxRichTextImageBlock TestPNG()
    {
        if ( !wxImage::FindHandler(wxBITMAP_TYPE_PNG) )
            wxImage::AddHandler(new wxPNGHandler);
        const char png[] =
            "89504E470D0A1A0A0000000D49484452000000020000000408020000002B8D796E0000"
            "001149444154789C63F8CFC0F09F018C31280077A407F96103238B0000000049454E"
            "44AE426082";
        wxMemoryInputStream input(png, sizeof(png) - 1);
        wxRichTextImageBlock block;
        REQUIRE(block.ReadHex(input, sizeof(png) - 1, wxBITMAP_TYPE_PNG));
        return block;
    }

    // The picture data of TestPNG(), as literal expected output.
    static wxString TestPNGData()
    {
        return "\n89504e470d0a1a0a0000000d49484452000000020000000408020000002b8d79\n"
               "6e0000001149444154789c63f8cfc0f09f018c31280077a407f96103238b0000\n"
               "000049454e44ae426082";
    }
#endif // wxUSE_LIBPNG && wxUSE_STREAMS

    wxString PlainTextBoxProperties(bool autoHeight = false)
    {
        return wxString("{\\sp{\\sn fPseudoInline}{\\sv 0}}") +
               (autoHeight ? "{\\sp{\\sn fFitShapeToText}{\\sv 1}}" : "") +
               "{\\sp{\\sn fLine}{\\sv 0}}"
               "{\\sp{\\sn dxTextLeft}{\\sv 0}}{\\sp{\\sn dyTextTop}{\\sv 0}}"
               "{\\sp{\\sn dxTextRight}{\\sv 0}}{\\sp{\\sn dyTextBottom}{\\sv 0}}"
               "{\\sp{\\sn anchorText}{\\sv 0}}{\\sp{\\sn fFilled}{\\sv 0}}";
    }

    wxRichTextBuffer buffer;
};

} // anonymous namespace

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFClipboardBasicStyle",
                 "[richtextctrl][rtf][clipboard]")
{
#if wxUSE_CLIPBOARD && wxUSE_DATAOBJ && !defined(__WXOSX__)
    wxRichTextAttr basic;
    basic.SetFontFaceName("Arial");
    basic.SetFontSize(18);
    basic.SetFontWeight(wxFONTWEIGHT_BOLD);
    basic.SetTextColour(wxColour(255, 0, 0));
    basic.SetAlignment(wxTEXT_ALIGNMENT_CENTRE);
    buffer.SetBasicStyle(basic);
    AddText("text");
    buffer.UpdateRanges();
    wxRichTextRange range = buffer.GetOwnRange();
    wxString body = "{\\pard\\itap0\\qc {\\plain\\f0\\fs36\\b\\cf1 text}\\par\n}";
    SECTION("Complete paragraph") { }
    SECTION("Partial paragraph")
    {
        range = wxRichTextRange(1, 2);
        body = "\\pard\\itap0\\qc {\\plain\\f0\\fs36\\b\\cf1 ex}\\pard\\itap0\\qc ";
    }
    REQUIRE(WaitFor("RTF basic-style clipboard copy", [&]()
    {
        return buffer.CopyToClipboard(range);
    }));
    // Another program may briefly have the clipboard open after the change.
    wxCustomDataObject data(RTFClipboardFormat());
    REQUIRE(WaitFor("RTF basic-style clipboard data", [&]()
    {
        wxClipboardLocker lock;
        return !!lock && wxTheClipboard->GetData(data);
    }));
    wxString actual;
    REQUIRE(GetClipboardRTF(data, actual));
    const wxString expected =
        "{\\rtf1\\ansi\\ansicpg1252\\deff0\\uc1\n"
        "{\\fonttbl{\\f0\\fnil Arial;}}\n{\\colortbl;\\red255\\green0\\blue0;}\n"
        "\\viewkind4\\deftab567\\dntblnsbdb\\expshrtn\\paperw12240\\paperh15840"
        "\\margl1440\\margr1440\\margt1440\\margb1440\n" + body + "}";
    CHECK(actual == expected);
    CHECK(buffer.GetBasicStyle() == basic);
#endif
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFDefaultPage", "[richtextctrl][rtf]")
{
    AddText("text");
    CheckPageRTF("\\paperw12240\\paperh15840\\margl1440\\margr1440\\margt1440\\margb1440",
                 "{\\pard\\itap0 {\\plain\\f0 text}\\par\n}");
    wxString implicit, explicitNull;
    REQUIRE(buffer.GetAsRTF(implicit));
    REQUIRE(buffer.GetAsRTF(explicitNull, nullptr));
    CHECK(implicit == explicitNull);
}

#if wxUSE_PRINTING_ARCHITECTURE
TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFPageSettings", "[richtextctrl][rtf]")
{
    struct PaperCase
    {
        wxPaperSize paper;
        wxPrintOrientation orientation;
        const char* header;
        int width, height; // Literal expected twips, not production conversions.
    };
    const PaperCase papers[] =
    {
        { wxPAPER_A4, wxPORTRAIT, "\\paperw11906\\paperh16838", 11906, 16838 },
        { wxPAPER_A4, wxLANDSCAPE, "\\paperw16838\\paperh11906\\landscape", 16838, 11906 },
        { wxPAPER_LETTER, wxPORTRAIT, "\\paperw12240\\paperh15840", 12240, 15840 },
        { wxPAPER_LETTER, wxLANDSCAPE, "\\paperw15840\\paperh12240\\landscape", 15840, 12240 }
    };
    struct MarginCase
    {
        bool useDefault;
        wxPoint topLeft, bottomRight;
        const char* header;
        int left, right, top, bottom;
    };
    const MarginCase margins[] =
    {
        { true, {0, 0}, {0, 0}, "\\margl0\\margr0\\margt0\\margb0", 0, 0, 0, 0 },
        { false, {10, 10}, {10, 10}, "\\margl567\\margr567\\margt567\\margb567", 567, 567, 567, 567 },
        { false, {10, 15}, {20, 25}, "\\margl567\\margr1134\\margt850\\margb1417", 567, 1134, 850, 1417 }
    };
    AddText("text");
    auto box = CreateTestBox();
    auto& attr = box->GetAttributes().GetTextBoxAttr();
    attr.GetWidth().SetValue(50, wxTEXT_ATTR_UNITS_PERCENTAGE);
    attr.GetHeight().SetValue(50, wxTEXT_ATTR_UNITS_PERCENTAGE);
    attr.SetFloatMode(wxTEXT_BOX_ATTR_FLOAT_RIGHT);
    box->AddParagraph("box");
    Anchor(buffer, box);

    for ( const auto& paper : papers )
    {
        for ( const auto& margin : margins )
        {
            INFO(paper.header << margin.header);
            wxPageSetupDialogData setup;
            setup.SetPaperSize(paper.paper);
            setup.GetPrintData().SetOrientation(paper.orientation);
            if ( !margin.useDefault )
            {
                setup.SetMarginTopLeft(margin.topLeft);
                setup.SetMarginBottomRight(margin.bottomRight);
            }
            const wxSize originalSize = setup.GetPaperSize();
            const int usableWidth = paper.width - margin.left - margin.right;
            const int usableHeight = paper.height - margin.top - margin.bottom;
            const int boxWidth = (usableWidth + 1) / 2;
            const int boxHeight = (usableHeight + 1) / 2;
            const wxString body =
                "{\\pard\\itap0 {\\plain\\f0 text}\\par\n}" +
                wxString::Format("{\\shp{\\*\\shpinst\\shpleft%d\\shptop0\\shpright%d\\shpbottom%d",
                                 usableWidth - boxWidth, usableWidth, boxHeight) +
                "\\shpbxcolumn\\shpbypara\\shpwr2\\shpwrk1{\\sp{\\sn shapeType}{\\sv 202}}" +
                PlainTextBoxProperties() +
                "{\\sp{\\sn dxWrapDistLeft}{\\sv 0}}{\\sp{\\sn dxWrapDistRight}{\\sv 0}}" +
                ShapeText("{\\pard\\itap0 {\\plain\\f0 box}\\par\n}") + ShapeAnchor();
            CheckPageRTF(wxString(paper.header) + margin.header, body, &setup);
            CHECK(setup.GetPaperSize() == originalSize);
            CHECK(setup.GetPaperId() == paper.paper);
            CHECK(setup.GetPrintData().GetOrientation() == paper.orientation);
            CHECK(setup.GetMarginTopLeft() == margin.topLeft);
            CHECK(setup.GetMarginBottomRight() == margin.bottomRight);
        }
    }
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFPageSizeResolution", "[richtextctrl][rtf]")
{
    wxPageSetupDialogData setup;
    setup.SetPaperSize(wxPAPER_A4);
    wxString paper = "\\paperw11906\\paperh16838";
    SECTION("Default margins with selected paper") { }
    SECTION("Default constructed settings use Letter fallback and zero margins")
    {
        setup = wxPageSetupDialogData();
        paper = "\\paperw12240\\paperh15840";
    }
    SECTION("Letter fallback retains landscape and supplied margins")
    {
        setup = wxPageSetupDialogData();
        setup.GetPrintData().SetOrientation(wxLANDSCAPE);
        setup.SetMarginTopLeft(wxPoint(10, 15));
        setup.SetMarginBottomRight(wxPoint(20, 25));
        AddText("text");
        CheckPageRTF("\\paperw15840\\paperh12240\\landscape"
                     "\\margl567\\margr1134\\margt850\\margb1417",
                     "{\\pard\\itap0 {\\plain\\f0 text}\\par\n}", &setup);
        return;
    }
    SECTION("Paper id without cached dimensions")
    {
        setup = wxPageSetupDialogData();
        setup.SetPaperId(wxPAPER_A4);
    }
    SECTION("Explicit custom paper size")
    {
        setup.SetPaperSize(wxSize(200, 300));
        paper = "\\paperw11339\\paperh17008";
    }
    SECTION("Custom paper overrides stale standard id")
    {
        setup.SetPaperSize(wxPAPER_LETTER);
        setup.SetPaperSize(wxSize(200, 300));
        paper = "\\paperw11339\\paperh17008";
    }
    SECTION("Custom size stored in print data")
    {
        setup = wxPageSetupDialogData();
        setup.GetPrintData().SetPaperSize(wxSize(200, 300));
        paper = "\\paperw11339\\paperh17008";
    }
    SECTION("Custom landscape")
    {
        setup.SetPaperSize(wxSize(200, 300));
        setup.GetPrintData().SetOrientation(wxLANDSCAPE);
        paper = "\\paperw17008\\paperh11339\\landscape";
    }
    SECTION("Landscape with paper id but no cached dimensions")
    {
        setup = wxPageSetupDialogData();
        setup.SetPaperId(wxPAPER_LETTER);
        setup.GetPrintData().SetOrientation(wxLANDSCAPE);
        paper = "\\paperw15840\\paperh12240\\landscape";
    }
    SECTION("Printer and dialog settings do not influence export")
    {
        setup.GetPrintData().SetPrinterName("Nonexistent printer for RTF export test");
        setup.SetMinMarginTopLeft(wxPoint(100, 100));
        setup.SetMinMarginBottomRight(wxPoint(100, 100));
        setup.EnableMargins(false);
        setup.SetDefaultMinMargins(true);
    }
    AddText("text");
    CheckPageRTF(paper + "\\margl0\\margr0\\margt0\\margb0",
                 "{\\pard\\itap0 {\\plain\\f0 text}\\par\n}", &setup);
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFInvalidPageSettings", "[richtextctrl][rtf]")
{
    wxPageSetupDialogData setup;
    setup.SetPaperSize(wxPAPER_A4);
    SECTION("Negative left margin") { setup.SetMarginTopLeft(wxPoint(-1, 0)); }
    SECTION("Negative top margin") { setup.SetMarginTopLeft(wxPoint(0, -1)); }
    SECTION("Negative right margin") { setup.SetMarginBottomRight(wxPoint(-1, 0)); }
    SECTION("Negative bottom margin") { setup.SetMarginBottomRight(wxPoint(0, -1)); }
    SECTION("Margins consume width")
    {
        setup.SetMarginTopLeft(wxPoint(100, 0));
        setup.SetMarginBottomRight(wxPoint(110, 0));
    }
    SECTION("Margins exceed height") { setup.SetMarginBottomRight(wxPoint(0, 300)); }
    SECTION("Zero custom width") { setup.SetPaperSize(wxSize(0, 300)); }
    SECTION("Negative custom height") { setup.SetPaperSize(wxSize(200, -2)); }
    SECTION("Overflowing margin")
    {
        setup.SetMarginTopLeft(wxPoint(std::numeric_limits<int>::max(), 0));
    }
    SECTION("Overflowing custom size in print data")
    {
        setup = wxPageSetupDialogData();
        setup.GetPrintData().SetPaperSize(wxSize(std::numeric_limits<int>::max(), 300));
    }
    AddText("text");
    wxString rtf = "unchanged";
    CHECK_FALSE(buffer.GetAsRTF(rtf, &setup));
    CHECK(rtf == "unchanged");
}
#endif // wxUSE_PRINTING_ARCHITECTURE

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFTextAttributes",
                 "[richtextctrl][rtf]")
{
    wxRichTextAttr attr;
    wxString character = "\\f0";
    wxString colours, fonts;

    SECTION("No explicit character attributes") {}
    SECTION("Font face")
    {
        attr.SetFontFaceName("Courier New");
        character = "\\f1";
        fonts = "{\\f1\\fnil Courier New;}";
    }
    SECTION("Point size")
    {
        attr.SetFontSize(13);
        character += "\\fs26";
    }
    SECTION("Pixel size at 96 dpi")
    {
        attr.SetFontPixelSize(16);
        character += "\\fs24";
    }
    // As set from a font without a size, e.g. wxNullFont in wxQt: RTF sizes
    // must be positive, so it is inherited.
    SECTION("Point size without a value") { attr.SetFontPointSize(-1); }
    SECTION("Pixel size without a value") { attr.SetFontPixelSize(0); }
    SECTION("Bold")
    {
        attr.SetFontWeight(wxFONTWEIGHT_BOLD);
        character += "\\b";
    }
    SECTION("Normal weight") { attr.SetFontWeight(wxFONTWEIGHT_NORMAL); }
    SECTION("Italic")
    {
        attr.SetFontStyle(wxFONTSTYLE_ITALIC);
        character += "\\i";
    }
    SECTION("Slant")
    {
        attr.SetFontStyle(wxFONTSTYLE_SLANT);
        character += "\\i";
    }
    SECTION("Normal style") { attr.SetFontStyle(wxFONTSTYLE_NORMAL); }
    SECTION("Underline")
    {
        attr.SetFontUnderlined(true);
        character += "\\ul";
    }
    SECTION("Double underline")
    {
        attr.SetFontUnderlined(wxTEXT_ATTR_UNDERLINE_DOUBLE);
        character += "\\uldb";
    }
    SECTION("Special underline")
    {
        attr.SetFontUnderlined(wxTEXT_ATTR_UNDERLINE_SPECIAL);
        character += "\\ulwave";
    }
    SECTION("No underline") { attr.SetFontUnderlined(false); }
    SECTION("Underline colour")
    {
        attr.SetFontUnderlined(wxTEXT_ATTR_UNDERLINE_SOLID, wxColour(12, 34, 56));
        character += "\\ul\\ulc1";
        colours = "\\red12\\green34\\blue56;";
    }
    SECTION("Strikethrough")
    {
        attr.SetFontStrikethrough(true);
        character += "\\strike";
    }
    SECTION("No strikethrough") { attr.SetFontStrikethrough(false); }
    SECTION("Foreground colour")
    {
        attr.SetTextColour(wxColour(12, 34, 56));
        character += "\\cf1";
        colours = "\\red12\\green34\\blue56;";
    }
    SECTION("Background colour")
    {
        attr.SetBackgroundColour(wxColour(65, 43, 21));
        character += "\\chshdng0\\chcbpat1";
        colours = "\\red65\\green43\\blue21;";
    }

    AddText("text", attr);
    CheckParagraph(character, "", colours, fonts);
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFCombinedTextAttributes",
                 "[richtextctrl][rtf]")
{
    wxRichTextAttr attr;
    attr.SetFontFaceName("Courier New");
    attr.SetFontSize(13);
    attr.SetFontWeight(wxFONTWEIGHT_BOLD);
    attr.SetFontStyle(wxFONTSTYLE_ITALIC);
    attr.SetFontUnderlined(wxTEXT_ATTR_UNDERLINE_DOUBLE, wxColour(0, 0, 255));
    attr.SetFontStrikethrough(true);
    attr.SetTextColour(wxColour(255, 0, 0));
    attr.SetBackgroundColour(wxColour(255, 255, 0));
    auto para = AddText("formatted", attr);
    para->AppendChild(new wxRichTextPlainText(" plain", para));

    CheckRTF("{\\pard\\itap0 "
             "{\\plain\\f1\\fs26\\b\\i\\uldb\\ulc3\\strike\\cf1\\chshdng0\\chcbpat2 formatted}"
             "{\\plain\\f0  plain}\\par\n}",
             "\\red255\\green0\\blue0;\\red255\\green255\\blue0;\\red0\\green0\\blue255;",
             "{\\f1\\fnil Courier New;}");
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFTextEffects",
                 "[richtextctrl][rtf]")
{
    // Known differences, the effects are exported as they are specified: wx
    // draws small capitals as capitals at 3/4 size, including the original
    // capitals, which Word keeps at full size; wx (MSW) doesn't draw double
    // strikethrough, shadow, emboss, outline or engrave; superscript and
    // subscript use the reader's size, which is smaller in Word than wx's 2/3.
    const struct { int effect; const char* rtf; } effects[] =
    {
        { wxTEXT_ATTR_EFFECT_CAPITALS, "\\caps" },
        { wxTEXT_ATTR_EFFECT_SMALL_CAPITALS, "\\scaps" },
        { wxTEXT_ATTR_EFFECT_STRIKETHROUGH, "\\strike" },
        { wxTEXT_ATTR_EFFECT_DOUBLE_STRIKETHROUGH, "\\striked1" },
        { wxTEXT_ATTR_EFFECT_SHADOW, "\\shad" },
        { wxTEXT_ATTR_EFFECT_EMBOSS, "\\embo" },
        { wxTEXT_ATTR_EFFECT_OUTLINE, "\\outl" },
        { wxTEXT_ATTR_EFFECT_ENGRAVE, "\\impr" },
        { wxTEXT_ATTR_EFFECT_SUPERSCRIPT, "\\super" },
        { wxTEXT_ATTR_EFFECT_SUBSCRIPT, "\\sub" },
        { wxTEXT_ATTR_EFFECT_RTL, "\\rtlch" }
    };
    for ( const auto& effect : effects )
    {
        DYNAMIC_SECTION(effect.rtf)
        {
            wxRichTextAttr attr;
            attr.SetTextEffects(effect.effect);
            attr.SetTextEffectFlags(effect.effect);
            AddText("text", attr);
            CheckParagraph(wxString("\\f0") + effect.rtf);
        }
    }
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFColourTableReuse",
                 "[richtextctrl][rtf]")
{
    wxRichTextAttr attr;
    attr.SetTextColour(wxColour(12, 34, 56));
    attr.SetBackgroundColour(wxColour(12, 34, 56));
    attr.SetFontUnderlined(wxTEXT_ATTR_UNDERLINE_SOLID, wxColour(12, 34, 56));
    auto para = AddText("first", attr);
    auto second = new wxRichTextPlainText("second", para);
    second->SetAttributes(attr);
    para->AppendChild(second);
    CheckRTF("{\\pard\\itap0 {\\plain\\f0\\ul\\ulc1\\cf1\\chshdng0\\chcbpat1 first}"
             "{\\plain\\f0\\ul\\ulc1\\cf1\\chshdng0\\chcbpat1 second}\\par\n}",
             "\\red12\\green34\\blue56;");
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFEscapingAndSaving",
                 "[richtextctrl][rtf]")
{
    AddText("{text} \\ \t\n" + wxString(wxUniChar(0x03a9)) + wxUniChar(0x1f600));
    CheckRTF("{\\pard\\itap0 {\\plain\\f0 \\{text\\} \\\\ \\tab \\line "
             "\\u937?\\u-10179?\\u-8704?}\\par\n}");

#if wxUSE_STREAMS
    wxString rtf;
    REQUIRE(buffer.GetAsRTF(rtf));
    wxMemoryOutputStream stream;
    REQUIRE(buffer.SaveFile(stream, wxRICHTEXT_TYPE_RTF));
    const auto bytes = rtf.utf8_str();
    REQUIRE(stream.GetLength() == static_cast<wxFileOffset>(bytes.length()));
    wxCharBuffer saved(bytes.length());
    REQUIRE(stream.CopyTo(saved.data(), bytes.length()) == bytes.length());
    CHECK(memcmp(saved.data(), bytes.data(), bytes.length()) == 0);
#endif
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFHyperlinks",
                 "[richtextctrl][rtf]")
{
    wxString url, instruction;
    SECTION("Plain URL")
    {
        url = "https://example.com/?q=1&r=2";
        instruction = "\"https://example.com/?q=1&r=2\"";
    }
    SECTION("Quotes are escaped inside the field argument")
    {
        url = "https://example.com/a\"b";
        instruction = "\"https://example.com/a\\\\\"b\"";
    }
    SECTION("Backslashes are escaped for the field and for RTF")
    {
        url = "C:\\dir\\file.txt";
        instruction = "\"C:\\\\\\\\dir\\\\\\\\file.txt\"";
    }
    SECTION("Bookmark references use the local link switch")
    {
        url = "#anchor";
        instruction = "\\\\l \"anchor\"";
    }
    wxRichTextAttr attr;
    attr.SetURL(url);
    AddText("text", attr);
    CheckRTF("{\\pard\\itap0 {\\plain\\f0 {\\field{\\*\\fldinst HYPERLINK " + instruction +
             "}{\\fldrslt text}}}\\par\n}");
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFSpecialCharacters",
                 "[richtextctrl][rtf]")
{
    SECTION("Characters with dedicated RTF symbols")
    {
        AddText("a" + wxString(wxUniChar(0x00a0)) + "b" + wxUniChar(0x00ad) +
                "c" + wxUniChar(0x2011) + "d");
        CheckRTF("{\\pard\\itap0 {\\plain\\f0 a\\~b\\-c\\_d}\\par\n}");
    }
    SECTION("Code page fallback for readers without Unicode support")
    {
        // Latin-1 and Windows-1252 specific characters have a code page
        // fallback, others use a question mark.
        AddText(wxString(wxUniChar(0x00e9)) + wxUniChar(0x20ac) + wxUniChar(0x2022) +
                wxUniChar(0x03a9));
        CheckRTF("{\\pard\\itap0 {\\plain\\f0 \\u233\\'e9\\u8364\\'80\\u8226\\'95\\u937?}\\par\n}");
    }
    SECTION("Font table names use Unicode with a code page fallback")
    {
        wxRichTextAttr attr;
        attr.SetFontFaceName("Caf" + wxString(wxUniChar(0x00e9)) + wxUniChar(0x00a0) + "Sans");
        AddText("text", attr);
        CheckParagraph("\\f1", "", "", "{\\f1\\fnil Caf\\u233\\'e9\\u160\\'a0Sans;}");
    }
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFRightToLeftParagraph",
                 "[richtextctrl][rtf]")
{
    auto para = AddText("text");
    para->GetAttributes().SetTextEffects(wxTEXT_ATTR_EFFECT_RTL);
    para->GetAttributes().SetTextEffectFlags(wxTEXT_ATTR_EFFECT_RTL);
    // The paragraph direction needs a paragraph property, while its text
    // inherits the right-to-left character formatting.
    CheckParagraph("\\f0\\rtlch", "\\rtlpar");
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFParagraphProperties",
                 "[richtextctrl][rtf]")
{
    wxRichTextAttr attr;
    wxString paragraph;
    SECTION("Left alignment")
    {
        attr.SetAlignment(wxTEXT_ALIGNMENT_LEFT);
        paragraph = "\\ql";
    }
    SECTION("Centred")
    {
        attr.SetAlignment(wxTEXT_ALIGNMENT_CENTRE);
        paragraph = "\\qc";
    }
    SECTION("Right alignment")
    {
        attr.SetAlignment(wxTEXT_ALIGNMENT_RIGHT);
        paragraph = "\\qr";
    }
    SECTION("Justified")
    {
        // Known difference: wx (MSW) draws justified paragraphs left-aligned.
        attr.SetAlignment(wxTEXT_ALIGNMENT_JUSTIFIED);
        paragraph = "\\qj";
    }
    SECTION("Single line spacing")
    {
        attr.SetLineSpacing(10);
        paragraph = "\\sl240\\slmult1";
    }
    SECTION("One and a half line spacing")
    {
        attr.SetLineSpacing(15);
        paragraph = "\\sl360\\slmult1";
    }
    SECTION("Double line spacing")
    {
        attr.SetLineSpacing(20);
        paragraph = "\\sl480\\slmult1";
    }
    SECTION("Space before")
    {
        attr.SetParagraphSpacingBefore(127);
        paragraph = "\\sb720";
    }
    SECTION("Space after")
    {
        attr.SetParagraphSpacingAfter(254);
        paragraph = "\\sa1440";
    }
    SECTION("Left indent")
    {
        attr.SetLeftIndent(127);
        paragraph = "\\li720\\fi0";
    }
    SECTION("Hanging indent")
    {
        attr.SetLeftIndent(254, 127);
        paragraph = "\\li2160\\fi-720";
    }
    SECTION("First line indent")
    {
        attr.SetLeftIndent(254, -127);
        paragraph = "\\li720\\fi720";
    }
    SECTION("Right indent")
    {
        attr.SetRightIndent(381);
        paragraph = "\\ri2160";
    }
    SECTION("Tabs")
    {
        wxArrayInt tabs;
        tabs.Add(127);
        tabs.Add(254);
        attr.SetTabs(tabs);
        paragraph = "\\tx720\\tx1440";
    }
    SECTION("Page break before")
    {
        attr.SetPageBreak(true);
        paragraph = "\\pagebb";
    }
    SECTION("Keep with next")
    {
        attr.SetFlags(wxTEXT_ATTR_AVOID_PAGE_BREAK_AFTER);
        paragraph = "\\keepn";
    }
    SECTION("Outline level")
    {
        attr.SetOutlineLevel(2);
        paragraph = "\\outlinelevel2";
    }
    SECTION("Suppress hyphenation")
    {
        attr.SetTextEffects(wxTEXT_ATTR_EFFECT_SUPPRESS_HYPHENATION);
        attr.SetTextEffectFlags(wxTEXT_ATTR_EFFECT_SUPPRESS_HYPHENATION);
        paragraph = "\\hyphpar0";
    }
    buffer.AddParagraph("text", &attr);
    CheckParagraph("\\f0", paragraph);
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFCombinedParagraphProperties",
                 "[richtextctrl][rtf]")
{
    wxRichTextAttr attr;
    attr.SetAlignment(wxTEXT_ALIGNMENT_JUSTIFIED);
    attr.SetLeftIndent(254, 127);
    attr.SetRightIndent(127);
    attr.SetParagraphSpacingBefore(127);
    attr.SetParagraphSpacingAfter(254);
    attr.SetLineSpacing(15);
    attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_ARABIC | wxTEXT_ATTR_BULLET_STYLE_PERIOD);
    attr.SetBulletNumber(3);
    buffer.AddParagraph("text", &attr);
    buffer.AddParagraph("plain");
    CheckRTF("{\\pard\\itap0\\qj\\li2160\\fi-720\\ri720\\sb720\\sa1440\\sl360\\slmult1\\tx2160 "
             "{\\listtext\\plain\\f0 3.\\tab}\\ls1\\ilvl0 {\\plain\\f0 text}\\par\n}"
             "{\\pard\\itap0 {\\plain\\f0 plain}\\par\n}", "", "",
             ListTables(ListDefinition(1, 0, 3, "\\'02\\'00.", "\\'01", "\\f0\\li2160\\fi-720\\tx2160")));
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFPartialParagraph",
                 "[richtextctrl][rtf]")
{
    wxRichTextAttr attr;
    attr.SetAlignment(wxTEXT_ALIGNMENT_RIGHT);
    attr.SetLeftIndent(127);
    buffer.AddParagraph("aligned", &attr);
    CheckRTF("{\\pard\\itap0\\qr\\li720\\fi0 {\\plain\\f0 aligned}\\par\n}");
    buffer.SetPartialParagraph(true);
    CheckRTF("\\pard\\itap0\\qr\\li720\\fi0 {\\plain\\f0 aligned}"
             "\\pard\\itap0\\qr\\li720\\fi0 ");
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFKeepWithPrevious",
                 "[richtextctrl][rtf]")
{
    buffer.AddParagraph("previous");
    wxRichTextAttr attr;
    attr.SetFlags(wxTEXT_ATTR_AVOID_PAGE_BREAK_BEFORE);
    buffer.AddParagraph("next", &attr);
    CheckRTF("{\\pard\\itap0\\keepn {\\plain\\f0 previous}\\par\n}"
             "{\\pard\\itap0 {\\plain\\f0 next}\\par\n}");
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFCharacterHyphenation",
                 "[richtextctrl][rtf]")
{
    wxRichTextAttr attr;
    attr.SetTextEffects(wxTEXT_ATTR_EFFECT_SUPPRESS_HYPHENATION);
    attr.SetTextEffectFlags(wxTEXT_ATTR_EFFECT_SUPPRESS_HYPHENATION);
    AddText("text", attr);
    // RTF can disable automatic hyphenation only for the whole paragraph.
    CheckRTF("{\\pard\\itap0 \\hyphpar0 {\\plain\\f0 text}\\par\n}");
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFBullets",
                 "[richtextctrl][rtf]")
{
    wxRichTextAttr attr;
    wxString bullet, fonts, before, after;
    int numberFormat = 0;
    int alignment = 0;
    attr.SetBulletNumber(4);
    SECTION("Arabic")
    {
        attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_ARABIC);
        bullet = "4";
    }
    SECTION("Uppercase letters")
    {
        attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_LETTERS_UPPER);
        bullet = "D";
        numberFormat = 3;
    }
    SECTION("Right-aligned marker")
    {
        attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_ARABIC | wxTEXT_ATTR_BULLET_STYLE_ALIGN_RIGHT);
        bullet = "4";
        alignment = 2;
    }
    SECTION("Centred marker")
    {
        attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_ARABIC | wxTEXT_ATTR_BULLET_STYLE_ALIGN_CENTRE);
        bullet = "4";
        alignment = 1;
    }
    SECTION("Lowercase letters")
    {
        attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_LETTERS_LOWER);
        bullet = "d";
        numberFormat = 4;
    }
    SECTION("Uppercase Roman")
    {
        attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_ROMAN_UPPER);
        bullet = "IV";
        numberFormat = 1;
    }
    SECTION("Lowercase Roman")
    {
        attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_ROMAN_LOWER);
        bullet = "iv";
        numberFormat = 2;
    }
    SECTION("Period")
    {
        attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_ARABIC | wxTEXT_ATTR_BULLET_STYLE_PERIOD);
        bullet = "4.";
        after = ".";
    }
    SECTION("Parentheses")
    {
        attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_ARABIC | wxTEXT_ATTR_BULLET_STYLE_PARENTHESES);
        bullet = "(4)";
        before = "("; after = ")";
    }
    SECTION("Right parenthesis")
    {
        attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_ARABIC | wxTEXT_ATTR_BULLET_STYLE_RIGHT_PARENTHESIS);
        bullet = "4)";
        after = ")";
    }
    SECTION("Outline numbering")
    {
        attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_OUTLINE);
        attr.SetBulletText("1.2.4");
        bullet = "1.2.4";
        numberFormat = -1;
    }
    SECTION("Symbol and bullet font")
    {
        attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_SYMBOL);
        attr.SetBulletText(wxString(wxUniChar(0x2022)));
        attr.SetBulletFont("Symbol");
        bullet = "\\u8226\\'95";
        fonts = "{\\f1\\fnil Symbol;}";
        numberFormat = 23; before = bullet;
    }
    SECTION("Standard circle")
    {
        // Known difference: wx draws standard bullets as shapes 0.3 times
        // the character height across, smaller than these characters.
        attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_STANDARD);
        attr.SetBulletName("standard/circle");
        bullet = "\\u9679?";
        numberFormat = 23; before = bullet;
    }
    SECTION("Standard outline circle")
    {
        attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_STANDARD);
        attr.SetBulletName("standard/circle-outline");
        bullet = "\\u9675?";
        numberFormat = 23; before = bullet;
    }
    SECTION("Standard square")
    {
        attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_STANDARD);
        attr.SetBulletName("standard/square");
        bullet = "\\u9632?";
        numberFormat = 23; before = bullet;
    }
    SECTION("Standard diamond")
    {
        attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_STANDARD);
        attr.SetBulletName("standard/diamond");
        bullet = "\\u9670?";
        numberFormat = 23; before = bullet;
    }
    SECTION("Standard triangle")
    {
        attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_STANDARD);
        attr.SetBulletName("standard/triangle");
        bullet = "\\u9654?";
        numberFormat = 23; before = bullet;
    }
    SECTION("Continuation omits the marker")
    {
        attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_ARABIC | wxTEXT_ATTR_BULLET_STYLE_CONTINUATION);
    }
    SECTION("No bullet") { attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_NONE); }

    buffer.AddParagraph("text", &attr);
    wxString lists;
    // Without a hanging indent, the text starts a space after the marker.
    if ( !bullet.empty() )
    {
        if ( numberFormat == -1 )
            bullet = "{\\plain\\f0 " + bullet + " }";
        else
        {
            const wxString font = fonts.empty() ? "\\f0" : "\\f1";
            wxString pattern, positions;
            if ( numberFormat == 23 )
                pattern = "\\'01" + bullet;
            else
            {
                pattern = wxString::Format("\\'%02x",
                              static_cast<unsigned int>(before.length() + after.length() + 1)) +
                          before + "\\'00" + after;
                positions = before.empty() ? "\\'01" : "\\'02";
            }
            lists = ListTables(ListDefinition(1, numberFormat, numberFormat == 23 ? 1 : 4,
                                              pattern, positions, font, alignment, 1));
            bullet = "{\\listtext\\plain" + font + " " + bullet + " }\\ls1\\ilvl0 ";
        }
    }
    CheckParagraph("\\f0", "", "", fonts, bullet, lists);
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFCustomBullets",
                 "[richtextctrl][rtf]")
{
    wxRichTextAttr attr;
    attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_SYMBOL);
    wxString marker, text, levelText;
    wxString font = "\\f0", fonts;
    bool useList = true;
    SECTION("ASCII custom marker")
    {
        marker = "->";
        text = "->";
        levelText = "\\'02->";
    }
    SECTION("Unicode BMP marker")
    {
        marker = wxUniChar(0x03a9);
        text = "\\u937?";
        levelText = "\\'01\\u937?";
    }
    SECTION("Unicode code unit above the signed 16-bit boundary")
    {
        marker = wxUniChar(0xff01);
        text = "\\u-255?";
        levelText = "\\'01\\u-255?";
    }
    SECTION("Supplementary character uses a UTF-16 surrogate pair")
    {
        marker = wxUniChar(0x1f600);
        text = "\\u-10179?\\u-8704?";
        levelText = "\\'02\\u-10179?\\u-8704?";
    }
    SECTION("Mixed Unicode marker with an explicit bullet font")
    {
        marker = wxString(wxUniChar(0x03a9)) + wxUniChar(0x1f600);
        text = "\\u937?\\u-10179?\\u-8704?";
        levelText = "\\'03\\u937?\\u-10179?\\u-8704?";
        attr.SetBulletFont("Segoe UI Symbol");
        font = "\\f1";
        fonts = "{\\f1\\fnil Segoe UI Symbol;}";
    }
    SECTION("RTF delimiters and the list-text semicolon terminator")
    {
        marker = "{\\;}";
        text = "\\{\\\\;\\}";
        // The semicolon must not terminate leveltext, but is literal in listtext.
        levelText = "\\'04\\{\\\\\\u59?\\}";
    }
    SECTION("Level text at the 255 UTF-16 code-unit limit")
    {
        const wxString prefix('x', 253);
        marker = prefix + wxUniChar(0x1f600);
        text = prefix + "\\u-10179?\\u-8704?";
        levelText = "\\'ff" + text;
    }
    SECTION("Overlong level text falls back to a visible marker")
    {
        const wxString prefix('x', 254);
        marker = prefix + wxUniChar(0x1f600);
        text = prefix + "\\u-10179?\\u-8704?";
        useList = false;
    }
    attr.SetBulletText(marker);
    buffer.AddParagraph("text", &attr);
    // Without a hanging indent, the text starts a space after the marker.
    if ( useList )
    {
        CheckParagraph("\\f0", "", "", fonts,
                       "{\\listtext\\plain" + font + " " + text + " }\\ls1\\ilvl0 ",
                       ListTables(ListDefinition(1, 23, 1, levelText, "", font, 0, 1)));
    }
    else
    {
        CheckParagraph("\\f0", "", "", fonts, "{\\plain" + font + " " + text + " }");
    }
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFNumberingRestarts",
                 "[richtextctrl][rtf]")
{
    struct Numbering
    {
        int style;
        int format;
        const char* markers[4];
    };
    Numbering numbering = { wxTEXT_ATTR_BULLET_STYLE_ARABIC, 0, { "4.", "5.", "1.", "2." } };
    SECTION("Arabic") { }
    SECTION("Uppercase letters")
    {
        numbering = { wxTEXT_ATTR_BULLET_STYLE_LETTERS_UPPER, 3, { "D.", "E.", "A.", "B." } };
    }
    SECTION("Lowercase letters")
    {
        numbering = { wxTEXT_ATTR_BULLET_STYLE_LETTERS_LOWER, 4, { "d.", "e.", "a.", "b." } };
    }
    SECTION("Uppercase Roman")
    {
        numbering = { wxTEXT_ATTR_BULLET_STYLE_ROMAN_UPPER, 1, { "IV.", "V.", "I.", "II." } };
    }
    SECTION("Lowercase Roman")
    {
        numbering = { wxTEXT_ATTR_BULLET_STYLE_ROMAN_LOWER, 2, { "iv.", "v.", "i.", "ii." } };
    }

    wxRichTextAttr attr;
    attr.SetBulletStyle(numbering.style | wxTEXT_ATTR_BULLET_STYLE_PERIOD);
    const int numbers[] = { 4, 5, 1, 2 };
    wxString definitions, body;
    for ( int i = 0; i < 4; ++i )
    {
        if ( i == 2 )
        {
            buffer.AddParagraph("plain");
            body += "{\\pard\\itap0 {\\plain\\f0 plain}\\par\n}";
        }
        attr.SetBulletNumber(numbers[i]);
        buffer.AddParagraph("item", &attr);
        // Consecutive numbers share a list, a restart starts a new one.
        const int list = i < 2 ? 1 : 2;
        if ( i == 0 || i == 2 )
            definitions += ListDefinition(list, numbering.format, numbers[i],
                                          "\\'02\\'00.", "\\'01", "\\f0", 0, 1);
        // Without a hanging indent, the text starts a space after the marker.
        body += "{\\pard\\itap0 {\\listtext\\plain\\f0 " + wxString(numbering.markers[i]) +
                wxString::Format(" }\\ls%d\\ilvl0 ", list) +
                "{\\plain\\f0 item}\\par\n}";
        if ( i == 0 )
        {
            wxRichTextAttr continuation(attr);
            continuation.SetBulletStyle(attr.GetBulletStyle() | wxTEXT_ATTR_BULLET_STYLE_CONTINUATION);
            buffer.AddParagraph("continued", &continuation);
            body += "{\\pard\\itap0 {\\plain\\f0 continued}\\par\n}";
        }
    }
    // Every stored number must survive, including the restart after plain text.
    // Neither the plain nor the continuation paragraph allocates a list ID or
    // interrupts the list.
    CheckRTF(body, "", "", ListTables(definitions, 2));
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFCharacterStyles",
                 "[richtextctrl][rtf]")
{
    wxRichTextStyleSheet sheet;
    buffer.SetStyleSheet(&sheet);
    auto base = new wxRichTextCharacterStyleDefinition("Base character");
    base->GetStyle().SetFontFaceName("Courier New");
    base->GetStyle().SetFontSize(13);
    REQUIRE(sheet.AddCharacterStyle(base));
    auto emphasis = new wxRichTextCharacterStyleDefinition("Emphasis");
    emphasis->SetBaseStyle("Base character");
    emphasis->GetStyle().SetFontWeight(wxFONTWEIGHT_BOLD);
    emphasis->GetStyle().SetTextColour(wxColour(255, 0, 0));
    REQUIRE(sheet.AddCharacterStyle(emphasis));

    REQUIRE(buffer.BeginCharacterStyle("Emphasis"));
    buffer.AddParagraph("styled");
    REQUIRE(buffer.EndCharacterStyle());
    buffer.AddParagraph("plain");
    wxString character;
    SECTION("Inherited face and size with derived weight and colour")
    {
        character = "\\cs2\\f1\\fs26\\b\\cf1";
    }
    SECTION("Direct formatting overrides the named style")
    {
        auto para = wxDynamicCast(buffer.GetChildren().GetFirst()->GetData(), wxRichTextParagraph);
        para->GetChildren().GetFirst()->GetData()->GetAttributes().SetFontWeight(wxFONTWEIGHT_NORMAL);
        character = "\\cs2\\f1\\fs26\\cf1";
    }
    // Style definitions contain the complete formatting, including the
    // inherited one, and refer to their base styles.
    CheckRTF("{\\pard\\itap0 {\\plain" + character + " styled}\\par\n}"
             "{\\pard\\itap0 {\\plain\\f0 plain}\\par\n}",
             "\\red255\\green0\\blue0;", "{\\f1\\fnil Courier New;}",
             "{\\stylesheet{\\*\\cs1\\additive\\f1\\fs26 Base character;}"
             "{\\*\\cs2\\additive\\f1\\fs26\\b\\cf1\\sbasedon1 Emphasis;}}\n");
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFParagraphStyles",
                 "[richtextctrl][rtf]")
{
    wxRichTextStyleSheet sheet;
    buffer.SetStyleSheet(&sheet);
    auto base = new wxRichTextParagraphStyleDefinition("Base paragraph");
    base->GetStyle().SetAlignment(wxTEXT_ALIGNMENT_RIGHT);
    base->GetStyle().SetLineSpacing(15);
    base->GetStyle().SetFontSize(12);
    REQUIRE(sheet.AddParagraphStyle(base));
    auto heading = new wxRichTextParagraphStyleDefinition("Heading");
    heading->SetBaseStyle("Base paragraph");
    heading->GetStyle().SetParagraphSpacingBefore(127);
    heading->GetStyle().SetParagraphSpacingAfter(254);
    heading->GetStyle().SetFontWeight(wxFONTWEIGHT_BOLD);
    REQUIRE(sheet.AddParagraphStyle(heading));

    REQUIRE(buffer.BeginParagraphStyle("Heading"));
    buffer.AddParagraph("heading");
    REQUIRE(buffer.EndParagraphStyle());
    buffer.AddParagraph("plain");
    wxString alignment;
    SECTION("Inherited and derived paragraph and character attributes")
    {
        alignment = "\\qr";
    }
    SECTION("Direct paragraph formatting overrides the named style")
    {
        buffer.GetChildren().GetFirst()->GetData()->GetAttributes().SetAlignment(wxTEXT_ALIGNMENT_CENTRE);
        alignment = "\\qc";
    }
    // Styles without a face don't select a font.
    CheckRTF("{\\pard\\itap0\\s2" + alignment + "\\sb720\\sa1440\\sl360\\slmult1 "
             "{\\plain\\f0\\fs24\\b heading}\\par\n}"
             "{\\pard\\itap0 {\\plain\\f0 plain}\\par\n}", "", "",
             "{\\stylesheet{\\s1\\qr\\sl360\\slmult1\\fs24 Base paragraph;}"
             "{\\s2\\qr\\sb720\\sa1440\\sl360\\slmult1\\fs24\\b\\sbasedon1 Heading;}}\n");
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFBulletStyles",
                 "[richtextctrl][rtf]")
{
    wxRichTextStyleSheet sheet;
    buffer.SetStyleSheet(&sheet);
    auto list = new wxRichTextListStyleDefinition("Outline list");
    list->GetStyle().SetFontSize(11);
    list->SetAttributes(0, 127, 127, wxTEXT_ATTR_BULLET_STYLE_SYMBOL, "*");
    list->SetAttributes(1, 254, 127,
        wxTEXT_ATTR_BULLET_STYLE_ROMAN_LOWER | wxTEXT_ATTR_BULLET_STYLE_PERIOD);
    REQUIRE(sheet.AddListStyle(list));

    REQUIRE(buffer.BeginListStyle("Outline list", 0, 1));
    buffer.AddParagraph("outer");
    REQUIRE(buffer.EndListStyle());
    REQUIRE(buffer.BeginListStyle("Outline list", 1, 4));
    buffer.AddParagraph("inner");
    REQUIRE(buffer.EndListStyle());
    buffer.AddParagraph("plain");
    // AddParagraph() splits the list's font size into the text run; the
    // marker uses the paragraph font, just as the rich text renderer does.
    CheckRTF("{\\pard\\itap0\\li1440\\fi-720\\tx1440 "
             "{\\listtext\\plain\\f0 *\\tab}\\ls1\\ilvl0 "
             "{\\plain\\f0\\fs22 outer}\\par\n}"
             "{\\pard\\itap0\\li2160\\fi-720\\tx2160 "
             "{\\listtext\\plain\\f0 iv.\\tab}\\ls1\\ilvl1 {\\plain\\f0\\fs22 inner}\\par\n}"
             "{\\pard\\itap0 {\\plain\\f0 plain}\\par\n}", "", "",
             // Both levels of the list style belong to one list. The number
             // placeholder of the second level refers to that level.
             ListTables("{\\list\\listtemplateid1"
                        "{\\listlevel\\levelnfc23\\levelnfcn23\\leveljc0\\leveljcn0"
                        "\\levelfollow0\\levelstartat1{\\leveltext\\'01*;}{\\levelnumbers;}"
                        "\\f0\\li1440\\fi-720\\tx1440}"
                        "{\\listlevel\\levelnfc2\\levelnfcn2\\leveljc0\\leveljcn0"
                        "\\levelfollow0\\levelstartat4{\\leveltext\\'02\\'01.;}{\\levelnumbers\\'01;}"
                        "\\f0\\li2160\\fi-720\\tx2160}\\listid1}"));
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFBulletTabStop",
                 "[richtextctrl][rtf]")
{
    // The tab after the bullet must end at the hanging indent, even if the
    // paragraph has tab stops before it (Word then ignores the implicit stop
    // at the hanging indent), as in the sample's list of features.
    wxArrayInt tabs;
    wxString tabStops;
    SECTION("Tab stops after the hanging indent")
    {
        tabs.Add(400);
        tabs.Add(600);
        tabStops = "\\tx907\\tx2268\\tx3402";
    }
    SECTION("Tab stop at the hanging indent is not repeated")
    {
        tabs.Add(160);
        tabs.Add(600);
        tabStops = "\\tx907\\tx3402";
    }
    SECTION("No tab stops") { tabStops = "\\tx907"; }
    wxRichTextAttr attr;
    if ( !tabs.empty() )
    {
        attr.SetFlags(wxTEXT_ATTR_TABS);
        attr.SetTabs(tabs);
    }
    buffer.SetDefaultStyle(attr);
    buffer.BeginSymbolBullet('*', 100, 60);
    buffer.AddParagraph("item");
    buffer.EndSymbolBullet();
    CheckRTF("{\\pard\\itap0\\li907\\fi-340" + tabStops +
             " {\\listtext\\plain\\f0 *\\tab}\\ls1\\ilvl0 {\\plain\\f0 item}\\par\n}", "", "",
             ListTables(ListDefinition(1, 23, 1, "\\'01*", "", "\\f0\\li907\\fi-340\\tx907")));
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFListIndentation",
                 "[richtextctrl][rtf]")
{
    // wx draws the marker at the first-line indent and starts the text at
    // the hanging indent, or one space after the marker without one.
    wxRichTextAttr attr;
    attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_SYMBOL);
    attr.SetBulletText("*");
    attr.SetLeftIndent(100, 60);
    wxString indents = "\\li907\\fi-340", tabStop = "\\tx907", follow = "\\tab";
    wxString level = "\\f0\\li907\\fi-340\\tx907";
    int format = 23, start = 1, align = 0, levelFollow = 0;
    wxString pattern = "\\'01*", positions, marker = "*", direction, characters;
    wxString cellStart, cellEnd, before = "{\\pard\\itap0", after = "\\par\n}";
    bool continuation = false, inBox = false;

    SECTION("Hanging indent") { }
    SECTION("Marker wider than the hanging indent")
    {
        // wx draws the text at the hanging indent, over the end of the
        // marker, while the tab in Word moves on to the next tab stop.
        attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_ARABIC | wxTEXT_ATTR_BULLET_STYLE_PERIOD);
        attr.SetBulletNumber(888);
        attr.SetLeftIndent(100, 20);
        indents = "\\li680\\fi-113";
        tabStop = "\\tx680";
        level = "\\f0\\li680\\fi-113\\tx680";
        format = 0;
        start = 888;
        pattern = "\\'02\\'00.";
        positions = "\\'01";
        marker = "888.";
    }
    SECTION("First-line indent instead of a hanging indent")
    {
        // wx draws the marker at the left indent, after the start of the
        // text: this can't be represented, keep the indents.
        attr.SetLeftIndent(100, -40);
        indents = "\\li340\\fi227";
        tabStop.clear();
        level = "\\f0\\li340\\fi227";
    }
    SECTION("No hanging indent: a space follows the marker")
    {
        attr.SetLeftIndent(100, 0);
        indents = "\\li567\\fi0";
        tabStop.clear();
        follow = " ";
        level = "\\f0\\li567\\fi0";
        levelFollow = 1;
    }
    SECTION("Continuation paragraph starts at the hanging indent")
    {
        continuation = true;
    }
    SECTION("Right-aligned marker")
    {
        // Word aligns the marker on the start of the first line, wx on the
        // hanging indent less the bullet margin: 10 + 6 - 2 mm.
        attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_SYMBOL | wxTEXT_ATTR_BULLET_STYLE_ALIGN_RIGHT);
        align = 2;
        indents = "\\li907\\fi-113";
        level = "\\f0\\li907\\fi-113\\tx907";
    }
    SECTION("Centred marker")
    {
        // wx centres the marker in the hanging indent: on 10 + 6 / 2 mm.
        attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_SYMBOL | wxTEXT_ATTR_BULLET_STYLE_ALIGN_CENTRE);
        align = 1;
        indents = "\\li907\\fi-170";
        level = "\\f0\\li907\\fi-170\\tx907";
    }
    SECTION("Right-aligned marker with a hanging indent smaller than the margin")
    {
        // The bullet margin is limited to the 1 mm hanging indent: the marker
        // ends at the left indent, 10 mm.
        attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_SYMBOL | wxTEXT_ATTR_BULLET_STYLE_ALIGN_RIGHT);
        attr.SetLeftIndent(100, 10);
        align = 2;
        indents = "\\li624\\fi-57";
        tabStop = "\\tx624";
        level = "\\f0\\li624\\fi-57\\tx624";
    }
    SECTION("Centred marker in an odd hanging indent")
    {
        // The centre, 10.75 mm, is rounded only once: 609 twips.
        attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_SYMBOL | wxTEXT_ATTR_BULLET_STYLE_ALIGN_CENTRE);
        attr.SetLeftIndent(100, 15);
        align = 1;
        indents = "\\li652\\fi-43";
        tabStop = "\\tx652";
        level = "\\f0\\li652\\fi-43\\tx652";
    }
    SECTION("Right-to-left paragraph measures indents from the right")
    {
        attr.SetTextEffects(wxTEXT_ATTR_EFFECT_RTL);
        attr.SetTextEffectFlags(wxTEXT_ATTR_EFFECT_RTL);
        direction = "\\rtlpar";
        // The effect also applies to the characters, including the marker.
        characters = "\\rtlch";
        level = "\\f0\\rtlch\\li907\\fi-340\\tx907";
    }
    SECTION("Indents in a table cell are relative to the cell")
    {
        cellStart = "{\\trowd\\trgaph0\\trleft0\\clftsWidth3\\clwWidth9360\\cellx9360\n";
        before = "{\\pard\\intbl\\itap1";
        after = "\\cell }";
        cellEnd = "\\row}\n";
    }
    SECTION("Indents in a text box are relative to the box")
    {
        inBox = true;
    }

    wxRichTextParagraphLayoutBox* container = &buffer;
    if ( !cellStart.empty() )
    {
        auto table = CreateTestTable(1, 1);
        Anchor(buffer, table);
        container = table->GetCell(0, 0);
        container->DeleteChildren();
    }
    else if ( inBox )
    {
        auto box = CreateTestBox();
        Anchor(buffer, box);
        container = box;
    }
    container->AddParagraph("item", &attr);
    wxString body = before + direction + indents + tabStop +
                    " {\\listtext\\plain\\f0" + characters + " " + marker + follow +
                    "}\\ls1\\ilvl0 {\\plain\\f0" + characters + " item}";
    if ( continuation )
    {
        wxRichTextAttr next(attr);
        next.SetBulletStyle(attr.GetBulletStyle() | wxTEXT_ATTR_BULLET_STYLE_CONTINUATION);
        container->AddParagraph("continued", &next);
        body += "\\par\n}" + before + "\\li907\\fi0 {\\plain\\f0 continued}";
    }
    body = cellStart + body + after + cellEnd;
    if ( inBox )
        body = "{\\shp{\\*\\shpinst\\shpleft0\\shptop0\\shpright9360\\shpbottom240"
               "\\shpbxcolumn\\shpbypara\\shpwr1{\\sp{\\sn shapeType}{\\sv 202}}" +
               PlainTextBoxProperties(true) + ShapeText(body) + ShapeAnchor();
    CheckRTF(body, "", "", ListTables(ListDefinition(1, format, start, pattern, positions,
                                                     level, align, levelFollow)));
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFMultiLevelLists",
                 "[richtextctrl][rtf]")
{
    wxRichTextStyleSheet sheet;
    buffer.SetStyleSheet(&sheet);
    auto list = new wxRichTextListStyleDefinition("Numbered outline");
    list->SetAttributes(0, 127, 127, wxTEXT_ATTR_BULLET_STYLE_ARABIC | wxTEXT_ATTR_BULLET_STYLE_PERIOD);
    list->SetAttributes(1, 254, 127,
        wxTEXT_ATTR_BULLET_STYLE_LETTERS_LOWER | wxTEXT_ATTR_BULLET_STYLE_PERIOD);
    REQUIRE(sheet.AddListStyle(list));

    struct Item
    {
        const char* text;
        int level;
        int number;
        const char* marker;
    };
    Item items[] =
    {
        { "A", 0, 1, "1." },
        { "B", 1, 1, "a." },
        { "C", 1, 2, "b." },
        { "D", 0, 2, "2." },
        { "E", 1, 1, "a." },
    };
    const wxString outer = "{\\listlevel\\levelnfc0\\levelnfcn0\\leveljc0\\leveljcn0"
                           "\\levelfollow0\\levelstartat1{\\leveltext\\'02\\'00.;}"
                           "{\\levelnumbers\\'01;}\\f0\\li1440\\fi-720\\tx1440}";
    const wxString inner = "{\\listlevel\\levelnfc4\\levelnfcn4\\leveljc0\\leveljcn0"
                           "\\levelfollow0\\levelstartat%d{\\leveltext\\'02\\'01.;}"
                           "{\\levelnumbers\\'01;}\\f0\\li2160\\fi-720\\tx2160}";
    wxString lists;
    int lastList = 1;
    SECTION("Deeper levels restart automatically after a higher level item")
    {
        lists = ListTables("{\\list\\listtemplateid1" + outer +
                           wxString::Format(inner, 1) + "\\listid1}");
    }
    SECTION("A number the reader wouldn't produce starts a new list")
    {
        items[4].number = 3;
        items[4].marker = "c.";
        lastList = 2;
        // The new list only uses its second level, but must define the first.
        lists = ListTables("{\\list\\listtemplateid1" + outer +
                           wxString::Format(inner, 1) + "\\listid1}"
                           "{\\list\\listtemplateid2"
                           "{\\listlevel\\levelnfc23\\levelnfcn23\\leveljc0\\leveljcn0"
                           "\\levelfollow0\\levelstartat1{\\leveltext\\'01\\u8226\\'95;}"
                           "{\\levelnumbers;}}" +
                           wxString::Format(inner, 3) + "\\listid2}", 2);
    }

    wxString body;
    for ( size_t n = 0; n < WXSIZEOF(items); ++n )
    {
        const Item& item = items[n];
        REQUIRE(buffer.BeginListStyle("Numbered outline", item.level, item.number));
        buffer.AddParagraph(item.text);
        REQUIRE(buffer.EndListStyle());
        const int indent = item.level ? 2160 : 1440;
        body += wxString::Format("{\\pard\\itap0\\li%d\\fi-720\\tx%d {\\listtext\\plain\\f0 %s\\tab}"
                                 "\\ls%d\\ilvl%d {\\plain\\f0 %s}\\par\n}",
                                 indent, indent, item.marker,
                                 n == 4 ? lastList : 1, item.level, item.text);
    }
    CheckRTF(body, "", "", lists);
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFUnspecifiedNestedFonts",
                 "[richtextctrl][rtf]")
{
    // Only the native default face is platform-dependent. All expected RTF
    // structure, font indices and explicit font overrides remain literal.
    const wxFont nativeFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
                            wxFONTWEIGHT_NORMAL);
    REQUIRE(nativeFont.IsOk());
    const wxString nativeFace = nativeFont.GetFaceName();
    REQUIRE(!nativeFace.empty());
    const wxString outerFace = nativeFace == "Times New Roman"
                             ? "Courier New" : "Times New Roman";
    wxRichTextAttr containerAttr, paraAttr, runAttr;
    containerAttr.SetFontSize(12);
    wxString fonts = "{\\f1\\fnil " + nativeFace + ";}";
    wxString character = "\\f1\\fs24";
    SECTION("Unspecified face") { }
    SECTION("Explicitly empty face")
    {
        containerAttr.SetFontFaceName(wxString());
    }
    SECTION("Explicit container face")
    {
        containerAttr.SetFontFaceName("Arial");
        fonts = "{\\f1\\fnil Arial;}";
    }
    SECTION("Explicit paragraph face")
    {
        paraAttr.SetFontFaceName("Arial");
        fonts = "{\\f1\\fnil Arial;}";
    }
    SECTION("Explicit run overrides unspecified paragraph")
    {
        runAttr.SetFontFaceName("Courier New");
        const bool reuseOuter = outerFace == "Courier New";
        const bool reuseNative = nativeFace == "Courier New";
        if ( !reuseOuter && !reuseNative )
            fonts += "{\\f2\\fnil Courier New;}";
        character = reuseOuter ? "\\f0\\fs24"
                  : reuseNative ? "\\f1\\fs24" : "\\f2\\fs24";
    }
    SECTION("Explicit run overrides container face")
    {
        containerAttr.SetFontFaceName("Arial");
        runAttr.SetFontFaceName(outerFace);
        fonts = "{\\f1\\fnil Arial;}";
        character = "\\f0\\fs24";
    }

    for ( bool inTable : { false, true } )
    {
        INFO("Table cell: " << inTable);
        buffer.Clear();
        buffer.GetAttributes().SetFontFaceName(outerFace);
        AddText("outside");
        wxRichTextParagraphLayoutBox* container;
        wxString prefix, suffix;
        if ( inTable )
        {
            auto table = new wxRichTextTable;
            REQUIRE(table->CreateTable(1, 1));
            Anchor(buffer, table);
            container = table->GetCell(0, 0);
            container->Clear();
            prefix = "{\\trowd\\trgaph0\\trleft0\\clftsWidth3\\clwWidth9360\\cellx9360\n"
                     "{\\pard\\intbl\\itap1 ";
            suffix = "\\cell }\\row}\n";
        }
        else
        {
            auto box = new wxRichTextBox;
            Anchor(buffer, box);
            container = box;
            prefix = "{\\shp{\\*\\shpinst\\shpleft0\\shptop0\\shpright9360\\shpbottom240"
                     "\\shpbxcolumn\\shpbypara\\shpwr1{\\sp{\\sn shapeType}{\\sv 202}}" +
                     PlainTextBoxProperties(true);
        }
        container->SetAttributes(containerAttr);
        container->AddParagraph("inside");
        auto para = wxDynamicCast(container->GetChildren().GetFirst()->GetData(),
                                  wxRichTextParagraph);
        REQUIRE(para);
        para->SetAttributes(paraAttr);
        auto run = para->GetChildren().GetFirst()->GetData();
        run->SetAttributes(runAttr);
        const wxRichTextAttr basicBefore = buffer.GetBasicStyle();
        const wxString expected =
            "{\\rtf1\\ansi\\ansicpg1252\\deff0\\uc1\n"
            "{\\fonttbl{\\f0\\fnil " + outerFace + ";}" + fonts + "}\n"
            "{\\colortbl;}\n\\viewkind4\\deftab567\\dntblnsbdb\\expshrtn\\paperw12240\\paperh15840"
            "\\margl1440\\margr1440\\margt1440\\margb1440\n"
            "{\\pard\\itap0 {\\plain\\f0 outside}\\par\n}" +
            prefix + (inTable ? "{\\plain" + character + " inside}" + suffix
                              : ShapeText("{\\pard\\itap0 {\\plain" + character +
                                          " inside}\\par\n}") + ShapeAnchor()) + "}";
        wxString actual;
        REQUIRE(buffer.GetAsRTF(actual));
        CHECK(actual == expected);
        CHECK(buffer.GetBasicStyle() == basicBefore);
        CHECK(container->GetAttributes() == containerAttr);
        CHECK(para->GetAttributes() == paraAttr);
        CHECK(run->GetAttributes() == runAttr);
    }
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFTables", "[richtextctrl][rtf]")
{
    auto table = CreateTestTable(2, 2);
    Anchor(buffer, table);
    wxString rtf;

    SECTION("Spans do not depend on layout or cell visibility caches")
    {
        table->GetCell(0, 0)->SetColSpan(2);
        table->GetCell(0, 0)->SetRowSpan(2);
        table->GetCell(0, 0)->AddParagraph("visible");
        table->GetCell(0, 1)->AddParagraph("covered horizontally");
        table->GetCell(1, 0)->AddParagraph("covered vertically");
        table->GetCell(1, 1)->AddParagraph("covered both ways");
        CheckRTF("{\\trowd\\trgaph0\\trleft0"
                 "\\clmgf\\clvmgf\\clftsWidth3\\clwWidth4680\\cellx4680"
                 "\\clmrg\\clvmgf\\clftsWidth3\\clwWidth4680\\cellx9360\n"
                 "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\par\n}"
                 "{\\pard\\intbl\\itap1 {\\plain\\f0 visible}\\cell }"
                 "{\\pard\\intbl\\itap1 \\cell}\\row}\n"
                 "{\\trowd\\trgaph0\\trleft0"
                 "\\clmgf\\clvmrg\\clftsWidth3\\clwWidth4680\\cellx4680"
                 "\\clmrg\\clvmrg\\clftsWidth3\\clwWidth4680\\cellx9360\n"
                 "{\\pard\\intbl\\itap1 \\cell}"
                 "{\\pard\\intbl\\itap1 \\cell}\\row}\n");
    }

    SECTION("Nested tables include row definitions and honour column widths")
    {
        table->GetCell(0, 0)->GetAttributes().GetTextBoxAttr().GetWidth().SetValue(100, wxTEXT_ATTR_UNITS_PIXELS);
        table->GetCell(0, 1)->GetAttributes().GetTextBoxAttr().GetWidth().SetValue(300, wxTEXT_ATTR_UNITS_PIXELS);
        auto nested = CreateTestTable(1, 1);
        nested->GetCell(0, 0)->AddParagraph("nested");
        wxString nestedHeight;
        SECTION("Automatic nested row height") { }
        SECTION("Explicit nested row height is written in nesttableprops")
        {
            nested->GetCell(0, 0)->GetAttributes().GetTextBoxAttr().GetHeight()
                .SetValue(40, wxTEXT_ATTR_UNITS_PIXELS);
            nestedHeight = "\\trrh600";
        }
        auto cellPara = wxDynamicCast(table->GetCell(0, 0)->GetChildren().GetFirst()->GetData(), wxRichTextParagraph);
        cellPara->AppendChild(nested);
        CheckRTF("{\\trowd\\trgaph0\\trleft0"
                 "\\clftsWidth3\\clwWidth1500\\cellx1500"
                 "\\clftsWidth3\\clwWidth4500\\cellx6000\n"
                 "{\\intbl\\itap2\n"
                 "{\\pard\\intbl\\itap2 {\\plain\\f0 }\\par\n}"
                 "{\\pard\\intbl\\itap2 {\\plain\\f0 nested}\\nestcell }"
                 "{\\*\\nesttableprops\\trowd\\trgaph0\\trleft0" + nestedHeight +
                 "\\clftsWidth3\\clwWidth1500\\cellx1500\\nestrow}}\n"
                 "{\\pard\\intbl\\itap1 \\cell}"
                 "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }\\row}\n"
                 "{\\trowd\\trgaph0\\trleft0"
                 "\\clftsWidth3\\clwWidth1500\\cellx1500"
                 "\\clftsWidth3\\clwWidth4500\\cellx6000\n"
                 "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }"
                 "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }\\row}\n");
    }

    SECTION("A cell with only hidden content still ends with a cell mark")
    {
        table->GetCell(0, 0)->GetChildren().GetFirst()->GetData()->Show(false);
        CheckRTF("{\\trowd\\trgaph0\\trleft0"
                 "\\clftsWidth3\\clwWidth4680\\cellx4680"
                 "\\clftsWidth3\\clwWidth4680\\cellx9360\n"
                 "{\\pard\\intbl\\itap1 \\cell}"
                 "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }\\row}\n"
                 "{\\trowd\\trgaph0\\trleft0"
                 "\\clftsWidth3\\clwWidth4680\\cellx4680"
                 "\\clftsWidth3\\clwWidth4680\\cellx9360\n"
                 "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }"
                 "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }\\row}\n");
    }

    SECTION("A cell ending with a non-paragraph object still ends with a cell mark")
    {
        table->GetCell(0, 0)->AppendChild(CreateTestTable(1, 1));
        CheckRTF("{\\trowd\\trgaph0\\trleft0"
                 "\\clftsWidth3\\clwWidth4680\\cellx4680"
                 "\\clftsWidth3\\clwWidth4680\\cellx9360\n"
                 "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\par\n}"
                 "{\\intbl\\itap2\n"
                 "{\\pard\\intbl\\itap2 {\\plain\\f0 }\\nestcell }"
                 "{\\*\\nesttableprops\\trowd\\trgaph0\\trleft0"
                 "\\clftsWidth3\\clwWidth4680\\cellx4680\\nestrow}}\n"
                 "{\\pard\\intbl\\itap1 \\cell}"
                 "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }\\row}\n"
                 "{\\trowd\\trgaph0\\trleft0"
                 "\\clftsWidth3\\clwWidth4680\\cellx4680"
                 "\\clftsWidth3\\clwWidth4680\\cellx9360\n"
                 "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }"
                 "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }\\row}\n");
    }

    SECTION("Invalid spans fail without returning a partial document")
    {
        table->GetCell(0, 0)->SetColSpan(3);
        rtf = "unchanged";
        CHECK_FALSE(buffer.GetAsRTF(rtf));
        CHECK(rtf == "unchanged");
    }
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFSharedCellBorders",
                 "[richtextctrl][rtf]")
{
    // wx draws both borders of adjacent cells, RTF only one per edge. The
    // more visible border is written on both sides of an edge, so that
    // readers can't prefer the plain border of the neighbouring cells.
    auto table = Anchor(buffer, CreateTestTable(1, 3));
    for ( int col = 0; col < 3; ++col )
    {
        auto& border = table->GetCell(0, col)->GetAttributes().GetTextBoxAttr().GetBorder();
        border.SetWidth(1, wxTEXT_ATTR_UNITS_PIXELS);
        border.SetStyle(col == 1 ? wxTEXT_BOX_ATTR_BORDER_DASHED : wxTEXT_BOX_ATTR_BORDER_SOLID);
        border.SetColour(col == 1 ? wxColour(186, 129, 20) : wxColour(144, 162, 185));
    }
    wxString neighbourEdge;
    SECTION("Dashed border wins over a solid border of the same width")
    {
        neighbourEdge = "\\brdrdash\\brdrw15\\brdrcf2";
    }
    SECTION("A wider border wins over a dashed border")
    {
        for ( int col : {0, 2} )
            table->GetCell(0, col)->GetAttributes().GetTextBoxAttr().GetBorder()
                .SetWidth(2, wxTEXT_ATTR_UNITS_PIXELS);
        neighbourEdge = "\\brdrs\\brdrw30\\brdrcf1";
    }
    const wxString solid = neighbourEdge.Contains("brdrs") ? neighbourEdge
                                                           : wxString("\\brdrs\\brdrw15\\brdrcf1");
    const wxString dashed = "\\brdrdash\\brdrw15\\brdrcf2";
    const wxString middleSide = neighbourEdge.Contains("brdrs") ? neighbourEdge : dashed;
    const wxString cell = "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }";
    CheckRTF("{\\trowd\\trgaph0\\trleft0"
             "\\clbrdrt" + solid + "\\clbrdrl" + solid + "\\clbrdrb" + solid +
             "\\clbrdrr" + neighbourEdge + "\\clftsWidth3\\clwWidth3120\\cellx3120"
             "\\clbrdrt" + dashed + "\\clbrdrl" + middleSide + "\\clbrdrb" + dashed +
             "\\clbrdrr" + middleSide + "\\clftsWidth3\\clwWidth3120\\cellx6240"
             "\\clbrdrt" + solid + "\\clbrdrl" + neighbourEdge + "\\clbrdrb" + solid +
             "\\clbrdrr" + solid + "\\clftsWidth3\\clwWidth3120\\cellx9360\n" +
             cell + cell + cell + "\\row}\n",
             "\\red144\\green162\\blue185;\\red186\\green129\\blue20;");
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFAdjacentTables",
                 "[richtextctrl][rtf]")
{
    // Consecutive rows form one table in RTF. A paragraph of minimal height
    // keeps adjacent wx tables separate, without adding a visible blank line.
    const wxString row = "{\\trowd\\trgaph0\\trleft0\\clftsWidth3\\clwWidth9360\\cellx9360\n"
                         "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }\\row}\n";
    const wxString separator = "{\\pard\\itap0\\plain\\fs1\\sl-1\\slmult0\\par}";
    SECTION("Two tables in consecutive paragraphs")
    {
        Anchor(buffer, CreateTestTable(1, 1));
        Anchor(buffer, CreateTestTable(1, 1));
        CheckRTF(row + separator + row);
    }
    SECTION("A paragraph between the tables needs no separator")
    {
        Anchor(buffer, CreateTestTable(1, 1));
        buffer.AddParagraph("between");
        Anchor(buffer, CreateTestTable(1, 1));
        CheckRTF(row + "{\\pard\\itap0 {\\plain\\f0 between}\\par\n}" + row);
    }
    SECTION("Two nested tables in the same cell")
    {
        auto outer = Anchor(buffer, CreateTestTable(1, 1));
        auto cell = outer->GetCell(0, 0);
        cell->DeleteChildren();
        Anchor(*cell, CreateTestTable(1, 1));
        Anchor(*cell, CreateTestTable(1, 1));
        const wxString nested = "{\\intbl\\itap2\n{\\pard\\intbl\\itap2 {\\plain\\f0 }\\nestcell }"
                                "{\\*\\nesttableprops\\trowd\\trgaph0\\trleft0"
                                "\\clftsWidth3\\clwWidth9360\\cellx9360\\nestrow}}\n";
        CheckRTF("{\\trowd\\trgaph0\\trleft0\\clftsWidth3\\clwWidth9360\\cellx9360\n" + nested +
                 "{\\pard\\intbl\\itap1\\plain\\fs1\\sl-1\\slmult0\\par}" + nested +
                 "{\\pard\\intbl\\itap1 \\cell}\\row}\n");
    }
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFTopLevelObjects",
                 "[richtextctrl][rtf]")
{
    // wxRichTextCtrl stores tables and text boxes in paragraphs of their own,
    // but code can also add them to a container directly. Although the wx
    // layout doesn't show them then, their RTF is the same.
    wxRichTextBuffer anchored;
    anchored.SetBasicStyle(buffer.GetBasicStyle());
    auto box = CreateTestBox();
    box->AddParagraph("box");
    auto anchoredBox = CreateTestBox();
    anchoredBox->AddParagraph("box");
    buffer.AppendChild(CreateTestTable(1, 2));
    buffer.AppendChild(box);
    Anchor(anchored, CreateTestTable(1, 2));
    Anchor(anchored, anchoredBox);
    CheckRTF("{\\trowd\\trgaph0\\trleft0"
             "\\clftsWidth3\\clwWidth4680\\cellx4680"
             "\\clftsWidth3\\clwWidth4680\\cellx9360\n"
             "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }"
             "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }\\row}\n"
             "{\\shp{\\*\\shpinst\\shpleft0\\shptop0\\shpright9360\\shpbottom240"
             "\\shpbxcolumn\\shpbypara\\shpwr1{\\sp{\\sn shapeType}{\\sv 202}}" +
             PlainTextBoxProperties(true) +
             ShapeText("{\\pard\\itap0 {\\plain\\f0 box}\\par\n}") + ShapeAnchor());
    wxString direct, inParagraphs;
    REQUIRE(buffer.GetAsRTF(direct));
    REQUIRE(anchored.GetAsRTF(inParagraphs));
    CHECK(inParagraphs == direct);
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFNestedTableBoundaries",
                 "[richtextctrl][rtf]")
{
    auto outer = CreateTestTable(2, 2);
    Anchor(buffer, outer);
    auto left = outer->GetCell(0, 0);
    left->GetAttributes().GetTextBoxAttr().GetWidth().SetValue(200, wxTEXT_ATTR_UNITS_PIXELS);
    outer->GetCell(0, 1)->GetAttributes().GetTextBoxAttr().GetWidth().SetValue(300, wxTEXT_ATTR_UNITS_PIXELS);
    left->DeleteChildren();
    left->AddParagraph("before");
    bool separate = false;
    SECTION("Text and table in the same wx paragraph") { }
    SECTION("Separate paragraphs surrounding the table as in the showcase")
    {
        separate = true;
        left->AddParagraph("");
    }
    auto para = wxDynamicCast(left->GetChildren().GetLast()->GetData(), wxRichTextParagraph);
    auto inner = CreateTestTable(2, 2);
    para->AppendChild(inner);
    if ( separate )
        left->AddParagraph("after");
    else
        para->AppendChild(new wxRichTextPlainText("after", para));
    for ( int row = 0; row < 2; ++row )
    {
        for ( int col = 0; col < 2; ++col )
        {
            auto cell = inner->GetCell(row, col);
            cell->DeleteChildren();
            cell->AddParagraph(wxString::Format("inner %d,%d", row + 1, col + 1));
            cell->GetAttributes().GetTextBoxAttr().GetWidth().SetValue(80, wxTEXT_ATTR_UNITS_PIXELS);
        }
    }
    outer->GetCell(0, 1)->DeleteChildren();
    outer->GetCell(0, 1)->AddParagraph("sibling");
    buffer.AddParagraph("outside");

    // Only the owning row may supply leading trowd/cellx definitions. Nested
    // definitions occur after the cells and at their own active itap level.
    const wxString outerDefinition = "\\trowd\\trgaph0\\trleft0"
        "\\clftsWidth3\\clwWidth3000\\cellx3000\\clftsWidth3\\clwWidth4500\\cellx7500";
    const wxString innerDefinition = "\\trowd\\trgaph0\\trleft0"
        "\\clftsWidth3\\clwWidth1200\\cellx1200\\clftsWidth3\\clwWidth1200\\cellx2400";
    wxString body = "{" + outerDefinition + "\n{\\pard\\intbl\\itap1 {\\plain\\f0 before}";
    // A table alone in its paragraph is written without empty paragraphs.
    body += separate ? "\\par\n}" : "\\par}";
    body += "{\\intbl\\itap2\n"
            "{\\pard\\intbl\\itap2 {\\plain\\f0 inner 1,1}\\nestcell }"
            "{\\pard\\intbl\\itap2 {\\plain\\f0 inner 1,2}\\nestcell }"
            "{\\*\\nesttableprops" + innerDefinition + "\\nestrow}}\n"
            "{\\intbl\\itap2\n"
            "{\\pard\\intbl\\itap2 {\\plain\\f0 inner 2,1}\\nestcell }"
            "{\\pard\\intbl\\itap2 {\\plain\\f0 inner 2,2}\\nestcell }"
            "{\\*\\nesttableprops" + innerDefinition + "\\nestrow}}\n";
    body += "{\\pard\\intbl\\itap1 {\\plain\\f0 after}\\cell }"
            "{\\pard\\intbl\\itap1 {\\plain\\f0 sibling}\\cell }\\row}\n"
            "{" + outerDefinition + "\n"
            "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }"
            "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }\\row}\n"
            "{\\pard\\itap0 {\\plain\\f0 outside}\\par\n}";
    CheckRTF(body);
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFPageBreakWithInlineTable",
                 "[richtextctrl][rtf]")
{
    auto para = AddText("before");
    para->GetAttributes().SetPageBreak();
    bool tableFirst = false;
    SECTION("Text before the table carries the page break") { }
    SECTION("Table first still starts a new page")
    {
        tableFirst = true;
        para->DeleteChildren();
    }
    auto table = CreateTestTable(1, 1);
    para->AppendChild(table);
    para->AppendChild(new wxRichTextPlainText("after", para));
    CheckRTF(wxString(tableFirst ? "{\\pard\\itap0\\pagebb \\plain\\fs1\\sb0\\sa0\\sl1\\slmult0\\par}" :
                                  "{\\pard\\itap0\\pagebb {\\plain\\f0 before}\\par}") +
             "{\\trowd\\trgaph0\\trleft0\\clftsWidth3\\clwWidth9360\\cellx9360\n"
             "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }\\row}\n"
             "{\\pard\\itap0 {\\plain\\f0 after}\\par\n}");
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFListsInTables",
                 "[richtextctrl][rtf]")
{
    wxRichTextAttr numbered;
    numbered.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_ARABIC | wxTEXT_ATTR_BULLET_STYLE_PERIOD);
    numbered.SetBulletNumber(1);
    numbered.SetLeftIndent(127, 127);
    buffer.AddParagraph("before", &numbered);

    auto table = CreateTestTable(1, 2);
    int depth = 1;
    SECTION("Top-level table")
    {
        Anchor(buffer, table);
    }
    SECTION("Nested table")
    {
        auto outer = CreateTestTable(1, 1);
        Anchor(buffer, outer);
        auto para = wxDynamicCast(outer->GetCell(0, 0)->GetChildren().GetFirst()->GetData(),
                                  wxRichTextParagraph);
        para->AppendChild(table);
        depth = 2;
    }
    table->GetCell(0, 0)->DeleteChildren();
    numbered.SetBulletNumber(3);
    table->GetCell(0, 0)->AddParagraph("first", &numbered);
    numbered.SetBulletNumber(4);
    table->GetCell(0, 0)->AddParagraph("last", &numbered);
    wxRichTextAttr symbol;
    symbol.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_SYMBOL);
    symbol.SetBulletText("*");
    table->GetCell(0, 1)->DeleteChildren();
    table->GetCell(0, 1)->AddParagraph("bullet", &symbol);
    table->GetCell(0, 1)->AddParagraph("plain");
    numbered.SetBulletNumber(1);
    buffer.AddParagraph("after", &numbered);

    const wxString definition = "\\trowd\\trgaph0\\trleft0"
                                "\\clftsWidth3\\clwWidth4680\\cellx4680"
                                "\\clftsWidth3\\clwWidth4680\\cellx9360";
    const wxString paragraph = wxString::Format("{\\pard\\intbl\\itap%d", depth);
    const wxString cellEnd = depth == 1 ? "\\cell " : "\\nestcell ";
    wxString body = "{" + (depth == 1 ? definition : wxString("\\intbl\\itap2")) + "\n" +
        paragraph + "\\li1440\\fi-720\\tx1440 {\\listtext\\plain\\f0 3.\\tab}\\ls2\\ilvl0 "
                    "{\\plain\\f0 first}\\par\n}" +
        paragraph + "\\li1440\\fi-720\\tx1440 {\\listtext\\plain\\f0 4.\\tab}\\ls2\\ilvl0 "
                    "{\\plain\\f0 last}" + cellEnd + "}" +
        paragraph + " {\\listtext\\plain\\f0 * }\\ls3\\ilvl0 "
                    "{\\plain\\f0 bullet}\\par\n}" +
        paragraph + " {\\plain\\f0 plain}" + cellEnd + "}";
    if ( depth == 1 )
        body += "\\row}\n";
    else
    {
        body += "{\\*\\nesttableprops" + definition + "\\nestrow}}\n";
        body = "{\\trowd\\trgaph0\\trleft0\\clftsWidth3\\clwWidth9360\\cellx9360\n" + body +
               "{\\pard\\intbl\\itap1 \\cell}\\row}\n";
    }
    CheckRTF("{\\pard\\itap0\\li1440\\fi-720\\tx1440 "
             "{\\listtext\\plain\\f0 1.\\tab}\\ls1\\ilvl0 {\\plain\\f0 before}\\par\n}" +
             body + "{\\pard\\itap0\\li1440\\fi-720\\tx1440 "
             "{\\listtext\\plain\\f0 1.\\tab}\\ls4\\ilvl0 {\\plain\\f0 after}\\par\n}", "", "",
             // Each cell has its own lists. The consecutive items in the first
             // cell share one, while the restart after the table needs another.
             ListTables(ListDefinition(1, 0, 1, "\\'02\\'00.", "\\'01", "\\f0\\li1440\\fi-720\\tx1440") +
                        ListDefinition(2, 0, 3, "\\'02\\'00.", "\\'01", "\\f0\\li1440\\fi-720\\tx1440") +
                        ListDefinition(3, 23, 1, "\\'01*", "", "\\f0", 0, 1) +
                        ListDefinition(4, 0, 1, "\\'02\\'00.", "\\'01", "\\f0\\li1440\\fi-720\\tx1440"), 4));
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFRowHeights",
                 "[richtextctrl][rtf]")
{
    auto table = CreateTestTable(1, 2);
    Anchor(buffer, table);
    auto& first = table->GetCell(0, 0)->GetAttributes().GetTextBoxAttr().GetHeight();
    auto& second = table->GetCell(0, 1)->GetAttributes().GetTextBoxAttr().GetHeight();
    wxString height;
    wxString firstMerge, secondMerge;
    wxString secondContents = "{\\plain\\f0 }\\cell ";
    SECTION("Automatic height") { }
    SECTION("Specified pixel height is a minimum")
    {
        first.SetValue(90, wxTEXT_ATTR_UNITS_PIXELS);
        height = "\\trrh1350";
    }
    SECTION("Use tallest cell regardless of column order")
    {
        first.SetValue(30, wxTEXT_ATTR_UNITS_PIXELS);
        second.SetValue(120, wxTEXT_ATTR_UNITS_PIXELS);
        height = "\\trrh1800";
    }
    SECTION("Physical units")
    {
        second.SetValue(254, wxTEXT_ATTR_UNITS_TENTHS_MM);
        height = "\\trrh1440";
    }
    SECTION("Percentage heights remain automatic like native layout")
    {
        first.SetValue(90, wxTEXT_ATTR_UNITS_PERCENTAGE);
    }
    SECTION("Covered cells cannot increase the row height before layout")
    {
        table->GetCell(0, 0)->SetColSpan(2);
        first.SetValue(90, wxTEXT_ATTR_UNITS_PIXELS);
        second.SetValue(900, wxTEXT_ATTR_UNITS_PIXELS);
        firstMerge = "\\clmgf"; secondMerge = "\\clmrg";
        secondContents = "\\cell";
        height = "\\trrh1350";
    }
    SECTION("Hidden cells cannot increase row height")
    {
        second.SetValue(900, wxTEXT_ATTR_UNITS_PIXELS);
        table->GetCell(0, 1)->Show(false);
        secondContents = "\\cell";
    }
    CheckRTF("{\\trowd\\trgaph0\\trleft0" + height + firstMerge +
             "\\clftsWidth3\\clwWidth4680\\cellx4680" + secondMerge +
             "\\clftsWidth3\\clwWidth4680\\cellx9360\n"
             "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }"
             "{\\pard\\intbl\\itap1 " + secondContents + "}\\row}\n");
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFRowHeightWithPaddingAndBorders",
                 "[richtextctrl][rtf]")
{
    // A wx cell height includes the padding and borders, while Word adds the
    // largest top and bottom padding of the cells in the row and the top
    // border to the row height: e.g. write 90 - 7 - 7 - 1 pixels, so that the
    // row is 90 pixels tall in both.
    auto table = CreateTestTable(1, 2);
    Anchor(buffer, table);
    table->GetCell(0, 0)->GetAttributes().GetTextBoxAttr().GetHeight()
         .SetValue(90, wxTEXT_ATTR_UNITS_PIXELS);
    int padding[2][2] = { { 7, 7 }, { 7, 7 } }, borders[2][2] = { { 1, 1 }, { 1, 1 } };
    wxString height = "\\trrh1125";
    SECTION("Same padding and borders") { }
    SECTION("Largest top and bottom padding of the row")
    {
        // The top of one cell and the bottom of the other: 90 - 20 - 10 - 1.
        padding[1][0] = 20;
        padding[0][1] = 10;
        height = "\\trrh885";
    }
    SECTION("Top border")
    {
        borders[1][0] = 4;
        height = "\\trrh1080";
    }
    SECTION("Bottom border isn't added")
    {
        borders[1][1] = 4;
    }
    wxString cells;
    int boundary = 0;
    for ( int col = 0; col < 2; ++col )
    {
        auto& box = table->GetCell(0, col)->GetAttributes().GetTextBoxAttr();
        box.GetPadding().GetTop().SetValue(padding[col][0], wxTEXT_ATTR_UNITS_PIXELS);
        box.GetPadding().GetBottom().SetValue(padding[col][1], wxTEXT_ATTR_UNITS_PIXELS);
        box.GetBorder().GetTop().SetStyle(wxTEXT_BOX_ATTR_BORDER_SOLID);
        box.GetBorder().GetTop().SetWidth(borders[col][0], wxTEXT_ATTR_UNITS_PIXELS);
        box.GetBorder().GetBottom().SetStyle(wxTEXT_BOX_ATTR_BORDER_SOLID);
        box.GetBorder().GetBottom().SetWidth(borders[col][1], wxTEXT_ATTR_UNITS_PIXELS);
        boundary += 4680;
        cells += wxString::Format("\\clbrdrt\\brdrs\\brdrw%d\\clbrdrb\\brdrs\\brdrw%d"
                                  "\\clpadl%d\\clpadfl3\\clpadb%d\\clpadfb3"
                                  "\\clftsWidth3\\clwWidth4680\\cellx%d",
                                  borders[col][0] * 15, borders[col][1] * 15,
                                  padding[col][0] * 15, padding[col][1] * 15, boundary);
    }
    CheckRTF("{\\trowd\\trgaph0\\trleft0" + height + cells + "\n"
             "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }"
             "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }\\row}\n");
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFRowHeightOfSpanningCell",
                 "[richtextctrl][rtf]")
{
    // As in wx, the height of a cell spanning rows applies to its first row.
    // Its padding is written in, and added by Word to, every row it spans.
    auto table = CreateTestTable(2, 1);
    Anchor(buffer, table);
    auto cell = table->GetCell(0, 0);
    cell->SetRowSpan(2);
    auto& box = cell->GetAttributes().GetTextBoxAttr();
    box.GetHeight().SetValue(90, wxTEXT_ATTR_UNITS_PIXELS);
    box.GetPadding().GetTop().SetValue(7, wxTEXT_ATTR_UNITS_PIXELS);
    box.GetPadding().GetBottom().SetValue(7, wxTEXT_ATTR_UNITS_PIXELS);
    const wxString style = "\\clpadl105\\clpadfl3\\clpadb105\\clpadfb3"
                           "\\clftsWidth3\\clwWidth9360\\cellx9360\n";
    CheckRTF("{\\trowd\\trgaph0\\trleft0\\trrh1140\\clvmgf" + style +
             "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }\\row}\n"
             "{\\trowd\\trgaph0\\trleft0\\clvmrg" + style +
             "{\\pard\\intbl\\itap1 \\cell}\\row}\n");
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFTableParagraphSpacing",
                 "[richtextctrl][rtf]")
{
    // wx lays out the spacing of the paragraph containing a table around the
    // table, RTF tables have none: it is kept in paragraphs of minimal height.
    auto table = Anchor(buffer, CreateTestTable(1, 1));
    auto& attr = table->GetParent()->GetAttributes();
    const wxString row = "{\\trowd\\trgaph0\\trleft0\\clftsWidth3\\clwWidth9360\\cellx9360\n"
                         "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }\\row}\n";
    SECTION("No spacing")
    {
        CheckRTF(row);
    }
    SECTION("Spacing before")
    {
        attr.SetParagraphSpacingBefore(20);
        CheckRTF("{\\pard\\itap0\\sb113 \\plain\\fs1\\sl-1\\slmult0\\par}" + row);
    }
    SECTION("Spacing after")
    {
        attr.SetParagraphSpacingAfter(18);
        CheckRTF(row + "{\\pard\\itap0\\sa102 \\plain\\fs1\\sl-1\\slmult0\\par}");
    }
    SECTION("Spacing after a table ending a cell provides its cell mark")
    {
        auto cell = table->GetCell(0, 0);
        cell->DeleteChildren();
        auto inner = Anchor(*cell, CreateTestTable(1, 1));
        inner->GetParent()->GetAttributes().SetParagraphSpacingAfter(18);
        CheckRTF("{\\trowd\\trgaph0\\trleft0\\clftsWidth3\\clwWidth9360\\cellx9360\n"
                 "{\\intbl\\itap2\n"
                 "{\\pard\\intbl\\itap2 {\\plain\\f0 }\\nestcell }"
                 "{\\*\\nesttableprops\\trowd\\trgaph0\\trleft0"
                 "\\clftsWidth3\\clwWidth9360\\cellx9360\\nestrow}}\n"
                 "{\\pard\\intbl\\itap1\\sa102 \\plain\\fs1\\sl-1\\slmult0\\cell}\\row}\n");
    }
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFVerticallyMergedRowHeights",
                 "[richtextctrl][rtf]")
{
    auto table = CreateTestTable(3, 2);
    Anchor(buffer, table);
    auto origin = table->GetCell(0, 0);
    origin->SetRowSpan(2);
    origin->GetAttributes().GetTextBoxAttr().GetHeight().SetValue(90, wxTEXT_ATTR_UNITS_PIXELS);
    auto covered = table->GetCell(1, 0);
    covered->GetAttributes().GetTextBoxAttr().GetHeight().SetValue(900, wxTEXT_ATTR_UNITS_PIXELS);
    covered->AddParagraph("must not be exported");
    auto& adjacent = table->GetCell(1, 1)->GetAttributes().GetTextBoxAttr().GetHeight();
    adjacent.SetValue(30, wxTEXT_ATTR_UNITS_PIXELS);
    wxString secondHeight = "\\trrh450";
    SECTION("Before layout with a still-visible covered cell") { }
    SECTION("After layout hides the covered cell")
    {
        covered->Show(false);
    }
    SECTION("A taller adjacent cell sets the second row height independently")
    {
        adjacent.SetValue(120, wxTEXT_ATTR_UNITS_PIXELS);
        secondHeight = "\\trrh1800";
    }
    SECTION("Percentage height does not inherit the spanning origin height")
    {
        adjacent.SetValue(50, wxTEXT_ATTR_UNITS_PERCENTAGE);
        secondHeight.clear();
    }
    // The spanning origin contributes only to its own row, as in wx layout.
    // Its covered cell must affect neither height nor content. The third row
    // is automatic, so the preceding explicit height must not leak into it.
    CheckRTF("{\\trowd\\trgaph0\\trleft0\\trrh1350"
             "\\clvmgf\\clftsWidth3\\clwWidth4680\\cellx4680"
             "\\clftsWidth3\\clwWidth4680\\cellx9360\n"
             "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }"
             "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }\\row}\n"
             "{\\trowd\\trgaph0\\trleft0" + secondHeight +
             "\\clvmrg\\clftsWidth3\\clwWidth4680\\cellx4680"
             "\\clftsWidth3\\clwWidth4680\\cellx9360\n"
             "{\\pard\\intbl\\itap1 \\cell}"
             "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }\\row}\n"
             "{\\trowd\\trgaph0\\trleft0"
             "\\clftsWidth3\\clwWidth4680\\cellx4680"
             "\\clftsWidth3\\clwWidth4680\\cellx9360\n"
             "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }"
             "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }\\row}\n");
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFTableWidthFitting",
                 "[richtextctrl][rtf]")
{
    // The page's text width is 9360 twips (624 pixels).
    auto table = CreateTestTable(1, 2);
    Anchor(buffer, table);
    auto& tableAttr = table->GetAttributes().GetTextBoxAttr();
    for ( int col = 0; col < 2; ++col )
        table->GetCell(0, col)->GetAttributes().GetTextBoxAttr().GetWidth()
            .SetValue(400, wxTEXT_ATTR_UNITS_PIXELS);
    int left = 0, first = 4680, second = 4680;
    SECTION("Oversized absolute columns fit the parent") { }
    SECTION("Unequal oversized fixed columns use the native equal-share fallback")
    {
        table->GetCell(0, 1)->GetAttributes().GetTextBoxAttr().GetWidth()
            .SetValue(500, wxTEXT_ATTR_UNITS_PIXELS);
    }
    SECTION("Already fitting columns keep their widths")
    {
        for ( int col = 0; col < 2; ++col )
            table->GetCell(0, col)->GetAttributes().GetTextBoxAttr().GetWidth()
                .SetValue(100, wxTEXT_ATTR_UNITS_PIXELS);
        first = second = 1500;
    }
    SECTION("Explicit smaller table width limits its columns")
    {
        tableAttr.GetWidth().SetValue(200, wxTEXT_ATTR_UNITS_PIXELS);
        first = second = 1500;
    }
    SECTION("Percentage table width limits its columns")
    {
        tableAttr.GetWidth().SetValue(50, wxTEXT_ATTR_UNITS_PERCENTAGE);
        first = second = 2340;
    }
    SECTION("Oversized table width is limited by the parent")
    {
        tableAttr.GetWidth().SetValue(1000, wxTEXT_ATTR_UNITS_PIXELS);
    }
    SECTION("Automatic table ignores screen layout width")
    {
        table->SetCachedSize(wxSize(200, 100));
    }
    SECTION("Explicit table width takes precedence over stale layout")
    {
        table->SetCachedSize(wxSize(200, 100));
        tableAttr.GetWidth().SetValue(300, wxTEXT_ATTR_UNITS_PIXELS);
        first = second = 2250;
    }
    SECTION("Percentage columns cannot overflow the table")
    {
        for ( int col = 0; col < 2; ++col )
            table->GetCell(0, col)->GetAttributes().GetTextBoxAttr().GetWidth()
                .SetValue(80, wxTEXT_ATTR_UNITS_PERCENTAGE);
    }
    SECTION("Unspecified column still receives space when the other is too wide")
    {
        table->GetCell(0, 0)->GetAttributes().GetTextBoxAttr().GetWidth()
            .SetValue(700, wxTEXT_ATTR_UNITS_PIXELS);
        table->GetCell(0, 1)->GetAttributes().GetTextBoxAttr().GetWidth().Reset();
    }
    SECTION("Margins are reserved outside the column grid")
    {
        tableAttr.GetMargins().GetLeft().SetValue(10, wxTEXT_ATTR_UNITS_PIXELS);
        tableAttr.GetMargins().GetRight().SetValue(20, wxTEXT_ATTR_UNITS_PIXELS);
        left = 150;
        first = second = 4455;
    }
    SECTION("Rounding remainder belongs to the last column")
    {
        tableAttr.GetWidth().SetValue(201, wxTEXT_ATTR_UNITS_PIXELS);
        first = 1507;
        second = 1508;
    }
    const wxRichTextAttr before = table->GetAttributes();
    const wxSize cacheBefore = table->GetCachedSize();
    CheckRTF(wxString::Format("{\\trowd\\trgaph0\\trleft%d"
             "\\clftsWidth3\\clwWidth%d\\cellx%d"
             "\\clftsWidth3\\clwWidth%d\\cellx%d\n",
             left, first, left + first, second, left + first + second) +
             "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }"
             "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }\\row}\n");
    CHECK(table->GetAttributes() == before);
    CHECK(table->GetCachedSize() == cacheBefore);
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFSpanningWidthFitting",
                 "[richtextctrl][rtf]")
{
    // An 800-pixel span (12000 twips) is wider than the page (9360 twips).
    auto table = CreateTestTable(2, 2);
    Anchor(buffer, table);
    auto span = table->GetCell(0, 0);
    span->SetColSpan(2);
    span->GetAttributes().GetTextBoxAttr().GetWidth().SetValue(800, wxTEXT_ATTR_UNITS_PIXELS);
    for ( int col = 0; col < 2; ++col )
        table->GetCell(1, col)->GetAttributes().GetTextBoxAttr().GetWidth()
            .SetValue(100, wxTEXT_ATTR_UNITS_PIXELS);
    CheckRTF("{\\trowd\\trgaph0\\trleft0"
             "\\clmgf\\clftsWidth3\\clwWidth4680\\cellx4680"
             "\\clmrg\\clftsWidth3\\clwWidth4680\\cellx9360\n"
             "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }"
             "{\\pard\\intbl\\itap1 \\cell}\\row}\n"
             "{\\trowd\\trgaph0\\trleft0"
             "\\clftsWidth3\\clwWidth4680\\cellx4680"
             "\\clftsWidth3\\clwWidth4680\\cellx9360\n"
             "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }"
             "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }\\row}\n");
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFNestedWidthFitting",
                 "[richtextctrl][rtf]")
{
    auto outer = CreateTestTable(1, 1);
    Anchor(buffer, outer);
    auto cell = outer->GetCell(0, 0);
    auto& cellAttr = cell->GetAttributes().GetTextBoxAttr();
    cellAttr.GetWidth().SetValue(200, wxTEXT_ATTR_UNITS_PIXELS);
    cellAttr.GetPadding().GetLeft().SetValue(10, wxTEXT_ATTR_UNITS_PIXELS);
    cellAttr.GetPadding().GetRight().SetValue(10, wxTEXT_ATTR_UNITS_PIXELS);
    auto para = wxDynamicCast(cell->GetChildren().GetFirst()->GetData(), wxRichTextParagraph);
    auto inner = CreateTestTable(1, 2);
    para->AppendChild(inner);
    for ( int col = 0; col < 2; ++col )
        inner->GetCell(0, col)->GetAttributes().GetTextBoxAttr().GetWidth()
            .SetValue(200, wxTEXT_ATTR_UNITS_PIXELS);
    // A sibling table must use the outer available width again.
    auto sibling = CreateTestTable(1, 1);
    Anchor(buffer, sibling);
    CheckRTF("{\\trowd\\trgaph0\\trleft0\\clpadt150\\clpadft3\\clpadr150\\clpadfr3"
             "\\clftsWidth3\\clwWidth3000\\cellx3000\n"
             "{\\intbl\\itap2\n"
             "{\\pard\\intbl\\itap2 {\\plain\\f0 }\\nestcell }"
             "{\\pard\\intbl\\itap2 {\\plain\\f0 }\\nestcell }"
             "{\\*\\nesttableprops\\trowd\\trgaph0\\trleft0"
             "\\clftsWidth3\\clwWidth1350\\cellx1350"
             "\\clftsWidth3\\clwWidth1350\\cellx2700\\nestrow}}\n"
             "{\\pard\\intbl\\itap1 \\cell}\\row}\n"
             // Keeps the sibling a separate table.
             "{\\pard\\itap0\\plain\\fs1\\sl-1\\slmult0\\par}"
             "{\\trowd\\trgaph0\\trleft0\\clftsWidth3\\clwWidth9360\\cellx9360\n"
             "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }\\row}\n");
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFBoxMarginBounds",
                 "[richtextctrl][rtf]")
{
    auto box = CreateTestBox();
    Anchor(buffer, box);
    box->AddParagraph("box");
    auto& attr = box->GetAttributes().GetTextBoxAttr();
    attr.GetMargins().GetLeft().SetValue(10, wxTEXT_ATTR_UNITS_PIXELS);
    attr.GetMargins().GetTop().SetValue(20, wxTEXT_ATTR_UNITS_PIXELS);
    attr.GetMargins().GetRight().SetValue(30, wxTEXT_ATTR_UNITS_PIXELS);
    attr.GetMargins().GetBottom().SetValue(40, wxTEXT_ATTR_UNITS_PIXELS);
    attr.GetWidth().SetValue(200, wxTEXT_ATTR_UNITS_PIXELS);
    attr.GetHeight().SetValue(100, wxTEXT_ATTR_UNITS_PIXELS);
    wxString position = "\\shpleft150\\shptop300\\shpright2550\\shpbottom900";
    wxString wrap = "\\shpwr1";
    bool autoHeight = false;
    SECTION("Explicit dimensions describe the outer margin box") { }
    SECTION("Automatic dimensions ignore cached screen layout")
    {
        attr.GetWidth().Reset();
        attr.GetHeight().Reset();
        box->SetCachedSize(wxSize(200, 100));
        position = "\\shpleft150\\shptop300\\shpright8910\\shpbottom540";
        autoHeight = true;
    }
    SECTION("Explicit dimensions override stale cached layout")
    {
        box->SetCachedSize(wxSize(800, 400));
    }
    SECTION("Percentage width is resolved before removing margins")
    {
        // Half of the page's 9360 twips, less 150 and 450 twips of margins.
        attr.GetWidth().SetValue(50, wxTEXT_ATTR_UNITS_PERCENTAGE);
        position = "\\shpleft150\\shptop300\\shpright4230\\shpbottom900";
    }
    SECTION("Unlaid-out auto width reserves margins within available space")
    {
        attr.GetWidth().Reset();
        position = "\\shpleft150\\shptop300\\shpright8910\\shpbottom900";
    }
    SECTION("Automatic width ignores a larger screen layout cache")
    {
        attr.GetWidth().Reset();
        box->SetCachedSize(wxSize(800, 100));
        position = "\\shpleft150\\shptop300\\shpright8910\\shpbottom900";
    }
    SECTION("Maximum outer width applies independently of cached dimensions")
    {
        attr.GetWidth().Reset();
        box->SetCachedSize(wxSize(400, 100));
        attr.GetMaxSize().GetWidth().SetValue(200, wxTEXT_ATTR_UNITS_PIXELS);
    }
    SECTION("Right floating frame leaves its right margin outside")
    {
        attr.SetFloatMode(wxTEXT_BOX_ATTR_FLOAT_RIGHT);
        position = "\\shpleft6510\\shptop300\\shpright8910\\shpbottom900";
        wrap = "\\shpwr2\\shpwrk1";
    }
    SECTION("Offsets position the margin box rather than the border")
    {
        attr.GetLeft().SetValue(5, wxTEXT_ATTR_UNITS_PIXELS);
        attr.GetTop().SetValue(10, wxTEXT_ATTR_UNITS_PIXELS);
        position = "\\shpleft225\\shptop450\\shpright2625\\shpbottom1050";
    }
    const wxRichTextAttr before = box->GetAttributes();
    const wxSize cacheBefore = box->GetCachedSize();
    CheckRTF("{\\shp{\\*\\shpinst" + position + "\\shpbxcolumn\\shpbypara" + wrap +
             "{\\sp{\\sn shapeType}{\\sv 202}}" + PlainTextBoxProperties(autoHeight) +
             "{\\sp{\\sn dxWrapDistLeft}{\\sv 95250}}"
             "{\\sp{\\sn dyWrapDistTop}{\\sv 190500}}"
             "{\\sp{\\sn dxWrapDistRight}{\\sv 285750}}"
             "{\\sp{\\sn dyWrapDistBottom}{\\sv 381000}}" +
             ShapeText("{\\pard\\itap0 {\\plain\\f0 box}\\par\n}") + ShapeAnchor());
    CHECK(box->GetAttributes() == before);
    CHECK(box->GetCachedSize() == cacheBefore);
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFScreenLayoutIndependence",
                 "[richtextctrl][rtf]")
{
    auto box = CreateTestBox();
    box->SetMargins(20);
    box->AddParagraph("text");
    Anchor(buffer, box);
    auto table = CreateTestTable(1, 2);
    Anchor(buffer, table);
    const wxRichTextAttr boxBefore = box->GetAttributes();
    const wxRichTextAttr tableBefore = table->GetAttributes();
    const wxString expected =
        "{\\shp{\\*\\shpinst\\shpleft300\\shptop300\\shpright9060\\shpbottom540"
        "\\shpbxcolumn\\shpbypara\\shpwr1{\\sp{\\sn shapeType}{\\sv 202}}" +
        PlainTextBoxProperties(true) +
        "{\\sp{\\sn dxWrapDistLeft}{\\sv 190500}}"
        "{\\sp{\\sn dyWrapDistTop}{\\sv 190500}}"
        "{\\sp{\\sn dxWrapDistRight}{\\sv 190500}}"
        "{\\sp{\\sn dyWrapDistBottom}{\\sv 190500}}" +
        ShapeText("{\\pard\\itap0 {\\plain\\f0 text}\\par\n}") + ShapeAnchor() +
        "{\\trowd\\trgaph0\\trleft0"
        "\\clftsWidth3\\clwWidth4680\\cellx4680"
        "\\clftsWidth3\\clwWidth4680\\cellx9360\n"
        "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }"
        "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }\\row}\n";
    // No layout, followed by representative 100%, 125%, 150% and 200%
    // screen caches. Test complete literal RTF, not rendered RTF controls.
    for ( int scale : { 0, 100, 125, 150, 200 } )
    {
        INFO("Simulated layout scale: " << scale);
        const wxSize size(800 * scale / 100, 80 * scale / 100);
        box->SetCachedSize(size);
        table->SetCachedSize(size);
        table->GetCell(0, 0)->SetCachedSize(size);
        box->SetPosition(wxPoint(scale, scale));
        table->SetPosition(wxPoint(scale, 2 * scale));
        buffer.SetScale(scale == 0 ? 1.0 : scale / 100.0);
        buffer.SetFontScale(scale == 0 ? 1.0 : scale / 100.0);
        CheckRTF(expected);
        CHECK(box->GetAttributes() == boxBefore);
        CHECK(table->GetAttributes() == tableBefore);
        CHECK(box->GetCachedSize() == size);
        CHECK(table->GetCachedSize() == size);
    }
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFConsecutiveTextBoxes",
                 "[richtextctrl][rtf]")
{
    // A shape is anchored to the following paragraph. Each text box gets an
    // anchor of its own, otherwise consecutive boxes overlap in Word. The
    // anchor keeps the spacing of the wx paragraph containing the box.
    Anchor(buffer, CreateTestBox())->AddParagraph("first");
    Anchor(buffer, CreateTestBox())->AddParagraph("second");
    auto last = wxDynamicCast(buffer.GetChildren().GetLast()->GetData(), wxRichTextParagraph);
    last->GetAttributes().SetParagraphSpacingAfter(30);
    const wxString shape =
        "{\\shp{\\*\\shpinst\\shpleft0\\shptop0\\shpright9360\\shpbottom240"
        "\\shpbxcolumn\\shpbypara\\shpwr1{\\sp{\\sn shapeType}{\\sv 202}}" +
        PlainTextBoxProperties(true);
    CheckRTF(shape + ShapeText("{\\pard\\itap0 {\\plain\\f0 first}\\par\n}") + ShapeAnchor() +
             shape + ShapeText("{\\pard\\itap0 {\\plain\\f0 second}\\par\n}") +
             "{\\pard\\itap0\\sa170 \\plain\\fs1\\sl-1\\slmult0\\par}");
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFAutoHeightPadding",
                 "[richtextctrl][rtf]")
{
    // Word fits a shape with an automatic height to its text, but limits the
    // text insets to the provisional height: it must include the padding.
    auto box = Anchor(buffer, CreateTestBox());
    box->AddParagraph("box");
    auto& padding = box->GetAttributes().GetTextBoxAttr().GetPadding();
    wxString bottom, top, bottomInset;
    SECTION("Absolute padding is part of the provisional height")
    {
        padding.GetTop().SetValue(12, wxTEXT_ATTR_UNITS_PIXELS);
        padding.GetBottom().SetValue(6, wxTEXT_ATTR_UNITS_PIXELS);
        bottom = "510";
        top = "114300";
        bottomInset = "57150";
    }
    SECTION("Percentage padding is no space, as in wx")
    {
        padding.GetTop().SetValue(10, wxTEXT_ATTR_UNITS_PERCENTAGE);
        bottom = "240";
        top = "0";
        bottomInset = "0";
    }
    CheckRTF("{\\shp{\\*\\shpinst\\shpleft0\\shptop0\\shpright9360\\shpbottom" + bottom +
             "\\shpbxcolumn\\shpbypara\\shpwr1{\\sp{\\sn shapeType}{\\sv 202}}"
             "{\\sp{\\sn fPseudoInline}{\\sv 0}}{\\sp{\\sn fFitShapeToText}{\\sv 1}}"
             "{\\sp{\\sn fLine}{\\sv 0}}{\\sp{\\sn dxTextLeft}{\\sv 0}}"
             "{\\sp{\\sn dyTextTop}{\\sv " + top + "}}{\\sp{\\sn dxTextRight}{\\sv 0}}"
             "{\\sp{\\sn dyTextBottom}{\\sv " + bottomInset + "}}"
             "{\\sp{\\sn anchorText}{\\sv 0}}{\\sp{\\sn fFilled}{\\sv 0}}" +
             ShapeText("{\\pard\\itap0 {\\plain\\f0 box}\\par\n}") + ShapeAnchor());
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFNestedBoxMarginBounds",
                 "[richtextctrl][rtf]")
{
    auto outer = CreateTestBox();
    Anchor(buffer, outer);
    outer->GetAttributes().GetTextBoxAttr().GetWidth().SetValue(400, wxTEXT_ATTR_UNITS_PIXELS);
    outer->GetAttributes().GetTextBoxAttr().GetHeight().SetValue(200, wxTEXT_ATTR_UNITS_PIXELS);
    auto inner = CreateTestBox();
    Anchor(*outer, inner);
    inner->GetAttributes().GetTextBoxAttr().GetWidth().SetValue(50, wxTEXT_ATTR_UNITS_PERCENTAGE);
    inner->GetAttributes().GetTextBoxAttr().GetMargins().GetLeft().SetValue(10, wxTEXT_ATTR_UNITS_PIXELS);
    inner->GetAttributes().GetTextBoxAttr().GetMargins().GetRight().SetValue(30, wxTEXT_ATTR_UNITS_PIXELS);
    inner->AddParagraph("inside");
    CheckRTF("{\\shp{\\*\\shpinst\\shpleft0\\shptop0\\shpright6000\\shpbottom3000"
             "\\shpbxcolumn\\shpbypara\\shpwr1{\\sp{\\sn shapeType}{\\sv 202}}" +
             PlainTextBoxProperties() +
             ShapeText("{\\trowd\\trgaph0\\trleft150\\clftsWidth3\\clwWidth2400\\cellx2550\n"
                       "{\\pard\\intbl\\itap1 {\\plain\\f0 inside}\\cell }\\row}\n") +
             ShapeAnchor());
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFBoxDimensions",
                 "[richtextctrl][rtf]")
{
    // Like wx, which lays out the buffer in the window or the printed page,
    // the export ignores the size of the buffer itself: the boxes are a half
    // and a quarter of the page's text width.
    buffer.GetAttributes().GetTextBoxAttr().GetWidth().SetValue(400, wxTEXT_ATTR_UNITS_PIXELS);
    buffer.AddParagraph("");
    auto para = wxDynamicCast(buffer.GetChildren().GetFirst()->GetData(), wxRichTextParagraph);
    auto outer = CreateTestBox();
    outer->GetAttributes().GetTextBoxAttr().GetWidth().SetValue(50, wxTEXT_ATTR_UNITS_PERCENTAGE);
    outer->AddParagraph("");
    auto inner = CreateTestBox();
    inner->GetAttributes().GetTextBoxAttr().GetWidth().SetValue(50, wxTEXT_ATTR_UNITS_PERCENTAGE);
    inner->AddParagraph("inside");
    wxDynamicCast(outer->GetChildren().GetFirst()->GetData(), wxRichTextParagraph)->AppendChild(inner);
    para->AppendChild(outer);
    // Boxes alone in their paragraphs are written as blocks.
    CheckRTF("{\\shp{\\*\\shpinst\\shpleft0\\shptop0\\shpright4680\\shpbottom240"
             "\\shpbxcolumn\\shpbypara\\shpwr1{\\sp{\\sn shapeType}{\\sv 202}}" +
             PlainTextBoxProperties(true) +
             ShapeText("{\\trowd\\trgaph0\\trleft0\\clftsWidth3\\clwWidth2340\\cellx2340\n"
                       "{\\pard\\intbl\\itap1 {\\plain\\f0 inside}\\cell }\\row}\n") +
             ShapeAnchor());
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFBoxPercentageOfContainer",
                 "[richtextctrl][rtf]")
{
    // wx resolves the percentage width of a box in a paragraph relative to the
    // outer size of its container, including the buffer's margins and the
    // padding of an enclosing box, but floating boxes relative to its content.
    auto& margins = buffer.GetAttributes().GetTextBoxAttr().GetMargins();
    margins.GetLeft().SetValue(20, wxTEXT_ATTR_UNITS_PIXELS);
    margins.GetRight().SetValue(20, wxTEXT_ATTR_UNITS_PIXELS);
    auto outer = Anchor(buffer, CreateTestBox());
    auto& outerAttr = outer->GetAttributes().GetTextBoxAttr();
    outerAttr.GetWidth().SetValue(60, wxTEXT_ATTR_UNITS_PERCENTAGE);
    outerAttr.GetPadding().GetLeft().SetValue(12, wxTEXT_ATTR_UNITS_PIXELS);
    outerAttr.GetPadding().GetRight().SetValue(12, wxTEXT_ATTR_UNITS_PIXELS);
    auto inner = Anchor(*outer, CreateTestBox());
    inner->GetAttributes().GetTextBoxAttr().GetWidth().SetValue(50, wxTEXT_ATTR_UNITS_PERCENTAGE);
    inner->AddParagraph("inside");
    // 60% of 9360 + 2 * 300 twips, or of 9360 for a floating box. The nested
    // box is 50% of that, not of its content without the 2 * 180 padding.
    wxString width = "5976", innerWidth = "2988", wrap = "\\shpwr1", wrapDistance;
    SECTION("Box in a paragraph") { }
    SECTION("Floating box")
    {
        outerAttr.SetFloatMode(wxTEXT_BOX_ATTR_FLOAT_LEFT);
        width = "5616";
        innerWidth = "2808";
        wrap = "\\shpwr2\\shpwrk2";
        wrapDistance = "{\\sp{\\sn dxWrapDistLeft}{\\sv 0}}{\\sp{\\sn dxWrapDistRight}{\\sv 0}}";
    }
    const wxString table = "{\\trowd\\trgaph0\\trleft0\\clftsWidth3\\clwWidth" + innerWidth +
                           "\\cellx" + innerWidth + "\n"
                           "{\\pard\\intbl\\itap1 {\\plain\\f0 inside}\\cell }\\row}\n";
    // The horizontal padding of 180 twips becomes text insets in EMU.
    wxString properties = PlainTextBoxProperties(true);
    properties.Replace("{\\sn dxTextLeft}{\\sv 0}", "{\\sn dxTextLeft}{\\sv 114300}");
    properties.Replace("{\\sn dxTextRight}{\\sv 0}", "{\\sn dxTextRight}{\\sv 114300}");
    CheckRTF("{\\shp{\\*\\shpinst\\shpleft0\\shptop0\\shpright" + width + "\\shpbottom240"
             "\\shpbxcolumn\\shpbypara" + wrap + "{\\sp{\\sn shapeType}{\\sv 202}}" +
             properties + wrapDistance + ShapeText(table) + ShapeAnchor());
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFNestedTextBoxes",
                 "[richtextctrl][rtf]")
{
    // Word cannot import a shape inside another text box's shptxt story.
    // Check the complete fallback RTF, without depending on an RTF control.
    auto outer = CreateTestBox();
    Anchor(buffer, outer);
    outer->AddParagraph("before");
    auto para = wxDynamicCast(outer->GetChildren().GetFirst()->GetData(), wxRichTextParagraph);
    auto inner = CreateTestBox();
    inner->GetAttributes().GetTextBoxAttr().GetWidth().SetValue(100, wxTEXT_ATTR_UNITS_PIXELS);
    para->AppendChild(inner);
    para->AppendChild(new wxRichTextPlainText("after", para));

    wxString definition = "\\trowd\\trgaph0\\trleft0";
    wxString contents;
    wxString colours;
    SECTION("Empty nested box still has a cell terminator")
    {
        contents = "{\\pard\\intbl\\itap1 \\cell}";
    }
    SECTION("Explicit height on an empty nested box")
    {
        inner->GetAttributes().GetTextBoxAttr().GetHeight().SetValue(90, wxTEXT_ATTR_UNITS_PIXELS);
        definition += "\\trrh1350";
        contents = "{\\pard\\intbl\\itap1 \\cell}";
    }
    SECTION("Explicit height on a populated nested box")
    {
        inner->GetAttributes().GetTextBoxAttr().GetHeight().SetValue(90, wxTEXT_ATTR_UNITS_PIXELS);
        definition += "\\trrh1350";
        inner->AddParagraph("tall");
        contents = "{\\pard\\intbl\\itap1 {\\plain\\f0 tall}\\cell }";
    }
    SECTION("Minimum height alone does not force the available page height")
    {
        inner->GetAttributes().GetTextBoxAttr().GetMinSize().GetHeight()
            .SetValue(90, wxTEXT_ATTR_UNITS_PIXELS);
        definition += "\\trrh1350";
        contents = "{\\pard\\intbl\\itap1 \\cell}";
    }
    SECTION("Percentage padding on a sized nested box")
    {
        auto& box = inner->GetAttributes().GetTextBoxAttr();
        box.GetHeight().SetValue(80, wxTEXT_ATTR_UNITS_PIXELS);
        box.GetPadding().GetLeft().SetValue(10, wxTEXT_ATTR_UNITS_PERCENTAGE);
        box.GetPadding().GetTop().SetValue(25, wxTEXT_ATTR_UNITS_PERCENTAGE);
        // Percentage padding is no space, as in wx: the row is 80 pixels.
        definition += "\\trrh1200";
        inner->AddParagraph("padded");
        contents = "{\\pard\\intbl\\itap1 {\\plain\\f0 padded}\\cell }";
    }
    SECTION("Nested box preserves fill borders padding and multiple paragraphs")
    {
        inner->GetAttributes().SetBackgroundColour(wxColour(255, 0, 0));
        auto& box = inner->GetAttributes().GetTextBoxAttr();
        box.GetBorder().SetStyle(wxTEXT_BOX_ATTR_BORDER_SOLID);
        box.GetBorder().SetColour(wxColour(0, 0, 0));
        box.GetBorder().SetWidth(2, wxTEXT_ATTR_UNITS_PIXELS);
        box.GetPadding().GetLeft().SetValue(3, wxTEXT_ATTR_UNITS_PIXELS);
        definition += "\\clbrdrt\\brdrs\\brdrw30\\brdrcf2"
                      "\\clbrdrl\\brdrs\\brdrw30\\brdrcf2"
                      "\\clbrdrb\\brdrs\\brdrw30\\brdrcf2"
                      "\\clbrdrr\\brdrs\\brdrw30\\brdrcf2"
                      "\\clcbpat1\\clpadt45\\clpadft3";
        colours = "\\red255\\green0\\blue0;\\red0\\green0\\blue0;";
        inner->AddParagraph("first");
        inner->AddParagraph("second");
        contents = "{\\pard\\intbl\\itap1 {\\plain\\f0 first}\\par\n}"
                   "{\\pard\\intbl\\itap1 {\\plain\\f0 second}\\cell }";
    }
    SECTION("Further nested boxes use nested table rows")
    {
        inner->AddParagraph("first");
        auto innerPara = wxDynamicCast(inner->GetChildren().GetFirst()->GetData(), wxRichTextParagraph);
        auto deep = CreateTestBox();
        deep->GetAttributes().GetTextBoxAttr().GetWidth().SetValue(50, wxTEXT_ATTR_UNITS_PERCENTAGE);
        deep->GetAttributes().GetTextBoxAttr().GetHeight().SetValue(40, wxTEXT_ATTR_UNITS_PIXELS);
        deep->AddParagraph("deep");
        innerPara->AppendChild(deep);
        innerPara->AppendChild(new wxRichTextPlainText("last", innerPara));
        contents = "{\\pard\\intbl\\itap1 {\\plain\\f0 first}\\par}"
                   "{\\intbl\\itap2\n"
                   "{\\pard\\intbl\\itap2 {\\plain\\f0 deep}\\nestcell }"
                   "{\\*\\nesttableprops\\trowd\\trgaph0\\trleft0\\trrh600"
                   "\\clftsWidth3\\clwWidth750\\cellx750\\nestrow}}\n"
                   "{\\pard\\intbl\\itap1 {\\plain\\f0 last}\\cell }";
    }
    definition += "\\clftsWidth3\\clwWidth1500\\cellx1500";

    // A following top-level box must still be a shape, not inherit the fallback.
    auto sibling = CreateTestBox();
    sibling->AddParagraph("sibling");
    Anchor(buffer, sibling);
    const wxString shape =
        "{\\shp{\\*\\shpinst\\shpleft0\\shptop0\\shpright9360\\shpbottom240"
        "\\shpbxcolumn\\shpbypara\\shpwr1{\\sp{\\sn shapeType}{\\sv 202}}" +
        PlainTextBoxProperties(true);
    CheckRTF(shape + ShapeText("{\\pard\\itap0 {\\plain\\f0 before}\\par}"
                               "{" + definition + "\n" + contents + "\\row}\n"
                               "{\\pard\\itap0 {\\plain\\f0 after}\\par\n}") + ShapeAnchor() +
             shape + ShapeText("{\\pard\\itap0 {\\plain\\f0 sibling}\\par\n}") + ShapeAnchor(),
             colours);
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFTextBoxFormatting",
                 "[richtextctrl][rtf]")
{
    auto box = CreateTestBox();
    Anchor(buffer, box);
    box->AddParagraph("box");
    auto& attr = box->GetAttributes().GetTextBoxAttr();
    attr.GetWidth().SetValue(100, wxTEXT_ATTR_UNITS_PIXELS);
    attr.GetHeight().SetValue(80, wxTEXT_ATTR_UNITS_PIXELS);
    wxString position = "\\shpleft0\\shptop0\\shpright1500\\shpbottom1200";
    wxString wrap = "\\shpwr1";
    wxString pseudoInline = "0", line = "{\\sp{\\sn fLine}{\\sv 0}}";
    wxString left = "0", top = "0", right = "0", bottom = "0", anchor = "0", colours;
    // Floating boxes override Word's default distance from wrapping text.
    const wxString floatWrapDistance = "{\\sp{\\sn dxWrapDistLeft}{\\sv 0}}"
                                       "{\\sp{\\sn dxWrapDistRight}{\\sv 0}}";
    wxString wrapDistance;
    wxString contents = "{\\pard\\itap0 {\\plain\\f0 box}\\par\n}";
    wxString shape = "{\\sp{\\sn shapeType}{\\sv 202}}";
    // Readers without shape support show bordered boxes as a bordered cell.
    wxString fallbackCell;
    // Its row height excludes the vertical padding and borders, which Word adds.
    wxString fallbackHeight = "\\trrh1200";
    // Set for a box fitted to its text.
    wxString fit;
    // A rounded rectangle path, given the first guide, radius (in twips) *
    // 635, and the radii and the distances of the Bezier control points from
    // the edges, in coordinates of the 21600 x 21600 shape. The horizontal
    // ones are constant. Word computes the vertical ones from the final
    // height with the guides 2-5 and reads them as the last value of a
    // vertex, Writer uses the second one, for the given height.
    const auto rounded = [](const wxString& radiusGuide, int rx, int kx, int ry, int ky)
    {
        const int s = 21600;
        return "{\\sp{\\sn shapeType}{\\sv 100}}"
               "{\\sp{\\sn pGuides}{\\sv 8;6;" + radiusGuide + ";(67133444,1279);"
               "(67231745,83689795);(67248129,655364477);(21209088,67239936);(21209088,67305472)}}" +
               wxString::Format("{\\sp{\\sn pVerticies}{\\sv 8;17;(%d,0);(%d,0);(%d,0);"
                   "(21600,%d,2147483651);(21600,%d,2147483650);"
                   "(21600,%d,2147483652);(21600,%d,2147483653);(%d,21600);(%d,21600);"
                   "(%d,21600);(%d,21600);(0,%d,2147483653);(0,%d,2147483652);"
                   "(0,%d,2147483650);(0,%d,2147483651);(%d,0);(%d,0)}}",
                   rx, s - rx, s - kx, ky, ry, s - ry, s - ky, s - kx, s - rx,
                   rx, kx, s - ky, s - ry, ry, ky, kx, rx) +
               "{\\sp{\\sn pSegmentInfo}{\\sv 2;11;16384;1;8193;1;8193;1;8193;1;8193;24577;32768}}"
               "{\\sp{\\sn pInscribe}{\\sv 16;1;01000100100000000000000000006054000060540000}}";
    };
    SECTION("In-flow borderless box wraps top and bottom without pseudo-inline overlap") { }
    SECTION("Corner radius uses a path with an absolute radius")
    {
        attr.SetCornerRadius(wxTextAttrDimension(10, wxTEXT_ATTR_UNITS_PIXELS));
        // 150 twips: (sgfProduct | 150 << 16, 635 | 1 << 16), 10% of the
        // width and 12.5% of the height.
        shape = rounded("(9830401,66171)", 2160, 967, 2700, 1209);
    }
    SECTION("Corner radius is limited to half of each side")
    {
        attr.SetCornerRadius(wxTextAttrDimension(100, wxTEXT_ATTR_UNITS_PIXELS));
        shape = rounded("(98304001,66171)", 10800, 4835, 10800, 4835);
    }
    SECTION("Corner radius of an automatic height doesn't depend on the provisional one")
    {
        // Word fits the shape to its text, the radius must remain 10 pixels.
        // Writer only gets it limited to half of the provisional height.
        attr.GetHeight().Reset();
        attr.SetCornerRadius(wxTextAttrDimension(10, wxTEXT_ATTR_UNITS_PIXELS));
        position = "\\shpleft0\\shptop0\\shpright1500\\shpbottom240";
        fit = "{\\sp{\\sn fFitShapeToText}{\\sv 1}}";
        shape = rounded("(9830401,66171)", 2160, 967, 10800, 4835);
    }
    SECTION("Zero corner radius keeps the rectangular text box")
    {
        attr.SetCornerRadius(wxTextAttrDimension(0, wxTEXT_ATTR_UNITS_PIXELS));
    }
    SECTION("Left float allows text only on the right")
    {
        attr.SetFloatMode(wxTEXT_BOX_ATTR_FLOAT_LEFT);
        wrap = "\\shpwr2\\shpwrk2";
        pseudoInline = "0";
        wrapDistance = floatWrapDistance;
    }
    SECTION("Right float is placed at the container right edge")
    {
        attr.SetFloatMode(wxTEXT_BOX_ATTR_FLOAT_RIGHT);
        position = "\\shpleft7860\\shptop0\\shpright9360\\shpbottom1200";
        wrap = "\\shpwr2\\shpwrk1";
        pseudoInline = "0";
        wrapDistance = floatWrapDistance;
    }
    SECTION("Explicit offsets remain anchored rather than inline")
    {
        attr.GetLeft().SetValue(10, wxTEXT_ATTR_UNITS_PIXELS);
        attr.GetTop().SetValue(20, wxTEXT_ATTR_UNITS_PIXELS);
        position = "\\shpleft150\\shptop300\\shpright1650\\shpbottom1500";
        pseudoInline = "0";
    }
    SECTION("Dashed uniform border")
    {
        fallbackHeight = "\\trrh1110";
        fallbackCell = "\\clbrdrt\\brdrdash\\brdrw45\\brdrcf1\\clbrdrl\\brdrdash\\brdrw45\\brdrcf1"
                       "\\clbrdrb\\brdrdash\\brdrw45\\brdrcf1\\clbrdrr\\brdrdash\\brdrw45\\brdrcf1";
        attr.GetBorder().SetStyle(wxTEXT_BOX_ATTR_BORDER_DASHED);
        attr.GetBorder().SetWidth(3, wxTEXT_ATTR_UNITS_PIXELS);
        attr.GetBorder().SetColour(wxColour(255, 0, 0));
        colours = "\\red255\\green0\\blue0;";
        line = "{\\sp{\\sn fLine}{\\sv 1}}{\\sp{\\sn lineWidth}{\\sv 28575}}"
               "{\\sp{\\sn lineColor}{\\sv 255}}{\\sp{\\sn lineDashing}{\\sv 1}}";
    }
    SECTION("Dotted uniform border")
    {
        fallbackHeight = "\\trrh1140";
        fallbackCell = "\\clbrdrt\\brdrdot\\brdrw30\\clbrdrl\\brdrdot\\brdrw30"
                       "\\clbrdrb\\brdrdot\\brdrw30\\clbrdrr\\brdrdot\\brdrw30";
        attr.GetBorder().SetStyle(wxTEXT_BOX_ATTR_BORDER_DOTTED);
        attr.GetBorder().SetWidth(2, wxTEXT_ATTR_UNITS_PIXELS);
        line = "{\\sp{\\sn fLine}{\\sv 1}}{\\sp{\\sn lineWidth}{\\sv 19050}}"
               "{\\sp{\\sn lineDashing}{\\sv 2}}";
    }
    SECTION("Named thin border width")
    {
        fallbackHeight = "\\trrh1180";
        fallbackCell = "\\clbrdrt\\brdrs\\brdrw10\\clbrdrl\\brdrs\\brdrw10"
                       "\\clbrdrb\\brdrs\\brdrw10\\clbrdrr\\brdrs\\brdrw10";
        attr.GetBorder().SetStyle(wxTEXT_BOX_ATTR_BORDER_SOLID);
        attr.GetBorder().SetWidth(wxTEXT_BOX_ATTR_BORDER_THIN);
        line = "{\\sp{\\sn fLine}{\\sv 1}}{\\sp{\\sn lineWidth}{\\sv 6350}}";
    }
    SECTION("Explicitly disabled border does not acquire an Office outline")
    {
        attr.GetBorder().SetStyle(wxTEXT_BOX_ATTR_BORDER_NONE);
        attr.GetBorder().SetWidth(3, wxTEXT_ATTR_UNITS_PIXELS);
    }
    SECTION("Double uniform border")
    {
        fallbackHeight = "\\trrh1140";
        fallbackCell = "\\clbrdrt\\brdrdb\\brdrw30\\clbrdrl\\brdrdb\\brdrw30"
                       "\\clbrdrb\\brdrdb\\brdrw30\\clbrdrr\\brdrdb\\brdrw30";
        attr.GetBorder().SetStyle(wxTEXT_BOX_ATTR_BORDER_DOUBLE);
        attr.GetBorder().SetWidth(2, wxTEXT_ATTR_UNITS_PIXELS);
        line = "{\\sp{\\sn fLine}{\\sv 1}}{\\sp{\\sn lineWidth}{\\sv 19050}}"
               "{\\sp{\\sn lineStyle}{\\sv 1}}";
    }
    SECTION("Mixed absolute and percentage insets with bottom alignment")
    {
        attr.GetPadding().GetLeft().SetValue(20, wxTEXT_ATTR_UNITS_PIXELS);
        attr.GetPadding().GetTop().SetValue(25, wxTEXT_ATTR_UNITS_PERCENTAGE);
        attr.GetPadding().GetRight().SetValue(2, wxTEXT_ATTR_UNITS_PIXELS);
        attr.GetPadding().GetBottom().SetValue(10, wxTEXT_ATTR_UNITS_PERCENTAGE);
        attr.SetVerticalAlignment(wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT_BOTTOM);
        // Percentage padding is no space, as in wx.
        left = "190500"; top = "0"; right = "19050"; bottom = "0";
        anchor = "2";
    }
    SECTION("Centre alignment")
    {
        attr.SetVerticalAlignment(wxTEXT_BOX_ATTR_VERTICAL_ALIGNMENT_CENTRE);
        anchor = "1";
    }
    SECTION("Different edges use a cell inside the positioned shape")
    {
        fallbackCell = "\\clbrdrl\\brdrdash\\brdrw30\\brdrcf1";
        attr.GetBorder().GetLeft().SetStyle(wxTEXT_BOX_ATTR_BORDER_DASHED);
        attr.GetBorder().GetLeft().SetWidth(2, wxTEXT_ATTR_UNITS_PIXELS);
        attr.GetBorder().GetLeft().SetColour(wxColour(255, 0, 0));
        colours = "\\red255\\green0\\blue0;";
        contents = "{\\pard\\itap0\\plain\\fs1\\sl-1\\slmult0\\par}"
                   "{\\trowd\\trgaph0\\trleft0\\trrh1198"
                   "\\clbrdrl\\brdrdash\\brdrw30\\brdrcf1"
                   "\\clftsWidth3\\clwWidth1500\\cellx1500\n"
                   "{\\pard\\intbl\\itap1 {\\plain\\f0 box}\\cell }"
                   "\\pard\\intbl\\itap1\\plain\\fs1\\sb0\\sa0\\sl-1\\slmult0\\row}\n"
                   "{\\pard\\itap0\\plain\\fs1\\sl-1\\slmult0\\par}";
    }
    SECTION("Asymmetric borders and padding fit inside the fixed shape height")
    {
        fallbackHeight = "\\trrh960";
        fallbackCell = "\\clbrdrt\\brdrs\\brdrw45\\clbrdrl\\brdrs\\brdrw30\\clbrdrb\\brdrs\\brdrw15"
                       "\\clpadl60\\clpadfl3\\clpadb120\\clpadfb3";
        attr.GetBorder().GetLeft().SetStyle(wxTEXT_BOX_ATTR_BORDER_SOLID);
        attr.GetBorder().GetLeft().SetWidth(2, wxTEXT_ATTR_UNITS_PIXELS);
        attr.GetBorder().GetTop().SetStyle(wxTEXT_BOX_ATTR_BORDER_SOLID);
        attr.GetBorder().GetTop().SetWidth(3, wxTEXT_ATTR_UNITS_PIXELS);
        attr.GetBorder().GetBottom().SetStyle(wxTEXT_BOX_ATTR_BORDER_SOLID);
        attr.GetBorder().GetBottom().SetWidth(1, wxTEXT_ATTR_UNITS_PIXELS);
        attr.GetPadding().GetTop().SetValue(4, wxTEXT_ATTR_UNITS_PIXELS);
        attr.GetPadding().GetBottom().SetValue(8, wxTEXT_ATTR_UNITS_PIXELS);
        contents = "{\\pard\\itap0\\plain\\fs1\\sl-1\\slmult0\\par}"
                   "{\\trowd\\trgaph0\\trleft0\\trrh958"
                   "\\clbrdrt\\brdrs\\brdrw45\\clbrdrl\\brdrs\\brdrw30\\clbrdrb\\brdrs\\brdrw15"
                   "\\clpadl60\\clpadfl3\\clpadb120\\clpadfb3"
                   "\\clftsWidth3\\clwWidth1500\\cellx1500\n"
                   "{\\pard\\intbl\\itap1 {\\plain\\f0 box}\\cell }"
                   "\\pard\\intbl\\itap1\\plain\\fs1\\sb0\\sa0\\sl-1\\slmult0\\row}\n"
                   "{\\pard\\itap0\\plain\\fs1\\sl-1\\slmult0\\par}";
    }
    const wxString fallback = fallbackCell.empty()
        ? wxString("{\\pard\\itap0 {\\plain\\f0 box}\\par\n}")
        : "{\\trowd\\trgaph0\\trleft0" + fallbackHeight + fallbackCell +
          "\\clftsWidth3\\clwWidth1500\\cellx1500\n"
          "{\\pard\\intbl\\itap1 {\\plain\\f0 box}\\cell }\\row}\n";
    CheckRTF("{\\shp{\\*\\shpinst" + position + "\\shpbxcolumn\\shpbypara" + wrap +
             shape + "{\\sp{\\sn fPseudoInline}{\\sv " + pseudoInline + "}}" + fit +
             line + "{\\sp{\\sn dxTextLeft}{\\sv " + left + "}}{\\sp{\\sn dyTextTop}{\\sv " + top + "}}"
             "{\\sp{\\sn dxTextRight}{\\sv " + right + "}}{\\sp{\\sn dyTextBottom}{\\sv " + bottom + "}}"
             "{\\sp{\\sn anchorText}{\\sv " + anchor + "}}{\\sp{\\sn fFilled}{\\sv 0}}" +
             wrapDistance + ShapeText(contents, fallback) + ShapeAnchor(), colours);
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFTextBoxInTableCell",
                 "[richtextctrl][rtf]")
{
    // The shape result of a text box in a cell is written separately from
    // its story: the story starts at table depth zero, the fallback keeps
    // the depth of the containing cell.
    auto table = CreateTestTable(1, 1);
    Anchor(buffer, table);
    auto cell = table->GetCell(0, 0);
    auto box = CreateTestBox();
    box->AddParagraph("box");
    auto& attr = box->GetAttributes().GetTextBoxAttr();
    attr.GetWidth().SetValue(100, wxTEXT_ATTR_UNITS_PIXELS);
    attr.GetHeight().SetValue(80, wxTEXT_ATTR_UNITS_PIXELS);
    wxString line = "{\\sp{\\sn fLine}{\\sv 0}}";
    wxString story = "{\\pard\\itap0 {\\plain\\f0 box}\\par\n}";
    wxString fallback = "{\\pard\\intbl\\itap1 {\\plain\\f0 box}\\par\n}";
    wxString colours;
    bool inParagraph = false, inNestedTable = false;
    // A bordered box without shape support is a bordered nested cell.
    // Its row height excludes the top and bottom borders, which Word adds.
    const auto nestedCell = [](const wxString& borders, const wxString& height)
    {
        return "{\\intbl\\itap2\n{\\pard\\intbl\\itap2 {\\plain\\f0 box}\\nestcell }"
               "{\\*\\nesttableprops\\trowd\\trgaph0\\trleft0" + height + borders +
               "\\clftsWidth3\\clwWidth1500\\cellx1500\\nestrow}}\n";
    };
    SECTION("Borderless box") { }
    SECTION("Uniform border uses the shape outline")
    {
        attr.GetBorder().SetStyle(wxTEXT_BOX_ATTR_BORDER_SOLID);
        attr.GetBorder().SetWidth(2, wxTEXT_ATTR_UNITS_PIXELS);
        line = "{\\sp{\\sn fLine}{\\sv 1}}{\\sp{\\sn lineWidth}{\\sv 19050}}";
        fallback = nestedCell("\\clbrdrt\\brdrs\\brdrw30\\clbrdrl\\brdrs\\brdrw30"
                              "\\clbrdrb\\brdrs\\brdrw30\\clbrdrr\\brdrs\\brdrw30",
                              "\\trrh1140");
    }
    SECTION("Mixed borders use a cell inside the shape story")
    {
        attr.GetBorder().GetLeft().SetStyle(wxTEXT_BOX_ATTR_BORDER_DASHED);
        attr.GetBorder().GetLeft().SetWidth(2, wxTEXT_ATTR_UNITS_PIXELS);
        attr.GetBorder().GetLeft().SetColour(wxColour(255, 0, 0));
        colours = "\\red255\\green0\\blue0;";
        story = "{\\pard\\itap0\\plain\\fs1\\sl-1\\slmult0\\par}"
                "{\\trowd\\trgaph0\\trleft0\\trrh1198"
                "\\clbrdrl\\brdrdash\\brdrw30\\brdrcf1"
                "\\clftsWidth3\\clwWidth1500\\cellx1500\n"
                "{\\pard\\intbl\\itap1 {\\plain\\f0 box}\\cell }"
                "\\pard\\intbl\\itap1\\plain\\fs1\\sb0\\sa0\\sl-1\\slmult0\\row}\n"
                "{\\pard\\itap0\\plain\\fs1\\sl-1\\slmult0\\par}";
        fallback = nestedCell("\\clbrdrl\\brdrdash\\brdrw30\\brdrcf1", "\\trrh1200");
    }
    SECTION("Box inside a cell paragraph continues the paragraph")
    {
        inParagraph = true;
        fallback = "\\pard\\intbl\\itap1 {\\plain\\f0 box}\\pard\\intbl\\itap1 ";
        SECTION("Borderless") { }
        SECTION("A bordered inline box can't use a table")
        {
            attr.GetBorder().SetStyle(wxTEXT_BOX_ATTR_BORDER_SOLID);
            attr.GetBorder().SetWidth(2, wxTEXT_ATTR_UNITS_PIXELS);
            line = "{\\sp{\\sn fLine}{\\sv 1}}{\\sp{\\sn lineWidth}{\\sv 19050}}";
        }
    }
    SECTION("Box in a nested table keeps the nesting depth")
    {
        inNestedTable = true;
        fallback = "{\\pard\\intbl\\itap2 {\\plain\\f0 box}\\par\n}";
    }

    if ( inParagraph )
    {
        auto para = wxDynamicCast(cell->GetChildren().GetFirst()->GetData(), wxRichTextParagraph);
        para->AppendChild(box);
        para->AppendChild(new wxRichTextPlainText("after", para));
    }
    else if ( inNestedTable )
    {
        auto nested = CreateTestTable(1, 1);
        cell->DeleteChildren();
        Anchor(*cell, nested);
        nested->GetCell(0, 0)->DeleteChildren();
        Anchor(*nested->GetCell(0, 0), box);
    }
    else
    {
        cell->DeleteChildren();
        Anchor(*cell, box);
    }

    const wxString shape =
        "{\\shp{\\*\\shpinst\\shpleft0\\shptop0\\shpright1500\\shpbottom1200"
        "\\shpbxcolumn\\shpbypara\\shpwr1{\\sp{\\sn shapeType}{\\sv 202}}"
        "{\\sp{\\sn fPseudoInline}{\\sv 0}}{\\sp{\\sn fLayoutInCell}{\\sv 1}}" + line +
        "{\\sp{\\sn dxTextLeft}{\\sv 0}}{\\sp{\\sn dyTextTop}{\\sv 0}}"
        "{\\sp{\\sn dxTextRight}{\\sv 0}}{\\sp{\\sn dyTextBottom}{\\sv 0}}"
        "{\\sp{\\sn anchorText}{\\sv 0}}{\\sp{\\sn fFilled}{\\sv 0}}" +
        ShapeText(story, fallback);
    wxString contents;
    if ( inParagraph )
        contents = "{\\pard\\intbl\\itap1 {\\plain\\f0 }" + shape + "{\\plain\\f0 after}\\cell }";
    else if ( inNestedTable )
        contents = "{\\intbl\\itap2\n" + shape + ShapeAnchor("\\nestcell", 2) +
                   "{\\*\\nesttableprops\\trowd\\trgaph0\\trleft0"
                   "\\clftsWidth3\\clwWidth9360\\cellx9360\\nestrow}}\n"
                   "{\\pard\\intbl\\itap1 \\cell}";
    else
        contents = shape + ShapeAnchor("\\cell", 1);
    CheckRTF("{\\trowd\\trgaph0\\trleft0\\clftsWidth3\\clwWidth9360\\cellx9360\n" +
             contents + "\\row}\n", colours);
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFNoShapeFallbacks",
                 "[richtextctrl][rtf]")
{
    auto box = CreateTestBox();
    box->AddParagraph("box");
    box->GetAttributes().GetTextBoxAttr().GetBorder().SetStyle(wxTEXT_BOX_ATTR_BORDER_SOLID);
    box->GetAttributes().GetTextBoxAttr().GetBorder().SetWidth(2, wxTEXT_ATTR_UNITS_PIXELS);
    Anchor(buffer, box);
    buffer.SetHandlerFlags(wxRICHTEXT_HANDLER_RTF_NO_SHAPE_FALLBACKS);
    // The shape group ends directly after the story.
    CheckRTF("{\\shp{\\*\\shpinst\\shpleft0\\shptop0\\shpright9360\\shpbottom240"
             "\\shpbxcolumn\\shpbypara\\shpwr1{\\sp{\\sn shapeType}{\\sv 202}}"
             "{\\sp{\\sn fPseudoInline}{\\sv 0}}{\\sp{\\sn fFitShapeToText}{\\sv 1}}"
             "{\\sp{\\sn fLine}{\\sv 1}}{\\sp{\\sn lineWidth}{\\sv 19050}}"
             "{\\sp{\\sn dxTextLeft}{\\sv 0}}{\\sp{\\sn dyTextTop}{\\sv 0}}"
             "{\\sp{\\sn dxTextRight}{\\sv 0}}{\\sp{\\sn dyTextBottom}{\\sv 0}}"
             "{\\sp{\\sn anchorText}{\\sv 0}}{\\sp{\\sn fFilled}{\\sv 0}}"
             "{\\shptxt {\\pard\\itap0 {\\plain\\f0 box}\\par\n}}}}" + ShapeAnchor());

#if wxUSE_STREAMS
    // The file handler gets the same flags from the buffer.
    wxString rtf;
    REQUIRE(buffer.GetAsRTF(rtf));
    wxMemoryOutputStream stream;
    REQUIRE(buffer.SaveFile(stream, wxRICHTEXT_TYPE_RTF));
    const auto bytes = rtf.utf8_str();
    REQUIRE(stream.GetLength() == static_cast<wxFileOffset>(bytes.length()));
    wxCharBuffer saved(bytes.length());
    REQUIRE(stream.CopyTo(saved.data(), bytes.length()) == bytes.length());
    CHECK(memcmp(saved.data(), bytes.data(), bytes.length()) == 0);
#endif
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFCellPaddingSides",
                 "[richtextctrl][rtf]")
{
    auto table = CreateTestTable(1, 1);
    Anchor(buffer, table);
    auto& p = table->GetCell(0, 0)->GetAttributes().GetTextBoxAttr().GetPadding();
    wxString padding;
    SECTION("Left uses Word's clpadt mapping")
    {
        p.GetLeft().SetValue(20, wxTEXT_ATTR_UNITS_PIXELS);
        padding = "\\clpadt300\\clpadft3";
    }
    SECTION("Top uses Word's clpadl mapping")
    {
        p.GetTop().SetValue(10, wxTEXT_ATTR_UNITS_PIXELS);
        padding = "\\clpadl150\\clpadfl3";
    }
    SECTION("Right")
    {
        p.GetRight().SetValue(30, wxTEXT_ATTR_UNITS_PIXELS);
        padding = "\\clpadr450\\clpadfr3";
    }
    SECTION("Bottom")
    {
        p.GetBottom().SetValue(40, wxTEXT_ATTR_UNITS_PIXELS);
        padding = "\\clpadb600\\clpadfb3";
    }
    SECTION("All four sides")
    {
        p.GetLeft().SetValue(20, wxTEXT_ATTR_UNITS_PIXELS);
        p.GetTop().SetValue(10, wxTEXT_ATTR_UNITS_PIXELS);
        p.GetRight().SetValue(30, wxTEXT_ATTR_UNITS_PIXELS);
        p.GetBottom().SetValue(40, wxTEXT_ATTR_UNITS_PIXELS);
        padding = "\\clpadt300\\clpadft3\\clpadl150\\clpadfl3"
                  "\\clpadr450\\clpadfr3\\clpadb600\\clpadfb3";
    }
    CheckRTF("{\\trowd\\trgaph0\\trleft0" + padding +
             "\\clftsWidth3\\clwWidth9360\\cellx9360\n"
             "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }\\row}\n");
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFCellPercentagePadding",
                 "[richtextctrl][rtf]")
{
    auto table = CreateTestTable(1, 2);
    Anchor(buffer, table);
    for ( int col = 0; col < 2; ++col )
    {
        auto& box = table->GetCell(0, col)->GetAttributes().GetTextBoxAttr();
        box.GetWidth().SetValue(col == 0 ? 100 : 300, wxTEXT_ATTR_UNITS_PIXELS);
        box.GetHeight().SetValue(80, wxTEXT_ATTR_UNITS_PIXELS);
        box.GetPadding().GetLeft().SetValue(10, wxTEXT_ATTR_UNITS_PERCENTAGE);
        box.GetPadding().GetRight().SetValue(10, wxTEXT_ATTR_UNITS_PERCENTAGE);
        box.GetPadding().GetTop().SetValue(25, wxTEXT_ATTR_UNITS_PERCENTAGE);
        box.GetPadding().GetBottom().SetValue(10, wxTEXT_ATTR_UNITS_PIXELS);
    }
    // wx doesn't support percentage padding (it lays it out as about one
    // pixel), so it is no space: only the absolute bottom padding remains,
    // which Word adds to the row height: 1200 - 150.
    const wxString padding = "\\clpadb150\\clpadfb3";
    SECTION("Separate cells")
    {
        CheckRTF("{\\trowd\\trgaph0\\trleft0\\trrh1050" + padding +
                 "\\clftsWidth3\\clwWidth1500\\cellx1500" + padding +
                 "\\clftsWidth3\\clwWidth4500\\cellx6000\n"
                 "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }"
                 "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }\\row}\n");
    }
    SECTION("Merged cell")
    {
        table->GetAttributes().GetTextBoxAttr().GetWidth().SetValue(400, wxTEXT_ATTR_UNITS_PIXELS);
        table->GetCell(0, 0)->SetColSpan(2);
        CheckRTF("{\\trowd\\trgaph0\\trleft0\\trrh1050\\clmgf" + padding +
                 "\\clftsWidth3\\clwWidth3000\\cellx3000\\clmrg" + padding +
                 "\\clftsWidth3\\clwWidth3000\\cellx6000\n"
                 "{\\pard\\intbl\\itap1 {\\plain\\f0 }\\cell }"
                 "{\\pard\\intbl\\itap1 \\cell}\\row}\n");
    }
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFEmptyField",
                 "[richtextctrl][rtf]")
{
    const wxString type = "rtf-empty-field-test";
    auto fieldType = new wxRichTextFieldTypeStandard(type, wxString());
    fieldType->SetFont(wxNullFont);
    fieldType->SetTextColour(wxColour(255, 0, 0));
    fieldType->SetBorderColour(wxColour(0, 0, 0));
    fieldType->SetBackgroundColour(wxColour(255, 255, 255));
    wxRichTextBuffer::AddFieldType(fieldType);
    struct Cleanup
    {
        ~Cleanup() { wxRichTextBuffer::RemoveFieldType("rtf-empty-field-test"); }
    } cleanup;
    buffer.AddParagraph("");
    auto para = wxDynamicCast(buffer.GetChildren().GetFirst()->GetData(), wxRichTextParagraph);
    auto field = new wxRichTextField(type);
    field->GetAttributes().SetFontFaceName("Arial");
    para->AppendChild(field);
    CheckRTF("{\\pard\\itap0 {\\plain\\f0 }"
             "{{\\*\\wxfield rtf-empty-field-test}{\\plain\\f0\\cf1\\chshdng0\\chcbpat2 ??}}\\par\n}",
             // The border colour is not exported, so it's not in the table.
             "\\red255\\green0\\blue0;\\red255\\green255\\blue255;");
}

namespace
{

// An application-defined object, unknown to the RTF exporter.
class RTFCustomObject : public wxRichTextObject
{
public:
    RTFCustomObject(const wxString& rtf, const wxString& text)
        : m_rtf(rtf), m_text(text)
    {
    }

    virtual bool Draw(wxDC&, wxRichTextDrawingContext&, const wxRichTextRange&,
                      const wxRichTextSelection&, const wxRect&, int, int) override
    {
        return true;
    }

    virtual bool Layout(wxReadOnlyDC&, wxRichTextDrawingContext&, const wxRect&,
                        const wxRect&, int) override
    {
        return true;
    }

    virtual bool GetRangeSize(const wxRichTextRange&, wxSize&, int&, wxReadOnlyDC&,
                              wxRichTextDrawingContext&, int, const wxPoint&,
                              const wxSize&, wxArrayInt*) const override
    {
        return true;
    }

    virtual wxString GetTextForRange(const wxRichTextRange&) const override
    {
        return m_text;
    }

    virtual bool ExportRTF(wxString& rtf) const override
    {
        if ( m_rtf.empty() )
            return false;

        rtf = m_rtf;
        return true;
    }

private:
    const wxString m_rtf;
    const wxString m_text;
};

} // anonymous namespace

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFCustomObjects",
                 "[richtextctrl][rtf]")
{
    auto para = AddText("before");
    SECTION("Object providing its own RTF")
    {
        para->AppendChild(new RTFCustomObject("{\\b custom}", "text"));
        CheckRTF("{\\pard\\itap0 {\\plain\\f0 before}{\\b custom}\\par\n}");
    }
    SECTION("Object without RTF falls back to its text")
    {
        para->AppendChild(new RTFCustomObject(wxString(), "text"));
        CheckRTF("{\\pard\\itap0 {\\plain\\f0 before}{\\plain\\f0 text}\\par\n}");
    }
    SECTION("Object without any representation fails the export")
    {
        para->AppendChild(new RTFCustomObject(wxString(), wxString()));
        wxString rtf = "unchanged";
        CHECK_FALSE(buffer.GetAsRTF(rtf));
        CHECK(rtf == "unchanged");
    }
}

#if wxUSE_LIBPNG && wxUSE_STREAMS
TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFImageBeforeLayout",
                 "[richtextctrl][rtf]")
{
    buffer.AddParagraph("");
    auto para = wxDynamicCast(buffer.GetChildren().GetFirst()->GetData(), wxRichTextParagraph);
    auto picture = new wxRichTextImage(TestPNG());
    para->AppendChild(picture);
    const wxString data = TestPNGData();

    wxString dimensions = "\\picwgoal30\\pichgoal60", alignment, decoration, colours;
    SECTION("Intrinsic size") { dimensions = "\\picwgoal30\\pichgoal60"; }
    SECTION("Size cached by the screen layout is not used")
    {
        // E.g. stale after the image block was replaced.
        picture->SetOriginalImageSize(wxSize(500, 300));
    }
    SECTION("Inline picture keeps paragraph centering")
    {
        para->GetAttributes().SetAlignment(wxTEXT_ALIGNMENT_CENTRE);
        alignment = "\\qc";
        dimensions = "\\picwgoal30\\pichgoal60";
    }
    SECTION("Explicit width preserves aspect ratio")
    {
        picture->GetAttributes().GetTextBoxAttr().GetWidth().SetValue(20, wxTEXT_ATTR_UNITS_PIXELS);
        dimensions = "\\picwgoal300\\pichgoal600";
    }
    SECTION("Explicit height preserves aspect ratio")
    {
        picture->GetAttributes().GetTextBoxAttr().GetHeight().SetValue(20, wxTEXT_ATTR_UNITS_PIXELS);
        dimensions = "\\picwgoal150\\pichgoal300";
    }
    SECTION("Maximum width alone preserves aspect ratio")
    {
        picture->GetAttributes().GetTextBoxAttr().GetMaxSize().GetWidth().SetValue(1, wxTEXT_ATTR_UNITS_PIXELS);
        dimensions = "\\picwgoal15\\pichgoal30";
    }
    SECTION("Maximum height alone preserves aspect ratio")
    {
        picture->GetAttributes().GetTextBoxAttr().GetMaxSize().GetHeight().SetValue(2, wxTEXT_ATTR_UNITS_PIXELS);
        dimensions = "\\picwgoal15\\pichgoal30";
    }
    SECTION("Maximum height is not overwritten by explicit width")
    {
        auto& box = picture->GetAttributes().GetTextBoxAttr();
        box.GetWidth().SetValue(20, wxTEXT_ATTR_UNITS_PIXELS);
        box.GetMaxSize().GetHeight().SetValue(10, wxTEXT_ATTR_UNITS_PIXELS);
        dimensions = "\\picwgoal75\\pichgoal150";
    }
    SECTION("Maximum width is not overwritten by explicit height")
    {
        auto& box = picture->GetAttributes().GetTextBoxAttr();
        box.GetHeight().SetValue(20, wxTEXT_ATTR_UNITS_PIXELS);
        box.GetMaxSize().GetWidth().SetValue(5, wxTEXT_ATTR_UNITS_PIXELS);
        dimensions = "\\picwgoal75\\pichgoal150";
    }
    SECTION("Both explicit dimensions follow native height precedence")
    {
        auto& box = picture->GetAttributes().GetTextBoxAttr();
        box.GetWidth().SetValue(20, wxTEXT_ATTR_UNITS_PIXELS);
        box.GetHeight().SetValue(20, wxTEXT_ATTR_UNITS_PIXELS);
        dimensions = "\\picwgoal150\\pichgoal300";
    }
    SECTION("Minimum width scales both axes")
    {
        picture->GetAttributes().GetTextBoxAttr().GetMinSize().GetWidth().SetValue(10, wxTEXT_ATTR_UNITS_PIXELS);
        dimensions = "\\picwgoal150\\pichgoal300";
    }
    SECTION("Percentage width is relative to the page's text width")
    {
        picture->GetAttributes().GetTextBoxAttr().GetWidth().SetValue(25, wxTEXT_ATTR_UNITS_PERCENTAGE);
        dimensions = "\\picwgoal2340\\pichgoal4680";
    }
    SECTION("The buffer's own width is ignored")
    {
        // Like wx, which lays out the buffer in the window or the page.
        buffer.GetAttributes().GetTextBoxAttr().GetWidth().SetValue(400, wxTEXT_ATTR_UNITS_PIXELS);
        picture->GetAttributes().GetTextBoxAttr().GetWidth().SetValue(25, wxTEXT_ATTR_UNITS_PERCENTAGE);
        dimensions = "\\picwgoal2340\\pichgoal4680";
    }
    SECTION("Maximum takes precedence over an incompatible minimum")
    {
        auto& box = picture->GetAttributes().GetTextBoxAttr();
        box.GetMinSize().GetWidth().SetValue(10, wxTEXT_ATTR_UNITS_PIXELS);
        box.GetMaxSize().GetHeight().SetValue(10, wxTEXT_ATTR_UNITS_PIXELS);
        dimensions = "\\picwgoal75\\pichgoal150";
    }
    SECTION("Containing width caps oversized images")
    {
        // 1000 pixels are wider than the page's 9360 twips.
        picture->GetAttributes().GetTextBoxAttr().GetWidth().SetValue(1000, wxTEXT_ATTR_UNITS_PIXELS);
        dimensions = "\\picwgoal9360\\pichgoal18720";
    }
    SECTION("Percentage maximum resolves against the container")
    {
        auto& box = picture->GetAttributes().GetTextBoxAttr();
        box.GetWidth().SetValue(80, wxTEXT_ATTR_UNITS_PIXELS);
        box.GetMaxSize().GetWidth().SetValue(10, wxTEXT_ATTR_UNITS_PERCENTAGE);
        dimensions = "\\picwgoal936\\pichgoal1872";
    }
    SECTION("Left image border")
    {
        auto& border = picture->GetAttributes().GetTextBoxAttr().GetBorder().GetLeft();
        border.SetWidth(2, wxTEXT_ATTR_UNITS_PIXELS);
        border.SetStyle(wxTEXT_BOX_ATTR_BORDER_DASHED);
        border.SetColour(wxColour(12, 34, 56));
        decoration = "\\brdrl\\brdrdash\\brdrw30\\brdrcf1";
        colours = "\\red12\\green34\\blue56;";
    }
    SECTION("Uniform inline outline supports Word and Writer")
    {
        auto& border = picture->GetAttributes().GetTextBoxAttr().GetBorder();
        border.SetWidth(2, wxTEXT_ATTR_UNITS_PIXELS);
        border.SetColour(wxColour(0, 0, 255));
        wxString style, property;
        SECTION("Solid") { border.SetStyle(wxTEXT_BOX_ATTR_BORDER_SOLID); style = "\\brdrs"; }
        SECTION("Dashes")
        {
            border.SetStyle(wxTEXT_BOX_ATTR_BORDER_DASHED); style = "\\brdrdash";
            property = "{\\sp{\\sn lineDashing}{\\sv 1}}";
        }
        SECTION("Dots")
        {
            border.SetStyle(wxTEXT_BOX_ATTR_BORDER_DOTTED); style = "\\brdrdot";
            property = "{\\sp{\\sn lineDashing}{\\sv 2}}";
        }
        SECTION("Double")
        {
            border.SetStyle(wxTEXT_BOX_ATTR_BORDER_DOUBLE); style = "\\brdrdb";
            property = "{\\sp{\\sn lineStyle}{\\sv 1}}";
        }
        colours = "\\red0\\green0\\blue255;";
        for ( const char* edge : {"\\brdrt", "\\brdrl", "\\brdrb", "\\brdrr"} )
            decoration += wxString(edge) + style + "\\brdrw30\\brdrcf1";
        decoration += "{\\*\\picprop{\\sp{\\sn fLine}{\\sv 1}}{\\sp{\\sn lineWidth}{\\sv 19050}}"
                      "{\\sp{\\sn lineColor}{\\sv 16711680}}" + property + "}";
    }
    SECTION("Different image borders on all four edges")
    {
        auto& b = picture->GetAttributes().GetTextBoxAttr().GetBorder();
        b.SetWidth(2, wxTEXT_ATTR_UNITS_PIXELS);
        b.SetStyle(wxTEXT_BOX_ATTR_BORDER_SOLID);
        b.GetTop().SetStyle(wxTEXT_BOX_ATTR_BORDER_DOTTED);
        b.GetLeft().SetStyle(wxTEXT_BOX_ATTR_BORDER_DASHED);
        b.GetBottom().SetStyle(wxTEXT_BOX_ATTR_BORDER_DOUBLE);
        decoration = "\\brdrt\\brdrdot\\brdrw30\\brdrl\\brdrdash\\brdrw30"
                     "\\brdrb\\brdrdb\\brdrw30\\brdrr\\brdrs\\brdrw30";
    }
    SECTION("Left inline margin")
    {
        picture->GetAttributes().GetTextBoxAttr().GetMargins().GetLeft().SetValue(3, wxTEXT_ATTR_UNITS_PIXELS);
        decoration = "\\piccropl-45";
    }
    SECTION("Top inline margin")
    {
        picture->GetAttributes().GetTextBoxAttr().GetMargins().GetTop().SetValue(4, wxTEXT_ATTR_UNITS_PIXELS);
        decoration = "\\piccropt-60";
    }
    SECTION("Right inline margin")
    {
        picture->GetAttributes().GetTextBoxAttr().GetMargins().GetRight().SetValue(5, wxTEXT_ATTR_UNITS_PIXELS);
        decoration = "\\piccropr-75";
    }
    SECTION("Bottom inline margin")
    {
        picture->GetAttributes().GetTextBoxAttr().GetMargins().GetBottom().SetValue(6, wxTEXT_ATTR_UNITS_PIXELS);
        decoration = "\\piccropb-90";
    }
    SECTION("Combined inline margins and border")
    {
        auto& box = picture->GetAttributes().GetTextBoxAttr();
        box.GetBorder().GetLeft().SetWidth(1, wxTEXT_ATTR_UNITS_PIXELS);
        box.GetBorder().GetLeft().SetStyle(wxTEXT_BOX_ATTR_BORDER_SOLID);
        box.GetMargins().GetLeft().SetValue(3, wxTEXT_ATTR_UNITS_PIXELS);
        box.GetMargins().GetTop().SetValue(4, wxTEXT_ATTR_UNITS_PIXELS);
        box.GetMargins().GetRight().SetValue(5, wxTEXT_ATTR_UNITS_PIXELS);
        box.GetMargins().GetBottom().SetValue(6, wxTEXT_ATTR_UNITS_PIXELS);
        decoration = "\\brdrl\\brdrs\\brdrw15\\piccropl-45\\piccropt-60\\piccropr-75\\piccropb-90";
    }
    SECTION("Inline padding on each side")
    {
        auto& padding = picture->GetAttributes().GetTextBoxAttr().GetPadding();
        SECTION("Left")
        {
            padding.GetLeft().SetValue(3, wxTEXT_ATTR_UNITS_PIXELS);
            decoration = "\\piccropl-45";
        }
        SECTION("Top")
        {
            padding.GetTop().SetValue(4, wxTEXT_ATTR_UNITS_PIXELS);
            decoration = "\\piccropt-60";
        }
        SECTION("Right")
        {
            padding.GetRight().SetValue(5, wxTEXT_ATTR_UNITS_PIXELS);
            decoration = "\\piccropr-75";
        }
        SECTION("Bottom")
        {
            padding.GetBottom().SetValue(6, wxTEXT_ATTR_UNITS_PIXELS);
            decoration = "\\piccropb-90";
        }
    }
    SECTION("Combined inline padding inside a border")
    {
        auto& box = picture->GetAttributes().GetTextBoxAttr();
        box.GetPadding().GetLeft().SetValue(3, wxTEXT_ATTR_UNITS_PIXELS);
        box.GetPadding().GetTop().SetValue(4, wxTEXT_ATTR_UNITS_PIXELS);
        box.GetPadding().GetRight().SetValue(5, wxTEXT_ATTR_UNITS_PIXELS);
        box.GetPadding().GetBottom().SetValue(6, wxTEXT_ATTR_UNITS_PIXELS);
        box.GetBorder().SetWidth(1, wxTEXT_ATTR_UNITS_PIXELS);
        box.GetBorder().SetStyle(wxTEXT_BOX_ATTR_BORDER_SOLID);
        decoration = "\\brdrt\\brdrs\\brdrw15\\brdrl\\brdrs\\brdrw15"
                     "\\brdrb\\brdrs\\brdrw15\\brdrr\\brdrs\\brdrw15"
                     "{\\*\\picprop{\\sp{\\sn fLine}{\\sv 1}}{\\sp{\\sn lineWidth}{\\sv 9525}}}"
                     "\\piccropl-45\\piccropt-60\\piccropr-75\\piccropb-90";
    }
    SECTION("Negative inline margins do not crop pixels or cancel padding")
    {
        auto& box = picture->GetAttributes().GetTextBoxAttr();
        box.GetMargins().GetLeft().SetValue(-20, wxTEXT_ATTR_UNITS_PIXELS);
        box.GetMargins().GetTop().SetValue(-20, wxTEXT_ATTR_UNITS_PIXELS);
        box.GetMargins().GetRight().SetValue(-20, wxTEXT_ATTR_UNITS_PIXELS);
        box.GetMargins().GetBottom().SetValue(-20, wxTEXT_ATTR_UNITS_PIXELS);
        box.GetPadding().GetLeft().SetValue(3, wxTEXT_ATTR_UNITS_PIXELS);
        decoration = "\\piccropl-45";
    }
    SECTION("Image fitting reserves decorations")
    {
        // 1000 pixels are wider than the page's 9360 twips.
        auto& box = picture->GetAttributes().GetTextBoxAttr();
        box.GetWidth().SetValue(1000, wxTEXT_ATTR_UNITS_PIXELS);
        SECTION("Margins")
        {
            box.GetMargins().GetLeft().SetValue(10, wxTEXT_ATTR_UNITS_PIXELS);
            box.GetMargins().GetRight().SetValue(20, wxTEXT_ATTR_UNITS_PIXELS);
            dimensions = "\\picwgoal8910\\pichgoal17820";
            decoration = "\\piccropl-150\\piccropr-300";
        }
        SECTION("Borders")
        {
            box.GetBorder().SetWidth(2, wxTEXT_ATTR_UNITS_PIXELS);
            box.GetBorder().SetStyle(wxTEXT_BOX_ATTR_BORDER_SOLID);
            dimensions = "\\picwgoal9300\\pichgoal18600";
            decoration = "\\brdrt\\brdrs\\brdrw30\\brdrl\\brdrs\\brdrw30"
                         "\\brdrb\\brdrs\\brdrw30\\brdrr\\brdrs\\brdrw30"
                         "{\\*\\picprop{\\sp{\\sn fLine}{\\sv 1}}{\\sp{\\sn lineWidth}{\\sv 19050}}}";
        }
        SECTION("Percentage padding is no space, as in wx")
        {
            box.GetPadding().GetLeft().SetValue(10, wxTEXT_ATTR_UNITS_PERCENTAGE);
            box.GetPadding().GetRight().SetValue(20, wxTEXT_ATTR_UNITS_PERCENTAGE);
            dimensions = "\\picwgoal9360\\pichgoal18720";
        }
        SECTION("Combined")
        {
            box.GetMargins().GetLeft().SetValue(10, wxTEXT_ATTR_UNITS_PIXELS);
            box.GetMargins().GetRight().SetValue(20, wxTEXT_ATTR_UNITS_PIXELS);
            box.GetPadding().GetLeft().SetValue(3, wxTEXT_ATTR_UNITS_PIXELS);
            box.GetPadding().GetRight().SetValue(5, wxTEXT_ATTR_UNITS_PIXELS);
            box.GetBorder().SetWidth(1, wxTEXT_ATTR_UNITS_PIXELS);
            box.GetBorder().SetStyle(wxTEXT_BOX_ATTR_BORDER_SOLID);
            dimensions = "\\picwgoal8760\\pichgoal17520";
            decoration = "\\brdrt\\brdrs\\brdrw15\\brdrl\\brdrs\\brdrw15"
                         "\\brdrb\\brdrs\\brdrw15\\brdrr\\brdrs\\brdrw15"
                         "{\\*\\picprop{\\sp{\\sn fLine}{\\sv 1}}{\\sp{\\sn lineWidth}{\\sv 9525}}}"
                         "\\piccropl-195\\piccropr-375";
        }
        SECTION("Oversubscribed spacing still produces positive dimensions")
        {
            box.GetMargins().GetLeft().SetValue(700, wxTEXT_ATTR_UNITS_PIXELS);
            dimensions = "\\picwgoal1\\pichgoal2";
            decoration = "\\piccropl-10500";
        }
    }
    const wxSize cachedSize = picture->GetOriginalImageSize();
    CheckRTF("{\\pard\\itap0" + alignment + " {\\plain\\f0 }{\\pict\\pngblip\\picw2\\pich4" +
             dimensions + decoration + data + "}\\par\n}", colours);
    CHECK(picture->GetOriginalImageSize() == cachedSize);
}

#if wxUSE_LIBJPEG
TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFJPEGImageSize",
                 "[richtextctrl][rtf]")
{
    // The size is read from the JPEG frame header, not from a layout cache.
    if ( !wxImage::FindHandler(wxBITMAP_TYPE_JPEG) )
        wxImage::AddHandler(new wxJPEGHandler);
    wxImage image(3, 5);
    image.SetRGB(wxRect(0, 0, 3, 5), 0, 128, 255);
    wxRichTextImageBlock block;
    REQUIRE(block.MakeImageBlockDefaultQuality(image, wxBITMAP_TYPE_JPEG));
    buffer.AddParagraph("");
    auto para = wxDynamicCast(buffer.GetChildren().GetFirst()->GetData(), wxRichTextParagraph);
    auto picture = new wxRichTextImage(block);
    picture->SetOriginalImageSize(wxSize(500, 300));
    para->AppendChild(picture);

    CheckRTF("{\\pard\\itap0 {\\plain\\f0 }{\\pict\\jpegblip\\picw3\\pich5"
             "\\picwgoal45\\pichgoal75" + PictureData(block) + "}\\par\n}");
}
#endif // wxUSE_LIBJPEG

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFFieldResolution",
                 "[richtextctrl][rtf]")
{
    // Field appearance must not depend on the resolution of the screen.
    const wxString type = "rtf-field-resolution-test";
    auto fieldType = new wxRichTextFieldTypeStandard(type, "label");
    fieldType->SetTextColour(wxNullColour);
    fieldType->SetBackgroundColour(wxNullColour);
    wxRichTextBuffer::AddFieldType(fieldType);
    struct Cleanup
    {
        ~Cleanup() { wxRichTextBuffer::RemoveFieldType("rtf-field-resolution-test"); }
    } cleanup;
    buffer.AddParagraph("");
    auto para = wxDynamicCast(buffer.GetChildren().GetFirst()->GetData(), wxRichTextParagraph);
    auto field = new wxRichTextField(type);
    field->GetAttributes().SetFontFaceName("Arial");
    para->AppendChild(field);

    SECTION("Pixel font size is not converted using the screen resolution")
    {
        const wxFont font(wxFontInfo(wxSize(0, 20)).FaceName("Arial"));
        // Only some ports, e.g. wxMSW, keep the pixel size of a font. Others
        // convert it to points using the screen resolution.
        if ( !font.IsUsingSizeInPixels() )
        {
            WARN("Skipped: fonts don't keep their pixel size in this port.");
            return;
        }
        fieldType->SetFont(font);
        CheckRTF("{\\pard\\itap0 {\\plain\\f0 }"
                 "{{\\*\\wxfield rtf-field-resolution-test}{\\plain\\f0\\fs30 label}}\\par\n}");
    }
#if wxUSE_LIBPNG
    SECTION("Bitmap scale factor gives the size in DIPs")
    {
        if ( !wxImage::FindHandler(wxBITMAP_TYPE_PNG) )
            wxImage::AddHandler(new wxPNGHandler);
        wxImage image(4, 6);
        image.SetRGB(wxRect(0, 0, 4, 6), 255, 0, 0);
        wxBitmap bitmap(image);
        double scale = 1.0;
        wxString goal = "\\picwgoal60\\pichgoal90";
        SECTION("Scale factor 1") { }
        SECTION("Scale factor 2")
        {
            scale = 2.0;
            goal = "\\picwgoal30\\pichgoal45";
        }
        bitmap.SetScaleFactor(scale);
        fieldType->SetBitmap(bitmap);

        // Encode the pixels of the bitmap, as the export does: the conversion
        // may add an alpha channel on some platforms.
        wxImage converted = bitmap.ConvertToImage();
        wxRichTextImageBlock png;
        REQUIRE(png.MakeImageBlockDefaultQuality(converted, wxBITMAP_TYPE_PNG));
        CheckRTF("{\\pard\\itap0 {\\plain\\f0 }"
                 "{{\\*\\wxfield rtf-field-resolution-test}"
                 "{\\pict\\pngblip\\picw4\\pich6" + goal + PictureData(png) + "}}\\par\n}");
    }
#endif // wxUSE_LIBPNG
}

TEST_CASE_METHOD(RTFExportTestCase, "wxRichTextBuffer::RTFFloatingImages",
                 "[richtextctrl][rtf]")
{
    const wxRichTextImageBlock block = TestPNG();
    auto picture = new wxRichTextImage(block);
    auto& attr = picture->GetAttributes().GetTextBoxAttr();
    attr.SetFloatMode(wxTEXT_BOX_ATTR_FLOAT_LEFT);
    wxString position = "\\shpleft0\\shptop0\\shpright30\\shpbottom60";
    wxString wrap = "\\shpwrk2", horizontal = "1", cell;
    wxString dimensions = "\\picwgoal30\\pichgoal60";
    wxString line = "{\\sp{\\sn fLine}{\\sv 0}}", colours, pictureBorder, cropping;
    wxString distances = "{\\sp{\\sn dxWrapDistLeft}{\\sv 0}}{\\sp{\\sn dxWrapDistRight}{\\sv 0}}";
    wxRichTextParagraphLayoutBox* container = &buffer;
    bool inTextBox = false, copyFragment = false, twoImages = false, noFallbacks = false;
    wxString cellDefinition = "\\clftsWidth3\\clwWidth9360\\cellx9360";
    // Puts the image in a table cell, of the given size in pixels if any.
    const auto useCell = [&](int width = 0, int height = 0)
    {
        auto table = CreateTestTable(1, 1);
        Anchor(buffer, table);
        container = table->GetCell(0, 0);
        container->DeleteChildren();
        auto& box = container->GetAttributes().GetTextBoxAttr();
        if ( width )
        {
            box.GetWidth().SetValue(width, wxTEXT_ATTR_UNITS_PIXELS);
            cellDefinition = wxString::Format("\\clftsWidth3\\clwWidth%d\\cellx%d",
                                              width * 15, width * 15);
        }
        if ( height )
        {
            box.GetHeight().SetValue(height, wxTEXT_ATTR_UNITS_PIXELS);
            cellDefinition = wxString::Format("\\trrh%d", height * 15) + cellDefinition;
        }
        cell = "{\\sp{\\sn fLayoutInCell}{\\sv 1}}";
    };
    SECTION("Left floating image after text") { }
    SECTION("Shape result can be omitted")
    {
        buffer.SetHandlerFlags(wxRICHTEXT_HANDLER_RTF_NO_SHAPE_FALLBACKS);
        noFallbacks = true;
    }
    SECTION("Floating padding on each side")
    {
        auto& padding = attr.GetPadding();
        SECTION("Left")
        {
            padding.GetLeft().SetValue(1, wxTEXT_ATTR_UNITS_PIXELS);
            cropping = "{\\sp{\\sn cropFromLeft}{\\sv -32768}}";
            position = "\\shpleft0\\shptop0\\shpright45\\shpbottom60";
        }
        SECTION("Right")
        {
            padding.GetRight().SetValue(1, wxTEXT_ATTR_UNITS_PIXELS);
            cropping = "{\\sp{\\sn cropFromRight}{\\sv -32768}}";
            position = "\\shpleft0\\shptop0\\shpright45\\shpbottom60";
        }
        SECTION("Top")
        {
            padding.GetTop().SetValue(1, wxTEXT_ATTR_UNITS_PIXELS);
            cropping = "{\\sp{\\sn cropFromTop}{\\sv -16384}}";
            position = "\\shpleft0\\shptop0\\shpright30\\shpbottom75";
        }
        SECTION("Bottom")
        {
            padding.GetBottom().SetValue(1, wxTEXT_ATTR_UNITS_PIXELS);
            cropping = "{\\sp{\\sn cropFromBottom}{\\sv -16384}}";
            position = "\\shpleft0\\shptop0\\shpright30\\shpbottom75";
        }
    }
    SECTION("Combined floating padding and margin")
    {
        attr.GetPadding().GetLeft().SetValue(1, wxTEXT_ATTR_UNITS_PIXELS);
        attr.GetPadding().GetRight().SetValue(3, wxTEXT_ATTR_UNITS_PIXELS);
        attr.GetPadding().GetTop().SetValue(2, wxTEXT_ATTR_UNITS_PIXELS);
        attr.GetPadding().GetBottom().SetValue(4, wxTEXT_ATTR_UNITS_PIXELS);
        attr.GetMargins().GetRight().SetValue(10, wxTEXT_ATTR_UNITS_PIXELS);
        cropping = "{\\sp{\\sn cropFromLeft}{\\sv -32768}}{\\sp{\\sn cropFromRight}{\\sv -98304}}"
                   "{\\sp{\\sn cropFromTop}{\\sv -32768}}{\\sp{\\sn cropFromBottom}{\\sv -65536}}";
        distances = "{\\sp{\\sn dxWrapDistLeft}{\\sv 0}}{\\sp{\\sn dxWrapDistRight}{\\sv 95250}}";
        position = "\\shpleft0\\shptop0\\shpright90\\shpbottom150";
        SECTION("Left") { }
        SECTION("Right")
        {
            attr.SetFloatMode(wxTEXT_BOX_ATTR_FLOAT_RIGHT);
            wrap = "\\shpwrk1";
            horizontal.clear();
            position = "\\shpleft9120\\shptop0\\shpright9210\\shpbottom150";
        }
    }
    SECTION("Right floating image with paragraph-relative offset")
    {
        attr.SetFloatMode(wxTEXT_BOX_ATTR_FLOAT_RIGHT);
        attr.GetTop().SetValue(200, wxTEXT_ATTR_UNITS_PIXELS);
        position = "\\shpleft9330\\shptop3000\\shpright9360\\shpbottom3060";
        wrap = "\\shpwrk1";
        horizontal = "3";
    }
    SECTION("Scaled right float and percentage offset use containing dimensions")
    {
        useCell(100, 200);
        attr.SetFloatMode(wxTEXT_BOX_ATTR_FLOAT_RIGHT);
        attr.GetWidth().SetValue(20, wxTEXT_ATTR_UNITS_PIXELS);
        attr.GetTop().SetValue(25, wxTEXT_ATTR_UNITS_PERCENTAGE);
        position = "\\shpleft1200\\shptop750\\shpright1500\\shpbottom1350";
        wrap = "\\shpwrk1";
        horizontal = "3";
        dimensions = "\\picwgoal300\\pichgoal600";
    }
    SECTION("Floating image in table cell")
    {
        useCell();
    }
    SECTION("Oversized float reserves its margins")
    {
        useCell(100);
        attr.GetWidth().SetValue(200, wxTEXT_ATTR_UNITS_PIXELS);
        attr.GetMargins().GetLeft().SetValue(10, wxTEXT_ATTR_UNITS_PIXELS);
        attr.GetMargins().GetRight().SetValue(20, wxTEXT_ATTR_UNITS_PIXELS);
        dimensions = "\\picwgoal1050\\pichgoal2100";
        position = "\\shpleft150\\shptop0\\shpright1200\\shpbottom2100";
        horizontal.clear();
        distances = "{\\sp{\\sn dxWrapDistLeft}{\\sv 95250}}{\\sp{\\sn dxWrapDistRight}{\\sv 190500}}";
        SECTION("Left") { }
        SECTION("Right")
        {
            attr.SetFloatMode(wxTEXT_BOX_ATTR_FLOAT_RIGHT);
            wrap = "\\shpwrk1";
        }
    }
    SECTION("Oversized float reserves padding before fitting its pixels")
    {
        useCell(100);
        attr.GetWidth().SetValue(200, wxTEXT_ATTR_UNITS_PIXELS);
        SECTION("Pixel padding")
        {
            attr.GetPadding().GetLeft().SetValue(10, wxTEXT_ATTR_UNITS_PIXELS);
            attr.GetPadding().GetRight().SetValue(20, wxTEXT_ATTR_UNITS_PIXELS);
            dimensions = "\\picwgoal1050\\pichgoal2100";
            position = "\\shpleft0\\shptop0\\shpright1500\\shpbottom2100";
            cropping = "{\\sp{\\sn cropFromLeft}{\\sv -9362}}"
                       "{\\sp{\\sn cropFromRight}{\\sv -18725}}";
        }
        SECTION("Percentage padding is no space, as in wx")
        {
            attr.GetPadding().GetLeft().SetValue(10, wxTEXT_ATTR_UNITS_PERCENTAGE);
            attr.GetPadding().GetRight().SetValue(20, wxTEXT_ATTR_UNITS_PERCENTAGE);
            dimensions = "\\picwgoal1500\\pichgoal3000";
            position = "\\shpleft0\\shptop0\\shpright1500\\shpbottom3000";
        }
    }
    SECTION("Unrepresentable floating crop fails without changing the output")
    {
        attr.GetPadding().GetTop().SetValue(200000, wxTEXT_ATTR_UNITS_PIXELS);
        buffer.AddParagraph("before");
        auto para = wxDynamicCast(buffer.GetChildren().GetLast()->GetData(), wxRichTextParagraph);
        para->AppendChild(picture);
        wxString rtf = "unchanged";
        CHECK_FALSE(buffer.GetAsRTF(rtf));
        CHECK(rtf == "unchanged");
        return;
    }
    SECTION("Word-compatible inline fallback inside a text-box story")
    {
        auto box = CreateTestBox();
        box->GetAttributes().GetTextBoxAttr().GetWidth().SetValue(100, wxTEXT_ATTR_UNITS_PIXELS);
        box->GetAttributes().GetTextBoxAttr().GetHeight().SetValue(100, wxTEXT_ATTR_UNITS_PIXELS);
        Anchor(buffer, box);
        container = box;
        inTextBox = true;
        SECTION("No padding") { }
        SECTION("Fallback retains inline padding")
        {
            attr.GetPadding().GetLeft().SetValue(3, wxTEXT_ATTR_UNITS_PIXELS);
            attr.GetPadding().GetTop().SetValue(4, wxTEXT_ATTR_UNITS_PIXELS);
            pictureBorder = "\\piccropl-45\\piccropt-60";
        }
    }
    SECTION("Two floating images in the same paragraph") { twoImages = true; }
    SECTION("Clipboard fragment keeps image floating attributes") { copyFragment = true; }
    SECTION("Uniform border on a floating picture")
    {
        attr.GetBorder().SetWidth(2, wxTEXT_ATTR_UNITS_PIXELS);
        attr.GetBorder().SetStyle(wxTEXT_BOX_ATTR_BORDER_DASHED);
        attr.GetBorder().SetColour(wxColour(0, 0, 255));
        colours = "\\red0\\green0\\blue255;";
        line = "{\\sp{\\sn fLine}{\\sv 1}}{\\sp{\\sn lineWidth}{\\sv 19050}}"
               "{\\sp{\\sn lineColor}{\\sv 16711680}}{\\sp{\\sn lineDashing}{\\sv 1}}";
        pictureBorder = "\\brdrt\\brdrdash\\brdrw30\\brdrcf1\\brdrl\\brdrdash\\brdrw30\\brdrcf1"
                        "\\brdrb\\brdrdash\\brdrw30\\brdrcf1\\brdrr\\brdrdash\\brdrw30\\brdrcf1";
    }
    SECTION("Margins offset the float and its wrapping boundary")
    {
        attr.GetMargins().GetLeft().SetValue(10, wxTEXT_ATTR_UNITS_PIXELS);
        attr.GetMargins().GetRight().SetValue(20, wxTEXT_ATTR_UNITS_PIXELS);
        attr.GetMargins().GetTop().SetValue(30, wxTEXT_ATTR_UNITS_PIXELS);
        attr.GetMargins().GetBottom().SetValue(40, wxTEXT_ATTR_UNITS_PIXELS);
        attr.GetTop().SetValue(5, wxTEXT_ATTR_UNITS_PIXELS);
        position = "\\shpleft150\\shptop525\\shpright180\\shpbottom585";
        horizontal.clear();
        distances = "{\\sp{\\sn dxWrapDistLeft}{\\sv 95250}}{\\sp{\\sn dxWrapDistRight}{\\sv 190500}}"
                    "{\\sp{\\sn dyWrapDistTop}{\\sv 285750}}{\\sp{\\sn dyWrapDistBottom}{\\sv 381000}}";
        SECTION("Left float") { }
        SECTION("Right float")
        {
            attr.SetFloatMode(wxTEXT_BOX_ATTR_FLOAT_RIGHT);
            position = "\\shpleft9030\\shptop525\\shpright9060\\shpbottom585";
            wrap = "\\shpwrk1";
        }
    }
    container->AddParagraph("before");
    auto para = wxDynamicCast(container->GetChildren().GetLast()->GetData(), wxRichTextParagraph);
    para->AppendChild(picture);
    if ( twoImages )
    {
        auto right = new wxRichTextImage(block);
        right->GetAttributes().GetTextBoxAttr().SetFloatMode(wxTEXT_BOX_ATTR_FLOAT_RIGHT);
        para->AppendChild(right);
    }
    para->AppendChild(new wxRichTextPlainText("after"));
    const wxString data = TestPNGData();
    const wxString payload = "{\\pict\\pngblip\\picw2\\pich4" + dimensions + pictureBorder + data + "}";
    const wxString properties = "{\\sp{\\sn posrelh}{\\sv 2}}{\\sp{\\sn posrelv}{\\sv 2}}" + cell +
        line + "{\\sp{\\sn fFilled}{\\sv 0}}" + distances + cropping +
        "{\\sp{\\sn pib}{\\sv " + payload + "}}}" +
        (noFallbacks ? wxString() : "{\\shprslt " + payload + "}") + "}";
    const wxString alignment = horizontal.empty() ? wxString() :
        "{\\sp{\\sn posh}{\\sv " + horizontal + "}}";
    wxString images = inTextBox ? payload :
        "{\\shp{\\*\\shpinst" + position + "\\shpbxcolumn\\shpbypara\\shpwr2" + wrap +
        "{\\sp{\\sn shapeType}{\\sv 75}}" + alignment + properties;
    if ( twoImages )
        images += "{\\shp{\\*\\shpinst\\shpleft9330\\shptop0\\shpright9360\\shpbottom60"
                  "\\shpbxcolumn\\shpbypara\\shpwr2\\shpwrk1"
                  "{\\sp{\\sn shapeType}{\\sv 75}}{\\sp{\\sn posh}{\\sv 3}}" + properties;
    wxString body = "{\\pard" + wxString(cell.empty() ? "\\itap0" : "\\intbl\\itap1") +
        " {\\plain\\f0 before}" + images + "{\\plain\\f0 after}" +
        (cell.empty() ? "\\par\n}" : "\\cell }");
    if ( !cell.empty() )
        body = "{\\trowd\\trgaph0\\trleft0" + cellDefinition + "\n" + body + "\\row}\n";
    if ( inTextBox )
        body = "{\\shp{\\*\\shpinst\\shpleft0\\shptop0\\shpright1500\\shpbottom1500"
               "\\shpbxcolumn\\shpbypara\\shpwr1{\\sp{\\sn shapeType}{\\sv 202}}" +
               PlainTextBoxProperties() + ShapeText(body) + ShapeAnchor();
    CheckRTF(body, colours);
    CHECK(picture->GetOriginalImageSize() == wxSize(-1, -1));
    if ( copyFragment )
    {
        buffer.UpdateRanges();
        wxRichTextBuffer fragment;
        fragment.SetBasicStyle(buffer.GetBasicStyle());
        REQUIRE(buffer.CopyFragment(buffer.GetOwnRange(), fragment));
        wxString original, copied;
        REQUIRE(buffer.GetAsRTF(original));
        REQUIRE(fragment.GetAsRTF(copied));
        CHECK(copied == original);
    }
}
#endif // wxUSE_LIBPNG && wxUSE_STREAMS

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

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::LineBreak",
                 "[richtextctrl]")
{
    m_rich->WriteText("one");

    REQUIRE(m_rich->LineBreak());

    m_rich->LayoutContent();

    CHECK(m_rich->GetFocusObject()->GetLineCount() == 2);
    CHECK(m_rich->GetCaretPosition() == 3);
    CHECK(m_rich->GetCaretAtLineStart());

    m_rich->MoveLeft();

    CHECK(m_rich->GetCaretPosition() == 2);
    CHECK(!m_rich->GetCaretAtLineStart());

    m_rich->MoveRight();

    CHECK(m_rich->GetCaretPosition() == 3);
    CHECK(m_rich->GetCaretAtLineStart());

    m_rich->WriteText("two");

    wxString text;
    text << "one" << wxRichTextLineBreakChar << "two";

    CHECK(m_rich->GetValue() == text);

    m_rich->MoveToLineStart();

    CHECK(m_rich->GetCaretPosition() == 3);
    CHECK(m_rich->GetCaretAtLineStart());

    m_rich->MoveToLineEnd();

    CHECK(m_rich->GetCaretPosition() == 6);
}

TEST_CASE_METHOD(RichTextCtrlTestCase,
                 "RichTextCtrl::MoveDownToSingleCharacterLine",
                 "[richtextctrl]")
{
    m_rich->SetValue("This line contains several characters.\nX");

    m_rich->SetInsertionPointEnd();
    const long endOfSingleCharacterLine = m_rich->GetCaretPosition();

    m_rich->SetInsertionPoint(10);
    REQUIRE(m_rich->MoveDown());

    CHECK(m_rich->GetCaretPosition() == endOfSingleCharacterLine);
}

TEST_CASE_METHOD(RichTextCtrlTestCase,
                 "RichTextCtrl::MoveUpDownThroughEmptyLines",
                 "[richtextctrl]")
{
    m_rich->SetValue("Line one\n\nLine three\n\n\nLine six");

    const auto currentLine = [this]()
    {
        long x = -1, y = -1;
        REQUIRE(m_rich->PositionToXY(m_rich->GetInsertionPoint(), &x, &y));
        return y;
    };

    m_rich->SetInsertionPoint(4);
    REQUIRE(currentLine() == 0);

    for ( long line = 1; line <= 5; line++ )
    {
        INFO("Moving down to line " << line);
        REQUIRE(m_rich->MoveDown());
        CHECK(currentLine() == line);
    }

    for ( long line = 4; line >= 0; line-- )
    {
        INFO("Moving up to line " << line);
        REQUIRE(m_rich->MoveUp());
        CHECK(currentLine() == line);
    }
}

TEST_CASE_METHOD(RichTextCtrlTestCase,
                 "RichTextCtrl::MoveUpToEmptyFirstLine",
                 "[richtextctrl]")
{
    m_rich->SetValue("\nLine two");

    m_rich->SetInsertionPointEnd();
    REQUIRE(m_rich->MoveUp());

    CHECK(m_rich->GetInsertionPoint() == 0);
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

static void CheckTextEffectToggle(wxRichTextCtrl& rich, int effect)
{
    CHECK(!rich.DoesSelectionHaveTextEffectFlag(effect));

    rich.ApplyTextEffectToSelection(effect);
    CHECK(rich.DoesSelectionHaveTextEffectFlag(effect));

    rich.ApplyTextEffectToSelection(effect);
    CHECK(!rich.DoesSelectionHaveTextEffectFlag(effect));
}

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::TextEffect",
                 "[richtextctrl]")
{
    m_rich->SetValue("text");
    m_rich->SetInsertionPointEnd();

    CheckTextEffectToggle(*m_rich, wxTEXT_ATTR_EFFECT_STRIKETHROUGH);
    CheckTextEffectToggle(*m_rich, wxTEXT_ATTR_EFFECT_SUPERSCRIPT);
    CheckTextEffectToggle(*m_rich, wxTEXT_ATTR_EFFECT_SUBSCRIPT);
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

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::AppendTextStyle",
                 "[richtextctrl]")
{
    m_rich->BeginTextColour(*wxRED);
    m_rich->WriteText("red");
    m_rich->EndTextColour();

    m_rich->BeginTextColour(*wxBLUE);
    m_rich->AppendText("blue");
    m_rich->EndTextColour();

    wxTextAttr colour;
    m_rich->GetStyle(1, colour);

    CHECK(colour.GetTextColour() == *wxRED);

    m_rich->GetStyle(5, colour);

    CHECK(colour.GetTextColour() == *wxBLUE);
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

    wxRichTextParagraph *firstPara =
        m_rich->GetFocusObject()->GetParagraphAtPosition(5);
    wxRichTextAttr nextBullet;

    REQUIRE(m_rich->GetFocusObject()->FindNextParagraphNumber(firstPara,
                                                              nextBullet));
    CHECK(nextBullet.HasBulletStyle());
    CHECK(nextBullet.HasBulletNumber());
    CHECK(nextBullet.GetBulletNumber() == 2);

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

TEST_CASE_METHOD(RichTextCtrlTestCase, "RichTextCtrl::TextEffects",
                 "[richtextctrl]")
{
    const int effect = wxTEXT_ATTR_EFFECT_SUPERSCRIPT;

    m_rich->ApplyTextEffectToSelection(effect);
    CHECK(m_rich->DoesSelectionHaveTextEffectFlag(effect));

    m_rich->WriteText("x");

    m_rich->ApplyTextEffectToSelection(effect);
    CHECK(!m_rich->DoesSelectionHaveTextEffectFlag(effect));

    m_rich->WriteText("y");

    wxTextAttr style;
    m_rich->GetStyle(1, style);
    CHECK((style.GetTextEffects() & effect) == 0);
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

TEST_CASE("RichTextCtrl::DeleteParagraphEndRefresh", "[richtextctrl]")
{
    auto rich =
        make_unique<RefreshCountingRichTextCtrl>(wxTheApp->GetTopWindow());

    rich->AddParagraph("first paragraph");
    const wxRichTextRange range = rich->AddParagraph("second paragraph");
    rich->AddParagraph("third paragraph");

    rich->ResetRefreshCount();

    rich->SetSelection(range.GetStart(), range.GetEnd() + 1);
    REQUIRE(rich->HasSelection());

    rich->DeleteSelectedContent();

    CHECK(rich->GetRefreshCount() > 0);
    CHECK(rich->GetValue().Contains("first paragraph"));
    CHECK(!rich->GetValue().Contains("second paragraph"));
    CHECK(rich->GetValue().Contains("third paragraph"));
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
