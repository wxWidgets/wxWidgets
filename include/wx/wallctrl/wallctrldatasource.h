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

// TODO: Decide where to put these types

typedef unsigned wxWallCtrlItemID;

enum wxWallCtrlRenderType
{
	wxWallCtrlRenderBitmap,
	wxWallCtrlRenderDC
};

//typedef wxBitmap wxWallCtrlItem;
struct wxWallCtrlItem
{
	wxSize size;
};

// This is an abstract class for wxWallCtrl data sources
// Wall ctrl data items must be supplied in a consecutive sequence of indices
class wxWallCtrlDataSource:
	public wxEvtHandler
{
	DECLARE_DYNAMIC_CLASS(wxWallCtrlDataSource)
public:
	wxWallCtrlDataSource(void);
	virtual ~wxWallCtrlDataSource(void);

	virtual wxWallCtrlRenderType GetRenderType(const wxWallCtrlItemID& itemId, const wxSize& availableResolution, wxSize& desiredResolution)=0;

	virtual bool RenderItem(const wxWallCtrlItemID& itemId, wxDC& dc, const wxSize& dcSize)=0;
	virtual wxBitmap wxWallCtrlDataSource::GetBitmap(const wxWallCtrlItemID& itemId)=0;
	virtual bool GetItemInfo(const wxWallCtrlItemID& itemId, wxWallCtrlItem& info)=0;

	// Returns the number of the items in the source (those that are accessible)
	virtual unsigned GetCount()=0;

	// Returns the starting index of the sequence
	virtual unsigned GetFirstItem();
};
#endif