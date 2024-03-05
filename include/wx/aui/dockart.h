///////////////////////////////////////////////////////////////////////////////
// Name:        wx/aui/dockart.h
// Purpose:     wxaui: wx advanced user interface - docking window manager
// Author:      Benjamin I. Williams
// Created:     2005-05-17
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
#include "wx/bmpbndl.h"
#include "wx/colour.h"

// dock art provider code - a dock provider provides all drawing
// functionality to the wxAui dock manager.  This allows the dock
// manager to have pluggable look-and-feels

class WXDLLIMPEXP_AUI wxAuiDockArt
{
public:

    wxAuiDockArt() = default;
    virtual ~wxAuiDockArt() = default;

    virtual wxAuiDockArt* Clone() = 0;
    virtual int GetMetric(int id) = 0;
    virtual void SetMetric(int id, int newVal) = 0;
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
                          int buttonState,
                          const wxRect& rect,
                          wxAuiPaneInfo& pane) = 0;

    // Provide opportunity for subclasses to recalculate colours
    virtual void UpdateColoursFromSystem() {}
};


// this is the default art provider for wxAuiManager.  Dock art
// can be customized by creating a class derived from this one,
// or replacing this class entirely

class WXDLLIMPEXP_AUI wxAuiDefaultDockArt : public wxAuiDockArt
{
public:

    wxAuiDefaultDockArt();

    wxAuiDockArt* Clone() override;
    int GetMetric(int metricId) override;
    void SetMetric(int metricId, int newVal) override;
    wxColour GetColour(int id) override;
    void SetColour(int id, const wxColor& colour) override;
    void SetFont(int id, const wxFont& font) override;
    wxFont GetFont(int id) override;

    void DrawSash(wxDC& dc,
                  wxWindow *window,
                  int orientation,
                  const wxRect& rect) override;

    void DrawBackground(wxDC& dc,
                  wxWindow *window,
                  int orientation,
                  const wxRect& rect) override;

    void DrawCaption(wxDC& dc,
                  wxWindow *window,
                  const wxString& text,
                  const wxRect& rect,
                  wxAuiPaneInfo& pane) override;

    void DrawGripper(wxDC& dc,
                  wxWindow *window,
                  const wxRect& rect,
                  wxAuiPaneInfo& pane) override;

    void DrawBorder(wxDC& dc,
                  wxWindow *window,
                  const wxRect& rect,
                  wxAuiPaneInfo& pane) override;

    void DrawPaneButton(wxDC& dc,
                  wxWindow *window,
                  int button,
                  int buttonState,
                  const wxRect& rect,
                  wxAuiPaneInfo& pane) override;

#if WXWIN_COMPATIBILITY_3_0
    wxDEPRECATED_MSG("This is not intended for the public API")
    void DrawIcon(wxDC& dc,
                  const wxRect& rect,
                  wxAuiPaneInfo& pane);
#endif

    virtual void UpdateColoursFromSystem() override;


protected:

    void DrawCaptionBackground(wxDC& dc, const wxRect& rect, bool active);

    void DrawIcon(wxDC& dc, wxWindow *window, const wxRect& rect, wxAuiPaneInfo& pane);

    void InitBitmaps();

protected:

    wxPen m_borderPen;
    wxBrush m_sashBrush;
    wxBrush m_backgroundBrush;
    wxBrush m_gripperBrush;
    wxFont m_captionFont;
    wxBitmapBundle m_inactiveCloseBitmap;
    wxBitmapBundle m_inactivePinBitmap;
    wxBitmapBundle m_inactiveMaximizeBitmap;
    wxBitmapBundle m_inactiveRestoreBitmap;
    wxBitmapBundle m_activeCloseBitmap;
    wxBitmapBundle m_activePinBitmap;
    wxBitmapBundle m_activeMaximizeBitmap;
    wxBitmapBundle m_activeRestoreBitmap;
    wxPen m_gripperPen1;
    wxPen m_gripperPen2;
    wxPen m_gripperPen3;
    wxColour m_baseColour;
    wxColour m_activeCaptionColour;
    wxColour m_activeCaptionGradientColour;
    wxColour m_activeCaptionTextColour;
    wxColour m_inactiveCaptionColour;
    wxColour m_inactiveCaptionGradientColour;
    wxColour m_inactiveCaptionTextColour;
    int m_borderSize;
    int m_captionSize;
    int m_sashSize;
    int m_buttonSize;
    int m_gripperSize;
    int m_gradientType;
};



#endif // wxUSE_AUI
#endif //_WX_DOCKART_H_
