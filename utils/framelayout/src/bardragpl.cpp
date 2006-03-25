/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     23/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "bardragpl.h"
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

#include "bardragpl.h"

#define POS_UNDEFINED -32768

// helpers, FOR NOW:: static

static inline bool rect_hits_rect( const wxRect& r1, const wxRect& r2 )
{
	if ( ( r2.x >= r1.x && r2.x <= r1.x + r1.width ) ||
		 ( r1.x >= r2.x && r1.x <= r2.x + r2.width ) )

		if ( ( r2.y >= r1.y && r2.y <= r1.y + r1.height ) ||
			 ( r1.y >= r2.y && r1.y <= r2.y + r2.height ) )
			 
			return TRUE;

	return FALSE;
}

static inline bool rect_contains_point( const wxRect& rect, int x, int y )
{
	return ( x >= rect.x &&
		     y >= rect.y &&
			 x <  rect.x + rect.width  &&
			 y <  rect.y + rect.height );
}

/***** Implementation for class cbBarDragPlugin *****/

IMPLEMENT_DYNAMIC_CLASS( cbBarDragPlugin, cbPluginBase )

BEGIN_EVENT_TABLE( cbBarDragPlugin, cbPluginBase )

	//EVT_PL_LEFT_DOWN          ( cbBarDragPlugin::OnLButtonDown      )
	EVT_PL_LEFT_UP			  ( cbBarDragPlugin::OnLButtonUp        )
	EVT_PL_MOTION			  ( cbBarDragPlugin::OnMouseMove        )
	EVT_PL_DRAW_HINT_RECT     ( cbBarDragPlugin::OnDrawHintRect     )
	EVT_PL_START_BAR_DRAGGING ( cbBarDragPlugin::OnStartBarDragging )
	EVT_PL_LEFT_DCLICK		  ( cbBarDragPlugin::OnLDblClick		)

END_EVENT_TABLE()

cbBarDragPlugin::cbBarDragPlugin(void)

	: mBarDragStarted    ( FALSE ),
	  mCanStick          ( TRUE ),
	  mpDraggedBar       ( NULL ),
	  mInClientHintBorder( 4 ),
	  mpScrDc            ( NULL ),
	  mpCurCursor        ( NULL )
{}

cbBarDragPlugin::cbBarDragPlugin( wxFrameLayout* pPanel, int paneMask )

	: cbPluginBase( pPanel, paneMask ),
		
	  mBarDragStarted    ( FALSE ),
	  mCanStick          ( TRUE ),
	  mpDraggedBar       ( NULL ),
	  mInClientHintBorder( 4 ),
	  mpScrDc            ( NULL ),
	  mpCurCursor        ( NULL )
{}

cbBarDragPlugin::~cbBarDragPlugin()
{
	// nothing
}

// helper methods (protected)

// clips (top/bottom) or (right/left) edges against the frame's bounding rect.

void do_clip_edges( int len, long& rectPos, long& rectLen )
{
	if ( rectPos < 0 )
	{
		rectLen += rectPos;
		rectPos = 0;
		if ( rectLen < 0 ) rectLen = 1;
	}
	else
	if ( rectPos > len-1 )
	{
		rectPos = len-1;
		rectLen = 1;
	}
	else
	if ( rectPos + rectLen - 1 > len )
	
		rectLen -= (rectPos + rectLen) - len + 1;
}

void cbBarDragPlugin::ClipRectInFrame( wxRect& rect )
{
	int w, h;
	mpLayout->GetParentFrame().GetClientSize( &w, &h );

	do_clip_edges( w, rect.x, rect.width  );
	do_clip_edges( h, rect.y, rect.height );
}

void cbBarDragPlugin::ClipPosInFrame( wxPoint& pos )
{
	int w, h;
	mpLayout->GetParentFrame().GetClientSize( &w, &h );

	if ( pos.x < 0 ) pos.x = 0;
	if ( pos.y < 0 ) pos.y = 0;
	if ( pos.x > w ) pos.x = w-1;
	if ( pos.y > h ) pos.y = h-1;
}

void cbBarDragPlugin::AdjustHintRect( wxPoint& mousePos )
{
	mHintRect.x = mousePos.x - mMouseInRectX;
	mHintRect.y = mousePos.y - mMouseInRectY;
}

cbDockPane* cbBarDragPlugin::HitTestPanes( wxRect& rect )
{
	//wxRect clipped = rect;

	//ClipRectInFrame( clipped );

	cbDockPane** pPanes = mpLayout->GetPanesArray();

	for( int i = 0; i != MAX_PANES; ++i )
	
		if ( rect_hits_rect( pPanes[i]->mBoundsInParent, rect ) )

			return pPanes[i];

	return NULL;
}

cbDockPane* cbBarDragPlugin::HitTestPanes( wxPoint& pos )
{
	wxPoint clipped = pos;

	//ClipPosInFrame( pos );

	cbDockPane** pPanes = mpLayout->GetPanesArray();

	for( int i = 0; i != MAX_PANES; ++i )
	
		if ( rect_contains_point( pPanes[i]->mBoundsInParent, clipped.x, clipped.y ) )

			return pPanes[i];

	return NULL;
}

bool cbBarDragPlugin::HitsPane( cbDockPane* pPane, wxRect& rect )
{
	return rect_hits_rect( pPane->mBoundsInParent, rect );
}

int cbBarDragPlugin::GetDistanceToPane( cbDockPane* pPane, wxPoint& mousePos )
{
	wxRect& bounds = pPane->mBoundsInParent;

	switch( pPane->mAlignment )
	{
		case wxTOP    : return mousePos.y - ( bounds.y + bounds.height );

		case wxBOTTOM : return bounds.y - mousePos.y;

		case wxLEFT   : return mousePos.x - ( bounds.x + bounds.width  );

		case wxRIGHT  : return bounds.x - mousePos.x;

		default : return 0; // never reached
	}

	return 0;
}

bool cbBarDragPlugin::IsInOtherPane( wxPoint& mousePos )
{
	cbDockPane* pPane = HitTestPanes( mousePos );

	if ( pPane && pPane != mpCurPane ) return TRUE;
								  else return FALSE;
}

bool cbBarDragPlugin::IsInClientArea( wxPoint& mousePos )
{
	return  ( HitTestPanes( mousePos ) == NULL );
}

bool cbBarDragPlugin::IsInClientArea( wxRect& rect )
{
	return ( HitTestPanes( rect ) == NULL );
}

void cbBarDragPlugin::CalcOnScreenDims( wxRect& rect )
{
	if ( !mpCurPane || mpDraggedBar->IsFixed() ) return;

	wxRect inPane = rect;

	mpCurPane->FrameToPane( &inPane );

	int rowNo = mpCurPane->GetRowAt( inPane.y, inPane.y + inPane.height );

	bool isMaximized = ( rowNo >= (int)mpCurPane->GetRowList().Count() || rowNo < 0 );

	if ( isMaximized )
	{
		inPane.x = 0;
		inPane.width = mpCurPane->mPaneWidth;

		mpCurPane->PaneToFrame( &inPane );

		rect = inPane;
	}
}

// helpers 

static inline void check_upper_overrun( long& pos, int width, int mousePos )
{
	if ( mousePos >= pos + width )

		pos = mousePos - width/2;
}

static inline void check_lower_overrun( long& pos, int width, int mousePos )
{
	if ( mousePos <= pos )

		pos = mousePos - width/2;
}

void cbBarDragPlugin::StickToPane( cbDockPane* pPane, wxPoint& mousePos )
{
	int wInPane = GetBarWidthInPane ( pPane );
	int hInPane = GetBarHeightInPane( pPane );

	// adjsut hint-rect horizontally (in pane's orientation)
	
	if ( pPane->IsHorizontal() )
	{
		mHintRect.width  = wInPane;
		mHintRect.height = hInPane;
	}
	else
	{
		mHintRect.height = wInPane;
		mHintRect.width  = hInPane;
	}

	// adjsut hint-rect vertically (in pane's orientation)

	wxRect& bounds = pPane->mBoundsInParent;

	// TRUE, if hint enters the pane through it's lower edge

	bool fromLowerEdge = ( pPane->IsHorizontal() ) 
						 ? mousePos.y > bounds.y
						 : mousePos.x > bounds.x;

	// NOTE:: about all the below min/max things: they are ment to ensure
	//        that mouse pointer doesn't overrun (leave) the hint-rectangle
	//        when dimensions it's are recalculated upon sticking it to the pane

	if ( pPane->IsHorizontal() && fromLowerEdge )
	{
		int paneBottomEdgeY = bounds.y + bounds.height;

		mHintRect.y = wxMin( paneBottomEdgeY, mousePos.y );

		check_lower_overrun( mHintRect.y, hInPane, mousePos.y );

	}
	else
	if ( pPane->IsHorizontal() && !fromLowerEdge )
	{
		int paneTopEdgeY = bounds.y;

		mHintRect.y = wxMax( paneTopEdgeY - hInPane, mousePos.y - hInPane );

		check_upper_overrun( mHintRect.y, hInPane, mousePos.y );
	}
	else
	if ( !pPane->IsHorizontal() && fromLowerEdge )
	{
		int paneRightEdgeX = bounds.x + bounds.width;

		mHintRect.x = wxMin( paneRightEdgeX, mousePos.x );

		check_lower_overrun( mHintRect.x, hInPane, mousePos.x );
	}
	else
	if ( !pPane->IsHorizontal() && !fromLowerEdge )
	{
		int paneLeftEdgeX = bounds.x;

		mHintRect.x = wxMax( paneLeftEdgeX - hInPane, mousePos.x - hInPane );

		check_upper_overrun( mHintRect.x, hInPane, mousePos.x );
	}

	mMouseInRectX = mousePos.x - mHintRect.x;
	mMouseInRectY = mousePos.y - mHintRect.y;

	mpCurPane = pPane; // memorize pane to which the hint is currently sticked
}

void cbBarDragPlugin::UnstickFromPane( cbDockPane* pPane, wxPoint& mousePos )
{
	// unsticking causes rectangle to get the shape, in which
	// dragged control-bar would be when floated


	int newWidth  = mpDraggedBar->mDimInfo.mSizes[wxCBAR_FLOATING].x;
	int newHeight = mpDraggedBar->mDimInfo.mSizes[wxCBAR_FLOATING].y;

	wxRect& flBounds = mpDraggedBar->mDimInfo.mBounds[wxCBAR_FLOATING];

	if ( flBounds.width != -1 )
	{
		newWidth  = flBounds.width;
		newHeight = flBounds.height;
	}

	mHintRect.width  = newWidth;
	mHintRect.height = newHeight;

	wxRect& bounds = pPane->mBoundsInParent;

	// TRUE, if hint leaves the pane through it's lower edge

	bool fromLowerEdge = ( pPane->IsHorizontal() ) 
						 ? mousePos.y > bounds.y
						 : mousePos.x > bounds.x;

	// NOTE:: ...all the below min/max things - see comments about it in StickToPane(..)

	if ( pPane->IsHorizontal() && fromLowerEdge )
	{	
		bool fromLowerEdge = mousePos.y > bounds.y;

		mHintRect.y = wxMax( bounds.y + bounds.height + 1, mousePos.y - newHeight );

		check_upper_overrun( mHintRect.y, newHeight, mousePos.y );

		// this is how MFC's hint behaves:

		if ( mMouseInRectX > newWidth )
		
			mHintRect.x = mousePos.x - ( newWidth / 2 );
	}
	else
	if ( pPane->IsHorizontal() && !fromLowerEdge )
	{
		mHintRect.y = wxMin( bounds.y - newHeight - 1, mousePos.y );

		// -/-

		if ( mMouseInRectX > newWidth )
		
			mHintRect.x = mousePos.x - ( newWidth / 2 );

		check_lower_overrun( mHintRect.y, newHeight, mousePos.y );
	}
	else
	if ( !pPane->IsHorizontal() && fromLowerEdge )
	{
		mHintRect.x = wxMax( bounds.x + bounds.width, mousePos.x - newWidth );

		// -/-
		
		if ( mMouseInRectY > newHeight )

			mHintRect.y = mousePos.y - ( newHeight / 2 );

		check_upper_overrun( mHintRect.x, newWidth, mousePos.x );
	}
	else
	if ( !pPane->IsHorizontal() && !fromLowerEdge )
	{
		mHintRect.x = wxMin( bounds.x - newWidth - 1, mousePos.x );

		// -/-
		
		if ( mMouseInRectY > newHeight )

			mHintRect.y = mousePos.y - ( newHeight / 2 );

		check_lower_overrun( mHintRect.x, newWidth, mousePos.x );
	}

	mMouseInRectX = mousePos.x - mHintRect.x;
	mMouseInRectY = mousePos.y - mHintRect.y;

	mpCurPane = NULL;
}				

int cbBarDragPlugin::GetBarWidthInPane( cbDockPane* pPane )
{
	if ( pPane == mpSrcPane )

		return mBarWidthInSrcPane;

	// this is how MFC's bars behave:

	if ( pPane->IsHorizontal() )
	
		return mpDraggedBar->mDimInfo.mSizes[wxCBAR_DOCKED_HORIZONTALLY].x;
	else
		return mpDraggedBar->mDimInfo.mSizes[wxCBAR_DOCKED_VERTICALLY  ].x;
}

int cbBarDragPlugin::GetBarHeightInPane( cbDockPane* pPane )
{
	if ( pPane->IsHorizontal() )
	
		return mpDraggedBar->mDimInfo.mSizes[wxCBAR_DOCKED_HORIZONTALLY].y;
	else
		return mpDraggedBar->mDimInfo.mSizes[wxCBAR_DOCKED_VERTICALLY  ].y;
}

void cbBarDragPlugin::ShowHint( bool prevWasInClient )
{
	bool wasDocked = FALSE;

	if ( mpDraggedBar->mState != wxCBAR_FLOATING && !mpCurPane )
	{
		mpLayout->SetBarState( mpDraggedBar, wxCBAR_FLOATING, TRUE );
	}
	else
	if ( mpDraggedBar->mState == wxCBAR_FLOATING && mpCurPane )
	{
		mpLayout->SetBarState( mpDraggedBar, wxCBAR_DOCKED_HORIZONTALLY, FALSE );

		wasDocked = TRUE;
	}

	if ( mpSrcPane->mProps.mRealTimeUpdatesOn == FALSE )
	{
		// do hevy calculations first

		wxRect actualRect = mHintRect; // will be adjusted depending on drag-settings

		if ( mpSrcPane->mProps.mExactDockPredictionOn && mpCurPane )
		{
			bool success = mpLayout->RedockBar( mpDraggedBar, mHintRect, mpCurPane, FALSE );

			wxASSERT( success ); // DBG::

			actualRect = mpDraggedBar->mBounds;

			mpCurPane->PaneToFrame( &actualRect );
		}
		else
			CalcOnScreenDims( actualRect );

		// release previouse hint

		if ( mPrevHintRect.x != POS_UNDEFINED )
		{
			// erase previouse rectangle

			cbDrawHintRectEvent evt( mPrevHintRect, prevWasInClient, TRUE, FALSE );

			mpLayout->FirePluginEvent( evt );
		}

		// draw new hint

		cbDrawHintRectEvent evt( actualRect, mpCurPane == NULL, FALSE, FALSE );

		mpLayout->FirePluginEvent( evt );

		mPrevHintRect = actualRect;
	}
	else 
	{
		// otherwise, if real-time updates option is ON

		if ( mpCurPane )
		{
			mpLayout->GetUpdatesManager().OnStartChanges();

			if ( wasDocked )

				mpDraggedBar->mUMgrData.SetDirty( TRUE );

			bool success = mpLayout->RedockBar( mpDraggedBar, mHintRect, mpCurPane, FALSE );

			wxASSERT( success ); // DBG ::

			mpLayout->GetUpdatesManager().OnFinishChanges();
			mpLayout->GetUpdatesManager().UpdateNow();
		}
		else
		{
			if ( mpLayout->mFloatingOn )
			{
				// move the top-most floated bar around as user drags the hint

				mpDraggedBar->mDimInfo.mBounds[ wxCBAR_FLOATING ] = mHintRect;

				mpLayout->ApplyBarProperties( mpDraggedBar );
			}
		}
	}
}

/*** event handlers ***/

void cbBarDragPlugin::OnMouseMove( cbMotionEvent& event )
{
	// calculate postion in frame's coordiantes

	if ( !mBarDragStarted )
	{
		event.Skip(); // pass event to the next plugin
		return;
	}

	wxPoint mousePos = event.mPos;

	event.mpPane->PaneToFrame( &mousePos.x, &mousePos.y );

	wxRect prevRect = mHintRect;
	bool   prevIsInClient = ( mpCurPane == 0 );

	AdjustHintRect( mousePos );

	// if the hint-rect is not "tempted" to any pane yet

	if ( mpCurPane == NULL )
	{
		cbDockPane* pPane = HitTestPanes( mHintRect );

		if ( !pPane ) 
			
			// enable sticking again, if we've left the pane completely
			mCanStick = TRUE;

		if ( mCanStick && pPane && 
			 GetDistanceToPane( pPane, mousePos ) < GetBarHeightInPane( pPane ) )
		
			StickToPane( pPane, mousePos );
		else
			if ( pPane && HitTestPanes( mousePos ) == pPane && 0 ) // FOR NOW:: disabled

				StickToPane( pPane, mousePos );
	}
	else
	{
		// otherwise, when rect is now sticked to some of the panes
		// check if it should still remain in this pane

		mCanStick = TRUE;

		bool mouseInOther = IsInOtherPane( mousePos );

		if ( mouseInOther )
		{
				cbDockPane* pPane = HitTestPanes( mousePos );

				StickToPane( pPane, mousePos );
		}
		else
		{
			if ( IsInClientArea( mousePos ) )
			{
				cbDockPane* pPane = HitTestPanes( mHintRect );

				if ( pPane && 
					 pPane != mpCurPane &&
					 GetDistanceToPane( pPane, mousePos ) < GetBarHeightInPane( pPane ) )

						StickToPane( pPane, mousePos );
				else
				if ( !pPane )
				{
					UnstickFromPane( mpCurPane, mousePos );

					// FOR NOW:: disabled, would cause some mess
					//mCanStick = FALSE; // prevents from sticking to this
								         // pane again, flag is reset when hint-rect
								         // leaves the pane completely
				}
				else
				if ( GetDistanceToPane( pPane, mousePos ) > GetBarHeightInPane( pPane ) )
				{
					if ( !HitsPane( mpCurPane, mHintRect ) )
					{
						UnstickFromPane( mpCurPane, mousePos );

						// FOR NOW:: disabled, would cause some mess
						//mCanStick = FALSE; // prevents from sticking to this
										     // pane again, flag is reset when hint-rect
										     // leaves the pane completely
					}
				}

			}
			else
			{
			}
		}
	}

	ShowHint( prevIsInClient );

	wxCursor* pPrevCurs = mpCurCursor;

	if ( mpCurPane )
	
		mpCurCursor = mpLayout->mpDragCursor;
	else
	{
		if ( mpLayout->mFloatingOn && mpSrcPane->mProps.mRealTimeUpdatesOn )
		
			mpCurCursor = mpLayout->mpDragCursor;
		else
			mpCurCursor = mpLayout->mpNECursor;
	}

	if ( pPrevCurs != mpCurCursor )

		mpLayout->GetParentFrame().SetCursor( *mpCurCursor );
}

void cbBarDragPlugin::OnLButtonDown( cbLeftDownEvent& event )
{
	if ( mBarDragStarted  )
	{
		wxMessageBox("DblClick!");
	}

	event.Skip();
}

void cbBarDragPlugin::OnLButtonUp( cbLeftUpEvent& event )
{
	if ( mBarDragStarted  )
	{
		if ( mpSrcPane->mProps.mRealTimeUpdatesOn == FALSE )
		{
			// erase current rectangle, and finsih on-screen drawing session

			cbDrawHintRectEvent evt( mPrevHintRect, mpCurPane == NULL, TRUE, TRUE );

			mpLayout->FirePluginEvent( evt );

			if ( mpCurPane != NULL )
			{
				if ( mpSrcPane->mProps.mExactDockPredictionOn )
				{
					mpLayout->RedockBar( mpDraggedBar, mHintRect, mpCurPane, FALSE );

					mpLayout->GetUpdatesManager().OnFinishChanges();
					mpLayout->GetUpdatesManager().UpdateNow();
				}
				else
					mpLayout->RedockBar( mpDraggedBar, mHintRect, mpCurPane );
			}
		}
	
		mHintRect.width = -1;

		mpLayout->GetParentFrame().SetCursor( *mpLayout->mpNormalCursor );

		mpLayout->ReleaseEventsFromPane( event.mpPane );
		mpLayout->ReleaseEventsFromPlugin( this );

		mBarDragStarted = FALSE;

		if ( mBarWasFloating && mpDraggedBar->mState != wxCBAR_FLOATING )
		{
			// save bar's floating position before it was docked 

			mpDraggedBar->mDimInfo.mBounds[ wxCBAR_FLOATING ] = mFloatedBarBounds;
		}
	}
	else
		event.Skip(); // pass event to the next plugin
}

void cbBarDragPlugin::OnLDblClick( cbLeftDClickEvent& event )
{
	if ( 1 )
	{
		cbBarInfo* pHittedBar;
		cbRowInfo* pRow;

		if ( event.mpPane->HitTestPaneItems( event.mPos, 	  // in pane's coordiantes
										     &pRow,
											 &pHittedBar ) == CB_BAR_CONTENT_HITTED
		   )
			{
				mpLayout->SetBarState( pHittedBar, wxCBAR_FLOATING, TRUE );

				mpLayout->RepositionFloatedBar( pHittedBar );

				return; // event is "eaten" by this plugin
			}

		mBarDragStarted = FALSE;

		event.Skip();
	}

	//wxMessageBox("Hi, dblclick arrived!");
}

void cbBarDragPlugin::OnStartBarDragging( cbStartBarDraggingEvent& event )
{
	mpDraggedBar = event.mpBar;
	mpSrcPane    = event.mpPane; 

	mpLayout->CaptureEventsForPane( event.mpPane );
	mpLayout->CaptureEventsForPlugin( this );

	mpLayout->GetParentFrame().SetCursor( *mpLayout->mpDragCursor );

	mBarDragStarted = TRUE;

	wxRect inParent = mpDraggedBar->mBounds;

	mBarWasFloating = mpDraggedBar->mState == wxCBAR_FLOATING;
		
	if ( mBarWasFloating )
	{	
		inParent = mpDraggedBar->mDimInfo.mBounds[ wxCBAR_FLOATING ];
		mFloatedBarBounds = inParent;
	}
	else
		event.mpPane->PaneToFrame( &inParent );

	mHintRect.x = POS_UNDEFINED;

	mHintRect.width  = inParent.width;
	mHintRect.height = inParent.height;

	mMouseInRectX = event.mPos.x - inParent.x;
	mMouseInRectY = event.mPos.y - inParent.y;

	mpSrcPane = event.mpPane;

	if ( mpDraggedBar->mState == wxCBAR_FLOATING )

		mpCurPane = NULL;
	else
		mpCurPane = event.mpPane;

	mPrevHintRect.x = POS_UNDEFINED;

	mCanStick = FALSE; // we're not stuck into any pane now - 
					   // there's nowhere to "stick-twice"

	mBarWidthInSrcPane = mpDraggedBar->mDimInfo.mSizes[ mpDraggedBar->mState ].x;

	if ( mpSrcPane->mProps.mRealTimeUpdatesOn == FALSE && 
		 mpSrcPane->mProps.mExactDockPredictionOn  )
	
		mpLayout->GetUpdatesManager().OnStartChanges(); // capture initial state of layout
	
	// simulate the first mouse movement

	long x = event.mPos.x, y = event.mPos.y;

	mpSrcPane->FrameToPane( &x, &y );

	cbMotionEvent motionEvt( wxPoint(x,y), event.mpPane );


	this->OnMouseMove( motionEvt );

	return; // event is "eaten" by this plugin
}

/*** on-screen hint-tracking related methods ***/

void cbBarDragPlugin::OnDrawHintRect( cbDrawHintRectEvent& event )
{
	if ( !mpScrDc ) StartTracking();

	DoDrawHintRect( event.mRect, event.mIsInClient );

	if ( event.mLastTime )

		FinishTracking();
}

#define _A  0xAA
#define _B  0x00
#define _C  0x55
#define _D  0x00

// FOR NOW:: static

static const unsigned char _gCheckerImg[16] = { _A,_B,_C,_D,
											    _A,_B,_C,_D,
											    _A,_B,_C,_D,
											    _A,_B,_C,_D
											  };

void cbBarDragPlugin::StartTracking()
{
	mpScrDc = new wxScreenDC;

	wxScreenDC::StartDrawingOnTop(&mpLayout->GetParentFrame());
}

void cbBarDragPlugin::DoDrawHintRect( wxRect& rect, bool isInClientRect)
{
	wxRect scrRect;

	RectToScr( rect, scrRect );

	int prevLF = mpScrDc->GetLogicalFunction();

	mpScrDc->SetLogicalFunction( wxXOR );

	if ( isInClientRect )
	{
		// BUG BUG BUG (wx):: somehow stippled brush works only  
		//					  when the bitmap created on stack, not
		//					  as a member of the class

		wxBitmap checker( (const char*)_gCheckerImg, 8,8 );

		wxBrush checkerBrush( checker );

		mpScrDc->SetPen( mpLayout->mNullPen );
		mpScrDc->SetBrush( checkerBrush );

		int half = mInClientHintBorder / 2;

		mpScrDc->DrawRectangle( scrRect.x - half, scrRect.y - half,
			                    scrRect.width + 2*half, mInClientHintBorder );

		mpScrDc->DrawRectangle( scrRect.x - half, scrRect.y + scrRect.height - half,
			                    scrRect.width + 2*half, mInClientHintBorder );

		mpScrDc->DrawRectangle( scrRect.x - half, scrRect.y + half - 1,
			                    mInClientHintBorder, scrRect.height - 2*half + 2);

		mpScrDc->DrawRectangle( scrRect.x + scrRect.width - half,
								scrRect.y + half - 1,
			                    mInClientHintBorder, scrRect.height - 2*half + 2);

		mpScrDc->SetBrush( wxNullBrush );
	}
	else
	{
		mpScrDc->SetPen( mpLayout->mBlackPen );

		mpScrDc->DrawLine( scrRect.x, scrRect.y, 
						   scrRect.x + scrRect.width, scrRect.y );

		mpScrDc->DrawLine( scrRect.x, scrRect.y + 1, 
						   scrRect.x, scrRect.y + scrRect.height );

		mpScrDc->DrawLine( scrRect.x+1, scrRect.y + scrRect.height, 
						   scrRect.x + scrRect.width, scrRect.y + scrRect.height );

		mpScrDc->DrawLine( scrRect.x + scrRect.width , scrRect.y, 
						   scrRect.x + scrRect.width, scrRect.y + scrRect.height + 1);
	}

	mpScrDc->SetLogicalFunction( prevLF );
}

void cbBarDragPlugin::DrawHintRect ( wxRect& rect, bool isInClientRect)
{
	DoDrawHintRect( rect, isInClientRect );	
}

void cbBarDragPlugin::EraseHintRect( wxRect& rect, bool isInClientRect)
{
	DoDrawHintRect( rect, isInClientRect );	
}

void cbBarDragPlugin::FinishTracking()
{
	wxScreenDC::EndDrawingOnTop();

	delete mpScrDc;

	mpScrDc = NULL;
}

void cbBarDragPlugin::RectToScr( wxRect& frameRect, wxRect& scrRect )
{
	scrRect = frameRect;

	int x = frameRect.x, y = frameRect.y;

	mpLayout->GetParentFrame().ClientToScreen( &x, &y );

	scrRect.x = x;
	scrRect.y = y;
}
