// Name:        htmlhelp.cpp
// Purpose:     Help controller
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation
#endif

#include <wx/wxprec.h>

#include "wx/defs.h"
#if wxUSE_HTML

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
#include <wx/wx.h>
#endif


#include <wx/notebook.h>
#include <wx/imaglist.h>
#include <wx/treectrl.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>
#include <wx/html/htmlwin.h>
#include <wx/html/htmlhelp.h>
#include <wx/busyinfo.h>

#if !((wxVERSION_NUMBER < 2100) || ((wxVERSION_NUMBER == 2100) && (wxBETA_NUMBER < 7)))
#include <wx/progdlg.h>
#endif


// Bitmaps:

#ifndef __WXMSW__
#include "bitmaps/panel.xpm"
#include "bitmaps/back.xpm"
#include "bitmaps/forward.xpm"
#include "bitmaps/book.xpm"
#include "bitmaps/folder.xpm"
#include "bitmaps/page.xpm"
#endif

#include "search.h"



//-----------------------------------------------------------------------------
// Helper constants
//-----------------------------------------------------------------------------


// Command IDs :

enum {
    wxID_HTML_PANEL = wxID_HIGHEST + 1,
    wxID_HTML_BACK,
    wxID_HTML_FORWARD,
    wxID_HTML_TREECTRL,
    wxID_HTML_INDEXPAGE,
    wxID_HTML_INDEXLIST,
    wxID_HTML_NOTEBOOK,
    wxID_HTML_SEARCHPAGE,
    wxID_HTML_SEARCHTEXT,
    wxID_HTML_SEARCHLIST,
    wxID_HTML_SEARCHBUTTON
};


// Images:

enum {
    IMG_Book = 0,
    IMG_Folder,
    IMG_Page
};






class HtmlHelpTreeItemData : public wxTreeItemData
{
    private:
        wxString m_Page;

    public:
        HtmlHelpTreeItemData(HtmlContentsItem *it) : wxTreeItemData() {m_Page = it -> m_Book -> GetBasePath() + it -> m_Page;}
        const wxString& GetPage() {return m_Page;}
};





#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(HtmlBookRecArray)









//-----------------------------------------------------------------------------
// wxHtmlHelpController
//-----------------------------------------------------------------------------


IMPLEMENT_DYNAMIC_CLASS(wxHtmlHelpController, wxEvtHandler)


wxHtmlHelpController::wxHtmlHelpController() : wxEvtHandler()
{
    m_Frame = NULL;
    m_Config = NULL;
    m_ConfigRoot = wxEmptyString;
    m_TitleFormat = _("Help : %s");
    m_TempPath = wxEmptyString;

    m_Cfg.x = m_Cfg.y = 0;
    m_Cfg.w = 700; m_Cfg.h = 480;
    m_Cfg.sashpos = 240;
    m_Cfg.navig_on = TRUE;

    m_ContentsImageList = new wxImageList(12, 12);
    m_ContentsImageList -> Add(wxICON(book));
    m_ContentsImageList -> Add(wxICON(folder));
    m_ContentsImageList -> Add(wxICON(page));

    m_Contents = NULL;
    m_ContentsCnt = 0;
    m_Index = NULL;
    m_IndexCnt = 0;
}



wxHtmlHelpController::~wxHtmlHelpController()
{
    int i;

    m_BookRecords.Empty();
    delete m_ContentsImageList;
    if (m_Contents) {
        for (i = 0; i < m_ContentsCnt; i++) {
            free(m_Contents[i].m_Page);
            free(m_Contents[i].m_Name);
        }
        free(m_Contents);
    }
    if (m_Index) {
        for (i = 0; i < m_IndexCnt; i++) {
            free(m_Index[i].m_Page);
            free(m_Index[i].m_Name);
        }
        free(m_Index);
    }
}



void wxHtmlHelpController::SetTempDir(const wxString& path)
{
    if (path == wxEmptyString) m_TempPath = path;
    else {
	if (wxIsAbsolutePath(path)) m_TempPath = path;
	else m_TempPath = wxGetCwd() + "/" + path;

	if (m_TempPath[m_TempPath.Length() - 1] != '/')
            m_TempPath << "/";
    }
}




// Reads one line, stores it into buf and returns pointer to new line or NULL.
static char* ReadLine(char *line, char *buf)
{
    char *writeptr = buf, *readptr = line;

    while (*readptr != 0 && *readptr != '\r' && *readptr != '\n') *(writeptr++) = *(readptr++);
    *writeptr = 0;
    while (*readptr == '\r' || *readptr == '\n') readptr++;
    if (*readptr == 0) return NULL;
    else return readptr;
}


static wxString SafeFileName(const wxString& s)
{
    wxString res = s;
    res.Replace(":", "_", TRUE);
    res.Replace(" ", "_", TRUE);
    res.Replace("/", "_", TRUE);
    res.Replace("\\", "_", TRUE);
    res.Replace("#", "_", TRUE);
    res.Replace(".", "_", TRUE);
    return res;
}


static int IndexCompareFunc(const void *a, const void *b)
{
    return strcmp(((HtmlContentsItem*)a) -> m_Name, ((HtmlContentsItem*)b) -> m_Name);
}



bool wxHtmlHelpController::AddBook(const wxString& book, bool show_wait_msg)
{
    wxFSFile *fi;
    wxFileSystem fsys;
    wxInputStream *s;
    HtmlBookRecord *bookr;
    wxString bookFull;

    int sz;
    char *buff, *lineptr;
    char linebuf[300];
     
    wxString title = _("noname"),
             safetitle,
             start = wxEmptyString,
             contents = wxEmptyString, index = wxEmptyString;
    
    if (wxIsAbsolutePath(book)) bookFull = book;
    else bookFull = wxGetCwd() + "/" + book;

    fi = fsys.OpenFile(bookFull);
    if (fi == NULL) return FALSE;
    fsys.ChangePathTo(bookFull);
    s = fi -> GetStream();
    sz = s -> StreamSize();
    buff = (char*) malloc(sz+1);
    buff[sz] = 0;
    s -> Read(buff, sz);
    lineptr = buff;
    delete fi;

    while ((lineptr = ReadLine(lineptr, linebuf)) != NULL) {
        if (strstr(linebuf, "Title=") == linebuf)
            title = linebuf + strlen("Title=");
        if (strstr(linebuf, "Default topic=") == linebuf)
            start = linebuf + strlen("Default topic=");
        if (strstr(linebuf, "Index file=") == linebuf)
            index = linebuf + strlen("Index file=");
        if (strstr(linebuf, "Contents file=") == linebuf)
            contents = linebuf + strlen("Contents file=");
    }
    free(buff);

    bookr = new HtmlBookRecord(fsys.GetPath(), title, start);

    if (m_ContentsCnt % HTML_REALLOC_STEP == 0)
        m_Contents = (HtmlContentsItem*) realloc(m_Contents, (m_ContentsCnt + HTML_REALLOC_STEP) * sizeof(HtmlContentsItem));
    m_Contents[m_ContentsCnt].m_Level = 0;
    m_Contents[m_ContentsCnt].m_ID = 0;
    m_Contents[m_ContentsCnt].m_Page = (char*) malloc(start.Length() + 1);
    strcpy(m_Contents[m_ContentsCnt].m_Page, start.c_str());
    m_Contents[m_ContentsCnt].m_Name = (char*) malloc(title.Length() + 1);
    strcpy(m_Contents[m_ContentsCnt].m_Name, title.c_str());
    m_Contents[m_ContentsCnt].m_Book = bookr;
    m_ContentsCnt++;

    // Try to find cached binary versions:
    safetitle = SafeFileName(title);
    fi = fsys.OpenFile(safetitle + ".cached");
    if (fi == NULL) fi = fsys.OpenFile(m_TempPath + safetitle + ".cached");
    if ((fi == NULL) || (m_TempPath == wxEmptyString)) {
        LoadMSProject(bookr, fsys, index, contents, show_wait_msg);
        if (m_TempPath != wxEmptyString) {
	        wxFileOutputStream *outs = new wxFileOutputStream(m_TempPath + safetitle + ".cached");
            SaveCachedBook(bookr, outs);
            delete outs;
	}
    }
    else {
        LoadCachedBook(bookr, fi -> GetStream());
        delete fi;
    }

    m_BookRecords.Add(bookr);
    if (m_IndexCnt > 0)
        qsort(m_Index, m_IndexCnt, sizeof(HtmlContentsItem), IndexCompareFunc);
    
    return TRUE;
}




void wxHtmlHelpController::Display(const wxString& x)
{
    int cnt;
    int i;
    wxFileSystem fsys;
    wxFSFile *f;

    CreateHelpWindow();

    /* 1. try to open given file: */

    cnt = m_BookRecords.GetCount();
    for (i = 0; i < cnt; i++) {
        f = fsys.OpenFile(m_BookRecords[i].GetBasePath() + x);
        if (f) {
            m_HtmlWin -> LoadPage(m_BookRecords[i].GetBasePath() + x);
            delete f;
            return;
        }
    }


    /* 2. try to find a book: */

    for (i = 0; i < cnt; i++) {
        if (m_BookRecords[i].GetTitle() == x) {
            m_HtmlWin -> LoadPage(m_BookRecords[i].GetBasePath() + m_BookRecords[i].GetStart());
            return;
        }
    }

    /* 3. try to find in contents: */

    cnt = m_ContentsCnt;
    for (i = 0; i < cnt; i++) {
        if (strcmp(m_Contents[i].m_Name, x) == 0) {
            m_HtmlWin -> LoadPage(m_Contents[i].m_Book -> GetBasePath() + m_Contents[i].m_Page);
            return;
        }
    }


    /* 4. try to find in index: */

    cnt = m_IndexCnt;
    for (i = 0; i < cnt; i++) {
        if (strcmp(m_Index[i].m_Name, x) == 0) {
            m_HtmlWin -> LoadPage(m_Index[i].m_Book -> GetBasePath() + m_Index[i].m_Page);
            return;
        }
    }


    /* 5. if everything failed, search the documents: */

    KeywordSearch(x);
}



void wxHtmlHelpController::Display(const int id)
{
    CreateHelpWindow();

    for (int i = 0; i < m_ContentsCnt; i++) {
        if (m_Contents[i].m_ID == id) {
            m_HtmlWin -> LoadPage(m_Contents[i].m_Book -> GetBasePath() + m_Contents[i].m_Page);
            return;
        }
    }
}



void wxHtmlHelpController::DisplayContents()
{
    CreateHelpWindow();
    m_Frame -> Raise();
    if (!m_Splitter -> IsSplit()) {
       m_NavigPan -> Show(TRUE);
       m_HtmlWin -> Show(TRUE);
       m_Splitter -> SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);
    }
    m_NavigPan -> SetSelection(0);
}



void wxHtmlHelpController::DisplayIndex()
{
    CreateHelpWindow();
    m_Frame -> Raise();
    if (!m_Splitter -> IsSplit()) {
       m_NavigPan -> Show(TRUE);
       m_HtmlWin -> Show(TRUE);
       m_Splitter -> SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);
    }
    m_NavigPan -> SetSelection(1);
}




#if (wxVERSION_NUMBER < 2100) || ((wxVERSION_NUMBER == 2100) && (wxBETA_NUMBER < 7))

class MyProgressDlg : public wxDialog
{
    public:
        bool m_Canceled;

        MyProgressDlg(wxWindow *parent) : wxDialog(parent, -1, 
	              _("Searching..."), 
	              wxPoint(0, 0), 
#ifdef __WXGTK__
		      wxSize(300, 110)) 
#else
		      wxSize(300, 130)) 
#endif
		      {m_Canceled = FALSE;}
        void OnCancel(wxCommandEvent& event) {m_Canceled = TRUE;}
        DECLARE_EVENT_TABLE()
};
BEGIN_EVENT_TABLE(MyProgressDlg, wxDialog)
    EVT_BUTTON(wxID_CANCEL, MyProgressDlg::OnCancel)
END_EVENT_TABLE()

#endif

	
bool wxHtmlHelpController::KeywordSearch(const wxString& keyword)
{
    int foundcnt = 0;

    CreateHelpWindow();
    m_Frame -> Raise();
    if (!m_Splitter -> IsSplit()) {
       m_NavigPan -> Show(TRUE);
       m_HtmlWin -> Show(TRUE);
       m_Splitter -> SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);
    }
    m_NavigPan -> SetSelection(2);
    m_SearchList -> Clear();
    m_SearchText -> SetValue(keyword);
    m_SearchButton -> Enable(FALSE);

    {
        int cnt = m_ContentsCnt;
        wxSearchEngine engine;
        wxFileSystem fsys;
        wxFSFile *file;
        wxString lastpage = wxEmptyString;
        wxString foundstr;

#if (wxVERSION_NUMBER < 2100) || ((wxVERSION_NUMBER == 2100) && (wxBETA_NUMBER < 7))
        MyProgressDlg progress(m_Frame);

        wxStaticText *prompt = new wxStaticText(&progress, -1, "", wxPoint(20, 50), wxSize(260, 25), wxALIGN_CENTER);
        wxGauge *gauge = new wxGauge(&progress, -1, cnt, wxPoint(20, 20), wxSize(260, 25));
        wxButton *btn = new wxButton(&progress, wxID_CANCEL, _("Cancel"), wxPoint(110, 70), wxSize(80, 25));
        btn = btn; /* fool compiler :-) */
        prompt -> SetLabel(_("No matching page found yet"));

	progress.Centre(wxBOTH);
        progress.Show(TRUE);
#else
	wxProgressDialog progress(_("Searching..."), _("No matching page found yet"), cnt, m_Frame, wxPD_APP_MODAL | wxPD_CAN_ABORT | wxPD_AUTO_HIDE);
#endif

        engine.LookFor(keyword);

        for (int i = 0; i < cnt; i++) {
#if (wxVERSION_NUMBER < 2100) || ((wxVERSION_NUMBER == 2100) && (wxBETA_NUMBER < 7))
            gauge -> SetValue(i);
            if (progress.m_Canceled) break;
#else
    	    if (progress.Update(i) == FALSE) break;
#endif
	    wxYield();

            file = fsys.OpenFile(m_Contents[i].m_Book -> GetBasePath() + m_Contents[i].m_Page);
            if (file) {
                if (lastpage != file -> GetLocation()) {
                    lastpage = file -> GetLocation();
                    if (engine.Scan(file -> GetStream())) {
                        foundstr.Printf(_("Found %i matches"), ++foundcnt);
#if (wxVERSION_NUMBER < 2100) || ((wxVERSION_NUMBER == 2100) && (wxBETA_NUMBER < 7))
                        prompt -> SetLabel(foundstr);
#else
    	    	    	progress.Update(i, foundstr);
#endif
                        wxYield();
                        m_SearchList -> Append(m_Contents[i].m_Name, (char*)(m_Contents + i));
                    }
                }
                delete file;
            }
        }

#if (wxVERSION_NUMBER < 2100) || ((wxVERSION_NUMBER == 2100) && (wxBETA_NUMBER < 7))
        progress.Close(TRUE);
#endif
    }

    m_SearchButton -> Enable(TRUE);
    m_SearchText -> SetSelection(0, keyword.Length());
    m_SearchText -> SetFocus();
    if (foundcnt) {
        HtmlContentsItem *it = (HtmlContentsItem*) m_SearchList -> GetClientData(0);
        if (it) m_HtmlWin -> LoadPage(it -> m_Book -> GetBasePath() + it -> m_Page);
    }
    return (foundcnt > 0);
}






void wxHtmlHelpController::CreateHelpWindow()
{
    wxBusyCursor cur;
    wxString oldpath;
    wxStatusBar *sbar;

    if (m_Frame) {
        m_Frame -> Raise();
	m_Frame -> Show(TRUE);
        return;
    }

#if wxUSE_BUSYINFO
    wxBusyInfo busyinfo(_("Preparing help window..."));
#endif

    if (m_Config) ReadCustomization(m_Config, m_ConfigRoot);

    m_Frame = new wxFrame(NULL, -1, "", wxPoint(m_Cfg.x, m_Cfg.y), wxSize(m_Cfg.w, m_Cfg.h));
    m_Frame -> PushEventHandler(this);
    sbar = m_Frame -> CreateStatusBar();

    {
        wxToolBar *toolBar;
        toolBar = m_Frame -> CreateToolBar(wxNO_BORDER | wxTB_HORIZONTAL | wxTB_FLAT | wxTB_DOCKABLE);
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

        toolBar -> AddTool(wxID_HTML_PANEL, *(toolBarBitmaps[0]), wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, _("Show/hide navigation panel"));
        currentX += width + 5;
        toolBar -> AddSeparator();
        toolBar -> AddTool(wxID_HTML_BACK, *(toolBarBitmaps[1]), wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, _("Go back to the previous HTML page"));
        currentX += width + 5;
        toolBar -> AddTool(wxID_HTML_FORWARD, *(toolBarBitmaps[2]), wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, _("Go forward to the next HTML page"));
        currentX += width + 5;

        toolBar -> Realize();

        // Can delete the bitmaps since they're reference counted
        for (int i = 0; i < 3; i++) delete toolBarBitmaps[i];
    }


    {
        m_Splitter = new wxSplitterWindow(m_Frame);

        m_HtmlWin = new wxHtmlWindow(m_Splitter);
        m_HtmlWin -> SetRelatedFrame(m_Frame, m_TitleFormat);
        m_HtmlWin -> SetRelatedStatusBar(0);
        if (m_Config) m_HtmlWin -> ReadCustomization(m_Config, m_ConfigRoot);

        m_NavigPan = new wxNotebook(m_Splitter, wxID_HTML_NOTEBOOK, wxDefaultPosition, wxDefaultSize);
        {
            m_ContentsBox = new wxTreeCtrl(m_NavigPan, wxID_HTML_TREECTRL, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxSUNKEN_BORDER);
            m_ContentsBox -> SetImageList(m_ContentsImageList);
            m_NavigPan -> AddPage(m_ContentsBox, _("Contents"));
        }

        {
            wxWindow *dummy = new wxPanel(m_NavigPan, wxID_HTML_INDEXPAGE);
            wxLayoutConstraints *b1 = new wxLayoutConstraints;
            b1 -> top.SameAs        (dummy, wxTop, 0);
            b1 -> left.SameAs       (dummy, wxLeft, 0);
            b1 -> width.PercentOf   (dummy, wxWidth, 100);
            b1 -> bottom.SameAs     (dummy, wxBottom, 0);
            m_IndexBox = new wxListBox(dummy, wxID_HTML_INDEXLIST, wxDefaultPosition, wxDefaultSize, 0);
            m_IndexBox -> SetConstraints(b1);
            dummy -> SetAutoLayout(TRUE);
            m_NavigPan -> AddPage(dummy, _("Index"));
        }

        {
            wxWindow *dummy = new wxPanel(m_NavigPan, wxID_HTML_SEARCHPAGE);

            wxLayoutConstraints *b1 = new wxLayoutConstraints;
            m_SearchText = new wxTextCtrl(dummy, wxID_HTML_SEARCHTEXT);
            b1 -> top.SameAs        (dummy, wxTop, 0);
            b1 -> left.SameAs       (dummy, wxLeft, 0);
            b1 -> right.SameAs      (dummy, wxRight, 0);
            b1 -> height.AsIs();
            m_SearchText -> SetConstraints(b1);

            wxLayoutConstraints *b2 = new wxLayoutConstraints;
            m_SearchButton = new wxButton(dummy, wxID_HTML_SEARCHBUTTON, _("Search!"));
            b2 -> top.Below         (m_SearchText, 10);
            b2 -> right.SameAs      (dummy, wxRight, 10);
            b2 -> width.AsIs();
            b2 -> height.AsIs();
            m_SearchButton -> SetConstraints(b2);

            wxLayoutConstraints *b3 = new wxLayoutConstraints;
            m_SearchList = new wxListBox(dummy, wxID_HTML_SEARCHLIST, wxDefaultPosition, wxDefaultSize, 0);
            b3 -> top.Below         (m_SearchButton, 10);
            b3 -> left.SameAs       (dummy, wxLeft, 0);
            b3 -> right.SameAs      (dummy, wxRight, 0);
            b3 -> bottom.SameAs     (dummy, wxBottom, 0);
            m_SearchList -> SetConstraints(b3);

            dummy -> SetAutoLayout(TRUE);
            dummy -> Layout();
            m_NavigPan -> AddPage(dummy, _("Search"));
        }

        RefreshLists();
        m_NavigPan -> Show(TRUE);
        m_HtmlWin -> Show(TRUE);
        m_Splitter -> SetMinimumPaneSize(20);
        m_Splitter -> SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);
        if (!m_Cfg.navig_on) m_Splitter -> Unsplit(m_NavigPan);
        wxYield();
    }

    m_Frame -> Show(TRUE);
    wxYield();
}



#define MAX_ROOTS 64

void wxHtmlHelpController::CreateContents()
{
    HtmlContentsItem *it;
    wxTreeItemId roots[MAX_ROOTS];
    bool imaged[MAX_ROOTS];
    int count = m_ContentsCnt;

    m_ContentsBox -> DeleteAllItems();
    roots[0] = m_ContentsBox -> AddRoot(_("(Help)"));
    imaged[0] = TRUE;

    for (int i = 0; i < count; i++) {
        it = m_Contents + i;
        roots[it -> m_Level + 1] = m_ContentsBox -> AppendItem(roots[it -> m_Level], it -> m_Name, IMG_Page, -1, new HtmlHelpTreeItemData(it));
        if (it -> m_Level == 0) {
            m_ContentsBox -> SetItemBold(roots[1], TRUE);
            m_ContentsBox -> SetItemImage(roots[1], IMG_Book);
            m_ContentsBox -> SetItemSelectedImage(roots[1], IMG_Book);
            imaged[1] = TRUE;
        }
        else imaged[it -> m_Level + 1] = FALSE;

        if (!imaged[it -> m_Level]) {
            m_ContentsBox -> SetItemImage(roots[it -> m_Level], IMG_Folder);
            m_ContentsBox -> SetItemSelectedImage(roots[it -> m_Level], IMG_Folder);
            imaged[it -> m_Level] = TRUE;
        }
    }

    m_ContentsBox -> Expand(roots[0]);
}




void wxHtmlHelpController::CreateIndex()
{
    m_IndexBox -> Clear();

    for (int i = 0; i < m_IndexCnt; i++)
        m_IndexBox -> Append(m_Index[i].m_Name, (char*)(m_Index + i));
}



void wxHtmlHelpController::RefreshLists()
{
    if (m_Frame) {
        CreateContents();
        CreateIndex();
        m_SearchList -> Clear();
    }
}







void wxHtmlHelpController::ReadCustomization(wxConfigBase *cfg, wxString path)
{
    wxString oldpath;
    wxString tmp;

    if (path != wxEmptyString) {
        oldpath = cfg -> GetPath();
        cfg -> SetPath(path);
    }

    m_Cfg.navig_on = (bool) cfg -> Read("hcNavigPanel", m_Cfg.navig_on);
    m_Cfg.sashpos = cfg -> Read("hcSashPos", m_Cfg.sashpos);
    m_Cfg.x = cfg -> Read("hcX", m_Cfg.x);
    m_Cfg.y = cfg -> Read("hcY", m_Cfg.y);
    m_Cfg.w = cfg -> Read("hcW", m_Cfg.w);
    m_Cfg.h = cfg -> Read("hcH", m_Cfg.h);

    if (path != wxEmptyString)
        cfg -> SetPath(oldpath);
}



void wxHtmlHelpController::WriteCustomization(wxConfigBase *cfg, wxString path)
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

    if (path != wxEmptyString)
        cfg -> SetPath(oldpath);
}





/*
EVENT HANDLING :
*/


void wxHtmlHelpController::OnToolbar(wxCommandEvent& event)
{
    switch (event.GetId()) {
        case wxID_HTML_BACK :
            m_HtmlWin -> HistoryBack();
            break;
        case wxID_HTML_FORWARD :
            m_HtmlWin -> HistoryForward();
            break;
        case wxID_HTML_PANEL :
            if (m_Splitter -> IsSplit()) {
                m_Cfg.sashpos = m_Splitter -> GetSashPosition();
                m_Splitter -> Unsplit(m_NavigPan);
            }
            else {
                m_NavigPan -> Show(TRUE);
                m_HtmlWin -> Show(TRUE);
                m_Splitter -> SplitVertically(m_NavigPan, m_HtmlWin, m_Cfg.sashpos);
            }
            break;
    }
}



void wxHtmlHelpController::OnContentsSel(wxTreeEvent& event)
{
    HtmlHelpTreeItemData *pg;

    pg = (HtmlHelpTreeItemData*) m_ContentsBox -> GetItemData(event.GetItem());
    if (pg) m_HtmlWin -> LoadPage(pg -> GetPage());
}



void wxHtmlHelpController::OnIndexSel(wxCommandEvent& event)
{
    HtmlContentsItem *it = (HtmlContentsItem*) m_IndexBox -> GetClientData(m_IndexBox -> GetSelection());
    if (it) m_HtmlWin -> LoadPage(it -> m_Book -> GetBasePath() + it -> m_Page);
}



void wxHtmlHelpController::OnSearchSel(wxCommandEvent& event)
{
    HtmlContentsItem *it = (HtmlContentsItem*) m_SearchList -> GetClientData(m_SearchList -> GetSelection());
    if (it) m_HtmlWin -> LoadPage(it -> m_Book -> GetBasePath() + it -> m_Page);
}



void wxHtmlHelpController::OnCloseWindow(wxCloseEvent& event)
{
    int a, b;

    m_Cfg.navig_on = m_Splitter -> IsSplit();
    if (m_Cfg.navig_on)
        m_Cfg.sashpos = m_Splitter -> GetSashPosition();
    m_Frame -> GetPosition(&a, &b);
    m_Cfg.x = a, m_Cfg.y = b;
    m_Frame -> GetSize(&a, &b);
    m_Cfg.w = a, m_Cfg.h = b;

    if (m_Config) {
        WriteCustomization(m_Config, m_ConfigRoot);
        m_HtmlWin -> WriteCustomization(m_Config, m_ConfigRoot);
    }
    m_Frame = NULL;

    event.Skip();
}



void wxHtmlHelpController::OnSearch(wxCommandEvent& event)
{
    wxString sr = m_SearchText -> GetLineText(0);

    if (sr != wxEmptyString) KeywordSearch(sr);
}



BEGIN_EVENT_TABLE(wxHtmlHelpController, wxEvtHandler)
    EVT_TOOL_RANGE(wxID_HTML_PANEL, wxID_HTML_FORWARD, wxHtmlHelpController::OnToolbar)
    EVT_TREE_SEL_CHANGED(wxID_HTML_TREECTRL, wxHtmlHelpController::OnContentsSel)
    EVT_LISTBOX(wxID_HTML_INDEXLIST, wxHtmlHelpController::OnIndexSel)
    EVT_LISTBOX(wxID_HTML_SEARCHLIST, wxHtmlHelpController::OnSearchSel)
    EVT_CLOSE(wxHtmlHelpController::OnCloseWindow)
    EVT_BUTTON(wxID_HTML_SEARCHBUTTON, wxHtmlHelpController::OnSearch)
    EVT_TEXT_ENTER(wxID_HTML_SEARCHTEXT, wxHtmlHelpController::OnSearch)
END_EVENT_TABLE()



#endif

