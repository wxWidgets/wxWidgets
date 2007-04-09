/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        samples/widgets/widgets.cpp
// Purpose:     Sample showing most of the simple wxWidgets widgets
// Author:      Vadim Zeitlin
// Created:     27.03.01
// Id:          $Id$
// Copyright:   (c) 2001 Vadim Zeitlin
// License:     wxWindows license
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
    Widgets_GoToPageLast = Widgets_GoToPage + 100
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
    void OnSetFont(wxCommandEvent& event);
    void OnEnable(wxCommandEvent& event);
    void OnSetBorder(wxCommandEvent& event);

    void OnToggleGlobalBusyCursor(wxCommandEvent& event);
    void OnToggleBusyCursor(wxCommandEvent& event);
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
    virtual void DoLog(wxLogLevel level, const wxChar *szString, time_t t)
    {
        // don't put trace messages into listbox or we can get into infinite
        // recursion
        if ( level == wxLOG_Trace )
        {
            if ( m_logOld )
            {
                // cast is needed to call protected method
                ((LboxLogger *)m_logOld)->DoLog(level, szString, t);
            }
        }
        else
        {
            wxLog::DoLog(level, szString, t);
        }
    }

    virtual void DoLogString(const wxChar *szString, time_t WXUNUSED(t))
    {
        wxString msg;
        TimeStamp(&msg);
        msg += szString;

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
    EVT_MENU(Widgets_SetFont,     WidgetsFrame::OnSetFont)
    EVT_MENU(Widgets_Enable,      WidgetsFrame::OnEnable)

    EVT_MENU_RANGE(Widgets_BorderNone, Widgets_BorderDefault,
                   WidgetsFrame::OnSetBorder)

    EVT_MENU(Widgets_GlobalBusyCursor,  WidgetsFrame::OnToggleGlobalBusyCursor)
    EVT_MENU(Widgets_BusyCursor,        WidgetsFrame::OnToggleBusyCursor)

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

    // the reason for having these ifdef's is that I often run two copies of
    // this sample side by side and it is useful to see which one is which
    wxString title;
#if defined(__WXUNIVERSAL__)
    title = _T("wxUniv/");
#endif

#if defined(__WXMSW__)
    title += _T("wxMSW");
#elif defined(__WXGTK__)
    title += _T("wxGTK");
#elif defined(__WXMAC__)
    title += _T("wxMAC");
#elif defined(__WXMOTIF__)
    title += _T("wxMOTIF");
#else
    title += _T("wxWidgets");
#endif

    wxFrame *frame = new WidgetsFrame(title + _T(" widgets demo"));
    frame->Show();

    //wxLog::AddTraceMask(_T("listbox"));
    //wxLog::AddTraceMask(_T("scrollbar"));
    //wxLog::AddTraceMask(_T("focus"));

    return true;
}

// ----------------------------------------------------------------------------
// WidgetsFrame construction
// ----------------------------------------------------------------------------

WidgetsFrame::WidgetsFrame(const wxString& title)
            : wxFrame(NULL, wxID_ANY, title)
{
    // set the frame icon
    SetIcon(wxICON(sample));

    // init everything
#if USE_LOG
    m_lboxLog = (wxListBox *)NULL;
    m_logTarget = (wxLog *)NULL;
#endif // USE_LOG
    m_book = (WidgetsBookCtrl *)NULL;

#if wxUSE_MENUS
    // create the menubar
    wxMenuBar *mbar = new wxMenuBar;
    wxMenu *menuWidget = new wxMenu;
#if wxUSE_TOOLTIPS
    menuWidget->Append(Widgets_SetTooltip, _T("Set &tooltip...\tCtrl-T"));
    menuWidget->AppendSeparator();
#endif // wxUSE_TOOLTIPS
    menuWidget->Append(Widgets_SetFgColour, _T("Set &foreground...\tCtrl-F"));
    menuWidget->Append(Widgets_SetBgColour, _T("Set &background...\tCtrl-B"));
    menuWidget->Append(Widgets_SetFont,     _T("Set f&ont...\tCtrl-O"));
    menuWidget->AppendCheckItem(Widgets_Enable,  _T("&Enable/disable\tCtrl-E"));

    wxMenu *menuBorders = new wxMenu;
    menuBorders->AppendRadioItem(Widgets_BorderDefault, _T("De&fault\tCtrl-Shift-9"));
    menuBorders->AppendRadioItem(Widgets_BorderNone,   _T("&None\tCtrl-Shift-0"));
    menuBorders->AppendRadioItem(Widgets_BorderSimple, _T("&Simple\tCtrl-Shift-1"));
    menuBorders->AppendRadioItem(Widgets_BorderDouble, _T("&Double\tCtrl-Shift-2"));
    menuBorders->AppendRadioItem(Widgets_BorderStatic, _T("Stati&c\tCtrl-Shift-3"));
    menuBorders->AppendRadioItem(Widgets_BorderRaised, _T("&Raised\tCtrl-Shift-4"));
    menuBorders->AppendRadioItem(Widgets_BorderSunken, _T("S&unken\tCtrl-Shift-5"));
    menuWidget->AppendSubMenu(menuBorders, _T("Set &border"));

    menuWidget->AppendSeparator();
    menuWidget->AppendCheckItem(Widgets_GlobalBusyCursor,
                                _T("Toggle &global busy cursor\tCtrl-Shift-U"));
    menuWidget->AppendCheckItem(Widgets_BusyCursor,
                                _T("Toggle b&usy cursor\tCtrl-U"));

    menuWidget->AppendSeparator();
    menuWidget->Append(wxID_EXIT, _T("&Quit\tCtrl-Q"));
    mbar->Append(menuWidget, _T("&Widget"));
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

    m_book = new WidgetsBookCtrl(m_panel, Widgets_BookCtrl, wxDefaultPosition,
#ifdef __WXMOTIF__
        wxSize(500, wxDefaultCoord), // under Motif, height is a function of the width...
#else
        wxDefaultSize,
#endif
        style);
    InitBook();

#ifndef __WXHANDHELD__
    // the lower one only has the log listbox and a button to clear it
#if USE_LOG
    wxSizer *sizerDown = new wxStaticBoxSizer(
        new wxStaticBox( m_panel, wxID_ANY, _T("&Log window") ),
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
    btn = new wxButton(m_panel, Widgets_ClearLog, _T("Clear &log"));
    sizerBtns->Add(btn);
    sizerBtns->Add(10, 0); // spacer
#endif // USE_LOG
    btn = new wxButton(m_panel, Widgets_Quit, _T("E&xit"));
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

    sizerTop->Fit(this);
    sizerTop->SetSizeHints(this);

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

    GetMenuBar()->Append(menuPages, _T("&Page"));

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

#if USE_TREEBOOK
    // for treebook page #0 is empty parent page only so select the first page
    // with some contents
    m_book->SetSelection(1);

    // but ensure that the top of the tree is shown nevertheless
    wxTreeCtrl * const tree = m_book->GetTreeCtrl();

    wxTreeItemIdValue cookie;
    tree->EnsureVisible(tree->GetFirstChild(tree->GetRootItem(), cookie));
#else
    // for other books set selection twice to force connected event handler
    // to force lazy creation of initial visible content
    m_book->SetSelection(1);
    m_book->SetSelection(0);
#endif // USE_TREEBOOK
}

WidgetsPage *WidgetsFrame::CurrentPage()
{
    wxWindow *page = m_book->GetCurrentPage();

#if !USE_TREEBOOK
    WidgetsBookCtrl *subBook = wxStaticCast(page, WidgetsBookCtrl);
    wxCHECK_MSG( subBook, NULL, _T("no WidgetsBookCtrl?") );

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
    static wxString s_tip = _T("This is a tooltip");

    wxTextEntryDialog dialog
                      (
                        this,
                        _T("Tooltip text (may use \\n, leave empty to remove): "),
                        _T("Widgets sample"),
                        s_tip
                      );

    if ( dialog.ShowModal() != wxID_OK )
        return;

    s_tip = dialog.GetValue();
    s_tip.Replace(_T("\\n"), _T("\n"));

    WidgetsPage *page = CurrentPage();

    page->GetWidget()->SetToolTip(s_tip);

    wxControl *ctrl2 = page->GetWidget2();
    if ( ctrl2 )
        ctrl2->SetToolTip(s_tip);
}

#endif // wxUSE_TOOLTIPS

void WidgetsFrame::OnSetFgCol(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_COLOURDLG
    // allow for debugging the default colour the first time this is called
    WidgetsPage *page = CurrentPage();

    if (!m_colFg.Ok())
        m_colFg = page->GetForegroundColour();

    wxColour col = wxGetColourFromUser(this, m_colFg);
    if ( !col.Ok() )
        return;

    m_colFg = col;

    page->GetWidget()->SetForegroundColour(m_colFg);
    page->GetWidget()->Refresh();

    wxControl *ctrl2 = page->GetWidget2();
    if ( ctrl2 )
    {
        ctrl2->SetForegroundColour(m_colFg);
        ctrl2->Refresh();
    }
#else
    wxLogMessage(_T("Colour selection dialog not available in current build."));
#endif
}

void WidgetsFrame::OnSetBgCol(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_COLOURDLG
    WidgetsPage *page = CurrentPage();

    if ( !m_colBg.Ok() )
        m_colBg = page->GetBackgroundColour();

    wxColour col = wxGetColourFromUser(this, m_colBg);
    if ( !col.Ok() )
        return;

    m_colBg = col;

    page->GetWidget()->SetBackgroundColour(m_colBg);
    page->GetWidget()->Refresh();

    wxControl *ctrl2 = page->GetWidget2();
    if ( ctrl2 )
    {
        ctrl2->SetBackgroundColour(m_colFg);
        ctrl2->Refresh();
    }
#else
    wxLogMessage(_T("Colour selection dialog not available in current build."));
#endif
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

    page->GetWidget()->SetFont(m_font);
    page->GetWidget()->Refresh();

    wxControl *ctrl2 = page->GetWidget2();
    if ( ctrl2 )
    {
        ctrl2->SetFont(m_font);
        ctrl2->Refresh();
    }
#else
    wxLogMessage(_T("Font selection dialog not available in current build."));
#endif
}

void WidgetsFrame::OnEnable(wxCommandEvent& event)
{
    CurrentPage()->GetWidget()->Enable(event.IsChecked());
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
            wxFAIL_MSG( _T("unknown border style") );
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
    CurrentPage()->GetWidget()->SetCursor(*(event.IsChecked()
                                                ? wxHOURGLASS_CURSOR
                                                : wxSTANDARD_CURSOR));
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
                         char* icon[])
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
