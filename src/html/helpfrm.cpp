/////////////////////////////////////////////////////////////////////////////
// Name:        helpfrm.cpp
// Purpose:     wxHtmlHelpFrame
// Notes:       Based on htmlhelp.cpp, implementing a monolithic
//              HTML Help controller class,  by Vaclav Slavik
// Author:      Harm van der Heijden and Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) Harm van der Heijden and Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "helpfrm.h"
#endif

// For compilers that support precompilation, includes "wx.h"

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_WXHTML_HELP

#ifndef WXPRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"

    #include "wx/object.h"
    #include "wx/sizer.h"

    #include "wx/bmpbuttn.h"
    #include "wx/statbox.h"
    #include "wx/radiobox.h"
#endif // WXPRECOMP

#ifdef __WXMAC__
    #include "wx/menu.h"
    #include "wx/msgdlg.h"
#endif

#include "wx/html/helpfrm.h"
#include "wx/html/helpctrl.h"
#include "wx/textctrl.h"
#include "wx/notebook.h"
#include "wx/imaglist.h"
#include "wx/treectrl.h"
#include "wx/tokenzr.h"
#include "wx/wfstream.h"
#include "wx/html/htmlwin.h"
#include "wx/busyinfo.h"
#include "wx/progdlg.h"
#include "wx/toolbar.h"
#include "wx/fontenum.h"
#include "wx/stream.h"
#include "wx/filedlg.h"
#include "wx/artprov.h"
#include "wx/spinctrl.h"

// what is considered "small index"?
#define INDEX_IS_SMALL 100

/* Motif defines this as a macro */
#ifdef Below
#undef Below
#endif

//--------------------------------------------------------------------------
// wxHtmlHelpTreeItemData (private)
//--------------------------------------------------------------------------

class wxHtmlHelpTreeItemData : public wxTreeItemData
{
    public:
#if defined(__VISAGECPP__)
//  VA needs a default ctor for some reason....
        wxHtmlHelpTreeItemData() : wxTreeItemData()
            { m_Id = 0; }
#endif
        wxHtmlHelpTreeItemData(int id) : wxTreeItemData()
            { m_Id = id;}

        int m_Id;
};


//--------------------------------------------------------------------------
// wxHtmlHelpHashData (private)
//--------------------------------------------------------------------------

class wxHtmlHelpHashData : public wxObject
{
    public:
        wxHtmlHelpHashData(int index, wxTreeItemId id) : wxObject()
            { m_Index = index; m_Id = id;}
        ~wxHtmlHelpHashData() {}

        int m_Index;
        wxTreeItemId m_Id;
};


//--------------------------------------------------------------------------
// wxHtmlHelpHtmlWindow (private)
//--------------------------------------------------------------------------

class wxHtmlHelpHtmlWindow : public wxHtmlWindow
{
    public:
        wxHtmlHelpHtmlWindow(wxHtmlHelpFrame *fr, wxWindow *parent) : wxHtmlWindow(parent), m_Frame(fr) {}

        virtual void OnLinkClicked(const wxHtmlLinkInfo& link)
        {
            wxHtmlWindow::OnLinkClicked(link);
            const wxMouseEvent *e = link.GetEvent();
            if (e == NULL || e->LeftUp())
                m_Frame->NotifyPageChanged();
        }

    private:
        wxHtmlHelpFrame *m_Frame;

    DECLARE_NO_COPY_CLASS(wxHtmlHelpHtmlWindow)
};



//---------------------------------------------------------------------------
// wxHtmlHelpFrame
//---------------------------------------------------------------------------

// Command IDs :
enum
{
    //wxID_HTML_HELPFRAME = wxID_HIGHEST + 1,
    wxID_HTML_PANEL = wxID_HIGHEST + 2,
    wxID_HTML_BACK,
    wxID_HTML_FORWARD,
    wxID_HTML_UPNODE,
    wxID_HTML_UP,
    wxID_HTML_DOWN,
    wxID_HTML_PRINT,
    wxID_HTML_OPENFILE,
    wxID_HTML_OPTIONS,
    wxID_HTML_BOOKMARKSLIST,
    wxID_HTML_BOOKMARKSADD,
    wxID_HTML_BOOKMARKSREMOVE,
    wxID_HTML_TREECTRL,
    wxID_HTML_INDEXPAGE,
    wxID_HTML_INDEXLIST,
    wxID_HTML_INDEXTEXT,
    wxID_HTML_INDEXBUTTON,
    wxID_HTML_INDEXBUTTONALL,
    wxID_HTML_NOTEBOOK,
    wxID_HTML_SEARCHPAGE,
    wxID_HTML_SEARCHTEXT,
    wxID_HTML_SEARCHLIST,
    wxID_HTML_SEARCHBUTTON,
    wxID_HTML_SEARCHCHOICE,
    wxID_HTML_COUNTINFO
};


IMPLEMENT_DYNAMIC_CLASS(wxHtmlHelpFrame, wxFrame)

wxHtmlHelpFrame::wxHtmlHelpFrame(wxWindow* parent, wxWindowID id, const wxString& title,
                                 int style, wxHtmlHelpData* data)
{
    Init(data);
    Create(parent, id, title, style);
}

void wxHtmlHelpFrame::Init(wxHtmlHelpData* data)
{
    if (data)
    {
        m_Data = data;
        m_DataCreated = FALSE;
    } else
    {
        m_Data = new wxHtmlHelpData();
        m_DataCreated = TRUE;
    }

    m_ContentsBox = NULL;
    m_IndexList = NULL;
    m_IndexButton = NULL;
    m_IndexButtonAll = NULL;
    m_IndexText = NULL;
    m_SearchList = NULL;
    m_SearchButton = NULL;
    m_SearchText = NULL;
    m_SearchChoice = NULL;
    m_IndexCountInfo = NULL;
    m_Splitter = NULL;
    m_NavigPan = NULL;
    m_NavigNotebook = NULL;
    m_HtmlWin = NULL;
    m_Bookmarks = NULL;
    m_SearchCaseSensitive = NULL;
    m_SearchWholeWords = NULL;

    m_Config = NULL;
    m_ConfigRoot = wxEmptyString;

    m_Cfg.x = m_Cfg.y = 0;
    m_Cfg.w = 700;
    m_Cfg.h = 480;
    m_Cfg.sashpos = 240;
    m_Cfg.navig_on = TRUE;

    m_NormalFonts = m_FixedFonts = NULL;
    m_NormalFace = m_FixedFace = wxEmptyString;
#ifdef __WXMSW__
    m_FontSize = 10;
#else
    m_FontSize = 14;
#endif

#if wxUSE_PRINTING_ARCHITECTURE
    m_Printer = NULL;
#endif

    m_PagesHash = NULL;
    m_UpdateContents = TRUE;
    m_helpController = (wxHelpControllerBase*) NULL;
}

// Create: builds the GUI components.
// with the style flag it's possible to toggle the toolbar, contents, index and search
// controls.
// m_HtmlWin will *always* be created, but it's important to realize that
// m_ContentsBox, m_IndexList, m_SearchList, m_SearchButton, m_SearchText and
// m_SearchButton may be NULL.
// moreover, if no contents, index or searchpage is needed, m_Splitter and
// m_NavigPan will be NULL too (with m_HtmlWin directly connected to the frame)

bool wxHtmlHelpFrame::Create(wxWindow* parent, wxWindowID id,
                             const wxString& WXUNUSED(title), int style)
{
    m_hfStyle = style;

    wxImageList *ContentsImageList = new wxImageList(16, 16);
    ContentsImageList->Add(wxArtProvider::GetIcon(wxART_HELP_BOOK, wxART_HELP_BROWSER));
    ContentsImageList->Add(wxArtProvider::GetIcon(wxART_HELP_FOLDER, wxART_HELP_BROWSER));
    ContentsImageList->Add(wxArtProvider::GetIcon(wxART_HELP_PAGE, wxART_HELP_BROWSER));

    // Do the config in two steps. We read the HtmlWindow customization after we
    // create the window.
    if (m_Config)
        ReadCustomization(m_Config, m_ConfigRoot);

    wxFrame::Create(parent, id, _("Help"), 
                    wxPoint(m_Cfg.x, m_Cfg.y), wxSize(m_Cfg.w, m_Cfg.h), 
                    wxDEFAULT_FRAME_STYLE, wxT("wxHtmlHelp"));

    GetPosition(&m_Cfg.x, &m_Cfg.y);

    SetIcon(wxArtProvider::GetIcon(wxART_HELP, wxART_HELP_BROWSER));

    // On the Mac, each modeless frame must have a menubar.
    // TODO: add more menu items, and perhaps add a style to show
    // the menubar: compulsory on the Mac, optional elsewhere.
#ifdef __WXMAC__
    wxMenuBar* menuBar = new wxMenuBar;

    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(wxID_HTML_OPENFILE, _("&Open..."));
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_CLOSE, _("&Close"));

    wxMenu* helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT, _("&About..."));

    menuBar->Append(fileMenu,_("File"));
    menuBar->Append(helpMenu,_("Help"));
    SetMenuBar(menuBar);
#endif

    int notebook_page = 0;

    CreateStatusBar();

    // toolbar?
    if (style & (wxHF_TOOLBAR | wxHF_FLAT_TOOLBAR))
    {
        wxToolBar *toolBar = CreateToolBar(wxNO_BORDER | wxTB_HORIZONTAL |
                                           wxTB_DOCKABLE |
                                           (style & wxHF_FLAT_TOOLBAR ? wxTB_FLAT : 0));
        toolBar->SetMargins( 2, 2 );
        AddToolbarButtons(toolBar, style);
        toolBar->Realize();
    }

    wxSizer *navigSizer = NULL;

    if (style & (wxHF_CONTENTS | wxHF_INDEX | wxHF_SEARCH))
    {
        // traditional help controller; splitter window with html page on the
        // right and a notebook containing various pages on the left
        m_Splitter = new wxSplitterWindow(this);

        m_HtmlWin = new wxHtmlHelpHtmlWindow(this, m_Splitter);
        m_NavigPan = new wxPanel(m_Splitter, -1);
        m_NavigNotebook = new wxNotebook(m_NavigPan, wxID_HTML_NOTEBOOK,
                                         wxDefaultPosition, wxDefaultSize);
        wxNotebookSizer *nbs = new wxNotebookSizer(m_NavigNotebook);
        
        navigSizer = new wxBoxSizer(wxVERTICAL);
        navigSizer->Add(nbs, 1, wxEXPAND);

        m_NavigPan->SetAutoLayout(TRUE);
        m_NavigPan->SetSizer(navigSizer);
    }
    else
    { // only html window, no notebook with index,contents etc
        m_HtmlWin = new wxHtmlWindow(this);
    }

    m_HtmlWin->SetRelatedFrame(this, m_TitleFormat);
    m_HtmlWin->SetRelatedStatusBar(0);
    if ( m_Config )
        m_HtmlWin->ReadCustomization(m_Config, m_ConfigRoot);

    // contents tree panel?
    if ( style & wxHF_CONTENTS )
    {
        wxWindow *dummy = new wxPanel(m_NavigNotebook, wxID_HTML_INDEXPAGE);
        wxSizer *topsizer = new wxBoxSizer(wxVERTICAL);
        
        topsizer->Add(0, 10);
        
        dummy->SetAutoLayout(TRUE);
        dummy->SetSizer(topsizer);

        if ( style & wxHF_BOOKMARKS )
        {
            m_Bookmarks = new wxComboBox(dummy, wxID_HTML_BOOKMARKSLIST, 
                                         wxEmptyString,
                                         wxDefaultPosition, wxDefaultSize, 
                                         0, NULL, wxCB_READONLY | wxCB_SORT);
            m_Bookmarks->Append(_("(bookmarks)"));
            for (unsigned i = 0; i < m_BookmarksNames.GetCount(); i++)
                m_Bookmarks->Append(m_BookmarksNames[i]);
            m_Bookmarks->SetSelection(0);

            wxBitmapButton *bmpbt1, *bmpbt2;
            bmpbt1 = new wxBitmapButton(dummy, wxID_HTML_BOOKMARKSADD, 
                                 wxArtProvider::GetBitmap(wxART_ADD_BOOKMARK, 
                                                          wxART_HELP_BROWSER));
            bmpbt2 = new wxBitmapButton(dummy, wxID_HTML_BOOKMARKSREMOVE, 
                                 wxArtProvider::GetBitmap(wxART_DEL_BOOKMARK, 
                                                          wxART_HELP_BROWSER));
#if wxUSE_TOOLTIPS
            bmpbt1->SetToolTip(_("Add current page to bookmarks"));
            bmpbt2->SetToolTip(_("Remove current page from bookmarks"));
#endif // wxUSE_TOOLTIPS

            wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
            
            sizer->Add(m_Bookmarks, 1, wxALIGN_CENTRE_VERTICAL | wxRIGHT, 5);
            sizer->Add(bmpbt1, 0, wxALIGN_CENTRE_VERTICAL | wxRIGHT, 2);
            sizer->Add(bmpbt2, 0, wxALIGN_CENTRE_VERTICAL, 0);
            
            topsizer->Add(sizer, 0, wxEXPAND | wxLEFT | wxBOTTOM | wxRIGHT, 10);
        }

        m_ContentsBox = new wxTreeCtrl(dummy, wxID_HTML_TREECTRL,
                                       wxDefaultPosition, wxDefaultSize,
                                       wxSUNKEN_BORDER | 
                                       wxTR_HAS_BUTTONS | wxTR_HIDE_ROOT |
                                       wxTR_LINES_AT_ROOT);

        m_ContentsBox->AssignImageList(ContentsImageList);
        
        topsizer->Add(m_ContentsBox, 1, wxEXPAND | wxLEFT | wxBOTTOM | wxRIGHT, 2);

        m_NavigNotebook->AddPage(dummy, _("Contents"));
        m_ContentsPage = notebook_page++;
    }

    // index listbox panel?
    if ( style & wxHF_INDEX )
    {
        wxWindow *dummy = new wxPanel(m_NavigNotebook, wxID_HTML_INDEXPAGE);       
        wxSizer *topsizer = new wxBoxSizer(wxVERTICAL);

        dummy->SetAutoLayout(TRUE);
        dummy->SetSizer(topsizer);

        m_IndexText = new wxTextCtrl(dummy, wxID_HTML_INDEXTEXT, wxEmptyString, 
                                     wxDefaultPosition, wxDefaultSize, 
                                     wxTE_PROCESS_ENTER);
        m_IndexButton = new wxButton(dummy, wxID_HTML_INDEXBUTTON, _("Find"));
        m_IndexButtonAll = new wxButton(dummy, wxID_HTML_INDEXBUTTONALL, 
                                        _("Show all"));
        m_IndexCountInfo = new wxStaticText(dummy, wxID_HTML_COUNTINFO, 
                                            wxEmptyString, wxDefaultPosition,
                                            wxDefaultSize, 
                                            wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
        m_IndexList = new wxListBox(dummy, wxID_HTML_INDEXLIST, 
                                    wxDefaultPosition, wxDefaultSize, 
                                    0, NULL, wxLB_SINGLE);

#if wxUSE_TOOLTIPS
        m_IndexButton->SetToolTip(_("Display all index items that contain given substring. Search is case insensitive."));
        m_IndexButtonAll->SetToolTip(_("Show all items in index"));
#endif //wxUSE_TOOLTIPS

        topsizer->Add(m_IndexText, 0, wxEXPAND | wxALL, 10);
        wxSizer *btsizer = new wxBoxSizer(wxHORIZONTAL);
        btsizer->Add(m_IndexButton, 0, wxRIGHT, 2);
        btsizer->Add(m_IndexButtonAll);
        topsizer->Add(btsizer, 0, 
                      wxALIGN_RIGHT | wxLEFT | wxRIGHT | wxBOTTOM, 10);
        topsizer->Add(m_IndexCountInfo, 0, wxEXPAND | wxLEFT | wxRIGHT, 2);
        topsizer->Add(m_IndexList, 1, wxEXPAND | wxALL, 2);

        m_NavigNotebook->AddPage(dummy, _("Index"));
        m_IndexPage = notebook_page++;
    }

    // search list panel?
    if ( style & wxHF_SEARCH )
    {
        wxWindow *dummy = new wxPanel(m_NavigNotebook, wxID_HTML_INDEXPAGE);       
        wxSizer *sizer = new wxBoxSizer(wxVERTICAL);

        dummy->SetAutoLayout(TRUE);
        dummy->SetSizer(sizer);

        m_SearchText = new wxTextCtrl(dummy, wxID_HTML_SEARCHTEXT, 
                                      wxEmptyString, 
                                      wxDefaultPosition, wxDefaultSize, 
                                      wxTE_PROCESS_ENTER);
        m_SearchChoice = new wxChoice(dummy, wxID_HTML_SEARCHCHOICE, 
                                      wxDefaultPosition, wxDefaultSize);
        m_SearchCaseSensitive = new wxCheckBox(dummy, -1, _("Case sensitive"));
        m_SearchWholeWords = new wxCheckBox(dummy, -1, _("Whole words only"));
        m_SearchButton = new wxButton(dummy, wxID_HTML_SEARCHBUTTON, _("Search"));
#if wxUSE_TOOLTIPS
        m_SearchButton->SetToolTip(_("Search contents of help book(s) for all occurences of the text you typed above"));
#endif //wxUSE_TOOLTIPS
        m_SearchList = new wxListBox(dummy, wxID_HTML_SEARCHLIST, 
                                     wxDefaultPosition, wxDefaultSize, 
                                     0, NULL, wxLB_SINGLE);
                                     
        sizer->Add(m_SearchText, 0, wxEXPAND | wxALL, 10);
        sizer->Add(m_SearchChoice, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
        sizer->Add(m_SearchCaseSensitive, 0, wxLEFT | wxRIGHT, 10);
        sizer->Add(m_SearchWholeWords, 0, wxLEFT | wxRIGHT, 10);
        sizer->Add(m_SearchButton, 0, wxALL | wxALIGN_RIGHT, 8);
        sizer->Add(m_SearchList, 1, wxALL | wxEXPAND, 2);

        m_NavigNotebook->AddPage(dummy, _("Search"));
        m_SearchPage = notebook_page++;
    }

    m_HtmlWin->Show(TRUE);

    RefreshLists();

    if ( navigSizer )
    {
        navigSizer->SetSizeHints(m_NavigPan);
        m_NavigPan->Layout();
    }

    // showtime
    if ( m_NavigPan && m_Splitter )
    {
        m_Splitter->SetMinimumPaneSize(20);
        if ( m_Cfg.navig_on )
            m_Splitter->SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);

        if ( m_Cfg.navig_on )
        {
            m_NavigPan->Show(TRUE);
            m_Splitter->SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);
        }
        else
        {
            m_NavigPan->Show(FALSE);
            m_Splitter->Initialize(m_HtmlWin);
        }
    }
    
    return TRUE;
}

wxHtmlHelpFrame::~wxHtmlHelpFrame()
{
    // PopEventHandler(); // wxhtmlhelpcontroller (not any more!)
    if (m_DataCreated)
        delete m_Data;
    if (m_NormalFonts) delete m_NormalFonts;
    if (m_FixedFonts) delete m_FixedFonts;
    if (m_PagesHash) 
    {
        WX_CLEAR_HASH_TABLE(*m_PagesHash);
        delete m_PagesHash;
    }
}


void wxHtmlHelpFrame::AddToolbarButtons(wxToolBar *toolBar, int style)
{
    wxBitmap wpanelBitmap = 
        wxArtProvider::GetBitmap(wxART_HELP_SIDE_PANEL, wxART_HELP_BROWSER);
    wxBitmap wbackBitmap = 
        wxArtProvider::GetBitmap(wxART_GO_BACK, wxART_HELP_BROWSER);
    wxBitmap wforwardBitmap = 
        wxArtProvider::GetBitmap(wxART_GO_FORWARD, wxART_HELP_BROWSER);
    wxBitmap wupnodeBitmap = 
        wxArtProvider::GetBitmap(wxART_GO_TO_PARENT, wxART_HELP_BROWSER);
    wxBitmap wupBitmap = 
        wxArtProvider::GetBitmap(wxART_GO_UP, wxART_HELP_BROWSER);
    wxBitmap wdownBitmap = 
        wxArtProvider::GetBitmap(wxART_GO_DOWN, wxART_HELP_BROWSER);
    wxBitmap wopenBitmap = 
        wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_HELP_BROWSER);
    wxBitmap wprintBitmap = 
        wxArtProvider::GetBitmap(wxART_PRINT, wxART_HELP_BROWSER);
    wxBitmap woptionsBitmap = 
        wxArtProvider::GetBitmap(wxART_HELP_SETTINGS, wxART_HELP_BROWSER);

    wxASSERT_MSG( (wpanelBitmap.Ok() && wbackBitmap.Ok() &&
                   wforwardBitmap.Ok() && wupnodeBitmap.Ok() &&
                   wupBitmap.Ok() && wdownBitmap.Ok() &&
                   wopenBitmap.Ok() && wprintBitmap.Ok() &&
                   woptionsBitmap.Ok()),
                  wxT("One or more HTML help frame toolbar bitmap could not be loaded.")) ;


    toolBar->AddTool(wxID_HTML_PANEL, wpanelBitmap, wxNullBitmap,
                       FALSE, -1, -1, (wxObject *) NULL,
                       _("Show/hide navigation panel"));

    toolBar->AddSeparator();
    toolBar->AddTool(wxID_HTML_BACK, wbackBitmap, wxNullBitmap,
                       FALSE, -1, -1, (wxObject *) NULL,
                       _("Go back"));
    toolBar->AddTool(wxID_HTML_FORWARD, wforwardBitmap, wxNullBitmap,
                       FALSE, -1, -1, (wxObject *) NULL,
                       _("Go forward"));
    toolBar->AddSeparator();

    toolBar->AddTool(wxID_HTML_UPNODE, wupnodeBitmap, wxNullBitmap,
                       FALSE, -1, -1, (wxObject *) NULL,
                       _("Go one level up in document hierarchy"));
    toolBar->AddTool(wxID_HTML_UP, wupBitmap, wxNullBitmap,
                       FALSE, -1, -1, (wxObject *) NULL,
                       _("Previous page"));
    toolBar->AddTool(wxID_HTML_DOWN, wdownBitmap, wxNullBitmap,
                       FALSE, -1, -1, (wxObject *) NULL,
                       _("Next page"));

    if ((style & wxHF_PRINT) || (style & wxHF_OPEN_FILES))
        toolBar->AddSeparator();

    if (style & wxHF_OPEN_FILES)
        toolBar->AddTool(wxID_HTML_OPENFILE, wopenBitmap, wxNullBitmap,
                           FALSE, -1, -1, (wxObject *) NULL,
                           _("Open HTML document"));

#if wxUSE_PRINTING_ARCHITECTURE
    if (style & wxHF_PRINT)
        toolBar->AddTool(wxID_HTML_PRINT, wprintBitmap, wxNullBitmap,
                           FALSE, -1, -1, (wxObject *) NULL,
                           _("Print this page"));
#endif

    toolBar->AddSeparator();
    toolBar->AddTool(wxID_HTML_OPTIONS, woptionsBitmap, wxNullBitmap,
                       FALSE, -1, -1, (wxObject *) NULL,
                       _("Display options dialog"));
}


void wxHtmlHelpFrame::SetTitleFormat(const wxString& format)
{
    if (m_HtmlWin)
        m_HtmlWin->SetRelatedFrame(this, format);
    m_TitleFormat = format;
}


bool wxHtmlHelpFrame::Display(const wxString& x)
{
    wxString url = m_Data->FindPageByName(x);
    if (!url.IsEmpty())
    {
        m_HtmlWin->LoadPage(url);
        NotifyPageChanged();
        return TRUE;
    }
    return FALSE;
}

bool wxHtmlHelpFrame::Display(const int id)
{
    wxString url = m_Data->FindPageById(id);
    if (!url.IsEmpty())
    {
        m_HtmlWin->LoadPage(url);
        NotifyPageChanged();
        return TRUE;
    }
    return FALSE;
}



bool wxHtmlHelpFrame::DisplayContents()
{
    if (! m_ContentsBox)
        return FALSE;
    if (!m_Splitter->IsSplit())
    {
        m_NavigPan->Show(TRUE);
        m_HtmlWin->Show(TRUE);
        m_Splitter->SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);
        m_Cfg.navig_on = TRUE;
    }
    m_NavigNotebook->SetSelection(0);
    if (m_Data->GetBookRecArray().GetCount() > 0)
    {
        wxHtmlBookRecord& book = m_Data->GetBookRecArray()[0];
        if (!book.GetStart().IsEmpty())
            m_HtmlWin->LoadPage(book.GetFullPath(book.GetStart()));
    }
    return TRUE;
}



bool wxHtmlHelpFrame::DisplayIndex()
{
    if (! m_IndexList)
        return FALSE;
    if (!m_Splitter->IsSplit())
    {
        m_NavigPan->Show(TRUE);
        m_HtmlWin->Show(TRUE);
        m_Splitter->SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);
    }
    m_NavigNotebook->SetSelection(1);
    if (m_Data->GetBookRecArray().GetCount() > 0)
    {
        wxHtmlBookRecord& book = m_Data->GetBookRecArray()[0];
        if (!book.GetStart().IsEmpty())
            m_HtmlWin->LoadPage(book.GetFullPath(book.GetStart()));
    }
    return TRUE;
}



bool wxHtmlHelpFrame::KeywordSearch(const wxString& keyword)
{
    if (! (m_SearchList && m_SearchButton && m_SearchText && m_SearchChoice))
        return FALSE;

    int foundcnt = 0, curi;
    wxString foundstr;
    wxString book = wxEmptyString;

    if (!m_Splitter->IsSplit())
    {
        m_NavigPan->Show(TRUE);
        m_HtmlWin->Show(TRUE);
        m_Splitter->SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);
    }
    m_NavigNotebook->SetSelection(m_SearchPage);
    m_SearchList->Clear();
    m_SearchText->SetValue(keyword);
    m_SearchButton->Enable(FALSE);

    if (m_SearchChoice->GetSelection() != 0)
        book = m_SearchChoice->GetStringSelection();

    wxHtmlSearchStatus status(m_Data, keyword,
                              m_SearchCaseSensitive->GetValue(), m_SearchWholeWords->GetValue(),
                              book);

    wxProgressDialog progress(_("Searching..."), _("No matching page found yet"),
                              status.GetMaxIndex(), this,
                              wxPD_APP_MODAL | wxPD_CAN_ABORT | wxPD_AUTO_HIDE);

    while (status.IsActive())
    {
        curi = status.GetCurIndex();
        if (curi % 32 == 0 && progress.Update(curi) == FALSE)
            break;
        if (status.Search())
        {
            foundstr.Printf(_("Found %i matches"), ++foundcnt);
            progress.Update(status.GetCurIndex(), foundstr);
            m_SearchList->Append(status.GetName(), status.GetContentsItem());
        }
    }

    m_SearchButton->Enable(TRUE);
    m_SearchText->SetSelection(0, keyword.Length());
    m_SearchText->SetFocus();
    if (foundcnt)
    {
        wxHtmlContentsItem *it = (wxHtmlContentsItem*) m_SearchList->GetClientData(0);
        if (it)
        {
            m_HtmlWin->LoadPage(it->GetFullPath());
            NotifyPageChanged();
        }
    }
    return (foundcnt > 0);
}

void wxHtmlHelpFrame::CreateContents()
{
    if (! m_ContentsBox)
        return ;

    m_ContentsBox->Clear();

    if (m_PagesHash)
    {
        WX_CLEAR_HASH_TABLE(*m_PagesHash);
        delete m_PagesHash;
    }
    m_PagesHash = new wxHashTable(wxKEY_STRING, 2 * m_Data->GetContentsCnt());

    int cnt = m_Data->GetContentsCnt();
    int i;

    wxHtmlContentsItem *it;

    const int MAX_ROOTS = 64;
    wxTreeItemId roots[MAX_ROOTS];
    // VS: this array holds information about whether we've set item icon at
    //     given level. This is neccessary because m_Data has flat structure
    //     and there's no way of recognizing if some item has subitems or not.
    //     We set the icon later: when we find an item with level=n, we know
    //     that the last item with level=n-1 was folder with subitems, so we
    //     set its icon accordingly
    bool imaged[MAX_ROOTS];
    m_ContentsBox->DeleteAllItems();
    
    roots[0] = m_ContentsBox->AddRoot(_("(Help)"));
    imaged[0] = TRUE;

    for (it = m_Data->GetContents(), i = 0; i < cnt; i++, it++)
    {
        // Handle books:
        if (it->m_Level == 0)
        {
            if (m_hfStyle & wxHF_MERGE_BOOKS)
                // VS: we don't want book nodes, books' content should
                //    appear under tree's root. This line will create "fake"
                //    record about book node so that the rest of this look
                //    will believe there really _is_ book node and will
                //    behave correctly.
                roots[1] = roots[0];
            else
            {
                roots[1] = m_ContentsBox->AppendItem(roots[0],
                                         it->m_Name, IMG_Book, -1,
                                         new wxHtmlHelpTreeItemData(i));
                m_ContentsBox->SetItemBold(roots[1], TRUE);
            }
            imaged[1] = TRUE;
        }
        // ...and their contents:
        else
        {
            roots[it->m_Level + 1] = m_ContentsBox->AppendItem(
                                     roots[it->m_Level], it->m_Name, IMG_Page,
                                     -1, new wxHtmlHelpTreeItemData(i));
            imaged[it->m_Level + 1] = FALSE;
        }

        m_PagesHash->Put(it->GetFullPath(),
                           new wxHtmlHelpHashData(i, roots[it->m_Level + 1]));

        // Set the icon for the node one level up in the hiearachy,
        // unless already done (see comment above imaged[] declaration)
        if (!imaged[it->m_Level])
        {
            int image = IMG_Folder;
            if (m_hfStyle & wxHF_ICONS_BOOK)
                image = IMG_Book;
            else if (m_hfStyle & wxHF_ICONS_BOOK_CHAPTER)
                image = (it->m_Level == 1) ? IMG_Book : IMG_Folder;
            m_ContentsBox->SetItemImage(roots[it->m_Level], image);
            m_ContentsBox->SetItemSelectedImage(roots[it->m_Level], image);
            imaged[it->m_Level] = TRUE;
        }
    }
}


void wxHtmlHelpFrame::CreateIndex()
{
    if (! m_IndexList)
        return ;

    m_IndexList->Clear();

    int cnt = m_Data->GetIndexCnt();

    wxString cnttext;
    if (cnt > INDEX_IS_SMALL) cnttext.Printf(_("%i of %i"), 0, cnt);
    else cnttext.Printf(_("%i of %i"), cnt, cnt);
    m_IndexCountInfo->SetLabel(cnttext);
    if (cnt > INDEX_IS_SMALL) return;

    wxHtmlContentsItem* index = m_Data->GetIndex();

    for (int i = 0; i < cnt; i++)
        m_IndexList->Append(index[i].m_Name, (char*)(index + i));
}

void wxHtmlHelpFrame::CreateSearch()
{
    if (! (m_SearchList && m_SearchChoice))
        return ;
    m_SearchList->Clear();
    m_SearchChoice->Clear();
    m_SearchChoice->Append(_("Search in all books"));
    const wxHtmlBookRecArray& bookrec = m_Data->GetBookRecArray();
    int i, cnt = bookrec.GetCount();
    for (i = 0; i < cnt; i++)
        m_SearchChoice->Append(bookrec[i].GetTitle());
    m_SearchChoice->SetSelection(0);
}


void wxHtmlHelpFrame::RefreshLists()
{
    CreateContents();
    CreateIndex();
    CreateSearch();
}

void wxHtmlHelpFrame::ReadCustomization(wxConfigBase *cfg, const wxString& path)
{
    wxString oldpath;
    wxString tmp;

    if (path != wxEmptyString)
    {
        oldpath = cfg->GetPath();
        cfg->SetPath(_T("/") + path);
    }

    m_Cfg.navig_on = cfg->Read(wxT("hcNavigPanel"), m_Cfg.navig_on) != 0;
    m_Cfg.sashpos = cfg->Read(wxT("hcSashPos"), m_Cfg.sashpos);
    m_Cfg.x = cfg->Read(wxT("hcX"), m_Cfg.x);
    m_Cfg.y = cfg->Read(wxT("hcY"), m_Cfg.y);
    m_Cfg.w = cfg->Read(wxT("hcW"), m_Cfg.w);
    m_Cfg.h = cfg->Read(wxT("hcH"), m_Cfg.h);

    m_FixedFace = cfg->Read(wxT("hcFixedFace"), m_FixedFace);
    m_NormalFace = cfg->Read(wxT("hcNormalFace"), m_NormalFace);
    m_FontSize = cfg->Read(wxT("hcBaseFontSize"), m_FontSize);

    {
        int i;
        int cnt;
        wxString val, s;

        cnt = cfg->Read(wxT("hcBookmarksCnt"), 0L);
        if (cnt != 0)
        {
            m_BookmarksNames.Clear();
            m_BookmarksPages.Clear();
            if (m_Bookmarks)
            {
                m_Bookmarks->Clear();
                m_Bookmarks->Append(_("(bookmarks)"));
            }

            for (i = 0; i < cnt; i++)
            {
                val.Printf(wxT("hcBookmark_%i"), i);
                s = cfg->Read(val);
                m_BookmarksNames.Add(s);
                if (m_Bookmarks) m_Bookmarks->Append(s);
                val.Printf(wxT("hcBookmark_%i_url"), i);
                s = cfg->Read(val);
                m_BookmarksPages.Add(s);
            }
        }
    }

    if (m_HtmlWin)
        m_HtmlWin->ReadCustomization(cfg);

    if (path != wxEmptyString)
        cfg->SetPath(oldpath);
}

void wxHtmlHelpFrame::WriteCustomization(wxConfigBase *cfg, const wxString& path)
{
    wxString oldpath;
    wxString tmp;

    if (path != wxEmptyString)
    {
        oldpath = cfg->GetPath();
        cfg->SetPath(_T("/") + path);
    }

    cfg->Write(wxT("hcNavigPanel"), m_Cfg.navig_on);
    cfg->Write(wxT("hcSashPos"), (long)m_Cfg.sashpos);
    if ( !IsIconized() )
    {
        //  Don't write if iconized as this would make the window
        //  disappear next time it is shown!
        cfg->Write(wxT("hcX"), (long)m_Cfg.x);
        cfg->Write(wxT("hcY"), (long)m_Cfg.y);
        cfg->Write(wxT("hcW"), (long)m_Cfg.w);
        cfg->Write(wxT("hcH"), (long)m_Cfg.h);
    }
    cfg->Write(wxT("hcFixedFace"), m_FixedFace);
    cfg->Write(wxT("hcNormalFace"), m_NormalFace);
    cfg->Write(wxT("hcBaseFontSize"), (long)m_FontSize);

    if (m_Bookmarks)
    {
        int i;
        int cnt = m_BookmarksNames.GetCount();
        wxString val;

        cfg->Write(wxT("hcBookmarksCnt"), (long)cnt);
        for (i = 0; i < cnt; i++)
        {
            val.Printf(wxT("hcBookmark_%i"), i);
            cfg->Write(val, m_BookmarksNames[i]);
            val.Printf(wxT("hcBookmark_%i_url"), i);
            cfg->Write(val, m_BookmarksPages[i]);
        }
    }

    if (m_HtmlWin)
        m_HtmlWin->WriteCustomization(cfg);

    if (path != wxEmptyString)
        cfg->SetPath(oldpath);
}





static void SetFontsToHtmlWin(wxHtmlWindow *win, wxString scalf, wxString fixf, int size)
{
    int f_sizes[7];
    f_sizes[0] = int(size * 0.6);
    f_sizes[1] = int(size * 0.8);
    f_sizes[2] = size;
    f_sizes[3] = int(size * 1.2);
    f_sizes[4] = int(size * 1.4);
    f_sizes[5] = int(size * 1.6);
    f_sizes[6] = int(size * 1.8);

    win->SetFonts(scalf, fixf, f_sizes);
}


class wxHtmlHelpFrameOptionsDialog : public wxDialog
{
public:
    wxComboBox *NormalFont, *FixedFont;
    wxSpinCtrl *FontSize;
    wxHtmlWindow *TestWin;

    wxHtmlHelpFrameOptionsDialog(wxWindow *parent) 
        : wxDialog(parent, -1, wxString(_("Help Browser Options")))
    {
        wxBoxSizer *topsizer = new wxBoxSizer(wxVERTICAL);
        wxFlexGridSizer *sizer = new wxFlexGridSizer(2, 3, 2, 5);

        sizer->Add(new wxStaticText(this, -1, _("Normal font:")));
        sizer->Add(new wxStaticText(this, -1, _("Fixed font:")));
        sizer->Add(new wxStaticText(this, -1, _("Font size:")));

        sizer->Add(NormalFont = new wxComboBox(this, -1, wxEmptyString, wxDefaultPosition,
                      wxSize(200, 200),
                      0, NULL, wxCB_DROPDOWN | wxCB_READONLY));

        sizer->Add(FixedFont = new wxComboBox(this, -1, wxEmptyString, wxDefaultPosition,
                      wxSize(200, 200),
                      0, NULL, wxCB_DROPDOWN | wxCB_READONLY));

        sizer->Add(FontSize = new wxSpinCtrl(this, -1));
        FontSize->SetRange(2, 100);

        topsizer->Add(sizer, 0, wxLEFT|wxRIGHT|wxTOP, 10);

        topsizer->Add(new wxStaticText(this, -1, _("Preview:")),
                        0, wxLEFT | wxTOP, 10);
        topsizer->Add(TestWin = new wxHtmlWindow(this, -1, wxDefaultPosition, wxSize(20, 150),
                                                 wxHW_SCROLLBAR_AUTO | wxSUNKEN_BORDER),
                        1, wxEXPAND | wxLEFT|wxTOP|wxRIGHT, 10);

        wxBoxSizer *sizer2 = new wxBoxSizer(wxHORIZONTAL);
        wxButton *ok;
        sizer2->Add(ok = new wxButton(this, wxID_OK, _("OK")), 0, wxALL, 10);
        ok->SetDefault();
        sizer2->Add(new wxButton(this, wxID_CANCEL, _("Cancel")), 0, wxALL, 10);
        topsizer->Add(sizer2, 0, wxALIGN_RIGHT);

        SetAutoLayout(TRUE);
        SetSizer(topsizer);
        topsizer->Fit(this);
        Centre(wxBOTH);
    }


    void UpdateTestWin()
    {
        wxBusyCursor bcur;
        SetFontsToHtmlWin(TestWin,
                          NormalFont->GetStringSelection(),
                          FixedFont->GetStringSelection(),
                          FontSize->GetValue());
        TestWin->SetPage(_(
"<html><body>\
<table><tr><td>\
Normal face<br>(and <u>underlined</u>. <i>Italic face.</i> \
<b>Bold face.</b> <b><i>Bold italic face.</i></b><br>\
<font size=-2>font size -2</font><br>\
<font size=-1>font size -1</font><br>\
<font size=+0>font size +0</font><br>\
<font size=+1>font size +1</font><br>\
<font size=+2>font size +2</font><br>\
<font size=+3>font size +3</font><br>\
<font size=+4>font size +4</font><br>\
<td>\
<p><tt>Fixed size face.<br> <b>bold</b> <i>italic</i> \
<b><i>bold italic <u>underlined</u></i></b><br>\
<font size=-2>font size -2</font><br>\
<font size=-1>font size -1</font><br>\
<font size=+0>font size +0</font><br>\
<font size=+1>font size +1</font><br>\
<font size=+2>font size +2</font><br>\
<font size=+3>font size +3</font><br>\
<font size=+4>font size +4</font></tt>\
</table></body></html>"
                          ));
    }

    void OnUpdate(wxCommandEvent& WXUNUSED(event))
    {
        UpdateTestWin();
    }
    void OnUpdateSpin(wxSpinEvent& WXUNUSED(event))
    {
        UpdateTestWin();
    }

    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxHtmlHelpFrameOptionsDialog)
};

BEGIN_EVENT_TABLE(wxHtmlHelpFrameOptionsDialog, wxDialog)
    EVT_COMBOBOX(-1, wxHtmlHelpFrameOptionsDialog::OnUpdate)
    EVT_SPINCTRL(-1, wxHtmlHelpFrameOptionsDialog::OnUpdateSpin)
END_EVENT_TABLE()

void wxHtmlHelpFrame::OptionsDialog()
{
    wxHtmlHelpFrameOptionsDialog dlg(this);
    unsigned i;

    if (m_NormalFonts == NULL)
    {
        wxFontEnumerator enu;
        enu.EnumerateFacenames();
        m_NormalFonts = new wxArrayString;
        *m_NormalFonts = *enu.GetFacenames();
        m_NormalFonts->Sort(wxStringSortAscending);
    }
    if (m_FixedFonts == NULL)
    {
        wxFontEnumerator enu;
        enu.EnumerateFacenames(wxFONTENCODING_SYSTEM, TRUE);
        m_FixedFonts = new wxArrayString;
        *m_FixedFonts = *enu.GetFacenames();
        m_FixedFonts->Sort(wxStringSortAscending);
    }
    
    // VS: We want to show the font that is actually used by wxHtmlWindow.
    //     If customization dialog wasn't used yet, facenames are empty and
    //     wxHtmlWindow uses default fonts -- let's find out what they
    //     are so that we can pass them to the dialog:
    if (m_NormalFace.empty())
    {
        wxFont fnt(m_FontSize, wxSWISS, wxNORMAL, wxNORMAL, FALSE);
        m_NormalFace = fnt.GetFaceName();
    }
    if (m_FixedFace.empty())
    {
        wxFont fnt(m_FontSize, wxMODERN, wxNORMAL, wxNORMAL, FALSE);
        m_FixedFace = fnt.GetFaceName();
    }

    for (i = 0; i < m_NormalFonts->GetCount(); i++)
        dlg.NormalFont->Append((*m_NormalFonts)[i]);
    for (i = 0; i < m_FixedFonts->GetCount(); i++)
        dlg.FixedFont->Append((*m_FixedFonts)[i]);
    if (!m_NormalFace.empty())
        dlg.NormalFont->SetStringSelection(m_NormalFace);
    else
        dlg.NormalFont->SetSelection(0);
    if (!m_FixedFace.empty())
        dlg.FixedFont->SetStringSelection(m_FixedFace);
    else
        dlg.FixedFont->SetSelection(0);
    dlg.FontSize->SetValue(m_FontSize);
    dlg.UpdateTestWin();

    if (dlg.ShowModal() == wxID_OK)
    {
        m_NormalFace = dlg.NormalFont->GetStringSelection();
        m_FixedFace = dlg.FixedFont->GetStringSelection();
        m_FontSize = dlg.FontSize->GetValue();
        SetFontsToHtmlWin(m_HtmlWin, m_NormalFace, m_FixedFace, m_FontSize);
    }
}



void wxHtmlHelpFrame::NotifyPageChanged()
{
    if (m_UpdateContents && m_PagesHash)
    {
        wxString an = m_HtmlWin->GetOpenedAnchor();
        wxHtmlHelpHashData *ha;
        if (an.IsEmpty())
            ha = (wxHtmlHelpHashData*) m_PagesHash->Get(m_HtmlWin->GetOpenedPage());
        else
            ha = (wxHtmlHelpHashData*) m_PagesHash->Get(m_HtmlWin->GetOpenedPage() + wxT("#") + an);
        if (ha)
        {
            bool olduc = m_UpdateContents;
            m_UpdateContents = FALSE;
            m_ContentsBox->SelectItem(ha->m_Id);
            m_ContentsBox->EnsureVisible(ha->m_Id);
            m_UpdateContents = olduc;
        }
    }
}



/*
EVENT HANDLING :
*/


void wxHtmlHelpFrame::OnActivate(wxActivateEvent& event)
{
    // This saves one mouse click when using the
    // wxHTML for context sensitive help systems
#ifndef __WXGTK__
    // NB: wxActivateEvent is a bit broken in wxGTK
    //     and is sometimes sent when it should not be
    if (event.GetActive() && m_HtmlWin)
        m_HtmlWin->SetFocus();
#endif

    event.Skip();
}

void wxHtmlHelpFrame::OnToolbar(wxCommandEvent& event)
{
    switch (event.GetId())
    {
        case wxID_HTML_BACK :
            m_HtmlWin->HistoryBack();
            NotifyPageChanged();
            break;

        case wxID_HTML_FORWARD :
            m_HtmlWin->HistoryForward();
            NotifyPageChanged();
            break;

        case wxID_HTML_UP :
            if (m_PagesHash)
            {
                wxString an = m_HtmlWin->GetOpenedAnchor();
                wxHtmlHelpHashData *ha;
                if (an.IsEmpty())
                    ha = (wxHtmlHelpHashData*) m_PagesHash->Get(m_HtmlWin->GetOpenedPage());
                else
                    ha = (wxHtmlHelpHashData*) m_PagesHash->Get(m_HtmlWin->GetOpenedPage() + wxT("#") + an);
                if (ha && ha->m_Index > 0)
                {
                    wxHtmlContentsItem *it = m_Data->GetContents() + (ha->m_Index - 1);
                    if (it->m_Page[0] != 0)
                    {
                        m_HtmlWin->LoadPage(it->GetFullPath());
                        NotifyPageChanged();
                    }
                }
            }
            break;

        case wxID_HTML_UPNODE :
            if (m_PagesHash)
            {
                wxString an = m_HtmlWin->GetOpenedAnchor();
                wxHtmlHelpHashData *ha;
                if (an.IsEmpty())
                    ha = (wxHtmlHelpHashData*) m_PagesHash->Get(m_HtmlWin->GetOpenedPage());
                else
                    ha = (wxHtmlHelpHashData*) m_PagesHash->Get(m_HtmlWin->GetOpenedPage() + wxT("#") + an);
                if (ha && ha->m_Index > 0)
                {
                    int level = m_Data->GetContents()[ha->m_Index].m_Level - 1;
                    wxHtmlContentsItem *it;
                    int ind = ha->m_Index - 1;

                    it = m_Data->GetContents() + ind;
                    while (ind >= 0 && it->m_Level != level) ind--, it--;
                    if (ind >= 0)
                    {
                        if (it->m_Page[0] != 0)
                        {
                            m_HtmlWin->LoadPage(it->GetFullPath());
                            NotifyPageChanged();
                        }
                    }
                }
            }
            break;

        case wxID_HTML_DOWN :
            if (m_PagesHash)
            {
                wxString an = m_HtmlWin->GetOpenedAnchor();
                wxString adr;
                wxHtmlHelpHashData *ha;

                if (an.IsEmpty()) adr = m_HtmlWin->GetOpenedPage();
                else adr = m_HtmlWin->GetOpenedPage() + wxT("#") + an;

                ha = (wxHtmlHelpHashData*) m_PagesHash->Get(adr);

                if (ha && ha->m_Index < m_Data->GetContentsCnt() - 1)
                {
                    wxHtmlContentsItem *it = m_Data->GetContents() + (ha->m_Index + 1);

                    while (it->GetFullPath() == adr) it++;

                    if (it->m_Page[0] != 0)
                    {
                        m_HtmlWin->LoadPage(it->GetFullPath());
                        NotifyPageChanged();
                    }
                }
            }
            break;

        case wxID_HTML_PANEL :
            {
                if (! (m_Splitter && m_NavigPan))
                    return ;
                if (m_Splitter->IsSplit())
                {
                    m_Cfg.sashpos = m_Splitter->GetSashPosition();
                    m_Splitter->Unsplit(m_NavigPan);
                    m_Cfg.navig_on = FALSE;
                }
                else
                {
                    m_NavigPan->Show(TRUE);
                    m_HtmlWin->Show(TRUE);
                    m_Splitter->SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);
                    m_Cfg.navig_on = TRUE;
                }
            }
            break;

        case wxID_HTML_OPTIONS :
            OptionsDialog();
            break;

        case wxID_HTML_BOOKMARKSADD :
            {
                wxString item;
                wxString url;

                item = m_HtmlWin->GetOpenedPageTitle();
                url = m_HtmlWin->GetOpenedPage();
                if (item == wxEmptyString)
                    item = url.AfterLast(wxT('/'));
                if (m_BookmarksPages.Index(url) == wxNOT_FOUND)
                {
                    m_Bookmarks->Append(item);
                    m_BookmarksNames.Add(item);
                    m_BookmarksPages.Add(url);
                }
            }
            break;

        case wxID_HTML_BOOKMARKSREMOVE :
            {
                wxString item;
                int pos;

                item = m_Bookmarks->GetStringSelection();
                pos = m_BookmarksNames.Index(item);
                if (pos != wxNOT_FOUND)
                {
                    m_BookmarksNames.RemoveAt(pos);
                    m_BookmarksPages.RemoveAt(pos);
                    m_Bookmarks->Delete(m_Bookmarks->GetSelection());
                }
            }
            break;

#if wxUSE_PRINTING_ARCHITECTURE
        case wxID_HTML_PRINT :
            {
                if (m_Printer == NULL)
                    m_Printer = new wxHtmlEasyPrinting(_("Help Printing"), this);
                if (!m_HtmlWin->GetOpenedPage())
                    wxLogWarning(_("Cannot print empty page."));
                else
                    m_Printer->PrintFile(m_HtmlWin->GetOpenedPage());
            }
            break;
#endif

        case wxID_HTML_OPENFILE :
            {
                wxString s = wxFileSelector(_("Open HTML document"),
                                            wxEmptyString,
                                            wxEmptyString,
                                            wxEmptyString,
                                            _(
"HTML files (*.htm)|*.htm|HTML files (*.html)|*.html|\
Help books (*.htb)|*.htb|Help books (*.zip)|*.zip|\
HTML Help Project (*.hhp)|*.hhp|\
All files (*.*)|*"
                                            ),
                                            wxOPEN | wxFILE_MUST_EXIST,
                                            this);
                if (!s.IsEmpty())
                {
                    wxString ext = s.Right(4).Lower();
                    if (ext == _T(".zip") || ext == _T(".htb") || ext == _T(".hhp"))
                    {
                        wxBusyCursor bcur;
                        m_Data->AddBook(s);
                        RefreshLists();
                    }
                    else
                        m_HtmlWin->LoadPage(s);
                }
            }
            break;
    }
}



void wxHtmlHelpFrame::OnContentsSel(wxTreeEvent& event)
{
    wxHtmlHelpTreeItemData *pg;
    wxHtmlContentsItem *it;

    pg = (wxHtmlHelpTreeItemData*) m_ContentsBox->GetItemData(event.GetItem());

    if (pg && m_UpdateContents)
    {
        it = m_Data->GetContents() + (pg->m_Id);
        m_UpdateContents = FALSE;
        if (it->m_Page[0] != 0)
            m_HtmlWin->LoadPage(it->GetFullPath());
        m_UpdateContents = TRUE;
    }
}



void wxHtmlHelpFrame::OnIndexSel(wxCommandEvent& WXUNUSED(event))
{
    wxHtmlContentsItem *it = (wxHtmlContentsItem*) m_IndexList->GetClientData(m_IndexList->GetSelection());
    if (it->m_Page[0] != 0)
        m_HtmlWin->LoadPage(it->GetFullPath());
    NotifyPageChanged();
}


void wxHtmlHelpFrame::OnIndexFind(wxCommandEvent& event)
{
    wxString sr = m_IndexText->GetLineText(0);
    sr.MakeLower();
    if (sr == wxEmptyString)
    {
        OnIndexAll(event);
    }
    else
    {
        wxBusyCursor bcur;
        const wxChar *cstr = sr.c_str();
        wxChar mybuff[512];
        wxChar *ptr;
        bool first = TRUE;

        m_IndexList->Clear();
        int cnt = m_Data->GetIndexCnt();
        wxHtmlContentsItem* index = m_Data->GetIndex();

        int displ = 0;
        for (int i = 0; i < cnt; i++)
        {
            wxStrncpy(mybuff, index[i].m_Name, 512);
            mybuff[511] = _T('\0');
            for (ptr = mybuff; *ptr != 0; ptr++)
                if (*ptr >= _T('A') && *ptr <= _T('Z'))
                    *ptr -= (wxChar)(_T('A') - _T('a'));
            if (wxStrstr(mybuff, cstr) != NULL)
            {
                m_IndexList->Append(index[i].m_Name, (char*)(index + i));
                displ++;
                if (first)
                {
                    if (index[i].m_Page[0] != 0)
                        m_HtmlWin->LoadPage(index[i].GetFullPath());
                    NotifyPageChanged();
                    first = FALSE;
                }
            }
        }

        wxString cnttext;
        cnttext.Printf(_("%i of %i"), displ, cnt);
        m_IndexCountInfo->SetLabel(cnttext);

        m_IndexText->SetSelection(0, sr.Length());
        m_IndexText->SetFocus();
    }
}

void wxHtmlHelpFrame::OnIndexAll(wxCommandEvent& WXUNUSED(event))
{
    wxBusyCursor bcur;

    m_IndexList->Clear();
    int cnt = m_Data->GetIndexCnt();
    bool first = TRUE;
    wxHtmlContentsItem* index = m_Data->GetIndex();

    for (int i = 0; i < cnt; i++)
    {
        m_IndexList->Append(index[i].m_Name, (char*)(index + i));
        if (first)
        {
            if (index[i].m_Page[0] != 0)
                m_HtmlWin->LoadPage(index[i].GetFullPath());
            NotifyPageChanged();
            first = FALSE;
        }
    }

    wxString cnttext;
    cnttext.Printf(_("%i of %i"), cnt, cnt);
    m_IndexCountInfo->SetLabel(cnttext);
}


void wxHtmlHelpFrame::OnSearchSel(wxCommandEvent& WXUNUSED(event))
{
    wxHtmlContentsItem *it = (wxHtmlContentsItem*) m_SearchList->GetClientData(m_SearchList->GetSelection());
    if (it)
    {
        if (it->m_Page[0] != 0)
            m_HtmlWin->LoadPage(it->GetFullPath());
        NotifyPageChanged();
    }
}

void wxHtmlHelpFrame::OnSearch(wxCommandEvent& WXUNUSED(event))
{
    wxString sr = m_SearchText->GetLineText(0);

    if (sr != wxEmptyString) KeywordSearch(sr);
}

void wxHtmlHelpFrame::OnBookmarksSel(wxCommandEvent& WXUNUSED(event))
{
    wxString sr = m_Bookmarks->GetStringSelection();

    if (sr != wxEmptyString && sr != _("(bookmarks)"))
    {
       m_HtmlWin->LoadPage(m_BookmarksPages[m_BookmarksNames.Index(sr)]);
       NotifyPageChanged();
    }
}

void wxHtmlHelpFrame::OnCloseWindow(wxCloseEvent& evt)
{
    GetSize(&m_Cfg.w, &m_Cfg.h);
    GetPosition(&m_Cfg.x, &m_Cfg.y);

#ifdef __WXGTK__
    if (IsGrabbed())
    {
        RemoveGrab();
    }
#endif
    
    if (m_Splitter && m_Cfg.navig_on) m_Cfg.sashpos = m_Splitter->GetSashPosition();

    if (m_Config)
        WriteCustomization(m_Config, m_ConfigRoot);

    if (m_helpController && m_helpController->IsKindOf(CLASSINFO(wxHtmlHelpController)))
    {
        ((wxHtmlHelpController*) m_helpController)->OnCloseFrame(evt);
    }

    evt.Skip();
}

#ifdef __WXMAC__
void wxHtmlHelpFrame::OnClose(wxCommandEvent& event)
{
    Close(TRUE);
}

void wxHtmlHelpFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox(wxT("wxWindows HTML Help Viewer (c) 1998-2003, Vaclav Slavik et al"), wxT("HelpView"),
        wxICON_INFORMATION|wxOK, this);
}
#endif

BEGIN_EVENT_TABLE(wxHtmlHelpFrame, wxFrame)
    EVT_ACTIVATE(wxHtmlHelpFrame::OnActivate)
    EVT_TOOL_RANGE(wxID_HTML_PANEL, wxID_HTML_OPTIONS, wxHtmlHelpFrame::OnToolbar)
    EVT_BUTTON(wxID_HTML_BOOKMARKSREMOVE, wxHtmlHelpFrame::OnToolbar)
    EVT_BUTTON(wxID_HTML_BOOKMARKSADD, wxHtmlHelpFrame::OnToolbar)
    EVT_TREE_SEL_CHANGED(wxID_HTML_TREECTRL, wxHtmlHelpFrame::OnContentsSel)
    EVT_LISTBOX(wxID_HTML_INDEXLIST, wxHtmlHelpFrame::OnIndexSel)
    EVT_LISTBOX(wxID_HTML_SEARCHLIST, wxHtmlHelpFrame::OnSearchSel)
    EVT_BUTTON(wxID_HTML_SEARCHBUTTON, wxHtmlHelpFrame::OnSearch)
    EVT_TEXT_ENTER(wxID_HTML_SEARCHTEXT, wxHtmlHelpFrame::OnSearch)
    EVT_BUTTON(wxID_HTML_INDEXBUTTON, wxHtmlHelpFrame::OnIndexFind)
    EVT_TEXT_ENTER(wxID_HTML_INDEXTEXT, wxHtmlHelpFrame::OnIndexFind)
    EVT_BUTTON(wxID_HTML_INDEXBUTTONALL, wxHtmlHelpFrame::OnIndexAll)
    EVT_COMBOBOX(wxID_HTML_BOOKMARKSLIST, wxHtmlHelpFrame::OnBookmarksSel)
    EVT_CLOSE(wxHtmlHelpFrame::OnCloseWindow)
#ifdef __WXMAC__
    EVT_MENU(wxID_CLOSE, wxHtmlHelpFrame::OnClose)
    EVT_MENU(wxID_ABOUT, wxHtmlHelpFrame::OnAbout)
#endif

END_EVENT_TABLE()

#endif // wxUSE_WXHTML_HELP

