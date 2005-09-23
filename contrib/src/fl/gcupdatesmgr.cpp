/////////////////////////////////////////////////////////////////////////////
// Name:        gcupdatesmgr.cpp
// Purpose:     cbGCUpdatesMgr class, optimizing refresh using GarbageCollector
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     19/10/98
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

#include "wx/fl/gcupdatesmgr.h"

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

// helper structure

struct cbRectInfo
{
    cbBarInfo*  mpBar;
    cbDockPane* mpPane;
    wxRect*     mpCurBounds;
    wxRect*     mpPrevBounds;
};

static inline cbRectInfo& node_to_rect_info( wxNode* pNode )
{
    return *( (cbRectInfo*) (pNode->GetData()) );
}

/***** Implementation for class cbSimpleUpdatesMgr *****/

IMPLEMENT_DYNAMIC_CLASS( cbGCUpdatesMgr, cbSimpleUpdatesMgr )

cbGCUpdatesMgr::cbGCUpdatesMgr( wxFrameLayout* pPanel )
    : cbSimpleUpdatesMgr( pPanel )
{}

void cbGCUpdatesMgr::AddItem( wxList&     itemList,
                              cbBarInfo*  pBar, 
                              cbDockPane* pPane,
                              wxRect&     curBounds,
                              wxRect&     prevBounds )
{
    cbRectInfo* pInfo = new cbRectInfo();

    pInfo->mpBar     = pBar;
    pInfo->mpPane       = pPane;
    pInfo->mpCurBounds  = &curBounds;
    pInfo->mpPrevBounds = &prevBounds;

    itemList.Append( (wxObject*) pInfo );
}

void cbGCUpdatesMgr::OnStartChanges()
{
    // memorize states of ALL items in the layout -
    // this is quite excessive, but OK for the decent 
    // implementation of updates manager

    mpLayout->GetPrevClientRect() = mpLayout->GetClientRect();

    cbDockPane** panes = mpLayout->GetPanesArray();

    for( int n = 0; n != MAX_PANES; ++n )
    {
        cbDockPane& pane = *(panes[n]);

        // store pane state
        pane.mUMgrData.StoreItemState( pane.mBoundsInParent );
        pane.mUMgrData.SetDirty( false );

        cbRowInfo* pRow = pane.GetFirstRow();

        while ( pRow )
        {
            cbBarInfo* pBar = pRow->GetFirstBar();

            // store row state
            pRow->mUMgrData.StoreItemState( pRow->mBoundsInParent );
            pRow->mUMgrData.SetDirty( false );

            while( pBar )
            {
                // store bar state
                pBar->mUMgrData.StoreItemState( pBar->mBoundsInParent );
                pBar->mUMgrData.SetDirty( false );

                pBar = pBar->mpNext;
            }

            pRow = pRow->mpNext;
        }
    }
}

void cbGCUpdatesMgr::UpdateNow()
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

    wxList mBarsToResize;

    int n;
    for ( n = 0; n != MAX_PANES; ++n )
    {
        cbDockPane& pane = *(panes[n]);

        bool paneChanged = WasChanged( pane.mUMgrData, pane.mBoundsInParent );

        if ( paneChanged )
        {
            wxClientDC dc( &mpLayout->GetParentFrame() );
            pane.PaintPaneBackground( dc );
        }

        wxRect realBounds;

        cbRowInfo* pRow = pane.GetFirstRow();

        while ( pRow )
        {
            wxDC* pDc = 0;

            cbBarInfo* pBar = pRow->GetFirstBar();

            bool rowChanged = false;
//            bool rowBkPainted  = false;

            // FIXME:: the below should not be fixed
            cbBarInfo* barsToRepaint[128];
            // number of bars, that were changed in the current row
            int nBars = 0; 

            //wxRect r1 = pRow->mUMgrData.mPrevBounds;
            //wxRect r2 = pRow->mBoundsInParent;

            if ( WasChanged( pRow->mUMgrData, pRow->mBoundsInParent ) )
            
                rowChanged = true;
            else
                while( pBar )
                {
                    if ( WasChanged( pBar->mUMgrData, pBar->mBoundsInParent ) )
                    
                        barsToRepaint[nBars++] = pBar;

                    pBar = pBar->mpNext;
                }

            if ( nBars || rowChanged )
            {
                realBounds = pRow->mBoundsInParent;

                // include 1-pixel thick shades around the row
                realBounds.x -= 1;
                realBounds.y -= 1;
                realBounds.width  += 2;
                realBounds.height += 2;

                pDc = pane.StartDrawInArea( realBounds );
            }

            if ( rowChanged )
            {
                // postphone the resizement and refreshing the changed
                // bar windows

                cbBarInfo* pCurBar = pRow->GetFirstBar();

                while ( pCurBar )
                {
                    if ( WasChanged( pCurBar->mUMgrData, 
                                     pCurBar->mBoundsInParent ) )

                        AddItem( mBarsToResize, pCurBar, &pane, 
                                 pCurBar->mBoundsInParent,
                                 pCurBar->mUMgrData.mPrevBounds );

                    pCurBar = pCurBar->mpNext;
                }

                // draw only their decorations now

                pane.PaintRow( pRow, *pDc );
            }
            else
            if ( nBars != 0 )
            {
                for ( int i = 0; i != nBars; ++i )

                    // postphone the resizement and refreshing the changed
                    // bar windows

                    AddItem( mBarsToResize, 
                             barsToRepaint[i], 
                             &pane, 
                             barsToRepaint[i]->mBoundsInParent,
                             barsToRepaint[i]->mUMgrData.mPrevBounds );

                // redraw decorations of entire row, regardless of how much
                // of the bars were changed

                pane.PaintRow( pRow, *pDc );
            }

            if ( pDc )
        
                pane.FinishDrawInArea( realBounds );

            pRow = pRow->mpNext;

        } // end of while

        if ( paneChanged )
        {
            wxClientDC dc( &mpLayout->GetParentFrame() );
            pane.PaintPaneDecorations( dc );
        }

    } // end of for

    if ( clientWindowChanged && !mpLayout->mClientWndRefreshPending )
    {
        // ptr to client-window object is "marked" as NULL

        AddItem( mBarsToResize, NULL, NULL, 
                 mpLayout->GetClientRect(),
                 mpLayout->GetPrevClientRect() );
    }

    // step #2 - do ordered refreshing and resizing of bar window objects now

    DoRepositionItems( mBarsToResize );
}

void cbGCUpdatesMgr::DoRepositionItems( wxList& items )
{
    wxNode* pNode1 = items.GetFirst();

    while( pNode1 )
    {
        cbRectInfo& info = node_to_rect_info( pNode1 );

        wxNode* pNode2 = items.GetFirst();

        // and node itself

        mGC.AddObject( &info );

        while( pNode2 )
        {
            if ( pNode2 != pNode1 ) // node should not depend on itself
            {
                // Add references to objects on which this object
                // depends. Dependency here indicates intersection of current
                // bounds of this object with the initial bounds of the
                // other object.

                cbRectInfo& otherInfo = node_to_rect_info( pNode2 );

                if ( rect_hits_rect( *info.mpCurBounds, *otherInfo.mpPrevBounds ) )
                
                                    // the node    depends on node
                    mGC.AddDependency( &info,      &otherInfo      );
            }

            pNode2 = pNode2->GetNext();
        }

        pNode1 = pNode1->GetNext();
    }

    mGC.ArrangeCollection(); // order nodes according "least-dependency" rule,
                             // and find out cycled chains

    // Regular item nodes need to be resized, but not repainted (since
    // they stand in linear (not cyclic) dependency with other
    // regular nodes).

    wxNode* pNode = mGC.GetRegularObjects().GetFirst();

    while ( pNode )
    {
        cbRectInfo& info = *((cbRectInfo*)gc_node_to_obj(pNode));

        if ( info.mpBar == NULL ) 
            
            mpLayout->PositionClientWindow();
        else
            info.mpPane->SizeBar( info.mpBar );

        pNode = pNode->GetNext();
    }

    // cycled item nodes, need to be both resized and repainted

    pNode = mGC.GetCycledObjects().GetFirst();

    while ( pNode )
    {
        cbRectInfo& info = *((cbRectInfo*)gc_node_to_obj(pNode));

        if ( info.mpBar == NULL ) 
        {
            wxWindow* pClntWnd = mpLayout->GetFrameClient();

            mpLayout->PositionClientWindow();

            // FIXME FIXME:: excessive!

            pClntWnd->Show( false );
            pClntWnd->Show( true  );

            // OLD STUFF:: mpLayout->PositionClientWindow();
        }
        else
        if ( info.mpBar->mpBarWnd )
        {
            wxWindow* pWnd = info.mpBar->mpBarWnd;

            // resize
            info.mpPane->SizeBar( info.mpBar );

            // repaint

            /* OLD STUFF:: bool isChoice = info.mpBar->IsKindOf( CLASSINFO( wxChoice ) );

            //#ifdef __WINDOWS__
            //int result = ::SendMessage( (HWND)pWnd->m_hWnd, WM_NCPAINT, 0, 0 );
            //#endif
            */

            // FIXME FIXME:: there's no other way to repaint non-client area of the wxWindow!!
            //                 so we do *excessive* "hide 'n show"

            pWnd->Show(false);
            pWnd->Show(true);
                
            pWnd->Refresh();
        }

        pNode = pNode->GetNext();
    }

    // release data prepared for GC alg.

    pNode = items.GetFirst();

    while( pNode )
    {
        cbRectInfo* pInfo = (cbRectInfo*)(pNode->GetData());

        delete pInfo;

        pNode = pNode->GetNext();
    }

    mGC.Reset(); // reinit GC

    // FIXME:: this is a dirty-workaround for messy client-area,
    //         as a result of docking bar out of floated-container window

    if ( mpLayout->mClientWndRefreshPending )
    {
        mpLayout->PositionClientWindow();
        mpLayout->GetFrameClient()->Refresh();
    }
}

