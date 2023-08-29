///////////////////////////////////////////////////////////////////////////////
// Name:        tests/drawing/basictest.cpp
// Purpose:     Basic tests for wxGraphicsContext
// Author:      Armel Asselin
// Created:     2014-02-28
// Copyright:   (c) 2014 Ellié Computing <opensource@elliecomputing.com>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/font.h"
//    #include "wx/window.h"
#endif // WX_PRECOMP

#define _WX_CURSOR_H_BASE_


#include "drawing.h"

#if wxUSE_TEST_GC_DRAWING

const GraphicsContextDrawingTestCase::DrawingTestCase
GraphicsContextDrawingTestCase::ms_drawingBasicTc = {
    1, &GraphicsContextDrawingTestCase::DoBasicDrawings, 800, 600, 72., true
};

void GraphicsContextDrawingTestCase::DoBasicDrawings (wxGraphicsContext *gc)
{
    // this test is expected to be portable, on any platform, in order to keep
    //  that property, it should contain only axis aligned/integer drawings so
    //  that the anti-aliasing method does not cause troubles.

    wxGraphicsBrush gbBackground =
        gc->CreateBrush (wxBrush (wxColour (255, 255, 255)));

    gc->SetBrush (gbBackground);
    gc->DrawRectangle (0, 0, 800, 600);
}

#endif // wxUSE_TEST_GC_DRAWING
