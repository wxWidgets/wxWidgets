/////////////////////////////////////////////////////////////////////////////
// Name:        curico.h
// Purpose:     Icon and cursor functions
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CURICO_H_
#define _WX_CURICO_H_

HICON    ReadIconFile(    wxChar *szFileName, HINSTANCE hInst,
                          int *W = 0, int *H = 0);
HCURSOR  ReadCursorFile(  wxChar *szFileName, HINSTANCE hInst,
                          int *W = 0, int *H = 0, int *XHot = 0, int *YHot = 0);
HCURSOR  IconToCursor(    wxChar *szFileName, HINSTANCE hInst, int XHot, int YHot,
                          int *W = 0, int *H = 0);
HICON    CursorToIcon(    wxChar *szFileName, HINSTANCE hInst,
                          int *W = 0, int *H = 0);

HCURSOR MakeCursorFromBitmap(HINSTANCE hInst, HBITMAP hBitmap, POINT *pPoint);
HICON MakeIconFromBitmap(HINSTANCE hInst, HBITMAP hBitmap);

#endif

