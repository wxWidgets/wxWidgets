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
#include <wx/image.h>
#include <wx/fs_zip.h>
#include <wx/fs_inet.h>
#include <wx/wfstream.h>
#include "helpsys.h"
%}

//---------------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

%extern wx.i
%extern windows.i
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
    wxID_HTML_SEARCHBUTTON
};

class wxHtmlHelpController : public wxEvtHandler {
public:
    wxHtmlHelpController();
    ~wxHtmlHelpController();

    // Images:
    enum {
        IMG_Book = 0,
        IMG_Folder,
        IMG_Page
    };

    void SetTitleFormat(const wxString& format) {m_TitleFormat = format;}
                // Sets format of title of the frame. Must contain exactly one "%s"
                // (for title of displayed HTML page)

    void SetTempDir(const wxString& path);
                // Sets directory where temporary files are stored.
                // These temp files are index & contents file in binary (much faster to read)
                // form. These files are NOT deleted on program's exit.

    bool AddBook(const wxString& book, bool show_wait_msg = FALSE);
                // Adds new book. 'book' is location of .htb file (stands for "html book").
                // See documentation for details on its format.
                // Returns success.
                // If show_wait_msg == true then message window with "loading book..." is displayed

    void Display(const wxString& x);
                // Displays page x. If not found it will offect the user a choice of searching
                // books.
                // Looking for the page runs in these steps:
                // 1. try to locate file named x (if x is for example "doc/howto.htm")
                // 2. try to open starting page of book x
                // 3. try to find x in contents (if x is for example "How To ...")
                // 4. try to find x in index (if x is for example "How To ...")
                // 5. offer searching and if the user agree, run KeywordSearch
    %name(DisplayID) void Display(const int id);
	        // Alternative version that works with numeric ID.
	        // (uses extension to MS format, <param name="ID" value=id>, see docs)

    void DisplayContents();
                // Displays help window and focuses contents.

    void DisplayIndex();
                // Displays help window and focuses index.

    bool KeywordSearch(const wxString& keyword);
                // Searches for keyword. Returns TRUE and display page if found, return
                // FALSE otherwise
                // Syntax of keyword is Altavista-like:
                // * words are separated by spaces
                //   (but "\"hello world\"" is only one world "hello world")
                // * word may be pretended by + or -
                //   (+ : page must contain the word ; - : page can't contain the word)
                // * if there is no + or - before the word, + is default

    void UseConfig(wxConfigBase *config, const wxString& rootpath = wxEmptyString);
                // Assigns config object to the controller. This config is then
                // used in subsequent calls to Read/WriteCustomization of both help
                // controller and it's wxHtmlWindow

    void ReadCustomization(wxConfigBase *cfg, wxString path = wxEmptyString);
                // saves custom settings into cfg config. it will use the path 'path'
                // if given, otherwise it will save info into currently selected path.
                // saved values : things set by SetFonts, SetBorders.
    void WriteCustomization(wxConfigBase *cfg, wxString path = wxEmptyString);
                // ...
};

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

//---------------------------------------------------------------------------
