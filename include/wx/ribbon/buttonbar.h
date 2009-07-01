///////////////////////////////////////////////////////////////////////////////
// Name:        wx/ribbon/buttonbar.h
// Purpose:     Ribbon control similar to a tool bar
// Author:      Peter Cawley
// Modified by:
// Created:     2009-07-01
// RCS-ID:      $Id$
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////
#ifndef _WX_RIBBON_BUTTON_BAR_H_
#define _WX_RIBBON_BUTTON_BAR_H_

#include "wx/defs.h"

#if wxUSE_RIBBON

#include "wx/ribbon/control.h"

enum wxRibbonButtonBarButtonKind
{
	wxRIBBON_BUTTONBAR_BUTTON_NORMAL	= 1 << 0,
	wxRIBBON_BUTTONBAR_BUTTON_DROPDOWN	= 1 << 1,
	wxRIBBON_BUTTONBAR_BUTTON_HYBRID	= wxRIBBON_BUTTONBAR_BUTTON_NORMAL | wxRIBBON_BUTTONBAR_BUTTON_DROPDOWN,
};

class wxRibbonButtonBarButtonBase;

class WXDLLIMPEXP_RIBBON wxRibbonButtonBar : public wxRibbonControl
{
public:
	wxRibbonButtonBar();

	wxRibbonButtonBar(wxWindow* parent,
				  wxWindowID id = wxID_ANY,
				  const wxPoint& pos = wxDefaultPosition,
				  const wxSize& size = wxDefaultSize,
				  long style = 0);

	virtual ~wxRibbonButtonBar();

	bool Create(wxWindow* parent,
				wxWindowID id = wxID_ANY,
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxDefaultSize,
				long style = 0);

	virtual wxRibbonButtonBarButtonBase* AddButton(
				int button_id,
				const wxString& label,
				const wxBitmap& bitmap,
				const wxString& help_string = wxEmptyString,
				wxRibbonButtonBarButtonKind kind = wxRIBBON_BUTTONBAR_BUTTON_NORMAL);

	virtual wxRibbonButtonBarButtonBase* AddDropdownButton(
				int button_id,
				const wxString& label,
				const wxBitmap& bitmap,
				const wxString& help_string = wxEmptyString);

	virtual wxRibbonButtonBarButtonBase* AddHybridButton(
				int button_id,
				const wxString& label,
				const wxBitmap& bitmap,
				const wxString& help_string = wxEmptyString);
	
	virtual wxRibbonButtonBarButtonBase* AddButton(
				int button_id,
				const wxString& label,
				const wxBitmap& bitmap,
				const wxBitmap& bitmap_small = wxNullBitmap,
				const wxBitmap& bitmap_disabled = wxNullBitmap,
				const wxBitmap& bitmap_small_disabled = wxNullBitmap,
				wxRibbonButtonBarButtonKind kind = wxRIBBON_BUTTONBAR_BUTTON_NORMAL,
				const wxString& help_string = wxEmptyString,
				wxObject* client_data = NULL);

	virtual void CleatButtons();
	virtual bool DeleteButton(int button_id);
	virtual void EnableButton(int button_id, bool enable = true);

	virtual bool IsSizingContinuous() const;
	virtual wxSize GetNextSmallerSize(wxOrientation direction) const;
	virtual wxSize GetNextLargerSize(wxOrientation direction) const;

protected:
	wxBorder GetDefaultBorder() const { return wxBORDER_NONE; }

	void OnEraseBackground(wxEraseEvent& evt);
	void OnPaint(wxPaintEvent& evt);

	void CommonInit(long style);

#ifndef SWIG
	DECLARE_CLASS(wxRibbonButtonBar)
	DECLARE_EVENT_TABLE()
#endif
};

#endif // wxUSE_RIBBON

#endif // _WX_RIBBON_BUTTON_BAR_H_
