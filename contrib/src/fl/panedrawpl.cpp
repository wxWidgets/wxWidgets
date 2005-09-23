/////////////////////////////////////////////////////////////////////////////
// Name:        panedrawpl.cpp
// Purpose:     cbPaneDrawPlugin implementation.
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     06/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <math.h>
#include <stdlib.h>

#include "wx/utils.h"     // import wxMin,wxMax macros

#include "wx/fl/panedrawpl.h"

// bitmap bits used by bar-resizing brush

#define _IMG_A  0xAA    // Note: modified from _A to _IMG_A, _A was already defined (cygwin)
#define _IMG_B  0x00    // Note: modified from _B to _IMG_A, _B was already defined (cygwin)
#define _IMG_C  0x55    // Note: modified from _C to _IMG_C, for consistency reasons.
#define _IMG_D  0x00    // Note: modified from _D to _IMG_D, for consistency reasons.

static const unsigned char _gCheckerImg[16] = { _IMG_A,_IMG_B,_IMG_C,_IMG_D,
                                                _IMG_A,_IMG_B,_IMG_C,_IMG_D,
                                                _IMG_A,_IMG_B,_IMG_C,_IMG_D,
                                                _IMG_A,_IMG_B,_IMG_C,_IMG_D
                                              };

// FIXME:: The below code somehow doesn't work - cursors remain unchanged
// Used: controlbar.cpp(1268):    set_cursor_bits( _gHorizCursorImg, bits, 32, 16 );
// Used: controlbar.cpp(1272):    set_cursor_bits( _gVertCursorImg, bits, 32, 16 );
/*
static void set_cursor_bits( const char** img, char* bits, int width, int height )
{
    for( int i = 0; i != (width*height)/8; ++i )
        bits[i] = 0;

    for( int y = 0; y != height; ++y )
    {
        const char* row = img[0];

        for( int x = 0; x != width; ++x )
        {
            int bitNo = y*width + x;

            char value = ( row[x] != '.' ) ? 1 : 0;

            bits[ bitNo / sizeof(char) ] |= 
                ( ( bitNo %sizeof(char) ) << value );
        }

        ++img;
    }
}
*/

/***** Implementation for class cbPaneDrawPlugin *****/

IMPLEMENT_DYNAMIC_CLASS( cbPaneDrawPlugin, cbPluginBase )

BEGIN_EVENT_TABLE( cbPaneDrawPlugin, cbPluginBase )

    EVT_PL_LEFT_DOWN           ( cbPaneDrawPlugin::OnLButtonDown         )
    EVT_PL_LEFT_UP               ( cbPaneDrawPlugin::OnLButtonUp           )
//    EVT_PL_LEFT_DCLICK           ( cbPaneDrawPlugin::OnLDblClick           )
    EVT_PL_RIGHT_UP               ( cbPaneDrawPlugin::OnRButtonUp           )
    EVT_PL_MOTION               ( cbPaneDrawPlugin::OnMouseMove           )


    EVT_PL_DRAW_PANE_BKGROUND  ( cbPaneDrawPlugin::OnDrawPaneBackground  )
    EVT_PL_DRAW_PANE_DECOR     ( cbPaneDrawPlugin::OnDrawPaneDecorations )

    EVT_PL_DRAW_ROW_DECOR      ( cbPaneDrawPlugin::OnDrawRowDecorations  )
    EVT_PL_DRAW_ROW_HANDLES    ( cbPaneDrawPlugin::OnDrawRowHandles      )
    EVT_PL_DRAW_ROW_BKGROUND   ( cbPaneDrawPlugin::OnDrawRowBackground   )

    EVT_PL_SIZE_BAR_WND           ( cbPaneDrawPlugin::OnSizeBarWindow       )
    EVT_PL_DRAW_BAR_DECOR       ( cbPaneDrawPlugin::OnDrawBarDecorations  )
    EVT_PL_DRAW_BAR_HANDLES       ( cbPaneDrawPlugin::OnDrawBarHandles      )

    EVT_PL_START_DRAW_IN_AREA  ( cbPaneDrawPlugin::OnStartDrawInArea     )
    EVT_PL_FINISH_DRAW_IN_AREA ( cbPaneDrawPlugin::OnFinishDrawInArea    )

END_EVENT_TABLE()

cbPaneDrawPlugin::cbPaneDrawPlugin(void)

    : mResizeStarted          ( false ),

      mResizeCursorOn         ( false ),
      mpDraggedBar              ( NULL  ),
      mpResizedRow            ( NULL  ),

      mRowHandleHitted        ( false ),
      mIsUpperHandle          ( false ),
      mBarHandleHitted        ( false ),
      mIsLeftHandle           ( false ),
      mBarContentHitted       ( false ),

      mpClntDc ( NULL ),
      mpPane   ( NULL )
{}

cbPaneDrawPlugin::cbPaneDrawPlugin( wxFrameLayout* pPanel, int paneMask )

    : cbPluginBase( pPanel, paneMask ),
    
      // bar-row resizing state varaibles

      mResizeStarted          ( false ),

      mResizeCursorOn         ( false ),
      mpDraggedBar              ( NULL ),
      mpResizedRow            ( NULL ),

      mRowHandleHitted        ( false ),
      mIsUpperHandle          ( false ),
      mBarHandleHitted        ( false ),
      mIsLeftHandle           ( false ),
      mBarContentHitted       ( false ),

      mpClntDc ( NULL ),
      mpPane   ( NULL )
{}

cbPaneDrawPlugin::~cbPaneDrawPlugin()
{
    // DBG::
    wxASSERT( mpClntDc == NULL );
}

void cbPaneDrawPlugin::DrawDraggedHandle( const wxPoint& pos, cbDockPane& pane )
{
    wxScreenDC dc;
    int ofsX = 0;
    int ofsY = 0;

    wxPoint fpos = pos;
    pane.PaneToFrame( &fpos.x, &fpos.y );

    // short-cut
    int resizeHndSize = pane.mProps.mResizeHandleSize;

    // "Required for X to specify that
    // that we wish to draw on top of all windows
    // - and we optimise by specifying the area
    // for creating the overlap window." --J.S.

    wxScreenDC::StartDrawingOnTop(&mpLayout->GetParentFrame());

    mpLayout->GetParentFrame().ClientToScreen( &ofsX, &ofsY );

    int prevLF = dc.GetLogicalFunction();

    // BUG BUG BUG (wx):: somehow stippled brush works only  
    //                      when the bitmap created on stack, not
    //                      as a member of the class

    wxBitmap checker( (const char*)_gCheckerImg, 8,8 );

    wxBrush checkerBrush( checker );

    dc.SetPen( mpLayout->mNullPen );
    dc.SetBrush( checkerBrush );
    dc.SetLogicalFunction( wxXOR );

    if ( mHandleIsVertical )
    {
        int delta = pos.x - mDragOrigin.x;

        if ( !pane.IsHorizontal() )

            delta = pos.y - mDragOrigin.y;

        int realHndOfs;
        realHndOfs = pane.mBoundsInParent.x + pane.mLeftMargin + mHandleOfs;

        int newX = realHndOfs + delta;

        if ( newX + resizeHndSize > mHandleDragArea.x + mHandleDragArea.width )

            newX = mHandleDragArea.x + mHandleDragArea.width  - 1;

        if ( newX < mHandleDragArea.x ) 

            newX = mHandleDragArea.x;

        mDraggedDelta = newX - realHndOfs;

        dc.DrawRectangle( newX + ofsX, mHandleDragArea.y + ofsY,
                          resizeHndSize + 1,
                          mHandleDragArea.height+1 );
    }
    else
    {
        // otherwise, draw horizontal handle

        int delta = pos.y - mDragOrigin.y;

        if ( !pane.IsHorizontal() )

            delta = pos.x - mDragOrigin.x;

        int realHndOfs;
        realHndOfs = pane.mBoundsInParent.y + pane.mTopMargin + mHandleOfs;

        int newY = realHndOfs + delta;

        if ( newY + resizeHndSize > mHandleDragArea.y + mHandleDragArea.height )

            newY = mHandleDragArea.y + mHandleDragArea.height - 1;

        if ( newY < mHandleDragArea.y ) 

            newY = mHandleDragArea.y;

        mDraggedDelta = newY - realHndOfs;

        dc.DrawRectangle( mHandleDragArea.x + ofsX, newY + ofsY,
                          mHandleDragArea.width + 1,
                          resizeHndSize + 1 );
    }

    dc.SetLogicalFunction( prevLF );

    // "End drawing on top (frees the window used for drawing
    // over the screen)" --J.S.
    wxScreenDC::EndDrawingOnTop();
}

void cbPaneDrawPlugin::OnMouseMove( cbMotionEvent& event ) 
{
    if ( !mResizeStarted )
    {
        // if nothing is started, do hit-tests

        bool prevWasRowHandle = mRowHandleHitted;

        mBarContentHitted = false;
        mBarHandleHitted  = false;
        mRowHandleHitted  = false;

        int testResult =  
            event.mpPane->HitTestPaneItems( event.mPos,        // in pane's coordiantes
                                            &mpResizedRow,
                                            &mpDraggedBar );

        if ( testResult != CB_NO_ITEMS_HITTED )
        {
            if ( testResult == CB_BAR_CONTENT_HITTED )
            {
                // restore cursor, if non of the handles were hit
                if ( mResizeCursorOn )
                {
                    // remove resizing hints

                    mpLayout->ReleaseEventsFromPane( event.mpPane );
                    mpLayout->ReleaseEventsFromPlugin( this );
    
                    mResizeCursorOn = false;

                    mBarContentHitted = true;

                    // In Windows, at least, the frame needs to have a null cursor
                    // else child windows (such as text windows) inherit the cursor
#if 1
                    mpLayout->GetParentFrame().SetCursor( wxNullCursor );
#else
                    mpLayout->GetParentFrame().SetCursor( *mpLayout->mpNormalCursor );
#endif
                }

                // TBD:: fire something like "mouse-over-bar" event

                event.Skip(); // pass event to the next handler in the chain
                return;
            }

            wxCursor* pCurs;

            if ( testResult == CB_UPPER_ROW_HANDLE_HITTED ||
                 testResult == CB_LOWER_ROW_HANDLE_HITTED)
            {
                if ( event.mpPane->IsHorizontal() )

                    pCurs = mpLayout->mpVertCursor;
                else
                    pCurs = mpLayout->mpHorizCursor;

                mRowHandleHitted = true;
                mIsUpperHandle    = ( testResult == CB_UPPER_ROW_HANDLE_HITTED );
            }
            else
            {
                // otherwise, if inter-bar handle was hitted

                if ( event.mpPane->IsHorizontal() )

                    pCurs = mpLayout->mpHorizCursor;
                else
                    pCurs = mpLayout->mpVertCursor;

                mBarHandleHitted = true;
                mIsLeftHandle    = ( testResult == CB_LEFT_BAR_HANDLE_HITTED );
            }

            // avoid setting the same cursor twice

            if ( !mResizeCursorOn || prevWasRowHandle != mRowHandleHitted )
            {
                mpLayout->GetParentFrame().SetCursor( *pCurs );

                if ( !mResizeCursorOn )
                {
                    // caputre if not captured yet
                    mpLayout->CaptureEventsForPane( event.mpPane );
                    mpLayout->CaptureEventsForPlugin( this );
                }
            }

            mResizeCursorOn = true;

            // handled is being dragged now, thus event is "eaten" by this plugin

            return;

        } // end of if (HitTestBarHandles())

        // restore cursor, if non of the handles were hit
        if ( mResizeCursorOn )
        {
            mpLayout->ReleaseEventsFromPane( event.mpPane );
            mpLayout->ReleaseEventsFromPlugin( this );

            // In Windows, at least, the frame needs to have a null cursor
            // else child windows (such as text windows) inherit the cursor
#if 1
            mpLayout->GetParentFrame().SetCursor( wxNullCursor );
#else
            mpLayout->GetParentFrame().SetCursor( *mpLayout->mpNormalCursor );
#endif

            mResizeCursorOn = false;
        }

        event.Skip(); // pass event to the next plugin
    }

    // othewise series of actions, if something has already started

    else
    if ( mResizeStarted )
    {
        // apply xor-mask twice 
        DrawDraggedHandle( mPrevPos,   *event.mpPane );

        // draw handle in the new position
        DrawDraggedHandle( event.mPos, *event.mpPane );
        mPrevPos = event.mPos;

        // handled is dragged, thus event is "eaten" by this plugin
    }
    else
        event.Skip(); // pass event to the next plugin
}

void cbPaneDrawPlugin::OnLDblClick( cbLeftDClickEvent& event )
{
    if ( !mResizeCursorOn )
    {
        cbBarInfo* pBarToFloat;

        if ( event.mpPane->HitTestPaneItems( event.mPos,       // in pane's coordiantes
                                             &mpResizedRow,
                                             &pBarToFloat ) == CB_BAR_CONTENT_HITTED
           )
            {
            return;
            }

        event.Skip();
    }
}

void cbPaneDrawPlugin::OnLButtonDown( cbLeftDownEvent& event ) 
{
    wxASSERT( !mResizeStarted );

    if ( mResizeCursorOn )
    {
        mResizeStarted = true;
        mDragOrigin    = event.mPos;
        
        // setup constraints for the dragging handle

        int from, till;
        mHandleOfs        = 0;
        mHandleIsVertical = false;

        if ( mRowHandleHitted )

            event.mpPane->GetRowResizeRange( mpResizedRow, &from, &till, mIsUpperHandle );
        else
            // otherwise if bar handle was hitted
            event.mpPane->GetBarResizeRange( mpDraggedBar, &from, &till, mIsLeftHandle );
         
        if ( mRowHandleHitted )
        {
            mHandleIsVertical = ( event.mpPane->IsHorizontal() ) ? false : true;

            mHandleDragArea.x      = 0;
            mHandleDragArea.width  = event.mpPane->mPaneWidth;

            mHandleDragArea.y      = from;
            mHandleDragArea.height = till - from;

            if ( mIsUpperHandle )

                mHandleOfs = mpResizedRow->mRowY;
            else
                mHandleOfs = mpResizedRow->mRowY + 
                             mpResizedRow->mRowHeight -
                             event.mpPane->mProps.mResizeHandleSize;
        }
        else
        {
            // otehrwise if bar handle dragged

//            cbRowInfo& rowInfo     = *mpDraggedBar->mpRow;
            wxRect& bounds         = mpDraggedBar->mBounds;

            mHandleIsVertical = ( event.mpPane->IsHorizontal() ) ? true : false;

            mHandleDragArea.x      = from;
            mHandleDragArea.width  = till - from;


            mHandleDragArea.y      = bounds.y;
            mHandleDragArea.height = bounds.height;

            // left-side-handle mBounds
            if ( mIsLeftHandle )

                mHandleOfs = bounds.x;
            else
                mHandleOfs = bounds.x + 
                             bounds.width - event.mpPane->mProps.mResizeHandleSize;

        }

        event.mpPane->PaneToFrame( &mHandleDragArea );
        DrawDraggedHandle(mDragOrigin, *event.mpPane);

        mPrevPos = mDragOrigin;

        return;
        // handled is dragged, thus event is "eaten" by this plugin
    }
    else
    {
        cbBarInfo* pDraggedBar;

        if ( event.mpPane->HitTestPaneItems( event.mPos,       // in pane's coordiantes
                                             &mpResizedRow,
                                             &pDraggedBar ) == CB_BAR_CONTENT_HITTED 
           )
        {
            int x = event.mPos.x,
                y = event.mPos.y;

            event.mpPane->PaneToFrame( &x, &y );

            cbStartBarDraggingEvent dragEvt( pDraggedBar, wxPoint(x,y), event.mpPane );

            mpLayout->FirePluginEvent( dragEvt );

            return; // event is "eaten" by this plugin
        }
    }

    event.Skip(); // pass event to the next plugin in the chain
}

void cbPaneDrawPlugin::OnLButtonUp( cbLeftUpEvent& event ) 
{
    if ( mResizeStarted )
    {
        DrawDraggedHandle( event.mPos, *event.mpPane );

        mResizeStarted  = false;
        mResizeCursorOn = false;

        mpLayout->ReleaseEventsFromPane( event.mpPane );
        mpLayout->ReleaseEventsFromPlugin( this );

        // In Windows, at least, the frame needs to have a null cursor
        // else child windows (such as text windows) inherit the cursor
#if 1
        mpLayout->GetParentFrame().SetCursor( wxNullCursor );
#else
        mpLayout->GetParentFrame().SetCursor( *mpLayout->mpNormalCursor );
#endif

        if ( mRowHandleHitted )
        {
            event.mpPane->ResizeRow( mpResizedRow, 
                                     mDraggedDelta,
                                     mIsUpperHandle );
        }
        else
        {
            event.mpPane->ResizeBar( mpDraggedBar, 
                                     mDraggedDelta,
                                     mIsLeftHandle );
        }

        mpDraggedBar = NULL;
        mpResizedRow = NULL;

        // handled dragging action was finished by this mouse-up, 
        // thus event is "eaten" by this plugin

        return;
    }

    event.Skip(); // pass event to the next plugin
}

void cbPaneDrawPlugin::OnRButtonUp( cbRightUpEvent&   event )
{
    wxPoint fpos = event.mPos;
    event.mpPane->PaneToFrame( &fpos.x, &fpos.y );

    cbBarInfo* pDraggedBar;

    // user clicks inside the bar contnet, fire bar-customization event

    if ( event.mpPane->HitTestPaneItems( event.mPos,       // in pane's coordiantes
                                         &mpResizedRow,
                                         &pDraggedBar  ) == CB_BAR_CONTENT_HITTED
       )
    {
        cbCustomizeBarEvent cbEvt( pDraggedBar, fpos, event.mpPane );

        mpLayout->FirePluginEvent( cbEvt );

        return; // event is "eaten" by this plugin
    }

    // otherwise fire whole-layout customization event

    cbCustomizeLayoutEvent csEvt( fpos );

    mpLayout->FirePluginEvent( csEvt );

    // event is "eaten" by this plugin
}

void cbPaneDrawPlugin::OnSizeBarWindow( cbSizeBarWndEvent& event ) 
{
    cbBarInfo& bar = *event.mpBar;
    mpPane         = event.mpPane;

    // it's possible that a bar does not have it's own window!
    if ( !bar.mpBarWnd ) return;

    wxRect& bounds = event.mBoundsInParent;

    // check visibility
    if ( bounds.height != 0 )
    {
        // size smaller than bounds, to leave space for shade lines

        // FIXME:: +/- 1s

        int nNewHeight = bounds.height - 2 - bar.mDimInfo.mVertGap *2;
        if(nNewHeight < 0)
           nNewHeight = 0;

        bar.mpBarWnd->wxWindow::SetSize( bounds.x      + 1 + bar.mDimInfo.mHorizGap,     
                                         bounds.y      + 1 + bar.mDimInfo.mVertGap,
                                         bounds.width  - 2 - bar.mDimInfo.mHorizGap*2,
                                         nNewHeight,
                                         0 
                                       );

        if ( !bar.mpBarWnd->IsShown() )

            bar.mpBarWnd->Show( true );
    }
    else
        // hide bar if not visible
        bar.mpBarWnd->Show( false );

    event.Skip(); // pass event to the next plugin in the chain
}

void cbPaneDrawPlugin::OnDrawRowDecorations( cbDrawRowDecorEvent& event )
{
    DrawPaneShadeForRow( event.mpRow, *event.mpDc );

    event.Skip(); // pass event to the next plugin
}

void cbPaneDrawPlugin::DrawUpperRowHandle( cbRowInfo* pRow, wxDC& dc )
{
    wxRect& bounds = pRow->mBoundsInParent;

    if ( mpPane->IsHorizontal() )
    {
         if ( pRow->mHasUpperHandle )
        
            mpPane->DrawHorizHandle( dc, bounds.x, 
                                     bounds.y-1, 
                                      pRow->mRowWidth );
    }
    else
    {
        if ( pRow->mHasUpperHandle )

            mpPane->DrawVertHandle( dc, bounds.x-1, 
                                    bounds.y, pRow->mRowWidth );
    }
}

void cbPaneDrawPlugin::DrawLowerRowHandle( cbRowInfo* pRow, wxDC& dc )
{
    wxRect& bounds = pRow->mBoundsInParent;

    // check if iter-row handles present

    if ( mpPane->IsHorizontal() )
    {
        if ( pRow->mHasLowerHandle )
        
            mpPane->DrawHorizHandle( dc, bounds.x, bounds.y + bounds.height - mpPane->mProps.mResizeHandleSize - 1, 
                                      pRow->mRowWidth );
    }
    else
    {
        if ( pRow->mHasLowerHandle )

            mpPane->DrawVertHandle( dc, bounds.x + bounds.width - mpPane->mProps.mResizeHandleSize - 1, 
                                     bounds.y, pRow->mRowWidth );
    }
}

void cbPaneDrawPlugin::OnDrawRowHandles( cbDrawRowHandlesEvent& event )
{
    // short-cuts
    cbRowInfo* pRow = event.mpRow;
    wxDC&   dc      = *event.mpDc;
    mpPane          = event.mpPane;

    // draw handles of surrounding rows first

    if ( pRow->mpPrev && pRow->mpPrev->mHasLowerHandle )

            DrawLowerRowHandle( pRow->mpPrev, dc );

    if ( pRow->mpNext && pRow->mpNext->mHasUpperHandle )

        DrawUpperRowHandle( pRow->mpNext, dc );

    // draw handles of the given row

    if ( pRow->mHasUpperHandle )
    
        DrawUpperRowHandle( pRow, dc );

    if ( pRow->mHasLowerHandle )

        DrawLowerRowHandle( pRow, dc );

    event.Skip(); // pass event to the next plugin
}

void cbPaneDrawPlugin::OnDrawPaneBackground ( cbDrawPaneBkGroundEvent& event )
{
    wxDC& dc = *event.mpDc;
    mpPane   = event.mpPane;

    // FOR NOW:: hard-coded
    wxBrush bkBrush( mpLayout->mBorderPen.GetColour(), wxSOLID );

    dc.SetBrush( bkBrush );
    dc.SetPen( mpLayout->mNullPen );

    wxRect& bounds = mpPane->mBoundsInParent;

    if ( mpPane->mTopMargin >= 1 )
    
        dc.DrawRectangle( bounds.x, bounds.y,
                          bounds.width+1,
                          mpPane->mTopMargin + 1);


    if ( mpPane->mBottomMargin >= 1 )
    
        dc.DrawRectangle( bounds.x, 
                          bounds.y + bounds.height - mpPane->mBottomMargin,
                          bounds.width + 1,
                          mpPane->mBottomMargin + 1);


    if ( mpPane->mLeftMargin >= 1 )
    
        dc.DrawRectangle( bounds.x, 
                          bounds.y + mpPane->mTopMargin - 1,
                          mpPane->mLeftMargin + 1,
                          bounds.height - mpPane->mTopMargin - mpPane->mBottomMargin + 2);


    if ( mpPane->mRightMargin >= 1 )
    
        dc.DrawRectangle( bounds.x + bounds.width - mpPane->mRightMargin,
                          bounds.y + mpPane->mTopMargin - 1,
                          mpPane->mRightMargin + 1,
                          bounds.height - mpPane->mTopMargin - mpPane->mBottomMargin + 2);

    event.Skip(); // pass event to the next plugin
}

void cbPaneDrawPlugin::OnDrawRowBackground ( cbDrawRowBkGroundEvent& event )
{
    // short-cuts
    cbRowInfo* pRow = event.mpRow;
    wxDC&   dc      = *event.mpDc;
    mpPane          = event.mpPane;

    // get ready
    wxRect     rowBounds    = pRow->mBoundsInParent;
    bool       isHorizontal    = event.mpPane->IsHorizontal();
    
//    int prevPos;

    if ( isHorizontal )
    {
//        prevPos = rowBounds.x;
        // include one line above and below the row
        --rowBounds.y;
        rowBounds.height += 2;

        --rowBounds.x;
        rowBounds.width  += 2;
    }
    else
    {
//        prevPos = rowBounds.y;
        // include one line above and below the row
        --rowBounds.x;
        rowBounds.width  += 2;

        --rowBounds.y;
        rowBounds.height += 2;
    }

//#define TEST_BK_ERASING

#ifdef TEST_BK_ERASING

    // DBG::
    wxBrush br0( wxColour(0,160,160), wxSOLID );
    dc.SetBrush(br0);
    dc.SetPen  ( mpLayout->mNullPen );
    dc.DrawRectangle( rowBounds.x, rowBounds.y,
                      rowBounds.width  + 1, 
                      rowBounds.height + 1  );
#endif

    wxBrush bkBrush( mpLayout->mGrayPen.GetColour(), wxSOLID );

    dc.SetPen  ( mpLayout->mNullPen );
    dc.SetBrush( bkBrush );

    // fill background-recatangle of entire row area
    dc.DrawRectangle( rowBounds.x, rowBounds.y,
                      rowBounds.width  + 1, 
                      rowBounds.height + 1  );

    dc.SetBrush( wxNullBrush );

    // draw "shaded-side-bars" for each bar
    for( size_t i = 0; i != pRow->mBars.Count(); ++i )
    {
        wxRect& bounds = pRow->mBars[i]->mBoundsInParent;

        if ( isHorizontal )
        {
            DrawShade( 1, bounds, FL_ALIGN_LEFT, dc );
            DrawShade( 1, bounds, FL_ALIGN_RIGHT, dc );
        }
        else
        {
            DrawShade( 1, bounds, FL_ALIGN_TOP, dc );
            DrawShade( 1, bounds, FL_ALIGN_BOTTOM, dc );
        }
    }

    // draw extra shades to simulate "glued-bricks" effect

    // TBD:: reduce exessive drawing of shades, when the
    //       row handle is present, and shades will be overr-drawn anyway

    DrawUpperRowShades( pRow, dc, 1 ); // outer shade

    if ( pRow->mpPrev )
    {
        DrawLowerRowShades( pRow->mpPrev, dc, 1 ); // outter shade
        DrawLowerRowShades( pRow->mpPrev, dc, 0 ); // inner shade
    }

    DrawLowerRowShades( pRow, dc, 1 );

    if ( pRow->mpNext )
    {
        DrawUpperRowShades( pRow->mpNext, dc, 1 ); 
        DrawUpperRowShades( pRow->mpNext, dc, 0 );
    }

    event.Skip(); // pass event to the next plugin
}

void cbPaneDrawPlugin::DrawUpperRowShades( cbRowInfo* pRow, wxDC& dc, int level )
{
    for( size_t i = 0; i != pRow->mBars.Count(); ++i )
    {
        wxRect& bounds = pRow->mBars[i]->mBoundsInParent;

        if ( mpPane->IsHorizontal() )
        {        
            DrawShade( level, bounds, FL_ALIGN_TOP, dc );
            if ( level == 1 )
            {
                dc.SetPen( mpLayout->mDarkPen );
                dc.DrawPoint( bounds.x - 1, bounds.y );
                dc.SetPen( mpLayout->mLightPen );
                dc.DrawPoint( bounds.x + bounds.width , bounds.y );
            }
        }
        else
        {
            DrawShade( level, bounds, FL_ALIGN_LEFT, dc );
            if ( level == 1 )
            {
                dc.SetPen( mpLayout->mDarkPen );
                dc.DrawPoint( bounds.x, bounds.y -1 );
                dc.SetPen( mpLayout->mLightPen );
                dc.DrawPoint( bounds.x, bounds.y + bounds.height );
            }
        }
    }
}

void cbPaneDrawPlugin::DrawLowerRowShades( cbRowInfo* pRow, wxDC& dc, int level )
{
    for( size_t i = 0; i != pRow->mBars.Count(); ++i )
    {
        wxRect& bounds = pRow->mBars[i]->mBoundsInParent;

        if ( mpPane->IsHorizontal() )
        {
            DrawShade( level, bounds, FL_ALIGN_BOTTOM, dc );
            if ( level == 1 )
            {
                dc.SetPen( mpLayout->mDarkPen );
                dc.DrawPoint( bounds.x - 1, bounds.y + bounds.height -1 );
                dc.SetPen( mpLayout->mLightPen );
                dc.DrawPoint( bounds.x + bounds.width , bounds.y + bounds.height -1 );
            }
        }
        else
        {
            DrawShade( level, bounds, FL_ALIGN_RIGHT, dc );
            if ( level == 1 )
            {
                dc.SetPen( mpLayout->mDarkPen );
                dc.DrawPoint( bounds.x + bounds.width - 1, bounds.y -1 );
                dc.SetPen( mpLayout->mLightPen );
                dc.DrawPoint( bounds.x + bounds.width - 1, bounds.y + bounds.height );
            }
        }
    }
}

void cbPaneDrawPlugin::DrawBarInnerShadeRect( cbBarInfo* pBar, wxDC& dc )
{
    wxRect& bounds = pBar->mBoundsInParent;

    dc.SetPen( mpLayout->mDarkPen );
    
    dc.DrawLine( bounds.x + bounds.width - 1,
                 bounds.y,
                 bounds.x + bounds.width - 1,
                 bounds.y + bounds.height );

    dc.DrawLine( bounds.x,
                 bounds.y + bounds.height - 1,
                 bounds.x + bounds.width,
                 bounds.y + bounds.height -1  );

    dc.SetPen( mpLayout->mLightPen );

    dc.DrawLine( bounds.x,
                 bounds.y,
                 bounds.x + bounds.width - 1,
                 bounds.y );

    dc.DrawLine( bounds.x,
                 bounds.y,
                 bounds.x,
                 bounds.y + bounds.height - 1 );
}

void cbPaneDrawPlugin::DrawShade( int level, wxRect& rect, int alignment, wxDC& dc )
{
    // simulates "guled-bricks" appearence of control bars

    if ( ( alignment == FL_ALIGN_TOP    && level == 1 ) ||
         ( alignment == FL_ALIGN_BOTTOM && level == 0 ) ||
         ( alignment == FL_ALIGN_LEFT   && level == 1 ) ||
         ( alignment == FL_ALIGN_RIGHT  && level == 0 )
       )
    
        dc.SetPen( mpLayout->mDarkPen  );
    else
        dc.SetPen( mpLayout->mLightPen );

    if ( alignment == FL_ALIGN_TOP )
    {
        if ( level == 0 )
        
            dc.DrawLine( rect.x, 
                         rect.y,
                         rect.x + rect.width - 1,
                         rect.y );
        else
            dc.DrawLine( rect.x - 1,
                         rect.y - 1,
                         rect.x + rect.width + 0,
                         rect.y - 1 );
    }
    else
    if ( alignment == FL_ALIGN_BOTTOM )
    {
        if ( level == 0 )
        
            dc.DrawLine( rect.x, 
                         rect.y + rect.height - 1,
                         rect.x + rect.width,
                         rect.y + rect.height - 1 );
        else
            dc.DrawLine( rect.x - 1,
                         rect.y + rect.height,
                         rect.x + rect.width + 1,
                         rect.y + rect.height );
    }
    else
    if ( alignment == FL_ALIGN_LEFT )
    {
        if ( level == 0 )
        
            dc.DrawLine( rect.x, 
                         rect.y,
                         rect.x,
                         rect.y + rect.height - 1 );
        else
            dc.DrawLine( rect.x - 1,
                         rect.y - 1,
                         rect.x - 1,
                         rect.y + rect.height );
    }
    else
    if ( alignment == FL_ALIGN_RIGHT )
    {
        if ( level == 0 )
        
            dc.DrawLine( rect.x + rect.width - 1, 
                         rect.y,
                         rect.x + rect.width - 1,
                         rect.y + rect.height );
        else
        {                     
            dc.DrawLine( rect.x + rect.width,
                         rect.y - 1,
                         rect.x + rect.width,
                         rect.y + rect.height + 1  );
        }
    }
}

void cbPaneDrawPlugin::DrawShade1( int level, wxRect& rect, int alignment, wxDC& dc )
{
    // simulates "guled-bricks" appearence of control bars

    if ( ( alignment == FL_ALIGN_TOP    && level == 1 ) ||
         ( alignment == FL_ALIGN_BOTTOM && level == 0 ) ||
         ( alignment == FL_ALIGN_LEFT   && level == 1 ) ||
         ( alignment == FL_ALIGN_RIGHT  && level == 0 )
       )
    
        dc.SetPen( mpLayout->mDarkPen  );
    else
        dc.SetPen( mpLayout->mLightPen );

    if ( alignment == FL_ALIGN_TOP )
    {
        if ( level == 0 )
        
            dc.DrawLine( rect.x, 
                         rect.y,
                         rect.x + rect.width,
                         rect.y );
        else
            dc.DrawLine( rect.x,
                         rect.y - 1,
                         rect.x + rect.width,
                         rect.y - 1 );
    }
    else
    if ( alignment == FL_ALIGN_BOTTOM )
    {
        if ( level == 0 )
        
            dc.DrawLine( rect.x, 
                         rect.y + rect.height - 1,
                         rect.x + rect.width,
                         rect.y + rect.height - 1 );
        else
            dc.DrawLine( rect.x,
                         rect.y + rect.height,
                         rect.x + rect.width,
                         rect.y + rect.height );
    }
    else
    if ( alignment == FL_ALIGN_LEFT )
    {
        if ( level == 0 )
        
            dc.DrawLine( rect.x, 
                         rect.y,
                         rect.x,
                         rect.y + rect.height );
        else
            dc.DrawLine( rect.x - 1,
                         rect.y,
                         rect.x - 1,
                         rect.y + rect.height );
    }
    else
    if ( alignment == FL_ALIGN_RIGHT )
    {
        if ( level == 0 )
        
            dc.DrawLine( rect.x + rect.width - 1, 
                         rect.y,
                         rect.x + rect.width - 1,
                         rect.y + rect.height );
        else
        {                     
            dc.DrawLine( rect.x + rect.width,
                         rect.y ,
                         rect.x + rect.width,
                         rect.y + rect.height );
        }
    }
}

void cbPaneDrawPlugin::DrawPaneShade( wxDC& dc, int alignment )
{
    if ( !mpPane->mProps.mShow3DPaneBorderOn ) return;

    wxRect bounds = mpPane->mBoundsInParent;

    bounds.x      += mpPane->mLeftMargin;
    bounds.y      += mpPane->mTopMargin;
    bounds.width  -= ( mpPane->mLeftMargin + mpPane->mRightMargin  );
    bounds.height -= ( mpPane->mTopMargin  + mpPane->mBottomMargin );

    DrawShade( 0, bounds, alignment, dc );
    DrawShade( 1, bounds, alignment, dc );
}

void cbPaneDrawPlugin::DrawPaneShadeForRow( cbRowInfo* pRow, wxDC& dc )
{
    if ( !mpPane->mProps.mShow3DPaneBorderOn ) return;

    // do not draw decoration, if pane has "vainished"
    if ( mpPane->mPaneWidth  < 0 ||
         mpPane->mPaneHeight < 0 )

         return;

    wxRect bounds = pRow->mBoundsInParent;

    if ( mpPane->mAlignment == FL_ALIGN_TOP ||
         mpPane->mAlignment == FL_ALIGN_BOTTOM )
    {
        --bounds.y;
        bounds.height += 2;

        DrawShade1( 0, bounds, FL_ALIGN_LEFT, dc );
        DrawShade1( 1, bounds, FL_ALIGN_LEFT, dc );
        DrawShade1( 0, bounds, FL_ALIGN_RIGHT, dc );
        DrawShade1( 1, bounds, FL_ALIGN_RIGHT, dc );

        if ( !pRow->mpNext )
            DrawPaneShade( dc, FL_ALIGN_BOTTOM );

        if ( !pRow->mpPrev )
            DrawPaneShade( dc, FL_ALIGN_TOP );
    }
    else
    {
        --bounds.x;
        bounds.width += 2;

        DrawShade1( 0, bounds, FL_ALIGN_TOP, dc );
        DrawShade1( 1, bounds, FL_ALIGN_TOP, dc );
        DrawShade1( 0, bounds, FL_ALIGN_BOTTOM, dc );
        DrawShade1( 1, bounds, FL_ALIGN_BOTTOM, dc );

        if ( !pRow->mpNext )
            DrawPaneShade( dc, FL_ALIGN_RIGHT );

        if ( !pRow->mpPrev )
            DrawPaneShade( dc, FL_ALIGN_LEFT );
    }
}

void cbPaneDrawPlugin::OnDrawPaneDecorations( cbDrawPaneDecorEvent& event )
{
    wxDC& dc = *event.mpDc;

    cbDockPane* pPane = event.mpPane;

    RowArrayT& lst = pPane->GetRowList();
    
    // FIXME:: this is a workaround for some glitches

    if ( lst.Count() )
    {
        cbRowInfo* pLastRow = lst[ lst.Count() - 1 ];

        pPane->PaintRowBackground( pLastRow,  dc );
        pPane->PaintRowDecorations( pLastRow, dc );
        pPane->PaintRowHandles( pLastRow, dc );
    }

    if ( !pPane->mProps.mShow3DPaneBorderOn ) return;

    // do not draw decoration, if pane is completely hidden
    if ( event.mpPane->mPaneWidth  < 0 ||
         event.mpPane->mPaneHeight < 0 )

         return;

    DrawPaneShade( dc, FL_ALIGN_TOP    );
    DrawPaneShade( dc, FL_ALIGN_BOTTOM );
    DrawPaneShade( dc, FL_ALIGN_LEFT   );
    DrawPaneShade( dc, FL_ALIGN_RIGHT  );

    event.Skip(); // pass event to the next plugin
}

// bar decoration/sizing handlers

void cbPaneDrawPlugin::OnDrawBarDecorations( cbDrawBarDecorEvent& event )
{
//    cbBarInfo* pBar = event.mpBar;
    wxDC&   dc      = *event.mpDc;

    // draw brick borders

    wxRect& rect = event.mBoundsInParent;

    dc.SetPen( mpLayout->mLightPen );

    // horiz
    dc.DrawLine( rect.x, rect.y, 
                 rect.x + rect.width-1, rect.y );

    // vert
    dc.DrawLine( rect.x, rect.y,
                 rect.x, rect.y + rect.height-1 );


    dc.SetPen( mpLayout->mDarkPen );

    // vert
    dc.DrawLine( rect.x + rect.width-1, rect.y,
                 rect.x + rect.width-1, rect.y + rect.height-1 );

    // horiz
    dc.DrawLine( rect.x, rect.y + rect.height-1,
                 rect.x + rect.width, rect.y + rect.height-1 );

    event.Skip(); // pass event to the next plugin
}

void cbPaneDrawPlugin::OnDrawBarHandles( cbDrawBarHandlesEvent& event )
{
    // short-cuts
    cbBarInfo* pBar = event.mpBar;
    wxDC&   dc      = *event.mpDc;
    mpPane          = event.mpPane;

    // draw handles around the bar if present

    if ( pBar->mHasLeftHandle ||
         pBar->mHasRightHandle )
    {
        wxRect& bounds = pBar->mBoundsInParent;

        if ( mpPane->IsHorizontal() )
        {
            if ( pBar->mHasLeftHandle )

                mpPane->DrawVertHandle( dc, bounds.x - mpPane->mProps.mResizeHandleSize -1, 
                                        bounds.y, bounds.height );

            if ( pBar->mHasRightHandle )

                mpPane->DrawVertHandle( dc, 
                                        bounds.x + bounds.width -1, 
                                        bounds.y, bounds.height );
        }
        else
        {
            if ( pBar->mHasLeftHandle )

                mpPane->DrawHorizHandle( dc, bounds.x, 
                                         bounds.y  - mpPane->mProps.mResizeHandleSize - 1, 
                                         bounds.width );

            if ( pBar->mHasRightHandle )

                mpPane->DrawHorizHandle( dc, bounds.x, 
                                         bounds.y + bounds.height - 1,
                                         bounds.width );
        }
    }

    event.Skip(); // pass event to the next plugin
}

void cbPaneDrawPlugin::OnStartDrawInArea( cbStartDrawInAreaEvent& event )
{
    // DBG::
    wxASSERT( mpClntDc == NULL );

    // FOR NOW:: create/destroy client-dc upon each drawing
    mpClntDc = new wxClientDC( &mpLayout->GetParentFrame() );

    (*event.mppDc) = mpClntDc;

    mpClntDc->SetClippingRegion( event.mArea.x,     event.mArea.y,
                                 event.mArea.width, event.mArea.height );
}

void cbPaneDrawPlugin::OnFinishDrawInArea( cbFinishDrawInAreaEvent& WXUNUSED(event) )
{
    // DBG::
    wxASSERT( mpClntDc );

    delete mpClntDc;

    mpClntDc = NULL;
}

