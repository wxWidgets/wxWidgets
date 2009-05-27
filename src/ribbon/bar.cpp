///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/bar.cpp
// Purpose:     Top-level component of the ribbon-bar-style interface
// Author:      Peter Cawley
// Modified by:
// Created:     2009-05-23
// RCS-ID:      $Id$
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/ribbon/bar.h"

#if wxUSE_RIBBON

#include "wx/ribbon/art.h"

#ifndef WX_PRECOMP
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

#include "wx/arrimpl.cpp"

WX_DEFINE_USER_EXPORTED_OBJARRAY(wxRibbonPageTabInfoArray);

IMPLEMENT_CLASS(wxRibbonBar, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxRibbonBarEvent, wxNotifyEvent)

BEGIN_EVENT_TABLE(wxRibbonBar, wxControl)
END_EVENT_TABLE()

void wxRibbonBar::AddChild(wxWindowBase *child)
{
    wxControl::AddChild(child);

	wxRibbonPage *page = wxDynamicCast(child, wxRibbonPage);
	if(page)
	{
		AddPage(page);
	}
	else
	{
		wxFAIL_MSG(wxT("The only immediate children of a wxRibbonBar should be wxRibbonPages"));
	}
}

void wxRibbonBar::AddPage(wxRibbonPage *page)
{
	wxRibbonPageTabInfo info;

	info.page = page;
	// info.rect not set (intentional)

	wxMemoryDC dcTemp;
	m_art->GetBarTabWidth(dcTemp, this,
						  m_flags & wxRIBBON_BAR_SHOW_PAGE_LABELS ? page->GetLabel() : wxEmptyString,
						  m_flags & wxRIBBON_BAR_SHOW_PAGE_ICONS ? page->GetIcon() : wxNullBitmap,
						  &info.ideal_width,
						  &info.small_begin_need_separator_width,
						  &info.small_must_have_separator_width,
						  &info.minimum_width);

	if(m_pages.IsEmpty())
	{
		m_tabs_total_width_ideal = info.ideal_width;
		m_tabs_total_width_minimum = info.minimum_width;
	}
	else
	{
		int sep = m_art->GetMetric(wxRIBBON_ART_TAB_SEPARATION_SIZE);
		m_tabs_total_width_ideal += sep + info.ideal_width;
		m_tabs_total_width_minimum += sep + info.minimum_width;
	}
	m_pages.Add(info);

	page->Hide(); // Most likely case is that this new page is not the active tab

	RecalculateTabSizes();
}

void wxRibbonBar::SetTabCtrlMargins(int left, int right)
{
	m_tab_margin_left = left;
	m_tab_margin_right = right;

	RecalculateTabSizes();
}

void wxRibbonBar::RecalculateTabSizes()
{
	size_t numtabs = m_pages.GetCount();

	if(numtabs == 0)
		return;

	int width = GetSize().GetWidth() - m_tab_margin_left - m_tab_margin_right;
	int tabsep = m_art->GetMetric(wxRIBBON_ART_TAB_SEPARATION_SIZE);

	if(width >= m_tabs_total_width_ideal)
	{
		// Simple case: everything at ideal width
	}
	else if(width < m_tabs_total_width_minimum)
	{
		// Simple case: everything minimum with scrollbar
	}
	else
	{
		// Complex case: everything sized such that: minimum <= width < ideal
	}
}

wxRibbonBar::wxRibbonBar()
{
	m_flags = 0;
	m_tabs_total_width_ideal = 0;
	m_tabs_total_width_minimum = 0;
	m_tab_scroll_amount = 0;
	m_current_page = -1;
	m_current_hovered_page = -1;
	m_tab_scroll_buttons_shown = false;
}

wxRibbonBar::wxRibbonBar(wxWindow* parent,
						 wxWindowID id,
						 const wxPoint& pos,
						 const wxSize& size,
						 long style) : wxControl(parent, id, pos, size, style)
{
	CommonInit(style);
}

wxRibbonBar::~wxRibbonBar()
{
}

bool wxRibbonBar::Create(wxWindow* parent,
				wxWindowID id,
				const wxPoint& pos,
				const wxSize& size,
				long style)
{
	if(!wxControl::Create(parent, id, pos, size, style))
		return false;

	CommonInit(style);

	return true;
}

void wxRibbonBar::CommonInit(long style)
{
    SetName(wxT("wxRibbonBar"));

	m_flags = style;
	m_tabs_total_width_ideal = 0;
	m_tabs_total_width_minimum = 0;
	m_tab_height = 20; // initial guess
	m_tab_scroll_amount = 0;
	m_current_page = -1;
	m_current_hovered_page = -1;
	m_tab_scroll_buttons_shown = false;

	SetArtProvider(new wxRibbonDefaultArtProvider);
}

#endif // wxUSE_RIBBON
