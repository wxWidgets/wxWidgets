/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     02/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __ROWLAYOUTPL_G__
#define __ROWLAYOUTPL_G__

#include "controlbar.h"

/*
 * Simple implementaiton of plugin, which handles row-layouting
 * requests sent from Frame Layout
 */

class cbRowLayoutPlugin : public cbPluginBase
{
	DECLARE_DYNAMIC_CLASS( cbRowLayoutPlugin )
protected:
	cbDockPane* mpPane; // is set up temorarely, while handling event
protected:

	// not-fixed-bars layouting related helpers

	void FitBarsToRange( int from, int till, cbBarInfo* pTheBar, cbRowInfo* pRow ); 
	void RelayoutNotFixedBarsAround( cbBarInfo* pTheBar, cbRowInfo* pRow ); 
	void MinimzeNotFixedBars( cbRowInfo* pRow, cbBarInfo* pBarToPreserve );
	int  GetRowFreeSpace( cbRowInfo* pRow );
	
	void RecalcLenghtRatios( cbRowInfo* pRow );
	void ApplyLenghtRatios( cbRowInfo* pRow );
	void ExpandNotFixedBars( cbRowInfo* pRow );
	void AdjustLenghtOfInserted( cbRowInfo* pRow, cbBarInfo* pTheBar );

	void DetectBarHandles( cbRowInfo* pRow );
	void CheckIfAtTheBoundary( cbBarInfo* pTheBar, cbRowInfo& rowInfo );


	// row-layouting helpers (simulate "bar-friction")

	int  CalcRowHeight( cbRowInfo& row );
	void LayoutItemsVertically( cbRowInfo& row );

	void StickRightSideBars( cbBarInfo* pToBar );

	void SlideLeftSideBars ( cbBarInfo* pTheBar );
	void SlideRightSideBars( cbBarInfo* pTheBar );

	void ShiftLeftTrashold ( cbBarInfo* pTheBar, cbRowInfo& row );
	void ShiftRightTrashold( cbBarInfo* pTheBar, cbRowInfo& row );

	void InsertBefore( cbBarInfo* pBeforeBar, 
					   cbBarInfo* pTheBar,
					   cbRowInfo& row
					 );

	void DoInsertBar( cbBarInfo* pTheBar, cbRowInfo& row );

public:

	cbRowLayoutPlugin(void);

	cbRowLayoutPlugin( wxFrameLayout* pPanel, int paneMask = wxALL_PANES );

	// event handlers

	void OnResizeRow ( cbResizeRowEvent&  event );
	void OnInsertBar ( cbInsertBarEvent&  event );
	void OnRemoveBar ( cbRemoveBarEvent&  event );
	void OnLayoutRow ( cbLayoutRowEvent&  event );
	void OnLayoutRows( cbLayoutRowsEvent& event );

	DECLARE_EVENT_TABLE()
};

#endif