/////////////////////////////////////////////////////////////////////////////
// Name:        rowlayoutpl.cpp
// Purpose:     cbRowLayoutPlugin implementation.
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     09/09/98
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

#include "wx/fl/rowlayoutpl.h"

// exerimental "features" are still buggy
#undef __EXPERIMENTAL

/***** Implementation for class cbRowLayoutPlugin *****/

IMPLEMENT_DYNAMIC_CLASS( cbRowLayoutPlugin, cbPluginBase )

BEGIN_EVENT_TABLE( cbRowLayoutPlugin, cbPluginBase )

    EVT_PL_LAYOUT_ROW ( cbRowLayoutPlugin::OnLayoutRow  )
    EVT_PL_LAYOUT_ROWS( cbRowLayoutPlugin::OnLayoutRows )
    EVT_PL_RESIZE_ROW ( cbRowLayoutPlugin::OnResizeRow  )

    EVT_PL_INSERT_BAR ( cbRowLayoutPlugin::OnInsertBar  )
    EVT_PL_REMOVE_BAR ( cbRowLayoutPlugin::OnRemoveBar  )

END_EVENT_TABLE()

cbRowLayoutPlugin::cbRowLayoutPlugin(void)
    : mpPane( 0 )
{}

cbRowLayoutPlugin::cbRowLayoutPlugin( wxFrameLayout* pPanel, int paneMask )

    : cbPluginBase( pPanel, paneMask ),
      mpPane( 0 )
{}

void cbRowLayoutPlugin::CheckIfAtTheBoundary( cbBarInfo* pTheBar, cbRowInfo& rowInfo )
{
    // this method handles situation, when fixed bar is inserted
    // into the row, where among fixed bars not-fixed ones are present.
    // In this case we need to check if the pBarNode appears to be inserted
    // chain of fixed-bars on the very right or left side of the row,
    // then all the white-space, such chain should be eliminated,
    // and the resulting chain justified to the right or the left
    // side of the row

    if ( !pTheBar->IsFixed() || rowInfo.mHasOnlyFixedBars )

        return;

    cbBarInfo* pBar = rowInfo.mBars[ rowInfo.mBars.Count() - 1 ];

    // slide fixed bars to the right on the right side relative to the pBarNode

    int prevX = mpPane->mPaneWidth;

    do
    {
        if ( !pBar->IsFixed() )
            break;

        wxRect& bounds = pBar->mBounds;

        bounds.x = prevX - bounds.width;

        prevX = bounds.x;

        if ( pBar == pTheBar ) break;

        pBar = pBar->mpPrev;
    }
    while( 1 );

    // slide fixed bars to the left on the left side relative to the pBarNode

    pBar = rowInfo.mBars[0];

    prevX = 0;

    do
    {
        if ( pBar->IsFixed() )

            break;

        wxRect& bounds = pBar->mBounds;

        bounds.x = prevX;

        prevX = bounds.x + bounds.width;

        if ( pBar == pTheBar ) break;

        pBar = pBar->mpNext;
    }
    while( 1 );
}

void cbRowLayoutPlugin::ExpandNotFixedBars( cbRowInfo* pRow )
{
    ApplyLengthRatios( pRow );

    #if 1

   // FIXME:: something's wrong?
   return;

    #else

    double freeSpc = (double)GetRowFreeSpace( pRow );

    // calculate sum of precents

    double pcntSum = 0.0;

    size_t i;
    for ( i = 0; i != pRow->mBars.Count(); ++i )
    {
        if ( !pRow->mBars[i]->IsFixed() )
            pcntSum += pRow->mBars[i]->mLenRatio;
    }

    // setup bar lengths

    int curX = 0;

    for ( i = 0; i != pRow->mBars.Count(); ++i )
    {
        cbBarInfo& bar = *pRow->mBars[i];

        if ( !bar.IsFixed() )
        {
            bar.mLenRatio = bar.mLenRatio/(pcntSum);

            bar.mBounds.width =

                wxMax( mpPane->mProps.mMinCBarDim.x, int( freeSpc*bar.mLenRatio ) );
        }

        bar.mBounds.x = curX;
        curX = bar.mBounds.x + bar.mBounds.width;
    }
    #endif
}

void cbRowLayoutPlugin::AdjustLengthOfInserted( cbRowInfo* WXUNUSED(pRow), cbBarInfo* WXUNUSED(pTheBar) )
{
    return;

#if 0

    // TBD: Makes following code unreachable

    // pTheBar is not-fixed

    // FIXME:: what is this for??

#if 1

    int totalLen = 0;

    size_t i;
    for ( i = 0; i != pRow->mBars.Count(); ++i )
    {
        if ( !pRow->mBars[i]->IsFixed() )
            totalLen += pRow->mBars[i]->mBounds.width;
    }

    double curWidth = pTheBar->mBounds.width;

    if ( pRow->mBars.Count() )

        pTheBar->mBounds.width = int( mpPane->mPaneWidth * (curWidth / double(totalLen)) );
#else

    double freeSpc = (double)GetRowFreeSpace( pRow );

    double pcntSum = 0.0;

   size_t i;
    for ( i = 0; i != pRow->mBars.Count(); ++i )
   {
        if ( !pRow->mBars[i]->IsFixed() )
            pcntSum += pRow->mBars[i]->mLenRatio;
   }

    // if no longer "balanced", assume that `pTheBar' was previously
    // removed from this row (kind of AI...)

    if ( pcntSum < 0.98 )

        pTheBar->mBounds.width = freeSpc * (1.0 - pcntSum);
#endif

#endif

}

void cbRowLayoutPlugin::FitBarsToRange( int from, int till,
                                        cbBarInfo* pTheBar, cbRowInfo* pRow )
{
    cbBarInfo* pFromBar;
    cbBarInfo* pTillBar;

    if ( pTheBar->mBounds.x > from )
    {
        // it's range from the left
        pFromBar = pRow->mBars[0];
        pTillBar = pTheBar;
    }
    else
    {
        pFromBar = pTheBar->mpNext;
        pTillBar = NULL;
    }

    // calc free space in the range

    cbBarInfo* pBar = pFromBar;
    int     freeSpc = till-from;
    double  pcntSum = 0;

    while( pBar != pTillBar )
    {
        if ( pBar->IsFixed() )
            freeSpc -= pBar->mBounds.width;
        else
            pcntSum += pBar->mLenRatio;

        pBar = pBar->mpNext;
    }

    // adjust not-fixed bar sizes in the range

    pBar = pFromBar;

    while ( pBar != pTillBar )
    {
        if ( !pBar->IsFixed() )
      {
            pBar->mBounds.width =
                wxMax( mpPane->mProps.mMinCBarDim.x,
                       (int)( ((double)freeSpc) * (pBar->mLenRatio/pcntSum) )
                     );
      }
        pBar = pBar->mpNext;
    }

    // layout range, starting from the left-most bar

    pBar      = pFromBar;
    int prevX = from;
    bool hasNotFixedBars = false;

    while ( pBar != pTillBar )
    {
        wxRect& bounds = pBar->mBounds;

        if ( !pBar->IsFixed() )
        {
            hasNotFixedBars = true;

            freeSpc -= bounds.width;
        }

        bounds.x = prevX;

        prevX = bounds.x + bounds.width;

        pBar = pBar->mpNext;
    }

    // make width adjustment for the right-most bar in the range, due to
    // lost precision when seting widths using f.p. length-ratios

    if ( hasNotFixedBars )
    {
        if ( pTheBar->mBounds.x > from )
        {
            if ( pTillBar->mpPrev )
            {
                wxRect& tillBar = pTillBar->mpPrev->mBounds;

                //tillBar.width = bar.mBounds.x - tillBar.x;
                tillBar.width += freeSpc;
            }
        }
        else
        {
            cbBarInfo* pLast = pRow->mBars[ pRow->mBars.Count() - 1 ];

            if ( pLast != pTheBar )
            {
                pTheBar->mBounds.width += freeSpc;

                SlideRightSideBars( pTheBar );
            }
        }
    }
}

void cbRowLayoutPlugin::MinimzeNotFixedBars( cbRowInfo* pRow, cbBarInfo* pBarToPreserve )
{
    size_t i;
    for ( i = 0; i != pRow->mBars.Count(); ++i )
    {
        if ( !pRow->mBars[i]->IsFixed() && pRow->mBars[i] != pBarToPreserve )
            pRow->mBars[i]->mBounds.width = mpPane->mProps.mMinCBarDim.x;
    }
}

int cbRowLayoutPlugin::GetRowFreeSpace( cbRowInfo* pRow )
{
    int freeSpc = mpPane->mPaneWidth;

    size_t i;
    for ( i = 0; i != pRow->mBars.Count(); ++i )
    {
        // not-fixed bars variable length, thus their
        // dimensions are ignored
        if ( pRow->mBars[i]->IsFixed() )
            freeSpc -= pRow->mBars[i]->mBounds.width;
    }

    return freeSpc;
}

void cbRowLayoutPlugin::RecalcLengthRatios( cbRowInfo* pRow )
{
    double freeSpc = double( GetRowFreeSpace( pRow ) );

    cbBarInfo* pBar          = pRow->mBars[0];
    cbBarInfo* pLastNotFixed = NULL;

    double pcntLeft = 1.0; // (100%)

#ifdef __EXPERIMENTAL

    int totalLen = 0;

    size_t i;
    for ( i = 0; i != pRow->mBars.Count(); ++i )
    {
        if ( !pRow->mBars[i]->IsFixed() )
            totalLen += pRow->mBars[i]->mBounds.width;
    }
#endif

    size_t i;
    for ( i = 0; i != pRow->mBars.Count(); ++i )
    {
        cbBarInfo& bar = *pRow->mBars[i];

        if ( !bar.IsFixed() )
        {

#ifdef __EXPERIMENTAL

            bar.mLenRatio = double(bar.mBounds.width)/double(totalLen);
#else
            bar.mLenRatio = double(bar.mBounds.width)/freeSpc;
#endif

            pcntLeft      -= bar.mLenRatio;
            pLastNotFixed  = pBar;
        }
    }

    // attach remainder (the result of lost precision) to the
    // last not-fixed bar

#if !defined(__EXPERIMENTAL)

    if ( pLastNotFixed )

        pLastNotFixed->mLenRatio += pcntLeft;
#endif

}

void cbRowLayoutPlugin::ApplyLengthRatios( cbRowInfo* pRow )
{
    size_t i;
    double pcntSum = 0;

    // FOR NOW:: all-in-one

    for ( i = 0; i != pRow->mBars.Count(); ++i )
    {
        if ( !pRow->mBars[i]->IsFixed() )
            pcntSum += pRow->mBars[i]->mLenRatio;
    }

    /*
    pBar = node_to_first_bar_node( pRow );

    while( pBar )
    {
        cbBarInfo& bar = node_to_bar( pBar );

        if ( !bar.IsFixed() )

            bar.mLenRatio = pcntSum / bar.mLenRatio;

        pBar = pBar->Next();
    }
    */

    int    prevX   = 0;
    double freeSpc = GetRowFreeSpace( pRow );

    // tricky stuff (improtant!):
    // when not-fixed bar is removed from the row and there are
    // still some other not-fixed ones left in that row, then
    // the sum of mLenRatio's is no longer 1.0 - this is left
    // intintionally to handle the case when the removed bar
    // is returned right back to the row - so that it would retain
    // it's original dimensions in this row (this is kind of AI...)
    //
    // The problem is - when it's remvoed, the sum of
    // mLenRatio's is not in "balance", i.e. is < 1.0,
    // it's possible to restore balance, but instead of that
    // we artifically ajdust freeSpc value in a way that it would
    // look like total of mLetRatio's is 1.0, thus original
    // len. ratios are _preserved_:

    if (pcntSum == 0.0)
        pcntSum = 1.0;

    double unit = freeSpc / pcntSum;

    bool haveSquished = false;

    for ( i = 0; i != pRow->mBars.Count(); ++i )
    {
        if ( !pRow->mBars[i]->IsFixed() )
        {
            cbBarInfo& bar = *pRow->mBars[i];

            if ( int( unit * bar.mLenRatio ) < mpPane->mProps.mMinCBarDim.x )
            {
                haveSquished = true;

                bar.mBounds.width = -1; // mark as "squished"

                pcntSum -= bar.mLenRatio;

                freeSpc -= mpPane->mProps.mMinCBarDim.x;
            }
        }
    }  // for

    if ( haveSquished )
        unit = freeSpc / pcntSum;

    for ( i = 0; i != pRow->mBars.Count(); ++i )
    {
        cbBarInfo& bar = *pRow->mBars[i];

        bar.mBounds.x = prevX;

        if ( !bar.IsFixed() )
        {
            if ( bar.mBounds.width == -1 )

                bar.mBounds.width = mpPane->mProps.mMinCBarDim.x;
            else
                bar.mBounds.width = int( unit * bar.mLenRatio );

            // a little bit of AI:
            // memorize bar's height and width, when docked in
            // the current orientation - by making the current
            // dimensions to be "preffered" ones for this docking state

            if ( !bar.IsFixed() )
            {
                bar.mDimInfo.mSizes[ bar.mState ].x = bar.mBounds.width;
                bar.mDimInfo.mSizes[ bar.mState ].y = bar.mBounds.height;
            }
        }

        prevX = bar.mBounds.x + bar.mBounds.width;
    }
}

void cbRowLayoutPlugin::DetectBarHandles( cbRowInfo* pRow )
{
    // first pass from left to right (detect left-side handles)

    bool foundNotFixed = false;

    size_t i;
    for ( i = 0; i != pRow->mBars.Count(); ++i )
    {
        cbBarInfo& bar = *pRow->mBars[i];

        bar.mHasLeftHandle = false;

        if ( !bar.IsFixed() )
        {
            if ( foundNotFixed )

                if ( bar.mpPrev &&
                     bar.mpPrev->IsFixed() )

                    bar.mHasLeftHandle = true;

            foundNotFixed = true;
        }
    }

    // pass from right to left (detect right-side handles)

    foundNotFixed = false;

    cbBarInfo* pBar = pRow->mBars[ pRow->mBars.Count() - 1 ];

    while( pBar )
    {
        pBar->mHasRightHandle = false;

        if ( !pBar->IsFixed() )
        {
            if ( foundNotFixed )

                if ( pBar->mpNext )

                     pBar->mHasRightHandle = true;

            foundNotFixed = true;
        }

        pBar = pBar->mpPrev;
    }
}

void cbRowLayoutPlugin::RelayoutNotFixedBarsAround( cbBarInfo* pTheBar, cbRowInfo* pRow )
{
    if ( !pTheBar->mpPrev )
    {
        if (  !pTheBar->IsFixed() )
        {
            // this bar the first in the row, move it's
            // left edge to the very left
            pTheBar->mBounds.width += pTheBar->mBounds.x;
            pTheBar->mBounds.x      = 0;
        }
    }
    else
        FitBarsToRange( 0, pTheBar->mBounds.x, pTheBar, pRow );

    if ( !pTheBar->mpNext )
    {
        if ( !pTheBar->IsFixed() )
        {
            // this bar is the last one, move it's
            // right edge to the very right

            pTheBar->mBounds.width = mpPane->mPaneWidth - pTheBar->mBounds.x;
        }
    }
    else
        FitBarsToRange( pTheBar->mBounds.x + pTheBar->mBounds.width, mpPane->mPaneWidth,
                        pTheBar, pRow
                      );
}

void cbRowLayoutPlugin::LayoutItemsVertically( cbRowInfo& row )
{
    size_t i;
    for ( i = 0; i != row.mBars.Count(); ++i )
    {
        cbBarInfo& bar = *row.mBars[i];

        bar.mBounds.y = row.mRowY;

        if ( !bar.IsFixed() )

            // make all not-fixed bars of equal height
            bar.mBounds.height = row.mRowHeight;

        if ( row.mHasUpperHandle )

            bar.mBounds.y += mpPane->mProps.mResizeHandleSize;
    }
}

int cbRowLayoutPlugin::CalcRowHeight( cbRowInfo& row )
{
    int maxHeight = 0;

    size_t i;
    for ( i = 0; i != row.mBars.Count(); ++i )

        maxHeight = wxMax( maxHeight, row.mBars[i]->mBounds.height );

    return maxHeight;
}

void cbRowLayoutPlugin::StickRightSideBars( cbBarInfo* pToBar )
{
    cbBarInfo* pBar  = pToBar->mpNext;
    cbBarInfo* pPrev = pToBar;

    while( pBar )
    {
        wxRect& cur  = pBar->mBounds;
        wxRect& prev = pPrev->mBounds;

        cur.x = prev.x + prev.width;

        pPrev = pBar;
        pBar  = pBar->mpNext;
    }
}

void cbRowLayoutPlugin::SlideLeftSideBars( cbBarInfo* pTheBar )
{
    // shift left-side-bars to the left (with respect to "theBar"),
    // so that they would not obscured by each other

    cbBarInfo* pBar  = pTheBar->mpPrev;
    cbBarInfo* pPrev = pTheBar;

    while( pBar )
    {
        wxRect& cur  = pBar->mBounds;
        wxRect& prev = pPrev->mBounds;

        if ( cur.x + cur.width > prev.x )

            cur.x = prev.x - cur.width;

        pPrev = pBar;
        pBar  = pBar->mpPrev;
    }
}

void cbRowLayoutPlugin::SlideRightSideBars( cbBarInfo* pTheBar )
{
    // shift right-side-bars to the right (with respect to "theBar"),
    // so that they would not be obscured by each other

    cbBarInfo* pBar  = pTheBar->mpNext;
    cbBarInfo* pPrev = pTheBar;

    while( pBar )
    {
        wxRect& cur  = pBar->mBounds;
        wxRect& prev = pPrev->mBounds;

        if ( cur.x < prev.x + prev.width )

            cur.x = prev.x + prev.width;

        pPrev = pBar;
        pBar  = pBar->mpNext;
    }
}

void cbRowLayoutPlugin::ShiftLeftTrashold( cbBarInfo* WXUNUSED(pTheBar), cbRowInfo& row )
{
    wxRect& first = row.mBars[0]->mBounds;

    if ( first.x < 0 )
    {
        row.mBars[0]->mBounds.x = 0;

        SlideRightSideBars( row.mBars[0] );
    }
}

void cbRowLayoutPlugin::ShiftRightTrashold( cbBarInfo* pTheBar, cbRowInfo& row )
{
    wxRect& theBar = pTheBar->mBounds;

    do
    {
        cbBarInfo* pBar = pTheBar;

        // calculate free spece on the left side

        int leftFreeSpc = 0;

        while( pBar )
        {
            wxRect& cur = pBar->mBounds;

            if ( pBar->mpPrev )
            {
                wxRect& prev = pBar->mpPrev->mBounds;

                leftFreeSpc += cur.x - prev.x - prev.width;
            }
            else
                leftFreeSpc += cur.x;

            if ( cur.x < 0 )
            {
                leftFreeSpc = 0;
                break;
            }

            pBar = pBar->mpPrev;
        }

        pBar = pTheBar;

        int rightOverflow = 0;

        if ( pTheBar->IsFixed() )

            while( pBar )
            {
                if ( !pBar->mpNext )
                {
                    wxRect& cur = pBar->mBounds;

                    if ( cur.x + cur.width > mpPane->mPaneWidth )

                        rightOverflow = cur.x + cur.width - mpPane->mPaneWidth;
                }

                pBar = pBar->mpNext;
            }

        if ( rightOverflow > 0 )
        {
            if ( leftFreeSpc <= 0 ) return;

            if ( pTheBar->mpNext )
            {
                wxRect& next = pTheBar->mpNext->mBounds;

                // if there's enough space on the left, move over one half-obscured
                // bar from the right to the left side with respect to "theBar"

                if ( next.width < leftFreeSpc )
                {
                    cbBarInfo* pNext = pTheBar->mpNext;

                    row.mBars.Remove( pNext );

                    row.mBars.Insert( pNext, row.mBars.Index( pTheBar ) );

                    next.x = theBar.x - next.width;

                    // re-setup mpPrev/mpNext references after insertion

                    mpPane->InitLinksForRow( &row );

                    // tighten things

                    StickRightSideBars( pTheBar );
                    SlideLeftSideBars ( pTheBar );

                    continue;
                }
            }

            int leftShift = ( rightOverflow > leftFreeSpc )
                            ? leftFreeSpc
                            : rightOverflow;

            theBar.x -= leftShift;

            StickRightSideBars( pTheBar );
            SlideLeftSideBars ( pTheBar );

            break;

        } // end of if ( rightOverflow )
        else
            break;

    } while(1);
}

void cbRowLayoutPlugin::InsertBefore( cbBarInfo* pBeforeBar,
                                      cbBarInfo* pTheBar,
                                      cbRowInfo& row        )
{
    if ( pBeforeBar )

        row.mBars.Insert( pTheBar, row.mBars.Index( pBeforeBar ) );
    else
        row.mBars.Add( pTheBar );

    pTheBar->mpRow = &row;
}

void cbRowLayoutPlugin::DoInsertBar( cbBarInfo* pTheBar, cbRowInfo& row )
{
    wxRect& theBar = pTheBar->mBounds;

    /* OLD STUFF::
    if ( theBar.x < 0 && !node_to_bar( pTheBar ).IsFixed() )
    {
        // AI::
        theBar.width += theBar.x;
        theBar.x = 0;
    } */

    size_t i;
    for ( i = 0; i != row.mBars.Count(); ++i )
    {
        cbBarInfo& bar = *row.mBars[i];

        wxRect& cur = bar.mBounds;

        // if bar hits the left edge
        if ( theBar.x <= cur.x )
        {
            InsertBefore( &bar, pTheBar, row );
            return;
        }

        else
        // if bar hits the right edge
        if ( theBar.x <= cur.x + cur.width )
        {
            if ( theBar.x + theBar.width > cur.x + cur.width )
            {
                InsertBefore( bar.mpNext, pTheBar, row );
                return;
            }

            // otherwise the bar lies within the bounds of current bar

            int leftDist  = theBar.x - cur.x;
            int rightDist = cur.x + cur.width - (theBar.x + theBar.width);

            if ( leftDist < rightDist )

                InsertBefore( &bar, pTheBar, row );
            else
                InsertBefore( bar.mpNext, pTheBar, row );

            return;
        }
    }

    InsertBefore( NULL, pTheBar, row ); // insert at the end
}

// evnet handlers

void cbRowLayoutPlugin::OnInsertBar( cbInsertBarEvent& event )
{
    cbBarInfo* pBarToInsert = event.mpBar;
    cbRowInfo* pIntoRow     = event.mpRow;
    mpPane                  = event.mpPane;

    if ( !pBarToInsert->IsFixed() )

        AdjustLengthOfInserted( pIntoRow, pBarToInsert );

    DoInsertBar( pBarToInsert, *pIntoRow );

    mpPane->InitLinksForRow( pIntoRow ); // relink "mpNext/mpPrev"s

    // perform relayouting of the bars after insertion

    // init bar location info
    pBarToInsert->mAlignment = event.mpPane->mAlignment;
    pBarToInsert->mRowNo     = event.mpPane->GetRowIndex( pIntoRow );

#ifdef __EXPERIMENTAL

    if ( !pIntoRow->mHasOnlyFixedBars || !pBarToInsert->IsFixed() )

        RecalcLengthRatios( pIntoRow );

#endif

    MinimzeNotFixedBars( pIntoRow, pBarToInsert );

    SlideLeftSideBars ( pBarToInsert );
    SlideRightSideBars( pBarToInsert );

    ShiftLeftTrashold ( pBarToInsert, *pIntoRow );
    ShiftRightTrashold( pBarToInsert, *pIntoRow );

    mpPane->SyncRowFlags( pIntoRow );

    CheckIfAtTheBoundary( pBarToInsert, *pIntoRow );

    if ( event.mpPane->IsHorizontal() )

        pBarToInsert->mState = wxCBAR_DOCKED_HORIZONTALLY;
    else
        pBarToInsert->mState = wxCBAR_DOCKED_VERTICALLY;

    if ( !pIntoRow->mHasOnlyFixedBars )
    {

#ifdef __EXPERIMENTAL

        ExpandNotFixedBars( pIntoRow );
#else

        RelayoutNotFixedBarsAround( pBarToInsert, pIntoRow );
        RecalcLengthRatios( pIntoRow );

#endif

        DetectBarHandles( pIntoRow );

        // do proportional resizing of not-fixed bars
        ApplyLengthRatios( pIntoRow );
    }

    // adjust the bar's docking state

    // a little bit of AI:
    // memorize bar's height and width, when docked in
    // the current orientation - by making the current
    // dimensions to be "preferred" ones for this docking state

    if ( !pBarToInsert->IsFixed() )
    {
        cbBarInfo& bar = *pBarToInsert;

        bar.mDimInfo.mSizes[ bar.mState ].x = bar.mBounds.width;
        bar.mDimInfo.mSizes[ bar.mState ].y = bar.mBounds.height;
    }
}

void cbRowLayoutPlugin::OnRemoveBar ( cbRemoveBarEvent& event )
{
    cbBarInfo* pBar = event.mpBar;
    mpPane          = event.mpPane;

    cbRowInfo* pRow = pBar->mpRow;

    mpLayout->GetUpdatesManager().OnBarWillChange( pBar, pRow, event.mpPane );

    // invalidate the whole row
    //pFirst->mpRowInfo->mMgrData.mPrevBounds.x = -1;

    pRow->mBars.Remove( pBar );

    // rest bar information after removing it from the row
    pBar->mpRow           = NULL;
    pBar->mHasLeftHandle  = false;
    pBar->mHasRightHandle = false;

    mpPane->InitLinksForRow( pRow ); // relink "mpNext/mpPrev"s

    if ( pRow->mBars.Count() == 0 )
    {
        // empty rows should not exist

        event.mpPane->GetRowList().Remove( pRow );

        delete pRow;

        mpPane->InitLinksForRows();
    }
    else
    {
        // force repainting of bars, in the row, from which the bar was removed

        // FIXME:: really needed?
        pRow->mBars[0]->mUMgrData.SetDirty(true);

        // re-setup mHasOnlyFixedBars flag for the row information
        event.mpPane->SyncRowFlags( pRow );

        DetectBarHandles( pRow );

        if ( !pRow->mHasOnlyFixedBars )

            ExpandNotFixedBars( pRow );
    }
}

void cbRowLayoutPlugin::OnLayoutRow( cbLayoutRowEvent& event )
{
    cbRowInfo* pRow = event.mpRow;
    mpPane          = event.mpPane;

    MinimzeNotFixedBars( pRow, NULL );

    if ( !pRow->mHasOnlyFixedBars )
    {
        // do proportional resizing of not-fixed bars
        ApplyLengthRatios( pRow );
    }

    cbBarInfo& lastBar  = *pRow->mBars[ pRow->mBars.Count() - 1 ];
    cbBarInfo& firstBar = *pRow->mBars[ 0 ];

    // FIXME:: Next line not used
    // wxRect& bounds = lastBar.mBounds;

    if ( lastBar.mBounds.x + lastBar.mBounds.width > mpPane->mPaneWidth )
    {
        lastBar.mBounds.x = mpPane->mPaneWidth - lastBar.mBounds.width;

        // first simulate left-row-edge friction

        SlideLeftSideBars( &lastBar );

        if ( firstBar.mBounds.x < 0 )
            firstBar.mBounds.x = 0;

        // then left-row-edge function, though this
        // may cause some of the right-side bars going
        // out of row bounds, but left-side always
        // has the highest "priority"

        SlideRightSideBars( &firstBar );
    }

    event.Skip(); // pass event to the next handler
}

void cbRowLayoutPlugin::OnLayoutRows( cbLayoutRowsEvent& event )
{
    mpPane       = event.mpPane;

    int curY = 0;

    // FIXME:: Next line not used.
    // RowArrayT& arr = mpPane->GetRowList();

    size_t i;
    for ( i = 0; i != mpPane->GetRowList().Count(); ++i )
    {
        cbRowInfo& row = *mpPane->GetRowList()[ i ];
        //mpPane->CalcLengthRatios(& row);

        // setup "has-handle" flags for rows, which depend on the existence
        // of not-fixed bars in the row

        if ( !row.mHasOnlyFixedBars )
        {
            if ( mpPane->mAlignment == FL_ALIGN_TOP ||
                 mpPane->mAlignment == FL_ALIGN_LEFT   )
            {
                row.mHasLowerHandle = true;

                row.mHasUpperHandle = false;
            }
            else
            {
                row.mHasUpperHandle = true;

                row.mHasLowerHandle = false;
            }
        }
        else
        {
            // otherwise, rows with fixed-bars only, have no height-resizing handles
            row.mHasUpperHandle = false;
            row.mHasLowerHandle = false;
        }

        // setup vertical positions for items in the row

        row.mRowY = curY;

        row.mRowWidth  = mpPane->mPaneWidth;
        row.mRowHeight = CalcRowHeight( row );

        LayoutItemsVertically( row );

        if ( row.mHasUpperHandle )
            row.mRowHeight += mpPane->mProps.mResizeHandleSize;
        if ( row.mHasLowerHandle )
            row.mRowHeight += mpPane->mProps.mResizeHandleSize;

        curY += row.mRowHeight;
    }

    event.Skip(); // pass event to the next handler - other hookeds plugin
                  // may also add some "refinements" to the layout now
}

void cbRowLayoutPlugin::OnResizeRow( cbResizeRowEvent& event )
{
    // extract resize-event info
    int     ofs            = event.mHandleOfs;
    bool    forUpperHandle = event.mForUpperHandle;
    cbRowInfo* pTheRow     = event.mpRow;
            mpPane         = event.mpPane;

    // FIXME:: Next line not used.
    //int     newHeight      = pTheRow->mRowHeight;

    if ( forUpperHandle )
    {
        // calculate available free space from above,
        // which can be obtained by squeezing not-fixed height rows

        cbRowInfo* pRow = pTheRow->mpPrev;

        while( pRow )
        {
            pRow = pRow->mpPrev;
        }
    }
    else
    {
        // calculate available free space from below,
        // which can be obtained by squeezing not-fixed height rows

        cbRowInfo* pRow = pTheRow->mpNext;

        while( pRow )
        {
            pRow = pRow->mpNext;
        }
    }

    mpLayout->GetUpdatesManager().OnStartChanges();

    int clientSize;

    // allow user adjusting pane vs. client-area space, for upper-handle

    if ( mpPane->IsHorizontal() )

        clientSize = mpLayout->GetClientHeight();
    else
        clientSize = mpLayout->GetClientWidth();

    if ( forUpperHandle && ofs < -clientSize )
    {
        int needed = -(ofs + clientSize);

        cbRowInfo* pRow = mpPane->GetRowList()[ 0 ];

        // start squeezing rows from the top row towards bottom

        while( pRow != pTheRow && needed )
        {
            // only not-fixed rows can be squeezed

            if ( !pRow->mHasOnlyFixedBars )
            {
                int prevHeight = pRow->mRowHeight;

                int newHeight  = wxMax( event.mpPane->GetMinimalRowHeight( pRow ),
                                        prevHeight - needed );

                if ( newHeight != prevHeight )
                {
                    event.mpPane->SetRowHeight( pRow, newHeight );

                    needed -= prevHeight - pRow->mRowHeight;
                }
            }

            pRow = pRow->mpNext;
        }
    }

    // allow user adjusting pane vs. client-area space, for lower-handle

    if ( !forUpperHandle && ofs > clientSize )
    {
        int needed = ofs - clientSize;

        cbRowInfo* pRow = mpPane->GetRowList()[ mpPane->GetRowList().Count() - 1 ];

        // start squeezing rows from the bottom towards the top row

        while( pRow && needed )
        {
            // only not-fixed rows can be squeezed

            if ( !pRow->mHasOnlyFixedBars )
            {
                int prevHeight = pRow->mRowHeight;

                int newHeight  = wxMax( event.mpPane->GetMinimalRowHeight( pRow ),
                                        prevHeight - needed );

                if ( newHeight != prevHeight )
                {
                    event.mpPane->SetRowHeight( pRow, newHeight );

                    needed -= prevHeight - pRow->mRowHeight;
                }
            }

            pRow = pRow->mpPrev;
        }
    }

    if ( forUpperHandle )

        event.mpPane->SetRowHeight( pTheRow, pTheRow->mRowHeight + (-ofs) );
    else
        event.mpPane->SetRowHeight( pTheRow, pTheRow->mRowHeight +   ofs  );

    mpLayout->RecalcLayout(false);

    mpLayout->GetUpdatesManager().OnFinishChanges();
    mpLayout->GetUpdatesManager().UpdateNow();
}

