/////////////////////////////////////////////////////////////////////////////
// Name:        oglpfuncs.cpp
// Purpose:     Pointers to OpenGL functions
// Author:      Manuel Martin
// Created:     2015/11/16
// Copyright:   (c) 2015 Manuel Martin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "oglpfuncs.h"

// Apple defines everything on his own
#ifndef __APPLE__

#if defined(_WIN32) || defined(__WIN32__)
    #ifndef WIN32_LEAN_AND_MEAN
        // Reduce a bit header VC++ compilation time
        #define WIN32_LEAN_AND_MEAN 1
        #define LE_ME_ISDEF
    #endif

    /*
    APIENTRY is defined in oglpfuncs.h as well as by windows.h. Undefine
    it to prevent a macro redefinition warning.
    */
    #undef APIENTRY
    #include <windows.h> //For wglGetProcAddress
    #ifdef LE_ME_ISDEF
        #undef WIN32_LEAN_AND_MEAN
        #undef LE_ME_ISDEF
    #endif
    // Our macro
    #define MyGetProcAddress(name) wglGetProcAddress((LPCSTR)name)
#else // Linux
    // GLX_ARB_get_proc_address
    // glXGetProcAddressARB is statically exported by all libGL implementations,
    // while glXGetProcAddress may be not available.
    #ifdef __cplusplus
        extern "C" {
    #endif
    extern void (*glXGetProcAddressARB(const GLubyte *procName))();
    #ifdef __cplusplus
        }
    #endif
    #define MyGetProcAddress(name) (*glXGetProcAddressARB)((const GLubyte*)name)
#endif

// The function to get the pointers
void* MyGetGLFuncAddress(const char* fname)
{
    void* pret = (void*) MyGetProcAddress(fname);

    // Some drivers return -apart from 0-, -1, 1, 2 or 3
    if ( pret == (void*)-1 || pret == (void*)1 || pret == (void*)2 || pret == (void*)3 )
        pret = (void*)0;

    return pret;
}

// Declare and initialize pointers
PFNGLACTIVETEXTUREPROC my_glActiveTexture = NULL;
PFNGLATTACHSHADERPROC my_glAttachShader = NULL;
PFNGLBINDATTRIBLOCATIONPROC my_glBindAttribLocation = NULL;
PFNGLBINDBUFFERPROC my_glBindBuffer = NULL;
PFNGLBINDVERTEXARRAYPROC my_glBindVertexArray = NULL;
PFNGLBUFFERDATAPROC my_glBufferData = NULL;
PFNGLBUFFERSUBDATAPROC my_glBufferSubData = NULL;
PFNGLCOMPILESHADERPROC my_glCompileShader = NULL;
PFNGLCREATEPROGRAMPROC my_glCreateProgram = NULL;
PFNGLCREATESHADERPROC my_glCreateShader = NULL;
PFNGLDELETEBUFFERSPROC my_glDeleteBuffers = NULL;
PFNGLDELETEPROGRAMPROC my_glDeleteProgram = NULL;
PFNGLDELETESHADERPROC my_glDeleteShader = NULL;
PFNGLDELETEVERTEXARRAYSPROC my_glDeleteVertexArrays = NULL;
PFNGLDETACHSHADERPROC my_glDetachShader = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC my_glDisableVertexAttribArray = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC my_glEnableVertexAttribArray = NULL;
PFNGLGENBUFFERSPROC my_glGenBuffers = NULL;
PFNGLGENVERTEXARRAYSPROC my_glGenVertexArrays = NULL;
PFNGLGETPROGRAMINFOLOGPROC my_glGetProgramInfoLog = NULL;
PFNGLGETPROGRAMIVPROC my_glGetProgramiv = NULL;
PFNGLGETSHADERINFOLOGPROC my_glGetShaderInfoLog = NULL;
PFNGLGETSHADERIVPROC my_glGetShaderiv = NULL;
PFNGLGETUNIFORMLOCATIONPROC my_glGetUniformLocation = NULL;
PFNGLLINKPROGRAMPROC my_glLinkProgram = NULL;
PFNGLPROVOKINGVERTEXPROC my_glProvokingVertex = NULL;
PFNGLSHADERSOURCEPROC my_glShaderSource = NULL;
PFNGLUNIFORM1IPROC my_glUniform1i = NULL;
PFNGLUNIFORM3FVPROC my_glUniform3fv = NULL;
PFNGLUNIFORM4FVPROC my_glUniform4fv = NULL;
PFNGLUNIFORMMATRIX4FVPROC my_glUniformMatrix4fv = NULL;
PFNGLUSEPROGRAMPROC my_glUseProgram = NULL;
PFNGLVERTEXATTRIBPOINTERPROC my_glVertexAttribPointer = NULL;

#endif // __APPLE__

// Retrieve the pointers
#define GETANDTEST(type, name)                       \
    my_ ## name = (type) MyGetGLFuncAddress(#name);  \
    if (name == 0)                                   \
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
