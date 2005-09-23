/////////////////////////////////////////////////////////////////////////////
// Name:        hintanimpl.cpp
// Purpose:     cbHintAnimationPlugin implementation.
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     9/11/98
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

#include "wx/fl/hintanimpl.h"

#define POS_UNDEFINED -32768

/***** Implementation for class cbHintAnimationPlugin *****/

// FIXME:: some of the below code should be eliminated by 
//         reusing parts of cbBarDragPlugin's implementation

IMPLEMENT_DYNAMIC_CLASS( cbHintAnimationPlugin, cbPluginBase )

BEGIN_EVENT_TABLE( cbHintAnimationPlugin, cbPluginBase )

    EVT_PL_DRAW_HINT_RECT( cbHintAnimationPlugin::OnDrawHintRect )

END_EVENT_TABLE()

cbHintAnimationPlugin::cbHintAnimationPlugin(void)

    : mpScrDc( NULL ),
      mpAnimTimer( 0 ),
      mAnimStarted( false ),
      
      mMorphDelay    ( 5   ),
      mMaxFrames     ( 20   ),
      mInClientHintBorder( 4 ),
      mAccelerationOn( true )
{}

cbHintAnimationPlugin::cbHintAnimationPlugin( wxFrameLayout* pPanel, int paneMask )

    : cbPluginBase( pPanel, paneMask ),
      mpScrDc( NULL ),
      mpAnimTimer( 0 ),
      mAnimStarted( false ),

      mMorphDelay    ( 5   ),
      mMaxFrames     ( 20   ),
      mInClientHintBorder( 4 ),
      mAccelerationOn( true )
{}

cbHintAnimationPlugin::~cbHintAnimationPlugin()
{
    if ( mpScrDc ) delete mpScrDc;
}

/*** rect-tracking related methods ***/

void cbHintAnimationPlugin::OnDrawHintRect( cbDrawHintRectEvent& event )
{
    if ( !mAnimStarted && !mpScrDc ) 
    {
        StartTracking();

        mPrevInClient = event.mIsInClient;

        mPrevRect = event.mRect;

        mStopPending = false;
    }

    if ( !event.mEraseRect )
    {
        // pass on current hint-rect info to the animation "thread", in 
        // order to make adjustments to the morph-target on-the-fly

        mCurRect.x = event.mRect.x;
        mCurRect.y = event.mRect.y;
        mCurRect.width  = event.mRect.width;
        mCurRect.height = event.mRect.height;
    }

    // check the amount of change in the shape of hint,
    // and start morph-effect if change is "sufficient"

    int change = abs( mCurRect.width  - mPrevRect.width  ) +
                 abs( mCurRect.height - mPrevRect.height );

    if ( change > 10 && !event.mLastTime && !event.mEraseRect )
    {
        if ( !mpAnimTimer )

            mpAnimTimer  = new cbHintAnimTimer();

        // init the animation "thread", or reinit if already started

        mpAnimTimer->Init( this, mAnimStarted );

        mAnimStarted = true;
    }
    else
    if ( !mAnimStarted )
    {
        DoDrawHintRect( event.mRect, event.mIsInClient );

        if ( event.mLastTime )

            FinishTracking();

        mPrevInClient = event.mIsInClient;
    }
    else
    {
        mCurInClient = event.mIsInClient;

        if ( event.mLastTime && mpAnimTimer ) 
        {
            mStopPending = true;

            if ( mpAnimTimer->mPrevMorphed.x != POS_UNDEFINED )

                // erase previous rect
                DoDrawHintRect( mpAnimTimer->mPrevMorphed, mPrevInClient );
        }
    }

    mPrevRect = event.mRect;
}

#define _IMG_A  0xAA    // Note: modified from _A to _IMG_A, _A was already defined (cygwin)
#define _IMG_B  0x00    // Note: modified from _B to _IMG_A, _B was already defined (cygwin)
#define _IMG_C  0x55    // Note: modified from _C to _IMG_C, for consistency reasons.
#define _IMG_D  0x00    // Note: modified from _D to _IMG_D, for consistency reasons.

static const unsigned char _gCheckerImg[16] = { _IMG_A,_IMG_B,_IMG_C,_IMG_D,
                                                _IMG_A,_IMG_B,_IMG_C,_IMG_D,
                                                _IMG_A,_IMG_B,_IMG_C,_IMG_D,
                                                _IMG_A,_IMG_B,_IMG_C,_IMG_D
                                              };

void cbHintAnimationPlugin::StartTracking()
{
    mpScrDc = new wxScreenDC;

    wxScreenDC::StartDrawingOnTop(&mpLayout->GetParentFrame());
}

void cbHintAnimationPlugin::DoDrawHintRect( wxRect& rect, bool isInClientRect)
{
    wxRect scrRect;

    RectToScr( rect, scrRect );

    int prevLF = mpScrDc->GetLogicalFunction();

    mpScrDc->SetLogicalFunction( wxXOR );

    if ( isInClientRect )
    {
        // BUG BUG BUG (wx):: somehow stippled brush works only  
        //                      when the bitmap created on stack, not
        //                      as a member of the class

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
        // otherwise draw 1-pixel thin borders

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

void cbHintAnimationPlugin::DrawHintRect ( wxRect& rect, bool isInClientRect)
{
    DoDrawHintRect( rect, isInClientRect );    
}

void cbHintAnimationPlugin::EraseHintRect( wxRect& rect, bool isInClientRect)
{
    DoDrawHintRect( rect, isInClientRect );    
}

void cbHintAnimationPlugin::FinishTracking()
{
    wxScreenDC::EndDrawingOnTop();

    delete mpScrDc;

    mpScrDc = NULL;
}

void cbHintAnimationPlugin::RectToScr( wxRect& frameRect, wxRect& scrRect )
{
    scrRect = frameRect;

    int x = frameRect.x, y = frameRect.y;

    mpLayout->GetParentFrame().ClientToScreen( &x, &y );

    scrRect.x = x;
    scrRect.y = y;
}

/***** Implementation for class cbHintAnimTimer *****/

cbHintAnimTimer::cbHintAnimTimer(void)
{
#ifdef __WINDOWS__
    mLock = 0L;
#endif

    mPrevMorphed.x = POS_UNDEFINED;
}

void cbHintAnimTimer::MorphPoint( wxPoint& origin, MorphInfoT& info, wxPoint& point )
{
    // simulate lienar movement (FOR NOW:: without acceleration)

    double k;
    
    if ( mpPl->mAccelerationOn )
        
        k = double( mCurIter*mCurIter ) / 
            double( (mpPl->mMaxFrames - 1)*(mpPl->mMaxFrames - 1) );
    else
        k = double( mCurIter ) / double( mpPl->mMaxFrames - 1 );

    point.x = int ( double ( info.mFrom.x + double (info.mTill.x - info.mFrom.x) * k ) );

    point.y = int ( double ( info.mFrom.y + double (info.mTill.y - info.mFrom.y) * k ) );

    point.x += origin.x;
    point.y += origin.y;
}

void cbHintAnimTimer::Notify(void)
{
    // FIXME:: "clean" implementation should use mutex to sync
    //         between GUI and animation threads

    if ( mpPl->mStopPending )
    {
        Stop(); // top timer

        mpPl->FinishTracking();

        mpPl->mStopPending = false;
        mpPl->mpAnimTimer  = NULL;
        mpPl->mAnimStarted = false;

        mPrevMorphed.x = POS_UNDEFINED;

        delete this;

        return;
    }

    wxPoint origin( mpPl->mCurRect.x, mpPl->mCurRect.y );

    wxPoint curUpper, curLower;

    MorphPoint( origin, mUpperLeft,  curUpper  );
    MorphPoint( origin, mLowerRight, curLower );

    if ( mPrevMorphed.x != POS_UNDEFINED )

        // erase previous rect
        mpPl->DoDrawHintRect( mPrevMorphed, mpPl->mPrevInClient );

    wxRect morphed( curUpper.x,  curUpper.y,
                    curLower.x - curUpper.x,
                    curLower.y - curUpper.y );

    // draw rect of current iteration
    mpPl->DoDrawHintRect( morphed, 
                          ( mCurIter != mpPl->mMaxFrames - 1 ) 
                          ? mpPl->mPrevInClient : mpPl->mCurInClient );

    mPrevMorphed = morphed;

    if ( mCurIter == mpPl->mMaxFrames - 1 ) 
    {
        Stop(); // top timer
        
        mpPl->FinishTracking();
        mpPl->mpAnimTimer  = NULL;
        mpPl->mAnimStarted = false;

        mPrevMorphed.x = POS_UNDEFINED;

        delete this;
    }
    else
        ++mCurIter;
}

bool cbHintAnimTimer::Init( cbHintAnimationPlugin* pAnimPl, bool reinit )
{

    mpPl = pAnimPl;

    // morph-points are set up relatively to the upper-left corner
    // of the current hint-rectangle

    if ( !reinit )
    {
        mUpperLeft.mFrom.x = mpPl->mPrevRect.x - mpPl->mCurRect.x;
        mUpperLeft.mFrom.y = mpPl->mPrevRect.y - mpPl->mCurRect.y;

        mLowerRight.mFrom.x = ( mUpperLeft.mFrom.x + mpPl->mPrevRect.width  );
        mLowerRight.mFrom.y = ( mUpperLeft.mFrom.y + mpPl->mPrevRect.height );
    }
    else
    {
        wxPoint origin( mpPl->mPrevRect.x, mpPl->mPrevRect.y );

        wxPoint curUpper, curLower;

        MorphPoint( origin, mUpperLeft,  curUpper  );
        MorphPoint( origin, mLowerRight, curLower );

        mUpperLeft.mFrom.x = curUpper.x - mpPl->mCurRect.x;
        mUpperLeft.mFrom.y = curUpper.y - mpPl->mCurRect.y;

        mLowerRight.mFrom.x = ( mUpperLeft.mFrom.x + curLower.x - curUpper.x );
        mLowerRight.mFrom.y = ( mUpperLeft.mFrom.y + curLower.y - curUpper.y );
    }

    mUpperLeft.mTill.x = 0;
    mUpperLeft.mTill.y = 0;

    mLowerRight.mTill.x = mpPl->mCurRect.width;
    mLowerRight.mTill.y = mpPl->mCurRect.height;

    mCurIter = 1;
    
    if ( !reinit )

        Start( mpPl->mMorphDelay );

    return true;
}

