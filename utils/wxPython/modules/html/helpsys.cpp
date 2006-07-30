/* htmlsys.h : wxHtmlHelpSystem is an extension of the wxHtmlHelpController.
 * mainly does two things:
 * - extend the interface somewhat so the programmer can dictate most of the
 * look and feel of the htmlhelp frame.
 * - make some protected functions public (adding _ to the function name) so
 * that SWIG can wrap them.
 *
 * Harm van der Heijden 32aug1999
 */
#include "helpsys.h"
#include <wx/wx.h>
#include <wx/wfstream.h>
#include <wx/busyinfo.h>

// Bitmaps:
#ifndef __WXMSW__
#include "bitmaps/panel.xpm"
#include "bitmaps/back.xpm"
#include "bitmaps/forward.xpm"
#include "bitmaps/book.xpm"
#include "bitmaps/folder.xpm"
#include "bitmaps/page.xpm"
#endif

IMPLEMENT_DYNAMIC_CLASS(wxHtmlHelpSystem, wxHtmlHelpController)

// The two static funtions below are also defined in htmlhelp.cpp
// maybe they should be protected class functions.
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

bool wxHtmlHelpSystem::AddBookParam(const wxString& title, const wxString& contfile,
				    const wxString& indexfile, const wxString& deftopic,
				    const wxString& path, bool show_wait_msg)
{
    wxFileSystem fsys;
    wxFSFile *fi;
    HtmlBookRecord *bookr;
    wxString safetitle;

    if (! path.IsEmpty())
	    // workaround for bug in ChangePathTo(name, TRUE)
	    fsys.ChangePathTo(path+"/gaga");

    bookr = new HtmlBookRecord(path+'/', title, deftopic);

    if (m_ContentsCnt % HTML_REALLOC_STEP == 0)
        m_Contents = (HtmlContentsItem*) realloc(m_Contents, (m_ContentsCnt + HTML_REALLOC_STEP) * sizeof(HtmlContentsItem));
    m_Contents[m_ContentsCnt].m_Level = 0;
    m_Contents[m_ContentsCnt].m_ID = 0;
    m_Contents[m_ContentsCnt].m_Page = new char[deftopic.Length() + 1];
    strcpy(m_Contents[m_ContentsCnt].m_Page, deftopic.c_str());
    m_Contents[m_ContentsCnt].m_Name = new char [title.Length() + 1];
    strcpy(m_Contents[m_ContentsCnt].m_Name, title.c_str());
    m_Contents[m_ContentsCnt].m_Book = bookr;
    m_ContentsCnt++;

    // Try to find cached binary versions:
    safetitle = SafeFileName(title);
    fi = fsys.OpenFile(safetitle + ".cached");
    if (fi == NULL) fi = fsys.OpenFile(m_TempPath + safetitle + ".cached");
    if ((fi == NULL) || (m_TempPath == wxEmptyString)) {
        LoadMSProject(bookr, fsys, indexfile, contfile, show_wait_msg);
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

wxToolBar* wxHtmlHelpSystem::CreateToolBar(wxFrame* frame)
{
        wxToolBar *toolBar;
        toolBar = frame -> CreateToolBar(wxNO_BORDER | wxTB_HORIZONTAL | wxTB_FLAT | wxTB_DOCKABLE);
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
	return toolBar;
}

wxTreeCtrl* wxHtmlHelpSystem::CreateContentsTree(wxWindow* parent)
{
	wxTreeCtrl* tree;
	tree = new wxTreeCtrl(parent, wxID_HTML_TREECTRL, wxDefaultPosition,
			      wxDefaultSize, wxTR_HAS_BUTTONS | wxSUNKEN_BORDER);
	tree -> SetImageList(m_ContentsImageList);
	return tree;
}

wxListBox* wxHtmlHelpSystem::CreateIndexList(wxWindow* parent)
{
	return new wxListBox(parent, wxID_HTML_INDEXLIST, wxDefaultPosition, wxDefaultSize, 0);
}

void wxHtmlHelpSystem::SetControls(wxFrame* frame, wxHtmlWindow* htmlwin,
				       wxTreeCtrl* contents, wxListBox* index,
				       wxListBox* searchlist)
{
	m_Frame = frame;
	m_HtmlWin = htmlwin;
	m_ContentsBox = contents;
	m_IndexBox = index;
	m_SearchList = searchlist;
	m_SearchText = NULL; // provide your own input box
	/* if you're setting your own controls, you and your event handlers are
	 * responsible for any splitter and notebook adjustments, not the
	 * htmlhelp framework */
	m_Splitter = NULL;
	m_NavigPan = NULL;
	RefreshLists();
}

void wxHtmlHelpSystem::CreateHelpWindow()
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

    CreateToolBar(m_Frame);

    {
        m_Splitter = new wxSplitterWindow(m_Frame);

        m_HtmlWin = new wxHtmlWindow(m_Splitter);
        m_HtmlWin -> SetRelatedFrame(m_Frame, m_TitleFormat);
        m_HtmlWin -> SetRelatedStatusBar(0);
        if (m_Config) m_HtmlWin -> ReadCustomization(m_Config, m_ConfigRoot);

        m_NavigPan = new wxNotebook(m_Splitter, wxID_HTML_NOTEBOOK, wxDefaultPosition, wxDefaultSize);
        if (m_ContentsCnt) {
            m_ContentsBox = CreateContentsTree(m_NavigPan);
            m_NavigPan -> AddPage(m_ContentsBox, _("Contents"));
        }

        if (m_IndexCnt) {
            wxWindow *dummy = new wxPanel(m_NavigPan, wxID_HTML_INDEXPAGE);
            wxLayoutConstraints *b1 = new wxLayoutConstraints;
            b1 -> top.SameAs        (dummy, wxTop, 0);
            b1 -> left.SameAs       (dummy, wxLeft, 0);
            b1 -> width.PercentOf   (dummy, wxWidth, 100);
            b1 -> bottom.SameAs     (dummy, wxBottom, 0);
	    m_IndexBox = CreateIndexList(dummy);
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

void wxHtmlHelpSystem::RefreshLists()
{
    if (m_Frame) {
        CreateContents();
        CreateIndex();
	if (m_SearchList)
		m_SearchList -> Clear();
    }
}

void wxHtmlHelpSystem::OnToolbar(wxCommandEvent& event)
{
    switch (event.GetId()) {
        case wxID_HTML_BACK :
            if (m_HtmlWin) m_HtmlWin -> HistoryBack();
            break;
        case wxID_HTML_FORWARD :
            if (m_HtmlWin) m_HtmlWin -> HistoryForward();
            break;
        case wxID_HTML_PANEL :
	    if (! (m_Splitter && m_NavigPan))
		    return;
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

void wxHtmlHelpSystem::OnCloseWindow(wxCloseEvent& event)
{
    int a, b;

    if (m_Splitter && m_NavigPan) {
	    m_Cfg.navig_on = m_Splitter -> IsSplit();
	    if (m_Cfg.navig_on)
		    m_Cfg.sashpos = m_Splitter -> GetSashPosition();
	    m_Frame -> GetPosition(&a, &b);
	    m_Cfg.x = a, m_Cfg.y = b;
	    m_Frame -> GetSize(&a, &b);
	    m_Cfg.w = a, m_Cfg.h = b;
    }
    if (m_Config) {
        WriteCustomization(m_Config, m_ConfigRoot);
        m_HtmlWin -> WriteCustomization(m_Config, m_ConfigRoot);
    }
    m_Frame = NULL;

    //event.Skip();
}

BEGIN_EVENT_TABLE(wxHtmlHelpSystem, wxHtmlHelpController)
    EVT_TOOL_RANGE(wxID_HTML_PANEL, wxID_HTML_FORWARD, wxHtmlHelpSystem::OnToolbar)
    EVT_CLOSE(wxHtmlHelpSystem::OnCloseWindow)
END_EVENT_TABLE()

