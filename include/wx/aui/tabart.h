//////////////////////////////////////////////////////////////////////////////
// Name:        wx/aui/tabart.h
// Purpose:     wxaui: wx advanced user interface - notebook
// Author:      Benjamin I. Williams
// Modified by: Jens Lody (extracted from wx/aui/auibook.h)
// Created:     2012-03-21
// Copyright:   (C) Copyright 2006, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////


#ifndef _WX_AUI_TABART_H_
#define _WX_AUI_TABART_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/defs.h"

#if wxUSE_AUI

#include "wx/colour.h"
#include "wx/gdicmn.h"
#include "wx/font.h"
#include "wx/pen.h"
#include "wx/brush.h"
#include "wx/bmpbndl.h"

#include "wx/aui/framemanager.h" // wxAuiPaneButtonState and wxAuiButtonId

#include <vector>

class wxAuiNotebookPage;
class wxAuiNotebookPageArray;
class wxAuiTabContainerButton;
class wxWindow;
class wxDC;
class wxReadOnlyDC;


// tab art class

class WXDLLIMPEXP_AUI wxAuiTabArt
{
public:

    wxAuiTabArt() = default;
    virtual ~wxAuiTabArt() = default;

    wxNODISCARD virtual wxAuiTabArt* Clone() = 0;
    virtual void SetFlags(unsigned int flags) = 0;

    virtual void SetSizingInfo(const wxSize& tabCtrlSize,
                               size_t tabCount,
                               wxWindow* wnd = nullptr) = 0;

    virtual void SetNormalFont(const wxFont& font) = 0;
    virtual void SetSelectedFont(const wxFont& font) = 0;
    virtual void SetMeasuringFont(const wxFont& font) = 0;
    virtual void SetColour(const wxColour& colour) = 0;
    virtual void SetActiveColour(const wxColour& colour) = 0;

    // These functions should be overridden in the derived class to return the
    // actually used fonts, but they're not pure virtual for compatibility
    // reasons.
    virtual wxFont GetNormalFont() const { return wxFont{}; }
    virtual wxFont GetSelectedFont() const { return wxFont{}; }

    virtual void DrawBorder(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& rect) = 0;

    virtual void DrawBackground(
                         wxDC& dc,
                         wxWindow* wnd,
                         const wxRect& rect) = 0;

    // This function is not pure virtual for compatibility: if the derived
    // class implements DrawTab(), then its default implementation is
    // sufficient as long as pinned tabs are not used, but it must be
    // overridden if the program does use them and it should be overridden
    // instead of DrawTab() in the new code.
    virtual int DrawPageTab(
                         wxDC& dc,
                         wxWindow* wnd,
                         wxAuiNotebookPage& page,
                         const wxRect& rect);

    // Override DrawPageTab() in the new code rather than this one.
    virtual void DrawTab(wxDC& dc,
                         wxWindow* wnd,
                         const wxAuiNotebookPage& pane,
                         const wxRect& inRect,
                         int closeButtonState,
                         wxRect* outTabRect,
                         wxRect* outButtonRect,
                         int* xExtent);

    virtual void DrawButton(
                         wxDC& dc,
                         wxWindow* wnd,
                         const wxRect& inRect,
                         int bitmapId,
                         int buttonState,
                         int orientation,
                         wxRect* outRect) = 0;

    // This function relationship with GetTabSize() is similar as for DrawTab()
    // and DrawPageTab(): this one should be overridden when pinned tabs are
    // used, but doesn't have to be if they are not and GetTabSize() itself is
    // overridden for compatibility with the existing code.
    //
    // It also allows to omit "xExtent" parameter if it is not needed.
    virtual wxSize GetPageTabSize(
                         wxReadOnlyDC& dc,
                         wxWindow* wnd,
                         const wxAuiNotebookPage& page,
                         int* xExtent = nullptr);

    virtual wxSize GetTabSize(
                         wxReadOnlyDC& dc,
                         wxWindow* wnd,
                         const wxString& caption,
                         const wxBitmapBundle& bitmap,
                         bool active,
                         int closeButtonState,
                         int* xExtent);

    // This function is not pure virtual because it is only for multi-line
    // tabs, but it must be implemented if wxAUI_NB_MULTILINE is used.
    //
    // If specified, the returned rectangle must be filled with the same value
    // as DrawButton() puts into its "outRect" but here it can also be null in
    // which case just its width is returned.
    virtual int GetButtonRect(
                         wxReadOnlyDC& dc,
                         wxWindow* wnd,
                         const wxRect& inRect,
                         int bitmapId,
                         int buttonState,
                         int orientation,
                         wxRect* outRect = nullptr) /* = 0 */;

    virtual int ShowDropDown(
                         wxWindow* wnd,
                         const wxAuiNotebookPageArray& items,
                         int activeIdx) = 0;

    virtual int GetIndentSize() = 0;

    virtual int GetBorderWidth(
                         wxWindow* wnd) = 0;

    virtual int GetAdditionalBorderSpace(
                         wxWindow* wnd) = 0;

    virtual int GetBestTabCtrlSize(
                         wxWindow* wnd,
                         const wxAuiNotebookPageArray& pages,
                         const wxSize& requiredBmpSize) = 0;

    // Provide opportunity for subclasses to recalculate colours
    virtual void UpdateColoursFromSystem() {}
    virtual void UpdateDpi() {}
};


// Base, still abstract, class for the concrete tab art classes below.
class WXDLLIMPEXP_AUI wxAuiTabArtBase : public wxAuiTabArt
{
public:
    void SetFlags(unsigned int flags) override;

    void SetSizingInfo(const wxSize& tabCtrlSize,
                       size_t tabCount,
                       wxWindow* wnd = nullptr) override;

    void SetNormalFont(const wxFont& font) override;
    void SetSelectedFont(const wxFont& font) override;
    void SetMeasuringFont(const wxFont& font) override;

    wxFont GetNormalFont() const override;
    wxFont GetSelectedFont() const override;

    int GetButtonRect(
                 wxReadOnlyDC& dc,
                 wxWindow* wnd,
                 const wxRect& inRect,
                 int bitmapId,
                 int buttonState,
                 int orientation,
                 wxRect* outRect) override;

    void DrawButton(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& inRect,
                 int bitmapId,
                 int buttonState,
                 int orientation,
                 wxRect* outRect) override;

    int ShowDropDown(
                 wxWindow* wnd,
                 const wxAuiNotebookPageArray& items,
                 int activeIdx) override;

    int GetBorderWidth(
                 wxWindow* wnd) override;

    int GetAdditionalBorderSpace(
                 wxWindow* wnd) override;

    void DrawBorder(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& rect) override;

protected:
    // Ctor is protected, this class is only used as a base class.
    //
    // Remember to call InitBitmaps() after setting up the colours in the
    // derived class ctor.
    wxAuiTabArtBase();

    // Initialize the bitmaps using the colours returned by GetButtonColour().
    void InitBitmaps();

    // Return pointer to our bitmap bundle corresponding to the button ID and
    // state or null if we don't support this button or if it is hidden.
    const wxBitmapBundle*
    GetButtonBitmapBundle(const wxAuiTabContainerButton& button) const;

    // Helper function for DrawButton() and GetButtonRect().
    bool DoGetButtonRectAndBitmap(
                 wxWindow* wnd,
                 const wxRect& inRect,
                 int bitmapId,
                 int buttonState,
                 int orientation,
                 wxRect* outRect,
                 wxBitmap* outBitmap = nullptr);


    // Note: all these fields are protected for compatibility reasons, but
    // shouldn't be accessed directly.
    wxFont m_normalFont;
    wxFont m_selectedFont;
    wxFont m_measuringFont;

    wxBitmapBundle m_activeCloseBmp;
    wxBitmapBundle m_disabledCloseBmp;
    wxBitmapBundle m_activeLeftBmp;
    wxBitmapBundle m_disabledLeftBmp;
    wxBitmapBundle m_activeRightBmp;
    wxBitmapBundle m_disabledRightBmp;
    wxBitmapBundle m_activeWindowListBmp;
    wxBitmapBundle m_disabledWindowListBmp;
    wxBitmapBundle m_activePinBmp;
    wxBitmapBundle m_disabledPinBmp;
    wxBitmapBundle m_activeUnpinBmp;
    wxBitmapBundle m_disabledUnpinBmp;

    int m_fixedTabWidth;
    int m_tabCtrlHeight; // Unused, kept only for compatibility.
    unsigned int m_flags = 0;

private:
    // This is called by InitBitmaps().
    //
    // The state parameter is currently always either wxAUI_BUTTON_STATE_NORMAL
    // or wxAUI_BUTTON_STATE_DISABLED, but the function could be called with
    // other values in the future.
    virtual wxColour
    GetButtonColour(wxAuiButtonId button, wxAuiPaneButtonState state) const = 0;

    // This is called by DrawButton().
    //
    // By default just draws the bitmap using wxDC::DrawBitmap().
    virtual void
    DrawButtonBitmap(wxDC& dc,
                     const wxRect& rect,
                     const wxBitmap& bmp,
                     int buttonState);
};

// This tab art provider draws flat tabs with a thin border.
class WXDLLIMPEXP_AUI wxAuiFlatTabArt : public wxAuiTabArtBase
{
public:
    wxAuiFlatTabArt();
    virtual ~wxAuiFlatTabArt();

    // Objects of this class are supposed to be used polymorphically, so
    // copying them is not allowed, use Clone() instead.
    wxAuiFlatTabArt(const wxAuiFlatTabArt&) = delete;
    wxAuiFlatTabArt& operator=(const wxAuiFlatTabArt&) = delete;

    wxNODISCARD wxAuiTabArt* Clone() override;

    void SetColour(const wxColour& colour) override;
    void SetActiveColour(const wxColour& colour) override;

    void DrawBackground(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& rect) override;

    int DrawPageTab(
                 wxDC& dc,
                 wxWindow* wnd,
                 wxAuiNotebookPage& page,
                 const wxRect& rect) override;

    int GetIndentSize() override;

    wxSize GetPageTabSize(
                 wxReadOnlyDC& dc,
                 wxWindow* wnd,
                 const wxAuiNotebookPage& page,
                 int* xExtent = nullptr) override;

    int GetBestTabCtrlSize(wxWindow* wnd,
                 const wxAuiNotebookPageArray& pages,
                 const wxSize& requiredBmpSize) override;

    void UpdateColoursFromSystem() override;

private:
    // Private pseudo-copy ctor used by Clone().
    explicit wxAuiFlatTabArt(wxAuiFlatTabArt* other);

    virtual wxColour
    GetButtonColour(wxAuiButtonId button,
                    wxAuiPaneButtonState state) const override;

    struct Data;
    Data* const m_data;
};


class WXDLLIMPEXP_AUI wxAuiGenericTabArt : public wxAuiTabArtBase
{

public:

    wxAuiGenericTabArt();

    wxNODISCARD wxAuiTabArt* Clone() override;

    void SetColour(const wxColour& colour) override;
    void SetActiveColour(const wxColour& colour) override;

    void DrawBackground(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& rect) override;

    int DrawPageTab(
                 wxDC& dc,
                 wxWindow* wnd,
                 wxAuiNotebookPage& page,
                 const wxRect& rect) override;

    int GetIndentSize() override;

    wxSize GetPageTabSize(
                 wxReadOnlyDC& dc,
                 wxWindow* wnd,
                 const wxAuiNotebookPage& page,
                 int* xExtent = nullptr) override;

    int GetBestTabCtrlSize(wxWindow* wnd,
                 const wxAuiNotebookPageArray& pages,
                 const wxSize& requiredBmpSize) override;

    // Provide opportunity for subclasses to recalculate colours
    virtual void UpdateColoursFromSystem() override;

protected:

    wxColour m_baseColour;
    wxPen m_baseColourPen;
    wxPen m_borderPen;
    wxBrush m_baseColourBrush;
    wxColour m_activeColour;

private:
    // Called from ctor and UpdateColoursFromSystem().
    void InitColours();

    virtual wxColour
    GetButtonColour(wxAuiButtonId button,
                    wxAuiPaneButtonState state) const override;
};


class WXDLLIMPEXP_AUI wxAuiSimpleTabArt : public wxAuiTabArtBase
{

public:

    wxAuiSimpleTabArt();

    wxNODISCARD wxAuiTabArt* Clone() override;

    void SetColour(const wxColour& colour) override;
    void SetActiveColour(const wxColour& colour) override;

    void DrawBackground(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& rect) override;

    void DrawTab(wxDC& dc,
                 wxWindow* wnd,
                 const wxAuiNotebookPage& pane,
                 const wxRect& inRect,
                 int closeButtonState,
                 wxRect* outTabRect,
                 wxRect* outButtonRect,
                 int* xExtent) override;

    int GetIndentSize() override;

    wxSize GetTabSize(
                 wxReadOnlyDC& dc,
                 wxWindow* wnd,
                 const wxString& caption,
                 const wxBitmapBundle& bitmap,
                 bool active,
                 int closeButtonState,
                 int* xExtent) override;

    int GetBestTabCtrlSize(wxWindow* wnd,
                 const wxAuiNotebookPageArray& pages,
                 const wxSize& requiredBmpSize) override;

protected:

    wxPen m_normalBkPen;
    wxPen m_selectedBkPen;
    wxBrush m_normalBkBrush;
    wxBrush m_selectedBkBrush;
    wxBrush m_bkBrush;

private:
    virtual wxColour
    GetButtonColour(wxAuiButtonId button,
                    wxAuiPaneButtonState state) const override;

    virtual void
    DrawButtonBitmap(wxDC& dc,
                     const wxRect& rect,
                     const wxBitmap& bmp,
                     int buttonState) override;
};

#ifndef __WXUNIVERSAL__
    #if defined(__WXGTK__) && !defined(__WXGTK3__)
        #define wxHAS_NATIVE_TABART
        #include "wx/aui/tabartgtk.h"
        using wxAuiNativeTabArt = wxAuiGtkTabArt;
    #elif defined(__WXMSW__) && wxUSE_UXTHEME
        #define wxHAS_NATIVE_TABART
        #include "wx/aui/tabartmsw.h"
        using wxAuiNativeTabArt = wxAuiMSWTabArt;
    #endif
#endif // !__WXUNIVERSAL__

#ifndef wxHAS_NATIVE_TABART
    using wxAuiNativeTabArt = wxAuiGenericTabArt;
#endif

#define wxAuiDefaultTabArt wxAuiFlatTabArt

#endif  // wxUSE_AUI

#endif  // _WX_AUI_TABART_H_
