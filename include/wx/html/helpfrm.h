/////////////////////////////////////////////////////////////////////////////
// Name:        helpfrm.h
// Purpose:     wxHtmlHelpFrame
// Notes:       Based on htmlhelp.cpp, implementing a monolithic
//              HTML Help controller class,  by Vaclav Slavik
// Author:      Harm van der Heijden and Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) Harm van der Heijden and Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HELPFRM_H_
#define _WX_HELPFRM_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "helpfrm.h"
#endif

#include "wx/defs.h"

#if wxUSE_WXHTML_HELP

#include "wx/helpbase.h"
#include "wx/html/helpdata.h"
#include "wx/window.h"
#include "wx/frame.h"
#include "wx/config.h"
#include "wx/splitter.h"
#include "wx/notebook.h"
#include "wx/listbox.h"
#include "wx/choice.h"
#include "wx/combobox.h"
#include "wx/checkbox.h"
#include "wx/stattext.h"
#include "wx/html/htmlwin.h"
#include "wx/html/htmprint.h"

class WXDLLIMPEXP_CORE wxButton;
class WXDLLIMPEXP_CORE wxTextCtrl;
class WXDLLIMPEXP_CORE wxTreeEvent;
class WXDLLIMPEXP_CORE wxTreeCtrl;


// style flags for the Help Frame
#define wxHF_TOOLBAR                0x0001
#define wxHF_CONTENTS               0x0002
#define wxHF_INDEX                  0x0004
#define wxHF_SEARCH                 0x0008
#define wxHF_BOOKMARKS              0x0010
#define wxHF_OPEN_FILES             0x0020
#define wxHF_PRINT                  0x0040
#define wxHF_FLAT_TOOLBAR           0x0080
#define wxHF_MERGE_BOOKS            0x0100
#define wxHF_ICONS_BOOK             0x0200
#define wxHF_ICONS_BOOK_CHAPTER     0x0400
#define wxHF_ICONS_FOLDER           0x0000 // this is 0 since it is default
#define wxHF_DEFAULT_STYLE          (wxHF_TOOLBAR | wxHF_CONTENTS | \
                                     wxHF_INDEX | wxHF_SEARCH | \
                                     wxHF_BOOKMARKS | wxHF_PRINT)
//compatibility:
#define wxHF_OPENFILES               wxHF_OPEN_FILES
#define wxHF_FLATTOOLBAR             wxHF_FLAT_TOOLBAR
#define wxHF_DEFAULTSTYLE            wxHF_DEFAULT_STYLE


struct wxHtmlHelpFrameCfg
{
    int x, y, w, h;
    long sashpos;
    bool navig_on;
};

struct wxHtmlHelpMergedIndexItem;
class wxHtmlHelpMergedIndex;

class WXDLLIMPEXP_CORE wxHelpControllerBase;

class WXDLLIMPEXP_HTML wxHtmlHelpFrame : public wxFrame
{
    DECLARE_DYNAMIC_CLASS(wxHtmlHelpFrame)

public:
    wxHtmlHelpFrame(wxHtmlHelpData* data = NULL) { Init(data); }
    wxHtmlHelpFrame(wxWindow* parent, wxWindowID wxWindowID,
                    const wxString& title = wxEmptyString,
                    int style = wxHF_DEFAULT_STYLE, wxHtmlHelpData* data = NULL);
    bool Create(wxWindow* parent, wxWindowID id, const wxString& title = wxEmptyString,
                int style = wxHF_DEFAULT_STYLE);
    ~wxHtmlHelpFrame();

    wxHtmlHelpData* GetData() { return m_Data; }
    wxHelpControllerBase* GetController() const { return m_helpController; }
    void SetController(wxHelpControllerBase* controller) { m_helpController = controller; }

    // Sets format of title of the frame. Must contain exactly one "%s"
    // (for title of displayed HTML page)
    void SetTitleFormat(const wxString& format);

    // Displays page x. If not found it will offect the user a choice of
    // searching books.
    // Looking for the page runs in these steps:
    // 1. try to locate file named x (if x is for example "doc/howto.htm")
    // 2. try to open starting page of book x
    // 3. try to find x in contents (if x is for example "How To ...")
    // 4. try to find x in index (if x is for example "How To ...")
    bool Display(const wxString& x);

    // Alternative version that works with numeric ID.
    // (uses extension to MS format, <param name="ID" value=id>, see docs)
    bool Display(const int id);

    // Displays help window and focuses contents.
    bool DisplayContents();

    // Displays help window and focuses index.
    bool DisplayIndex();

    // Searches for keyword. Returns true and display page if found, return
    // false otherwise
    // Syntax of keyword is Altavista-like:
    // * words are separated by spaces
    //   (but "\"hello world\"" is only one world "hello world")
    // * word may be pretended by + or -
    //   (+ : page must contain the word ; - : page can't contain the word)
    // * if there is no + or - before the word, + is default
    bool KeywordSearch(const wxString& keyword,
                       wxHelpSearchMode mode = wxHELP_SEARCH_ALL);

    void UseConfig(wxConfigBase *config, const wxString& rootpath = wxEmptyString)
        {
            m_Config = config;
            m_ConfigRoot = rootpath;
            ReadCustomization(config, rootpath);
        }

    // Saves custom settings into cfg config. it will use the path 'path'
    // if given, otherwise it will save info into currently selected path.
    // saved values : things set by SetFonts, SetBorders.
    void ReadCustomization(wxConfigBase *cfg, const wxString& path = wxEmptyString);
    void WriteCustomization(wxConfigBase *cfg, const wxString& path = wxEmptyString);

    // call this to let wxHtmlHelpFrame know page changed
    void NotifyPageChanged();

    // Refreshes Contents and Index tabs
    void RefreshLists();

protected:
    void Init(wxHtmlHelpData* data = NULL);

    // Adds items to m_Contents tree control
    void CreateContents();

    // Adds items to m_IndexList
    void CreateIndex();

    // Add books to search choice panel
    void CreateSearch();

    // Updates "merged index" structure that combines indexes of all books
    // into better searchable structure
    void UpdateMergedIndex();

    // Add custom buttons to toolbar
    virtual void AddToolbarButtons(wxToolBar *toolBar, int style);

    // Displays options dialog (fonts etc.)
    virtual void OptionsDialog();

    void OnToolbar(wxCommandEvent& event);
    void OnContentsSel(wxTreeEvent& event);
    void OnIndexSel(wxCommandEvent& event);
    void OnIndexFind(wxCommandEvent& event);
    void OnIndexAll(wxCommandEvent& event);
    void OnSearchSel(wxCommandEvent& event);
    void OnSearch(wxCommandEvent& event);
    void OnBookmarksSel(wxCommandEvent& event);
    void OnCloseWindow(wxCloseEvent& event);
    void OnActivate(wxActivateEvent& event);

#ifdef __WXMAC__
    void OnClose(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
#endif

    // Images:
    enum {
        IMG_Book = 0,
        IMG_Folder,
        IMG_Page
    };

protected:
    wxHtmlHelpData* m_Data;
    bool m_DataCreated;  // m_Data created by frame, or supplied?
    wxString m_TitleFormat;  // title of the help frame
    // below are various pointers to GUI components
    wxHtmlWindow *m_HtmlWin;
    wxSplitterWindow *m_Splitter;
    wxPanel *m_NavigPan;
    wxNotebook *m_NavigNotebook;
    wxTreeCtrl *m_ContentsBox;
    wxTextCtrl *m_IndexText;
    wxButton *m_IndexButton;
    wxButton *m_IndexButtonAll;
    wxListBox *m_IndexList;
    wxTextCtrl *m_SearchText;
    wxButton *m_SearchButton;
    wxListBox *m_SearchList;
    wxChoice *m_SearchChoice;
    wxStaticText *m_IndexCountInfo;
    wxCheckBox *m_SearchCaseSensitive;
    wxCheckBox *m_SearchWholeWords;

    wxComboBox *m_Bookmarks;
    wxArrayString m_BookmarksNames, m_BookmarksPages;

    wxHtmlHelpFrameCfg m_Cfg;

    wxConfigBase *m_Config;
    wxString m_ConfigRoot;

    // pagenumbers of controls in notebook (usually 0,1,2)
    int m_ContentsPage;
    int m_IndexPage;
    int m_SearchPage;

    // lists of available fonts (used in options dialog)
    wxArrayString *m_NormalFonts, *m_FixedFonts;
    int m_FontSize; // 0,1,2 = small,medium,big
    wxString m_NormalFace, m_FixedFace;

    bool m_UpdateContents;

#if wxUSE_PRINTING_ARCHITECTURE
    wxHtmlEasyPrinting *m_Printer;
#endif
    wxHashTable *m_PagesHash;
    wxHelpControllerBase* m_helpController;

    int m_hfStyle;

private:
    void DisplayIndexItem(const wxHtmlHelpMergedIndexItem *it);
    wxHtmlHelpMergedIndex *m_mergedIndex;

    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxHtmlHelpFrame)
};

#endif // wxUSE_WXHTML_HELP

#endif
