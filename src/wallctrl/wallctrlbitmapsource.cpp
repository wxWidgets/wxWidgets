#include "wx/wallctrl/WallCtrlBitmapSource.h"

wxWallCtrlBitmapSource::wxWallCtrlBitmapSource(void)
{
}

wxWallCtrlBitmapSource::~wxWallCtrlBitmapSource(void)
{
}

wxWallCtrlRenderType wxWallCtrlBitmapSource::GetRenderType(const wxWallCtrlItemID& itemId, const wxSize& availableResolution, wxSize& desiredResolution )
{
	return wxWallCtrlRenderType::wxWallCtrlRenderBitmap;
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
	// TODO: Validate ItemID
	return bitmaps[itemID];
}

bool wxWallCtrlBitmapSource::GetItemInfo(const wxWallCtrlItemID& itemID, wxWallCtrlItem& info )
{
	// TODO: Validate ItemID
	info.size.SetHeight(bitmaps[itemID].GetHeight());
	info.size.SetWidth(bitmaps[itemID].GetWidth());
	return true;
}

unsigned wxWallCtrlBitmapSource::GetCount()
{
	return bitmaps.size();
}

void wxWallCtrlBitmapSource::AppendBitmap( wxBitmap bitmap )
{
	// Add the bitmap to the collection and assign it an incremental ID
	bitmaps[bitmaps.size()] = bitmap;
}