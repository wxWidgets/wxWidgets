/////////////////////////////////////////////////////////////////////////////
// Name:        oglpfuncs.h
// Purpose:     Types, constants, and pointers to OpenGL functions
// Author:      Manuel Martin
// Created:     2015/11/16
// Copyright:   (c) 2015 Manuel Martin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef OGLPFUNCS_H
#define OGLPFUNCS_H

/* What is this file for?
You would expect the OS to provide constants and functions used in OpenGL.
That was so up to OGL version 1.1; you just needed to include 'gl.h'. But for
newer versions things are not so simple. Constants and function prototypes (well,
typedefs for their pointers) are included in 'glext.h', a file provided by the
OpenGL Registry, and 'glcorearb.h' (also from OGL, only Core Profile definitions).
The functions "live" in the graphics card driver, and they are used through their
pointers.
The reason of this file is to retrieve those pointers, and also to define the
types, constants, and functions used in this sample, so as to avoid the inclusion
of glext.h, which is a very big file.

In the OpenGL Registry we can find also glxext.h and wglext.h (MSW/UXs versions).
These files are used for "extensions", or functions provided by GPU vendors instead
of the OpenGL Consortium. This sample doesn't use them.

OK, there are some good libraries to do this job (GLEW, GL3W, etc). Why do we
reinvent the wheel? Just because for this sample we need a small subset, and not
all of thousands of lines of code that those libraries consist of.
*/

/*
** Copyright (c) 2013-2015 The Khronos Group Inc.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and/or associated documentation files (the
** "Materials"), to deal in the Materials without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Materials, and to
** permit persons to whom the Materials are furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be included
** in all copies or substantial portions of the Materials.
**
** THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/

#if defined(__gl_h_) || defined(__GL_H__) || defined(_GL_H) || defined(__X_GL_H)
    #error gl.h included before oglpfuncs.h
#endif

// Apple defines everything on his own
#ifdef __APPLE__
    // Suppress warnings when including gl3.h
    #define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
    #include <OpenGL/gl3.h> // For Core Profile
#else
// The rest of the world needs the rest of this file

// Avoid gl.h definitions, may clash with our own definitions
#define __gl_h_
#define __GL_H__
#define _GL_H
#define __X_GL_H

// APIENTRY and GLAPI definitions
#if defined(_WIN32) || defined(__WIN32__)
    // MS Windows needs APIENTRY.
    #ifndef APIENTRY
        #define APIENTRY __stdcall
    #endif

    #ifndef GLAPI
        #if defined(_MSC_VER) //VC++
            #define GLAPI __declspec(dllimport)
        #else
            #define GLAPI extern
        #endif
    #endif
#endif // _WIN32 / __WIN32__

// Other platforms are simpler
#ifndef APIENTRY
    #define APIENTRY
#endif
#ifndef GLAPI
    #define GLAPI extern
#endif

// For ptrdiff_t
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

// GL types
typedef void GLvoid;
typedef unsigned char GLubyte;
typedef char GLchar;
typedef unsigned char GLboolean;
typedef unsigned short GLushort;
typedef int GLint;
typedef int GLsizei;
typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef unsigned int GLbitfield;
typedef float GLfloat;
typedef ptrdiff_t GLsizeiptr;
typedef ptrdiff_t GLintptr;

// Constants
#define GL_ARRAY_BUFFER                   0x8892
#define GL_BLEND                          0x0BE2
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_COLOR_BUFFER_BIT               0x00004000
#define GL_COMPILE_STATUS                 0x8B81
#define GL_DEPTH_BUFFER_BIT               0x00000100
#define GL_DEPTH_TEST                     0x0B71
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_FALSE                          0
#define GL_FIRST_VERTEX_CONVENTION        0x8E4D
#define GL_FLOAT                          0x1406
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_LEQUAL                         0x0203
#define GL_LINK_STATUS                    0x8B82
#define GL_LINEAR                         0x2601
#define GL_NO_ERROR                       0
#define GL_ONE_MINUS_SRC_ALPHA            0x0303
#define GL_RENDERER                       0x1F01
#define GL_RGBA                           0x1908
#define GL_SRC_ALPHA                      0x0302
#define GL_STATIC_DRAW                    0x88E4
#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE_2D                     0x0DE1
#define GL_TEXTURE_BASE_LEVEL             0x813C
#define GL_TEXTURE_MAX_LEVEL              0x813D
#define GL_TEXTURE_MAG_FILTER             0x2800
#define GL_TEXTURE_MIN_FILTER             0x2801
#define GL_TEXTURE_WRAP_S                 0x2802
#define GL_TEXTURE_WRAP_T                 0x2803
#define GL_TRIANGLE_STRIP                 0x0005
#define GL_UNSIGNED_BYTE                  0x1401
#define GL_UNSIGNED_SHORT                 0x1403
#define GL_VENDOR                         0x1F00
#define GL_VERSION                        0x1F02
#define GL_VERTEX_SHADER                  0x8B31

// This functions exist since OpenGL 1.1, and they don't need typedefs nor pointers.
// Trying to retrieve their pointers with wglGetProcAddress() will fail.
GLAPI void APIENTRY glBindTexture (GLenum target, GLuint texture);
GLAPI void APIENTRY glBlendFunc (GLenum sfactor, GLenum dfactor);
GLAPI void APIENTRY glClear (GLbitfield mask);
GLAPI void APIENTRY glClearColor (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
GLAPI void APIENTRY glDeleteTextures (GLsizei n, const GLuint *textures);
GLAPI void APIENTRY glDepthFunc (GLenum func);
GLAPI void APIENTRY glDisable (GLenum cap);
GLAPI void APIENTRY glDrawArrays (GLenum mode, GLint first, GLsizei count);
GLAPI void APIENTRY glDrawElements (GLenum mode, GLsizei count, GLenum type, const void *indices);
GLAPI void APIENTRY glEnable (GLenum cap);
GLAPI void APIENTRY glFinish (void);
GLAPI void APIENTRY glFlush (void);
GLAPI void APIENTRY glGenTextures (GLsizei n, GLuint *textures);
GLAPI GLenum APIENTRY glGetError (void);
GLAPI const GLubyte* APIENTRY glGetString (GLenum name);
GLAPI void APIENTRY glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
GLAPI void APIENTRY glTexParameteri (GLenum target, GLenum pname, GLint param);
GLAPI void APIENTRY glViewport (GLint x, GLint y, GLsizei width, GLsizei height);

// Typedefs for functions for OpenGL > 1.1
typedef void (APIENTRY* PFNGLACTIVETEXTUREPROC) (GLenum texture);
typedef void (APIENTRY* PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (APIENTRY* PFNGLBINDATTRIBLOCATIONPROC) (GLuint program, GLuint index, const GLchar *name);
typedef void (APIENTRY* PFNGLBINDBUFFERPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRY* PFNGLBINDVERTEXARRAYPROC) (GLuint array);
typedef void (APIENTRY* PFNGLBUFFERDATAPROC) (GLenum target, GLsizeiptr size, const void *data, GLenum usage);
typedef void (APIENTRY* PFNGLBUFFERSUBDATAPROC) (GLenum target, GLintptr offset, GLsizeiptr size, const void *data);
typedef void (APIENTRY* PFNGLCOMPILESHADERPROC) (GLuint shader);
typedef GLuint (APIENTRY* PFNGLCREATEPROGRAMPROC) (void);
typedef GLuint (APIENTRY* PFNGLCREATESHADERPROC) (GLenum type);
typedef void (APIENTRY* PFNGLDELETEBUFFERSPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRY* PFNGLDELETEPROGRAMPROC) (GLuint program);
typedef void (APIENTRY* PFNGLDELETESHADERPROC) (GLuint shader);
typedef void (APIENTRY* PFNGLDELETEVERTEXARRAYSPROC) (GLsizei n, const GLuint *arrays);
typedef void (APIENTRY* PFNGLDETACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (APIENTRY* PFNGLDISABLEVERTEXATTRIBARRAYPROC) (GLuint index);
typedef void (APIENTRY* PFNGLENABLEVERTEXATTRIBARRAYPROC) (GLuint index);
typedef void (APIENTRY* PFNGLGENBUFFERSPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRY* PFNGLGENVERTEXARRAYSPROC) (GLsizei n, GLuint *arrays);
typedef void (APIENTRY* PFNGLGETPROGRAMINFOLOGPROC) (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (APIENTRY* PFNGLGETPROGRAMIVPROC) (GLuint program, GLenum pname, GLint *params);
typedef void (APIENTRY* PFNGLGETSHADERIVPROC) (GLuint shader, GLenum pname, GLint *params);
typedef void (APIENTRY* PFNGLGETSHADERINFOLOGPROC) (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef GLint (APIENTRY* PFNGLGETUNIFORMLOCATIONPROC) (GLuint program, const GLchar *name);
typedef void (APIENTRY* PFNGLLINKPROGRAMPROC) (GLuint program);
typedef void (APIENTRY* PFNGLPROVOKINGVERTEXPROC) (GLenum mode);
typedef void (APIENTRY* PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef void (APIENTRY* PFNGLUNIFORM1IPROC) (GLint location, GLint v0);
typedef void (APIENTRY* PFNGLUNIFORM3FVPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRY* PFNGLUNIFORM4FVPROC) (GLint location, GLsizei count, const GLfloat *value);
typedef void (APIENTRY* PFNGLUNIFORMMATRIX4FVPROC) (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void (APIENTRY* PFNGLUSEPROGRAMPROC) (GLuint program);
typedef void (APIENTRY* PFNGLVERTEXATTRIBPOINTERPROC) (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);

// Pointers to functions
// NOTE:
//   For each glFunction we use a pointer name which is not the name of the function.
//   This is needed in some Linux [old] drivers, which define also these function
//   names, leading to a crash because of function vs pointer use.

#define glActiveTexture            my_glActiveTexture
#define glAttachShader             my_glAttachShader
#define glBindAttribLocation       my_glBindAttribLocation
#define glBindBuffer               my_glBindBuffer
#define glBindVertexArray          my_glBindVertexArray
#define glBufferData               my_glBufferData
#define glBufferSubData            my_glBufferSubData
#define glCompileShader            my_glCompileShader
#define glCreateProgram            my_glCreateProgram
#define glCreateShader             my_glCreateShader
#define glDeleteBuffers            my_glDeleteBuffers
#define glDeleteProgram            my_glDeleteProgram
#define glDeleteShader             my_glDeleteShader
#define glDeleteVertexArrays       my_glDeleteVertexArrays
#define glDetachShader             my_glDetachShader
#define glDisableVertexAttribArray my_glDisableVertexAttribArray
#define glEnableVertexAttribArray  my_glEnableVertexAttribArray
#define glGenBuffers               my_glGenBuffers
#define glGenVertexArrays          my_glGenVertexArrays
#define glGetProgramInfoLog        my_glGetProgramInfoLog
#define glGetProgramiv             my_glGetProgramiv
#define glGetShaderInfoLog         my_glGetShaderInfoLog
#define glGetShaderiv              my_glGetShaderiv
#define glGetUniformLocation       my_glGetUniformLocation
#define glLinkProgram              my_glLinkProgram
#define glProvokingVertex          my_glProvokingVertex
#define glShaderSource             my_glShaderSource
#define glUniform1i                my_glUniform1i
#define glUniform3fv               my_glUniform3fv
#define glUniform4fv               my_glUniform4fv
#define glUniformMatrix4fv         my_glUniformMatrix4fv
#define glUseProgram               my_glUseProgram
#define glVertexAttribPointer      my_glVertexAttribPointer

// NOTE:
//   Windows uses different pointers for different OGLcontexts.
//   If multi-context was used, these pointers must be used inside some
//   structure, something like activeOGLcontext->my_glTheFunction

extern PFNGLACTIVETEXTUREPROC my_glActiveTexture;
extern PFNGLATTACHSHADERPROC my_glAttachShader;
extern PFNGLBINDATTRIBLOCATIONPROC my_glBindAttribLocation;
extern PFNGLBINDBUFFERPROC my_glBindBuffer;
extern PFNGLBINDVERTEXARRAYPROC my_glBindVertexArray;
extern PFNGLBUFFERDATAPROC my_glBufferData;
extern PFNGLBUFFERSUBDATAPROC my_glBufferSubData;
extern PFNGLCOMPILESHADERPROC my_glCompileShader;
extern PFNGLCREATEPROGRAMPROC my_glCreateProgram;
extern PFNGLCREATESHADERPROC my_glCreateShader;
extern PFNGLDELETEBUFFERSPROC my_glDeleteBuffers;
extern PFNGLDELETEPROGRAMPROC my_glDeleteProgram;
extern PFNGLDELETESHADERPROC my_glDeleteShader;
extern PFNGLDELETEVERTEXARRAYSPROC my_glDeleteVertexArrays;
extern PFNGLDETACHSHADERPROC my_glDetachShader;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC my_glDisableVertexAttribArray;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC my_glEnableVertexAttribArray;
extern PFNGLGENBUFFERSPROC my_glGenBuffers;
extern PFNGLGENVERTEXARRAYSPROC my_glGenVertexArrays;
extern PFNGLGETPROGRAMINFOLOGPROC my_glGetProgramInfoLog;
extern PFNGLGETPROGRAMIVPROC my_glGetProgramiv;
extern PFNGLGETSHADERINFOLOGPROC my_glGetShaderInfoLog;
extern PFNGLGETSHADERIVPROC my_glGetShaderiv;
extern PFNGLGETUNIFORMLOCATIONPROC my_glGetUniformLocation;
extern PFNGLLINKPROGRAMPROC my_glLinkProgram;
extern PFNGLPROVOKINGVERTEXPROC my_glProvokingVertex;
extern PFNGLSHADERSOURCEPROC my_glShaderSource;
extern PFNGLUNIFORM1IPROC my_glUniform1i;
extern PFNGLUNIFORM3FVPROC my_glUniform3fv;
extern PFNGLUNIFORM4FVPROC my_glUniform4fv;
extern PFNGLUNIFORMMATRIX4FVPROC my_glUniformMatrix4fv;
extern PFNGLUSEPROGRAMPROC my_glUseProgram;
extern PFNGLVERTEXATTRIBPOINTERPROC my_glVertexAttribPointer;

#ifdef __cplusplus
}
#endif

#endif // __APPLE__ and rest

// This is the function to use in oglstuff.cpp
bool MyInitGLPointers();

#endif // OGLPFUNCS_H
