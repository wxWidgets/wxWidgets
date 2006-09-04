///////////////////////////////////////////////////////////////////////////////
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
#include "wx/aui/floatpane.h"
#include "wx/control.h"


// event declarations/classes

class WXDLLIMPEXP_AUI wxAuiNotebookEvent : public wxNotifyEvent
{
public:
    wxAuiNotebookEvent(wxEventType command_type = wxEVT_NULL,
                       int win_id = 0)
          : wxNotifyEvent(command_type, win_id)
    {
    }
#ifndef SWIG
    wxAuiNotebookEvent(const wxAuiNotebookEvent& c) : wxNotifyEvent(c)
    {
        old_selection = c.old_selection;
        selection = c.selection;
    }
#endif
    wxEvent *Clone() const { return new wxAuiNotebookEvent(*this); }

    void SetSelection(int s) { selection = s; }
    void SetOldSelection(int s) { old_selection = s; }
    int GetSelection() const { return selection; }
    int GetOldSelection() const { return old_selection; }

public:
    int old_selection;
    int selection;

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

class WXDLLIMPEXP_AUI wxAuiTabContainerButton
{
public:
    int id;               // button's id
    int cur_state;        // current state (normal, hover, pressed)
    wxBitmap bitmap;      // button's bitmap
    wxRect rect;          // button's hit rectangle
};


#ifndef SWIG
WX_DECLARE_USER_EXPORTED_OBJARRAY(wxAuiNotebookPage, wxAuiNotebookPageArray, WXDLLIMPEXP_AUI);
WX_DECLARE_USER_EXPORTED_OBJARRAY(wxAuiTabContainerButton, wxAuiTabContainerButtonArray, WXDLLIMPEXP_AUI);
#endif


class WXDLLIMPEXP_AUI wxAuiTabContainer
{
public:

    wxAuiTabContainer();
    virtual ~wxAuiTabContainer();

    bool AddPage(wxWindow* page, const wxAuiNotebookPage& info);
    bool InsertPage(wxWindow* page, const wxAuiNotebookPage& info, size_t idx);
    bool RemovePage(wxWindow* page);
    bool SetActivePage(wxWindow* page);
    bool SetActivePage(size_t page);
    void SetNoneActive();
    int GetActivePage() const;
    bool TabHitTest(int x, int y, wxWindow** hit) const;
    bool ButtonHitTest(int x, int y, wxAuiTabContainerButton** hit) const;
    wxWindow* GetWindowFromIdx(size_t idx) const;
    int GetIdxFromWindow(wxWindow* page) const;
    size_t GetPageCount() const;
    wxAuiNotebookPage& GetPage(size_t idx);
    wxAuiNotebookPageArray& GetPages();
    void SetNormalFont(const wxFont& normal_font);
    void SetSelectedFont(const wxFont& selected_font);
    void SetMeasuringFont(const wxFont& measuring_font);
    void DoShowHide();
    void SetRect(const wxRect& rect);
    void AddButton(int id, const wxBitmap& bmp);

protected:

    virtual void Render(wxDC* dc);

    virtual void DrawTab(wxDC* dc,
                         const wxRect& in_rect,
                         const wxString& caption,
                         bool active,
                         wxRect* out_rect,
                         int* x_extent);
private:

    wxAuiNotebookPageArray m_pages;
    wxAuiTabContainerButtonArray m_buttons;
    wxRect m_rect;
    wxFont m_normal_font;
    wxFont m_selected_font;
    wxFont m_measuring_font;
    wxPen m_normal_bkpen;
    wxPen m_selected_bkpen;
    wxBrush m_normal_bkbrush;
    wxBrush m_selected_bkbrush;
    wxBrush m_bkbrush;
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

protected:

    void OnPaint(wxPaintEvent& evt);
    void OnEraseBackground(wxEraseEvent& evt);
    void OnSize(wxSizeEvent& evt);
    void OnLeftDown(wxMouseEvent& evt);
    void OnLeftUp(wxMouseEvent& evt);
    void OnMotion(wxMouseEvent& evt);
    void OnLeaveWindow(wxMouseEvent& evt);

protected:

    wxPoint m_click_pt;
    int m_click_tab;
    bool m_is_dragging;
    wxAuiTabContainerButton* m_hover_button;

#ifndef SWIG
    DECLARE_EVENT_TABLE()
#endif
};




class WXDLLIMPEXP_AUI wxAuiMultiNotebook : public wxControl
{

public:

    wxAuiMultiNotebook();

    wxAuiMultiNotebook(wxWindow* parent,
                       wxWindowID id = wxID_ANY,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = 0);

    virtual ~wxAuiMultiNotebook();

    bool Create(wxWindow* parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0);

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

    bool SetPageText(size_t page, const wxString& text);
    size_t SetSelection(size_t new_page);
    int GetSelection() const;
    size_t GetPageCount() const;
    wxWindow* GetPage(size_t page_idx) const;

protected:

    wxAuiTabCtrl* GetTabCtrlFromPoint(const wxPoint& pt);
    wxWindow* GetTabFrameFromTabCtrl(wxWindow* tab_ctrl);
    wxAuiTabCtrl* GetActiveTabCtrl();
    bool FindTab(wxWindow* page, wxAuiTabCtrl** ctrl, int* idx);
    void RemoveEmptyTabFrames();

protected:

    void DoSizing();
    void InitNotebook();

    void OnChildFocus(wxChildFocusEvent& evt);
    void OnRender(wxFrameManagerEvent& evt);
    void OnEraseBackground(wxEraseEvent& evt);
    void OnSize(wxSizeEvent& evt);
    void OnTabClicked(wxCommandEvent& evt);
    void OnTabBeginDrag(wxCommandEvent& evt);
    void OnTabDragMotion(wxCommandEvent& evt);
    void OnTabEndDrag(wxCommandEvent& evt);
    void OnTabButton(wxCommandEvent& evt);

protected:

    wxFrameManager m_mgr;
    wxAuiTabContainer m_tabs;
    int m_curpage;
    int m_tab_id_counter;
    wxWindow* m_dummy_wnd;

    wxFont m_selected_font;
    wxFont m_normal_font;
    int m_tab_ctrl_height;

#ifndef SWIG
    DECLARE_EVENT_TABLE()
#endif
};




// wx event machinery

#ifndef SWIG

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_BUTTON, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_BEGIN_DRAG, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_END_DRAG, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_DRAG_MOTION, 0)
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::*wxAuiNotebookEventFunction)(wxAuiNotebookEvent&);

#define wxAuiNotebookEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxAuiNotebookEventFunction, &func)

#define EVT_AUINOTEBOOK_PAGE_CHANGED(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_PAGE_CHANGING(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_PAGE_BUTTON(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_BUTTON, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_BEGIN_DRAG(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_BEGIN_DRAG, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_END_DRAG(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_END_DRAG, winid, wxAuiNotebookEventHandler(fn))
#define EVT_AUINOTEBOOK_DRAG_MOTION(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_DRAG_MOTION, winid, wxAuiNotebookEventHandler(fn))

#else

// wxpython/swig event work
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_BUTTON;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_BEGIN_DRAG;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_END_DRAG;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_DRAG_MOTION;

%pythoncode {
    EVT_AUINOTEBOOK_PAGE_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, 1 )
    EVT_AUINOTEBOOK_PAGE_CHANGING = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING, 1 )
    EVT_AUINOTEBOOK_BUTTON = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_BUTTON, 1 )
    EVT_AUINOTEBOOK_BEGIN_DRAG = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_BEGIN_DRAG, 1 )
    EVT_AUINOTEBOOK_END_DRAG = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_END_DRAG, 1 )
    EVT_AUINOTEBOOK_DRAG_MOTION = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_DRAG_MOTION, 1 )
}
#endif


#endif  // wxUSE_AUI
#endif  // _WX_AUINOTEBOOK_H_
