///////////////////////////////////////////////////////////////////////////////
// Name:        tests/drawing/drawing.h
// Purpose:     Drawing test case header
// Author:      Armel Asselin
// Created:     2014-02-26
// Copyright:   (c) 2014 Ellié Computing <opensource@elliecomputing.com>
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TESTS_DRAWING_H_
#define _WX_TESTS_DRAWING_H_

#include "wx/graphics.h"

class GraphicsContextDrawingTestCase
{
public:
    // Standard drawing routines that can be used by any backend test
    static void DoBasicDrawings(wxGraphicsContext *gc);
    static void DoFontDrawings(wxGraphicsContext *gc);
    static void DoFullDrawings(wxGraphicsContext *gc);
};

#endif // _WX_TESTS_DRAWING_H_
