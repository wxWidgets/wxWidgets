///////////////////////////////////////////////////////////////////////////////
// Name:        src/ribbon/art.cpp
// Purpose:     Art providers for ribbon-bar-style interface
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

#include "wx/ribbon/art.h"

#if wxUSE_RIBBON

#ifndef WX_PRECOMP
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

wxRibbonDefaultArtProvider::wxRibbonDefaultArtProvider()
{
}

wxRibbonDefaultArtProvider::~wxRibbonDefaultArtProvider()
{
}

wxRibbonArtProvider* wxRibbonDefaultArtProvider::Clone()
{
	wxRibbonDefaultArtProvider *copy = new wxRibbonDefaultArtProvider;

	copy->m_tab_separation_size = m_tab_separation_size;

	return copy;
}

void wxRibbonDefaultArtProvider::SetFlags(unsigned int flags)
{
}

int wxRibbonDefaultArtProvider::GetMetric(int id)
{
	switch(id)
	{
		case wxRIBBON_ART_TAB_SEPARATION_SIZE: return m_tab_separation_size;
		default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
	}

	return 0;
}

void wxRibbonDefaultArtProvider::SetMetric(int id, int new_val)
{
	switch(id)
	{
		case wxRIBBON_ART_TAB_SEPARATION_SIZE: m_tab_separation_size = new_val;
		default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
	}
}

void wxRibbonDefaultArtProvider::SetFont(int id, const wxFont& font)
{
	switch(id)
	{
		default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
	}
}

wxFont wxRibbonDefaultArtProvider::GetFont(int id)
{
	switch(id)
	{
		default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
	}

	return wxNullFont;
}

wxColour wxRibbonDefaultArtProvider::GetColour(int id)
{
	switch(id)
	{
		default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
	}

	return wxColour();
}

void wxRibbonDefaultArtProvider::SetColour(int id, const wxColor& colour)
{
	switch(id)
	{
		default: wxFAIL_MSG(wxT("Invalid Metric Ordinal")); break;
	}
}

void wxRibbonDefaultArtProvider::DrawTabCtrlBackground(
						wxDC& dc,
						wxWindow* wnd,
						const wxRect& rect)
{
}

void wxRibbonDefaultArtProvider::DrawPageBackground(
						wxDC& dc,
						wxWindow* wnd,
						const wxRect& rect)
{
}

void wxRibbonDefaultArtProvider::GetBarTabWidth(
						wxDC& dc,
						wxWindow* wnd,
						const wxString& label,
						const wxBitmap& bitmap,
						int* ideal,
						int* small_begin_need_separator,
						int* small_must_have_separator,
						int* minimum)
{
}

#endif // wxUSE_RIBBON
