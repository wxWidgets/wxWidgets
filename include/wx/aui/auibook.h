//////////////////////////////////////////////////////////////////////////////
// Name:        wx/aui/auibook.h
// Purpose:     wxaui: wx advanced user interface - notebook
// Author:      Benjamin I. Williams
// Modified by: Malcolm MacLeod (mmacleod@webmail.co.za)
// Modified by: Jens Lody
// Created:     2006-06-28
// Copyright:   (C) Copyright 2006, Kirix Corporation, All Rights Reserved.
//                            2012, Jens Lody for the code related to left
//                                  and right positioning
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////




#ifndef _WX_AUINOTEBOOK_H_
#define _WX_AUINOTEBOOK_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/defs.h"

#if wxUSE_AUI

#include "wx/bookctrl.h"
#include "wx/containr.h"
#include "wx/aui/framemanager.h"


enum wxAuiNotebookOption
{
    wxAUI_NB_TOP                 = wxAUI_MGR_NB_TOP,
    wxAUI_NB_LEFT                = wxAUI_MGR_NB_LEFT,
    wxAUI_NB_RIGHT               = wxAUI_MGR_NB_RIGHT,
    wxAUI_NB_BOTTOM              = wxAUI_MGR_NB_BOTTOM,
    wxAUI_NB_TAB_MOVE            = wxAUI_MGR_NB_TAB_MOVE,
    wxAUI_NB_TAB_EXTERNAL_MOVE   = wxAUI_MGR_ALLOW_EXTERNAL_MOVE,
    wxAUI_NB_TAB_FIXED_WIDTH     = wxAUI_MGR_NB_TAB_FIXED_WIDTH,
    wxAUI_NB_TAB_FIXED_HEIGHT    = wxAUI_MGR_NB_TAB_FIXED_HEIGHT,
    wxAUI_NB_SCROLL_BUTTONS      = wxAUI_MGR_NB_SCROLL_BUTTONS,
    wxAUI_NB_WINDOWLIST_BUTTON   = wxAUI_MGR_NB_WINDOWLIST_BUTTON,
    wxAUI_NB_CLOSE_BUTTON        = wxAUI_MGR_NB_CLOSE_BUTTON,
    wxAUI_NB_CLOSE_ON_ACTIVE_TAB = wxAUI_MGR_NB_CLOSE_ON_ACTIVE_TAB,
    wxAUI_NB_CLOSE_ON_ALL_TABS   = wxAUI_MGR_NB_CLOSE_ON_ALL_TABS,
    wxAUI_NB_MIDDLE_CLICK_CLOSE  = wxAUI_MGR_MIDDLE_CLICK_CLOSE,
    wxAUI_NB_TAB_SPLIT           = wxAUI_MGR_NB_TAB_SPLIT,

    wxAUI_NB_DEFAULT_STYLE = wxAUI_NB_TOP |
                             wxAUI_NB_TAB_SPLIT |
                             wxAUI_NB_TAB_MOVE |
                             wxAUI_NB_SCROLL_BUTTONS |
                             wxAUI_NB_CLOSE_ON_ACTIVE_TAB |
                             wxAUI_NB_MIDDLE_CLICK_CLOSE |
                             wxAUI_MGR_TRANSPARENT_HINT
};


class wxAuiNotebook;

// aui notebook event class
class WXDLLIMPEXP_AUI wxAuiNotebookEvent : public wxBookCtrlEvent
{
public:
    wxAuiNotebookEvent(wxEventType commandType = wxEVT_NULL, int winId = 0)
          : wxBookCtrlEvent(commandType, winId)
    {
        m_dragSource = NULL;
    }
#ifndef SWIG
    wxAuiNotebookEvent(const wxAuiNotebookEvent& c) : wxBookCtrlEvent(c)
    {
        m_dragSource = c.m_dragSource;
    }
#endif
    wxEvent *Clone() const { return new wxAuiNotebookEvent(*this); }

    void SetDragSource(wxAuiNotebook* s) { m_dragSource = s; }
    wxAuiNotebook* GetDragSource() const { return m_dragSource; }

private:
    wxAuiNotebook* m_dragSource;

#ifndef SWIG
private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxAuiNotebookEvent)
#endif
};


class WXDLLIMPEXP_AUI wxAuiNotebook : public wxNavigationEnabled<wxBookCtrlBase>
{
public:

    void SetColour(const wxColour& colour);
    void SetActiveColour(const wxColour& colour);


    wxAuiNotebook() { }
    wxAuiNotebook(wxWindow* parent,
                  wxWindowID id = wxID_ANY,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxAUI_NB_DEFAULT_STYLE)
    {
        Create(parent, id, pos, size, style);
    }

    virtual ~wxAuiNotebook();

    bool Create(wxWindow* parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0);

    void SetWindowStyleFlag(long style);
    void SetArtProvider(wxAuiTabArt* art);
    wxAuiTabArt* GetArtProvider() const;

    virtual void SetUniformBitmapSize(const wxSize& size);
    virtual void SetTabCtrlHeight(int height);
    virtual void SetTabCtrlWidth(int width);

    bool AddPage(wxWindow* page,
                 const wxString& caption,
                 bool select = false,
                 const wxBitmap& bitmap = wxNullBitmap);

    bool InsertPage(size_t pageIndex,
                    wxWindow* page,
                    const wxString& caption,
                    bool select = false,
                    const wxBitmap& bitmap = wxNullBitmap);

    bool DeletePage(size_t page);
    bool RemovePage(size_t page);

    virtual size_t GetPageCount() const;
    virtual wxWindow* GetPage(size_t pageIndex) const;
    int GetPageIndex(wxWindow* pageWindow);

    bool SetPageText(size_t page, const wxString& text);
    wxString GetPageText(size_t pageIndex) const;

    bool SetPageToolTip(size_t page_idx, const wxString& text);
    wxString GetPageToolTip(size_t page_idx) const;

    bool SetPageBitmap(size_t page, const wxBitmap& bitmap);
    wxBitmap GetPageBitmap(size_t pageIndex) const;

    int SetSelection(size_t newPage);
    int GetSelection() const;

    virtual void Split(size_t page, int direction);

    const wxAuiManager& GetAuiManager() const { return m_mgr; }

    // Sets the normal font
    void SetNormalFont(const wxFont& font);

    // Sets the selected tab font
    void SetSelectedFont(const wxFont& font);

    // Sets the measuring font
    void SetMeasuringFont(const wxFont& font);

    // Sets the tab font
    virtual bool SetFont(const wxFont& font);

    // Gets the tab control height
    int GetTabCtrlHeight() const;

    // Gets the tab control width
    int GetTabCtrlWidth() const;

    // Gets the height of the notebook for a given page height
    int GetHeightForPageHeight(int pageHeight);

    // Gets the width of the notebook for a given page width
    int GetWidthForPageWidth(int pageWidth);

    // Advances the selection, generation page selection events
    void AdvanceSelection(bool forward = true);

    // Shows the window menu
    bool ShowWindowMenu();

    // we do have multiple pages
    virtual bool HasMultiplePages() const { return true; }

    // we don't want focus for ourselves
    // virtual bool AcceptsFocus() const { return false; }

    //wxBookCtrlBase functions

    virtual void SetPageSize (const wxSize& size);
    virtual int  HitTest (const wxPoint& pt, long* flags=NULL) const;

    virtual int GetPageImage(size_t n) const;
    virtual bool SetPageImage(size_t n, int imageId);

    wxWindow* GetCurrentPage() const;

    virtual int ChangeSelection(size_t n);

    virtual bool AddPage(wxWindow* page, const wxString& text, bool select,  int imageId);
    virtual bool DeleteAllPages();
    virtual bool InsertPage(size_t index, wxWindow* page, const wxString& text, bool select, int imageId);

    // Returns true if the tabart has the given flag bit set
    bool HasFlag(int flag) const    { return m_mgr.HasFlag(flag); }
    // returns true if we have wxAUI_NB_TOP or wxAUI_NB_BOTTOM style
    bool IsHorizontal() const { return HasFlag(wxAUI_NB_TOP | wxAUI_NB_BOTTOM); }

protected:
    // choose the default border for this window
    virtual wxBorder GetDefaultBorder() const { return wxBORDER_NONE; }

    // Redo sizing after thawing
    virtual void DoThaw();

    // these can be overridden

    // update the size, return true if it was done or false if the new size
    // calculated by CalculateTabCtrlSize is the same as the old one
    virtual bool UpdateTabCtrlSize();

    virtual wxSize CalculateTabCtrlSize();

    // remove the page and return a pointer to it
    virtual wxWindow* DoRemovePage(size_t WXUNUSED(page)) { return NULL; }

    //A general selection function
    virtual int DoModifySelection(size_t n, bool events);

protected:

    void Init(long style);
    wxAuiTabContainer* GetActiveTabCtrl();
    void OnTabCancelDrag(wxAuiNotebookEvent& evt);
    void OnPaneDrop(wxAuiManagerEvent& evt);

protected:
    wxAuiManager m_mgr;
#ifndef SWIG
    DECLARE_CLASS(wxAuiNotebook)
    DECLARE_EVENT_TABLE()
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
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_AUINOTEBOOK_TAB_DCLICK, wxAuiNotebookEvent);

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
#define EVT_AUINOTEBOOK_TAB_DCLICK(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_AUINOTEBOOK_TAB_DCLICK, winid, wxAuiNotebookEventHandler(fn))
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
%constant wxEventType wxEVT_AUINOTEBOOK_TAB_DCLICK;

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
    EVT_AUINOTEBOOK_TAB_MIDDLE_DOWN = wx.PyEventBinder( wxEVT_AUINOTEBOOK_TAB_MIDDLE_DOWN, 1 )
    EVT_AUINOTEBOOK_TAB_MIDDLE_UP = wx.PyEventBinder( wxEVT_AUINOTEBOOK_TAB_MIDDLE_UP, 1 )
    EVT_AUINOTEBOOK_TAB_RIGHT_DOWN = wx.PyEventBinder( wxEVT_AUINOTEBOOK_TAB_RIGHT_DOWN, 1 )
    EVT_AUINOTEBOOK_TAB_RIGHT_UP = wx.PyEventBinder( wxEVT_AUINOTEBOOK_TAB_RIGHT_UP, 1 )
    EVT_AUINOTEBOOK_BG_DCLICK = wx.PyEventBinder( wxEVT_AUINOTEBOOK_BG_DCLICK, 1 )
    EVT_AUINOTEBOOK_TAB_DCLICK = wx.PyEventBinder( wxEVT_AUINOTEBOOK_TAB_DCLICK, 1 )
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
