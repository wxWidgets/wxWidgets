/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     30/11/98 (my 22th birthday :-)
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////
 
#ifdef __GNUG__
#pragma implementation "rowlayoutpl.h"
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

#include "wx/utils.h"
#include "barhintspl.h"

// fixed settings

#define GROOVE_WIDTH         3  // left shade + middle line + right shade
#define GROOVE_TO_GROOVE_GAP 1  
#define BOX_T_BOX_GAP        2
#define BOX_TO_GROOVE_GAP    3

#define BOXES_IN_HINT        2
#define CLOSE_BOX_IDX        0
#define COLLAPSE_BOX_IDX     1

// used interally 

#define CLOSE_BOX_HITTED    1
#define COLLAPSE_BOX_HITTED 2

/***** Implementation fro class cbBarHintsPlugin *****/

IMPLEMENT_DYNAMIC_CLASS( cbBarHintsPlugin, cbPluginBase )

BEGIN_EVENT_TABLE( cbBarHintsPlugin, cbPluginBase )

	EVT_PL_SIZE_BAR_WND  ( cbBarHintsPlugin::OnSizeBarWindow )
	EVT_PL_DRAW_BAR_DECOR( cbBarHintsPlugin::OnDrawBarDecorations )

	EVT_PL_LEFT_DOWN( cbBarHintsPlugin::OnLeftDown )
	EVT_PL_LEFT_UP  ( cbBarHintsPlugin::OnLeftUp   )
	EVT_PL_MOTION	( cbBarHintsPlugin::OnMotion   )

END_EVENT_TABLE()

cbBarHintsPlugin::cbBarHintsPlugin(void)

	: mpPane( 0 ),
	  mCollapseBoxOn( TRUE ),
	  mCloseBoxOn   ( TRUE ),
	  mBtnPressed   ( FALSE ),
	  mGrooveCount  ( 2 ),
	  mHintGap      ( 4 ),
	  mXWeight      ( 2 )
{}

cbBarHintsPlugin::cbBarHintsPlugin( wxFrameLayout* pLayout, int paneMask )

	: cbPluginBase( pLayout, paneMask ),
	  mpPane( 0 ),
	  mCollapseBoxOn( TRUE ),
	  mCloseBoxOn   ( TRUE ),
	  mBtnPressed   ( FALSE ),
	  mGrooveCount  ( 2 ),
	  mHintGap      ( 5 ),
	  mXWeight      ( 2 )
{}

void cbBarHintsPlugin::SetGrooveCount( int nGrooves )
{
	mGrooveCount = nGrooves;
}

void cbBarHintsPlugin::CreateBoxes()
{
	cbCloseBox*    box1 = new cbCloseBox();
	cbCollapseBox* box2 = new cbCollapseBox();

	mBoxes[CLOSE_BOX_IDX]    = box1;
	mBoxes[COLLAPSE_BOX_IDX] = box2;

	for( int i = 0; i != BOXES_IN_HINT; ++i )
	{
		mBoxes[i]->mpLayout = mpLayout;
		mBoxes[i]->mpPlugin = this;
		mBoxes[i]->mpWnd    = NULL;
	}
}

void cbBarHintsPlugin::Draw3DBox( wxDC& dc, const wxPoint& pos, bool pressed )
{
}

void cbBarHintsPlugin::DrawCloseBox( wxDC& dc, const wxPoint& pos, bool pressed )
{
}

void cbBarHintsPlugin::DrawCollapseBox( wxDC& dc, const wxPoint& pos, 
										bool atLeft, bool disabled, bool pressed )
{
}

void cbBarHintsPlugin::DrawGrooves( wxDC& dc, const wxPoint& pos, int length )
{
	int ofs = 0;

	for( int i = 0; i != mGrooveCount; ++i, ofs += ( GROOVE_WIDTH + GROOVE_TO_GROOVE_GAP ) )
	
		if ( mpPane->IsHorizontal() )
		{
			dc.SetPen( mpLayout->mLightPen );
			dc.DrawLine( pos.x + ofs, pos.y, pos.x + ofs, pos.y + length - 1 );
			dc.DrawPoint( pos.x + ofs + 1, pos.y );

			dc.SetPen( mpLayout->mDarkPen );
			dc.DrawLine( pos.x + ofs + 2, pos.y, pos.x + ofs + 2, pos.y + length );
			dc.DrawPoint( pos.x + ofs + 1, pos.y + length - 1 );
			dc.DrawPoint( pos.x + ofs,     pos.y + length - 1 );
		}
		else
		{
			dc.SetPen( mpLayout->mLightPen );
			dc.DrawLine( pos.x, pos.y + ofs, pos.x + length - 1, pos.y + ofs );
			dc.DrawPoint( pos.x, pos.y + ofs + 1 );

			dc.SetPen( mpLayout->mDarkPen );
			dc.DrawLine( pos.x, pos.y + ofs + 2, pos.x + length, pos.y + ofs + 2 );
			dc.DrawPoint( pos.x + length - 1, pos.y + ofs + 1 );
			dc.DrawPoint( pos.x + length - 1, pos.y + ofs );
		}
}

void cbBarHintsPlugin::ExcludeHints( wxRect& rect, cbBarInfo& info )
{
	int boxHeight = BTN_BOX_HEIGHT;

	// collapse and close box are not placed on fixed bars

	if ( info.IsFixed() || ( !mCloseBoxOn && !mCollapseBoxOn ) )

		boxHeight = 0;

	int height = wxMax( mGrooveCount*(GROOVE_WIDTH + GROOVE_TO_GROOVE_GAP) 
		                - GROOVE_TO_GROOVE_GAP, 
						boxHeight 
				      );

	if ( mpPane->IsHorizontal() )
	{
		rect.x     += ( mHintGap*2 + height );
		rect.width -= (height + 2*mHintGap);

		rect.x     -= info.mDimInfo.mHorizGap + 2;
		rect.width += info.mDimInfo.mHorizGap + 2;
	}
	else
	{
		rect.y      += (mHintGap*2 + height);
		rect.height -= (height + 2*mHintGap);

		rect.y      -= info.mDimInfo.mVertGap + 2;
		rect.height += info.mDimInfo.mVertGap + 2;
	}
}

void cbBarHintsPlugin::DoDrawHint( wxDC& dc, wxRect& rect, 
								   int pos, int boxOfs, int grooveOfs,
								   bool isFixed )
{
	if ( !isFixed )
	{
		if ( mpPane->IsHorizontal() )
		{
			if ( mCloseBoxOn )
			
				mBoxes[CLOSE_BOX_IDX]->Draw( dc );

			if ( mCollapseBoxOn )
			
				mBoxes[COLLAPSE_BOX_IDX]->Draw( dc );
		}
		else
		{
			if ( mCloseBoxOn )
			
				mBoxes[CLOSE_BOX_IDX]->Draw( dc );

			if ( mCollapseBoxOn )
			
				mBoxes[COLLAPSE_BOX_IDX]->Draw( dc );
		}
	}

	if ( mpPane->IsHorizontal() )
	
		DrawGrooves( dc, wxPoint( rect.x + mHintGap + grooveOfs, pos ), 
					 rect.height - (pos - rect.y) - mHintGap );
	else
		DrawGrooves( dc, wxPoint( rect.x + mHintGap, rect.y + mHintGap + grooveOfs ), 
					 (pos - rect.x) - mHintGap );
}

void cbBarHintsPlugin::GetHintsLayout( wxRect& rect, cbBarInfo& info, 
									   int& boxOfs, int& grooveOfs, int& pos )
{
	int boxHeight = BTN_BOX_HEIGHT;
	int boxWidth  = BTN_BOX_WIDTH + BOX_TO_GROOVE_GAP + BTN_BOX_WIDTH;

	// collapse and close box are not placed on fixed bars

	if ( info.IsFixed() || ( !mCloseBoxOn && !mCollapseBoxOn ) )
	{
		boxHeight = 0;
		boxWidth = 0;
	}
	else
	if ( !mCloseBoxOn || !mCollapseBoxOn ) 

		boxWidth = BTN_BOX_WIDTH;

	int grooveHeight = mGrooveCount*(GROOVE_WIDTH + GROOVE_TO_GROOVE_GAP) 
		               - GROOVE_TO_GROOVE_GAP;

	int height = wxMax( grooveHeight, boxHeight );

	// center boxs and groves with respect to each other

	boxOfs    = ( height - boxHeight    ) / 2;
	grooveOfs = ( height - grooveHeight ) / 2;

	pos = ( mpPane->IsHorizontal() ) ? rect.y + mHintGap
									 : rect.x + rect.width - mHintGap;

	// setup positions for boxes

	if ( !info.IsFixed() )
	{
		// what direction "collapse-triangle" should look at?

		bool& isAtLeft = ((cbCollapseBox*)(mBoxes[COLLAPSE_BOX_IDX]))->mIsAtLeft;
		 
		isAtLeft= info.mBounds.x <= mpPane->mPaneWidth - ( info.mBounds.x + info.mBounds.width );

		if ( info.IsExpanded() )
		{
			isAtLeft = FALSE;

			cbBarInfo* pCur = info.mpPrev;

			while( pCur )
			{
				if ( !pCur->IsFixed() )
				{
					isAtLeft = TRUE; break;
				}

				pCur = pCur->mpPrev;
			}
		}

		// collapse/expand works only when more not-fixed bars are present in the same row

		mBoxes[COLLAPSE_BOX_IDX]->Enable( info.mpRow->mNotFixedBarsCnt > 1 );

		for( int i = 0; i != BOXES_IN_HINT; ++i )

			mBoxes[i]->mpPane = mpPane;


		if ( mpPane->IsHorizontal() )
		{
			if ( mCloseBoxOn )
			{
				mBoxes[CLOSE_BOX_IDX]->mPos = wxPoint( rect.x + mHintGap + boxOfs, pos );

				pos += BTN_BOX_HEIGHT;
			}

			if ( mCollapseBoxOn )
			{
				if ( mCloseBoxOn ) pos += BOX_T_BOX_GAP;

				mBoxes[COLLAPSE_BOX_IDX]->mPos = wxPoint( rect.x + mHintGap + boxOfs, pos );

				pos += BTN_BOX_HEIGHT;

				pos += BOX_TO_GROOVE_GAP;
			}
		}
		else
		{
			if ( mCloseBoxOn )
			{
				pos -= BTN_BOX_WIDTH;

				mBoxes[CLOSE_BOX_IDX]->mPos = wxPoint( pos , rect.y + mHintGap + boxOfs );
			}

			if ( mCollapseBoxOn )
			{
				if ( mCloseBoxOn ) pos -= BOX_T_BOX_GAP;

				pos -= BTN_BOX_WIDTH;

				mBoxes[COLLAPSE_BOX_IDX]->mPos = wxPoint( pos, rect.y + mHintGap + boxOfs );

				pos -= BOX_TO_GROOVE_GAP;
			}
		}
	}
}

static inline bool is_in_box( const wxPoint& rectPos, const wxPoint& mousePos )
{
	return ( mousePos.x >= rectPos.x &&
			 mousePos.y >= rectPos.y &&
			 mousePos.x < rectPos.x + BTN_BOX_WIDTH &&
			 mousePos.y < rectPos.y + BTN_BOX_HEIGHT );
}

int cbBarHintsPlugin::HitTestHints( cbBarInfo& info, const wxPoint& pos )
{
	wxPoint inPane = pos;
	mpPane->PaneToFrame( &inPane.x, &inPane.y );

	wxRect& rect = info.mBoundsInParent;

	if ( info.IsFixed() ) return FALSE;

	int boxOfs, grooveOfs, coord;

	GetHintsLayout( rect, info, boxOfs, grooveOfs, coord );

	if ( mpPane->IsHorizontal() )
	{
		if ( mCloseBoxOn )
		{
			if ( is_in_box( wxPoint( rect.x + mHintGap + boxOfs, coord ), inPane ) )

				return CLOSE_BOX_HITTED;

			coord += BTN_BOX_HEIGHT;
		}

		if ( mCollapseBoxOn )
		{
			if ( mCloseBoxOn ) coord += BOX_T_BOX_GAP;

			if ( is_in_box( wxPoint( rect.x + mHintGap + boxOfs, coord ), inPane ) )

				return COLLAPSE_BOX_HITTED;

			coord += BTN_BOX_HEIGHT;
		}
	}
	else
	{
		if ( mCloseBoxOn )
		{
			coord -= BTN_BOX_WIDTH;

			if ( is_in_box( wxPoint( coord , rect.y + mHintGap + boxOfs ), inPane ) )

				return CLOSE_BOX_HITTED;
		}

		if ( mCollapseBoxOn )
		{
			if ( mCloseBoxOn ) coord -= BOX_T_BOX_GAP;
			coord -= BTN_BOX_WIDTH;

			if ( is_in_box( wxPoint( coord, rect.y + mHintGap + boxOfs ), inPane ) )

				return COLLAPSE_BOX_HITTED;
		}
	}

	return FALSE;
}

// handlers for plugin-events

void cbBarHintsPlugin::OnSizeBarWindow( cbSizeBarWndEvent& event )
{
	wxRect& rect     = event.mBoundsInParent;
	mpPane           = event.mpPane;

	ExcludeHints( rect, *event.mpBar );

	event.Skip(); // pass event to the next plugin in the chain
}

void cbBarHintsPlugin::OnDrawBarDecorations( cbDrawBarDecorEvent& event )
{
	wxRect& rect     = event.mBoundsInParent;
	mpPane           = event.mpPane;

	int boxOfs, grooveOfs, pos;

	GetHintsLayout( rect, *event.mpBar, boxOfs, grooveOfs, pos );

	DoDrawHint( *event.mpDc, rect, pos, boxOfs, grooveOfs, event.mpBar->IsFixed() );

	// let other plugins add on their decorations

	event.Skip();
}

void cbBarHintsPlugin::OnLeftDown( cbLeftDownEvent& event )
{
	mpPane           = event.mpPane;

	wxPoint inFrame = event.mPos;
	mpPane->PaneToFrame( &inFrame.x, &inFrame.y );

	wxBarIterator iter( mpPane->GetRowList() );

	mpClickedBar = NULL;

	while ( iter.Next() )
	{
		cbBarInfo& bar = iter.BarInfo();

		int boxOfs, grooveOfs, pos;

		GetHintsLayout( bar.mBoundsInParent, bar, boxOfs, grooveOfs, pos );

		if ( !bar.IsFixed() )
		
			for( int i = 0; i != BOXES_IN_HINT; ++i )
			{
				mBoxes[i]->OnLeftDown( inFrame );

				if ( mBoxes[i]->mPressed )
				{
					mBtnPressed = TRUE;
					mpClickedBar = &bar;

					return; // event handled
				}
			}
	}

	event.Skip();
}

void cbBarHintsPlugin::OnLeftUp( cbLeftUpEvent&   event )
{
	if ( mBtnPressed )
	{
		wxPoint inFrame = event.mPos;
		mpPane->PaneToFrame( &inFrame.x, &inFrame.y );

		int boxOfs, grooveOfs, pos;

		GetHintsLayout( mpClickedBar->mBoundsInParent, *mpClickedBar, boxOfs, grooveOfs, pos );

		int result = HitTestHints( *mpClickedBar, event.mPos );

		for( int i = 0; i != BOXES_IN_HINT; ++i )
		{
			mBoxes[i]->OnLeftUp( inFrame );

			if ( mBoxes[i]->WasClicked() )
			{
				if ( i == 0 )

					mpLayout->SetBarState( mpClickedBar, wxCBAR_HIDDEN, TRUE );
				else
				{
					if ( mpClickedBar->IsExpanded() )

						mpPane->ContractBar( mpClickedBar );
					else
						mpPane->ExpandBar( mpClickedBar );
				}
			}
		}

		mBtnPressed = FALSE;
		return;
	}
	else
		event.Skip();
}

void cbBarHintsPlugin::OnMotion( cbMotionEvent&   event )
{
	if ( mBtnPressed )
	{
		wxPoint inFrame = event.mPos;
		mpPane->PaneToFrame( &inFrame.x, &inFrame.y );

		mpPane = event.mpPane;

		for( int i = 0; i != BOXES_IN_HINT; ++i )
		
			mBoxes[i]->OnMotion( inFrame );
	}
	else
		event.Skip();
}

void cbBarHintsPlugin::OnInitPlugin()
{
	cbPluginBase::OnInitPlugin();

	cbDockPane** panes = mpLayout->GetPanesArray();

	for( int i = 0; i != MAX_PANES; ++i )
	
		if ( panes[i]->MatchesMask( mPaneMask ) )
		{
			panes[i]->mProps.mMinCBarDim.x = 25;
			panes[i]->mProps.mMinCBarDim.y = 16;
		}

	CreateBoxes();
}