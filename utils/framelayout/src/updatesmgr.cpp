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

#ifdef __GNUG__
#pragma implementation "updatesmgr.h"
// #pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "updatesmgr.h"

// helper function

static inline bool rect_hits_rect( const wxRect& r1, const wxRect& r2 )
{
	if ( ( r2.x >= r1.x && r2.x <= r1.x + r1.width ) ||
		 ( r1.x >= r2.x && r1.x <= r2.x + r2.width ) )

		if ( ( r2.y >= r1.y && r2.y <= r1.y + r1.height ) ||
			 ( r1.y >= r2.y && r1.y <= r2.y + r2.height ) )
			 
			return 1;

	return 0;
}

/***** Implementation for class cbSimpleUpdatesMgr *****/

IMPLEMENT_DYNAMIC_CLASS( cbSimpleUpdatesMgr, cbUpdatesManagerBase )

cbSimpleUpdatesMgr::cbSimpleUpdatesMgr( wxFrameLayout* pPanel )
	: cbUpdatesManagerBase( pPanel )
{}

bool cbSimpleUpdatesMgr::WasChanged( cbUpdateMgrData& data, wxRect& currentBounds )
{
	return (   data.IsDirty() ||

		     ( data.mPrevBounds.x      != currentBounds.x     ||
		       data.mPrevBounds.y      != currentBounds.y     ||
			   data.mPrevBounds.width  != currentBounds.width ||
			   data.mPrevBounds.height != currentBounds.height  )
		   );
}

void cbSimpleUpdatesMgr::OnStartChanges()
{
	// memorize states of ALL items in the layout -
	// this is quite excessive, but OK for the simple 
	// implementation of updates manager

	mpLayout->GetPrevClientRect() = mpLayout->GetClientRect();

	cbDockPane** panes = mpLayout->GetPanesArray();

	for( int n = 0; n != MAX_PANES; ++n )
	{
		cbDockPane& pane = *panes[n];
		// store pane state
		pane.mUMgrData.StoreItemState( pane.mBoundsInParent );
		pane.mUMgrData.SetDirty( FALSE );

		for( size_t i = 0; i != pane.GetRowList().Count(); ++i )
		{
			cbRowInfo& row = *pane.GetRowList()[ i ];

			// store row state
			row.mUMgrData.StoreItemState( row.mBoundsInParent );
			row.mUMgrData.SetDirty( FALSE );

			for( size_t k = 0; k != row.mBars.Count(); ++k )
			{
				cbBarInfo& bar = *row.mBars[ k ];

				// store bar state
				bar.mUMgrData.StoreItemState( bar.mBoundsInParent );
				bar.mUMgrData.SetDirty( FALSE );
			}
		}
	}
}

void cbSimpleUpdatesMgr::OnFinishChanges()
{
	// nothing here, could be overriden by more sophisticated updates-managers
}

void cbSimpleUpdatesMgr::OnRowWillChange( cbRowInfo* pRow, cbDockPane* pInPane )
{
	// -/-
}

void cbSimpleUpdatesMgr::OnBarWillChange( cbBarInfo* pBar, 
										  cbRowInfo* pInRow, cbDockPane* pInPane )
{
	// -/-
}

void cbSimpleUpdatesMgr::OnPaneMarginsWillChange( cbDockPane* pPane )
{
	// -/-
}

void cbSimpleUpdatesMgr::OnPaneWillChange( cbDockPane* pPane )
{
	// -/-
}

void cbSimpleUpdatesMgr::UpdateNow()
{
	cbDockPane** panes = mpLayout->GetPanesArray();

	wxRect& r1 = mpLayout->GetClientRect();
	wxRect& r2 = mpLayout->GetPrevClientRect();

	// detect changes in client window's area

	bool clientWindowChanged = ( r1.x      != r2.x     ||
		                         r1.y      != r2.y     ||
								 r1.width  != r2.width ||
								 r1.height != r2.height );

	// step #1 - detect changes in each row of each pane,
	//           and repaint decorations around changed windows

	wxList mBarsToRefresh;
	wxList mPanesList;

	for( int n = 0; n != MAX_PANES; ++n )
	{
		cbDockPane& pane = *(panes[n]);

		bool paneChanged = WasChanged( pane.mUMgrData, pane.mBoundsInParent );

		if ( paneChanged )
		{
			wxClientDC dc( &mpLayout->GetParentFrame() );
			pane.PaintPaneBackground( dc );
		}

		wxRect realBounds;

		for( size_t i = 0; i != pane.GetRowList().Count(); ++i )
		{
			cbRowInfo& row = *pane.GetRowList()[ i ];

			wxDC* pDc = NULL;

			bool rowChanged = FALSE;
			bool rowBkPainted  = FALSE;

			// FIXME:: the below should not be fixed
			cbBarInfo* barsToRepaint[256];

			// number of bars, that were changed in the current row
			int nBars = 0; 

			if ( WasChanged( row.mUMgrData, row.mBoundsInParent ) )
			
				rowChanged = TRUE;
			else
				for( size_t k = 0; k != row.mBars.Count(); ++k )

					if ( WasChanged( row.mBars[k]->mUMgrData, 
						 row.mBars[k]->mBoundsInParent ) 
					   )
					
						barsToRepaint[nBars++] = row.mBars[k];

			if ( nBars || rowChanged )
			{
				realBounds = row.mBoundsInParent;

				// include 1-pixel thick shades around the row
				realBounds.x -= 1;
				realBounds.y -= 1;
				realBounds.width  += 2;
				realBounds.height += 2;

				pDc = pane.StartDrawInArea( realBounds );
			}

			if ( rowChanged )
			{
				// postphone the resizing and refreshing the changed
				// bar windows

				for( size_t k = 0; k != row.mBars.Count(); ++k )
				{
					mBarsToRefresh.Append( (wxObject*)row.mBars[k] );
					mPanesList.Append( &pane );
				}

				// draw only their decorations now

				pane.PaintRow( &row, *pDc );
			}
			else
			if ( nBars != 0 )
			{
				for( int i = 0; i != nBars; ++i )
				{
					// postphone the resizement and refreshing the changed
					// bar windows

					mBarsToRefresh.Append( (wxObject*)barsToRepaint[i] );
					mPanesList.Append( &pane );
				}

				// redraw decorations of entire row, regardless of how much
				// of the bars were changed
				pane.PaintRow( &row, *pDc );
			}

			if ( pDc )
		
				pane.FinishDrawInArea( realBounds );
		} // end of while

		if ( paneChanged )
		{
			wxClientDC dc( &mpLayout->GetParentFrame() );
			pane.PaintPaneDecorations( dc );
		}

	} // end of for

	if ( clientWindowChanged )
	{
		mpLayout->PositionClientWindow();
		// ptr to client-window object is "marked" as 0
	}

	// step #2 - do ordered refreshing and resizing of bar window objects now

	wxNode* pNode     = mBarsToRefresh.First();
	wxNode* pPaneNode = mPanesList.First();

	while( pNode )
	{
		cbBarInfo*  pBar  = (cbBarInfo*) pNode->Data();
		cbDockPane* pPane = (cbDockPane*)pPaneNode->Data();

		pPane->SizeBar( pBar );

		pNode     = pNode->Next();
		pPaneNode = pPaneNode->Next();
	}

	pNode = mBarsToRefresh.First();

	while( pNode )
	{
		cbBarInfo* pBar = (cbBarInfo*)pNode->Data();

		if ( pBar->mpBarWnd ) 
		{
			pBar->mpBarWnd->Refresh();

			// FIXME::
			//info.mpBarWnd->Show(FALSE);
			//info.mpBarWnd->Show(TRUE);
		}

		pNode  = pNode->Next();
	}

	if ( clientWindowChanged )
	{
		// FIXME:: excessive?

		mpLayout->GetFrameClient()->Refresh();
	}
}
