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

	virtual bool Layout();

	wxOrientation GetMajorAxis() const;

protected:
	virtual wxBorder GetDefaultBorder() const { return wxBORDER_NONE; }

	void OnEraseBackground(wxEraseEvent& evt);
	void OnPaint(wxPaintEvent& evt);

	void CommonInit(const wxString& label, const wxBitmap& icon);

	wxBitmap m_icon;
	int m_scroll_amount;
	bool m_scroll_buttons_visible;

#ifndef SWIG
	DECLARE_CLASS(wxRibbonPage)
	DECLARE_EVENT_TABLE()
#endif
};

#endif // wxUSE_RIBBON

#endif // _WX_RIBBON_PAGE_H_
