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
#include "wx/notebook.h"

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
    MyFrame(const wxString& title, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE|wxCLIP_CHILDREN|wxNO_FULL_REPAINT_ON_RESIZE);

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

#if wxUSE_NOTEBOOK
    void OnNotebook(wxNotebookEvent& event);
#endif
#if wxUSE_CHOICEBOOK
    void OnChoicebook(wxChoicebookEvent& event);
#endif
#if wxUSE_LISTBOOK
    void OnListbook(wxListbookEvent& event);
#endif

    void OnIdle(wxIdleEvent& event);

    wxBookCtrlBase *GetCurrentBook();

private:
    wxLog *m_logTargetOld;

    int SelectFlag(int id, int nb, int lb, int chb);
    void ShowCurrentBook();
    void RecreateBooks();

    // Sample setup
    int m_type;
    int m_orient;
    bool m_chkShowImages;
    bool m_multi;

    // Controls

    wxPanel *m_panel; // Panel containing notebook and other controls

#if wxUSE_NOTEBOOK
    wxNotebook   *m_notebook;
#endif
#if wxUSE_CHOICEBOOK
    wxChoicebook *m_choicebook;
#endif
#if wxUSE_LISTBOOK
    wxListbook   *m_listbook;
#endif

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
    ID_BOOK_NOTEBOOK = wxID_HIGHEST,
    ID_BOOK_LISTBOOK,
    ID_BOOK_CHOICEBOOK,
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
    ID_NOTEBOOK,
    ID_LISTBOOK,
    ID_CHOICEBOOK
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
