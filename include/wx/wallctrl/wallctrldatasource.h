/////////////////////////////////////////////////////////////////////////////
// Name:        wallctrldatasource.h
// Purpose:     The base for wxWallCtrl data sources
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     09/06/2008
// Copyright:   (c) Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WALLCTRLDATASOURCE_H
#define WX_WALLCTRLDATASOURCE_H

#include "wx/wx.h"
#include "wx/wallctrl/wallctrltypes.h"


// This is an abstract class for wxWallCtrl data sources
// Wall ctrl data items must be supplied in a consecutive sequence of indices
class wxWallCtrlDataSource:
	public wxEvtHandler
{
	DECLARE_DYNAMIC_CLASS(wxWallCtrlDataSource)
public:
	wxWallCtrlDataSource(void);
	virtual ~wxWallCtrlDataSource(void);

	// Returns the item's preferred rendering type
	virtual wxWallCtrlRenderType GetRenderType(const wxWallCtrlItemID& itemId, const wxSize& availableResolution, wxSize& desiredResolution)=0;

	// Renders an item into the supplied DC
	virtual bool RenderItem(const wxWallCtrlItemID& itemId, wxDC& dc, const wxSize& dcSize)=0;

	// Returns a bitmap for the specified item
	virtual wxBitmap wxWallCtrlDataSource::GetBitmap(const wxWallCtrlItemID& itemId) =0;

	// Returns more information about an item, including its dimensions
	virtual bool GetItemInfo(const wxWallCtrlItemID& itemId, wxWallCtrlItem& info)  =0;

	// Returns true if data source data changed since last call to HasDataChanged()
	virtual bool HasDataChanged() =0;

	// Flags that data has changed
	virtual void DataChanged()=0;

	// Returns the number of the items in the source (those that are accessible)
	virtual unsigned GetCount() const =0;

	// Returns the starting index of the sequence
	virtual unsigned GetFirstItem() const = 0;
};
#endif