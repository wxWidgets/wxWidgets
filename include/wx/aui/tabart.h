//////////////////////////////////////////////////////////////////////////////
// Name:        wx/aui/tabart.h
// Purpose:     wxaui: wx advanced user interface - notebook
// Author:      Benjamin I. Williams
// Modified by: Malcolm MacLeod (mmacleod@webmail.co.za)
// Modified by: Jens Lody (extracted from wx/aui/auibook.h)
// Created:     2012-03-21
// RCS-ID:      $Id:$
// Copyright:   (C) Copyright 2006, Kirix Corporation, All Rights Reserved.
//                            2012, Jens Lody for the code related to left
//                                  and right positioning
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////


#ifndef _WX_AUI_TABART_H_
#define _WX_AUI_TABART_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/defs.h"

#if wxUSE_AUI

#include "wx/aui/auibook.h"

#include "wx/colour.h"
#include "wx/gdicmn.h"
#include "wx/font.h"
#include "wx/pen.h"
#include "wx/brush.h"
#include "wx/bitmap.h"


class wxWindow;
class wxDC;
class wxAuiPaneInfo;
class wxAuiPaneInfoPtrArray;

// tab art class

class WXDLLIMPEXP_AUI wxAuiTabArt
{
public:

    wxAuiTabArt() { }
    virtual ~wxAuiTabArt() { }

    virtual wxAuiTabArt* Clone() = 0;
    virtual void SetFlags(unsigned int flags) = 0;

    virtual void SetSizingInfo(const wxSize& tabCtrlSize,
                               size_t tabCount) = 0;

    virtual void SetNormalFont(const wxFont& font) = 0;
    virtual void SetSelectedFont(const wxFont& font) = 0;
    virtual void SetMeasuringFont(const wxFont& font) = 0;
    virtual void SetColour(const wxColour& colour) = 0;
    virtual void SetActiveColour(const wxColour& colour) = 0;
    // this should actually set the requested size
    virtual void SetTabCtrlHeight(int size) = 0;
    // this should actually set the requested size
    virtual void SetTabCtrlWidth(int size) = 0;

    virtual void SetUniformBitmapSize(const wxSize& size) = 0;

    virtual void DrawBorder(
                            wxDC& dc,
                            wxWindow* wnd,
                            const wxRect& rect) = 0;
    
    virtual void DrawBackground(
                         wxDC& dc,
                         wxWindow* wnd,
                         const wxRect& rect) = 0;

    virtual void DrawTab(wxDC& dc,
                         wxWindow* wnd,
                         const wxAuiPaneInfo& pane,
                         const wxRect& inRect,
                         int closeButtonState,
                         bool haveFocus,
                         wxRect* outTabRect,
                         wxRect* outButtonRect,
                         int* xExtent) = 0;

    virtual void DrawButton(
                         wxDC& dc,
                         wxWindow* wnd,
                         const wxRect& inRect,
                         int bitmapId,
                         int buttonState,
                         int orientation,
                         wxRect* outRect) = 0;

    virtual wxSize GetTabSize(
                         wxDC& dc,
                         wxWindow* wnd,
                         const wxString& caption,
                         const wxBitmap& bitmap,
                         bool active,
                         int closeButtonState,
                         int* xExtent) = 0;

    virtual int ShowDropDown(
                         wxWindow* wnd,
                         const wxAuiPaneInfoPtrArray& items,
                         int activeIdx) = 0;

    virtual int GetIndentSize() = 0;



    virtual int GetAdditionalBorderSpace(
                         wxWindow* wnd) = 0;

    virtual wxSize GetBestTabSize(
                         wxWindow* wnd,
                         const wxAuiPaneInfoPtrArray& pages,
                         const wxSize& requiredBmpSize) = 0;

    virtual int GetBestTabCtrlSize(
                         wxWindow* wnd,
                         const wxAuiPaneInfoPtrArray& pages) = 0;

    // this should return -1,-1 if there is no requested width \ height
    virtual wxSize GetRequestedSize() const = 0;
    virtual wxSize GetRequiredBitmapSize() const = 0;

    int m_fixedTabSize;
    int m_tabCtrlHeight;
    int m_tabCtrlWidth;
    unsigned int m_flags;
};


class WXDLLIMPEXP_AUI wxAuiGenericTabArt : public wxAuiTabArt
{

public:

    wxAuiGenericTabArt();
    virtual ~wxAuiGenericTabArt();

    wxAuiTabArt* Clone();
    void SetFlags(unsigned int flags);
    void SetSizingInfo(const wxSize& tabCtrlSize,
                       size_t tabCount);

    void SetNormalFont(const wxFont& font);
    void SetSelectedFont(const wxFont& font);
    void SetMeasuringFont(const wxFont& font);
    void SetColour(const wxColour& colour);
    void SetActiveColour(const wxColour& colour);
    void SetTabCtrlHeight(int size);
    void SetTabCtrlWidth(int size);
    void SetUniformBitmapSize(const wxSize& size);

    void DrawBorder(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& rect);

    void DrawBackground(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& rect);

    void DrawTab(wxDC& dc,
                 wxWindow* wnd,
                 const wxAuiPaneInfo& pane,
                 const wxRect& inRect,
                 int closeButtonState,
                 bool haveFocus,
                 wxRect* outTabRect,
                 wxRect* outButtonRect,
                 int* xExtent);

    void DrawButton(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& inRect,
                 int bitmapId,
                 int buttonState,
                 int orientation,
                 wxRect* outRect);

    int GetIndentSize();

    int GetBorderWidth(
                 wxWindow* wnd);

    int GetAdditionalBorderSpace(
                 wxWindow* wnd);

    wxSize GetTabSize(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxString& caption,
                 const wxBitmap& bitmap,
                 bool active,
                 int closeButtonState,
                 int* xExtent);

    int ShowDropDown(
                 wxWindow* wnd,
                 const wxAuiPaneInfoPtrArray& items,
                 int activeIdx);

    wxSize GetBestTabSize(wxWindow* wnd,
                 const wxAuiPaneInfoPtrArray& pages,
                 const wxSize& requiredBmpSize);

    int GetBestTabCtrlSize(wxWindow* wnd,
                 const wxAuiPaneInfoPtrArray& pages)
                 {
                     return GetBestTabSize(wnd, pages, m_requiredBitmapSize).GetHeight();
                 }


    wxSize GetRequiredBitmapSize() const;
    wxSize GetRequestedSize() const;

    // Returns true if the tabart has the given flag bit set
    bool HasFlag(int flag) const    { return (m_flags & flag) != 0; }
    // returns true if we have wxAUI_NB_TOP or wxAUI_NB_BOTTOM style
    bool IsHorizontal() const { return HasFlag(wxAUI_NB_TOP | wxAUI_NB_BOTTOM); }

protected:
    wxSize m_requiredBitmapSize;
    wxSize m_requestedSize;

    wxFont m_normalFont;
    wxFont m_selectedFont;
    wxFont m_measuringFont;
    wxColour m_baseColour;
    wxPen m_baseColourPen;
    wxPen m_borderPen;
    wxBrush m_baseColourBrush;
    wxColour m_activeColour;
    wxBitmap m_activeCloseBmp;
    wxBitmap m_disabledCloseBmp;
    wxBitmap m_activeLeftBmp;
    wxBitmap m_disabledLeftBmp;
    wxBitmap m_activeUpBmp;
    wxBitmap m_disabledUpBmp;
    wxBitmap m_activeDownBmp;
    wxBitmap m_disabledDownBmp;
    wxBitmap m_activeRightBmp;
    wxBitmap m_disabledRightBmp;
    wxBitmap m_activeWindowListBmp;
    wxBitmap m_disabledWindowListBmp;
};


class WXDLLIMPEXP_AUI wxAuiSimpleTabArt : public wxAuiTabArt
{

public:

    wxAuiSimpleTabArt();
    virtual ~wxAuiSimpleTabArt();

    wxAuiTabArt* Clone();
    void SetFlags(unsigned int flags);

    void SetSizingInfo(const wxSize& tabCtrlSize,
                       size_t tabCount);

    void SetNormalFont(const wxFont& font);
    void SetSelectedFont(const wxFont& font);
    void SetMeasuringFont(const wxFont& font);
    void SetColour(const wxColour& colour);
    void SetActiveColour(const wxColour& colour);
    void SetTabCtrlHeight(int size);
    void SetTabCtrlWidth(int size);
    void SetUniformBitmapSize(const wxSize& size);

    void DrawBorder(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& rect);

    void DrawBackground(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& rect);

    void DrawTab(wxDC& dc,
                 wxWindow* wnd,
                 const wxAuiPaneInfo& pane,
                 const wxRect& inRect,
                 int closeButtonState,
                 bool haveFocus,
                 wxRect* outTabRect,
                 wxRect* outButtonRect,
                 int* xExtent);

    void DrawButton(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxRect& inRect,
                 int bitmapId,
                 int buttonState,
                 int orientation,
                 wxRect* outRect);

    int GetIndentSize();

    int GetBorderWidth(
                 wxWindow* wnd);

    int GetAdditionalBorderSpace(
                 wxWindow* wnd);

    wxSize GetTabSize(
                 wxDC& dc,
                 wxWindow* wnd,
                 const wxString& caption,
                 const wxBitmap& bitmap,
                 bool active,
                 int closeButtonState,
                 int* xExtent);

    int ShowDropDown(
                 wxWindow* wnd,
                 const wxAuiPaneInfoPtrArray& items,
                 int activeIdx);

   wxSize GetBestTabSize(wxWindow* wnd,
                 const wxAuiPaneInfoPtrArray& pages,
                 const wxSize& requiredBmpSize);

    int GetBestTabCtrlSize(wxWindow* wnd,
                 const wxAuiPaneInfoPtrArray& pages)
                 {
                     return GetBestTabSize(wnd, pages, m_requiredBitmapSize).GetHeight();
                 }

    wxSize GetRequiredBitmapSize() const;
    wxSize GetRequestedSize() const;

    // Returns true if the tabart has the given flag bit set
    bool HasFlag(int flag) const    { return (m_flags & flag) != 0; }
    // returns true if we have wxAUI_NB_TOP or wxAUI_NB_BOTTOM style
    bool IsHorizontal() const { return HasFlag(wxAUI_NB_TOP | wxAUI_NB_BOTTOM); }

protected:
    wxSize m_requiredBitmapSize;
    wxSize m_requestedSize;
    wxFont m_normalFont;
    wxFont m_selectedFont;
    wxFont m_measuringFont;
    wxPen m_normalBkPen;
    wxPen m_selectedBkPen;
    wxBrush m_normalBkBrush;
    wxBrush m_selectedBkBrush;
    wxBrush m_bkBrush;
    wxBitmap m_activeCloseBmp;
    wxBitmap m_disabledCloseBmp;
    wxBitmap m_activeLeftBmp;
    wxBitmap m_disabledLeftBmp;
    wxBitmap m_activeUpBmp;
    wxBitmap m_disabledUpBmp;
    wxBitmap m_activeDownBmp;
    wxBitmap m_disabledDownBmp;
    wxBitmap m_activeRightBmp;
    wxBitmap m_disabledRightBmp;
    wxBitmap m_activeWindowListBmp;
    wxBitmap m_disabledWindowListBmp;
};

#ifndef __WXUNIVERSAL__
    #if defined(__WXGTK20__) && !defined(__WXGTK3__)
        #define wxHAS_NATIVE_TABART
        #include "wx/aui/tabartgtk.h"
        #define wxAuiDefaultTabArt wxAuiGtkTabArt
        #define wxAuiNativeTabArt wxAuiGtkTabArt
    #endif
#endif // !__WXUNIVERSAL__

#ifndef wxHAS_NATIVE_TABART
    #define wxAuiDefaultTabArt wxAuiGenericTabArt
#endif

#endif  // wxUSE_AUI

#endif  // _WX_AUI_TABART_H_
