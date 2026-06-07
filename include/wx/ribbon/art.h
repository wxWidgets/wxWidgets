///////////////////////////////////////////////////////////////////////////////
// Name:        wx/ribbon/art.h
// Purpose:     Art providers for ribbon-bar-style interface
// Author:      Peter Cawley
// Created:     2009-05-25
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RIBBON_ART_H_
#define _WX_RIBBON_ART_H_

#include "wx/defs.h"

#if wxUSE_RIBBON

#include "wx/brush.h"
#include "wx/colour.h"
#include "wx/font.h"
#include "wx/pen.h"
#include "wx/bitmap.h"
#include "wx/bmpbndl.h"
#include "wx/ribbon/bar.h"

class WXDLLIMPEXP_FWD_CORE wxDC;
class WXDLLIMPEXP_FWD_CORE wxReadOnlyDC;
class WXDLLIMPEXP_FWD_CORE wxWindow;

enum wxRibbonArtSetting
{
    wxRIBBON_ART_TAB_SEPARATION_SIZE,
    wxRIBBON_ART_PAGE_BORDER_LEFT_SIZE,
    wxRIBBON_ART_PAGE_BORDER_TOP_SIZE,
    wxRIBBON_ART_PAGE_BORDER_RIGHT_SIZE,
    wxRIBBON_ART_PAGE_BORDER_BOTTOM_SIZE,
    wxRIBBON_ART_PANEL_X_SEPARATION_SIZE,
    wxRIBBON_ART_PANEL_Y_SEPARATION_SIZE,
    wxRIBBON_ART_TOOL_GROUP_SEPARATION_SIZE,
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
    wxRIBBON_ART_BUTTON_BAR_ACTIVE_BORDER_COLOUR,
    wxRIBBON_ART_BUTTON_BAR_ACTIVE_BACKGROUND_TOP_COLOUR,
    wxRIBBON_ART_BUTTON_BAR_ACTIVE_BACKGROUND_TOP_GRADIENT_COLOUR,
    wxRIBBON_ART_BUTTON_BAR_ACTIVE_BACKGROUND_COLOUR,
    wxRIBBON_ART_BUTTON_BAR_ACTIVE_BACKGROUND_GRADIENT_COLOUR,
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
    wxRIBBON_ART_TAB_ACTIVE_LABEL_COLOUR,
    wxRIBBON_ART_TAB_HOVER_LABEL_COLOUR,
    wxRIBBON_ART_TAB_SEPARATOR_COLOUR,
    wxRIBBON_ART_TAB_SEPARATOR_GRADIENT_COLOUR,
    wxRIBBON_ART_TAB_CTRL_BACKGROUND_COLOUR,
    wxRIBBON_ART_TAB_CTRL_BACKGROUND_GRADIENT_COLOUR,
    wxRIBBON_ART_TAB_HOVER_BACKGROUND_TOP_COLOUR,
    wxRIBBON_ART_TAB_HOVER_BACKGROUND_TOP_GRADIENT_COLOUR,
    wxRIBBON_ART_TAB_HOVER_BACKGROUND_COLOUR,
    wxRIBBON_ART_TAB_HOVER_BACKGROUND_GRADIENT_COLOUR,
    wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_TOP_COLOUR,
    wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_TOP_GRADIENT_COLOUR,
    wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_COLOUR,
    wxRIBBON_ART_TAB_ACTIVE_BACKGROUND_GRADIENT_COLOUR,
    wxRIBBON_ART_TAB_BORDER_COLOUR,
    wxRIBBON_ART_PANEL_BORDER_COLOUR,
    wxRIBBON_ART_PANEL_BORDER_GRADIENT_COLOUR,
    wxRIBBON_ART_PANEL_HOVER_BORDER_COLOUR,
    wxRIBBON_ART_PANEL_HOVER_BORDER_GRADIENT_COLOUR,
    wxRIBBON_ART_PANEL_MINIMISED_BORDER_COLOUR,
    wxRIBBON_ART_PANEL_MINIMISED_BORDER_GRADIENT_COLOUR,
    wxRIBBON_ART_PANEL_LABEL_BACKGROUND_COLOUR,
    wxRIBBON_ART_PANEL_LABEL_BACKGROUND_GRADIENT_COLOUR,
    wxRIBBON_ART_PANEL_LABEL_COLOUR,
    wxRIBBON_ART_PANEL_HOVER_LABEL_BACKGROUND_COLOUR,
    wxRIBBON_ART_PANEL_HOVER_LABEL_BACKGROUND_GRADIENT_COLOUR,
    wxRIBBON_ART_PANEL_HOVER_LABEL_COLOUR,
    wxRIBBON_ART_PANEL_MINIMISED_LABEL_COLOUR,
    wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_TOP_COLOUR,
    wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_TOP_GRADIENT_COLOUR,
    wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_COLOUR,
    wxRIBBON_ART_PANEL_ACTIVE_BACKGROUND_GRADIENT_COLOUR,
    wxRIBBON_ART_PANEL_BUTTON_FACE_COLOUR,
    wxRIBBON_ART_PANEL_BUTTON_HOVER_FACE_COLOUR,

    wxRIBBON_ART_PAGE_TOGGLE_FACE_COLOUR,
    wxRIBBON_ART_PAGE_TOGGLE_HOVER_FACE_COLOUR,

    wxRIBBON_ART_PAGE_BORDER_COLOUR,
    wxRIBBON_ART_PAGE_BACKGROUND_TOP_COLOUR,
    wxRIBBON_ART_PAGE_BACKGROUND_TOP_GRADIENT_COLOUR,
    wxRIBBON_ART_PAGE_BACKGROUND_COLOUR,
    wxRIBBON_ART_PAGE_BACKGROUND_GRADIENT_COLOUR,
    wxRIBBON_ART_PAGE_HOVER_BACKGROUND_TOP_COLOUR,
    wxRIBBON_ART_PAGE_HOVER_BACKGROUND_TOP_GRADIENT_COLOUR,
    wxRIBBON_ART_PAGE_HOVER_BACKGROUND_COLOUR,
    wxRIBBON_ART_PAGE_HOVER_BACKGROUND_GRADIENT_COLOUR,
    wxRIBBON_ART_TOOLBAR_BORDER_COLOUR,
    wxRIBBON_ART_TOOLBAR_HOVER_BORDER_COLOUR,
    wxRIBBON_ART_TOOLBAR_FACE_COLOUR,
    wxRIBBON_ART_TOOL_BACKGROUND_TOP_COLOUR,
    wxRIBBON_ART_TOOL_BACKGROUND_TOP_GRADIENT_COLOUR,
    wxRIBBON_ART_TOOL_BACKGROUND_COLOUR,
    wxRIBBON_ART_TOOL_BACKGROUND_GRADIENT_COLOUR,
    wxRIBBON_ART_TOOL_HOVER_BACKGROUND_TOP_COLOUR,
    wxRIBBON_ART_TOOL_HOVER_BACKGROUND_TOP_GRADIENT_COLOUR,
    wxRIBBON_ART_TOOL_HOVER_BACKGROUND_COLOUR,
    wxRIBBON_ART_TOOL_HOVER_BACKGROUND_GRADIENT_COLOUR,
    wxRIBBON_ART_TOOL_ACTIVE_BACKGROUND_TOP_COLOUR,
    wxRIBBON_ART_TOOL_ACTIVE_BACKGROUND_TOP_GRADIENT_COLOUR,
    wxRIBBON_ART_TOOL_ACTIVE_BACKGROUND_COLOUR,
    wxRIBBON_ART_TOOL_ACTIVE_BACKGROUND_GRADIENT_COLOUR,
    wxRIBBON_ART_BUTTON_BAR_LABEL_DISABLED_COLOUR,
    wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_COLOUR,
    wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_GRADIENT_COLOUR,
    wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_TOP_COLOUR,
    wxRIBBON_ART_BUTTON_BAR_LABEL_HIGHLIGHT_GRADIENT_TOP_COLOUR
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

    wxRIBBON_SCROLL_BTN_FOR_MASK = 48
};

enum wxRibbonButtonKind
{
    wxRIBBON_BUTTON_NORMAL    = 1 << 0,
    wxRIBBON_BUTTON_DROPDOWN  = 1 << 1,
    wxRIBBON_BUTTON_HYBRID    = wxRIBBON_BUTTON_NORMAL | wxRIBBON_BUTTON_DROPDOWN,
    wxRIBBON_BUTTON_TOGGLE    = 1 << 2
};

enum wxRibbonButtonBarButtonState
{
    wxRIBBON_BUTTONBAR_BUTTON_SMALL     = 0 << 0,
    wxRIBBON_BUTTONBAR_BUTTON_MEDIUM    = 1 << 0,
    wxRIBBON_BUTTONBAR_BUTTON_LARGE     = 2 << 0,
    wxRIBBON_BUTTONBAR_BUTTON_SIZE_MASK = 3 << 0,

    wxRIBBON_BUTTONBAR_BUTTON_NORMAL_HOVERED    = 1 << 3,
    wxRIBBON_BUTTONBAR_BUTTON_DROPDOWN_HOVERED  = 1 << 4,
    wxRIBBON_BUTTONBAR_BUTTON_HOVER_MASK        = wxRIBBON_BUTTONBAR_BUTTON_NORMAL_HOVERED | wxRIBBON_BUTTONBAR_BUTTON_DROPDOWN_HOVERED,
    wxRIBBON_BUTTONBAR_BUTTON_NORMAL_ACTIVE     = 1 << 5,
    wxRIBBON_BUTTONBAR_BUTTON_DROPDOWN_ACTIVE   = 1 << 6,
    wxRIBBON_BUTTONBAR_BUTTON_ACTIVE_MASK       = wxRIBBON_BUTTONBAR_BUTTON_NORMAL_ACTIVE | wxRIBBON_BUTTONBAR_BUTTON_DROPDOWN_ACTIVE,
    wxRIBBON_BUTTONBAR_BUTTON_DISABLED          = 1 << 7,
    wxRIBBON_BUTTONBAR_BUTTON_TOGGLED           = 1 << 8,
    wxRIBBON_BUTTONBAR_BUTTON_STATE_MASK        = 0x1F8
};

enum wxRibbonGalleryButtonState
{
    wxRIBBON_GALLERY_BUTTON_NORMAL,
    wxRIBBON_GALLERY_BUTTON_HOVERED,
    wxRIBBON_GALLERY_BUTTON_ACTIVE,
    wxRIBBON_GALLERY_BUTTON_DISABLED
};

class wxRibbonBar;
class wxRibbonPage;
class wxRibbonPanel;
class wxRibbonGallery;
class wxRibbonGalleryItem;
class wxRibbonPageTabInfo;
class wxRibbonPageTabInfoArray;

class WXDLLIMPEXP_RIBBON wxRibbonArtProvider
{
public:
    wxRibbonArtProvider();
    virtual ~wxRibbonArtProvider();

    wxNODISCARD virtual wxRibbonArtProvider* Clone() const = 0;
    virtual void SetFlags(long flags) = 0;
    virtual long GetFlags() const = 0;

    virtual int GetMetric(int id) const = 0;
    virtual void SetMetric(int id, int newVal) = 0;
    virtual void SetFont(int id, const wxFont& font) = 0;
    virtual wxFont GetFont(int id) const = 0;
    virtual wxColour GetColour(int id) const = 0;
    virtual void SetColour(int id, const wxColor& colour) = 0;
    wxColour GetColor(int id) const { return GetColour(id); }
    void SetColor(int id, const wxColour& color) { SetColour(id, color); }
    virtual void GetColourScheme(wxColour* primary,
                        wxColour* secondary,
                        wxColour* tertiary) const = 0;
    virtual void SetColourScheme(const wxColour& primary,
                        const wxColour& secondary,
                        const wxColour& tertiary) = 0;

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
                        wxRibbonButtonKind kind,
                        long state,
                        const wxString& label,
                        const wxBitmap& bitmapLarge,
                        const wxBitmap& bitmapSmall) = 0;

    virtual void DrawToolBarBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect) = 0;

    virtual void DrawToolGroupBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect) = 0;

    virtual void DrawTool(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        const wxBitmap& bitmap,
                        wxRibbonButtonKind kind,
                        long state) = 0;

    virtual void DrawToggleButton(
                        wxDC& dc,
                        wxRibbonBar* wnd,
                        const wxRect& rect,
                        wxRibbonDisplayMode mode) = 0;

    virtual void DrawHelpButton(
                        wxDC& dc,
                        wxRibbonBar* wnd,
                        const wxRect& rect) = 0;

    virtual void GetBarTabWidth(
                        wxReadOnlyDC& dc,
                        wxWindow* wnd,
                        const wxString& label,
                        const wxBitmap& bitmap,
                        int* ideal,
                        int* smallBeginNeedSeparator,
                        int* smallMustHaveSeparator,
                        int* minimum) = 0;

    virtual int GetTabCtrlHeight(
                        wxReadOnlyDC& dc,
                        wxWindow* wnd,
                        const wxRibbonPageTabInfoArray& pages) = 0;

    virtual wxSize GetScrollButtonMinimumSize(
                        wxReadOnlyDC& dc,
                        wxWindow* wnd,
                        long style) = 0;

    virtual wxSize GetPanelSize(
                        wxReadOnlyDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize clientSize,
                        wxPoint* clientOffset) = 0;

    virtual wxSize GetPanelClientSize(
                        wxReadOnlyDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize size,
                        wxPoint* clientOffset) = 0;

    virtual wxRect GetPanelExtButtonArea(
                        wxReadOnlyDC& dc,
                        const wxRibbonPanel* wnd,
                        wxRect rect) = 0;

    virtual wxSize GetGallerySize(
                        wxReadOnlyDC& dc,
                        const wxRibbonGallery* wnd,
                        wxSize clientSize) = 0;

    virtual wxSize GetGalleryClientSize(
                        wxReadOnlyDC& dc,
                        const wxRibbonGallery* wnd,
                        wxSize size,
                        wxPoint* clientOffset,
                        wxRect* scrollUpButton,
                        wxRect* scrollDownButton,
                        wxRect* extensionButton) = 0;

    virtual wxRect GetPageBackgroundRedrawArea(
                        wxReadOnlyDC& dc,
                        const wxRibbonPage* wnd,
                        wxSize pageOldSize,
                        wxSize pageNewSize) = 0;

    virtual bool GetButtonBarButtonSize(
                        wxReadOnlyDC& dc,
                        wxWindow* wnd,
                        wxRibbonButtonKind kind,
                        wxRibbonButtonBarButtonState size,
                        const wxString& label,
                        wxCoord textMinWidth,
                        wxSize bitmapSizeLarge,
                        wxSize bitmapSizeSmall,
                        wxSize* buttonSize,
                        wxRect* normalRegion,
                        wxRect* dropdownRegion) = 0;

    virtual wxCoord GetButtonBarButtonTextWidth(
                        wxReadOnlyDC& dc, const wxString& label,
                        wxRibbonButtonKind kind,
                        wxRibbonButtonBarButtonState size) = 0;

    virtual wxSize GetMinimisedPanelMinimumSize(
                        wxReadOnlyDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize* desiredBitmapSize,
                        wxDirection* expandedPanelDirection) = 0;

    virtual wxSize GetToolSize(
                        wxReadOnlyDC& dc,
                        wxWindow* wnd,
                        wxSize bitmapSize,
                        wxRibbonButtonKind kind,
                        bool isFirst,
                        bool isLast,
                        wxRect* dropdownRegion) = 0;

    virtual wxRect GetBarToggleButtonArea(const wxRect& rect)= 0;

    virtual wxRect GetRibbonHelpButtonArea(const wxRect& rect) = 0;
};

class WXDLLIMPEXP_RIBBON wxRibbonMSWArtProvider : public wxRibbonArtProvider
{
public:
    wxRibbonMSWArtProvider(bool set_colour_scheme = true);
    virtual ~wxRibbonMSWArtProvider();

    wxNODISCARD wxRibbonArtProvider* Clone() const override;
    void SetFlags(long flags) override;
    long GetFlags() const override;

    int GetMetric(int id) const override;
    void SetMetric(int id, int newVal) override;
    void SetFont(int id, const wxFont& font) override;
    wxFont GetFont(int id) const override;
    wxColour GetColour(int id) const override;
    void SetColour(int id, const wxColor& colour) override;
    void GetColourScheme(wxColour* primary,
                         wxColour* secondary,
                         wxColour* tertiary) const override;
    void SetColourScheme(const wxColour& primary,
                         const wxColour& secondary,
                         const wxColour& tertiary) override;

    int GetTabCtrlHeight(
                        wxReadOnlyDC& dc,
                        wxWindow* wnd,
                        const wxRibbonPageTabInfoArray& pages) override;

    void DrawTabCtrlBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect) override;

    void DrawTab(wxDC& dc,
                 wxWindow* wnd,
                 const wxRibbonPageTabInfo& tab) override;

    void DrawTabSeparator(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        double visibility) override;

    void DrawPageBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect) override;

    void DrawScrollButton(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        long style) override;

    void DrawPanelBackground(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect) override;

    void DrawGalleryBackground(
                        wxDC& dc,
                        wxRibbonGallery* wnd,
                        const wxRect& rect) override;

    void DrawGalleryItemBackground(
                        wxDC& dc,
                        wxRibbonGallery* wnd,
                        const wxRect& rect,
                        wxRibbonGalleryItem* item) override;

    void DrawMinimisedPanel(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect,
                        wxBitmap& bitmap) override;

    void DrawButtonBarBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect) override;

    void DrawButtonBarButton(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        wxRibbonButtonKind kind,
                        long state,
                        const wxString& label,
                        const wxBitmap& bitmapLarge,
                        const wxBitmap& bitmapSmall) override;

    void DrawToolBarBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect) override;

    void DrawToolGroupBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect) override;

    void DrawTool(
                wxDC& dc,
                wxWindow* wnd,
                const wxRect& rect,
                const wxBitmap& bitmap,
                wxRibbonButtonKind kind,
                long state) override;

    void DrawToggleButton(
                        wxDC& dc,
                        wxRibbonBar* wnd,
                        const wxRect& rect,
                        wxRibbonDisplayMode mode) override;

    void DrawHelpButton(wxDC& dc,
                        wxRibbonBar* wnd,
                        const wxRect& rect) override;

    void GetBarTabWidth(
                        wxReadOnlyDC& dc,
                        wxWindow* wnd,
                        const wxString& label,
                        const wxBitmap& bitmap,
                        int* ideal,
                        int* smallBeginNeedSeparator,
                        int* smallMustHaveSeparator,
                        int* minimum) override;

    wxSize GetScrollButtonMinimumSize(
                        wxReadOnlyDC& dc,
                        wxWindow* wnd,
                        long style) override;

    wxSize GetPanelSize(
                        wxReadOnlyDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize clientSize,
                        wxPoint* clientOffset) override;

    wxSize GetPanelClientSize(
                        wxReadOnlyDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize size,
                        wxPoint* clientOffset) override;

    wxRect GetPanelExtButtonArea(
                        wxReadOnlyDC& dc,
                        const wxRibbonPanel* wnd,
                        wxRect rect) override;

    wxSize GetGallerySize(
                        wxReadOnlyDC& dc,
                        const wxRibbonGallery* wnd,
                        wxSize clientSize) override;

    wxSize GetGalleryClientSize(
                        wxReadOnlyDC& dc,
                        const wxRibbonGallery* wnd,
                        wxSize size,
                        wxPoint* clientOffset,
                        wxRect* scrollUpButton,
                        wxRect* scrollDownButton,
                        wxRect* extensionButton) override;

    wxRect GetPageBackgroundRedrawArea(
                        wxReadOnlyDC& dc,
                        const wxRibbonPage* wnd,
                        wxSize pageOldSize,
                        wxSize pageNewSize) override;

    bool GetButtonBarButtonSize(
                        wxReadOnlyDC& dc,
                        wxWindow* wnd,
                        wxRibbonButtonKind kind,
                        wxRibbonButtonBarButtonState size,
                        const wxString& label,
                        wxCoord textMinWidth,
                        wxSize bitmapSizeLarge,
                        wxSize bitmapSizeSmall,
                        wxSize* buttonSize,
                        wxRect* normalRegion,
                        wxRect* dropdownRegion) override;

    wxCoord GetButtonBarButtonTextWidth(
                        wxReadOnlyDC& dc, const wxString& label,
                        wxRibbonButtonKind kind,
                        wxRibbonButtonBarButtonState size) override;

    wxSize GetMinimisedPanelMinimumSize(
                        wxReadOnlyDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize* desiredBitmapSize,
                        wxDirection* expandedPanelDirection) override;

    wxSize GetToolSize(
                        wxReadOnlyDC& dc,
                        wxWindow* wnd,
                        wxSize bitmapSize,
                        wxRibbonButtonKind kind,
                        bool isFirst,
                        bool isLast,
                        wxRect* dropdownRegion) override;

    wxRect GetBarToggleButtonArea(const wxRect& rect) override;

    wxRect GetRibbonHelpButtonArea(const wxRect& rect) override;

protected:
    void ReallyDrawTabSeparator(wxWindow* wnd, const wxRect& rect, double visibility);
    void DrawPartialPageBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect,
        bool allowHovered = true);
    void DrawPartialPageBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect,
         wxRibbonPage* page, wxPoint offset, bool hovered = false);
    void DrawPanelBorder(wxDC& dc, const wxRect& rect, wxPen& primaryColour,
        wxPen& secondaryColour);
    void RemovePanelPadding(wxRect* rect);
    void DrawDropdownArrow(wxDC& dc, int x, int y, const wxColour& colour);
    void DrawGalleryBackgroundCommon(wxDC& dc, wxRibbonGallery* wnd,
                        const wxRect& rect);
    virtual void DrawGalleryButton(wxDC& dc, wxRect rect,
        wxRibbonGalleryButtonState state, wxBitmapBundle* bundles, wxWindow* wnd);
    void DrawButtonBarButtonForeground(
                        wxDC& dc,
                        const wxRect& rect,
                        wxRibbonButtonKind kind,
                        long state,
                        const wxString& label,
                        const wxBitmap& bitmapLarge,
                        const wxBitmap& bitmapSmall);
    void DrawMinimisedPanelCommon(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect,
                        wxRect* previewRect);
    void CloneTo(wxRibbonMSWArtProvider* copy) const;

    wxBitmap m_cachedTabSeparator;
    wxBitmapBundle m_galleryUpBundle[4];
    wxBitmapBundle m_galleryDownBundle[4];
    wxBitmapBundle m_galleryExtensionBundle[4];
    wxBitmapBundle m_toolbarDropBundle;
    wxBitmapBundle m_panelExtensionBundle[2];
    wxBitmapBundle m_ribbonToggleUpBundle[2];
    wxBitmapBundle m_ribbonToggleDownBundle[2];
    wxBitmapBundle m_ribbonTogglePinBundle[2];
    wxBitmapBundle m_ribbonBarHelpButtonBundle[2];

    wxColour m_primarySchemeColour;
    wxColour m_secondarySchemeColour;
    wxColour m_tertiarySchemeColour;

    wxColour m_buttonBarLabelColour;
    wxColour m_buttonBarLabelDisabledColour;
    wxColour m_tabLabelColour;
    wxColour m_tabActiveLabelColour;
    wxColour m_tabHoverLabelColour;
    wxColour m_tabSeparatorColour;
    wxColour m_tabSeparatorGradientColour;
    wxColour m_tabActiveBackgroundColour;
    wxColour m_tabActiveBackgroundGradientColour;
    wxColour m_tabHoverBackgroundColour;
    wxColour m_tabHoverBackgroundGradientColour;
    wxColour m_tabHoverBackgroundTopColour;
    wxColour m_tabHoverBackgroundTopGradientColour;
    wxColour m_tabHighlightTopColour;
    wxColour m_tabHighlightTopGradientColour;
    wxColour m_tabHighlightColour;
    wxColour m_tabHighlightGradientColour;
    wxColour m_panelLabelColour;
    wxColour m_panelMinimisedLabelColour;
    wxColour m_panelHoverLabelColour;
    wxColour m_panelActiveBackgroundColour;
    wxColour m_panelActiveBackgroundGradientColour;
    wxColour m_panelActiveBackgroundTopColour;
    wxColour m_panelActiveBackgroundTopGradientColour;
    wxColour m_panelButtonFaceColour;
    wxColour m_panelButtonHoverFaceColour;
    wxColour m_pageToggleFaceColour;
    wxColour m_pageToggleHoverFaceColour;
    wxColour m_pageBackgroundColour;
    wxColour m_pageBackgroundGradientColour;
    wxColour m_pageBackgroundTopColour;
    wxColour m_pageBackgroundTopGradientColour;
    wxColour m_pageHoverBackgroundColour;
    wxColour m_pageHoverBackgroundGradientColour;
    wxColour m_pageHoverBackgroundTopColour;
    wxColour m_pageHoverBackgroundTopGradientColour;
    wxColour m_buttonBarHoverBackgroundColour;
    wxColour m_buttonBarHoverBackgroundGradientColour;
    wxColour m_buttonBarHoverBackgroundTopColour;
    wxColour m_buttonBarHoverBackgroundTopGradientColour;
    wxColour m_buttonBarActiveBackgroundColour;
    wxColour m_buttonBarActiveBackgroundGradientColour;
    wxColour m_buttonBarActiveBackgroundTopColour;
    wxColour m_buttonBarActiveBackgroundTopGradientColour;
    wxColour m_galleryButtonBackgroundColour;
    wxColour m_galleryButtonBackgroundGradientColour;
    wxColour m_galleryButtonHoverBackgroundColour;
    wxColour m_galleryButtonHoverBackgroundGradientColour;
    wxColour m_galleryButtonActiveBackgroundColour;
    wxColour m_galleryButtonActiveBackgroundGradientColour;
    wxColour m_galleryButtonDisabledBackgroundColour;
    wxColour m_galleryButtonDisabledBackgroundGradientColour;
    wxColour m_galleryButtonFaceColour;
    wxColour m_galleryButtonHoverFaceColour;
    wxColour m_galleryButtonActiveFaceColour;
    wxColour m_galleryButtonDisabledFaceColour;

    wxColour m_toolFaceColour;
    wxColour m_toolBackgroundTopColour;
    wxColour m_toolBackgroundTopGradientColour;
    wxColour m_toolBackgroundColour;
    wxColour m_toolBackgroundGradientColour;
    wxColour m_toolHoverBackgroundTopColour;
    wxColour m_toolHoverBackgroundTopGradientColour;
    wxColour m_toolHoverBackgroundColour;
    wxColour m_toolHoverBackgroundGradientColour;
    wxColour m_toolActiveBackgroundTopColour;
    wxColour m_toolActiveBackgroundTopGradientColour;
    wxColour m_toolActiveBackgroundColour;
    wxColour m_toolActiveBackgroundGradientColour;

    wxBrush m_tabCtrlBackgroundBrush;
    wxBrush m_panelLabelBackgroundBrush;
    wxBrush m_panelHoverLabelBackgroundBrush;
    wxBrush m_panelHoverButtonBackgroundBrush;
    wxBrush m_galleryHoverBackgroundBrush;
    wxBrush m_galleryButtonBackgroundTopBrush;
    wxBrush m_galleryButtonHoverBackgroundTopBrush;
    wxBrush m_galleryButtonActiveBackgroundTopBrush;
    wxBrush m_galleryButtonDisabledBackgroundTopBrush;
    wxBrush m_ribbonToggleBrush;

    wxFont m_tabLabelFont;
    wxFont m_panelLabelFont;
    wxFont m_buttonBarLabelFont;

    wxPen m_pageBorderPen;
    wxPen m_panelBorderPen;
    wxPen m_panelBorderGradientPen;
    wxPen m_panelHoverBorderPen;
    wxPen m_panelHoverBorderGradientPen;
    wxPen m_panelMinimisedBorderPen;
    wxPen m_panelMinimisedBorderGradientPen;
    wxPen m_panelHoverButtonBorderPen;
    wxPen m_tabBorderPen;
    wxPen m_buttonBarHoverBorderPen;
    wxPen m_buttonBarActiveBorderPen;
    wxPen m_galleryBorderPen;
    wxPen m_galleryItemBorderPen;
    wxPen m_toolbarBorderPen;
    wxPen m_ribbonTogglePen;

    double m_cachedTabSeparatorVisibility;
    long m_flags;

    int m_tabSeparationSize;
    int m_pageBorderLeft;
    int m_pageBorderTop;
    int m_pageBorderRight;
    int m_pageBorderBottom;
    int m_panelXSeparationSize;
    int m_panelYSeparationSize;
    int m_toolGroupSeparationSize;
    int m_galleryBitmapPaddingLeftSize;
    int m_galleryBitmapPaddingRightSize;
    int m_galleryBitmapPaddingTopSize;
    int m_galleryBitmapPaddingBottomSize;
    int m_toggleButtonOffset;
    int m_helpButtonOffset;
};

class WXDLLIMPEXP_RIBBON wxRibbonMSWFlatArtProvider : public wxRibbonMSWArtProvider
{
public:
    wxRibbonMSWFlatArtProvider();
    virtual ~wxRibbonMSWFlatArtProvider();

    wxNODISCARD wxRibbonArtProvider* Clone() const override;

    void DrawTab(wxDC& dc,
                 wxWindow* wnd,
                 const wxRibbonPageTabInfo& tab) override;

    void DrawTabSeparator(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        double visibility) override;

    void DrawPageBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect) override;

    void DrawScrollButton(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        long style) override;

    void DrawPanelBackground(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect) override;

    void DrawGalleryBackground(
                        wxDC& dc,
                        wxRibbonGallery* wnd,
                        const wxRect& rect) override;

    void DrawGalleryItemBackground(
                        wxDC& dc,
                        wxRibbonGallery* wnd,
                        const wxRect& rect,
                        wxRibbonGalleryItem* item) override;

    void DrawMinimisedPanel(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect,
                        wxBitmap& bitmap) override;

    void DrawButtonBarBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect) override;

    void DrawButtonBarButton(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        wxRibbonButtonKind kind,
                        long state,
                        const wxString& label,
                        const wxBitmap& bitmapLarge,
                        const wxBitmap& bitmapSmall) override;

    void DrawToolBarBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect) override;

    void DrawTool(
                wxDC& dc,
                wxWindow* wnd,
                const wxRect& rect,
                const wxBitmap& bitmap,
                wxRibbonButtonKind kind,
                long state) override;

    void DrawToggleButton(
                        wxDC& dc,
                        wxRibbonBar* wnd,
                        const wxRect& rect,
                        wxRibbonDisplayMode mode) override;

    void DrawHelpButton(wxDC& dc,
                        wxRibbonBar* wnd,
                        const wxRect& rect) override;

protected:
    void DrawPartialPageBackground(wxDC& dc, wxWindow* wnd, const wxRect& r,
        wxRibbonPage* page, wxPoint offset, bool hovered = false);
    void DrawPartialPageBackground(wxDC& dc, wxWindow* wnd,
        const wxRect& rect, bool allowHovered = true);
    void DrawGalleryButton(wxDC& dc, wxRect rect,
        wxRibbonGalleryButtonState state, wxBitmapBundle* bundles, wxWindow* wnd) override;
    void DrawPanelBorder(wxDC& dc, const wxRect& rect, wxPen& primaryColour,
        wxPen& secondaryColour);
    void ReallyDrawTabSeparator(wxWindow* wnd, const wxRect& rect,
        double visibility);
};

class WXDLLIMPEXP_RIBBON wxRibbonAUIArtProvider : public wxRibbonMSWArtProvider
{
public:
    wxRibbonAUIArtProvider();
    virtual ~wxRibbonAUIArtProvider();

    wxNODISCARD wxRibbonArtProvider* Clone() const override;

    wxColour GetColour(int id) const override;
    void SetColour(int id, const wxColor& colour) override;
    void SetColourScheme(const wxColour& primary,
                         const wxColour& secondary,
                         const wxColour& tertiary) override;
    void SetFont(int id, const wxFont& font) override;

    wxSize GetScrollButtonMinimumSize(
                        wxReadOnlyDC& dc,
                        wxWindow* wnd,
                        long style) override;

    void DrawScrollButton(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        long style) override;

    wxSize GetPanelSize(
                        wxReadOnlyDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize clientSize,
                        wxPoint* clientOffset) override;

    wxSize GetPanelClientSize(
                        wxReadOnlyDC& dc,
                        const wxRibbonPanel* wnd,
                        wxSize size,
                        wxPoint* clientOffset) override;

    wxRect GetPanelExtButtonArea(
                        wxReadOnlyDC& dc,
                        const wxRibbonPanel* wnd,
                        wxRect rect) override;

    void DrawTabCtrlBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect) override;

    int GetTabCtrlHeight(
                        wxReadOnlyDC& dc,
                        wxWindow* wnd,
                        const wxRibbonPageTabInfoArray& pages) override;

    void GetBarTabWidth(
                        wxReadOnlyDC& dc,
                        wxWindow* wnd,
                        const wxString& label,
                        const wxBitmap& bitmap,
                        int* ideal,
                        int* smallBeginNeedSeparator,
                        int* smallMustHaveSeparator,
                        int* minimum) override;

    void DrawTab(wxDC& dc,
                 wxWindow* wnd,
                 const wxRibbonPageTabInfo& tab) override;

    void DrawTabSeparator(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        double visibility) override;

    void DrawPageBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect) override;

    void DrawPanelBackground(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect) override;

    void DrawMinimisedPanel(
                        wxDC& dc,
                        wxRibbonPanel* wnd,
                        const wxRect& rect,
                        wxBitmap& bitmap) override;

    void DrawGalleryBackground(
                        wxDC& dc,
                        wxRibbonGallery* wnd,
                        const wxRect& rect) override;

    void DrawGalleryItemBackground(
                        wxDC& dc,
                        wxRibbonGallery* wnd,
                        const wxRect& rect,
                        wxRibbonGalleryItem* item) override;

    void DrawButtonBarBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect) override;

    void DrawButtonBarButton(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect,
                        wxRibbonButtonKind kind,
                        long state,
                        const wxString& label,
                        const wxBitmap& bitmapLarge,
                        const wxBitmap& bitmapSmall) override;

    void DrawToolBarBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect) override;

    void DrawToolGroupBackground(
                        wxDC& dc,
                        wxWindow* wnd,
                        const wxRect& rect) override;

    void DrawTool(
                wxDC& dc,
                wxWindow* wnd,
                const wxRect& rect,
                const wxBitmap& bitmap,
                wxRibbonButtonKind kind,
                long state) override;

protected:
    void DrawPartialPanelBackground(wxDC& dc, wxWindow* wnd,
        const wxRect& rect);
    void DrawGalleryButton(wxDC& dc, wxRect rect,
        wxRibbonGalleryButtonState state, wxBitmapBundle* bundles, wxWindow* wnd) override;

    wxColour m_tabCtrlBackgroundColour;
    wxColour m_tabCtrlBackgroundGradientColour;
    wxColour m_panelLabelBackgroundColour;
    wxColour m_panelLabelBackgroundGradientColour;
    wxColour m_panelHoverLabelBackgroundColour;
    wxColour m_panelHoverLabelBackgroundGradientColour;

    wxBrush m_backgroundBrush;
    wxBrush m_tabActiveTopBackgroundBrush;
    wxBrush m_tabHoverBackgroundBrush;
    wxBrush m_buttonBarHoverBackgroundBrush;
    wxBrush m_buttonBarActiveBackgroundBrush;
    wxBrush m_galleryButtonActiveBackgroundBrush;
    wxBrush m_galleryButtonHoverBackgroundBrush;
    wxBrush m_galleryButtonDisabledBackgroundBrush;
    wxBrush m_toolHoverBackgroundBrush;
    wxBrush m_toolActiveBackgroundBrush;

    wxPen m_toolbarHoverBordenPen;

    wxFont m_tabActiveLabelFont;
};

#if defined(__WXMSW__)
typedef wxRibbonMSWArtProvider wxRibbonDefaultArtProvider;
#elif defined(__WXOSX_COCOA__) || \
      defined(__WXOSX_IPHONE__)
// TODO: Once implemented, change typedef to OSX
// typedef wxRibbonOSXArtProvider wxRibbonDefaultArtProvider;
typedef wxRibbonAUIArtProvider wxRibbonDefaultArtProvider;
#else
// TODO: Once implemented, change typedef to AUI
typedef wxRibbonAUIArtProvider wxRibbonDefaultArtProvider;
#endif

#endif // wxUSE_RIBBON

#endif // _WX_RIBBON_ART_H_
