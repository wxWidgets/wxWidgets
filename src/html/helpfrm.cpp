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

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/defs.h"

#if wxUSE_HTML

#ifndef WXPRECOMP
#include <wx/wx.h>
#endif

#include "wx/html/helpfrm.h"
#include "wx/notebook.h"
#include "wx/imaglist.h"
#include "wx/treectrl.h"
#include "wx/tokenzr.h"
#include "wx/wfstream.h"
#include "wx/html/htmlwin.h"
#include "wx/busyinfo.h"
#include "wx/progdlg.h"

// Bitmaps:

#ifndef __WXMSW__
#include "bitmaps/panel.xpm"
#include "bitmaps/back.xpm"
#include "bitmaps/forward.xpm"
#include "bitmaps/book.xpm"
#include "bitmaps/folder.xpm"
#include "bitmaps/page.xpm"
#endif

#include "wx/stream.h"

// number of times that the contents/index creation progress dialog
// is updated.
#define PROGRESS_STEP 40

//--------------------------------------------------------------------------
// wxHtmlHelpTreeItemData
//--------------------------------------------------------------------------

class wxHtmlHelpTreeItemData : public wxTreeItemData
{
    private:
        wxString m_Page;

    public:
        wxHtmlHelpTreeItemData(wxHtmlContentsItem *it) : wxTreeItemData()
        {
            m_Page = it -> m_Book -> GetBasePath() + it -> m_Page;
        }
        const wxString& GetPage() { return m_Page; }
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

    m_ContentsImageList = new wxImageList(12, 12);
    m_ContentsImageList -> Add(wxICON(book));
    m_ContentsImageList -> Add(wxICON(folder));
    m_ContentsImageList -> Add(wxICON(page));

    m_ContentsBox = NULL;
    m_IndexBox = NULL;
    m_SearchList = NULL;
    m_SearchButton = NULL;
    m_SearchText = NULL;
    m_SearchChoice = NULL;
    m_Splitter = NULL;
    m_NavigPan = NULL;
    m_HtmlWin = NULL;
    m_Config = NULL;
    m_ConfigRoot = wxEmptyString;

    m_Cfg.x = m_Cfg.y = 0;
    m_Cfg.w = 700;
    m_Cfg.h = 480;
    m_Cfg.sashpos = 240;
    m_Cfg.navig_on = TRUE;
}

// Create: builds the GUI components.
// with the style flag it's possible to toggle the toolbar, contents, index and search
// controls.
// m_HtmlWin will *always* be created, but it's important to realize that
// m_ContentsBox, m_IndexBox, m_SearchList, m_SearchButton, m_SearchText and
// m_SearchButton may be NULL.
// moreover, if no contents, index or searchpage is needed, m_Splitter and
// m_NavigPan will be NULL too (with m_HtmlWin directly connected to the frame)

bool wxHtmlHelpFrame::Create(wxWindow* parent, wxWindowID id, const wxString& title,
                             int style)
{
    // Do the config in two steps. We read the HtmlWindow customization after we
    // create the window.
    if (m_Config)
        ReadCustomization(m_Config, m_ConfigRoot);

    wxFrame::Create(parent, id, "", wxPoint(m_Cfg.x, m_Cfg.y), wxSize(m_Cfg.w, m_Cfg.h));

    int notebook_page = 0;

    CreateStatusBar();

    // toolbar?
    if (style & wxHF_TOOLBAR) {
        wxToolBar *toolBar = CreateToolBar(wxNO_BORDER | wxTB_HORIZONTAL |  /*wxTB_FLAT | */
                                           wxTB_DOCKABLE);
        toolBar -> SetMargins(2, 2);
        wxBitmap* toolBarBitmaps[3];

#ifdef __WXMSW__
        toolBarBitmaps[0] = new wxBitmap("panel");
        toolBarBitmaps[1] = new wxBitmap("back");
        toolBarBitmaps[2] = new wxBitmap("forward");
        int width = 24;
#else
toolBarBitmaps[0] = new wxBitmap(panel_xpm);
        toolBarBitmaps[1] = new wxBitmap(back_xpm);
        toolBarBitmaps[2] = new wxBitmap(forward_xpm);
        int width = 16;
#endif

        int currentX = 5;

        toolBar -> AddTool(wxID_HTML_PANEL, *(toolBarBitmaps[0]), wxNullBitmap,
                           FALSE, currentX, -1, (wxObject *) NULL,
                           _("Show/hide navigation panel"));
        currentX += width + 5;
        toolBar -> AddSeparator();
        toolBar -> AddTool(wxID_HTML_BACK, *(toolBarBitmaps[1]), wxNullBitmap,
                           FALSE, currentX, -1, (wxObject *) NULL,
                           _("Go back to the previous HTML page"));
        currentX += width + 5;
        toolBar -> AddTool(wxID_HTML_FORWARD, *(toolBarBitmaps[2]), wxNullBitmap,
                           FALSE, currentX, -1, (wxObject *) NULL,
                           _("Go forward to the next HTML page"));
        currentX += width + 5;

        toolBar -> Realize();

        // Can delete the bitmaps since they're reference counted
        for (int i = 0; i < 3; i++)
            delete toolBarBitmaps[i];
    }

    if (style & (wxHF_CONTENTS | wxHF_INDEX | wxHF_SEARCH)) {
        // traditional help controller; splitter window with html page on the
        // right and a notebook containing various pages on the left
        m_Splitter = new wxSplitterWindow(this);

        m_HtmlWin = new wxHtmlWindow(m_Splitter);
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
        m_ContentsBox = new wxTreeCtrl(m_NavigPan, wxID_HTML_TREECTRL,
                                       wxDefaultPosition, wxDefaultSize,
                                       wxTR_HAS_BUTTONS | wxSUNKEN_BORDER);
        m_ContentsBox -> SetImageList(m_ContentsImageList);
        m_NavigPan -> AddPage(m_ContentsBox, _("Contents"));
        m_ContentsPage = notebook_page++;
    }

    // index listbox panel?
    if (style & wxHF_INDEX) {
        wxWindow *dummy = new wxPanel(m_NavigPan, wxID_HTML_INDEXPAGE);
        wxLayoutConstraints *b1 = new wxLayoutConstraints;
        b1 -> top.SameAs (dummy, wxTop, 0);
        b1 -> left.SameAs (dummy, wxLeft, 0);
        b1 -> width.PercentOf (dummy, wxWidth, 100);
        b1 -> bottom.SameAs (dummy, wxBottom, 0);
        m_IndexBox = new wxListBox(dummy, wxID_HTML_INDEXLIST, wxDefaultPosition,
                                   wxDefaultSize, 0, NULL, wxLB_SINGLE | wxLB_ALWAYS_SB);
        m_IndexBox -> SetConstraints(b1);
        dummy -> SetAutoLayout(TRUE);
        m_NavigPan -> AddPage(dummy, _("Index"));
        m_IndexPage = notebook_page++;
    }

    // search list panel?
    if (style & wxHF_SEARCH) {
        wxWindow *dummy = new wxPanel(m_NavigPan, wxID_HTML_SEARCHPAGE);

        wxLayoutConstraints *b1 = new wxLayoutConstraints;
        m_SearchText = new wxTextCtrl(dummy, wxID_HTML_SEARCHTEXT);
        b1 -> top.SameAs (dummy, wxTop, 10);
        b1 -> left.SameAs (dummy, wxLeft, 10);
        b1 -> right.SameAs (dummy, wxRight, 10);
        b1 -> height.AsIs();
        m_SearchText -> SetConstraints(b1);

        wxLayoutConstraints *b2 = new wxLayoutConstraints;
        m_SearchButton = new wxButton(dummy, wxID_HTML_SEARCHBUTTON, _("Search"));
        b2 -> top.Below (m_SearchText, 10);
        b2 -> left.SameAs (dummy, wxLeft, 10);
        b2 -> width.AsIs();
        b2 -> height.AsIs();
        m_SearchButton -> SetConstraints(b2);

        wxLayoutConstraints *b3 = new wxLayoutConstraints;
        m_SearchList = new wxListBox(dummy, wxID_HTML_SEARCHLIST, wxDefaultPosition, wxDefaultSize, 0);
        b3 -> top.Below (m_SearchButton, 10);
        b3 -> left.SameAs (dummy, wxLeft, 0);
        b3 -> right.SameAs (dummy, wxRight, 0);
        b3 -> bottom.SameAs (dummy, wxBottom, 0);
        m_SearchList -> SetConstraints(b3);

        wxLayoutConstraints *b4 = new wxLayoutConstraints;
        m_SearchChoice = new wxChoice(dummy, wxID_HTML_SEARCHCHOICE, wxDefaultPosition,
                                      wxDefaultSize);
        b4 -> top.Below (m_SearchText, 10);
        b4 -> left.SameAs (m_SearchButton, wxRight, 10);
        b4 -> right.SameAs (dummy, wxRight, 10);
        b4 -> height.AsIs();
        m_SearchChoice -> SetConstraints(b4);

        dummy -> SetAutoLayout(TRUE);
        dummy -> Layout();
        m_NavigPan -> AddPage(dummy, _("Search"));
        m_SearchPage = notebook_page++;
    }

    //RefreshLists();

    // showtime
    if (m_NavigPan && m_Splitter) {
        m_NavigPan -> Show(TRUE);
        m_Splitter -> SetMinimumPaneSize(20);
        if (m_Cfg.navig_on)
            m_Splitter -> SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);
    }
    m_HtmlWin -> Show(TRUE);
    return TRUE;
}

wxHtmlHelpFrame::~wxHtmlHelpFrame()
{
    delete m_ContentsImageList;
    if (m_DataCreated)
        delete m_Data;
}

bool wxHtmlHelpFrame::Display(const wxString& x)
{
    wxString url = m_Data->FindPageByName(x);
    if (! url.IsEmpty()) {
        m_HtmlWin->LoadPage(url);
        return TRUE;
    }
    return FALSE;
}

bool wxHtmlHelpFrame::Display(const int id)
{
    wxString url = m_Data->FindPageById(id);
    if (! url.IsEmpty()) {
        m_HtmlWin->LoadPage(url);
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
    }
    m_NavigPan -> SetSelection(0);
    return TRUE;
}



bool wxHtmlHelpFrame::DisplayIndex()
{
    if (! m_IndexBox)
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

    int foundcnt = 0;
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

    wxHtmlSearchStatus status(m_Data, keyword, book);

    wxProgressDialog progress(_("Searching..."), _("No matching page found yet"),
                              status.GetMaxIndex(), this,
                              wxPD_APP_MODAL | wxPD_CAN_ABORT | wxPD_AUTO_HIDE);

    while (status.IsActive()) {
        if (progress.Update(status.GetCurIndex()) == FALSE)
            break;
        if (status.Search()) {
            foundstr.Printf(_("Found %i matches"), ++foundcnt);
            progress.Update(status.GetCurIndex(), foundstr);
            m_SearchList -> Append(status.GetName(), status.GetContentsItem());
        }
        wxYield();
    }

    m_SearchButton -> Enable(TRUE);
    m_SearchText -> SetSelection(0, keyword.Length());
    m_SearchText -> SetFocus();
    if (foundcnt) {
        wxHtmlContentsItem *it = (wxHtmlContentsItem*) m_SearchList -> GetClientData(0);
        if (it) m_HtmlWin -> LoadPage(it -> m_Book -> GetBasePath() + it -> m_Page);
    }
    return (foundcnt > 0);
}

#define MAX_ROOTS 64

void wxHtmlHelpFrame::CreateContents(bool show_progress)
{
    if (! m_ContentsBox)
        return ;

    wxProgressDialog *progress;
    wxString proginfo;

    m_ContentsBox->Clear();

    int cnt = m_Data->GetContentsCnt();
    int div = (cnt / PROGRESS_STEP) + 1;
    int i;

    wxHtmlContentsItem *it = m_Data->GetContents();

    if (show_progress)
        progress = new wxProgressDialog(_("Building contents tree..."), wxEmptyString,
                                        cnt, this, wxPD_APP_MODAL | wxPD_CAN_ABORT |
                                        wxPD_AUTO_HIDE);

    wxTreeItemId roots[MAX_ROOTS];
    bool imaged[MAX_ROOTS];

    m_ContentsBox -> DeleteAllItems();
    roots[0] = m_ContentsBox -> AddRoot(_("(Help)"));
    imaged[0] = TRUE;

    for (i = 0; i < cnt; i++, it++) {
        if (show_progress && ((i % div) == 0)) {
            proginfo.Printf(_("Added %d/%d items"), i, cnt);
            if (! progress->Update(i, proginfo))
                break;
            wxYield();
        }
        roots[it -> m_Level + 1] = m_ContentsBox -> AppendItem(
                                       roots[it -> m_Level], it -> m_Name, IMG_Page, -1,
                                       new wxHtmlHelpTreeItemData(it));

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
    if (show_progress)
        delete progress;
    m_ContentsBox -> Expand(roots[0]);
}


void wxHtmlHelpFrame::CreateIndex(bool show_progress)
{
    if (! m_IndexBox)
        return ;

    wxProgressDialog *progress;
    wxString proginfo;

    m_IndexBox->Clear();

    int cnt = m_Data->GetIndexCnt();
    int div = (cnt / PROGRESS_STEP) + 1;

    wxHtmlContentsItem* index = m_Data->GetIndex();

    if (show_progress)
        progress = new wxProgressDialog(_("Building index list..."), wxEmptyString,
                                        cnt, this, wxPD_APP_MODAL | wxPD_CAN_ABORT |
                                        wxPD_AUTO_HIDE);
    for (int i = 0; i < cnt; i++) {
        if (show_progress && ((i % div) == 0)) {
            proginfo.Printf(_("Added %d/%d items"), i, cnt);
            if (! progress->Update(i, proginfo))
                break;
            wxYield();
        }
        m_IndexBox -> Append(index[i].m_Name, (char*)(index + i));
    }

    if (show_progress)
        delete progress;
}

void wxHtmlHelpFrame::CreateSearch()
{
    if (! (m_SearchList && m_SearchChoice))
        return ;
    m_SearchList -> Clear();
    m_SearchChoice -> Clear();
    m_SearchChoice -> Append(_("all books"));
    const wxHtmlBookRecArray& bookrec = m_Data->GetBookRecArray();
    int i, cnt = bookrec.GetCount();
    for (i = 0; i < cnt; i++)
        m_SearchChoice->Append(bookrec[i].GetTitle());
    m_SearchChoice->SetSelection(0);
}


void wxHtmlHelpFrame::RefreshLists(bool show_progress)
{
    CreateContents(show_progress);
    CreateIndex(show_progress);
    CreateSearch();
}

void wxHtmlHelpFrame::ReadCustomization(wxConfigBase *cfg, const wxString& path)
{
    wxString oldpath;
    wxString tmp;

    if (path != wxEmptyString) {
        oldpath = cfg -> GetPath();
        cfg -> SetPath(path);
    }

    m_Cfg.navig_on = cfg -> Read("hcNavigPanel", m_Cfg.navig_on) != 0;
    m_Cfg.sashpos = cfg -> Read("hcSashPos", m_Cfg.sashpos);
    m_Cfg.x = cfg -> Read("hcX", m_Cfg.x);
    m_Cfg.y = cfg -> Read("hcY", m_Cfg.y);
    m_Cfg.w = cfg -> Read("hcW", m_Cfg.w);
    m_Cfg.h = cfg -> Read("hcH", m_Cfg.h);

    if (m_HtmlWin)
        m_HtmlWin->ReadCustomization(cfg, path);

    if (path != wxEmptyString)
        cfg -> SetPath(oldpath);
}

void wxHtmlHelpFrame::WriteCustomization(wxConfigBase *cfg, const wxString& path)
{
    wxString oldpath;
    wxString tmp;

    if (path != wxEmptyString) {
        oldpath = cfg -> GetPath();
        cfg -> SetPath(path);
    }

    cfg -> Write("hcNavigPanel", m_Cfg.navig_on);
    cfg -> Write("hcSashPos", (long)m_Cfg.sashpos);
    cfg -> Write("hcX", (long)m_Cfg.x);
    cfg -> Write("hcY", (long)m_Cfg.y);
    cfg -> Write("hcW", (long)m_Cfg.w);
    cfg -> Write("hcH", (long)m_Cfg.h);

    if (m_HtmlWin)
        m_HtmlWin->WriteCustomization(cfg, path);

    if (path != wxEmptyString)
        cfg -> SetPath(oldpath);
}


/*
EVENT HANDLING :
*/


void wxHtmlHelpFrame::OnToolbar(wxCommandEvent& event)
{
    switch (event.GetId()) {
        case wxID_HTML_BACK :
            m_HtmlWin -> HistoryBack();
            break;
        case wxID_HTML_FORWARD :
            m_HtmlWin -> HistoryForward();
            break;
        case wxID_HTML_PANEL :
            if (! (m_Splitter && m_NavigPan))
                return ;
            if (m_Splitter -> IsSplit()) {
                m_Cfg.sashpos = m_Splitter -> GetSashPosition();
                m_Splitter -> Unsplit(m_NavigPan);
            } else {
                m_NavigPan -> Show(TRUE);
                m_HtmlWin -> Show(TRUE);
                m_Splitter -> SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);
            }
            break;
    }
}



void wxHtmlHelpFrame::OnContentsSel(wxTreeEvent& event)
{
    wxHtmlHelpTreeItemData *pg;

    pg = (wxHtmlHelpTreeItemData*) m_ContentsBox -> GetItemData(event.GetItem());
    if (pg) m_HtmlWin -> LoadPage(pg -> GetPage());
}



void wxHtmlHelpFrame::OnIndexSel(wxCommandEvent& WXUNUSED(event))
{
    wxHtmlContentsItem *it = (wxHtmlContentsItem*) m_IndexBox -> GetClientData(m_IndexBox -> GetSelection());
    if (it) m_HtmlWin -> LoadPage(it -> m_Book -> GetBasePath() + it -> m_Page);
}



void wxHtmlHelpFrame::OnSearchSel(wxCommandEvent& WXUNUSED(event))
{
    wxHtmlContentsItem *it = (wxHtmlContentsItem*) m_SearchList -> GetClientData(m_SearchList -> GetSelection());
    if (it) m_HtmlWin -> LoadPage(it -> m_Book -> GetBasePath() + it -> m_Page);
}

void wxHtmlHelpFrame::OnSearch(wxCommandEvent& WXUNUSED(event))
{
    wxString sr = m_SearchText -> GetLineText(0);

    if (sr != wxEmptyString) KeywordSearch(sr);
}

void wxHtmlHelpFrame::OnCloseWindow(wxCloseEvent& evt)
{
    if (m_Config)
        WriteCustomization(m_Config, m_ConfigRoot);
    evt.Skip();
}

BEGIN_EVENT_TABLE(wxHtmlHelpFrame, wxFrame)
    EVT_TOOL_RANGE(wxID_HTML_PANEL, wxID_HTML_FORWARD, wxHtmlHelpFrame::OnToolbar)
    EVT_TREE_SEL_CHANGED(wxID_HTML_TREECTRL, wxHtmlHelpFrame::OnContentsSel)
    EVT_LISTBOX(wxID_HTML_INDEXLIST, wxHtmlHelpFrame::OnIndexSel)
    EVT_LISTBOX(wxID_HTML_SEARCHLIST, wxHtmlHelpFrame::OnSearchSel)
    EVT_BUTTON(wxID_HTML_SEARCHBUTTON, wxHtmlHelpFrame::OnSearch)
    EVT_TEXT_ENTER(wxID_HTML_SEARCHTEXT, wxHtmlHelpFrame::OnSearch)
    EVT_CLOSE(wxHtmlHelpFrame::OnCloseWindow)
END_EVENT_TABLE()

#endif
