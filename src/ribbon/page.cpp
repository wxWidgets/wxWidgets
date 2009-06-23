///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/page.cpp
// Purpose:     Container for ribbon-bar-style interface panels
// Author:      Peter Cawley
// Modified by:
// Created:     2009-05-25
// RCS-ID:      $Id$
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/ribbon/page.h"

#if wxUSE_RIBBON

#include "wx/ribbon/art.h"
#include "wx/ribbon/bar.h"
#include "wx/dcbuffer.h"

#ifndef WX_PRECOMP
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

IMPLEMENT_CLASS(wxRibbonPage, wxRibbonControl)

BEGIN_EVENT_TABLE(wxRibbonPage, wxRibbonControl)
  EVT_ERASE_BACKGROUND(wxRibbonPage::OnEraseBackground)
  EVT_PAINT(wxRibbonPage::OnPaint)
  EVT_SIZE(wxRibbonPage::OnSize)
END_EVENT_TABLE()

wxRibbonPage::wxRibbonPage()
{
	m_scroll_amount = 0;
	m_scroll_buttons_visible = false;
}

wxRibbonPage::wxRibbonPage(wxRibbonBar* parent,
				   wxWindowID id,
				   const wxString& label,
				   const wxBitmap& icon,
				   const wxPoint& pos,
				   const wxSize& size,
				   long style) : wxRibbonControl(parent, id, pos, size, style)
{
	CommonInit(label, icon);
}

wxRibbonPage::~wxRibbonPage()
{
}

bool wxRibbonPage::Create(wxRibbonBar* parent,
				wxWindowID id,
				const wxString& label,
				const wxBitmap& icon,
				const wxPoint& pos,
				const wxSize& size,
				long style)
{
	if(!wxRibbonControl::Create(parent, id, pos, size, style))
		return false;

	CommonInit(label, icon);

	return true;
}

void wxRibbonPage::CommonInit(const wxString& label, const wxBitmap& icon)
{
    SetName(label);

	SetLabel(label);
	m_old_size = wxSize(0, 0);
	m_icon = icon;
	m_scroll_amount = 0;
	m_scroll_buttons_visible = false;

	SetBackgroundStyle(wxBG_STYLE_CUSTOM);

	wxDynamicCast(GetParent(), wxRibbonBar)->AddPage(this);
}

void wxRibbonPage::SetArtProvider(wxRibbonArtProvider* art)
{
	m_art = art;
	for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow* child = node->GetData();
		wxRibbonControl* ribbon_child = wxDynamicCast(child, wxRibbonControl);
		if(ribbon_child)
		{
			ribbon_child->SetArtProvider(art);
		}
	}
}

void wxRibbonPage::OnEraseBackground(wxEraseEvent& evt)
{
	wxDC* dc = evt.GetDC();
	if(dc != NULL)
	{
		m_art->DrawPageBackground(*dc, this, GetClientSize());
	}
	else
	{
		wxClientDC cdc(this);
		m_art->DrawPageBackground(cdc, this, GetClientSize());
	}
}

void wxRibbonPage::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
	// No foreground painting done by the page itself, but a paint DC
	// must be created anyway.
	wxPaintDC dc(this);
}

wxOrientation wxRibbonPage::GetMajorAxis() const
{
	if(m_art && (m_art->GetFlags() & wxRIBBON_BAR_FLOW_VERTICAL))
	{
		return wxVERTICAL;
	}
	else
	{
		return wxHORIZONTAL;
	}
}

void wxRibbonPage::OnSize(wxSizeEvent& evt)
{
	Layout();

	wxMemoryDC temp_dc;
	wxRect invalid_rect = m_art->GetPageBackgroundRedrawArea(temp_dc, this, m_old_size, evt.GetSize());
	Refresh(true, &invalid_rect);

	m_old_size = evt.GetSize();
}

bool wxRibbonPage::Layout()
{
	if(GetChildren().GetCount() == 0)
	{
		return true;
	}

	wxPoint origin(m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_LEFT_SIZE), m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_TOP_SIZE));

	if(GetMajorAxis() == wxHORIZONTAL)
	{
		for(int iteration = 1; iteration <= 2; ++iteration)
		{
			int gap = m_art->GetMetric(wxRIBBON_ART_PANEL_X_SEPARATION_SIZE);
			int height = GetSize().GetHeight() - origin.y - m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_BOTTOM_SIZE);
			for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
				  node;
				  node = node->GetNext() )
			{
				wxWindow* child = node->GetData();
				int w, h;
				child->GetSize(&w, &h);
				child->SetSize(origin.x, origin.y, w, height);
				origin.x += w + gap;
			}
			if(iteration == 1)
			{
				int available_space = GetSize().GetWidth() - m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_RIGHT_SIZE) - origin.x + gap;
				if(available_space > 0)
				{
					if(ExpandPanels(wxHORIZONTAL, available_space))
					{
						break;
					}
				}
				else if (available_space < 0)
				{
					if(!CollapsePanels(wxHORIZONTAL, -available_space))
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
		}
	}
	else
	{
		int gap = m_art->GetMetric(wxRIBBON_ART_PANEL_Y_SEPARATION_SIZE);
		int width = GetSize().GetWidth() - origin.x - m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_RIGHT_SIZE);
		for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
              node;
              node = node->GetNext() )
		{
			wxWindow* child = node->GetData();
			int w, h;
			child->GetSize(&w, &h);
			child->SetSize(origin.x, origin.y, width, h);
			origin.y += h + gap;
		}
	}

	return true;
}

static int GetSizeInOrientation(wxSize size, wxOrientation orientation)
{
	switch(orientation)
	{
	case wxHORIZONTAL: return size.GetWidth();
	case wxVERTICAL: return size.GetHeight();
	case wxBOTH: return size.GetWidth() * size.GetHeight();
	default: return 0;
	}
}

bool wxRibbonPage::ExpandPanels(wxOrientation direction, int maximum_amount)
{
	bool expanded_something = false;
	while(maximum_amount > 0)
	{
		int smallest_size = INT_MAX;
		wxRibbonPanel* smallest_panel = NULL;
		for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
				  node;
				  node = node->GetNext() )
		{
			wxRibbonPanel* panel = wxDynamicCast(node->GetData(), wxRibbonPanel);
			if(panel == NULL)
			{
				continue;
			}
			if(panel->IsSizingContinuous())
			{
				int size = GetSizeInOrientation(panel->GetSize(), direction);
				if(size < smallest_size)
				{
					smallest_size = size;
					smallest_panel = panel;
				}
			}
			else
			{
				wxSize current = panel->GetSize();
				int size = GetSizeInOrientation(current, direction);
				if(size < smallest_size)
				{
					wxSize larger = panel->GetNextLargerSize(direction);
					if(larger != current && GetSizeInOrientation(larger, direction) > size)
					{
						smallest_size = size;
						smallest_panel = panel;
					}
				}
			}
		}
		if(smallest_panel != NULL)
		{
			if(smallest_panel->IsSizingContinuous())
			{
				wxSize size = smallest_panel->GetSize();
				int amount = maximum_amount;
				if(amount > 32)
				{
					// For "large" growth, grow this panel a bit, and then re-allocate
					// the remainder (which may come to this panel again anyway)
					amount = 32;
				}
				if(direction & wxHORIZONTAL)
				{
					size.x += amount;
				}
				if(direction & wxVERTICAL)
				{
					size.y += amount;
				}
				smallest_panel->SetSize(size);
				maximum_amount -= amount;
				expanded_something = true;
			}
			else
			{
				wxSize current = smallest_panel->GetSize();
				wxSize larger = smallest_panel->GetNextLargerSize(direction);
				wxSize delta = larger - current;
				if(GetSizeInOrientation(delta, direction) <= maximum_amount)
				{
					smallest_panel->SetSize(larger);
					maximum_amount -= GetSizeInOrientation(delta, direction);
					expanded_something = true;
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			break;
		}
	}
	if(expanded_something)
	{
		Refresh();
		return true;
	}
	else
	{
		return false;
	}
}

bool wxRibbonPage::CollapsePanels(wxOrientation direction, int minimum_amount)
{
	bool collapsed_something = false;
	while(minimum_amount > 0)
	{
		int largest_size = 0;
		wxRibbonPanel* largest_panel = NULL;
		for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
				  node;
				  node = node->GetNext() )
		{
			wxRibbonPanel* panel = wxDynamicCast(node->GetData(), wxRibbonPanel);
			if(panel == NULL)
			{
				continue;
			}
			if(panel->IsSizingContinuous())
			{
				int size = GetSizeInOrientation(panel->GetSize(), direction);
				if(size > largest_size)
				{
					largest_size = size;
					largest_panel = panel;
				}
			}
			else
			{
				wxSize current = panel->GetSize();
				int size = GetSizeInOrientation(current, direction);
				if(size > largest_size)
				{
					wxSize smaller = panel->GetNextSmallerSize(direction);
					if(smaller != current && GetSizeInOrientation(smaller, direction) < size)
					{
						largest_size = size;
						largest_panel = panel;
					}
				}
			}
		}
		if(largest_panel != NULL)
		{
			if(largest_panel->IsSizingContinuous())
			{
				wxSize size = largest_panel->GetSize();
				int amount = minimum_amount;
				if(amount > 32)
				{
					// For "large" contraction, reduce this panel a bit, and then re-allocate
					// the remainder of the quota (which may come to this panel again anyway)
					amount = 32;
				}
				if(direction & wxHORIZONTAL)
				{
					size.x -= amount;
				}
				if(direction & wxVERTICAL)
				{
					size.y -= amount;
				}
				largest_panel->SetSize(size);
				minimum_amount -= amount;
				collapsed_something = true;
			}
			else
			{
				wxSize current = largest_panel->GetSize();
				wxSize smaller = largest_panel->GetNextSmallerSize(direction);
				wxSize delta = current - smaller;
				largest_panel->SetSize(smaller);
				minimum_amount -= GetSizeInOrientation(delta, direction);
				collapsed_something = true;
			}
		}
		else
		{
			break;
		}
	}
	if(collapsed_something)
	{
		Refresh();
		return true;
	}
	else
	{
		return false;
	}
}

wxSize wxRibbonPage::GetMinSize() const
{
	wxSize min(wxDefaultCoord, wxDefaultCoord);

	for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow* child = node->GetData();
		wxSize child_min(child->GetMinSize());

		min.x = wxMax(min.x, child_min.x);
		min.y = wxMax(min.y, child_min.y);
	}

	if(GetMajorAxis() == wxHORIZONTAL)
	{
		min.x = wxDefaultCoord;
		if(min.y != wxDefaultCoord)
		{
			min.y += m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_TOP_SIZE) + m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_BOTTOM_SIZE);
		}
	}
	else
	{
		if(min.x != wxDefaultCoord)
		{
			min.x += m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_LEFT_SIZE) + m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_RIGHT_SIZE);
		}
		min.y = wxDefaultCoord;
	}

	return min;
}

wxSize wxRibbonPage::DoGetBestSize() const
{
	wxSize best(0, 0);
	size_t count = 0;

	if(GetMajorAxis() == wxHORIZONTAL)
	{
		best.y = wxDefaultCoord;

		for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
          node;
          node = node->GetNext() )
		{
			wxWindow* child = node->GetData();
			wxSize child_best(child->GetBestSize());

			if(child_best.x != wxDefaultCoord)
			{
				best.IncBy(child_best.x, 0);
			}
			best.y = wxMax(best.y, child_best.y);

			++count;
		}

		if(count > 1)
		{
			best.IncBy((count - 1) * m_art->GetMetric(wxRIBBON_ART_PANEL_X_SEPARATION_SIZE), 0);
		}
	}
	else
	{
		best.x = wxDefaultCoord;

		for ( wxWindowList::compatibility_iterator node = GetChildren().GetFirst();
          node;
          node = node->GetNext() )
		{
			wxWindow* child = node->GetData();
			wxSize child_best(child->GetBestSize());

			best.x = wxMax(best.x, child_best.x);
			if(child_best.y != wxDefaultCoord)
			{
				best.IncBy(0, child_best.y);
			}

			++count;
		}

		if(count > 1)
		{
			best.IncBy(0, (count - 1) * m_art->GetMetric(wxRIBBON_ART_PANEL_Y_SEPARATION_SIZE));
		}
	}

	if(best.x != wxDefaultCoord)
	{
		best.x += m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_LEFT_SIZE) + m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_RIGHT_SIZE);
	}
	if(best.y != wxDefaultCoord)
	{
		best.y += m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_TOP_SIZE) + m_art->GetMetric(wxRIBBON_ART_PAGE_BORDER_BOTTOM_SIZE);
	}
	return best;
}

#endif // wxUSE_RIBBON
