/////////////////////////////////////////////////////////////////////////////
// Name:        htmlhelp.i
// Purpose:     SWIG definitions of html classes
//
// Author:      Robin Dunn
//
// Created:     25-nov-1998
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module htmlhelp

%{
#include "helpers.h"
#include <wx/html/htmlwin.h>
#include <wx/html/helpctrl.h>
#include <wx/image.h>
#include <wx/fs_zip.h>
#include <wx/fs_inet.h>
#include <wx/wfstream.h>
%}

//---------------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

%extern wx.i
%extern windows.i
%extern frames.i
%extern _defs.i
%extern events.i
%extern controls.i
%extern controls2.i

%extern utils.i

%extern html.i

//---------------------------------------------------------------------------

enum {
    wxID_HTML_PANEL,
    wxID_HTML_BACK,
    wxID_HTML_FORWARD,
    wxID_HTML_TREECTRL,
    wxID_HTML_INDEXPAGE,
    wxID_HTML_INDEXLIST,
    wxID_HTML_NOTEBOOK,
    wxID_HTML_SEARCHPAGE,
    wxID_HTML_SEARCHTEXT,
    wxID_HTML_SEARCHLIST,
    wxID_HTML_SEARCHBUTTON,
    wxID_HTML_SEARCHCHOICE,
    wxID_HTML_HELPFRAME

};

//---------------------------------------------------------------------------

class  wxHtmlHelpFrameCfg
{
public:
    wxHtmlHelpFrameCfg();

    long x, y, w, h;
    long sashpos;
    bool navig_on;
};


//---------------------------------------------------------------------------

class wxHtmlBookRecord  {
public:
    wxHtmlBookRecord(const wxString& basepath, const wxString& title,
		     const wxString& start);

    wxString GetTitle();
    wxString GetStart();
    wxString GetBasePath();

    void SetContentsRange(int start, int end);
    int GetContentsStart();
    int GetContentsEnd();
};

//---------------------------------------------------------------------------

typedef struct
{
    short int m_Level;
    int m_ID;
    char* m_Name;
    char* m_Page;
    wxHtmlBookRecord *m_Book;
} wxHtmlContentsItem;

//---------------------------------------------------------------------------

class wxHtmlSearchStatus
{
public:
    //wxHtmlSearchStatus(wxHtmlHelpData* base, const wxString& keyword,
    //                   const wxString& book = wxEmptyString);
    bool Search();
    bool IsActive();
    int GetCurIndex();
    int GetMaxIndex();
    const wxString& GetName();
    wxHtmlContentsItem* GetContentsItem();
};

//---------------------------------------------------------------------------

class wxHtmlHelpData {
public:
    wxHtmlHelpData();
    ~wxHtmlHelpData();

    void SetTempDir(const wxString& path);
    bool AddBook(const wxString& book);
    bool AddBookParam(const wxString& title, const wxString& contfile,
		      const wxString& indexfile=wxEmptyString,
		      const wxString& deftopic=wxEmptyString,
		      const wxString& path=wxEmptyString);

    wxString FindPageByName(const wxString& page);
    wxString FindPageById(int id);

    // **** this one needs fixed...
    const wxHtmlBookRecArray& GetBookRecArray();

    wxHtmlContentsItem* GetContents();
    int GetContentsCnt();
    wxHtmlContentsItem* GetIndex();
    int GetIndexCnt();
};

//---------------------------------------------------------------------------

class wxHtmlHelpFrame : public wxFrame {
public:
    wxHtmlHelpFrame(wxWindow* parent, int wxWindowID,
		    const wxString& title = wxEmptyString,
		    int style = wxHF_DEFAULTSTYLE, wxHtmlHelpData* data = NULL);

    wxHtmlHelpData* GetData();
    void SetTitleFormat(const wxString& format);
    void Display(const wxString& x);
    %name(DisplayID) void Display(int id);
    void DisplayContents();
    void DisplayIndex();
    bool KeywordSearch(const wxString& keyword);
    void UseConfig(wxConfigBase *config, const wxString& rootpath = wxEmptyString);
    void ReadCustomization(wxConfigBase *cfg, wxString path = wxEmptyString);
    void WriteCustomization(wxConfigBase *cfg, wxString path = wxEmptyString);
};


//---------------------------------------------------------------------------

class wxHtmlHelpController : public wxEvtHandler {
public:
    wxHtmlHelpController();
    ~wxHtmlHelpController();

    void SetTitleFormat(const wxString& format);
    void SetTempDir(const wxString& path);
    bool AddBook(const wxString& book, int show_wait_msg = FALSE);
    void Display(const wxString& x);
    %name(DisplayID) void Display(int id);
    void DisplayContents();
    void DisplayIndex();
    bool KeywordSearch(const wxString& keyword);
    void UseConfig(wxConfigBase *config, const wxString& rootpath = wxEmptyString);
    void ReadCustomization(wxConfigBase *cfg, wxString path = wxEmptyString);
    void WriteCustomization(wxConfigBase *cfg, wxString path = wxEmptyString);
    wxHtmlHelpFrame* GetFrame();
};

//---------------------------------------------------------------------------















#ifdef DO_WE_NEED_TO_KEEP_THIS
class wxHtmlHelpSystem : public wxHtmlHelpController {
public:
    wxHtmlHelpSystem() {};
    ~wxHtmlHelpSystem() {};

    bool AddBookParam(const wxString& title, const wxString& contfile,
                      const wxString& indexfile=wxEmptyString, const wxString& deftopic=wxEmptyString,
                      const wxString& path=wxEmptyString, bool show_wait_msg=FALSE);
    // Alternative to AddBook(wxString& hhpfile)
    wxToolBar* CreateToolBar(wxFrame* frame);
    // creates a dockable toolbar for the frame, containing hide/show, back and forward buttons
    wxTreeCtrl* CreateContentsTree(wxWindow* parent);
    // creates a treecontrol with imagelist for books, folders etc and id wxID_HTML_TREECTRL
    wxListBox* CreateIndexList(wxWindow* parent);
    // creates a listbox with the right id
    virtual void CreateHelpWindow();
    // Slightly different version than in wxHtmlHelpController; uses helpers above
    // Do nothing if the window already exists
    void SetControls(wxFrame* frame, wxHtmlWindow* htmlwin,
                     wxTreeCtrl* contents=NULL, wxListBox* index=NULL,
                     wxListBox* searchlist=NULL);
    // alternative for CreateHelpWindow(), sets frame, htmlwindow, contents tree, index
    // listbox and searchlist listbox. If null, their functionality won't be used

    // Some extra accessor functions
    wxFrame* GetFrame() { return m_Frame; }
    wxHtmlWindow* GetHtmlWindow() { return m_HtmlWin; }
    wxTreeCtrl* GetContentsTree() { return m_ContentsBox; }
    wxListBox* GetIndexList() { return m_IndexBox; }
    wxListBox* GetSearchList() { return m_SearchList; }
    wxImageList* GetContentsImageList() { return m_ContentsImageList; }
    // public interface for wxHtmlHelpControllers handlers, so wxPython can call them
    void OnToolbar(wxCommandEvent& event);
    void OnContentsSel(wxTreeEvent& event) {wxHtmlHelpController::OnContentsSel(event);}
    void OnIndexSel(wxCommandEvent& event) {wxHtmlHelpController::OnIndexSel(event);}
    void OnSearchSel(wxCommandEvent& event) {wxHtmlHelpController::OnSearchSel(event);}
    void OnSearch(wxCommandEvent& event) {wxHtmlHelpController::OnSearch(event);}
    void OnCloseWindow(wxCloseEvent& event);

    // some more protected functions that should be accessible from wxPython
    void RefreshLists();
    void CreateContents() { if (!m_IndexBox) return; wxHtmlHelpController::CreateContents(); }
    // Adds items to m_Contents tree control
    void CreateIndex() { if (! m_ContentsBox) return; wxHtmlHelpController::CreateIndex(); }
    // Adds items to m_IndexList
};

// just for fun, an Altavista-like search engine (the gems that Vaclav has hidden in wxHtml...)
// but not for wxMSW because it's not DLL exported
//#ifndef __WXMSW__
#ifdef THIS_IS_NOT_DEFINED_OKAY
class wxSearchEngine
{
    public:
        wxSearchEngine() : wxObject() {m_Keyword = NULL;}
        ~wxSearchEngine() {if (m_Keyword) free(m_Keyword);}

        void LookFor(const wxString& keyword);
            // Sets the keyword we will be searching for

        bool Scan(wxInputStream *stream);
            // Scans the stream for the keyword.
            // Returns TRUE if the stream contains keyword, fALSE otherwise
	%addmethods {
		bool ScanFile(const wxString& filename) {
			if (filename.IsEmpty())
				return false;
			wxFileInputStream istr(filename);
			return self->Scan(&istr);
		}
	}
};
#endif
#endif
//---------------------------------------------------------------------------
