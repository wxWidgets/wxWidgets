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


class wxAuiNotebook;


enum wxAuiNotebookOption
{
    wxAUI_NB_TOP                 = 1 << 0,
    wxAUI_NB_LEFT                = 1 << 1,  // not implemented yet
    wxAUI_NB_RIGHT               = 1 << 2,  // not implemented yet
    wxAUI_NB_BOTTOM              = 1 << 3,  // not implemented yet
    wxAUI_NB_TAB_SPLIT           = 1 << 4,
    wxAUI_NB_TAB_MOVE            = 1 << 5,
    wxAUI_NB_TAB_EXTERNAL_MOVE   = 1 << 6,
    wxAUI_NB_TAB_FIXED_WIDTH     = 1 << 7,
    wxAUI_NB_SCROLL_BUTTONS      = 1 << 8,
    wxAUI_NB_WINDOWLIST_BUTTON   = 1 << 9,
    wxAUI_NB_CLOSE_BUTTON        = 1 << 10,
    wxAUI_NB_CLOSE_ON_ACTIVE_TAB = 1 << 11,
    wxAUI_NB_CLOSE_ON_ALL_TABS   = 1 << 12,
    
    
    wxAUI_NB_DEFAULT_STYLE = wxAUI_NB_TOP |
                             wxAUI_NB_TAB_SPLIT |
                             wxAUI_NB_TAB_MOVE |
                             wxAUI_NB_SCROLL_BUTTONS |
                             wxAUI_NB_CLOSE_ON_ACTIVE_TAB
};




// tab art class

class WXDLLIMPEXP_AUI wxAuiTabArt
{
public:

    wxAuiTabArt() { }
    virtual ~wxAuiTabArt() { }
    
    virtual wxAuiTabArt* Clone() = 0;
    virtual void SetFlags(unsigned int flags) = 0;

    virtual void SetSizingInfo(const wxSize& tab_ctrl_size,
                               size_t tab_count) = 0;
                               
    virtual void SetNormalFont(const wxFont& font) = 0;
    virtual void SetSelectedFont(const wxFont& font) = 0;
    virtual void SetMeasuringFont(const wxFont& font) = 0;

    virtual void DrawBackground(
                         wxDC& dc,
                         wxWindow* wnd,
                         const wxRect& rect) = 0;

    virtual void DrawTab(wxDC& dc,
                         wxWindow* wnd,
                         const wxRect& in_rect,
                         const wxString& caption,
                         bool active,
                         int close_button_state,
                         wxRect* out_tab_rect,
                         wxRect* out_button_rect,
                         int* x_extent) = 0;     
    
    virtual void DrawButton(
                         wxDC& dc,
                         wxWindow* wnd,
                         const wxRect& in_rect,
                         int bitmap_id,
                         int button_state,
                         int orientation,
                         const wxBitmap& bitmap_override,
                         wxRect* out_rect) = 0;
               
    virtual int GetIndentSize() = 0;
  
    virtual wxSize GetTabSize(
                         wxDC& dc,
                         wxWindow* wnd,
                         const wxString& caption,
                         bool active,
                         int close_button_state,
                         int* x_extent) = 0;
                         
    virtual int ShowWindowList(
                         wxWindow* wnd,
                         const wxArrayString& items,
                         int active_idx) = 0;
    
    virtual int GetBestTabCtrlSize(wxWindow* wnd) = 0;    
};


class WXDLLIMPEXP_AUI wxAuiSimpleTabArt : public wxAuiTabArt
{

public:

    wxAuiSimpleTabArt();
    virtual ~wxAuiSimpleTabArt();
    
    wxAuiTabArt* Clone();
    void SetFlags(unsigned int flags);

    void SetSizingInfo(const wxSize& tab_ctrl_size,
                       size_t tab_count);

    void SetNormalFont(const wxFont& font);
    void SetSelectedFont(const wxFont& font);
    void SetMeasuringFont(const wxFont& font);

    void DrawBackground(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& rect);
                                          
    void DrawTab(wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& in_rect,
                 const wxString& caption,
                 bool active,
                 int close_button_state,
                 wxRect* out_tab_rect,
                 wxRect* out_button_rect,
                 int* x_extent);
    
    void DrawButton(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& in_rect,
                 int bitmap_id,
                 int button_state,
                 int orientation,
                 const wxBitmap& bitmap_override,
                 wxRect* out_rect);
             
    int GetIndentSize();
    
    wxSize GetTabSize(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxString& caption,
                 bool active,
                 int close_button_state,
                 int* x_extent);
                  
    int ShowWindowList(
                 wxWindow* wnd,
                 const wxArrayString& items,
                 int active_idx);

    int GetBestTabCtrlSize(wxWindow* wnd);    

protected:

    wxFont m_normal_font;
    wxFont m_selected_font;
    wxFont m_measuring_font;
    wxPen m_normal_bkpen;
    wxPen m_selected_bkpen;
    wxBrush m_normal_bkbrush;
    wxBrush m_selected_bkbrush;
    wxBrush m_bkbrush;
    wxBitmap m_active_close_bmp;
    wxBitmap m_disabled_close_bmp;
    wxBitmap m_active_left_bmp;
    wxBitmap m_disabled_left_bmp;
    wxBitmap m_active_right_bmp;
    wxBitmap m_disabled_right_bmp;
    wxBitmap m_active_windowlist_bmp;
    wxBitmap m_disabled_windowlist_bmp;
    
    int m_fixed_tab_width;
    unsigned int m_flags;
};


class WXDLLIMPEXP_AUI wxAuiDefaultTabArt : public wxAuiTabArt
{

public:

    wxAuiDefaultTabArt();
    virtual ~wxAuiDefaultTabArt();
    
    wxAuiTabArt* Clone();
    void SetFlags(unsigned int flags);
    void SetSizingInfo(const wxSize& tab_ctrl_size,
                       size_t tab_count);

    void SetNormalFont(const wxFont& font);
    void SetSelectedFont(const wxFont& font);
    void SetMeasuringFont(const wxFont& font);

    void DrawBackground(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& rect);
                     
    void DrawTab(wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& in_rect,
                 const wxString& caption,
                 bool active,
                 int close_button_state,
                 wxRect* out_tab_rect,
                 wxRect* out_button_rect,
                 int* x_extent);
    
    void DrawButton(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& in_rect,
                 int bitmap_id,
                 int button_state,
                 int orientation,
                 const wxBitmap& bitmap_override,
                 wxRect* out_rect);
    
    int GetIndentSize();
                 
    wxSize GetTabSize(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxString& caption,
                 bool active,
                 int close_button_state,
                 int* x_extent);
                  
    int ShowWindowList(
                 wxWindow* wnd,
                 const wxArrayString& items,
                 int active_idx);

    int GetBestTabCtrlSize(wxWindow* wnd);    

protected:

    wxFont m_normal_font;
    wxFont m_selected_font;
    wxFont m_measuring_font;
    wxPen m_normal_bkpen;
    wxPen m_selected_bkpen;
    wxBrush m_normal_bkbrush;
    wxBrush m_selected_bkbrush;
    wxBrush m_bkbrush;
    wxBitmap m_active_close_bmp;
    wxBitmap m_disabled_close_bmp;
    wxBitmap m_active_left_bmp;
    wxBitmap m_disabled_left_bmp;
    wxBitmap m_active_right_bmp;
    wxBitmap m_disabled_right_bmp;
    wxBitmap m_active_windowlist_bmp;
    wxBitmap m_disabled_windowlist_bmp;
    
    int m_fixed_tab_width;
    unsigned int m_flags;
};




// event declarations/classes

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

class WXDLLIMPEXP_AUI wxAuiTabContainerButton
{
public:

    int id;               // button's id
    int cur_state;        // current state (normal, hover, pressed, etc.)
    int location;         // buttons location (wxLEFT, wxRIGHT, or wxCENTER)
    wxBitmap bitmap;      // button's hover bitmap
    wxBitmap dis_bitmap;  // button's disabled bitmap
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

    void SetArtProvider(wxAuiTabArt* art);
    wxAuiTabArt* GetArtProvider();

    void SetFlags(unsigned int flags);
    unsigned int GetFlags() const;

    bool AddPage(wxWindow* page, const wxAuiNotebookPage& info);
    bool InsertPage(wxWindow* page, const wxAuiNotebookPage& info, size_t idx);
    bool MovePage(wxWindow* page, size_t new_idx);
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
    
    void RemoveButton(int id);
    void AddButton(int id,
                   int location,
                   const wxBitmap& normal_bitmap = wxNullBitmap,
                   const wxBitmap& disabled_bitmap = wxNullBitmap);

    size_t GetTabOffset() const;
    void SetTabOffset(size_t offset);
    
protected:

    virtual void Render(wxDC* dc, wxWindow* wnd);

protected:

    wxAuiTabArt* m_art;
    wxAuiNotebookPageArray m_pages;
    wxAuiTabContainerButtonArray m_buttons;
    wxAuiTabContainerButtonArray m_tab_close_buttons;
    wxRect m_rect;
    size_t m_tab_offset;
    unsigned int m_flags;
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
    
protected:

    void OnPaint(wxPaintEvent& evt);
    void OnEraseBackground(wxEraseEvent& evt);
    void OnSize(wxSizeEvent& evt);
    void OnLeftDown(wxMouseEvent& evt);
    void OnLeftUp(wxMouseEvent& evt);
    void OnMotion(wxMouseEvent& evt);
    void OnLeaveWindow(wxMouseEvent& evt);
    void OnButton(wxAuiNotebookEvent& evt);

    
protected:

    wxPoint m_click_pt;
    wxWindow* m_click_tab;
    bool m_is_dragging;
    wxAuiTabContainerButton* m_hover_button;

#ifndef SWIG
    DECLARE_CLASS(wxAuiTabCtrl)
    DECLARE_EVENT_TABLE()
#endif
};




class WXDLLIMPEXP_AUI wxAuiNotebook : public wxControl
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
    
    void SetWindowStyleFlag(long style);

    bool SetPageText(size_t page, const wxString& text);
    size_t SetSelection(size_t new_page);
    int GetSelection() const;
    size_t GetPageCount() const;
    wxWindow* GetPage(size_t page_idx) const;


    void SetArtProvider(wxAuiTabArt* art);
    wxAuiTabArt* GetArtProvider();

protected:

    wxAuiTabCtrl* GetTabCtrlFromPoint(const wxPoint& pt);
    wxWindow* GetTabFrameFromTabCtrl(wxWindow* tab_ctrl);
    wxAuiTabCtrl* GetActiveTabCtrl();
    bool FindTab(wxWindow* page, wxAuiTabCtrl** ctrl, int* idx);
    void RemoveEmptyTabFrames();

protected:

    void DoSizing();
    void InitNotebook(long style);

    void OnChildFocus(wxChildFocusEvent& evt);
    void OnRender(wxAuiManagerEvent& evt);
    void OnEraseBackground(wxEraseEvent& evt);
    void OnSize(wxSizeEvent& evt);
    void OnTabClicked(wxCommandEvent& evt);
    void OnTabBeginDrag(wxCommandEvent& evt);
    void OnTabDragMotion(wxCommandEvent& evt);
    void OnTabEndDrag(wxCommandEvent& evt);
    void OnTabButton(wxCommandEvent& evt);
    
protected:

    wxAuiManager m_mgr;
    wxAuiTabContainer m_tabs;
    int m_curpage;
    int m_tab_id_counter;
    wxWindow* m_dummy_wnd;

    wxFont m_selected_font;
    wxFont m_normal_font;
    int m_tab_ctrl_height;
    
    int m_last_drag_x;
    unsigned int m_flags;

#ifndef SWIG
    DECLARE_CLASS(wxAuiNotebook)
    DECLARE_EVENT_TABLE()
#endif
};




// wx event machinery

#ifndef SWIG

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_BUTTON, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_BEGIN_DRAG, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_END_DRAG, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_DRAG_MOTION, 0)
    DECLARE_EXPORTED_EVENT_TYPE(WXDLLIMPEXP_AUI, wxEVT_COMMAND_AUINOTEBOOK_ALLOW_DND, 0)
END_DECLARE_EVENT_TYPES()

typedef void (wxEvtHandler::*wxAuiNotebookEventFunction)(wxAuiNotebookEvent&);

#define wxAuiNotebookEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxAuiNotebookEventFunction, &func)
    
#define EVT_AUINOTEBOOK_PAGE_CLOSE(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE, winid, wxAuiNotebookEventHandler(fn))
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

#else

// wxpython/swig event work
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_BUTTON;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_BEGIN_DRAG;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_END_DRAG;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_DRAG_MOTION;
%constant wxEventType wxEVT_COMMAND_AUINOTEBOOK_ALLOW_DND;

%pythoncode {
    EVT_AUINOTEBOOK_PAGE_CLOSE = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE, 1 )
    EVT_AUINOTEBOOK_PAGE_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, 1 )
    EVT_AUINOTEBOOK_PAGE_CHANGING = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING, 1 )
    EVT_AUINOTEBOOK_BUTTON = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_BUTTON, 1 )
    EVT_AUINOTEBOOK_BEGIN_DRAG = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_BEGIN_DRAG, 1 )
    EVT_AUINOTEBOOK_END_DRAG = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_END_DRAG, 1 )
    EVT_AUINOTEBOOK_DRAG_MOTION = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_DRAG_MOTION, 1 )
    EVT_AUINOTEBOOK_ALLOW_DND = wx.PyEventBinder( wxEVT_COMMAND_AUINOTEBOOK_ALLOW_DND, 1 )
}
#endif


#endif  // wxUSE_AUI
#endif  // _WX_AUINOTEBOOK_H_
