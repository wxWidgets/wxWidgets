//////////////////////////////////////////////////////////////////////////////
// Name:        wx/aui/auibook.h
// Purpose:     wxaui: wx advanced user interface - notebook
// Author:      Benjamin I. Williams
// Modified by: Jens Lody
// Created:     2006-06-28
// Copyright:   (C) Copyright 2006, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////



#ifndef _WX_AUINOTEBOOK_H_
#define _WX_AUINOTEBOOK_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/defs.h"

#if wxUSE_AUI

#include "wx/aui/tabart.h"
#include "wx/aui/framemanager.h"
#include "wx/compositebookctrl.h"

#include <vector>

class wxAuiBookSerializer;
class wxAuiBookDeserializer;

class wxAuiNotebook;
class wxAuiTabFrame;


enum wxAuiNotebookOption
{
    wxAUI_NB_TOP                 = 1 << 0,
    wxAUI_NB_LEFT                = 1 << 1,  // not implemented yet
    wxAUI_NB_RIGHT               = 1 << 2,  // not implemented yet
    wxAUI_NB_BOTTOM              = 1 << 3,
    wxAUI_NB_TAB_SPLIT           = 1 << 4,
    wxAUI_NB_TAB_MOVE            = 1 << 5,
    wxAUI_NB_TAB_EXTERNAL_MOVE   = 1 << 6,
    wxAUI_NB_TAB_FIXED_WIDTH     = 1 << 7,
    wxAUI_NB_SCROLL_BUTTONS      = 1 << 8,
    wxAUI_NB_WINDOWLIST_BUTTON   = 1 << 9,
    wxAUI_NB_CLOSE_BUTTON        = 1 << 10,
    wxAUI_NB_CLOSE_ON_ACTIVE_TAB = 1 << 11,
    wxAUI_NB_CLOSE_ON_ALL_TABS   = 1 << 12,
    wxAUI_NB_MIDDLE_CLICK_CLOSE  = 1 << 13,
    wxAUI_NB_MULTILINE           = 1 << 14,
    wxAUI_NB_PIN_ON_ACTIVE_TAB   = 1 << 15,
    wxAUI_NB_UNPIN_ON_ALL_PINNED = 1 << 16,

    wxAUI_NB_DEFAULT_STYLE = wxAUI_NB_TOP |
                             wxAUI_NB_TAB_SPLIT |
                             wxAUI_NB_TAB_MOVE |
                             wxAUI_NB_SCROLL_BUTTONS |
                             wxAUI_NB_CLOSE_ON_ACTIVE_TAB |
                             wxAUI_NB_MIDDLE_CLICK_CLOSE
};

wxALLOW_COMBINING_ENUMS(wxAuiNotebookOption, wxBorder)



// aui notebook event class

class WXDLLIMPEXP_AUI wxAuiNotebookEvent : public wxBookCtrlEvent
{
public:
    wxAuiNotebookEvent(wxEventType commandType = wxEVT_NULL,
                       int winId = 0)
          : wxBookCtrlEvent(commandType, winId)
    {
        m_dragSource = nullptr;
    }
    wxNODISCARD wxEvent *Clone() const override { return new wxAuiNotebookEvent(*this); }

    void SetDragSource(wxAuiNotebook* s) { m_dragSource = s; }
    wxAuiNotebook* GetDragSource() const { return m_dragSource; }

private:
    wxAuiNotebook* m_dragSource;

#ifndef SWIG
private:
    wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN_DEF_COPY(wxAuiNotebookEvent);
#endif
};


class WXDLLIMPEXP_AUI wxAuiTabContainerButton
{
public:
    wxAuiTabContainerButton() = default;

    wxAuiTabContainerButton(
            int id_,
            int location_,
            wxAuiPaneButtonState curState_ = wxAUI_BUTTON_STATE_NORMAL
        ) : id(id_), curState(curState_), location(location_)
    {
    }

    int id = 0;               // button's id
    int curState = 0;         // current state (normal, hover, pressed, etc.)
    int location = 0;         // buttons location (wxLEFT, wxRIGHT, or wxCENTER)
    wxBitmapBundle bitmap;    // button's hover bitmap
    wxBitmapBundle disBitmap; // button's disabled bitmap
    wxRect rect;          // button's hit rectangle
};


// Possible tab states.
enum class wxAuiTabKind
{
    Normal, // Can be closed and dragged by user.
    Pinned, // Can be closed but can't be dragged. Can be unpinned by user.
    Locked  // Can't be closed, dragged nor unlocked by user.
};


class WXDLLIMPEXP_AUI wxAuiNotebookPage
{
public:
    wxWindow* window = nullptr; // page's associated window
    wxString caption;     // caption displayed on the tab
    wxString tooltip;     // tooltip displayed when hovering over tab title
    wxBitmapBundle bitmap;// tab's bitmap
    wxAuiTabKind kind = wxAuiTabKind::Normal; // tab's kind

    wxRect rect;          // tab's hit rectangle (only used internally)
    bool active = false;  // true if the page is currently active

    // These fields are internal, don't use them.
    bool hover = false;   // true if mouse hovering over tab
    bool rowEnd = false;  // true if the tab is the last in the row

    // This vector contains per-page buttons, i.e. "close" and, optionally,
    // "pin" buttons. It can be empty if none are used.
    std::vector<wxAuiTabContainerButton> buttons;
};


// These legacy classes can't be just typedefs as they can be (and are)
// forward-declared in the existing code.
class wxAuiNotebookPageArray : public wxBaseArray<wxAuiNotebookPage>
{
public:
    using wxBaseArray<wxAuiNotebookPage>::wxBaseArray;
};

class wxAuiTabContainerButtonArray : public wxBaseArray<wxAuiTabContainerButton>
{
public:
    using wxBaseArray<wxAuiTabContainerButton>::wxBaseArray;
};


class WXDLLIMPEXP_AUI wxAuiTabContainer
{
public:

    wxAuiTabContainer();
    virtual ~wxAuiTabContainer();

    void SetArtProvider(wxAuiTabArt* art);
    wxAuiTabArt* GetArtProvider() const;

    void SetFlags(unsigned int flags);
    unsigned int GetFlags() const { return m_flags; }
    bool IsFlagSet(unsigned int flag) const { return (m_flags & flag) != 0; }

    bool AddPage(const wxAuiNotebookPage& info);
    bool InsertPage(const wxAuiNotebookPage& info, size_t idx);
    bool MovePage(wxWindow* page, size_t newIdx);
    bool MovePage(size_t oldIdx, size_t newIdx);
    bool RemovePage(wxWindow* page);
    void RemovePageAt(size_t idx);
    bool SetActivePage(const wxWindow* page);
    bool SetActivePage(size_t page);
    void SetNoneActive();
    int GetActivePage() const;

    // Struct containing the result of a tab hit test.
    struct HitTestResult
    {
        HitTestResult() = default;

        HitTestResult(wxWindow* window_, int pos_)
            : window(window_), pos(pos_)
        {
        }

        // Check if the result is valid.
        explicit operator bool() const { return window != nullptr; }

        // The window at the given position or null if none.
        wxWindow* window = nullptr;

        // The position of the tab in the tab control.
        int pos = wxNOT_FOUND;
    };

    // Flags allowing to customize the behaviour of TabHitTest().
    enum HitTestFlags
    {
        HitTest_Default = 0,
        HitTest_AllowAfterTab = 1
    };

    HitTestResult TabHitTest(const wxPoint& pt, int flags = HitTest_Default) const;

    wxAuiTabContainerButton* ButtonHitTest(const wxPoint& pt) const;
    wxWindow* GetWindowFromIdx(size_t idx) const;
    int GetIdxFromWindow(const wxWindow* page) const;
    size_t GetPageCount() const;
    wxAuiNotebookPage& GetPage(size_t idx);
    const wxAuiNotebookPage& GetPage(size_t idx) const;
    const wxAuiNotebookPageArray& GetPages() const;
    void SetNormalFont(const wxFont& normalFont);
    void SetSelectedFont(const wxFont& selectedFont);
    void SetMeasuringFont(const wxFont& measuringFont);
    void SetColour(const wxColour& colour);
    void SetActiveColour(const wxColour& colour);
    void DoShowHide();
    void SetRect(const wxRect& rect, wxWindow* wnd = nullptr);
    void SetRowHeight(int rowHeight);

    void RemoveButton(int id);
    void AddButton(int id,
                   int location,
                   const wxBitmapBundle& normalBitmap = wxBitmapBundle(),
                   const wxBitmapBundle& disabledBitmap = wxBitmapBundle());

    size_t GetTabOffset() const;
    void SetTabOffset(size_t offset);

    // Is the tab visible?
    bool IsTabVisible(int tabPage, int tabOffset, wxReadOnlyDC* dc, wxWindow* wnd);

    // Make the tab visible if it wasn't already
    void MakeTabVisible(int tabPage, wxWindow* win);

    // Internal, don't use.
    void RemoveAll();

    // Backwards compatible variants of internal functions, which shouldn't be
    // used anyhow.
    bool AddPage(wxWindow* page, wxAuiNotebookPage info)
    {
        info.window = page;
        return AddPage(info);
    }

    bool InsertPage(wxWindow* page, wxAuiNotebookPage info, size_t idx)
    {
        info.window = page;
        return InsertPage(info, idx);
    }

    bool ButtonHitTest(int x, int y, wxAuiTabContainerButton** hit) const
    {
        auto* const button = ButtonHitTest(wxPoint(x, y));
        if ( hit )
            *hit = button;

        return button != nullptr;
    }

    bool TabHitTest(int x, int y, wxWindow** hit) const
    {
        auto const res = TabHitTest(wxPoint(x, y));
        if ( hit )
            *hit = res.window;

        return res.window != nullptr;
    }

    // Internal functions only, don't use.

    // Layout tabs in wxAUI_NB_MULTILINE case using either the width of the
    // given rectangle or the current width and return the extra height needed
    // for the additional rows.
    //
    // This function has an important side effect of updating rowEnd for all
    // pages -- which defines the layout.
    int LayoutMultiLineTabs(const wxRect& rect, wxWindow* wnd);
    int LayoutMultiLineTabs(wxWindow* wnd)
    {
        return LayoutMultiLineTabs(m_rect, wnd);
    }

    // Get the index of the first tab of the given kind or of a different kind,
    // returning GetPageCount() if there is no such tabs.
    int GetFirstTabOfKind(wxAuiTabKind kind) const;
    int GetFirstTabNotOfKind(wxAuiTabKind kind) const;

protected:

    virtual void Render(wxDC* dc, wxWindow* wnd);

protected:

    wxAuiTabArt* m_art;

    // Contains pages in the display order.
    wxAuiNotebookPageArray m_pages;

    // This vector contains container-level buttons, e.g. left/right scroll
    // buttons, close button if it's not per-tab, window list button etc.
    std::vector<wxAuiTabContainerButton> m_buttons;

    wxRect m_rect;
    size_t m_tabOffset;
    unsigned int m_flags;

private:
    // Return the width that can be used for the tabs, i.e. without the space
    // reserved for the buttons.
    int GetAvailableForTabs(const wxRect& rect, wxReadOnlyDC& dc, wxWindow* wnd);

    // Render the buttons: part of Render(), returns the extent of the buttons
    // on the left and right side.
    void RenderButtons(wxDC& dc, wxWindow* wnd,
                       int& left_buttons_width, int& right_buttons_width);

    // Update the state of the page buttons depending on its state and flags.
    //
    // If forceActive is true, the page is always considered as active, see the
    // comment in LayoutMultiLineTabs() for the reason why this is needed.
    void UpdateButtonsState(wxAuiNotebookPage& page, bool forceActive = false);

    int m_tabRowHeight;
};



class WXDLLIMPEXP_AUI wxAuiTabCtrl : public wxControl,
                                     public wxAuiTabContainer
{
public:

    wxAuiTabCtrl(wxWindow* parent,
                 wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0);

    ~wxAuiTabCtrl();

    bool IsDragging() const { return m_isDragging; }

    void SetRect(const wxRect& rect) { wxAuiTabContainer::SetRect(rect, this); }

    // Internal helpers.
    void DoShowTab(int idx);
    void DoUpdateActive();

    // Internal function taking the total tab frame area and setting the size
    // of the window to its sub-rectangle corresponding to tabs orientation.
    //
    // Also updates rowEnd for all pages in m_pages when using multiple rows.
    void DoApplyRect(const wxRect& rect, int tabCtrlHeight);

    // Another internal helper: return the hint rectangle corresponding to this
    // tab control in screen coordinates.
    wxRect GetHintScreenRect() const;

protected:
    // choose the default border for this window
    virtual wxBorder GetDefaultBorder() const override { return wxBORDER_NONE; }

    void OnPaint(wxPaintEvent& evt);
    void OnEraseBackground(wxEraseEvent& evt);
    void OnSize(wxSizeEvent& evt);
    void OnLeftDown(wxMouseEvent& evt);
    void OnLeftDClick(wxMouseEvent& evt);
    void OnLeftUp(wxMouseEvent& evt);
    void OnMiddleDown(wxMouseEvent& evt);
    void OnMiddleUp(wxMouseEvent& evt);
    void OnRightDown(wxMouseEvent& evt);
    void OnRightUp(wxMouseEvent& evt);
    void OnMotion(wxMouseEvent& evt);
    void OnLeaveWindow(wxMouseEvent& evt);
    void OnButton(wxAuiNotebookEvent& evt);
    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);
    void OnChar(wxKeyEvent& event);
    void OnCaptureLost(wxMouseCaptureLostEvent& evt);
    void OnSysColourChanged(wxSysColourChangedEvent& event);
    void OnDpiChanged(wxDPIChangedEvent& event);

protected:

    wxPoint m_clickPt = wxDefaultPosition;
    wxWindow* m_clickTab = nullptr;
    bool m_isDragging = false;

    wxAuiTabContainerButton* m_hoverButton = nullptr;
    wxAuiTabContainerButton* m_pressedButton = nullptr;

    void SetHoverTab(wxWindow* wnd);

private:
    // Reset dragging-related fields above to their initial values.
    void DoEndDragging();

#ifndef SWIG
    wxDECLARE_CLASS(wxAuiTabCtrl);
    wxDECLARE_EVENT_TABLE();
#endif

    // Rectangle corresponding to the full tab control area, including both
    // tabs (which is this window) and the page area.
    wxRect m_fullRect;
};


// Simple struct combining wxAuiTabCtrl with the position inside it.
struct wxAuiNotebookPosition
{
    wxAuiNotebookPosition() = default;

    wxAuiNotebookPosition(wxAuiTabCtrl* tabCtrl_, int tabIdx_)
        : tabCtrl(tabCtrl_), tabIdx(tabIdx_)
    {
    }

    // Check if the position is valid.
    explicit operator bool() const { return tabCtrl != nullptr; }

    wxAuiTabCtrl* tabCtrl = nullptr;
    int tabIdx = wxNOT_FOUND;
};



class WXDLLIMPEXP_AUI wxAuiNotebook : public wxCompositeBookCtrlBase
{

public:

    wxAuiNotebook() { Init(); }

    wxAuiNotebook(wxWindow* parent,
                  wxWindowID id = wxID_ANY,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxAUI_NB_DEFAULT_STYLE)
    {
        Init();
        Create(parent, id, pos, size, style);
    }

    virtual ~wxAuiNotebook();

    bool Create(wxWindow* parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0);

    void SetWindowStyleFlag(long style) override;
    void SetArtProvider(wxAuiTabArt* art);
    wxAuiTabArt* GetArtProvider() const;

    virtual void SetUniformBitmapSize(const wxSize& size);
    virtual void SetTabCtrlHeight(int height);

    bool AddPage(wxWindow* page,
                 const wxString& caption,
                 bool select = false,
                 const wxBitmapBundle& bitmap = wxBitmapBundle());

    bool InsertPage(size_t pageIdx,
                    wxWindow* page,
                    const wxString& caption,
                    bool select = false,
                    const wxBitmapBundle& bitmap = wxBitmapBundle());

    virtual size_t GetPageCount() const override;
    virtual wxWindow* GetPage(size_t pageIdx) const override;
    virtual int FindPage(const wxWindow* page) const override;

    // This is wxAUI-specific equivalent of FindPage(), prefer to use the other
    // function.
    int GetPageIndex(wxWindow* pageWnd) const { return FindPage(pageWnd); }

    bool SetPageText(size_t page, const wxString& text) override;
    wxString GetPageText(size_t pageIdx) const override;

    bool SetPageToolTip(size_t page, const wxString& text);
    wxString GetPageToolTip(size_t pageIdx) const;

    bool SetPageBitmap(size_t page, const wxBitmapBundle& bitmap);
    wxBitmap GetPageBitmap(size_t pageIdx) const;

    wxAuiTabKind GetPageKind(size_t pageIdx) const;
    bool SetPageKind(size_t pageIdx, wxAuiTabKind kind);

    int SetSelection(size_t newPage) override;
    int GetSelection() const override;

    // Return the tab control containing the page with the given index and its
    // visual position in it (i.e. 0 for the leading one).
    wxAuiNotebookPosition GetPagePosition(size_t page) const;

    // Return all pages in the given tab control in display order.
    std::vector<size_t> GetPagesInDisplayOrder(wxAuiTabCtrl* tabCtrl) const;


    void Split(size_t page, int direction);
    void UnsplitAll();

    const wxAuiManager& GetAuiManager() const { return m_mgr; }

    void SetManagerFlags(unsigned int flags) { m_mgr.SetFlags(flags); }

    // Sets the normal font
    void SetNormalFont(const wxFont& font);

    // Sets the selected tab font
    void SetSelectedFont(const wxFont& font);

    // Sets the measuring font
    void SetMeasuringFont(const wxFont& font);

    // Sets the tab font
    virtual bool SetFont(const wxFont& font) override;

    // Gets the tab control height
    int GetTabCtrlHeight() const;

    // Gets the height of the notebook for a given page height
    int GetHeightForPageHeight(int pageHeight);

    // Shows the window menu
    bool ShowWindowMenu();

    //wxBookCtrlBase functions

    virtual void SetPageSize (const wxSize &size) override;
    virtual int  HitTest (const wxPoint &pt, long *flags=nullptr) const override;

    virtual int GetPageImage(size_t n) const override;
    virtual bool SetPageImage(size_t n, int imageId) override;

    virtual int ChangeSelection(size_t n) override;

    virtual bool AddPage(wxWindow *page, const wxString &text, bool select,
                         int imageId) override;
    virtual bool DeleteAllPages() override;
    virtual bool InsertPage(size_t index, wxWindow *page, const wxString &text,
                            bool select, int imageId) override;

    virtual wxSize DoGetBestSize() const override;

    wxAuiTabCtrl* GetTabCtrlFromPoint(const wxPoint& pt);
    wxAuiTabCtrl* GetActiveTabCtrl();

    // Get main tab control, creating it on demand if necessary.
    wxAuiTabCtrl* GetMainTabCtrl();

    // Get all tab controls.
    std::vector<wxAuiTabCtrl*> GetAllTabCtrls();


    // Internal, don't use: use GetPagePosition() instead.
    bool FindTab(wxWindow* page, wxAuiTabCtrl** ctrl, int* idx) const;

    // Serialization support: this is used by wxAuiManager but can also be
    // called directly to save/load layout of just this notebook.
    void SaveLayout(const wxString& name, wxAuiBookSerializer& serializer) const;
    void LoadLayout(const wxString& name, wxAuiBookDeserializer& deserializer);

protected:
    // Common part of all ctors.
    void Init();

    // choose the default border for this window
    virtual wxBorder GetDefaultBorder() const override { return wxBORDER_NONE; }

    // Redo sizing after thawing
    virtual void DoThaw() override;

    // these can be overridden

    // update the height, return true if it was done or false if the new height
    // calculated by CalculateTabCtrlHeight() is the same as the old one
    virtual bool UpdateTabCtrlHeight();

    virtual int CalculateTabCtrlHeight();
    virtual wxSize CalculateNewSplitSize();

    // get next page in physical (display) order
    virtual int GetNextPage(bool forward) const override;

    // remove the page and return a pointer to it
    virtual wxWindow *DoRemovePage(size_t page) override;

    //A general selection function
    virtual int DoModifySelection(size_t n, bool events);

protected:

    void DoSizing();
    void InitNotebook(long style);
    wxWindow* GetTabFrameFromTabCtrl(wxWindow* tabCtrl);
    void RemoveEmptyTabFrames();
    void UpdateHintWindowSize();

protected:

    void OnChildFocusNotebook(wxChildFocusEvent& evt);
    void OnRender(wxAuiManagerEvent& evt);
    void OnSize(wxSizeEvent& evt);
    void OnTabClicked(wxAuiNotebookEvent& evt);
    void OnTabBeginDrag(wxAuiNotebookEvent& evt);
    void OnTabDragMotion(wxAuiNotebookEvent& evt);
    void OnTabEndDrag(wxAuiNotebookEvent& evt);
    void OnTabCancelDrag(wxAuiNotebookEvent& evt);
    void OnTabButton(wxAuiNotebookEvent& evt);
    void OnTabMiddleDown(wxAuiNotebookEvent& evt);
    void OnTabMiddleUp(wxAuiNotebookEvent& evt);
    void OnTabRightDown(wxAuiNotebookEvent& evt);
    void OnTabRightUp(wxAuiNotebookEvent& evt);
    void OnTabBgDClick(wxAuiNotebookEvent& evt);
    void OnNavigationKeyNotebook(wxNavigationKeyEvent& event);
    void OnSysColourChanged(wxSysColourChangedEvent& event);
    void OnDpiChanged(wxDPIChangedEvent& event);

    // set selection to the given window (which must be non-null and be one of
    // our pages, otherwise an assert is raised)
    void SetSelectionToWindow(wxWindow *win);
    void SetSelectionToPage(const wxAuiNotebookPage& page)
    {
        SetSelectionToWindow(page.window);
    }

protected:

    wxAuiManager m_mgr;

    // Contains all pages in the insertion order.
    wxAuiTabContainer m_tabs;

    // Current page index in m_tabs or wxNOT_FOUND if none.
    int m_curPage;

    int m_tabIdCounter;
    wxWindow* m_dummyWnd;

    wxSize m_requestedBmpSize;
    int m_requestedTabCtrlHeight;
    wxFont m_selectedFont;
    wxFont m_normalFont;
    int m_tabCtrlHeight;

    int m_lastDropMovePos = -1;
    unsigned int m_flags;

private:
    // Create a new tab frame, containing a new wxAuiTabCtrl.
    wxAuiTabFrame* CreateTabFrame(wxSize size = wxSize());

    // Inserts the page at the given position into the given tab control.
    void InsertPageAt(wxAuiNotebookPage& info,
                      size_t page_idx,
                      wxAuiTabCtrl* tabctrl,
                      int tab_page_idx, // Can be -1 to append.
                      bool select);

    struct TabInfo;

    TabInfo FindTab(wxWindow* page) const;

    // Return the index at which the given page should be inserted in this tab
    // control if it's dropped at the given (in screen coordinates) point or
    // wxNOT_FOUND if dropping it is not allowed.
    int GetDropIndex(const wxAuiNotebookPage& srcPage,
                     wxAuiTabCtrl* tabCtrl,
                     const wxPoint& ptScreen) const;

#ifndef SWIG
    wxDECLARE_CLASS(wxAuiNotebook);
    wxDECLARE_EVENT_TABLE();
#endif
};




// wx event machinery

#ifndef SWIG

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_AUINOTEBOOK_PAGE_CLOSE, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_AUINOTEBOOK_PAGE_CHANGING, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_AUINOTEBOOK_PAGE_CLOSED, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_AUINOTEBOOK_BUTTON, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_AUINOTEBOOK_BEGIN_DRAG, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_AUINOTEBOOK_END_DRAG, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_AUINOTEBOOK_DRAG_MOTION, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_AUINOTEBOOK_ALLOW_DND, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_AUINOTEBOOK_TAB_MIDDLE_DOWN, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_AUINOTEBOOK_TAB_MIDDLE_UP, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_AUINOTEBOOK_TAB_RIGHT_DOWN, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_AUINOTEBOOK_TAB_RIGHT_UP, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_AUINOTEBOOK_DRAG_DONE, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_AUINOTEBOOK_BG_DCLICK, wxAuiNotebookEvent);

typedef void (wxEvtHandler::*wxAuiNotebookEventFunction)(wxAuiNotebookEvent&);

#define wxAuiNotebookEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxAuiNotebookEventFunction, func)

#define EVT_AUINOTEBOOK_PAGE_CLOSE(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_AUINOTEBOOK_PAGE_CLOSE, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_PAGE_CLOSED(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_AUINOTEBOOK_PAGE_CLOSED, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_PAGE_CHANGED(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_AUINOTEBOOK_PAGE_CHANGED, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_PAGE_CHANGING(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_AUINOTEBOOK_PAGE_CHANGING, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_BUTTON(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_AUINOTEBOOK_BUTTON, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_BEGIN_DRAG(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_AUINOTEBOOK_BEGIN_DRAG, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_END_DRAG(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_AUINOTEBOOK_END_DRAG, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_DRAG_MOTION(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_AUINOTEBOOK_DRAG_MOTION, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_ALLOW_DND(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_AUINOTEBOOK_ALLOW_DND, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_DRAG_DONE(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_AUINOTEBOOK_DRAG_DONE, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_TAB_MIDDLE_DOWN(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_AUINOTEBOOK_TAB_MIDDLE_DOWN, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_TAB_MIDDLE_UP(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_AUINOTEBOOK_TAB_MIDDLE_UP, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_TAB_RIGHT_DOWN(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_AUINOTEBOOK_TAB_RIGHT_DOWN, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_TAB_RIGHT_UP(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_AUINOTEBOOK_TAB_RIGHT_UP, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_BG_DCLICK(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_AUINOTEBOOK_BG_DCLICK, winid, wxAuiNotebookEventHandler(fn))
#else

// wxpython/swig event work
%constant wxEventType wxEVT_AUINOTEBOOK_PAGE_CLOSE;
%constant wxEventType wxEVT_AUINOTEBOOK_PAGE_CLOSED;
%constant wxEventType wxEVT_AUINOTEBOOK_PAGE_CHANGED;
%constant wxEventType wxEVT_AUINOTEBOOK_PAGE_CHANGING;
%constant wxEventType wxEVT_AUINOTEBOOK_BUTTON;
%constant wxEventType wxEVT_AUINOTEBOOK_BEGIN_DRAG;
%constant wxEventType wxEVT_AUINOTEBOOK_END_DRAG;
%constant wxEventType wxEVT_AUINOTEBOOK_DRAG_MOTION;
%constant wxEventType wxEVT_AUINOTEBOOK_ALLOW_DND;
%constant wxEventType wxEVT_AUINOTEBOOK_DRAG_DONE;
%constant wxEventType wxEVT_AUINOTEBOOK_TAB_MIDDLE_DOWN;
%constant wxEventType wxEVT_AUINOTEBOOK_TAB_MIDDLE_UP;
%constant wxEventType wxEVT_AUINOTEBOOK_TAB_RIGHT_DOWN;
%constant wxEventType wxEVT_AUINOTEBOOK_TAB_RIGHT_UP;
%constant wxEventType wxEVT_AUINOTEBOOK_BG_DCLICK;

%pythoncode {
    EVT_AUINOTEBOOK_PAGE_CLOSE = wx.PyEventBinder( wxEVT_AUINOTEBOOK_PAGE_CLOSE, 1 )
    EVT_AUINOTEBOOK_PAGE_CLOSED = wx.PyEventBinder( wxEVT_AUINOTEBOOK_PAGE_CLOSED, 1 )
    EVT_AUINOTEBOOK_PAGE_CHANGED = wx.PyEventBinder( wxEVT_AUINOTEBOOK_PAGE_CHANGED, 1 )
    EVT_AUINOTEBOOK_PAGE_CHANGING = wx.PyEventBinder( wxEVT_AUINOTEBOOK_PAGE_CHANGING, 1 )
    EVT_AUINOTEBOOK_BUTTON = wx.PyEventBinder( wxEVT_AUINOTEBOOK_BUTTON, 1 )
    EVT_AUINOTEBOOK_BEGIN_DRAG = wx.PyEventBinder( wxEVT_AUINOTEBOOK_BEGIN_DRAG, 1 )
    EVT_AUINOTEBOOK_END_DRAG = wx.PyEventBinder( wxEVT_AUINOTEBOOK_END_DRAG, 1 )
    EVT_AUINOTEBOOK_DRAG_MOTION = wx.PyEventBinder( wxEVT_AUINOTEBOOK_DRAG_MOTION, 1 )
    EVT_AUINOTEBOOK_ALLOW_DND = wx.PyEventBinder( wxEVT_AUINOTEBOOK_ALLOW_DND, 1 )
    EVT_AUINOTEBOOK_DRAG_DONE = wx.PyEventBinder( wxEVT_AUINOTEBOOK_DRAG_DONE, 1 )
    EVT__AUINOTEBOOK_TAB_MIDDLE_DOWN = wx.PyEventBinder( wxEVT_AUINOTEBOOK_TAB_MIDDLE_DOWN, 1 )
    EVT__AUINOTEBOOK_TAB_MIDDLE_UP = wx.PyEventBinder( wxEVT_AUINOTEBOOK_TAB_MIDDLE_UP, 1 )
    EVT__AUINOTEBOOK_TAB_RIGHT_DOWN = wx.PyEventBinder( wxEVT_AUINOTEBOOK_TAB_RIGHT_DOWN, 1 )
    EVT__AUINOTEBOOK_TAB_RIGHT_UP = wx.PyEventBinder( wxEVT_AUINOTEBOOK_TAB_RIGHT_UP, 1 )
    EVT_AUINOTEBOOK_BG_DCLICK = wx.PyEventBinder( wxEVT_AUINOTEBOOK_BG_DCLICK, 1 )
}
#endif


// old wxEVT_COMMAND_* constants
#define wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE        wxEVT_AUINOTEBOOK_PAGE_CLOSE
#define wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED       wxEVT_AUINOTEBOOK_PAGE_CLOSED
#define wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED      wxEVT_AUINOTEBOOK_PAGE_CHANGED
#define wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING     wxEVT_AUINOTEBOOK_PAGE_CHANGING
#define wxEVT_COMMAND_AUINOTEBOOK_BUTTON            wxEVT_AUINOTEBOOK_BUTTON
#define wxEVT_COMMAND_AUINOTEBOOK_BEGIN_DRAG        wxEVT_AUINOTEBOOK_BEGIN_DRAG
#define wxEVT_COMMAND_AUINOTEBOOK_END_DRAG          wxEVT_AUINOTEBOOK_END_DRAG
#define wxEVT_COMMAND_AUINOTEBOOK_DRAG_MOTION       wxEVT_AUINOTEBOOK_DRAG_MOTION
#define wxEVT_COMMAND_AUINOTEBOOK_ALLOW_DND         wxEVT_AUINOTEBOOK_ALLOW_DND
#define wxEVT_COMMAND_AUINOTEBOOK_DRAG_DONE         wxEVT_AUINOTEBOOK_DRAG_DONE
#define wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_DOWN   wxEVT_AUINOTEBOOK_TAB_MIDDLE_DOWN
#define wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_UP     wxEVT_AUINOTEBOOK_TAB_MIDDLE_UP
#define wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_DOWN    wxEVT_AUINOTEBOOK_TAB_RIGHT_DOWN
#define wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_UP      wxEVT_AUINOTEBOOK_TAB_RIGHT_UP
#define wxEVT_COMMAND_AUINOTEBOOK_BG_DCLICK         wxEVT_AUINOTEBOOK_BG_DCLICK
#define wxEVT_COMMAND_AUINOTEBOOK_CANCEL_DRAG       wxEVT_AUINOTEBOOK_CANCEL_DRAG

#endif  // wxUSE_AUI
#endif  // _WX_AUINOTEBOOK_H_
