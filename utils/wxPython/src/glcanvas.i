/////////////////////////////////////////////////////////////////////////////
// Name:        glcanvas.i
// Purpose:     SWIG definitions for the OpenGL wxWindows classes
//
// Author:      Robin Dunn
//
// Created:     15-Mar-1999
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module glcanvas

%{
#include "helpers.h"
#include <glcanvas.h>
%}

//---------------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

%extern wx.i
%extern windows.i
%extern windows2.i
%extern windows3.i
%extern frames.i
%extern _defs.i
%extern misc.i
%extern gdi.i
%extern controls.i
%extern events.i


%{
#ifdef SEPARATE
    static wxString wxPyEmptyStr("");
    static wxPoint  wxPyDefaultPosition(-1, -1);
    static wxSize   wxPyDefaultSize(-1, -1);
#endif
%}

%pragma(python) code = "import wx"

//---------------------------------------------------------------------------

class wxPalette;
class wxWindow;
class wxSize;
class wxPoint;

//---------------------------------------------------------------------------

class wxGLContext {
public:
    wxGLContext(bool isRGB, wxWindow *win, const wxPalette& palette = wxNullPalette);
    ~wxGLContext();

    void SetCurrent();
    void SetColour(const char *colour);
    void SwapBuffers();

    void SetupPixelFormat();
    void SetupPalette(const wxPalette& palette);
    wxPalette CreateDefaultPalette();

    wxPalette* GetPalette();
    wxWindow* GetWindow();
};

//---------------------------------------------------------------------------

class wxGLCanvas : public wxScrolledWindow {
public:
    wxGLCanvas(wxWindow *parent, wxWindowID id = -1,
               const wxPoint& pos = wxPyDefaultPosition,
               const wxSize& size = wxPyDefaultSize, long style = 0,
               const char* name = "GLCanvas",
               int *attribList = 0,
               const wxPalette& palette = wxNullPalette);

    %pragma(python) addtomethod = "__init__:wx._StdWindowCallbacks(self)"

    void SetCurrent();
    void SetColour(const char *colour);
    void SwapBuffers();

    wxGLContext* GetContext();
};


//---------------------------------------------------------------------------

typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void GLvoid;


//---------------------------------------------------------------------------
/* EXT_vertex_array */
void glArrayElementEXT(GLint i);
void glColorPointerEXT(GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
void glDrawArraysEXT(GLenum mode, GLint first, GLsizei count);
void glEdgeFlagPointerEXT(GLsizei stride, GLsizei count, const GLboolean *pointer);
void glGetPointervEXT(GLenum pname, GLvoid* *params);
void glIndexPointerEXT(GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
void glNormalPointerEXT(GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
void glTexCoordPointerEXT(GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
void glVertexPointerEXT(GLint size, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);

/* EXT_color_subtable */
void glColorSubtableEXT(GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *table);

/* EXT_color_table */
void glColorTableEXT(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table);
void glCopyColorTableEXT(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width);
void glGetColorTableEXT(GLenum target, GLenum format, GLenum type, GLvoid *table);
void glGetColorTableParamaterfvEXT(GLenum target, GLenum pname, GLfloat *params);
void glGetColorTavleParameterivEXT(GLenum target, GLenum pname, GLint *params);

/* SGI_compiled_vertex_array */
void glLockArraysSGI(GLint first, GLsizei count);
void glUnlockArraysSGI();

/* SGI_cull_vertex */
void glCullParameterdvSGI(GLenum pname, GLdouble* params);
void glCullParameterfvSGI(GLenum pname, GLfloat* params);

/* SGI_index_func */
void glIndexFuncSGI(GLenum func, GLclampf ref);

/* SGI_index_material */
void glIndexMaterialSGI(GLenum face, GLenum mode);

/* WIN_swap_hint */
void glAddSwapHintRectWin(GLint x, GLint y, GLsizei width, GLsizei height);


//----------------------------------------------------------------------
// From GL.H

enum {
    GL_2D,
    GL_2_BYTES,
    GL_3D,
    GL_3D_COLOR,
    GL_3D_COLOR_TEXTURE,
    GL_3_BYTES,
    GL_4D_COLOR_TEXTURE,
    GL_4_BYTES,
    GL_ACCUM,
    GL_ACCUM_ALPHA_BITS,
    GL_ACCUM_BLUE_BITS,
    GL_ACCUM_BUFFER_BIT,
    GL_ACCUM_CLEAR_VALUE,
    GL_ACCUM_GREEN_BITS,
    GL_ACCUM_RED_BITS,
    GL_ADD,
    GL_ALL_ATTRIB_BITS,
    GL_ALPHA,
    GL_ALPHA12,
    GL_ALPHA16,
    GL_ALPHA4,
    GL_ALPHA8,
    GL_ALPHA_BIAS,
    GL_ALPHA_BITS,
    GL_ALPHA_SCALE,
    GL_ALPHA_TEST,
    GL_ALPHA_TEST_FUNC,
    GL_ALPHA_TEST_REF,
    GL_ALWAYS,
    GL_AMBIENT,
    GL_AMBIENT_AND_DIFFUSE,
    GL_AND,
    GL_AND_INVERTED,
    GL_AND_REVERSE,
    GL_ATTRIB_STACK_DEPTH,
    GL_AUTO_NORMAL,
    GL_AUX0,
    GL_AUX1,
    GL_AUX2,
    GL_AUX3,
    GL_AUX_BUFFERS,
    GL_BACK,
    GL_BACK_LEFT,
    GL_BACK_RIGHT,
    GL_BGRA_EXT,
    GL_BGR_EXT,
    GL_BITMAP,
    GL_BITMAP_TOKEN,
    GL_BLEND,
    GL_BLEND_DST,
    GL_BLEND_SRC,
    GL_BLUE,
    GL_BLUE_BIAS,
    GL_BLUE_BITS,
    GL_BLUE_SCALE,
    GL_BYTE,
    GL_C3F_V3F,
    GL_C4F_N3F_V3F,
    GL_C4UB_V2F,
    GL_C4UB_V3F,
    GL_CCW,
    GL_CLAMP,
    GL_CLEAR,
    GL_CLIENT_ALL_ATTRIB_BITS,
    GL_CLIENT_ATTRIB_STACK_DEPTH,
    GL_CLIENT_PIXEL_STORE_BIT,
    GL_CLIENT_VERTEX_ARRAY_BIT,
    GL_CLIP_PLANE0,
    GL_CLIP_PLANE1,
    GL_CLIP_PLANE2,
    GL_CLIP_PLANE3,
    GL_CLIP_PLANE4,
    GL_CLIP_PLANE5,
    GL_COEFF,
    GL_COLOR,
    GL_COLOR_ARRAY,
    GL_COLOR_ARRAY_COUNT_EXT,
    GL_COLOR_ARRAY_EXT,
    GL_COLOR_ARRAY_POINTER,
    GL_COLOR_ARRAY_POINTER_EXT,
    GL_COLOR_ARRAY_SIZE,
    GL_COLOR_ARRAY_SIZE_EXT,
    GL_COLOR_ARRAY_STRIDE,
    GL_COLOR_ARRAY_STRIDE_EXT,
    GL_COLOR_ARRAY_TYPE,
    GL_COLOR_ARRAY_TYPE_EXT,
    GL_COLOR_BUFFER_BIT,
    GL_COLOR_CLEAR_VALUE,
    GL_COLOR_INDEX,
    GL_COLOR_INDEX12_EXT,
    GL_COLOR_INDEX16_EXT,
    GL_COLOR_INDEX1_EXT,
    GL_COLOR_INDEX2_EXT,
    GL_COLOR_INDEX4_EXT,
    GL_COLOR_INDEX8_EXT,
    GL_COLOR_INDEXES,
    GL_COLOR_LOGIC_OP,
    GL_COLOR_MATERIAL,
    GL_COLOR_MATERIAL_FACE,
    GL_COLOR_MATERIAL_PARAMETER,
    GL_COLOR_TABLE_ALPHA_SIZE_EXT,
    GL_COLOR_TABLE_BLUE_SIZE_EXT,
    GL_COLOR_TABLE_FORMAT_EXT,
    GL_COLOR_TABLE_GREEN_SIZE_EXT,
    GL_COLOR_TABLE_INTENSITY_SIZE_EXT,
    GL_COLOR_TABLE_LUMINANCE_SIZE_EXT,
    GL_COLOR_TABLE_RED_SIZE_EXT,
    GL_COLOR_TABLE_WIDTH_EXT,
    GL_COLOR_WRITEMASK,
    GL_COMPILE,
    GL_COMPILE_AND_EXECUTE,
    GL_CONSTANT_ATTENUATION,
    GL_COPY,
    GL_COPY_INVERTED,
    GL_COPY_PIXEL_TOKEN,
    GL_CULL_FACE,
    GL_CULL_FACE_MODE,
    GL_CURRENT_BIT,
    GL_CURRENT_COLOR,
    GL_CURRENT_INDEX,
    GL_CURRENT_NORMAL,
    GL_CURRENT_RASTER_COLOR,
    GL_CURRENT_RASTER_DISTANCE,
    GL_CURRENT_RASTER_INDEX,
    GL_CURRENT_RASTER_POSITION,
    GL_CURRENT_RASTER_POSITION_VALID,
    GL_CURRENT_RASTER_TEXTURE_COORDS,
    GL_CURRENT_TEXTURE_COORDS,
    GL_CW,
    GL_DECAL,
    GL_DECR,
    GL_DEPTH,
    GL_DEPTH_BIAS,
    GL_DEPTH_BITS,
    GL_DEPTH_BUFFER_BIT,
    GL_DEPTH_CLEAR_VALUE,
    GL_DEPTH_COMPONENT,
    GL_DEPTH_FUNC,
    GL_DEPTH_RANGE,
    GL_DEPTH_SCALE,
    GL_DEPTH_TEST,
    GL_DEPTH_WRITEMASK,
    GL_DIFFUSE,
    GL_DITHER,
    GL_DOMAIN,
    GL_DONT_CARE,
    GL_DOUBLE,
    GL_DOUBLEBUFFER,
    GL_DOUBLE_EXT,
    GL_DRAW_BUFFER,
    GL_DRAW_PIXEL_TOKEN,
    GL_DST_ALPHA,
    GL_DST_COLOR,
    GL_EDGE_FLAG,
    GL_EDGE_FLAG_ARRAY,
    GL_EDGE_FLAG_ARRAY_COUNT_EXT,
    GL_EDGE_FLAG_ARRAY_EXT,
    GL_EDGE_FLAG_ARRAY_POINTER,
    GL_EDGE_FLAG_ARRAY_POINTER_EXT,
    GL_EDGE_FLAG_ARRAY_STRIDE,
    GL_EDGE_FLAG_ARRAY_STRIDE_EXT,
    GL_EMISSION,
    GL_ENABLE_BIT,
    GL_EQUAL,
    GL_EQUIV,
    GL_EVAL_BIT,
    GL_EXP,
    GL_EXP2,
    GL_EXTENSIONS,
    GL_EXT_bgra,
    GL_EXT_paletted_texture,
    GL_EXT_vertex_array,
    GL_EYE_LINEAR,
    GL_EYE_PLANE,
    GL_FALSE,
    GL_FASTEST,
    GL_FEEDBACK,
    GL_FEEDBACK_BUFFER_POINTER,
    GL_FEEDBACK_BUFFER_SIZE,
    GL_FEEDBACK_BUFFER_TYPE,
    GL_FILL,
    GL_FLAT,
    GL_FLOAT,
    GL_FOG,
    GL_FOG_BIT,
    GL_FOG_COLOR,
    GL_FOG_DENSITY,
    GL_FOG_END,
    GL_FOG_HINT,
    GL_FOG_INDEX,
    GL_FOG_MODE,
    GL_FOG_SPECULAR_TEXTURE_WIN,
    GL_FOG_START,
    GL_FRONT,
    GL_FRONT_AND_BACK,
    GL_FRONT_FACE,
    GL_FRONT_LEFT,
    GL_FRONT_RIGHT,
    GL_GEQUAL,
    GL_GREATER,
    GL_GREEN,
    GL_GREEN_BIAS,
    GL_GREEN_BITS,
    GL_GREEN_SCALE,
    GL_HINT_BIT,
    GL_INCR,
    GL_INDEX_ARRAY,
    GL_INDEX_ARRAY_COUNT_EXT,
    GL_INDEX_ARRAY_EXT,
    GL_INDEX_ARRAY_POINTER,
    GL_INDEX_ARRAY_POINTER_EXT,
    GL_INDEX_ARRAY_STRIDE,
    GL_INDEX_ARRAY_STRIDE_EXT,
    GL_INDEX_ARRAY_TYPE,
    GL_INDEX_ARRAY_TYPE_EXT,
    GL_INDEX_BITS,
    GL_INDEX_CLEAR_VALUE,
    GL_INDEX_LOGIC_OP,
    GL_INDEX_MODE,
    GL_INDEX_OFFSET,
    GL_INDEX_SHIFT,
    GL_INDEX_WRITEMASK,
    GL_INT,
    GL_INTENSITY,
    GL_INTENSITY12,
    GL_INTENSITY16,
    GL_INTENSITY4,
    GL_INTENSITY8,
    GL_INVALID_ENUM,
    GL_INVALID_OPERATION,
    GL_INVALID_VALUE,
    GL_INVERT,
    GL_KEEP,
    GL_LEFT,
    GL_LEQUAL,
    GL_LESS,
    GL_LIGHT0,
    GL_LIGHT1,
    GL_LIGHT2,
    GL_LIGHT3,
    GL_LIGHT4,
    GL_LIGHT5,
    GL_LIGHT6,
    GL_LIGHT7,
    GL_LIGHTING,
    GL_LIGHTING_BIT,
    GL_LIGHT_MODEL_AMBIENT,
    GL_LIGHT_MODEL_LOCAL_VIEWER,
    GL_LIGHT_MODEL_TWO_SIDE,
    GL_LINE,
    GL_LINEAR,
    GL_LINEAR_ATTENUATION,
    GL_LINEAR_MIPMAP_LINEAR,
    GL_LINEAR_MIPMAP_NEAREST,
    GL_LINES,
    GL_LINE_BIT,
    GL_LINE_LOOP,
    GL_LINE_RESET_TOKEN,
    GL_LINE_SMOOTH,
    GL_LINE_SMOOTH_HINT,
    GL_LINE_STIPPLE,
    GL_LINE_STIPPLE_PATTERN,
    GL_LINE_STIPPLE_REPEAT,
    GL_LINE_STRIP,
    GL_LINE_TOKEN,
    GL_LINE_WIDTH,
    GL_LINE_WIDTH_GRANULARITY,
    GL_LINE_WIDTH_RANGE,
    GL_LIST_BASE,
    GL_LIST_BIT,
    GL_LIST_INDEX,
    GL_LIST_MODE,
    GL_LOAD,
    GL_LOGIC_OP,
    GL_LOGIC_OP_MODE,
    GL_LUMINANCE,
    GL_LUMINANCE12,
    GL_LUMINANCE12_ALPHA12,
    GL_LUMINANCE12_ALPHA4,
    GL_LUMINANCE16,
    GL_LUMINANCE16_ALPHA16,
    GL_LUMINANCE4,
    GL_LUMINANCE4_ALPHA4,
    GL_LUMINANCE6_ALPHA2,
    GL_LUMINANCE8,
    GL_LUMINANCE8_ALPHA8,
    GL_LUMINANCE_ALPHA,
    GL_MAP1_COLOR_4,
    GL_MAP1_GRID_DOMAIN,
    GL_MAP1_GRID_SEGMENTS,
    GL_MAP1_INDEX,
    GL_MAP1_NORMAL,
    GL_MAP1_TEXTURE_COORD_1,
    GL_MAP1_TEXTURE_COORD_2,
    GL_MAP1_TEXTURE_COORD_3,
    GL_MAP1_TEXTURE_COORD_4,
    GL_MAP1_VERTEX_3,
    GL_MAP1_VERTEX_4,
    GL_MAP2_COLOR_4,
    GL_MAP2_GRID_DOMAIN,
    GL_MAP2_GRID_SEGMENTS,
    GL_MAP2_INDEX,
    GL_MAP2_NORMAL,
    GL_MAP2_TEXTURE_COORD_1,
    GL_MAP2_TEXTURE_COORD_2,
    GL_MAP2_TEXTURE_COORD_3,
    GL_MAP2_TEXTURE_COORD_4,
    GL_MAP2_VERTEX_3,
    GL_MAP2_VERTEX_4,
    GL_MAP_COLOR,
    GL_MAP_STENCIL,
    GL_MATRIX_MODE,
    GL_MAX_ATTRIB_STACK_DEPTH,
    GL_MAX_CLIENT_ATTRIB_STACK_DEPTH,
    GL_MAX_CLIP_PLANES,
    GL_MAX_ELEMENTS_INDICES_WIN,
    GL_MAX_ELEMENTS_VERTICES_WIN,
    GL_MAX_EVAL_ORDER,
    GL_MAX_LIGHTS,
    GL_MAX_LIST_NESTING,
    GL_MAX_MODELVIEW_STACK_DEPTH,
    GL_MAX_NAME_STACK_DEPTH,
    GL_MAX_PIXEL_MAP_TABLE,
    GL_MAX_PROJECTION_STACK_DEPTH,
    GL_MAX_TEXTURE_SIZE,
    GL_MAX_TEXTURE_STACK_DEPTH,
    GL_MAX_VIEWPORT_DIMS,
    GL_MODELVIEW,
    GL_MODELVIEW_MATRIX,
    GL_MODELVIEW_STACK_DEPTH,
    GL_MODULATE,
    GL_MULT,
    GL_N3F_V3F,
    GL_NAME_STACK_DEPTH,
    GL_NAND,
    GL_NEAREST,
    GL_NEAREST_MIPMAP_LINEAR,
    GL_NEAREST_MIPMAP_NEAREST,
    GL_NEVER,
    GL_NICEST,
    GL_NONE,
    GL_NOOP,
    GL_NOR,
    GL_NORMALIZE,
    GL_NORMAL_ARRAY,
    GL_NORMAL_ARRAY_COUNT_EXT,
    GL_NORMAL_ARRAY_EXT,
    GL_NORMAL_ARRAY_POINTER,
    GL_NORMAL_ARRAY_POINTER_EXT,
    GL_NORMAL_ARRAY_STRIDE,
    GL_NORMAL_ARRAY_STRIDE_EXT,
    GL_NORMAL_ARRAY_TYPE,
    GL_NORMAL_ARRAY_TYPE_EXT,
    GL_NOTEQUAL,
    GL_NO_ERROR,
    GL_OBJECT_LINEAR,
    GL_OBJECT_PLANE,
    GL_ONE,
    GL_ONE_MINUS_DST_ALPHA,
    GL_ONE_MINUS_DST_COLOR,
    GL_ONE_MINUS_SRC_ALPHA,
    GL_ONE_MINUS_SRC_COLOR,
    GL_OR,
    GL_ORDER,
    GL_OR_INVERTED,
    GL_OR_REVERSE,
    GL_OUT_OF_MEMORY,
    GL_PACK_ALIGNMENT,
    GL_PACK_LSB_FIRST,
    GL_PACK_ROW_LENGTH,
    GL_PACK_SKIP_PIXELS,
    GL_PACK_SKIP_ROWS,
    GL_PACK_SWAP_BYTES,
    GL_PASS_THROUGH_TOKEN,
    GL_PERSPECTIVE_CORRECTION_HINT,
    GL_PHONG_HINT_WIN,
    GL_PHONG_WIN,
    GL_PIXEL_MAP_A_TO_A,
    GL_PIXEL_MAP_A_TO_A_SIZE,
    GL_PIXEL_MAP_B_TO_B,
    GL_PIXEL_MAP_B_TO_B_SIZE,
    GL_PIXEL_MAP_G_TO_G,
    GL_PIXEL_MAP_G_TO_G_SIZE,
    GL_PIXEL_MAP_I_TO_A,
    GL_PIXEL_MAP_I_TO_A_SIZE,
    GL_PIXEL_MAP_I_TO_B,
    GL_PIXEL_MAP_I_TO_B_SIZE,
    GL_PIXEL_MAP_I_TO_G,
    GL_PIXEL_MAP_I_TO_G_SIZE,
    GL_PIXEL_MAP_I_TO_I,
    GL_PIXEL_MAP_I_TO_I_SIZE,
    GL_PIXEL_MAP_I_TO_R,
    GL_PIXEL_MAP_I_TO_R_SIZE,
    GL_PIXEL_MAP_R_TO_R,
    GL_PIXEL_MAP_R_TO_R_SIZE,
    GL_PIXEL_MAP_S_TO_S,
    GL_PIXEL_MAP_S_TO_S_SIZE,
    GL_PIXEL_MODE_BIT,
    GL_POINT,
    GL_POINTS,
    GL_POINT_BIT,
    GL_POINT_SIZE,
    GL_POINT_SIZE_GRANULARITY,
    GL_POINT_SIZE_RANGE,
    GL_POINT_SMOOTH,
    GL_POINT_SMOOTH_HINT,
    GL_POINT_TOKEN,
    GL_POLYGON,
    GL_POLYGON_BIT,
    GL_POLYGON_MODE,
    GL_POLYGON_OFFSET_FACTOR,
    GL_POLYGON_OFFSET_FILL,
    GL_POLYGON_OFFSET_LINE,
    GL_POLYGON_OFFSET_POINT,
    GL_POLYGON_OFFSET_UNITS,
    GL_POLYGON_SMOOTH,
    GL_POLYGON_SMOOTH_HINT,
    GL_POLYGON_STIPPLE,
    GL_POLYGON_STIPPLE_BIT,
    GL_POLYGON_TOKEN,
    GL_POSITION,
    GL_PROJECTION,
    GL_PROJECTION_MATRIX,
    GL_PROJECTION_STACK_DEPTH,
    GL_PROXY_TEXTURE_1D,
    GL_PROXY_TEXTURE_2D,
    GL_Q,
    GL_QUADRATIC_ATTENUATION,
    GL_QUADS,
    GL_QUAD_STRIP,
    GL_R,
    GL_R3_G3_B2,
    GL_READ_BUFFER,
    GL_RED,
    GL_RED_BIAS,
    GL_RED_BITS,
    GL_RED_SCALE,
    GL_RENDER,
    GL_RENDERER,
    GL_RENDER_MODE,
    GL_REPEAT,
    GL_REPLACE,
    GL_RETURN,
    GL_RGB,
    GL_RGB10,
    GL_RGB10_A2,
    GL_RGB12,
    GL_RGB16,
    GL_RGB4,
    GL_RGB5,
    GL_RGB5_A1,
    GL_RGB8,
    GL_RGBA,
    GL_RGBA12,
    GL_RGBA16,
    GL_RGBA2,
    GL_RGBA4,
    GL_RGBA8,
    GL_RGBA_MODE,
    GL_RIGHT,
    GL_S,
    GL_SCISSOR_BIT,
    GL_SCISSOR_BOX,
    GL_SCISSOR_TEST,
    GL_SELECT,
    GL_SELECTION_BUFFER_POINTER,
    GL_SELECTION_BUFFER_SIZE,
    GL_SET,
    GL_SHADE_MODEL,
    GL_SHININESS,
    GL_SHORT,
    GL_SMOOTH,
    GL_SPECULAR,
    GL_SPHERE_MAP,
    GL_SPOT_CUTOFF,
    GL_SPOT_DIRECTION,
    GL_SPOT_EXPONENT,
    GL_SRC_ALPHA,
    GL_SRC_ALPHA_SATURATE,
    GL_SRC_COLOR,
    GL_STACK_OVERFLOW,
    GL_STACK_UNDERFLOW,
    GL_STENCIL,
    GL_STENCIL_BITS,
    GL_STENCIL_BUFFER_BIT,
    GL_STENCIL_CLEAR_VALUE,
    GL_STENCIL_FAIL,
    GL_STENCIL_FUNC,
    GL_STENCIL_INDEX,
    GL_STENCIL_PASS_DEPTH_FAIL,
    GL_STENCIL_PASS_DEPTH_PASS,
    GL_STENCIL_REF,
    GL_STENCIL_TEST,
    GL_STENCIL_VALUE_MASK,
    GL_STENCIL_WRITEMASK,
    GL_STEREO,
    GL_SUBPIXEL_BITS,
    GL_T,
    GL_T2F_C3F_V3F,
    GL_T2F_C4F_N3F_V3F,
    GL_T2F_C4UB_V3F,
    GL_T2F_N3F_V3F,
    GL_T2F_V3F,
    GL_T4F_C4F_N3F_V4F,
    GL_T4F_V4F,
    GL_TEXTURE,
    GL_TEXTURE_1D,
    GL_TEXTURE_2D,
    GL_TEXTURE_ALPHA_SIZE,
    GL_TEXTURE_BINDING_1D,
    GL_TEXTURE_BINDING_2D,
    GL_TEXTURE_BIT,
    GL_TEXTURE_BLUE_SIZE,
    GL_TEXTURE_BORDER,
    GL_TEXTURE_BORDER_COLOR,
    GL_TEXTURE_COMPONENTS,
    GL_TEXTURE_COORD_ARRAY,
    GL_TEXTURE_COORD_ARRAY_COUNT_EXT,
    GL_TEXTURE_COORD_ARRAY_EXT,
    GL_TEXTURE_COORD_ARRAY_POINTER,
    GL_TEXTURE_COORD_ARRAY_POINTER_EXT,
    GL_TEXTURE_COORD_ARRAY_SIZE,
    GL_TEXTURE_COORD_ARRAY_SIZE_EXT,
    GL_TEXTURE_COORD_ARRAY_STRIDE,
    GL_TEXTURE_COORD_ARRAY_STRIDE_EXT,
    GL_TEXTURE_COORD_ARRAY_TYPE,
    GL_TEXTURE_COORD_ARRAY_TYPE_EXT,
    GL_TEXTURE_ENV,
    GL_TEXTURE_ENV_COLOR,
    GL_TEXTURE_ENV_MODE,
    GL_TEXTURE_GEN_MODE,
    GL_TEXTURE_GEN_Q,
    GL_TEXTURE_GEN_R,
    GL_TEXTURE_GEN_S,
    GL_TEXTURE_GEN_T,
    GL_TEXTURE_GREEN_SIZE,
    GL_TEXTURE_HEIGHT,
    GL_TEXTURE_INTENSITY_SIZE,
    GL_TEXTURE_INTERNAL_FORMAT,
    GL_TEXTURE_LUMINANCE_SIZE,
    GL_TEXTURE_MAG_FILTER,
    GL_TEXTURE_MATRIX,
    GL_TEXTURE_MIN_FILTER,
    GL_TEXTURE_PRIORITY,
    GL_TEXTURE_RED_SIZE,
    GL_TEXTURE_RESIDENT,
    GL_TEXTURE_STACK_DEPTH,
    GL_TEXTURE_WIDTH,
    GL_TEXTURE_WRAP_S,
    GL_TEXTURE_WRAP_T,
    GL_TRANSFORM_BIT,
    GL_TRIANGLES,
    GL_TRIANGLE_FAN,
    GL_TRIANGLE_STRIP,
    GL_TRUE,
    GL_UNPACK_ALIGNMENT,
    GL_UNPACK_LSB_FIRST,
    GL_UNPACK_ROW_LENGTH,
    GL_UNPACK_SKIP_PIXELS,
    GL_UNPACK_SKIP_ROWS,
    GL_UNPACK_SWAP_BYTES,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_INT,
    GL_UNSIGNED_SHORT,
    GL_V2F,
    GL_V3F,
    GL_VENDOR,
    GL_VERSION,
    GL_VERTEX_ARRAY,
    GL_VERTEX_ARRAY_COUNT_EXT,
    GL_VERTEX_ARRAY_EXT,
    GL_VERTEX_ARRAY_POINTER,
    GL_VERTEX_ARRAY_POINTER_EXT,
    GL_VERTEX_ARRAY_SIZE,
    GL_VERTEX_ARRAY_SIZE_EXT,
    GL_VERTEX_ARRAY_STRIDE,
    GL_VERTEX_ARRAY_STRIDE_EXT,
    GL_VERTEX_ARRAY_TYPE,
    GL_VERTEX_ARRAY_TYPE_EXT,
    GL_VIEWPORT,
    GL_VIEWPORT_BIT,
    GL_WIN_draw_range_elements,
    GL_WIN_swap_hint,
    GL_XOR,
    GL_ZERO,
    GL_ZOOM_X,
    GL_ZOOM_Y,
};


void glAccum (GLenum op, GLfloat value);
void glAlphaFunc (GLenum func, GLclampf ref);
GLboolean glAreTexturesResident (GLsizei n, const GLuint *textures, GLboolean *residences);
void glArrayElement (GLint i);
void glBegin (GLenum mode);
void glBindTexture (GLenum target, GLuint texture);
void glBitmap (GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap);
void glBlendFunc (GLenum sfactor, GLenum dfactor);
void glCallList (GLuint list);
void glCallLists (GLsizei n, GLenum type, const GLvoid *lists);
void glClear (GLbitfield mask);
void glClearAccum (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
void glClearDepth (GLclampd depth);
void glClearIndex (GLfloat c);
void glClearStencil (GLint s);
void glClipPlane (GLenum plane, const GLdouble *equation);
void glColor3b (GLbyte red, GLbyte green, GLbyte blue);
void glColor3bv (const GLbyte *v);
void glColor3d (GLdouble red, GLdouble green, GLdouble blue);
void glColor3dv (const GLdouble *v);
void glColor3f (GLfloat red, GLfloat green, GLfloat blue);
void glColor3fv (const GLfloat *v);
void glColor3i (GLint red, GLint green, GLint blue);
void glColor3iv (const GLint *v);
void glColor3s (GLshort red, GLshort green, GLshort blue);
void glColor3sv (const GLshort *v);
void glColor3ub (GLubyte red, GLubyte green, GLubyte blue);
void glColor3ubv (const GLubyte *v);
void glColor3ui (GLuint red, GLuint green, GLuint blue);
void glColor3uiv (const GLuint *v);
void glColor3us (GLushort red, GLushort green, GLushort blue);
void glColor3usv (const GLushort *v);
void glColor4b (GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha);
void glColor4bv (const GLbyte *v);
void glColor4d (GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha);
void glColor4dv (const GLdouble *v);
void glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void glColor4fv (const GLfloat *v);
void glColor4i (GLint red, GLint green, GLint blue, GLint alpha);
void glColor4iv (const GLint *v);
void glColor4s (GLshort red, GLshort green, GLshort blue, GLshort alpha);
void glColor4sv (const GLshort *v);
void glColor4ub (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
void glColor4ubv (const GLubyte *v);
void glColor4ui (GLuint red, GLuint green, GLuint blue, GLuint alpha);
void glColor4uiv (const GLuint *v);
void glColor4us (GLushort red, GLushort green, GLushort blue, GLushort alpha);
void glColor4usv (const GLushort *v);
void glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
void glColorMaterial (GLenum face, GLenum mode);
void glColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void glCopyPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum type);
void glCopyTexImage1D (GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border);
void glCopyTexImage2D (GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
void glCopyTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
void glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
void glCullFace (GLenum mode);
void glDeleteLists (GLuint list, GLsizei range);
void glDeleteTextures (GLsizei n, const GLuint *textures);
void glDepthFunc (GLenum func);
void glDepthMask (GLboolean flag);
void glDepthRange (GLclampd zNear, GLclampd zFar);
void glDisable (GLenum cap);
void glDisableClientState (GLenum array);
void glDrawArrays (GLenum mode, GLint first, GLsizei count);
void glDrawBuffer (GLenum mode);
void glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
void glDrawPixels (GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
void glEdgeFlag (GLboolean flag);
void glEdgeFlagPointer (GLsizei stride, const GLvoid *pointer);
void glEdgeFlagv (const GLboolean *flag);
void glEnable (GLenum cap);
void glEnableClientState (GLenum array);
void glEnd (void);
void glEndList (void);
void glEvalCoord1d (GLdouble u);
void glEvalCoord1dv (const GLdouble *u);
void glEvalCoord1f (GLfloat u);
void glEvalCoord1fv (const GLfloat *u);
void glEvalCoord2d (GLdouble u, GLdouble v);
void glEvalCoord2dv (const GLdouble *u);
void glEvalCoord2f (GLfloat u, GLfloat v);
void glEvalCoord2fv (const GLfloat *u);
void glEvalMesh1 (GLenum mode, GLint i1, GLint i2);
void glEvalMesh2 (GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2);
void glEvalPoint1 (GLint i);
void glEvalPoint2 (GLint i, GLint j);
void glFeedbackBuffer (GLsizei size, GLenum type, GLfloat *buffer);
void glFinish (void);
void glFlush (void);
void glFogf (GLenum pname, GLfloat param);
void glFogfv (GLenum pname, const GLfloat *params);
void glFogi (GLenum pname, GLint param);
void glFogiv (GLenum pname, const GLint *params);
void glFrontFace (GLenum mode);
void glFrustum (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
GLuint glGenLists (GLsizei range);
void glGenTextures (GLsizei n, GLuint *textures);
void glGetBooleanv (GLenum pname, GLboolean *params);
void glGetClipPlane (GLenum plane, GLdouble *equation);
void glGetDoublev (GLenum pname, GLdouble *params);
GLenum glGetError (void);
void glGetFloatv (GLenum pname, GLfloat *params);
void glGetIntegerv (GLenum pname, GLint *params);
void glGetLightfv (GLenum light, GLenum pname, GLfloat *params);
void glGetLightiv (GLenum light, GLenum pname, GLint *params);
void glGetMapdv (GLenum target, GLenum query, GLdouble *v);
void glGetMapfv (GLenum target, GLenum query, GLfloat *v);
void glGetMapiv (GLenum target, GLenum query, GLint *v);
void glGetMaterialfv (GLenum face, GLenum pname, GLfloat *params);
void glGetMaterialiv (GLenum face, GLenum pname, GLint *params);
void glGetPixelMapfv (GLenum map, GLfloat *values);
void glGetPixelMapuiv (GLenum map, GLuint *values);
void glGetPixelMapusv (GLenum map, GLushort *values);
void glGetPointerv (GLenum pname, GLvoid* *params);
void glGetPolygonStipple (GLubyte *mask);
const GLubyte * glGetString (GLenum name);
void glGetTexEnvfv (GLenum target, GLenum pname, GLfloat *params);
void glGetTexEnviv (GLenum target, GLenum pname, GLint *params);
void glGetTexGendv (GLenum coord, GLenum pname, GLdouble *params);
void glGetTexGenfv (GLenum coord, GLenum pname, GLfloat *params);
void glGetTexGeniv (GLenum coord, GLenum pname, GLint *params);
void glGetTexImage (GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels);
void glGetTexLevelParameterfv (GLenum target, GLint level, GLenum pname, GLfloat *params);
void glGetTexLevelParameteriv (GLenum target, GLint level, GLenum pname, GLint *params);
void glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params);
void glGetTexParameteriv (GLenum target, GLenum pname, GLint *params);
void glHint (GLenum target, GLenum mode);
void glIndexMask (GLuint mask);
void glIndexPointer (GLenum type, GLsizei stride, const GLvoid *pointer);
void glIndexd (GLdouble c);
void glIndexdv (const GLdouble *c);
void glIndexf (GLfloat c);
void glIndexfv (const GLfloat *c);
void glIndexi (GLint c);
void glIndexiv (const GLint *c);
void glIndexs (GLshort c);
void glIndexsv (const GLshort *c);
void glIndexub (GLubyte c);
void glIndexubv (const GLubyte *c);
void glInitNames (void);
void glInterleavedArrays (GLenum format, GLsizei stride, const GLvoid *pointer);
GLboolean glIsEnabled (GLenum cap);
GLboolean glIsList (GLuint list);
GLboolean glIsTexture (GLuint texture);
void glLightModelf (GLenum pname, GLfloat param);
void glLightModelfv (GLenum pname, const GLfloat *params);
void glLightModeli (GLenum pname, GLint param);
void glLightModeliv (GLenum pname, const GLint *params);
void glLightf (GLenum light, GLenum pname, GLfloat param);
void glLightfv (GLenum light, GLenum pname, const GLfloat *params);
void glLighti (GLenum light, GLenum pname, GLint param);
void glLightiv (GLenum light, GLenum pname, const GLint *params);
void glLineStipple (GLint factor, GLushort pattern);
void glLineWidth (GLfloat width);
void glListBase (GLuint base);
void glLoadIdentity (void);
void glLoadMatrixd (const GLdouble *m);
void glLoadMatrixf (const GLfloat *m);
void glLoadName (GLuint name);
void glLogicOp (GLenum opcode);
void glMap1d (GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points);
void glMap1f (GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points);
void glMap2d (GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points);
void glMap2f (GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points);
void glMapGrid1d (GLint un, GLdouble u1, GLdouble u2);
void glMapGrid1f (GLint un, GLfloat u1, GLfloat u2);
void glMapGrid2d (GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2);
void glMapGrid2f (GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2);
void glMaterialf (GLenum face, GLenum pname, GLfloat param);
void glMaterialfv (GLenum face, GLenum pname, const GLfloat *params);
void glMateriali (GLenum face, GLenum pname, GLint param);
void glMaterialiv (GLenum face, GLenum pname, const GLint *params);
void glMatrixMode (GLenum mode);
void glMultMatrixd (const GLdouble *m);
void glMultMatrixf (const GLfloat *m);
void glNewList (GLuint list, GLenum mode);
void glNormal3b (GLbyte nx, GLbyte ny, GLbyte nz);
void glNormal3bv (const GLbyte *v);
void glNormal3d (GLdouble nx, GLdouble ny, GLdouble nz);
void glNormal3dv (const GLdouble *v);
void glNormal3f (GLfloat nx, GLfloat ny, GLfloat nz);
void glNormal3fv (const GLfloat *v);
void glNormal3i (GLint nx, GLint ny, GLint nz);
void glNormal3iv (const GLint *v);
void glNormal3s (GLshort nx, GLshort ny, GLshort nz);
void glNormal3sv (const GLshort *v);
void glNormalPointer (GLenum type, GLsizei stride, const GLvoid *pointer);
void glOrtho (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
void glPassThrough (GLfloat token);
void glPixelMapfv (GLenum map, GLsizei mapsize, const GLfloat *values);
void glPixelMapuiv (GLenum map, GLsizei mapsize, const GLuint *values);
void glPixelMapusv (GLenum map, GLsizei mapsize, const GLushort *values);
void glPixelStoref (GLenum pname, GLfloat param);
void glPixelStorei (GLenum pname, GLint param);
void glPixelTransferf (GLenum pname, GLfloat param);
void glPixelTransferi (GLenum pname, GLint param);
void glPixelZoom (GLfloat xfactor, GLfloat yfactor);
void glPointSize (GLfloat size);
void glPolygonMode (GLenum face, GLenum mode);
void glPolygonOffset (GLfloat factor, GLfloat units);
void glPolygonStipple (const GLubyte *mask);
void glPopAttrib (void);
void glPopClientAttrib (void);
void glPopMatrix (void);
void glPopName (void);
void glPrioritizeTextures (GLsizei n, const GLuint *textures, const GLclampf *priorities);
void glPushAttrib (GLbitfield mask);
void glPushClientAttrib (GLbitfield mask);
void glPushMatrix (void);
void glPushName (GLuint name);
void glRasterPos2d (GLdouble x, GLdouble y);
void glRasterPos2dv (const GLdouble *v);
void glRasterPos2f (GLfloat x, GLfloat y);
void glRasterPos2fv (const GLfloat *v);
void glRasterPos2i (GLint x, GLint y);
void glRasterPos2iv (const GLint *v);
void glRasterPos2s (GLshort x, GLshort y);
void glRasterPos2sv (const GLshort *v);
void glRasterPos3d (GLdouble x, GLdouble y, GLdouble z);
void glRasterPos3dv (const GLdouble *v);
void glRasterPos3f (GLfloat x, GLfloat y, GLfloat z);
void glRasterPos3fv (const GLfloat *v);
void glRasterPos3i (GLint x, GLint y, GLint z);
void glRasterPos3iv (const GLint *v);
void glRasterPos3s (GLshort x, GLshort y, GLshort z);
void glRasterPos3sv (const GLshort *v);
void glRasterPos4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w);
void glRasterPos4dv (const GLdouble *v);
void glRasterPos4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w);
void glRasterPos4fv (const GLfloat *v);
void glRasterPos4i (GLint x, GLint y, GLint z, GLint w);
void glRasterPos4iv (const GLint *v);
void glRasterPos4s (GLshort x, GLshort y, GLshort z, GLshort w);
void glRasterPos4sv (const GLshort *v);
void glReadBuffer (GLenum mode);
void glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
void glRectd (GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
void glRectdv (const GLdouble *v1, const GLdouble *v2);
void glRectf (GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
void glRectfv (const GLfloat *v1, const GLfloat *v2);
void glRecti (GLint x1, GLint y1, GLint x2, GLint y2);
void glRectiv (const GLint *v1, const GLint *v2);
void glRects (GLshort x1, GLshort y1, GLshort x2, GLshort y2);
void glRectsv (const GLshort *v1, const GLshort *v2);
GLint glRenderMode (GLenum mode);
void glRotated (GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
void glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void glScaled (GLdouble x, GLdouble y, GLdouble z);
void glScalef (GLfloat x, GLfloat y, GLfloat z);
void glScissor (GLint x, GLint y, GLsizei width, GLsizei height);
void glSelectBuffer (GLsizei size, GLuint *buffer);
void glShadeModel (GLenum mode);
void glStencilFunc (GLenum func, GLint ref, GLuint mask);
void glStencilMask (GLuint mask);
void glStencilOp (GLenum fail, GLenum zfail, GLenum zpass);
void glTexCoord1d (GLdouble s);
void glTexCoord1dv (const GLdouble *v);
void glTexCoord1f (GLfloat s);
void glTexCoord1fv (const GLfloat *v);
void glTexCoord1i (GLint s);
void glTexCoord1iv (const GLint *v);
void glTexCoord1s (GLshort s);
void glTexCoord1sv (const GLshort *v);
void glTexCoord2d (GLdouble s, GLdouble t);
void glTexCoord2dv (const GLdouble *v);
void glTexCoord2f (GLfloat s, GLfloat t);
void glTexCoord2fv (const GLfloat *v);
void glTexCoord2i (GLint s, GLint t);
void glTexCoord2iv (const GLint *v);
void glTexCoord2s (GLshort s, GLshort t);
void glTexCoord2sv (const GLshort *v);
void glTexCoord3d (GLdouble s, GLdouble t, GLdouble r);
void glTexCoord3dv (const GLdouble *v);
void glTexCoord3f (GLfloat s, GLfloat t, GLfloat r);
void glTexCoord3fv (const GLfloat *v);
void glTexCoord3i (GLint s, GLint t, GLint r);
void glTexCoord3iv (const GLint *v);
void glTexCoord3s (GLshort s, GLshort t, GLshort r);
void glTexCoord3sv (const GLshort *v);
void glTexCoord4d (GLdouble s, GLdouble t, GLdouble r, GLdouble q);
void glTexCoord4dv (const GLdouble *v);
void glTexCoord4f (GLfloat s, GLfloat t, GLfloat r, GLfloat q);
void glTexCoord4fv (const GLfloat *v);
void glTexCoord4i (GLint s, GLint t, GLint r, GLint q);
void glTexCoord4iv (const GLint *v);
void glTexCoord4s (GLshort s, GLshort t, GLshort r, GLshort q);
void glTexCoord4sv (const GLshort *v);
void glTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void glTexEnvf (GLenum target, GLenum pname, GLfloat param);
void glTexEnvfv (GLenum target, GLenum pname, const GLfloat *params);
void glTexEnvi (GLenum target, GLenum pname, GLint param);
void glTexEnviv (GLenum target, GLenum pname, const GLint *params);
void glTexGend (GLenum coord, GLenum pname, GLdouble param);
void glTexGendv (GLenum coord, GLenum pname, const GLdouble *params);
void glTexGenf (GLenum coord, GLenum pname, GLfloat param);
void glTexGenfv (GLenum coord, GLenum pname, const GLfloat *params);
void glTexGeni (GLenum coord, GLenum pname, GLint param);
void glTexGeniv (GLenum coord, GLenum pname, const GLint *params);
void glTexImage1D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
void glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
void glTexParameterf (GLenum target, GLenum pname, GLfloat param);
void glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params);
void glTexParameteri (GLenum target, GLenum pname, GLint param);
void glTexParameteriv (GLenum target, GLenum pname, const GLint *params);
void glTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels);
void glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
void glTranslated (GLdouble x, GLdouble y, GLdouble z);
void glTranslatef (GLfloat x, GLfloat y, GLfloat z);
void glVertex2d (GLdouble x, GLdouble y);
void glVertex2dv (const GLdouble *v);
void glVertex2f (GLfloat x, GLfloat y);
void glVertex2fv (const GLfloat *v);
void glVertex2i (GLint x, GLint y);
void glVertex2iv (const GLint *v);
void glVertex2s (GLshort x, GLshort y);
void glVertex2sv (const GLshort *v);
void glVertex3d (GLdouble x, GLdouble y, GLdouble z);
void glVertex3dv (const GLdouble *v);
void glVertex3f (GLfloat x, GLfloat y, GLfloat z);
void glVertex3fv (const GLfloat *v);
void glVertex3i (GLint x, GLint y, GLint z);
void glVertex3iv (const GLint *v);
void glVertex3s (GLshort x, GLshort y, GLshort z);
void glVertex3sv (const GLshort *v);
void glVertex4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w);
void glVertex4dv (const GLdouble *v);
void glVertex4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w);
void glVertex4fv (const GLfloat *v);
void glVertex4i (GLint x, GLint y, GLint z, GLint w);
void glVertex4iv (const GLint *v);
void glVertex4s (GLshort x, GLshort y, GLshort z, GLshort w);
void glVertex4sv (const GLshort *v);
void glVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void glViewport (GLint x, GLint y, GLsizei width, GLsizei height);

//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
//
// $Log$
// Revision 1.1  1999/04/30 03:29:18  RD
// wxPython 2.0b9, first phase (win32)
// Added gobs of stuff, see wxPython/README.txt for details
//
//
