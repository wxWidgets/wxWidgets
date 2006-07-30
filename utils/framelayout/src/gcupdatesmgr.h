/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     19/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __GCUPDATESMGR_G__
#define __GCUPDATESMGR_G__

#include "controlbar.h"
#include "updatesmgr.h"

#include "garbagec.h"

/*
 * class implements optimized logic for refreshing
 * areas of frame layout - which actually need to be updated. 
 * Is used as default updates-manager by wxFrameLayout.
 *
 * it is called "Garbage Collecting" u.mgr for it's impelmentation
 * tries to find out dependencies between bars, and to order
 * them ito "hierarchy", this hierarchical sorting resembles
 * impelmenation of heap-garbage collectors, which resolve
 * dependencies between referencs.
 *
 * Example: there are situations where the order of moving
 *          the windows does matter:
 *
 *   case 1)                
 *      ------		    ---
 *      | A	 |	 	    |B|  
 *		------	 --->  	| |
 *			---		   	--- ------ 
 *			|B|		   		| A  | 
 *			| |		   		------
 *			---		   		
 *						(future)
 *	   (past)            
 *
 *  past/future positions of A and B windows completely overlapp, i.e.
 *  depend on each other, and there is not solution for
 *  moving the windows witout refereshing both of them,
 *  -- we have cyclic dependency here. The gc. alg will
 *  find this cyclic dependecy and will force "refresh"
 *  after movement.
 *
 *   case 2)
 *
 *      ------		                        
 *      | A	 |	 	                        
 * 		------	 --->                       
 * 			---		                  
 * 			|B|		   		           ------   
 * 			| |		   		           | A  |   
 * 			---		   		           ------         
 *						 ---
 *						 |B|
 *						 | |
 *					     ---
 *
 * 						(future)                
 * 	   (past)       
 *
 *  in this case past/future positions do not overlapp, thus
 *  it's enough only to move windows, without refreshing them.
 *  GC will "notice" it.
 *
 *  there is also third case, when overlapping is partial
 *  in this case the refershing can be also avoided by
 *  moving windows in the order of "most-dependant" towards the
 *  "least-dependent". GC handles this automatically, by
 *  sorting windows by their dependency-level (or "hierarchy")
 *
 *  See garbagec.h for more details of this method, garbagec.h/cpp
 *  implement sorting of generic-depenencies (does not deal
 *  with graphical objects directly)
 *
 *  Summary: improves performance when complex/large windows are 
 *           moved around, by reducing number of repaints. Also helps 
 *           to avoid dirty non-client areas of moved windows
 *           in some sepcal cases of "overlapping anomalies"
 */

class cbGCUpdatesMgr : public cbSimpleUpdatesMgr
{
	DECLARE_DYNAMIC_CLASS( cbGCUpdatesMgr )
protected:

	GarbageCollector mGC;

	void DoRepositionItems( wxList& items );

	void AddItem( wxList&     itemList,
				  cbBarInfo*  pBar, 
		          cbDockPane* pPane,
				  wxRect&     curBounds,
				  wxRect&     prevBounds );

public:

	cbGCUpdatesMgr(void) {}

	cbGCUpdatesMgr( wxFrameLayout* pPanel );

	// notificiactions received from Frame Layout :

	virtual void OnStartChanges();

	// refreshes parts of the frame layout, which need an update
	virtual void UpdateNow();
};

#endif