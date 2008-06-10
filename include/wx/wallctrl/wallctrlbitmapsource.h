#pragma once
#include "wallctrldatasource.h"

class wxWallCtrlBitmapSource :
	public wxWallCtrlDataSource
{
public:
	wxWallCtrlBitmapSource(void);
	virtual ~wxWallCtrlBitmapSource(void);

	// Queries the item to get its preferred rendering mode
	virtual wxWallCtrlRenderType GetRenderType(wxWallCtrlItemID& itemId, const wxSize& availableResolution, wxSize& desiredResolution);

	// Renders the specified item to the DC
	virtual bool RenderItem(wxWallCtrlItemID& itemID, wxDC& dc, const wxSize& dcSize);

	// Retrieves the bitmap of the specified item
	virtual wxBitmap GetBitmap(wxWallCtrlItemID& itemID);

	// Queries an item to get available information
	virtual bool GetItemInfo(wxWallCtrlItemID& itemID, wxWallCtrlItem& info);

	// Returns the number of items available in the source
	virtual unsigned GetCount();

	// Appends a bitmap at the end of the list and give it an ordered id
	void AppendBitmap(wxBitmap bitmap);

	


private:
	//WX_DEFINE_ARRAY(wxBitmap, ArrayOfBitmaps);
	WX_DECLARE_HASH_MAP(int, wxBitmap, wxIntegerHash, wxIntegerEqual, BitmapMap);

	BitmapMap bitmaps;
	wxBitmap tempTexture;
};
