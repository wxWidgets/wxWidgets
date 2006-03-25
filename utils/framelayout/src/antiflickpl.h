/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas (@Lithuania)
// Modified by:
// Created:     23/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __ANTIFLICKPL_G__
#define __ANTIFLICKPL_G__

#include "controlbar.h"

class cbAntiflickerPlugin : public cbPluginBase
{
	DECLARE_DYNAMIC_CLASS( cbAntiflickerPlugin )
protected:
	// double-buffers are shared "resource" among all instances of 
	// antiflicker plugin within the application 
	//
	// TODO:: locking should be implemented, for multithreaded GUIs

	static wxBitmap*   mpVertBuf;
	static wxBitmap*   mpHorizBuf;
	static wxMemoryDC* mpVertBufDc;
	static wxMemoryDC* mpHorizBufDc;

	static int mRefCount;

	wxDC*  mpLRUBufDc; // last-reacently-used buffer
	wxRect mLRUArea;   // last-reacently-used area

protected:
	// returns NULL, if sutable buffer is not present
	wxDC* FindSuitableBuffer( const wxRect& forArea );
	wxDC* AllocNewBuffer( const wxRect& forArea );
	wxDC& GetWindowDC();

	wxDC& GetClientDC();
public:

	cbAntiflickerPlugin(void);

	cbAntiflickerPlugin( wxFrameLayout* pPanel, int paneMask = wxALL_PANES );

	virtual ~cbAntiflickerPlugin();

	// handlers for plugin events

	void OnStartDrawInArea ( cbStartDrawInAreaEvent& event );
	void OnFinishDrawInArea( cbFinishDrawInAreaEvent& event );

	DECLARE_EVENT_TABLE()
};

#endif