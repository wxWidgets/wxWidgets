/////////////////////////////////////////////////////////////////////////////
// Name:        notebook.h
// Purpose:     wxNotebook demo
// Author:      Julian Smart
// Modified by:
// Created:     25/10/98
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/notebook.h"

// Define a new application
class MyApp: public wxApp
{
public:
    bool OnInit();
    void InitTabView(wxNotebook* notebook, wxPanel* window);

    wxButton*   m_okButton;
    wxButton*   m_cancelButton;
    wxButton*   m_addPageButton, *m_insertPageButton;
    wxButton*   m_nextPageButton;
};

DECLARE_APP(MyApp)

#if USE_TABBED_DIALOG

class MyDialog: public wxDialog
{
public:
    MyDialog(wxWindow* parent, const wxWindowID id, const wxString& title,
        const wxPoint& pos, const wxSize& size, const long windowStyle = wxDEFAULT_DIALOG_STYLE);

    void OnOK(wxCommandEvent& event);
    void OnCloseWindow(wxCloseEvent& event);
    void Init();

protected:
    wxNotebook* m_notebook;

    DECLARE_EVENT_TABLE()
};

#else // USE_TABBED_DIALOG

class MyFrame: public wxFrame
{
public:
    MyFrame(wxFrame* parent, const wxWindowID id, const wxString& title,
        const wxPoint& pos, const wxSize& size, const long windowStyle = wxDEFAULT_FRAME_STYLE);

    void OnOK(wxCommandEvent& event);
    void OnCloseWindow(wxCloseEvent& event);
    void OnAddPage(wxCommandEvent& event);
    void OnInsertPage(wxCommandEvent& event);
    void OnNextPage(wxCommandEvent& event);
    void OnDeletePage(wxCommandEvent& event);
    void OnIdle(wxIdleEvent& event);

    void Init();

protected:
    wxNotebook*     m_notebook;
    wxPanel*        m_panel; // Panel containing notebook and OK/Cancel/Help

    DECLARE_EVENT_TABLE()
};

#endif // USE_TABBED_DIALOG

// File ids
#define TEST_ABOUT          2

// Tab ids
#define TEST_TAB_DOG        1
#define TEST_TAB_CAT        2
#define TEST_TAB_GOAT       3
#define TEST_TAB_GUINEAPIG  4
#define TEST_TAB_ANTEATER   5
#define TEST_TAB_HUMMINGBIRD 6
#define TEST_TAB_SHEEP      7
#define TEST_TAB_COW        8
#define TEST_TAB_HORSE      9
#define TEST_TAB_PIG        10
#define TEST_TAB_OSTRICH    11
#define TEST_TAB_AARDVARK   12

#define ID_NOTEBOOK         1000
#define ID_ADD_PAGE         1200
#define ID_DELETE_PAGE      1201
#define ID_NEXT_PAGE        1202
#define ID_INSERT_PAGE      1203

