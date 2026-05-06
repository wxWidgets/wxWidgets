/////////////////////////////////////////////////////////////////////////////
// Name:        oglpfuncs.cpp
// Purpose:     Pointers to OpenGL functions
// Author:      Manuel Martin
// Created:     2015/11/16
// Copyright:   (c) 2015 Manuel Martin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// This header needs to be included before GL/gl.h included by wx/glcanvas.h.
#include "oglstuff.h"
#include "wx/glcanvas.h"

// Apple defines everything on his own
#ifndef __APPLE__

// Declare and initialize pointers
PFNGLACTIVETEXTUREPROC my_glActiveTexture = nullptr;
PFNGLATTACHSHADERPROC my_glAttachShader = nullptr;
PFNGLBINDATTRIBLOCATIONPROC my_glBindAttribLocation = nullptr;
PFNGLBINDBUFFERPROC my_glBindBuffer = nullptr;
PFNGLBINDVERTEXARRAYPROC my_glBindVertexArray = nullptr;
PFNGLBUFFERDATAPROC my_glBufferData = nullptr;
PFNGLBUFFERSUBDATAPROC my_glBufferSubData = nullptr;
PFNGLCOMPILESHADERPROC my_glCompileShader = nullptr;
PFNGLCREATEPROGRAMPROC my_glCreateProgram = nullptr;
PFNGLCREATESHADERPROC my_glCreateShader = nullptr;
PFNGLDELETEBUFFERSPROC my_glDeleteBuffers = nullptr;
PFNGLDELETEPROGRAMPROC my_glDeleteProgram = nullptr;
PFNGLDELETESHADERPROC my_glDeleteShader = nullptr;
PFNGLDELETEVERTEXARRAYSPROC my_glDeleteVertexArrays = nullptr;
PFNGLDETACHSHADERPROC my_glDetachShader = nullptr;
PFNGLDISABLEVERTEXATTRIBARRAYPROC my_glDisableVertexAttribArray = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC my_glEnableVertexAttribArray = nullptr;
PFNGLGENBUFFERSPROC my_glGenBuffers = nullptr;
PFNGLGENVERTEXARRAYSPROC my_glGenVertexArrays = nullptr;
PFNGLGETPROGRAMINFOLOGPROC my_glGetProgramInfoLog = nullptr;
PFNGLGETPROGRAMIVPROC my_glGetProgramiv = nullptr;
PFNGLGETSHADERINFOLOGPROC my_glGetShaderInfoLog = nullptr;
PFNGLGETSHADERIVPROC my_glGetShaderiv = nullptr;
PFNGLGETUNIFORMLOCATIONPROC my_glGetUniformLocation = nullptr;
PFNGLLINKPROGRAMPROC my_glLinkProgram = nullptr;
PFNGLPROVOKINGVERTEXPROC my_glProvokingVertex = nullptr;
PFNGLSHADERSOURCEPROC my_glShaderSource = nullptr;
PFNGLUNIFORM1IPROC my_glUniform1i = nullptr;
PFNGLUNIFORM3FVPROC my_glUniform3fv = nullptr;
PFNGLUNIFORM4FVPROC my_glUniform4fv = nullptr;
PFNGLUNIFORMMATRIX4FVPROC my_glUniformMatrix4fv = nullptr;
PFNGLUSEPROGRAMPROC my_glUseProgram = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC my_glVertexAttribPointer = nullptr;

#endif // __APPLE__

// Retrieve the pointers
#define GETANDTEST(type, name)                       \
    my_ ## name = wxGLContext::GetProcAddress<type>(#name);  \
    if (name == nullptr)                                   \
        return false;

bool MyInitGLPointers()
{
#ifdef __APPLE__
    return true;
#else
    GETANDTEST(PFNGLACTIVETEXTUREPROC, glActiveTexture)
    GETANDTEST(PFNGLATTACHSHADERPROC, glAttachShader)
    GETANDTEST(PFNGLBINDATTRIBLOCATIONPROC, glBindAttribLocation)
    GETANDTEST(PFNGLBINDBUFFERPROC, glBindBuffer)
    GETANDTEST(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray)
    GETANDTEST(PFNGLBUFFERDATAPROC, glBufferData)
    GETANDTEST(PFNGLBUFFERSUBDATAPROC, glBufferSubData)
    GETANDTEST(PFNGLCOMPILESHADERPROC, glCompileShader)
    GETANDTEST(PFNGLCREATEPROGRAMPROC, glCreateProgram)
    GETANDTEST(PFNGLCREATESHADERPROC, glCreateShader)
    GETANDTEST(PFNGLDELETEBUFFERSPROC, glDeleteBuffers)
    GETANDTEST(PFNGLDELETEPROGRAMPROC, glDeleteProgram)
    GETANDTEST(PFNGLDELETESHADERPROC, glDeleteShader)
    GETANDTEST(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays)
    GETANDTEST(PFNGLDETACHSHADERPROC, glDetachShader)
    GETANDTEST(PFNGLDISABLEVERTEXATTRIBARRAYPROC, glDisableVertexAttribArray)
    GETANDTEST(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray)
    GETANDTEST(PFNGLGENBUFFERSPROC, glGenBuffers)
    GETANDTEST(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays)
    GETANDTEST(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog)
    GETANDTEST(PFNGLGETPROGRAMIVPROC, glGetProgramiv)
    GETANDTEST(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog)
    GETANDTEST(PFNGLGETSHADERIVPROC, glGetShaderiv)
    GETANDTEST(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation)
    GETANDTEST(PFNGLLINKPROGRAMPROC, glLinkProgram)
    GETANDTEST(PFNGLPROVOKINGVERTEXPROC, glProvokingVertex)
    GETANDTEST(PFNGLSHADERSOURCEPROC, glShaderSource)
    GETANDTEST(PFNGLUNIFORM1IPROC, glUniform1i)
    GETANDTEST(PFNGLUNIFORM3FVPROC, glUniform3fv)
    GETANDTEST(PFNGLUNIFORM4FVPROC, glUniform4fv)
    GETANDTEST(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv)
    GETANDTEST(PFNGLUSEPROGRAMPROC, glUseProgram)
    GETANDTEST(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer)

    return true;
#endif
}
