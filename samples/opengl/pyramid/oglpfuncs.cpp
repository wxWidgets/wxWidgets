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
