/////////////////////////////////////////////////////////////////////////////
// Name:        minifram.cpp
// Purpose:     wxMiniFrame
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "minifram.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/window.h"
#include "wx/msw/private.h"

#ifndef WX_PRECOMP
#include "wx/setup.h"
#include "wx/event.h"
#include "wx/app.h"
#include "wx/utils.h"
#endif


#ifdef __WIN32__

#include "wx/minifram.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxMiniFrame, wxFrame)
#endif


#else


#if wxUSE_ITSY_BITSY

#include "wx/minifram.h"

#ifndef __TWIN32__
#ifdef __GNUWIN32__
#ifndef wxUSE_NORLANDER_HEADERS
#include "wx/msw/gnuwin32/extra.h"
#endif
#endif
#endif

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxMiniFrame, wxFrame)
#endif

long wxMiniFrame::MSWDefWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
  if ((GetWindowStyleFlag() & wxTINY_CAPTION_HORIZ) ||
      (GetWindowStyleFlag() & wxTINY_CAPTION_VERT))
    return ::ibDefWindowProc((HWND) GetHWND(), nMsg, wParam, lParam);
  else if ( m_oldWndProc )
  	return ::CallWindowProc(CASTWNDPROC m_oldWndProc, (HWND) GetHWND(), (UINT) nMsg, (WPARAM) wParam, (LPARAM) lParam);
  else
  	return ::DefWindowProc((HWND) GetHWND(), nMsg, wParam, lParam);
}

wxMiniFrame::~wxMiniFrame(void)
{
}

/////////////////////////////////////////////////////////////////////////
//
// Project:     ItsyBitsy window support module
// Module:      itsybits.c
//
//
//    ItsyBitsy is a support module that allows you to create windows
//    that look and act very much like a popup window witha system
//    menu and caption bar, except everything is scaled to about 2/3
//    scale.
//
//    For documentation on how to use ItsyBits, read the document
//    ITSYBITS.DOC.
//
//   Revisions:
//      9/27/91     Charlie Kindel (cek/ckindel)
//                          Wrote and documented it.
//
//      1/14/93     cek         
//      2/23/93     cek     Added minimize/maximize buttons.
//      3/18/93     cek     Fixed system menu bug where system menu 
//                          popped back up if you clicked on the 
//                          icon again while it was up.
//      3/24/93     cek     More comments.  Fixed DS_MODALDIALOG style
//                          problem.  Use auto precompiled headers
//                          in MSVC.
//
//////////////////////////////////////////////////////////////////////////

#include "wx/window.h"
#include "wx/msw/private.h"

#include <string.h>

#if !defined( __WATCOMC__ ) && !defined( __MWERKS__ ) && !defined(__SALFORDC__)
#include <memory.h>
#endif

#include <stdlib.h>

#ifndef _RGB_H_
#define _RGB_H_
         
   // Some mildly useful macros for the standard 16 colors
   #define RGBBLACK     RGB(0,0,0)
   #define RGBRED       RGB(128,0,0)
   #define RGBGREEN     RGB(0,128,0)
   #define RGBBLUE      RGB(0,0,128)
   
   #define RGBBROWN     RGB(128,128,0)
   #define RGBMAGENTA   RGB(128,0,128)
   #define RGBCYAN      RGB(0,128,128)
   #define RGBLTGRAY    RGB(192,192,192)
   
   #define RGBGRAY      RGB(128,128,128)
   #define RGBLTRED     RGB(255,0,0)
   #define RGBLTGREEN   RGB(0,255,0)
   #define RGBLTBLUE    RGB(0,0,255)
   
   #define RGBYELLOW    RGB(255,255,0)
   #define RGBLTMAGENTA RGB(255,0,255)
   #define RGBLTCYAN    RGB(0,255,255)
   #define RGBWHITE     RGB(255,255,255)
#endif

#ifndef GlobalAllocPtr
#define     GlobalPtrHandle(lp)         \
                ((HGLOBAL)GlobalHandle(lp))

#define     GlobalLockPtr(lp)                \
                ((BOOL)GlobalLock(GlobalPtrHandle(lp)))
#define     GlobalUnlockPtr(lp)      \
                GlobalUnlock(GlobalPtrHandle(lp))

#define     GlobalAllocPtr(flags, cb)        \
                (GlobalLock(GlobalAlloc((flags), (cb))))
#define     GlobalReAllocPtr(lp, cbNew, flags)       \
                (GlobalUnlockPtr(lp), GlobalLock(GlobalReAlloc(GlobalPtrHandle(lp) , (cbNew), (flags))))
#define     GlobalFreePtr(lp)                \
                (GlobalUnlockPtr(lp), (BOOL)GlobalFree(GlobalPtrHandle(lp)))
#endif

#if defined(__BORLANDC__) || defined(__WATCOMC__)
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

// CAPTIONXY is the default size of the system menu icon.  This
// determines the height/width of the caption.
//
// The value that results from the following formula works out
// nicely for the veritcal caption on VGA, 8514 (Large Fonts),
// 8514 (Small Fonts), XGA (Small Fonts), XGA (Large Fonts),
// and TIGA (Small Fonts).  It may not be good on other displays.
//
// The problem is that TT fonts turn into bitmap fonts when they
// are sized below a certain threshold.  The idea is to make the
// size of the caption just big enough to get the smallest TT
// (scalable) font to fit.
//
#define CAPTIONXY (GetSystemMetrics( SM_CYCAPTION ) / 2 + 1)

#define TestWinStyle( hWnd, dwStyleBit ) \
        (((DWORD)GetWindowLong( hWnd, GWL_STYLE ) & (DWORD)dwStyleBit) ? TRUE : FALSE )
#define HASCAPTION( hwnd )    (TestWinStyle( hwnd, IBS_VERTCAPTION ) ||\
                               TestWinStyle( hwnd, IBS_HORZCAPTION ))
                               
#define SETCAPTIONSIZE(h,i)   (UINT)SetProp(h,wxT("ibSize"),(HANDLE)i)
#define GETCAPTIONSIZE(h)     (UINT)GetProp(h,wxT("ibSize"))
#define FREECAPTIONSIZE(h)    RemoveProp(h,wxT("ibSize"))

#define SETMENUWASUPFLAG(h,i) (UINT)SetProp(h,wxT("ibFlag"),(HANDLE)i)
#define GETMENUWASUPFLAG(h)   (UINT)GetProp(h,wxT("ibFlag"))
#define FREEMENUWASUPFLAG(h)  RemoveProp(h,wxT("ibFlag"))

/////////////////////////////////////////////////////////////////////
// Little known fact:
//    ExtTextOut() is the fastest way to draw a filled rectangle
//    in Windows (if you don't want dithered colors or borders).
//
//    Unfortunately there is a bug in the Windows 3.0 8514 driver
//    in using ExtTextOut() to a memory DC.  If you are drawing
//    to an off screen bitmap, then blitting that bitmap to the
//    display, do not #define wxUSE_EXTTEXTOUT below.
//
//    The following macro (DRAWFASTRECT) draws a filled rectangle
//    with no border and a solid color.  It uses the current back-
//    ground color as the fill color.
//////////////////////////////////////////////////////////////////////
#define wxUSE_EXTTEXTOUT
#ifdef wxUSE_EXTTEXTOUT
   #define DRAWFASTRECT(hdc,lprc) ExtTextOut(hdc,0,0,ETO_OPAQUE,lprc,NULL,0,NULL)
#else
   #define DRAWFASTRECT(hdc,lprc) {\
   HBRUSH hbr = CreateSolidBrush( GetBkColor( hdc ) ) ;\
   hbr = SelectObject(hdc, hbr) ;\
   PatBlt(hdc,(lprc)->left,(lprc)->top,(lprc)->right-(lprc)->left,(lprc)->bottom-(lprc)->top,PATCOPY) ;\
   hbr = SelectObject(hdc, hbr) ;\
   DeleteObject( hbr ) ;\
   }
#endif

// The DrawArrow function takes the following to indicate what
// kind of arrow to draw.
//
#define ARROW_UP        0
#define ARROW_DOWN      1
#define ARROW_RESTORE   2

BOOL PASCAL DepressMinMaxButton( HWND hWnd, UINT uiHT, LPRECT ) ;
BOOL PASCAL DoMenu( HWND hWnd ) ;
void PASCAL SetupSystemMenu( HWND hWnd, HMENU hMenu ) ;
BOOL PASCAL GetCaptionRect( HWND hWnd, LPRECT lprc ) ;
BOOL PASCAL GetIconRect( HWND hWnd, LPRECT lprc ) ;
BOOL PASCAL GetButtonRect( HWND hWnd, UINT nPos, LPRECT lprc ) ;
BOOL PASCAL GetMinButtonRect( HWND hWnd, LPRECT lprc ) ;
BOOL PASCAL GetMaxButtonRect( HWND hWnd, LPRECT lprc ) ;
BOOL PASCAL DrawCaption( HDC hDC, HWND hWnd, LPRECT lprc,
                              BOOL fVert, BOOL fSysMenu,
                              BOOL fMin, BOOL fMax, BOOL fActive ) ;
void PASCAL DrawSysMenu( HDC hDC, HWND hWnd, BOOL fInvert ) ;
void PASCAL DrawButton( HDC hDC, HWND hWnd, BOOL fMin, BOOL fDepressed ) ;
void PASCAL DrawArrow( HDC hdc, LPRECT lprc, UINT uiStyle ) ;

// Global vars
//
static BOOL fWin31 ;

///////////////////////////////////////////////////////////////////////
// External/Public functions
///////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//  UINT WINAPI ibGetCaptionSize( HWND hWnd )
//
//  Description: 
//
//    Gets the size of the caption (height if horz, width if
//    vertical).
//
//  Comments:
//
///////////////////////////////////////////////////////////////
UINT WINAPI ibGetCaptionSize( HWND hWnd  )
{
    return GETCAPTIONSIZE( hWnd ) + 1 ;
} // ibSetCaptionSize()

/////////////////////////////////////////////////////////////////
//  UINT WINAPI ibSetCaptionSize( HWND hWnd, UINT nSize )
//
//  Description: 
//
//    Changes the size of the caption (height if horz, width if
//    vertical).
//
//  Comments:
//
//////////////////////////////////////////////////////////////////
UINT WINAPI ibSetCaptionSize( HWND hWnd, UINT nSize )
{
    UINT ui ;

    if (nSize <= 0)
        return 0 ;

    nSize-- ;
    ui = SETCAPTIONSIZE( hWnd, nSize ) ;

    // Once we change the window style, we need a WM_NCCALCRECT
    // to be generated.
    //
    // SWP_FRAMECHANGED is not documented in the 3.1 SDK docs,
    // but *is* in WINDOWS.H.
    //
    SetWindowPos( hWnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | 
                        SWP_NOSIZE | SWP_NOMOVE | 
                        SWP_NOACTIVATE | SWP_NOZORDER) ;

    return ui ;

} // ibSetCaptionSize()

/////////////////////////////////////////////////////////////////
//  LRESULT WINAPI ibDefWindowProc( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
//
//  Description: 
//
//    This function should be called instead of DefWindowProc() for
//    windows that want to have itsybitsy captions.
//
//  Comments:
//
//////////////////////////////////////////////////////////////////
LRESULT WINAPI ibDefWindowProc( HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam )
{
    LRESULT lRet ;
    UINT  nCapSize ;

    switch( uiMsg )
    {
        case WM_SYSCHAR:
            // If ALT-SPACE 
            // was hit then pop up the menu
            // 
            if (HASCAPTION( hWnd ) && (wParam == VK_SPACE))
            {
                DoMenu( hWnd ) ;
                break ;
            }

            // FALL THROUGH!!!!
            // 

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            DWORD dw = GetWindowLong( hWnd, GWL_STYLE ) ;

            // Fool DefWindowProc into thinking we do not have
            // a system menu.  Otherwise it will try to
            // pop up its own.
            // 
            SetWindowLong( hWnd, GWL_STYLE, dw &~WS_SYSMENU ) ;
            lRet = DefWindowProc( hWnd, uiMsg, wParam, lParam ) ;
            SetWindowLong( hWnd, GWL_STYLE, dw ) ;
            return lRet ;
        }
        break ;
        
        case WM_COMMAND:
            // The menu that is poped up for the system menu with
            // TrackPopupMenu() sends it's notifications as WM_COMMAND
            // messages.  We need to translate these into
            // WM_SYSCOMMAND messages.  All standard WM_SYSCOMMAND
            // ids are greater than 0xF000.
            //
            // This could be a possible cause of confusion if the
            // itsybitsy window had children that used SC_MOVE or SC_CLOSE
            // as their IDs.  Take note and be careful.
            //
            // Also, because ibDefWindowProc looks at WM_COMMAND messages,
            // you will need to be careful to call ibDefWindowProc() for
            // any wm_command messages that you would normally ignore.
            // Otherwise the system menu won't work.
            //
            if (wParam >= 0xF000)
                // Call PostMessage() here instead of SendMessage!
                // Here's why:
                //      Our menu was created by TrackPopupMenu().  TPM() does
                //      not return until after the menu has been destroyed 
                //      (and thus the command associated with the menu selection
                //      sent).  Therefore when we get here, we are still
                //      *inside* TPM().  If we Send WM_SYSCOMMAND, SC_CLOSE
                //      to the window, the window will be destroyed before
                //      TPM() returns to our code within DoMenu() below.  Wel...
                //      We do stuff with the window handle after DoMenu()
                //      returns (namely GetProp()).  Since the window has
                //      been destroyed,this is bad.  
                PostMessage( hWnd, WM_SYSCOMMAND, wParam, lParam ) ;

        return DefWindowProc( hWnd, uiMsg, wParam, lParam ) ;

        case WM_GETMINMAXINFO:
        {
            nCapSize = GETCAPTIONSIZE( hWnd ) ;
            if (HASCAPTION( hWnd ) && TestWinStyle( hWnd, WS_THICKFRAME ))
            {
                LPPOINT lppt = (LPPOINT)lParam ;
                RECT    rcMenu ;
                RECT    rcMin ;
                RECT    rcMax ;
                int     nX ;
                int     cx, cy ;    // window frame/border width

                if (TestWinStyle( hWnd, WS_THICKFRAME ))
                {
                    cx = GetSystemMetrics( SM_CXFRAME ) ;
                    cy = GetSystemMetrics( SM_CYFRAME ) ;
                }
                else if (TestWinStyle(hWnd, WS_BORDER ))
                {
                    cx = GetSystemMetrics( SM_CXBORDER ) ;
                    cy = GetSystemMetrics( SM_CYBORDER ) ;
                }
                else
                {
                    // VZ: I don't know what should be here, but the vars must
                    //     be inited!
                    wxFAIL_MSG(wxT("don't know how to initialize cx, cy"));

                    cx = cy = 0;
                }
                                
                GetIconRect( hWnd, &rcMenu ) ;
                GetMinButtonRect( hWnd, &rcMin ) ;
                GetMaxButtonRect( hWnd, &rcMax ) ;
                nX = (rcMenu.right-rcMenu.left) + 
                    (rcMin.right-rcMin.left) +
                    (rcMin.right-rcMin.left) ;


                if (TestWinStyle( hWnd, IBS_VERTCAPTION ) )
                {
                    lppt[3].x = nCapSize + cx * 2 - 1 ;
                    lppt[3].y = nX + (2* nCapSize) ;
                }
                else
                {
                    lppt[3].x = nX + (2* nCapSize) ;
                    lppt[3].y = nCapSize + cy * 2 - 1 ;
                }
            }
            return DefWindowProc( hWnd, uiMsg, wParam, lParam ) ;
        }
        break ;

        /////////////////////////////////////////////////////////////////////
        // Non-client area messages.  These are used to allow the
        // minature caption bar to be handled correctly.
        //
        case WM_NCCREATE:
        {
            DWORD dwStyle ;
            
            // We have two things that we need to store somewhere:
            //     1) The caption height (width).
            // and 2) A flag indicating whether the sysmenu was
            //        just up or not.
            // 
            // CAPTIONXY is a macro that calls GetSystemMetrics.
            //
            SETCAPTIONSIZE( hWnd, CAPTIONXY ) ;

            // Set our flag that tells us whether the system menu was
            // 'just up'. 
            //
            SETMENUWASUPFLAG( hWnd, FALSE ) ;

            // Are we in 3.1?  If so we have some neat features
            // we can use like rotated TrueType fonts.
            //
            fWin31 = (BOOL)(LOWORD( GetVersion() ) >= 0x030A) ;

            // If IBS_????CAPTION was specified and the WS_DLGFRAME (or
            // WS_DLGFRAME 'cause it == WS_CAPTION | WS_BORDER)
            // was specified the creator made a mistake.  Things get really
            // ugly if DefWindowProc sees WS_DLGFRAME, so we strip
            // the WS_DLGFRAME part off!
            //
            dwStyle = GetWindowLong( hWnd, GWL_STYLE ) ;
            if ((dwStyle & IBS_VERTCAPTION || dwStyle & IBS_HORZCAPTION) &&
                 dwStyle & WS_DLGFRAME)
            {
                dwStyle &= (DWORD)~WS_DLGFRAME ;
                SetWindowLong( hWnd, GWL_STYLE, dwStyle ) ;
            }
        }
        return DefWindowProc( hWnd, uiMsg, wParam, lParam ) ;

        case WM_NCDESTROY:
            // We store the caption size in a window prop. so we
            // must remove props.
            //
            FREECAPTIONSIZE( hWnd ) ;
            FREEMENUWASUPFLAG( hWnd ) ;
        return DefWindowProc( hWnd, uiMsg, wParam, lParam ) ;

        case WM_NCCALCSIZE:
            // This is sent when the window manager wants to find out
            // how big our client area is to be.  If we have a mini-caption
            // then we trap this message and calculate the cleint area rect,
            // which is the client area rect calculated by DefWindowProc()
            // minus the width/height of the mini-caption bar
            //
            lRet = DefWindowProc( hWnd, uiMsg, wParam, lParam ) ;
            if (!IsIconic( hWnd ) && HASCAPTION( hWnd ))
            {
                nCapSize = GETCAPTIONSIZE( hWnd ) ;

                if (TestWinStyle( hWnd, IBS_VERTCAPTION ) )
                    ((LPRECT)lParam)->left += nCapSize ;
                else
                    ((LPRECT)lParam)->top += nCapSize ;
            }
        return lRet ;

        case WM_NCHITTEST:
            // This message is sent whenever the mouse moves over us.
            // We will depend on DefWindowProc for everything unless
            // there is a mini-caption, in which case we will
            // return HTCAPTION or HTSYSMENU.  When the user clicks
            // or double clicks, NC_LBUTTON/ message are sent with
            // wParam equal to what we return here.
            // This means that this is an ideal place to figure out
            // where we are!
            //
            // let defwindowproc handle the standard borders etc...
            //
            lRet = DefWindowProc( hWnd, uiMsg, wParam, lParam ) ;
            if (!IsIconic( hWnd ) && HASCAPTION( hWnd ) && lRet == HTNOWHERE)
            {
                RECT  rc ;
                RECT  rcMenu ;
                RECT  rcMinButton ;
                RECT  rcMaxButton ;
                POINT pt ;

                nCapSize = GETCAPTIONSIZE( hWnd ) ;

                // if DefWindowProc returned HTCAPTION then we have to
                // refine the area and return HTSYSMENU if appropriate
                //
                pt.x = LOWORD( lParam ) ;
                pt.y = HIWORD( lParam ) ;

                GetCaptionRect( hWnd, &rc ) ;  // window coords
                if (PtInRect( &rc, pt ))
                {
                     lRet = HTCAPTION ;

                     // rely on the fact that Get???Rect() return an invalid 
                     // (empty) rectangle if the menu/buttons don't exist
                     //
                     GetIconRect( hWnd, &rcMenu ) ;
                     GetMinButtonRect( hWnd, &rcMinButton ) ;
                     GetMaxButtonRect( hWnd, &rcMaxButton ) ;
     
                     if (PtInRect( &rcMenu, pt ))
                          lRet = HTSYSMENU ;

                     if (PtInRect( &rcMinButton, pt ))
                          lRet = HTMINBUTTON ;
                     else
                          if (PtInRect( &rcMaxButton, pt ))
                                lRet = HTMAXBUTTON ;
                }
            }
            if (lRet != HTSYSMENU)
                SETMENUWASUPFLAG( hWnd, FALSE ) ;
        return lRet ;

        case WM_NCLBUTTONDBLCLK:
            // Windows recieve WM_NC?BUTTONDBLCLK messages whether they
            // have CS_DBLCLKS or not.  We watch for one of these
            // to see if the user double clicked on the system menu (to
            // close the window) or on the caption (to maximize the window).
            //
            if (!IsIconic( hWnd ) && HASCAPTION( hWnd ) && wParam == HTSYSMENU)
            {
                SendMessage( hWnd, WM_CLOSE, 0, 0L ) ;
                break ;
            }
        return DefWindowProc( hWnd, uiMsg, wParam, lParam ) ;

        case WM_NCLBUTTONDOWN:
        {
            RECT rc ;

            // If we're iconic or we don't have a caption then 
            // DefWindowProc will do the job just fine.
            //            
            if (IsIconic( hWnd ) || !HASCAPTION( hWnd ))
                 return DefWindowProc( hWnd, uiMsg, wParam, lParam ) ;

            // Here's were we handle the system menu, the min and max buttons.
            // If you wanted to change the behavior of the min/max buttons
            // do something like swap tool palettes or something, you 
            // would change the SendMessage() calls below.
            //
            switch (wParam)
            {
                case HTSYSMENU:
                    if (GETMENUWASUPFLAG( hWnd ) == FALSE && DoMenu( hWnd ))
                        SETMENUWASUPFLAG( hWnd, TRUE ) ;
                    else
                        SETMENUWASUPFLAG( hWnd, FALSE ) ;
                break ;
                
                case HTMINBUTTON:
                    GetMinButtonRect( hWnd, &rc ) ;
                    // Note that DepressMinMaxButton() goes into
                    // a PeekMessage() loop waiting for the mouse
                    // to come back up.
                    //
                    if (DepressMinMaxButton( hWnd, wParam, &rc ))
                        SendMessage( hWnd, WM_SYSCOMMAND, SC_MINIMIZE, lParam ) ;
                break ;
                
                case HTMAXBUTTON:
                    GetMaxButtonRect( hWnd, &rc ) ;
                    // Note that DepressMinMaxButton() goes into
                    // a PeekMessage() loop waiting for the mouse
                    // to come back up.
                    //
                    if (DepressMinMaxButton( hWnd, wParam, &rc ))
                    {
                        if (IsZoomed(hWnd))
                            SendMessage( hWnd, WM_SYSCOMMAND, SC_RESTORE, lParam ) ;
                        else
                            SendMessage( hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, lParam ) ;
                    }
                break ;
                
                default:
                    // Well, it appears as though the user clicked somewhere other
                    // than the buttons.  We let DefWindowProc do it's magic.
                    // This is where things like dragging and sizing the
                    // window happen.
                    //
                    return DefWindowProc( hWnd, uiMsg, wParam, lParam ) ;
            }
        }
        break ;

        case WM_NCPAINT:
        case WM_NCACTIVATE:
            if (IsIconic( hWnd ))
                return DefWindowProc( hWnd, uiMsg, wParam, lParam ) ;

            // Paint the non-client area here.  We will call DefWindowProc
            // after we are done so it can paint the borders and so
            // forth...
            //
            lRet = DefWindowProc( hWnd, uiMsg, wParam, lParam ) ;
            if (HASCAPTION( hWnd ))
            {
                RECT  rcCap ;
                RECT  rc ;
                HDC    hDC = GetWindowDC( hWnd ) ;
                BOOL  fActive ;

                GetCaptionRect( hWnd, &rcCap ) ;    // Convert to window coords
                GetWindowRect( hWnd, &rc ) ;
                OffsetRect( &rcCap, -rc.left, -rc.top ) ;

                if (uiMsg == WM_NCPAINT)
                    fActive = (hWnd == GetActiveWindow()) ;
                else
                    fActive = wParam ;

                DrawCaption( hDC, hWnd, &rcCap,
                                        TestWinStyle(hWnd, IBS_VERTCAPTION),
                                        TestWinStyle(hWnd, WS_SYSMENU),
                                        TestWinStyle(hWnd, WS_MINIMIZEBOX),
                                        TestWinStyle(hWnd, WS_MAXIMIZEBOX),
                                        fActive ) ;

                ReleaseDC( hWnd, hDC ) ;
            }
            return lRet;
        break;

        default:
            return DefWindowProc( hWnd, uiMsg, wParam, lParam ) ;
    }

    return 0L ;

} // ibDefWindowProc()

//  ibAdjustWindowRect( HWND hWnd, LPRECT lprc )
//
//    Does the same thing as the USER function AdjustWindowRect(),
//    but knows about itsybitsy windows.  AdjustWindowRect() is
//    bogus for stuff like this.
//
void WINAPI ibAdjustWindowRect( HWND hWnd, LPRECT lprc )
{
    short    cx = 0, cy = 0 ;
    UINT     nCapSize ;
    
    nCapSize = GETCAPTIONSIZE( hWnd ) ;
    
    // First check Windows's styles, then our own.
    //
    if (TestWinStyle( hWnd, WS_THICKFRAME ))
    {
        cx = GetSystemMetrics( SM_CXFRAME ) ;
        cy = GetSystemMetrics( SM_CYFRAME ) ;
    }
    else
        if (TestWinStyle(hWnd, DS_MODALFRAME ))
        {
            cx = GetSystemMetrics( SM_CXDLGFRAME ) + GetSystemMetrics( SM_CXBORDER ) ;
            cy = GetSystemMetrics( SM_CYDLGFRAME ) + GetSystemMetrics( SM_CYBORDER ) ;
        }
        else
            if (TestWinStyle(hWnd, WS_BORDER ))
            {
                cx = GetSystemMetrics( SM_CXBORDER ) ;
                cy = GetSystemMetrics( SM_CYBORDER ) ;
            }
    
    InflateRect( lprc, cx, cy ) ;
    
    if (TestWinStyle( hWnd, IBS_VERTCAPTION ))
        lprc->left -= nCapSize ;
    else
        if (TestWinStyle( hWnd, IBS_HORZCAPTION ))
            lprc->top -= nCapSize ;

} // ibAdjustWindowRect() 


///////////////////////////////////////////////////////////////////////
// Internal functions
///////////////////////////////////////////////////////////////////////

// DepressMinMaxButton()
//
// This function is called when the user has pressed either the min or
// max button (i.e. WM_NCLBUTTONDOWN).  We go into a Peekmessage() loop,
// waiting for the mouse to come back up.  This allows us to make the
// button change up/down state like a real button does.
//
// lprc points to the rectangle that describes the button the
// user has clicked on.
//
BOOL PASCAL DepressMinMaxButton( HWND hWnd, UINT uiHT, LPRECT lprc )
{
    BOOL    fDepressed = TRUE ;
    MSG     msg ;
                      
    // Draw button in down state
    DrawButton( NULL, hWnd, uiHT == HTMINBUTTON, fDepressed ) ;
    SetCapture( hWnd ) ;
    
    while (TRUE)
    {
        if (PeekMessage((LPMSG)&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE))
        {
            switch (msg.message)
            {
                case WM_LBUTTONUP:
                    if (fDepressed)
                        DrawButton( NULL, hWnd, uiHT == HTMINBUTTON, !fDepressed ) ;
                    ReleaseCapture();
                return PtInRect( lprc, msg.pt ) ;
                
                case WM_MOUSEMOVE:
                    if (PtInRect( lprc, msg.pt ))
                    {
                        if (!fDepressed)
                            DrawButton( NULL, hWnd, uiHT == HTMINBUTTON, fDepressed = TRUE ) ;
                    }
                    else
                    {
                        if (fDepressed)
                            DrawButton( NULL, hWnd, uiHT == HTMINBUTTON, fDepressed = FALSE ) ;
                    }
                break;
            }
        }
    }
    
} // DepressMinMaxButton()  

// DrawCaption( HDC hDC, HWND hWnd, LPRECT lprc,
//                   BOOL fVert, BOOL fSysMenu, BOOL fActive )
//
//    This function draws an itsy bitsy caption bar with or
//    without system menu to the dc specified by hDC.  The
//    caption is drawn to fit within the lprc RECT and is
//    drawn//withOut/ borders.
//
BOOL PASCAL DrawCaption( HDC hDC, HWND hWnd, LPRECT lprc,
                              BOOL fVert, BOOL fSysMenu, BOOL fMin,
                              BOOL fMax, BOOL fActive )
{
    RECT        rc ;
    RECT        rcCap ;
    COLORREF    rgbCaptionBG ;
    COLORREF    rgbText ;
    COLORREF    rgbWindowFrame ;
    HBRUSH      hbrCaption ;
    UINT        ui ;
    UINT        nCapSize ;
    
    nCapSize = GETCAPTIONSIZE( hWnd ) ;
    
    // Get the colors.
    //
    rgbWindowFrame = GetSysColor( COLOR_WINDOWFRAME ) ;
    
    // if we have focus use the active caption color
    // otherwise use the inactive caption color
    //
    if (fActive)
    {
        rgbText = GetSysColor( COLOR_CAPTIONTEXT ) ;
        rgbCaptionBG = GetSysColor( COLOR_ACTIVECAPTION ) ;
    }
    else
    {
        if (fWin31)
            rgbText = GetSysColor( COLOR_INACTIVECAPTIONTEXT ) ;
        else
            rgbText = GetSysColor( COLOR_CAPTIONTEXT ) ;

        rgbCaptionBG = GetSysColor( COLOR_INACTIVECAPTION )  ;
    }

    SetBkMode( hDC, TRANSPARENT ) ;
    SelectObject( hDC, GetStockObject( NULL_BRUSH ) ) ;
    SelectObject( hDC, GetStockObject( NULL_PEN ) ) ;
    
    rcCap = *lprc ;
    
    if (fSysMenu)
    {
        if (fVert)
            rcCap.top += nCapSize ;
        else
            rcCap.left += nCapSize ;
    }
    
    if (fMax)
    {
        if (fVert)
            rcCap.bottom -= nCapSize ;
        else
            rcCap.right -= nCapSize ;
    }
    
    if (fMin)
    {
        if (fVert)
            rcCap.bottom -= nCapSize ;
        else
            rcCap.right -= nCapSize ;
    }
    
    if (fVert)
    {
        rc.left = lprc->right - 1 ;
        rc.right = lprc->right ;
        rc.top = lprc->top ;
        rc.bottom = lprc->bottom ;
    }
    else
    {
        rc.left = lprc->left ;
        rc.right = lprc->right ;
        rc.bottom = lprc->bottom ;
        rc.top = rc.bottom - 1 ;
    }
    
    SetBkColor( hDC, rgbWindowFrame ) ;
    DRAWFASTRECT( hDC, &rc ) ;

    hbrCaption = CreateSolidBrush( rgbCaptionBG ) ;
    hbrCaption = (HBRUSH) SelectObject( hDC, hbrCaption ) ;
    SelectObject( hDC, (HPEN) GetStockObject( NULL_PEN ) ) ;
    if (fVert)
        Rectangle( hDC, rcCap.left, rcCap.top, rcCap.right, rcCap.bottom + 1 ) ;
    else
        Rectangle( hDC, rcCap.left, rcCap.top, rcCap.right+1, rcCap.bottom ) ;
    hbrCaption = (HBRUSH) SelectObject( hDC, hbrCaption ) ;
    DeleteObject( hbrCaption ) ;

    
    // Draw caption text here.  Only do it in 3.1 'cause 3.1 gives
    // us 'small fonts'.
    //
    ui = GetWindowTextLength( hWnd ) ;
    if (fWin31)
    {
        HFONT          hFont ;
        LPTSTR         lpsz ;
        LOGFONT        lf ;
        TEXTMETRIC     tm ;
        int            cx ;
        int            cy ;
        SIZE           Size ;
        
        lpsz = (wxChar*)GlobalAllocPtr( GHND, (ui + 2) * sizeof(wxChar) );
        if (lpsz)
        {
            UINT    nBkMode ;

            GetWindowText( hWnd, lpsz, ui + 1 ) ;
            nBkMode = SetBkMode( hDC, TRANSPARENT ) ;
            rgbText = SetTextColor( hDC, rgbText ) ;
            
            memset( &lf, '\0', sizeof(LOGFONT) ) ;

            lf.lfHeight = -(int)(nCapSize - 3) ;
            lf.lfCharSet = ANSI_CHARSET ;
            lf.lfQuality = DEFAULT_QUALITY ;
            lf.lfClipPrecision = CLIP_LH_ANGLES | CLIP_STROKE_PRECIS ;
            if (nCapSize >= 20)
            {
                lf.lfWeight = FW_BOLD ;
            }
            
            if (fVert)
            {
                // Can only rotate true type fonts (well, ok, we could
                // try and use "modern").
                wxStrcpy( lf.lfFaceName, wxT("Arial") ) ;
                lf.lfPitchAndFamily = FF_SWISS | 0x04;
                lf.lfEscapement = 900 ;

                // Note:  The Win 3.1 documentation for CreateFont() say's
                // that the lfOrientation member is ignored.  It appears,
                // that on Windows 16 3.1 this is true, but when running
                // as a 16 bit WinApp on WindowsNT 3.1 the lfOrientation
                // must be set or the text does not rotate!
                //
                lf.lfOrientation = 900 ;
                
                hFont = CreateFontIndirect( &lf ) ;
                hFont = (HFONT) SelectObject( hDC, hFont ) ;
                
                GetTextExtentPoint( hDC, lpsz, ui, &Size ) ;
                cx = rcCap.bottom - ((rcCap.bottom - rcCap.top - Size.cx) / 2) ;
                cy = rcCap.left - 1 + ((rcCap.right - rcCap.left - Size.cy) / 2) ;

                // Make sure we got a rotatable font back.
                //
                GetTextMetrics( hDC, &tm ) ;
                if (tm.tmPitchAndFamily & TMPF_VECTOR     ||
                    tm.tmPitchAndFamily & TMPF_TRUETYPE)
                {
                    ExtTextOut( hDC,
                               cy,
                               min( (long)cx, rcCap.bottom),
                               ETO_CLIPPED, &rcCap,
                               lpsz, ui, NULL ) ;
                }
                
                hFont = (HFONT) SelectObject( hDC, hFont ) ;
                DeleteObject( hFont ) ;
            }
            else
            {
                // Use small fonts always for the horizontal. Cause it looks
                // more like "System" than Arial.
                //
                lf.lfPitchAndFamily = FF_SWISS ;
                
                hFont = CreateFontIndirect( &lf ) ;
                hFont = (HFONT) SelectObject( hDC, hFont ) ;
                
                GetTextExtentPoint( hDC, lpsz, ui, &Size ) ;
                cx = rcCap.left + ((rcCap.right - rcCap.left - Size.cx) / 2) ;
                cy = rcCap.top + ((rcCap.bottom - rcCap.top - Size.cy) / 2) ;
                
                // Figger out how big the string is
                //
                ExtTextOut( hDC,
                            max( (long)cx, rcCap.left ),
                            cy,
                            ETO_CLIPPED, &rcCap,
                            lpsz, ui, NULL ) ;
                
                hFont = (HFONT) SelectObject( hDC, hFont ) ;
                DeleteObject( hFont ) ;
            }

            // Unsetup the DC
            //
            rgbText = SetTextColor( hDC, rgbText ) ;
            SetBkMode( hDC, nBkMode ) ;
            
#ifdef __WIN16__
            GlobalFreePtr( (unsigned int)(DWORD) lpsz ) ;
#else
            GlobalFreePtr( lpsz ) ;
#endif
        }
    }
    
    if (fSysMenu)
        DrawSysMenu( hDC, hWnd, FALSE ) ;
    
    if (fMin)
        DrawButton( hDC, hWnd, TRUE, FALSE ) ;

    if (fMax)
        DrawButton( hDC, hWnd, FALSE, FALSE ) ;
    
    return TRUE ;
    
} // DrawCaption()


// DrawSysMenu( HDC hDC, hWnd, BOOL fInvert )
//
// Draws the little system menu icon.
//
void PASCAL DrawSysMenu( HDC hDC, HWND hWnd, BOOL fInvert )
{
    RECT         rcIcon ;
    RECT         rcTemp ;
    RECT         rc ;
    COLORREF     rgbIconFace ;
    COLORREF     rgbWindowFrame ;
    BOOL         fDC ;
    UINT         nCapSize ;
        
    nCapSize = GETCAPTIONSIZE( hWnd ) ;
    
    if (!hDC)
    {
        fDC = TRUE ;
        hDC = GetWindowDC( hWnd ) ;
    }
    else
        fDC = FALSE ;
    
    if (hDC)
    {
        rgbIconFace    = GetNearestColor( hDC, RGBLTGRAY ) ;
        rgbWindowFrame = GetSysColor( COLOR_WINDOWFRAME ) ;
        
        GetIconRect( hWnd, &rcIcon ) ;
        GetWindowRect( hWnd, &rc ) ;
           
        OffsetRect( &rcIcon, -rc.left, -rc.top ) ;
        
        rcTemp = rcIcon ;
        
        if (TestWinStyle( hWnd, IBS_VERTCAPTION ))
        {
            rc = rcIcon ;      // separator line
            rc.top = ++rc.bottom - 1 ;
        }
        else
        {
            rc = rcIcon ;      // separator line
            rc.left = ++rc.right - 1 ;
        }
        
        // Fill
        SetBkColor( hDC, rgbIconFace ) ;
        DRAWFASTRECT( hDC, &rcTemp ) ;
        
        // Draw separator line
        SetBkColor( hDC, rgbWindowFrame ) ;
        DRAWFASTRECT( hDC, &rc ) ;
        
        if (nCapSize > 4)
        {
            // Draw the little horizontal doo-hickey
            //
            rcTemp.top = rcIcon.top + ((nCapSize-1) / 2) ;
            rcTemp.bottom = rcTemp.top + 3 ;
            rcTemp.left  = rcTemp.left + 3 ; 
            rcTemp.right = rcTemp.right - 1 ;
                
            SetBkColor( hDC, RGBGRAY ) ;
            DRAWFASTRECT( hDC, &rcTemp ) ;
                
            rc = rcTemp ;
            OffsetRect( &rc, -1, -1 ) ;
            SetBkColor( hDC, RGBBLACK ) ;
            DRAWFASTRECT( hDC, &rc ) ;
                
            InflateRect( &rc, -1, -1 ) ;
            SetBkColor( hDC, RGBWHITE ) ;
            DRAWFASTRECT( hDC, &rc ) ;
        }
        
        if (fInvert)
            InvertRect( hDC, &rcIcon ) ;
    
        if (fDC)
            ReleaseDC( hWnd, hDC ) ;
    }

} // DrawSysMenu()

// DoMenu( HWND hWnd )
//
//    Pops up the system menu.
//
BOOL PASCAL DoMenu( HWND hWnd )
{
    HDC    hDC ;
    RECT   rcIcon ;
    RECT   rc ;
    POINT  pt ;
    HMENU  hMenu ;
    DWORD  dw ;

    if (!TestWinStyle(hWnd, WS_SYSMENU))
        return FALSE ;
    
    hDC = GetWindowDC( hWnd );
    if (hDC)
    {
        // Invert the icon
        //
        DrawSysMenu( hDC, hWnd, TRUE ) ;
        
        // Pop up the menu
        //
        if (TestWinStyle( hWnd, IBS_VERTCAPTION ))
        {
            pt.x = -1 ;
            pt.y = 0 ;
        }
        else
        {
            pt.x = 0 ;
            pt.y = -1 ;
        }
        
        GetIconRect( hWnd, &rcIcon ) ;
        GetWindowRect( hWnd, &rc ) ;
        OffsetRect( &rcIcon, -rc.left, -rc.top ) ;
        
        ClientToScreen( hWnd, &pt ) ;
        ClientToScreen( hWnd, (LPPOINT)&rc.right ) ;
        
        dw = GetWindowLong( hWnd, GWL_STYLE ) ;
        SetWindowLong( hWnd, GWL_STYLE, dw | WS_SYSMENU ) ;
        
        hMenu = GetSystemMenu( hWnd, FALSE ) ;
        SetupSystemMenu( hWnd, hMenu ) ;
        
        SetWindowLong( hWnd, GWL_STYLE, dw ) ;

        TrackPopupMenu( hMenu, 0, //TPM_LEFTALIGN,
                    pt.x,
                    pt.y,
                    0,
                    hWnd,
                    &rc ) ;
    
        DrawSysMenu( hDC, hWnd, FALSE ) ;
        ReleaseDC( hWnd, hDC ) ;
    }
    return TRUE ;

} // DoMenu()

// SetupSystemMenu( HWND hWnd, HMENU hMenu )
//
//    Enables/Disables the appropriate menu items on the
//    menu passed for the window passed.
//
void PASCAL SetupSystemMenu( HWND hWnd, HMENU hMenu )
{
    UINT     wMove ;        
    UINT     wSize ;
    UINT     wMinBox ;
    UINT     wMaxBox ;
    UINT     wRestore ;
    
    // Assume all should be grayed.
    //
    wSize = wMove = wMinBox = wMaxBox = wRestore = MF_GRAYED ;
    
    if (TestWinStyle( hWnd, WS_MAXIMIZEBOX ) || IsIconic( hWnd ))
        wMaxBox = MF_ENABLED ;
    
    if (TestWinStyle( hWnd, WS_MINIMIZEBOX ))
        wMinBox = MF_ENABLED ;
    
    if (IsZoomed( hWnd ))
        wRestore = MF_ENABLED ;
    
    if (TestWinStyle( hWnd, WS_THICKFRAME ) &&
        !(IsIconic( hWnd ) || IsZoomed( hWnd )))
        wSize = MF_ENABLED ;
    
    if (!IsZoomed( hWnd ) &&
        !IsIconic( hWnd ) &&
        TestWinStyle( hWnd, WS_CAPTION ) )
        wMove = MF_ENABLED ;
    
    EnableMenuItem( hMenu, SC_MOVE,     wMove ) ;
    EnableMenuItem( hMenu, SC_SIZE,     wSize ) ;
    EnableMenuItem( hMenu, SC_MINIMIZE, wMinBox ) ;
    EnableMenuItem( hMenu, SC_MAXIMIZE, wMaxBox ) ;
    EnableMenuItem( hMenu, SC_RESTORE,  wRestore ) ;
                        
} // SetupSystemMenu()

// GetCaptionRect( HWND hWnd, LPRECT lprc )
//
//    calcluales the rectangle of the mini-caption in screen coords.
//
BOOL PASCAL GetCaptionRect( HWND hWnd, LPRECT lprc )
{
    UINT     nCapSize ;
    
    nCapSize = GETCAPTIONSIZE( hWnd ) ;
    
    if (!HASCAPTION( hWnd ))
    {
        SetRectEmpty( lprc ) ;
        return FALSE ;
    }
    
    GetWindowRect( hWnd, lprc ) ;
    
    // the window might have other non-client components like
    // borders 
    //
    if (TestWinStyle( hWnd, WS_THICKFRAME ))
    {
        lprc->left += GetSystemMetrics( SM_CXFRAME ) ;
        lprc->top  += GetSystemMetrics( SM_CYFRAME ) ;
        lprc->right -= GetSystemMetrics( SM_CXFRAME ) ;
        lprc->bottom -= GetSystemMetrics( SM_CYFRAME ) ;
    }
    else
        if (TestWinStyle( hWnd, DS_MODALFRAME ))  // if it's a dialog box
        {   
            lprc->left += GetSystemMetrics( SM_CXDLGFRAME ) + GetSystemMetrics( SM_CXBORDER ) ;
            lprc->top  += GetSystemMetrics( SM_CYDLGFRAME ) + GetSystemMetrics( SM_CYBORDER ) ;
            lprc->right -= GetSystemMetrics( SM_CXDLGFRAME ) + GetSystemMetrics( SM_CXBORDER ) ;
            lprc->bottom -= GetSystemMetrics( SM_CYDLGFRAME ) + GetSystemMetrics( SM_CYBORDER ) ;
        }
        else
            if (TestWinStyle( hWnd, WS_BORDER ))
            {   
                lprc->left += GetSystemMetrics( SM_CXBORDER ) ;
                lprc->top  += GetSystemMetrics( SM_CYBORDER ) ;
                lprc->right -= GetSystemMetrics( SM_CXBORDER ) ;
                lprc->bottom -= GetSystemMetrics( SM_CYBORDER ) ;
            }
    
    if (TestWinStyle( hWnd, IBS_VERTCAPTION ))
        lprc->right = lprc->left + nCapSize ;
    else
        lprc->bottom = lprc->top + nCapSize ;
    
    return TRUE ;
} // GetCaptionRect()

// GetIconRect( HWND hWnd, LPRECT lprc )
//
//    Calculates the rect of the icon in screen coordinates.
//
BOOL PASCAL GetIconRect( HWND hWnd, LPRECT lprc )
{
    UINT    nCapSize ;
    BOOL    fMenu, fVert ;

    fMenu= TestWinStyle( hWnd, WS_SYSMENU ) ;
    fVert = TestWinStyle( hWnd, IBS_VERTCAPTION ) ;

    if (!GetCaptionRect( hWnd, lprc ))   // window coords
      return FALSE ;

    if (!fMenu)
    {
        SetRectEmpty( lprc ) ;
        return FALSE ;
    }

    nCapSize = GETCAPTIONSIZE( hWnd ) ;

    if (fVert)
        lprc->bottom = lprc->top + nCapSize ;
    else
        lprc->right = lprc->left + nCapSize ;
    
    lprc->bottom-- ;
    lprc->right-- ;
    
    return TRUE ;

} // GetIconRect()

// GetMinButtonRect()
//
// Calculates the rect of the minimize button in screen
// coordinates.
//
// For horizontal captions, we have the following situation ('Y' is minimize
// and '^' is maximize or restore):
//
//   +---------------------------------+
//   | - |                     | Y | ^ |
//   +---------------------------------+
//   |                         |.......| <-- This is the width (nSize)
//
// For vertical captions, we have the following:
//
//   |  |
//   |  |
//   |  |
//   |  |
//   |  |
//   |  |
//   |--|--
//   | Y| .
//   |--| . <-- This is the height of the rectangle (nSize)
//   | ^| .
//   +--+--
//
// In order to figure out where the minimize button goes, we first need
// to know if there is a maximize button.  If so, use GetMaxButtonRect()
// to place...
//
BOOL PASCAL GetMinButtonRect( HWND hWnd, LPRECT lprc )
{
    if (!TestWinStyle( hWnd, WS_MINIMIZEBOX )) 
    {
        SetRectEmpty( lprc ) ;
        return FALSE ;
    }

    // The minimize button can be in either position 1 or 2.  If there
    // is a maximize button, it's in position 2.
    //
    if (TestWinStyle( hWnd, WS_MAXIMIZEBOX ))
        return GetButtonRect( hWnd, 2, lprc ) ;
    else
        return GetButtonRect( hWnd, 1, lprc ) ;
}

// GetMaxButtonRect()
//
// Calculates the rect of the maximize button in screen
// coordinates.
//
// The maximize button, if present, is always to the far right
// or bottom.
//
BOOL PASCAL GetMaxButtonRect( HWND hWnd, LPRECT lprc )
{
    //The maximize button can only be in position 1.
    //
    if (TestWinStyle( hWnd, WS_MAXIMIZEBOX ))
        return GetButtonRect( hWnd, 1, lprc ) ;
    else
    {
        SetRectEmpty( lprc ) ;
        return FALSE ;
    }
}

// Get the rect where a button would go. 
// 
// This function does not care if it's a min or max, just whether
// it is the first from the right/bottom or second from the right/bottom
// and so on..
//
BOOL PASCAL GetButtonRect( HWND hWnd, UINT nPos, LPRECT lprc )
{
    UINT    nSize = 0 ;

    if (!GetCaptionRect( hWnd, lprc ))   //window coords
      return FALSE ;

    nSize = GETCAPTIONSIZE( hWnd ) ;

    if (TestWinStyle( hWnd, IBS_VERTCAPTION ))
    {
        lprc->bottom -= nSize * (nPos-1) ;
        lprc->top = lprc->bottom - nSize + 1 ;
    }
    else
    {
        lprc->right -= nSize * (nPos-1) ;
        lprc->left = lprc->right - nSize + 1 ;
    }

    return TRUE ;
} // GetButtonRect()

// DrawButton( HDC hDC, HWND hWnd, BOOL fMin, BOOL fDepressed )
// 
// Draws either the min, max, or restore buttons. If fMin is FALSE then it
// will draw either the Max or Restore button.  If fDepressed is TRUE it will
// draw the button in a down state.
//
void PASCAL DrawButton( HDC hDC, HWND hWnd, BOOL fMin, BOOL fDepressed)
{
    RECT            rcButton ;
    RECT            rc ;
    COLORREF        rgbWindowFrame ;
    BOOL            fDC ;
    UINT            nCapSize ;
    UINT            nOffset ;
    int             n ;

    nCapSize = GETCAPTIONSIZE( hWnd ) ;

    // If you look at the standard Windows' min/max buttons, you will notice
    // that they have two pixels of 'shadow' to the bottom and right.  Since
    // our buttons can be really, really small, we only want one pixel of 
    // shadow when they are small.  I arbitrarily decided that if the 
    // caption size is greater than or equal to 20 we will use two
    // pixels.  That's what this THREASHOLD stuff does.
    //
    #define THRESHOLD   20
    nOffset = (nCapSize >= THRESHOLD) ? 2 : 1 ;    

    if (!hDC)
    {
        fDC = TRUE ;
        hDC = GetWindowDC( hWnd ) ;
    }
    else
        fDC = FALSE ;
    
    if (hDC)
    {
        rgbWindowFrame = GetSysColor( COLOR_WINDOWFRAME ) ;
        
        if (fMin)
            GetMinButtonRect( hWnd, &rcButton ) ;
        else
            GetMaxButtonRect( hWnd, &rcButton ) ;
        
        GetWindowRect( hWnd, &rc ) ;
        OffsetRect( &rcButton, -rc.left, -rc.top ) ;
        
        rc = rcButton ;
        if (TestWinStyle( hWnd, IBS_VERTCAPTION ))
        {
            rc = rcButton ;     //separator line
            rc.bottom = --rc.top + 1 ;
            rcButton.right-- ;
        }
        else
        {
            rc = rcButton ;     //separator line
            rc.right = --rc.left + 1 ;
            rcButton.bottom-- ;
        }
        
        //Draw separator line
        SetBkColor( hDC, rgbWindowFrame ) ;
        DRAWFASTRECT( hDC, &rc ) ;
        
        //Fill
        SetBkColor( hDC, RGBLTGRAY ) ;
        DRAWFASTRECT( hDC, &rcButton ) ;
        
        if (!fDepressed)
        {
            //The normal min/max buttons have one pixel on the top and left
            //sides for the highlight, and two pixels on the bottom and
            //right side for the shadow.
            //
            //When our caption is 'small' we only use one pixel on all
            //sides.
            //
            SetBkColor( hDC, RGBWHITE ) ;
            //Draw left side
            rc = rcButton ;
            rc.right = rc.left + 1 ;
            DRAWFASTRECT( hDC, &rc ) ;

            //Draw Top
            rc = rcButton ;
            rc.bottom = rc.top + 1 ;
            DRAWFASTRECT( hDC, &rc ) ;

            SetBkColor( hDC, RGBGRAY ) ;
            //Draw right side
            rc = rcButton ;
            rc.left = rc.right - 1 ;
            DRAWFASTRECT( hDC, &rc ) ;
            if (nCapSize > THRESHOLD)
            {
                rc.left-- ;
                rc.top++ ;
                DRAWFASTRECT( hDC, &rc ) ;
            }

            //Draw bottom
            rc = rcButton ;
            rc.top = rc.bottom - 1 ;
            DRAWFASTRECT( hDC, &rc ) ;
            if (nCapSize > THRESHOLD)
            {
                rc.top-- ;
                rc.left++ ;
                DRAWFASTRECT( hDC, &rc ) ;
            }

            rcButton.left++ ;
            rcButton.top++ ;
            rcButton.right -= nOffset ;
            rcButton.bottom -= nOffset ;
        }
        else
        {
            //Draw depressed state

            SetBkColor( hDC, RGBGRAY ) ;
            //Draw left side
            rc = rcButton ;
            rc.right = rc.left + nOffset ;
            DRAWFASTRECT( hDC, &rc ) ;

            //Draw Top
            rc = rcButton ;
            rc.bottom = rc.top + nOffset ;
            DRAWFASTRECT( hDC, &rc ) ;

            rcButton.left += 2 * nOffset ;
            rcButton.top += 2 * nOffset ;
        }

        // Now draw the arrows.  We do not want the
        // arrows to grow too large when we have a bigger than
        // normal caption, so we restrict their size.
        //
        // rcButton now represents where we can place our
        // arrows.  
        //
        // The maximum size of our arrows (i.e. the width of rcButton)
        // has been empirically determined to be SM_CYCAPTION / 2
        //
        n = ((GetSystemMetrics( SM_CYCAPTION )) / 2) - 
            (rcButton.right - rcButton.left) ;
        if (n < 1)
            InflateRect( &rcButton, n/2-1, n/2-1 ) ;

        if (fMin)
            DrawArrow( hDC, &rcButton, ARROW_DOWN ) ;
        else 
            if (IsZoomed( hWnd ))
            {
                DrawArrow( hDC, &rcButton, ARROW_RESTORE ) ;
            }
            else
                DrawArrow( hDC, &rcButton, ARROW_UP ) ;
        
        if (fDC)
            ReleaseDC( hWnd, hDC ) ;
    }

} // DrawButton()


// DrawArrow
//
// Draws either a up or down arrow.  The arrow is bound by the rectangle
//
void PASCAL DrawArrow( HDC hdc, LPRECT lprc, UINT uiStyle )
{
    int     row ;
    int     xTip ;
    int     yTip ;
    RECT    rc ;
    int     nMax = (lprc->bottom - lprc->top) >> 1 ;

    SetBkColor( hdc, RGBBLACK ) ;

    // We draw the arrow by drawing a series of horizontal lines
    //
    xTip = lprc->left + ((lprc->right - lprc->left+1) >> 1) ;
    switch (uiStyle)
    {
        case ARROW_UP:
            yTip = lprc->top + ((lprc->bottom - lprc->top-1) >> 2) ;
            for (row = 1 ; row <= nMax ; row++ )
            {
                rc.left = xTip - row ;
                rc.right = xTip + row - 1 ;
                rc.top = yTip + row ;
                rc.bottom = rc.top + 1 ;
                DRAWFASTRECT( hdc, &rc ) ;
            }
        break ;

        case ARROW_DOWN:
            yTip = lprc->bottom - ((lprc->bottom - lprc->top-1) >> 2) ;
            for ( row = nMax ; row > 0 ; row-- )
            {
                rc.left = xTip - row ;
                rc.right = xTip + row - 1 ;
                rc.top = yTip - row ;
                rc.bottom = rc.top + 1 ;
                DRAWFASTRECT( hdc, &rc ) ;
            }
        break ;

        case ARROW_RESTORE:
        default:
            yTip = lprc->top + ((lprc->bottom - lprc->top-1) >> 3) - 2;
            for (row = 1 ; row <= nMax ; row++ )
            {
                rc.left = xTip - row ;
                rc.right = xTip + row - 1 ;
                rc.top = yTip + row ;
                rc.bottom = rc.top + 1 ;
                DRAWFASTRECT( hdc, &rc ) ;
            }

            yTip += (nMax+1) * 2 ;
            for ( row = nMax ; row > 0 ; row-- )
            {
                rc.left = xTip - row ;
                rc.right = xTip + row - 1 ;
                rc.top = yTip - row ;
                rc.bottom = rc.top + 1 ;
                DRAWFASTRECT( hdc, &rc ) ;
            }
        break ;
    }

}  // DrawArrow()   

#endif // wxUSE_ITSY_BITSY

#endif // __WIN32__
