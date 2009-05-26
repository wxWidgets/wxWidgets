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

class wxRibbonBar;

class WXDLLIMPEXP_RIBBON wxRibbonPage : public wxControl
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

protected:
	void CommonInit(const wxString& label, const wxBitmap& icon);

	wxBitmap m_icon;
	int m_scroll_amount;
	bool m_scroll_buttons_visible;

#ifndef SWIG
	DECLARE_CLASS(wxRibbonPage)
	DECLARE_EVENT_TABLE()
#endif

	WX_DECLARE_CONTROL_CONTAINER();
};

#endif // wxUSE_RIBBON

#endif // _WX_RIBBON_PAGE_H_
