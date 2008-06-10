// Mokhtar M. Khorshid

#pragma once

#include "wx/wx.h"

// TODO: Decide where to put these types

typedef unsigned wxWallCtrlItemID;

enum wxWallCtrlRenderType
{
	wxWallCtrlRenderBitmap
};

//typedef wxBitmap wxWallCtrlItem;
struct wxWallCtrlItem
{
	wxSize size;
};

// This is an abstract class for wxWallCtrl data sources
// Wall ctrl data items must be supplied in a consecutive sequence of indices
class wxWallCtrlDataSource
{
public:
	wxWallCtrlDataSource(void);
	virtual ~wxWallCtrlDataSource(void);

	virtual wxWallCtrlRenderType GetRenderType(wxWallCtrlItemID& itemId, const wxSize& availableResolution, wxSize& desiredResolution)=0;

	virtual bool RenderItem(wxWallCtrlItemID& itemId, wxDC& dc, const wxSize& dcSize)=0;
	virtual wxBitmap wxWallCtrlDataSource::GetBitmap(wxWallCtrlItemID& itemId)=0;
	virtual bool GetItemInfo(wxWallCtrlItemID& itemId, wxWallCtrlItem& info)=0;

	// Returns the number of the items in the source (those that are accessible)
	virtual unsigned GetCount()=0;

	// Returns the starting index of the sequence
	virtual unsigned GetFirstItem()
	{
		return 0;
	}
};
