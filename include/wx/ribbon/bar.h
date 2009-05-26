///////////////////////////////////////////////////////////////////////////////
// Name:        wx/ribbon/bar.h
// Purpose:     Top-level component of the ribbon-bar-style interface
// Author:      Peter Cawley
// Modified by:
// Created:     2009-05-23
// RCS-ID:      $Id$
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RIBBON_BAR_H_
#define _WX_RIBBON_BAR_H_

#include "wx/defs.h"

#if wxUSE_RIBBON

enum wxRibbonBarOption
{
	wxRIBBON_BAR_SHOW_PAGE_LABELS	= 1 << 0,
	wxRIBBON_BAR_SHOW_PAGE_ICONS	= 1 << 1,

	wxRIBBON_BAR_DEFAULT_STYLE = wxRIBBON_BAR_SHOW_PAGE_LABELS
};

class WXDLLIMPEXP_RIBBON wxRibbonBarEvent : public wxNotifyEvent
{
public:
	wxRibbonBarEvent(wxEventType command_type = wxEVT_NULL,
					   int win_id = 0)
		: wxNotifyEvent(command_type, win_id)
	{
	}
#ifndef SWIG
	wxRibbonBarEvent(const wxRibbonBarEvent& c) : wxNotifyEvent(c)
	{
	}
#endif
	wxEvent *Clone() const { return new wxRibbonBarEvent(*this); }

#ifndef SWIG
private:
	DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxRibbonBarEvent)
#endif
};

class WXDLLIMPEXP_RIBBON wxRibbonBar : public wxControl
{
public:
	wxRibbonBar();

	wxRibbonBar(wxWindow* parent,
				  wxWindowID id = wxID_ANY,
				  const wxPoint& pos = wxDefaultPosition,
				  const wxSize& size = wxDefaultSize,
				  long style = wxRIBBON_BAR_DEFAULT_STYLE);

	virtual ~wxRibbonBar();

	bool Create(wxWindow* parent,
				wxWindowID id = wxID_ANY,
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxDefaultSize,
				long style = wxRIBBON_BAR_DEFAULT_STYLE);

protected:
	void CommonInit(long style);

	long m_flags;
	int m_tab_height;
	int m_tab_scroll_amount;
	int m_current_page;
	int m_current_hovered_page;
	bool m_tab_scroll_buttons_shown;

#ifndef SWIG
	DECLARE_CLASS(wxRibbonBar)
	DECLARE_EVENT_TABLE()
#endif

	WX_DECLARE_CONTROL_CONTAINER();
};

#ifndef SWIG

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_RIBBON, wxEVT_COMMAND_RIBBONBAR_PAGE_CHANGED, wxRibbonBarEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_RIBBON, wxEVT_COMMAND_RIBBONBAR_PAGE_CHANGING, wxRibbonBarEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_RIBBON, wxEVT_COMMAND_RIBBONBAR_TAB_MIDDLE_DOWN, wxRibbonBarEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_RIBBON, wxEVT_COMMAND_RIBBONBAR_TAB_MIDDLE_UP, wxRibbonBarEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_RIBBON, wxEVT_COMMAND_RIBBONBAR_TAB_RIGHT_DOWN, wxRibbonBarEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_RIBBON, wxEVT_COMMAND_RIBBONBAR_TAB_RIGHT_UP, wxRibbonBarEvent);

typedef void (wxEvtHandler::*wxRibbonBarEventFunction)(wxRibbonBarEvent&);

#define wxRibbonBarEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxRibbonBarEventFunction, func)

#define EVT_RIBBONBAR_PAGE_CHANGED(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_RIBBONBAR_PAGE_CHANGED, winid, wxRibbonBarEventHandler(fn))
#define EVT_RIBBONBAR_PAGE_CHANGING(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_RIBBONBAR_PAGE_CHANGING, winid, wxRibbonBarEventHandler(fn))
#define EVT_RIBBONBAR_TAB_MIDDLE_DOWN(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_RIBBONBAR_TAB_MIDDLE_DOWN, winid, wxRibbonBarEventHandler(fn))
#define EVT_RIBBONBAR_TAB_MIDDLE_UP(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_RIBBONBAR_TAB_MIDDLE_UP, winid, wxRibbonBarEventHandler(fn))
#define EVT_RIBBONBAR_TAB_RIGHT_DOWN(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_RIBBONBAR_TAB_RIGHT_DOWN, winid, wxRibbonBarEventHandler(fn))
#define EVT_RIBBONBAR_TAB_RIGHT_UP(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_RIBBONBAR_TAB_RIGHT_UP, winid, wxRibbonBarEventHandler(fn))
#else

// wxpython/swig event work
%constant wxEventType wxEVT_COMMAND_RIBBONBAR_PAGE_CHANGED;
%constant wxEventType wxEVT_COMMAND_RIBBONBAR_PAGE_CHANGING;
%constant wxEventType wxEVT_COMMAND_RIBBONBAR_TAB_MIDDLE_DOWN;
%constant wxEventType wxEVT_COMMAND_RIBBONBAR_TAB_MIDDLE_UP;
%constant wxEventType wxEVT_COMMAND_RIBBONBAR_TAB_RIGHT_DOWN;
%constant wxEventType wxEVT_COMMAND_RIBBONBAR_TAB_RIGHT_UP;

%pythoncode {
    EVT_RIBBONBAR_PAGE_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_RIBBONBAR_PAGE_CHANGED, 1 )
    EVT_RIBBONBAR_PAGE_CHANGING = wx.PyEventBinder( wxEVT_COMMAND_RIBBONBAR_PAGE_CHANGING, 1 )
    EVT_RIBBONBAR_TAB_MIDDLE_DOWN = wx.PyEventBinder( wxEVT_COMMAND_RIBBONBAR_TAB_MIDDLE_DOWN, 1 )
    EVT_RIBBONBAR_TAB_MIDDLE_UP = wx.PyEventBinder( wxEVT_COMMAND_RIBBONBAR_TAB_MIDDLE_UP, 1 )
    EVT_RIBBONBAR_TAB_RIGHT_DOWN = wx.PyEventBinder( wxEVT_COMMAND_RIBBONBAR_TAB_RIGHT_DOWN, 1 )
    EVT_RIBBONBAR_TAB_RIGHT_UP = wx.PyEventBinder( wxEVT_COMMAND_RIBBONBAR_TAB_RIGHT_UP, 1 )
}
#endif

#endif // wxUSE_RIBBON

#endif // _WX_RIBBON_BAR_H_

