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
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h"

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/defs.h"

#if wxUSE_HTML && wxUSE_STREAMS
#ifndef WXPRECOMP
#include "wx/wx.h"
#endif

#include "wx/html/helpfrm.h"
#include "wx/html/helpctrl.h"
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

// Bitmaps:

#ifndef __WXMSW__
#include "bitmaps/wpanel.xpm"
#include "bitmaps/wback.xpm"
#include "bitmaps/wforward.xpm"
#include "bitmaps/wbook.xpm"
#include "bitmaps/woptions.xpm"
#include "bitmaps/wfolder.xpm"
#include "bitmaps/wpage.xpm"
#include "bitmaps/whelp.xpm"
#include "bitmaps/whlproot.xpm"
#include "bitmaps/wbkadd.xpm"
#include "bitmaps/wbkdel.xpm"
#include "bitmaps/wup.xpm"
#include "bitmaps/wupnode.xpm"
#include "bitmaps/wdown.xpm"
#include "bitmaps/wopen.xpm"
#include "bitmaps/wprint.xpm"
#endif


// what is considered "small index"?
#define INDEX_IS_SMALL 100


//--------------------------------------------------------------------------
// wxHtmlHelpTreeItemData (private)
//--------------------------------------------------------------------------

class wxHtmlHelpTreeItemData : public wxTreeItemData
{
    public:
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
            m_Frame -> NotifyPageChanged();
        }

    private:
        wxHtmlHelpFrame *m_Frame;
};



//---------------------------------------------------------------------------
// wxHtmlHelpFrame
//---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxHtmlHelpFrame, wxFrame)

wxHtmlHelpFrame::wxHtmlHelpFrame(wxWindow* parent, wxWindowID id, const wxString& title,
                                 int style, wxHtmlHelpData* data)
{
    Init(data);
    Create(parent, id, title, style);
}

void wxHtmlHelpFrame::Init(wxHtmlHelpData* data)
{
    if (data) {
        m_Data = data;
        m_DataCreated = FALSE;
    } else {
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
    m_FontSize = 1;

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

bool wxHtmlHelpFrame::Create(wxWindow* parent, wxWindowID id, const wxString& title,
                             int style)
{
    wxImageList *ContentsImageList = new wxImageList(16, 16);
    ContentsImageList->Add(wxICON(wbook));
    ContentsImageList->Add(wxICON(wfolder));
    ContentsImageList->Add(wxICON(wpage));
    ContentsImageList->Add(wxICON(whlproot));

    // Do the config in two steps. We read the HtmlWindow customization after we
    // create the window.
    if (m_Config)
        ReadCustomization(m_Config, m_ConfigRoot);

    wxFrame::Create(parent, id, _("Help"), wxPoint(m_Cfg.x, m_Cfg.y), wxSize(m_Cfg.w, m_Cfg.h), wxDEFAULT_FRAME_STYLE, "wxHtmlHelp");

    GetPosition(&m_Cfg.x, &m_Cfg.y);

    SetIcon(wxICON(whelp));

    int notebook_page = 0;

    CreateStatusBar();

    // toolbar?
    if (style & (wxHF_TOOLBAR | wxHF_FLATTOOLBAR)) {
        wxToolBar *toolBar = CreateToolBar(wxNO_BORDER | wxTB_HORIZONTAL |
                                           wxTB_DOCKABLE |
                                           (style & wxHF_FLATTOOLBAR ? wxTB_FLAT : 0));
        toolBar->SetMargins( 2, 2 );
        AddToolbarButtons(toolBar, style);
        toolBar -> Realize();
    }

    if (style & (wxHF_CONTENTS | wxHF_INDEX | wxHF_SEARCH)) {
        // traditional help controller; splitter window with html page on the
        // right and a notebook containing various pages on the left
        m_Splitter = new wxSplitterWindow(this);

        m_HtmlWin = new wxHtmlHelpHtmlWindow(this, m_Splitter);
        m_NavigPan = new wxNotebook(m_Splitter, wxID_HTML_NOTEBOOK,
                                    wxDefaultPosition, wxDefaultSize);
    } else { // only html window, no notebook with index,contents etc
        m_HtmlWin = new wxHtmlWindow(this);
    }

    m_HtmlWin -> SetRelatedFrame(this, m_TitleFormat);
    m_HtmlWin -> SetRelatedStatusBar(0);
    if (m_Config)
        m_HtmlWin -> ReadCustomization(m_Config, m_ConfigRoot);

    // contents tree panel?
    if (style & wxHF_CONTENTS) {
        wxWindow *dummy = new wxPanel(m_NavigPan, wxID_HTML_INDEXPAGE);

        if (style & wxHF_BOOKMARKS) {
            wxLayoutConstraints *b1 = new wxLayoutConstraints;
            wxBitmapButton *bmpbt = new wxBitmapButton(dummy, wxID_HTML_BOOKMARKSREMOVE, wxBITMAP(wbkdel), wxDefaultPosition, wxSize(20,20));

            b1 -> top.SameAs (dummy, wxTop, 10);
            b1 -> right.SameAs (dummy, wxRight, 10);
            b1 -> height.AsIs();
            b1 -> width.AsIs();
            bmpbt -> SetConstraints(b1);

            wxLayoutConstraints *b2 = new wxLayoutConstraints;
            wxBitmapButton *bmpbt2 = new wxBitmapButton(dummy, wxID_HTML_BOOKMARKSADD, wxBITMAP(wbkadd), wxDefaultPosition, wxSize(20,20));

            b2 -> top.SameAs (dummy, wxTop, 10);
            b2 -> right.LeftOf (bmpbt, 2);
            b2 -> height.AsIs();
            b2 -> width.AsIs();
            bmpbt2 -> SetConstraints(b2);

#if wxUSE_TOOLTIPS
            bmpbt -> SetToolTip(_("Remove current page from bookmarks"));
            bmpbt2 -> SetToolTip(_("Add current page to bookmarks"));
#endif //wxUSE_TOOLTIPS

            wxLayoutConstraints *b3 = new wxLayoutConstraints;
            m_Bookmarks = new wxComboBox(dummy, wxID_HTML_BOOKMARKSLIST, wxEmptyString,
                                         wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY | wxCB_SORT);
            m_Bookmarks -> Append(_("(bookmarks)"));
            for (unsigned i = 0; i < m_BookmarksNames.GetCount(); i++)
                m_Bookmarks -> Append(m_BookmarksNames[i]);
            m_Bookmarks -> SetSelection(0);

            b3 -> centreY.SameAs (bmpbt2, wxCentreY);
            b3 -> left.SameAs (dummy, wxLeft, 10);
            b3 -> right.LeftOf (bmpbt2, 5);
            b3 -> height.AsIs();
            m_Bookmarks -> SetConstraints(b3);


            wxLayoutConstraints *b4 = new wxLayoutConstraints;
            m_ContentsBox = new wxTreeCtrl(dummy, wxID_HTML_TREECTRL,
                                           wxDefaultPosition, wxDefaultSize,
                                           wxTR_HAS_BUTTONS | wxSUNKEN_BORDER);
            m_ContentsBox -> AssignImageList(ContentsImageList);

            b4 -> top.Below (m_Bookmarks, 10);
            b4 -> left.SameAs (dummy, wxLeft, 0);
            b4 -> right.SameAs (dummy, wxRight, 0);
            b4 -> bottom.SameAs (dummy, wxBottom, 0);
            m_ContentsBox -> SetConstraints(b4);

            dummy -> SetAutoLayout(TRUE);
            dummy -> Layout();

            m_NavigPan -> AddPage(dummy, _("Contents"));
        }

        else
        {
            m_ContentsBox = new wxTreeCtrl(m_NavigPan, wxID_HTML_TREECTRL,
                                           wxDefaultPosition, wxDefaultSize,
                                           wxTR_HAS_BUTTONS | wxSUNKEN_BORDER);
            m_ContentsBox -> AssignImageList(ContentsImageList);
            m_NavigPan -> AddPage(m_ContentsBox, _("Contents"));
        }

        m_ContentsPage = notebook_page++;
    }

    // index listbox panel?
    if (style & wxHF_INDEX) {
        wxWindow *dummy = new wxPanel(m_NavigPan, wxID_HTML_INDEXPAGE);

        wxLayoutConstraints *b1 = new wxLayoutConstraints;
        m_IndexText = new wxTextCtrl(dummy, wxID_HTML_INDEXTEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
        b1 -> top.SameAs (dummy, wxTop, 10);
        b1 -> left.SameAs (dummy, wxLeft, 10);
        b1 -> right.SameAs (dummy, wxRight, 10);
        b1 -> height.AsIs();
        m_IndexText -> SetConstraints(b1);

        wxLayoutConstraints *b4 = new wxLayoutConstraints;
        m_IndexButtonAll = new wxButton(dummy, wxID_HTML_INDEXBUTTONALL, _("Show all"));

        b4 -> top.Below (m_IndexText, 10);
        b4 -> right.SameAs (dummy, wxRight, 10);
        b4 -> width.AsIs();
        b4 -> height.AsIs();
        m_IndexButtonAll -> SetConstraints(b4);

        wxLayoutConstraints *b2 = new wxLayoutConstraints;
        m_IndexButton = new wxButton(dummy, wxID_HTML_INDEXBUTTON, _("Find"));
        b2 -> top.Below (m_IndexText, 10);
        b2 -> right.LeftOf (m_IndexButtonAll, 10);
        b2 -> width.AsIs();
        b2 -> height.AsIs();
        m_IndexButton -> SetConstraints(b2);

        wxLayoutConstraints *b5 = new wxLayoutConstraints;
        m_IndexCountInfo = new wxStaticText(dummy, wxID_HTML_COUNTINFO, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE);

        b5 -> top.Below (m_IndexButton, 5);
        b5 -> right.SameAs (dummy, wxRight, 10);
        b5 -> left.SameAs (dummy, wxLeft, 10);
        b5 -> height.AsIs();
        m_IndexCountInfo -> SetConstraints(b5);

        wxLayoutConstraints *b3 = new wxLayoutConstraints;
        m_IndexList = new wxListBox(dummy, wxID_HTML_INDEXLIST, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE);
        b3 -> top.Below (m_IndexCountInfo, 5);
        b3 -> left.SameAs (dummy, wxLeft, 0);
        b3 -> right.SameAs (dummy, wxRight, 0);
        b3 -> bottom.SameAs (dummy, wxBottom, 0);
        m_IndexList -> SetConstraints(b3);

#if wxUSE_TOOLTIPS
        m_IndexButtonAll -> SetToolTip(_("Show all items in index"));
        m_IndexButton -> SetToolTip(_("Display all index items that contain given substring. Search is case insensitive."));
#endif //wxUSE_TOOLTIPS

        dummy -> SetAutoLayout(TRUE);
        dummy -> Layout();

        m_NavigPan -> AddPage(dummy, _("Index"));
        m_IndexPage = notebook_page++;
    }

    // search list panel?
    if (style & wxHF_SEARCH) {
        wxWindow *dummy = new wxPanel(m_NavigPan, wxID_HTML_SEARCHPAGE);

        wxLayoutConstraints *b1 = new wxLayoutConstraints;
        m_SearchText = new wxTextCtrl(dummy, wxID_HTML_SEARCHTEXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
        b1 -> top.SameAs (dummy, wxTop, 10);
        b1 -> left.SameAs (dummy, wxLeft, 10);
        b1 -> right.SameAs (dummy, wxRight, 10);
        b1 -> height.AsIs();
        m_SearchText -> SetConstraints(b1);

        wxLayoutConstraints *b4 = new wxLayoutConstraints;
        m_SearchChoice = new wxChoice(dummy, wxID_HTML_SEARCHCHOICE, wxDefaultPosition,
                                      wxDefaultSize);
        b4 -> top.Below (m_SearchText, 10);
        b4 -> left.SameAs (dummy, wxLeft, 10);
        b4 -> right.SameAs (dummy, wxRight, 10);
        b4 -> height.AsIs();
        m_SearchChoice -> SetConstraints(b4);

        wxLayoutConstraints *b5 = new wxLayoutConstraints;
        m_SearchCaseSensitive = new wxCheckBox(dummy, -1, _("Case sensitive"));
        b5 -> top.Below (m_SearchChoice, 10);
        b5 -> left.SameAs (dummy, wxLeft, 10);
        b5 -> width.AsIs();
        b5 -> height.AsIs ();
        m_SearchCaseSensitive -> SetConstraints(b5);

        wxLayoutConstraints *b6 = new wxLayoutConstraints;
        m_SearchWholeWords = new wxCheckBox(dummy, -1, _("Whole words only"));
        b6 -> top.Below (m_SearchCaseSensitive, 0);
        b6 -> left.SameAs (dummy, wxLeft, 10);
        b6 -> width.AsIs();
        b6 -> height.AsIs ();
        m_SearchWholeWords -> SetConstraints(b6);

        wxLayoutConstraints *b2 = new wxLayoutConstraints;
        m_SearchButton = new wxButton(dummy, wxID_HTML_SEARCHBUTTON, _("Search"));
#if wxUSE_TOOLTIPS
        m_SearchButton -> SetToolTip(_("Search contents of help book(s) for all occurences of the text you typed above"));
#endif //wxUSE_TOOLTIPS
        b2 -> top.Below (m_SearchWholeWords, 0);
        b2 -> right.SameAs (dummy, wxRight, 10);
        b2 -> width.AsIs();
        b2 -> height.AsIs();
        m_SearchButton -> SetConstraints(b2);

        wxLayoutConstraints *b3 = new wxLayoutConstraints;
        m_SearchList = new wxListBox(dummy, wxID_HTML_SEARCHLIST, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE);
        b3 -> top.Below (m_SearchButton, 10);
        b3 -> left.SameAs (dummy, wxLeft, 0);
        b3 -> right.SameAs (dummy, wxRight, 0);
        b3 -> bottom.SameAs (dummy, wxBottom, 0);
        m_SearchList -> SetConstraints(b3);

        dummy -> SetAutoLayout(TRUE);
        dummy -> Layout();
        m_NavigPan -> AddPage(dummy, _("Search"));
        m_SearchPage = notebook_page++;
    }
    m_HtmlWin -> Show(TRUE);

    RefreshLists();

    // showtime
    if (m_NavigPan && m_Splitter) {
        m_Splitter -> SetMinimumPaneSize(20);
        if (m_Cfg.navig_on)
            m_Splitter -> SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);
        else {
            m_Splitter -> SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);
            m_Splitter -> Unsplit();
        }
        if (m_Cfg.navig_on) {
            m_NavigPan -> Show(TRUE);
            m_Splitter -> SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);
        }
        else {
            m_NavigPan -> Show(FALSE);
            m_Splitter -> Initialize(m_HtmlWin);
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
    if (m_PagesHash) delete m_PagesHash;
}


void wxHtmlHelpFrame::AddToolbarButtons(wxToolBar *toolBar, int style)
{
    wxBitmap wpanelBitmap = wxBITMAP(wpanel);
    wxBitmap wbackBitmap = wxBITMAP(wback);
    wxBitmap wforwardBitmap = wxBITMAP(wforward);
    wxBitmap wupnodeBitmap = wxBITMAP(wupnode);
    wxBitmap wupBitmap = wxBITMAP(wup);
    wxBitmap wdownBitmap = wxBITMAP(wdown);
    wxBitmap wopenBitmap = wxBITMAP(wopen);
    wxBitmap wprintBitmap = wxBITMAP(wprint);
    wxBitmap woptionsBitmap = wxBITMAP(woptions);

    wxASSERT_MSG( (wpanelBitmap.Ok() && wbackBitmap.Ok() &&
                   wforwardBitmap.Ok() && wupnodeBitmap.Ok() &&
                   wupBitmap.Ok() && wdownBitmap.Ok() &&
                   wopenBitmap.Ok() && wprintBitmap.Ok() &&
                   woptionsBitmap.Ok()),
                  wxT("One or more HTML help frame toolbar bitmap could not be loaded.")) ;


    toolBar -> AddTool(wxID_HTML_PANEL, wpanelBitmap, wxNullBitmap,
                       FALSE, -1, -1, (wxObject *) NULL,
                       _("Show/hide navigation panel"));

    toolBar -> AddSeparator();
    toolBar -> AddTool(wxID_HTML_BACK, wbackBitmap, wxNullBitmap,
                       FALSE, -1, -1, (wxObject *) NULL,
                       _("Go back"));
    toolBar -> AddTool(wxID_HTML_FORWARD, wforwardBitmap, wxNullBitmap,
                       FALSE, -1, -1, (wxObject *) NULL,
                       _("Go forward"));
    toolBar -> AddSeparator();

    toolBar -> AddTool(wxID_HTML_UPNODE, wupnodeBitmap, wxNullBitmap,
                       FALSE, -1, -1, (wxObject *) NULL,
                       _("Go one level up in document hierarchy"));
    toolBar -> AddTool(wxID_HTML_UP, wupBitmap, wxNullBitmap,
                       FALSE, -1, -1, (wxObject *) NULL,
                       _("Previous page"));
    toolBar -> AddTool(wxID_HTML_DOWN, wdownBitmap, wxNullBitmap,
                       FALSE, -1, -1, (wxObject *) NULL,
                       _("Next page"));

    if ((style & wxHF_PRINT) || (style & wxHF_OPENFILES))
        toolBar -> AddSeparator();

    if (style & wxHF_OPENFILES)
        toolBar -> AddTool(wxID_HTML_OPENFILE, wopenBitmap, wxNullBitmap,
                           FALSE, -1, -1, (wxObject *) NULL,
                           _("Open HTML document"));

#if wxUSE_PRINTING_ARCHITECTURE
    if (style & wxHF_PRINT)
        toolBar -> AddTool(wxID_HTML_PRINT, wprintBitmap, wxNullBitmap,
                           FALSE, -1, -1, (wxObject *) NULL,
                           _("Print this page"));
#endif

    toolBar -> AddSeparator();
    toolBar -> AddTool(wxID_HTML_OPTIONS, woptionsBitmap, wxNullBitmap,
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
    if (! url.IsEmpty()) {
        m_HtmlWin->LoadPage(url);
        NotifyPageChanged();
        return TRUE;
    }
    return FALSE;
}

bool wxHtmlHelpFrame::Display(const int id)
{
    wxString url = m_Data->FindPageById(id);
    if (! url.IsEmpty()) {
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
    if (!m_Splitter -> IsSplit()) {
        m_NavigPan -> Show(TRUE);
        m_HtmlWin -> Show(TRUE);
        m_Splitter -> SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);
        m_Cfg.navig_on = TRUE;
    }
    m_NavigPan -> SetSelection(0);
    return TRUE;
}



bool wxHtmlHelpFrame::DisplayIndex()
{
    if (! m_IndexList)
        return FALSE;
    if (!m_Splitter -> IsSplit()) {
        m_NavigPan -> Show(TRUE);
        m_HtmlWin -> Show(TRUE);
        m_Splitter -> SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);
    }
    m_NavigPan -> SetSelection(1);
    return TRUE;
}



bool wxHtmlHelpFrame::KeywordSearch(const wxString& keyword)
{
    if (! (m_SearchList && m_SearchButton && m_SearchText && m_SearchChoice))
        return FALSE;

    int foundcnt = 0, curi;
    wxString foundstr;
    wxString book = wxEmptyString;

    if (!m_Splitter -> IsSplit()) {
        m_NavigPan -> Show(TRUE);
        m_HtmlWin -> Show(TRUE);
        m_Splitter -> SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);
    }
    m_NavigPan -> SetSelection(m_SearchPage);
    m_SearchList -> Clear();
    m_SearchText -> SetValue(keyword);
    m_SearchButton -> Enable(FALSE);

    if (m_SearchChoice->GetSelection() != 0)
        book = m_SearchChoice->GetStringSelection();

    wxHtmlSearchStatus status(m_Data, keyword,
                              m_SearchCaseSensitive -> GetValue(), m_SearchWholeWords -> GetValue(),
                              book);

    wxProgressDialog progress(_("Searching..."), _("No matching page found yet"),
                              status.GetMaxIndex(), this,
                              wxPD_APP_MODAL | wxPD_CAN_ABORT | wxPD_AUTO_HIDE);

    while (status.IsActive()) {
        curi = status.GetCurIndex();
        if (curi % 32 == 0 && progress.Update(curi) == FALSE)
            break;
        if (status.Search()) {
            foundstr.Printf(_("Found %i matches"), ++foundcnt);
            progress.Update(status.GetCurIndex(), foundstr);
            m_SearchList -> Append(status.GetName(), status.GetContentsItem());
        }
    }

    m_SearchButton -> Enable(TRUE);
    m_SearchText -> SetSelection(0, keyword.Length());
    m_SearchText -> SetFocus();
    if (foundcnt) {
        wxHtmlContentsItem *it = (wxHtmlContentsItem*) m_SearchList -> GetClientData(0);
        if (it)
        {
            m_HtmlWin -> LoadPage(it -> m_Book -> GetBasePath() + it -> m_Page);
            NotifyPageChanged();
        }
    }
    return (foundcnt > 0);
}

#define MAX_ROOTS 64

void wxHtmlHelpFrame::CreateContents()
{
    if (! m_ContentsBox)
        return ;

    m_ContentsBox->Clear();

    if (m_PagesHash) delete m_PagesHash;
    m_PagesHash = new wxHashTable(wxKEY_STRING, 2 * m_Data -> GetContentsCnt());
    m_PagesHash -> DeleteContents(TRUE);

    int cnt = m_Data->GetContentsCnt();
    int i;

    wxHtmlContentsItem *it;

    wxTreeItemId roots[MAX_ROOTS];
    bool imaged[MAX_ROOTS];

    m_ContentsBox -> DeleteAllItems();
    roots[0] = m_ContentsBox -> AddRoot(_("(Help)"));
    m_ContentsBox -> SetItemImage(roots[0], IMG_RootFolder);
    m_ContentsBox -> SetItemSelectedImage(roots[0], IMG_RootFolder);
    imaged[0] = TRUE;

    for (it = m_Data->GetContents(), i = 0; i < cnt; i++, it++) {
        roots[it -> m_Level + 1] =  m_ContentsBox -> AppendItem(
                                       roots[it -> m_Level], it -> m_Name, IMG_Page, -1,
                                       new wxHtmlHelpTreeItemData(i));
        m_PagesHash -> Put(it -> m_Book -> GetBasePath() + it -> m_Page,
                           new wxHtmlHelpHashData(i, roots[it -> m_Level + 1]));

        if (it -> m_Level == 0) {
            m_ContentsBox -> SetItemBold(roots[1], TRUE);
            m_ContentsBox -> SetItemImage(roots[1], IMG_Book);
            m_ContentsBox -> SetItemSelectedImage(roots[1], IMG_Book);
            imaged[1] = TRUE;
        } else imaged[it -> m_Level + 1] = FALSE;

        if (!imaged[it -> m_Level]) {
            m_ContentsBox -> SetItemImage(roots[it -> m_Level], IMG_Folder);
            m_ContentsBox -> SetItemSelectedImage(roots[it -> m_Level], IMG_Folder);
            imaged[it -> m_Level] = TRUE;
        }
    }
    m_ContentsBox -> Expand(roots[0]);
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
    m_IndexCountInfo -> SetLabel(cnttext);
    if (cnt > INDEX_IS_SMALL) return;

    wxHtmlContentsItem* index = m_Data->GetIndex();

    for (int i = 0; i < cnt; i++)
        m_IndexList -> Append(index[i].m_Name, (char*)(index + i));
}

void wxHtmlHelpFrame::CreateSearch()
{
    if (! (m_SearchList && m_SearchChoice))
        return ;
    m_SearchList -> Clear();
    m_SearchChoice -> Clear();
    m_SearchChoice -> Append(_("Search in all books"));
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

    if (path != wxEmptyString) {
        oldpath = cfg -> GetPath();
        cfg -> SetPath(_T("/") + path);
    }

    m_Cfg.navig_on = cfg -> Read(wxT("hcNavigPanel"), m_Cfg.navig_on) != 0;
    m_Cfg.sashpos = cfg -> Read(wxT("hcSashPos"), m_Cfg.sashpos);
    m_Cfg.x = cfg -> Read(wxT("hcX"), m_Cfg.x);
    m_Cfg.y = cfg -> Read(wxT("hcY"), m_Cfg.y);
    m_Cfg.w = cfg -> Read(wxT("hcW"), m_Cfg.w);
    m_Cfg.h = cfg -> Read(wxT("hcH"), m_Cfg.h);

    m_FixedFace = cfg -> Read(wxT("hcFixedFace"), m_FixedFace);
    m_NormalFace = cfg -> Read(wxT("hcNormalFace"), m_NormalFace);
    m_FontSize = cfg -> Read(wxT("hcFontSize"), m_FontSize);

    {
        int i;
        int cnt;
        wxString val, s;

        cnt = cfg -> Read(wxT("hcBookmarksCnt"), 0L);
        if (cnt != 0) {
            m_BookmarksNames.Clear();
            m_BookmarksPages.Clear();
            if (m_Bookmarks) {
                m_Bookmarks -> Clear();
                m_Bookmarks -> Append(_("(bookmarks)"));
            }

            for (i = 0; i < cnt; i++) {
                val.Printf(wxT("hcBookmark_%i"), i);
                s = cfg -> Read(val);
                m_BookmarksNames.Add(s);
                if (m_Bookmarks) m_Bookmarks -> Append(s);
                val.Printf(wxT("hcBookmark_%i_url"), i);
                s = cfg -> Read(val);
                m_BookmarksPages.Add(s);
            }
        }
    }

    if (m_HtmlWin)
        m_HtmlWin->ReadCustomization(cfg);

    if (path != wxEmptyString)
        cfg -> SetPath(oldpath);
}

void wxHtmlHelpFrame::WriteCustomization(wxConfigBase *cfg, const wxString& path)
{
    wxString oldpath;
    wxString tmp;

    if (path != wxEmptyString) {
        oldpath = cfg -> GetPath();
        cfg -> SetPath(_T("/") + path);
    }

    cfg -> Write(wxT("hcNavigPanel"), m_Cfg.navig_on);
    cfg -> Write(wxT("hcSashPos"), (long)m_Cfg.sashpos);
    cfg -> Write(wxT("hcX"), (long)m_Cfg.x);
    cfg -> Write(wxT("hcY"), (long)m_Cfg.y);
    cfg -> Write(wxT("hcW"), (long)m_Cfg.w);
    cfg -> Write(wxT("hcH"), (long)m_Cfg.h);
    cfg -> Write(wxT("hcFixedFace"), m_FixedFace);
    cfg -> Write(wxT("hcNormalFace"), m_NormalFace);
    cfg -> Write(wxT("hcFontSize"), (long)m_FontSize);

    if (m_Bookmarks) {
        int i;
        int cnt = m_BookmarksNames.GetCount();
        wxString val;

        cfg -> Write(wxT("hcBookmarksCnt"), (long)cnt);
        for (i = 0; i < cnt; i++) {
            val.Printf(wxT("hcBookmark_%i"), i);
            cfg -> Write(val, m_BookmarksNames[i]);
            val.Printf(wxT("hcBookmark_%i_url"), i);
            cfg -> Write(val, m_BookmarksPages[i]);
        }
    }

    if (m_HtmlWin)
        m_HtmlWin->WriteCustomization(cfg);

    if (path != wxEmptyString)
        cfg -> SetPath(oldpath);
}





static void SetFontsToHtmlWin(wxHtmlWindow *win, wxString scalf, wxString fixf, int size)
{
    static int f_sizes[5][7] =
        {
            { 6,  7,  9, 12, 14, 16, 19},
            { 8,  9, 12, 14, 16, 19, 22},
            {10, 12, 14, 16, 19, 24, 32},
            {14, 16, 18, 24, 32, 38, 45},
            {16, 20, 24, 32, 38, 45, 50}
        };

    win -> SetFonts(scalf, fixf, f_sizes[size]);
}


class wxHtmlHelpFrameOptionsDialog : public wxDialog
{
    public:
        wxComboBox *NormalFont, *FixedFont;
        wxRadioBox *RadioBox;
        wxHtmlWindow *TestWin;

        wxHtmlHelpFrameOptionsDialog(wxWindow *parent) : wxDialog(parent, -1, wxString(_("Help Browser Options")))
            {
                wxString choices[5] = {_("very small"), _("small"), _("medium"), _("large"), _("very large")};
                wxBoxSizer *topsizer, *sizer, *sizer2;

                topsizer = new wxBoxSizer(wxVERTICAL);

                sizer = new wxBoxSizer(wxHORIZONTAL);

                sizer2 = new wxStaticBoxSizer( new wxStaticBox(this, -1, _("Normal font:")), wxVERTICAL);
                sizer2 -> Add(NormalFont = new wxComboBox(this, -1, wxEmptyString, wxDefaultPosition,
                              wxSize(200, 200),
                              0, NULL, wxCB_DROPDOWN | wxCB_READONLY),
                              1, wxEXPAND | wxLEFT | wxRIGHT, 10);

                sizer -> Add(sizer2, 0, wxEXPAND | wxLEFT|wxRIGHT|wxTOP, 10);

                sizer2 = new wxStaticBoxSizer( new wxStaticBox(this, -1, _("Fixed font:")), wxVERTICAL);
                sizer2 -> Add(FixedFont = new wxComboBox(this, -1, wxEmptyString, wxDefaultPosition,
                              wxSize(200, 200),
                              0, NULL, wxCB_DROPDOWN | wxCB_READONLY),
                              1, wxEXPAND | wxLEFT | wxRIGHT, 10);

                sizer -> Add(sizer2, 0, wxEXPAND | wxLEFT|wxRIGHT|wxTOP, 10);

                topsizer -> Add(sizer);

                topsizer -> Add(RadioBox = new wxRadioBox(this, -1, _("Font size:"),
                                                          wxDefaultPosition, wxDefaultSize, 5, choices, 5),
                                0, wxEXPAND | wxLEFT|wxRIGHT|wxTOP, 10);

                topsizer -> Add(new wxStaticText(this, -1, _("Preview:")),
                                0, wxLEFT | wxTOP, 10);
                topsizer -> Add(TestWin = new wxHtmlWindow(this, -1, wxDefaultPosition, wxSize(-1, 150)),
                                1, wxEXPAND | wxLEFT|wxTOP|wxRIGHT, 10);

                sizer = new wxBoxSizer(wxHORIZONTAL);
                sizer -> Add(new wxButton(this, wxID_OK, _("OK")), 0, wxALL, 10);
                sizer -> Add(new wxButton(this, wxID_CANCEL, _("Cancel")), 0, wxALL, 10);
                topsizer -> Add(sizer, 0, wxALIGN_RIGHT);

                SetAutoLayout(TRUE);
                SetSizer(topsizer);
                topsizer -> Fit(this);
                Centre(wxBOTH);
            }


        void UpdateTestWin()
            {
                wxBusyCursor bcur;
                SetFontsToHtmlWin(TestWin,
                                  NormalFont -> GetStringSelection(),
                                  FixedFont -> GetStringSelection(),
                                  RadioBox -> GetSelection());
                TestWin -> SetPage(_(
"<html><body>\
Normal face<br>(and <u>underlined</u>. <i>Italic face.</i> \
<b>Bold face.</b> <b><i>Bold italic face.</i></b><br>\
<font size=-2>font size -2</font><br>\
<font size=-1>font size -1</font><br>\
<font size=+0>font size +0</font><br>\
<font size=+1>font size +1</font><br>\
<font size=+2>font size +2</font><br>\
<font size=+3>font size +3</font><br>\
<font size=+4>font size +4</font><br>\
\
<p><tt>Fixed size face.<br> <b>bold</b> <i>italic</i> \
<b><i>bold italic <u>underlined</u></i></b><br>\
<font size=-2>font size -2</font><br>\
<font size=-1>font size -1</font><br>\
<font size=+0>font size +0</font><br>\
<font size=+1>font size +1</font><br>\
<font size=+2>font size +2</font><br>\
<font size=+3>font size +3</font><br>\
<font size=+4>font size +4</font></tt>\
</body></html>"
                                  ));
            }

        void OnUpdate(wxCommandEvent& event)
            {
                UpdateTestWin();
            }

        DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxHtmlHelpFrameOptionsDialog, wxDialog)
    EVT_COMBOBOX(-1, wxHtmlHelpFrameOptionsDialog::OnUpdate)
    EVT_RADIOBOX(-1, wxHtmlHelpFrameOptionsDialog::OnUpdate)
END_EVENT_TABLE()


void wxHtmlHelpFrame::OptionsDialog()
{
    wxHtmlHelpFrameOptionsDialog dlg(this);
    unsigned i;

    if (m_NormalFonts == NULL) {
        wxFontEnumerator enu;
        enu.EnumerateFacenames();
        m_NormalFonts = new wxArrayString;
        *m_NormalFonts = *enu.GetFacenames();
        m_NormalFonts -> Sort();
    }
    if (m_FixedFonts == NULL) {
        wxFontEnumerator enu;
        enu.EnumerateFacenames(wxFONTENCODING_SYSTEM, TRUE);
        m_FixedFonts = new wxArrayString;
        *m_FixedFonts = *enu.GetFacenames();
        m_FixedFonts -> Sort();
    }

    for (i = 0; i < m_NormalFonts -> GetCount(); i++)
        dlg.NormalFont -> Append((*m_NormalFonts)[i]);
    for (i = 0; i < m_FixedFonts -> GetCount(); i++)
        dlg.FixedFont -> Append((*m_FixedFonts)[i]);
    if (!m_NormalFace.IsEmpty()) dlg.NormalFont -> SetStringSelection(m_NormalFace);
    else dlg.NormalFont -> SetSelection(0);
    if (!m_FixedFace.IsEmpty()) dlg.FixedFont -> SetStringSelection(m_FixedFace);
    else dlg.FixedFont -> SetSelection(0);
    dlg.RadioBox -> SetSelection(m_FontSize);
    dlg.UpdateTestWin();

    if (dlg.ShowModal() == wxID_OK) {
        m_NormalFace = dlg.NormalFont -> GetStringSelection();
        m_FixedFace = dlg.FixedFont -> GetStringSelection();
        m_FontSize = dlg.RadioBox -> GetSelection();
        SetFontsToHtmlWin(m_HtmlWin, m_NormalFace, m_FixedFace, m_FontSize);
    }
}



void wxHtmlHelpFrame::NotifyPageChanged()
{
    if (m_UpdateContents && m_PagesHash)
    {
        wxString an = m_HtmlWin -> GetOpenedAnchor();
        wxHtmlHelpHashData *ha;
        if (an.IsEmpty())
            ha = (wxHtmlHelpHashData*) m_PagesHash -> Get(m_HtmlWin -> GetOpenedPage());
        else
            ha = (wxHtmlHelpHashData*) m_PagesHash -> Get(m_HtmlWin -> GetOpenedPage() + wxT("#") + an);
        if (ha)
        {
            bool olduc = m_UpdateContents;
            m_UpdateContents = FALSE;
            m_ContentsBox -> SelectItem(ha -> m_Id);
            m_ContentsBox -> EnsureVisible(ha -> m_Id);
            m_UpdateContents = olduc;
        }
    }
}



/*
EVENT HANDLING :
*/


void wxHtmlHelpFrame::OnToolbar(wxCommandEvent& event)
{
    switch (event.GetId()) {

        case wxID_HTML_BACK :
            m_HtmlWin -> HistoryBack();
            NotifyPageChanged();
            break;

        case wxID_HTML_FORWARD :
            m_HtmlWin -> HistoryForward();
            NotifyPageChanged();
            break;

        case wxID_HTML_UP :
            if (m_PagesHash)
            {
                wxString an = m_HtmlWin -> GetOpenedAnchor();
                wxHtmlHelpHashData *ha;
                if (an.IsEmpty())
                    ha = (wxHtmlHelpHashData*) m_PagesHash -> Get(m_HtmlWin -> GetOpenedPage());
                else
                    ha = (wxHtmlHelpHashData*) m_PagesHash -> Get(m_HtmlWin -> GetOpenedPage() + wxT("#") + an);
                if (ha && ha -> m_Index > 0)
                {
                    wxHtmlContentsItem *it = m_Data -> GetContents() + (ha -> m_Index - 1);
                    m_HtmlWin -> LoadPage(it -> m_Book -> GetBasePath() + it -> m_Page);
                    NotifyPageChanged();
                }
            }
            break;

        case wxID_HTML_UPNODE :
            if (m_PagesHash)
            {
                wxString an = m_HtmlWin -> GetOpenedAnchor();
                wxHtmlHelpHashData *ha;
                if (an.IsEmpty())
                    ha = (wxHtmlHelpHashData*) m_PagesHash -> Get(m_HtmlWin -> GetOpenedPage());
                else
                    ha = (wxHtmlHelpHashData*) m_PagesHash -> Get(m_HtmlWin -> GetOpenedPage() + wxT("#") + an);
                if (ha && ha -> m_Index > 0)
                {
                    int level = m_Data -> GetContents()[ha -> m_Index].m_Level - 1;
                    wxHtmlContentsItem *it;
                    int ind = ha -> m_Index - 1;

                    it = m_Data -> GetContents() + ind;
                    while (ind >= 0 && it -> m_Level != level) ind--, it--;
                    if (ind >= 0)
                    {
                        m_HtmlWin -> LoadPage(it -> m_Book -> GetBasePath() + it -> m_Page);
                        NotifyPageChanged();
                    }
                }
            }
            break;

        case wxID_HTML_DOWN :
            if (m_PagesHash)
            {
                wxString an = m_HtmlWin -> GetOpenedAnchor();
                wxString adr;
                wxHtmlHelpHashData *ha;

                if (an.IsEmpty()) adr = m_HtmlWin -> GetOpenedPage();
                else adr = m_HtmlWin -> GetOpenedPage() + wxT("#") + an;

                ha = (wxHtmlHelpHashData*) m_PagesHash -> Get(adr);

                if (ha && ha -> m_Index < m_Data -> GetContentsCnt() - 1)
                {
                    wxHtmlContentsItem *it = m_Data -> GetContents() + (ha -> m_Index + 1);

                    while (it -> m_Book -> GetBasePath() + it -> m_Page == adr) it++;
                    m_HtmlWin -> LoadPage(it -> m_Book -> GetBasePath() + it -> m_Page);
                    NotifyPageChanged();
                }
            }
            break;

        case wxID_HTML_PANEL :
            {
                if (! (m_Splitter && m_NavigPan))
                    return ;
                if (m_Splitter -> IsSplit()) {
                    m_Cfg.sashpos = m_Splitter -> GetSashPosition();
                    m_Splitter -> Unsplit(m_NavigPan);
                    m_Cfg.navig_on = FALSE;
                } else {
                    m_NavigPan -> Show(TRUE);
                    m_HtmlWin -> Show(TRUE);
                    m_Splitter -> SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);
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

                item = m_HtmlWin -> GetOpenedPageTitle();
                url = m_HtmlWin -> GetOpenedPage();
                if (item == wxEmptyString) item = url.AfterLast(wxT('/'));
                if (m_BookmarksPages.Index(url) == wxNOT_FOUND) {
                    m_Bookmarks -> Append(item);
                    m_BookmarksNames.Add(item);
                    m_BookmarksPages.Add(url);
                }
            }
            break;

        case wxID_HTML_BOOKMARKSREMOVE :
            {
                wxString item;
                int pos;

                item = m_Bookmarks -> GetStringSelection();
                pos = m_BookmarksNames.Index(item);
                if (pos != wxNOT_FOUND) {
                    m_BookmarksNames.Remove(pos);
                    m_BookmarksPages.Remove(pos);
                    m_Bookmarks -> Delete(m_Bookmarks -> GetSelection());
                }
            }
            break;

#if wxUSE_PRINTING_ARCHITECTURE
        case wxID_HTML_PRINT :
            {
                if (m_Printer == NULL)
                    m_Printer = new wxHtmlEasyPrinting(_("Help Printing"), this);
                if (!m_HtmlWin -> GetOpenedPage())
                    wxLogWarning(_("Cannot print empty page."));
                else
                    m_Printer -> PrintFile(m_HtmlWin -> GetOpenedPage());
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
                        m_Data -> AddBook(s);
                        RefreshLists();
                    }
                    else
                        m_HtmlWin -> LoadPage(s);
                }
            }
            break;
    }
}



void wxHtmlHelpFrame::OnContentsSel(wxTreeEvent& event)
{
    wxHtmlHelpTreeItemData *pg;
    wxHtmlContentsItem *it;

    pg = (wxHtmlHelpTreeItemData*) m_ContentsBox -> GetItemData(event.GetItem());

    if (pg && m_UpdateContents)
    {
        it = m_Data -> GetContents() + (pg -> m_Id);
        m_UpdateContents = FALSE;
        m_HtmlWin -> LoadPage(it -> m_Book -> GetBasePath() + it -> m_Page);
        m_UpdateContents = TRUE;
    }
}



void wxHtmlHelpFrame::OnIndexSel(wxCommandEvent& WXUNUSED(event))
{
    wxHtmlContentsItem *it = (wxHtmlContentsItem*) m_IndexList -> GetClientData(m_IndexList -> GetSelection());
    m_HtmlWin -> LoadPage(it -> m_Book -> GetBasePath() + it -> m_Page);
    NotifyPageChanged();
}


void wxHtmlHelpFrame::OnIndexFind(wxCommandEvent& event)
{
    wxString sr = m_IndexText -> GetLineText(0);
    sr.MakeLower();
    if (sr == wxEmptyString)
        OnIndexAll(event);

    else {
        wxBusyCursor bcur;
        const wxChar *cstr = sr.c_str();
        wxChar mybuff[512], *ptr;
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
            if (wxStrstr(mybuff, cstr) != NULL) {
                m_IndexList -> Append(index[i].m_Name, (char*)(index + i));
                displ++;
                if (first) {
                    m_HtmlWin -> LoadPage(index[i].m_Book -> GetBasePath() + index[i].m_Page);
                    NotifyPageChanged();
                    first = FALSE;
                }
            }
        }

        wxString cnttext;
        cnttext.Printf(_("%i of %i"), displ, cnt);
        m_IndexCountInfo -> SetLabel(cnttext);

        m_IndexText -> SetSelection(0, sr.Length());
        m_IndexText -> SetFocus();
    }
}

void wxHtmlHelpFrame::OnIndexAll(wxCommandEvent& WXUNUSED(event))
{
    wxBusyCursor bcur;

    m_IndexList->Clear();
    int cnt = m_Data->GetIndexCnt();
    bool first = TRUE;
    wxHtmlContentsItem* index = m_Data->GetIndex();

    for (int i = 0; i < cnt; i++) {
        m_IndexList -> Append(index[i].m_Name, (char*)(index + i));
        if (first) {
            m_HtmlWin -> LoadPage(index[i].m_Book -> GetBasePath() + index[i].m_Page);
            NotifyPageChanged();
            first = FALSE;
        }
    }

    wxString cnttext;
    cnttext.Printf(_("%i of %i"), cnt, cnt);
    m_IndexCountInfo -> SetLabel(cnttext);
}


void wxHtmlHelpFrame::OnSearchSel(wxCommandEvent& WXUNUSED(event))
{
    wxHtmlContentsItem *it = (wxHtmlContentsItem*) m_SearchList -> GetClientData(m_SearchList -> GetSelection());
    if (it)
    {
        m_HtmlWin -> LoadPage(it -> m_Book -> GetBasePath() + it -> m_Page);
        NotifyPageChanged();
    }
}

void wxHtmlHelpFrame::OnSearch(wxCommandEvent& WXUNUSED(event))
{
    wxString sr = m_SearchText -> GetLineText(0);

    if (sr != wxEmptyString) KeywordSearch(sr);
}

void wxHtmlHelpFrame::OnBookmarksSel(wxCommandEvent& WXUNUSED(event))
{
    wxString sr = m_Bookmarks -> GetStringSelection();

    if (sr != wxEmptyString && sr != _("(bookmarks)"))
    {
       m_HtmlWin -> LoadPage(m_BookmarksPages[m_BookmarksNames.Index(sr)]);
       NotifyPageChanged();
    }
}

void wxHtmlHelpFrame::OnCloseWindow(wxCloseEvent& evt)
{
    GetSize(&m_Cfg.w, &m_Cfg.h);
    GetPosition(&m_Cfg.x, &m_Cfg.y);

    if (m_Splitter && m_Cfg.navig_on) m_Cfg.sashpos = m_Splitter -> GetSashPosition();

    if (m_Config)
        WriteCustomization(m_Config, m_ConfigRoot);

    if (m_helpController && m_helpController->IsKindOf(CLASSINFO(wxHtmlHelpController)))
    {
        ((wxHtmlHelpController*) m_helpController)->OnCloseFrame(evt);
    }

    evt.Skip();
}

BEGIN_EVENT_TABLE(wxHtmlHelpFrame, wxFrame)
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
END_EVENT_TABLE()

#endif
