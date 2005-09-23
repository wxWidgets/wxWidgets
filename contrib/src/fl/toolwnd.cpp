/////////////////////////////////////////////////////////////////////////////
// Name:        toolwnd.cpp
// Purpose:     wxToolWindow implementation.
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

#include "wx/fl/toolwnd.h"

#define _IMG_A  0xAA    // Note: modified from _A to _IMG_A, _A was already defined (cygwin)
#define _IMG_B  0x00    // Note: modified from _B to _IMG_A, _B was already defined (cygwin)
#define _IMG_C  0x55    // Note: modified from _C to _IMG_C, for consistency reasons.
#define _IMG_D  0x00    // Note: modified from _D to _IMG_D, for consistency reasons.

// FOR NOW:: static

static const unsigned char _gCheckerImg[16] = { _IMG_A,_IMG_B,_IMG_C,_IMG_D,
                                                _IMG_A,_IMG_B,_IMG_C,_IMG_D,
                                                _IMG_A,_IMG_B,_IMG_C,_IMG_D,
                                                _IMG_A,_IMG_B,_IMG_C,_IMG_D
                                              };

/***** Implementation for class wxToolWindow *****/

IMPLEMENT_DYNAMIC_CLASS( wxToolWindow, wxFrame)

BEGIN_EVENT_TABLE( wxToolWindow, wxFrame )

    EVT_PAINT    ( wxToolWindow::OnPaint )
    EVT_MOTION   ( wxToolWindow::OnMotion )
    EVT_LEFT_DOWN( wxToolWindow::OnLeftDown )
    EVT_LEFT_UP  ( wxToolWindow::OnLeftUp )
    EVT_SIZE     ( wxToolWindow::OnSize )


    EVT_ERASE_BACKGROUND( wxToolWindow::OnEraseBackground )

END_EVENT_TABLE()

enum INTERNAL_HIT_CODES
{
    HITS_WND_NOTHING,
    HITS_WND_CLIENT,
    HITS_WND_TITLE,

    HITS_WND_LEFT_EDGE,
    HITS_WND_RIGHT_EDGE,
    HITS_WND_TOP_EDGE,
    HITS_WND_BOTTOM_EDGE,

    HITS_WND_TOP_LEFT_CORNER,
    HITS_WND_BOTTOM_RIGHT_CORNER,
    HITS_WND_TOP_RIGHT_CORNER,
    HITS_WND_BOTTOM_LEFT_CORNER
};

wxToolWindow::wxToolWindow()

    : mpClientWnd   ( NULL ),

#ifndef __WXMSW__
    mTitleFont( 8, wxSWISS,  wxNORMAL, wxNORMAL ),
#else
    // just to simulate MS-Dev style
    mTitleFont( 8, wxSWISS,  wxNORMAL, wxNORMAL, false, wxT("MS Sans Serif") ),
#endif

    mTitleHeight  ( 16 ),
    mClntHorizGap ( 2 ),
    mClntVertGap  ( 2 ),
    mWndVertGap   ( 4 ),
    mWndHorizGap  ( 4 ),

    mButtonGap    ( 2 ),
    mInTitleMargin( 4 ),
    mHintBorder   ( 4 ),

    mResizeStarted( false ),
    mRealTimeUpdatesOn( true ),

    mMTolerance   ( 5 ), // mouse-resizing tollerance

    mCursorType( HITS_WND_NOTHING ),
    mMouseCaptured( false ),

    mpScrDc( NULL )

{
}

wxToolWindow::~wxToolWindow()
{
    if ( mpScrDc ) delete mpScrDc;

    for( size_t i = 0; i != mButtons.Count(); ++i )
        delete mButtons[i];
}

void wxToolWindow::LayoutMiniButtons()
{
    int w,h;

    GetSize( &w, &h );

    int x = w - mWndHorizGap - mInTitleMargin - BTN_BOX_WIDTH;
    int y = mWndVertGap + 2;

    for( size_t i = 0; i != mButtons.Count(); ++i )
    {
        mButtons[i]->SetPos( wxPoint( x,y ) );
        x-= BTN_BOX_WIDTH + mButtonGap;
    }
}

void wxToolWindow::SetClient( wxWindow* pWnd )
{
    mpClientWnd = pWnd;
}

wxWindow* wxToolWindow::GetClient()
{
    return mpClientWnd;
}

void wxToolWindow::SetTitleFont( wxFont& font )
{
    mTitleFont = font;
}

void wxToolWindow::AddMiniButton( cbMiniButton* pBtn )
{
    pBtn->mpWnd = this;

    mButtons.Add( pBtn );

    // not necesserely now..
    //LayoutMiniButtons();
}

void wxToolWindow::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC pdc( this );
    wxWindowDC dc( this );

    int w,h;
    GetSize( &w, &h );

    wxBrush backGround( wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE), wxSOLID );
    //dc.SetBrush( *wxLIGHT_GREY_BRUSH );
    dc.SetBrush( backGround );
    dc.SetPen( *wxTRANSPARENT_PEN );

    int y = mWndVertGap + mTitleHeight + mClntVertGap;

    dc.DrawRectangle( 0,0, w, y );                                      // Top grey part.
    dc.DrawRectangle( 0,y-1, mWndHorizGap + mClntHorizGap, h - y );     // Left grey part.
    dc.DrawRectangle( w - ( mWndHorizGap + mClntHorizGap ), y-1,
                      mWndHorizGap + mClntHorizGap, h - y );            // Right grey part.
    dc.DrawRectangle( 0, h - mWndVertGap - mClntVertGap, w, mWndVertGap + mClntVertGap ); // Bottom grey part.

    // draw shades
    dc.SetPen( *wxLIGHT_GREY_PEN );

    dc.DrawLine( 0,0, w, 0 );
    dc.DrawLine( 0,0, 0, h );

    dc.SetPen( *wxWHITE_PEN );

    dc.DrawLine( 1,1, w, 1 );
    dc.DrawLine( 1,2, 1, h );

    dc.SetPen( *wxGREY_PEN );

    dc.DrawLine( w - 2, 1, w - 2, h - 1 );
    dc.DrawLine( 1, h - 2, w - 2, h - 2 );

    dc.SetPen( *wxBLACK_PEN );

    dc.DrawLine( 0, h - 1, w, h - 1 );
    dc.DrawLine( w-1, 0, w-1, h );

    // fill inner area

    dc.SetBrush( *wxTheBrushList->FindOrCreateBrush( wxColour( 0,0,128 ), wxSOLID ) );

    dc.DrawRectangle( mWndHorizGap, mWndVertGap, w - mWndHorizGap*2, mTitleHeight );

    dc.SetFont( mTitleFont );

    for( size_t i = 0; i != mButtons.Count(); ++i )
        mButtons[i]->Draw( dc );

    int x1 = mWndHorizGap + mClntHorizGap;
    int x2 = mButtons[ mButtons.GetCount() - 1 ]->mPos.x - mClntHorizGap*2;

    dc.SetClippingRegion( x1, mWndVertGap + mClntVertGap, x2 - x1, mTitleHeight );

    dc.SetTextForeground( *wxWHITE );
    dc.SetBackgroundMode(  wxTRANSPARENT );
    dc.DrawText( GetTitle(), mWndHorizGap + 2, mWndVertGap + 1 );
}

void wxToolWindow::GetScrWindowRect( wxRect& r )
{
    int x,y;
    GetPosition(&x,&y);
    int w,h;
    GetSize( &w, &h );

    r.x = x; r.y = y;
    r.width = w; r.height = h;
}

void wxToolWindow::GetScrMousePos( wxMouseEvent& event, wxPoint& pos )
{
    int x = event.m_x, y = event.m_y;

    ClientToScreen( &x, &y );

    pos.x = x; pos.y = y;
}

int wxToolWindow::HitTestWindow( wxMouseEvent& event )
{
    wxPoint pos;
    wxRect r;

    GetScrMousePos( event, pos );
    GetScrWindowRect( r );

    int k = mMTolerance;

    if ( !( pos.x >= r.x && pos.y >= r.y &&
            pos.x < r.x + r.width &&
            pos.y < r.y + r.height )
       )
        return HITS_WND_NOTHING;

    if ( pos.y <= r.y + k )
    {
        if ( pos.x < r.x + k*2 )
            return HITS_WND_TOP_LEFT_CORNER;
        else
        {
            if ( pos.x >= r.x + r.width - k*2 )
                return HITS_WND_TOP_RIGHT_CORNER;
            else
                return HITS_WND_TOP_EDGE;
        }
    }
    else
    {
        if ( pos.y >= r.y + r.height - k )
        {
            if ( pos.x < r.x + k*2 )
                return HITS_WND_BOTTOM_LEFT_CORNER;
            else
            {
                if ( pos.x > r.x + r.width - k*2 )
                    return HITS_WND_BOTTOM_RIGHT_CORNER;
                else
                    return HITS_WND_BOTTOM_EDGE;
            }
        }
        else
        {
            if ( pos.x <= r.x + k )
                return HITS_WND_LEFT_EDGE;
            else
            {
                if ( pos.x >= r.x + r.width - k )
                    return HITS_WND_RIGHT_EDGE;
                else
                {
                    if ( pos.y <= r.y + mWndVertGap + mTitleHeight + mClntVertGap )
                        return HITS_WND_TITLE;
                    else
                        return HITS_WND_CLIENT;
                }
            }
        }
    }
}

void wxToolWindow::DrawHintRect( const wxRect& r )
{
    // BUG BUG BUG (wx):: somehow stippled brush works only
    //                    when the bitmap created on stack, not
    //                    as a member of the class

    int prevLF = mpScrDc->GetLogicalFunction();

    mpScrDc->SetLogicalFunction( wxXOR );

    wxBitmap checker( (const char*)_gCheckerImg, 8,8 );

    wxBrush checkerBrush( checker );

    mpScrDc->SetPen( *wxTRANSPARENT_PEN );
    mpScrDc->SetBrush( checkerBrush );

    int half = mHintBorder / 2;

    mpScrDc->DrawRectangle( r.x - half, r.y - half,
                            r.width + 2*half, mHintBorder );

    mpScrDc->DrawRectangle( r.x - half, r.y + r.height - half,
                            r.width + 2*half, mHintBorder );

    mpScrDc->DrawRectangle( r.x - half, r.y + half - 1,
                            mHintBorder, r.height - 2*half + 2);

    mpScrDc->DrawRectangle( r.x + r.width - half,
                            r.y + half - 1,
                            mHintBorder, r.height - 2*half + 2);

    mpScrDc->SetBrush( wxNullBrush );

    mpScrDc->SetLogicalFunction( prevLF );
}

void wxToolWindow::SetHintCursor( int type )
{
    if ( mResizeStarted )
        return;

    if ( type == HITS_WND_NOTHING || type == HITS_WND_CLIENT )
    {
        // the cursor is out of window - reset to arrow

        if ( mMouseCaptured )
        {
            ReleaseMouse();
            mMouseCaptured = false;
        }

        SetCursor( wxCURSOR_ARROW );

        mCursorType = type;

        return;
    }

    // did the cursor actually changed?

    if ( type != mCursorType )
    {
        mCursorType = type;

        switch ( type )
        {
            case HITS_WND_LEFT_EDGE   : SetCursor( wxCURSOR_SIZEWE ); break;
            case HITS_WND_RIGHT_EDGE  : SetCursor( wxCURSOR_SIZEWE ); break;
            case HITS_WND_TOP_EDGE    : SetCursor( wxCURSOR_SIZENS ); break;
            case HITS_WND_BOTTOM_EDGE : SetCursor( wxCURSOR_SIZENS ); break;

            case HITS_WND_TOP_LEFT_CORNER     : SetCursor( wxCURSOR_SIZENWSE ); break;
            case HITS_WND_BOTTOM_RIGHT_CORNER : SetCursor( wxCURSOR_SIZENWSE ); break;
            case HITS_WND_TOP_RIGHT_CORNER    : SetCursor( wxCURSOR_SIZENESW ); break;
            case HITS_WND_BOTTOM_LEFT_CORNER  : SetCursor( wxCURSOR_SIZENESW ); break;

            case HITS_WND_TITLE  : SetCursor( wxCURSOR_ARROW ); break;
            case HITS_WND_CLIENT : SetCursor( wxCURSOR_ARROW ); break;

            default: break;
        }

        if (mMouseCaptured)
        {
            mMouseCaptured = false;
            ReleaseMouse();
        }
    }

    if ( !mMouseCaptured )
    {
        mMouseCaptured = true;
        CaptureMouse();
    }
}

#define FL_INFINITY 32768

static inline void clip_to( int& value, long from, long till )
{
    if ( value < from )
        value = from;

    if ( value > till )
        value = till;
}

void wxToolWindow::AdjustRectPos( const wxRect& original, const wxSize& newDim, wxRect& newRect )
{
    if ( mCursorType == HITS_WND_TOP_EDGE ||
         mCursorType == HITS_WND_TOP_LEFT_CORNER )
    {
        newRect.x = original.x + original.width  - newDim.x;
        newRect.y = original.y + original.height - newDim.y;
    }
    else
    if ( mCursorType == HITS_WND_LEFT_EDGE ||
         mCursorType == HITS_WND_BOTTOM_LEFT_CORNER )
    {
        newRect.x = original.x + original.width  - newDim.x;
        newRect.y = original.y;
    }
    else
    if ( mCursorType == HITS_WND_RIGHT_EDGE ||
         mCursorType == HITS_WND_TOP_RIGHT_CORNER )
    {
        newRect.x = original.x;
        newRect.y = original.y + original.height - newDim.y;
    }
    else
    if ( mCursorType == HITS_WND_BOTTOM_EDGE ||
         mCursorType == HITS_WND_BOTTOM_RIGHT_CORNER )
    {
        newRect.x = original.x;
        newRect.y = original.y;
    }

    newRect.width  = newDim.x;
    newRect.height = newDim.y;
}

void wxToolWindow::CalcResizedRect( wxRect& rect, wxPoint& delta, const wxSize& minDim )
{
    // Microsoft's rect-coordinates are best suited
    // for the case of corner-clipping

    int left   = mInitialRect.x;
    int top    = mInitialRect.y;
    int right  = mInitialRect.x + mInitialRect.width;
    int bottom = mInitialRect.y + mInitialRect.height;

    // constraint delta edge is dragged

    switch ( mCursorType )
    {
        case HITS_WND_LEFT_EDGE   : delta.y = 0; break;
        case HITS_WND_RIGHT_EDGE  : delta.y = 0; break;
        case HITS_WND_TOP_EDGE    : delta.x = 0; break;
        case HITS_WND_BOTTOM_EDGE : delta.x = 0; break;
        default: break;
    }

    if ( mCursorType == HITS_WND_TOP_EDGE ||
         mCursorType == HITS_WND_TOP_LEFT_CORNER )
    {
        left += delta.x;
        top  += delta.y;

        clip_to( left, -FL_INFINITY, mInitialRect.x + mInitialRect.width  - minDim.x  );
        clip_to( top,  -FL_INFINITY, mInitialRect.y + mInitialRect.height - minDim.y );
    }
    else
    if ( mCursorType == HITS_WND_LEFT_EDGE ||
         mCursorType == HITS_WND_BOTTOM_LEFT_CORNER )
    {
        left   += delta.x;
        bottom += delta.y;

        clip_to( left,    -FL_INFINITY, mInitialRect.x + mInitialRect.width  - minDim.x  );
        clip_to( bottom,  mInitialRect.y + minDim.y, FL_INFINITY );
    }
    else
    if ( mCursorType == HITS_WND_RIGHT_EDGE ||
        mCursorType == HITS_WND_TOP_RIGHT_CORNER )
    {
        right += delta.x;
        top   += delta.y;

        clip_to( right, mInitialRect.x + minDim.x, FL_INFINITY );
        clip_to( top,   -FL_INFINITY, mInitialRect.y + mInitialRect.height - minDim.y );
    }
    else
    if ( mCursorType == HITS_WND_BOTTOM_EDGE ||
         mCursorType == HITS_WND_BOTTOM_RIGHT_CORNER )
    {
        right  += delta.x;
        bottom += delta.y;

        clip_to( right,  mInitialRect.x + minDim.x,  FL_INFINITY );
        clip_to( bottom, mInitialRect.y + minDim.y, FL_INFINITY );
    }
    else
    {
        wxFAIL_MSG( _T("what did the cursor hit?") );
    }

    rect.x = left;
    rect.y = top;
    rect.width  = right - left;
    rect.height = bottom - top;
}

wxSize wxToolWindow::GetMinimalWndDim()
{
    return wxSize( (mWndHorizGap + mClntHorizGap)*2 + BTN_BOX_WIDTH*4,
                   (mWndVertGap  + mClntVertGap )*2 + mTitleHeight );
}

void wxToolWindow::OnMotion( wxMouseEvent& event )
{
    if ( !mResizeStarted )
    {
        for( size_t i = 0; i != mButtons.Count(); ++i )
            mButtons[i]->OnMotion( wxPoint( event.m_x, event.m_y ) );

        SetHintCursor( HitTestWindow( event ) );
        return;
    }

    wxPoint pos;
    GetScrMousePos( event, pos );

    if ( mCursorType == HITS_WND_TITLE )
    {
        int w,h;
        GetSize( &w, &h );

        SetSize( mInitialRect.x + pos.x - mDragOrigin.x,
                 mInitialRect.y + pos.y - mDragOrigin.y,
                 w,h, 0 );
    }

    else
    {
        wxPoint delta( pos.x - mDragOrigin.x, pos.y - mDragOrigin.y );

        wxRect newRect;

        wxSize minDim = GetMinimalWndDim();

        CalcResizedRect( newRect, delta, minDim );

        wxSize borderDim( ( mWndHorizGap + mClntHorizGap )*2,
                          ( mWndVertGap  + mClntVertGap  )*2 + mTitleHeight );

        wxSize preferred = GetPreferredSize( wxSize( newRect.width  - borderDim.x,
                                             newRect.height - borderDim.y ) );

        preferred.x += borderDim.x;
        preferred.y += borderDim.y;

        //CalcResizedRect( newRect, delta, preferred );

        wxRect finalRect = newRect;

        AdjustRectPos( newRect, preferred, finalRect );

        if ( mRealTimeUpdatesOn )
        {
            SetSize( finalRect.x, finalRect.y,
                     finalRect.width, finalRect.height, 0 );
        }
        else
        {
            DrawHintRect( mPrevHintRect );
            DrawHintRect( finalRect );

            ::wxLogTrace(wxT("wxToolWindow"),wxT("%d,%d / %d,%d\n"), finalRect.x, finalRect.y, finalRect.width, finalRect.height);
        }

        mPrevHintRect = finalRect;
    }
}

void wxToolWindow::OnLeftDown( wxMouseEvent& event )
{
    int result = HitTestWindow( event );

    for( size_t i = 0; i != mButtons.Count(); ++i )
    {
        mButtons[i]->OnLeftDown( wxPoint( event.m_x, event.m_y ) );

        if ( mButtons[i]->IsPressed() )
            return; // button hitted,
    }

    if ( result >= HITS_WND_LEFT_EDGE || result == HITS_WND_TITLE )
    {
        GetScrMousePos( event, mDragOrigin );

        /*
        if ( mMouseCaptured `)
        {
            ReleaseMouse();
            mMouseCaptured = false;
        }*/

        if ( result == HITS_WND_TITLE &&
             HandleTitleClick( event ) )
            return;

        mResizeStarted = true;

        int x,y;
        GetPosition( &x, &y );

        mInitialRect.x = x;
        mInitialRect.y = y;

        GetSize( &x, &y );
        mInitialRect.width  = x;
        mInitialRect.height = y;

        mPrevHintRect = mInitialRect;

        if ( mCursorType != HITS_WND_TITLE && !mRealTimeUpdatesOn )
        {
            mpScrDc = new wxScreenDC();

            wxScreenDC::StartDrawingOnTop( (wxRect*)NULL );

            DrawHintRect( mInitialRect );
        }
    }
}

void wxToolWindow::OnLeftUp( wxMouseEvent& event )
{
    for( size_t i = 0; i != mButtons.Count(); ++i )
    {
        mButtons[i]->OnLeftUp( wxPoint( event.m_x, event.m_y ) );

        if ( mButtons[i]->WasClicked() )
        {
            OnMiniButtonClicked( i ); // notify derived classes
            mButtons[i]->Reset();
        }
    }

    if ( mResizeStarted )
    {
        mResizeStarted = false;

        if ( mCursorType != HITS_WND_TITLE )
        {
            if ( !mRealTimeUpdatesOn )
            {
                DrawHintRect( mPrevHintRect );

                wxScreenDC::EndDrawingOnTop();

                delete mpScrDc;

                mpScrDc = NULL;

                SetSize( mPrevHintRect.x, mPrevHintRect.y,
                         mPrevHintRect.width, mPrevHintRect.height, 0 );
            }
        }
    }
}

void wxToolWindow::OnSize( wxSizeEvent& WXUNUSED(event) )
{
    if ( mpClientWnd )
    {
        int w,h;
        GetSize( &w, &h );

        int x = mWndHorizGap + mClntHorizGap;
        int y = mWndVertGap  + mTitleHeight + mClntVertGap;

        mpClientWnd->SetSize( x-1, y-1,
                              w - 2*(mWndHorizGap + mClntHorizGap),
                              h - y - mClntVertGap - mWndVertGap,
                              0
                            );
    }

    LayoutMiniButtons();
}

wxSize wxToolWindow::GetPreferredSize( const wxSize& given )
{
    return given;
}

void wxToolWindow::OnEraseBackground( wxEraseEvent& WXUNUSED(event) )
{
    // nothing
}

/***** Implementation for class cbMiniButton *****/

cbMiniButton::cbMiniButton()

    : mVisible( true ),
      mEnabled( true ),

      mpLayout( NULL ),
      mpPane  ( NULL ),
      mpPlugin( NULL ),
      mpWnd   ( NULL ),

      mWasClicked( false ),
      mDragStarted( false ),
      mPressed( false )
{}

void cbMiniButton::SetPos( const wxPoint& pos )
{
    mPos = pos;
}

bool cbMiniButton::HitTest( const wxPoint& pos )
{
    if ( !mVisible ) return false;

    return ( pos.x >= mPos.x && pos.y >= mPos.y &&
             pos.x < mPos.x + BTN_BOX_WIDTH     &&
             pos.y < mPos.y + BTN_BOX_HEIGHT );
}

void cbMiniButton::OnLeftDown( const wxPoint& pos )
{
    if ( !mVisible || mDragStarted ) return;

    if ( HitTest( pos ) && mEnabled )
    {
        if ( mpPlugin )
        {
            mpLayout->CaptureEventsForPane( mpPane );
            mpLayout->CaptureEventsForPlugin( mpPlugin );
        }
        else
            mpWnd->CaptureMouse();

        mDragStarted = true;
        mPressed     = true;
        mWasClicked  = false;

        Refresh();
    }
}

void cbMiniButton::OnLeftUp( const wxPoint& WXUNUSED(pos) )
{
    if ( !mVisible || !mDragStarted ) return;

    if ( mpPlugin )
    {
        mpLayout->ReleaseEventsFromPane( mpPane );
        mpLayout->ReleaseEventsFromPlugin( mpPlugin );
    }
    else
        mpWnd->ReleaseMouse();

    mWasClicked  = mPressed;
    mDragStarted = false;

    mPressed = false;
    Refresh();
}

void cbMiniButton::OnMotion( const wxPoint& pos )
{
    if ( !mVisible ) return;

    if ( mDragStarted )
    {
        mPressed = HitTest( pos );

        Refresh();
    }
}

void cbMiniButton::Refresh()
{
    if ( mpLayout )
    {
         wxClientDC dc( &mpLayout->GetParentFrame() );

         Draw( dc );
    }
    else
    {
        wxWindowDC dc( mpWnd );

        Draw( dc );
    }
}

void cbMiniButton::Draw( wxDC& dc )
{
    if ( !mVisible ) return;

    dc.SetPen( *wxTRANSPARENT_PEN );

    dc.SetBrush( *wxLIGHT_GREY_BRUSH );

    dc.DrawRectangle( mPos.x + 1, mPos.y + 1, BTN_BOX_WIDTH - 2, BTN_BOX_HEIGHT - 2 );

    // "hard-code" metafile

    if ( !mPressed )
        dc.SetPen( *wxWHITE_PEN );
    else
        dc.SetPen( *wxBLACK_PEN );

    dc.DrawLine( mPos.x, mPos.y, mPos.x + BTN_BOX_WIDTH, mPos.y );
    dc.DrawLine( mPos.x, mPos.y, mPos.x, mPos.y + BTN_BOX_HEIGHT );

    dc.SetPen( *wxGREY_PEN );

    if ( !mPressed )
    {
        dc.DrawLine( mPos.x + 1, mPos.y + BTN_BOX_HEIGHT - 2,
                     mPos.x + BTN_BOX_WIDTH - 1, mPos.y + BTN_BOX_HEIGHT - 2 );

        dc.DrawLine( mPos.x + BTN_BOX_WIDTH - 2, mPos.y + 1,
                     mPos.x + BTN_BOX_WIDTH - 2, mPos.y + BTN_BOX_HEIGHT - 1 );
    }
    else
    {
        dc.DrawLine( mPos.x + 1, mPos.y + 1,
                     mPos.x + BTN_BOX_WIDTH - 2, mPos.y + 1 );

        dc.DrawLine( mPos.x + 1, mPos.y + 1,
                     mPos.x + 1, mPos.y + BTN_BOX_HEIGHT - 2 );
    }

    if ( !mPressed )
        dc.SetPen( *wxBLACK_PEN );
    else
        dc.SetPen( *wxWHITE_PEN );

    dc.DrawLine( mPos.x, mPos.y + BTN_BOX_HEIGHT - 1,
                 mPos.x + BTN_BOX_WIDTH, mPos.y + BTN_BOX_HEIGHT - 1 );

    dc.DrawLine( mPos.x + BTN_BOX_WIDTH - 1, mPos.y ,
                 mPos.x + BTN_BOX_WIDTH - 1, mPos.y + BTN_BOX_HEIGHT );
}

bool cbMiniButton::WasClicked()
{
    return mWasClicked;
}

void cbMiniButton::Reset()
{
    mWasClicked = false;
}

/***** Implementation fro class cbCloseBox *****/

void cbCloseBox::Draw( wxDC& dc )
{
    cbMiniButton::Draw( dc );

    dc.SetPen( *wxBLACK_PEN );

    int width = BTN_BOX_WIDTH - 7;

    int xOfs = (mPressed) ? 4 : 3;
    int yOfs = (mPressed) ? 4 : 3;

    for( int i = 0; i != BTN_X_WEIGHT; ++i )
    {
        dc.DrawLine( mPos.x + xOfs + i,
                     mPos.y + yOfs,
                     mPos.x + xOfs + i + width,
                     mPos.y + yOfs + width );

        dc.DrawLine( mPos.x + xOfs + i + width - 1,
                     mPos.y + yOfs,
                     mPos.x + xOfs + i - 1,
                     mPos.y + yOfs + width );
    }
}

/***** Implementation fro class cbCollapseBox *****/

inline static void my_swap( int& a, int& b )
{
    long tmp = a;
    a = b;
    b = tmp;
}

void cbCollapseBox::Draw( wxDC& dc )
{
    cbMiniButton::Draw( dc );

    dc.SetPen( *wxTRANSPARENT_PEN );

    wxPoint arr[3];

    int yOfs  = (mPressed) ? 3 : 2;
    int xOfs  = (mPressed) ? 5 : 4;
    int width = BTN_BOX_WIDTH - 8;

    // rotating/shifting triangle inside collapse box

    arr[0].x = xOfs;
    arr[0].y = yOfs-1;
    arr[2].x = xOfs;
    arr[2].y = BTN_BOX_HEIGHT - yOfs - 1;
    arr[1].x = xOfs + width;
    arr[1].y = (arr[2].y + arr[0].y)/2;

    if ( !mIsAtLeft )
    {
        arr[0].x = BTN_BOX_WIDTH - arr[0].x;
        arr[1].x = BTN_BOX_WIDTH - arr[1].x;
        arr[2].x = BTN_BOX_WIDTH - arr[2].x;
    }

    if ( !mpPane->IsHorizontal() )
    {
        my_swap( arr[0].y, arr[0].x );
        my_swap( arr[1].y, arr[1].x );
        my_swap( arr[2].y, arr[2].x );

        arr[0].x += 1;
        arr[1].x += 1;
        arr[2].x += 1;

        //arr[1].y -= 1;
    }

    arr[0].x += mPos.x;
    arr[0].y += mPos.y;
    arr[1].x += mPos.x;
    arr[1].y += mPos.y;
    arr[2].x += mPos.x;
    arr[2].y += mPos.y;

    if ( !mEnabled ) dc.SetBrush( *wxGREY_BRUSH );
    else dc.SetBrush( *wxBLACK_BRUSH );

    dc.DrawPolygon( 3, arr );
    dc.SetBrush( wxNullBrush );
}

/***** Implementation for class cbDockBoxBox *****/

void cbDockBox::Draw( wxDC& dc )
{
    cbMiniButton::Draw( dc );

    int width = BTN_BOX_WIDTH - 7;

    int xOfs = (mPressed) ? 4 : 3;
    int yOfs = (mPressed) ? 4 : 3;

    dc.SetPen( *wxBLACK_PEN );
    dc.SetBrush( *wxBLACK_BRUSH );

    dc.DrawRectangle( mPos.x + xOfs, mPos.y + yOfs, width, width );

    xOfs += 1;
    yOfs += 1;

    dc.SetBrush( *wxWHITE_BRUSH );

    dc.DrawRectangle( mPos.x + xOfs, mPos.y + yOfs, width-2, width-2 );
}

/***** Implementation for class wxToolWindow *****/

IMPLEMENT_DYNAMIC_CLASS( cbFloatedBarWindow, wxToolWindow )

BEGIN_EVENT_TABLE( cbFloatedBarWindow, wxToolWindow )

    EVT_LEFT_DCLICK( cbFloatedBarWindow::OnDblClick )

END_EVENT_TABLE()

cbFloatedBarWindow::cbFloatedBarWindow()

    : mpBar( NULL )
{
    AddMiniButton( new cbCloseBox() );
    AddMiniButton( new cbDockBox()  );
}

void cbFloatedBarWindow::SetBar( cbBarInfo* pBar )
{
    mpBar = pBar;
}

cbBarInfo* cbFloatedBarWindow::GetBar()
{
    return mpBar;
}

void cbFloatedBarWindow::SetLayout( wxFrameLayout* pLayout )
{
    mpLayout = pLayout;
}

void cbFloatedBarWindow::PositionFloatedWnd( int scrX,  int scrY,
                                             int width, int height )
{
    wxSize minDim = GetMinimalWndDim();

    SetSize( scrX - mWndHorizGap - mClntHorizGap,
             scrY - mClntVertGap - mTitleHeight - mWndVertGap,
             width + minDim.x, height + minDim.y, 0 );
}

wxSize cbFloatedBarWindow::GetPreferredSize( const wxSize& given )
{
    if ( mpBar->mDimInfo.GetDimHandler() )
    {
        cbBarDimHandlerBase* pHandler = mpBar->mDimInfo.GetDimHandler();

        wxSize prefDim;

        // int vtad = *((int*)pHandler);

        pHandler->OnResizeBar( mpBar, given, prefDim );

        return prefDim;
    }
    else
    {
        if ( mpBar->IsFixed() )
            return mpBar->mDimInfo.mSizes[ wxCBAR_FLOATING ];
        else
            return given; // not-fixed bars are resized exactly the way user wants
    }
}

void cbFloatedBarWindow::OnMiniButtonClicked( int btnIdx )
{
    // #1 - close mini-button
    // #0 - dock mini-button

    if ( btnIdx == 0 )
    {
        mpBar->mAlignment = -1; // sepcial "marking" for hidden bars out of floated state
        mpLayout->SetBarState( mpBar, wxCBAR_HIDDEN, true );
    }
    else
        mpLayout->SetBarState( mpBar, wxCBAR_DOCKED_HORIZONTALLY, true );
}

bool cbFloatedBarWindow::HandleTitleClick( wxMouseEvent& event )
{
    ReleaseMouse();
    mMouseCaptured = false;

    wxPoint scrPos;
    GetScrMousePos( event, scrPos );

    int msX = scrPos.x,
        msY = scrPos.y;

    mpLayout->GetParentFrame().ScreenToClient( &msX, &msY );

    int x,y;
    GetPosition(&x,&y);
    int w,h;
    GetSize( &w, &h );

    wxSize minDim = GetMinimalWndDim();

    w -= minDim.x;
    h -= minDim.y;

    x += mWndHorizGap + mClntHorizGap;
    y += mWndVertGap  + mTitleHeight + mClntVertGap;

    mpLayout->GetParentFrame().ScreenToClient( &x, &y );

    wxRect& bounds = mpBar->mDimInfo.mBounds[ wxCBAR_FLOATING ];

    bounds.x = x;
    bounds.y = y;
    bounds.width  = w;
    bounds.height = h;

    cbStartBarDraggingEvent dragEvt( mpBar, wxPoint(msX,msY),
                                     mpLayout->GetPanesArray()[FL_ALIGN_TOP] );

    mpLayout->FirePluginEvent( dragEvt );

    return true;
}

void cbFloatedBarWindow::OnDblClick( wxMouseEvent& WXUNUSED(event) )
{
    mpLayout->SetBarState( mpBar, wxCBAR_DOCKED_HORIZONTALLY, true );

    //wxMessageBox("toolWnd - dblClick!");
}

