/////////////////////////////////////////////////////////////////////////////
// Name:        ogl.h
// Purpose:     OGL main include
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _OGL_OGL_H_
#define _OGL_OGL_H_

#include "basic.h"      // Basic shapes
#include "lines.h"      // Lines and splines
#include "divided.h"    // Vertically-divided rectangle
#include "composit.h"   // Composite images
#include "canvas.h"     // wxShapeCanvas for displaying objects
#include "ogldiag.h"    // wxDiagram

extern void wxOGLInitialize();
extern void wxOGLCleanUp();

#endif
 // _OGL_OGL_H_
