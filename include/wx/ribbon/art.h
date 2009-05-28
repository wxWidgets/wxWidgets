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

enum wxRibbonArtSetting
{
	wxRIBBON_ART_TAB_SEPARATION_SIZE,
	wxRIBBON_ART_TAB_LABEL_FONT,
	wxRIBBON_ART_TAB_LABEL_COLOUR,
	wxRIBBON_ART_TAB_CTRL_BACKGROUND_COLOUR,
};

class wxRibbonPageTabInfo;

class WXDLLIMPEXP_AUI wxRibbonArtProvider
{
public:
	wxRibbonArtProvider() { }
	virtual ~wxRibbonArtProvider() { }

	virtual wxRibbonArtProvider* Clone() = 0;
    virtual void SetFlags(long flags) = 0;

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

	virtual void GetBarTabWidth(
						wxDC& dc,
                        wxWindow* wnd,
                        const wxString& label,
                        const wxBitmap& bitmap,
						int* ideal,
						int* small_begin_need_separator,
						int* small_must_have_separator,
						int* minimum) = 0;
};

class WXDLLIMPEXP_AUI wxRibbonDefaultArtProvider : public wxRibbonArtProvider
{
public:
	wxRibbonDefaultArtProvider();
	virtual ~wxRibbonDefaultArtProvider();

	wxRibbonArtProvider* Clone();
	void SetFlags(long flags);

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

	void GetBarTabWidth(
						wxDC& dc,
						wxWindow* wnd,
						const wxString& label,
						const wxBitmap& bitmap,
						int* ideal,
						int* small_begin_need_separator,
						int* small_must_have_separator,
						int* minimum);

protected:
	wxColour m_tab_label_colour;
	wxBrush m_tab_ctrl_background_brush;
	wxFont m_tab_label_font;
	long m_flags;
	int m_tab_separation_size;
};

#endif // wxUSE_RIBBON

#endif // _WX_RIBBON_ART_H_
