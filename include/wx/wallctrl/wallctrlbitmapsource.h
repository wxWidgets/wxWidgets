/////////////////////////////////////////////////////////////////////////////
// Name:        wallctrlbitmapsource.h
// Purpose:     A concrete bitmap datasource for wxWallCtrl
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     09/06/2008
// Copyright:   (c) Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef WX_WALLCTRLBITMAPSOURCE_H
#define WX_WALLCTRLBITMAPSOURCE_H

#include "wallctrldatasource.h"

WX_DECLARE_HASH_MAP(int, wxBitmap, wxIntegerHash, wxIntegerEqual, BitmapMap);

class wxWallCtrlBitmapSource :
	public wxWallCtrlDataSource
{
	DECLARE_DYNAMIC_CLASS(wxWallCtrlBitmapSource)
public:
	wxWallCtrlBitmapSource(void);
	virtual ~wxWallCtrlBitmapSource(void);

	// Queries the item to get its preferred rendering mode
	virtual wxWallCtrlRenderType GetRenderType(const wxWallCtrlItemID& itemId, const wxSize& availableResolution, wxSize& desiredResolution);

	// Renders the specified item to the DC
	virtual bool RenderItem(const wxWallCtrlItemID& itemID, wxDC& dc, const wxSize& dcSize);

	// Retrieves the bitmap of the specified item
	virtual wxBitmap GetBitmap(const wxWallCtrlItemID& itemID);

	// Queries an item to get available information
	virtual bool GetItemInfo(const wxWallCtrlItemID& itemID, wxWallCtrlItem& info);

	// Returns the number of items available in the source
	virtual unsigned GetCount() const;

	// Returns true if data source data changed since last call to HasDataChanged()
	virtual bool HasDataChanged()
	{
		if (m_dataChanged)
		{
			m_dataChanged = false;
			return true;
		}
		
		return false;
	}

	// Returns the starting index of the sequence
	virtual unsigned GetFirstItem() const
	{
		return 0;
	}

	// Flags that data has changed
	virtual void DataChanged()
	{
		m_dataChanged = true;
	}

	// Appends a bitmap at the end of the list and give it an ordered id
	void AppendBitmap(wxBitmap bitmap);

private:
	//WX_DEFINE_ARRAY(wxBitmap, ArrayOfBitmaps);

	BitmapMap m_bitmaps;
	wxBitmap tempTexture;

	// This flag is set to true whenever anything changes in the data
	bool m_dataChanged;
};
#endif