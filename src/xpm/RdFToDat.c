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
*  RdFToDat.c:                                                                *
*                                                                             *
*  XPM library                                                                *
*  Parse an XPM file and create an array of strings corresponding to it.      *
*                                                                             *
*  Developed by Dan Greening dgreen@cs.ucla.edu / dgreen@sti.com              *
\*****************************************************************************/

#include "XpmI.h"

#ifdef __OS2__
/* Visual Age cannot deal with old, non-ansi, code */
int XpmReadFileToData(char* filename, char*** data_return)
#else
int
XpmReadFileToData(filename, data_return)
    char *filename;
    char ***data_return;
#endif
{
    XpmImage image;
    XpmInfo info;
    int ErrorStatus;

    info.valuemask = XpmReturnComments | XpmReturnExtensions;

    /*
     * initialize return value
     */
    if (data_return)
	*data_return = NULL;

    ErrorStatus = XpmReadFileToXpmImage(filename, &image, &info);
    if (ErrorStatus != XpmSuccess)
	return (ErrorStatus);

    ErrorStatus =
	XpmCreateDataFromXpmImage(data_return, &image, &info);

    XpmFreeXpmImage(&image);
    XpmFreeXpmInfo(&info);

    return (ErrorStatus);
}
