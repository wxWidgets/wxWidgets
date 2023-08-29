/////////////////////////////////////////////////////////////////////////////
// Name:        treectrl.h
// Purpose:     wxTreeCtrl sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// This can be defined to 1 to force using wxGenericTreeCtrl even on the
// platforms where the native controls would normally be used (wxMSW and wxQt).
#define USE_GENERIC_TREECTRL 0

#if USE_GENERIC_TREECTRL
#include "wx/generic/treectlg.h"
#ifndef wxTreeCtrl
#define wxTreeCtrl wxGenericTreeCtrl
#define sm_classwxTreeCtrl sm_classwxGenericTreeCtrl
#endif
#endif

// This one is defined if we're actually using the generic control, either
// because it was explicitly requested above or because there is no other one
// on this platform anyhow.
#if USE_GENERIC_TREECTRL || (!defined(__WXMSW__) && !defined(__WXQT__))
    #define HAS_GENERIC_TREECTRL
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

    void ShowInfo(wxTreeCtrl *tree);
    wxString const& GetDesc() const { return m_desc; }

private:
    wxString m_desc;
};

class MyTreeCtrl : public wxTreeCtrl
{
public:
    enum
    {
        TreeCtrlIcon_File,
        TreeCtrlIcon_FileSelected,
        TreeCtrlIcon_Folder,
        TreeCtrlIcon_FolderSelected,
        TreeCtrlIcon_FolderOpened
    };

    MyTreeCtrl() { m_alternateImages = false; m_alternateStates = false; }
    MyTreeCtrl(wxWindow *parent, const wxWindowID id,
               const wxPoint& pos, const wxSize& size,
               long style);
    virtual ~MyTreeCtrl(){}

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

    wxTreeItemId GetLastTreeITem() const;
    void GetItemsRecursively(const wxTreeItemId& idParent,
                             wxTreeItemIdValue cookie = 0);

    // This function behaves differently depending on the value of size:
    //  - If it's -1, it turns off the use of images entirely.
    //  - If it's 0, it reuses the last used size.
    //  - If it's strictly positive, it creates icons in this size.
    void CreateImages(int size);

    void CreateButtonsImageList(int size = 11);
    void CreateStateImageList(bool del = false);

    void AddTestItemsToTree(size_t numChildren, size_t depth);

    void DoSortChildren(const wxTreeItemId& item, bool reverse = false)
        { m_reverseSort = reverse; wxTreeCtrl::SortChildren(item); }

    void DoToggleIcon(const wxTreeItemId& item);
    void DoToggleState(const wxTreeItemId& item);

    void ShowMenu(wxTreeItemId id, const wxPoint& pt);

    int ImageSize(void) const { return m_imageSize; }

    void SetAlternateImages(bool show) { m_alternateImages = show; }
    bool AlternateImages() const { return m_alternateImages; }

    void SetAlternateStates(bool show) { m_alternateStates = show; }
    bool AlternateStates() const { return m_alternateStates; }

    void ResetBrokenStateImages()
    {
        const size_t count = GetStateImageList()->GetImageCount();
        int state = count > 0 ? count - 1 : wxTREE_ITEMSTATE_NONE;
        DoResetBrokenStateImages(GetRootItem(), 0, state);
    }

protected:
    virtual int OnCompareItems(const wxTreeItemId& i1, const wxTreeItemId& i2) override;

    // is this the test item which we use in several event handlers?
    bool IsTestItem(const wxTreeItemId& item)
    {
        // the test item is the first child folder
        return GetItemParent(item) == GetRootItem() && !GetPrevSibling(item);
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

    int          m_imageSize;               // current size of images
    bool         m_reverseSort;             // flag for OnCompareItems
    wxTreeItemId m_draggedItem;             // item being dragged right now
    bool         m_alternateImages;
    bool         m_alternateStates;

    // NB: due to an ugly wxMSW hack you _must_ use wxDECLARE_DYNAMIC_CLASS();
    //     if you want your overloaded OnCompareItems() to be called.
    //     OTOH, if you don't want it you may omit the next line - this will
    //     make default (alphabetical) sorting much faster under wxMSW.
    wxDECLARE_DYNAMIC_CLASS(MyTreeCtrl);
    wxDECLARE_EVENT_TABLE();
};

// Define a new frame type
class MyFrame: public wxFrame
{
public:
    // ctor and dtor
    MyFrame(const wxString& title, int x, int y, int w, int h);
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
        { DoShowFirstOrLast(&wxTreeCtrl::GetFirstVisibleItem, "first visible"); }
#ifdef wxHAS_LAST_VISIBLE // we don't have it currently but may add later
    void OnShowLastVisible(wxCommandEvent& WXUNUSED(event))
        { DoShowFirstOrLast(&wxTreeCtrl::GetLastVisibleItem, "last visible"); }
#endif // wxHAS_LAST_VISIBLE

    void OnShowNextVisible(wxCommandEvent& WXUNUSED(event))
        { DoShowRelativeItem(&wxTreeCtrl::GetNextVisible, "next visible"); }
    void OnShowPrevVisible(wxCommandEvent& WXUNUSED(event))
        { DoShowRelativeItem(&wxTreeCtrl::GetPrevVisible, "previous visible"); }

    void OnShowParent(wxCommandEvent& WXUNUSED(event))
        { DoShowRelativeItem(&wxTreeCtrl::GetItemParent, "parent"); }
    void OnShowPrevSibling(wxCommandEvent& WXUNUSED(event))
        { DoShowRelativeItem(&wxTreeCtrl::GetPrevSibling, "previous sibling"); }
    void OnShowNextSibling(wxCommandEvent& WXUNUSED(event))
        { DoShowRelativeItem(&wxTreeCtrl::GetNextSibling, "next sibling"); }

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
    typedef wxTreeItemId (wxTreeCtrl::*TreeFunc0_t)() const;
    void DoShowFirstOrLast(TreeFunc0_t pfn, const wxString& label);

    typedef wxTreeItemId (wxTreeCtrl::*TreeFunc1_t)(const wxTreeItemId&) const;
    void DoShowRelativeItem(TreeFunc1_t pfn, const wxString& label);


    wxPanel *m_panel;
    MyTreeCtrl *m_treeCtrl;
#if wxUSE_LOG
    wxTextCtrl *m_textCtrl;
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
