/*
 * Copyright (C) 1998 Stefan Csomor, 1996 Lorens Younes
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * Lorens Younes BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Lorens Younes shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from Lorens Younes.
 */

/*****************************************************************************\
* macx.h:                                                                     *
*                                                                             *
*  XPM library                                                                *
*  Emulates some Xlib functionality for MacOS.                                *
*                                                                             *
*  Developed by Stefan Csomor csomor@advanced.ch,                             *
*  Amiga version by Lorens Younes (d93-hyo@nada.kth.se)                       *
*  1.00 10/11/98 Initial attempt ;-)                                          *
\*****************************************************************************/

#ifndef MAC_X
#define MAC_X

#define Success   0

/* really never used */
#define ZPixmap   2

#define Bool     int
#define Status   int
#define True     1
#define False    0

typedef CTabHandle  	Colormap;

typedef void  *Visual;

typedef struct {
    	int               	width, height;
		GWorldPtr			gworldptr ;
}   XImage;

typedef struct {
    unsigned long    pixel;
    unsigned short   red, green, blue;
}   XColor;

typedef GDevice*   	Display; // actually this is a entire workstation (analogon x-server)

#define XGrabServer(dpy)     /*GrabServer*/
#define XUngrabServer(dpy)   /*UngrabServer*/

#define XDefaultScreen(dpy)          (GetMainDevice())
#define XDefaultVisual(dpy, scr)     (NULL)
#define XDefaultColormap(dpy, scr)   ((**((*dpy)->gdPMap)).pmTable)
#define XDefaultDepth(dpy, scr)      ((**((*dpy)->gdPMap)).pixelSize)

#define XCreateImage(dpy, vi, depth, format, offset, data, width, height, pad, bpl) \
	(AllocXImage (width, height, depth))
int XDestroyImage(XImage  *ximage) ;

Status XAllocColor( Display dpy, Colormap colormap, XColor  *screen_in_out ) ;
int XFreeColors(Display dpy, Colormap colormap, unsigned long  *pixels, int npixels, int planes) ;
Status XParseColor(Display dpy, Colormap colormap, char *spec, XColor *exact_def_return) ;
int XQueryColor(Display dpy, Colormap colormap, XColor *def_in_out) ;
int XQueryColors(Display dpy, Colormap colormap, XColor *defs_in_out, int ncolors) ;
XImage * AllocXImage (unsigned int   width, unsigned int   height,unsigned int   depth);
int XPutPixel ( XImage *ximage,int  x,int y,unsigned long   pixel);

#endif
