///////////////////////////////////////////////////////////////////////////////
// Name:        tests/xml/xrctest.cpp
// Purpose:     XRC classes unit test
// Author:      wxWidgets team
// Created:     2010-10-30
// Copyright:   (c) 2010 wxWidgets team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#if wxUSE_XRC

#if defined(__WXWINUI__) && wxUSE_WINUI3
    #include "range-test-access.h"
#endif

#include "wx/fs_inet.h"
#include "wx/imagxpm.h"
#include "wx/log.h"
#include "wx/xml/xml.h"
#include "wx/sstream.h"
#include "wx/weakref.h"
#include "wx/wfstream.h"
#include "wx/xrc/xmlres.h"
#include "wx/xrc/xh_bmp.h"

#if wxUSE_BUTTON
    #include "wx/button.h"
#endif

#if wxUSE_CHECKBOX
    #include "wx/checkbox.h"
#endif

#if wxUSE_TOGGLEBTN
    #include "wx/tglbtn.h"
#endif

#if wxUSE_RADIOBTN
    #include "wx/radiobut.h"
#endif

#if wxUSE_RADIOBOX
    #include "wx/radiobox.h"
#endif

#if wxUSE_SEARCHCTRL
    #include "wx/srchctrl.h"
#endif

#if wxUSE_TEXTCTRL
    #include "wx/textctrl.h"
#endif

#if wxUSE_CHOICE
    #include "wx/choice.h"
#endif

#if wxUSE_COMBOBOX
    #include "wx/combobox.h"
#endif

#if wxUSE_LISTBOX
    #include "wx/listbox.h"
#endif

#if wxUSE_CHECKLISTBOX
    #include "wx/checklst.h"
#endif

#if wxUSE_BITMAPCOMBOBOX
    #include "wx/bmpcbox.h"
#endif

#if wxUSE_GAUGE
    #include "wx/gauge.h"
#endif

#if wxUSE_DATEPICKCTRL
    #include "wx/datectrl.h"
#endif

#if wxUSE_TIMEPICKCTRL
    #include "wx/timectrl.h"
#endif

#if wxUSE_CALENDARCTRL
    #include "wx/calctrl.h"
#endif

#if wxUSE_SLIDER
    #include "wx/slider.h"
#endif

#if wxUSE_SCROLLBAR
    #include "wx/scrolbar.h"
#endif

#if wxUSE_SPINBTN
    #include "wx/spinbutt.h"
#endif

#if wxUSE_SPINCTRL
    #include "wx/spinctrl.h"
#endif

#if wxUSE_TOOLBAR
    #include "wx/toolbar.h"
#endif

#if wxUSE_STATUSBAR
    #include "wx/statusbr.h"
#endif

#if wxUSE_HYPERLINKCTRL
    #include "wx/hyperlink.h"
#endif

#if wxUSE_ACTIVITYINDICATOR
    #include "wx/activityindicator.h"
#endif

#if wxUSE_INFOBAR
    #include "wx/infobar.h"
#endif

#if wxUSE_STATTEXT
    #include "wx/stattext.h"
#endif

#if wxUSE_STATBMP
    #include "wx/statbmp.h"
#endif

#if wxUSE_STATBOX
    #include "wx/statbox.h"
#endif

#if wxUSE_NOTEBOOK
    #include "wx/notebook.h"
#endif

#if wxUSE_BOOKCTRL
    #include "wx/simplebook.h"
#endif

#if wxUSE_CHOICEBOOK
    #include "wx/choicebk.h"
#endif

#if wxUSE_LISTBOOK
    #include "wx/listbook.h"
    #include "wx/listctrl.h"
#endif

#if wxUSE_TOOLBOOK
    #include "wx/toolbook.h"
#endif

#if wxUSE_TREEBOOK
    #include "wx/treebook.h"
#endif

#if wxUSE_TREECTRL
    #include "wx/treectrl.h"
#endif

#if wxUSE_SPLITTER
    #include "wx/splitter.h"
#endif

#include "wx/scrolwin.h"

#if wxUSE_MENUS
    #include "wx/menu.h"
#endif

#if wxUSE_ACCEL
    #include "wx/accel.h"
#endif

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#if defined(__WXWINUI__) && wxUSE_WINUI3
    #include "wx/winui/private/textpeer.h"
    #include "wx/winui/private/tlwhost.h"
    #include "wx/winui/private/tlwhostmsw.h"
#endif

#include <stdarg.h>

#include <memory>
#include <vector>

#include "testfile.h"
#include "waitfor.h"

// ----------------------------------------------------------------------------
// helpers to create/save some xrc
// ----------------------------------------------------------------------------

namespace
{

static const char *TEST_XRC_FILE = "test.xrc";

void LoadXrcFrom(const wxString& xrcText)
{
    wxStringInputStream sis(xrcText);
    std::unique_ptr<wxXmlDocument> xmlDoc(new wxXmlDocument(sis));
    REQUIRE( xmlDoc->IsOk() );

    // Load the xrc we've just created
    REQUIRE( wxXmlResource::Get()->LoadDocument(xmlDoc.release(), TEST_XRC_FILE) );
}

class XrcDocumentUnloadGuard
{
public:
    XrcDocumentUnloadGuard(wxXmlResource* xrc, const wxString& filename)
        : m_xrc(xrc),
          m_filename(filename)
    {
    }

    ~XrcDocumentUnloadGuard()
    {
        if ( !m_filename.empty() )
            m_xrc->Unload(m_filename);
    }

    bool Unload()
    {
        const bool unloaded = m_xrc->Unload(m_filename);
        if ( unloaded )
            m_filename.clear();
        return unloaded;
    }

private:
    wxXmlResource* const m_xrc;
    wxString m_filename;

    wxDECLARE_NO_COPY_CLASS(XrcDocumentUnloadGuard);
};

class DestroyWindowOnFontErrorLog final : public wxLog
{
public:
    explicit DestroyWindowOnFontErrorLog(wxWindow* const window)
        : m_window(window),
          m_windowLifetime(window),
          m_previous(wxLog::SetActiveTarget(this))
    {
    }

    ~DestroyWindowOnFontErrorLog() override
    {
        wxLog::SetActiveTarget(m_previous);
    }

    bool SawExpectedError() const { return m_sawExpectedError; }

#if defined(__WXWINUI__) && wxUSE_WINUI3
    void ReleaseRetainedCallback() { m_retainedCallback.reset(); }
#endif

protected:
    void DoLogText(const wxString& message) override
    {
        if ( m_sawExpectedError ||
                message.Find("double specification") == wxNOT_FOUND )
        {
            return;
        }

        m_sawExpectedError = true;
        if ( m_windowLifetime.get() != m_window )
            return;

#if defined(__WXWINUI__) && wxUSE_WINUI3
        m_retainedCallback.reset(
            new wxWinUITLWHostWindowEventGuard(m_window));
#endif
        m_window->Destroy();
    }

private:
    bool m_sawExpectedError = false;
    wxWindow* const m_window;
    const wxWeakRef<wxWindow> m_windowLifetime;
    wxLog* const m_previous;
#if defined(__WXWINUI__) && wxUSE_WINUI3
    std::unique_ptr<wxWinUITLWHostWindowEventGuard> m_retainedCallback;
#endif
};

#if wxUSE_CHOICEBOOK
class DestructivePageCountChoicebook final : public wxChoicebook
{
public:
    size_t GetPageCount() const override
    {
        const size_t count = wxChoicebook::GetPageCount();
        if ( !m_destroyedCandidate && count == 0 )
        {
            for ( wxWindow* const child : GetChildren() )
            {
                if ( child->GetName() == "destructivePageCountCandidate" )
                {
                    m_destroyedCandidate = true;
                    delete child;
                    break;
                }
            }
        }
        return count;
    }

    bool DestroyedCandidate() const { return m_destroyedCandidate; }

private:
    mutable bool m_destroyedCandidate = false;
};
#endif

// I'm hard-wiring the xrc into this function for now
// If different xrcs are wanted for future tests, it'll be easy to refactor
void LoadTestXrc()
{
    const char *xrcText =
    "<?xml version=\"1.0\" ?>"
    "<resource>"
    "  <object class=\"wxDialog\" name=\"dialog\">"
    "    <object class=\"wxBoxSizer\">"
    "      <orient>wxVERTICAL</orient>"
    "      <object class=\"sizeritem\">"
    "        <object class=\"wxPanel\" name=\"panel1\">"
    "          <object class=\"wxBoxSizer\">"
    "            <object class=\"sizeritem\">"
    "              <object class=\"wxBoxSizer\">"
    "                <orient>wxVERTICAL</orient>"
    "                <object class=\"sizeritem\">"
    "                  <object class=\"wxButton\" name=\"FirstCol[0]\">"
    "                    <label>0</label>"
    "                  </object>"
    "                </object>"
    "                <object class=\"sizeritem\">"
    "                  <object class=\"wxButton\" name=\"FirstCol[1]\">"
    "                    <label>1</label>"
    "                  </object>"
    "                </object>"
    "                <object class=\"sizeritem\">"
    "                  <object class=\"wxButton\" name=\"FirstCol[2]\">"
    "                    <label>2</label>"
    "                  </object>"
    "                </object>"
    "                <object class=\"sizeritem\">"
    "                  <object class=\"wxButton\" name=\"FirstCol[3]\">"
    "                    <label>3</label>"
    "                  </object>"
    "                </object>"
    "              </object>"
    "            </object>"
    "            <object class=\"sizeritem\">"
    "              <object class=\"wxBoxSizer\">"
    "                <orient>wxVERTICAL</orient>"
    "                <object class=\"sizeritem\">"
    "                  <object class=\"wxButton\" name=\"SecondCol[start]\">"
    "                    <label>0</label>"
    "                  </object>"
    "                </object>"
    "                <object class=\"sizeritem\">"
    "                  <object class=\"wxButton\" name=\"SecondCol[1]\">"
    "                    <label>1</label>"
    "                  </object>"
    "                </object>"
    "                <object class=\"sizeritem\">"
    "                  <object class=\"wxButton\" name=\"SecondCol[2]\">"
    "                    <label>2</label>"
    "                  </object>"
    "                </object>"
    "                <object class=\"sizeritem\">"
    "                  <object class=\"wxButton\" name=\"SecondCol[end]\">"
    "                    <label>3</label>"
    "                  </object>"
    "                </object>"
    "              </object>"
    "            </object>"
    "            <orient>wxHORIZONTAL</orient>"
    "          </object>"
    "      </object>"
    "    </object>"
    "      <object class=\"sizeritem\">"
    "        <object class=\"wxPanel\" name=\"ref_of_panel1\">"
    "          <object_ref ref=\"panel1\"/>"
    "        </object>"
    "      </object>"
    "    </object>"
    "    <title>test</title>"
    "  </object>"
    "  <ids-range name=\"FirstCol\" size=\"2\" start=\"10000\"/>"
    "  <ids-range name=\"SecondCol\" size=\"100\" />"
    "</resource>"
      ;

    LoadXrcFrom(wxString::FromAscii(xrcText));
}

class XrcSizeHintPanel : public wxPanel
{
public:
    explicit XrcSizeHintPanel(wxWindow *parent)
        : wxPanel(parent, wxID_ANY)
    {
        wxBoxSizer * const sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(220, 120);
        SetSizer(sizer);
        sizer->SetSizeHints(this);
    }
};

} // anon namespace


// ----------------------------------------------------------------------------
// test fixture and the tests using it
// ----------------------------------------------------------------------------

class XrcTestCase
{
public:
    XrcTestCase() { }

private:
    wxDECLARE_NO_COPY_CLASS(XrcTestCase);
};

TEST_CASE_METHOD(XrcTestCase, "XRC::ObjectReferences", "[xrc]")
{
    wxXmlResource::Get()->InitAllHandlers();

    for ( int n = 0; n < 2; ++n )
    {
        LoadTestXrc();

        // In xrc there's now a dialog containing two panels, one an object
        // reference of the other
        wxDialog dlg;
        REQUIRE( wxXmlResource::Get()->LoadDialog(&dlg, nullptr, "dialog") );
        // Might as well test XRCCTRL too
        wxPanel* panel1 = XRCCTRL(dlg,"panel1",wxPanel);
        wxPanel* panel2 = XRCCTRL(dlg,"ref_of_panel1",wxPanel);
        // Check that the object reference panel is a different object
        CHECK( panel2 != panel1 );

        // Unload the xrc, so it can be reloaded and the test rerun
        CHECK( wxXmlResource::Get()->Unload(TEST_XRC_FILE) );
    }
}

TEST_CASE_METHOD(XrcTestCase,
                 "XRC::Destructive invalid inherited font",
                 "[xrc][lifetime][font]")
{
    static const char xrcFile[] = "destructive-invalid-font.xrc";
    static const char xrcText[] =
        "<?xml version=\"1.0\" ?>"
        "<resource>"
        "  <object class=\"wxDialog\" name=\"fontDialog\">"
        "    <title>Destructive font diagnostic</title>"
        "    <font>"
        "      <sysfont>wxSYS_DEFAULT_GUI_FONT</sysfont>"
        "      <inherit>1</inherit>"
        "    </font>"
        "  </object>"
        "</resource>";

    wxXmlResource* const xrc = wxXmlResource::Get();
    xrc->InitAllHandlers();
    wxStringInputStream stream(wxString::FromAscii(xrcText));
    std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
    REQUIRE(document->IsOk());
    REQUIRE(xrc->LoadDocument(document.release(), xrcFile));
    XrcDocumentUnloadGuard unloadGuard(xrc, xrcFile);

    wxDialog* const dialog = new wxDialog;
    const wxWeakRef<wxWindow> weakDialog(dialog);
    DestroyWindowOnFontErrorLog destructiveLog(dialog);

    // The invalid sysfont+inherit combination reports through the active log
    // target while GetFont() still owns a raw parent argument. The target
    // destroys that exact dialog; GetFont() must restore m_node and stop before
    // reusing the retained/dying parent.
    (void)xrc->LoadObject(dialog, wxTheApp->GetTopWindow(),
                          "fontDialog", "wxDialog");
    CHECK(destructiveLog.SawExpectedError());

#if defined(__WXWINUI__) && wxUSE_WINUI3
    if ( weakDialog )
        CHECK(wxWinUITLWHostIsDestroyScheduled(dialog));
    destructiveLog.ReleaseRetainedCallback();
#endif

    REQUIRE(WaitFor("destructive XRC font cleanup",
                    [&weakDialog]() { return !weakDialog; }));
}

#if wxUSE_CHOICEBOOK

TEST_CASE_METHOD(XrcTestCase,
                 "XRC::Book page transaction rolls back page N",
                 "[xrc][bookctrl][lifetime]")
{
    static const char xrcFile[] = "book-page-rollback.xrc";
    static const char xrcText[] =
        "<?xml version=\"1.0\" ?>"
        "<resource>"
        "  <object class=\"wxDialog\" name=\"bookRollbackDialog\">"
        "    <title>Book rollback</title>"
        "    <object class=\"wxChoicebook\" name=\"rollbackBook\">"
        "      <object class=\"choicebookpage\">"
        "        <label>Must roll back</label>"
        "        <selected>1</selected>"
        "        <object class=\"wxPanel\" name=\"rolledBackPage\"/>"
        "      </object>"
        "      <object class=\"choicebookpage\">"
        "        <label>Invalid page N</label>"
        "      </object>"
        "    </object>"
        "  </object>"
        "</resource>";

    wxXmlResource* const xrc = wxXmlResource::Get();
    xrc->InitAllHandlers();
    wxStringInputStream stream(wxString::FromAscii(xrcText));
    std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
    REQUIRE(document->IsOk());
    REQUIRE(xrc->LoadDocument(document.release(), xrcFile));
    XrcDocumentUnloadGuard unloadGuard(xrc, xrcFile);

    wxDialog dialog;
    {
        wxLogNull suppressExpectedInvalidPage;
        REQUIRE(xrc->LoadDialog(&dialog, nullptr, "bookRollbackDialog"));
    }

    wxChoicebook* const book =
        XRCCTRL(dialog, "rollbackBook", wxChoicebook);
    REQUIRE(book);
    CHECK(book->GetPageCount() == 0);
    CHECK(book->GetChoiceCtrl()->GetCount() == 0);
    CHECK(book->GetSelection() == wxNOT_FOUND);
    CHECK(dialog.FindWindow(XRCID("rolledBackPage")) == nullptr);
}

TEST_CASE_METHOD(XrcTestCase,
                 "XRC::Book accessor destruction never publishes dangling page",
                 "[xrc][bookctrl][lifetime][reentrancy]")
{
    static const char xrcFile[] = "book-destructive-page-count.xrc";
    static const char xrcText[] =
        "<?xml version=\"1.0\" ?>"
        "<resource>"
        "  <object class=\"wxChoicebook\" name=\"destructiveBook\">"
        "    <object class=\"choicebookpage\">"
        "      <label>Never published</label>"
        "      <object class=\"wxPanel\" "
        "              name=\"destructivePageCountCandidate\"/>"
        "    </object>"
        "  </object>"
        "</resource>";

    wxXmlResource* const xrc = wxXmlResource::Get();
    xrc->InitAllHandlers();
    wxStringInputStream stream(wxString::FromAscii(xrcText));
    std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
    REQUIRE(document->IsOk());
    REQUIRE(xrc->LoadDocument(document.release(), xrcFile));
    XrcDocumentUnloadGuard unloadGuard(xrc, xrcFile);

    wxDialog parent(wxTheApp->GetTopWindow(), wxID_ANY,
                    "XRC destructive book parent");
    DestructivePageCountChoicebook book;
    {
        wxLogNull suppressExpectedRollback;
        // The page transaction is rolled back coherently, so the historical
        // XRC contract keeps the surrounding (now empty) book load successful.
        REQUIRE(xrc->LoadObject(
            &book, &parent, "destructiveBook", "wxChoicebook"));
    }

    CHECK(book.DestroyedCandidate());
    CHECK(book.GetPageCount() == 0);
    CHECK(book.GetChoiceCtrl()->GetCount() == 0);
    CHECK(parent.FindWindow(
              XRCID("destructivePageCountCandidate")) == nullptr);
}

#endif // wxUSE_CHOICEBOOK

#if wxUSE_SEARCHCTRL && wxUSE_TEXTCTRL

TEST_CASE_METHOD(XrcTestCase, "XRC::Text entry controls",
                 "[xrc][winui-textpeer]")
{
    static const char textEntryXrcFile[] = "text-entry-controls.xrc";
    static const char textEntryXrc[] =
        "<?xml version=\"1.0\" ?>"
        "<resource>"
        "  <object class=\"wxDialog\" name=\"textEntryDialog\">"
        "    <title>Text entry XRC smoke</title>"
        "    <object class=\"wxBoxSizer\">"
        "      <orient>wxVERTICAL</orient>"
        "      <object class=\"sizeritem\">"
        "        <object class=\"wxTextCtrl\" name=\"readonlyText\">"
        "          <value>Read-only value</value>"
        "          <style>wxTE_READONLY</style>"
        "          <hint>Read-only placeholder</hint>"
        "        </object>"
        "      </object>"
        "      <object class=\"sizeritem\">"
        "        <object class=\"wxSearchCtrl\" name=\"searchText\">"
        "          <value>Search value</value>"
        "          <style>wxTE_PROCESS_ENTER</style>"
        "          <hint>Search placeholder</hint>"
        "        </object>"
        "      </object>"
        "    </object>"
        "  </object>"
        "</resource>";

    wxXmlResource* const xrc = wxXmlResource::Get();
    xrc->InitAllHandlers();

    wxStringInputStream stream(wxString::FromAscii(textEntryXrc));
    std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
    REQUIRE( document->IsOk() );
    REQUIRE( xrc->LoadDocument(document.release(), textEntryXrcFile) );
    XrcDocumentUnloadGuard unloadGuard(xrc, textEntryXrcFile);

    // Destroy the controls before unloading their resource. Besides matching
    // normal XRC ownership, this exercises the native peer teardown path.
    wxWeakRef<wxTextCtrl> text;
    wxWeakRef<wxSearchCtrl> search;
    {
        wxDialog dialog;
        REQUIRE( xrc->LoadDialog(&dialog, nullptr, "textEntryDialog") );

        text = XRCCTRL(dialog, "readonlyText", wxTextCtrl);
        search = XRCCTRL(dialog, "searchText", wxSearchCtrl);
        REQUIRE( text );
        REQUIRE( search );

        CHECK( text->GetValue() == "Read-only value" );
        CHECK_FALSE( text->IsEditable() );
        CHECK( text->HasFlag(wxTE_READONLY) );
        CHECK( text->GetHint() == "Read-only placeholder" );

        CHECK( search->GetValue() == "Search value" );
        CHECK( search->HasFlag(wxTE_PROCESS_ENTER) );
        CHECK( search->GetDescriptiveText() == "Search placeholder" );
    }

    CHECK_FALSE( text );
    CHECK_FALSE( search );
    CHECK( unloadGuard.Unload() );
}

#endif // wxUSE_SEARCHCTRL && wxUSE_TEXTCTRL

#if wxUSE_CHOICE && wxUSE_COMBOBOX && wxUSE_LISTBOX && \
    wxUSE_CHECKLISTBOX && wxUSE_BITMAPCOMBOBOX

TEST_CASE_METHOD(XrcTestCase, "XRC::WinUI item controls",
                 "[xrc][winui-itemmodel]")
{
    static const char itemXrcFile[] = "winui-item-controls.xrc";
    static const char itemXrc[] =
        "<?xml version=\"1.0\" ?>"
        "<resource>"
        "  <object class=\"wxDialog\" name=\"itemDialog\">"
        "    <title>WinUI item XRC smoke</title>"
        "    <object class=\"wxBoxSizer\">"
        "      <orient>wxVERTICAL</orient>"
        "      <object class=\"sizeritem\">"
        "        <object class=\"wxChoice\" name=\"choice\">"
        "          <selection>1</selection>"
        "          <content><item>alpha</item><item>beta</item></content>"
        "        </object>"
        "      </object>"
        "      <object class=\"sizeritem\">"
        "        <object class=\"wxComboBox\" name=\"combo\">"
        "          <value>typed</value><hint>combo hint</hint>"
        "          <content><item>one</item><item>two</item></content>"
        "        </object>"
        "      </object>"
        "      <object class=\"sizeritem\">"
        "        <object class=\"wxListBox\" name=\"list\">"
        "          <style>wxLB_MULTIPLE</style>"
        "          <content><item>red</item><item>green</item></content>"
        "        </object>"
        "      </object>"
        "      <object class=\"sizeritem\">"
        "        <object class=\"wxCheckListBox\" name=\"checklist\">"
        "          <content>"
        "            <item checked=\"1\">checked</item>"
        "            <item>clear</item>"
        "          </content>"
        "        </object>"
        "      </object>"
        "      <object class=\"sizeritem\">"
        "        <object class=\"wxBitmapComboBox\" name=\"bitmapcombo\">"
        "          <selection>0</selection>"
        "          <object class=\"ownerdrawnitem\">"
        "            <text>bitmap item</text>"
        "          </object>"
        "        </object>"
        "      </object>"
        "    </object>"
        "  </object>"
        "</resource>";

    wxXmlResource* const xrc = wxXmlResource::Get();
    xrc->InitAllHandlers();
    wxStringInputStream stream(wxString::FromAscii(itemXrc));
    std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
    REQUIRE( document->IsOk() );
    REQUIRE( xrc->LoadDocument(document.release(), itemXrcFile) );
    XrcDocumentUnloadGuard unloadGuard(xrc, itemXrcFile);

    wxWeakRef<wxChoice> choice;
    wxWeakRef<wxComboBox> combo;
    wxWeakRef<wxListBox> list;
    wxWeakRef<wxCheckListBox> checklist;
    wxWeakRef<wxBitmapComboBox> bitmapCombo;
    {
        wxDialog dialog;
        REQUIRE( xrc->LoadDialog(&dialog, nullptr, "itemDialog") );
        choice = XRCCTRL(dialog, "choice", wxChoice);
        combo = XRCCTRL(dialog, "combo", wxComboBox);
        list = XRCCTRL(dialog, "list", wxListBox);
        checklist = XRCCTRL(dialog, "checklist", wxCheckListBox);
        bitmapCombo =
            XRCCTRL(dialog, "bitmapcombo", wxBitmapComboBox);
        REQUIRE( choice );
        REQUIRE( combo );
        REQUIRE( list );
        REQUIRE( checklist );
        REQUIRE( bitmapCombo );

        CHECK( choice->GetCount() == 2 );
        CHECK( choice->GetSelection() == 1 );
        CHECK( combo->GetValue() == "typed" );
        CHECK( combo->GetHint() == "combo hint" );
        CHECK( list->GetCount() == 2 );
        CHECK( list->HasMultipleSelection() );
        CHECK( checklist->GetCount() == 2 );
        CHECK( checklist->IsChecked(0) );
        CHECK_FALSE( checklist->IsChecked(1) );
        CHECK( bitmapCombo->GetCount() == 1 );
        CHECK( bitmapCombo->GetSelection() == 0 );
    }

    CHECK_FALSE( choice );
    CHECK_FALSE( combo );
    CHECK_FALSE( list );
    CHECK_FALSE( checklist );
    CHECK_FALSE( bitmapCombo );
    CHECK( unloadGuard.Unload() );
}

#endif // all WinUI item controls

#if defined(__WXWINUI__) && \
    wxUSE_GAUGE && wxUSE_DATEPICKCTRL && \
    wxUSE_TIMEPICKCTRL && wxUSE_CALENDARCTRL && \
    wxUSE_SLIDER && wxUSE_SCROLLBAR && \
    wxUSE_SPINBTN && wxUSE_SPINCTRL

TEST_CASE_METHOD(XrcTestCase, "XRC::WinUI 008c range and date controls",
                 "[xrc][winui-range-date]")
{
    static const char rangeDateXrcFile[] =
        "winui-range-date-controls.xrc";
    static const char rangeDateXrc[] =
        "<?xml version=\"1.0\" ?>"
        "<resource>"
        "  <object class=\"wxDialog\" name=\"rangeDateDialog\">"
        "    <title>WinUI range/date XRC smoke</title>"
        "    <object class=\"wxBoxSizer\">"
        "      <orient>wxVERTICAL</orient>"
        "      <object class=\"sizeritem\">"
        "        <object class=\"wxGauge\" name=\"gauge\">"
        "          <range>0</range><value>0</value>"
        "          <style>wxGA_VERTICAL|wxGA_PROGRESS</style>"
        "        </object>"
        "      </object>"
        "      <object class=\"sizeritem\">"
        "        <object class=\"wxDatePickerCtrl\" name=\"date\">"
        "          <style>wxDP_DROPDOWN|wxDP_SHOWCENTURY|"
        "wxDP_ALLOWNONE</style>"
        "        </object>"
        "      </object>"
        "      <object class=\"sizeritem\">"
        "        <object class=\"wxTimePickerCtrl\" name=\"time\"/>"
        "      </object>"
        "      <object class=\"sizeritem\">"
        "        <object class=\"wxCalendarCtrl\" name=\"calendar\">"
        "          <style>wxCAL_MONDAY_FIRST|"
        "wxCAL_NO_MONTH_CHANGE</style>"
        "        </object>"
        "      </object>"
        "      <object class=\"sizeritem\">"
        "        <object class=\"wxSlider\" name=\"slider\">"
        "          <value>25</value><min>-10</min><max>90</max>"
        "          <style>wxSL_VERTICAL|wxSL_INVERSE|"
        "wxSL_AUTOTICKS|wxSL_BOTH</style>"
        "          <tickfreq>5</tickfreq><pagesize>20</pagesize>"
        "          <linesize>2</linesize><thumb>24</thumb>"
        "        </object>"
        "      </object>"
        "      <object class=\"sizeritem\">"
        "        <object class=\"wxScrollBar\" name=\"scroll\">"
        "          <style>wxSB_HORIZONTAL</style>"
        "          <value>15</value><thumbsize>10</thumbsize>"
        "          <range>100</range><pagesize>20</pagesize>"
        "        </object>"
        "      </object>"
        "      <object class=\"sizeritem\">"
        "        <object class=\"wxSpinButton\" name=\"spinButton\">"
        "          <style>wxSP_HORIZONTAL|wxSP_WRAP</style>"
        "          <value>3</value><min>-5</min><max>15</max>"
        "          <inc>2</inc>"
        "        </object>"
        "      </object>"
        "      <object class=\"sizeritem\">"
        "        <object class=\"wxSpinCtrl\" name=\"spinCtrl\">"
        "          <style>wxSP_ARROW_KEYS|wxSP_WRAP</style>"
        "          <value>26</value><min>0</min><max>100</max>"
        "          <inc>3</inc><base>16</base>"
        "        </object>"
        "      </object>"
        "      <object class=\"sizeritem\">"
        "        <object class=\"wxSpinCtrlDouble\" name=\"spinDouble\">"
        "          <style>wxSP_ARROW_KEYS|wxSP_WRAP</style>"
        "          <value>1.25</value><min>-2.5</min><max>5.5</max>"
        "          <inc>0.25</inc><digits>2</digits>"
        "        </object>"
        "      </object>"
        "    </object>"
        "  </object>"
        "</resource>";

    wxXmlResource* const xrc = wxXmlResource::Get();
    xrc->InitAllHandlers();
    wxStringInputStream stream(wxString::FromAscii(rangeDateXrc));
    std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
    REQUIRE( document->IsOk() );
    REQUIRE( xrc->LoadDocument(document.release(), rangeDateXrcFile) );
    XrcDocumentUnloadGuard unloadGuard(xrc, rangeDateXrcFile);

    wxWeakRef<wxGauge> gauge;
    wxWeakRef<wxDatePickerCtrl> date;
    wxWeakRef<wxTimePickerCtrl> time;
    wxWeakRef<wxCalendarCtrl> calendar;
    wxWeakRef<wxSlider> slider;
    wxWeakRef<wxScrollBar> scroll;
    wxWeakRef<wxSpinButton> spinButton;
    wxWeakRef<wxSpinCtrl> spinCtrl;
    wxWeakRef<wxSpinCtrlDouble> spinDouble;
    {
        wxDialog dialog;
        // wxTimePickerCtrl uses the current local time, including seconds,
        // when XRC doesn't provide an explicit value. Bracket creation so the
        // oracle remains exact even when the test crosses midnight.
        const wxDateTime timeBeforeCreate = wxDateTime::Now();
        REQUIRE( xrc->LoadDialog(
            &dialog, nullptr, "rangeDateDialog") );
        gauge = XRCCTRL(dialog, "gauge", wxGauge);
        date = XRCCTRL(dialog, "date", wxDatePickerCtrl);
        time = XRCCTRL(dialog, "time", wxTimePickerCtrl);
        calendar = XRCCTRL(dialog, "calendar", wxCalendarCtrl);
        slider = XRCCTRL(dialog, "slider", wxSlider);
        scroll = XRCCTRL(dialog, "scroll", wxScrollBar);
        spinButton =
            XRCCTRL(dialog, "spinButton", wxSpinButton);
        spinCtrl = XRCCTRL(dialog, "spinCtrl", wxSpinCtrl);
        spinDouble =
            XRCCTRL(dialog, "spinDouble", wxSpinCtrlDouble);
        REQUIRE( gauge );
        REQUIRE( date );
        REQUIRE( time );
        REQUIRE( calendar );
        REQUIRE( slider );
        REQUIRE( scroll );
        REQUIRE( spinButton );
        REQUIRE( spinCtrl );
        REQUIRE( spinDouble );

        CHECK( gauge->GetRange() == 0 );
        CHECK( gauge->GetValue() == 0 );
        CHECK( gauge->IsVertical() );
        CHECK( gauge->HasFlag(wxGA_PROGRESS) );
        CHECK( wxWinUIRangeTestAccess::HasAppProgress(*gauge) );

        // The public zero range is valid for wxGauge even though the taskbar
        // progress bridge requires a positive maximum. Exercise both bridge
        // creation above and its range transition back to zero here.
        gauge->SetRange(25);
        CHECK( gauge->GetRange() == 25 );
        CHECK( gauge->GetValue() == 0 );
        CHECK( wxWinUIRangeTestAccess::HasAppProgress(*gauge) );
        gauge->SetRange(0);
        CHECK( gauge->GetRange() == 0 );
        CHECK( gauge->GetValue() == 0 );
        CHECK( wxWinUIRangeTestAccess::HasAppProgress(*gauge) );

        CHECK( date->HasFlag(wxDP_ALLOWNONE) );
        CHECK_FALSE( date->GetValue().IsValid() );

        int hour = -1;
        int minute = -1;
        int second = -1;
        REQUIRE( time->GetTime(&hour, &minute, &second) );
        CHECK( hour >= 0 );
        CHECK( hour < 24 );
        CHECK( minute >= 0 );
        CHECK( minute < 60 );
        CHECK( second >= 0 );
        CHECK( second < 60 );
        const wxDateTime timeAfterCreate = wxDateTime::Now();
        const auto secondsSinceMidnight = [](const wxDateTime& value)
        {
            return value.GetHour() * 60 * 60 +
                   value.GetMinute() * 60 + value.GetSecond();
        };
        const int createdSeconds = hour * 60 * 60 + minute * 60 + second;
        const int beforeSeconds = secondsSinceMidnight(timeBeforeCreate);
        const int afterSeconds = secondsSinceMidnight(timeAfterCreate);
        if ( beforeSeconds <= afterSeconds )
        {
            CHECK( createdSeconds >= beforeSeconds );
            CHECK( createdSeconds <= afterSeconds );
        }
        else
        {
            CHECK( (createdSeconds >= beforeSeconds ||
                    createdSeconds <= afterSeconds) );
        }

        CHECK( calendar->HasFlag(wxCAL_MONDAY_FIRST) );
        CHECK_FALSE( calendar->AllowMonthChange() );
        wxDateTime calendarMinimum;
        wxDateTime calendarMaximum;
        REQUIRE( calendar->WinUIGetPeerStateForTesting(
            nullptr, &calendarMinimum, &calendarMaximum) );
        wxDateTime monthMinimum(calendar->GetDate());
        monthMinimum.SetDay(1);
        CHECK( calendarMinimum == monthMinimum );
        CHECK( calendarMaximum == monthMinimum.GetLastMonthDay() );

        CHECK( slider->GetValue() == 25 );
        CHECK( slider->GetMin() == -10 );
        CHECK( slider->GetMax() == 90 );
        CHECK( slider->GetLineSize() == 2 );
        CHECK( slider->GetPageSize() == 20 );
        CHECK( slider->GetTickFreq() == 5 );
        // The XRC handler must preserve its explicit public thumb-length
        // property, matching wxMSW TBM_SET/GETTHUMBLENGTH semantics.
        CHECK( slider->GetThumbLength() == 24 );
        double sliderMinimum = 0;
        double sliderMaximum = 0;
        double sliderValue = 0;
        double sliderSmall = 0;
        double sliderLarge = 0;
        bool sliderVertical = false;
        bool sliderReversed = false;
        REQUIRE( slider->WinUIGetPeerStateForTesting(
            &sliderMinimum, &sliderMaximum, &sliderValue,
            &sliderSmall, &sliderLarge,
            &sliderVertical, &sliderReversed) );
        CHECK( sliderMinimum == -10.0 );
        CHECK( sliderMaximum == 90.0 );
        CHECK( sliderValue == 25.0 );
        CHECK( sliderSmall == 2.0 );
        CHECK( sliderLarge == 20.0 );
        CHECK( sliderVertical );
        CHECK( sliderReversed );

        CHECK( scroll->GetThumbPosition() == 15 );
        CHECK( scroll->GetThumbSize() == 10 );
        CHECK( scroll->GetRange() == 100 );
        CHECK( scroll->GetPageSize() == 20 );
        double scrollMinimum = -1;
        double scrollMaximum = -1;
        double scrollValue = -1;
        double scrollViewport = -1;
        double scrollLarge = -1;
        bool scrollVertical = true;
        REQUIRE( wxWinUIRangeTestAccess::GetPeerState(*scroll,
            &scrollMinimum, &scrollMaximum, &scrollValue,
            &scrollViewport, nullptr, &scrollLarge,
            &scrollVertical) );
        CHECK( scrollMinimum == 0.0 );
        CHECK( scrollMaximum == 90.0 );
        CHECK( scrollValue == 15.0 );
        CHECK( scrollViewport == 10.0 );
        CHECK( scrollLarge == 20.0 );
        CHECK_FALSE( scrollVertical );

        CHECK( spinButton->GetValue() == 3 );
        CHECK( spinButton->GetMin() == -5 );
        CHECK( spinButton->GetMax() == 15 );
        CHECK( spinButton->GetIncrement() == 2 );
        bool spinVertical = true;
        unsigned spinRows = 0;
        unsigned spinColumns = 0;
        REQUIRE( wxWinUIRangeTestAccess::GetPeerLayout(*spinButton,
            &spinVertical, &spinRows, &spinColumns) );
        CHECK_FALSE( spinVertical );
        CHECK( spinRows == 0 );
        CHECK( spinColumns == 2 );

        CHECK( spinCtrl->GetValue() == 26 );
        CHECK( spinCtrl->GetMin() == 0 );
        CHECK( spinCtrl->GetMax() == 100 );
        CHECK( spinCtrl->GetIncrement() == 3 );
        CHECK( spinCtrl->GetBase() == 16 );
        double spinMinimum = 0;
        double spinMaximum = 0;
        double spinIncrement = 0;
        bool spinWrap = false;
        wxString spinText;
        REQUIRE( spinCtrl->WinUIGetPeerStateForTesting(
            &spinMinimum, &spinMaximum, &spinIncrement,
            &spinWrap, &spinText) );
        CHECK( spinMinimum == 0.0 );
        CHECK( spinMaximum == 100.0 );
        CHECK( spinIncrement == 3.0 );
        CHECK( spinWrap );
        CHECK( spinText.Lower().Contains("1a") );

        CHECK( spinDouble->GetValue() == 1.25 );
        CHECK( spinDouble->GetMin() == -2.5 );
        CHECK( spinDouble->GetMax() == 5.5 );
        CHECK( spinDouble->GetIncrement() == 0.25 );
        CHECK( spinDouble->GetDigits() == 2 );
        wxString spinDoubleText;
        REQUIRE( spinDouble->WinUIGetPeerStateForTesting(
            &spinMinimum, &spinMaximum, &spinIncrement,
            &spinWrap, &spinDoubleText) );
        CHECK( spinMinimum == -2.5 );
        CHECK( spinMaximum == 5.5 );
        CHECK( spinIncrement == 0.25 );
        CHECK( spinWrap );
        CHECK( spinDoubleText == spinDouble->GetTextValue() );
    }

    CHECK_FALSE( gauge );
    CHECK_FALSE( date );
    CHECK_FALSE( time );
    CHECK_FALSE( calendar );
    CHECK_FALSE( slider );
    CHECK_FALSE( scroll );
    CHECK_FALSE( spinButton );
    CHECK_FALSE( spinCtrl );
    CHECK_FALSE( spinDouble );
    CHECK( unloadGuard.Unload() );
}

#endif // all WinUI 008c controls

#if defined(__WXWINUI__) && \
    wxUSE_TOOLBAR && wxUSE_STATUSBAR && \
    wxUSE_HYPERLINKCTRL && wxUSE_ACTIVITYINDICATOR && \
    wxUSE_INFOBAR && wxUSE_STATTEXT && wxUSE_STATBMP && wxUSE_STATBOX

class XrcWinUIHyperlinkSubclass final : public wxHyperlinkCtrl
{
public:
    XrcWinUIHyperlinkSubclass() = default;

private:
    wxDECLARE_DYNAMIC_CLASS(XrcWinUIHyperlinkSubclass);
};

wxIMPLEMENT_DYNAMIC_CLASS(XrcWinUIHyperlinkSubclass, wxHyperlinkCtrl);

class XrcWinUIGenericHyperlinkSubclass final
    : public wxGenericHyperlinkCtrl
{
public:
    XrcWinUIGenericHyperlinkSubclass() = default;

private:
    wxDECLARE_DYNAMIC_CLASS(XrcWinUIGenericHyperlinkSubclass);
};

wxIMPLEMENT_DYNAMIC_CLASS(
    XrcWinUIGenericHyperlinkSubclass, wxGenericHyperlinkCtrl);

TEST_CASE_METHOD(XrcTestCase, "XRC::WinUI 008f chrome and feedback controls",
                 "[xrc][winui-chrome-feedback]")
{
    static const char chromeFeedbackXrcFile[] =
        "winui-chrome-feedback-controls.xrc";
    static const char chromeFeedbackXrc[] =
        "<?xml version=\"1.0\" ?>"
        "<resource>"
        "  <object class=\"wxFrame\" name=\"chromeFeedbackFrame\">"
        "    <title>WinUI chrome/feedback XRC smoke</title>"
        "    <size>520,360</size>"
        "    <object class=\"wxToolBar\" name=\"chromeToolbar\">"
        "      <style>wxTB_HORIZONTAL|wxTB_TEXT</style>"
        "      <object class=\"tool\" name=\"chromeNormalTool\">"
        "        <label>&amp;Open</label>"
        "        <tooltip>Open tooltip</tooltip>"
        "        <bitmap stock_id=\"wxART_INFORMATION\"/>"
        "      </object>"
        "      <object class=\"tool\" name=\"chromeCheckTool\">"
        "        <label>Checked</label><toggle>1</toggle><checked>1</checked>"
        "        <bitmap stock_id=\"wxART_TICK_MARK\"/>"
        "      </object>"
        "      <object class=\"space\"/>"
        "    </object>"
        "    <object class=\"wxStatusBar\" name=\"chromeStatusBar\">"
        "      <fields>2</fields><widths>-1,120</widths>"
        "      <styles>wxSB_NORMAL,wxSB_FLAT</styles>"
        "    </object>"
        "    <object class=\"wxPanel\" name=\"feedbackPanel\">"
        "      <object class=\"wxBoxSizer\">"
        "        <orient>wxVERTICAL</orient>"
        "        <object class=\"sizeritem\">"
        "          <object class=\"wxHyperlinkCtrl\" "
        "subclass=\"XrcWinUIHyperlinkSubclass\" name=\"feedbackLink\">"
        "            <label>Project site</label>"
        "            <url>https://example.invalid/winui</url>"
        "            <style>wxHL_ALIGN_LEFT|wxHL_CONTEXTMENU</style>"
        "          </object>"
        "        </object>"
        "        <object class=\"sizeritem\">"
        "          <object class=\"wxGenericHyperlinkCtrl\" "
        "subclass=\"XrcWinUIGenericHyperlinkSubclass\" "
        "name=\"feedbackGenericLink\">"
        "            <label>Generic project site</label>"
        "            <url>https://example.invalid/generic</url>"
        "            <style>wxHL_ALIGN_RIGHT</style>"
        "          </object>"
        "        </object>"
        "        <object class=\"sizeritem\">"
        "          <object class=\"wxStatusBar\" "
        "name=\"feedbackSingleStatus\">"
        "            <fields>1</fields><widths>137</widths>"
        "            <enabled>0</enabled>"
        "          </object>"
        "        </object>"
        "        <object class=\"sizeritem\">"
        "          <object class=\"wxActivityIndicator\" "
        "name=\"feedbackActivity\"><running>1</running></object>"
        "        </object>"
        "        <object class=\"sizeritem\">"
        "          <object class=\"wxInfoBar\" name=\"feedbackInfoBar\">"
        "            <checkboxlabel>Remember</checkboxlabel><checked>1</checked>"
        "            <effectduration>250</effectduration>"
        "            <showeffect>wxSHOW_EFFECT_EXPAND</showeffect>"
        "            <hideeffect>wxSHOW_EFFECT_SLIDE_TO_RIGHT</hideeffect>"
        "            <object class=\"button\" name=\"wxID_CLOSE\">"
        "              <label>Close</label>"
        "            </object>"
        "          </object>"
        "        </object>"
        "        <object class=\"sizeritem\">"
        "          <object class=\"wxStaticText\" name=\"feedbackStaticText\">"
        "            <label>Static feedback</label>"
        "          </object>"
        "        </object>"
        "        <object class=\"sizeritem\">"
        "          <object class=\"wxStaticBitmap\" name=\"feedbackStaticBitmap\">"
        "            <bitmap stock_id=\"wxART_INFORMATION\"/>"
        "          </object>"
        "        </object>"
        "        <object class=\"sizeritem\">"
        "          <object class=\"wxStaticBox\" name=\"feedbackStaticBox\">"
        "            <label>Static group</label>"
        "          </object>"
        "        </object>"
        "      </object>"
        "    </object>"
        "  </object>"
        "</resource>";

    wxXmlResource* const xrc = wxXmlResource::Get();
    xrc->InitAllHandlers();
    wxStringInputStream stream(wxString::FromAscii(chromeFeedbackXrc));
    std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
    REQUIRE( document->IsOk() );
    REQUIRE( xrc->LoadDocument(document.release(), chromeFeedbackXrcFile) );
    XrcDocumentUnloadGuard unloadGuard(xrc, chromeFeedbackXrcFile);

    wxFrame* const frame =
        xrc->LoadFrame(nullptr, "chromeFeedbackFrame");
    REQUIRE( frame );

    wxToolBar* const toolbar =
        XRCCTRL(*frame, "chromeToolbar", wxToolBar);
    wxStatusBar* const status =
        XRCCTRL(*frame, "chromeStatusBar", wxStatusBar);
    wxHyperlinkCtrl* const link =
        XRCCTRL(*frame, "feedbackLink", wxHyperlinkCtrl);
    wxWindow* const genericWindow =
        frame->FindWindow(XRCID("feedbackGenericLink"));
    wxGenericHyperlinkCtrl* const genericLink =
        static_cast<wxGenericHyperlinkCtrl *>(genericWindow);
    wxStatusBar* const singleStatus =
        XRCCTRL(*frame, "feedbackSingleStatus", wxStatusBar);
    wxPanel* const feedbackPanel =
        XRCCTRL(*frame, "feedbackPanel", wxPanel);
    wxActivityIndicator* const activity =
        XRCCTRL(*frame, "feedbackActivity", wxActivityIndicator);
    wxInfoBar* const infoBar =
        XRCCTRL(*frame, "feedbackInfoBar", wxInfoBar);
    wxStaticText* const staticText =
        XRCCTRL(*frame, "feedbackStaticText", wxStaticText);
    wxStaticBitmap* const staticBitmap =
        XRCCTRL(*frame, "feedbackStaticBitmap", wxStaticBitmap);
    wxStaticBox* const staticBox =
        XRCCTRL(*frame, "feedbackStaticBox", wxStaticBox);

    REQUIRE( toolbar );
    REQUIRE( status );
    REQUIRE( link );
    REQUIRE( genericLink );
    REQUIRE( singleStatus );
    REQUIRE( feedbackPanel );
    REQUIRE( activity );
    REQUIRE( infoBar );
    REQUIRE( staticText );
    REQUIRE( staticBitmap );
    REQUIRE( staticBox );

    CHECK( frame->GetToolBar() == toolbar );
    CHECK( toolbar->GetToolsCount() == 3 );
    CHECK( toolbar->GetToolShortHelp(XRCID("chromeNormalTool")) ==
           "Open tooltip" );
    CHECK( toolbar->GetToolState(XRCID("chromeCheckTool")) );

    CHECK( frame->GetStatusBar() == status );
    CHECK( status->GetFieldsCount() == 2 );
    CHECK( status->GetStatusWidth(0) == -1 );
    CHECK( status->GetStatusWidth(1) == 120 );
    CHECK( status->GetStatusStyle(0) == wxSB_NORMAL );
    CHECK( status->GetStatusStyle(1) == wxSB_FLAT );

    CHECK( link->GetLabel() == "Project site" );
    CHECK( link->GetURL() == "https://example.invalid/winui" );
    CHECK( link->HasFlag(wxHL_ALIGN_LEFT) );
    CHECK( link->HasFlag(wxHL_CONTEXTMENU) );
    CHECK( wxDynamicCast(link, XrcWinUIHyperlinkSubclass) == link );
    CHECK( link->GetParent() == feedbackPanel );
    CHECK( link->GetHandle() != nullptr );
    CHECK( genericLink->GetLabel() == "Generic project site" );
    CHECK( genericLink->GetURL() == "https://example.invalid/generic" );
    CHECK( genericLink->HasFlag(wxHL_ALIGN_RIGHT) );
    CHECK( wxDynamicCast(genericLink, XrcWinUIGenericHyperlinkSubclass) ==
           genericLink );
    CHECK( genericLink->GetParent() == feedbackPanel );
    CHECK( genericLink->GetHandle() != nullptr );
    CHECK( singleStatus->GetFieldsCount() == 1 );
    CHECK( singleStatus->GetStatusWidth(0) == 137 );
    CHECK_FALSE( singleStatus->IsEnabled() );
    CHECK( activity->IsRunning() );

    CHECK( infoBar->GetButtonCount() == 1 );
    CHECK( infoBar->GetButtonId(0) == wxID_CLOSE );
    CHECK( infoBar->IsCheckBoxChecked() );
    CHECK( infoBar->GetEffectDuration() == 250 );
    CHECK( infoBar->GetShowEffect() == wxSHOW_EFFECT_EXPAND );
    CHECK( infoBar->GetHideEffect() ==
           wxSHOW_EFFECT_SLIDE_TO_RIGHT );

    CHECK( staticText->GetLabel() == "Static feedback" );
    CHECK( staticBitmap->GetBitmap().IsOk() );
    CHECK( staticBox->GetLabel() == "Static group" );

    wxWeakRef<wxFrame> frameAlive(frame);
    wxWeakRef<wxHyperlinkCtrl> linkAlive(link);
    wxWeakRef<wxGenericHyperlinkCtrl> genericLinkAlive(genericLink);
    wxWeakRef<wxStatusBar> singleStatusAlive(singleStatus);
    frame->Destroy();
    wxYield();
    CHECK_FALSE( frameAlive );
    CHECK_FALSE( linkAlive );
    CHECK_FALSE( genericLinkAlive );
    CHECK_FALSE( singleStatusAlive );
    CHECK( unloadGuard.Unload() );
}

#endif // all WinUI 008f XRC controls

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_INFOBAR && \
    wxUSE_NOTEBOOK && wxUSE_TREECTRL && wxUSE_BOOKCTRL && \
    wxUSE_CHOICEBOOK && wxUSE_LISTBOOK && wxUSE_TREEBOOK

TEST_CASE_METHOD(XrcTestCase, "XRC::WinUI V0 supported inventory",
                 "[winui-v0-supported][xrc]")
{
    static const char supportedXrcFile[] = "winui-v0-supported.xrc";
    static const char supportedXrc[] = R"XRC(<?xml version="1.0" ?>
<resource>
  <object class="wxPanel" name="supportedPanel">
    <object class="wxBoxSizer">
      <orient>wxVERTICAL</orient>
      <object class="sizeritem">
        <object class="wxInfoBar" name="supportedInfoBar">
          <checkboxlabel>Remember supported choice</checkboxlabel>
          <checked>1</checked>
          <showeffect>wxSHOW_EFFECT_EXPAND</showeffect>
          <hideeffect>wxSHOW_EFFECT_SLIDE_TO_RIGHT</hideeffect>
          <effectduration>125</effectduration>
          <object class="button" name="supportedInfoAction">
            <label>Details</label>
          </object>
        </object>
      </object>
      <object class="sizeritem">
        <object class="wxNotebook" name="supportedNotebook">
          <style>wxNB_TOP</style>
          <object class="notebookpage">
            <label>Notebook first</label>
            <object class="wxPanel" name="notebookPageFirst"/>
          </object>
          <object class="notebookpage">
            <label>Notebook selected</label>
            <selected>1</selected>
            <object class="wxPanel" name="notebookPageSelected"/>
          </object>
        </object>
      </object>
      <object class="sizeritem">
        <object class="wxTreeCtrl" name="supportedTree">
          <style>wxTR_HAS_BUTTONS|wxTR_EDIT_LABELS</style>
        </object>
      </object>
      <object class="sizeritem">
        <object class="wxSimplebook" name="supportedSimpleBook">
          <object class="simplebookpage">
            <label>Simple first</label>
            <object class="wxPanel" name="simplePageFirst"/>
          </object>
          <object class="simplebookpage">
            <label>Simple selected</label>
            <selected>1</selected>
            <object class="wxPanel" name="simplePageSelected"/>
          </object>
        </object>
      </object>
      <object class="sizeritem">
        <object class="wxChoicebook" name="supportedChoiceBook">
          <object class="choicebookpage">
            <label>Choice first</label>
            <object class="wxPanel" name="choicePageFirst"/>
          </object>
          <object class="choicebookpage">
            <label>Choice selected</label>
            <selected>1</selected>
            <object class="wxPanel" name="choicePageSelected"/>
          </object>
        </object>
      </object>
      <object class="sizeritem">
        <object class="wxListbook" name="supportedListBook">
          <object class="listbookpage">
            <label>List first</label>
            <object class="wxPanel" name="listPageFirst"/>
          </object>
          <object class="listbookpage">
            <label>List selected</label>
            <selected>1</selected>
            <object class="wxPanel" name="listPageSelected"/>
          </object>
        </object>
      </object>
      <object class="sizeritem">
        <object class="wxTreebook" name="supportedTreeBook">
          <object class="treebookpage">
            <label>Tree parent</label>
            <depth>0</depth>
            <object class="wxPanel" name="treePageParent"/>
          </object>
          <object class="treebookpage">
            <label>Tree selected child</label>
            <depth>1</depth>
            <selected>1</selected>
            <object class="wxPanel" name="treePageSelectedChild"/>
          </object>
          <object class="treebookpage">
            <label>Tree sibling root</label>
            <depth>0</depth>
            <object class="wxPanel" name="treePageSiblingRoot"/>
          </object>
        </object>
      </object>
    </object>
  </object>
</resource>
)XRC";

    struct LifetimeSnapshot
    {
        unsigned hosts;
        unsigned slots;
        unsigned slotStates;
        unsigned hostStates;
        unsigned subclassContexts;
        unsigned loadedHooks;
        unsigned slotHandlerAdds;
        unsigned slotHandlerRevokes;
        unsigned rootHandlerAdds;
        unsigned rootHandlerRevokes;
        unsigned flushAttempts;
        std::size_t notebookRetirements;
        std::size_t notebookCallbacks;
        std::size_t frameworkRetirements;
        std::size_t treeCallbacks;
    };

    const auto captureLifetime = []()
    {
        return LifetimeSnapshot
        {
            wxWinUITopLevelHost::GetLiveHostCount(),
            wxWinUITopLevelHost::GetLiveSlotCount(),
            wxWinUITopLevelHost::GetLiveSlotLifetimeCount(),
            wxWinUITopLevelHost::GetLiveHostLifetimeCount(),
            wxWinUITopLevelHost::GetLiveSubclassContextCount(),
            wxWinUITopLevelHost::GetLiveLoadedHookCount(),
            wxWinUITopLevelHost::GetSlotHandlerAddCount(),
            wxWinUITopLevelHost::GetSlotHandlerRevokeCount(),
            wxWinUITopLevelHost::GetRootHandlerAddCount(),
            wxWinUITopLevelHost::GetRootHandlerRevokeCount(),
            wxWinUITopLevelHost::GetFlushCallbackAttemptCount(),
            wxNotebook::WinUIGetPendingPeerRetirementCountForTesting(),
            wxNotebook::WinUIGetLiveCallbackStateCountForTesting(),
            wxNotebook::WinUIGetFrameworkRetirementCountForTesting(),
            wxTreeCtrl::WinUIGetLiveCallbackStateCountForTesting()
        };
    };

    const auto drainDispatch = []()
    {
        // Slot publication and host teardown are coalesced with CallAfter.
        // Notebook peer retirement has its own causal wait below.
        for ( int n = 0; n < 20; ++n )
        {
            wxYield();
            wxMilliSleep(2);
        }
    };

    wxXmlResource* const xrc = wxXmlResource::Get();
    xrc->InitAllHandlers();
    wxStringInputStream stream(wxString::FromAscii(supportedXrc));
    std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
    REQUIRE( document->IsOk() );
    REQUIRE( xrc->LoadDocument(document.release(), supportedXrcFile) );
    XrcDocumentUnloadGuard unloadGuard(xrc, supportedXrcFile);

    drainDispatch();
    const std::size_t pendingBeforeBaseline =
        wxNotebook::WinUIGetPendingPeerRetirementCountForTesting();
    const std::size_t callbacksBeforeBaseline =
        wxNotebook::WinUIGetLiveCallbackStateCountForTesting();
    const std::size_t frameworkBeforeBaseline =
        wxNotebook::WinUIGetFrameworkRetirementCountForTesting();
    REQUIRE(callbacksBeforeBaseline >= pendingBeforeBaseline);
    REQUIRE(frameworkBeforeBaseline >= pendingBeforeBaseline);
    REQUIRE(WaitFor(
        "supported XRC baseline notebook retirements",
        [callbacksBeforeBaseline,
         frameworkBeforeBaseline,
         pendingBeforeBaseline]()
        {
            return
                wxNotebook::
                    WinUIGetPendingPeerRetirementCountForTesting() == 0 &&
                wxNotebook::WinUIGetLiveCallbackStateCountForTesting() ==
                    callbacksBeforeBaseline - pendingBeforeBaseline &&
                wxNotebook::WinUIGetFrameworkRetirementCountForTesting() ==
                    frameworkBeforeBaseline - pendingBeforeBaseline;
        },
        2000));
    const LifetimeSnapshot baseline = captureLifetime();

    for ( int cycle = 0; cycle < 2; ++cycle )
    {
        INFO("supported XRC lifecycle cycle " << cycle);
        std::vector<wxWeakRef<wxWindow>> lifetimes;

        {
            // Every handler in the Supported inventory is exercised through
            // XRC_MAKE_INSTANCE plus Create(). A dedicated hidden TLW gives
            // this resource exactly one causally owned island without relying
            // on test order or on the shared test frame having made a slot.
            // No Experimental control is part of this XML.
            wxFrame frame(
                nullptr, wxID_ANY, "WinUI Supported XRC gate",
                wxPoint(-30000, -30000), wxSize(640, 720));
            wxPanel panel;
            REQUIRE( xrc->LoadPanel(&panel, &frame, "supportedPanel") );

            wxInfoBar* const infoBar =
                XRCCTRL(panel, "supportedInfoBar", wxInfoBar);
            wxNotebook* const notebook =
                XRCCTRL(panel, "supportedNotebook", wxNotebook);
            wxTreeCtrl* const treeCtrl =
                XRCCTRL(panel, "supportedTree", wxTreeCtrl);
            wxSimplebook* const simple =
                XRCCTRL(panel, "supportedSimpleBook", wxSimplebook);
            wxChoicebook* const choice =
                XRCCTRL(panel, "supportedChoiceBook", wxChoicebook);
            wxListbook* const list =
                XRCCTRL(panel, "supportedListBook", wxListbook);
            wxTreebook* const treeBook =
                XRCCTRL(panel, "supportedTreeBook", wxTreebook);
            REQUIRE( infoBar );
            REQUIRE( notebook );
            REQUIRE( treeCtrl );
            REQUIRE( simple );
            REQUIRE( choice );
            REQUIRE( list );
            REQUIRE( treeBook );

            // Slot creation above is the causal publication boundary for the
            // dedicated TLW host. Capture it now, never before the resource.
            wxWinUITopLevelHost* const expectedHost =
                wxWinUITopLevelHost::FindSlotOwner(infoBar);
            REQUIRE( expectedHost );
            CHECK( wxWinUITopLevelHost::FindForTLW(&frame) == expectedHost );
            CHECK( wxWinUITopLevelHost::GetLiveHostCount() ==
                   baseline.hosts + 1 );

            CHECK( infoBar->GetId() == XRCID("supportedInfoBar") );
            CHECK( notebook->GetId() == XRCID("supportedNotebook") );
            CHECK( treeCtrl->GetId() == XRCID("supportedTree") );
            CHECK( simple->GetId() == XRCID("supportedSimpleBook") );
            CHECK( choice->GetId() == XRCID("supportedChoiceBook") );
            CHECK( list->GetId() == XRCID("supportedListBook") );
            CHECK( treeBook->GetId() == XRCID("supportedTreeBook") );

            CHECK( infoBar->HasFlag(wxINFOBAR_CHECKBOX) );
            CHECK( infoBar->IsCheckBoxChecked() );
            CHECK( infoBar->GetButtonCount() == 1 );
            CHECK( infoBar->GetButtonId(0) ==
                   XRCID("supportedInfoAction") );
            CHECK( infoBar->GetShowEffect() == wxSHOW_EFFECT_EXPAND );
            CHECK( infoBar->GetHideEffect() ==
                   wxSHOW_EFFECT_SLIDE_TO_RIGHT );
            CHECK( infoBar->GetEffectDuration() == 125 );

            wxPanel* const notebookFirst =
                XRCCTRL(panel, "notebookPageFirst", wxPanel);
            wxPanel* const notebookSelected =
                XRCCTRL(panel, "notebookPageSelected", wxPanel);
            REQUIRE( notebookFirst );
            REQUIRE( notebookSelected );
            CHECK( notebook->HasFlag(wxNB_TOP) );
            CHECK( (notebook->GetWindowStyleFlag() &
                    (wxNB_BOTTOM | wxNB_LEFT | wxNB_RIGHT |
                     wxNB_MULTILINE | wxNB_NOPAGETHEME)) == 0 );
            CHECK( notebook->GetPageCount() == 2 );
            CHECK( notebook->GetSelection() == 1 );
            CHECK( notebook->GetCurrentPage() == notebookSelected );
            CHECK( notebook->GetPage(0) == notebookFirst );
            CHECK( notebook->GetPageText(0) == "Notebook first" );
            CHECK( notebook->GetPageText(1) == "Notebook selected" );

            const long unsupportedTreeStyles =
                wxTR_LINES_AT_ROOT | wxTR_ROW_LINES |
                wxTR_FULL_ROW_HIGHLIGHT | wxTR_TWIST_BUTTONS;
            CHECK( treeCtrl->HasFlag(wxTR_HAS_BUTTONS) );
            CHECK( treeCtrl->HasFlag(wxTR_EDIT_LABELS) );
            CHECK( (treeCtrl->GetWindowStyleFlag() &
                    unsupportedTreeStyles) == 0 );
            const wxTreeItemId root = treeCtrl->AddRoot("root");
            REQUIRE( root.IsOk() );
            const wxTreeItemId child = treeCtrl->AppendItem(root, "child");
            REQUIRE( child.IsOk() );
            treeCtrl->Expand(root);
            treeCtrl->SelectItem(child);
            CHECK( treeCtrl->GetSelection() == child );
            CHECK( treeCtrl->GetCount() == 2 );

            wxPanel* const simpleFirst =
                XRCCTRL(panel, "simplePageFirst", wxPanel);
            wxPanel* const simpleSelected =
                XRCCTRL(panel, "simplePageSelected", wxPanel);
            wxPanel* const choiceFirst =
                XRCCTRL(panel, "choicePageFirst", wxPanel);
            wxPanel* const choiceSelected =
                XRCCTRL(panel, "choicePageSelected", wxPanel);
            wxPanel* const listFirst =
                XRCCTRL(panel, "listPageFirst", wxPanel);
            wxPanel* const listSelected =
                XRCCTRL(panel, "listPageSelected", wxPanel);
            wxPanel* const treeParent =
                XRCCTRL(panel, "treePageParent", wxPanel);
            wxPanel* const treeSelectedChild =
                XRCCTRL(panel, "treePageSelectedChild", wxPanel);
            wxPanel* const treeSiblingRoot =
                XRCCTRL(panel, "treePageSiblingRoot", wxPanel);
            REQUIRE( simpleFirst );
            REQUIRE( simpleSelected );
            REQUIRE( choiceFirst );
            REQUIRE( choiceSelected );
            REQUIRE( listFirst );
            REQUIRE( listSelected );
            REQUIRE( treeParent );
            REQUIRE( treeSelectedChild );
            REQUIRE( treeSiblingRoot );

            const auto checkTwoPageBook =
                [](wxBookCtrlBase* const book,
                   wxWindow* const first,
                   wxWindow* const selected,
                   const wxString& firstLabel,
                   const wxString& selectedLabel)
                {
                    CHECK( book->GetPageCount() == 2 );
                    CHECK( book->GetSelection() == 1 );
                    CHECK( book->GetCurrentPage() == selected );
                    CHECK( book->GetPage(0) == first );
                    CHECK( book->GetPage(1) == selected );
                    CHECK( book->GetPageText(0) == firstLabel );
                    CHECK( book->GetPageText(1) == selectedLabel );
                    CHECK( first->GetParent() == book );
                    CHECK( selected->GetParent() == book );
                };

            checkTwoPageBook(simple, simpleFirst, simpleSelected,
                             "Simple first", "Simple selected");
            checkTwoPageBook(choice, choiceFirst, choiceSelected,
                             "Choice first", "Choice selected");
            checkTwoPageBook(list, listFirst, listSelected,
                             "List first", "List selected");
            CHECK( treeBook->GetPageCount() == 3 );
            CHECK( treeBook->GetSelection() == 1 );
            CHECK( treeBook->GetCurrentPage() == treeSelectedChild );
            CHECK( treeBook->GetPage(0) == treeParent );
            CHECK( treeBook->GetPage(1) == treeSelectedChild );
            CHECK( treeBook->GetPage(2) == treeSiblingRoot );
            CHECK( treeBook->GetPageText(0) == "Tree parent" );
            CHECK( treeBook->GetPageText(1) == "Tree selected child" );
            CHECK( treeBook->GetPageText(2) == "Tree sibling root" );
            CHECK( treeBook->GetPageParent(0) == wxNOT_FOUND );
            CHECK( treeBook->GetPageParent(1) == 0 );
            CHECK( treeBook->GetPageParent(2) == wxNOT_FOUND );
            CHECK( treeParent->GetParent() == treeBook );
            CHECK( treeSelectedChild->GetParent() == treeBook );
            CHECK( treeSiblingRoot->GetParent() == treeBook );

            wxChoice* const choiceController = choice->GetChoiceCtrl();
            wxListView* const listController = list->GetListView();
            wxTreeCtrl* const treeBookController = treeBook->GetTreeCtrl();
            REQUIRE( choiceController );
            REQUIRE( listController );
            REQUIRE( treeBookController );

            // Named IDs from the resource must be usable directly as binding
            // filters for native controls as well as common/composite books.
            int infoEvents = 0;
            infoBar->Bind(
                wxEVT_BUTTON,
                [&](wxCommandEvent& event)
                {
                    CHECK( event.GetEventObject() == infoBar );
                    ++infoEvents;
                },
                XRCID("supportedInfoAction"));
            wxCommandEvent infoEvent(
                wxEVT_BUTTON, XRCID("supportedInfoAction"));
            infoEvent.SetEventObject(infoBar);
            REQUIRE( infoBar->ProcessWindowEvent(infoEvent) );
            CHECK( infoEvents == 1 );

            int notebookEvents = 0;
            notebook->Bind(
                wxEVT_NOTEBOOK_PAGE_CHANGED,
                [&](wxBookCtrlEvent& event)
                {
                    CHECK( event.GetEventObject() == notebook );
                    ++notebookEvents;
                },
                notebook->GetId());
            wxBookCtrlEvent notebookEvent(
                wxEVT_NOTEBOOK_PAGE_CHANGED, notebook->GetId(), 1, 0);
            notebookEvent.SetEventObject(notebook);
            REQUIRE( notebook->ProcessWindowEvent(notebookEvent) );
            CHECK( notebookEvents == 1 );

            int treeEvents = 0;
            treeCtrl->Bind(
                wxEVT_TREE_SEL_CHANGED,
                [&](wxTreeEvent& event)
                {
                    CHECK( event.GetEventObject() == treeCtrl );
                    CHECK( event.GetItem() == child );
                    ++treeEvents;
                },
                treeCtrl->GetId());
            wxTreeEvent treeEvent(wxEVT_TREE_SEL_CHANGED, treeCtrl, child);
            REQUIRE( treeCtrl->ProcessWindowEvent(treeEvent) );
            CHECK( treeEvents == 1 );

            int simpleEvents = 0;
            simple->Bind(
                wxEVT_BOOKCTRL_PAGE_CHANGED,
                [&](wxBookCtrlEvent&) { ++simpleEvents; },
                simple->GetId());
            wxBookCtrlEvent simpleEvent(
                wxEVT_BOOKCTRL_PAGE_CHANGED, simple->GetId(), 1, 0);
            simpleEvent.SetEventObject(simple);
            REQUIRE( simple->ProcessWindowEvent(simpleEvent) );
            CHECK( simpleEvents == 1 );

            int choiceEvents = 0;
            choice->Bind(
                wxEVT_CHOICEBOOK_PAGE_CHANGED,
                [&](wxBookCtrlEvent&) { ++choiceEvents; },
                choice->GetId());
            wxBookCtrlEvent choiceEvent(
                wxEVT_CHOICEBOOK_PAGE_CHANGED, choice->GetId(), 1, 0);
            choiceEvent.SetEventObject(choice);
            REQUIRE( choice->ProcessWindowEvent(choiceEvent) );
            CHECK( choiceEvents == 1 );

            int listEvents = 0;
            list->Bind(
                wxEVT_LISTBOOK_PAGE_CHANGED,
                [&](wxBookCtrlEvent&) { ++listEvents; },
                list->GetId());
            wxBookCtrlEvent listEvent(
                wxEVT_LISTBOOK_PAGE_CHANGED, list->GetId(), 1, 0);
            listEvent.SetEventObject(list);
            REQUIRE( list->ProcessWindowEvent(listEvent) );
            CHECK( listEvents == 1 );

            int treeBookEvents = 0;
            treeBook->Bind(
                wxEVT_TREEBOOK_PAGE_CHANGED,
                [&](wxBookCtrlEvent&) { ++treeBookEvents; },
                treeBook->GetId());
            wxBookCtrlEvent treeBookEvent(
                wxEVT_TREEBOOK_PAGE_CHANGED, treeBook->GetId(), 1, 0);
            treeBookEvent.SetEventObject(treeBook);
            REQUIRE( treeBook->ProcessWindowEvent(treeBookEvent) );
            CHECK( treeBookEvents == 1 );

            REQUIRE( wxWinUITopLevelHost::FindSlotOwner(infoBar) ==
                     expectedHost );
            REQUIRE( wxWinUITopLevelHost::FindSlotOwner(notebook) ==
                     expectedHost );
            REQUIRE( wxWinUITopLevelHost::FindSlotOwner(treeCtrl) ==
                     expectedHost );
            CHECK( wxWinUITopLevelHost::GetLiveSlotCount() >=
                   baseline.slots + 3 );

            // Composite books may introduce native controller children, but
            // every slot in the resource must remain in the dedicated host.
            std::vector<wxWindow*> pending{&panel};
            while ( !pending.empty() )
            {
                wxWindow* const window = pending.back();
                pending.pop_back();
                wxWinUITopLevelHost* const owner =
                    wxWinUITopLevelHost::FindSlotOwner(window);
                if ( owner )
                    CHECK( owner == expectedHost );

                for ( wxWindow* const childWindow : window->GetChildren() )
                    pending.push_back(childWindow);
            }

            lifetimes =
            {
                &frame, &panel, infoBar, notebook, treeCtrl,
                simple, choice, list, treeBook,
                notebookFirst, notebookSelected,
                simpleFirst, simpleSelected,
                choiceFirst, choiceSelected,
                listFirst, listSelected,
                treeParent, treeSelectedChild, treeSiblingRoot,
                choiceController, listController, treeBookController
            };
        }

        drainDispatch();
        for ( const auto& lifetime : lifetimes )
            CHECK_FALSE( lifetime );

        // A notebook deliberately keeps its invalidated callback state and
        // peer graph alive for two low-priority DispatcherQueue turns. A
        // fixed number of wxYield() calls is not a barrier for those turns,
        // so wait for the complete causal retirement triple instead.
        REQUIRE(WaitFor(
            "supported XRC notebook retirement",
            [&baseline]()
            {
                return
                    wxNotebook::
                        WinUIGetPendingPeerRetirementCountForTesting() ==
                            baseline.notebookRetirements &&
                    wxNotebook::WinUIGetLiveCallbackStateCountForTesting() ==
                        baseline.notebookCallbacks &&
                    wxNotebook::
                        WinUIGetFrameworkRetirementCountForTesting() ==
                            baseline.frameworkRetirements;
            },
            2000));

        const LifetimeSnapshot afterDestroy = captureLifetime();
        CHECK( afterDestroy.hosts == baseline.hosts );
        CHECK( afterDestroy.slots == baseline.slots );
        CHECK( afterDestroy.slotStates == baseline.slotStates );
        CHECK( afterDestroy.hostStates == baseline.hostStates );
        CHECK( afterDestroy.subclassContexts == baseline.subclassContexts );
        CHECK( afterDestroy.loadedHooks == baseline.loadedHooks );
        CHECK( afterDestroy.notebookCallbacks == baseline.notebookCallbacks );
        CHECK( afterDestroy.treeCallbacks == baseline.treeCallbacks );
        CHECK( afterDestroy.slotHandlerAdds - baseline.slotHandlerAdds ==
               afterDestroy.slotHandlerRevokes -
                   baseline.slotHandlerRevokes );
        CHECK( afterDestroy.rootHandlerAdds - baseline.rootHandlerAdds ==
               afterDestroy.rootHandlerRevokes -
                   baseline.rootHandlerRevokes );

        const unsigned stableFlushAttempts = afterDestroy.flushAttempts;
        drainDispatch();
        CHECK( wxWinUITopLevelHost::GetFlushCallbackAttemptCount() ==
               stableFlushAttempts );
    }

    CHECK( unloadGuard.Unload() );
}

#endif // WinUI V0 Supported XRC inventory

#if defined(__WXWINUI__) && wxUSE_WINUI3 && wxUSE_BOOKCTRL && wxUSE_TOOLBOOK

TEST_CASE_METHOD(XrcTestCase, "XRC::WinUI Toolbook default construction",
                 "[xrc][winui-books][winui-toolbook]")
{
    static const char toolbookXrcFile[] = "winui-toolbook.xrc";
    static const char toolbookXrc[] = R"XRC(<?xml version="1.0" ?>
<resource>
  <object class="wxDialog" name="toolbookDialog">
    <title>WinUI Toolbook XRC smoke</title>
    <object class="wxBoxSizer">
      <orient>wxVERTICAL</orient>
      <object class="sizeritem">
        <object class="wxToolbook" name="toolBook">
          <object class="toolbookpage">
            <label>Tool first</label>
            <bitmap stock_id="wxART_INFORMATION"/>
            <object class="wxPanel" name="toolPageFirst"/>
          </object>
          <object class="toolbookpage">
            <label>Tool selected</label>
            <selected>1</selected>
            <bitmap stock_id="wxART_TICK_MARK"/>
            <object class="wxPanel" name="toolPageSelected"/>
          </object>
        </object>
      </object>
    </object>
  </object>
</resource>
)XRC";

    wxXmlResource* const xrc = wxXmlResource::Get();
    xrc->InitAllHandlers();
    wxStringInputStream stream(wxString::FromAscii(toolbookXrc));
    std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
    REQUIRE( document->IsOk() );
    REQUIRE( xrc->LoadDocument(document.release(), toolbookXrcFile) );
    XrcDocumentUnloadGuard unloadGuard(xrc, toolbookXrcFile);

    wxWeakRef<wxWindow> toolLifetime;
    wxWeakRef<wxWindow> firstLifetime;
    wxWeakRef<wxWindow> selectedLifetime;
    {
        wxDialog dialog;
        REQUIRE( xrc->LoadDialog(&dialog, nullptr, "toolbookDialog") );

        wxToolbook* const tool =
            XRCCTRL(dialog, "toolBook", wxToolbook);
        wxPanel* const first =
            XRCCTRL(dialog, "toolPageFirst", wxPanel);
        wxPanel* const selected =
            XRCCTRL(dialog, "toolPageSelected", wxPanel);
        REQUIRE( tool );
        REQUIRE( first );
        REQUIRE( selected );
        REQUIRE( tool->GetToolBar() );
        CHECK( tool->GetPageCount() == 2 );
        CHECK( tool->GetSelection() == 1 );
        CHECK( tool->GetCurrentPage() == selected );
        CHECK( tool->GetPage(0) == first );
        CHECK( tool->GetPage(1) == selected );
        CHECK( tool->GetPageText(0) == "Tool first" );
        CHECK( tool->GetPageText(1) == "Tool selected" );
        CHECK( first->GetParent() == tool );
        CHECK( selected->GetParent() == tool );

        toolLifetime = tool;
        firstLifetime = first;
        selectedLifetime = selected;
    }

    CHECK_FALSE( toolLifetime );
    CHECK_FALSE( firstLifetime );
    CHECK_FALSE( selectedLifetime );
    CHECK( unloadGuard.Unload() );
}

#endif // WinUI Toolbook

#if defined(__WXWINUI__) && wxUSE_WINUI3 && \
    wxUSE_BUTTON && wxUSE_CHECKBOX && \
    wxUSE_TOGGLEBTN && wxUSE_RADIOBTN && wxUSE_RADIOBOX && \
    wxUSE_SEARCHCTRL && wxUSE_TEXTCTRL && \
    wxUSE_CHOICE && wxUSE_LISTBOX && \
    wxUSE_SLIDER && wxUSE_DATEPICKCTRL && \
    wxUSE_STATTEXT && wxUSE_INFOBAR && \
    wxUSE_NOTEBOOK && wxUSE_TREECTRL && wxUSE_SPLITTER && \
    wxUSE_TOOLBAR && wxUSE_STATUSBAR && \
    wxUSE_MENUS && wxUSE_MENUBAR && wxUSE_ACCEL && wxUSE_TOOLTIPS

TEST_CASE_METHOD(XrcTestCase, "XRC::WinUI V0 mixed supported resource",
                 "[xrc][winui-v0-mixed]")
{
    static const char mixedXrcFile[] = "winui-v0-mixed-supported.xrc";
    static const char mixedXrc[] = R"XRC(<?xml version="1.0" ?>
<resource>
  <object class="wxFrame" name="v0MixedFrame">
    <title>WinUI V0 mixed XRC gate</title>
    <size>800,700</size>
    <object class="wxMenuBar" name="v0MenuBar">
      <object class="wxMenu" name="v0Menu">
        <label>&amp;File</label>
        <object class="wxMenuItem" name="v0MenuAction">
          <label>&amp;Run mixed action</label>
          <help>Exercise the mixed XRC command route</help>
          <accel>Ctrl-M</accel>
        </object>
      </object>
    </object>
    <object class="wxToolBar" name="v0ToolBar">
      <style>wxTB_HORIZONTAL|wxTB_TEXT</style>
      <object class="tool" name="v0ToolAction">
        <label>&amp;Run</label>
        <tooltip>Run mixed action</tooltip>
        <bitmap stock_id="wxART_INFORMATION"/>
      </object>
    </object>
    <object class="wxStatusBar" name="v0StatusBar">
      <fields>2</fields>
      <widths>-1,120</widths>
      <styles>wxSB_NORMAL,wxSB_FLAT</styles>
    </object>
    <object class="wxPanel" name="v0RootPanel">
      <object class="wxBoxSizer">
        <orient>wxVERTICAL</orient>
        <object class="sizeritem">
          <object class="wxScrolledWindow" name="v0Scrolled">
            <style>wxHSCROLL|wxVSCROLL|wxTAB_TRAVERSAL</style>
            <scrollrate>4,5</scrollrate>
            <object class="wxBoxSizer">
              <orient>wxVERTICAL</orient>
              <object class="sizeritem">
                <object class="wxButton" name="v0ActionButton">
                  <label>&amp;Run mixed action</label>
                  <tooltip>Mixed V0 tooltip</tooltip>
                </object>
              </object>
              <object class="sizeritem">
                <object class="wxCheckBox" name="v0Check">
                  <label>Enable mixed mode</label>
                  <checked>1</checked>
                </object>
              </object>
              <object class="sizeritem">
                <object class="wxToggleButton" name="v0Toggle">
                  <label>Pin mixed mode</label>
                  <checked>1</checked>
                </object>
              </object>
              <object class="sizeritem">
                <object class="wxRadioButton" name="v0Radio">
                  <label>Mixed radio action</label>
                  <style>wxRB_SINGLE</style>
                  <value>1</value>
                </object>
              </object>
              <object class="sizeritem">
                <object class="wxRadioBox" name="v0RadioBox">
                  <label>Mixed choices</label>
                  <style>wxRA_SPECIFY_COLS</style>
                  <dimension>2</dimension>
                  <selection>1</selection>
                  <content>
                    <item>first</item>
                    <item>second</item>
                  </content>
                </object>
              </object>
              <object class="sizeritem">
                <object class="wxTextCtrl" name="v0Text">
                  <value>mixed text</value>
                  <hint>mixed text hint</hint>
                  <style>wxTE_PROCESS_ENTER</style>
                </object>
              </object>
              <object class="sizeritem">
                <object class="wxSearchCtrl" name="v0Search">
                  <value>mixed search</value>
                  <hint>mixed search hint</hint>
                </object>
              </object>
              <object class="sizeritem">
                <object class="wxChoice" name="v0Choice">
                  <selection>1</selection>
                  <content>
                    <item>alpha</item>
                    <item>beta</item>
                  </content>
                </object>
              </object>
              <object class="sizeritem">
                <object class="wxListBox" name="v0List">
                  <style>wxLB_SINGLE</style>
                  <content>
                    <item>red</item>
                    <item>green</item>
                  </content>
                </object>
              </object>
              <object class="sizeritem">
                <object class="wxSlider" name="v0Slider">
                  <value>35</value>
                  <min>10</min>
                  <max>90</max>
                  <linesize>5</linesize>
                  <pagesize>20</pagesize>
                  <style>wxSL_HORIZONTAL|wxSL_AUTOTICKS</style>
                </object>
              </object>
              <object class="sizeritem">
                <object class="wxDatePickerCtrl" name="v0Date">
                  <style>wxDP_DROPDOWN|wxDP_SHOWCENTURY|wxDP_ALLOWNONE</style>
                  <null-text>No mixed date</null-text>
                </object>
              </object>
              <object class="sizeritem">
                <object class="wxStaticText" name="v0StaticText">
                  <label>Mixed static feedback</label>
                </object>
              </object>
              <object class="sizeritem">
                <object class="wxInfoBar" name="v0InfoBar">
                  <checkboxlabel>Remember mixed choice</checkboxlabel>
                  <checked>1</checked>
                  <effectduration>125</effectduration>
                  <object class="button" name="v0InfoAction">
                    <label>Details</label>
                  </object>
                </object>
              </object>
            </object>
          </object>
          <option>1</option>
          <flag>wxEXPAND</flag>
        </object>
        <object class="sizeritem">
          <object class="wxSplitterWindow" name="v0Splitter">
            <orientation>vertical</orientation>
            <sashpos>360</sashpos>
            <minsize>80</minsize>
            <style>wxSP_LIVE_UPDATE|wxSP_3DSASH</style>
            <object class="wxPanel" name="v0BookPane">
              <object class="wxNotebook" name="v0Notebook">
                <style>wxNB_TOP</style>
                <object class="notebookpage">
                  <label>First</label>
                  <selected>1</selected>
                  <object class="wxPanel" name="v0NotebookPageOne"/>
                </object>
                <object class="notebookpage">
                  <label>Second</label>
                  <object class="wxPanel" name="v0NotebookPageTwo"/>
                </object>
              </object>
            </object>
            <object class="wxPanel" name="v0TreePane">
              <object class="wxTreeCtrl" name="v0Tree">
                <style>wxTR_HAS_BUTTONS|wxTR_LINES_AT_ROOT|wxTR_SINGLE</style>
              </object>
            </object>
          </object>
          <option>1</option>
          <flag>wxEXPAND</flag>
        </object>
      </object>
    </object>
  </object>
</resource>
)XRC";

    struct LifetimeSnapshot
    {
        unsigned hosts;
        unsigned slots;
        unsigned slotStates;
        unsigned hostStates;
        unsigned subclassContexts;
        unsigned loadedHooks;
        unsigned slotHandlerAdds;
        unsigned slotHandlerRevokes;
        unsigned rootHandlerAdds;
        unsigned rootHandlerRevokes;
        unsigned flushAttempts;
        std::size_t textCallbacks;
        std::size_t notebookRetirements;
        std::size_t notebookCallbacks;
        std::size_t frameworkRetirements;
        std::size_t treeCallbacks;
        std::size_t toolbarCallbacks;
    };

    const auto captureLifetime = []()
    {
        return LifetimeSnapshot
        {
            wxWinUITopLevelHost::GetLiveHostCount(),
            wxWinUITopLevelHost::GetLiveSlotCount(),
            wxWinUITopLevelHost::GetLiveSlotLifetimeCount(),
            wxWinUITopLevelHost::GetLiveHostLifetimeCount(),
            wxWinUITopLevelHost::GetLiveSubclassContextCount(),
            wxWinUITopLevelHost::GetLiveLoadedHookCount(),
            wxWinUITopLevelHost::GetSlotHandlerAddCount(),
            wxWinUITopLevelHost::GetSlotHandlerRevokeCount(),
            wxWinUITopLevelHost::GetRootHandlerAddCount(),
            wxWinUITopLevelHost::GetRootHandlerRevokeCount(),
            wxWinUITopLevelHost::GetFlushCallbackAttemptCount(),
            wxWinUITextCallbackState::GetLiveCountForTesting(),
            wxNotebook::WinUIGetPendingPeerRetirementCountForTesting(),
            wxNotebook::WinUIGetLiveCallbackStateCountForTesting(),
            wxNotebook::WinUIGetFrameworkRetirementCountForTesting(),
            wxTreeCtrl::WinUIGetLiveCallbackStateCountForTesting(),
            wxToolBar::WinUIGetLiveCallbackStateCountForTesting()
        };
    };

    const auto drainDispatch = []()
    {
        // Slots use CallAfter for coalesced publication and host teardown.
        // Notebook peer retirement has its own causal wait below.
        for ( int n = 0; n < 20; ++n )
        {
            wxYield();
            wxMilliSleep(2);
        }
    };

    wxXmlResource* const xrc = wxXmlResource::Get();
    xrc->InitAllHandlers();
    wxStringInputStream stream(wxString::FromAscii(mixedXrc));
    std::unique_ptr<wxXmlDocument> document(new wxXmlDocument(stream));
    REQUIRE( document->IsOk() );
    REQUIRE( xrc->LoadDocument(document.release(), mixedXrcFile) );
    XrcDocumentUnloadGuard unloadGuard(xrc, mixedXrcFile);

    drainDispatch();
    const std::size_t pendingBeforeBaseline =
        wxNotebook::WinUIGetPendingPeerRetirementCountForTesting();
    const std::size_t callbacksBeforeBaseline =
        wxNotebook::WinUIGetLiveCallbackStateCountForTesting();
    const std::size_t frameworkBeforeBaseline =
        wxNotebook::WinUIGetFrameworkRetirementCountForTesting();
    REQUIRE(callbacksBeforeBaseline >= pendingBeforeBaseline);
    REQUIRE(frameworkBeforeBaseline >= pendingBeforeBaseline);
    REQUIRE(WaitFor(
        "mixed XRC baseline notebook retirements",
        [callbacksBeforeBaseline,
         frameworkBeforeBaseline,
         pendingBeforeBaseline]()
        {
            return
                wxNotebook::
                    WinUIGetPendingPeerRetirementCountForTesting() == 0 &&
                wxNotebook::WinUIGetLiveCallbackStateCountForTesting() ==
                    callbacksBeforeBaseline - pendingBeforeBaseline &&
                wxNotebook::WinUIGetFrameworkRetirementCountForTesting() ==
                    frameworkBeforeBaseline - pendingBeforeBaseline;
        },
        2000));
    const LifetimeSnapshot baseline = captureLifetime();

    for ( int cycle = 0; cycle < 2; ++cycle )
    {
        INFO("mixed XRC lifecycle cycle " << cycle);

        wxFrame* const frame = xrc->LoadFrame(nullptr, "v0MixedFrame");
        REQUIRE( frame );
        CHECK_FALSE( frame->IsShown() );
        CHECK( frame->GetTitle() == "WinUI V0 mixed XRC gate" );

        wxPanel* const rootPanel =
            XRCCTRL(*frame, "v0RootPanel", wxPanel);
        wxScrolledWindow* const scrolled =
            XRCCTRL(*frame, "v0Scrolled", wxScrolledWindow);
        wxSplitterWindow* const splitter =
            XRCCTRL(*frame, "v0Splitter", wxSplitterWindow);
        wxPanel* const bookPane =
            XRCCTRL(*frame, "v0BookPane", wxPanel);
        wxPanel* const treePane =
            XRCCTRL(*frame, "v0TreePane", wxPanel);
        wxButton* const button =
            XRCCTRL(*frame, "v0ActionButton", wxButton);
        wxCheckBox* const check =
            XRCCTRL(*frame, "v0Check", wxCheckBox);
        wxToggleButton* const toggle =
            XRCCTRL(*frame, "v0Toggle", wxToggleButton);
        wxRadioButton* const radio =
            XRCCTRL(*frame, "v0Radio", wxRadioButton);
        wxRadioBox* const radioBox =
            XRCCTRL(*frame, "v0RadioBox", wxRadioBox);
        wxTextCtrl* const text =
            XRCCTRL(*frame, "v0Text", wxTextCtrl);
        wxSearchCtrl* const search =
            XRCCTRL(*frame, "v0Search", wxSearchCtrl);
        wxChoice* const choice =
            XRCCTRL(*frame, "v0Choice", wxChoice);
        wxListBox* const list =
            XRCCTRL(*frame, "v0List", wxListBox);
        wxSlider* const slider =
            XRCCTRL(*frame, "v0Slider", wxSlider);
        wxDatePickerCtrl* const date =
            XRCCTRL(*frame, "v0Date", wxDatePickerCtrl);
        wxStaticText* const staticText =
            XRCCTRL(*frame, "v0StaticText", wxStaticText);
        wxInfoBar* const infoBar =
            XRCCTRL(*frame, "v0InfoBar", wxInfoBar);
        wxNotebook* const notebook =
            XRCCTRL(*frame, "v0Notebook", wxNotebook);
        wxTreeCtrl* const tree =
            XRCCTRL(*frame, "v0Tree", wxTreeCtrl);
        wxToolBar* const toolbar =
            XRCCTRL(*frame, "v0ToolBar", wxToolBar);
        wxStatusBar* const status =
            XRCCTRL(*frame, "v0StatusBar", wxStatusBar);

        REQUIRE( rootPanel );
        REQUIRE( scrolled );
        REQUIRE( splitter );
        REQUIRE( bookPane );
        REQUIRE( treePane );
        REQUIRE( button );
        REQUIRE( check );
        REQUIRE( toggle );
        REQUIRE( radio );
        REQUIRE( radioBox );
        REQUIRE( text );
        REQUIRE( search );
        REQUIRE( choice );
        REQUIRE( list );
        REQUIRE( slider );
        REQUIRE( date );
        REQUIRE( staticText );
        REQUIRE( infoBar );
        REQUIRE( notebook );
        REQUIRE( tree );
        REQUIRE( toolbar );
        REQUIRE( status );

        CHECK( rootPanel->GetParent() == frame );
        int scrollX = 0;
        int scrollY = 0;
        scrolled->GetScrollPixelsPerUnit(&scrollX, &scrollY);
        CHECK( scrollX == 4 );
        CHECK( scrollY == 5 );
        CHECK( splitter->IsSplit() );
        CHECK( splitter->GetSplitMode() == wxSPLIT_VERTICAL );
        CHECK( splitter->GetWindow1() == bookPane );
        CHECK( splitter->GetWindow2() == treePane );

        CHECK( button->GetId() == XRCID("v0ActionButton") );
        CHECK( button->GetLabel() == "&Run mixed action" );
        REQUIRE( button->GetToolTip() );
        CHECK( button->GetToolTip()->GetTip() == "Mixed V0 tooltip" );
        CHECK( check->GetValue() );
        CHECK( toggle->GetValue() );
        CHECK( radio->GetValue() );
        CHECK( radioBox->GetCount() == 2 );
        CHECK( radioBox->GetSelection() == 1 );
        CHECK( radioBox->GetString(1) == "second" );
        CHECK( text->GetValue() == "mixed text" );
        CHECK( text->GetHint() == "mixed text hint" );
        CHECK( text->HasFlag(wxTE_PROCESS_ENTER) );
        CHECK( search->GetValue() == "mixed search" );
        CHECK( search->GetDescriptiveText() == "mixed search hint" );
        CHECK( choice->GetCount() == 2 );
        CHECK( choice->GetSelection() == 1 );
        CHECK( choice->GetString(1) == "beta" );
        CHECK( list->GetCount() == 2 );
        CHECK( list->GetString(0) == "red" );
        CHECK( slider->GetValue() == 35 );
        CHECK( slider->GetMin() == 10 );
        CHECK( slider->GetMax() == 90 );
        CHECK( slider->GetLineSize() == 5 );
        CHECK( slider->GetPageSize() == 20 );
        CHECK( date->HasFlag(wxDP_ALLOWNONE) );
        CHECK_FALSE( date->GetValue().IsValid() );
        CHECK( staticText->GetLabel() == "Mixed static feedback" );
        CHECK( infoBar->GetButtonCount() == 1 );
        CHECK( infoBar->GetButtonId(0) == XRCID("v0InfoAction") );
        CHECK( infoBar->IsCheckBoxChecked() );
        CHECK( infoBar->GetEffectDuration() == 125 );

        CHECK( notebook->GetPageCount() == 2 );
        CHECK( notebook->GetSelection() == 0 );
        CHECK( notebook->GetPageText(0) == "First" );
        CHECK( notebook->GetPageText(1) == "Second" );
        const wxTreeItemId root = tree->AddRoot("root");
        REQUIRE( root.IsOk() );
        REQUIRE( tree->AppendItem(root, "child").IsOk() );
        tree->Expand(root);
        CHECK( tree->GetCount() == 2 );

        CHECK( frame->GetToolBar() == toolbar );
        CHECK( toolbar->GetToolsCount() == 1 );
        CHECK( toolbar->GetToolShortHelp(XRCID("v0ToolAction")) ==
               "Run mixed action" );
        CHECK( frame->GetStatusBar() == status );
        CHECK( status->GetFieldsCount() == 2 );
        status->SetStatusText("mixed ready", 0);
        CHECK( status->GetStatusText(0) == "mixed ready" );

        wxMenuBar* const menuBar = frame->GetMenuBar();
        REQUIRE( menuBar );
        CHECK( menuBar->GetMenuCount() == 1 );
        CHECK( menuBar->GetMenuLabelText(0) == "File" );
        wxMenuItem* const menuItem =
            menuBar->FindItem(XRCID("v0MenuAction"));
        REQUIRE( menuItem );
        CHECK( menuItem->GetItemLabelText() == "Run mixed action" );
        CHECK( menuItem->GetHelp() ==
               "Exercise the mixed XRC command route" );
        std::unique_ptr<wxAcceleratorEntry> accelerator(menuItem->GetAccel());
        REQUIRE( accelerator );
        CHECK( (accelerator->GetFlags() & wxACCEL_CTRL) != 0 );
        CHECK( accelerator->GetKeyCode() == 'M' );

        int buttonEvents = 0;
        int menuEvents = 0;
        frame->Bind(
            wxEVT_BUTTON,
            [&](wxCommandEvent& event)
            {
                CHECK( event.GetId() == XRCID("v0ActionButton") );
                CHECK( event.GetEventObject() == button );
                ++buttonEvents;
            },
            XRCID("v0ActionButton"));
        frame->Bind(
            wxEVT_MENU,
            [&](wxCommandEvent& event)
            {
                CHECK( event.GetId() == XRCID("v0MenuAction") );
                ++menuEvents;
            },
            XRCID("v0MenuAction"));

        // Exercise wx routing synchronously. This deliberately avoids
        // SendInput, flyout activation and any interaction with the desktop.
        wxCommandEvent buttonEvent(
            wxEVT_BUTTON, XRCID("v0ActionButton"));
        buttonEvent.SetEventObject(button);
        button->Command(buttonEvent);
        CHECK( buttonEvents == 1 );

        wxCommandEvent menuEvent(
            wxEVT_MENU, XRCID("v0MenuAction"));
        menuEvent.SetEventObject(frame);
        CHECK( frame->ProcessWindowEvent(menuEvent) );
        CHECK( menuEvents == 1 );

        wxWinUITopLevelHost* const host =
            wxWinUITopLevelHost::FindForTLW(frame);
        REQUIRE( host );
        CHECK( wxWinUITopLevelHost::GetLiveHostCount() ==
               baseline.hosts + 1 );

        const std::vector<wxWindow*> slottedWindows
        {
            toolbar, status, button, check, toggle, radio, radioBox,
            text, search, choice, list, slider, date, staticText, infoBar,
            notebook, tree
        };
        for ( wxWindow* const window : slottedWindows )
        {
            INFO("slot owner for " << window->GetName());
            REQUIRE( wxWinUITopLevelHost::FindSlotOwner(window) == host );
            REQUIRE( host->FindSlot(window) );
        }
        // wxMenuBar is not itself a wxWindow, so the WinUI port owns one
        // private wxControl bridge under the frame. It is deliberately a
        // slot of this same host and is the only non-resource slot here.
        CHECK( wxWinUITopLevelHost::GetLiveSlotCount() ==
               baseline.slots + slottedWindows.size() + 1 );

        // Generic foundation containers are represented by the same island
        // but remain HWND/GDI windows: they must not manufacture slots.
        CHECK_FALSE( wxWinUITopLevelHost::FindSlotOwner(rootPanel) );
        CHECK_FALSE( wxWinUITopLevelHost::FindSlotOwner(scrolled) );
        CHECK_FALSE( wxWinUITopLevelHost::FindSlotOwner(splitter) );
        CHECK_FALSE( wxWinUITopLevelHost::FindSlotOwner(bookPane) );
        CHECK_FALSE( wxWinUITopLevelHost::FindSlotOwner(treePane) );

        wxWeakRef<wxFrame> frameAlive(frame);
        wxWeakRef<wxTextCtrl> textAlive(text);
        wxWeakRef<wxNotebook> notebookAlive(notebook);
        wxWeakRef<wxTreeCtrl> treeAlive(tree);
        frame->Destroy();
        drainDispatch();

        CHECK_FALSE( frameAlive );
        CHECK_FALSE( textAlive );
        CHECK_FALSE( notebookAlive );
        CHECK_FALSE( treeAlive );

        REQUIRE(WaitFor(
            "mixed XRC notebook retirement",
            [&baseline]()
            {
                return
                    wxNotebook::
                        WinUIGetPendingPeerRetirementCountForTesting() ==
                            baseline.notebookRetirements &&
                    wxNotebook::WinUIGetLiveCallbackStateCountForTesting() ==
                        baseline.notebookCallbacks &&
                    wxNotebook::
                        WinUIGetFrameworkRetirementCountForTesting() ==
                            baseline.frameworkRetirements;
            },
            2000));

        const LifetimeSnapshot afterDestroy = captureLifetime();
        CHECK( afterDestroy.hosts == baseline.hosts );
        CHECK( afterDestroy.slots == baseline.slots );
        CHECK( afterDestroy.slotStates == baseline.slotStates );
        CHECK( afterDestroy.hostStates == baseline.hostStates );
        CHECK( afterDestroy.subclassContexts == baseline.subclassContexts );
        CHECK( afterDestroy.loadedHooks == baseline.loadedHooks );
        CHECK( afterDestroy.textCallbacks == baseline.textCallbacks );
        CHECK( afterDestroy.notebookCallbacks == baseline.notebookCallbacks );
        CHECK( afterDestroy.treeCallbacks == baseline.treeCallbacks );
        CHECK( afterDestroy.toolbarCallbacks == baseline.toolbarCallbacks );
        CHECK( afterDestroy.slotHandlerAdds - baseline.slotHandlerAdds ==
               afterDestroy.slotHandlerRevokes -
                   baseline.slotHandlerRevokes );
        CHECK( afterDestroy.rootHandlerAdds - baseline.rootHandlerAdds ==
               afterDestroy.rootHandlerRevokes -
                   baseline.rootHandlerRevokes );

        // A second drain must be a strict no-op. This catches a callback
        // posted by teardown that retained an already-destroyed slot.
        const unsigned stableFlushAttempts = afterDestroy.flushAttempts;
        drainDispatch();
        CHECK( wxWinUITopLevelHost::GetFlushCallbackAttemptCount() ==
               stableFlushAttempts );
    }

    CHECK( unloadGuard.Unload() );
}

#endif // WinUI V0 mixed supported resource

TEST_CASE_METHOD(XrcTestCase, "XRC::IDRanges", "[xrc]")
{
    // Tests ID ranges
    for ( int n = 0; n < 2; ++n )
    {
        LoadTestXrc();

        // foo[start] should == foo[0]
        CHECK( XRCID("SecondCol[start]") == XRCID("SecondCol[0]") );
        // foo[start] should be < foo[end]. Usually that means more negative
        CHECK( XRCID("SecondCol[start]") < XRCID("SecondCol[end]") );
        // Check it works for the positive values in FirstCol too
        CHECK( XRCID("FirstCol[start]") < XRCID("FirstCol[end]") );

        // Check that values are adjacent
        CHECK( XRCID("SecondCol[0]")+1 == XRCID("SecondCol[1]") );
        CHECK( XRCID("SecondCol[1]")+1 == XRCID("SecondCol[2]") );
        // And for the positive range
        CHECK( XRCID("FirstCol[2]")+1 == XRCID("FirstCol[3]") );

        // Check that a large-enough range was created, despite the small
        // 'size' parameter
        CHECK( XRCID("FirstCol[end]") - XRCID("FirstCol[start]") + 1 == 4 );

        // Check that the far-too-large size range worked off the scale too
        CHECK( XRCID("SecondCol[start]") < XRCID("SecondCol[90]") );
        CHECK( XRCID("SecondCol[90]") < XRCID("SecondCol[end]") );
        CHECK( XRCID("SecondCol[90]")+1 == XRCID("SecondCol[91]") );

        // Check that the positive range-start parameter worked, even after a
        // reload
        CHECK( XRCID("FirstCol[start]") == 10000 );

        // Unload the xrc, so it can be reloaded and the tests rerun
        CHECK( wxXmlResource::Get()->Unload(TEST_XRC_FILE) );
    }
}

TEST_CASE("XRC::UnknownControlSizeHints", "[xrc]")
{
    wxXmlResource::Get()->InitAllHandlers();

    LoadXrcFrom(R"(<?xml version="1.0" ?>
<resource>
  <object class="wxDialog" name="unknown_dialog">
    <title>unknown</title>
    <object class="wxBoxSizer">
      <orient>wxVERTICAL</orient>
      <object class="sizeritem">
        <object class="unknown" name="unknown_panel">
          <size>100,100</size>
        </object>
      </object>
    </object>
  </object>
</resource>
    )");

    wxDialog dlg;
    REQUIRE( wxXmlResource::Get()->LoadDialog(&dlg, nullptr, "unknown_dialog") );

    const wxSize sizeBefore = dlg.GetClientSize();
    XrcSizeHintPanel * const panel = new XrcSizeHintPanel(&dlg);
    const wxSize panelMin = panel->GetMinSize();

    REQUIRE( panelMin.x > sizeBefore.x );
    REQUIRE( wxXmlResource::Get()->AttachUnknownControl("unknown_panel",
                                                        panel,
                                                        &dlg) );

    CHECK( dlg.GetClientSize().x >= panelMin.x );
    CHECK( panel->GetSize().x >= panelMin.x );
}

TEST_CASE("XRC::PathWithFragment", "[xrc][uri]")
{
    wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
    wxImage::AddHandler(new wxXPMHandler);

    const wxString filename = "image#1.xpm";
    TempFile xpmFile(filename);

    // Simplest possible XPM, just to have something to create a bitmap from.
    static const char* xpm =
        "/* XPM */\n"
        "static const char *const xpm[] = {\n"
        "/* columns rows colors chars-per-pixel */\n"
        "\"1 1 1 1\",\n"
        "\"  c None\",\n"
        "/* pixels */\n"
        "\" \"\n"
        ;

    wxFFile ff;
    REQUIRE( ff.Open(filename, "w") );
    REQUIRE( ff.Write(wxString::FromAscii(xpm)) );
    REQUIRE( ff.Close() );

    // Opening a percent-encoded URI should work.
    wxString url = filename;
    url.Replace("#", "%23");

    LoadXrcFrom
    (
        wxString::Format
        (
            "<?xml version=\"1.0\" ?>"
            "<resource>"
            "  <object class=\"wxBitmap\" name=\"bad\">%s</object>"
            "  <object class=\"wxBitmap\" name=\"good\">%s</object>"
            "</resource>",
            filename,
            url
        )
    );

    CHECK( wxXmlResource::Get()->LoadBitmap("good").IsOk() );
    CHECK( !wxXmlResource::Get()->LoadBitmap("bad").IsOk() );
}

TEST_CASE("XRC::Features", "[xrc]")
{
    auto& xrc = *wxXmlResource::Get();

    xrc.EnableFeature("European");
    xrc.EnableFeature("African");

    // Not all birds are available in all geographic editions of this program.
    LoadXrcFrom(R"(<?xml version="1.0" ?>
<resource>
  <object class="wxFrame" name="pigeon"/> <!-- Those are everywhere -->
  <object class="wxFrame" name="eagle" feature="American"/>
  <object class="wxFrame" name="rooster" feature="European"/>
  <object class="wxFrame" name="swallow" feature="African"/>
  <object class="wxFrame" name="sparrow" feature="American|European"/>
  <object class="wxFrame" name="dodo" feature="African|extinct"/>
</resource>
    )");

    CHECK( xrc.LoadFrame(nullptr, "pigeon") );
    CHECK(!xrc.LoadFrame(nullptr, "eagle") );
    CHECK( xrc.LoadFrame(nullptr, "rooster") );
    CHECK( xrc.LoadFrame(nullptr, "sparrow") );
    CHECK( xrc.LoadFrame(nullptr, "swallow") );
    CHECK( xrc.LoadFrame(nullptr, "dodo") );
}

TEST_CASE("XRC::EnvVarInPath", "[xrc]")
{
    wxStringInputStream sis(
#ifdef __WINDOWS__
        "<root><bitmap>%WX_TEST_ENV_IN_PATH%.bmp</bitmap></root>"
#else
        "<root><bitmap>$(WX_TEST_ENV_IN_PATH).bmp</bitmap></root>"
#endif
    );
    wxXmlDocument xmlDoc(sis);
    REQUIRE( xmlDoc.IsOk() );

    class wxTestEnvXmlHandler : public wxXmlResourceHandler
    {
    public:
        wxTestEnvXmlHandler(wxXmlNode* testNode)
        {
            varIsSet = wxSetEnv("WX_TEST_ENV_IN_PATH", "horse");

            wxXmlResource::Get()->SetFlags(wxXRC_USE_LOCALE | wxXRC_USE_ENVVARS);
            SetParentResource(wxXmlResource::Get());

            m_node = testNode;
        }
        ~wxTestEnvXmlHandler()
        {
            wxUnsetEnv("WX_TEST_ENV_IN_PATH");
            wxXmlResource::Get()->SetFlags(wxXRC_USE_LOCALE);
        }
        virtual wxObject* DoCreateResource() override { return nullptr; }
        virtual bool CanHandle(wxXmlNode*) override { return false; }
        bool varIsSet;
    } handler(xmlDoc.GetRoot());

    REQUIRE( handler.varIsSet );

    wxXmlResourceHandlerImpl *impl = new wxXmlResourceHandlerImpl(&handler);
    handler.SetImpl(impl);

    CHECK( impl->GetBitmap().IsOk() );
    CHECK( impl->GetBitmapBundle().IsOk() );
}

// This test is disabled by default as it requires the environment variable
// below to be defined to point to a HTTP URL with the file to load.
//
// Use something like "python3 -m http.server samples/xrc/rc" and set
// WX_TEST_XRC_URL to http://localhost/menu.xrc to run this test.
TEST_CASE_METHOD(XrcTestCase, "XRC::LoadURL", "[.]")
{
    wxString url;
    REQUIRE( wxGetEnv("WX_TEST_XRC_URL", &url) );

    // Ensure that loading from HTTP URLs is supported.
    struct InetHandler : wxInternetFSHandler
    {
        InetHandler()
        {
            wxFileSystem::AddHandler(this);
        }

        ~InetHandler()
        {
            wxFileSystem::RemoveHandler(this);
        }
    } inetHandler;

    CHECK( wxXmlResource::Get()->Load(url) );
}

#endif // wxUSE_XRC
