/*
 * Copyright (C) 1989-94 GROUPE BULL
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
* xpm.h:                                                                      *
*                                                                             *
*  XPM library                                                                *
*  Include file                                                               *
*                                                                             *
*  Developed by Arnaud Le Hors                                                *
\*****************************************************************************/

/*
 * The code related to FOR_MSW has been added by
 * HeDu (hedu@cul-ipn.uni-kiel.de) 4/94
 */

#ifndef XPM_h
#define XPM_h

#if (defined(_WINDOWS) || defined(__WXMSW__) || defined(WIN32)) && !defined(FOR_MSW)
#define FOR_MSW
#endif
/* Piggyback on MSW for now */
#if (defined(__OS2__) || defined(__WXPM__) || defined(OS232)) && !defined(FOR_MSW)
#define FOR_MSW
#endif

/*
 * first some identification numbers:
 * the following revision numbers is determined with the following rule:
 * SO Major number = LIB minor version number.
 * SO Minor number = LIB sub-minor version number.
 * e.g: Xpm version 3.2f
 *      we forget the 3 which is the format number, 2 gives 2, and f gives 6.
 *      thus we have XpmVersion = 2 and XpmRevision = 6
 *      which gives  SOXPMLIBREV = 2.6
 *
 * Then the XpmIncludeVersion number is built with the following rule:
 * (XpmFormat*100 + XpmVersion)*100 + XpmRevision
 */
#define XpmFormat 3
#define XpmVersion 4
#define XpmRevision 2

#define XpmIncludeVersion 30402

#ifndef XPM_NUMBERS

#ifdef VMS
#include "decw$include:Xlib.h"
#include "decw$include:Xutil.h"
#else /* VMS */
#ifdef FOR_MSW
#define SYSV			/* uses memcpy string.h etc. */
#include <malloc.h>
#include "simx.h"		/* defines some X stuff using MSW types */
#ifndef __GNUWIN32__
#define NEED_STRCASECMP		/* at least for MSVC++ */
#endif
#else /* FOR_MSW */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>	/* HEDU */
#endif /* FOR_MSW */
#endif /* VMS */

/* let's define Pixel if it is not done yet */
#if ! defined(_XtIntrinsic_h) && ! defined(PIXEL_ALREADY_TYPEDEFED)
typedef unsigned long Pixel;	/* Index into colormap */
#define PIXEL_ALREADY_TYPEDEFED
#endif

/* Return ErrorStatus codes:
 * null     if full success
 * positive if partial success
 * negative if failure
 */

#define XpmColorError    1
#define XpmSuccess       0
#define XpmOpenFailed   -1
#define XpmFileInvalid  -2
#define XpmNoMemory     -3
#define XpmColorFailed  -4

/* the following should help people wanting to use their own functions */
#define XpmFree(ptr) free(ptr)

typedef struct {
    char *name;			/* Symbolic color name */
    char *value;		/* Color value */
    Pixel pixel;		/* Color pixel */
}      XpmColorSymbol;

typedef struct {
    char *name;			/* name of the extension */
    unsigned int nlines;	/* number of lines in this extension */
    char **lines;		/* pointer to the extension array of strings */
}      XpmExtension;

typedef struct {
    char *string;		/* characters string */
    char *symbolic;		/* symbolic name */
    char *m_color;		/* monochrom default */
    char *g4_color;		/* 4 level grayscale default */
    char *g_color;		/* other level grayscale default */
    char *c_color;		/* color default */
}      XpmColor;

typedef struct {
    unsigned int width;		/* image width */
    unsigned int height;	/* image height */
    unsigned int cpp;		/* number of characters per pixel */
    unsigned int ncolors;	/* number of colors */
    XpmColor *colorTable;	/* list of related colors */
    unsigned int *data;		/* image data */
}      XpmImage;

typedef struct {
    unsigned long valuemask;	/* Specifies which attributes are defined */
    char *hints_cmt;		/* Comment of the hints section */
    char *colors_cmt;		/* Comment of the colors section */
    char *pixels_cmt;		/* Comment of the pixels section */
    unsigned int x_hotspot;	/* Returns the x hotspot's coordinate */
    unsigned int y_hotspot;	/* Returns the y hotspot's coordinate */
    unsigned int nextensions;	/* number of extensions */
    XpmExtension *extensions;	/* pointer to array of extensions */
}      XpmInfo;

typedef struct {
    unsigned long valuemask;		/* Specifies which attributes are
					 * defined */

    Visual *visual;			/* Specifies the visual to use */
    Colormap colormap;			/* Specifies the colormap to use */
    unsigned int depth;			/* Specifies the depth */
    unsigned int width;			/* Returns the width of the created
					 * pixmap */
    unsigned int height;		/* Returns the height of the created
					 * pixmap */
    unsigned int x_hotspot;		/* Returns the x hotspot's
					 * coordinate */
    unsigned int y_hotspot;		/* Returns the y hotspot's
					 * coordinate */
    unsigned int cpp;			/* Specifies the number of char per
					 * pixel */
    Pixel *pixels;			/* List of used color pixels */
    unsigned int npixels;		/* Number of pixels */
    XpmColorSymbol *colorsymbols;	/* Array of color symbols to
					 * override */
    unsigned int numsymbols;		/* Number of symbols */
    char *rgb_fname;			/* RGB text file name */
    unsigned int nextensions;		/* number of extensions */
    XpmExtension *extensions;		/* pointer to array of extensions */

    unsigned int ncolors;               /* Number of colors */
    XpmColor *colorTable;               /* Color table pointer */
/* 3.2 backward compatibility code */
    char *hints_cmt;                    /* Comment of the hints section */
    char *colors_cmt;                   /* Comment of the colors section */
    char *pixels_cmt;                   /* Comment of the pixels section */
/* end 3.2 bc */
    unsigned int mask_pixel;            /* Transparent pixel's color table
                                         * index */

    /* Color Allocation Directives */
    unsigned int exactColors;		/* Only use exact colors for visual */
    unsigned int closeness;		/* Allowable RGB deviation */
    unsigned int red_closeness;		/* Allowable red deviation */
    unsigned int green_closeness;	/* Allowable green deviation */
    unsigned int blue_closeness;	/* Allowable blue deviation */
    int color_key;			/* Use colors from this color set */

}      XpmAttributes;

/* XpmAttributes value masks bits */
#define XpmVisual	   (1L<<0)
#define XpmColormap	   (1L<<1)
#define XpmDepth	   (1L<<2)
#define XpmSize		   (1L<<3)	/* width & height */
#define XpmHotspot	   (1L<<4)	/* x_hotspot & y_hotspot */
#define XpmCharsPerPixel   (1L<<5)
#define XpmColorSymbols	   (1L<<6)
#define XpmRgbFilename	   (1L<<7)
/* 3.2 backward compatibility code */
#define XpmInfos	   (1L<<8)
#define XpmReturnInfos	   XpmInfos
/* end 3.2 bc */
#define XpmReturnPixels	   (1L<<9)
#define XpmExtensions      (1L<<10)
#define XpmReturnExtensions XpmExtensions

#define XpmExactColors     (1L<<11)
#define XpmCloseness	   (1L<<12)
#define XpmRGBCloseness	   (1L<<13)
#define XpmColorKey	   (1L<<14)

#define XpmColorTable      (1L<<15)
#define XpmReturnColorTable XpmColorTable

/* XpmInfo value masks bits */
#define XpmComments        XpmInfos
#define XpmReturnComments  XpmComments

/* XpmAttributes mask_pixel value when there is no mask */
#ifndef FOR_MSW
#define XpmUndefPixel 0x80000000
#else
/* int is only 16 bit for MSW */
#define XpmUndefPixel 0x8000
#endif

/*
 * color keys for visual type, they must fit along with the number key of
 * each related element in xpmColorKeys[] defined in xpmP.h
 */
#define XPM_MONO	2
#define XPM_GREY4	3
#define XPM_GRAY4	3
#define XPM_GREY 	4
#define XPM_GRAY 	4
#define XPM_COLOR	5


/*
 * minimal portability layer between ansi and KR C
 */

/* forward declaration of functions with prototypes */

#if __STDC__ || defined(__cplusplus) || defined(c_plusplus)
 /* ANSI || C++ */
#define FUNC(f, t, p) extern t f p
#define LFUNC(f, t, p) static t f p
/* #define LFUNC(f, t, p) t f p */
#else  /* K&R */
#define FUNC(f, t, p) extern t f()
#define LFUNC(f, t, p) static t f()
/* #define LFUNC(f, t, p) t f() */
#endif /* end of K&R */


/*
 * functions declarations
 */

#ifdef __cplusplus
extern "C" {
#endif

/* For Microsoft C++ at any rate, the FUNC macro just doesn't work: it causes
 * arguments to be corrupted espec. in XpmWriteFileFromXpmImage.
 * So, define all prototypes explicitly.
 */
#if defined(_MSC_VER) || defined(__OS2__)

/* FOR_MSW, all ..Pixmap.. are excluded, only the ..XImage.. are used */

    int XpmCreateImageFromData(Display *display,
				       char **data,
				       XImage **image_return,
				       XImage **shapemask_return,
				       XpmAttributes *attributes);

    int XpmCreateDataFromImage(Display *display,
				       char ***data_return,
				       XImage *image,
				       XImage *shapeimage,
				       XpmAttributes *attributes);

    int XpmReadFileToImage(Display *display,
				   char *filename,
				   XImage **image_return,
				   XImage **shapeimage_return,
				   XpmAttributes *attributes);

    int XpmWriteFileFromImage(Display *display,
				      char *filename,
				      XImage *image,
				      XImage *shapeimage,
				      XpmAttributes *attributes);

    int XpmCreateImageFromBuffer(Display *display,
					 char *buffer,
					 XImage **image_return,
					 XImage **shapemask_return,
					 XpmAttributes *attributes);

    int XpmReadFileToBuffer(char *filename, char **buffer_return);
    int XpmWriteFileFromBuffer(char *filename, char *buffer);

    int XpmReadFileToData(char *filename, char ***data_return);
    int XpmWriteFileFromData(char *filename, char **data);

    int XpmAttributesSize();
    void XpmFreeAttributes(XpmAttributes *attributes);
    void XpmFreeExtensions(XpmExtension *extensions,
				   int nextensions);

    void XpmFreeXpmImage(XpmImage *image);
    void XpmFreeXpmInfo(XpmInfo *info);
    char *XpmGetErrorString(int errcode);
    int XpmLibraryVersion();

    /* XpmImage functions */
    int XpmReadFileToXpmImage(char *filename,
				      XpmImage *image,
				      XpmInfo *info);

    int XpmWriteFileFromXpmImage(char *filename,
					 XpmImage *image,
					 XpmInfo *info);

    int XpmWriteFileFromXpmImage(char *filename, XpmImage *image, XpmInfo *info);

    int XpmCreateImageFromXpmImage(Display *display,
					   XpmImage *image,
					   XImage **image_return,
					   XImage **shapeimage_return,
					   XpmAttributes *attributes);

    int XpmCreateXpmImageFromImage(Display *display,
					   XImage *image,
					   XImage *shapeimage,
					   XpmImage *xpmimage,
					   XpmAttributes *attributes);
    int XpmCreateDataFromXpmImage(char ***data_return,
					  XpmImage *image,
					  XpmInfo *info);

    int XpmCreateXpmImageFromData(char **data,
					  XpmImage *image,
					  XpmInfo *info);

    int XpmCreateXpmImageFromBuffer(char *buffer,
					    XpmImage *image,
					    XpmInfo *info);

    int XpmCreateBufferFromXpmImage(char **buffer_return,
					    XpmImage *image,
					    XpmInfo *info);

#else // _MSC_VER

/* FOR_MSW, all ..Pixmap.. are excluded, only the ..XImage.. are used */

#ifndef FOR_MSW
    FUNC(XpmCreatePixmapFromData, int, (Display *display,
					Drawable d,
					char **data,
					Pixmap *pixmap_return,
					Pixmap *shapemask_return,
					XpmAttributes *attributes));

    FUNC(XpmCreateDataFromPixmap, int, (Display *display,
					char ***data_return,
					Pixmap pixmap,
					Pixmap shapemask,
					XpmAttributes *attributes));

    FUNC(XpmReadFileToPixmap, int, (Display *display,
				    Drawable d,
				    char *filename,
				    Pixmap *pixmap_return,
				    Pixmap *shapemask_return,
				    XpmAttributes *attributes));

    FUNC(XpmWriteFileFromPixmap, int, (Display *display,
				       char *filename,
				       Pixmap pixmap,
				       Pixmap shapemask,
				       XpmAttributes *attributes));
#endif  /* ndef FOR_MSW */

    FUNC(XpmCreateImageFromData, int, (Display *display,
				       char **data,
				       XImage **image_return,
				       XImage **shapemask_return,
				       XpmAttributes *attributes));

    FUNC(XpmCreateDataFromImage, int, (Display *display,
				       char ***data_return,
				       XImage *image,
				       XImage *shapeimage,
				       XpmAttributes *attributes));

    FUNC(XpmReadFileToImage, int, (Display *display,
				   char *filename,
				   XImage **image_return,
				   XImage **shapeimage_return,
				   XpmAttributes *attributes));

    FUNC(XpmWriteFileFromImage, int, (Display *display,
				      char *filename,
				      XImage *image,
				      XImage *shapeimage,
				      XpmAttributes *attributes));

    FUNC(XpmCreateImageFromBuffer, int, (Display *display,
					 char *buffer,
					 XImage **image_return,
					 XImage **shapemask_return,
					 XpmAttributes *attributes));
#ifndef FOR_MSW
    FUNC(XpmCreatePixmapFromBuffer, int, (Display *display,
					  Drawable d,
					  char *buffer,
					  Pixmap *pixmap_return,
					  Pixmap *shapemask_return,
					  XpmAttributes *attributes));

    FUNC(XpmCreateBufferFromImage, int, (Display *display,
					 char **buffer_return,
					 XImage *image,
					 XImage *shapeimage,
					 XpmAttributes *attributes));

    FUNC(XpmCreateBufferFromPixmap, int, (Display *display,
					  char **buffer_return,
					  Pixmap pixmap,
					  Pixmap shapemask,
					  XpmAttributes *attributes));
#endif  /* ndef FOR_MSW */
    FUNC(XpmReadFileToBuffer, int, (char *filename, char **buffer_return));
    FUNC(XpmWriteFileFromBuffer, int, (char *filename, char *buffer));

    FUNC(XpmReadFileToData, int, (char *filename, char ***data_return));
    FUNC(XpmWriteFileFromData, int, (char *filename, char **data));

    FUNC(XpmAttributesSize, int, ());
    FUNC(XpmFreeAttributes, void, (XpmAttributes *attributes));
    FUNC(XpmFreeExtensions, void, (XpmExtension *extensions,
				   int nextensions));

    FUNC(XpmFreeXpmImage, void, (XpmImage *image));
    FUNC(XpmFreeXpmInfo, void, (XpmInfo *info));
    FUNC(XpmGetErrorString, char *, (int errcode));
    FUNC(XpmLibraryVersion, int, ());

    /* XpmImage functions */
    FUNC(XpmReadFileToXpmImage, int, (char *filename,
				      XpmImage *image,
				      XpmInfo *info));

    FUNC(XpmWriteFileFromXpmImage, int, (char *filename,
					 XpmImage *image,
					 XpmInfo *info));

    FUNC(XpmWriteFileFromXpmImage, int, (char *filename, XpmImage *image, XpmInfo *info));
/*    extern int XpmWriteFileFromXpmImage(char *filename, XpmImage *image, XpmInfo *info); */

#ifndef FOR_MSW
    FUNC(XpmCreatePixmapFromXpmImage, int, (Display *display,
					    Drawable d,
					    XpmImage *image,
					    Pixmap *pixmap_return,
					    Pixmap *shapemask_return,
					    XpmAttributes *attributes));
#endif
    FUNC(XpmCreateImageFromXpmImage, int, (Display *display,
					   XpmImage *image,
					   XImage **image_return,
					   XImage **shapeimage_return,
					   XpmAttributes *attributes));

    FUNC(XpmCreateXpmImageFromImage, int, (Display *display,
					   XImage *image,
					   XImage *shapeimage,
					   XpmImage *xpmimage,
					   XpmAttributes *attributes));
#ifndef FOR_MSW
    FUNC(XpmCreateXpmImageFromPixmap, int, (Display *display,
					    Pixmap pixmap,
					    Pixmap shapemask,
					    XpmImage *xpmimage,
					    XpmAttributes *attributes));
#endif
    FUNC(XpmCreateDataFromXpmImage, int, (char ***data_return,
					  XpmImage *image,
					  XpmInfo *info));

    FUNC(XpmCreateXpmImageFromData, int, (char **data,
					  XpmImage *image,
					  XpmInfo *info));

    FUNC(XpmCreateXpmImageFromBuffer, int, (char *buffer,
					    XpmImage *image,
					    XpmInfo *info));

    FUNC(XpmCreateBufferFromXpmImage, int, (char **buffer_return,
					    XpmImage *image,
					    XpmInfo *info));

#endif // _MSC_VER
					
#ifdef __cplusplus
} /* for C++ V2.0 */
#endif


/* backward compatibility */

/* for version 3.0c */
#define XpmPixmapColorError  XpmColorError
#define XpmPixmapSuccess     XpmSuccess
#define XpmPixmapOpenFailed  XpmOpenFailed
#define XpmPixmapFileInvalid XpmFileInvalid
#define XpmPixmapNoMemory    XpmNoMemory
#define XpmPixmapColorFailed XpmColorFailed

#define XpmReadPixmapFile(dpy, d, file, pix, mask, att) \
    XpmReadFileToPixmap(dpy, d, file, pix, mask, att)
#define XpmWritePixmapFile(dpy, file, pix, mask, att) \
    XpmWriteFileFromPixmap(dpy, file, pix, mask, att)

/* for version 3.0b */
#define PixmapColorError  XpmColorError
#define PixmapSuccess     XpmSuccess
#define PixmapOpenFailed  XpmOpenFailed
#define PixmapFileInvalid XpmFileInvalid
#define PixmapNoMemory    XpmNoMemory
#define PixmapColorFailed XpmColorFailed

#define ColorSymbol XpmColorSymbol

#define XReadPixmapFile(dpy, d, file, pix, mask, att) \
    XpmReadFileToPixmap(dpy, d, file, pix, mask, att)
#define XWritePixmapFile(dpy, file, pix, mask, att) \
    XpmWriteFileFromPixmap(dpy, file, pix, mask, att)
#define XCreatePixmapFromData(dpy, d, data, pix, mask, att) \
    XpmCreatePixmapFromData(dpy, d, data, pix, mask, att)
#define XCreateDataFromPixmap(dpy, data, pix, mask, att) \
    XpmCreateDataFromPixmap(dpy, data, pix, mask, att)

#endif /* XPM_NUMBERS */
#endif
