#pragma once
#include "wallctrldatasource.h"

class wxWallCtrlBitmapSource :
	public wxWallCtrlDataSource
{
public:
	wxWallCtrlBitmapSource(void);
	virtual ~wxWallCtrlBitmapSource(void);

	virtual wxWallCtrlRenderType GetRenderType(wxWallCtrlItemID& itemId, const wxSize& availableResolution, wxSize& desiredResolution)
	{
		return wxWallCtrlRenderType::wxWallCtrlRenderBitmap;
	}

	virtual bool RenderItem(wxWallCtrlItemID& itemID, wxDC& dc, const wxSize& dcSize)
	{
		// TODO: Do error checking for itemID existence if needed
		// Create a temp memory DC to hold the bitmap
		wxMemoryDC tempDC;
		tempDC.SelectObject(GetBitmap(itemID));

		dc.Blit(0, 0, dcSize.GetWidth(), dcSize.GetHeight(), &tempDC, 0, 0);

		return true;
	}

	virtual wxBitmap wxWallCtrlDataSource::GetBitmap(wxWallCtrlItemID& itemID)
	{
		return bitmaps[itemID];
	}

	virtual bool GetItemInfo(wxWallCtrlItemID& itemID, wxWallCtrlItem& info)
	{
		info.size.SetHeight(bitmaps[itemID].GetHeight());
		info.size.SetWidth(bitmaps[itemID].GetWidth());
		return true;
	}

	// Returns the number of items available in the source
	virtual unsigned GetCount()
	{
		return bitmaps.size();
	}

	// Appends a bitmap at the end of the list and give it an ordered id
	void AppendBitmap(wxBitmap bitmap)
	{
		bitmaps[bitmaps.size()] = bitmap;
	}

	


private:
	//WX_DEFINE_ARRAY(wxBitmap, ArrayOfBitmaps);
	WX_DECLARE_HASH_MAP(int, wxBitmap, wxIntegerHash, wxIntegerEqual, BitmapMap);

	BitmapMap bitmaps;
	wxBitmap tempTexture;
};
