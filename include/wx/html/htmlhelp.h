/////////////////////////////////////////////////////////////////////////////
// Name:        htmlhelp.h
// Purpose:     Help controller
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __HTMLHELP_H__
#define __HTMLHELP_H__

#ifdef __GNUG__
#pragma interface "htmlhelp.h"
#endif

#include "wx/defs.h"

#if wxUSE_HTML

#include "wx/window.h"
#include "wx/config.h"
#include "wx/splitter.h"
#include "wx/notebook.h"
#include "wx/listctrl.h"
#include "wx/html/htmlwin.h"



//--------------------------------------------------------------------------------
// helper classes & structs - please ignore 'em
//--------------------------------------------------------------------------------



class WXDLLEXPORT HtmlBookRecord : public wxObject
{
    public:
        wxString m_BasePath;
        wxString m_Title;
        wxString m_Start;

        HtmlBookRecord(const wxString& basepath, const wxString& title, const wxString& start) {m_BasePath = basepath; m_Title = title; m_Start = start;}
        wxString GetTitle() const {return m_Title;}
        wxString GetStart() const {return m_Start;}
        wxString GetBasePath() const {return m_BasePath;}
};


#undef WXDLLEXPORTLOCAL
#define WXDLLEXPORTLOCAL WXDLLEXPORT
     // ?? Don't know why - but Allen Van Sickel reported it to fix problems with DLL
WX_DECLARE_OBJARRAY(HtmlBookRecord, HtmlBookRecArray);

#undef WXDLLEXPORTLOCAL
#define WXDLLEXPORTLOCAL


typedef struct
    {
        short int m_Level;
	int m_ID;
        char* m_Name;
        char* m_Page;
        HtmlBookRecord *m_Book;
    } HtmlContentsItem;




//--------------------------------------------------------------------------------
// wxHtmlHelpController
//                  This class ensures dislaying help.
//                  See documentation for details on its philosophy.
//
//                  WARNING!!
//                  This class is not derived from wxHelpController and is not
//                  compatible with it!
//--------------------------------------------------------------------------------


class WXDLLEXPORT wxHtmlHelpController : public wxEvtHandler
{
    DECLARE_DYNAMIC_CLASS(wxHtmlHelpController)

    protected:
        wxConfigBase *m_Config;
        wxString m_ConfigRoot;
                // configuration file/registry used to store custom settings
        wxString m_TitleFormat;
                // title of the help frame
        wxString m_TempPath;

        wxFrame *m_Frame;
        wxHtmlWindow *m_HtmlWin;
        wxSplitterWindow *m_Splitter;
        wxNotebook *m_NavigPan;
        wxTreeCtrl *m_ContentsBox;
        wxImageList *m_ContentsImageList;
        wxListBox *m_IndexBox;
        wxTextCtrl *m_SearchText;
        wxButton *m_SearchButton;
        wxListBox *m_SearchList;
                // ...pointers to parts of help window

        struct {
            long x, y, w, h;
            long sashpos;
            bool navig_on;
        } m_Cfg;
                // settings (window size, position, sash pos etc..)

        HtmlBookRecArray m_BookRecords;
                // each book has one record in this array
        HtmlContentsItem* m_Contents;
        int m_ContentsCnt;
                // list of all available books and pages.
        HtmlContentsItem* m_Index;
        int m_IndexCnt;
                // list of index items

    public:
        wxHtmlHelpController();
        ~wxHtmlHelpController();

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
        void Display(const int id);
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

        void UseConfig(wxConfigBase *config, const wxString& rootpath = wxEmptyString) {m_Config = config; m_ConfigRoot = rootpath;}
                // Assigns config object to the controller. This config is then
                // used in subsequent calls to Read/WriteCustomization of both help
                // controller and it's wxHtmlWindow

        void ReadCustomization(wxConfigBase *cfg, wxString path = wxEmptyString);
                // saves custom settings into cfg config. it will use the path 'path'
                // if given, otherwise it will save info into currently selected path.
                // saved values : things set by SetFonts, SetBorders.
        void WriteCustomization(wxConfigBase *cfg, wxString path = wxEmptyString);
                // ...

    protected:
        virtual void CreateHelpWindow();
                // Creates frame & html window and sets m_Frame and other variables;
                // Do nothing if the window already exists
        void RefreshLists();
                // Refreshes Contents and Index tabs
        void CreateContents();
                // Adds items to m_Contents tree control
        void CreateIndex();
                // Adds items to m_IndexList

        void LoadMSProject(HtmlBookRecord *book, wxFileSystem& fsys, const wxString& indexfile, const wxString& contentsfile, bool show_wait_msg);
                // Imports .hhp files (MS HTML Help Workshop)
        void LoadCachedBook(HtmlBookRecord *book, wxInputStream *f);
                // Reads binary book
        void SaveCachedBook(HtmlBookRecord *book, wxOutputStream *f);
                // Writes binary book

        void OnToolbar(wxCommandEvent& event);
        void OnContentsSel(wxTreeEvent& event);
        void OnIndexSel(wxCommandEvent& event);
        void OnSearchSel(wxCommandEvent& event);
        void OnSearch(wxCommandEvent& event);
        void OnCloseWindow(wxCloseEvent& event);

    DECLARE_EVENT_TABLE()
};



#endif // __HTMLHELP_H__

#endif










