/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     ??/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "newbmpbtn.cpp"
#pragma interface "newbmpbtn.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

/*
#ifdef __BORLANDC__
#pragma hdrstop
#endif
*/

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "newbmpbtn.h"
#include "wx/utils.h"     // import wxMin,wxMax macros

///////////// button-label rendering helpers //////////////////

static int* create_array( int width, int height, int fill = 0 )
{
	int* array = new int[width*height];

	int len = width*height;
	for( int i = 0; i != len; ++i ) array[i] = fill;

	return array;
}

#define GET_ELEM(array,x,y) (array[width*(y)+(x)])

#define MIN_COLOR_DIFF 10

#define IS_IN_ARRAY(x,y) ( (x) < width && (y) < height && (x) >= 0 && (y) >= 0 )

#define GET_RED(col)    col        & 0xFF  
#define GET_GREEN(col) (col >> 8)  & 0xFF 	  
#define GET_BLUE(col)  (col >> 16) & 0xFF 

#define MAKE_INT_COLOR(red,green,blue) (     (red)                      | \
                                         ( ( (green) << 8 ) & 0xFF00  ) | \
										 ( ( (blue)  << 16) & 0xFF0000) \
                                       )   

#define IS_GREATER(col1,col2) ( ( (GET_RED(col1)  ) > (GET_RED(col2)  ) + MIN_COLOR_DIFF ) && \
                                ( (GET_GREEN(col1)) > (GET_GREEN(col2)) + MIN_COLOR_DIFF ) &&  \
								( (GET_BLUE(col1) ) > (GET_BLUE(col2) ) + MIN_COLOR_DIFF ) 	   \
							  )

#define MASK_BG	   0
#define MASK_DARK  1
#define MASK_LIGHT 2

// helper function, used internally

static void gray_out_pixmap( int* src, int* dest, int width, int height )
{
	// assuming the pixels along the edges are of the background color
	int bgCol = GET_ELEM(src,0,0);

	int x = 0;
	int y = 1;

	do
	{
		int cur       = GET_ELEM(src,x,y);

		int r = GET_RED(cur);
		int g = GET_GREEN(cur);
		int b = GET_BLUE(cur);

		if ( IS_IN_ARRAY(x-1,y-1) )
		{
			int upperElem = GET_ELEM(src,x-1,y-1);

			// if the upper element is lighter than current
			if ( IS_GREATER(upperElem,cur) )
			{
				GET_ELEM(dest,x,y) = MASK_DARK;
			}
			else
			// if the current element is ligher than the upper
			if ( IS_GREATER(cur,upperElem) )
			{
				GET_ELEM(dest,x,y) = MASK_LIGHT;
			}
			else
			{
				if ( GET_ELEM(dest,x-1,y-1) == MASK_LIGHT )
					
					GET_ELEM(dest,x,y) = MASK_BG;

				if ( GET_ELEM(dest,x-1,y-1 ) == MASK_DARK )

					GET_ELEM(dest,x,y) = MASK_DARK;
				else
					GET_ELEM(dest,x,y) = MASK_BG;
			}
		}

		// go zig-zag

		if ( IS_IN_ARRAY(x+1,y-1) ) 
		{
			++x;--y;
		}
		else
		{
			while( IS_IN_ARRAY(x-1,y+1) ) 
			{
				--x;++y;
			}

			if ( IS_IN_ARRAY(x,y+1) )
			{
					++y; continue;
			}
			else
			{
				if ( IS_IN_ARRAY(x+1,y) )
				{
					++x; continue;
				}
				else break;
			}
		}

	} while(1);
}

// alg. for making the image look "grayed" (e.g. disabled button)
// NOTE:: used GetPixel(), which is Windows-Only!

void greay_out_image_on_dc( wxDC& dc, int width, int height )
{
	// assuming the pixels along the edges are of the background color
	wxColour bgCol;
	dc.GetPixel( 0, 0, &bgCol );

	wxPen darkPen ( wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DSHADOW),1, wxSOLID );
	wxPen lightPen( wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DHIGHLIGHT),1, wxSOLID );
	wxPen bgPen   ( bgCol,                1, wxSOLID );

	int* src  = create_array( width, height, MASK_BG );
	int* dest = create_array( width, height, MASK_BG );

	int y = 0;
	for( y = 0; y != height; ++y )

		for( int x = 0; x != width; ++x )
		{
			wxColour col;
			dc.GetPixel( x,y, &col );

			int r = col.Red(), 
				g = col.Green(), 
				b = col.Blue();

			int o = MAKE_INT_COLOR( r,g,b );

			GET_ELEM(src,x,y) = MAKE_INT_COLOR( col.Red(), col.Green(), col.Blue() );
		}

	gray_out_pixmap( src, dest, width, height );

	for( y = 0; y != height; ++y )

		for( int x = 0; x != width; ++x )
		{
			int mask = GET_ELEM(dest,x,y);

			switch (mask)
			{
				case MASK_BG    : { dc.SetPen( bgPen ); 
					                dc.DrawPoint( x,y ); break;
							      }
				case MASK_DARK  : { dc.SetPen( darkPen ); 
					                dc.DrawPoint( x,y ); break;
							      }
				case MASK_LIGHT : { dc.SetPen( lightPen ); 
					                dc.DrawPoint( x,y ); break;
							      }
				default : break;
			}
		}

	delete [] src;
	delete [] dest;
}

///////////////////////////////

/***** Impelementation for class wxNewBitmapButton *****/

IMPLEMENT_DYNAMIC_CLASS(wxNewBitmapButton, wxPanel)

BEGIN_EVENT_TABLE( wxNewBitmapButton, wxPanel )

	EVT_LEFT_DOWN( wxNewBitmapButton::OnLButtonDown )
	EVT_LEFT_UP  ( wxNewBitmapButton::OnLButtonUp   )
	EVT_MOTION   ( wxNewBitmapButton::OnMouseMove   )

	EVT_SIZE ( wxNewBitmapButton::OnSize  )
	EVT_PAINT( wxNewBitmapButton::OnPaint )

	//EVT_KILL_FOCUS( wxNewBitmapButton::OnKillFocus )

	EVT_ERASE_BACKGROUND( wxNewBitmapButton::OnEraseBackground )

END_EVENT_TABLE()

wxNewBitmapButton::wxNewBitmapButton( const wxBitmap& labelBitmap, 
									  const wxString& labelText,
									  int  alignText,
									  bool isFlat,
									  int  firedEventType, 
									  int  marginX,
									  int  marginY,
									  int  textToLabelGap,
									  bool isSticky)
	: 	mpDepressedImg( NULL ),
		mpPressedImg  ( NULL ),
		mpDisabledImg ( NULL ),
		mpFocusedImg  ( NULL ),

		mMarginX( marginX ),
		mMarginY( marginY ),
		mTextAlignment( alignText ),
		mIsFlat( isFlat ),

		mIsPressed       ( FALSE ),
		mDragStarted     ( FALSE ),
		mPrevPressedState( FALSE ),
		mTextToLabelGap  ( textToLabelGap ),

		mBlackPen( wxColour(  0,  0,  0), 1, wxSOLID ),
		mDarkPen ( wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DSHADOW), 1, wxSOLID ),
		mGrayPen ( wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE), 1, wxSOLID ),
		mLightPen( wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DHIGHLIGHT), 1, wxSOLID ),

		mFiredEventType( firedEventType ),
		mIsSticky( isSticky ),
		mIsCreated( FALSE ),
		mSizeIsSet( FALSE ),

		mHasFocusedBmp( FALSE ),
		mIsInFocus( FALSE ),

		mDepressedBmp( labelBitmap ),
		mLabelText( labelText ),
		mImageFileType( -1 )
{
}

wxNewBitmapButton::wxNewBitmapButton( const wxString& bitmapFileName,
									  const int       bitmapFileType,
									  const wxString& labelText,
									  int  alignText,
									  bool isFlat,
									  int  firedEventType, 
									  int  marginX,
									  int  marginY,
									  int  textToLabelGap,
									  bool isSticky)

	: 	mpDepressedImg( NULL ),
		mpPressedImg  ( NULL ),
		mpDisabledImg ( NULL ),
		mpFocusedImg  ( NULL ),

		mMarginX( 2 ),
		mMarginY( 2 ),
		mTextAlignment( alignText ),
		mIsFlat( isFlat ),

		mIsPressed       ( FALSE ),
		mDragStarted     ( FALSE ),
		mPrevPressedState( FALSE ),
		mTextToLabelGap  ( 2 ),

		mBlackPen( wxColour(  0,  0,  0), 1, wxSOLID ),
		mDarkPen ( wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DSHADOW), 1, wxSOLID ),
		mGrayPen ( wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE), 1, wxSOLID ),
		mLightPen( wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DHIGHLIGHT), 1, wxSOLID ),

		mFiredEventType( wxEVT_COMMAND_MENU_SELECTED ),
		mIsSticky( FALSE ),
		mIsCreated( FALSE ),
		mSizeIsSet( FALSE ),

		mHasFocusedBmp( FALSE ),
		mIsInFocus( FALSE ),

		mLabelText( labelText ),
		mImageFileName( bitmapFileName ),
		mImageFileType( bitmapFileType )
{
	//mDepressedBmp.LoadFile( bitmapFileName, bitmapFileType );
}

wxNewBitmapButton::~wxNewBitmapButton(void) 
{
	DestroyLabels();
}

void wxNewBitmapButton::DrawShade( int outerLevel,
								   wxDC&  dc,
							       wxPen& upperLeftSidePen,
								   wxPen& lowerRightSidePen )
{
	wxBitmap* pBmp = GetStateLabel();

	int x = mMarginX - (outerLevel + 1);
	int y = mMarginY - (outerLevel + 1);

	int height = pBmp->GetHeight() + (outerLevel + 1)*2 - 1;
	int width  = pBmp->GetWidth()  + (outerLevel + 1)*2 - 1;

	dc.SetPen( upperLeftSidePen );
	dc.DrawLine( x,y, x + width, y  );
	dc.DrawLine( x,y, x, y + height );

	dc.SetPen( lowerRightSidePen );
	dc.DrawLine( x + width, y, x + width, y + height + 1  );
	dc.DrawLine( x, y + height, x + width, y + height );
}

void wxNewBitmapButton::DestroyLabels()
{
	if ( mpDepressedImg ) delete mpDepressedImg;
	if ( mpPressedImg   ) delete mpPressedImg;
	if ( mpDisabledImg  ) delete mpDisabledImg;
	if ( mpFocusedImg   ) delete mpFocusedImg;

	mpDepressedImg = NULL;
	mpPressedImg   = NULL;
	mpDisabledImg  = NULL;
	mpFocusedImg   = NULL;
}

wxBitmap* wxNewBitmapButton::GetStateLabel()
{
	if ( IsEnabled() )
	{
		if ( mIsPressed )
		{
			return mpPressedImg;
		}
		else
		{
			if ( mIsInFocus )
			{
				if ( mHasFocusedBmp )

					return mpFocusedImg;
				else
					return mpDepressedImg;
			}
			else
				return mpDepressedImg;
		}
	}
	else
		return mpDisabledImg;
}

void wxNewBitmapButton::RenderLabelImage( wxBitmap*& destBmp, wxBitmap* srcBmp,
										  bool isEnabled, bool isPressed )
{
	if ( destBmp != 0 ) return;

	// render lables on-demand

	wxMemoryDC srcDc;
	srcDc.SelectObject( *srcBmp );

	bool hasText = ( mTextAlignment != NB_NO_TEXT ) &&
		           ( mLabelText.length() != 0 );

	bool hasImage = (mTextAlignment != NB_NO_IMAGE);

	wxSize destDim;
	wxPoint txtPos;
	wxPoint imgPos;

	if ( hasText )
	{
		long txtWidth, txtHeight;

		srcDc.SetFont( wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT) );
		srcDc.GetTextExtent( mLabelText, &txtWidth, &txtHeight );

		if ( mTextAlignment == NB_ALIGN_TEXT_RIGHT )
		{
			destDim.x = srcBmp->GetWidth() + 2*mTextToLabelGap + txtWidth;

			destDim.y = 
				wxMax( srcBmp->GetHeight(), txtHeight );

			txtPos.x = srcBmp->GetWidth() + mTextToLabelGap;
			txtPos.y = (destDim.y - txtHeight)/2;
			imgPos.x = 0;
			imgPos.y = (destDim.y - srcBmp->GetHeight())/2;
		}
		else
		if ( mTextAlignment == NB_ALIGN_TEXT_BOTTOM )
		{
			destDim.x = 
				wxMax( srcBmp->GetWidth(), txtWidth );

			destDim.y = srcBmp->GetHeight() + mTextToLabelGap + txtHeight;

			txtPos.x = (destDim.x - txtWidth)/2;
			txtPos.y = srcBmp->GetHeight() + mTextToLabelGap;
			imgPos.x = (destDim.x - srcBmp->GetWidth())/2;
			imgPos.y = 0;
		}
		else wxASSERT(0);// unsupported alignment type
	}
	else
	{
		imgPos.x = 0;
		imgPos.y = 0;
		destDim.x = srcBmp->GetWidth();
		destDim.y = srcBmp->GetHeight();
	}

	destBmp = new wxBitmap( int(destDim.x), int(destDim.y) );

	wxMemoryDC destDc;
	destDc.SelectObject( *destBmp );

	wxBrush grayBrush( wxSystemSettings::GetSystemColour( wxSYS_COLOUR_3DFACE), wxSOLID );
	wxPen   nullPen( wxColour(0,0,0), 1, wxTRANSPARENT );

	destDc.SetBrush( grayBrush );
	destDc.SetPen( nullPen );

	destDc.DrawRectangle( 0,0, destDim.x+1, destDim.y+1 );

	if ( isPressed )
	{
		++imgPos.x; ++imgPos.y;
		++txtPos.x; ++txtPos.y;
	}

	if ( hasImage )
	{
		destDc.Blit( imgPos.x, imgPos.y, 
					 srcBmp->GetWidth()+1,
					 srcBmp->GetHeight()+1,
					 &srcDc, 0,0, wxCOPY,TRUE );
	}

	if ( hasText )
	{
		wxWindow* pTopWnd = this;

		do
		{
			wxWindow* pParent = pTopWnd->GetParent();

			if ( pParent == 0 ) break;

			pTopWnd = pParent;
		} while(1);

		destDc.SetFont( wxSystemSettings::GetSystemFont( wxSYS_DEFAULT_GUI_FONT) );

		// Should be wxSYS_COLOUR_BTNTEXT, but gtk gives white?
		destDc.SetTextForeground( wxSystemSettings::GetSystemColour(wxSYS_COLOUR_BTNTEXT) );
		destDc.SetTextBackground( wxSystemSettings::GetSystemColour(wxSYS_COLOUR_BTNFACE) );

		destDc.DrawText( mLabelText, txtPos.x, txtPos.y );
	}

	if ( !isEnabled )
	
		greay_out_image_on_dc( destDc, destDim.x, destDim.y );

	// adjust button size to fit the new dimensions of the label
	if ( !mSizeIsSet && 0 )
	{
		mSizeIsSet = TRUE; 
		SetSize( -1,-1, 
				 destBmp->GetWidth()  + mMarginX*2,
				 destBmp->GetHeight() + mMarginY*2, 0 
			);
	}
}

void wxNewBitmapButton::RenderLabelImages()
{
	if ( !mIsCreated ) return;

	if ( !IsEnabled() )
	{
		RenderLabelImage( mpDisabledImg, &mDepressedBmp, FALSE );
	}
	else

	if ( mIsPressed ) 

		RenderLabelImage( mpPressedImg,   &mDepressedBmp, TRUE, TRUE );
	else
	{
		if ( mIsInFocus )
		{
			if ( mHasFocusedBmp )
			
				RenderLabelImage( mpFocusedImg, &mFocusedBmp, TRUE, FALSE );
			else
				RenderLabelImage( mpDepressedImg, &mDepressedBmp, TRUE, FALSE );
		}
		else
			RenderLabelImage( mpDepressedImg, &mDepressedBmp, TRUE, FALSE );
	}
}

void wxNewBitmapButton::DrawDecorations( wxDC& dc )
{
	if ( mIsFlat )
	{
		DrawShade( 1, dc, mGrayPen,  mGrayPen  );
		
		if ( mIsInFocus )
		{
			if ( mIsPressed )
		
				DrawShade( 0, dc, mDarkPen,  mLightPen  );
			else
				DrawShade( 0, dc, mLightPen,  mDarkPen  );
		}
		else
			DrawShade( 0, dc, mGrayPen,  mGrayPen  );
	}
	else
	{
		if ( mIsPressed )
		{
			DrawShade( 0, dc, mDarkPen,  mGrayPen  );
			DrawShade( 1, dc, mBlackPen, mLightPen );
		}
		else
		{
			DrawShade( 0, dc, mGrayPen,  mDarkPen  );
			DrawShade( 1, dc, mLightPen, mBlackPen );
		}
	}
}

void wxNewBitmapButton::SetLabel(const wxBitmap& labelBitmap, 
								 const wxString& labelText )
{
	DestroyLabels();

	mLabelText    = labelText;
	mDepressedBmp = labelBitmap;

	RenderLabelImages();
}

void wxNewBitmapButton::SetAlignments( int alignText,
								       int marginX,
									   int marginY,
									   int textToLabelGap)
{
	DestroyLabels();

	mMarginX        = marginX;
	mMarginY        = marginY;
	mTextAlignment  = alignText;
	mTextToLabelGap = textToLabelGap;

	RenderLabelImages();
}

// event handlers

void wxNewBitmapButton::OnLButtonDown( wxMouseEvent& event )
{
	mPrevPressedState = FALSE;   
	mDragStarted      = TRUE;
	mIsPressed        = TRUE;
	Refresh();

	if ( !mIsInFocus )
	
			CaptureMouse();
}

void wxNewBitmapButton::OnLButtonUp( wxMouseEvent& event )
{
	if ( !mDragStarted ) return;

	mDragStarted = FALSE;
	mIsPressed   = FALSE;
	mIsInFocus   = FALSE;
	Refresh();

	ReleaseMouse();

	if ( IsInWindow( event.m_x, event.m_y ) ) 
	{
		// fire event, if mouse was released
		// within the bounds of button
		wxCommandEvent cmd( mFiredEventType, GetId() );
		GetParent()->ProcessEvent( cmd );
	}
}

bool wxNewBitmapButton::IsInWindow( int x, int y )
{
	int width, height;
	GetSize( &width, &height );

	return ( x >= 0 && y >= 0 &&
			 x < width &&
			 y < height );
}

void wxNewBitmapButton::OnMouseMove( wxMouseEvent& event )
{
	if ( !mIsInFocus && IsInWindow( event.m_x, event.m_y ) )
	{
		if ( !mDragStarted )
			CaptureMouse();

		mIsInFocus = TRUE;
	}
	else
	if ( mIsInFocus && !IsInWindow( event.m_x, event.m_y ) )
	{
		mIsInFocus = FALSE;

		if ( !mDragStarted )
			ReleaseMouse();
	}

	if ( mDragStarted )
	{
		if ( IsInWindow( event.m_x, event.m_y ) )

			mIsPressed = TRUE;
		else
			mIsPressed = FALSE;

		if ( mIsPressed != mPrevPressedState )
			
			Refresh();

		mPrevPressedState = mIsPressed;
	}

	// FOR NOW::
	Refresh();
}

void wxNewBitmapButton::OnSize( wxSizeEvent& event )
{
		//Reshape();
}

void wxNewBitmapButton::Reshape( )
{
	bool wasCreated = mIsCreated;
	mIsCreated = TRUE;

	if ( !wasCreated )
	{
		// in the case of loading button from stream, check if we
		// have non-empty image-file name, load if possible 

		if ( mImageFileName != "" )
		{
			mDepressedBmp.LoadFile( mImageFileName, mImageFileType );

			//wxMessageBox("Image Loaded!!!");
		}

		RenderLabelImages();

		wxBitmap* pCurImg = GetStateLabel();

		int w = pCurImg->GetWidth(),
			h = pCurImg->GetHeight();

		SetSize( 0,0, w + mMarginX*2, h + mMarginY*2 , 0 );
	}
}

void wxNewBitmapButton::DrawLabel( wxDC& dc )
{
	wxBitmap* pCurBmp = GetStateLabel();

	if ( pCurBmp == NULL )
	{
		wxSizeEvent evt;
		OnSize( evt ); // fake it up!

		RenderLabelImages();
		pCurBmp = GetStateLabel();
	}

	wxMemoryDC mdc;
	mdc.SelectObject( *pCurBmp );

	dc.Blit( mMarginX, mMarginY, 
			 pCurBmp->GetWidth(),
			 pCurBmp->GetHeight(),
			 &mdc, 0,0, wxCOPY 
		   );

	mdc.SelectObject( wxNullBitmap );
}

void wxNewBitmapButton::OnPaint( wxPaintEvent& event )
{
	wxPaintDC dc(this);

	// first, make sure images for current state are prepared
	RenderLabelImages();

	DrawLabel( dc );

	DrawDecorations( dc );
}

void wxNewBitmapButton::OnEraseBackground( wxEraseEvent& event )
{
	// do nothing
}

void wxNewBitmapButton::OnKillFocus( wxFocusEvent& event )
{
	// useless

	wxMessageBox("kill-focus for button!");
}

