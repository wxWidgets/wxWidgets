/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas (@Lithuania)
// Modified by:
// Created:     19/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __UPDATESMGR_G__
#define __UPDATESMGR_G__

#include "controlbar.h"

/*
 * class implements slightly optimized logic for refreshing
 * areas of frame layout - which actually need to be updated. 
 */

class cbSimpleUpdatesMgr : public cbUpdatesManagerBase
{
	DECLARE_DYNAMIC_CLASS( cbSimpleUpdatesMgr )
protected:

	bool WasChanged( cbUpdateMgrData& data, wxRect& currentBounds );

public:

	cbSimpleUpdatesMgr(void) {}

	cbSimpleUpdatesMgr( wxFrameLayout* pPanel );

	// notificiactions received from Frame Layout (in the order, in which
	// they usually would be invoked)

	virtual void OnStartChanges();

	virtual void OnRowWillChange( cbRowInfo* pRow, cbDockPane* pInPane );
	virtual void OnBarWillChange( cbBarInfo* pBar, cbRowInfo* pInRow, cbDockPane* pInPane );
	virtual void OnPaneMarginsWillChange( cbDockPane* pPane );
	virtual void OnPaneWillChange( cbDockPane* pPane );

	virtual void OnFinishChanges();

	// refreshes parts of the frame layout, which need an update
	virtual void UpdateNow();
};

#endif