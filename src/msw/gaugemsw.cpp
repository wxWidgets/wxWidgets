/////////////////////////////////////////////////////////////////////////////
// Name:        gaugemsw.cpp
// Purpose:     wxGaugeMSW class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "gauge.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/defs.h"
#endif

#if USE_GAUGE 

#include "wx/msw/gaugemsw.h"
#include "wx/msw/private.h"

/* gas gauge graph control messages--class "zYzGauge" */
#define ZYZG_SETRANGE       (WM_USER + 0)
#define ZYZG_GETRANGE       (WM_USER + 1)
#define ZYZG_SETPOSITION    (WM_USER + 2)
#define ZYZG_GETPOSITION    (WM_USER + 3)
#define ZYZG_SETORIENTATION (WM_USER + 4)
#define ZYZG_GETORIENTATION (WM_USER + 5)
#define ZYZG_SETFGCOLOR     (WM_USER + 6)
#define ZYZG_GETFGCOLOR     (WM_USER + 7)
#define ZYZG_SETBKCOLOR     (WM_USER + 8)
#define ZYZG_GETBKCOLOR     (WM_USER + 9)
#define ZYZG_SETWIDTH3D     (WM_USER + 10)
#define ZYZG_GETWIDTH3D     (WM_USER + 11)
#define ZYZG_SETBEZELFACE   (WM_USER + 12)
#define ZYZG_GETBEZELFACE   (WM_USER + 13)
#define ZYZG_SETDELTAPOS    (WM_USER + 14)


/* orientations for ZYZG_WW_ORIENTATION */
#define ZYZG_ORIENT_LEFTTORIGHT     0
#define ZYZG_ORIENT_RIGHTTOLEFT     1
#define ZYZG_ORIENT_BOTTOMTOTOP     2
#define ZYZG_ORIENT_TOPTOBOTTOM     3

/* gauge styles */
#define ZYZGS_3D        0x8000L     /* control will be 3D       */

/* public function prototypes */
BOOL FAR PASCAL gaugeInit(HINSTANCE hInstance);

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxGaugeMSW, wxControl)
#endif

bool wxGaugeMSW::Create(wxWindow *parent, const wxWindowID id,
           const int range,
           const wxPoint& pos,
           const wxSize& size,
           const long style,
           const wxValidator& validator,
           const wxString& name)
{
  static bool wxGaugeMSWInitialised = FALSE;

  if ( !wxGaugeMSWInitialised )
  {
    if (!gaugeInit((HWND) wxGetInstance()))
    	wxFatalError("Cannot initalize Gauge library");
	wxGaugeMSWInitialised = TRUE;
  }

  SetName(name);
  SetValidator(validator);

  if (parent) parent->AddChild(this);
  m_rangeMax = range;

  SetBackgroundColour(parent->GetDefaultBackgroundColour()) ;
  SetForegroundColour(parent->GetDefaultForegroundColour()) ;

  m_windowStyle = style;

  if ( id == -1 )
  	m_windowId = (int)NewControlId();
  else
	m_windowId = id;

  int x = pos.x;
  int y = pos.y;
  int width = size.x;
  int height = size.y;

  long msFlags = WS_CHILD | WS_VISIBLE | WS_TABSTOP;
  msFlags |= ZYZGS_3D;

  HWND wx_button =
      CreateWindowEx(MakeExtendedStyle(m_windowStyle), "zYzGauge", NULL, msFlags,
                    0, 0, 0, 0, (HWND) parent->GetHWND(), (HMENU)m_windowId,
                    wxGetInstance(), NULL);

  m_hWnd = (WXHWND)wx_button;

  // Subclass again for purposes of dialog editing mode
  SubclassWin((WXHWND)wx_button);

  int wOrient = 0;

  if (m_windowStyle & wxGA_HORIZONTAL)
    wOrient = ZYZG_ORIENT_LEFTTORIGHT;
  else
    wOrient = ZYZG_ORIENT_BOTTOMTOTOP;
  
  SendMessage(wx_button, ZYZG_SETORIENTATION, wOrient, 0);
  SendMessage(wx_button, ZYZG_SETRANGE, range, 0);

  SendMessage((HWND) GetHWND(), ZYZG_SETFGCOLOR, 0, RGB(GetForegroundColour().Red(), GetForegroundColour().Green(), GetForegroundColour().Blue()));
  SendMessage((HWND) GetHWND(), ZYZG_SETBKCOLOR, 0, RGB(GetBackgroundColour().Red(), GetBackgroundColour().Green(), GetBackgroundColour().Blue()));

  SetFont(* parent->GetFont());

  if (width == -1)
    width = 50;
  if (height == -1)
    height = 50;
  SetSize(x, y, width, height);

  ShowWindow((HWND) GetHWND(), SW_SHOW);

  return TRUE;
}

void wxGaugeMSW::SetSize(const int x, const int y, const int width, const int height, const int sizeFlags)
{
  int currentX, currentY;
  GetPosition(&currentX, &currentY);
  int x1 = x;
  int y1 = y;
  int w1 = width;
  int h1 = height;

  if (x == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    x1 = currentX;
  if (y == -1 || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
    y1 = currentY;

  // If we're prepared to use the existing size, then...
  if (width == -1 && height == -1 && ((sizeFlags & wxSIZE_AUTO) != wxSIZE_AUTO))
  {
    GetSize(&w1, &h1);
  }

  // Deal with default size (using -1 values)
  if (w1<=0)
    w1 = DEFAULT_ITEM_WIDTH;

  if (h1<=0)
    h1 = DEFAULT_ITEM_HEIGHT;

  MoveWindow((HWND) GetHWND(), x1, y1, w1, h1, TRUE);

#if WXWIN_COMPATIBILITY
  GetEventHandler()->OldOnSize(width, height);
#else
  wxSizeEvent event(wxSize(width, height), m_windowId);
  event.eventObject = this;
  GetEventHandler()->ProcessEvent(event);
#endif
}

void wxGaugeMSW::SetShadowWidth(const int w)
{
  SendMessage((HWND) GetHWND(), ZYZG_SETWIDTH3D, w, 0);
}

void wxGaugeMSW::SetBezelFace(const int w)
{
  SendMessage((HWND) GetHWND(), ZYZG_SETBEZELFACE, w, 0);
}

void wxGaugeMSW::SetRange(const int r)
{
  m_rangeMax = r;

  SendMessage((HWND) GetHWND(), ZYZG_SETRANGE, r, 0);
}

void wxGaugeMSW::SetValue(const int pos)
{
  m_gaugePos = pos;

  SendMessage((HWND) GetHWND(), ZYZG_SETPOSITION, pos, 0);
}

int wxGaugeMSW::GetShadowWidth(void) const
{
  return (int) SendMessage((HWND) GetHWND(), ZYZG_GETWIDTH3D, 0, 0);
}

int wxGaugeMSW::GetBezelFace(void) const
{
  return (int) SendMessage((HWND) GetHWND(), ZYZG_GETBEZELFACE, 0, 0);
}

int wxGaugeMSW::GetRange(void) const
{
  return (int) SendMessage((HWND) GetHWND(), ZYZG_GETRANGE, 0, 0);
}

int wxGaugeMSW::GetValue(void) const
{
  return (int) SendMessage((HWND) GetHWND(), ZYZG_GETPOSITION, 0, 0);
}

void wxGaugeMSW::SetForegroundColour(const wxColour& col)
{
  m_foregroundColour = col ;
  SendMessage((HWND) GetHWND(), ZYZG_SETFGCOLOR, 0, RGB(col.Red(), col.Green(), col.Blue()));
}

void wxGaugeMSW::SetBackgroundColour(const wxColour& col)
{
  m_backgroundColour = col ;
  SendMessage((HWND) GetHWND(), ZYZG_SETBKCOLOR, 0, RGB(col.Red(), col.Green(), col.Blue()));
}


/** zyz3d.c
 *
 *  DESCRIPTION: 
 *      This module contains functions for creating nifty 3D borders
 *      around controls like zYzGauge.
 *
 *  HISTORY:
 *      3/14/91     cjp     put in this comment
 *      6/19/92     cjp     touched it a bit
 *
 ** cjp */
// COPYRIGHT:
//
//   (C) Copyright Microsoft Corp. 1992.  All rights reserved.
//
//   You have a royalty-free right to use, modify, reproduce and
//   distribute the Sample Files (and/or any modified version) in
//   any way you find useful, provided that you agree that
//   Microsoft has no warranty obligations or liability for any
//   Sample Application Files which are modified.
//


/* get the includes we need */
#include <windows.h>

/* misc. control flag defines */
#define DRAW3D_IN           0x0001
#define DRAW3D_OUT          0x0002

#define DRAW3D_TOPLINE      0x0004
#define DRAW3D_BOTTOMLINE   0x0008
#define DRAW3D_LEFTLINE     0x0010
#define DRAW3D_RIGHTLINE    0x0020


/* public function prototypes */
void FAR PASCAL Draw3DFaceFrame(HDC, LPRECT, WORD);
void FAR PASCAL Draw3DRect(HDC, LPRECT, WORD, WORD);
void FAR PASCAL Draw3DLine(HDC, WORD, WORD, WORD, WORD, WORD);


/** void FAR PASCAL Draw3DFaceFrame(HDC hdc, LPRECT rc, WORD wWidth)
 *
 *  DESCRIPTION: 
 *      This function draws a flat frame with the current button-face
 *      color.
 *
 *  ARGUMENTS:
 *      HDC hdc     :   The DC to draw into.
 *
 *      LPRECT rc   :   The containing rect for the new frame.
 *
 *      WORD wWidth :   The width of the frame to draw.
 *
 *  RETURN (void FAR PASCAL):
 *      The frame will have been drawn into the DC.
 *
 *  NOTES:
 *
 ** cjp */

void FAR PASCAL Draw3DFaceFrame(HDC hdc, LPRECT rc, WORD wWidth)
{
    RECT    rc1;
    DWORD   rgbOld;

    /* don't go through a bunch of work if we don't have to */
    if (!wWidth)
        return;

    /* set up color to be button-face color--so it may not be gray */
    rgbOld = SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));

    /* perform CopyRect w/o bloody windows style overhead */
    rc1 = *rc;

    /* top */
    rc1.top = rc->top;
    rc1.left = rc->left;
    rc1.bottom = rc->top + wWidth;
    rc1.right = rc->right;

    /* blast it out */
    ExtTextOut(hdc, rc1.left, rc1.top, ETO_OPAQUE, &rc1, NULL, 0, NULL);

    /* right */
    rc1.left = rc->right - wWidth;
    rc1.bottom = rc->bottom;

    /* blast this part now */
    ExtTextOut(hdc, rc1.left, rc1.top, ETO_OPAQUE, &rc1, NULL, 0, NULL);

    /* left */
    rc1.left = rc->left;
    rc1.right = rc->left + wWidth;

    /* and another part */
    ExtTextOut(hdc, rc1.left, rc1.top, ETO_OPAQUE, &rc1, NULL, 0, NULL);

    /* bottom */
    rc1.right = rc->right;
    rc1.top = rc->bottom - wWidth;

    /* finish it off */
    ExtTextOut(hdc, rc1.left, rc1.top, ETO_OPAQUE, &rc1, NULL, 0, NULL);

    /* restore the old bk color */
    SetBkColor(hdc, rgbOld);
} /* Draw3DFaceFrame() */


/** void FAR PASCAL Draw3DRect(HDC, LPRECT, WORD, WORD)
 *
 *  DESCRIPTION: 
 *      Draws a 3D rectangle that is shaded.  wFlags can be used to
 *      control how the rectangle looks.
 *
 *  ARGUMENTS:
 *      HDC hdc             :   Handle to the device context that will be
 *                              used to display the rectangle.
 *
 *      RECT rect           :   A rectangle describing the dimensions of
 *                              the rectangle in device coordinates.
 *
 *      WORD wShadowWidth   :   Width of the shadow in device coordinates.
 *
 *      WORD wFlags         :   The following flags may be passed to describe
 *                              the style of the rectangle:
 *
 *                              DRAW3D_IN   :   The shadow is drawn such that
 *                              the box appears to be sunk in to the screen.
 *                              This is default if 0 is passed.
 *
 *                              DRAW3D_OUT  :   The shadow is drawn such that
 *                              the box appears to be sticking out of the
 *                              screen.
 *
 *  RETURN (void FAR PASCAL):
 *      The 3D looking rectangle will have been drawn into the DC.
 *
 *  NOTES:
 *
 ** cjp */

void FAR PASCAL Draw3DRect(HDC hdc, LPRECT lpRect,
                               WORD wShadowWidth, WORD wFlags)
{
    /* sanity check--don't work if you don't have to! */
    if (!wShadowWidth || !RectVisible(hdc, lpRect))
        return;

    /* draw the top line */
    Draw3DLine(hdc, lpRect->left, lpRect->top,
                    lpRect->right - lpRect->left,
                    wShadowWidth, DRAW3D_TOPLINE | wFlags);

    /* right line */
    Draw3DLine(hdc, lpRect->right, lpRect->top,
                    lpRect->bottom - lpRect->top,
                    wShadowWidth, DRAW3D_RIGHTLINE | wFlags);

    /* bottom line */
    Draw3DLine(hdc, lpRect->left, lpRect->bottom,
                    lpRect->right - lpRect->left,
                    wShadowWidth, DRAW3D_BOTTOMLINE | wFlags);

    /* left line */
    Draw3DLine(hdc, lpRect->left, lpRect->top,
                    lpRect->bottom - lpRect->top,
                    wShadowWidth, DRAW3D_LEFTLINE | wFlags);
} /* Draw3DRect() */


/** void FAR PASCAL Draw3DLine(HDC hdc, WORD x, WORD y, WORD nLen,
 *
 *  DESCRIPTION: 
 *      Draws a 3D line that can be used to make a 3D box.
 *
 *  ARGUMENTS:
 *      HDC hdc             :   Handle to the device context that will be
 *                              used to display the 3D line.
 *
 *      WORD x, y           :   Coordinates of the beginning of the line.
 *                              These coordinates are in device units and
 *                              represent the _outside_ most point. Horiz-
 *                              ontal lines are drawn from left to right and
 *                              vertical lines are drawn from top to bottom.
 *
 *      WORD wShadowWidth   :   Width of the shadow in device coordinates.
 *
 *      WORD  wFlags        :   The following flags may be passed to
 *                              describe the style of the 3D line:
 *
 *                              DRAW3D_IN   :   The shadow is drawn such that
 *                              the box appears to be sunk in to the screen.
 *                              This is default if 0 is passed.
 *
 *                              DRAW3D_OUT  :   The shadow is drawn such that
 *                              the box appears to be sticking out of the
 *                              screen.
 *
 *                              DRAW3D_TOPLINE, _BOTTOMLINE, _LEFTLINE, and
 *                              _RIGHTLINE  :   Specifies that a "top",
 *                              "Bottom", "Left", or"Right" line is to be
 *                              drawn.
 *
 *  RETURN (void FAR PASCAL):
 *      The line will have been drawn into the DC.
 *
 *  NOTES:
 *
 ** cjp */

void FAR PASCAL Draw3DLine(HDC hdc, WORD x, WORD y, WORD nLen,
                               WORD wShadowWidth, WORD wFlags) 
{
    HBRUSH  hOldBrush;
    HPEN    hOldPen;
    BOOL    fDark;
    POINT   Point[ 4 ];         /* define a polgon with 4 points    */

    /* if width is zero, don't do nothin'! */
    if (!wShadowWidth)
        return;

    /* define shape of polygon--origin is always the same */
    Point[0].x = x;
    Point[0].y = y;

    /*  To do this we'll simply draw a polygon with four sides, using 
     *  the appropriate brush.  I dare you to ask me why this isn't a
     *  switch/case!
     */
    if (wFlags & DRAW3D_TOPLINE)
    {
        /* across to right */
        Point[1].x = x + nLen - (wShadowWidth == 1 ? 1 : 0);
        Point[1].y = y;

        /* down/left */
        Point[2].x = x + nLen - wShadowWidth;
        Point[2].y = y + wShadowWidth;

        /* accross to left */
        Point[3].x = x + wShadowWidth;
        Point[3].y = y + wShadowWidth;

        /* select 'dark' brush if 'in'--'light' for 'out' */
        fDark = (wFlags & DRAW3D_IN) ? TRUE : FALSE;
    }

    /* possibly the bottom? */
    else if (wFlags & DRAW3D_BOTTOMLINE)
    {
        /* across to right */
        Point[1].x = x + nLen;
        Point[1].y = y;

        /* up/left */
        Point[2].x = x + nLen - wShadowWidth;
        Point[2].y = y - wShadowWidth;

        /* accross to left */
        Point[3].x = x + wShadowWidth;
        Point[3].y = y - wShadowWidth;

        /* select 'light' brush if 'in' */
        fDark = (wFlags & DRAW3D_IN) ? FALSE : TRUE;
    }

    /* ok, it's gotta be left? */
    else if (wFlags & DRAW3D_LEFTLINE)
    {
        /* down */
        Point[1].x = x;
        Point[1].y = y + nLen - (wShadowWidth == 1 ? 1 : 0);

        /* up/right */
        Point[2].x = x + wShadowWidth;
        Point[2].y = y + nLen - wShadowWidth;

        /* down */
        Point[3].x = x + wShadowWidth;
        Point[3].y = y + wShadowWidth;

        /* select 'dark' brush if 'in'--'light' for 'out' */
        fDark = (wFlags & DRAW3D_IN) ? TRUE : FALSE;
    }
    
    /* well maybe it's for the right side? */
    else if (wFlags & DRAW3D_RIGHTLINE)
    {
        /* down */
        Point[1].x = x;
        Point[1].y = y + nLen;

        /* up/left */
        Point[2].x = x - wShadowWidth;
        Point[2].y = y + nLen - wShadowWidth;

        /* up */
        Point[3].x = x - wShadowWidth;
        Point[3].y = y + wShadowWidth;

        /* select 'light' brush if 'in' */
        fDark = (wFlags & DRAW3D_IN) ? FALSE : TRUE;
    }

    /* bad drugs? */
    else return;

    /* select NULL_PEN for no borders */
    hOldPen = SelectObject(hdc, GetStockObject(NULL_PEN));

    /* select the appropriate color for the fill */
    if (fDark)
        hOldBrush = SelectObject(hdc, GetStockObject(GRAY_BRUSH));
    else
        hOldBrush = SelectObject(hdc, GetStockObject(WHITE_BRUSH));

    /* finally, draw the dern thing */
    Polygon(hdc, (LPPOINT)&Point, 4);

    /* restore what we killed */
    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
} /* Draw3DLine() */

/** EOF: zyz3d.c **/

/** zyzgauge.c
 *
 *  DESCRIPTION: 
 *      Yet another 'Gas Gauge Custom Control.'  This control gives you
 *      a 'progress bar' class (named zYzGauge) for use in your applications.
 *      You can set the range, position, font, color, orientation, and 3d
 *      effect of the gauge by sending messages to the control.
 *
 *      Before you can use this control, you MUST first export the window
 *      procedure for the control (or define it with the _export keyword):
 *
 *          EXPORTS     gaugeWndProc
 *
 *      You then need initialize the class before you use it:
 *
 *          if (!gaugeInit(hInstance))
 *              die a horrible death
 *          else
 *              you are good to go
 *
 *      The colors used by the control default to black and white if you
 *      are running on a mono-display.  They default to blue and white
 *      if you are on a color display.  You enable the 3D effect by setting
 *      the ZYZGS_3D style flag in the styles field of the control (like
 *      any other control).
 *
 *      To select your own colors, you can send the ZYZG_SETFGCOLOR and
 *      ZYZG_SETBKCOLOR messages to set the foreground (percent done) and
 *      background (percent not done) colors.  The lParam is the RGB()
 *      value--wParam is ignored.
 *
 *      In all of the following ZYZG_??? messages, the arguments are
 *      WORDS.  If you are setting parameters, the value is sent as
 *      the wParam (lParam is ignored).  If you are getting parameters,
 *      the value is returned as a LONG and should be cast to a *signed*
 *      integer.
 *
 *      To set the depth of the 3D effect (if enabled), you can send the
 *      ZYZG_SETBEZELFACE and ZYZG_SETWIDTH3D messages.  The bezel face
 *      is the flat top on the 3D border--its color will be that of the
 *      button-face.  The 3D width is the width of the bezel itself; inside
 *      and outside.  The light color is white, the dark color is gray.
 *      Both widths *can* be zero--both default to 2 which looks to me.
 *
 *      The range of the control can be set by sending the ZYZG_SETRANGE
 *      message to the control.  It can be any integer from 1 to 32767.
 *      What this specifies is the number of pieces that create a whole.
 *      The default is 100.  You can get the current range setting by
 *      sending the ZYZG_GETRANGE message to the control.
 *
 *      The position (number of pieces out of the whole have been used) is
 *      set with the ZYZG_SETPOSITION message.  It can be any integer from
 *      0 to the current range setting of the control--it will be clipped
 *      if the position is out of bounds.  The default position is 0.  You
 *      can get the current position at any time with the ZYZG_GETPOSITION
 *      message.
 *
 *      You can also set the range using a delta from the current range.
 *      This is done by sending the ZYZG_SETDELTAPOS message with wParam
 *      set to a _signed_ integer value within the range of the control.
 *
 *      The font used for the percentage text can be set using the standard
 *      WM_SETFONT message.  You can get the current font at any time with
 *      the WM_GETFONT message.
 *
 *      The orientation can be left to right, right to left, bottom to top,
 *      or top to bottom.  Whatever suits your needs.  You set this by
 *      sending the ZYZG_ORIENTATION message to the control with one of
 *      the following values (default is ZYZG_ORIENT_LEFTTORIGHT):
 *
 *          ZYZG_ORIENT_LEFTTORIGHT (0)
 *          ZYZG_ORIENT_RIGHTTOLEFT (1)
 *          ZYZG_ORIENT_BOTTOMTOTOP (2)
 *          ZYZG_ORIENT_TOPTOBOTTOM (3)
 *
 *  HISTORY:
 *      3/12/91     cjp     put in this comment
 *      6/19/92     cjp     touched it a bit
 *
 ** cjp */
// COPYRIGHT:
//
//   (C) Copyright Microsoft Corp. 1992.  All rights reserved.
//
//   You have a royalty-free right to use, modify, reproduce and
//   distribute the Sample Files (and/or any modified version) in
//   any way you find useful, provided that you agree that
//   Microsoft has no warranty obligations or liability for any
//   Sample Application Files which are modified.
//


/* get the includes we need */
#ifndef __GNUWIN32__
#include <malloc.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// #include "zyz3d.h"
// #include "zyzgauge.h"


/* static global variables */
static char gszzYzGaugeClass[] = "zYzGauge";
    

/* window word position definitions */
#define ZYZG_WW_PZYZGAUGE   0
/* #define ZYZG_WW_EXTRABYTES  2 */
#define ZYZG_WW_EXTRABYTES  4


/* control block structure typedef */
typedef struct tZYZGAUGE
{
    WORD    wRange;
    WORD    wPosition;
    WORD    wOrientation;
    WORD    wWidth3D;
    WORD    wWidthBezelFace;
    HFONT   hFont;
    DWORD   rgbTextColor;
    DWORD   rgbBkColor;

} ZYZGAUGE, *PZYZGAUGE, FAR *LPZYZGAUGE;


/* some default values for the control */
#define ZYZG_DEF_RANGE          100
#define ZYZG_DEF_POSITION       0
#define ZYZG_DEF_ORIENTATION    ZYZG_ORIENT_LEFTTORIGHT
#define ZYZG_DEF_WIDTH3D        2
#define ZYZG_DEF_BEZELFACE      2



/* the default settings for drawing colors--display dependent */
static DWORD    rgbDefTextColor;
static DWORD    rgbDefBkColor;
static BOOL     fSupport3D;

#if !defined(APIENTRY)	// NT defines APIENTRY, 3.x not
#define APIENTRY FAR PASCAL
#endif
 
#ifdef __WIN32__
#define _EXPORT /**/
#else
#define _EXPORT _export
typedef signed short int SHORT ;
#endif

/* internal function prototypes */
static void PASCAL gaugePaint(HWND, HDC);
/* LRESULT FAR PASCAL */
LRESULT APIENTRY _EXPORT gaugeWndProc(HWND, UINT, WPARAM, LPARAM);



/** BOOL FAR PASCAL gaugeInit(HINSTANCE hInstance)
 *
 *  DESCRIPTION: 
 *      Registers the window class for the zYzGauge control.  Performs
 *      other initialization for the zYzGauge text control.  This must
 *      be done before the zYzGauge control is used--or it will fail
 *      and your dialog box will not open!
 *
 *  ARGUMENTS:
 *      HINSTANCE hInstance :   Instance handle to register class with.
 *
 *  RETURN (BOOL FAR):
 *      The return value is TRUE if the zYzGauge class was successfully
 *      registered.  It is FALSE if the initialization fails.
 *
 *  NOTES:
 *
 ** cjp */

//#pragma alloc_text(init, gaugeInit)

BOOL FAR PASCAL gaugeInit(HINSTANCE hInstance)
{
    static BOOL fRegistered = FALSE;
    WNDCLASS    wc;
    HDC         hdc;
    
    /* assume already registered if not first instance */
    if (fRegistered)
        return (TRUE);

    /* fill in the class structure for the zyzgauge control */
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon            = NULL;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = gszzYzGaugeClass;
    wc.hbrBackground    = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hInstance        = hInstance;

#ifdef ZYZGAUGE_DLL
    wc.style            = CS_GLOBALCLASS | CS_HREDRAW | CS_VREDRAW;
#else
    wc.style            = CS_HREDRAW | CS_VREDRAW;
#endif

    wc.lpfnWndProc      = gaugeWndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = ZYZG_WW_EXTRABYTES;

    /* attempt to register it--return FALSE if fail */
    if (!RegisterClass(&wc))
        return (FALSE);

    /*  Get a DC to determine whether device is mono or not, and set
     *  default foreground/background colors as appropriate.
     */
    if ((hdc = CreateIC("DISPLAY", NULL, NULL, 0L)))
    {
        /* check for mono-display */
        if ((GetDeviceCaps(hdc, BITSPIXEL) == 1) &&
             (GetDeviceCaps(hdc, PLANES) == 1))
        {
            /* using a mono DC--white foreground, black background */
            rgbDefTextColor = RGB(255, 255, 255);
            rgbDefBkColor = RGB(0, 0, 0);
        }

        /* good! we have color: blue foreground, white background */
        else 
        {
            rgbDefTextColor = RGB(0, 0, 255);
            rgbDefBkColor = RGB(255, 255, 255);
        }

        /* need at _least_ 8 for two shades of gray (>=VGA) */
        fSupport3D = (GetDeviceCaps(hdc, NUMCOLORS) >= 8) ? TRUE : FALSE;

        /* get rid of the DC (IC) */
        DeleteDC(hdc);
    }

    /* uh-oh... can't get DC (IC)... fail */
    else
    {
        /* unregister the class */
        UnregisterClass(gszzYzGaugeClass, hInstance);
        return (FALSE);
    }

    /* return success */
    return (fRegistered = TRUE);
} /* gaugeInit() */


/** static void PASCAL gaugePaint(HWND hwnd, HDC hdc)
 *
 *  DESCRIPTION: 
 *      This function is responsible for painting the zYzGauge control.
 *
 *  ARGUMENTS:
 *      HWND hwnd   :   The window handle for the gauge.
 *
 *      HDC hdc     :   The DC for the gauge's window.
 *
 *  RETURN (void):
 *      The control will have been painted.
 *
 *  NOTES:
 *
 ** cjp */

static void PASCAL gaugePaint(HWND hwnd, HDC hdc)
{
    PZYZGAUGE   pgauge;
    WORD        iRange, iPos;
    WORD        Offset = 1;
    DWORD       dwExtent;
    RECT        rc1, rc2;
    HFONT       hFont;
    char        ach[ 6 ];
    WORD        dx, dy, wGomerX, wGomerY;
/* Win32s has no GetTextExtent(); let's try GetTextExtentPoint() instead,
 * which needs a SIZE* parameter */
#if defined(__WIN32__)
    SIZE size;
#endif

    /* get pointer to the control's control block */
//    pgauge = (PZYZGAUGE)GetWindowWord(hwnd, ZYZG_WW_PZYZGAUGE);
    pgauge = (PZYZGAUGE)GetWindowLong(hwnd, ZYZG_WW_PZYZGAUGE);

    /* set the colors into for the gauge into the control */
    SetTextColor(hdc, pgauge->rgbTextColor);
    SetBkColor(hdc, pgauge->rgbBkColor);

    /* draw black rectangle for gauge */
    GetClientRect(hwnd, &rc1);

    /* draw a black border on the _outside_ */
    FrameRect(hdc, &rc1, GetStockObject(BLACK_BRUSH));

    /* we want to draw _just inside_ the black border */
    InflateRect(&rc1, -1, -1);

    /* one line thick so far... */
//    Offset = (WORD) 1;

    /* for 3D stuff, we need to have at least two shades of gray */
    if ((GetWindowLong(hwnd, GWL_STYLE) & ZYZGS_3D) && fSupport3D)
    {
        Draw3DRect(hdc, &rc1, pgauge->wWidth3D, DRAW3D_OUT);
	InflateRect(&rc1, ~(pgauge->wWidth3D), ~(pgauge->wWidth3D));

        Draw3DFaceFrame(hdc, &rc1, pgauge->wWidthBezelFace);
	InflateRect(&rc1, ~(pgauge->wWidthBezelFace), ~(pgauge->wWidthBezelFace));

        Draw3DRect(hdc, &rc1, pgauge->wWidth3D, DRAW3D_IN);
	InflateRect(&rc1, ~(pgauge->wWidth3D), ~(pgauge->wWidth3D));

        /* draw a black border on the _inside_ */
        FrameRect(hdc, &rc1, GetStockObject(BLACK_BRUSH));

        /* we want to draw _just inside_ the black border */
        InflateRect(&rc1, -1, -1);

        /* add all the other pixels into the border width */
        Offset += (2 * pgauge->wWidth3D) + pgauge->wWidthBezelFace + 1;
    }
   
    /* dup--one rc for 'how much filled', one rc for 'how much empty' */
    rc2 = rc1;

    /* get the range--make sure it's a valid range */
    if ((iRange = pgauge->wRange) <= 0)
        iRange = 1;

    /* get the position--greater than 100% would be bad */
    if ((iPos = pgauge->wPosition) > iRange)
        iPos = iRange;

    /* compute the actual size of the gauge */
    dx = rc1.right - rc1.left;
    dy = rc1.bottom - rc1.top;
    wGomerX = (WORD)((DWORD)iPos * dx / iRange);
    wGomerY = (WORD)((DWORD)iPos * dy / iRange);

    /* get the orientation and munge rects accordingly */
    switch (pgauge->wOrientation)
    {
        case ZYZG_ORIENT_RIGHTTOLEFT:
            rc1.left = rc2.right = rc1.right - wGomerX;
            break;

        case ZYZG_ORIENT_BOTTOMTOTOP:
            rc1.top = rc2.bottom = rc1.bottom - wGomerY;
            break;

        case ZYZG_ORIENT_TOPTOBOTTOM:
            rc1.bottom = rc2.top += wGomerY;
            break;

        default:
            rc1.right = rc2.left += wGomerX;
            break;
    } /* switch () */

    /* select the correct font */
    hFont = SelectObject(hdc, pgauge->hFont);

    /* build up a string to blit out--ie the meaning of life: "42%" */
    wsprintf(ach, "%3d%%", (WORD)((DWORD)iPos * 100 / iRange));
/* Win32s has no GetTextExtent(); let's try GetTextExtentPoint() instead */
#if defined(__WIN32__)
    GetTextExtentPoint(hdc, ach, wGomerX = lstrlen(ach), &size);
    dwExtent = size.cx;
#else
    dwExtent = GetTextExtent(hdc, ach, wGomerX = lstrlen(ach));
#endif  


    /*  Draw the finished (ie the percent done) side of box.  If
     *  ZYZG_WW_POSITION is 42, (in range of 0 to 100) this ExtTextOut
     *  draws the meaning of life (42%) bar.
     */
    ExtTextOut(hdc, (dx - LOWORD(dwExtent)) / 2 + Offset,
                    (dy - HIWORD(dwExtent)) / 2 + Offset,
                    ETO_OPAQUE | ETO_CLIPPED, &rc2, ach, wGomerX, NULL);

    /*  Reverse fore and back colors for drawing the undone (ie the non-
     *  finished) side of the box.
     */
    SetBkColor(hdc, pgauge->rgbTextColor);
    SetTextColor(hdc, pgauge->rgbBkColor);

    ExtTextOut(hdc, (dx - LOWORD(dwExtent)) / 2 + Offset,
                    (dy - HIWORD(dwExtent)) / 2 + Offset,
                    ETO_OPAQUE | ETO_CLIPPED, &rc1, ach, wGomerX, NULL);

    /* unselect the font */
    SelectObject(hdc, hFont);
} /* gaugePaint() */
  

/** LRESULT FAR PASCAL gaugeWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
 *
 *  DESCRIPTION: 
 *      This is the control's window procedure.  Its purpose is to handle
 *      special messages for this custom control.
 *
 *      The special control messages for the gauge control are:
 *
 *          ZYZG_SETRANGE       :   Sets the range of the gauge.  In other
 *                                  words, the number of parts that make a
 *                                  whole.
 *
 *          ZYZG_GETRANGE       :   Returns the current range of the gauge.
 *
 *          ZYZG_SETORIENTATION :   Sets the orientation of the gauge.  This
 *                                  can be one of the ZYZG_ORIENT_?? msgs.
 *
 *          ZYZG_GETORIENTATION :   Gets the current orientation of the 
 *                                  gauge.
 *
 *          ZYZG_SETPOSITION    :   Sets the current position of the gauge.
 *                                  In other words, how many pieces of the
 *                                  whole have been used.
 *
 *          ZYZG_GETPOSITION    :   Gets the current position of the gauge.
 *
 *          ZYZG_SETDELTAPOS    :   Sets the position of the gauge +/- the
 *                                  specified amount.
 *
 *          ZYZG_SETFGCOLOR     :   Sets the foreground (percent done) color.
 *
 *          ZYZG_GETFGCOLOR     :   Gets the foreground (percent done) color.
 *
 *          ZYZG_SETBKCOLOR     :   Sets the background (percent not done)
 *                                  color.
 *
 *          ZYZG_GETBKCOLOR     :   Gets the background (percent not done)
 *                                  color.
 *
 *          WM_SETFONT          :   Sets the font to use for the percentage
 *                                  text of the gauge.
 *
 *          WM_GETFONT          :   Gets the current font in use by the
 *                                  gauge.
 *
 *  NOTES:
 *
 ** cjp */

/* LRESULT FAR PASCAL */

LRESULT APIENTRY _EXPORT gaugeWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HFONT       hFont;
    PAINTSTRUCT ps;
    PZYZGAUGE   pgauge;
    RECT        rc;

//    pgauge = (PZYZGAUGE)GetWindowWord(hwnd, ZYZG_WW_PZYZGAUGE);
    pgauge = (PZYZGAUGE)GetWindowLong(hwnd, ZYZG_WW_PZYZGAUGE);

    /* break to get DefWindowProc() */
    switch (uMsg)
    {
        case WM_CREATE:
            /* need to allocate a control block */
//            pgauge = (PZYZGAUGE)LocalAlloc(LPTR, sizeof(ZYZGAUGE));
            pgauge = (PZYZGAUGE)malloc(sizeof(ZYZGAUGE));
            if (!pgauge)
                return (0L);

            /* hang on to this control block */
//            SetWindowWord(hwnd, ZYZG_WW_PZYZGAUGE, (WORD)pgauge);
            SetWindowLong(hwnd, ZYZG_WW_PZYZGAUGE, (LONG)pgauge);

            /* fill control block with defaults */
            pgauge->wRange           = ZYZG_DEF_RANGE;
            pgauge->wPosition        = ZYZG_DEF_POSITION;
            pgauge->wOrientation     = ZYZG_DEF_ORIENTATION;
            pgauge->wWidth3D         = ZYZG_DEF_WIDTH3D;
            pgauge->wWidthBezelFace  = ZYZG_DEF_BEZELFACE;
            pgauge->rgbTextColor     = rgbDefTextColor;
            pgauge->rgbBkColor       = rgbDefBkColor;

            /* use system font */
            SendMessage(hwnd, WM_SETFONT, (WPARAM)NULL, 0L);

            /* go to DefWindowProc() to finish the job */
            break;

        case WM_DESTROY:
            /* get rid of the control's memory */
            if (pgauge)
//                LocalFree((HANDLE)pgauge);
                free(pgauge);
            break;

        case ZYZG_GETPOSITION:
            return (pgauge->wPosition);

        case ZYZG_GETRANGE:
            return (pgauge->wRange);

        case ZYZG_GETORIENTATION:
            return (pgauge->wOrientation);

        case ZYZG_GETWIDTH3D:
            return (pgauge->wWidth3D);

        case ZYZG_GETBEZELFACE:
            return (pgauge->wWidthBezelFace);

        case ZYZG_GETBKCOLOR:
            return (pgauge->rgbTextColor);

        case ZYZG_GETFGCOLOR:
            return (pgauge->rgbBkColor);

        case ZYZG_SETBKCOLOR:
            pgauge->rgbBkColor = lParam;
            return (0L);

        case ZYZG_SETFGCOLOR:
            pgauge->rgbTextColor = lParam;
            return (0L);


        case ZYZG_SETPOSITION:
            pgauge->wPosition = wParam;

zyzgForceRepaint:
            GetClientRect(hwnd, &rc);
            if ((GetWindowLong(hwnd, GWL_STYLE) & ZYZGS_3D) && fSupport3D)
            {
                wParam = (2 * pgauge->wWidth3D) +
                            pgauge->wWidthBezelFace + 2;
            }

            else
                wParam = 1;

	    InflateRect(&rc, ~(wParam), ~(wParam));
            InvalidateRect(hwnd, &rc, FALSE);
            UpdateWindow(hwnd);
            return (0L);

        case ZYZG_SETRANGE:
            pgauge->wRange = wParam;
            goto zyzgForceRepaint;

        case ZYZG_SETORIENTATION:
            pgauge->wOrientation = wParam;
            goto zyzgForceRepaint;

        case ZYZG_SETWIDTH3D:
            pgauge->wWidth3D = wParam;

zyzgForceRepaint3D:
            InvalidateRect(hwnd, NULL, FALSE);
            UpdateWindow(hwnd);
            return (0L);

        case ZYZG_SETBEZELFACE:
            pgauge->wWidthBezelFace = wParam;
            goto zyzgForceRepaint3D;

        case ZYZG_SETDELTAPOS:
/* Watcom doesn't like the following line so removing typecasts */
/*            (int)pgauge->wPosition += (int)wParam; */
            pgauge->wPosition += wParam;
            goto zyzgForceRepaint;

        case WM_PAINT:
            BeginPaint(hwnd, &ps);
            gaugePaint(hwnd, ps.hdc);
            EndPaint(hwnd, &ps);
            return (0L);

        case WM_GETFONT:
            hFont = pgauge->hFont;

            /* if system font, then return NULL handle */
            return (long)((hFont == GetStockObject(SYSTEM_FONT)) ? NULL : hFont);

        case WM_SETFONT:
            /* if NULL hFont, use system font */
            if (!(hFont = (HFONT)wParam))
                hFont = GetStockObject(SYSTEM_FONT);

            pgauge->hFont = hFont;

            /* redraw if indicated in message */
            if ((BOOL)lParam)
            {
                InvalidateRect(hwnd, NULL, TRUE);
                UpdateWindow(hwnd);
            }
            return (0L);
    } /* switch () */

    /* let the dialog mangler take care of this message */
    return (DefWindowProc(hwnd, uMsg, wParam, lParam));
} /* gaugeWndProc() */


/** EOF: zyzgauge.c **/

#endif // USE_GAUGE
