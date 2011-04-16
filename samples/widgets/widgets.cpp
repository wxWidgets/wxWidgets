/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        samples/widgets/widgets.cpp
// Purpose:     Sample showing most of the simple wxWidgets widgets
// Author:      Vadim Zeitlin
// Created:     27.03.01
// Id:          $Id$
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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

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

#include "wx/sysopt.h"
#include "wx/bookctrl.h"
#include "wx/treebook.h"
#include "wx/sizer.h"
#include "wx/colordlg.h"
#include "wx/fontdlg.h"
#include "wx/textdlg.h"
#include "wx/imaglist.h"
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

    Widgets_BorderNone,
    Widgets_BorderStatic,
    Widgets_BorderSimple,
    Widgets_BorderRaised,
    Widgets_BorderSunken,
    Widgets_BorderDouble,
    Widgets_BorderDefault,

    Widgets_GlobalBusyCursor,
    Widgets_BusyCursor,

    Widgets_GoToPage,
    Widgets_GoToPageLast = Widgets_GoToPage + 100,


    TextEntry_Begin,
    TextEntry_DisableAutoComplete = TextEntry_Begin,
    TextEntry_AutoCompleteFixed,
    TextEntry_AutoCompleteFilenames,
    TextEntry_AutoCompleteCustom,

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
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit();
};

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
    void OnSetBorder(wxCommandEvent& event);

    void OnToggleGlobalBusyCursor(wxCommandEvent& event);
    void OnToggleBusyCursor(wxCommandEvent& event);

    // wxTextEntry-specific tests
    void OnDisableAutoComplete(wxCommandEvent& event);
    void OnAutoCompleteFixed(wxCommandEvent& event);
    void OnAutoCompleteFilenames(wxCommandEvent& event);
    void OnAutoCompleteCustom(wxCommandEvent& event);

    void OnSetHint(wxCommandEvent& event);

    void OnUpdateTextUI(wxUpdateUIEvent& event)
    {
        event.Enable( CurrentPage()->GetTextEntry() != NULL );
    }
#endif // wxUSE_MENUS

    // initialize the book: add all pages to it
    void InitBook();

    // return the currently selected page (never NULL)
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

#if wxUSE_MENUS
    // last chosen fg/bg colours and font
    wxColour m_colFg,
             m_colBg;
    wxFont   m_font;
#endif // wxUSE_MENUS

    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
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
    virtual void DoLogTextAtLevel(wxLogLevel level, const wxString& msg)
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

// array of pages
WX_DEFINE_ARRAY_PTR(WidgetsPage *, ArrayWidgetsPage);

// ----------------------------------------------------------------------------
// misc macros
// ----------------------------------------------------------------------------

IMPLEMENT_APP(WidgetsApp)

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(WidgetsFrame, wxFrame)
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

    EVT_MENU_RANGE(Widgets_BorderNone, Widgets_BorderDefault,
                   WidgetsFrame::OnSetBorder)

    EVT_MENU(Widgets_GlobalBusyCursor,  WidgetsFrame::OnToggleGlobalBusyCursor)
    EVT_MENU(Widgets_BusyCursor,        WidgetsFrame::OnToggleBusyCursor)

    EVT_MENU(TextEntry_DisableAutoComplete,   WidgetsFrame::OnDisableAutoComplete)
    EVT_MENU(TextEntry_AutoCompleteFixed,     WidgetsFrame::OnAutoCompleteFixed)
    EVT_MENU(TextEntry_AutoCompleteFilenames, WidgetsFrame::OnAutoCompleteFilenames)
    EVT_MENU(TextEntry_AutoCompleteCustom,    WidgetsFrame::OnAutoCompleteCustom)

    EVT_MENU(TextEntry_SetHint, WidgetsFrame::OnSetHint)

    EVT_UPDATE_UI_RANGE(TextEntry_Begin, TextEntry_End - 1,
                        WidgetsFrame::OnUpdateTextUI)

    EVT_MENU(wxID_EXIT, WidgetsFrame::OnExit)
#endif // wxUSE_MENUS
END_EVENT_TABLE()

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
    title = wxT("wxUniv/");
#endif

#if defined(__WXMSW__)
    title += wxT("wxMSW");
#elif defined(__WXGTK__)
    title += wxT("wxGTK");
#elif defined(__WXMAC__)
    title += wxT("wxMAC");
#elif defined(__WXMOTIF__)
    title += wxT("wxMOTIF");
#elif defined(__WXPALMOS5__)
    title += wxT("wxPALMOS5");
#elif defined(__WXPALMOS6__)
    title += wxT("wxPALMOS6");
#else
    title += wxT("wxWidgets");
#endif

    wxFrame *frame = new WidgetsFrame(title + wxT(" widgets demo"));
    frame->Show();

    return true;
}

// ----------------------------------------------------------------------------
// WidgetsFrame construction
// ----------------------------------------------------------------------------

WidgetsFrame::WidgetsFrame(const wxString& title)
            : wxFrame(NULL, wxID_ANY, title)
{
    const bool sizeSet = wxPersistentRegisterAndRestore(this, "Main");

    // set the frame icon
    SetIcon(wxICON(sample));

    // init everything
#if USE_LOG
    m_lboxLog = NULL;
    m_logTarget = NULL;
#endif // USE_LOG
    m_book = NULL;

#if wxUSE_MENUS
    // create the menubar
    wxMenuBar *mbar = new wxMenuBar;
    wxMenu *menuWidget = new wxMenu;
#if wxUSE_TOOLTIPS
    menuWidget->Append(Widgets_SetTooltip, wxT("Set &tooltip...\tCtrl-T"));
    menuWidget->AppendSeparator();
#endif // wxUSE_TOOLTIPS
    menuWidget->Append(Widgets_SetFgColour, wxT("Set &foreground...\tCtrl-F"));
    menuWidget->Append(Widgets_SetBgColour, wxT("Set &background...\tCtrl-B"));
    menuWidget->Append(Widgets_SetPageBg,   wxT("Set &page background...\tShift-Ctrl-B"));
    menuWidget->Append(Widgets_SetFont,     wxT("Set f&ont...\tCtrl-O"));
    menuWidget->AppendCheckItem(Widgets_Enable,  wxT("&Enable/disable\tCtrl-E"));

    wxMenu *menuBorders = new wxMenu;
    menuBorders->AppendRadioItem(Widgets_BorderDefault, wxT("De&fault\tCtrl-Shift-9"));
    menuBorders->AppendRadioItem(Widgets_BorderNone,   wxT("&None\tCtrl-Shift-0"));
    menuBorders->AppendRadioItem(Widgets_BorderSimple, wxT("&Simple\tCtrl-Shift-1"));
    menuBorders->AppendRadioItem(Widgets_BorderDouble, wxT("&Double\tCtrl-Shift-2"));
    menuBorders->AppendRadioItem(Widgets_BorderStatic, wxT("Stati&c\tCtrl-Shift-3"));
    menuBorders->AppendRadioItem(Widgets_BorderRaised, wxT("&Raised\tCtrl-Shift-4"));
    menuBorders->AppendRadioItem(Widgets_BorderSunken, wxT("S&unken\tCtrl-Shift-5"));
    menuWidget->AppendSubMenu(menuBorders, wxT("Set &border"));

    menuWidget->AppendSeparator();
    menuWidget->AppendCheckItem(Widgets_GlobalBusyCursor,
                                wxT("Toggle &global busy cursor\tCtrl-Shift-U"));
    menuWidget->AppendCheckItem(Widgets_BusyCursor,
                                wxT("Toggle b&usy cursor\tCtrl-U"));

    menuWidget->AppendSeparator();
    menuWidget->Append(wxID_EXIT, wxT("&Quit\tCtrl-Q"));
    mbar->Append(menuWidget, wxT("&Widget"));

    wxMenu *menuTextEntry = new wxMenu;
    menuTextEntry->AppendRadioItem(TextEntry_DisableAutoComplete,
                                   wxT("&Disable auto-completion"));
    menuTextEntry->AppendRadioItem(TextEntry_AutoCompleteFixed,
                                   wxT("Fixed-&list auto-completion"));
    menuTextEntry->AppendRadioItem(TextEntry_AutoCompleteFilenames,
                                   wxT("&Files names auto-completion"));
    menuTextEntry->AppendRadioItem(TextEntry_AutoCompleteCustom,
                                   wxT("&Custom auto-completion"));
    menuTextEntry->AppendSeparator();
    menuTextEntry->Append(TextEntry_SetHint, "Set help &hint");

    mbar->Append(menuTextEntry, wxT("&Text"));

    SetMenuBar(mbar);

    mbar->Check(Widgets_Enable, true);
#endif // wxUSE_MENUS

    // create controls
    m_panel = new wxPanel(this, wxID_ANY);

    wxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

    // we have 2 panes: book with pages demonstrating the controls in the
    // upper one and the log window with some buttons in the lower

    int style = wxBK_DEFAULT;
    // Uncomment to suppress page theme (draw in solid colour)
    //style |= wxNB_NOPAGETHEME;

    m_book = new WidgetsBookCtrl(m_panel, Widgets_BookCtrl,
                                 wxDefaultPosition, wxDefaultSize,
                                 style, "Widgets");

    InitBook();

#ifndef __WXHANDHELD__
    // the lower one only has the log listbox and a button to clear it
#if USE_LOG
    wxSizer *sizerDown = new wxStaticBoxSizer(
        new wxStaticBox( m_panel, wxID_ANY, wxT("&Log window") ),
        wxVERTICAL);

    m_lboxLog = new wxListBox(m_panel, wxID_ANY);
    sizerDown->Add(m_lboxLog, 1, wxGROW | wxALL, 5);
    sizerDown->SetMinSize(100, 150);
#else
    wxSizer *sizerDown = new wxBoxSizer(wxVERTICAL);
#endif // USE_LOG

    wxBoxSizer *sizerBtns = new wxBoxSizer(wxHORIZONTAL);
    wxButton *btn;
#if USE_LOG
    btn = new wxButton(m_panel, Widgets_ClearLog, wxT("Clear &log"));
    sizerBtns->Add(btn);
    sizerBtns->Add(10, 0); // spacer
#endif // USE_LOG
    btn = new wxButton(m_panel, Widgets_Quit, wxT("E&xit"));
    sizerBtns->Add(btn);
    sizerDown->Add(sizerBtns, 0, wxALL | wxALIGN_RIGHT, 5);

    // put everything together
    sizerTop->Add(m_book, 1, wxGROW | (wxALL & ~(wxTOP | wxBOTTOM)), 10);
    sizerTop->Add(0, 5, 0, wxGROW); // spacer in between
    sizerTop->Add(sizerDown, 0,  wxGROW | (wxALL & ~wxTOP), 10);

#else // !__WXHANDHELD__/__WXHANDHELD__

    sizerTop->Add(m_book, 1, wxGROW | wxALL );

#endif // __WXHANDHELD__

    m_panel->SetSizer(sizerTop);

    const wxSize sizeMin = m_panel->GetBestSize();
    if ( !sizeSet )
        SetClientSize(sizeMin);
    SetMinClientSize(sizeMin);

#if USE_LOG && !defined(__WXCOCOA__)
    // wxCocoa's listbox is too flakey to use for logging right now
    // now that everything is created we can redirect the log messages to the
    // listbox
    m_logTarget = new LboxLogger(m_lboxLog, wxLog::GetActiveTarget());
    wxLog::SetActiveTarget(m_logTarget);
#endif
}

void WidgetsFrame::InitBook()
{
#if USE_ICONS_IN_BOOK
    wxImageList *imageList = new wxImageList(ICON_SIZE, ICON_SIZE);

    wxImage img(sample_xpm);
    imageList->Add(wxBitmap(img.Scale(ICON_SIZE, ICON_SIZE)));
#else
    wxImageList *imageList = NULL;
#endif

#if !USE_TREEBOOK
    WidgetsBookCtrl *books[MAX_PAGES];
#endif

    ArrayWidgetsPage pages[MAX_PAGES];
    wxArrayString labels[MAX_PAGES];

    wxMenu *menuPages = new wxMenu;
    unsigned int nPage = 0, nFKey = 0;
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
            pages[cat].Add(page);

            labels[cat].Add(info->GetLabel());
            if ( cat == ALL_PAGE )
            {
                wxString radioLabel(info->GetLabel());
                nFKey++;
                if ( nFKey <= 12 )
                {
                    radioLabel << wxT("\tF" ) << nFKey;
                }

                menuPages->AppendRadioItem(
                            Widgets_GoToPage + nPage,
                            radioLabel
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

    GetMenuBar()->Append(menuPages, wxT("&Page"));

#if USE_ICONS_IN_BOOK
    m_book->AssignImageList(imageList);
#endif

    for ( cat = 0; cat < MAX_PAGES; cat++ )
    {
#if USE_TREEBOOK
        m_book->AddPage(NULL,WidgetsCategories[cat],false,0);
#else
        m_book->AddPage(books[cat],WidgetsCategories[cat],false,0);
#if USE_ICONS_IN_BOOK
        books[cat]->SetImageList(imageList);
#endif
#endif

        // now do add them
        size_t count = pages[cat].GetCount();
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

    Connect( wxID_ANY,
             wxEVT_COMMAND_WIDGETS_PAGE_CHANGED,
             wxWidgetsbookEventHandler(WidgetsFrame::OnPageChanged) );

    const bool pageSet = wxPersistentRegisterAndRestore(m_book);

#if USE_TREEBOOK
    // for treebook page #0 is empty parent page only so select the first page
    // with some contents
    if ( !pageSet )
        m_book->SetSelection(1);

    // but ensure that the top of the tree is shown nevertheless
    wxTreeCtrl * const tree = m_book->GetTreeCtrl();

    wxTreeItemIdValue cookie;
    tree->EnsureVisible(tree->GetFirstChild(tree->GetRootItem(), cookie));
#else
    if ( !pageSet )
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
    wxCHECK_MSG( subBook, NULL, wxT("no WidgetsBookCtrl?") );

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
    WidgetsPage *page = CurrentPage();
    if ( page->GetChildren().empty() )
    {
        wxWindowUpdateLocker noUpdates(page);
        page->CreateContent();
        //page->Layout();
        page->GetSizer()->Fit(page);

        WidgetsBookCtrl *book = wxStaticCast(page->GetParent(), WidgetsBookCtrl);
        wxSize size;
        for ( size_t i = 0; i < book->GetPageCount(); ++i )
        {
            wxWindow *page = book->GetPage(i);
            if ( page )
            {
                size.IncTo(page->GetSize());
            }
        }
        page->SetSize(size);
    }

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
    static wxString s_tip = wxT("This is a tooltip");

    wxTextEntryDialog dialog
                      (
                        this,
                        wxT("Tooltip text (may use \\n, leave empty to remove): "),
                        wxT("Widgets sample"),
                        s_tip
                      );

    if ( dialog.ShowModal() != wxID_OK )
        return;

    s_tip = dialog.GetValue();
    s_tip.Replace(wxT("\\n"), wxT("\n"));

    WidgetsPage *page = CurrentPage();

    const Widgets widgets = page->GetWidgets();
    for ( Widgets::const_iterator it = widgets.begin();
          it != widgets.end();
          ++it )
    {
        (*it)->SetToolTip(s_tip);
    }
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

    if (!m_colFg.Ok())
        m_colFg = page->GetForegroundColour();

    wxColour col = GetColourFromUser(this, m_colFg);
    if ( !col.Ok() )
        return;

    m_colFg = col;

    const Widgets widgets = page->GetWidgets();
    for ( Widgets::const_iterator it = widgets.begin();
          it != widgets.end();
          ++it )
    {
        (*it)->SetForegroundColour(m_colFg);
        (*it)->Refresh();
    }
}

void WidgetsFrame::OnSetBgCol(wxCommandEvent& WXUNUSED(event))
{
    WidgetsPage *page = CurrentPage();

    if ( !m_colBg.Ok() )
        m_colBg = page->GetBackgroundColour();

    wxColour col = GetColourFromUser(this, m_colBg);
    if ( !col.Ok() )
        return;

    m_colBg = col;

    const Widgets widgets = page->GetWidgets();
    for ( Widgets::const_iterator it = widgets.begin();
          it != widgets.end();
          ++it )
    {
        (*it)->SetBackgroundColour(m_colBg);
        (*it)->Refresh();
    }
}

void WidgetsFrame::OnSetPageBg(wxCommandEvent& WXUNUSED(event))
{
    wxColour col = GetColourFromUser(this, GetBackgroundColour());
    if ( !col.Ok() )
        return;

    CurrentPage()->SetBackgroundColour(col);
    CurrentPage()->Refresh();
}

void WidgetsFrame::OnSetFont(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_FONTDLG
    WidgetsPage *page = CurrentPage();

    if (!m_font.Ok())
        m_font = page->GetFont();

    wxFont font = wxGetFontFromUser(this, m_font);
    if ( !font.Ok() )
        return;

    m_font = font;

    const Widgets widgets = page->GetWidgets();
    for ( Widgets::const_iterator it = widgets.begin();
          it != widgets.end();
          ++it )
    {
        (*it)->SetFont(m_font);
        (*it)->Refresh();
    }
#else
    wxLogMessage(wxT("Font selection dialog not available in current build."));
#endif
}

void WidgetsFrame::OnEnable(wxCommandEvent& event)
{
    const Widgets widgets = CurrentPage()->GetWidgets();
    for ( Widgets::const_iterator it = widgets.begin();
          it != widgets.end();
          ++it )
    {
        (*it)->Enable(event.IsChecked());
    }
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
            wxFAIL_MSG( wxT("unknown border style") );
            // fall through

        case Widgets_BorderDefault: border = wxBORDER_DEFAULT; break;
    }

    WidgetsPage::ms_defaultFlags &= ~wxBORDER_MASK;
    WidgetsPage::ms_defaultFlags |= border;

    WidgetsPage *page = CurrentPage();

    page->RecreateWidget();
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
    wxCursor cursor(*(event.IsChecked() ? wxHOURGLASS_CURSOR
                                        : wxSTANDARD_CURSOR));

    const Widgets widgets = CurrentPage()->GetWidgets();
    for ( Widgets::const_iterator it = widgets.begin();
          it != widgets.end();
          ++it )
    {
        (*it)->SetCursor(cursor);
    }
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

void WidgetsFrame::OnAutoCompleteCustom(wxCommandEvent& WXUNUSED(event))
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
        virtual void GetCompletions(const wxString& prefix, wxArrayString& res)
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


            // Normally it doesn't make sense to complete empty control, there
            // are too many choices and listing them all wouldn't be helpful.
            if ( prefix.empty() )
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
    };

    if ( entry->AutoComplete(new CustomTextCompleter) )
    {
        wxLogMessage("Enabled custom auto completer for \"NNN XX\" items "
                     "(where N is a digit and X is a letter).");
    }
    else
    {
        wxLogMessage("AutoComplete() failed.");
    }
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

WidgetsPageInfo::WidgetsPageInfo(Constructor ctor, const wxChar *label, int categories)
               : m_label(label)
               , m_categories(categories)
{
    m_ctor = ctor;

    m_next = NULL;

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
                        node_next = NULL;
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

int WidgetsPage::ms_defaultFlags = wxBORDER_DEFAULT;
WidgetsPageInfo *WidgetsPage::ms_widgetPages = NULL;

WidgetsPage::WidgetsPage(WidgetsBookCtrl *book,
                         wxImageList *imaglist,
                         const char *const icon[])
           : wxPanel(book, wxID_ANY,
                     wxDefaultPosition, wxDefaultSize,
                     wxNO_FULL_REPAINT_ON_RESIZE |
                     wxCLIP_CHILDREN |
                     wxTAB_TRAVERSAL)
{
#if USE_ICONS_IN_BOOK
    imaglist->Add(wxBitmap(wxImage(icon).Scale(ICON_SIZE, ICON_SIZE)));
#else
    wxUnusedVar(imaglist);
    wxUnusedVar(icon);
#endif
}

wxSizer *WidgetsPage::CreateSizerWithText(wxControl *control,
                                          wxWindowID id,
                                          wxTextCtrl **ppText)
{
    wxSizer *sizerRow = new wxBoxSizer(wxHORIZONTAL);
    wxTextCtrl *text = new wxTextCtrl(this, id, wxEmptyString,
        wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);

    sizerRow->Add(control, 0, wxRIGHT | wxALIGN_CENTRE_VERTICAL, 5);
    sizerRow->Add(text, 1, wxLEFT | wxALIGN_CENTRE_VERTICAL, 5);

    if ( ppText )
        *ppText = text;

    return sizerRow;
}

// create a sizer containing a label and a text ctrl
wxSizer *WidgetsPage::CreateSizerWithTextAndLabel(const wxString& label,
                                                  wxWindowID id,
                                                  wxTextCtrl **ppText)
{
    return CreateSizerWithText(new wxStaticText(this, wxID_ANY, label),
        id, ppText);
}

// create a sizer containing a button and a text ctrl
wxSizer *WidgetsPage::CreateSizerWithTextAndButton(wxWindowID idBtn,
                                                   const wxString& label,
                                                   wxWindowID id,
                                                   wxTextCtrl **ppText)
{
    return CreateSizerWithText(new wxButton(this, idBtn, label), id, ppText);
}

wxCheckBox *WidgetsPage::CreateCheckBoxAndAddToSizer(wxSizer *sizer,
                                                     const wxString& label,
                                                     wxWindowID id)
{
    wxCheckBox *checkbox = new wxCheckBox(this, id, label);
    sizer->Add(checkbox, 0, wxLEFT | wxRIGHT, 5);
    sizer->Add(0, 2, 0, wxGROW); // spacer

    return checkbox;
}
