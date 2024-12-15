/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        samples/widgets/widgets.cpp
// Purpose:     Sample showing most of the simple wxWidgets widgets
// Author:      Vadim Zeitlin
// Created:     27.03.01
// Copyright:   (c) 2001 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
    #include "wx/frame.h"
    #include "wx/menu.h"
    #include "wx/image.h"

    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/listbox.h"
    #include "wx/statbox.h"
    #include "wx/stattext.h"
    #include "wx/textctrl.h"
    #include "wx/msgdlg.h"
#endif

#include "wx/config.h"
#include "wx/stdpaths.h"

#include "wx/sysopt.h"
#include "wx/bookctrl.h"
#include "wx/treebook.h"
#include "wx/sizer.h"
#include "wx/colordlg.h"
#include "wx/fontdlg.h"
#include "wx/numdlg.h"
#include "wx/textdlg.h"
#include "wx/imaglist.h"
#include "wx/treectrl.h"
#include "wx/wupdlock.h"
#include "wx/textcompleter.h"

#include "wx/persist/toplevel.h"
#include "wx/persist/treebook.h"

#include "widgets.h"

#include "../sample.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    Widgets_ClearLog = 100,
    Widgets_Quit,

    Widgets_BookCtrl,

#if wxUSE_TOOLTIPS
    Widgets_SetTooltip,
#endif // wxUSE_TOOLTIPS
    Widgets_SetFgColour,
    Widgets_SetBgColour,
    Widgets_SetPageBg,
    Widgets_SetFont,
    Widgets_Enable,
    Widgets_Show,

    Widgets_BorderNone,
    Widgets_BorderStatic,
    Widgets_BorderSimple,
    Widgets_BorderRaised,
    Widgets_BorderSunken,
    Widgets_BorderDouble,
    Widgets_BorderDefault,

    Widgets_VariantNormal,
    Widgets_VariantSmall,
    Widgets_VariantMini,
    Widgets_VariantLarge,

    Widgets_LayoutDirection,

    Widgets_GlobalBusyCursor,
    Widgets_BusyCursor,

    Widgets_GoToPage,
    Widgets_GoToPageLast = Widgets_GoToPage + 100,


    TextEntry_Begin,
    TextEntry_DisableAutoComplete = TextEntry_Begin,
    TextEntry_AutoCompleteFixed,
    TextEntry_AutoCompleteFilenames,
    TextEntry_AutoCompleteDirectories,
    TextEntry_AutoCompleteCustom,
    TextEntry_AutoCompleteKeyLength,

    TextEntry_SetHint,
    TextEntry_End
};

const wxChar *WidgetsCategories[MAX_PAGES] = {
#if defined(__WXUNIVERSAL__)
    wxT("Universal"),
#else
    wxT("Native"),
#endif
    wxT("Generic"),
    wxT("Pickers"),
    wxT("Comboboxes"),
    wxT("With items"),
    wxT("Editable"),
    wxT("Books"),
    wxT("All controls")
};

// ----------------------------------------------------------------------------
// our classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class WidgetsApp : public wxApp
{
public:
    WidgetsApp()
    {
#if USE_LOG
        m_logTarget = nullptr;
#endif // USE_LOG

#ifdef wxHAS_CONFIG_AS_FILECONFIG
        // We want to put our config file (implicitly created for persistent
        // controls settings) in XDG-compliant location, so we want to change
        // the default file layout, but before doing this migrate any existing
        // config files to the new location as the previous versions of this
        // sample didn't use XDG layout.
        const auto
            res = wxFileConfig::MigrateLocalFile("widgets", wxCONFIG_USE_XDG);
        if ( !res.oldPath.empty() )
        {
            if ( res.error.empty() )
            {
                wxLogMessage("Config file was migrated from \"%s\" to \"%s\"",
                             res.oldPath, res.newPath);
            }
            else
            {
                wxLogWarning("Migrating old config failed: %s.", res.error);
            }
        }

        wxStandardPaths::Get().SetFileLayout(wxStandardPaths::FileLayout_XDG);
#endif // wxHAS_CONFIG_AS_FILECONFIG
    }
    WidgetsApp(const WidgetsApp&) = delete;
    WidgetsApp& operator=(const WidgetsApp&) = delete;

    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit() override;

    // real implementation of WidgetsPage method with the same name
    bool IsUsingLogWindow() const;

    // connects handlers showing some interesting widget events to the given
    // widget
    void ConnectToWidgetEvents(wxWindow* w);

private:
#if USE_LOG
    wxLog* m_logTarget;
#endif // USE_LOG
};

wxDECLARE_APP(WidgetsApp); // This provides a convenient wxGetApp() accessor.

// Define a new frame type: this is going to be our main frame
class WidgetsFrame : public wxFrame
{
public:
    // ctor(s) and dtor
    WidgetsFrame(const wxString& title);
    virtual ~WidgetsFrame();

protected:
    // event handlers
#if USE_LOG
    void OnButtonClearLog(wxCommandEvent& event);
#endif // USE_LOG
    void OnExit(wxCommandEvent& event);

#if wxUSE_MENUS
    void OnPageChanging(WidgetsBookCtrlEvent& event);
    void OnPageChanged(WidgetsBookCtrlEvent& event);
    void OnGoToPage(wxCommandEvent& event);

#if wxUSE_TOOLTIPS
    void OnSetTooltip(wxCommandEvent& event);
#endif // wxUSE_TOOLTIPS
    void OnSetFgCol(wxCommandEvent& event);
    void OnSetBgCol(wxCommandEvent& event);
    void OnSetPageBg(wxCommandEvent& event);
    void OnSetFont(wxCommandEvent& event);
    void OnEnable(wxCommandEvent& event);
    void OnShow(wxCommandEvent &event);
    void OnSetBorder(wxCommandEvent& event);
    void OnSetVariant(wxCommandEvent& event);

    void OnToggleLayoutDirection(wxCommandEvent& event);

    void OnToggleGlobalBusyCursor(wxCommandEvent& event);
    void OnToggleBusyCursor(wxCommandEvent& event);

    // wxTextEntry-specific tests
    void OnDisableAutoComplete(wxCommandEvent& event);
    void OnAutoCompleteFixed(wxCommandEvent& event);
    void OnAutoCompleteFilenames(wxCommandEvent& event);
    void OnAutoCompleteDirectories(wxCommandEvent& event);
    void OnAutoCompleteCustom(wxCommandEvent& event);
    void OnAutoCompleteKeyLength(wxCommandEvent& event);

    void DoUseCustomAutoComplete(size_t minLength = 1);

    void OnSetHint(wxCommandEvent& event);

    void OnUpdateTextUI(wxUpdateUIEvent& event)
    {
        event.Enable( CurrentPage()->GetTextEntry() != nullptr );
    }
#endif // wxUSE_MENUS

    // initialize the book: add all pages to it
    void InitBook();

    // return the currently selected page (never null)
    WidgetsPage *CurrentPage();

private:
    // the panel containing everything
    wxPanel *m_panel;

#if USE_LOG
    // the listbox for logging messages
    wxListBox *m_lboxLog;

    // the log target we use to redirect messages to the listbox
    wxLog *m_logTarget;
#endif // USE_LOG

    // the book containing the test pages
    WidgetsBookCtrl *m_book;

    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};

#if USE_LOG
// A log target which just redirects the messages to a listbox
class LboxLogger : public wxLog
{
public:
    LboxLogger(wxListBox *lbox, wxLog *logOld)
    {
        m_lbox = lbox;
        //m_lbox->Disable(); -- looks ugly under MSW
        m_logOld = logOld;
    }

    virtual ~LboxLogger()
    {
        wxLog::SetActiveTarget(m_logOld);
    }

private:
    // implement sink functions
    virtual void DoLogTextAtLevel(wxLogLevel level, const wxString& msg) override
    {
        if ( level == wxLOG_Trace )
        {
            if ( m_logOld )
                m_logOld->LogTextAtLevel(level, msg);
            return;
        }

        #ifdef __WXUNIVERSAL__
            m_lbox->AppendAndEnsureVisible(msg);
        #else // other ports don't have this method yet
            m_lbox->Append(msg);
            m_lbox->SetFirstItem(m_lbox->GetCount() - 1);
        #endif
    }

    // the control we use
    wxListBox *m_lbox;

    // the old log target
    wxLog *m_logOld;
};
#endif // USE_LOG

// ----------------------------------------------------------------------------
// misc macros
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(WidgetsApp);

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(WidgetsFrame, wxFrame)
#if USE_LOG
    EVT_BUTTON(Widgets_ClearLog, WidgetsFrame::OnButtonClearLog)
#endif // USE_LOG
    EVT_BUTTON(Widgets_Quit, WidgetsFrame::OnExit)

#if wxUSE_TOOLTIPS
    EVT_MENU(Widgets_SetTooltip, WidgetsFrame::OnSetTooltip)
#endif // wxUSE_TOOLTIPS

#if wxUSE_MENUS
    EVT_WIDGETS_PAGE_CHANGING(wxID_ANY, WidgetsFrame::OnPageChanging)
    EVT_MENU_RANGE(Widgets_GoToPage, Widgets_GoToPageLast,
                   WidgetsFrame::OnGoToPage)

    EVT_MENU(Widgets_SetFgColour, WidgetsFrame::OnSetFgCol)
    EVT_MENU(Widgets_SetBgColour, WidgetsFrame::OnSetBgCol)
    EVT_MENU(Widgets_SetPageBg,   WidgetsFrame::OnSetPageBg)
    EVT_MENU(Widgets_SetFont,     WidgetsFrame::OnSetFont)
    EVT_MENU(Widgets_Enable,      WidgetsFrame::OnEnable)
    EVT_MENU(Widgets_Show,        WidgetsFrame::OnShow)

    EVT_MENU_RANGE(Widgets_BorderNone, Widgets_BorderDefault,
                   WidgetsFrame::OnSetBorder)

    EVT_MENU_RANGE(Widgets_VariantNormal, Widgets_VariantLarge,
                   WidgetsFrame::OnSetVariant)

    EVT_MENU(Widgets_LayoutDirection,   WidgetsFrame::OnToggleLayoutDirection)

    EVT_MENU(Widgets_GlobalBusyCursor,  WidgetsFrame::OnToggleGlobalBusyCursor)
    EVT_MENU(Widgets_BusyCursor,        WidgetsFrame::OnToggleBusyCursor)

    EVT_MENU(TextEntry_DisableAutoComplete,   WidgetsFrame::OnDisableAutoComplete)
    EVT_MENU(TextEntry_AutoCompleteFixed,     WidgetsFrame::OnAutoCompleteFixed)
    EVT_MENU(TextEntry_AutoCompleteFilenames, WidgetsFrame::OnAutoCompleteFilenames)
    EVT_MENU(TextEntry_AutoCompleteDirectories, WidgetsFrame::OnAutoCompleteDirectories)
    EVT_MENU(TextEntry_AutoCompleteCustom,    WidgetsFrame::OnAutoCompleteCustom)
    EVT_MENU(TextEntry_AutoCompleteKeyLength, WidgetsFrame::OnAutoCompleteKeyLength)

    EVT_MENU(TextEntry_SetHint, WidgetsFrame::OnSetHint)

    EVT_UPDATE_UI_RANGE(TextEntry_Begin, TextEntry_End - 1,
                        WidgetsFrame::OnUpdateTextUI)

    EVT_MENU(wxID_EXIT, WidgetsFrame::OnExit)
#endif // wxUSE_MENUS
wxEND_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// app class
// ----------------------------------------------------------------------------

bool WidgetsApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    SetVendorName("wxWidgets_Samples");

    // the reason for having these ifdef's is that I often run two copies of
    // this sample side by side and it is useful to see which one is which
    wxString title;
#if defined(__WXUNIVERSAL__)
    title = "wxUniv/";
#endif

#if defined(__WXMSW__)
    title += "wxMSW";
#elif defined(__WXGTK__)
    title += "wxGTK";
#elif defined(__WXMAC__)
    title += "wxMAC";
#else
    title += "wxWidgets";
#endif

    wxFrame *frame = new WidgetsFrame(title + " widgets demo");
    frame->Show();

#if USE_LOG
    m_logTarget = wxLog::GetActiveTarget();
#endif // USE_LOG

    return true;
}

bool WidgetsApp::IsUsingLogWindow() const
{
#if USE_LOG
    return wxLog::GetActiveTarget() == m_logTarget;
#else // !USE_LOG
    return false;
#endif // USE_LOG
}

namespace
{

void OnFocus(wxFocusEvent& event)
{
    // Don't show annoying message boxes when starting or closing the sample,
    // only log these events in our own logger.
    if ( wxGetApp().IsUsingLogWindow() )
    {
        wxWindow* win = (wxWindow*)event.GetEventObject();
        wxLogMessage("Widget '%s' %s focus", win->GetClassInfo()->GetClassName(),
                     event.GetEventType() == wxEVT_SET_FOCUS ? "got" : "lost");
    }

    event.Skip();
}

} // anonymous namespace

void WidgetsApp::ConnectToWidgetEvents(wxWindow* w)
{
    w->Bind(wxEVT_SET_FOCUS, OnFocus);
    w->Bind(wxEVT_KILL_FOCUS, OnFocus);

    w->Bind(wxEVT_ENTER_WINDOW, [w](wxMouseEvent& event)
        {
            wxLogMessage("Mouse entered into '%s'", w->GetClassInfo()->GetClassName());
            event.Skip();
        });
    w->Bind(wxEVT_LEAVE_WINDOW, [w](wxMouseEvent& event)
        {
            wxLogMessage("Mouse left '%s'", w->GetClassInfo()->GetClassName());
            event.Skip();
        });

    w->Bind(wxEVT_CONTEXT_MENU, [w](wxContextMenuEvent& event)
        {
            wxLogMessage("Context menu event for '%s' at %dx%d",
                         w->GetClassInfo()->GetClassName(),
                         event.GetPosition().x,
                         event.GetPosition().y);
            event.Skip();
        });
}

// ----------------------------------------------------------------------------
// WidgetsFrame construction
// ----------------------------------------------------------------------------

WidgetsFrame::WidgetsFrame(const wxString& title)
            : wxFrame(nullptr, wxID_ANY, title)
{
    // set the frame icon
    SetIcon(wxICON(sample));

    // init everything
#if USE_LOG
    m_lboxLog = nullptr;
    m_logTarget = nullptr;
#endif // USE_LOG
    m_book = nullptr;

#if wxUSE_MENUS
    // create the menubar
    wxMenuBar *mbar = new wxMenuBar;
    wxMenu *menuWidget = new wxMenu;
#if wxUSE_TOOLTIPS
    menuWidget->Append(Widgets_SetTooltip, "Set &tooltip...\tCtrl-T");
    menuWidget->AppendSeparator();
#endif // wxUSE_TOOLTIPS
    menuWidget->Append(Widgets_SetFgColour, "Set &foreground...\tCtrl-F");
    menuWidget->Append(Widgets_SetBgColour, "Set &background...\tCtrl-B");
    menuWidget->Append(Widgets_SetPageBg,   "Set &page background...\tShift-Ctrl-B");
    menuWidget->Append(Widgets_SetFont,     "Set f&ont...\tCtrl-O");
    menuWidget->AppendCheckItem(Widgets_Enable,  "&Enable/disable\tCtrl-E");
    menuWidget->AppendCheckItem(Widgets_Show, "Show/Hide");

    wxMenu *menuBorders = new wxMenu;
    menuBorders->AppendRadioItem(Widgets_BorderDefault, "De&fault\tCtrl-Shift-9");
    menuBorders->AppendRadioItem(Widgets_BorderNone,   "&None\tCtrl-Shift-0");
    menuBorders->AppendRadioItem(Widgets_BorderSimple, "&Simple\tCtrl-Shift-1");
    menuBorders->AppendRadioItem(Widgets_BorderDouble, "&Double\tCtrl-Shift-2");
    menuBorders->AppendRadioItem(Widgets_BorderStatic, "Stati&c\tCtrl-Shift-3");
    menuBorders->AppendRadioItem(Widgets_BorderRaised, "&Raised\tCtrl-Shift-4");
    menuBorders->AppendRadioItem(Widgets_BorderSunken, "S&unken\tCtrl-Shift-5");
    menuWidget->AppendSubMenu(menuBorders, "Set &border");

    wxMenu* const menuVariants = new wxMenu;
    menuVariants->AppendRadioItem(Widgets_VariantMini, "&Mini\tCtrl-Shift-6");
    menuVariants->AppendRadioItem(Widgets_VariantSmall, "&Small\tCtrl-Shift-7");
    menuVariants->AppendRadioItem(Widgets_VariantNormal, "&Normal\tCtrl-Shift-8");
    menuVariants->AppendRadioItem(Widgets_VariantLarge, "&Large\tCtrl-Shift-9");
    menuWidget->AppendSubMenu(menuVariants, "Set &variant");

    menuWidget->AppendSeparator();
    menuWidget->AppendCheckItem(Widgets_LayoutDirection,
                                "Toggle &layout direction\tCtrl-L");
    menuWidget->Check(Widgets_LayoutDirection,
                      GetLayoutDirection() == wxLayout_RightToLeft);

    menuWidget->AppendSeparator();
    menuWidget->AppendCheckItem(Widgets_GlobalBusyCursor,
                                "Toggle &global busy cursor\tCtrl-Shift-U");
    menuWidget->AppendCheckItem(Widgets_BusyCursor,
                                "Toggle b&usy cursor\tCtrl-U");

    menuWidget->AppendSeparator();
    menuWidget->Append(wxID_EXIT, "&Quit\tCtrl-Q");
    mbar->Append(menuWidget, "&Widget");

    wxMenu *menuTextEntry = new wxMenu;
    menuTextEntry->AppendRadioItem(TextEntry_DisableAutoComplete,
                                   "&Disable auto-completion");
    menuTextEntry->AppendRadioItem(TextEntry_AutoCompleteFixed,
                                   "Fixed-&list auto-completion");
    menuTextEntry->AppendRadioItem(TextEntry_AutoCompleteFilenames,
                                   "&Files names auto-completion");
    menuTextEntry->AppendRadioItem(TextEntry_AutoCompleteDirectories,
                                   "&Directories names auto-completion");
    menuTextEntry->AppendRadioItem(TextEntry_AutoCompleteCustom,
                                   "&Custom auto-completion");
    menuTextEntry->AppendRadioItem(TextEntry_AutoCompleteKeyLength,
                                   "Custom with &min length");
    menuTextEntry->AppendSeparator();
    menuTextEntry->Append(TextEntry_SetHint, "Set help &hint");

    mbar->Append(menuTextEntry, "&Text");

    SetMenuBar(mbar);

    mbar->Check(Widgets_Enable, true);
    mbar->Check(Widgets_Show, true);

    mbar->Check(Widgets_VariantNormal, true);
#endif // wxUSE_MENUS

    // create controls
    m_panel = new wxPanel(this, wxID_ANY);

    wxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

    // we have 2 panes: book with pages demonstrating the controls in the
    // upper one and the log window with some buttons in the lower

    int style = wxBK_DEFAULT;
    // Uncomment to suppress page theme (draw in solid colour)
    //style |= wxNB_NOPAGETHEME;

    // Give it some reasonably big minimal size by default.
    m_book = new WidgetsBookCtrl(m_panel, Widgets_BookCtrl,
                                 wxDefaultPosition, FromDIP(wxSize(900, 500)),
                                 style, "Widgets");

    InitBook();

    // the lower one only has the log listbox and a button to clear it
#if USE_LOG
    wxStaticBoxSizer *sizerDown = new wxStaticBoxSizer(wxVERTICAL, m_panel, "&Log window");
    wxStaticBox* const sizerDownBox = sizerDown->GetStaticBox();

    m_lboxLog = new wxListBox(sizerDownBox, wxID_ANY);
    sizerDown->Add(m_lboxLog, wxSizerFlags(1).Expand().Border());
    sizerDown->SetMinSize(100, 150);
#else
    wxSizer *sizerDown = new wxBoxSizer(wxVERTICAL);
#endif // USE_LOG

    wxBoxSizer *sizerBtns = new wxBoxSizer(wxHORIZONTAL);
    wxButton *btn;
#if USE_LOG
    btn = new wxButton(sizerDownBox, Widgets_ClearLog, "Clear &log");
    sizerBtns->Add(btn);
    sizerBtns->AddSpacer(10);
#endif // USE_LOG
    btn = new wxButton(sizerDownBox, Widgets_Quit, "E&xit");
    sizerBtns->Add(btn);
    sizerDown->Add(sizerBtns, wxSizerFlags().Border().Right());

    // put everything together
    sizerTop->Add(m_book, wxSizerFlags(1).Expand().DoubleBorder(wxALL & ~(wxTOP | wxBOTTOM)));
    sizerTop->AddSpacer(5);
    sizerTop->Add(sizerDown, wxSizerFlags(0).Expand().DoubleBorder(wxALL & ~wxTOP));

    m_panel->SetSizer(sizerTop);

    const bool sizeSet = wxPersistentRegisterAndRestore(this, "Main");

    const wxSize sizeMin = m_panel->GetBestSize();
    if ( !sizeSet )
        SetClientSize(sizeMin);
    SetMinClientSize(sizeMin);

#if USE_LOG
    // now that everything is created we can redirect the log messages to the
    // listbox
    m_logTarget = new LboxLogger(m_lboxLog, wxLog::GetActiveTarget());
    wxLog::SetActiveTarget(m_logTarget);
#endif
}

void WidgetsFrame::InitBook()
{
    wxImageList *imageList = new wxImageList(ICON_SIZE, ICON_SIZE);

    wxImage img(sample_xpm);
    imageList->Add(wxBitmap(img.Scale(ICON_SIZE, ICON_SIZE)));

#if !USE_TREEBOOK
    WidgetsBookCtrl *books[MAX_PAGES];
#endif

    using ArrayWidgetsPage = std::vector<WidgetsPage*>;
    ArrayWidgetsPage pages[MAX_PAGES];
    wxArrayString labels[MAX_PAGES];

    wxMenu *menuPages = new wxMenu;
    unsigned int nPage = 0;
    int cat, imageId = 1;

    // we need to first create all pages and only then add them to the book
    // as we need the image list first
    //
    // we also construct the pages menu during this first iteration
    for ( cat = 0; cat < MAX_PAGES; cat++ )
    {
#if USE_TREEBOOK
        nPage++; // increase for parent page
#else
        books[cat] = new WidgetsBookCtrl(m_book,
                                         wxID_ANY,
                                         wxDefaultPosition,
                                         wxDefaultSize,
                                         wxBK_DEFAULT);
#endif

        for ( WidgetsPageInfo *info = WidgetsPage::ms_widgetPages;
              info;
              info = info->GetNext() )
        {
            if( (info->GetCategories() & ( 1 << cat )) == 0)
                continue;

            WidgetsPage *page = (*info->GetCtor())(
#if USE_TREEBOOK
                                 m_book
#else
                                 books[cat]
#endif
                                 , imageList);
            pages[cat].push_back(page);

            labels[cat].Add(info->GetLabel());
            if ( cat == ALL_PAGE )
            {
                menuPages->AppendRadioItem(
                            Widgets_GoToPage + nPage,
                            info->GetLabel()
                           );
#if !USE_TREEBOOK
                // consider only for book in book architecture
                nPage++;
#endif
            }

#if USE_TREEBOOK
            // consider only for treebook architecture (with subpages)
            nPage++;
#endif
        }
    }

    GetMenuBar()->Append(menuPages, "&Page");

    m_book->AssignImageList(imageList);

    for ( cat = 0; cat < MAX_PAGES; cat++ )
    {
#if USE_TREEBOOK
        m_book->AddPage(nullptr,WidgetsCategories[cat],false,0);
#else
        m_book->AddPage(books[cat],WidgetsCategories[cat],false,0);
        books[cat]->SetImageList(imageList);
#endif

        // now do add them
        size_t count = pages[cat].size();
        for ( size_t n = 0; n < count; n++ )
        {
#if USE_TREEBOOK
            m_book->AddSubPage
#else
            books[cat]->AddPage
#endif
                           (
                            pages[cat][n],
                            labels[cat][n],
                            false, // don't select
                            imageId++
                           );
        }
    }

    Bind(wxEVT_COMMAND_WIDGETS_PAGE_CHANGED, &WidgetsFrame::OnPageChanged, this);

    const bool pageSet = wxPersistentRegisterAndRestore(m_book);

#if USE_TREEBOOK
    // for treebook page #0 is empty parent page only so select the first page
    // with some contents
    if ( !pageSet || !m_book->GetCurrentPage() )
        m_book->SetSelection(1);

    // but ensure that the top of the tree is shown nevertheless
    wxTreeCtrl * const tree = m_book->GetTreeCtrl();

    wxTreeItemIdValue cookie;
    tree->EnsureVisible(tree->GetFirstChild(tree->GetRootItem(), cookie));
#else
    if ( !pageSet || !m_book->GetCurrentPage() )
    {
        // for other books set selection twice to force connected event handler
        // to force lazy creation of initial visible content
        m_book->SetSelection(1);
        m_book->SetSelection(0);
    }
#endif // USE_TREEBOOK
}

WidgetsPage *WidgetsFrame::CurrentPage()
{
    wxWindow *page = m_book->GetCurrentPage();

#if !USE_TREEBOOK
    WidgetsBookCtrl *subBook = wxStaticCast(page, WidgetsBookCtrl);
    wxCHECK_MSG( subBook, nullptr, "no WidgetsBookCtrl?" );

    page = subBook->GetCurrentPage();
#endif // !USE_TREEBOOK

    return wxStaticCast(page, WidgetsPage);
}

WidgetsFrame::~WidgetsFrame()
{
#if USE_LOG
    delete m_logTarget;
#endif // USE_LOG
}

// ----------------------------------------------------------------------------
// WidgetsFrame event handlers
// ----------------------------------------------------------------------------

void WidgetsFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

#if USE_LOG
void WidgetsFrame::OnButtonClearLog(wxCommandEvent& WXUNUSED(event))
{
    m_lboxLog->Clear();
}
#endif // USE_LOG

#if wxUSE_MENUS

void WidgetsFrame::OnPageChanging(WidgetsBookCtrlEvent& event)
{
#if USE_TREEBOOK
    // don't allow selection of entries without pages (categories)
    if ( !m_book->GetPage(event.GetSelection()) )
        event.Veto();
#else
    wxUnusedVar(event);
#endif
}

void WidgetsFrame::OnPageChanged(WidgetsBookCtrlEvent& event)
{
    const int sel = event.GetSelection();

    // adjust "Page" menu selection
    wxMenuItem *item = GetMenuBar()->FindItem(Widgets_GoToPage + sel);
    if ( item )
        item->Check();

    GetMenuBar()->Check(Widgets_BusyCursor, false);

    // create the pages on demand, otherwise the sample startup is too slow as
    // it creates hundreds of controls
    WidgetsPage *curPage = CurrentPage();

    bool hasChildren = false;
    for ( const auto child : curPage->GetChildren() )
    {
        if ( curPage->IsClientAreaChild(child) )
        {
            hasChildren = true;
            break;
        }
    }

    if ( !hasChildren )
    {
        wxWindowUpdateLocker noUpdates(curPage);
        curPage->CreateContent();
        curPage->SetScrollRate(10, 10);
        curPage->FitInside();

        auto& app = wxGetApp();
        for ( const auto w : CurrentPage()->GetWidgets() )
        {
            app.ConnectToWidgetEvents(w);
        }

        // From now on, we're interested in these notifications as we'll need
        // to reconnect to the widget events if it's recreated (unfortunately
        // we can't rely getting them on creation as some page don't generate
        // them -- but neither can we rely on not getting them as some pages do
        // generate them, hence the use of m_notifyRecreate flag).
        curPage->EnableRecreationNotifications();
    }

    // re-apply the attributes to the widget(s)
    curPage->SetUpWidget();

    event.Skip();
}

void WidgetsFrame::OnGoToPage(wxCommandEvent& event)
{
#if USE_TREEBOOK
    m_book->SetSelection(event.GetId() - Widgets_GoToPage);
#else
    m_book->SetSelection(m_book->GetPageCount()-1);
    WidgetsBookCtrl *book = wxStaticCast(m_book->GetCurrentPage(), WidgetsBookCtrl);
    book->SetSelection(event.GetId() - Widgets_GoToPage);
#endif
}

#if wxUSE_TOOLTIPS

void WidgetsFrame::OnSetTooltip(wxCommandEvent& WXUNUSED(event))
{
    wxTextEntryDialog dialog
                      (
                        this,
                        "Tooltip text (may use \\n, leave empty to remove): ",
                        "Widgets sample",
                        WidgetsPage::GetAttrs().m_tooltip
                      );

    if ( dialog.ShowModal() != wxID_OK )
        return;

    WidgetsPage::GetAttrs().m_tooltip = dialog.GetValue();
    WidgetsPage::GetAttrs().m_tooltip.Replace("\\n", "\n");

    CurrentPage()->SetUpWidget();
}

#endif // wxUSE_TOOLTIPS

namespace
{

// Trivial wrapper for wxGetColourFromUser() which also does something even if
// the colour dialog is not available in the current build (which may happen
// for the ports in development and it is still useful to see how colours work)
wxColour GetColourFromUser(wxWindow *parent, const wxColour& colDefault)
{
#if wxUSE_COLOURDLG
    return wxGetColourFromUser(parent, colDefault);
#else // !wxUSE_COLOURDLG
    if ( colDefault == *wxBLACK )
        return *wxWHITE;
    else
        return *wxBLACK;
#endif // wxUSE_COLOURDLG/!wxUSE_COLOURDLG
}

} // anonymous namespace

void WidgetsFrame::OnSetFgCol(wxCommandEvent& WXUNUSED(event))
{
    // allow for debugging the default colour the first time this is called
    WidgetsPage *page = CurrentPage();

    if (!WidgetsPage::GetAttrs().m_colFg.IsOk())
        WidgetsPage::GetAttrs().m_colFg = page->GetForegroundColour();

    wxColour col = GetColourFromUser(this, WidgetsPage::GetAttrs().m_colFg);
    if ( !col.IsOk() )
        return;

    WidgetsPage::GetAttrs().m_colFg = col;

    page->SetUpWidget();
}

void WidgetsFrame::OnSetBgCol(wxCommandEvent& WXUNUSED(event))
{
    WidgetsPage *page = CurrentPage();

    if ( !WidgetsPage::GetAttrs().m_colBg.IsOk() )
        WidgetsPage::GetAttrs().m_colBg = page->GetBackgroundColour();

    wxColour col = GetColourFromUser(this, WidgetsPage::GetAttrs().m_colBg);
    if ( !col.IsOk() )
        return;

    WidgetsPage::GetAttrs().m_colBg = col;

    page->SetUpWidget();
}

void WidgetsFrame::OnSetPageBg(wxCommandEvent& WXUNUSED(event))
{
    wxColour col = GetColourFromUser(this, GetBackgroundColour());
    if ( !col.IsOk() )
        return;

    WidgetsPage::GetAttrs().m_colPageBg = col;

    CurrentPage()->SetUpWidget();
}

void WidgetsFrame::OnSetFont(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_FONTDLG
    WidgetsPage *page = CurrentPage();

    if (!WidgetsPage::GetAttrs().m_font.IsOk())
        WidgetsPage::GetAttrs().m_font = page->GetFont();

    wxFont font = wxGetFontFromUser(this, WidgetsPage::GetAttrs().m_font);
    if ( !font.IsOk() )
        return;

    WidgetsPage::GetAttrs().m_font = font;

    page->SetUpWidget();
    // The best size of the widget could have changed after changing its font,
    // so re-layout to show it correctly.
    page->Layout();
#else
    wxLogMessage("Font selection dialog not available in current build.");
#endif
}

void WidgetsFrame::OnEnable(wxCommandEvent& event)
{
    WidgetsPage::GetAttrs().m_enabled = event.IsChecked();

    CurrentPage()->SetUpWidget();
}

void WidgetsFrame::OnShow(wxCommandEvent &event)
{
    WidgetsPage::GetAttrs().m_show = event.IsChecked();

    CurrentPage()->SetUpWidget();
}

void WidgetsFrame::OnSetBorder(wxCommandEvent& event)
{
    int border;
    switch ( event.GetId() )
    {
        case Widgets_BorderNone:   border = wxBORDER_NONE;   break;
        case Widgets_BorderStatic: border = wxBORDER_STATIC; break;
        case Widgets_BorderSimple: border = wxBORDER_SIMPLE; break;
        case Widgets_BorderRaised: border = wxBORDER_RAISED; break;
        case Widgets_BorderSunken: border = wxBORDER_SUNKEN; break;
        case Widgets_BorderDouble: border = wxBORDER_DOUBLE; break;

        default:
            wxFAIL_MSG( "unknown border style" );
            wxFALLTHROUGH;

        case Widgets_BorderDefault: border = wxBORDER_DEFAULT; break;
    }

    WidgetsPage::GetAttrs().m_defaultFlags &= ~wxBORDER_MASK;
    WidgetsPage::GetAttrs().m_defaultFlags |= border;

    WidgetsPage *page = CurrentPage();

    page->RecreateWidget();
}

void WidgetsFrame::OnSetVariant(wxCommandEvent& event)
{
    wxWindowVariant v;
    switch ( event.GetId() )
    {
        case Widgets_VariantSmall:  v = wxWINDOW_VARIANT_SMALL; break;
        case Widgets_VariantMini:   v = wxWINDOW_VARIANT_MINI; break;
        case Widgets_VariantLarge:  v = wxWINDOW_VARIANT_LARGE; break;

        default:
            wxFAIL_MSG( "unknown window variant" );
            wxFALLTHROUGH;

        case Widgets_VariantNormal: v = wxWINDOW_VARIANT_NORMAL; break;
    }

    WidgetsPage::GetAttrs().m_variant = v;

    CurrentPage()->SetUpWidget();
    CurrentPage()->Layout();
}

void WidgetsFrame::OnToggleLayoutDirection(wxCommandEvent&)
{
    wxLayoutDirection dir = WidgetsPage::GetAttrs().m_dir;
    if (dir == wxLayout_Default)
        dir = GetLayoutDirection();
    WidgetsPage::GetAttrs().m_dir =
        (dir == wxLayout_LeftToRight) ? wxLayout_RightToLeft : wxLayout_LeftToRight;

    CurrentPage()->SetUpWidget();
}

void WidgetsFrame::OnToggleGlobalBusyCursor(wxCommandEvent& event)
{
    if ( event.IsChecked() )
        wxBeginBusyCursor();
    else
        wxEndBusyCursor();
}

void WidgetsFrame::OnToggleBusyCursor(wxCommandEvent& event)
{
    WidgetsPage::GetAttrs().m_cursor = (event.IsChecked() ? *wxHOURGLASS_CURSOR
                                                          : wxNullCursor);

    CurrentPage()->SetUpWidget();
}

void WidgetsFrame::OnDisableAutoComplete(wxCommandEvent& WXUNUSED(event))
{
    wxTextEntryBase *entry = CurrentPage()->GetTextEntry();
    wxCHECK_RET( entry, "menu item should be disabled" );

    if ( entry->AutoComplete(wxArrayString()) )
    {
        wxLogMessage("Disabled auto completion.");
    }
    else
    {
        wxLogMessage("AutoComplete() failed.");
    }
}

void WidgetsFrame::OnAutoCompleteFixed(wxCommandEvent& WXUNUSED(event))
{
    wxTextEntryBase *entry = CurrentPage()->GetTextEntry();
    wxCHECK_RET( entry, "menu item should be disabled" );

    wxArrayString completion_choices;

    // add a few strings so a completion occurs on any letter typed
    for ( char idxc = 'a'; idxc < 'z'; ++idxc )
        completion_choices.push_back(wxString::Format("%c%c", idxc, idxc));

    completion_choices.push_back("is this string for test?");
    completion_choices.push_back("this is a test string");
    completion_choices.push_back("this is another test string");
    completion_choices.push_back("this string is for test");

    if ( entry->AutoComplete(completion_choices) )
    {
        wxLogMessage("Enabled auto completion of a set of fixed strings.");
    }
    else
    {
        wxLogMessage("AutoComplete() failed.");
    }
}

void WidgetsFrame::OnAutoCompleteFilenames(wxCommandEvent& WXUNUSED(event))
{
    wxTextEntryBase *entry = CurrentPage()->GetTextEntry();
    wxCHECK_RET( entry, "menu item should be disabled" );

    if ( entry->AutoCompleteFileNames() )
    {
        wxLogMessage("Enabled auto completion of file names.");
    }
    else
    {
        wxLogMessage("AutoCompleteFileNames() failed.");
    }
}

void WidgetsFrame::OnAutoCompleteDirectories(wxCommandEvent& WXUNUSED(event))
{
    wxTextEntryBase *entry = CurrentPage()->GetTextEntry();
    wxCHECK_RET( entry, "menu item should be disabled" );

    if ( entry->AutoCompleteDirectories() )
    {
        wxLogMessage("Enabled auto completion of directories.");
    }
    else
    {
        wxLogMessage("AutoCompleteDirectories() failed.");
    }
}

void WidgetsFrame::OnAutoCompleteCustom(wxCommandEvent& WXUNUSED(event))
{
    DoUseCustomAutoComplete();
}

void WidgetsFrame::DoUseCustomAutoComplete(size_t minLength)
{
    wxTextEntryBase *entry = CurrentPage()->GetTextEntry();
    wxCHECK_RET( entry, "menu item should be disabled" );

    // This is a simple (and hence rather useless) example of a custom
    // completer class that completes the first word (only) initially and only
    // build the list of the possible second words once the first word is
    // known. This allows to avoid building the full 676000 item list of
    // possible strings all at once as the we have 1000 possibilities for the
    // first word (000..999) and 676 (aa..zz) for the second one.
    class CustomTextCompleter : public wxTextCompleterSimple
    {
    public:
        explicit CustomTextCompleter(size_t minLength)
            : m_minLength(minLength)
        {
        }

        virtual void GetCompletions(const wxString& prefix, wxArrayString& res) override
        {
            // This is used for illustrative purposes only and shows how many
            // completions we return every time when we're called.
            class LogCompletions
            {
            public:
                LogCompletions(const wxString& prefix, const wxArrayString& res)
                    : m_prefix(prefix),
                      m_res(res)
                {
                }

                ~LogCompletions()
                {
                    wxLogMessage("Returning %lu possible completions for "
                                 "prefix \"%s\"",
                                 m_res.size(), m_prefix);
                }

            private:
                const wxString& m_prefix;
                const wxArrayString& m_res;
            } logCompletions(prefix, res);


            // Wait for enough text to be entered before proposing completions:
            // this is done to avoid proposing too many of them when the
            // control is empty, for example.
            if ( prefix.length() < m_minLength )
                return;

            // The only valid strings start with 3 digits so check for their
            // presence proposing to complete the remaining ones.
            if ( !wxIsdigit(prefix[0]) )
                return;

            if ( prefix.length() == 1 )
            {
                for ( int i = 0; i < 10; i++ )
                    for ( int j = 0; j < 10; j++ )
                        res.push_back(wxString::Format("%s%02d",
                                                       prefix, 10*i + j));
                return;
            }
            else if ( !wxIsdigit(prefix[1]) )
                return;

            if ( prefix.length() == 2 )
            {
                for ( int i = 0; i < 10; i++ )
                    res.push_back(wxString::Format("%s%d", prefix, i));
                return;
            }
            else if ( !wxIsdigit(prefix[2]) )
                return;

            // Next we must have a space and two letters.
            wxString prefix2(prefix);
            if ( prefix.length() == 3 )
                prefix2 += ' ';
            else if ( prefix[3] != ' ' )
                return;

            if ( prefix2.length() == 4 )
            {
                for ( char c = 'a'; c <= 'z'; c++ )
                    for ( char d = 'a'; d <= 'z'; d++ )
                        res.push_back(wxString::Format("%s%c%c", prefix2, c, d));
                return;
            }
            else if ( !wxIslower(prefix[4]) )
                return;

            if ( prefix.length() == 5 )
            {
                for ( char c = 'a'; c <= 'z'; c++ )
                    res.push_back(prefix + c);
            }
        }

        size_t m_minLength;
    };

    if ( entry->AutoComplete(new CustomTextCompleter(minLength)))
    {
        wxLogMessage("Enabled custom auto completer for \"NNN XX\" items "
                     "(where N is a digit and X is a letter).");
    }
    else
    {
        wxLogMessage("AutoComplete() failed.");
    }
}

void WidgetsFrame::OnAutoCompleteKeyLength(wxCommandEvent& WXUNUSED(event))
{
    const wxString message = "The auto-completion is triggered if and only if\n"
                             "the length of the search key (prefix) is at least [LENGTH].\n"
                             "Hint: 0 disables the length check completely.";
    const wxString prompt = "Enter the minimum key length:";
    const wxString caption = "Minimum key length";

    int res = wxGetNumberFromUser(message, prompt, caption, 1, 0, 100, this);
    if ( res == -1 )
        return;

    wxLogMessage("The minimum key length for autocomplete is %d.", res);

    DoUseCustomAutoComplete(static_cast<size_t>(res));
}

void WidgetsFrame::OnSetHint(wxCommandEvent& WXUNUSED(event))
{
    wxTextEntryBase *entry = CurrentPage()->GetTextEntry();
    wxCHECK_RET( entry, "menu item should be disabled" );

    static wxString s_hint("Type here");
    wxString
        hint = wxGetTextFromUser("Text hint:", "Widgets sample", s_hint, this);
    if ( hint.empty() )
        return;

    s_hint = hint;

    if ( entry->SetHint(hint) )
    {
        wxLogMessage("Set hint to \"%s\".", hint);
    }
    else
    {
        wxLogMessage("Text hints not supported.");
    }
}

#endif // wxUSE_MENUS

// ----------------------------------------------------------------------------
// WidgetsPageInfo
// ----------------------------------------------------------------------------

WidgetsPageInfo::WidgetsPageInfo(Constructor ctor, const wxString& label, int categories)
               : m_label(label)
               , m_categories(categories)
{
    m_ctor = ctor;

    m_next = nullptr;

    // dummy sorting: add and immediately sort in the list according to label
    if ( WidgetsPage::ms_widgetPages )
    {
        WidgetsPageInfo *node_prev = WidgetsPage::ms_widgetPages;
        if ( wxStrcmp(label, node_prev->GetLabel().c_str()) < 0 )
        {
            // add as first
            m_next = node_prev;
            WidgetsPage::ms_widgetPages = this;
        }
        else
        {
            WidgetsPageInfo *node_next;
            do
            {
                node_next = node_prev->GetNext();
                if ( node_next )
                {
                    // add if between two
                    if ( wxStrcmp(label, node_next->GetLabel().c_str()) < 0 )
                    {
                        node_prev->SetNext(this);
                        m_next = node_next;
                        // force to break loop
                        node_next = nullptr;
                    }
                }
                else
                {
                    // add as last
                    node_prev->SetNext(this);
                    m_next = node_next;
                }
                node_prev = node_next;
            }
            while ( node_next );
        }
    }
    else
    {
        // add when first
        WidgetsPage::ms_widgetPages = this;
    }
}

// ----------------------------------------------------------------------------
// WidgetsPage
// ----------------------------------------------------------------------------

WidgetsPageInfo *WidgetsPage::ms_widgetPages = nullptr;

WidgetsPage::WidgetsPage(WidgetsBookCtrl *book,
                         wxImageList *imaglist,
                         const char *const icon[])
           : wxScrolledWindow(book, wxID_ANY,
                     wxDefaultPosition, wxDefaultSize,
                     wxCLIP_CHILDREN | wxTAB_TRAVERSAL)
{
    imaglist->Add(wxBitmap(wxImage(icon).Scale(ICON_SIZE, ICON_SIZE)));
}

/* static */
WidgetAttributes& WidgetsPage::GetAttrs()
{
    static WidgetAttributes s_attrs;

    return s_attrs;
}

void WidgetsPage::SetUpWidget()
{
    const Widgets widgets = GetWidgets();

    for ( Widgets::const_iterator it = widgets.begin();
            it != widgets.end();
            ++it )
    {
        wxCHECK_RET(*it, "null widget");

#if wxUSE_TOOLTIPS
        (*it)->SetToolTip(GetAttrs().m_tooltip);
#endif // wxUSE_TOOLTIPS
#if wxUSE_FONTDLG
        if ( GetAttrs().m_font.IsOk() )
        {
            (*it)->SetFont(GetAttrs().m_font);
        }
#endif // wxUSE_FONTDLG
        if ( GetAttrs().m_colFg.IsOk() )
        {
            (*it)->SetForegroundColour(GetAttrs().m_colFg);
        }

        if ( GetAttrs().m_colBg.IsOk() )
        {
            (*it)->SetBackgroundColour(GetAttrs().m_colBg);
        }

        if (GetAttrs().m_dir != wxLayout_Default)
            (*it)->SetLayoutDirection(GetAttrs().m_dir);
        (*it)->Enable(GetAttrs().m_enabled);
        (*it)->Show(GetAttrs().m_show);

        (*it)->SetCursor(GetAttrs().m_cursor);

        (*it)->SetWindowVariant(GetAttrs().m_variant);

        (*it)->Refresh();
    }

    if ( GetAttrs().m_colPageBg.IsOk() )
    {
        SetBackgroundColour(GetAttrs().m_colPageBg);
        Refresh();
    }
}

wxSizer *WidgetsPage::CreateSizerWithText(wxControl *control,
                                          wxWindowID id,
                                          wxTextCtrl **ppText)
{
    wxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);
    wxTextCtrl *text = new wxTextCtrl(control->GetParent(), id, wxEmptyString,
        wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);

    sizerRow->Add(control, wxSizerFlags(0).Border(wxRIGHT).CentreVertical());
    sizerRow->Add(text, wxSizerFlags(1).Border(wxLEFT).CentreVertical());

    if ( ppText )
        *ppText = text;

    return sizerRow;
}

// create a sizer containing a label and a text ctrl
wxSizer *WidgetsPage::CreateSizerWithTextAndLabel(const wxString& label,
                                                  wxWindowID id,
                                                  wxTextCtrl **ppText,
                                                  wxWindow* statBoxParent)
{
    return CreateSizerWithText(new wxStaticText(statBoxParent ? statBoxParent: this, wxID_ANY, label),
        id, ppText);
}

// create a sizer containing a button and a text ctrl
wxSizer *WidgetsPage::CreateSizerWithTextAndButton(wxWindowID idBtn,
                                                   const wxString& label,
                                                   wxWindowID id,
                                                   wxTextCtrl **ppText,
                                                   wxWindow* statBoxParent)
{
    return CreateSizerWithText(new wxButton(statBoxParent ? statBoxParent: this, idBtn, label), id, ppText);
}

wxCheckBox *WidgetsPage::CreateCheckBoxAndAddToSizer(wxSizer *sizer,
                                                     const wxString& label,
                                                     wxWindowID id,
                                                     wxWindow* statBoxParent)
{
    wxCheckBox *checkbox = new wxCheckBox(statBoxParent ? statBoxParent: this, id, label);
    sizer->Add(checkbox, wxSizerFlags().HorzBorder());
    sizer->AddSpacer(2);

    return checkbox;
}

void WidgetsPage::NotifyWidgetRecreation(wxWindow* widget)
{
    if ( !m_notifyRecreate )
    {
        // We're in the process of initialization, don't notify yet.
        return;
    }

    SetUpWidget();

    wxGetApp().ConnectToWidgetEvents(widget);
}

/* static */
bool WidgetsPage::IsUsingLogWindow()
{
    return wxGetApp().IsUsingLogWindow();
}
