/////////////////////////////////////////////////////////////////////////////
// Name:        ogl.i
// Purpose:     SWIG definitions for the wxWindows Object Graphics Library
//
// Author:      Robin Dunn
//
// Created:     30-June-1999
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module ogl

%{
#include "helpers.h"
#include "oglhelpers.h"
%}

//---------------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

%extern wx.i
%import windows.i
%extern _defs.i
%extern misc.i
%extern gdi.i

%include _ogldefs.i

%import oglbasic.i
%import oglshapes.i
%import oglshapes2.i
%import oglcanvas.i


%pragma(python) code = "import wx"

//---------------------------------------------------------------------------


enum {
    KEY_SHIFT,
    KEY_CTRL,
    ARROW_NONE,
    ARROW_END,
    ARROW_BOTH,
    ARROW_MIDDLE,
    ARROW_START,
    ARROW_HOLLOW_CIRCLE,
    ARROW_FILLED_CIRCLE,
    ARROW_ARROW,
    ARROW_SINGLE_OBLIQUE,
    ARROW_DOUBLE_OBLIQUE,
    ARROW_METAFILE,
    ARROW_POSITION_END,
    ARROW_POSITION_START,
    CONTROL_POINT_VERTICAL,
    CONTROL_POINT_HORIZONTAL,
    CONTROL_POINT_DIAGONAL,
    CONTROL_POINT_ENDPOINT_TO,
    CONTROL_POINT_ENDPOINT_FROM,
    CONTROL_POINT_LINE,
    FORMAT_NONE,
    FORMAT_CENTRE_HORIZ,
    FORMAT_CENTRE_VERT,
    FORMAT_SIZE_TO_CONTENTS,
    LINE_ALIGNMENT_HORIZ,
    LINE_ALIGNMENT_VERT,
    LINE_ALIGNMENT_TO_NEXT_HANDLE,
    LINE_ALIGNMENT_NONE,
    SHADOW_NONE,
    SHADOW_LEFT,
    SHADOW_RIGHT,
//      SHAPE_BASIC,
//      SHAPE_RECTANGLE,
//      SHAPE_ELLIPSE,
//      SHAPE_POLYGON,
//      SHAPE_CIRCLE,
//      SHAPE_LINE,
//      SHAPE_DIVIDED_RECTANGLE,
//      SHAPE_COMPOSITE,
//      SHAPE_CONTROL_POINT,
//      SHAPE_DRAWN,
//      SHAPE_DIVISION,
//      SHAPE_LABEL_OBJECT,
//      SHAPE_BITMAP,
//      SHAPE_DIVIDED_OBJECT_CONTROL_POINT,
//      OBJECT_REGION,
    OP_CLICK_LEFT,
    OP_CLICK_RIGHT,
    OP_DRAG_LEFT,
    OP_DRAG_RIGHT,
    OP_ALL,
    ATTACHMENT_MODE_NONE,
    ATTACHMENT_MODE_EDGE,
    ATTACHMENT_MODE_BRANCHING,
    BRANCHING_ATTACHMENT_NORMAL,
    BRANCHING_ATTACHMENT_BLOB,

    gyCONSTRAINT_CENTRED_VERTICALLY,
    gyCONSTRAINT_CENTRED_HORIZONTALLY,
    gyCONSTRAINT_CENTRED_BOTH,
    gyCONSTRAINT_LEFT_OF,
    gyCONSTRAINT_RIGHT_OF,
    gyCONSTRAINT_ABOVE,
    gyCONSTRAINT_BELOW,
    gyCONSTRAINT_ALIGNED_TOP,
    gyCONSTRAINT_ALIGNED_BOTTOM,
    gyCONSTRAINT_ALIGNED_LEFT,
    gyCONSTRAINT_ALIGNED_RIGHT,
    gyCONSTRAINT_MIDALIGNED_TOP,
    gyCONSTRAINT_MIDALIGNED_BOTTOM,
    gyCONSTRAINT_MIDALIGNED_LEFT,
    gyCONSTRAINT_MIDALIGNED_RIGHT,

    DIVISION_SIDE_NONE,
    DIVISION_SIDE_LEFT,
    DIVISION_SIDE_TOP,
    DIVISION_SIDE_RIGHT,
    DIVISION_SIDE_BOTTOM,

};



//---------------------------------------------------------------------------

void wxOGLInitialize();
void wxOGLCleanUp();


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
%{
extern "C" SWIGEXPORT(void) initoglbasicc();
extern "C" SWIGEXPORT(void) initoglshapesc();
extern "C" SWIGEXPORT(void) initoglshapes2c();
extern "C" SWIGEXPORT(void) initoglcanvasc();
%}


%init %{

    initoglbasicc();
    initoglshapesc();
    initoglshapes2c();
    initoglcanvasc();


    wxClassInfo::CleanUpClasses();
    wxClassInfo::InitializeClasses();

%}

//----------------------------------------------------------------------
// And this gets appended to the shadow class file.
//----------------------------------------------------------------------

%pragma(python) include="_oglextras.py";

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
