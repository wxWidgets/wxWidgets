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

#include <wx/ogl/basic.h>      // Basic shapes
#include <wx/ogl/lines.h>      // Lines and splines
#include <wx/ogl/divided.h>    // Vertically-divided rectangle
#include <wx/ogl/composit.h>   // Composite images
#include <wx/ogl/canvas.h>     // wxShapeCanvas for displaying objects
#include <wx/ogl/ogldiag.h>    // wxDiagram

// TODO: replace with wxModule implementation
extern void wxOGLInitialize();
extern void wxOGLCleanUp();

#endif
 // _OGL_OGL_H_
