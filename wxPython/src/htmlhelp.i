/////////////////////////////////////////////////////////////////////////////
// Name:        htmlhelp.i
// Purpose:     SWIG definitions of html classes
//
// Author:      Robin Dunn
//
// Created:     25-Nov-1998
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module htmlhelp

%{
#include "wxPython.h"
#include <wx/html/htmlwin.h>
#include <wx/html/helpctrl.h>
#include <wx/image.h>
#include <wx/fs_zip.h>
#include <wx/fs_inet.h>
#include <wx/wfstream.h>
%}

//---------------------------------------------------------------------------
%{
    // Put some wx default wxChar* values into wxStrings.
    static const wxString wxPyEmptyString(wxT(""));
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

class wxHtmlBookRecord {
public:
    wxHtmlBookRecord(const wxString& bookfile, const wxString& basepath,
                     const wxString& title, const wxString& start);

    wxString GetBookFile();
    wxString GetTitle();
    wxString GetStart();
    wxString GetBasePath();

    void SetContentsRange(int start, int end);
    int GetContentsStart();
    int GetContentsEnd();

    void SetTitle(const wxString& title);
    void SetBasePath(const wxString& path);
    void SetStart(const wxString& start);

    wxString GetFullPath(const wxString &page) const;
};

//---------------------------------------------------------------------------

struct wxHtmlContentsItem
{
    %addmethods {
        int GetLevel() { return self->m_Level; }
        int GetID() { return self->m_ID; }
        wxString GetName() { return self->m_Name; }
        wxString GetPage() { return self->m_Page; }
        wxHtmlBookRecord* GetBook() { return self->m_Book; }
    }
};

//---------------------------------------------------------------------------

class wxHtmlSearchStatus
{
public:
    //wxHtmlSearchStatus(wxHtmlHelpData* base, const wxString& keyword,
    //                   const wxString& book = wxPyEmptyString);
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
//  		      const wxString& indexfile=wxPyEmptyString,
//  		      const wxString& deftopic=wxPyEmptyString,
//  		      const wxString& path=wxPyEmptyString);

    wxString FindPageByName(const wxString& page);
    wxString FindPageById(int id);

    // TODO: this one needs fixed...
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
		    const wxString& title = wxPyEmptyString,
		    int style = wxHF_DEFAULTSTYLE, wxHtmlHelpData* data = NULL);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

    wxHtmlHelpData* GetData();
    void SetTitleFormat(const wxString& format);
    void Display(const wxString& x);
    %name(DisplayID) void Display(int id);
    void DisplayContents();
    void DisplayIndex();
    bool KeywordSearch(const wxString& keyword);
    void UseConfig(wxConfigBase *config, const wxString& rootpath = wxPyEmptyString);
    void ReadCustomization(wxConfigBase *cfg, wxString path = wxPyEmptyString);
    void WriteCustomization(wxConfigBase *cfg, wxString path = wxPyEmptyString);
};


//---------------------------------------------------------------------------


enum {
    wxHF_TOOLBAR,
    wxHF_FLATTOOLBAR,
    wxHF_CONTENTS,
    wxHF_INDEX,
    wxHF_SEARCH,
    wxHF_BOOKMARKS,
    wxHF_OPENFILES,
    wxHF_PRINT,
    wxHF_DEFAULTSTYLE,
};


class wxHtmlHelpController : public wxEvtHandler {
public:
    wxHtmlHelpController(int style = wxHF_DEFAULTSTYLE);
    ~wxHtmlHelpController();

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

    void SetTitleFormat(const wxString& format);
    void SetTempDir(const wxString& path);
    bool AddBook(const wxString& book, int show_wait_msg = FALSE);
    void Display(const wxString& x);
    %name(DisplayID) void Display(int id);
    void DisplayContents();
    void DisplayIndex();
    bool KeywordSearch(const wxString& keyword);
    void UseConfig(wxConfigBase *config, const wxString& rootpath = wxPyEmptyString);
    void ReadCustomization(wxConfigBase *cfg, wxString path = wxPyEmptyString);
    void WriteCustomization(wxConfigBase *cfg, wxString path = wxPyEmptyString);
    wxHtmlHelpFrame* GetFrame();
};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
