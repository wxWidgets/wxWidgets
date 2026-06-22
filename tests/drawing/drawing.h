///////////////////////////////////////////////////////////////////////////////
// Name:        tests/drawing/drawing.h
// Purpose:     Drawing test case header
// Author:      Armel Asselin
// Created:     2014-02-26
// Copyright:   (c) 2014 Ellié Computing <opensource@elliecomputing.com>
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TESTS_DRAWING_H_
#define _WX_TESTS_DRAWING_H_

#if wxUSE_GRAPHICS_CONTEXT && !defined(__WXX11__)

#include "wx/graphics.h"

void DoBasicDrawings(wxGraphicsContext *gc);
void DoFontDrawings(wxGraphicsContext *gc);
void DoFullDrawings(wxGraphicsContext *gc);

#endif // wxUSE_GRAPHICS_CONTEXT && !defined(__WXX11__)

#endif // _WX_TESTS_DRAWING_H_
