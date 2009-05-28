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
#include "wx/dcbuffer.h"

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
  EVT_ERASE_BACKGROUND(wxRibbonBar::OnEraseBackground)
  EVT_PAINT(wxRibbonBar::OnPaint)
  EVT_SIZE(wxRibbonBar::OnSize)  
END_EVENT_TABLE()

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

static int OrderPageTabInfoBySmallWidthAsc(wxRibbonPageTabInfo **first, wxRibbonPageTabInfo **second)
{
	return (**first).small_must_have_separator_width - (**second).small_must_have_separator_width;
}

void wxRibbonBar::RecalculateTabSizes()
{
	size_t numtabs = m_pages.GetCount();

	if(numtabs == 0)
		return;

	int width = GetSize().GetWidth() - m_tab_margin_left - m_tab_margin_right;
	int tabsep = m_art->GetMetric(wxRIBBON_ART_TAB_SEPARATION_SIZE);
	int x = m_tab_margin_left;
	const int y = 0;

	if(width >= m_tabs_total_width_ideal)
	{
		// Simple case: everything at ideal width
		for(size_t i = 0; i < numtabs; ++i)
		{
			wxRibbonPageTabInfo& info = m_pages.Item(i);
			info.rect.x = x;
			info.rect.y = y;
			info.rect.width = info.ideal_width;
			info.rect.height = m_tab_height;
			x += info.rect.width + tabsep;
		}
		m_tab_scroll_buttons_shown = false;
	}
	else if(width < m_tabs_total_width_minimum)
	{
		// Simple case: everything minimum with scrollbar
		for(size_t i = 0; i < numtabs; ++i)
		{
			wxRibbonPageTabInfo& info = m_pages.Item(i);
			info.rect.x = x;
			info.rect.y = y;
			info.rect.width = info.minimum_width;
			info.rect.height = m_tab_height;
			x += info.rect.width + tabsep;
		}
		m_tab_scroll_buttons_shown = true;
		// TODO: fix m_tab_scroll_amount if it is out of bounds
	}
	else
	{
		// Complex case: everything sized such that: minimum <= width < ideal
		/*
		   Strategy:
		     1) Uniformly reduce all tab widths from ideal to small_must_have_separator_width
			 2) Reduce the largest tab by 1 pixel, repeating until all tabs are same width (or at minimum)
			 3) Uniformly reduce all tabs down to their minimum width
		*/
		int smallest_tab_width = INT_MAX;
		int total_small_width = tabsep * (numtabs - 1);
		for(size_t i = 0; i < numtabs; ++i)
		{
			wxRibbonPageTabInfo& info = m_pages.Item(i);
			if(info.small_must_have_separator_width < smallest_tab_width)
			{
				smallest_tab_width = info.small_must_have_separator_width;
			}
			total_small_width += info.small_must_have_separator_width;
		}
		if(width >= total_small_width)
		{
			// Do (1)
			int total_delta = m_tabs_total_width_ideal - total_small_width;
			total_small_width -= tabsep * (numtabs - 1);
			width -= tabsep * (numtabs - 1);
			for(size_t i = 0; i < numtabs; ++i)
			{
				wxRibbonPageTabInfo& info = m_pages.Item(i);
				int delta = info.ideal_width - info.small_must_have_separator_width;
				info.rect.x = x;
				info.rect.y = y;
				info.rect.width = info.small_must_have_separator_width + delta * (width - total_small_width) / total_delta;
				info.rect.height = m_tab_height;

				x += info.rect.width + tabsep;
				total_delta -= delta;
				total_small_width -= info.small_must_have_separator_width;
				width -= info.rect.width;
			}
		}
		else
		{
			total_small_width = tabsep * (numtabs - 1);
			for(size_t i = 0; i < numtabs; ++i)
			{
				wxRibbonPageTabInfo& info = m_pages.Item(i);
				if(info.minimum_width < smallest_tab_width)
				{
					total_small_width += smallest_tab_width;
				}
				else
				{
					total_small_width += info.minimum_width;
				}
			}
			if(width >= total_small_width)
			{
				// Do (2)
				wxRibbonPageTabInfoArray sorted_pages;
				for(size_t i = 0; i < numtabs; ++i)
				{
					// Sneaky obj array trickery to not copy the tab descriptors
					sorted_pages.Add(&m_pages.Item(i));
				}
				sorted_pages.Sort(OrderPageTabInfoBySmallWidthAsc);
				width -= tabsep * (numtabs - 1);
				for(size_t i = 0; i < numtabs; ++i)
				{
					wxRibbonPageTabInfo& info = sorted_pages.Item(i);
					if(info.small_must_have_separator_width * (int)(numtabs - i) <= width)
					{
						info.rect.width = info.small_must_have_separator_width;;
					}
					else
					{
						info.rect.width = width / (numtabs - i);
					}
					width -= info.rect.width;
				}
				for(size_t i = 0; i < numtabs; ++i)
				{
					wxRibbonPageTabInfo& info = m_pages.Item(i);
					info.rect.x = x;
					info.rect.y = y;
					info.rect.height = m_tab_height;
					x += info.rect.width + tabsep;
					sorted_pages.Detach(numtabs - (i + 1));
				}
			}
			else
			{
				// Do (3)
				total_small_width = (smallest_tab_width + tabsep) * numtabs - tabsep;
				int total_delta = total_small_width - m_tabs_total_width_minimum;
				total_small_width = m_tabs_total_width_minimum - tabsep * (numtabs - 1);
				width -= tabsep * (numtabs - 1);
				for(size_t i = 0; i < numtabs; ++i)
				{
					wxRibbonPageTabInfo& info = m_pages.Item(i);
					int delta = smallest_tab_width - info.minimum_width;
					info.rect.x = x;
					info.rect.y = y;
					info.rect.width = info.minimum_width + delta * (width - total_small_width) / total_delta;
					info.rect.height = m_tab_height;

					x += info.rect.width + tabsep;
					total_delta -= delta;
					total_small_width -= info.minimum_width;
					width -= info.rect.width;
				}
			}
		}
	}
}

wxRibbonBar::wxRibbonBar()
{
	m_art = NULL;
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
	SetArtProvider(NULL);
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

	m_art = NULL;
	m_flags = style;
	m_tabs_total_width_ideal = 0;
	m_tabs_total_width_minimum = 0;
	m_tab_margin_left = 50;
	m_tab_margin_right = 20;
	m_tab_height = 20; // initial guess
	m_tab_scroll_amount = 0;
	m_current_page = -1;
	m_current_hovered_page = -1;
	m_tab_scroll_buttons_shown = false;

	SetArtProvider(new wxRibbonDefaultArtProvider);
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
}

void wxRibbonBar::SetArtProvider(wxRibbonArtProvider* art)
{
	delete m_art;
	m_art = art;

	if(m_art)
	{
		m_art->SetFlags(m_flags);
	}
}

void wxRibbonBar::OnPaint(wxPaintEvent& evt)
{
	wxAutoBufferedPaintDC dc(this);

	DoEraseBackground(dc);

	size_t numtabs = m_pages.GetCount();
	for(size_t i = 0; i < numtabs; ++i)
	{
		wxRibbonPageTabInfo& info = m_pages.Item(i);

		dc.DestroyClippingRegion();
		dc.SetClippingRegion(info.rect);
		m_art->DrawTab(dc, this, info);
	}
}

void wxRibbonBar::OnEraseBackground(wxEraseEvent& evt)
{
}

void wxRibbonBar::DoEraseBackground(wxDC& dc)
{
	wxRect tabs(GetClientSize());
	tabs.height = m_tab_height;
	m_art->DrawTabCtrlBackground(dc, this, tabs);
}

void wxRibbonBar::OnSize(wxSizeEvent& evt)
{
	RecalculateTabSizes();
	Refresh();
}

#endif // wxUSE_RIBBON
