///////////////////////////////////////////////////////////////////////////////
// Name:        tests/drawing/plugin.h
// Purpose:     Defines the functions needed to implement a drawing test plugin
// Author:      Armel Asselin
// Created:     2014-02-2
// Copyright:   (c) 2014 Ell8ié Computing <opensource@elliecomputing.com>
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TESTS_DRAWINGPLG_H_
#define _WX_TESTS_DRAWINGPLG_H_

#include "gcfactory.h"

#if wxUSE_TEST_GC_DRAWING

extern "C" WXEXPORT DrawingTestGCFactory * CreateDrawingTestLifeCycle();
extern "C" WXEXPORT void DestroyDrawingTestLifeCycle (DrawingTestGCFactory* lc);

#endif // wxUSE_TEST_GC_DRAWING

#endif // #ifndef _WX_TESTS_DRAWINGPLG_H_
