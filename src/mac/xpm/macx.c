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

#include "XpmI.h"
//#include "macx.h"
#include "simx.h"

XImage *
AllocXImage (
    unsigned int   width,
    unsigned int   height,
    unsigned int   depth)
{
    XImage  *img;
    
    img = XpmMalloc (sizeof (*img));
    if (img != NULL)
    {
			Rect rect ;
			rect.left = rect.top = 0 ;
			rect.bottom = height ;
			rect.right = width ;

			img->width = width;
			img->height = height;
			
		
			NewGWorld( &img->gworldptr , depth , &rect , NULL , NULL , 0 ) ;
			LockPixels( GetGWorldPixMap( img->gworldptr ) ) ;
			if (img->gworldptr == NULL)
			{
		    	XDestroyImage (img);
		    	return NULL;
			}
    }
    
    return img;
}
