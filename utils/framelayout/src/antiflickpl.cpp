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
 
#ifdef __GNUG__
#pragma implementation "antiflickpl.h"
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

#include "antiflickpl.h"

/***** Implementation for class cbAntiflickerPlugin *****/

IMPLEMENT_DYNAMIC_CLASS( cbAntiflickerPlugin, cbPluginBase )

BEGIN_EVENT_TABLE( cbAntiflickerPlugin, cbPluginBase )

	EVT_PL_START_DRAW_IN_AREA  ( cbAntiflickerPlugin::OnStartDrawInArea  )
	EVT_PL_FINISH_DRAW_IN_AREA ( cbAntiflickerPlugin::OnFinishDrawInArea )

END_EVENT_TABLE()

// initialization of static members

int cbAntiflickerPlugin::mRefCount    = 0;

wxBitmap*   cbAntiflickerPlugin::mpVertBuf    = 0;
wxBitmap*   cbAntiflickerPlugin::mpHorizBuf   = 0;
wxMemoryDC* cbAntiflickerPlugin::mpVertBufDc  = 0;
wxMemoryDC* cbAntiflickerPlugin::mpHorizBufDc = 0;

// constructors

cbAntiflickerPlugin::cbAntiflickerPlugin(void)
	: mpLRUBufDc  ( NULL ),
	  mLRUArea    ( -1,-1, -1,-1 )
{
	++mRefCount;
}

cbAntiflickerPlugin::cbAntiflickerPlugin( wxFrameLayout* pPanel, int paneMask )

	: cbPluginBase( pPanel, paneMask ),
	  mpLRUBufDc  ( NULL ),
	  mLRUArea    ( -1,-1, -1,-1 )
{
	++mRefCount;
}

cbAntiflickerPlugin::~cbAntiflickerPlugin()
{
	if ( --mRefCount == 0 )
	{
		if ( mpHorizBuf )
		{
			mpHorizBufDc->SelectObject( wxNullBitmap );
			delete mpHorizBuf;
			delete mpHorizBufDc;
			mpHorizBuf   = 0;
			mpHorizBufDc = 0;
		}

		if ( mpVertBuf ) 
		{
			mpVertBufDc->SelectObject( wxNullBitmap );
			delete mpVertBuf; 
			delete mpVertBufDc;
			mpVertBuf   = 0;
			mpVertBufDc = 0;
		}
	}
}

wxDC* cbAntiflickerPlugin::FindSuitableBuffer( const wxRect& forArea )
{
	if ( mpVertBuf )
	{
		if ( mpVertBuf->GetHeight() >= forArea.height &&
			 mpVertBuf->GetWidth()  >= forArea.width )

			 return mpVertBufDc;
	}
	else
	if ( mpHorizBuf )
	{
		if ( mpHorizBuf->GetHeight() >= forArea.height &&
			 mpHorizBuf->GetWidth()  >= forArea.width )

			 return mpHorizBufDc;
	}

	return 0;
}

wxDC* cbAntiflickerPlugin::AllocNewBuffer( const wxRect& forArea )
{
	// TBD:: preallocate bit larger bitmap at once, to avoid
	//       excessive realocations later

	// check whether the given area is oriented horizontally
	// or verticallya and choose correspoinding bitmap to create or
	// recreate

	wxBitmap* pBuf = 0;

	if ( forArea.height > forArea.width )
	{
		wxSize prevDim( 0,0 );

		if ( mpVertBuf ) 
		{
			prevDim.x = mpVertBuf->GetWidth();
			prevDim.y = mpVertBuf->GetHeight();

			mpVertBufDc->SelectObject( wxNullBitmap );
			delete mpVertBuf;
		}
		else
			mpVertBufDc = new wxMemoryDC();
		
		mpVertBuf = new wxBitmap( int( wxMax(forArea.width,  prevDim.x ) ), 
								  int( wxMax(forArea.height, prevDim.y ) )
								);

		mpVertBufDc->SelectObject( *mpVertBuf );

		return mpVertBufDc;
	}
	else
	{
		wxSize prevDim( 0,0 );

		if ( mpHorizBuf ) 
		{
			prevDim.x = mpHorizBuf->GetWidth();
			prevDim.y = mpHorizBuf->GetHeight();

			mpHorizBufDc->SelectObject( wxNullBitmap );
			delete mpHorizBuf;
		}
		else
			mpHorizBufDc = new wxMemoryDC();
		
		mpHorizBuf = new wxBitmap( int( wxMax(forArea.width,  prevDim.x ) ), 
								   int( wxMax(forArea.height, prevDim.y ) )
								 );

		mpHorizBufDc->SelectObject( *mpHorizBuf );

		return mpHorizBufDc;
	}
}

void cbAntiflickerPlugin::OnStartDrawInArea( cbStartDrawInAreaEvent& event )
{
	wxASSERT( mpLRUBufDc == NULL ); // DBG:: see comments in OnFinishDrawInArea(..) method

	// short-cut
	wxRect& area = event.mArea;

	if ( event.mArea.width  < 0 || 
		 event.mArea.height < 0 ) return;

	// memorize given area
	mLRUArea.x      = area.x;
	mLRUArea.y      = area.y;
	mLRUArea.width  = area.width;
	mLRUArea.height = area.height;

	wxDC* pBufDc = FindSuitableBuffer( area );

	if ( !pBufDc )
	
		pBufDc = AllocNewBuffer( area );

	pBufDc->SetDeviceOrigin( -area.x, -area.y );

	pBufDc->SetClippingRegion( area.x,     area.y,
							   area.width, area.height );

	wxClientDC clntDc( &mpLayout->GetParentFrame() );

	(*event.mppDc) = pBufDc;

	mpLRUBufDc = pBufDc; // memorize buffer, which will be flushed to screen
	                     // upon "commiting" the drawing

	/*
	// OLD STUFF::
	mpLRUBufDc->Blit( pos.x, pos.y, size.x, size.y,
				      &clntDc, pos.x, pos.y, wxCOPY );
					*/
}

void cbAntiflickerPlugin::OnFinishDrawInArea( cbFinishDrawInAreaEvent& event )
{
	wxRect& area = event.mArea;

	if ( event.mArea.width  < 0 || 
		 event.mArea.height < 0 ) return;

	wxASSERT( mpLRUBufDc ); // DBG:: OnStartDrawInArea should be called first

	// FOR NOW:: OnStartDrawInArea(..) should be immediatelly followed
	//           by OnFinishDrawInArea(..) for the same area

	wxASSERT( mLRUArea.x      == area.x      );
	wxASSERT( mLRUArea.y      == area.y      );
	wxASSERT( mLRUArea.width  == area.width  );
	wxASSERT( mLRUArea.height == area.height );

	wxClientDC clntDc( &mpLayout->GetParentFrame() );

	// "commit" drawings in one-shot
	clntDc.Blit( area.x, area.y, area.width, area.height,
				 mpLRUBufDc, area.x, area.y, wxCOPY );

	mpLRUBufDc->DestroyClippingRegion();
	mpLRUBufDc = 0;
}
