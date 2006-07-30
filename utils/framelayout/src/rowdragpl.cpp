/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     06/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "rowdragpl.h"
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

#include "rowdragpl.h"

#define MINIMAL_ROW_DRAG_OFS  5

// parameters for row-hints of NC-look

#define TRIANGLE_OFFSET       2
#define TRIANGLE_TO_PAT_GAP   2
#define PAT_OFFSET            2
#define COLLAPSED_ICON_WIDTH  45
#define COLLAPSED_ICON_HEIGHT 9
#define ROW_DRAG_HINT_WIDTH   10
#define ICON_TRIAN_WIDTH      6
#define ICON_TRIAN_HEIGHT     3

/***** Implementaiton for class cbHiddenBarInfo *****/

IMPLEMENT_DYNAMIC_CLASS( cbHiddenBarInfo, wxObject )

/***** Implementaiton for class cbRowDragPlugin *****/

IMPLEMENT_DYNAMIC_CLASS( cbRowDragPlugin, cbPluginBase )

BEGIN_EVENT_TABLE( cbRowDragPlugin, cbPluginBase )

	EVT_PL_LEFT_DOWN		  ( cbRowDragPlugin::OnLButtonDown        )
	EVT_PL_LEFT_UP			  ( cbRowDragPlugin::OnLButtonUp          )
	EVT_PL_MOTION			  ( cbRowDragPlugin::OnMouseMove          )

	EVT_PL_DRAW_PANE_DECOR    ( cbRowDragPlugin::OnDrawPaneBackground )

END_EVENT_TABLE()

// FIXME:: how to eliminated these cut&pasted constructors?

cbRowDragPlugin::cbRowDragPlugin(void)

	: mDragStarted         ( FALSE ),
	  mDecisionMode        ( FALSE ),
	  mCurDragOfs          ( 0 ),
	  mpPaneImage          ( NULL ),
	  mpRowImage           ( NULL ),
	  mpCombinedImage 	   ( NULL ),

	  mpRowInFocus         ( NULL ),
	  mCollapsedIconInFocus( -1 ),

	  mCaptureIsOn         ( FALSE ),

	  mTrianInnerColor     ( 0,0,255 ),
	  mHightColor          ( 192, 192, 255 ),
	  mLowColor            ( 192, 192, 192 ),
	  mTrianInnerPen       ( mTrianInnerColor, 1, wxSOLID ),

	  mSvTopMargin         ( -1 ),
	  mSvBottomMargin      ( -1 ),
	  mSvLeftMargin        ( -1 ),
	  mSvRightMargin       ( -1 ),
	  mpPane               ( NULL )
{
}

cbRowDragPlugin::cbRowDragPlugin( wxFrameLayout* pLayout, int paneMask )

	: cbPluginBase( pLayout, paneMask ),
	  
      mDragStarted         ( FALSE ),
	  mDecisionMode        ( FALSE ),
	  mCurDragOfs          ( 0 ),
	  mpPaneImage          ( NULL ),
	  mpRowImage           ( NULL ),
	  mpCombinedImage 	   ( NULL ),

	  mpRowInFocus         ( NULL ),
	  mCollapsedIconInFocus( -1 ),

	  mCaptureIsOn         ( FALSE ),

	  mTrianInnerColor     ( 0,0,255 ),
	  mHightColor          ( 192, 192, 255 ),
	  mLowColor            ( 192, 192, 192 ),
	  mTrianInnerPen       ( mTrianInnerColor, 1, wxSOLID ),

	  mSvTopMargin         ( -1 ),
	  mSvBottomMargin      ( -1 ),
	  mSvLeftMargin        ( -1 ),
	  mSvRightMargin       ( -1 ),
	  mpPane               ( NULL )
{
}

cbRowDragPlugin::~cbRowDragPlugin()
{
}

// handlers for plugin events
void cbRowDragPlugin::OnMouseMove( cbMotionEvent& event )
{
	// short-cuts
	wxPoint pos = event.mPos;
	mpPane      = event.mpPane;

	mpPane->PaneToFrame( &pos.x, &pos.y );

	if ( !mDragStarted )
	{
		if ( mDecisionMode && mpRowInFocus )
		{
			int ofs;

			if ( mpPane->IsHorizontal() )

				ofs = pos.y - mDragOrigin.y;
			else
				ofs = pos.x - mDragOrigin.x;

			// check if the item was dragged sufficeintly
			// far, enough to consider that user really intends 
			// to drag it

			if ( ofs >= MINIMAL_ROW_DRAG_OFS ||
				 ofs <= -MINIMAL_ROW_DRAG_OFS )
			{
				// DBG::
				//.wxPoint pos = event.mPos;
				//wxPoint drg = mDragOrigin;
				//int dif = event.mPos.x - mDragOrigin.x;

				mDragStarted  = TRUE;
				mDecisionMode = FALSE;
				mDragOrigin   = pos;

				PrepareForRowDrag();
				return;
			}

			// this plugin "eats" all mouse input while item is dragged,
			return;
		}

		cbRowInfo* pRow = GetFirstRow();

		bool focusFound = FALSE;

		while( pRow )
		{
			if ( HitTestRowDragHint( pRow, pos ) )
			{
				CheckPrevItemInFocus( pRow, -1 );
				SetMouseCapture( TRUE );

				focusFound = TRUE;

				mpRowInFocus          = pRow;
				mCollapsedIconInFocus = -1;
				break;
			}

			pRow = pRow->mpNext;
		}

		if ( !focusFound )
		{
			int hrCnt = GetHRowsCountForPane( event.mpPane );

			for( int i = 0; i != hrCnt; ++i )
			{
				if ( HitTestCollapsedRowIcon( i, pos ) )
				{
					CheckPrevItemInFocus( NULL, i );
					SetMouseCapture( TRUE );

					focusFound = TRUE;

					mCollapsedIconInFocus = i;
					mpRowInFocus          = NULL;
					break;
				}
			}
		}

		if ( !focusFound && ItemIsInFocus() )
		{
			// kill focus from item previousely been in focus
			UnhiglightItemInFocus();

			mpRowInFocus          = NULL;
			mCollapsedIconInFocus = -1;
			SetMouseCapture( FALSE );
		}

		if ( !ItemIsInFocus() ) 

				// delegate it to other plugins
				event.Skip();
	}
	else
	{
		// otherwise mouse pointer moves, when dragging is started

		if ( mpPane->IsHorizontal() )
		{
			// DBG::
			wxPoint p = event.mPos;
			wxPoint d = mDragOrigin;
			int dif = event.mPos.x - mDragOrigin.x;

			// row is dragged up or down;
			ShowDraggedRow( pos.y - mDragOrigin.y );
		}
		else
		{
			// DBG::
			wxPoint p = event.mPos;
			wxPoint d = mDragOrigin;
			int dif = event.mPos.x - mDragOrigin.x;

			// row is dragged left or right
			ShowDraggedRow( pos.x - mDragOrigin.x );
		}

		// this plugin "eats" all mouse input while item is dragged,
	}
}

void cbRowDragPlugin::OnLButtonDown( cbLeftDownEvent& event )
{
	mpPane = event.mpPane;

	// DBG::
	wxASSERT( !mDragStarted && !mDecisionMode );

	if ( ItemIsInFocus() )
	{
		mDecisionMode = TRUE;

		wxPoint pos = event.mPos;
		mpPane->PaneToFrame( &pos.x, &pos.y );

		mDragOrigin = pos;

		SetMouseCapture( TRUE );
	}
	else
		// propagate event to other plugins
		event.Skip();
}

void cbRowDragPlugin::OnLButtonUp  ( cbLeftUpEvent& event )
{
	if ( !mDragStarted && !mDecisionMode ) 
	{
		event.Skip();
		return;
	}

	mpPane = event.mpPane;

	if ( mDecisionMode )
	{
		cbDockPane* pPane = mpPane;

		SetMouseCapture( FALSE );

		mDecisionMode = FALSE;
		mDragStarted  = FALSE;

		wxPoint frmPos = event.mPos;
		pPane->PaneToFrame( &frmPos.x, &frmPos.y );

		if ( mpRowInFocus ) 
		{
			CollapseRow( mpRowInFocus );
			mpRowInFocus = 0;
		}
		else
		{
			ExpandRow( mCollapsedIconInFocus );
			mCollapsedIconInFocus = -1;
		}

		mpRowInFocus  = NULL;
		mpPane = pPane;

		pPane->FrameToPane( &frmPos.x, &frmPos.y );

		// give it another try after relayouting bars

		cbMotionEvent moveEvt( frmPos, pPane );
		this->OnMouseMove( moveEvt );

		// this plugin has "eaten" the mouse-up event

		return;
	}
	else
	{
		// otherwise, the dragged row was dropped, determine
		// where to insert it

		// restore initial pane appearence
		ShowPaneImage();
		FinishOnScreenDraw();

		cbRowInfo* pRow = GetFirstRow();

		mpLayout->GetUpdatesManager().OnStartChanges();

		pRow->mUMgrData.SetDirty(TRUE);

		cbBarInfo* pBar = mpRowInFocus->mBars[0];

		while ( pBar )
		{
			pBar->mUMgrData.SetDirty(TRUE);

			if ( pBar->mpBarWnd )
			{
				// do complete refresh
				pBar->mpBarWnd->Show(FALSE);
				pBar->mpBarWnd->Show(TRUE);
			}

			pBar = pBar->mpNext;
		}

		while( pRow )
		{
			if ( mCurDragOfs < pRow->mRowY )
			{
				InsertDraggedRowBefore( pRow );
				break;
			}

			pRow = pRow->mpNext;
		}

		if ( pRow == NULL ) InsertDraggedRowBefore( NULL );

		mpRowInFocus = NULL;

		mpLayout->RecalcLayout(FALSE);

		// finish change "transaction"
		mpLayout->GetUpdatesManager().OnFinishChanges();
		mpLayout->GetUpdatesManager().UpdateNow();

		// finish drag action
		SetMouseCapture( FALSE );
		mDragStarted = FALSE;
	}
}

void cbRowDragPlugin::OnDrawPaneBackground ( cbDrawPaneDecorEvent& event )
{
	mpPane = event.mpPane;

	// FIXME:: this may harm operation of other plugins

	if ( GetNextHandler() && mpPane->GetRowList().GetCount() )
	{
		// first, let other plugins add their decorations now
	
		GetNextHandler()->ProcessEvent( event );
		event.Skip(FALSE);
	}

	wxClientDC dc( &mpLayout->GetParentFrame() );

	dc.SetClippingRegion( mpPane->mBoundsInParent.x,
						  mpPane->mBoundsInParent.y,
						  mpPane->mBoundsInParent.width,
						  mpPane->mBoundsInParent.height );

	int cnt = GetHRowsCountForPane( event.mpPane );

	if ( cnt > 0 ) 

		DrawCollapsedRowsBorder( dc );

	if ( mpPane->GetRowList().GetCount() )
	
		DrawRowsDragHintsBorder( dc );

	cbRowInfo* pRow = GetFirstRow();

	while( pRow )
	{
		DrawRowDragHint( pRow, dc, FALSE );
		pRow = pRow->mpNext;
	}

	for( int i = 0; i != cnt; ++i )

		DrawCollapsedRowIcon(i, dc, FALSE );
}

int cbRowDragPlugin::GetHRowsCountForPane( cbDockPane* pPane )
{
	wxNode* pNode = mHiddenBars.First();

	int maxIconNo = -1;

	while( pNode )
	{
		cbHiddenBarInfo* pHBInfo = (cbHiddenBarInfo*)pNode->Data();

		if ( pHBInfo->mAlignment == pPane->mAlignment )

			maxIconNo = wxMax( maxIconNo, pHBInfo->mIconNo );

		pNode = pNode->Next();
	}

	return ( maxIconNo + 1 );
}

int cbRowDragPlugin::GetCollapsedRowIconHeight()
{
	return COLLAPSED_ICON_HEIGHT;
}

int cbRowDragPlugin::GetRowDragHintWidth()
{
	return ROW_DRAG_HINT_WIDTH;
}

void cbRowDragPlugin::SetPaneMargins()
{
	int hiddenRowsCnt = GetHRowsCountForPane( mpPane );

	if ( mSvTopMargin == -1 )
	{
		mSvTopMargin    = mpPane->mTopMargin;
		mSvBottomMargin	= mpPane->mBottomMargin;
		mSvLeftMargin   = mpPane->mLeftMargin;
		mSvRightMargin  = mpPane->mRightMargin;
	}

	if ( mpPane->IsHorizontal() )
	{
		mpPane->mTopMargin    = mSvTopMargin;
		mpPane->mBottomMargin = ( hiddenRowsCnt == 0 ) 
								?  mSvBottomMargin 
								:  mSvBottomMargin + GetCollapsedRowIconHeight();

		mpPane->mLeftMargin   = mSvLeftMargin + GetRowDragHintWidth();
		mpPane->mRightMargin  = mSvRightMargin;
	}
	else
	{
		mpPane->mTopMargin    = mSvTopMargin;
		mpPane->mBottomMargin = mSvBottomMargin + GetRowDragHintWidth();

		mpPane->mLeftMargin   = mSvLeftMargin;
		mpPane->mRightMargin  = ( hiddenRowsCnt == 0 ) ?
								mSvRightMargin : mSvRightMargin + GetCollapsedRowIconHeight();
	}
}

void cbRowDragPlugin::OnInitPlugin()
{
	cbDockPane** panes = mpLayout->GetPanesArray();

	for( int i = 0; i != MAX_PANES; ++i )

		if ( panes[i]->MatchesMask( mPaneMask ) )
		{
			mpPane = panes[i];

			SetPaneMargins();
		}
}

/*** helpers for drag&drop ***/

void cbRowDragPlugin::SetMouseCapture( bool captureOn )
{
	if ( mCaptureIsOn == captureOn ) return;

	if ( captureOn ) 
	{
		mpLayout->CaptureEventsForPane( mpPane );
		mpLayout->CaptureEventsForPlugin( this );
	}
	else 
	{
		mpLayout->ReleaseEventsFromPane( mpPane );
		mpLayout->ReleaseEventsFromPlugin( this );
	}

	mCaptureIsOn = captureOn;
}

void cbRowDragPlugin::UnhiglightItemInFocus()
{
	wxClientDC dc( &mpLayout->GetParentFrame() );

	if ( mpRowInFocus ) 

		DrawRowDragHint( mpRowInFocus, dc, FALSE );
	else
	if ( mCollapsedIconInFocus != - 1 )

		DrawCollapsedRowIcon( mCollapsedIconInFocus, dc, FALSE );
}

void cbRowDragPlugin::ShowDraggedRow( int offset )
{
	// create combined image of pane and dragged
	// row on it, in the mpCombinedImage bitmap

	if ( mpPane->IsHorizontal() )
	{
		if ( mInitalRowOfs + offset + mRowImgDim.y > mCombRect.y + mCombRect.height )

			offset = mCombRect.y + mCombRect.height - mRowImgDim.y - mInitalRowOfs;

		if ( mInitalRowOfs + offset < mCombRect.y )

			offset = mCombRect.y - mInitalRowOfs;

		long x, y = mInitalRowOfs + offset;
		mpPane->FrameToPane( &x, &y );
		mCurDragOfs = y;
	}
	else
	{
		if ( mInitalRowOfs + offset + mRowImgDim.x > mCombRect.x + mCombRect.width )

			offset = mCombRect.x + mCombRect.width - mRowImgDim.x - mInitalRowOfs;

		if ( mInitalRowOfs + offset < mCombRect.x )

			offset = mCombRect.x - mInitalRowOfs;

		long x = mInitalRowOfs + offset, y;
		mpPane->FrameToPane( &x, &y );
		mCurDragOfs = x;
	}

	wxMemoryDC rowImgDc;
	rowImgDc.SelectObject ( *mpRowImage );

	wxMemoryDC paneImgDc;
	paneImgDc.SelectObject( *mpPaneImage );

	wxMemoryDC combImgDc;
	combImgDc.SelectObject( *mpCombinedImage );

	combImgDc.Blit( 0,0, mCombRect.width, mCombRect.height,
		            &paneImgDc, 0,0, wxCOPY );

	if ( mpPane->IsHorizontal() )
	{	
		combImgDc.Blit( 0, mInitalRowOfs + offset - mCombRect.y,
						mCombRect.width, mRowImgDim.y,
						&rowImgDc, 0,0, wxCOPY );
	}
	else
	{
		combImgDc.Blit( mInitalRowOfs + offset - mCombRect.x,
			            0,
						mRowImgDim.x, mCombRect.height,
						&rowImgDc, 0,0, wxCOPY );
	}

	int scrX = mCombRect.x,
	    scrY = mCombRect.y;

	mpLayout->GetParentFrame().ClientToScreen( &scrX, &scrY );

	mpScrDc->Blit( scrX, scrY, mCombRect.width, mCombRect.height,
		           &combImgDc, 0,0, wxCOPY );

	rowImgDc .SelectObject( wxNullBitmap );
	paneImgDc.SelectObject( wxNullBitmap );
	combImgDc.SelectObject( wxNullBitmap );
}

wxBitmap* cbRowDragPlugin::CaptureDCArea( wxDC& dc, wxRect& area )
{
	wxBitmap* pBmp = new wxBitmap( int(area.width), int(area.height) );

	wxMemoryDC mdc;
	mdc.SelectObject( *pBmp );

	mdc.Blit( 0,0, area.width, area.height, &dc, area.x, area.y, wxCOPY );
	mdc.SelectObject( wxNullBitmap );

	return pBmp;
}

void cbRowDragPlugin::PrepareForRowDrag()
{
	wxRect rowBounds = mpRowInFocus->mBoundsInParent;

	if ( mpPane->IsHorizontal() )
	{
		mCombRect         = mpPane->mBoundsInParent;

		mCombRect.x += mpPane->mLeftMargin - ROW_DRAG_HINT_WIDTH - 1;
		mCombRect.y += mpPane->mTopMargin;

		mCombRect.width  -= mpPane->mLeftMargin + mpPane->mRightMargin - ROW_DRAG_HINT_WIDTH - 1 - 1;
		mCombRect.height -= mpPane->mTopMargin  + mpPane->mBottomMargin;

		mCombRect.height += 2*rowBounds.height;
		mCombRect.y      -= rowBounds.height;
		mInitalRowOfs     = rowBounds.y;

		rowBounds.y      -= 1;
		rowBounds.height += 2;
		rowBounds.x      = mCombRect.x;
		rowBounds.width  = mCombRect.width;

		mRowImgDim.y     = rowBounds.height;
	}
	else
	{
		mCombRect = mpPane->mBoundsInParent;

		mCombRect.y += mpPane->mTopMargin  - 1; 
		mCombRect.x += mpPane->mLeftMargin - 1;
			;
		mCombRect.height -= mpPane->mTopMargin  + mpPane->mBottomMargin - ROW_DRAG_HINT_WIDTH - 1 - 1;
		mCombRect.width  -= mpPane->mLeftMargin + mpPane->mRightMargin;

		mCombRect.width += 2*rowBounds.width;
		mCombRect.x     -= rowBounds.width;
		mInitalRowOfs    = rowBounds.x;

		rowBounds.x      -= 1;
		rowBounds.width  += 2;
		rowBounds.y      = mCombRect.y;
		rowBounds.height = mCombRect.height;

		mRowImgDim.x     = rowBounds.width;
	}
	// output cobination results onto frame's client area
	wxScreenDC::StartDrawingOnTop(&mpLayout->GetParentFrame());
	mpScrDc = new wxScreenDC();

	int x = mCombRect.x, y = mCombRect.y;
	mpLayout->GetParentFrame().ClientToScreen( &x, &y );

	wxRect scrRect = mCombRect;
	scrRect.x = x;
	scrRect.y = y;

	mpPaneImage = CaptureDCArea( *mpScrDc, scrRect );

	wxMemoryDC mdc;
	mdc.SelectObject( *mpPaneImage );
	mdc.SetDeviceOrigin( -mCombRect.x, -mCombRect.y );

	DrawRectShade( rowBounds, mdc, -1, mpLayout->mGrayPen,  mpLayout->mDarkPen  );
	DrawRectShade( rowBounds, mdc, 0, mpLayout->mLightPen, mpLayout->mBlackPen );

	mpRowImage = CaptureDCArea( mdc, rowBounds );

	// draw dark empty-row placeholder
	DrawEmptyRow( mdc, rowBounds );

	//DrawRectShade( rowBounds, mdc, 0, mpLayout->mGrayPen,  mpLayout->mDarkPen  );
	DrawRectShade( rowBounds, mdc, -1, mpLayout->mGrayPen, mpLayout->mGrayPen );

	mdc.SelectObject( wxNullBitmap );

	mpCombinedImage = new wxBitmap( int(mCombRect.width), int(mCombRect.height) );

	// show it for the first time
	ShowDraggedRow( 0 );
}

void cbRowDragPlugin::DrawEmptyRow( wxDC& dc, wxRect& rowBounds )
{
	wxBrush bkBrush( mpLayout->mDarkPen.GetColour(), wxSOLID );

	// paint the "dark" empty-row placeholder

	dc.SetBrush( bkBrush );
	dc.SetPen  ( mpLayout->mNullPen );

	dc.DrawRectangle( rowBounds.x, rowBounds.y, 
					  rowBounds.width+1, rowBounds.height+1 );

	dc.SetBrush( wxNullBrush );
}

void cbRowDragPlugin::ShowPaneImage()
{
	int scrX = 0, scrY = 0;

	mpLayout->GetParentFrame().ClientToScreen( &scrX, &scrY );

	wxMemoryDC mdc;
	mdc.SelectObject( *mpPaneImage );

	mpScrDc->Blit( mCombRect.x + scrX, mCombRect.y + scrY, 
				   mCombRect.width, mCombRect.height,
			       &mdc, 0,0, wxCOPY );

	mdc.SelectObject( wxNullBitmap );
}

void cbRowDragPlugin::FinishOnScreenDraw()
{
	wxScreenDC::EndDrawingOnTop();

	delete mpScrDc;
	delete mpCombinedImage;
	delete mpPaneImage;
	delete mpRowImage;

	mpScrDc = NULL;
	
	mpCombinedImage = mpPaneImage = mpRowImage = NULL;
}

void cbRowDragPlugin::CollapseRow( cbRowInfo* pRow )
{
	int iconCnt = GetHRowsCountForPane( mpPane );

	mpLayout->GetUpdatesManager().OnStartChanges();

	cbBarInfo* pBar = pRow->mBars[0];

	int rowNo = 0;

	cbRowInfo* pCur = pRow;
	while( pCur->mpPrev ) { ++rowNo; pCur = pCur->mpPrev; }

	while( pBar )
	{
		cbHiddenBarInfo* pHBInfo = new cbHiddenBarInfo();
						    
		pHBInfo->mpBar      = pBar;
		pHBInfo->mRowNo     = rowNo;
		pHBInfo->mIconNo    = iconCnt;
		pHBInfo->mAlignment	= mpPane->mAlignment;

		mHiddenBars.Append( (wxObject*) pHBInfo );

		// hide it
		if ( pBar->mpBarWnd )

			pBar->mpBarWnd->Show( FALSE );

		pBar->mState = wxCBAR_HIDDEN;

		cbBarInfo* pNext = pBar->mpNext;

		pBar->mpRow  = NULL;
		pBar->mpNext = NULL;
		pBar->mpPrev = NULL;

		pBar = pNext;
	}

	mpPane->GetRowList().Remove( pRow );
	mpPane->InitLinksForRows();

	delete pRow;

	SetPaneMargins();

	mpLayout->RecalcLayout(FALSE);

	mpRowInFocus = NULL;

	mpLayout->GetUpdatesManager().OnFinishChanges();
	mpLayout->GetUpdatesManager().UpdateNow();
}

void cbRowDragPlugin::ExpandRow( int collapsedIconIdx )
{
	mpLayout->GetUpdatesManager().OnStartChanges();

	cbRowInfo* pNewRow = new cbRowInfo();

	wxNode* pNode = mHiddenBars.First();

	int rowNo = 0;

	// move bars from internal list to the newly expanded row 

	while( pNode )
	{
		cbHiddenBarInfo* pHBInfo = (cbHiddenBarInfo*)pNode->Data();

		if ( pHBInfo->mAlignment     == mpPane->mAlignment &&
			 pHBInfo->mIconNo        == collapsedIconIdx   )
		{
			rowNo = pHBInfo->mRowNo;

			if ( pHBInfo->mpBar->mState == wxCBAR_HIDDEN )
			{
				pNewRow->mBars.Add( pHBInfo->mpBar );

				pHBInfo->mpBar->mState = ( mpPane->IsHorizontal() ) 
										 ? wxCBAR_DOCKED_HORIZONTALLY
										 : wxCBAR_DOCKED_VERTICALLY;
			}

			// remove bar info from internal list

			wxNode* pNext = pNode->Next();

			delete pHBInfo;
			mHiddenBars.DeleteNode( pNode );

			pNode = pNext;
		}
		else
		{
			// decrease incon numbers with higher indicies, since this
			// row is now removed from the hidden-rows list

			if ( pHBInfo->mIconNo    > collapsedIconIdx &&
				 pHBInfo->mAlignment == mpPane->mAlignment )

				--pHBInfo->mIconNo;

			pNode = pNode->Next();
		}
	}

	mpPane->InitLinksForRow( pNewRow );

	// insert row into pane at it's original position

	if ( pNewRow->mBars.GetCount() )
	{
		cbRowInfo* beforeRowNode = mpPane->GetRow( rowNo );

		mpPane->InsertRow( pNewRow, beforeRowNode );
	}
	else
		delete pNewRow;

	SetPaneMargins();

	mpLayout->RecalcLayout(FALSE);

	mCollapsedIconInFocus = -1;

	mpLayout->GetUpdatesManager().OnFinishChanges();
	mpLayout->GetUpdatesManager().UpdateNow();


	/*
	wxNode* pRowNode = mHiddenRows.Nth( collapsedIconIdx );

	mpLayout->GetUpdatesManager().OnStartChanges();

	// insert at the end of rows list
	mpPane->InsertRow( pRowNode, NULL );

	int success = mHiddenRows.DeleteNode( pRowNode );
	// DBG::
	wxASSERT( success );

	SetPaneMargins();

	mpLayout->RecalcLayout(FALSE);

	mCollapsedIconInFocus = -1;

	mpLayout->GetUpdatesManager().OnFinishChanges();
	mpLayout->GetUpdatesManager().UpdateNow();
	*/
}

void cbRowDragPlugin::InsertDraggedRowBefore( cbRowInfo* pBeforeRow )
{
	if ( mpRowInFocus != pBeforeRow &&
		 mpRowInFocus->mpNext != pBeforeRow 
	   )
	{
		mpPane->GetRowList().Remove( mpRowInFocus );

		mpPane->InsertRow( mpRowInFocus, pBeforeRow );
	}
	else 
	{
		// otherwise, nothing has happned (row positions do not change)

		//wxClientDC dc( &mpLayout->GetParentFrame() );

		//mpPane->PaintRow( mpRowInFocus, dc );
		//DrawRowDragHint( mpRowInFocus, dc, FALSE );
	}
}

bool cbRowDragPlugin::ItemIsInFocus()
{
	return ( mpRowInFocus || mCollapsedIconInFocus != - 1 );
}

void cbRowDragPlugin::CheckPrevItemInFocus( cbRowInfo* pRow, int iconIdx )
{
	wxClientDC dc( &mpLayout->GetParentFrame() );

	if ( pRow != NULL && mpRowInFocus == pRow ) return;
	if ( iconIdx != -1 && mCollapsedIconInFocus == iconIdx ) return;

	UnhiglightItemInFocus();

	if ( iconIdx != - 1 )
	
		DrawCollapsedRowIcon( iconIdx, dc, TRUE );

	else
	if ( pRow != NULL )

		DrawRowDragHint( pRow, dc, TRUE );
}

cbRowInfo* cbRowDragPlugin::GetFirstRow()
{
	return ( mpPane->GetRowList().GetCount() ) 
		   ? mpPane->GetRowList()[0]
		   : NULL;
}

/*** "hard-coded" metafile for NN-look ***/

void cbRowDragPlugin::DrawTrianUp( wxRect& inRect, wxDC& dc )
{
	int xOfs = (inRect.width - ICON_TRIAN_WIDTH)/2;

	wxBrush br( mTrianInnerColor, wxSOLID );

	dc.SetBrush( br );
	dc.SetPen( mpLayout->mBlackPen );

	wxPoint points[3];
	points[0].x = inRect.x + xOfs;
	points[0].y = inRect.y + inRect.height - 1;
	points[1].x = inRect.x + xOfs + ICON_TRIAN_WIDTH/2 + 1;
	points[1].y = inRect.y + inRect.height - 2 - ICON_TRIAN_HEIGHT;
	points[2].x = inRect.x + xOfs + ICON_TRIAN_WIDTH+1;
	points[2].y = inRect.y + inRect.height - 1;

	dc.DrawPolygon( 3, points );

	// higlight upper-right edge of triangle
	dc.SetPen( mpLayout->mLightPen );
	dc.DrawLine( points[2].x, points[2].y,
		         points[0].x, points[0].y );

	dc.SetBrush( wxNullBrush );
}

void cbRowDragPlugin::DrawTrianDown( wxRect& inRect, wxDC& dc )
{
	int xOfs = (inRect.width - ICON_TRIAN_WIDTH)/2;

	wxBrush br( mTrianInnerColor, wxSOLID );

	dc.SetBrush( br );
	dc.SetPen( mpLayout->mBlackPen );

	wxPoint points[3];
	points[0].x = inRect.x + xOfs;
	points[0].y = inRect.y;
	points[1].x = inRect.x + xOfs + ICON_TRIAN_WIDTH;
	points[1].y = inRect.y;
	points[2].x = inRect.x + xOfs + ICON_TRIAN_WIDTH/2;
	points[2].y = inRect.y + ICON_TRIAN_HEIGHT;

	dc.DrawPolygon( 3, points );

	// higlight upper-right edge of triangle
	dc.SetPen( mpLayout->mLightPen );
	dc.DrawLine( points[2].x, points[2].y,
		         points[1].x, points[1].y );

	dc.SetBrush( wxNullBrush );
}

void cbRowDragPlugin::DrawTrianRight( wxRect& inRect, wxDC& dc )
{
	int yOfs = (inRect.height - ICON_TRIAN_WIDTH)/2;

	wxBrush br( mTrianInnerColor, wxSOLID );

	dc.SetBrush( br );
	dc.SetPen( mpLayout->mBlackPen );

	wxPoint points[3];
	points[0].x = inRect.x;
	points[0].y = inRect.y + yOfs + ICON_TRIAN_WIDTH;
	points[1].x = inRect.x;
	points[1].y = inRect.y + yOfs;
	points[2].x = inRect.x + ICON_TRIAN_HEIGHT;
	points[2].y = inRect.y + yOfs + ICON_TRIAN_WIDTH/2;

	dc.DrawPolygon( 3, points );

	// higlight upper-right edge of triangle
	dc.SetPen( mpLayout->mLightPen );
	dc.DrawLine( points[0].x, points[0].y,
		         points[2].x, points[2].y );

	dc.SetBrush( wxNullBrush );
}

void cbRowDragPlugin::Draw3DPattern( wxRect& inRect, wxDC& dc )
{
	for( int y = inRect.y; y < inRect.y + inRect.height; y+=3 )
	
		for( int x = inRect.x; x < inRect.x + inRect.width; x+=3 )
		{
			dc.SetPen( mpLayout->mLightPen );
			dc.DrawPoint( x,y );
			dc.SetPen( mpLayout->mBlackPen );
			dc.DrawPoint( x+1, y+1 );
		}
}

void cbRowDragPlugin::DrawRombShades( wxPoint& p1, wxPoint& p2, 
									  wxPoint& p3, wxPoint& p4,
									  wxDC& dc )
{
	dc.SetPen( mpLayout->mLightPen );
	dc.DrawLine( p1.x, p1.y, p2.x, p2.y );
	dc.DrawLine( p2.x, p2.y, p3.x, p3.y );
	dc.SetPen( mpLayout->mDarkPen );
	dc.DrawLine( p3.x, p3.y, p4.x, p4.y );
	dc.DrawLine( p4.x, p4.y, p1.x, p1.y );
}

void cbRowDragPlugin::DrawOrtoRomb( wxRect& inRect, wxDC& dc, wxBrush& bkBrush )
{
	dc.SetBrush( bkBrush );
	dc.SetPen( mpLayout->mBlackPen );

	wxPoint points[4];

	if ( inRect.width > inRect.height )
	{
		// horizontal orienation
		points[0].x = inRect.x;
		points[0].y = inRect.y + inRect.height;
		points[1].x = inRect.x;
		points[1].y = inRect.y;
		points[2].x = inRect.x + inRect.width;
		points[2].y = inRect.y;
		points[3].x = inRect.x + inRect.width - COLLAPSED_ICON_HEIGHT;
		points[3].y = inRect.y + inRect.height;

		dc.DrawPolygon( 4, points );

		// squeeze romb's bounds to create an inner-shade shape
		++points[0].x;
		--points[0].y;
		++points[1].x;
		++points[1].y;
		--points[2].x; --points[2].x;
		++points[2].y;
		--points[3].y;

		DrawRombShades( points[0], points[1], points[2], points[3], dc );
	}
	else
	{
		// vertical orientation
		points[0].x = inRect.x + inRect.width;
		points[0].y = inRect.y + inRect.height;
		points[1].x = inRect.x;
		points[1].y = inRect.y + inRect.height;
		points[2].x = inRect.x;
		points[2].y = inRect.y;
		points[3].x = inRect.x + inRect.width;
		points[3].y = inRect.y + COLLAPSED_ICON_HEIGHT;

		dc.DrawPolygon( 4, points );

		// squeeze romb's bounds to create an inner-shade shape
		--points[0].y ;
		--points[0].x;
		++points[1].x;
		--points[1].y;
		++points[2].y; ++points[2].y;
		++points[2].x;
		--points[3].x;

		DrawRombShades( points[1], points[2], points[3], points[0], dc );
	}

	dc.SetBrush( wxNullBrush );
}

void cbRowDragPlugin::DrawRomb( wxRect& inRect, wxDC& dc, wxBrush& bkBrush )
{
	wxPoint points[4];

	dc.SetBrush( bkBrush );
	dc.SetPen( mpLayout->mBlackPen );

	if ( inRect.width > inRect.height )
	{
		// horizontal orientation
		points[0].x = inRect.x;
		points[0].y = inRect.y + inRect.height;
		points[1].x = inRect.x + COLLAPSED_ICON_HEIGHT;
		points[1].y = inRect.y;
		points[2].x = inRect.x + inRect.width;
		points[2].y = inRect.y;
		points[3].x = inRect.x + inRect.width - COLLAPSED_ICON_HEIGHT;
		points[3].y = inRect.y + inRect.height;

		dc.DrawPolygon( 4, points );

		// squeeze romb's bounds to create an inner-shade shape
		++points[0].x ;++points[0].x ;
		--points[0].y;
		++points[1].y;
		--points[2].x; --points[2].x;
		++points[2].y;
		//--points[3].x ;
		--points[3].y;

		DrawRombShades( points[0], points[1], points[2], points[3], dc );

	}
	else
	{
		// vertical orientation
		points[0].x = inRect.x + inRect.width;
		points[0].y = inRect.y + inRect.height;
		points[1].x = inRect.x;
		points[1].y = inRect.y + inRect.height - COLLAPSED_ICON_HEIGHT;
		points[2].x = inRect.x;
		points[2].y = inRect.y;
		points[3].x = inRect.x + inRect.width;
		points[3].y = inRect.y + COLLAPSED_ICON_HEIGHT;

		dc.DrawPolygon( 4, points );

		// squeeze romb's bounds to create an inner-shade shape
		--points[0].y ;--points[0].y ;
		--points[0].x;
		++points[1].x;
		++points[2].y; ++points[2].y;
		++points[2].x;
		--points[3].x;

		DrawRombShades( points[1], points[2], points[3], points[0], dc );
	}

	dc.SetBrush( wxNullBrush );
}

void cbRowDragPlugin::DrawRectShade( wxRect& inRect, wxDC& dc, 
									 int level, wxPen& upperPen, wxPen& lowerPen )
{
	// upper shade
	dc.SetPen( upperPen );
	dc.DrawLine( inRect.x - level, 
				 inRect.y - level, 
		         inRect.x + inRect.width - 1 + level, 
				 inRect.y - level);
	dc.DrawLine( inRect.x - level, inRect.y - level,
		         inRect.x - level, inRect.y + inRect.height - 1 + level );

	// lower shade
	dc.SetPen( lowerPen );
	dc.DrawLine( inRect.x - level, 
				 inRect.y + inRect.height - 1 + level,
		         inRect.x + inRect.width  + level,
			     inRect.y + inRect.height - 1 + level);
	dc.DrawLine( inRect.x + inRect.width - 1 + level,
		         inRect.y - level,
				 inRect.x + inRect.width - 1 + level,
				 inRect.y + inRect.height + level);

	dc.SetBrush( wxNullBrush );
}

void cbRowDragPlugin::Draw3DRect( wxRect& inRect, wxDC& dc, wxBrush& bkBrush )
{
	dc.SetPen( mpLayout->mNullPen );
	dc.SetBrush( bkBrush );

	dc.DrawRectangle( inRect.x, inRect.y,
		              inRect.width, inRect.height );

	DrawRectShade( inRect, dc, 0, mpLayout->mLightPen, mpLayout->mDarkPen );
}

int  cbRowDragPlugin::GetCollapsedIconsPos()
{
	RowArrayT& rows = mpPane->GetRowList();

	if ( rows.GetCount() == 0 ) 
	{
		if ( mpPane->IsHorizontal() )
			
			return mpPane->mBoundsInParent.y + mpPane->mTopMargin;
		else
			return mpPane->mBoundsInParent.x + mpPane->mLeftMargin;
	}

	wxRect& bounds = rows[ rows.GetCount() - 1 ]->mBoundsInParent;

	if ( mpPane->IsHorizontal() )

		return bounds.y + bounds.height + 1;
	else
		return bounds.x + bounds.width  + 1;

}

void cbRowDragPlugin::GetRowHintRect( cbRowInfo* pRow, wxRect& rect )
{
	wxRect& bounds = pRow->mBoundsInParent;

	if ( mpPane->IsHorizontal() )
	{
		rect.x = bounds.x - ROW_DRAG_HINT_WIDTH - 1;
		rect.y = bounds.y;
		rect.width = ROW_DRAG_HINT_WIDTH;
		rect.height = bounds.height;
	}
	else
	{
		rect.x = bounds.x;
		rect.y = bounds.y + bounds.height + 1;
		rect.width = bounds.width;
		rect.height = ROW_DRAG_HINT_WIDTH;
	}
}

void cbRowDragPlugin::GetCollapsedInconRect( int iconIdx, wxRect& rect )
{
	int upper = GetCollapsedIconsPos();

	int right = (iconIdx == 0 ) 
				? 0 : iconIdx * (COLLAPSED_ICON_WIDTH - COLLAPSED_ICON_HEIGHT);

	if ( mpPane->IsHorizontal() )
	{
		rect.x = mpPane->mBoundsInParent.x + mpPane->mLeftMargin - ROW_DRAG_HINT_WIDTH - 1
				 + right;

		rect.y = upper;
		rect.width  = COLLAPSED_ICON_WIDTH;
		rect.height = COLLAPSED_ICON_HEIGHT;
	}
	else
	{
	     rect.x = upper;
		 rect.y = mpPane->mBoundsInParent.y + mpPane->mBoundsInParent.height 
				 - mpPane->mBottomMargin + ROW_DRAG_HINT_WIDTH + 1
				 - right - COLLAPSED_ICON_WIDTH;

		rect.height = COLLAPSED_ICON_WIDTH;
		rect.width  = COLLAPSED_ICON_HEIGHT;
	}
}

/*** overridables ***/

void cbRowDragPlugin::DrawCollapsedRowIcon( int index, wxDC& dc, bool isHighlighted )
{
	wxRect rect;
	GetCollapsedInconRect( index, rect );

	wxBrush  hiBrush ( mHightColor, wxSOLID );
	wxBrush  lowBrush( mLowColor,   wxSOLID );
	wxBrush& curBrush = ( isHighlighted ) ? hiBrush : lowBrush;

	if ( mpPane->IsHorizontal() )
	{				 
		if ( index == 0 )

			DrawOrtoRomb( rect, dc, curBrush );
		else
			DrawRomb( rect, dc, curBrush );

		int triOfs = (index == 0) ? TRIANGLE_OFFSET : TRIANGLE_OFFSET + COLLAPSED_ICON_HEIGHT;

		wxRect triRect;
		triRect.x = triOfs + rect.x;

		triRect.width = ICON_TRIAN_HEIGHT;
		triRect.y = rect.y;
		triRect.height = rect.height;

		DrawTrianRight( triRect, dc );

		wxRect patRect;
		patRect.x      = triOfs + ICON_TRIAN_HEIGHT + TRIANGLE_TO_PAT_GAP + rect.x;
		patRect.y      = rect.y + PAT_OFFSET; 
		patRect.width  = rect.width - (patRect.x - rect.x) - COLLAPSED_ICON_HEIGHT - PAT_OFFSET;
		patRect.height = rect.height - PAT_OFFSET*2;

		Draw3DPattern( patRect, dc );
	}
	else
	{
		if ( index == 0 )

			DrawOrtoRomb( rect, dc, curBrush );
		else
			DrawRomb( rect, dc, curBrush );

		int triOfs = (index == 0) 
					 ? TRIANGLE_OFFSET + ICON_TRIAN_HEIGHT
					 : TRIANGLE_OFFSET + COLLAPSED_ICON_HEIGHT + ICON_TRIAN_HEIGHT;

		wxRect triRect;
		triRect.y      = rect.y + rect.height - triOfs;
		triRect.x      = rect.x;
		triRect.width  = rect.width;
		triRect.height = ICON_TRIAN_HEIGHT;

		DrawTrianUp( triRect, dc );

		wxRect patRect;
		patRect.y      = rect.y + COLLAPSED_ICON_HEIGHT + PAT_OFFSET;
		patRect.x      = rect.x + PAT_OFFSET; 
		patRect.width  = rect.width - 2*PAT_OFFSET ;
		patRect.height = rect.height - triOfs - 2*PAT_OFFSET - COLLAPSED_ICON_HEIGHT;

		Draw3DPattern( patRect, dc );
	}
}

void cbRowDragPlugin::DrawRowDragHint( cbRowInfo* pRow , wxDC& dc, bool isHighlighted )
{
	wxRect rect;
	GetRowHintRect( pRow, rect );

	wxBrush  hiBrush ( mHightColor, wxSOLID );
	wxBrush  lowBrush( mLowColor,   wxSOLID );
	wxBrush& curBrush = ( isHighlighted ) ? hiBrush : lowBrush;

	Draw3DRect( rect, dc, curBrush );

	if ( mpPane->IsHorizontal() )
	{
		wxRect triRect;
		triRect.y	   = rect.y + TRIANGLE_OFFSET;
		triRect.x	   = rect.x;
		triRect.width  = rect.width;
		triRect.height = ICON_TRIAN_HEIGHT;

		DrawTrianDown( triRect, dc );

		wxRect patRect;
		patRect.x      = rect.x + PAT_OFFSET;
		patRect.y      = rect.y + TRIANGLE_OFFSET + ICON_TRIAN_HEIGHT + TRIANGLE_TO_PAT_GAP;
		patRect.width  = rect.width - 2*PAT_OFFSET;
		patRect.height = rect.height - ( patRect.y - rect.y ) - PAT_OFFSET;
		Draw3DPattern( patRect, dc );

		dc.SetPen( mpLayout->mLightPen );
		dc.DrawLine( rect.x, rect.y + rect.height, rect.x + rect.width, rect.y + rect.height );
	}
	else
	{
		wxRect triRect;
		triRect.x	   = rect.x + TRIANGLE_OFFSET;
		triRect.y	   = rect.y;
		triRect.height = rect.height;
		triRect.width  = ICON_TRIAN_HEIGHT;

		DrawTrianRight( triRect, dc );

		wxRect patRect;
		patRect.y      = rect.y + PAT_OFFSET;
		patRect.x      = rect.x + TRIANGLE_OFFSET + ICON_TRIAN_HEIGHT + TRIANGLE_TO_PAT_GAP;
		patRect.height = rect.height - 2*PAT_OFFSET;
		patRect.width  = rect.width - ( patRect.x - rect.x ) - PAT_OFFSET;
		Draw3DPattern( patRect, dc );

		dc.SetPen( mpLayout->mLightPen );
		dc.DrawLine( rect.x + rect.width, rect.y, rect.x + rect.width, rect.y + rect.height );
	}
}

void cbRowDragPlugin::DrawRowsDragHintsBorder( wxDC& dc )
{
	// FIXME:: what was that?
}

void cbRowDragPlugin::DrawCollapsedRowsBorder( wxDC& dc )
{
	int colRowOfs = GetCollapsedIconsPos();
	wxRect& bounds = mpPane->mBoundsInParent;

	wxBrush bkBrush( mpLayout->mGrayPen.GetColour(), wxSOLID );
	dc.SetBrush( bkBrush );
	dc.SetPen( mpLayout->mDarkPen );

	if ( mpPane->IsHorizontal() )
	
		dc.DrawRectangle( bounds.x + mpPane->mLeftMargin - ROW_DRAG_HINT_WIDTH - 1,
						  colRowOfs,
						  bounds.width - mpPane->mLeftMargin - mpPane->mRightMargin + 2 + ROW_DRAG_HINT_WIDTH,
						  COLLAPSED_ICON_HEIGHT + 1);
	else
		dc.DrawRectangle( colRowOfs,
						  bounds.y + mpPane->mTopMargin - 1,
						  COLLAPSED_ICON_HEIGHT + 1,
						  bounds.height - mpPane->mTopMargin - mpPane->mBottomMargin 
						  - ROW_DRAG_HINT_WIDTH - 2 );

	dc.SetBrush( wxNullBrush );
}

static inline bool rect_contains_point( const wxRect& rect, int x, int y )
{
	return ( x >= rect.x &&
		     y >= rect.y &&
			 x <  rect.x + rect.width  &&
			 y <  rect.y + rect.height );
}

bool cbRowDragPlugin::HitTestCollapsedRowIcon( int iconIdx, const wxPoint& pos )
{
	wxRect bounds;
	GetCollapsedInconRect( iconIdx, bounds );

	return rect_contains_point( bounds, pos.x, pos.y );
}

bool cbRowDragPlugin::HitTestRowDragHint( cbRowInfo* pRow, const wxPoint& pos )
{
	wxRect bounds;
	GetRowHintRect( pRow, bounds );

	return rect_contains_point( bounds, pos.x, pos.y );
}

