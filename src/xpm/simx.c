/*
 * Copyright (C) 1989-95 GROUPE BULL
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
 * GROUPE BULL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of GROUPE BULL shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from GROUPE BULL.
 */

/*****************************************************************************\
* simx.c: 0.1a                                                                *
*                                                                             *
* This emulates some Xlib functionality for MSW. It's not a general solution, *
* it is close related to XPM-lib. It is only intended to satisfy what is need *
* there. Thus allowing to read XPM files under MS windows.                    *
*                                                                             *
* Developed by HeDu 3/94 (hedu@cul-ipn.uni-kiel.de)                           *
\*****************************************************************************/

/* Moved here so that FOR_MSW gets defined if we are using wxWindows (GRG) */
#include "xpm.h"

#ifdef FOR_MSW

#include "XpmI.h"           /* for XpmMalloc */

/*
 * On DOS size_t is only 2 bytes, thus malloc(size_t s) can only malloc
 * 64K. BUT an expression data=malloc(width*height) may result in an
 * overflow. So this function takes a long as input, and returns NULL if the
 * request is larger than 64K, is size_t is only 2 bytes.
 *
 * This requires casts like XpmMalloc( (long)width*(long(height)), else it
 * might have no effect at all.
 */

void *
boundCheckingMalloc(long s)
{
    if (sizeof(size_t) == sizeof(long)) {	/* same size, just do it */
	return (malloc((size_t) s));
    } else {
	if (sizeof(size_t) == 2) {
	    if (s > 0xFFFF)
		return (NULL);		/* to large, size_t with 2 bytes
					 * only allows 16 bits */
	    else
		return (malloc((size_t) s));
	} else {			/* it's not a long, not 2 bytes,
					 * what is it ??? */
	    return (malloc((size_t) s));
	}
    }
}
void *
boundCheckingCalloc(long num, long s)
{
    if (sizeof(size_t) == sizeof(long)) {	/* same size, just do it */
	return (calloc((size_t) num, (size_t) s));
    } else {
	if (sizeof(size_t) == 2) {
	    if (s > 0xFFFF || num * s > 0xFFFF)
		return (NULL);		/* to large, size_t with 2 bytes
					 * only allows 16 bits */
	    else
		return (calloc((size_t) num, (size_t) s));
	} else {			/* it's not a long, not 2 bytes,
					 * what is it ??? */
	    return (calloc((size_t) num, (size_t) s));
	}
    }
}
void *
boundCheckingRealloc(void *p, long s)
{
    if (sizeof(size_t) == sizeof(long)) {	/* same size, just do it */
	return (realloc(p, (size_t) s));
    } else {
	if (sizeof(size_t) == 2) {
	    if (s > 0xFFFF)
		return (NULL);		/* to large, size_t with 2 bytes
					 * only allows 16 bits */
	    else
		return (realloc(p, (size_t) s));
	} else {			/* it's not a long, not 2 bytes,
					 * what is it ??? */
	    return (realloc(p, (size_t) s));
	}
    }
}

/* static Visual theVisual = { 0 }; */
Visual *
XDefaultVisual(Display *display, Screen *screen)
{
    return (NULL);			/* struct could contain info about
					 * MONO, GRAY, COLOR */
}

Screen *
XDefaultScreen(Display *d)
{
    return (NULL);
}

/* I get only 1 plane but 8 bits per pixel,
   so I think BITSPIXEL should be depth */
int
XDefaultDepth(Display *display, Screen *screen)
{
    int d, b;
#ifdef __OS2__
    HPS                             hpsScreen;
    HDC                             hdcScreen;
    LONG                            lPlanes;
    LONG                            lBitsPerPixel;
    LONG                            nDepth;

    hpsScreen = WinGetScreenPS(HWND_DESKTOP);
    hdcScreen = GpiQueryDevice(hpsScreen);
    DevQueryCaps(hdcScreen, CAPS_COLOR_PLANES, 1L, &lPlanes);
    DevQueryCaps(hdcScreen, CAPS_COLOR_BITCOUNT, 1L, &lBitsPerPixel);
    b = (int)lBitsPerPixel;
#else
    b = GetDeviceCaps(*display, BITSPIXEL);
    d = GetDeviceCaps(*display, PLANES);
#endif
    return (b);
}

Colormap *
XDefaultColormap(Display *display, Screen *screen)
{
    return (NULL);
}

/* convert hex color names,
   wrong digits (not a-f,A-F,0-9) are treated as zero */
static int
hexCharToInt(char c)
{
    int r;

    if (c >= '0' && c <= '9')
	r = c - '0';
    else if (c >= 'a' && c <= 'f')
	r = c - 'a' + 10;
    else if (c >= 'A' && c <= 'F')
	r = c - 'A' + 10;
    else
	r = 0;

    return (r);
}

static int
rgbFromHex(char *hex, int *r, int *g, int *b)
{
    int len;

    if (hex == NULL || hex[0] != '#')
	return (0);

    len = strlen(hex);
    if (len == 3 + 1) {
	*r = hexCharToInt(hex[1]);
	*g = hexCharToInt(hex[2]);
	*b = hexCharToInt(hex[3]);
    } else if (len == 6 + 1) {
	*r = hexCharToInt(hex[1]) * 16 + hexCharToInt(hex[2]);
	*g = hexCharToInt(hex[3]) * 16 + hexCharToInt(hex[4]);
	*b = hexCharToInt(hex[5]) * 16 + hexCharToInt(hex[6]);
    } else if (len == 12 + 1) {
	/* it's like c #32329999CCCC */
	/* so for now only take two digits */
	*r = hexCharToInt(hex[1]) * 16 + hexCharToInt(hex[2]);
	*g = hexCharToInt(hex[5]) * 16 + hexCharToInt(hex[6]);
	*b = hexCharToInt(hex[9]) * 16 + hexCharToInt(hex[10]);
    } else
	return (0);

    return (1);
}

/* Color related functions */
int
XParseColor(Display *d, Colormap *cmap, char *name, XColor *color)
{
    int r, g, b;			/* only 8 bit values used */
    int okay;

/* TODO: use colormap via PALETTE */
    /* parse name either in table or #RRGGBB #RGB */
    if (name == NULL)
	return (0);

    if (name[0] == '#') {		/* a hex string */
	okay = rgbFromHex(name, &r, &g, &b);
    } else {
	okay = xpmGetRGBfromName(name, &r, &g, &b);
    }

    if (okay) {
#ifdef __OS2__
	color->pixel = OS2RGB(r, g, b);
#else
	color->pixel = RGB(r, g, b);
#endif
	color->red = (BYTE) r;
	color->green = (BYTE) g;
	color->blue = (BYTE) b;
	return (1);
    } else
	return (0);			/* --> ColorError */
}


/* GRG: 2nd arg is Colormap*, not Colormap */

int
XAllocColor(Display *d, Colormap *cmap, XColor *color)
{
/* colormap not used yet so color->pixel is the real COLORREF (RBG) and not an
   index in some colormap as in X */
    return (1);
}
void
XQueryColors(Display *display, Colormap *colormap,
	     XColor *xcolors, int ncolors)
{
/* under X this fills the rgb values to given .pixel */
/* since there no colormap use FOR_MSW (not yet!!), rgb is plain encoded */
    XColor *xc = xcolors;
    int i;

    for (i = 0; i < ncolors; i++, xc++) {
	xc->red = GetRValue(xc->pixel);
	xc->green = GetGValue(xc->pixel);
	xc->blue = GetBValue(xc->pixel);
    }
    return;
}
int
XFreeColors(Display *d, Colormap cmap,
	    unsigned long pixels[], int npixels, unsigned long planes)
{
    /* no colormap yet */
    return (0);				/* correct ??? */
}

/* XImage functions */
XImage *
XCreateImage(Display *d, Visual *v,
	     int depth, int format,
	     int x, int y, int width, int height,
	     int pad, int foo)
{
    XImage *img = (XImage *) XpmMalloc(sizeof(XImage));
#ifdef __OS2__
    HPS                  hps;
    BITMAPINFOHEADER2    bmih;

    hps = WinGetScreenPS(HWND_DESKTOP);
    memset(&bmih, 0, sizeof(BITMAPINFOHEADER2));
    bmih.cbFix = sizeof(BITMAPINFOHEADER2);
    bmih.cx = width;
    bmih.cy = height;
    bmih.cPlanes = 1;
    bmih.cBitCount = depth;
#endif

    if (img) {
	/*JW: This is what it should be, but the picture comes out
	      just black!?  It appears to be doing monochrome reduction,
	      but I've got no clue why.  Using CreateBitmap() is supposed
	      to be slower, but otherwise ok
	  if ( depth == GetDeviceCaps(*d, BITSPIXEL) ) {
	    img->bitmap = CreateCompatibleBitmap(*d, width, height);
        } else*/ {
#ifdef __OS2__
     img->bitmap = GpiCreateBitmap(hps, &bmih, 0L, NULL, NULL);
#else
	    img->bitmap = CreateBitmap(width, height, 1 /* plane */ ,
				       depth /* bits per pixel */ , NULL);
#endif
	}
	img->width = width;
	img->height = height;
	img->depth = depth;
    }
    return (img);

}

void
XImageFree(XImage *img)
{
    if (img) {
	XpmFree(img);
    }
}
void
XDestroyImage(XImage *img)
{
    if (img) {
#ifdef __OS2__
     GpiDeleteBitmap(img->bitmap);
#else
	DeleteObject(img->bitmap);	/* check return ??? */
#endif
	XImageFree(img);
    }
}

#endif
