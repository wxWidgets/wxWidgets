/////////////////////////////////////////////////////////////////////////////
// Name:        treectrl.h
// Purpose:     wxTreeCtrl sample
// Author:      Julian Smart
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// If native control is available, allow to also use the generic one for
// testing.
#ifndef wxHAS_GENERIC_TREECTRL
    #include "wx/generic/treectlg.h"

    #define wxUSE_ALT_GENERIC_TREECTRL 1
#else
    #define wxUSE_ALT_GENERIC_TREECTRL 0
#endif

// Define a new application type
class MyApp : public wxApp
{
public:
    MyApp() { m_showImages = true; m_showStates = true; m_showButtons = false; }

    bool OnInit() override;

    void SetShowImages(bool show) { m_showImages = show; }
    bool ShowImages() const { return m_showImages; }

    void SetShowStates(bool show) { m_showStates = show; }
    bool ShowStates() const { return m_showStates; }

    void SetShowButtons(bool show) { m_showButtons = show; }
    bool ShowButtons() const { return m_showButtons; }

private:
    bool m_showImages, m_showStates, m_showButtons;
};

class MyTreeItemData : public wxTreeItemData
{
public:
    MyTreeItemData(const wxString& desc) : m_desc(desc) { }

    void ShowInfo(wxTreeCtrlBase *tree);
    wxString const& GetDesc() const { return m_desc; }

private:
    wxString m_desc;
};

enum
{
    TreeCtrlIcon_File,
    TreeCtrlIcon_FileSelected,
    TreeCtrlIcon_Folder,
    TreeCtrlIcon_FolderSelected,
    TreeCtrlIcon_FolderOpened
};

// This interface defines some helper functions that are used by the sample to
// perform operations on the tree control without knowing its exact type
// (native or generic).
//
// It's not a pure interface as it also contains some data members, but this is
// just done for convenience to allow implementing some trivial functions
// directly in it. Its meaning is still that it defines the interface used by
// MyFrame to manipulate the tree control.
class MyTreeCtrlInterface
{
public:
    virtual ~MyTreeCtrlInterface() = default;

    virtual void CreateImages(int size) = 0;
    virtual void CreateStateImages() = 0;

    // This one can be only used with wxGenericTreeCtrl currently, so it takes
    // it as argument.
    void CreateButtonsImageList(wxGenericTreeCtrl* treectrl, int size);

    virtual void AddTestItemsToTree(size_t numChildren, size_t depth) = 0;

    int ImageSize() const { return m_imageSize; }

    void SetAlternateImages(bool show) { m_alternateImages = show; }
    bool AlternateImages() const { return m_alternateImages; }

    void SetAlternateStates(bool show) { m_alternateStates = show; }
    bool AlternateStates() const { return m_alternateStates; }

    virtual void ResetBrokenStateImages() = 0;
    virtual void DoToggleIcon(const wxTreeItemId& item) = 0;
    virtual void DoToggleState(const wxTreeItemId& item) = 0;
    virtual wxTreeItemId GetLastTreeITem() const = 0;
    virtual void GetItemsRecursively(const wxTreeItemId& idParent,
                                     wxTreeItemIdValue cookie = nullptr) = 0;
    virtual void DoSortChildren(const wxTreeItemId& item, bool reverse) = 0;

protected:
    int          m_imageSize;               // current size of images
    bool         m_alternateImages = false;
    bool         m_alternateStates = false;
};

// This template can be instantiated for either wxTreeCtrl or wxGenericTreeCtrl.
//
// Please note that this class is a template just for idiosyncratic reasons in
// this sample, you wouldn't write your real code in this way and would either
// just use wxTreeCtrl or wxGenericTreeCtrl directly or use composition instead
// of inheritance. In other words, do not follow this example in your code.
template <class TreeCtrlBase>
class MyTreeCtrl : public TreeCtrlBase, public MyTreeCtrlInterface
{
public:
    MyTreeCtrl() = default;
    bool Create(wxWindow *parent, const wxWindowID id,
                const wxPoint& pos, const wxSize& size,
                long style);

    virtual ~MyTreeCtrl();

    void OnBeginDrag(wxTreeEvent& event);
    void OnBeginRDrag(wxTreeEvent& event);
    void OnEndDrag(wxTreeEvent& event);
    void OnBeginLabelEdit(wxTreeEvent& event);
    void OnEndLabelEdit(wxTreeEvent& event);
    void OnDeleteItem(wxTreeEvent& event);
    void OnContextMenu(wxContextMenuEvent& event);
    void OnItemMenu(wxTreeEvent& event);
    void OnGetInfo(wxTreeEvent& event);
    void OnSetInfo(wxTreeEvent& event);
    void OnItemExpanded(wxTreeEvent& event);
    void OnItemExpanding(wxTreeEvent& event);
    void OnItemCollapsed(wxTreeEvent& event);
    void OnItemCollapsing(wxTreeEvent& event);
    void OnSelChanged(wxTreeEvent& event);
    void OnSelChanging(wxTreeEvent& event);
    void OnTreeKeyDown(wxTreeEvent& event);
    void OnItemActivated(wxTreeEvent& event);
    void OnItemStateClick(wxTreeEvent& event);
    void OnItemRClick(wxTreeEvent& event);

    void OnRMouseDown(wxMouseEvent& event);
    void OnRMouseUp(wxMouseEvent& event);
    void OnRMouseDClick(wxMouseEvent& event);

    wxTreeItemId GetLastTreeITem() const override;
    void GetItemsRecursively(const wxTreeItemId& idParent,
                             wxTreeItemIdValue cookie = nullptr) override;

    // This function behaves differently depending on the value of size:
    //  - If it's -1, it turns off the use of images entirely.
    //  - If it's 0, it reuses the last used size.
    //  - If it's strictly positive, it creates icons in this size.
    void CreateImages(int size) override;

    void CreateStateImages() override;

    void AddTestItemsToTree(size_t numChildren, size_t depth) override;

    void DoSortChildren(const wxTreeItemId& item, bool reverse) override
        { m_reverseSort = reverse; TreeCtrlBase::SortChildren(item); }

    void DoToggleIcon(const wxTreeItemId& item) override;
    void DoToggleState(const wxTreeItemId& item) override;

    void ShowMenu(wxTreeItemId id, const wxPoint& pt);

    void ResetBrokenStateImages() override
    {
        const size_t count = this->GetStateImageList()->GetImageCount();
        int state = count > 0 ? count - 1 : wxTREE_ITEMSTATE_NONE;
        DoResetBrokenStateImages(this->GetRootItem(), nullptr, state);
    }

protected:
    virtual int OnCompareItems(const wxTreeItemId& i1, const wxTreeItemId& i2) override;

    // is this the test item which we use in several event handlers?
    bool IsTestItem(const wxTreeItemId& item)
    {
        // the test item is the first child folder
        return this->GetItemParent(item) == this->GetRootItem() && !this->GetPrevSibling(item);
    }

private:
    // Find the very last item in the tree.
    void AddItemsRecursively(const wxTreeItemId& idParent,
                             size_t nChildren,
                             size_t depth,
                             size_t folder);

    void DoResetBrokenStateImages(const wxTreeItemId& idParent,
                                  wxTreeItemIdValue cookie, int state);

    void LogEvent(const wxString& name, const wxTreeEvent& event);

    bool m_reverseSort = false;             // flag for OnCompareItems
    wxTreeItemId m_draggedItem;             // item being dragged right now

    // NB: due to an ugly wxMSW hack you _must_ use wxDECLARE_DYNAMIC_CLASS();
    //     if you want your overloaded OnCompareItems() to be called.
    //     OTOH, if you don't want it you may omit the next line - this will
    //     make default (alphabetical) sorting much faster under wxMSW.
    wxDECLARE_DYNAMIC_CLASS(MyTreeCtrl);
};

// Define a new frame type
class MyFrame: public wxFrame
{
public:
    // ctor and dtor
    MyFrame();
    virtual ~MyFrame();

    // menu callbacks
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnClearLog(wxCommandEvent& event);

    void OnTogButtons(wxCommandEvent& event)
        { TogStyle(event.GetId(), wxTR_HAS_BUTTONS); }
    void OnTogTwist(wxCommandEvent& event)
        { TogStyle(event.GetId(), wxTR_TWIST_BUTTONS); }
    void OnTogLines(wxCommandEvent& event)
        { TogStyle(event.GetId(), wxTR_NO_LINES); }
    void OnTogEdit(wxCommandEvent& event)
        { TogStyle(event.GetId(), wxTR_EDIT_LABELS); }
    void OnTogHideRoot(wxCommandEvent& event)
        { TogStyle(event.GetId(), wxTR_HIDE_ROOT); }
    void OnTogRootLines(wxCommandEvent& event)
        { TogStyle(event.GetId(), wxTR_LINES_AT_ROOT); }
    void OnTogBorder(wxCommandEvent& event)
        { TogStyle(event.GetId(), wxTR_ROW_LINES); }
    void OnTogFullHighlight(wxCommandEvent& event)
        { TogStyle(event.GetId(), wxTR_FULL_ROW_HIGHLIGHT); }

#if wxUSE_ALT_GENERIC_TREECTRL
    void OnUseGeneric(wxCommandEvent& event);
#endif // wxUSE_ALT_GENERIC_TREECTRL

    void OnResetStyle(wxCommandEvent& WXUNUSED(event))
        { CreateTreeWithDefStyle(); }

    void OnSetFgColour(wxCommandEvent& event);
    void OnSetBgColour(wxCommandEvent& event);

    void OnHighlight(wxCommandEvent& event);
    void OnDump(wxCommandEvent& event);
#ifndef NO_MULTIPLE_SELECTION
    void OnDumpSelected(wxCommandEvent& event);
    void OnSelect(wxCommandEvent& event);
    void OnUnselect(wxCommandEvent& event);
    void OnToggleSel(wxCommandEvent& event);
    void OnSelectChildren(wxCommandEvent& event);
#endif // NO_MULTIPLE_SELECTION
    void OnSelectRoot(wxCommandEvent& event);
    void OnSetFocusedRoot(wxCommandEvent& event);
    void OnClearFocused(wxCommandEvent& event);
    void OnDelete(wxCommandEvent& event);
    void OnDeleteChildren(wxCommandEvent& event);
    void OnDeleteAll(wxCommandEvent& event);

    void OnFreezeThaw(wxCommandEvent& event);
    void OnRecreate(wxCommandEvent& event);
    void OnToggleButtons(wxCommandEvent& event);
    void OnToggleImages(wxCommandEvent& event);
    void OnToggleStates(wxCommandEvent& event);
    void OnToggleBell(wxCommandEvent& event);
    void OnToggleAlternateImages(wxCommandEvent& event);
    void OnToggleAlternateStates(wxCommandEvent& event);
    void OnSetImageSize(wxCommandEvent& event);
    void OnCollapseAndReset(wxCommandEvent& event);

    void OnSetBold(wxCommandEvent& WXUNUSED(event)) { DoSetBold(true); }
    void OnClearBold(wxCommandEvent& WXUNUSED(event)) { DoSetBold(false); }

    void OnEnsureVisible(wxCommandEvent& event);
    void OnSetFocus(wxCommandEvent& event);

    void OnCount(wxCommandEvent& event);
    void OnCountRec(wxCommandEvent& event);

    void OnRename(wxCommandEvent& event);
    void OnSort(wxCommandEvent& WXUNUSED(event)) { DoSort(); }
    void OnSortRev(wxCommandEvent& WXUNUSED(event)) { DoSort(true); }

    void OnAddItem(wxCommandEvent& event);
    void OnAddManyItems(wxCommandEvent& event);
    void OnInsertItem(wxCommandEvent& event);

    void OnIncIndent(wxCommandEvent& event);
    void OnDecIndent(wxCommandEvent& event);

    void OnIncSpacing(wxCommandEvent& event);
    void OnDecSpacing(wxCommandEvent& event);

    void OnToggleIcon(wxCommandEvent& event);
    void OnToggleState(wxCommandEvent& event);

    void OnShowFirstVisible(wxCommandEvent& WXUNUSED(event))
        { DoShowFirstOrLast(&wxTreeCtrlBase::GetFirstVisibleItem, "first visible"); }
#ifdef wxHAS_LAST_VISIBLE // we don't have it currently but may add later
    void OnShowLastVisible(wxCommandEvent& WXUNUSED(event))
        { DoShowFirstOrLast(&wxTreeCtrlBase::GetLastVisibleItem, "last visible"); }
#endif // wxHAS_LAST_VISIBLE

    void OnShowNextVisible(wxCommandEvent& WXUNUSED(event))
        { DoShowRelativeItem(&wxTreeCtrlBase::GetNextVisible, "next visible"); }
    void OnShowPrevVisible(wxCommandEvent& WXUNUSED(event))
        { DoShowRelativeItem(&wxTreeCtrlBase::GetPrevVisible, "previous visible"); }

    void OnShowParent(wxCommandEvent& WXUNUSED(event))
        { DoShowRelativeItem(&wxTreeCtrlBase::GetItemParent, "parent"); }
    void OnShowPrevSibling(wxCommandEvent& WXUNUSED(event))
        { DoShowRelativeItem(&wxTreeCtrlBase::GetPrevSibling, "previous sibling"); }
    void OnShowNextSibling(wxCommandEvent& WXUNUSED(event))
        { DoShowRelativeItem(&wxTreeCtrlBase::GetNextSibling, "next sibling"); }

    void OnScrollTo(wxCommandEvent& event);
    void OnSelectLast(wxCommandEvent& event);

    void OnIdle(wxIdleEvent& event);
    void OnSize(wxSizeEvent& event);

private:
    void TogStyle(int id, long flag);

    void DoSort(bool reverse = false);

    void Resize();

    void CreateTreeWithDefStyle();
    void CreateTree(long style);

    // common parts of OnShowFirst/LastVisible() and OnShowNext/PrevVisible()
    typedef wxTreeItemId (wxTreeCtrlBase::*TreeFunc0_t)() const;
    void DoShowFirstOrLast(TreeFunc0_t pfn, const wxString& label);

    typedef wxTreeItemId (wxTreeCtrlBase::*TreeFunc1_t)(const wxTreeItemId&) const;
    void DoShowRelativeItem(TreeFunc1_t pfn, const wxString& label);


    wxPanel *m_panel = nullptr;
    wxTreeCtrlBase *m_treeCtrl = nullptr;
    MyTreeCtrlInterface *m_treeCtrlIface = nullptr;
#if wxUSE_LOG
    wxTextCtrl *m_textCtrl = nullptr;
#endif // wxUSE_LOG

    void DoSetBold(bool bold = true);

    wxDECLARE_EVENT_TABLE();
};

// menu and control ids
enum
{
    TreeTest_Quit = wxID_EXIT,
    TreeTest_About = wxID_ABOUT,
    TreeTest_ClearLog = wxID_CLEAR,
    TreeTest_TogButtons = wxID_HIGHEST,
    TreeTest_TogTwist,
    TreeTest_TogLines,
    TreeTest_TogEdit,
    TreeTest_TogHideRoot,
    TreeTest_TogRootLines,
    TreeTest_TogBorder,
    TreeTest_TogFullHighlight,
    TreeTest_SetFgColour,
    TreeTest_SetBgColour,
#if wxUSE_ALT_GENERIC_TREECTRL
    TreeTest_UseGeneric,
#endif // wxUSE_ALT_GENERIC_TREECTRL
    TreeTest_ResetStyle,
    TreeTest_Highlight,
    TreeTest_Dump,
    TreeTest_DumpSelected,
    TreeTest_Count,
    TreeTest_CountRec,
    TreeTest_Sort,
    TreeTest_SortRev,
    TreeTest_SetBold,
    TreeTest_ClearBold,
    TreeTest_Rename,
    TreeTest_Delete,
    TreeTest_DeleteChildren,
    TreeTest_DeleteAll,
    TreeTest_FreezeThaw,
    TreeTest_Recreate,
    TreeTest_ToggleImages,
    TreeTest_ToggleStates,
    TreeTest_ToggleBell,
    TreeTest_ToggleAlternateImages,
    TreeTest_ToggleAlternateStates,
    TreeTest_ToggleButtons,
    TreeTest_SetImageSize,
    TreeTest_ToggleSel,
    TreeTest_CollapseAndReset,
    TreeTest_EnsureVisible,
    TreeTest_SetFocus,
    TreeTest_AddItem,
    TreeTest_AddManyItems,
    TreeTest_InsertItem,
    TreeTest_IncIndent,
    TreeTest_DecIndent,
    TreeTest_IncSpacing,
    TreeTest_DecSpacing,
    TreeTest_ToggleIcon,
    TreeTest_ToggleState,
    TreeTest_Select,
    TreeTest_Unselect,
    TreeTest_SelectRoot,
    TreeTest_ClearFocused,
    TreeTest_SetFocusedRoot,
    TreeTest_SelectChildren,
    TreeTest_ShowFirstVisible,
    TreeTest_ShowLastVisible,
    TreeTest_ShowNextVisible,
    TreeTest_ShowPrevVisible,
    TreeTest_ShowParent,
    TreeTest_ShowPrevSibling,
    TreeTest_ShowNextSibling,
    TreeTest_ScrollTo,
    TreeTest_SelectLast,
    TreeTest_Ctrl = 1000
};
