/////////////////////////////////////////////////////////////////////////////
// Name:        samples/notebook/notebook.h
// Purpose:     a sample demonstrating notebook usage
// Author:      Julian Smart
// Modified by: Dimitri Schoolwerth
// Created:     25/10/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998-2002 wxWindows team
// License:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "wx/notebook.h"

// Define a new application
class MyApp : public wxApp
{
public:
    bool OnInit();
};

DECLARE_APP(MyApp)

//
class MyNotebook : public wxNotebook
{
public:
    MyNotebook(wxWindow *parent, wxWindowID id = -1,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0);

    void CreateInitialPages();

    int GetIconIndex() const;
};

//
class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE);

    virtual ~MyFrame();

    // Recreates the notebook with the same pages, but with possibly
    // a different orientation and optionally with images.
    void ReInitNotebook();

    void CreateImageList();

    void OnCheckOrRadioBox(wxCommandEvent& event);

    void OnButtonAddPage(wxCommandEvent& event);
    void OnButtonInsertPage(wxCommandEvent& event);
    void OnButtonDeletePage(wxCommandEvent& event);
    void OnButtonNextPage(wxCommandEvent& event);

    void OnButtonExit(wxCommandEvent& event);

    void OnNotebook(wxNotebookEvent& event);

    void OnIdle(wxIdleEvent& event);


private:
    wxLog *m_logTargetOld;


    // Controls

    wxPanel *m_panel; // Panel containing notebook and other controls

    wxRadioBox *m_radioOrient;
    wxCheckBox *m_chkShowImages;

    wxButton *m_btnAddPage;
    wxButton *m_btnInsertPage;
    wxButton *m_btnDeletePage;
    wxButton *m_btnNextPage;
    wxButton *m_btnExit;

    MyNotebook *m_notebook;

    // Log window
    wxTextCtrl *m_text;


    // Sizers

    // The frame's sizer. Consists of m_sizerTop and the log window
    // at the bottom.
    wxBoxSizer *m_sizerFrame;

    // Sizer that contains the notebook and controls on the left
    wxBoxSizer *m_sizerTop;

    // Sizer for m_notebook
    wxNotebookSizer *m_sizerNotebook;

    wxImageList *m_imageList;

    DECLARE_EVENT_TABLE()
};

enum ID_CONTROLS
{
    ID_RADIO_ORIENT = wxID_HIGHEST,
    ID_CHK_SHOWIMAGES,
    ID_BTN_ADD_PAGE,
    ID_BTN_INSERT_PAGE,
    ID_BTN_DELETE_PAGE,
    ID_BTN_NEXT_PAGE,
    ID_NOTEBOOK
};

// notebook orientations
enum ORIENT
{
    ORIENT_TOP,
    ORIENT_BOTTOM,
    ORIENT_LEFT,
    ORIENT_RIGHT,
    ORIENT_MAX
};
