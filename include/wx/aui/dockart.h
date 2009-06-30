///////////////////////////////////////////////////////////////////////////////
// Name:        wx/aui/dockart.h
// Purpose:     wxaui: wx advanced user interface - docking window manager
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2005-05-17
// RCS-ID:      $Id$
// Copyright:   (C) Copyright 2005, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DOCKART_H_
#define _WX_DOCKART_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/defs.h"

#if wxUSE_AUI

#include "wx/pen.h"
#include "wx/brush.h"
#include "wx/bitmap.h"
#include "wx/colour.h"

// dock art provider code - a dock provider provides all drawing
// functionality to the wxAui dock manager.  This allows the dock
// manager to have plugable look-and-feels

class WXDLLIMPEXP_AUI wxAuiDockArt
{
public:

    wxAuiDockArt() { }
    virtual ~wxAuiDockArt() { }

    virtual int GetMetric(int id) = 0;
    virtual void SetMetric(int id, int new_val) = 0;
    virtual void SetFont(int id, const wxFont& font) = 0;
    virtual wxFont GetFont(int id) = 0;
    virtual wxColour GetColour(int id) = 0;
    virtual void SetColour(int id, const wxColor& colour) = 0;
    wxColour GetColor(int id) { return GetColour(id); }
    void SetColor(int id, const wxColour& color) { SetColour(id, color); }

    virtual void DrawSash(wxDC& dc,
                          wxWindow* window,
                          int orientation,
                          const wxRect& rect) = 0;

    virtual void DrawBackground(wxDC& dc,
                          wxWindow* window,
                          int orientation,
                          const wxRect& rect) = 0;

    virtual void DrawCaption(wxDC& dc,
                          wxWindow* window,
                          const wxString& text,
                          const wxRect& rect,
                          wxAuiPaneInfo& pane) = 0;

    virtual void DrawGripper(wxDC& dc,
                          wxWindow* window,
                          const wxRect& rect,
                          wxAuiPaneInfo& pane) = 0;

    virtual void DrawBorder(wxDC& dc,
                          wxWindow* window,
                          const wxRect& rect,
                          wxAuiPaneInfo& pane) = 0;

    virtual void DrawPaneButton(wxDC& dc,
                          wxWindow* window,
                          int button,
                          int button_state,
                          const wxRect& rect,
                          wxAuiPaneInfo& pane) = 0;
};


// this is the default art provider for wxAuiManager.  Dock art
// can be customized by creating a class derived from this one,
// or replacing this class entirely

class WXDLLIMPEXP_AUI wxAuiDefaultDockArt : public wxAuiDockArt
{
public:

    wxAuiDefaultDockArt();

    int GetMetric(int metric_id);
    void SetMetric(int metric_id, int new_val);
    wxColour GetColour(int id);
    void SetColour(int id, const wxColor& colour);
    void SetFont(int id, const wxFont& font);
    wxFont GetFont(int id);

    void DrawSash(wxDC& dc,
                  wxWindow *window,
                  int orientation,
                  const wxRect& rect);

    void DrawBackground(wxDC& dc,
                  wxWindow *window,
                  int orientation,
                  const wxRect& rect);

    void DrawCaption(wxDC& dc,
                  wxWindow *window,
                  const wxString& text,
                  const wxRect& rect,
                  wxAuiPaneInfo& pane);

    void DrawGripper(wxDC& dc,
                  wxWindow *window,
                  const wxRect& rect,
                  wxAuiPaneInfo& pane);

    void DrawBorder(wxDC& dc,
                  wxWindow *window,
                  const wxRect& rect,
                  wxAuiPaneInfo& pane);

    void DrawPaneButton(wxDC& dc,
                  wxWindow *window,
                  int button,
                  int button_state,
                  const wxRect& rect,
                  wxAuiPaneInfo& pane);

protected:

    void DrawCaptionBackground(wxDC& dc, const wxRect& rect, bool active);

protected:

    wxPen m_border_pen;
    wxBrush m_sash_brush;
    wxBrush m_background_brush;
    wxBrush m_gripper_brush;
    wxFont m_caption_font;
    wxBitmap m_inactive_close_bitmap;
    wxBitmap m_inactive_pin_bitmap;
    wxBitmap m_inactive_maximize_bitmap;
    wxBitmap m_inactive_restore_bitmap;
    wxBitmap m_active_close_bitmap;
    wxBitmap m_active_pin_bitmap;
    wxBitmap m_active_maximize_bitmap;
    wxBitmap m_active_restore_bitmap;
    wxPen m_gripper_pen1;
    wxPen m_gripper_pen2;
    wxPen m_gripper_pen3;
    wxColour m_base_colour;
    wxColour m_active_caption_colour;
    wxColour m_active_caption_gradient_colour;
    wxColour m_active_caption_text_colour;
    wxColour m_inactive_caption_colour;
    wxColour m_inactive_caption_gradient_colour;
    wxColour m_inactive_caption_text_colour;
    int m_border_size;
    int m_caption_size;
    int m_sash_size;
    int m_button_size;
    int m_gripper_size;
    int m_gradient_type;
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
                         const wxAuiPaneInfo& pane,
                         const wxRect& in_rect,
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
                         wxRect* out_rect) = 0;

    virtual wxSize GetTabSize(
                         wxDC& dc,
                         wxWindow* wnd,
                         const wxString& caption,
                         const wxBitmap& bitmap,
                         bool active,
                         int close_button_state,
                         int* x_extent) = 0;

    virtual int ShowDropDown(
                         wxWindow* wnd,
                         const wxAuiPaneInfoPtrArray& items,
                         int active_idx) = 0;

    virtual int GetIndentSize() = 0;

    virtual int GetBestTabCtrlSize(
                         wxWindow* wnd,
                         const wxAuiPaneInfoPtrArray& pages,
                         const wxSize& required_bmp_size) = 0;
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
                 const wxAuiPaneInfo& pane,
                 const wxRect& in_rect,
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
                 wxRect* out_rect);

    int GetIndentSize();

    wxSize GetTabSize(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxString& caption,
                 const wxBitmap& bitmap,
                 bool active,
                 int close_button_state,
                 int* x_extent);

    int ShowDropDown(
                 wxWindow* wnd,
                 const wxAuiPaneInfoPtrArray& items,
                 int active_idx);

    int GetBestTabCtrlSize(wxWindow* wnd,
                 const wxAuiPaneInfoPtrArray& pages,
                 const wxSize& required_bmp_size);

protected:

    wxFont m_normal_font;
    wxFont m_selected_font;
    wxFont m_measuring_font;
    wxColour m_base_colour;
    wxPen m_base_colour_pen;
    wxPen m_border_pen;
    wxBrush m_base_colour_brush;
    wxBitmap m_active_close_bmp;
    wxBitmap m_disabled_close_bmp;
    wxBitmap m_active_left_bmp;
    wxBitmap m_disabled_left_bmp;
    wxBitmap m_active_right_bmp;
    wxBitmap m_disabled_right_bmp;
    wxBitmap m_active_windowlist_bmp;
    wxBitmap m_disabled_windowlist_bmp;

    int m_fixed_tab_width;
    int m_tab_ctrl_height;
    unsigned int m_flags;
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
WX_DECLARE_USER_EXPORTED_OBJARRAY(wxAuiTabContainerButton, wxAuiTabContainerButtonArray, WXDLLIMPEXP_AUI);
#endif

class WXDLLIMPEXP_AUI wxAuiTabContainer
{
public:

    wxAuiTabContainer();
    virtual ~wxAuiTabContainer();

    void SetArtProvider(wxAuiTabArt* art);
    wxAuiTabArt* GetArtProvider() const;

    void SetFlags(unsigned int flags);
    unsigned int GetFlags() const;

    bool AddPage(wxAuiPaneInfo& info);
    bool InsertPage(wxWindow* page, wxAuiPaneInfo& info, size_t idx);
    bool MovePage(wxWindow* page, size_t new_idx);
    bool RemovePage(wxWindow* page);
    bool SetActivePage(wxWindow* page);
    bool SetActivePage(size_t page);
    void SetNoneActive();
    int GetActivePage() const;
    bool TabHitTest(int x, int y, wxAuiPaneInfo** hit) const;
    bool ButtonHitTest(int x, int y, wxAuiTabContainerButton** hit) const;
    wxWindow* GetWindowFromIdx(size_t idx) const;
    int GetIdxFromWindow(wxWindow* page) const;
    size_t GetPageCount() const;
    wxAuiPaneInfo& GetPage(size_t idx);
    const wxAuiPaneInfo& GetPage(size_t idx) const;
    wxAuiPaneInfoPtrArray& GetPages();
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

    // Is the tab visible?
    bool IsTabVisible(int tabPage, int tabOffset, wxDC* dc, wxWindow* wnd);

    // Make the tab visible if it wasn't already
    void MakeTabVisible(int tabPage);

    void DrawTabs(wxDC* dc, wxWindow* wnd,const wxRect& rect);
protected:

    virtual void Render(wxDC* dc, wxWindow* wnd);
    
    void CalculateRequiredWidth(wxDC& dc,wxWindow* wnd,int& total_width,int& visible_width) const;

protected:

    wxAuiTabArt* m_art;
    wxAuiPaneInfoPtrArray m_pages;
    wxAuiTabContainerButtonArray m_buttons;
    wxAuiTabContainerButtonArray m_tab_close_buttons;
    wxRect m_rect;
    wxRect m_target_rect;
    size_t m_tab_offset;
    unsigned int m_flags;
};

#endif // wxUSE_AUI
#endif //_WX_DOCKART_H_
