/////////////////////////////////////////////////////////////////////////////
// Name:        listctrl.h
// Purpose:     wxListCtrl sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Define a new application type
class MyApp: public wxApp
{
public:
    virtual bool OnInit();

    wxImageList *m_imageListNormal;
    wxImageList *m_imageListSmall;
};

class MyListCtrl: public wxListCtrl
{
public:
    MyListCtrl(wxWindow *parent, const wxWindowID id, const wxPoint& pos,
            const wxSize& size, long style):
        wxListCtrl(parent, id, pos, size, style)
        {
        }

    void OnColClick(wxListEvent& event);
    void OnBeginDrag(wxListEvent& event);
    void OnBeginRDrag(wxListEvent& event);
    void OnBeginLabelEdit(wxListEvent& event);
    void OnEndLabelEdit(wxListEvent& event);
    void OnDeleteItem(wxListEvent& event);
    void OnDeleteAllItems(wxListEvent& event);
    void OnGetInfo(wxListEvent& event);
    void OnSetInfo(wxListEvent& event);
    void OnSelected(wxListEvent& event);
    void OnDeselected(wxListEvent& event);
    void OnListKeyDown(wxListEvent& event);
    void OnActivated(wxListEvent& event);

    void OnChar(wxKeyEvent& event);

    DECLARE_EVENT_TABLE()
};

// Define a new frame type
class MyFrame: public wxFrame
{
public:
    MyListCtrl *m_listCtrl;
    wxTextCtrl *m_logWindow;

    MyFrame(wxFrame *frame, char *title, int x, int y, int w, int h);
    ~MyFrame();

public:
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnListView(wxCommandEvent& event);
    void OnReportView(wxCommandEvent& event);
    void OnIconView(wxCommandEvent& event);
    void OnIconTextView(wxCommandEvent& event);
    void OnSmallIconView(wxCommandEvent& event);
    void OnSmallIconTextView(wxCommandEvent& event);
    void OnDeselectAll(wxCommandEvent& event);
    void OnSelectAll(wxCommandEvent& event);
    void OnDeleteAll(wxCommandEvent& event);
    void OnSort(wxCommandEvent& event);
    void OnSetFgColour(wxCommandEvent& event);
    void OnSetBgColour(wxCommandEvent& event);
    void OnToggleMultiSel(wxCommandEvent& event);

    void BusyOn(wxCommandEvent& event);
    void BusyOff(wxCommandEvent& event);

    DECLARE_EVENT_TABLE()
};


// ID for the menu quit command
enum
{
    LIST_QUIT                   = 1,
    LIST_LIST_VIEW              = 2,
    LIST_ICON_VIEW              = 3,
    LIST_ICON_TEXT_VIEW         = 4,
    LIST_SMALL_ICON_VIEW        = 5,
    LIST_SMALL_ICON_TEXT_VIEW   = 6,
    LIST_REPORT_VIEW            = 7,
    LIST_DESELECT_ALL           = 8,
    LIST_SELECT_ALL             = 9,
    LIST_ABOUT                  = 102,
    BUSY_ON                     = 10,
    BUSY_OFF                    = 11,
    LIST_DELETE_ALL             = 12,
    LIST_SORT,
    LIST_SET_FG_COL,
    LIST_SET_BG_COL,
    LIST_TOGGLE_MULTI_SEL,

    LIST_CTRL                   = 1000
};

