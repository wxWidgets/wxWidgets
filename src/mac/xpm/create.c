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
* create.c:                                                                   *
*                                                                             *
*  XPM library                                                                *
*  Create an X image and possibly its related shape mask                      *
*  from the given XpmImage.                                                   *
*                                                                             *
*  Developed by Arnaud Le Hors                                                *
\*****************************************************************************/

/*
 * The code related to FOR_MSW has been added by
 * HeDu (hedu@cul-ipn.uni-kiel.de) 4/94
 */

/*
 * The code related to AMIGA has been added by
 * Lorens Younes (d93-hyo@nada.kth.se) 4/96
 */

#include "XpmI.h"
#include <ctype.h>

LFUNC(xpmVisualType, int, (Visual *visual));

LFUNC(AllocColor, int, (Display *display, Colormap colormap,
			char *colorname, XColor *xcolor, void *closure));
LFUNC(FreeColors, int, (Display *display, Colormap colormap,
			Pixel *pixels, int n, void *closure));

LFUNC(SetColor, int, (Display *display, Colormap colormap, Visual *visual,
		      char *colorname, unsigned int color_index,
		      Pixel *image_pixel, Pixel *mask_pixel,
		      unsigned int *mask_pixel_index,
		      Pixel *alloc_pixels, unsigned int *nalloc_pixels,
		      Pixel *used_pixels, unsigned int *nused_pixels,
		      XpmAttributes *attributes, XColor *cols, int ncols,
		      XpmAllocColorFunc allocColor, void *closure));

LFUNC(CreateXImage, int, (Display *display, Visual *visual,
			  unsigned int depth, int format, unsigned int width,
                          unsigned int height, XImage **image_return));

LFUNC(CreateColors, int, (Display *display, XpmAttributes *attributes,
                          XpmColor *colors, unsigned int ncolors,
                          Pixel *image_pixels, Pixel *mask_pixels,
                          unsigned int *mask_pixel_index,
                          Pixel *alloc_pixels, unsigned int *nalloc_pixels,
                          Pixel *used_pixels, unsigned int *nused_pixels));

LFUNC(ParseAndPutPixels, int, (Display *dc, xpmData *data, unsigned int width,
			       unsigned int height, unsigned int ncolors,
			       unsigned int cpp, XpmColor *colorTable,
			       xpmHashTable *hashtable,
			       XImage *image, Pixel *image_pixels,
			       XImage *mask, Pixel *mask_pixels));

LFUNC(PlatformPutImagePixels, void, (Display *dc, XImage *image,
				unsigned int width, unsigned int height,
				unsigned int *pixelindex, Pixel *pixels));


#ifdef NEED_STRCASECMP
FUNC(xpmstrcasecmp, int, (char *s1, char *s2));

/*
 * in case strcasecmp is not provided by the system here is one
 * which does the trick
 */
int
xpmstrcasecmp(s1, s2)
    register char *s1, *s2;
{
    register int c1, c2;

    while (*s1 && *s2) {
	c1 = tolower(*s1);
	c2 = tolower(*s2);
	if (c1 != c2)
	    return (c1 - c2);
	s1++;
	s2++;
    }
    return (int) (*s1 - *s2);
}

#endif

/*
 * return the default color key related to the given visual
 */
static int
xpmVisualType(visual)
    Visual *visual;
{
    return (XPM_COLOR);
}


typedef struct {
    int cols_index;
    long closeness;
}      CloseColor;

static int
closeness_cmp(a, b)
    Const void *a, *b;
{
    CloseColor *x = (CloseColor *) a, *y = (CloseColor *) b;

    /* cast to int as qsort requires */
    return (int) (x->closeness - y->closeness);
}


/* default AllocColor function:
 *   call XParseColor if colorname is given, return negative value if failure
 *   call XAllocColor and return 0 if failure, positive otherwise
 */
static int
AllocColor(display, colormap, colorname, xcolor, closure)
    Display *display;
    Colormap colormap;
    char *colorname;
    XColor *xcolor;
    void *closure;		/* not used */
{
    int status;
    if (colorname)
	if (!XParseColor(display, colormap, colorname, xcolor))
	    return -1;
    status = XAllocColor(display, colormap, xcolor);
    return status != 0 ? 1 : 0;
}

/*
 * set the color pixel related to the given colorname,
 * return 0 if success, 1 otherwise.
 */

static int
SetColor(display, colormap, visual, colorname, color_index,
	 image_pixel, mask_pixel, mask_pixel_index,
	 alloc_pixels, nalloc_pixels, used_pixels, nused_pixels,
	 attributes, cols, ncols, allocColor, closure)
    Display *display;
    Colormap colormap;
    Visual *visual;
    char *colorname;
    unsigned int color_index;
    Pixel *image_pixel, *mask_pixel;
    unsigned int *mask_pixel_index;
    Pixel *alloc_pixels;
    unsigned int *nalloc_pixels;
    Pixel *used_pixels;
    unsigned int *nused_pixels;
    XpmAttributes *attributes;
    XColor *cols;
    int ncols;
    XpmAllocColorFunc allocColor;
    void *closure;
{
    XColor xcolor;
    int status;

    if (xpmstrcasecmp(colorname, TRANSPARENT_COLOR)) 
	{
		status = (*allocColor)(display, colormap, colorname, &xcolor, closure);
		if (status < 0)		/* parse color failed */
		    return (1);

		if (status == 0) {
			return (1);
		} else
		    alloc_pixels[(*nalloc_pixels)++] = xcolor.pixel;
		*image_pixel = xcolor.pixel;

		SET_ZERO_PIXEL( *mask_pixel );

		used_pixels[(*nused_pixels)++] = xcolor.pixel;
	} 
	else 
	{
		// this is a special for mac - we have to get white as background for transparency
		#ifdef macintosh
  		SET_WHITE_PIXEL( *image_pixel) ;
		#else
		SET_ZERO_PIXEL( *image_pixel);
		#endif
  		SET_WHITE_PIXEL( *mask_pixel) ;

		/* store the color table index */
		*mask_pixel_index = color_index;
    }
    return (0);
}


static int
CreateColors(display, attributes, colors, ncolors, image_pixels, mask_pixels,
             mask_pixel_index, alloc_pixels, nalloc_pixels,
             used_pixels, nused_pixels)
    Display *display;
    XpmAttributes *attributes;
    XpmColor *colors;
    unsigned int ncolors;
    Pixel *image_pixels;
    Pixel *mask_pixels;
    unsigned int *mask_pixel_index;
    Pixel *alloc_pixels;
    unsigned int *nalloc_pixels;
    Pixel *used_pixels;
    unsigned int *nused_pixels;
{
    /* variables stored in the XpmAttributes structure */
    Visual *visual;
    Colormap colormap;
    XpmColorSymbol *colorsymbols;
    unsigned int numsymbols;
    XpmAllocColorFunc allocColor;
    void *closure;

    char *colorname;
    unsigned int color, key;
    Bool pixel_defined;
    XpmColorSymbol *symbol;
    char **defaults;
    int ErrorStatus = XpmSuccess;
    char *s;
    int default_index;

    XColor *cols = NULL;
    unsigned int ncols = 0;

    /*
     * retrieve information from the XpmAttributes
     */
    if (attributes && attributes->valuemask & XpmColorSymbols) {
	colorsymbols = attributes->colorsymbols;
	numsymbols = attributes->numsymbols;
    } else
	numsymbols = 0;

    if (attributes && attributes->valuemask & XpmVisual)
	visual = attributes->visual;
    else
	visual = XDefaultVisual(display, XDefaultScreen(display));

    if (attributes && (attributes->valuemask & XpmColormap))
	colormap = attributes->colormap;
    else
	colormap = XDefaultColormap(display, XDefaultScreen(display));

    if (attributes && (attributes->valuemask & XpmColorKey))
	key = attributes->color_key;
    else
	key = xpmVisualType(visual);

    if (attributes && (attributes->valuemask & XpmAllocColor))
	allocColor = attributes->alloc_color;
    else
	allocColor = AllocColor;
    if (attributes && (attributes->valuemask & XpmColorClosure))
	closure = attributes->color_closure;
    else
	closure = NULL;

    switch (key) {
    case XPM_MONO:
	default_index = 2;
	break;
    case XPM_GRAY4:
	default_index = 3;
	break;
    case XPM_GRAY:
	default_index = 4;
	break;
    case XPM_COLOR:
    default:
	default_index = 5;
	break;
    }

    for (color = 0; color < ncolors; color++, colors++,
					 image_pixels++, mask_pixels++) {
	colorname = NULL;
	pixel_defined = False;
	defaults = (char **) colors;

	/*
	 * look for a defined symbol
	 */
	if (numsymbols) {

	    unsigned int n;

	    s = defaults[1];
	    for (n = 0, symbol = colorsymbols; n < numsymbols; n++, symbol++) {
		if (symbol->name && s && !strcmp(symbol->name, s))
		    /* override name */
		    break;
		if (!symbol->name && symbol->value) {	/* override value */
		    int def_index = default_index;

		    while (defaults[def_index] == NULL)	/* find defined
							 * colorname */
			--def_index;
		    if (def_index < 2) {/* nothing towards mono, so try
					 * towards color */
			def_index = default_index + 1;
			while (def_index <= 5 && defaults[def_index] == NULL)
			    ++def_index;
		    }
		    if (def_index >= 2 && defaults[def_index] != NULL &&
			!xpmstrcasecmp(symbol->value, defaults[def_index]))
			break;
		}
	    }
	    if (n != numsymbols) {
		if (symbol->name && symbol->value)
		    colorname = symbol->value;
		else
		    pixel_defined = True;
	    }
	}
	if (!pixel_defined) {		/* pixel not given as symbol value */

	    unsigned int k;

	    if (colorname) {		/* colorname given as symbol value */
		if (!SetColor(display, colormap, visual, colorname, color,
			      image_pixels, mask_pixels, mask_pixel_index,
			      alloc_pixels, nalloc_pixels, used_pixels,
			      nused_pixels, attributes, cols, ncols,
			      allocColor, closure))
		    pixel_defined = True;
		else
		    ErrorStatus = XpmColorError;
	    }
	    k = key;
	    while (!pixel_defined && k > 1) {
		if (defaults[k]) {
		    if (!SetColor(display, colormap, visual, defaults[k],
				  color, image_pixels, mask_pixels,
				  mask_pixel_index, alloc_pixels,
				  nalloc_pixels, used_pixels, nused_pixels,
				  attributes, cols, ncols,
				  allocColor, closure)) {
			pixel_defined = True;
			break;
		    } else
			ErrorStatus = XpmColorError;
		}
		k--;
	    }
	    k = key + 1;
	    while (!pixel_defined && k < NKEYS + 1) {
		if (defaults[k]) {
		    if (!SetColor(display, colormap, visual, defaults[k],
				  color, image_pixels, mask_pixels,
				  mask_pixel_index, alloc_pixels,
				  nalloc_pixels, used_pixels, nused_pixels,
				  attributes, cols, ncols,
				  allocColor, closure)) {
			pixel_defined = True;
			break;
		    } else
			ErrorStatus = XpmColorError;
		}
		k++;
	    }
	    if (!pixel_defined) {
		if (cols)
		    XpmFree(cols);
		return (XpmColorFailed);
	    }
	} else {
	    /* simply use the given pixel */
	    *image_pixels = symbol->pixel;
	    /* the following makes the mask to be built even if none
	       is given a particular pixel */
	    if (symbol->value
		&& !xpmstrcasecmp(symbol->value, TRANSPARENT_COLOR)) 
		{
			SET_ZERO_PIXEL( *mask_pixels ) ;
			*mask_pixel_index = color;
	    } 
	    else
	    {
	    	#if defined(macintosh) || defined(__APPLE__)
			SET_WHITE_PIXEL( *mask_pixels ) ; // is this correct CS ????
			#else
			*mask_pixels = 1; // is this correct CS ????
			#endif
		}
	    used_pixels[(*nused_pixels)++] = *image_pixels;
	}
    }
    if (cols)
	XpmFree(cols);
    return (ErrorStatus);
}


/* default FreeColors function, simply call XFreeColors */
static int
FreeColors(display, colormap, pixels, n, closure)
    Display *display;
    Colormap colormap;
    Pixel *pixels;
    int n;
    void *closure;		/* not used */
{
    return XFreeColors(display, colormap, pixels, n, 0);
}


/* function call in case of error */
#undef RETURN
#define RETURN(status) \
{ \
      ErrorStatus = status; \
      goto error; \
}

int
XpmCreateImageFromXpmImage(display, image,
			   image_return, shapeimage_return, attributes)
    Display *display;
    XpmImage *image;
    XImage **image_return;
    XImage **shapeimage_return;
    XpmAttributes *attributes;
{
    /* variables stored in the XpmAttributes structure */
    Visual *visual;
    Colormap colormap;
    unsigned int depth;
    int bitmap_format;
    XpmFreeColorsFunc freeColors;
    void *closure;

    /* variables to return */
    XImage *ximage = NULL;
    XImage *shapeimage = NULL;
    unsigned int mask_pixel_index = XpmUndefPixel;
    int ErrorStatus;

    /* calculation variables */
    Pixel *image_pixels = NULL;
    Pixel *mask_pixels = NULL;
    Pixel *alloc_pixels = NULL;
    Pixel *used_pixels = NULL;
    unsigned int nalloc_pixels = 0;
    unsigned int nused_pixels = 0;

    /* initialize return values */
    if (image_return)
	*image_return = NULL;
    if (shapeimage_return)
	*shapeimage_return = NULL;

    /* retrieve information from the XpmAttributes */
    if (attributes && (attributes->valuemask & XpmVisual))
	visual = attributes->visual;
    else
	visual = XDefaultVisual(display, XDefaultScreen(display));

    if (attributes && (attributes->valuemask & XpmColormap))
	colormap = attributes->colormap;
    else
	colormap = XDefaultColormap(display, XDefaultScreen(display));

    if (attributes && (attributes->valuemask & XpmDepth))
	depth = attributes->depth;
    else
	depth = XDefaultDepth(display, XDefaultScreen(display));

    if (attributes && (attributes->valuemask & XpmBitmapFormat))
	bitmap_format = attributes->bitmap_format;
    else
	bitmap_format = ZPixmap;

    if (attributes && (attributes->valuemask & XpmFreeColors))
	freeColors = attributes->free_colors;
    else
	freeColors = FreeColors;
    if (attributes && (attributes->valuemask & XpmColorClosure))
	closure = attributes->color_closure;
    else
	closure = NULL;

    ErrorStatus = XpmSuccess;

    /* malloc pixels index tables */
    image_pixels = (Pixel *) XpmMalloc(sizeof(Pixel) * image->ncolors);
    if (!image_pixels)
	return (XpmNoMemory);

    mask_pixels = (Pixel *) XpmMalloc(sizeof(Pixel) * image->ncolors);
    if (!mask_pixels)
	RETURN(XpmNoMemory);

    /* maximum of allocated pixels will be the number of colors */
    alloc_pixels = (Pixel *) XpmMalloc(sizeof(Pixel) * image->ncolors);
    if (!alloc_pixels)
	RETURN(XpmNoMemory);

    /* maximum of allocated pixels will be the number of colors */
    used_pixels = (Pixel *) XpmMalloc(sizeof(Pixel) * image->ncolors);
    if (!used_pixels)
	RETURN(XpmNoMemory);

    /* get pixel colors, store them in index tables */
    ErrorStatus = CreateColors(display, attributes, image->colorTable,
			       image->ncolors, image_pixels, mask_pixels,
			       &mask_pixel_index, alloc_pixels, &nalloc_pixels,
			       used_pixels, &nused_pixels);

    if (ErrorStatus != XpmSuccess
	&& (ErrorStatus < 0 || (attributes
				&& (attributes->valuemask & XpmExactColors)
				&& attributes->exactColors)))
	RETURN(ErrorStatus);

    /* create the ximage */
    if (image_return) {
	ErrorStatus = CreateXImage(display, visual, depth,
				   (depth == 1 ? bitmap_format : ZPixmap),
				   image->width, image->height, &ximage);
	if (ErrorStatus != XpmSuccess)
	    RETURN(ErrorStatus);

	PlatformPutImagePixels(display, ximage, image->width, image->height,
			  image->data, image_pixels);

    }
    /* create the shape mask image */
    if (mask_pixel_index != XpmUndefPixel && shapeimage_return) {
	ErrorStatus = CreateXImage(display, visual, 1, bitmap_format,
				   image->width, image->height, &shapeimage);
	if (ErrorStatus != XpmSuccess)
	    RETURN(ErrorStatus);

	PlatformPutImagePixels(display, shapeimage, image->width, image->height,
			  image->data, mask_pixels);

    }
    XpmFree(image_pixels);
    XpmFree(mask_pixels);

    /* if requested return used pixels in the XpmAttributes structure */
    if (attributes && (attributes->valuemask & XpmReturnPixels ||
/* 3.2 backward compatibility code */
	attributes->valuemask & XpmReturnInfos)) {
/* end 3.2 bc */
	attributes->pixels = used_pixels;
	attributes->npixels = nused_pixels;
	attributes->mask_pixel = mask_pixel_index;
    } else
	XpmFree(used_pixels);

    /* if requested return alloc'ed pixels in the XpmAttributes structure */
    if (attributes && (attributes->valuemask & XpmReturnAllocPixels)) {
	attributes->alloc_pixels = alloc_pixels;
	attributes->nalloc_pixels = nalloc_pixels;
    } else
	XpmFree(alloc_pixels);

    /* return created images */
    if (image_return)
	*image_return = ximage;
    if (shapeimage_return)
	*shapeimage_return = shapeimage;

    return (ErrorStatus);

/* exit point in case of error, free only locally allocated variables */
error:
    if (ximage)
	XDestroyImage(ximage);
    if (shapeimage)
	XDestroyImage(shapeimage);
    if (image_pixels)
	XpmFree(image_pixels);
    if (mask_pixels)
	XpmFree(mask_pixels);
    if (nalloc_pixels)
	(*freeColors)(display, colormap, alloc_pixels, nalloc_pixels, NULL);
    if (alloc_pixels)
	XpmFree(alloc_pixels);
    if (used_pixels)
	XpmFree(used_pixels);

    return (ErrorStatus);
}


/*
 * Create an XImage with its data
 */
static int
CreateXImage(display, visual, depth, format, width, height, image_return)
    Display *display;
    Visual *visual;
    unsigned int depth;
    int format;
    unsigned int width;
    unsigned int height;
    XImage **image_return;
{
    int bitmap_pad;

    /* first get bitmap_pad */
    if (depth > 16)
	bitmap_pad = 32;
    else if (depth > 8)
	bitmap_pad = 16;
    else
	bitmap_pad = 8;

    /* then create the XImage with data = NULL and bytes_per_line = 0 */
    *image_return = XCreateImage(display, visual, depth, format, 0, 0,
				 width, height, bitmap_pad, 0);
    if (!*image_return)
	return (XpmNoMemory);

    /* XCreateImage has done it all */

    return (XpmSuccess);
}

static void
PlatformPutImagePixels(dc, image, width, height, pixelindex, pixels)
    Display *dc;
    XImage *image;
    unsigned int width;
    unsigned int height;
    unsigned int *pixelindex;
    Pixel *pixels;
{

#ifdef FOR_MSW
    unsigned int *data = pixelindex;
    unsigned int x, y;
    HBITMAP obm;

    obm = SelectObject(*dc, image->bitmap);
    for (y = 0; y < height; y++) {
	for (x = 0; x < width; x++) {
	    SetPixel(*dc, x, y, pixels[*(data++)]); // data is [x+y*width] 
	}
    }
    SelectObject(*dc, obm);
#elif defined(macintosh) || defined(__APPLE__)
	GrafPtr 	origPort ;
	GDHandle	origDevice ;
	
    unsigned int *data = pixelindex;
    unsigned int x, y;

	GetGWorld( &origPort , &origDevice ) ;
	SetGWorld( image->gworldptr , NULL ) ;

    for (y = 0; y < height; y++) 
    {
		for (x = 0; x < width; x++) 
		{
	    	SetCPixel( x, y, &pixels[*(data++)]); // data is [x+y*width] 
		}
    }
    SetGWorld( origPort , origDevice ) ;
#endif
}

/*
 * This function parses an Xpm file or data and directly create an XImage
 */
int
xpmParseDataAndCreate(display, data, image_return, shapeimage_return,
		      image, info, attributes)
    Display *display;
    xpmData *data;
    XImage **image_return;
    XImage **shapeimage_return;
    XpmImage *image;
    XpmInfo *info;
    XpmAttributes *attributes;
{
    /* variables stored in the XpmAttributes structure */
    Visual *visual;
    Colormap colormap;
    unsigned int depth;
    int bitmap_format;
    XpmFreeColorsFunc freeColors;
    void *closure;

    /* variables to return */
    XImage *ximage = NULL;
    XImage *shapeimage = NULL;
    unsigned int mask_pixel_index = XpmUndefPixel;

    /* calculation variables */
    Pixel *image_pixels = NULL;
    Pixel *mask_pixels = NULL;
    Pixel *alloc_pixels = NULL;
    Pixel *used_pixels = NULL;
    unsigned int nalloc_pixels = 0;
    unsigned int nused_pixels = 0;
    unsigned int width, height, ncolors, cpp;
    unsigned int x_hotspot, y_hotspot, hotspot = 0, extensions = 0;
    XpmColor *colorTable = NULL;
    char *hints_cmt = NULL;
    char *colors_cmt = NULL;
    char *pixels_cmt = NULL;

    unsigned int cmts;
    int ErrorStatus;
    xpmHashTable hashtable;


    /* initialize return values */
    if (image_return)
	*image_return = NULL;
    if (shapeimage_return)
	*shapeimage_return = NULL;


    /* retrieve information from the XpmAttributes */
    if (attributes && (attributes->valuemask & XpmVisual))
	visual = attributes->visual;
    else
	visual = XDefaultVisual(display, XDefaultScreen(display));

    if (attributes && (attributes->valuemask & XpmColormap))
	colormap = attributes->colormap;
    else
	colormap = XDefaultColormap(display, XDefaultScreen(display));

    if (attributes && (attributes->valuemask & XpmDepth))
	depth = attributes->depth;
    else
	depth = XDefaultDepth(display, XDefaultScreen(display));

    if (attributes && (attributes->valuemask & XpmBitmapFormat))
	bitmap_format = attributes->bitmap_format;
    else
	bitmap_format = ZPixmap;

    if (attributes && (attributes->valuemask & XpmFreeColors))
	freeColors = attributes->free_colors;
    else
	freeColors = FreeColors;
    if (attributes && (attributes->valuemask & XpmColorClosure))
	closure = attributes->color_closure;
    else
	closure = NULL;

    cmts = info && (info->valuemask & XpmReturnComments);

    /*
     * parse the header
     */
    ErrorStatus = xpmParseHeader(data);
    if (ErrorStatus != XpmSuccess)
	return (ErrorStatus);

    /*
     * read values
     */
    ErrorStatus = xpmParseValues(data, &width, &height, &ncolors, &cpp,
				 &x_hotspot, &y_hotspot, &hotspot,
				 &extensions);
    if (ErrorStatus != XpmSuccess)
	return (ErrorStatus);

    /*
     * store the hints comment line
     */
    if (cmts)
	xpmGetCmt(data, &hints_cmt);

    /*
     * init the hastable
     */
    if (USE_HASHTABLE) {
	ErrorStatus = xpmHashTableInit(&hashtable);
	if (ErrorStatus != XpmSuccess)
	    return (ErrorStatus);
    }

    /*
     * read colors
     */
    ErrorStatus = xpmParseColors(data, ncolors, cpp, &colorTable, &hashtable);
    if (ErrorStatus != XpmSuccess)
	RETURN(ErrorStatus);

    /*
     * store the colors comment line
     */
    if (cmts)
	xpmGetCmt(data, &colors_cmt);

    /* malloc pixels index tables */
    image_pixels = (Pixel *) XpmMalloc(sizeof(Pixel) * ncolors);
    if (!image_pixels)
	RETURN(XpmNoMemory);

    mask_pixels = (Pixel *) XpmMalloc(sizeof(Pixel) * ncolors);
    if (!mask_pixels)
	RETURN(XpmNoMemory);

    /* maximum of allocated pixels will be the number of colors */
    alloc_pixels = (Pixel *) XpmMalloc(sizeof(Pixel) * ncolors);
    if (!alloc_pixels)
	RETURN(XpmNoMemory);

    /* maximum of allocated pixels will be the number of colors */
    used_pixels = (Pixel *) XpmMalloc(sizeof(Pixel) * ncolors);
    if (!used_pixels)
	RETURN(XpmNoMemory);

    /* get pixel colors, store them in index tables */
    ErrorStatus = CreateColors(display, attributes, colorTable, ncolors,
			       image_pixels, mask_pixels, &mask_pixel_index,
			       alloc_pixels, &nalloc_pixels, used_pixels,
			       &nused_pixels);

    if (ErrorStatus != XpmSuccess
	&& (ErrorStatus < 0 || (attributes
				&& (attributes->valuemask & XpmExactColors)
				&& attributes->exactColors)))
	RETURN(ErrorStatus);

    /* now create the ximage */
    if (image_return) {
	ErrorStatus = CreateXImage(display, visual, depth,
				   (depth == 1 ? bitmap_format : ZPixmap),
				   width, height, &ximage);
	if (ErrorStatus != XpmSuccess)
	    RETURN(ErrorStatus);

    }

    /* create the shape mask image */
    if (mask_pixel_index != XpmUndefPixel && shapeimage_return) {
	ErrorStatus = CreateXImage(display, visual, 1, bitmap_format,
				   width, height, &shapeimage);
	if (ErrorStatus != XpmSuccess)
	    RETURN(ErrorStatus);

    }

    /*
     * read pixels and put them in the XImage
     */
    ErrorStatus = ParseAndPutPixels(
				    display,
				    data, width, height, ncolors, cpp,
				    colorTable, &hashtable,
				    ximage, image_pixels,
				    shapeimage, mask_pixels);
    XpmFree(image_pixels);
    image_pixels = NULL;
    XpmFree(mask_pixels);
    mask_pixels = NULL;

    /*
     * free the hastable
     */
    if (ErrorStatus != XpmSuccess)
	RETURN(ErrorStatus)
    else if (USE_HASHTABLE)
	xpmHashTableFree(&hashtable);

    /*
     * store the pixels comment line
     */
    if (cmts)
	xpmGetCmt(data, &pixels_cmt);

    /*
     * parse extensions
     */
    if (info && (info->valuemask & XpmReturnExtensions))
	if (extensions) {
	    ErrorStatus = xpmParseExtensions(data, &info->extensions,
					     &info->nextensions);
	    if (ErrorStatus != XpmSuccess)
		RETURN(ErrorStatus);
	} else {
	    info->extensions = NULL;
	    info->nextensions = 0;
	}

    /*
     * store found informations in the XpmImage structure
     */
    image->width = width;
    image->height = height;
    image->cpp = cpp;
    image->ncolors = ncolors;
    image->colorTable = colorTable;
    image->data = NULL;

    if (info) {
	if (cmts) {
	    info->hints_cmt = hints_cmt;
	    info->colors_cmt = colors_cmt;
	    info->pixels_cmt = pixels_cmt;
	}
	if (hotspot) {
	    info->x_hotspot = x_hotspot;
	    info->y_hotspot = y_hotspot;
	    info->valuemask |= XpmHotspot;
	}
    }
    /* if requested return used pixels in the XpmAttributes structure */
    if (attributes && (attributes->valuemask & XpmReturnPixels ||
/* 3.2 backward compatibility code */
	attributes->valuemask & XpmReturnInfos)) {
/* end 3.2 bc */
	attributes->pixels = used_pixels;
	attributes->npixels = nused_pixels;
	attributes->mask_pixel = mask_pixel_index;
    } else
	XpmFree(used_pixels);

    /* if requested return alloc'ed pixels in the XpmAttributes structure */
    if (attributes && (attributes->valuemask & XpmReturnAllocPixels)) {
	attributes->alloc_pixels = alloc_pixels;
	attributes->nalloc_pixels = nalloc_pixels;
    } else
	XpmFree(alloc_pixels);

    /* return created images */
    if (image_return)
	*image_return = ximage;
    if (shapeimage_return)
	*shapeimage_return = shapeimage;

    return (XpmSuccess);

/* exit point in case of error, free only locally allocated variables */
error:
    if (USE_HASHTABLE)
	xpmHashTableFree(&hashtable);
    if (colorTable)
	xpmFreeColorTable(colorTable, ncolors);
    if (hints_cmt)
	XpmFree(hints_cmt);
    if (colors_cmt)
	XpmFree(colors_cmt);
    if (pixels_cmt)
	XpmFree(pixels_cmt);
    if (ximage)
	XDestroyImage(ximage);
    if (shapeimage)
	XDestroyImage(shapeimage);
    if (image_pixels)
	XpmFree(image_pixels);
    if (mask_pixels)
	XpmFree(mask_pixels);
    if (nalloc_pixels)
	(*freeColors)(display, colormap, alloc_pixels, nalloc_pixels, NULL);
    if (alloc_pixels)
	XpmFree(alloc_pixels);
    if (used_pixels)
	XpmFree(used_pixels);

    return (ErrorStatus);
}

static int
ParseAndPutPixels(
		  dc,
		  data, width, height, ncolors, cpp, colorTable, hashtable,
		  image, image_pixels, shapeimage, shape_pixels)
    Display *dc;
    xpmData *data;
    unsigned int width;
    unsigned int height;
    unsigned int ncolors;
    unsigned int cpp;
    XpmColor *colorTable;
    xpmHashTable *hashtable;
    XImage *image;
    Pixel *image_pixels;
    XImage *shapeimage;
    Pixel *shape_pixels;
{
    unsigned int a, x, y;

    switch (cpp) {

    case (1):				/* Optimize for single character
					 * colors */
	{
	    unsigned short colidx[256];
	    #ifdef FOR_MSW

	    HDC shapedc;
	    HBITMAP obm, sobm;

	    if ( shapeimage ) {
		shapedc = CreateCompatibleDC(*dc);
		sobm = SelectObject(shapedc, shapeimage->bitmap);
	    } else {
	        shapedc = NULL;
	    }
	    obm = SelectObject(*dc, image->bitmap);

		#elif defined(macintosh) || defined(__APPLE__)
		GrafPtr 	origPort ;
		GDHandle	origDevice ;
		
		GetGWorld( &origPort , &origDevice ) ;
		// ignore shapedc
		SetGWorld( image->gworldptr , NULL ) ;
		
		#endif
		
	    bzero((char *)colidx, 256 * sizeof(short));
	    for (a = 0; a < ncolors; a++)
		colidx[(unsigned char)colorTable[a].string[0]] = a + 1;

	    for (y = 0; y < height; y++) 
	    {
		xpmNextString(data);
			for (x = 0; x < width; x++) 
			{
			    int c = xpmGetC(data);

			    if (c > 0 && c < 256 && colidx[c] != 0) 
				{
	   				#if FOR_MSW
					SetPixel(*dc, x, y, image_pixels[colidx[c] - 1]);
					if (shapedc) 
					{
					    SetPixel(shapedc, x, y, shape_pixels[colidx[c] - 1]);
					}
					#elif defined(macintosh) || defined(__APPLE__)
					SetCPixel( x, y, &image_pixels[colidx[c] - 1]);
					if (shapeimage) 
					{
						SetGWorld( shapeimage->gworldptr , NULL ) ;
					    SetCPixel( x, y, &shape_pixels[colidx[c] - 1]);
						SetGWorld( image->gworldptr , NULL ) ;
					}
					#endif
				} 
				else
					return (XpmFileInvalid);
			}
	    }
	   	#if FOR_MSW
	    if ( shapedc ) 
	    {
	        SelectObject(shapedc, sobm);
			DeleteDC(shapedc);
	    }
	    SelectObject(*dc, obm);
	   	#elif defined(macintosh) || defined(__APPLE__)
    	SetGWorld( origPort , origDevice ) ;
	   	#endif
	}
	break;

    case (2):				/* Optimize for double character
					 * colors */
	{

/* free all allocated pointers at all exits */
#define FREE_CIDX {int f; for (f = 0; f < 256; f++) \
if (cidx[f]) XpmFree(cidx[f]);}

	    /* array of pointers malloced by need */
	    unsigned short *cidx[256];
	    int char1;
		#if defined(macintosh) || defined(__APPLE__)
		GrafPtr 	origPort ;
		GDHandle	origDevice ;
		GetGWorld( &origPort , &origDevice ) ;
		SetGWorld( image->gworldptr , NULL ) ;
		#endif
	    bzero((char *)cidx, 256 * sizeof(unsigned short *)); /* init */
	    for (a = 0; a < ncolors; a++) {
		char1 = colorTable[a].string[0];
		if (cidx[char1] == NULL) { /* get new memory */
		    cidx[char1] = (unsigned short *)
			XpmCalloc(256, sizeof(unsigned short));
		    if (cidx[char1] == NULL) { /* new block failed */
			#if defined(macintosh) || defined(__APPLE__)
    		SetGWorld( origPort , origDevice ) ;
			#endif
			FREE_CIDX;
			return (XpmNoMemory);
		    }
		}
		cidx[char1][(unsigned char)colorTable[a].string[1]] = a + 1;
	    }

	    for (y = 0; y < height; y++) {
		xpmNextString(data);
		for (x = 0; x < width; x++) 
		{
		    int cc1 = xpmGetC(data);
		    if (cc1 > 0 && cc1 < 256) {
			int cc2 = xpmGetC(data);
			if (cc2 > 0 && cc2 < 256 &&
			    cidx[cc1] && cidx[cc1][cc2] != 0) {

			#ifdef FOR_MSW
			SelectObject(*dc, image->bitmap);
			SetPixel(*dc, x, y, image_pixels[cidx[cc1][cc2] - 1]);
			if (shapeimage) 
			{
			    SelectObject(*dc, shapeimage->bitmap);
			    SetPixel(*dc, x, y,
				     shape_pixels[cidx[cc1][cc2] - 1]);
			}
			#elif defined(macintosh) || defined(__APPLE__)
	    		SetCPixel( x, y, &image_pixels[cidx[cc1][cc2] - 1]);  
			#endif
			} else 
			{
				#if defined(macintosh) || defined(__APPLE__)
    			SetGWorld( origPort , origDevice ) ;
				#endif
			    FREE_CIDX;
			    return (XpmFileInvalid);
			}
		    } else {
				#if defined(macintosh) || defined(__APPLE__)
    			SetGWorld( origPort , origDevice ) ;
				#endif
			FREE_CIDX;
			return (XpmFileInvalid);
		    }
		}
	    }
		#if defined(macintosh) || defined(__APPLE__)
		SetGWorld( origPort , origDevice ) ;
		#endif
	    FREE_CIDX;
	}
	break;

    default:				/* Non-optimized case of long color
					 * names */
	{
	    char *s;
	    char buf[BUFSIZ];
		#if defined(macintosh) || defined(__APPLE__)
		GrafPtr 	origPort ;
		GDHandle	origDevice ;
		GetGWorld( &origPort , &origDevice ) ;
		SetGWorld( image->gworldptr , NULL ) ;
		#endif

	    buf[cpp] = '\0';
	    if (USE_HASHTABLE) {
		xpmHashAtom *slot;

		for (y = 0; y < height; y++) {
		    xpmNextString(data);
		    for (x = 0; x < width; x++) {
			for (a = 0, s = buf; a < cpp; a++, s++)
			    *s = xpmGetC(data);
			slot = xpmHashSlot(hashtable, buf);
			if (!*slot)	/* no color matches */
			{
				#if defined(macintosh) || defined(__APPLE__)
    			SetGWorld( origPort , origDevice ) ;
				#endif
			    return (XpmFileInvalid);
			}

			#if FOR_MSW
			SelectObject(*dc, image->bitmap);
			SetPixel(*dc, x, y,
				 image_pixels[HashColorIndex(slot)]);
			if (shapeimage) 
			{
			    SelectObject(*dc, shapeimage->bitmap);
			    SetPixel(*dc, x, y,
				     shape_pixels[HashColorIndex(slot)]);
			}
			#elif defined(macintosh) || defined(__APPLE__)
	    		SetCPixel( x, y, &image_pixels[HashColorIndex(slot)]); 
			#endif
		    }
		}
	    } else {
		for (y = 0; y < height; y++) {
		    xpmNextString(data);
		    for (x = 0; x < width; x++) {
			for (a = 0, s = buf; a < cpp; a++, s++)
			    *s = xpmGetC(data);
			for (a = 0; a < ncolors; a++)
			    if (!strcmp(colorTable[a].string, buf))
				break;
			if (a == ncolors)	/* no color matches */
			{
				#if defined(macintosh) || defined(__APPLE__)
    			SetGWorld( origPort , origDevice ) ;
				#endif
			    return (XpmFileInvalid);
			}
			
			#if FOR_MSW
			SelectObject(*dc, image->bitmap);
			SetPixel(*dc, x, y, image_pixels[a]);
			if (shapeimage) {
			    SelectObject(*dc, shapeimage->bitmap);
			    SetPixel(*dc, x, y, shape_pixels[a]);
			}
			#elif defined(macintosh) || defined(__APPLE__)
	    	SetCPixel( x, y, &image_pixels[a]); // data is [x+y*width] 
			#endif
		    }
		}
	    }
		#if defined(macintosh) || defined(__APPLE__)
    	SetGWorld( origPort , origDevice ) ;
		#endif

	}
	break;
    }
    return (XpmSuccess);
}
