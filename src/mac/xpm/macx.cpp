/*
 * Copyright (C) 1998 Stefan Csomor
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
* amigax.c:                                                                   *
*                                                                             *
*  XPM library                                                                *
*  Emulates some Xlib functionality for Amiga.                                *
*                                                                             *
*  Developed by Lorens Younes (d93-hyo@nada.kth.se) 7/95                      *
*  Revised 4/96                                                               *
\*****************************************************************************/

/*
extern "C" 
{
	#include "XpmI.h"
	#include "macx.h"
} ;

XImage * AllocXImage (
    unsigned int   width,
    unsigned int   height,
    unsigned int   depth)
{
    XImage  *img;
    
    img = (XImage*) XpmMalloc (sizeof (*img));
    if (img != NULL)
    {
		Rect rect = { 0 , 0 , height , width } ;

		img->width = width;
		img->height = height;
		
	
		NewGWorld( &img->gworldptr , depth , &rect , NULL , NULL , 0 ) ;
		if (img->gworldptr == NULL)
		{
	    	XDestroyImage (img);
	    	return NULL;
		}
    }
    
    return img;
}


int XDestroyImage ( XImage  *ximage)
{
    if (ximage != NULL)
    {
		if ( ximage->gworldptr )
			DisposeGWorld( ximage->gworldptr ) ;

		XpmFree (ximage);
    }
    
    return Success;
}

Status XAllocColor( Display dpy, Colormap colormap, XColor  *c ) 
{
    unsigned long    pixel;
    unsigned short   red, green, blue;
	c->pixel = ( ( (c->red)>>8 ) << 16 ) +  ( ( (c->green)>>8 ) << 8 ) +  ( ( (c->blue)>>8 ) ) ;
    return Success;
}

int XFreeColors(Display dpy, Colormap colormap, unsigned long  *pixels, 
	int npixels, int planes) 
{
    return Success;
}

int XParseColor(Display dpy, Colormap colormap, char *spec, XColor *exact_def_return) 
{
    int spec_length;
    
    if (spec == 0)
	return False;
    
    spec_length = strlen(spec);
    if (spec[0] == '#')
    {
		int hexlen;
		char hexstr[10];
	
		hexlen = (spec_length - 1) / 3;
		if (hexlen < 1 || hexlen > 4 || hexlen * 3 != spec_length - 1)
		    return False;
		
		hexstr[hexlen] = '\0';
		strncpy (hexstr, spec + 1, hexlen);
		exact_def_return->red = strtoul (hexstr, NULL, 16) << (16 - 4*hexlen);
		strncpy (hexstr, spec + 1 + hexlen, hexlen);
		exact_def_return->green = strtoul (hexstr, NULL, 16) << (16 - 4*hexlen);
		strncpy (hexstr, spec + 1 + 2 * hexlen, hexlen);
		exact_def_return->blue = strtoul (hexstr, NULL, 16) << (16 - 4*hexlen);
		
		return True;
	}
	else
	{
	}
	return FALSE ;
}

int XQueryColor(Display dpy, Colormap colormap, XColor *c) 
{
	c->pixel = ( ( (c->red)>>8 ) << 16 ) +  ( ( (c->green)>>8 ) << 8 ) +  ( ( (c->blue)>>8 ) ) ;

    if (GfxBase->LibNode.lib_Version >= 39)
    {
	unsigned long   rgb[3];
	
	GetRGB32 (colormap, def_in_out->pixel, 1, rgb);
	def_in_out->red = rgb[0] >> 16;
	def_in_out->green = rgb[1] >> 16;
	def_in_out->blue = rgb[2] >> 16;
    }
    else
    {
	unsigned short   rgb;
	
	rgb = GetRGB4 (colormap, def_in_out->pixel);
	def_in_out->red = ((rgb >> 8) & 0xF) * 0x1111;
	def_in_out->green = ((rgb >> 4) & 0xF) * 0x1111;
	def_in_out->blue = (rgb & 0xF) * 0x1111;
    }

    return Success;
}

int XQueryColors(Display dpy, Colormap colormap, XColor *defs_in_out, int ncolors) 
{
    int   i;
    
    for (i = 0; i < ncolors; i++)
		XQueryColor (dpy, colormap , &defs_in_out[i]);
    
    return Success;
}

int XPutPixel ( XImage *ximage,int  x,int y,unsigned long   pixel)
{
	
	CGrafPtr	origPort ;
	GDHandle	origDev ;
	
	GetGWorld( &origPort , &origDev ) ;
	SetGWorld( ximage->gworldptr , NULL ) ;
	RGBColor color ;
	color.red = (pixel & 0x00FF0000) >> 16 ;
	color.green = (pixel & 0x0000FF00) >> 8 ;
	color.blue =  (pixel & 0x000000FF) ;
	color.red = ( color.red<<8) + color.red ;
	color.green = ( color.green<<8) + color.green ;
	color.blue = ( color.blue<<8) + color.blue ;
	SetCPixel( x , y , &color ) ;
	SetGWorld( origPort , origDev ) ;
    return Success;
}
*/
/*

static struct RastPort *
AllocRastPort (unsigned int, unsigned int, unsigned int);
static void
FreeRastPort (struct RastPort *, unsigned int,unsigned int);


static struct RastPort *
AllocRastPort (
    unsigned int   width,
    unsigned int   height,
    unsigned int   depth)
{
    struct RastPort  *rp;
    
    rp = XpmMalloc (sizeof (*rp));
    if (rp != NULL)
    {
	InitRastPort (rp);
	if (GfxBase->LibNode.lib_Version >= 39)
	{
	    rp->BitMap = AllocBitMap (width, height, depth, BMF_CLEAR, NULL);
	    if (rp->BitMap == NULL)
	    {
		FreeRastPort (rp, width, height);
		return NULL;
	    }
	}
	else
	{
	    unsigned int   i;
	    
	    rp->BitMap = XpmMalloc (sizeof (*rp->BitMap));
	    if (rp->BitMap == NULL)
	    {
		FreeRastPort (rp, width, height);
		return NULL;
	    }
	    
	    InitBitMap (rp->BitMap, depth, width, height);
	    for (i = 0; i < depth; ++i)
		rp->BitMap->Planes[i] = NULL;
	    for (i = 0; i < depth; ++i)
	    {
		rp->BitMap->Planes[i] = (PLANEPTR)AllocRaster (width, height);
		if (rp->BitMap->Planes[i] == NULL)
		{
		    FreeRastPort (rp, width, height);
		    return NULL;
		}
	    }
	}
    }
    
    return rp;
}


static void
FreeRastPort (
    struct RastPort  *rp,
    unsigned int      width,
    unsigned int      height)
{
    if (rp != NULL)
    {
	if (rp->BitMap != NULL)
	{
	    WaitBlit ();
	    if (GfxBase->LibNode.lib_Version >= 39)
		FreeBitMap (rp->BitMap);
	    else
	    {
		unsigned int   i;
		
		for (i = 0; i < rp->BitMap->Depth; ++i)
		{
		    if (rp->BitMap->Planes[i] != NULL)
			FreeRaster (rp->BitMap->Planes[i], width, height);
		}
		XpmFree (rp->BitMap);
	    }
	}
	XpmFree (rp);
    }
}

int
XPutPixel (
    XImage         *ximage,
    int             x,
    int             y,
    unsigned long   pixel)
{
    SetAPen (ximage->rp, pixel);
    WritePixel (ximage->rp, x, y);
    
    return Success;
}


Status
AllocBestPen (
    Colormap        colormap,
    XColor         *screen_in_out,
    unsigned long   precision,
    Bool            fail_if_bad)
{
    if (GfxBase->LibNode.lib_Version >= 39)
    {
	unsigned long   r, g, b;
	
	r = screen_in_out->red * 0x00010001;
	g = screen_in_out->green * 0x00010001;
	b = screen_in_out->blue * 0x00010001;
	screen_in_out->pixel = ObtainBestPen (colormap, r, g, b,
					      OBP_Precision, precision,
					      OBP_FailIfBad, fail_if_bad,
					      TAG_DONE);
	if (screen_in_out->pixel == -1)
	    return False;
	
	QueryColor (colormap, screen_in_out);
    }
    else
    {
	XColor   nearest, trial;
	long     nearest_delta, trial_delta;
	int      num_cells, i;
	
	num_cells = colormap->Count;
	nearest.pixel = 0;
	QueryColor (colormap, &nearest);
	nearest_delta = ((((screen_in_out->red >> 8) - (nearest.red >> 8))
			  * ((screen_in_out->red >> 8) - (nearest.red >> 8)))
			 +
			 (((screen_in_out->green >> 8) - (nearest.green >> 8))
			  * ((screen_in_out->green >> 8) - (nearest.green >> 8)))
			 +
			 (((screen_in_out->blue >> 8) - (nearest.blue >> 8))
			  * ((screen_in_out->blue >> 8) - (nearest.blue >> 8))));
	for (i = 1; i < num_cells; i++)
	{
	// precision and fail_if_bad is ignored under pre V39 
	    trial.pixel = i;
	    QueryColor (colormap, &trial);
	    trial_delta = ((((screen_in_out->red >> 8) - (trial.red >> 8))
			    * ((screen_in_out->red >> 8) - (trial.red >> 8)))
			   +
			   (((screen_in_out->green >> 8) - (trial.green >> 8))
			    * ((screen_in_out->green >> 8) - (trial.green >> 8)))
			   +
			   (((screen_in_out->blue >> 8) - (trial.blue >> 8))
			    * ((screen_in_out->blue >> 8) - (trial.blue >> 8))));
	    if (trial_delta < nearest_delta)
	    {
		nearest = trial;
		nearest_delta = trial_delta;
	    }
	}
	screen_in_out->pixel = nearest.pixel;
	screen_in_out->red = nearest.red;
	screen_in_out->green = nearest.green;
	screen_in_out->blue = nearest.blue;
    }
    
    return True;
}


int
FreePens (
    Colormap        colormap,
    unsigned long  *pixels,
    int             npixels)
{
    if (GfxBase->LibNode.lib_Version >= 39)
    {
	int   i;
	
	for (i = 0; i < npixels; i++)
	    ReleasePen (colormap, pixels[i]);
    }
    
    return Success;
}


Status
ParseColor (
    char    *spec,
    XColor  *exact_def_return)
{
    int spec_length;
    
    if (spec == 0)
	return False;
    
    spec_length = strlen(spec);
    if (spec[0] == '#')
    {
	int hexlen;
	char hexstr[10];
	
	hexlen = (spec_length - 1) / 3;
	if (hexlen < 1 || hexlen > 4 || hexlen * 3 != spec_length - 1)
	    return False;
	
	hexstr[hexlen] = '\0';
	strncpy (hexstr, spec + 1, hexlen);
	exact_def_return->red = strtoul (hexstr, NULL, 16) << (16 - 4*hexlen);
	strncpy (hexstr, spec + 1 + hexlen, hexlen);
	exact_def_return->green = strtoul (hexstr, NULL, 16) << (16 - 4*hexlen);
	strncpy (hexstr, spec + 1 + 2 * hexlen, hexlen);
	exact_def_return->blue = strtoul (hexstr, NULL, 16) << (16 - 4*hexlen);
	
	return True;
    }
    else
    {
	FILE  *rgbf;
	int    items, red, green, blue;
	char   line[512], name[512];
	Bool   success = False;
	
	rgbf = fopen ("LIBS:rgb.txt", "r");
	if (rgbf == NULL)
	    return False;
	
	while (fgets(line, sizeof (line), rgbf) && !success)
	{
	    items = sscanf (line, "%d %d %d %[^\n]\n",
			    &red, &green, &blue, name);
	    if (items != 4)
		continue;
	    
	    if (red < 0 || red > 0xFF
		|| green < 0 || green > 0xFF
		|| blue < 0 || blue > 0xFF)
	    {
		continue;
	    }
	    
	    if (0 == xpmstrcasecmp (spec, name))
	    {
		exact_def_return->red = red * 0x0101;
		exact_def_return->green = green * 0x0101;
		exact_def_return->blue = blue * 0x0101;
		success = True;
	    }
	}
	fclose (rgbf);
	
	return success;
    }
}


int
QueryColor (
    Colormap   colormap,
    XColor    *def_in_out)
{
    if (GfxBase->LibNode.lib_Version >= 39)
    {
	unsigned long   rgb[3];
	
	GetRGB32 (colormap, def_in_out->pixel, 1, rgb);
	def_in_out->red = rgb[0] >> 16;
	def_in_out->green = rgb[1] >> 16;
	def_in_out->blue = rgb[2] >> 16;
    }
    else
    {
	unsigned short   rgb;
	
	rgb = GetRGB4 (colormap, def_in_out->pixel);
	def_in_out->red = ((rgb >> 8) & 0xF) * 0x1111;
	def_in_out->green = ((rgb >> 4) & 0xF) * 0x1111;
	def_in_out->blue = (rgb & 0xF) * 0x1111;
    }
    
    return Success;
}


int
QueryColors (
    Colormap   colormap,
    XColor    *defs_in_out,
    int        ncolors)
{
    int   i;
    
    for (i = 0; i < ncolors; i++)
	QueryColor (colormap, &defs_in_out[i]);
    
    return Success;
}

*/
