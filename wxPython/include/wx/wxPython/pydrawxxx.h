/////////////////////////////////////////////////////////////////////////////
// Name:        pydrawxxx.h
// Purpose:     functions used by the DrawXXXList enhancements added to wxDC
//
// Author:      Robin Dunn
//
// Created:     14-Oct-2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __pydrawxxx_h__
#define __pydrawxxx_h__

void wxPyDrawList_SetAPIPtr();

typedef bool (*wxPyDrawListOp_t)(wxDC& dc, PyObject* coords);
PyObject* wxPyDrawXXXList(wxDC& dc, wxPyDrawListOp_t doDraw,
                          PyObject* pyCoords, PyObject* pyPens, PyObject* pyBrushes);
bool wxPyDrawXXXPoint(wxDC& dc, PyObject* coords);
bool wxPyDrawXXXLine(wxDC& dc, PyObject* coords);
bool wxPyDrawXXXRectangle(wxDC& dc, PyObject* coords);
bool wxPyDrawXXXEllipse(wxDC& dc, PyObject* coords);
bool wxPyDrawXXXPolygon(wxDC& dc, PyObject* coords);

PyObject* wxPyDrawTextList(wxDC& dc, PyObject* textList, PyObject* pyPoints,
                           PyObject* foregroundList, PyObject* backgroundList);


#endif
