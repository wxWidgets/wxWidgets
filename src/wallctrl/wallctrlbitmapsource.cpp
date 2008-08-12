/////////////////////////////////////////////////////////////////////////////
// Name:        wallctrlbitmapsource.cpp
// Purpose:     A concrete bitmap datasource for wxWallCtrl
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     09/06/2008
// Copyright:   (c) Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#include "wx/wallctrl/WallCtrlBitmapSource.h"


IMPLEMENT_CLASS(wxWallCtrlBitmapSource, wxWallCtrlDataSource)

wxWallCtrlBitmapSource::wxWallCtrlBitmapSource(void)
{
	m_dataChanged = false;
}

wxWallCtrlBitmapSource::~wxWallCtrlBitmapSource(void)
{
}

wxWallCtrlRenderType wxWallCtrlBitmapSource::GetRenderType(const wxWallCtrlItemID& itemId, const wxSize& availableResolution, wxSize& desiredResolution )
{
	// Both DC & Bitmap rendering types would work, but Bitmaps are faster
	return wxWallCtrlRenderBitmap;
}

bool wxWallCtrlBitmapSource::RenderItem(const wxWallCtrlItemID& itemID, wxDC& dc, const wxSize& dcSize )
{
	// TODO: Do error checking for itemID existence if needed
	// Create a temp memory DC to hold the bitmap
	wxMemoryDC tempDC;
	tempDC.SelectObject(GetBitmap(itemID));

	// Blit the bitmap to the DC
	dc.Blit(0, 0, dcSize.GetWidth(), dcSize.GetHeight(), &tempDC, 0, 0);

	return true;
}

wxBitmap wxWallCtrlBitmapSource::GetBitmap(const wxWallCtrlItemID& itemID )
{
	if (m_bitmaps.count(itemID) == 0)
	{
		wxLogError(wxT("Invalid Item ID specified for wxWallCtrlBitmapSource"));
		wxBitmap invalid;
		return invalid;
	}
	return m_bitmaps[itemID];
}

bool wxWallCtrlBitmapSource::GetItemInfo(const wxWallCtrlItemID& itemID, wxWallCtrlItem& info )
{
	if (m_bitmaps.count(itemID) == 0)
	{
		return false;
	}
	info.size.SetHeight(m_bitmaps[itemID].GetHeight());
	info.size.SetWidth(m_bitmaps[itemID].GetWidth());
	return true;
}

unsigned wxWallCtrlBitmapSource::GetCount() const
{
		int x = m_bitmaps.size();
	return m_bitmaps.size();
}

void wxWallCtrlBitmapSource::AppendBitmap( wxBitmap bitmap )
{
	// Add the bitmap to the collection and assign it an incremental ID
	m_bitmaps[m_bitmaps.size()] = bitmap;

	// Flag that data has changed
	DataChanged();
}

bool wxWallCtrlBitmapSource::HasDataChanged()
{
	if (m_dataChanged)
	{
		m_dataChanged = false;
		return true;
	}

	return false;
}

unsigned wxWallCtrlBitmapSource::GetFirstItem() const
{
	return 0;
}

void wxWallCtrlBitmapSource::DataChanged()
{
	m_dataChanged = true;
}

void wxWallCtrlBitmapSource::ClearDataSource()
{
	m_bitmaps.clear();
	DataChanged();
}