/////////////////////////////////////////////////////////////////////////////
// Name:        glcanvas.h
// Purpose:     wxGLCanvas, for using OpenGL with wxWindows under Windows
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma interface "glcanvas.h"
#endif

#ifndef _WX_GLCANVAS_H_
#define _WX_GLCANVAS_H_

#include <wx/scrolwin.h>

#include "gl/gl.h"

class wxGLContext: public wxObject
{
public:
   wxGLContext(bool isRGB, wxWindow *win, const wxPalette& palette = wxNullPalette);
   ~wxGLContext();

   void SetCurrent();
   void SetColour(const char *colour);
   void SwapBuffers();

   void SetupPixelFormat();
   void SetupPalette(const wxPalette& palette);
   wxPalette CreateDefaultPalette();

   inline wxPalette* GetPalette() const { return (wxPalette*) & m_palette; }
   inline wxWindow* GetWindow() const { return m_window; }
   inline WXHDC GetHDC() const { return m_hDC; }
   inline HGLRC GetGLRC() const { return m_glContext; }

public:
   HGLRC            m_glContext;
   WXHDC            m_hDC;
   wxPalette        m_palette;
   wxWindow*        m_window;
};

class wxGLCanvas: public wxScrolledWindow
{
   DECLARE_CLASS(wxGLCanvas)
 public:
   wxGLCanvas(wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = "GLCanvas", int *attribList = 0, const wxPalette& palette = wxNullPalette);
   ~wxGLCanvas();

   void SetCurrent();
   void SetColour(const char *colour);
   void SwapBuffers();

   void OnSize(wxSizeEvent& event);

   void OnQueryNewPalette(wxQueryNewPaletteEvent& event);
   void OnPaletteChanged(wxPaletteChangedEvent& event);

   inline wxGLContext* GetContext() const { return m_glContext; }

protected:
    wxGLContext*   m_glContext;  // this is typedef-ed ptr, in fact

DECLARE_EVENT_TABLE()
};

#ifdef __cplusplus
extern "C" {
#endif

/* Give extensions proper function names. */

/* N.B. - this is not completely implemented as yet */

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

#ifdef __cplusplus
}
#endif

#endif
