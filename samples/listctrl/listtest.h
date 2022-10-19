/////////////////////////////////////////////////////////////////////////////
// Name:        listctrl.h
// Purpose:     wxListCtrl sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// not all ports have support for EVT_CONTEXT_MENU yet, don't define
// USE_CONTEXT_MENU for those which don't
#if defined(__WXX11__)
    #define USE_CONTEXT_MENU 0
#else
    #define USE_CONTEXT_MENU 1
#endif

// Define a new application type
class MyApp: public wxApp
{
public:
    MyApp() { }

    virtual bool OnInit() override;

private:
    wxDECLARE_NO_COPY_CLASS(MyApp);
};

class MyListCtrl: public wxListCtrl
{
public:
    MyListCtrl(wxWindow *parent,
               const wxWindowID id,
               const wxPoint& pos,
               const wxSize& size,
               long style)
        : wxListCtrl(parent, id, pos, size, style)
        {
            m_updated = -1;

        }

    // add one item to the listctrl in report mode
    void InsertItemInReportView(int i);

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
    void OnSelected(wxListEvent& event);
    void OnDeselected(wxListEvent& event);
    void OnListKeyDown(wxListEvent& event);
    void OnActivated(wxListEvent& event);
    void OnFocused(wxListEvent& event);
    void OnItemRightClick(wxListEvent& event);
    void OnChecked(wxListEvent& event);
    void OnUnChecked(wxListEvent& event);
    void OnCacheHint(wxListEvent& event);

    void OnChar(wxKeyEvent& event);

#if USE_CONTEXT_MENU
    void OnContextMenu(wxContextMenuEvent& event);
#endif

    void OnRightClick(wxMouseEvent& event);

    virtual void CheckItem(long item, bool check) override;
    virtual bool IsItemChecked(long item) const override;

private:
    void ShowContextMenu(const wxPoint& pos, long item);
    void SetColumnImage(int col, int image);

    void LogEvent(const wxListEvent& event, const wxString& eventName);
    void LogColEvent(const wxListEvent& event, const wxString& eventName);

    virtual wxString OnGetItemText(long item, long column) const override;
    virtual bool OnGetItemIsChecked(long item) const override;
    virtual int OnGetItemColumnImage(long item, long column) const override;
    virtual wxItemAttr *OnGetItemAttr(long item) const override;

    long m_updated;

    // checked boxes in virtual list
    wxSelectionStore m_checked;

    wxDECLARE_NO_COPY_CLASS(MyListCtrl);
    wxDECLARE_EVENT_TABLE();
};

// Define a new frame type
class MyFrame: public wxFrame
{
public:
    MyFrame(const wxString& title);
    virtual ~MyFrame();

protected:
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
    void OnCheckVisibility(wxCommandEvent& event);
    void OnSetItemsCount(wxCommandEvent& event);


    void OnGoTo(wxCommandEvent& event);
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
    void OnSetRowLines(wxCommandEvent& event);
    void OnSetRowLinesOnBlank(wxCommandEvent& event);
    void OnCustomHeaderAttr(wxCommandEvent& event);
    void OnToggleMultiSel(wxCommandEvent& event);
    void OnShowColInfo(wxCommandEvent& event);
    void OnShowSelInfo(wxCommandEvent& event);
    void OnShowViewRect(wxCommandEvent& event);
#ifdef wxHAS_LISTCTRL_COLUMN_ORDER
    void OnSetColOrder(wxCommandEvent& event);
    void OnGetColOrder(wxCommandEvent& event);
#endif // wxHAS_LISTCTRL_COLUMN_ORDER
    void OnFreeze(wxCommandEvent& event);
    void OnThaw(wxCommandEvent& event);
    void OnToggleLines(wxCommandEvent& event);
    void OnToggleHeader(wxCommandEvent& event);
    void OnToggleBell(wxCommandEvent& event);
    void OnFind(wxCommandEvent& event);
    void OnToggleItemCheckBox(wxCommandEvent& event);
    void OnGetItemCheckBox(wxCommandEvent& event);
    void OnToggleCheckBoxes(wxCommandEvent& event);

    void OnUpdateUIEnableInReport(wxUpdateUIEvent& event);
    void OnUpdateToggleMultiSel(wxUpdateUIEvent& event);
    void OnUpdateToggleCheckBoxes(wxUpdateUIEvent& event);
    void OnUpdateToggleHeader(wxUpdateUIEvent& event);
    void OnUpdateRowLines(wxUpdateUIEvent& event);

    wxVector<wxBitmapBundle> m_imagesNormal;
    wxVector<wxBitmapBundle> m_imagesSmall;

    wxPanel *m_panel;
    MyListCtrl *m_listCtrl;
    wxTextCtrl *m_logWindow;

private:
    // recreate the list control with the new flags
    void RecreateList(long flags, bool withText = true);

    // fill the control with items depending on the view
    void InitWithListItems();
    void InitWithReportItems();
    void InitWithIconItems(bool withText, bool sameIcon = false);
    void InitWithVirtualItems();

    // return true if the control is not in virtual view, give an error message
    // and return false if it is
    bool CheckNonVirtual() const;


    wxLog *m_logOld;

    bool m_smallVirtual;

    // number of items to initialize list/report view with
    int m_numListItems;


    wxDECLARE_NO_COPY_CLASS(MyFrame);
    wxDECLARE_EVENT_TABLE();
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
    LIST_SET_ITEMS_COUNT,

    LIST_DESELECT_ALL,
    LIST_SELECT_ALL,
    LIST_DELETE_ALL,
    LIST_DELETE,
    LIST_ADD,
    LIST_EDIT,
    LIST_SORT,
    LIST_FIND,
    LIST_SET_FG_COL,
    LIST_SET_BG_COL,
    LIST_ROW_LINES,
    LIST_ROW_LINES_ON_BLANK,
    LIST_CUSTOM_HEADER_ATTR,
    LIST_TOGGLE_MULTI_SEL,
    LIST_TOGGLE_HEADER,
    LIST_TOGGLE_BELL,
    LIST_TOGGLE_CHECKBOX,
    LIST_GET_CHECKBOX,
    LIST_TOGGLE_CHECKBOXES,
    LIST_TOGGLE_FIRST,
    LIST_SHOW_COL_INFO,
    LIST_SHOW_SEL_INFO,
    LIST_SHOW_VIEW_RECT,
#ifdef wxHAS_LISTCTRL_COLUMN_ORDER
    LIST_SET_COL_ORDER,
    LIST_GET_COL_ORDER,
#endif // wxHAS_LISTCTRL_COLUMN_ORDER
    LIST_GOTO,
    LIST_FOCUS_LAST,
    LIST_FREEZE,
    LIST_THAW,
    LIST_TOGGLE_LINES,
    LIST_CHECKVISIBILITY,
    LIST_CTRL                   = 1000
};
