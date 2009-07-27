///////////////////////////////////////////////////////////////////////////////
// Name:        wx/ribbon/art.h
// Purpose:     Art providers for ribbon-bar-style interface
// Author:      Peter Cawley
// Modified by:
// Created:     2009-05-25
// RCS-ID:      $Id$
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RIBBON_ART_H_
#define _WX_RIBBON_ART_H_

#include "wx/defs.h"

#if wxUSE_RIBBON

#include <wx/brush.h>
#include <wx/pen.h>

enum wxRibbonArtSetting
{
    wxRIBBON_ART_TAB_SEPARATION_SIZE,
    wxRIBBON_ART_PAGE_BORDER_LEFT_SIZE,
    wxRIBBON_ART_PAGE_BORDER_TOP_SIZE,
    wxRIBBON_ART_PAGE_BORDER_RIGHT_SIZE,
    wxRIBBON_ART_PAGE_BORDER_BOTTOM_SIZE,
    wxRIBBON_ART_PANEL_X_SEPARATION_SIZE,
    wxRIBBON_ART_PANEL_Y_SEPARATION_SIZE,
    wxRIBBON_ART_GALLERY_BITMAP_PADDING_LEFT_SIZE,
    wxRIBBON_ART_GALLERY_BITMAP_PADDING_RIGHT_SIZE,
    wxRIBBON_ART_GALLERY_BITMAP_PADDING_TOP_SIZE,
    wxRIBBON_ART_GALLERY_BITMAP_PADDING_BOTTOM_SIZE,
    wxRIBBON_ART_PANEL_LABEL_FONT,
    wxRIBBON_ART_BUTTON_BAR_LABEL_FONT,
    wxRIBBON_ART_TAB_LABEL_FONT,
    wxRIBBON_ART_BUTTON_BAR_LABEL_COLOUR,
    wxRIBBON_ART_BUTTON_BAR_HOVER_BORDER_COLOUR,
    wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_TOP_COLOUR,
    wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_TOP_GRADIENT_COLOUR,
    wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_COLOUR,
    wxRIBBON_ART_BUTTON_BAR_HOVER_BACKGROUND_GRADIENT_COLOUR,
    wxRIBBON_ART_GALLERY_BORDER_COLOUR,
    wxRIBBON_ART_GALLERY_HOVER_BACKGROUND_COLOUR,
    wxRIBBON_ART_GALLERY_BUTTON_BACKGROUND_COLOUR,
    wxRIBBON_ART_GALLERY_BUTTON_BACKGROUND_GRADIENT_COLOUR,
    wxRIBBON_ART_GALLERY_BUTTON_BACKGROUND_TOP_COLOUR,
    wxRIBBON_ART_GALLERY_BUTTON_FACE_COLOUR,
    wxRIBBON_ART_GALLERY_BUTTON_HOVER_BACKGROUND_COLOUR,
    wxRIBBON_ART_GALLERY_BUTTON_HOVER_BACKGROUND_GRADIENT_COLOUR,
    wxRIBBON_ART_GALLERY_BUTTON_HOVER_BACKGROUND_TOP_COLOUR,
    wxRIBBON_ART_GALLERY_BUTTON_HOVER_FACE_COLOUR,
    wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_BACKGROUND_COLOUR,
    wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_BACKGROUND_GRADIENT_COLOUR,
    wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_BACKGROUND_TOP_COLOUR,
    wxRIBBON_ART_GALLERY_BUTTON_ACTIVE_FACE_COLOUR,
    wxRIBBON_ART_GALLERY_BUTTON_DISABLED_BACKGROUND_COLOUR,
    wxRIBBON_ART_GALLERY_BUTTON_DISABLED_BACKGROUND_GRADIENT_COLOUR,
    wxRIBBON_ART_GALLERY_BUTTON_DISABLED_BACKGROUND_TOP_COLOUR,
    wxRIBBON_ART_GALLERY_BUTTON_DISABLED_FACE_COLOUR,
    wxRIBBON_ART_GALLERY_ITEM_BORDER_COLOUR,
    wxRIBBON_ART_TAB_LABEL_COLOUR,
    wxRIBBON_ART_TAB_SEPARATOR_COLOUR,
    wxRIBBON_ART_TAB_SEPARATOR_GRADIENT_COLOUR,
    wxRIBBON_ART_TAB_CTRL_BACKGROUND_COLOUR,
    wxRIBBON_ART_TAB_HOVER_BACKGROUND_TOP_COLOUR,
    wxRIBBON_ART_TAB_HOVER_BACKGROUND_TOP_GRADIENT_COLOUR,
    wxRIBBON_ART_TAB_HOVER_BACKGROUND_COLOUR,
    wxRIBBON_ART_TAB_HOVER_BACKGROUND_GRADIENT_COLOUR,
    wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_COLOUR,
    wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_GRADIENT_COLOUR,
    wxRIBBON_ART_TAB_BORDER_COLOUR,
    wxRIBBON_ART_PANEL_BORDER_COLOUR,
    wxRIBBON_ART_PANEL_BORDER_GRADIENT_COLOUR,
    wxRIBBON_ART_PANEL_MINIMISED_BORDER_COLOUR,
    wxRIBBON_ART_PANEL_MINIMISED_BORDER_GRADIENT_COLOUR,
    wxRIBBON_ART_PANEL_LABEL_BACKGROUND_COLOUR,
    wxRIBBON_ART_PANEL_LABEL_COLOUR,
    wxRIBBON_ART_PANEL_HOVER_LABEL_BACKGROUND_COLOUR,
    wxRIBBON_ART_PANEL_HOVER_LABEL_COLOUR,
    wxRIBBON_ART_PANEL_MINIMISED_LABEL_COLOUR,
    wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_TOP_COLOUR,
    wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_TOP_GRADIENT_COLOUR,
    wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_COLOUR,
    wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_GRADIENT_COLOUR,
    wxRIBBON_ART_PAGE_BORDER_COLOUR,
    wxRIBBON_ART_PAGE_BACKGROUND_TOP_COLOUR,
    wxRIBBON_ART_PAGE_BACKGROUND_TOP_GRADIENT_COLOUR,
    wxRIBBON_ART_PAGE_BACKGROUND_COLOUR,
    wxRIBBON_ART_PAGE_BACKGROUND_GRADIENT_COLOUR,
    wxRIBBON_ART_PAGE_HOVER_BACKGROUND_TOP_COLOUR,
    wxRIBBON_ART_PAGE_HOVER_BACKGROUND_TOP_GRADIENT_COLOUR,
    wxRIBBON_ART_PAGE_HOVER_BACKGROUND_COLOUR,
    wxRIBBON_ART_PAGE_HOVER_BACKGROUND_GRADIENT_COLOUR,
};

enum wxRibbonScrollButtonStyle
{
    wxRIBBON_SCROLL_BTN_LEFT = 0,
    wxRIBBON_SCROLL_BTN_RIGHT = 1,
    wxRIBBON_SCROLL_BTN_UP = 2,
    wxRIBBON_SCROLL_BTN_DOWN = 3,

    wxRIBBON_SCROLL_BTN_DIRECTION_MASK = 3,

    wxRIBBON_SCROLL_BTN_NORMAL = 0,
    wxRIBBON_SCROLL_BTN_HOVERED = 4,
    wxRIBBON_SCROLL_BTN_ACTIVE = 8,

    wxRIBBON_SCROLL_BTN_STATE_MASK = 12,

    wxRIBBON_SCROLL_BTN_FOR_OTHER = 0,
    wxRIBBON_SCROLL_BTN_FOR_TABS = 16,
    wxRIBBON_SCROLL_BTN_FOR_PAGE = 32,

    wxRIBBON_SCROLL_BTN_FOR_MASK = 48,
};

enum wxRibbonGalleryButtonState
{
    wxRIBBON_GALLERY_BUTTON_NORMAL,
    wxRIBBON_GALLERY_BUTTON_HOVERED,
    wxRIBBON_GALLERY_BUTTON_ACTIVE,
    wxRIBBON_GALLERY_BUTTON_DISABLED,
};

class wxRibbonPage;
class wxRibbonPanel;
class wxRibbonGallery;
class wxRibbonGalleryItem;
class wxRibbonPageTabInfo;
class wxRibbonPageTabInfoArray;
enum wxRibbonButtonBarButtonKind;
enum wxRibbonButtonBarButtonState;

class WXDLLIMPEXP_RIBBON wxRibbonArtProvider
{
public:
    wxRibbonArtProvider();
    virtual ~wxRibbonArtProvider();

    virtual wxRibbonArtProvider* Clone() = 0;
    virtual void SetFlags(long flags) = 0;
    virtual long GetFlags() = 0;

    virtual int GetMetric(int id) = 0;
    virtual void SetMetric(int id, int new_val) = 0;
    virtual void SetFont(int id, const wxFont& font) = 0;
    virtual wxFont GetFont(int id) = 0;
    virtual wxColour GetColour(int id) = 0;
    virtual void SetColour(int id, const wxColor& colour) = 0;
    wxColour GetColor(int id) { return GetColour(id); }
    void SetColor(int id, const wxColour& color) { SetColour(id, color); }

    virtual void DrawTabCtrlBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect) = 0;

    virtual void DrawTab(wxDC& dc,
                        wxWindow* wnd,
                        const wxRibbonPageTabInfo& tab) = 0;

    virtual void DrawTabSeparator(wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        double visibility) = 0;

    virtual void DrawPageBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect) = 0;

    virtual void DrawScrollButton(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        long style) = 0;

    virtual void DrawPanelBackground(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect) = 0;

    virtual void DrawGalleryBackground(
                        wxDC& dc,
                        wxRibbonGallery* wnd,
                        const wxRect& rect) = 0;

    virtual void DrawGalleryItemBackground(
                        wxDC& dc,
                        wxRibbonGallery* wnd,
                        const wxRect& rect,
                        wxRibbonGalleryItem* item) = 0;

    virtual void DrawMinimisedPanel(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect,
                        wxBitmap& bitmap) = 0;

    virtual void DrawButtonBarBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect) = 0;

    virtual void DrawButtonBarButton(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        wxRibbonButtonBarButtonKind kind,
                        long state,
                        const wxString& label,
                        const wxBitmap& bitmap_large,
                        const wxBitmap& bitmap_small) = 0;

    virtual void GetBarTabWidth(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxString& label,
                        const wxBitmap& bitmap,
                        int* ideal,
                        int* small_begin_need_separator,
                        int* small_must_have_separator,
                        int* minimum) = 0;

    virtual int GetTabCtrlHeight(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRibbonPageTabInfoArray& pages) = 0;

    virtual wxSize GetScrollButtonMinimumSize(
                        wxDC& dc,
                        wxWindow* wnd,
                        long style) = 0;

    virtual wxSize GetPanelSize(
                        wxDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize client_size,
                        wxPoint* client_offset) = 0;

    virtual wxSize GetPanelClientSize(
                        wxDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize size,
                        wxPoint* client_offset) = 0;

    virtual wxSize GetGallerySize(
                        wxDC& dc,
                        const wxRibbonGallery* wnd,
                        wxSize client_size) = 0;

    virtual wxSize GetGalleryClientSize(
                        wxDC& dc,
                        const wxRibbonGallery* wnd,
                        wxSize size,
                        wxPoint* client_offset,
                        wxRect* scroll_up_button,
                        wxRect* scroll_down_button,
                        wxRect* extension_button) = 0;

    virtual wxRect GetPageBackgroundRedrawArea(
                        wxDC& dc,
                        const wxRibbonPage* wnd,
                        wxSize page_old_size,
                        wxSize page_new_size) = 0;

    virtual bool GetButtonBarButtonSize(
                        wxDC& dc,
                        wxWindow* wnd,
                        wxRibbonButtonBarButtonKind kind,
                        wxRibbonButtonBarButtonState size,
                        const wxString& label,
                        wxSize bitmap_size_large,
                        wxSize bitmap_size_small,
                        wxSize* button_size,
                        wxRect* normal_region,
                        wxRect* dropdown_region) = 0;

    virtual wxSize GetMinimisedPanelMinimumSize(
                        wxDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize* desired_bitmap_size,
                        wxDirection* expanded_panel_direction) = 0;
};

class WXDLLIMPEXP_RIBBON wxRibbonMSWArtProvider : public wxRibbonArtProvider
{
public:
    wxRibbonMSWArtProvider();
    virtual ~wxRibbonMSWArtProvider();

    wxRibbonArtProvider* Clone();
    void SetFlags(long flags);
    long GetFlags();

    int GetMetric(int id);
    void SetMetric(int id, int new_val);
    void SetFont(int id, const wxFont& font);
    wxFont GetFont(int id);
    wxColour GetColour(int id);
    void SetColour(int id, const wxColor& colour);

    void DrawTabCtrlBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect);

    void DrawTab(wxDC& dc,
                 wxWindow* wnd,
                 const wxRibbonPageTabInfo& tab);

    void DrawTabSeparator(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        double visibility);

    void DrawPageBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect);

    void DrawScrollButton(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        long style);

    void DrawPanelBackground(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect);

    void DrawGalleryBackground(
                        wxDC& dc,
                        wxRibbonGallery* wnd,
                        const wxRect& rect);

    void DrawGalleryItemBackground(
                        wxDC& dc,
                        wxRibbonGallery* wnd,
                        const wxRect& rect,
                        wxRibbonGalleryItem* item);

    void DrawMinimisedPanel(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect,
                        wxBitmap& bitmap);

    void DrawButtonBarBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect);

    void DrawButtonBarButton(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        wxRibbonButtonBarButtonKind kind,
                        long state,
                        const wxString& label,
                        const wxBitmap& bitmap_large,
                        const wxBitmap& bitmap_small);

    void GetBarTabWidth(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxString& label,
                        const wxBitmap& bitmap,
                        int* ideal,
                        int* small_begin_need_separator,
                        int* small_must_have_separator,
                        int* minimum);

    int GetTabCtrlHeight(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRibbonPageTabInfoArray& pages);

    wxSize GetScrollButtonMinimumSize(
                        wxDC& dc,
                        wxWindow* wnd,
                        long style);

    wxSize GetPanelSize(
                        wxDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize client_size,
                        wxPoint* client_offset);

    wxSize GetPanelClientSize(
                        wxDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize size,
                        wxPoint* client_offset);

    wxSize GetGallerySize(
                        wxDC& dc,
                        const wxRibbonGallery* wnd,
                        wxSize client_size);

    wxSize GetGalleryClientSize(
                        wxDC& dc,
                        const wxRibbonGallery* wnd,
                        wxSize size,
                        wxPoint* client_offset,
                        wxRect* scroll_up_button,
                        wxRect* scroll_down_button,
                        wxRect* extension_button);

    wxRect GetPageBackgroundRedrawArea(
                        wxDC& dc,
                        const wxRibbonPage* wnd,
                        wxSize page_old_size,
                        wxSize page_new_size);

    bool GetButtonBarButtonSize(
                        wxDC& dc,
                        wxWindow* wnd,
                        wxRibbonButtonBarButtonKind kind,
                        wxRibbonButtonBarButtonState size,
                        const wxString& label,
                        wxSize bitmap_size_large,
                        wxSize bitmap_size_small,
                        wxSize* button_size,
                        wxRect* normal_region,
                        wxRect* dropdown_region);

    wxSize GetMinimisedPanelMinimumSize(
                        wxDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize* desired_bitmap_size,
                        wxDirection* expanded_panel_direction);

protected:
    void ReallyDrawTabSeparator(wxWindow* wnd, const wxRect& rect, double visibility);
    void DrawPartialPageBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect,
        bool allow_hovered = true);
    void DrawPartialPageBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect,
         wxRibbonPage* page, wxPoint offset, bool hovered = false);
    void DrawPanelBorder(wxDC& dc, const wxRect& rect, wxPen& primary_colour,
        wxPen& secondary_colour);
    void RemovePanelPadding(wxRect* rect);
    void DrawDropdownArrow(wxDC& dc, int x, int y, const wxColour& colour);
    void DrawGalleryButton(wxDC& dc, wxRect rect,
        wxRibbonGalleryButtonState state, wxBitmap* bitmaps);

    wxBitmap m_cached_tab_separator;
    wxBitmap m_gallery_up_bitmap[4];
    wxBitmap m_gallery_down_bitmap[4];
    wxBitmap m_gallery_extension_bitmap[4];

    wxColour m_button_bar_label_colour;
    wxColour m_tab_label_colour;
    wxColour m_tab_separator_colour;
    wxColour m_tab_separator_gradient_colour;
    wxColour m_tab_active_background_colour;
    wxColour m_tab_active_background_gradient_colour;
    wxColour m_tab_hover_background_colour;
    wxColour m_tab_hover_background_gradient_colour;
    wxColour m_tab_hover_background_top_colour;
    wxColour m_tab_hover_background_top_gradient_colour;
    wxColour m_panel_label_colour;
    wxColour m_panel_minimised_label_colour;
    wxColour m_panel_hover_label_colour;
    wxColour m_panel_active_background_colour;
    wxColour m_panel_active_background_gradient_colour;
    wxColour m_panel_active_background_top_colour;
    wxColour m_panel_active_background_top_gradient_colour;
    wxColour m_page_background_colour;
    wxColour m_page_background_gradient_colour;
    wxColour m_page_background_top_colour;
    wxColour m_page_background_top_gradient_colour;
    wxColour m_page_hover_background_colour;
    wxColour m_page_hover_background_gradient_colour;
    wxColour m_page_hover_background_top_colour;
    wxColour m_page_hover_background_top_gradient_colour;
    wxColour m_button_bar_hover_background_colour;
    wxColour m_button_bar_hover_background_gradient_colour;
    wxColour m_button_bar_hover_background_top_colour;
    wxColour m_button_bar_hover_background_top_gradient_colour;
    wxColour m_gallery_button_background_colour;
    wxColour m_gallery_button_background_gradient_colour;    
    wxColour m_gallery_button_hover_background_colour;
    wxColour m_gallery_button_hover_background_gradient_colour;
    wxColour m_gallery_button_active_background_colour;
    wxColour m_gallery_button_active_background_gradient_colour;
    wxColour m_gallery_button_disabled_background_colour;
    wxColour m_gallery_button_disabled_background_gradient_colour;
    wxColour m_gallery_button_face_colour;
    wxColour m_gallery_button_hover_face_colour;
    wxColour m_gallery_button_active_face_colour;
    wxColour m_gallery_button_disabled_face_colour;

    wxBrush m_tab_ctrl_background_brush;
    wxBrush m_panel_label_background_brush;
    wxBrush m_panel_hover_label_background_brush;
    wxBrush m_gallery_hover_background_brush;
    wxBrush m_gallery_button_background_top_brush;
    wxBrush m_gallery_button_hover_background_top_brush;
    wxBrush m_gallery_button_active_background_top_brush;
    wxBrush m_gallery_button_disabled_background_top_brush;

    wxFont m_tab_label_font;
    wxFont m_panel_label_font;
    wxFont m_button_bar_label_font;
    wxPen m_page_border_pen;
    wxPen m_panel_border_pen;
    wxPen m_panel_border_gradient_pen;
    wxPen m_panel_minimised_border_pen;
    wxPen m_panel_minimised_border_gradient_pen;
    wxPen m_tab_border_pen;
    wxPen m_button_bar_hover_border_pen;
    wxPen m_gallery_border_pen;
    wxPen m_gallery_item_border_pen;

    double m_cached_tab_separator_visibility;
    long m_flags;

    int m_tab_separation_size;
    int m_page_border_left;
    int m_page_border_top;
    int m_page_border_right;
    int m_page_border_bottom;
    int m_panel_x_separation_size;
    int m_panel_y_separation_size;
    int m_gallery_bitmap_padding_left_size;
    int m_gallery_bitmap_padding_right_size;
    int m_gallery_bitmap_padding_top_size;
    int m_gallery_bitmap_padding_bottom_size;
};

#if defined(__WXMSW__)
typedef wxRibbonMSWArtProvider wxRibbonDefaultArtProvider;
#elif defined(__WXOSX_CARBON__) || \
      defined(__WXOSX_COCOA__) || \
      defined(__WXOSX_IPHONE__) || \
      defined(__WXCOCOA__)
// TODO: Once implemented, change typedef to OSX
// typedef wxRibbonOSXArtProvider wxRibbonDefaultArtProvider;
typedef wxRibbonMSWArtProvider wxRibbonDefaultArtProvider;
#else
// TODO: Once implemented, change typedef to AUI
// typedef wxRibbonAUIArtProvider wxRibbonDefaultArtProvider;
typedef wxRibbonMSWArtProvider wxRibbonDefaultArtProvider;
#endif

#endif // wxUSE_RIBBON

#endif // _WX_RIBBON_ART_H_
