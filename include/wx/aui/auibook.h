//////////////////////////////////////////////////////////////////////////////
// Name:        wx/aui/auibook.h
// Purpose:     wxaui: wx advanced user interface - notebook
// Author:      Benjamin I. Williams
// Modified by:
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

#include "wx/aui/framemanager.h"
#include "wx/aui/dockart.h"



class wxAuiNotebook;


//temp (MJM)
class wxAuiTabCtrl : public wxAuiTabContainer
{
};


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

    wxAUI_NB_DEFAULT_STYLE = wxAUI_NB_TOP |
                             wxAUI_NB_TAB_SPLIT |
                             wxAUI_NB_TAB_MOVE |
                             wxAUI_NB_SCROLL_BUTTONS |
                             wxAUI_NB_CLOSE_ON_ACTIVE_TAB |
                             wxAUI_NB_MIDDLE_CLICK_CLOSE
};




// aui notebook event class
class WXDLLIMPEXP_AUI wxAuiNotebookEvent : public wxNotifyEvent
{
public:
    wxAuiNotebookEvent(wxEventType command_type = wxEVT_NULL,
                       int win_id = 0)
          : wxNotifyEvent(command_type, win_id)
    {
        old_selection = -1;
        selection = -1;
        drag_source = NULL;
    }
#ifndef SWIG
    wxAuiNotebookEvent(const wxAuiNotebookEvent& c) : wxNotifyEvent(c)
    {
        old_selection = c.old_selection;
        selection = c.selection;
        drag_source = c.drag_source;
    }
#endif
    wxEvent *Clone() const { return new wxAuiNotebookEvent(*this); }

    void SetSelection(int s) { selection = s; m_commandInt = s; }
    int GetSelection() const { return selection; }

    void SetOldSelection(int s) { old_selection = s; }
    int GetOldSelection() const { return old_selection; }

    void SetDragSource(wxAuiNotebook* s) { drag_source = s; }
    wxAuiNotebook* GetDragSource() const { return drag_source; }

public:
    int old_selection;
    int selection;
    wxAuiNotebook* drag_source;

#ifndef SWIG
private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxAuiNotebookEvent)
#endif
};


class WXDLLIMPEXP_AUI wxAuiNotebookPage
{
public:
    wxWindow* window;     // page's associated window
    wxString caption;     // caption displayed on the tab
    wxBitmap bitmap;      // tab's bitmap
    wxRect rect;          // tab's hit rectangle
    bool active;          // true if the page is currently active
};

#ifndef SWIG
WX_DECLARE_USER_EXPORTED_OBJARRAY(wxAuiNotebookPage, wxAuiNotebookPageArray, WXDLLIMPEXP_AUI);
#endif


class WXDLLIMPEXP_AUI wxAuiNotebook : public wxPanel
{

public:

    wxAuiNotebook();

    wxAuiNotebook(wxWindow* parent,
                  wxWindowID id = wxID_ANY,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxAUI_NB_DEFAULT_STYLE);

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

    bool AddPage(wxWindow* page,
                 const wxString& caption,
                 bool select = false,
                 const wxBitmap& bitmap = wxNullBitmap);

    bool InsertPage(size_t page_idx,
                    wxWindow* page,
                    const wxString& caption,
                    bool select = false,
                    const wxBitmap& bitmap = wxNullBitmap);

    bool DeletePage(size_t page);
    bool RemovePage(size_t page);

    size_t GetPageCount() const;
    wxWindow* GetPage(size_t page_idx) const;
    int GetPageIndex(wxWindow* page_wnd);

    bool SetPageText(size_t page, const wxString& text);
    wxString GetPageText(size_t page_idx) const;

    bool SetPageBitmap(size_t page, const wxBitmap& bitmap);
    wxBitmap GetPageBitmap(size_t page_idx) const;

    size_t SetSelection(size_t new_page);
    int GetSelection();

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

    // Gets the height of the notebook for a given page height
    int GetHeightForPageHeight(int pageHeight);

    // Advances the selection, generation page selection events
    void AdvanceSelection(bool forward = true);

    // Shows the window menu
    bool ShowWindowMenu();

    // we do have multiple pages
    virtual bool HasMultiplePages() const { return true; }

    // we don't want focus for ourselves
    // virtual bool AcceptsFocus() const { return false; }

protected:
    // choose the default border for this window
    virtual wxBorder GetDefaultBorder() const { return wxBORDER_NONE; }

    // these can be overridden
    virtual void UpdateTabCtrlHeight();
    virtual int CalculateTabCtrlHeight();

protected:

    void InitNotebook(long style);
    wxAuiTabCtrl* GetActiveTabCtrl();

protected:
    wxAuiManager m_mgr;
    //temp: (MJM) - The below should all be removed, they belong in the aui manager or the art class
    wxSize m_requested_bmp_size;
    int m_requested_tabctrl_height;
    wxFont m_selected_font;
    wxFont m_normal_font;
    int m_tab_ctrl_height;
    unsigned int m_flags;
#ifndef SWIG
    DECLARE_CLASS(wxAuiNotebook)
#endif
};




// wx event machinery

#ifndef SWIG

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_BUTTON, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_BEGIN_DRAG, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_END_DRAG, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_DRAG_MOTION, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_ALLOW_DND, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_DOWN, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_UP, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_DOWN, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_UP, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_DRAG_DONE, wxAuiNotebookEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_BG_DCLICK, wxAuiNotebookEvent);

typedef void (wxEvtHandler::*wxAuiNotebookEventFunction)(wxAuiNotebookEvent&);

#define wxAuiNotebookEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxAuiNotebookEventFunction, func)

#define EVT_AUINOTEBOOK_PAGE_CLOSE(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_PAGE_CLOSED(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_PAGE_CHANGED(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_PAGE_CHANGING(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_BUTTON(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_BUTTON, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_BEGIN_DRAG(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_BEGIN_DRAG, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_END_DRAG(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_END_DRAG, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_DRAG_MOTION(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_DRAG_MOTION, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_ALLOW_DND(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_ALLOW_DND, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_DRAG_DONE(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_DRAG_DONE, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_TAB_MIDDLE_DOWN(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_DOWN, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_TAB_MIDDLE_UP(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_UP, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_TAB_RIGHT_DOWN(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_DOWN, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_TAB_RIGHT_UP(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_UP, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_BG_DCLICK(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_BG_DCLICK, winid, wxAuiNotebookEventHandler(fn))
#else

// wxpython/swig event work
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_BUTTON;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_BEGIN_DRAG;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_END_DRAG;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_DRAG_MOTION;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_ALLOW_DND;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_DRAG_DONE;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_DOWN;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_UP;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_DOWN;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_UP;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_BG_DCLICK;

%pythoncode {
    EVT_AUINOTEBOOK_PAGE_CLOSE = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE, 1 )
    EVT_AUINOTEBOOK_PAGE_CLOSED = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSED, 1 )
    EVT_AUINOTEBOOK_PAGE_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, 1 )
    EVT_AUINOTEBOOK_PAGE_CHANGING = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING, 1 )
    EVT_AUINOTEBOOK_BUTTON = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_BUTTON, 1 )
    EVT_AUINOTEBOOK_BEGIN_DRAG = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_BEGIN_DRAG, 1 )
    EVT_AUINOTEBOOK_END_DRAG = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_END_DRAG, 1 )
    EVT_AUINOTEBOOK_DRAG_MOTION = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_DRAG_MOTION, 1 )
    EVT_AUINOTEBOOK_ALLOW_DND = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_ALLOW_DND, 1 )
    EVT_AUINOTEBOOK_DRAG_DONE = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_DRAG_DONE, 1 )
    EVT__AUINOTEBOOK_TAB_MIDDLE_DOWN = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_DOWN, 1 )
    EVT__AUINOTEBOOK_TAB_MIDDLE_UP = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_UP, 1 )
    EVT__AUINOTEBOOK_TAB_RIGHT_DOWN = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_DOWN, 1 )
    EVT__AUINOTEBOOK_TAB_RIGHT_UP = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_TAB_RIGHT_UP, 1 )
    EVT_AUINOTEBOOK_BG_DCLICK = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_BG_DCLICK, 1 )
}
#endif


#endif  // wxUSE_AUI
#endif  // _WX_AUINOTEBOOK_H_