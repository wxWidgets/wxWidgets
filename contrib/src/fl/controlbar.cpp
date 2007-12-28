/////////////////////////////////////////////////////////////////////////////
// Name:        controlbar.cpp
// Purpose:     Implementation for main controlbar classes.
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     06/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:     wxWindows license
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

#include "wx/string.h"
#include "wx/utils.h"     // import wxMin,wxMax macros
#include "wx/minifram.h"

#include "wx/fl/controlbar.h"

// import classes of default plugins

#include "wx/fl/panedrawpl.h"
#include "wx/fl/rowlayoutpl.h"
#include "wx/fl/antiflickpl.h"
#include "wx/fl/bardragpl.h"
#include "wx/fl/cbcustom.h"

#include "wx/fl/gcupdatesmgr.h"   // import default updates manager class ("garbage-collecting" one)
#include "wx/fl/updatesmgr.h"

#include "wx/fl/toolwnd.h"

// These are the event IDs being initialized to a value to
// meet the new event paradigm as of wx2.3.0.  Probably we
// should find a way to make these be non-global, but this
// works for right now.
    wxEventType cbEVT_PL_LEFT_DOWN = wxNewEventType();
    wxEventType cbEVT_PL_LEFT_UP = wxNewEventType();
    wxEventType cbEVT_PL_RIGHT_DOWN = wxNewEventType();
    wxEventType cbEVT_PL_RIGHT_UP = wxNewEventType();
    wxEventType cbEVT_PL_MOTION = wxNewEventType();

    wxEventType cbEVT_PL_LEFT_DCLICK = wxNewEventType();

    wxEventType cbEVT_PL_LAYOUT_ROW = wxNewEventType();
    wxEventType cbEVT_PL_RESIZE_ROW = wxNewEventType();
    wxEventType cbEVT_PL_LAYOUT_ROWS = wxNewEventType();
    wxEventType cbEVT_PL_INSERT_BAR = wxNewEventType();
    wxEventType cbEVT_PL_RESIZE_BAR = wxNewEventType();
    wxEventType cbEVT_PL_REMOVE_BAR = wxNewEventType();
    wxEventType cbEVT_PL_SIZE_BAR_WND = wxNewEventType();

    wxEventType cbEVT_PL_DRAW_BAR_DECOR = wxNewEventType();
    wxEventType cbEVT_PL_DRAW_ROW_DECOR = wxNewEventType();
    wxEventType cbEVT_PL_DRAW_PANE_DECOR = wxNewEventType();
    wxEventType cbEVT_PL_DRAW_BAR_HANDLES = wxNewEventType();
    wxEventType cbEVT_PL_DRAW_ROW_HANDLES = wxNewEventType();
    wxEventType cbEVT_PL_DRAW_ROW_BKGROUND = wxNewEventType();
    wxEventType cbEVT_PL_DRAW_PANE_BKGROUND = wxNewEventType();

    wxEventType cbEVT_PL_START_BAR_DRAGGING = wxNewEventType();
    wxEventType cbEVT_PL_DRAW_HINT_RECT = wxNewEventType();

    wxEventType cbEVT_PL_START_DRAW_IN_AREA = wxNewEventType();
    wxEventType cbEVT_PL_FINISH_DRAW_IN_AREA = wxNewEventType();

    wxEventType cbEVT_PL_CUSTOMIZE_BAR = wxNewEventType();
    wxEventType cbEVT_PL_CUSTOMIZE_LAYOUT = wxNewEventType();

    wxEventType wxCUSTOM_CB_PLUGIN_EVENTS_START_AT = wxNewEventType();

// some ascii-art, still can't get these *nice* cursors working on wx... :-(

/*
// FIXME:: see places where _gHorizCursorImg is used

static const char* _gHorizCursorImg[] =
{
    "............XX....XX............",
    "............XX....XX............",
    "............XX....XX............",
    "............XX....XX............",
    "............XX....XX............",
    "...X........XX....XX........X...",
    "..XX........XX....XX........XX..",
    ".XXX........XX....XX........XXX.",
    "XXXXXXXXXXXXXX....XXXXXXXXXXXXXX",
    ".XXX........XX....XX........XXX.",
    "..XX........XX....XX........XX..",
    "...X........XX....XX........X...",
    "............XX....XX............",
    "............XX....XX............",
    "............XX....XX............",
    "............XX....XX............"
};

static const char* _gVertCursorImg[] =
{
    "................X...............",
    "...............XXX..............",
    "..............XXXXX.............",
    ".............XXXXXXX............",
    "................X...............",
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
    "................................",
    "................................",
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
    "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
    "................X...............",
    ".............XXXXXXX............",
    "..............XXXXX.............",
    "...............XXX..............",
    "................X..............."
};
*/

// helper inline functions

static inline bool rect_contains_point( const wxRect& rect, int x, int y )
{
    return ( x >= rect.x &&
             y >= rect.y &&
             x <  rect.x + rect.width  &&
             y <  rect.y + rect.height );
}

static inline bool rect_hits_rect( const wxRect& r1, const wxRect& r2 )
{
    if ( ( r2.x >= r1.x && r2.x <= r1.x + r1.width ) ||
         ( r1.x >= r2.x && r1.x <= r2.x + r2.width ) )

        if ( ( r2.y >= r1.y && r2.y <= r1.y + r1.height ) ||
             ( r1.y >= r2.y && r1.y <= r2.y + r2.height ) )

            return 1;

    return 0;
}

static inline void hide_rect( wxRect& r )
{
    r.x = 32768;
    r.y = 32768;
    r.width  = 1;
    r.height = 1;
}

static inline void clip_rect_against_rect( wxRect& r1, const wxRect& r2 )
{
    if ( r1.x < r2.x              ||
         r1.y < r2.y              ||
         r1.x >= r2.x + r2.width  ||
         r1.y >= r2.y + r2.height
       )
    {
        hide_rect( r1 );
        return;
    }
    else
    {
        if ( r1.x + r1.width  > r2.x + r2.width )

            r1.width = r2.x + r2.width - r1.x;

        if ( r1.y + r1.height > r2.y + r2.height )

            r1.height = r2.y + r2.height - r1.y;
    }
}

/***** Implementation for class cbBarSpy *****/

IMPLEMENT_DYNAMIC_CLASS( cbBarSpy, wxEvtHandler )

cbBarSpy::cbBarSpy(void)
    : mpLayout(0),
      mpBarWnd(0)
{}

cbBarSpy::cbBarSpy( wxFrameLayout* pPanel )

    : mpLayout(pPanel),
      mpBarWnd(0)
{}

void cbBarSpy::SetBarWindow( wxWindow* pWnd )
{
    mpBarWnd = pWnd;
}

bool cbBarSpy::ProcessEvent(wxEvent& event)
{
    bool handled = wxEvtHandler::ProcessEvent( event );

    int type = event.GetEventType();

    if ( !handled && ( type == wxEVT_LEFT_DOWN ||
                       type == wxEVT_LEFT_DCLICK ) )
    {
        wxMouseEvent& mevent = *((wxMouseEvent*)&event);

        int x = mevent.m_x;
        int y = mevent.m_y;

        mpBarWnd->ClientToScreen( &x, &y );
        mpLayout->GetParentFrame().ScreenToClient( &x, &y );

        mevent.m_x = x;
        mevent.m_y = y;

        // forwared not-handled event to frame-layout

        if ( type == wxEVT_LEFT_DOWN )
        {
            //mpLayout->OnLButtonDown( mevent );
            event.Skip();
        }
        else
            mpLayout->OnLDblClick( mevent );

        //event.Skip(false);
    }

    return handled;
}

/***** Implementation for class wxFrameLayout *****/

IMPLEMENT_DYNAMIC_CLASS( wxFrameLayout, wxEvtHandler )

BEGIN_EVENT_TABLE( wxFrameLayout, wxEvtHandler )

    EVT_PAINT      ( wxFrameLayout::OnPaint       )
    EVT_SIZE       ( wxFrameLayout::OnSize        )
    EVT_LEFT_DOWN  ( wxFrameLayout::OnLButtonDown )
    EVT_LEFT_UP    ( wxFrameLayout::OnLButtonUp   )
    EVT_RIGHT_DOWN ( wxFrameLayout::OnRButtonDown )
    EVT_RIGHT_UP   ( wxFrameLayout::OnRButtonUp   )
    EVT_MOTION     ( wxFrameLayout::OnMouseMove   )

    EVT_LEFT_DCLICK( wxFrameLayout::OnLDblClick   )

    EVT_IDLE       ( wxFrameLayout::OnIdle        )

    EVT_ERASE_BACKGROUND( wxFrameLayout::OnEraseBackground )

END_EVENT_TABLE()

// FIXME:: how to eliminate these cut&pasted constructors?

wxFrameLayout::wxFrameLayout(void)

    : mpFrame      ( NULL ),
      mpFrameClient( NULL ),

      mDarkPen  ( wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW), 1, wxSOLID ),
      mLightPen ( wxSystemSettings::GetColour(wxSYS_COLOUR_3DHILIGHT), 1, wxSOLID ),
      mGrayPen  ( wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE), 1, wxSOLID ),
      mBlackPen ( wxColour(  0,  0,  0), 1, wxSOLID ),
      mBorderPen( wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE), 1, wxSOLID ),

      mNullPen( wxColour(0,0,0), 1, wxTRANSPARENT ),

      mpPaneInFocus( NULL ),
      mpLRUPane    ( NULL ),


      mpTopPlugin   ( NULL ),
      mpCaputesInput( NULL ),

      mClientWndRefreshPending( false ),
      mRecalcPending( true ),
      mCheckFocusWhenIdle( false )
{
    CreateCursors();

    int i;
    for ( i = 0; i != MAX_PANES; ++i )
        mPanes[i]  = NULL;

    mFloatingOn = CanReparent();
}

wxFrameLayout::wxFrameLayout( wxWindow* pParentFrame, wxWindow* pFrameClient, bool activateNow )

    : mpFrame( pParentFrame ),
      mpFrameClient(pFrameClient),

      mDarkPen  ( wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW), 1, wxSOLID ),
      mLightPen ( wxSystemSettings::GetColour(wxSYS_COLOUR_3DHILIGHT), 1, wxSOLID ),
      mGrayPen  ( wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE), 1, wxSOLID ),
      mBlackPen ( wxColour(  0,  0,  0), 1, wxSOLID ),
      mBorderPen( wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE), 1, wxSOLID ),

      mNullPen( wxColour(0,0,0), 1, wxTRANSPARENT ),

      mpPaneInFocus( NULL ),
      mpLRUPane    ( NULL ),

      mFloatingOn   ( true ),

      mpTopPlugin   ( NULL ),
      mpCaputesInput( NULL ),

      mClientWndRefreshPending( false ),
      mRecalcPending( true ),
      mCheckFocusWhenIdle( false ),

      mpUpdatesMgr( NULL )
{
    CreateCursors();

    int i;
    for ( i = 0; i != MAX_PANES; ++i )
        mPanes[i]  = new cbDockPane( i, this );

    if ( activateNow )
    {
        HookUpToFrame();

        // FOR NOW::
        // DBG:: set RED color of frame's background for the
        //       prurpose of tracking engine bugs "visually"

        GetParentFrame().SetBackgroundColour( wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE) );
    }

    mFloatingOn = CanReparent();
}

// NOTE:: below are the only platform-check "ifdef"s in the docking system!

bool wxFrameLayout::CanReparent()
{
#ifdef __WXMSW__
    return true;
#elif defined(__WXGTK20__)
    return true;
#elif defined (__WXGTK__)
    //return true;
    return false;
#else

    return false; // reparenting is not yet supported by Motif and others
#endif
}

/*
#ifdef __WXMSW__
    #inlcude "windows.h"
#endif
*/

void wxFrameLayout::ReparentWindow( wxWindow* pChild, wxWindow* pNewParent )
{
#if defined(__WXMSW__) || defined(__WXGTK20__) || defined(__WXMAC__)
    pChild->Reparent(pNewParent);

    return;
#elif defined(__WXGTK__) || defined(__WXX11__)
    // FOR NOW:: floating with wxGtk still very buggy

    return;

    //pChild->ReParent( pNewParent );

    //return;
#else
    wxUnusedVar(pChild);
    wxUnusedVar(pNewParent);
    wxMessageBox( _("Sorry, docking is not supported for ports other than wxMSW, wxMac and wxGTK") );
#endif
}

void wxFrameLayout::DestroyBarWindows()
{
    wxObjectList::compatibility_iterator pSpy = mBarSpyList.GetFirst();

    while( pSpy )
    {
        cbBarSpy& spy = *((cbBarSpy*)pSpy->GetData());

        if ( spy.mpBarWnd->GetEventHandler() == &spy )

            spy.mpBarWnd->PopEventHandler();

        delete &spy;

        pSpy = pSpy->GetNext();
    }

    mBarSpyList.Clear();

    size_t i;
    for ( i = 0; i != mAllBars.Count(); ++i )
    {
        if ( mAllBars[i]->mpBarWnd )
        {
            mAllBars[i]->mpBarWnd->Destroy();
            mAllBars[i]->mpBarWnd = NULL;
        }
    }
}

void wxFrameLayout::ShowFloatedWindows( bool show )
{
    wxObjectList::compatibility_iterator pNode = mFloatedFrames.GetFirst();

    while( pNode )
    {
        cbFloatedBarWindow* pFFrm = ((cbFloatedBarWindow*)pNode->GetData());

        pFFrm->Show( show );

        pNode = pNode->GetNext();
    }
}

wxFrameLayout::~wxFrameLayout()
{
    size_t i;

    UnhookFromFrame();

    if ( mpUpdatesMgr )
        delete mpUpdatesMgr;

    PopAllPlugins();

    // destoy the chain of plugins from left to right

    wxEvtHandler* pCur = mpTopPlugin;

    if ( pCur )

        while ( pCur->GetPreviousHandler() )

            pCur = pCur->GetPreviousHandler();

    while ( pCur )
    {
        wxEvtHandler* pNext = pCur->GetNextHandler();

        delete pCur;

        pCur = pNext;
    }

    // destroy contents of arrays and lists

    for ( i = 0; i != MAX_PANES; ++i )
    {
        if ( mPanes[i] )
            delete mPanes[i];
    }
    if ( mpHorizCursor  )
        delete mpHorizCursor;
    if ( mpVertCursor   )
        delete mpVertCursor;
    if ( mpNormalCursor )
        delete mpNormalCursor;
    if ( mpDragCursor   )
        delete mpDragCursor;
    if ( mpNECursor     )
        delete mpNECursor;

    wxObjectList::compatibility_iterator pSpy = mBarSpyList.GetFirst();

    while( pSpy )
    {
        cbBarSpy& spy = *((cbBarSpy*)pSpy->GetData());

        if ( spy.mpBarWnd->GetEventHandler() == &spy )

            spy.mpBarWnd->PopEventHandler();

        delete &spy;

        pSpy = pSpy->GetNext();
    }

    for ( i = 0; i != mAllBars.Count(); ++i )
        delete mAllBars[i];
}

void wxFrameLayout::EnableFloating( bool enable )
{
    mFloatingOn = enable && CanReparent();
}

void wxFrameLayout::Activate()
{
    HookUpToFrame();

    RefreshNow( true );

    ShowFloatedWindows( true );
}

void wxFrameLayout::Deactivate()
{
    ShowFloatedWindows( false );

    UnhookFromFrame();

    HideBarWindows();
}

void wxFrameLayout::SetFrameClient( wxWindow* pFrameClient )
{
    mpFrameClient = pFrameClient;
}

wxWindow* wxFrameLayout::GetFrameClient()
{
    return mpFrameClient;
}

cbUpdatesManagerBase& wxFrameLayout::GetUpdatesManager()
{
    if ( !mpUpdatesMgr )
        mpUpdatesMgr = CreateUpdatesManager();

    return *mpUpdatesMgr;
}

void wxFrameLayout::SetUpdatesManager( cbUpdatesManagerBase* pUMgr )
{
    if ( mpUpdatesMgr )
        delete mpUpdatesMgr;

    mpUpdatesMgr = pUMgr;

    mpUpdatesMgr->SetLayout( this );
}

cbUpdatesManagerBase* wxFrameLayout::CreateUpdatesManager()
{
    return new cbGCUpdatesMgr( this );
    //return new cbSimpleUpdatesMgr( this );
}

void wxFrameLayout::AddBar( wxWindow*        pBarWnd,
                            const cbDimInfo& dimInfo,
                            int              alignment,
                            int              rowNo,
                            int              columnPos,
                            const wxString&  name,
                            bool             spyEvents,
                            int              state
                          )
{
    if ( pBarWnd && spyEvents )
    {
        // hook up spy to bar window
        cbBarSpy* pSpy = new cbBarSpy( this );

        pSpy->SetBarWindow( pBarWnd );
        pBarWnd->PushEventHandler( pSpy );

        mBarSpyList.Append( pSpy );
    }

    cbBarInfo* pInfo = new cbBarInfo();

    pInfo->mName      = name;
    pInfo->mpBarWnd   = pBarWnd;
    pInfo->mDimInfo   = dimInfo;
    pInfo->mDimInfo.mLRUPane = alignment;
    pInfo->mState     = state;
    pInfo->mAlignment = alignment;
    pInfo->mRowNo     = rowNo;
    pInfo->mBounds.x  = columnPos;

    mAllBars.Add( pInfo );

    DoSetBarState( pInfo );
}

bool wxFrameLayout::RedockBar( cbBarInfo*    pBar,
                               const wxRect& shapeInParent,
                               cbDockPane*   pToPane,
                               bool          updateNow )
{
    if ( !pToPane )

        pToPane = HitTestPanes( shapeInParent, NULL );

    if ( !pToPane )

        return false; // bar's shape does not hit any pane
                     // - redocking is NOT possible

    cbDockPane* pBarPane = GetBarPane( pBar );

    if ( updateNow )

        GetUpdatesManager().OnStartChanges();

    pBarPane->RemoveBar( pBar );

    // FIXME FIXME:: the recalculation below may be a *huge* performance
    //               hit, it could be eliminated though...
    //               but first the "pane-postion-changed" problem
    //               has to be fixed

    RecalcLayout( false );

    pToPane->InsertBar( pBar, shapeInParent );

    RecalcLayout( false );

    // finish update "transaction"

    if ( updateNow )
    {
        GetUpdatesManager().OnFinishChanges();
        GetUpdatesManager().UpdateNow();
    }

    return true;
}

cbBarInfo* wxFrameLayout::FindBarByName( const wxString& name )
{
    size_t i;
    for ( i = 0; i != mAllBars.Count(); ++i )
        if ( mAllBars[i]->mName == name )
            return mAllBars[i];

    return NULL;
}

cbBarInfo* wxFrameLayout::FindBarByWindow( const wxWindow* pWnd )
{
    size_t i;
    for ( i = 0; i != mAllBars.Count(); ++i )
        if ( mAllBars[i]->mpBarWnd == pWnd )
            return mAllBars[i];

    return NULL;
}

BarArrayT& wxFrameLayout::GetBars()
{
    return mAllBars;
}

void wxFrameLayout::SetBarState( cbBarInfo* pBar, int newState, bool updateNow )
{
    if ( newState == wxCBAR_FLOATING && !(mFloatingOn && pBar->mFloatingOn))

        return;

    if ( updateNow )

        GetUpdatesManager().OnStartChanges();

    pBar->mUMgrData.SetDirty(true);

    // check bar's previous state

    if ( pBar->mState != wxCBAR_HIDDEN && pBar->mState != wxCBAR_FLOATING )
    {
        cbDockPane* pPane;
        cbRowInfo*  pRow;

        #ifdef  __WXDEBUG__
        bool success =
        #endif
                       LocateBar( pBar, &pRow, &pPane );

        wxASSERT( success ); // DBG::

        // save LRU-dim info before removing bar

        pBar->mDimInfo.mLRUPane = pPane->GetAlignment();
        pBar->mDimInfo.mBounds[ pPane->GetAlignment() ] = pBar->mBounds;

        // remove it from the pane it was docked on

        pPane->RemoveBar( pBar );

    }

    if ( pBar->mState == wxCBAR_FLOATING && newState != wxCBAR_FLOATING )
    {
        // remove bar's window from the containing mini-frame
        // and set its parent to be layout's parent frame

        if ( pBar->mpBarWnd )
        {
            pBar->mpBarWnd->Show(false); // to avoid flicker upon reparenting

            wxObjectList::compatibility_iterator pNode = mFloatedFrames.GetFirst();

            while( pNode )
            {
                cbFloatedBarWindow* pFFrm = ((cbFloatedBarWindow*)pNode->GetData());

                if ( pFFrm->GetBar() == pBar )
                {
                    pFFrm->Show( false ); // reduces flicker sligthly

                    ReparentWindow( pBar->mpBarWnd, &GetParentFrame() );

                    pBar->mBounds = pBar->mDimInfo.mBounds[ pBar->mDimInfo.mLRUPane ];

                    if ( newState != wxCBAR_HIDDEN )

                        pBar->mAlignment = pBar->mDimInfo.mLRUPane;

                    mFloatedFrames.Erase( pNode );

                    pFFrm->Show( false );

                    // Workaround assert that causes a crash on the next time something tries to CaptureMouse
                    if (pFFrm->HasCapture()) pFFrm->ReleaseMouse();

                    pFFrm->Destroy(); break;
                }

                pNode = pNode->GetNext();
            }

            // FOR NOW:: excessive!
            //if ( mpFrameClient ) mpFrameClient->Refresh();
            if ( mpFrameClient )
                mClientWndRefreshPending = true;
        }
    }

    if ( pBar->mDimInfo.GetDimHandler() )
    {
        pBar->mDimInfo.GetDimHandler()->OnChangeBarState( pBar, newState );
    }

    pBar->mState = newState;

    DoSetBarState( pBar );

    if ( updateNow )
    {
        RecalcLayout(false);

        GetUpdatesManager().OnFinishChanges();
        GetUpdatesManager().UpdateNow();
    }
}

void wxFrameLayout::InverseVisibility( cbBarInfo* pBar )
{
    wxASSERT( pBar ); // DBG::

    // "inverse" bar-visibility of the selected bar

    int newState;

    if ( pBar->mState == wxCBAR_HIDDEN )
    {
        if ( pBar->mAlignment == -1 )
        {
            pBar->mAlignment = 0;       // just remove "-1" marking
            newState = wxCBAR_FLOATING;
        }
        else
        if ( pBar->mAlignment == FL_ALIGN_TOP ||
             pBar->mAlignment == FL_ALIGN_BOTTOM )

            newState = wxCBAR_DOCKED_HORIZONTALLY;
        else
            newState = wxCBAR_DOCKED_VERTICALLY;
    }
    else
    {
        newState = wxCBAR_HIDDEN;

        if ( pBar->mState == wxCBAR_FLOATING )

            pBar->mAlignment = -1;
    }

    this->SetBarState( pBar, newState, true );

    if ( newState == wxCBAR_FLOATING )

        this->RepositionFloatedBar( pBar );
}

void wxFrameLayout::ApplyBarProperties( cbBarInfo* pBar )
{
    if ( pBar->mState == wxCBAR_FLOATING )
    {
        RepositionFloatedBar( pBar );
    }
    else
    if ( pBar->mState == wxCBAR_DOCKED_HORIZONTALLY ||
         pBar->mState == wxCBAR_DOCKED_VERTICALLY
       )
    {
        // FOR NOW:: nothing
    }

}

void wxFrameLayout::RepositionFloatedBar( cbBarInfo* pBar )
{
    if ( !(mFloatingOn && pBar->mFloatingOn)) return;

    wxObjectList::compatibility_iterator pNode = mFloatedFrames.GetFirst();

    while( pNode )
    {
        cbFloatedBarWindow* pFFrm = ((cbFloatedBarWindow*)pNode->GetData());

        if ( pFFrm->GetBar() == pBar )
        {
            wxRect& bounds = pBar->mDimInfo.mBounds[wxCBAR_FLOATING];

            int x = bounds.x,
                y = bounds.y;

            GetParentFrame().ClientToScreen( &x, &y );

            pFFrm->PositionFloatedWnd( x,y,
                                       bounds.width,
                                       bounds.height );

            break;
        }

        pNode = pNode->GetNext();
    }
}

void wxFrameLayout::DoSetBarState( cbBarInfo* pBar )
{
    if ( pBar->mState != wxCBAR_FLOATING &&
         pBar->mState != wxCBAR_HIDDEN )

        // dock it

        mPanes[pBar->mAlignment]->InsertBar( pBar );
    else
    if ( pBar->mState == wxCBAR_HIDDEN )
    {
        // hide it

        if ( pBar->mpBarWnd )

            pBar->mpBarWnd->Show( false );
    }
    else
    {
        if ( !(mFloatingOn && pBar->mFloatingOn) )
          return;

        // float it

        if ( pBar->mpBarWnd == NULL || !CanReparent() )
        {
            // FOR NOW:: just hide it

            if ( pBar->mpBarWnd )

                pBar->mpBarWnd->Show( false );

            pBar->mState = wxCBAR_HIDDEN;

            return;
        }

        cbFloatedBarWindow* pMiniFrm = new cbFloatedBarWindow();

        pMiniFrm->SetBar( pBar );
        pMiniFrm->SetLayout( this );

        pMiniFrm->Create( &GetParentFrame(), wxID_ANY, pBar->mName,
                          wxPoint( 50,50 ),
                          wxSize ( 0, 0  ),
                          wxFRAME_TOOL_WINDOW | wxFRAME_FLOAT_ON_PARENT
                        );

        pMiniFrm->SetClient( pBar->mpBarWnd );

        ReparentWindow( pBar->mpBarWnd, pMiniFrm );

        mFloatedFrames.Append( pMiniFrm );

        wxRect& bounds = pBar->mDimInfo.mBounds[wxCBAR_FLOATING];

        // check if it wasn't floated anytime before

        if ( bounds.width == -1 )
        {
            wxRect& clntRect = GetClientRect();

            // adjust position into which the next floated bar will be placed

            if ( mNextFloatedWndPos.x + bounds.width > clntRect.width )

                mNextFloatedWndPos.x = mFloatingPosStep.x;

            if ( mNextFloatedWndPos.y + bounds.height > clntRect.height )

                mNextFloatedWndPos.y = mFloatingPosStep.y;

            bounds.x = mNextFloatedWndPos.x + clntRect.x;
            bounds.y = mNextFloatedWndPos.y + clntRect.y;

            bounds.width  = pBar->mDimInfo.mSizes[wxCBAR_FLOATING].x;
            bounds.height = pBar->mDimInfo.mSizes[wxCBAR_FLOATING].y;

            mNextFloatedWndPos.x += mFloatingPosStep.x;
            mNextFloatedWndPos.y += mFloatingPosStep.y;
        }

        pMiniFrm->Show( true );
        RepositionFloatedBar(pMiniFrm->GetBar());

        // FIXME:: this is excessive
        pBar->mpBarWnd->Show(true);
    }
}

void wxFrameLayout::RemoveBar( cbBarInfo* pBarInfo )
{
    // first, try to "guess" what was the perviouse state of the bar

    cbDockPane* pPane;
    cbRowInfo*  pRow;

    if ( LocateBar( pBarInfo, &pRow, &pPane ) )
    {
        // ...aha, bar was docked into one of the panes,
        // remove it from there

        pPane->RemoveBar( pBarInfo );
    }

    size_t i;
    for ( i = 0; i != mAllBars.Count(); ++i )
    {
        if ( mAllBars[i] == pBarInfo )
        {
#if wxCHECK_VERSION(2,3,2)
            mAllBars.RemoveAt(i);
#else
            mAllBars.Remove(i);
#endif
            if ( pBarInfo->mpBarWnd ) // hides it's window

                pBarInfo->mpBarWnd->Show( false );

            delete pBarInfo;

            return;
        }
    }
    wxFAIL_MSG(wxT("bar info should be present in the list of all bars of all panes"));
}

bool wxFrameLayout::LocateBar( cbBarInfo* pBarInfo,
                               cbRowInfo**  ppRow,
                               cbDockPane** ppPane )
{
    (*ppRow)  = NULL;
    (*ppPane) = NULL;

    int n;
    for ( n = 0; n != MAX_PANES; ++n )
    {
        wxBarIterator i( mPanes[n]->GetRowList() );

        while ( i.Next() )

            if ( &i.BarInfo() == pBarInfo )
            {
                (*ppPane) = mPanes[n];
                (*ppRow ) = &i.RowInfo();

                return true;
            }
    }

    return false;
}

void wxFrameLayout::RecalcLayout( bool repositionBarsNow )
{
    mRecalcPending = false;

    int frmWidth, frmHeight;
    mpFrame->GetClientSize( &frmWidth, &frmHeight );

    int curY = 0;
    int curX = 0;
    wxRect rect;

    // pane positioning priorities in decreasing order:
    // top, bottom, left, right

    // setup TOP pane

    cbDockPane* pPane = mPanes[ FL_ALIGN_TOP ];

    pPane->SetPaneWidth( frmWidth );
    pPane->RecalcLayout();

    int paneHeight = pPane->GetPaneHeight();

    rect.x      = curX;
    rect.y      = curY;
    rect.width  = frmWidth;
    rect.height = wxMin( paneHeight, frmHeight - curY );

    pPane->SetBoundsInParent( rect );

    curY += paneHeight;

    // setup BOTTOM pane

    pPane = mPanes[ FL_ALIGN_BOTTOM ];

    pPane->SetPaneWidth( frmWidth );
    pPane->RecalcLayout();

    paneHeight = pPane->GetPaneHeight();

    rect.x      = curX;
    rect.y      = wxMax( frmHeight - paneHeight, curY );
    rect.width  = frmWidth;
    rect.height = frmHeight - rect.y;

    pPane->SetBoundsInParent( rect );

    // setup LEFT pane

    pPane = mPanes[ FL_ALIGN_LEFT ];

                         // bottom pane's y
    pPane->SetPaneWidth( rect.y - curY );

    pPane->RecalcLayout();
    paneHeight = pPane->GetPaneHeight();

                  // bottom rect's y
    rect.height = rect.y - curY;
    rect.x = curX;
    rect.y = curY;
    rect.width  = wxMin( paneHeight, frmWidth );

    pPane->SetBoundsInParent( rect );

    curX += rect.width;

    // setup RIGHT pane

    pPane = mPanes[ FL_ALIGN_RIGHT ];

                         // left pane's height
    pPane->SetPaneWidth( rect.height );

    pPane->RecalcLayout();
    paneHeight = pPane->GetPaneHeight();

                  // left pane's height
    rect.height = rect.height;
    rect.x = wxMax( frmWidth - paneHeight, curX );
    rect.y = curY;
    rect.width  = frmWidth - rect.x;

    pPane->SetBoundsInParent( rect );

    // recalc bounds of the client-window

    mClntWndBounds.x = mPanes[FL_ALIGN_LEFT]->mBoundsInParent.x +
                       mPanes[FL_ALIGN_LEFT]->mBoundsInParent.width;
    mClntWndBounds.y = mPanes[FL_ALIGN_TOP ]->mBoundsInParent.y +
                       mPanes[FL_ALIGN_TOP ]->mBoundsInParent.height;

    mClntWndBounds.width  = mPanes[FL_ALIGN_RIGHT]->mBoundsInParent.x -
                            mClntWndBounds.x;
    mClntWndBounds.height = mPanes[FL_ALIGN_BOTTOM]->mBoundsInParent.y -
                            mClntWndBounds.y;

    if ( repositionBarsNow )

        PositionPanes();
}

int wxFrameLayout::GetClientHeight()
{
    // for better portablility wxWindow::GetSzie() is not used here

    return mClntWndBounds.height;
}

int wxFrameLayout::GetClientWidth()
{
    // for better portablility wxWindow::GetSzie() is not used here

    return mClntWndBounds.width;
}

void wxFrameLayout::PositionClientWindow()
{
    if ( mpFrameClient )
    {
        if ( mClntWndBounds.width >= 1 && mClntWndBounds.height >= 1 )
        {
            mpFrameClient->SetSize( mClntWndBounds.x,     mClntWndBounds.y,
                                    mClntWndBounds.width, mClntWndBounds.height, 0 );

            if ( !mpFrameClient->IsShown() )

                mpFrameClient->Show( true );
        }
        else
            mpFrameClient->Show( false );
    }
}

void wxFrameLayout::PositionPanes()
{
    PositionClientWindow();

    // FOR NOW:: excessive updates!
    // reposition bars within all panes

    int i;
    for ( i = 0; i != MAX_PANES; ++i )
        mPanes[i]->SizePaneObjects();
}

void wxFrameLayout::OnSize( wxSizeEvent& event )
{
    mpFrame->ProcessEvent( event );
    event.Skip( false ); // stop its progpagation

    if ( event.GetEventObject() == (wxObject*) mpFrame )
    {
        GetUpdatesManager().OnStartChanges();
        RecalcLayout(true);
        GetUpdatesManager().OnFinishChanges();
        GetUpdatesManager().UpdateNow();
    }

}

/*** protected members ***/

void wxFrameLayout::HideBarWindows()
{
    size_t i;
    for ( i = 0; i != mAllBars.Count(); ++i )
        if ( mAllBars[i]->mpBarWnd && mAllBars[i]->mState != wxCBAR_FLOATING )
            mAllBars[i]->mpBarWnd->Show( false );

    // then floated frames

    ShowFloatedWindows( false );

    if ( mpFrameClient )

        mpFrameClient->Show( false );
}

void wxFrameLayout::UnhookFromFrame()
{
    // NOTE:: the SetEvtHandlerEnabled() method is not used
    //        here, since it is assumed that unhooking layout
    //        from window may result destroying of the layout itself
    //
    //        BUG BUG BUG (wx):: this would not be a problem if
    //                           wxEvtHandler's destructor checked if
    //                           this handler is currently the top-most
    //                           handler of some window, and additionally
    //                           to the reconnecting itself from the chain.
    //                           It would also re-setup current event handler
    //                           of the window using wxWindow::SetEventHandler()

    // FOR NOW::

    if ( mpFrame->GetEventHandler() == this )
    {
        mpFrame->PopEventHandler();
        return;
    }

    if ( mpFrame )
    {
        if ( this == mpFrame->GetEventHandler() )
        {
            mpFrame->SetEventHandler( this->GetNextHandler() );
        }
        else
        {
            wxEvtHandler* pCur = mpFrame->GetEventHandler();

            while ( pCur )
            {
                if ( pCur == this )
                    break;

                pCur = pCur->GetNextHandler();
            }

            // do not try to unhook ourselves if we're not hooked yet
            if ( !pCur )
                return;
        }

        if ( GetPreviousHandler() )
            GetPreviousHandler()->SetNextHandler( GetNextHandler() );
        else
        {
            mpFrame->PopEventHandler();
            return;
        }

        if ( GetNextHandler() )
            GetNextHandler()->SetPreviousHandler( GetPreviousHandler() );

        SetNextHandler( NULL );
        SetPreviousHandler( NULL );
    }
}

void wxFrameLayout::HookUpToFrame()
{
    // unhook us first, we're already hooked up

    UnhookFromFrame();

    // put ourselves on top

    mpFrame->PushEventHandler( this );
}

cbDockPane* wxFrameLayout::GetBarPane( cbBarInfo* pBar )
{
    int i;
    for ( i = 0; i != MAX_PANES; ++i )
        if ( mPanes[i]->BarPresent( pBar ) )
            return mPanes[i];

    return NULL;
}

void wxFrameLayout::CreateCursors()
{
    /*
    // FIXME:: The below code somehow doesn't work - cursors remain unchanged
    char bits[64];

    set_cursor_bits( _gHorizCursorImg, bits, 32, 16 );

    mpHorizCursor = new wxCursor( bits, 32, 16 );

    set_cursor_bits( _gVertCursorImg, bits, 32, 16 );

    mpVertCursor  = new wxCursor( bits, 32, 16 );
    */

    // FOR NOW:: use standard ones

    mpHorizCursor  = new wxCursor(wxCURSOR_SIZEWE);
    mpVertCursor   = new wxCursor(wxCURSOR_SIZENS);
    mpNormalCursor = new wxCursor(wxCURSOR_ARROW );
    mpDragCursor   = new wxCursor(wxCURSOR_CROSS );
    mpNECursor     = new wxCursor(wxCURSOR_NO_ENTRY);

    mFloatingPosStep.x = 25;
    mFloatingPosStep.y = 25;

    mNextFloatedWndPos.x = mFloatingPosStep.x;
    mNextFloatedWndPos.y = mFloatingPosStep.y;
}

bool wxFrameLayout::HitTestPane( cbDockPane* pPane, int x, int y )
{
    return rect_contains_point( pPane->GetRealRect(), x, y );
}

cbDockPane* wxFrameLayout::HitTestPanes( const wxRect& rect,
                                         cbDockPane* pCurPane )
{
    // first, give the privilege to the current pane

    if ( pCurPane && rect_hits_rect( pCurPane->GetRealRect(), rect ) )

        return pCurPane;

    int i;
    for ( i = 0; i != MAX_PANES; ++i )
    {
        if ( pCurPane != mPanes[i] &&
             rect_hits_rect( mPanes[i]->GetRealRect(), rect ) )
        {
            return mPanes[i];
        }
    }
    return 0;
}

void wxFrameLayout::ForwardMouseEvent( wxMouseEvent& event,
                                           cbDockPane*   pToPane,
                                           int           eventType )
{
    wxPoint pos( event.m_x, event.m_y );
    pToPane->FrameToPane( &pos.x, &pos.y );

    if ( eventType == cbEVT_PL_LEFT_DOWN )
    {
        cbLeftDownEvent evt( pos, pToPane );
        FirePluginEvent( evt );
    }
    else if ( eventType == cbEVT_PL_LEFT_DCLICK )
    {
        cbLeftDClickEvent evt( pos, pToPane );
        FirePluginEvent( evt );
    }
    else if ( eventType == cbEVT_PL_LEFT_UP )
    {
        cbLeftUpEvent evt( pos, pToPane );
        FirePluginEvent( evt );
    }
    else if ( eventType == cbEVT_PL_RIGHT_DOWN )
    {
        cbRightDownEvent evt( pos, pToPane );
        FirePluginEvent( evt );
    }
    else if ( eventType == cbEVT_PL_RIGHT_UP )
    {
        cbRightUpEvent evt( pos, pToPane );
        FirePluginEvent( evt );
    }
    else if ( eventType == cbEVT_PL_MOTION )
    {
        cbMotionEvent evt( pos, pToPane );
        FirePluginEvent( evt );
    }
}  // wxFrameLayout::ForwardMouseEvent()


void wxFrameLayout::RouteMouseEvent( wxMouseEvent& event, int pluginEvtType )
{
    if ( mpPaneInFocus )

        ForwardMouseEvent( event, mpPaneInFocus, pluginEvtType );
    else
    {
        int i;
        for ( i = 0; i != MAX_PANES; ++i )
        {
            if ( HitTestPane( mPanes[i], event.m_x, event.m_y ) )
            {
                ForwardMouseEvent( event, mPanes[i], pluginEvtType );
                return;
            }
        }
    }
}

/*** event handlers ***/

void wxFrameLayout::OnRButtonDown( wxMouseEvent& event )
{
    RouteMouseEvent( event, cbEVT_PL_RIGHT_DOWN );
}

void wxFrameLayout::OnRButtonUp( wxMouseEvent& event )
{
    RouteMouseEvent( event, cbEVT_PL_RIGHT_UP );
}

void wxFrameLayout::OnLButtonDown( wxMouseEvent& event )
{
    RouteMouseEvent( event, cbEVT_PL_LEFT_DOWN );
}

void wxFrameLayout::OnLDblClick( wxMouseEvent& event )
{
    RouteMouseEvent( event, cbEVT_PL_LEFT_DCLICK );
}

void wxFrameLayout::OnLButtonUp( wxMouseEvent& event )
{
    RouteMouseEvent( event, cbEVT_PL_LEFT_UP );
}

void wxFrameLayout::OnMouseMove( wxMouseEvent& event )
{
    if ( mpPaneInFocus )

        ForwardMouseEvent( event, mpPaneInFocus, cbEVT_PL_MOTION );
    else
    {
        int i;
        for ( i = 0; i != MAX_PANES; ++i )
        {
            if ( HitTestPane( mPanes[i], event.m_x, event.m_y ) )
            {
                if ( mpLRUPane && mpLRUPane != mPanes[i] )
                {
                    // simulate "mouse-leave" event
                    ForwardMouseEvent( event, mpLRUPane, cbEVT_PL_MOTION );
                }

                ForwardMouseEvent( event, mPanes[i], cbEVT_PL_MOTION );

                mpLRUPane = mPanes[i];

                return;
            }
        }
    }

    if ( mpLRUPane )
    {
        // simulate "mouse-leave" event
        ForwardMouseEvent( event, mpLRUPane, cbEVT_PL_MOTION );
        mpLRUPane = 0;
    }
}

void wxFrameLayout::OnPaint( wxPaintEvent& event )
{
    if ( mRecalcPending  )
        RecalcLayout( true );

    wxPaintDC dc(mpFrame);

    int i;
    for ( i = 0; i != MAX_PANES; ++i )
    {
        wxRect& rect = mPanes[i]->mBoundsInParent;

        dc.SetClippingRegion( rect.x, rect.y, rect.width, rect.height );

        mPanes[i]->PaintPane(dc);

        dc.DestroyClippingRegion();
    }

    event.Skip();
}

void wxFrameLayout::OnEraseBackground( wxEraseEvent& WXUNUSED(event) )
{
    // do nothing
}

void wxFrameLayout::OnIdle( wxIdleEvent& event )
{
    wxWindow* focus = wxWindow::FindFocus();

    if ( !focus && mCheckFocusWhenIdle )
    {
        wxMessageBox(wxT("Hi, no more focus in this app!"));

        mCheckFocusWhenIdle = false;
        //ShowFloatedWindows( false );
    }

    mCheckFocusWhenIdle = false;

    event.Skip();
}

void wxFrameLayout::GetPaneProperties( cbCommonPaneProperties& props, int alignment )
{
    props = mPanes[alignment]->mProps;
}

void wxFrameLayout::SetPaneProperties( const cbCommonPaneProperties& props, int paneMask )
{
    int i;
    for ( i = 0; i != MAX_PANES; ++i )
    {
        if ( mPanes[i]->MatchesMask( paneMask ) )
            mPanes[i]->mProps = props;
    }
}

void wxFrameLayout::SetMargins( int top, int bottom, int left, int right,
                                int paneMask )
{
    int i;
    for ( i = 0; i != MAX_PANES; ++i )
    {
        cbDockPane& pane = *mPanes[i];

        if ( pane.MatchesMask( paneMask ) )
        {
            pane.mTopMargin = top;
            pane.mBottomMargin = bottom;
            pane.mLeftMargin = left;
            pane.mRightMargin = right;
        }
    }
}

void wxFrameLayout::SetPaneBackground( const wxColour& colour )
{
    mBorderPen.SetColour( colour );
}

void wxFrameLayout::RefreshNow( bool recalcLayout )
{
    if ( recalcLayout )
        RecalcLayout( true );

    if ( mpFrame )
        mpFrame->Refresh();
}

/*** plugin-related methods ***/

void wxFrameLayout::FirePluginEvent( cbPluginEvent& event )
{
    // check state of input capture, before processing the event

    if ( mpCaputesInput )
    {
        bool isInputEvt = true;
#if wxCHECK_VERSION(2,3,0)
        if ( event.GetEventType() != cbEVT_PL_LEFT_DOWN &&
             event.GetEventType() != cbEVT_PL_LEFT_UP &&
             event.GetEventType() != cbEVT_PL_RIGHT_DOWN &&
             event.GetEventType() != cbEVT_PL_RIGHT_UP &&
             event.GetEventType() != cbEVT_PL_MOTION )
            isInputEvt = false;
#else
        switch ( event.m_eventType )
        {
            case cbEVT_PL_LEFT_DOWN  : break;
            case cbEVT_PL_LEFT_UP    : break;
            case cbEVT_PL_RIGHT_DOWN : break;
            case cbEVT_PL_RIGHT_UP   : break;
            case cbEVT_PL_MOTION     : break;

            default : isInputEvt = false; break;
        }
#endif  // #if wxCHECK_VERSION(2,3,0)

        if ( isInputEvt )
        {
            mpCaputesInput->ProcessEvent( event );
            return;
        }
    }

    GetTopPlugin().ProcessEvent( event );
}

void wxFrameLayout::CaptureEventsForPlugin ( cbPluginBase* pPlugin )
{
    // cannot capture events for more than one plugin at a time
    wxASSERT( mpCaputesInput == NULL );

    mpCaputesInput = pPlugin;

}

void wxFrameLayout::ReleaseEventsFromPlugin( cbPluginBase* WXUNUSED(pPlugin) )
{
    // events should be captured first
    wxASSERT( mpCaputesInput != NULL );

    mpCaputesInput = NULL;
}

void wxFrameLayout::CaptureEventsForPane( cbDockPane* toPane )
{
    // cannot capture events twice (without releasing)
    wxASSERT( mpPaneInFocus == NULL );

    mpFrame->CaptureMouse();

    mpPaneInFocus = toPane;
}

void wxFrameLayout::ReleaseEventsFromPane( cbDockPane* WXUNUSED(fromPane) )
{
    // cannot release events without capturing them
    wxASSERT( mpPaneInFocus != NULL );

    mpFrame->ReleaseMouse();

    mpPaneInFocus = NULL;
}

cbPluginBase& wxFrameLayout::GetTopPlugin()
{
    if ( !mpTopPlugin )

        PushDefaultPlugins(); // automatic configuration

    return *mpTopPlugin;
}

void wxFrameLayout::SetTopPlugin( cbPluginBase* pPlugin )
{
    mpTopPlugin = pPlugin;
}

bool wxFrameLayout::HasTopPlugin()
{
    return ( mpTopPlugin != NULL );
}

void wxFrameLayout::PushPlugin( cbPluginBase* pPlugin )
{
    if ( !mpTopPlugin )

        mpTopPlugin = pPlugin;
    else
    {
        pPlugin->SetNextHandler( mpTopPlugin );

        mpTopPlugin->SetPreviousHandler( pPlugin );

        mpTopPlugin = pPlugin;
    }

    mpTopPlugin->OnInitPlugin(); // notification
}

void wxFrameLayout::PopPlugin()
{
    wxASSERT( mpTopPlugin ); // DBG:: at least one plugin should be present

    cbPluginBase* pPopped = mpTopPlugin;

    mpTopPlugin = (cbPluginBase*)mpTopPlugin->GetNextHandler();

    delete pPopped;
}

void wxFrameLayout::PopAllPlugins()
{
    while( mpTopPlugin ) PopPlugin();
}

void wxFrameLayout::PushDefaultPlugins()
{
    // FIXME:: to much of the stuff for the default...

    AddPlugin( CLASSINFO( cbRowLayoutPlugin       ) );
    AddPlugin( CLASSINFO( cbBarDragPlugin         ) );
    AddPlugin( CLASSINFO( cbPaneDrawPlugin ) );
}

void wxFrameLayout::AddPlugin( wxClassInfo* pPlInfo, int paneMask )
{
    if ( FindPlugin ( pPlInfo ) ) return; // same type of plugin cannot be added twice

    cbPluginBase* pObj = (cbPluginBase*)pPlInfo->CreateObject();

    wxASSERT(pObj); // DBG:: plugin's class should be dynamic

    pObj->mPaneMask = paneMask;
    pObj->mpLayout  = this;

    PushPlugin( pObj );
}

void wxFrameLayout::AddPluginBefore( wxClassInfo* pNextPlInfo, wxClassInfo* pPlInfo,
                                       int paneMask )
{
    wxASSERT( pNextPlInfo != pPlInfo ); // DBG:: no sense

    cbPluginBase* pNextPl = FindPlugin( pNextPlInfo );

    if ( !pNextPl )
    {
        AddPlugin( pPlInfo, paneMask );

        return;
    }

    // remove existing one if present

    cbPluginBase* pExistingPl = FindPlugin( pPlInfo );

    if ( pExistingPl ) RemovePlugin( pPlInfo );

    // create an instance

    cbPluginBase* pNewPl = (cbPluginBase*)pPlInfo->CreateObject();

    wxASSERT(pNewPl); // DBG:: plugin's class should be dynamic

    // insert it to the chain

    if ( pNextPl->GetPreviousHandler() )
        pNextPl->GetPreviousHandler()->SetNextHandler( pNewPl );
    else
        mpTopPlugin = pNewPl;

    pNewPl->SetNextHandler( pNextPl );

    pNewPl->SetPreviousHandler( pNextPl->GetPreviousHandler() );

    pNextPl->SetPreviousHandler( pNewPl );

    // set it up

    pNewPl->mPaneMask = paneMask;
    pNewPl->mpLayout  = this;

    pNewPl->OnInitPlugin();
}

void wxFrameLayout::RemovePlugin( wxClassInfo* pPlInfo )
{
    cbPluginBase* pPlugin = FindPlugin( pPlInfo );

    if ( !pPlugin ) return; // it's OK to remove not-existing plugin ;-)

    if ( pPlugin->GetPreviousHandler() == NULL )

        mpTopPlugin = (cbPluginBase*)pPlugin->GetNextHandler();

    delete pPlugin;
}

cbPluginBase* wxFrameLayout::FindPlugin( wxClassInfo* pPlInfo )
{
    cbPluginBase *pCur = mpTopPlugin;

    while( pCur )
    {
        // NOTE:: it might appear useful matching plugin
        //        classes "polymorphically":

        if ( pCur->GetClassInfo()->IsKindOf( pPlInfo ) )

            return pCur;

        pCur = (cbPluginBase*)pCur->GetNextHandler();
    }

    return NULL;
}

/***** Implementation for class cbUpdateMgrData *****/

IMPLEMENT_DYNAMIC_CLASS( cbUpdateMgrData, wxObject )

cbUpdateMgrData::cbUpdateMgrData()

    : mPrevBounds( -1,-1,0,0 ),
      mIsDirty( true ),           // inidicate initial change
      mpCustomData(0)
{}

void cbUpdateMgrData::StoreItemState( const wxRect& boundsInParent )
{
    mPrevBounds = boundsInParent;
}

void cbUpdateMgrData::SetDirty( bool isDirty )
{
    mIsDirty = isDirty;
}

void cbUpdateMgrData::SetCustomData( wxObject* pCustomData )
{
    mpCustomData = pCustomData;
}

/***** Implementation for class cbDockPane *****/

void wxBarIterator::Reset()
{
    mpRow = ( mpRows->Count() ) ? (*mpRows)[0] : NULL;
    mpBar = NULL;
}

wxBarIterator::wxBarIterator( RowArrayT& rows )

    : mpRows( &rows ),
      mpRow ( NULL  ),
      mpBar ( NULL  )
{
    Reset();
}

bool wxBarIterator::Next()
{
    if ( mpRow )
    {
        if ( mpBar )
            mpBar = mpBar->mpNext;
        else
        {
            if ( mpRow->mBars.GetCount() == 0 )
            {
                return false;
            }

            mpBar = mpRow->mBars[0];
        }

        if ( !mpBar )
        {
            // skip to the next row

            mpRow = mpRow->mpNext;

            if ( mpRow )
                mpBar = mpRow->mBars[0];
            else
                return false;
        }

        return true;
    }
    else
        return false;
}

cbBarInfo& wxBarIterator::BarInfo()
{
    return *mpBar;
}

cbRowInfo& wxBarIterator::RowInfo()
{
    return *mpRow;
}

/***** Implementation for class cbBarDimHandlerBase *****/

IMPLEMENT_ABSTRACT_CLASS( cbBarDimHandlerBase, wxObject )

cbBarDimHandlerBase::cbBarDimHandlerBase()
    : mRefCount(0)
{}

void cbBarDimHandlerBase::AddRef()
{
    ++mRefCount;
}

void cbBarDimHandlerBase::RemoveRef()
{
    if ( --mRefCount <= 0 ) delete this;
}

/***** Implementation for class cbDimInfo *****/

IMPLEMENT_DYNAMIC_CLASS( cbDimInfo, wxObject )

cbDimInfo::cbDimInfo()

    : mVertGap ( 0 ),
      mHorizGap( 0 ),

      mIsFixed(true),
      mpHandler( NULL )
{
    size_t i;
    for ( i = 0; i != MAX_BAR_STATES; ++i )
    {
        mSizes[i].x = 20;
        mSizes[i].y = 20;

        mBounds[i] = wxRect( -1,-1,-1,-1 );
    }
}

cbDimInfo::cbDimInfo( cbBarDimHandlerBase* pDimHandler,
                      bool                 isFixed  )

    : mVertGap ( 0 ),
      mHorizGap( 0 ),
      mIsFixed ( isFixed  ),

      mpHandler( pDimHandler )
{
    if ( mpHandler )
    {
        // int vtad = *((int*)mpHandler);
        mpHandler->AddRef();
    }

    size_t i;
    for ( i = 0; i != MAX_BAR_STATES; ++i )
    {
        mSizes[i].x = -1;
        mSizes[i].y = -1;

        mBounds[i] = wxRect( -1,-1,-1,-1 );
    }
}

cbDimInfo::cbDimInfo( int dh_x, int dh_y,
                      int dv_x, int dv_y,
                      int f_x,  int f_y,

                      bool isFixed,
                      int horizGap,
                      int vertGap,

                      cbBarDimHandlerBase* pDimHandler
                    )
    : mVertGap  ( vertGap   ),
      mHorizGap ( horizGap  ),
      mIsFixed  ( isFixed   ),
      mpHandler( pDimHandler )
{
    if ( mpHandler )
    {
        // int vtad = *((int*)mpHandler);
        mpHandler->AddRef();
    }

    mSizes[wxCBAR_DOCKED_HORIZONTALLY].x = dh_x;
    mSizes[wxCBAR_DOCKED_HORIZONTALLY].y = dh_y;
    mSizes[wxCBAR_DOCKED_VERTICALLY  ].x = dv_x;
    mSizes[wxCBAR_DOCKED_VERTICALLY  ].y = dv_y;
    mSizes[wxCBAR_FLOATING           ].x = f_x;
    mSizes[wxCBAR_FLOATING           ].y = f_y;

    size_t i;
    for ( i = 0; i != MAX_BAR_STATES; ++i )
        mBounds[i] = wxRect( -1,-1,-1,-1 );
}

cbDimInfo::cbDimInfo( int x, int y,
                      bool isFixed, int gap,
                      cbBarDimHandlerBase* pDimHandler)
  : mVertGap  ( gap ),
    mHorizGap ( gap ),
    mIsFixed  ( isFixed ),
    mpHandler( pDimHandler )
{
    if ( mpHandler )
    {
        // int vtad = *((int*)mpHandler);
        mpHandler->AddRef();
    }

    mSizes[wxCBAR_DOCKED_HORIZONTALLY].x = x;
    mSizes[wxCBAR_DOCKED_HORIZONTALLY].y = y;
    mSizes[wxCBAR_DOCKED_VERTICALLY  ].x = x;
    mSizes[wxCBAR_DOCKED_VERTICALLY  ].y = y;
    mSizes[wxCBAR_FLOATING           ].x = x;
    mSizes[wxCBAR_FLOATING           ].y = y;

    size_t i;
    for ( i = 0; i != MAX_BAR_STATES; ++i )
        mBounds[i] = wxRect( -1,-1,-1,-1 );
}

cbDimInfo::~cbDimInfo()
{
    if ( mpHandler )

        mpHandler->RemoveRef();
}

const cbDimInfo& cbDimInfo::operator=( const cbDimInfo& other )
{
    if ( this == &other )
        return *this;

    int i;
    for ( i = 0; i != MAX_BAR_STATES; ++i )
        mSizes[i] = other.mSizes[i];

    mIsFixed  = other.mIsFixed;
    mpHandler = other.mpHandler;

    mVertGap  = other.mVertGap;
    mHorizGap = other.mHorizGap;

    if ( mpHandler )

        mpHandler->AddRef();

    return *this;
}

/***** Implementation for structure cbCommonPaneProperties *****/

IMPLEMENT_DYNAMIC_CLASS( cbCommonPaneProperties, wxObject )

cbCommonPaneProperties::cbCommonPaneProperties(void)

    : mRealTimeUpdatesOn    ( true  ),
      mOutOfPaneDragOn      ( true  ),
      mExactDockPredictionOn( false ),
      mNonDestructFrictionOn( false ),
      mShow3DPaneBorderOn   ( true  ),
      mBarFloatingOn        ( false ),
      mRowProportionsOn     ( false ),
      mColProportionsOn     ( true  ),
      mBarCollapseIconsOn   ( false ),
      mBarDragHintsOn       ( false ),

      mMinCBarDim( 16, 16 ),
      mResizeHandleSize( 4 )
{}

cbCommonPaneProperties::cbCommonPaneProperties(const cbCommonPaneProperties& props)

    : wxObject(),
      mRealTimeUpdatesOn    (props.mRealTimeUpdatesOn),
      mOutOfPaneDragOn      (props.mOutOfPaneDragOn),
      mExactDockPredictionOn(props.mExactDockPredictionOn),
      mNonDestructFrictionOn(props.mNonDestructFrictionOn),
      mShow3DPaneBorderOn   (props.mShow3DPaneBorderOn),
      mBarFloatingOn        (props.mBarFloatingOn),
      mRowProportionsOn     (props.mRowProportionsOn),
      mColProportionsOn     (props.mColProportionsOn),
      mBarCollapseIconsOn   (props.mBarCollapseIconsOn),
      mBarDragHintsOn       (props.mBarDragHintsOn),

      mMinCBarDim(props.mMinCBarDim),
      mResizeHandleSize(props.mResizeHandleSize)
{}

cbCommonPaneProperties& cbCommonPaneProperties::operator=(const cbCommonPaneProperties& props)
{
    mRealTimeUpdatesOn     = props.mRealTimeUpdatesOn;
    mOutOfPaneDragOn       = props.mOutOfPaneDragOn;
    mExactDockPredictionOn = props.mExactDockPredictionOn;
    mNonDestructFrictionOn = props.mNonDestructFrictionOn;
    mShow3DPaneBorderOn    = props.mShow3DPaneBorderOn;
    mBarFloatingOn         = props.mBarFloatingOn;
    mRowProportionsOn      = props.mRowProportionsOn;
    mColProportionsOn      = props.mColProportionsOn;
    mBarCollapseIconsOn    = props.mBarCollapseIconsOn;
    mBarDragHintsOn        = props.mBarDragHintsOn;

    mMinCBarDim            = props.mMinCBarDim;
    mResizeHandleSize      = props.mResizeHandleSize;

    return *this;
}

/***** Implementation for class cbRowInfo *****/

IMPLEMENT_DYNAMIC_CLASS( cbRowInfo, wxObject )

cbRowInfo::cbRowInfo(void)

    : mNotFixedBarsCnt( false ),
      mpNext          ( NULL ),
      mpPrev          ( NULL ),
      mpExpandedBar   ( NULL )
{}

cbRowInfo::~cbRowInfo()
{
    // nothing! all bars are removed using global bar
    // list in wxFrameLayout class
}

/***** Implementation for class cbBarInfo *****/

IMPLEMENT_DYNAMIC_CLASS( cbBarInfo, wxObject )

cbBarInfo::cbBarInfo(void)

    : mpRow( NULL ),
      mFloatingOn( true ),
      mpNext( NULL ),
      mpPrev( NULL )
{}

cbBarInfo::~cbBarInfo()
{
    // nothing
}

/***** Implementation for class cbDockPane *****/

IMPLEMENT_DYNAMIC_CLASS( cbDockPane, wxObject )

// FIXME:: how to eliminate these cut&pasted constructors?

cbDockPane::cbDockPane(void)
    : mLeftMargin  ( 1 ),
      mRightMargin ( 1 ),
      mTopMargin   ( 1 ),
      mBottomMargin( 1 ),
      mPaneWidth ( 32768     ), // fake-up very large pane dims,
                                // since the real dimensions of the pane may not
                                // be known, while inserting bars initially
      mPaneHeight( 32768     ),
      mAlignment ( -1   ),
      mpLayout   ( 0 ),
      mpStoredRow( NULL )
{}

cbDockPane::cbDockPane( int alignment, wxFrameLayout* pPanel )

    :  mLeftMargin  ( 1 ),
      mRightMargin ( 1 ),
      mTopMargin   ( 1 ),
      mBottomMargin( 1 ),
      mPaneWidth ( 32768     ), // fake-up very large pane dims,
                                // since the real dimensions of the pane may not
                                // be known, while inserting bars initially
      mPaneHeight( 32768     ),
      mAlignment ( alignment ),
      mpLayout   ( pPanel    ),
      mpStoredRow( NULL )
{}

cbDockPane::~cbDockPane()
{
    size_t i;
    for ( i = 0; i != mRows.Count(); ++i )
        delete mRows[i];

    WX_CLEAR_LIST(wxList,mRowShapeData)

    // NOTE:: control bar infromation structures are cleaned-up
    //        in wxFrameLayout's destructor, using global control-bar list
}

void cbDockPane::SetMargins( int top, int bottom, int left, int right )
{
    mTopMargin    = top;
    mBottomMargin = bottom;
    mLeftMargin   = left;
    mRightMargin  = right;
}

/*** helpers of cbDockPane ***/

void cbDockPane::PaintBarDecorations( cbBarInfo* pBar, wxDC& dc )
{
    cbDrawBarDecorEvent evt( pBar, dc, this );

    mpLayout->FirePluginEvent( evt );
}

void cbDockPane::PaintBarHandles( cbBarInfo* pBar, wxDC& dc )
{
    cbDrawBarHandlesEvent evt( pBar, dc, this );

    mpLayout->FirePluginEvent( evt );
}

void cbDockPane::PaintBar( cbBarInfo* pBar, wxDC& dc )
{
    PaintBarDecorations( pBar, dc );
    PaintBarHandles( pBar, dc );
}

void cbDockPane::PaintRowHandles( cbRowInfo* pRow, wxDC& dc )
{
    cbDrawRowHandlesEvent evt( pRow, dc, this );

    mpLayout->FirePluginEvent( evt );

    cbDrawRowDecorEvent evt1( pRow, dc, this );

    mpLayout->FirePluginEvent( evt1 );
}

void cbDockPane::PaintRowBackground ( cbRowInfo* pRow, wxDC& dc )
{
    cbDrawRowBkGroundEvent evt( pRow, dc, this );

    mpLayout->FirePluginEvent( evt );
}

void cbDockPane::PaintRowDecorations( cbRowInfo* pRow, wxDC& dc )
{
    size_t i;

    // decorations first
    for ( i = 0; i != pRow->mBars.Count(); ++i )

        PaintBarDecorations( pRow->mBars[i], dc );

    // then handles if present
    for ( i = 0; i != pRow->mBars.Count(); ++i )

        PaintBarHandles( pRow->mBars[i], dc );
}

void cbDockPane::PaintRow( cbRowInfo* pRow, wxDC& dc )
{
    PaintRowBackground ( pRow, dc );
    PaintRowDecorations( pRow, dc );
    PaintRowHandles    ( pRow, dc );
}

void cbDockPane::PaintPaneBackground( wxDC& dc )
{
    cbDrawPaneBkGroundEvent evt( dc, this );

    mpLayout->FirePluginEvent( evt );
}

void cbDockPane::PaintPaneDecorations( wxDC& dc )
{
    cbDrawPaneDecorEvent evt( dc, this );

    mpLayout->FirePluginEvent( evt );
}

void cbDockPane::PaintPane( wxDC& dc )
{
    size_t i;

    PaintPaneBackground( dc );

    // first decorations
    for ( i = 0; i != mRows.Count(); ++i )
    {
        PaintRowBackground( mRows[i], dc );
        PaintRowDecorations( mRows[i], dc );
    }

    // than handles
    for ( i = 0; i != mRows.Count(); ++i )
        PaintRowHandles( mRows[i], dc );

    // and finally
    PaintPaneDecorations( dc );
}

void cbDockPane::SizeBar( cbBarInfo* pBar )
{
    cbSizeBarWndEvent evt( pBar, this );

    mpLayout->FirePluginEvent( evt );
    return;
}

void cbDockPane::SizeRowObjects( cbRowInfo* pRow )
{
    size_t i;
    for ( i = 0; i != pRow->mBars.Count(); ++i )
        SizeBar( pRow->mBars[i] );
}

void cbDockPane::SizePaneObjects()
{
    size_t i;
    for ( i = 0; i != mRows.Count(); ++i )
        SizeRowObjects( mRows[i] );
}

wxDC* cbDockPane::StartDrawInArea( const wxRect& area )
{
    wxDC* pDc = 0;

    cbStartDrawInAreaEvent evt( area, &pDc, this );

    mpLayout->FirePluginEvent( evt );

    return pDc;
}

void cbDockPane::FinishDrawInArea( const wxRect& area )
{
    cbFinishDrawInAreaEvent evt( area, this );

    mpLayout->FirePluginEvent( evt );
}

bool cbDockPane::IsFixedSize( cbBarInfo* pInfo )
{
    return ( pInfo->mDimInfo.mIsFixed );
}

int cbDockPane::GetNotFixedBarsCount( cbRowInfo* pRow )
{
    int cnt = 0;

    size_t i;
    for ( i = 0; i != pRow->mBars.Count(); ++i )
    {
        if ( !pRow->mBars[i]->IsFixed() )
            ++cnt;
    }

    return cnt;
}

void cbDockPane::RemoveBar( cbBarInfo* pBar )
{
    bool needsRestoring = mProps.mNonDestructFrictionOn &&
                          mpStoredRow == pBar->mpRow;

    cbRemoveBarEvent evt( pBar, this );

    mpLayout->FirePluginEvent( evt );

    if ( needsRestoring )
    {
        SetRowShapeData( mpStoredRow, &mRowShapeData );

        mpStoredRow = NULL;
    }
}

void cbDockPane::SyncRowFlags( cbRowInfo* pRow )
{
    // setup mHasOnlyFixedBars flag for the row information
    pRow->mHasOnlyFixedBars = true;

    pRow->mNotFixedBarsCnt = 0;

    size_t i;
    for ( i = 0; i != pRow->mBars.Count(); ++i )
    {
        cbBarInfo& bar = *pRow->mBars[i];

        bar.mpRow = pRow;

        if ( !bar.IsFixed() )
        {
            pRow->mHasOnlyFixedBars = false;
            ++pRow->mNotFixedBarsCnt;
        }
    }
}

void cbDockPane::FrameToPane( int* x, int* y )
{
    *x -= mLeftMargin;
    *y -= mTopMargin;

    if ( mAlignment == FL_ALIGN_TOP ||
         mAlignment == FL_ALIGN_BOTTOM
       )
    {
        *x -= mBoundsInParent.x;
        *y -= mBoundsInParent.y;
    }
    else
    {
        int rx = *x, ry = *y;

        *x = ry - mBoundsInParent.y;

        *y = rx - mBoundsInParent.x;
    }
}

void cbDockPane::PaneToFrame( int* x, int* y )
{
    if ( mAlignment == FL_ALIGN_TOP ||
         mAlignment == FL_ALIGN_BOTTOM
       )
    {
        *x += mBoundsInParent.x;
        *y += mBoundsInParent.y;
    }
    else
    {
        int rx = *x, ry = *y;

        *x = ry + mBoundsInParent.x;

        *y = mBoundsInParent.y + rx;
    }

    *x += mLeftMargin;
    *y += mTopMargin;
}

void cbDockPane::FrameToPane( wxRect* pRect )
{
    wxPoint upperLeft ( pRect->x, pRect->y );
    wxPoint lowerRight( pRect->x + pRect->width,
                        pRect->y + pRect->height );

    FrameToPane( &upperLeft.x,  &upperLeft.y  );
    FrameToPane( &lowerRight.x, &lowerRight.y );

    pRect->x = wxMin(upperLeft.x,lowerRight.x);
    pRect->y = wxMin(upperLeft.y,lowerRight.y);

    pRect->width  = abs( lowerRight.x - upperLeft.x );
    pRect->height = abs( lowerRight.y - upperLeft.y );
}

void cbDockPane::PaneToFrame( wxRect* pRect )
{
    wxPoint upperLeft ( pRect->x, pRect->y );
    wxPoint lowerRight( pRect->x + pRect->width,
                        pRect->y + pRect->height );

    PaneToFrame( &upperLeft.x,  &upperLeft.y  );
    PaneToFrame( &lowerRight.x, &lowerRight.y );

    //wxRect newRect = wxRect( upperLeft, lowerRight );

    pRect->x = wxMin(upperLeft.x,lowerRight.x);
    pRect->y = wxMin(upperLeft.y,lowerRight.y);

    pRect->width  = abs( lowerRight.x - upperLeft.x );
    pRect->height = abs( lowerRight.y - upperLeft.y );
}

int cbDockPane::GetRowAt( int paneY )
{
    if ( paneY < 0 )
        return -1;

    int curY = 0;

    size_t i = 0;

    for ( ; i != mRows.Count(); ++i )
    {
        int rowHeight = mRows[i]->mRowHeight;

        int third = rowHeight/3;

        if ( paneY >= curY && paneY < curY + third )
            return i-1;

        if ( paneY >= curY + third && paneY < curY + rowHeight - third )
            return i;

        curY += rowHeight;
    }

    return i;
}

int cbDockPane::GetRowAt( int upperY, int lowerY )
{
    /*
    // OLD STUFF::
    int range    = lowerY - upperY;
    int oneThird = range / 3;

    wxNode* pRow = mRows.GetFirst();
    int row = 0;
    int curY = 0;

    if ( lowerY <= 0 ) return -1;

    while( pRow )
    {
        int rowHeight = GetRowHeight( (wxList*)pRow->GetData() );

        if ( upperY >= curY &&
             lowerY < curY ) return row;

        if ( upperY <= curY &&
             lowerY >= curY &&
             curY - upperY >= oneThird ) return row-1;

        if ( ( upperY <  curY + rowHeight &&
               lowerY >= curY + rowHeight &&
               curY + rowHeight - lowerY >= oneThird )
           )
            return row+1;

        if ( lowerY <= curY + rowHeight ) return row;

        ++row;
        curY += rowHeight;
        pRow = pRow->GetNext();
    }
    */

    int mid = upperY + (lowerY - upperY)/2;

    if ( mid < 0 )
        return -1;

    int curY = 0;
    size_t i = 0;

    for ( ; i != mRows.Count(); ++i )
    {
        int rowHeight = mRows[i]->mRowHeight;

        if ( mid >= curY && mid < curY + rowHeight ) return i;

        curY += rowHeight;
    }

    return i;
}

int cbDockPane::GetRowY( cbRowInfo* pRow )
{
    int curY = 0;

    size_t i;
    for ( i = 0; i != mRows.Count(); ++i )
    {
        if ( mRows[i] == pRow )
            break;

        curY += mRows[i]->mRowHeight;
    }

    return curY;
}

bool cbDockPane::HasNotFixedRowsAbove( cbRowInfo* pRow )
{
    while ( pRow->mpPrev )
    {
        pRow = pRow->mpPrev;

        if ( pRow->mHasOnlyFixedBars )

            return true;
    }

    return false;
}

bool cbDockPane::HasNotFixedRowsBelow( cbRowInfo* pRow )
{
    while( pRow->mpNext )
    {
        pRow = pRow->mpNext;

        if ( pRow->mHasOnlyFixedBars )

            return true;
    }

    return false;
}

bool cbDockPane::HasNotFixedBarsLeft( cbBarInfo* pBar )
{
    while( pBar->mpPrev )
    {
        pBar = pBar->mpPrev;

        if ( pBar->IsFixed() )

            return true;
    }

    return false;
}

bool cbDockPane::HasNotFixedBarsRight( cbBarInfo* pBar )
{
    while( pBar->mpNext )
    {
        pBar = pBar->mpNext;

        if ( pBar->IsFixed() )

            return true;
    }

    return false;
}

void cbDockPane::CalcLengthRatios( cbRowInfo* pInRow )
{
    size_t i;

    int totalWidth = 0;

    // calc current-maximal-total-length of all maximized bars

    for ( i = 0; i != pInRow->mBars.GetCount(); ++i )
    {
        cbBarInfo& bar = *pInRow->mBars[i];

        if ( !bar.IsFixed() )
            totalWidth += bar.mBounds.width;
    }

    // set up percentages of occupied space for each maximized bar

    for ( i = 0; i != pInRow->mBars.Count(); ++i )
    {
        cbBarInfo& bar = *pInRow->mBars[i];

        if ( !bar.IsFixed() )
            bar.mLenRatio = double(bar.mBounds.width)/double(totalWidth);
    }
}

void cbDockPane::RecalcRowLayout( cbRowInfo* pRow )
{
    cbLayoutRowEvent evt( pRow, this );

    mpLayout->FirePluginEvent( evt );
}

void cbDockPane::ExpandBar( cbBarInfo* pBar )
{
    mpLayout->GetUpdatesManager().OnStartChanges();

    if ( !pBar->mpRow->mpExpandedBar )
    {
        // save ratios only when there arent any bars expanded yet

        cbArrayFloat& ratios = pBar->mpRow->mSavedRatios;

        ratios.Clear();
        ratios.Alloc( pBar->mpRow->mNotFixedBarsCnt );

        cbBarInfo* pCur = pBar->mpRow->mBars[0];

        while( pCur )
        {
            if ( !pCur->IsFixed() )
            {
                ratios.Add( 0.0 );
                ratios[ ratios.GetCount() - 1 ] = pCur->mLenRatio;
            }

            pCur = pCur->mpNext;
        }
    }

    cbBarInfo* pCur = pBar->mpRow->mBars[0];

    while( pCur )
    {
        pCur->mLenRatio = 0.0; // minimize the rest

        pCur = pCur->mpNext;
    }

    pBar->mLenRatio     = 1.0; // 100%
    pBar->mBounds.width = 0;

    pBar->mpRow->mpExpandedBar = pBar;

    mpLayout->RecalcLayout( false );

    mpLayout->GetUpdatesManager().OnFinishChanges();
    mpLayout->GetUpdatesManager().UpdateNow();
}

void cbDockPane::ContractBar( cbBarInfo* pBar )
{
    mpLayout->GetUpdatesManager().OnStartChanges();

    // FIXME: What's the purpose of this???
    // double ratio = 1.0/ double( pBar->mpRow->mNotFixedBarsCnt );

    // restore ratios which were present before expansion

    cbBarInfo* pCur = pBar->mpRow->mBars[0];

    cbArrayFloat& ratios = pBar->mpRow->mSavedRatios;

    size_t i = 0;

    while( pCur )
    {
        if ( !pCur->IsFixed() )
        {
            pCur->mLenRatio = ratios[i];
            ++i;
        }

        pCur = pCur->mpNext;
    }

    ratios.Clear();
    ratios.Shrink();

    pBar->mpRow->mpExpandedBar = NULL;

    mpLayout->RecalcLayout( false );

    mpLayout->GetUpdatesManager().OnFinishChanges();
    mpLayout->GetUpdatesManager().UpdateNow();
}

void cbDockPane::InitLinksForRow( cbRowInfo* pRow )
{
    size_t i;
    for ( i = 0; i != pRow->mBars.Count(); ++i )
    {
        cbBarInfo& bar = *pRow->mBars[i];

        if ( i == 0 )
            bar.mpPrev = NULL;
        else
            bar.mpPrev = pRow->mBars[i-1];

        if ( i == pRow->mBars.Count() - 1 )
            bar.mpNext = NULL;
        else
            bar.mpNext = pRow->mBars[i+1];
    }
}

void cbDockPane::InitLinksForRows()
{
    size_t i;
    for ( i = 0; i != mRows.Count(); ++i )
    {
        cbRowInfo& row = *mRows[i];

        if ( i == 0 )
            row.mpPrev = NULL;
        else
            row.mpPrev = mRows[i-1];

        if ( i == mRows.Count() - 1 )
            row.mpNext = NULL;
        else
            row.mpNext = mRows[i+1];
    }
}

void cbDockPane::DoInsertBar( cbBarInfo* pBar, int rowNo )
{
    cbRowInfo* pRow;

    if ( rowNo == -1 || rowNo >= (int)mRows.Count() )
    {
        pRow = new cbRowInfo();

        if ( rowNo == -1 && mRows.Count() )

            mRows.Insert( pRow, 0 );
        else
            mRows.Add( pRow );

        InitLinksForRows();
    }
    else
    {
        pRow = mRows[rowNo];

        if ( mProps.mNonDestructFrictionOn == true )
        {
            // store original shape of the row (before the bar is inserted)

            mpStoredRow = pRow;

            GetRowShapeData( mpStoredRow, &mRowShapeData );
        }
    }

    if ( pRow->mBars.Count() )

        pRow->mpExpandedBar = NULL;

    cbInsertBarEvent insEvt( pBar, pRow, this );

    mpLayout->FirePluginEvent( insEvt );

    mpLayout->GetUpdatesManager().OnRowWillChange( pRow, this );
}

void cbDockPane::InsertBar( cbBarInfo* pBarInfo, const wxRect& atRect )
{
    wxRect rect = atRect;
    FrameToPane( &rect );

    pBarInfo->mBounds.x      = rect.x;
    pBarInfo->mBounds.width  = rect.width;
    pBarInfo->mBounds.height = rect.height;

    int row = GetRowAt( rect.y, rect.y + rect.height );

    DoInsertBar( pBarInfo, row );
}

void cbDockPane::InsertBar( cbBarInfo* pBar, cbRowInfo* pIntoRow )
{
    cbInsertBarEvent insEvt( pBar, pIntoRow, this );

    mpLayout->FirePluginEvent( insEvt );

    mpLayout->GetUpdatesManager().OnRowWillChange( pIntoRow, this );
}

void cbDockPane::InsertBar( cbBarInfo* pBarInfo )
{
    // set transient properties

    pBarInfo->mpRow           = NULL;
    pBarInfo->mHasLeftHandle  = false;
    pBarInfo->mHasRightHandle = false;
    pBarInfo->mLenRatio       = 0.0;

    // set preferred bar dimensions, according to the state in which
    // the bar is being inserted

    pBarInfo->mBounds.width   = pBarInfo->mDimInfo.mSizes[ pBarInfo->mState ].x;
    pBarInfo->mBounds.height  = pBarInfo->mDimInfo.mSizes[ pBarInfo->mState ].y;

    DoInsertBar( pBarInfo, pBarInfo->mRowNo );
}

void cbDockPane::RemoveRow( cbRowInfo* pRow )
{
    size_t i;
    // first, hide all bar-windows in the removed row
    for ( i = 0; i != pRow->mBars.Count(); ++i )
    {
        if ( pRow->mBars[i]->mpBarWnd )
            pRow->mBars[i]->mpBarWnd->Show( false );
    }

    mRows.Remove( pRow );

    pRow->mUMgrData.SetDirty(true);
}

void cbDockPane::InsertRow( cbRowInfo* pRow, cbRowInfo* pBeforeRow )
{
    if ( !pBeforeRow )

        mRows.Add( pRow );
    else
        mRows.Insert( pRow, mRows.Index( pBeforeRow ) );

    InitLinksForRows();

    pRow->mUMgrData.SetDirty(true);

    size_t i;
    for ( i = 0; i != pRow->mBars.Count(); ++i )
        pRow->mBars[i]->mUMgrData.SetDirty( true );

    SyncRowFlags( pRow );
}

void cbDockPane::SetPaneWidth(int width)
{
    if ( IsHorizontal() )
        mPaneWidth = width - mLeftMargin - mRightMargin;
    else
        mPaneWidth = width - mTopMargin - mBottomMargin;
}


void cbDockPane::SetBoundsInParent( const wxRect& rect )
{
    mBoundsInParent = rect;

    // set pane dimensions in local coordinates

    if ( IsHorizontal() )
    {
        mPaneWidth  = mBoundsInParent.width  - ( mRightMargin + mLeftMargin   );
        mPaneHeight = mBoundsInParent.height - ( mTopMargin   + mBottomMargin );
    }
    else
    {
        mPaneWidth  = mBoundsInParent.height - ( mTopMargin   + mBottomMargin );
        mPaneHeight = mBoundsInParent.width  - ( mRightMargin + mLeftMargin   );
    }

    // convert bounding rectangles of all pane items into parent frame's coordinates

    wxBarIterator i( mRows );

    wxRect noMarginsRect = mBoundsInParent;

    noMarginsRect.x      += mLeftMargin;
    noMarginsRect.y      += mTopMargin;
    noMarginsRect.width  -= ( mLeftMargin + mRightMargin  );
    noMarginsRect.height -= ( mTopMargin  + mBottomMargin );

    // hide the whole pane, if it's bounds became reverted (i.e. pane vanished)

    if ( mBoundsInParent.width < 0 ||
         mBoundsInParent.height < 0 )

         hide_rect( mBoundsInParent );

    if ( noMarginsRect.width < 0 ||
         noMarginsRect.height < 0 )

        hide_rect( noMarginsRect   );

    // calculate mBoundsInParent for each item in the pane

    while( i.Next() )
    {
        cbBarInfo& bar = i.BarInfo();

        cbRowInfo* pRowInfo = bar.mpRow;

        // set up row info, if this is first bar in the row

        if ( pRowInfo && bar.mpPrev == NULL )
        {
            pRowInfo->mBoundsInParent.y      = pRowInfo->mRowY;
            pRowInfo->mBoundsInParent.x      = 0;
            pRowInfo->mBoundsInParent.width  = mPaneWidth;
            pRowInfo->mBoundsInParent.height = pRowInfo->mRowHeight;

            PaneToFrame( &pRowInfo->mBoundsInParent );

            clip_rect_against_rect( pRowInfo->mBoundsInParent, noMarginsRect );
        }

        wxRect bounds = bar.mBounds;

        // exclude dimensions of handles, when calculating
        // bar's bounds in parent (i.e. "visual bounds")

        if ( bar.mHasLeftHandle )
        {
            bounds.x     += mProps.mResizeHandleSize;
            bounds.width -= mProps.mResizeHandleSize;
        }

        if ( bar.mHasRightHandle )

            bounds.width -= mProps.mResizeHandleSize;

        PaneToFrame( &bounds );

        clip_rect_against_rect( bounds, noMarginsRect );

        bar.mBoundsInParent = bounds;
    }
}

bool cbDockPane::BarPresent( cbBarInfo* pBar )
{
    wxBarIterator iter( mRows );

    while( iter.Next() )

        if ( &iter.BarInfo() == pBar ) return true;

    return false;
}

cbRowInfo* cbDockPane::GetRow( int row )
{
    if ( row >= (int)mRows.Count() ) return NULL;

    return mRows[ row ];
}

int cbDockPane::GetRowIndex( cbRowInfo* pRow )
{
    size_t i;
    for ( i = 0; i != mRows.Count(); ++i )
    {
        if ( mRows[i] == pRow )
            return i;
    }

    wxFAIL_MSG(wxT("Row must be present to call cbDockPane::GetRowIndex()"));

    return 0;
}

int cbDockPane::GetPaneHeight()
{
    // first, recalculate row heights and the Y-positions

    cbLayoutRowsEvent evt( this );
    mpLayout->FirePluginEvent( evt );

    int height = 0;

    if ( IsHorizontal() )
        height += mTopMargin  + mBottomMargin;
    else
        height += mLeftMargin + mRightMargin;

    int count = mRows.Count();

    if ( count )
        height += mRows[count-1]->mRowY + mRows[count-1]->mRowHeight;

    return height;
}

int cbDockPane::GetAlignment()
{
    return mAlignment;
}

bool cbDockPane::MatchesMask( int paneMask )
{
    int thisMask = 0;

    // FIXME:: use array instead of switch()

    switch (mAlignment)
    {
        case FL_ALIGN_TOP    : thisMask = FL_ALIGN_TOP_PANE;   break;
        case FL_ALIGN_BOTTOM : thisMask = FL_ALIGN_BOTTOM_PANE;break;
        case FL_ALIGN_LEFT   : thisMask = FL_ALIGN_LEFT_PANE;  break;
        case FL_ALIGN_RIGHT  : thisMask = FL_ALIGN_RIGHT_PANE; break;

        default:
            wxFAIL_MSG(wxT("Bad FL alignment type detected in cbDockPane::MatchesMask()"));
    }

    return ( thisMask & paneMask ) != 0;
}

void cbDockPane::RecalcLayout()
{
    // first, reposition rows and items vertically

    cbLayoutRowsEvent evt( this );
    mpLayout->FirePluginEvent( evt );

    // then horizontally in each row

    size_t i;
    for ( i = 0; i != mRows.Count(); ++i )
        RecalcRowLayout( mRows[i] );
}

int cbDockPane::GetDockingState()
{
    if ( mAlignment == FL_ALIGN_TOP ||
         mAlignment == FL_ALIGN_BOTTOM )
    {
        return wxCBAR_DOCKED_HORIZONTALLY;
    }
    else
        return wxCBAR_DOCKED_VERTICALLY;
}

inline bool cbDockPane::HasPoint( const wxPoint& pos, int x, int y,
                                  int width, int height )
{
    return ( pos.x >= x &&
             pos.y >= y &&
             pos.x < x + width &&
             pos.y < y + height   );
}

int cbDockPane::HitTestPaneItems( const wxPoint& pos,
                                  cbRowInfo**    ppRow,
                                  cbBarInfo**    ppBar
                                )
{
    (*ppRow) = NULL;
    (*ppBar) = NULL;

    size_t i;
    for ( i = 0; i != mRows.Count(); ++i )
    {
        cbRowInfo& row = *mRows[i];

        *ppRow = &row;

        // hit-test handles of the row, if present

        if ( row.mHasUpperHandle )
        {
            if ( HasPoint( pos, 0, row.mRowY,
                           row.mRowWidth, mProps.mResizeHandleSize ) )

                return CB_UPPER_ROW_HANDLE_HITTED;
        }
        else
        if ( row.mHasLowerHandle )
        {
            if ( HasPoint( pos, 0, row.mRowY + row.mRowHeight - mProps.mResizeHandleSize,
                           row.mRowWidth, mProps.mResizeHandleSize ) )

                    return CB_LOWER_ROW_HANDLE_HITTED;
        }

        // hit-test bar handles and bar content

        size_t k;
        for ( k = 0; k != row.mBars.Count(); ++k )
        {
            cbBarInfo& bar    = *row.mBars[k];
            wxRect&    bounds = bar.mBounds;

            *ppBar = &bar;

            if ( bar.mHasLeftHandle )
            {
                if ( HasPoint( pos, bounds.x, bounds.y,
                               mProps.mResizeHandleSize, bounds.height ) )

                    return CB_LEFT_BAR_HANDLE_HITTED;
            }
            else
            if ( bar.mHasRightHandle )
            {
                if ( HasPoint( pos, bounds.x + bounds.width - mProps.mResizeHandleSize, bounds.y,
                               mProps.mResizeHandleSize, bounds.height ) )

                    return CB_RIGHT_BAR_HANDLE_HITTED;
            }

            if ( HasPoint( pos, bounds.x, bounds.y, bounds.width, bounds.height ) )
                return CB_BAR_CONTENT_HITTED;

        } // hit-test next bar

    } // next row

    return CB_NO_ITEMS_HITTED;
}

void cbDockPane::GetBarResizeRange( cbBarInfo* pBar, int* from, int *till,
                                    bool forLeftHandle )
{
    cbBarInfo* pGivenBar = pBar;

    int notFree = 0;

    // calc unavailable space from the left

    while( pBar->mpPrev )
    {
        pBar = pBar->mpPrev;

        if ( !pBar->IsFixed() ) notFree += mProps.mMinCBarDim.x;
                                else notFree += pBar->mBounds.width;
    }

    *from = notFree;

    pBar = pGivenBar;

    notFree = 0;

    // calc unavailable space from the right

    while( pBar->mpNext )
    {
        pBar = pBar->mpNext;

        if ( pBar->mBounds.x >= mPaneWidth ) break;

        // treat not-fixed bars as minimized

        if ( !pBar->IsFixed() )
            notFree += mProps.mMinCBarDim.x;
        else
        {
            if ( pBar->mBounds.x + pBar->mBounds.width >= mPaneWidth )
            {
                notFree += mPaneWidth - pBar->mBounds.x;
                break;
            }
            else
                notFree += pBar->mBounds.width;
        }

    }

    *till = mPaneWidth - notFree;

    // do not let resizing totally deform the bar itself

    if ( forLeftHandle )
        (*till) -= mProps.mMinCBarDim.x;
    else
        (*from) += mProps.mMinCBarDim.x;
}

int cbDockPane::GetMinimalRowHeight( cbRowInfo* pRow )
{
    int height = mProps.mMinCBarDim.y;

    size_t i;
    for ( i = 0; i != pRow->mBars.Count(); ++i )
    {
        if ( pRow->mBars[i]->IsFixed() )
            height = wxMax( height, pRow->mBars[i]->mBounds.height );
    }

    if ( pRow->mHasUpperHandle )
        height += mProps.mResizeHandleSize;

    if ( pRow->mHasLowerHandle )
        height += mProps.mResizeHandleSize;

    return height;
}

void cbDockPane::SetRowHeight( cbRowInfo* pRow, int newHeight )
{
    if ( pRow->mHasUpperHandle )

        newHeight -= mProps.mResizeHandleSize;

    if ( pRow->mHasLowerHandle )

        newHeight -= mProps.mResizeHandleSize;

    size_t i;
    for ( i = 0; i != pRow->mBars.Count(); ++i )
    {
        if ( !pRow->mBars[i]->IsFixed() )
            pRow->mBars[i]->mBounds.height = newHeight;
    }
}

void cbDockPane::GetRowResizeRange( cbRowInfo* pRow, int* from, int* till,
                                    bool forUpperHandle )
{
    cbRowInfo* pGivenRow = pRow;

    // calc unavailable space from above

    int notFree = 0;

    while( pRow->mpPrev )
    {
        pRow = pRow->mpPrev;

        notFree += GetMinimalRowHeight( pRow );

    };

    *from = notFree;

    // allow accupy the client window space by resizing pane rows
    if ( mAlignment == FL_ALIGN_BOTTOM )

        *from -= mpLayout->GetClientHeight();
    else
    if ( mAlignment == FL_ALIGN_RIGHT )

        *from -= mpLayout->GetClientWidth();

    // calc unavailable space from below

    pRow = pGivenRow;

    notFree = 0;

    while( pRow->mpNext )
    {
        pRow = pRow->mpNext;

        notFree += GetMinimalRowHeight( pRow );

    }

    *till = mPaneHeight - notFree;

    // allow adjustinig pane space vs. client window space by resizing pane row heights

    if ( mAlignment == FL_ALIGN_TOP )

        *till += mpLayout->GetClientHeight();
    else
    if ( mAlignment == FL_ALIGN_LEFT )

        *till += mpLayout->GetClientWidth();

    // do not let the resizing of the row totally squeeze the row itself

    cbRowInfo& row = *pGivenRow;

    if ( forUpperHandle )
    {
        *till = row.mRowY + row.mRowHeight - GetMinimalRowHeight( pGivenRow );

        if ( row.mHasUpperHandle )

            *till -= mProps.mResizeHandleSize;
    }
    else
    {
        *from += GetMinimalRowHeight( pGivenRow );

        if ( row.mHasLowerHandle )

            *from -= mProps.mResizeHandleSize;
    }
}

void cbDockPane::ResizeRow( cbRowInfo* pRow, int ofs,
                            bool forUpperHandle )
{
    cbResizeRowEvent evt( pRow, ofs, forUpperHandle, this );

    mpLayout->FirePluginEvent( evt );
}

void cbDockPane::ResizeBar( cbBarInfo* pBar, int ofs,
                            bool forLeftHandle )
{
    pBar->mpRow->mpExpandedBar = NULL;

    mpLayout->GetUpdatesManager().OnStartChanges();

    wxRect&  bounds = pBar->mBounds;

    if ( forLeftHandle )
    {
        // do not allow bar width become less then minimal
        if ( bounds.x + ofs > bounds.x + bounds.width - mProps.mMinCBarDim.x )
        {
            bounds.width = mProps.mMinCBarDim.x;
            bounds.x += ofs;
        }
        else
        {
            bounds.x     += ofs;
            bounds.width -= ofs;
        }
    }
    else
    {
        // move bar left if necessary
        if ( bounds.width + ofs < mProps.mMinCBarDim.x )
        {
            bounds.x     = bounds.x + bounds.width + ofs - mProps.mMinCBarDim.x;
            bounds.width = mProps.mMinCBarDim.x;
        }
        else
            // resize right border only
            bounds.width += ofs;
    }


    cbRowInfo* pToRow = pBar->mpRow;

    this->RemoveBar( pBar );

    InsertBar( pBar, pToRow );

    mpLayout->RecalcLayout(false);

    mpLayout->GetUpdatesManager().OnFinishChanges();
    mpLayout->GetUpdatesManager().UpdateNow();
}


/*** row/bar resizing related methods ***/

void cbDockPane::DrawVertHandle( wxDC& dc, int x, int y, int height )
{
    int lower = y + height;

    dc.SetPen( mpLayout->mLightPen );
    dc.DrawLine( x,y, x, lower );

    dc.SetPen( mpLayout->mGrayPen );
    int i;
    for ( i = 0; i != mProps.mResizeHandleSize-1; ++i )
    {
        ++x;
        dc.DrawLine( x,y, x, lower );
    }

    dc.SetPen( mpLayout->mDarkPen );
    ++x;
    dc.DrawLine( x,y, x, lower );

    dc.SetPen( mpLayout->mBlackPen );
    ++x;
    dc.DrawLine( x,y, x, lower );
}

void cbDockPane::DrawHorizHandle( wxDC& dc, int x, int y, int width  )
{
    int right = x + width;

    dc.SetPen( mpLayout->mLightPen );
    dc.DrawLine( x,y, right, y );

    dc.SetPen( mpLayout->mGrayPen );

    int i;
    for ( i = 0; i != mProps.mResizeHandleSize-1; ++i )
    {
        ++y;
        dc.DrawLine( x,y, right, y );
    }

    ++y;
    dc.SetPen( mpLayout->mDarkPen );
    dc.DrawLine( x,y, right, y );

    ++y;
    dc.SetPen( mpLayout->mBlackPen );
    dc.DrawLine( x,y, right, y );
}

cbBarInfo* cbDockPane::GetBarInfoByWindow( wxWindow* pBarWnd )
{
    wxBarIterator i( mRows );

    while( i.Next() )

        if ( i.BarInfo().mpBarWnd == pBarWnd )

            return &i.BarInfo();

    return NULL;
}

void cbDockPane::GetRowShapeData( cbRowInfo* pRow, wxList* pLst )
{
    if(pLst)
    {
        WX_CLEAR_LIST(wxList,*pLst);
    }

    pLst->Clear();

    size_t i;
    for ( i = 0; i != pRow->mBars.Count(); ++i )
    {
        cbBarInfo& bar = *pRow->mBars[i];

        cbBarShapeData* pData = new cbBarShapeData();

        pLst->Append( (wxObject*)pData );

        pData->mBounds   = bar.mBounds;
        pData->mLenRatio = bar.mLenRatio;
    }
}

void cbDockPane::SetRowShapeData( cbRowInfo* pRow, wxList* pLst )
{
    if ( pLst->GetFirst() == NULL )
        return;

    wxObjectList::compatibility_iterator pData = pLst->GetFirst();

    size_t i;
    for ( i = 0; i != pRow->mBars.Count(); ++i )
    {
        wxASSERT( pData ); // DBG::

        cbBarInfo& bar = *pRow->mBars[i];;

        cbBarShapeData& data = *((cbBarShapeData*)pData->GetData());

        bar.mBounds   = data.mBounds;
        bar.mLenRatio = data.mLenRatio;

        pData = pData->GetNext();
    }
}

/***** Implementation for class cbUpdatesManagerBase *****/

IMPLEMENT_ABSTRACT_CLASS( cbUpdatesManagerBase, wxObject )

/***** Implementation for class cbPluginBase *****/

IMPLEMENT_ABSTRACT_CLASS( cbPluginBase, wxEvtHandler )

cbPluginBase::~cbPluginBase()
{
    // nothing
}

bool cbPluginBase::ProcessEvent(wxEvent& event)
{
    if ( mPaneMask == wxALL_PANES )

        return wxEvtHandler::ProcessEvent( event );

    // extract mask info. from received event

    cbPluginEvent& evt = *( (cbPluginEvent*)&event );

    if ( evt.mpPane == 0 &&
         mPaneMask  == wxALL_PANES )

         return wxEvtHandler::ProcessEvent( event );

    int mask = 0;

    switch ( evt.mpPane->mAlignment )
    {
        case FL_ALIGN_TOP    : mask = FL_ALIGN_TOP_PANE;   break;
        case FL_ALIGN_BOTTOM : mask = FL_ALIGN_BOTTOM_PANE;break;
        case FL_ALIGN_LEFT   : mask = FL_ALIGN_LEFT_PANE;  break;
        case FL_ALIGN_RIGHT  : mask = FL_ALIGN_RIGHT_PANE; break;
    }

    // if event's pane maks matches the plugin's mask

    if ( mPaneMask & mask )

        return wxEvtHandler::ProcessEvent( event );

    // otherwise pass to the next handler if present

    if ( GetNextHandler() && GetNextHandler()->ProcessEvent( event ) )

        return true;
    else
        return false;
}
