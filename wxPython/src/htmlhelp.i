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
#include "export.h"
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
//      bool AddBookParam(const wxString& title, const wxString& contfile,
//  		      const wxString& indexfile=wxEmptyString,
//  		      const wxString& deftopic=wxEmptyString,
//  		      const wxString& path=wxEmptyString);

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
//---------------------------------------------------------------------------
