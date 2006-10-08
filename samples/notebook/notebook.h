/////////////////////////////////////////////////////////////////////////////
// Name:        samples/notebook/notebook.h
// Purpose:     a sample demonstrating notebook usage
// Author:      Julian Smart
// Modified by: Dimitri Schoolwerth
// Created:     25/10/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998-2002 wxWidgets team
// License:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "wx/choicebk.h"
#include "wx/listbook.h"
#include "wx/treebook.h"
#include "wx/notebook.h"
#include "wx/toolbook.h"

#if wxUSE_LOG && !defined( __SMARTPHONE__ )
    #define USE_LOG 1
#else
    #define USE_LOG 0
#endif

// Define a new application
class MyApp : public wxApp
{
public:
    bool OnInit();
};

DECLARE_APP(MyApp)


class MyFrame : public wxFrame
{
public:
    MyFrame();
    virtual ~MyFrame();

    void OnType(wxCommandEvent& event);
    void OnOrient(wxCommandEvent& event);
    void OnShowImages(wxCommandEvent& event);
    void OnMulti(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);

    void OnAddPage(wxCommandEvent& event);
    void OnInsertPage(wxCommandEvent& event);
    void OnDeleteCurPage(wxCommandEvent& event);
    void OnDeleteLastPage(wxCommandEvent& event);
    void OnNextPage(wxCommandEvent& event);
    void OnGoHome(wxCommandEvent &event);

    void OnAddSubPage(wxCommandEvent& event);
    void OnAddPageBefore(wxCommandEvent& event);

#if wxUSE_HELP
    void OnContextHelp(wxCommandEvent& event);
#endif // wxUSE_HELP

    void OnHitTest(wxCommandEvent& event);

    void OnBookCtrl(wxBookCtrlBaseEvent& event);
#if wxUSE_NOTEBOOK
    void OnNotebook(wxNotebookEvent& event) { OnBookCtrl(event); }
#endif
#if wxUSE_CHOICEBOOK
    void OnChoicebook(wxChoicebookEvent& event) { OnBookCtrl(event); }
#endif
#if wxUSE_LISTBOOK
    void OnListbook(wxListbookEvent& event) { OnBookCtrl(event); }
#endif
#if wxUSE_TREEBOOK
    void OnTreebook(wxTreebookEvent& event) { OnBookCtrl(event); }
#endif
#if wxUSE_TOOLBOOK
    void OnToolbook(wxToolbookEvent& event) { OnBookCtrl(event); }
#endif

    void OnIdle(wxIdleEvent& event);

#if wxUSE_TREEBOOK
    void OnUpdateTreeMenu(wxUpdateUIEvent& event);
#endif // wxUSE_TREEBOOK

    wxBookCtrlBase *GetCurrentBook() const { return m_bookCtrl; }

private:
    wxLog *m_logTargetOld;

    void RecreateBook();
    wxPanel *CreateNewPage() const;
    int TranslateBookFlag(int nb, int lb, int chb, int tbk, int toolbk) const;
    void AddFlagStrIfFlagPresent(wxString & flagStr, long flags, long flag, const wxChar * flagName) const;

    // Sample setup
    enum BookType
    {
        Type_Notebook,
        Type_Listbook,
        Type_Choicebook,
        Type_Treebook,
        Type_Toolbook,
        Type_Max
    } m_type;
    int m_orient;
    bool m_chkShowImages;
    bool m_multi;

    // Controls

    wxPanel *m_panel; // Panel containing notebook and other controls
    wxBookCtrlBase *m_bookCtrl;

#if USE_LOG
    // Log window
    wxTextCtrl *m_text;
#endif // USE_LOG

    wxBoxSizer *m_sizerFrame;

    wxImageList *m_imageList;

    DECLARE_EVENT_TABLE()
};

enum ID_COMMANDS
{
    // these should be in the same order as Type_XXX elements above
    ID_BOOK_NOTEBOOK = wxID_HIGHEST,
    ID_BOOK_LISTBOOK,
    ID_BOOK_CHOICEBOOK,
    ID_BOOK_TREEBOOK,
    ID_BOOK_TOOLBOOK,
    ID_BOOK_MAX,

    ID_ORIENT_DEFAULT,
    ID_ORIENT_TOP,
    ID_ORIENT_BOTTOM,
    ID_ORIENT_LEFT,
    ID_ORIENT_RIGHT,
    ID_ORIENT_MAX,
    ID_SHOW_IMAGES,
    ID_MULTI,
    ID_ADD_PAGE,
    ID_INSERT_PAGE,
    ID_DELETE_CUR_PAGE,
    ID_DELETE_LAST_PAGE,
    ID_NEXT_PAGE,
    ID_ADD_PAGE_BEFORE,
    ID_ADD_SUB_PAGE,
    ID_GO_HOME,

#if wxUSE_HELP
    ID_CONTEXT_HELP,
#endif // wxUSE_HELP
    ID_HITTEST
};

/*
Name of each notebook page.
Used as a label for a page, and used when cloning the notebook
to decide what type of page it is.
*/

#define I_WAS_INSERTED_PAGE_NAME  wxT("Inserted")
#define RADIOBUTTONS_PAGE_NAME wxT("Radiobuttons")
#define VETO_PAGE_NAME wxT("Veto")
#define MAXIMIZED_BUTTON_PAGE_NAME wxT("Maximized button")

// Pages that can be added by the user
#define INSERTED_PAGE_NAME wxT("Inserted ")
#define ADDED_PAGE_NAME wxT("Added ")
#define ADDED_PAGE_NAME_BEFORE wxT(" Inserted before ")
#define ADDED_SUB_PAGE_NAME wxT(" Inserted sub-page ")


