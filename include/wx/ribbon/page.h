///////////////////////////////////////////////////////////////////////////////
// Name:        wx/ribbon/page.h
// Purpose:     Container for ribbon-bar-style interface panels
// Author:      Peter Cawley
// Modified by:
// Created:     2009-05-25
// RCS-ID:      $Id$
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RIBBON_PAGE_H_
#define _WX_RIBBON_PAGE_H_

#include "wx/defs.h"

#if wxUSE_RIBBON

#include "wx/ribbon/control.h"
#include "wx/ribbon/panel.h"
#include "wx/bitmap.h"

class wxRibbonBar;
class wxRibbonPageScrollButton;

class WXDLLIMPEXP_RIBBON wxRibbonPage : public wxRibbonControl
{
public:
	wxRibbonPage();

	wxRibbonPage(wxRibbonBar* parent,
				 wxWindowID id = wxID_ANY,
				 const wxString& label = wxEmptyString,
				 const wxBitmap& icon = wxNullBitmap,
				 const wxPoint& pos = wxDefaultPosition,
				 const wxSize& size = wxDefaultSize,
				 long style = 0);

	virtual ~wxRibbonPage();

	bool Create(wxRibbonBar* parent,
				wxWindowID id = wxID_ANY,
				const wxString& label = wxEmptyString,
				const wxBitmap& icon = wxNullBitmap,
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxDefaultSize,
				long style = 0);

	void SetArtProvider(wxRibbonArtProvider* art);

	wxBitmap& GetIcon() {return m_icon;}
	virtual wxSize GetMinSize() const;
	virtual wxSize DoGetBestSize() const;
	void SetSizeWithScrollButtonAdjustment(int x, int y, int width, int height);

	virtual bool Show(bool show = true);
	virtual bool Layout();
	virtual bool ScrollLines(int lines);
	bool ScrollPixels(int pixels);

	wxOrientation GetMajorAxis() const;

protected:
	virtual wxBorder GetDefaultBorder() const { return wxBORDER_NONE; }

	void DoSetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
	void OnEraseBackground(wxEraseEvent& evt);
	void OnPaint(wxPaintEvent& evt);
	void OnSize(wxSizeEvent& evt);

	bool ExpandPanels(wxOrientation direction, int maximum_amount);
	bool CollapsePanels(wxOrientation direction, int minimum_amount);
	void ShowScrollButtons();
	void HideScrollButtons();

	void CommonInit(const wxString& label, const wxBitmap& icon);

	wxBitmap m_icon;
	wxSize m_old_size;
	// NB: Scroll button windows are siblings rather than children (to get correct clipping of children)
	wxRibbonPageScrollButton* m_scroll_left_btn;
	wxRibbonPageScrollButton* m_scroll_right_btn;
	int m_scroll_amount;
	int m_scroll_amount_limit;
	int size_in_major_axis_for_children;
	bool m_scroll_buttons_visible;

#ifndef SWIG
	DECLARE_CLASS(wxRibbonPage)
	DECLARE_EVENT_TABLE()
#endif
};

#endif // wxUSE_RIBBON

#endif // _WX_RIBBON_PAGE_H_
