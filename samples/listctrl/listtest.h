/////////////////////////////////////////////////////////////////////////////
// Name:        listctrl.h
// Purpose:     wxListCtrl sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// not all ports have support for EVT_CONTEXT_MENU yet, don't define
// USE_CONTEXT_MENU for those which don't
#if defined(__WXMOTIF__) || defined(__WXPM__) || defined(__WXX11__) || defined(__WXMGL__)
    #define USE_CONTEXT_MENU 0
#else
    #define USE_CONTEXT_MENU 1
#endif

// Define a new application type
class MyApp: public wxApp
{
public:
    MyApp() { }

    virtual bool OnInit();

private:
    DECLARE_NO_COPY_CLASS(MyApp)
};

class MyListCtrl: public wxListCtrl
{
public:
    MyListCtrl(wxWindow *parent,
               const wxWindowID id,
               const wxPoint& pos,
               const wxSize& size,
               long style)
        : wxListCtrl(parent, id, pos, size, style),
          m_attr(*wxBLUE, *wxLIGHT_GREY, wxNullFont)
        {
#ifdef __POCKETPC__
            EnableContextMenu();
#endif
        }

    // add one item to the listctrl in report mode
    void InsertItemInReportView(int i, int shift);

    void OnColClick(wxListEvent& event);
    void OnColRightClick(wxListEvent& event);
    void OnColBeginDrag(wxListEvent& event);
    void OnColDragging(wxListEvent& event);
    void OnColEndDrag(wxListEvent& event);
    void OnBeginDrag(wxListEvent& event);
    void OnBeginRDrag(wxListEvent& event);
    void OnBeginLabelEdit(wxListEvent& event);
    void OnEndLabelEdit(wxListEvent& event);
    void OnDeleteItem(wxListEvent& event);
    void OnDeleteAllItems(wxListEvent& event);
#if WXWIN_COMPATIBILITY_2_4
    void OnGetInfo(wxListEvent& event);
    void OnSetInfo(wxListEvent& event);
#endif
    void OnSelected(wxListEvent& event);
    void OnDeselected(wxListEvent& event);
    void OnListKeyDown(wxListEvent& event);
    void OnActivated(wxListEvent& event);
    void OnFocused(wxListEvent& event);
    void OnCacheHint(wxListEvent& event);

    void OnChar(wxKeyEvent& event);

#if USE_CONTEXT_MENU
    void OnContextMenu(wxContextMenuEvent& event);
#endif

    void OnRightClick(wxMouseEvent& event);

private:
    void ShowContextMenu(const wxPoint& pos);
    wxLog *m_logOld;
    void SetColumnImage(int col, int image);

    void LogEvent(const wxListEvent& event, const wxChar *eventName);
    void LogColEvent(const wxListEvent& event, const wxChar *eventName);

    virtual wxString OnGetItemText(long item, long column) const;
    virtual int OnGetItemColumnImage(long item, long column) const;
    virtual wxListItemAttr *OnGetItemAttr(long item) const;

    wxListItemAttr m_attr;

    DECLARE_NO_COPY_CLASS(MyListCtrl)
    DECLARE_EVENT_TABLE()
};

// Define a new frame type
class MyFrame: public wxFrame
{
public:
    MyFrame(const wxChar *title);
    virtual ~MyFrame();

    void DoSize();

protected:
    void OnSize(wxSizeEvent& event);

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnListView(wxCommandEvent& event);
    void OnReportView(wxCommandEvent& event);
    void OnIconView(wxCommandEvent& event);
    void OnIconTextView(wxCommandEvent& event);
    void OnSmallIconView(wxCommandEvent& event);
    void OnSmallIconTextView(wxCommandEvent& event);
    void OnVirtualView(wxCommandEvent& event);
    void OnSmallVirtualView(wxCommandEvent& event);

    void OnFocusLast(wxCommandEvent& event);
    void OnToggleFirstSel(wxCommandEvent& event);
    void OnDeselectAll(wxCommandEvent& event);
    void OnSelectAll(wxCommandEvent& event);
    void OnAdd(wxCommandEvent& event);
    void OnEdit(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnDeleteAll(wxCommandEvent& event);
    void OnSort(wxCommandEvent& event);
    void OnSetFgColour(wxCommandEvent& event);
    void OnSetBgColour(wxCommandEvent& event);
    void OnToggleMultiSel(wxCommandEvent& event);
    void OnShowColInfo(wxCommandEvent& event);
    void OnShowSelInfo(wxCommandEvent& event);
    void OnFreeze(wxCommandEvent& event);
    void OnThaw(wxCommandEvent& event);
    void OnToggleLines(wxCommandEvent& event);
    void OnToggleMacUseGeneric(wxCommandEvent& event);
    void OnLongOperation(wxCommandEvent & event);

    void OnUpdateShowColInfo(wxUpdateUIEvent& event);
    void OnUpdateToggleMultiSel(wxUpdateUIEvent& event);

    wxImageList *m_imageListNormal;
    wxImageList *m_imageListSmall;

    wxPanel *m_panel;
    MyListCtrl *m_listCtrl;
    wxTextCtrl *m_logWindow;

private:
    // recreate the list control with the new flags
    void RecreateList(long flags, bool withText = true);

    // fill the control with items depending on the view
    void InitWithListItems();
    void InitWithReportItems(int shift);
    void InitWithIconItems(bool withText, bool sameIcon = false);
    void InitWithVirtualItems();

    // return true if the control is not in virtual view, give an error message
    // and return false if it is
    bool CheckNonVirtual() const;


    wxLog *m_logOld;

    bool m_smallVirtual;

    DECLARE_NO_COPY_CLASS(MyFrame)
    DECLARE_EVENT_TABLE()
};


// IDs for the menu commands
enum
{
    LIST_ABOUT = wxID_ABOUT,
    LIST_QUIT = wxID_EXIT,

    LIST_LIST_VIEW = wxID_HIGHEST,
    LIST_ICON_VIEW,
    LIST_ICON_TEXT_VIEW,
    LIST_SMALL_ICON_VIEW,
    LIST_SMALL_ICON_TEXT_VIEW,
    LIST_REPORT_VIEW,
    LIST_VIRTUAL_VIEW,
    LIST_SMALL_VIRTUAL_VIEW,

    LIST_DESELECT_ALL,
    LIST_SELECT_ALL,
    LIST_DELETE_ALL,
    LIST_DELETE,
    LIST_ADD,
    LIST_EDIT,
    LIST_SORT,
    LIST_SET_FG_COL,
    LIST_SET_BG_COL,
    LIST_TOGGLE_MULTI_SEL,
    LIST_TOGGLE_FIRST,
    LIST_SHOW_COL_INFO,
    LIST_SHOW_SEL_INFO,
    LIST_FOCUS_LAST,
    LIST_FREEZE,
    LIST_THAW,
    LIST_TOGGLE_LINES,
    LIST_MAC_USE_GENERIC,
    LIST_LONG_OPERATION,

    LIST_CTRL                   = 1000
};
