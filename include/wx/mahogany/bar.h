///////////////////////////////////////////////////////////////////////////////
// Name:        wx/mahogany/bar.h
// Purpose:     Top-level component of the ribbon-bar-style interface
// Author:      Peter Cawley
// Modified by:
// Created:     2009-05-23
// RCS-ID:      $Id$
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MAHOGANY_BAR_H_
#define _WX_MAHOGANY_BAR_H_

#include "wx/defs.h"

#if wxUSE_MAHOGANY

enum wxMahoganyBarOption
{
	wxMAHOGANY_BAR_SHOW_PAGE_LABELS	= 1 << 0,
	wxMAHOGANY_BAR_SHOW_PAGE_ICONS	= 1 << 1,

	wxMAHOGANY_BAR_DEFAULT_STYLE = wxMAHOGANY_BAR_SHOW_PAGE_LABELS
};

class WXDLLIMPEXP_MAHOGANY wxMahoganyBarEvent : public wxNotifyEvent
{
public:
	wxMahoganyBarEvent(wxEventType command_type = wxEVT_NULL,
					   int win_id = 0)
		: wxNotifyEvent(command_type, win_id)
	{
	}
#ifndef SWIG
	wxMahoganyBarEvent(const wxMahoganyBarEvent& c) : wxNotifyEvent(c)
	{
	}
#endif
	wxEvent *Clone() const { return new wxMahoganyBarEvent(*this); }

#ifndef SWIG
private:
	DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxMahoganyBarEvent)
#endif
};

class WXDLLIMPEXP_MAHOGANY wxMahoganyBar : public wxControl
{
public:
	wxMahoganyBar();

	wxMahoganyBar(wxWindow* parent,
				  wxWindowID id = wxID_ANY,
				  const wxPoint& pos = wxDefaultPosition,
				  const wxSize& size = wxDefaultSize,
				  long style = wxMAHOGANY_BAR_DEFAULT_STYLE);

	virtual ~wxMahoganyBar();

	bool Create(wxWindow* parent,
				wxWindowID id = wxID_ANY,
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxDefaultSize,
				long style = wxMAHOGANY_BAR_DEFAULT_STYLE);

protected:
	void CommonInit(long style);

	long m_flags;
	int m_tab_height;
	int m_tab_scroll_amount;
	int m_current_page;
	int m_current_hovered_page;
	bool m_tab_scroll_buttons_shown;

#ifndef SWIG
	DECLARE_CLASS(wxMahoganyBar)
	DECLARE_EVENT_TABLE()
#endif

	WX_DECLARE_CONTROL_CONTAINER();
};

#ifndef SWIG

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_MAHOGANY, wxEVT_COMMAND_MAHOGANYBAR_PAGE_CHANGED, wxMahoganyBarEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_MAHOGANY, wxEVT_COMMAND_MAHOGANYBAR_PAGE_CHANGING, wxMahoganyBarEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_MAHOGANY, wxEVT_COMMAND_MAHOGANYBAR_TAB_MIDDLE_DOWN, wxMahoganyBarEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_MAHOGANY, wxEVT_COMMAND_MAHOGANYBAR_TAB_MIDDLE_UP, wxMahoganyBarEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_MAHOGANY, wxEVT_COMMAND_MAHOGANYBAR_TAB_RIGHT_DOWN, wxMahoganyBarEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_MAHOGANY, wxEVT_COMMAND_MAHOGANYBAR_TAB_RIGHT_UP, wxMahoganyBarEvent);

typedef void (wxEvtHandler::*wxMahoganyBarEventFunction)(wxMahoganyBarEvent&);

#define wxMahoganyBarEventHandler(func) \
    wxEVENT_HANDLER_CAST(wxMahoganyBarEventFunction, func)

#define EVT_MAHOGANYBAR_PAGE_CHANGED(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_MAHOGANYBAR_PAGE_CHANGED, winid, wxMahoganyBarEventHandler(fn))
#define EVT_MAHOGANYBAR_PAGE_CHANGING(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_MAHOGANYBAR_PAGE_CHANGING, winid, wxMahoganyBarEventHandler(fn))
#define EVT_MAHOGANYBAR_TAB_MIDDLE_DOWN(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_MAHOGANYBAR_TAB_MIDDLE_DOWN, winid, wxMahoganyBarEventHandler(fn))
#define EVT_MAHOGANYBAR_TAB_MIDDLE_UP(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_MAHOGANYBAR_TAB_MIDDLE_UP, winid, wxMahoganyBarEventHandler(fn))
#define EVT_MAHOGANYBAR_TAB_RIGHT_DOWN(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_MAHOGANYBAR_TAB_RIGHT_DOWN, winid, wxMahoganyBarEventHandler(fn))
#define EVT_MAHOGANYBAR_TAB_RIGHT_UP(winid, fn) \
    wx__DECLARE_EVT1(wxEVT_COMMAND_MAHOGANYBAR_TAB_RIGHT_UP, winid, wxMahoganyBarEventHandler(fn))
#else

// wxpython/swig event work
%constant wxEventType wxEVT_COMMAND_MAHOGANYBAR_PAGE_CHANGED;
%constant wxEventType wxEVT_COMMAND_MAHOGANYBAR_PAGE_CHANGING;
%constant wxEventType wxEVT_COMMAND_MAHOGANYBAR_TAB_MIDDLE_DOWN;
%constant wxEventType wxEVT_COMMAND_MAHOGANYBAR_TAB_MIDDLE_UP;
%constant wxEventType wxEVT_COMMAND_MAHOGANYBAR_TAB_RIGHT_DOWN;
%constant wxEventType wxEVT_COMMAND_MAHOGANYBAR_TAB_RIGHT_UP;

%pythoncode {
    EVT_MAHOGANYBAR_PAGE_CHANGED = wx.PyEventBinder( wxEVT_COMMAND_MAHOGANYBAR_PAGE_CHANGED, 1 )
    EVT_MAHOGANYBAR_PAGE_CHANGING = wx.PyEventBinder( wxEVT_COMMAND_MAHOGANYBAR_PAGE_CHANGING, 1 )
    EVT_MAHOGANYBAR_TAB_MIDDLE_DOWN = wx.PyEventBinder( wxEVT_COMMAND_MAHOGANYBAR_TAB_MIDDLE_DOWN, 1 )
    EVT_MAHOGANYBAR_TAB_MIDDLE_UP = wx.PyEventBinder( wxEVT_COMMAND_MAHOGANYBAR_TAB_MIDDLE_UP, 1 )
    EVT_MAHOGANYBAR_TAB_RIGHT_DOWN = wx.PyEventBinder( wxEVT_COMMAND_MAHOGANYBAR_TAB_RIGHT_DOWN, 1 )
    EVT_MAHOGANYBAR_TAB_RIGHT_UP = wx.PyEventBinder( wxEVT_COMMAND_MAHOGANYBAR_TAB_RIGHT_UP, 1 )
}
#endif

#endif // wxUSE_MAHOGANY

#endif // _WX_MAHOGANY_BAR_H_

