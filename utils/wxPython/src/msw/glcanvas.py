# This file was created automatically by SWIG.
import glcanvasc

from misc import *

from misc2 import *

from windows import *

from gdi import *

from events import *

from mdi import *

from frames import *

from stattool import *

from controls import *

from controls2 import *

from windows2 import *

from cmndlgs import *

from windows3 import *

from image import *

from printfw import *
import wx
class wxGLContextPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def __del__(self, glcanvasc=glcanvasc):
        if self.thisown == 1 :
            glcanvasc.delete_wxGLContext(self.this)
    def SetCurrent(self):
        val = glcanvasc.wxGLContext_SetCurrent(self.this)
        return val
    def SetColour(self,arg0):
        val = glcanvasc.wxGLContext_SetColour(self.this,arg0)
        return val
    def SwapBuffers(self):
        val = glcanvasc.wxGLContext_SwapBuffers(self.this)
        return val
    def GetWindow(self):
        val = glcanvasc.wxGLContext_GetWindow(self.this)
        val = wxWindowPtr(val)
        return val
    def __repr__(self):
        return "<C wxGLContext instance>"
class wxGLContext(wxGLContextPtr):
    def __init__(self,arg0,arg1,*args) :
        argl = map(None,args)
        try: argl[0] = argl[0].this
        except: pass
        args = tuple(argl)
        self.this = apply(glcanvasc.new_wxGLContext,(arg0,arg1.this,)+args)
        self.thisown = 1




class wxGLCanvasPtr(wxScrolledWindowPtr):
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def SetCurrent(self):
        val = glcanvasc.wxGLCanvas_SetCurrent(self.this)
        return val
    def SetColour(self,arg0):
        val = glcanvasc.wxGLCanvas_SetColour(self.this,arg0)
        return val
    def SwapBuffers(self):
        val = glcanvasc.wxGLCanvas_SwapBuffers(self.this)
        return val
    def GetContext(self):
        val = glcanvasc.wxGLCanvas_GetContext(self.this)
        val = wxGLContextPtr(val)
        return val
    def __repr__(self):
        return "<C wxGLCanvas instance>"
class wxGLCanvas(wxGLCanvasPtr):
    def __init__(self,arg0,*args) :
        argl = map(None,args)
        try: argl[1] = argl[1].this
        except: pass
        try: argl[2] = argl[2].this
        except: pass
        try: argl[6] = argl[6].this
        except: pass
        args = tuple(argl)
        self.this = apply(glcanvasc.new_wxGLCanvas,(arg0.this,)+args)
        self.thisown = 1
        wx._StdWindowCallbacks(self)






#-------------- FUNCTION WRAPPERS ------------------

glArrayElementEXT = glcanvasc.glArrayElementEXT

glColorPointerEXT = glcanvasc.glColorPointerEXT

glDrawArraysEXT = glcanvasc.glDrawArraysEXT

glEdgeFlagPointerEXT = glcanvasc.glEdgeFlagPointerEXT

glGetPointervEXT = glcanvasc.glGetPointervEXT

glIndexPointerEXT = glcanvasc.glIndexPointerEXT

glNormalPointerEXT = glcanvasc.glNormalPointerEXT

glTexCoordPointerEXT = glcanvasc.glTexCoordPointerEXT

glVertexPointerEXT = glcanvasc.glVertexPointerEXT

glColorSubtableEXT = glcanvasc.glColorSubtableEXT

glColorTableEXT = glcanvasc.glColorTableEXT

glCopyColorTableEXT = glcanvasc.glCopyColorTableEXT

glGetColorTableEXT = glcanvasc.glGetColorTableEXT

glGetColorTableParamaterfvEXT = glcanvasc.glGetColorTableParamaterfvEXT

glGetColorTavleParameterivEXT = glcanvasc.glGetColorTavleParameterivEXT

glLockArraysSGI = glcanvasc.glLockArraysSGI

glUnlockArraysSGI = glcanvasc.glUnlockArraysSGI

glCullParameterdvSGI = glcanvasc.glCullParameterdvSGI

glCullParameterfvSGI = glcanvasc.glCullParameterfvSGI

glIndexFuncSGI = glcanvasc.glIndexFuncSGI

glIndexMaterialSGI = glcanvasc.glIndexMaterialSGI

glAddSwapHintRectWin = glcanvasc.glAddSwapHintRectWin

glAccum = glcanvasc.glAccum

glAlphaFunc = glcanvasc.glAlphaFunc

glAreTexturesResident = glcanvasc.glAreTexturesResident

glArrayElement = glcanvasc.glArrayElement

glBegin = glcanvasc.glBegin

glBindTexture = glcanvasc.glBindTexture

glBitmap = glcanvasc.glBitmap

glBlendFunc = glcanvasc.glBlendFunc

glCallList = glcanvasc.glCallList

glCallLists = glcanvasc.glCallLists

glClear = glcanvasc.glClear

glClearAccum = glcanvasc.glClearAccum

glClearColor = glcanvasc.glClearColor

glClearDepth = glcanvasc.glClearDepth

glClearIndex = glcanvasc.glClearIndex

glClearStencil = glcanvasc.glClearStencil

glClipPlane = glcanvasc.glClipPlane

glColor3b = glcanvasc.glColor3b

glColor3bv = glcanvasc.glColor3bv

glColor3d = glcanvasc.glColor3d

glColor3dv = glcanvasc.glColor3dv

glColor3f = glcanvasc.glColor3f

glColor3fv = glcanvasc.glColor3fv

glColor3i = glcanvasc.glColor3i

glColor3iv = glcanvasc.glColor3iv

glColor3s = glcanvasc.glColor3s

glColor3sv = glcanvasc.glColor3sv

glColor3ub = glcanvasc.glColor3ub

glColor3ubv = glcanvasc.glColor3ubv

glColor3ui = glcanvasc.glColor3ui

glColor3uiv = glcanvasc.glColor3uiv

glColor3us = glcanvasc.glColor3us

glColor3usv = glcanvasc.glColor3usv

glColor4b = glcanvasc.glColor4b

glColor4bv = glcanvasc.glColor4bv

glColor4d = glcanvasc.glColor4d

glColor4dv = glcanvasc.glColor4dv

glColor4f = glcanvasc.glColor4f

glColor4fv = glcanvasc.glColor4fv

glColor4i = glcanvasc.glColor4i

glColor4iv = glcanvasc.glColor4iv

glColor4s = glcanvasc.glColor4s

glColor4sv = glcanvasc.glColor4sv

glColor4ub = glcanvasc.glColor4ub

glColor4ubv = glcanvasc.glColor4ubv

glColor4ui = glcanvasc.glColor4ui

glColor4uiv = glcanvasc.glColor4uiv

glColor4us = glcanvasc.glColor4us

glColor4usv = glcanvasc.glColor4usv

glColorMask = glcanvasc.glColorMask

glColorMaterial = glcanvasc.glColorMaterial

glColorPointer = glcanvasc.glColorPointer

glCopyPixels = glcanvasc.glCopyPixels

glCopyTexImage1D = glcanvasc.glCopyTexImage1D

glCopyTexImage2D = glcanvasc.glCopyTexImage2D

glCopyTexSubImage1D = glcanvasc.glCopyTexSubImage1D

glCopyTexSubImage2D = glcanvasc.glCopyTexSubImage2D

glCullFace = glcanvasc.glCullFace

glDeleteLists = glcanvasc.glDeleteLists

glDeleteTextures = glcanvasc.glDeleteTextures

glDepthFunc = glcanvasc.glDepthFunc

glDepthMask = glcanvasc.glDepthMask

glDepthRange = glcanvasc.glDepthRange

glDisable = glcanvasc.glDisable

glDisableClientState = glcanvasc.glDisableClientState

glDrawArrays = glcanvasc.glDrawArrays

glDrawBuffer = glcanvasc.glDrawBuffer

glDrawElements = glcanvasc.glDrawElements

glDrawPixels = glcanvasc.glDrawPixels

glEdgeFlag = glcanvasc.glEdgeFlag

glEdgeFlagPointer = glcanvasc.glEdgeFlagPointer

glEdgeFlagv = glcanvasc.glEdgeFlagv

glEnable = glcanvasc.glEnable

glEnableClientState = glcanvasc.glEnableClientState

glEnd = glcanvasc.glEnd

glEndList = glcanvasc.glEndList

glEvalCoord1d = glcanvasc.glEvalCoord1d

glEvalCoord1dv = glcanvasc.glEvalCoord1dv

glEvalCoord1f = glcanvasc.glEvalCoord1f

glEvalCoord1fv = glcanvasc.glEvalCoord1fv

glEvalCoord2d = glcanvasc.glEvalCoord2d

glEvalCoord2dv = glcanvasc.glEvalCoord2dv

glEvalCoord2f = glcanvasc.glEvalCoord2f

glEvalCoord2fv = glcanvasc.glEvalCoord2fv

glEvalMesh1 = glcanvasc.glEvalMesh1

glEvalMesh2 = glcanvasc.glEvalMesh2

glEvalPoint1 = glcanvasc.glEvalPoint1

glEvalPoint2 = glcanvasc.glEvalPoint2

glFeedbackBuffer = glcanvasc.glFeedbackBuffer

glFinish = glcanvasc.glFinish

glFlush = glcanvasc.glFlush

glFogf = glcanvasc.glFogf

glFogfv = glcanvasc.glFogfv

glFogi = glcanvasc.glFogi

glFogiv = glcanvasc.glFogiv

glFrontFace = glcanvasc.glFrontFace

glFrustum = glcanvasc.glFrustum

glGenLists = glcanvasc.glGenLists

glGenTextures = glcanvasc.glGenTextures

glGetBooleanv = glcanvasc.glGetBooleanv

glGetClipPlane = glcanvasc.glGetClipPlane

glGetDoublev = glcanvasc.glGetDoublev

glGetError = glcanvasc.glGetError

glGetFloatv = glcanvasc.glGetFloatv

glGetIntegerv = glcanvasc.glGetIntegerv

glGetLightfv = glcanvasc.glGetLightfv

glGetLightiv = glcanvasc.glGetLightiv

glGetMapdv = glcanvasc.glGetMapdv

glGetMapfv = glcanvasc.glGetMapfv

glGetMapiv = glcanvasc.glGetMapiv

glGetMaterialfv = glcanvasc.glGetMaterialfv

glGetMaterialiv = glcanvasc.glGetMaterialiv

glGetPixelMapfv = glcanvasc.glGetPixelMapfv

glGetPixelMapuiv = glcanvasc.glGetPixelMapuiv

glGetPixelMapusv = glcanvasc.glGetPixelMapusv

glGetPointerv = glcanvasc.glGetPointerv

glGetPolygonStipple = glcanvasc.glGetPolygonStipple

glGetString = glcanvasc.glGetString

glGetTexEnvfv = glcanvasc.glGetTexEnvfv

glGetTexEnviv = glcanvasc.glGetTexEnviv

glGetTexGendv = glcanvasc.glGetTexGendv

glGetTexGenfv = glcanvasc.glGetTexGenfv

glGetTexGeniv = glcanvasc.glGetTexGeniv

glGetTexImage = glcanvasc.glGetTexImage

glGetTexLevelParameterfv = glcanvasc.glGetTexLevelParameterfv

glGetTexLevelParameteriv = glcanvasc.glGetTexLevelParameteriv

glGetTexParameterfv = glcanvasc.glGetTexParameterfv

glGetTexParameteriv = glcanvasc.glGetTexParameteriv

glHint = glcanvasc.glHint

glIndexMask = glcanvasc.glIndexMask

glIndexPointer = glcanvasc.glIndexPointer

glIndexd = glcanvasc.glIndexd

glIndexdv = glcanvasc.glIndexdv

glIndexf = glcanvasc.glIndexf

glIndexfv = glcanvasc.glIndexfv

glIndexi = glcanvasc.glIndexi

glIndexiv = glcanvasc.glIndexiv

glIndexs = glcanvasc.glIndexs

glIndexsv = glcanvasc.glIndexsv

glIndexub = glcanvasc.glIndexub

glIndexubv = glcanvasc.glIndexubv

glInitNames = glcanvasc.glInitNames

glInterleavedArrays = glcanvasc.glInterleavedArrays

glIsEnabled = glcanvasc.glIsEnabled

glIsList = glcanvasc.glIsList

glIsTexture = glcanvasc.glIsTexture

glLightModelf = glcanvasc.glLightModelf

glLightModelfv = glcanvasc.glLightModelfv

glLightModeli = glcanvasc.glLightModeli

glLightModeliv = glcanvasc.glLightModeliv

glLightf = glcanvasc.glLightf

glLightfv = glcanvasc.glLightfv

glLighti = glcanvasc.glLighti

glLightiv = glcanvasc.glLightiv

glLineStipple = glcanvasc.glLineStipple

glLineWidth = glcanvasc.glLineWidth

glListBase = glcanvasc.glListBase

glLoadIdentity = glcanvasc.glLoadIdentity

glLoadMatrixd = glcanvasc.glLoadMatrixd

glLoadMatrixf = glcanvasc.glLoadMatrixf

glLoadName = glcanvasc.glLoadName

glLogicOp = glcanvasc.glLogicOp

glMap1d = glcanvasc.glMap1d

glMap1f = glcanvasc.glMap1f

glMap2d = glcanvasc.glMap2d

glMap2f = glcanvasc.glMap2f

glMapGrid1d = glcanvasc.glMapGrid1d

glMapGrid1f = glcanvasc.glMapGrid1f

glMapGrid2d = glcanvasc.glMapGrid2d

glMapGrid2f = glcanvasc.glMapGrid2f

glMaterialf = glcanvasc.glMaterialf

glMaterialfv = glcanvasc.glMaterialfv

glMateriali = glcanvasc.glMateriali

glMaterialiv = glcanvasc.glMaterialiv

glMatrixMode = glcanvasc.glMatrixMode

glMultMatrixd = glcanvasc.glMultMatrixd

glMultMatrixf = glcanvasc.glMultMatrixf

glNewList = glcanvasc.glNewList

glNormal3b = glcanvasc.glNormal3b

glNormal3bv = glcanvasc.glNormal3bv

glNormal3d = glcanvasc.glNormal3d

glNormal3dv = glcanvasc.glNormal3dv

glNormal3f = glcanvasc.glNormal3f

glNormal3fv = glcanvasc.glNormal3fv

glNormal3i = glcanvasc.glNormal3i

glNormal3iv = glcanvasc.glNormal3iv

glNormal3s = glcanvasc.glNormal3s

glNormal3sv = glcanvasc.glNormal3sv

glNormalPointer = glcanvasc.glNormalPointer

glOrtho = glcanvasc.glOrtho

glPassThrough = glcanvasc.glPassThrough

glPixelMapfv = glcanvasc.glPixelMapfv

glPixelMapuiv = glcanvasc.glPixelMapuiv

glPixelMapusv = glcanvasc.glPixelMapusv

glPixelStoref = glcanvasc.glPixelStoref

glPixelStorei = glcanvasc.glPixelStorei

glPixelTransferf = glcanvasc.glPixelTransferf

glPixelTransferi = glcanvasc.glPixelTransferi

glPixelZoom = glcanvasc.glPixelZoom

glPointSize = glcanvasc.glPointSize

glPolygonMode = glcanvasc.glPolygonMode

glPolygonOffset = glcanvasc.glPolygonOffset

glPolygonStipple = glcanvasc.glPolygonStipple

glPopAttrib = glcanvasc.glPopAttrib

glPopClientAttrib = glcanvasc.glPopClientAttrib

glPopMatrix = glcanvasc.glPopMatrix

glPopName = glcanvasc.glPopName

glPrioritizeTextures = glcanvasc.glPrioritizeTextures

glPushAttrib = glcanvasc.glPushAttrib

glPushClientAttrib = glcanvasc.glPushClientAttrib

glPushMatrix = glcanvasc.glPushMatrix

glPushName = glcanvasc.glPushName

glRasterPos2d = glcanvasc.glRasterPos2d

glRasterPos2dv = glcanvasc.glRasterPos2dv

glRasterPos2f = glcanvasc.glRasterPos2f

glRasterPos2fv = glcanvasc.glRasterPos2fv

glRasterPos2i = glcanvasc.glRasterPos2i

glRasterPos2iv = glcanvasc.glRasterPos2iv

glRasterPos2s = glcanvasc.glRasterPos2s

glRasterPos2sv = glcanvasc.glRasterPos2sv

glRasterPos3d = glcanvasc.glRasterPos3d

glRasterPos3dv = glcanvasc.glRasterPos3dv

glRasterPos3f = glcanvasc.glRasterPos3f

glRasterPos3fv = glcanvasc.glRasterPos3fv

glRasterPos3i = glcanvasc.glRasterPos3i

glRasterPos3iv = glcanvasc.glRasterPos3iv

glRasterPos3s = glcanvasc.glRasterPos3s

glRasterPos3sv = glcanvasc.glRasterPos3sv

glRasterPos4d = glcanvasc.glRasterPos4d

glRasterPos4dv = glcanvasc.glRasterPos4dv

glRasterPos4f = glcanvasc.glRasterPos4f

glRasterPos4fv = glcanvasc.glRasterPos4fv

glRasterPos4i = glcanvasc.glRasterPos4i

glRasterPos4iv = glcanvasc.glRasterPos4iv

glRasterPos4s = glcanvasc.glRasterPos4s

glRasterPos4sv = glcanvasc.glRasterPos4sv

glReadBuffer = glcanvasc.glReadBuffer

glReadPixels = glcanvasc.glReadPixels

glRectd = glcanvasc.glRectd

glRectdv = glcanvasc.glRectdv

glRectf = glcanvasc.glRectf

glRectfv = glcanvasc.glRectfv

glRecti = glcanvasc.glRecti

glRectiv = glcanvasc.glRectiv

glRects = glcanvasc.glRects

glRectsv = glcanvasc.glRectsv

glRenderMode = glcanvasc.glRenderMode

glRotated = glcanvasc.glRotated

glRotatef = glcanvasc.glRotatef

glScaled = glcanvasc.glScaled

glScalef = glcanvasc.glScalef

glScissor = glcanvasc.glScissor

glSelectBuffer = glcanvasc.glSelectBuffer

glShadeModel = glcanvasc.glShadeModel

glStencilFunc = glcanvasc.glStencilFunc

glStencilMask = glcanvasc.glStencilMask

glStencilOp = glcanvasc.glStencilOp

glTexCoord1d = glcanvasc.glTexCoord1d

glTexCoord1dv = glcanvasc.glTexCoord1dv

glTexCoord1f = glcanvasc.glTexCoord1f

glTexCoord1fv = glcanvasc.glTexCoord1fv

glTexCoord1i = glcanvasc.glTexCoord1i

glTexCoord1iv = glcanvasc.glTexCoord1iv

glTexCoord1s = glcanvasc.glTexCoord1s

glTexCoord1sv = glcanvasc.glTexCoord1sv

glTexCoord2d = glcanvasc.glTexCoord2d

glTexCoord2dv = glcanvasc.glTexCoord2dv

glTexCoord2f = glcanvasc.glTexCoord2f

glTexCoord2fv = glcanvasc.glTexCoord2fv

glTexCoord2i = glcanvasc.glTexCoord2i

glTexCoord2iv = glcanvasc.glTexCoord2iv

glTexCoord2s = glcanvasc.glTexCoord2s

glTexCoord2sv = glcanvasc.glTexCoord2sv

glTexCoord3d = glcanvasc.glTexCoord3d

glTexCoord3dv = glcanvasc.glTexCoord3dv

glTexCoord3f = glcanvasc.glTexCoord3f

glTexCoord3fv = glcanvasc.glTexCoord3fv

glTexCoord3i = glcanvasc.glTexCoord3i

glTexCoord3iv = glcanvasc.glTexCoord3iv

glTexCoord3s = glcanvasc.glTexCoord3s

glTexCoord3sv = glcanvasc.glTexCoord3sv

glTexCoord4d = glcanvasc.glTexCoord4d

glTexCoord4dv = glcanvasc.glTexCoord4dv

glTexCoord4f = glcanvasc.glTexCoord4f

glTexCoord4fv = glcanvasc.glTexCoord4fv

glTexCoord4i = glcanvasc.glTexCoord4i

glTexCoord4iv = glcanvasc.glTexCoord4iv

glTexCoord4s = glcanvasc.glTexCoord4s

glTexCoord4sv = glcanvasc.glTexCoord4sv

glTexCoordPointer = glcanvasc.glTexCoordPointer

glTexEnvf = glcanvasc.glTexEnvf

glTexEnvfv = glcanvasc.glTexEnvfv

glTexEnvi = glcanvasc.glTexEnvi

glTexEnviv = glcanvasc.glTexEnviv

glTexGend = glcanvasc.glTexGend

glTexGendv = glcanvasc.glTexGendv

glTexGenf = glcanvasc.glTexGenf

glTexGenfv = glcanvasc.glTexGenfv

glTexGeni = glcanvasc.glTexGeni

glTexGeniv = glcanvasc.glTexGeniv

glTexImage1D = glcanvasc.glTexImage1D

glTexImage2D = glcanvasc.glTexImage2D

glTexParameterf = glcanvasc.glTexParameterf

glTexParameterfv = glcanvasc.glTexParameterfv

glTexParameteri = glcanvasc.glTexParameteri

glTexParameteriv = glcanvasc.glTexParameteriv

glTexSubImage1D = glcanvasc.glTexSubImage1D

glTexSubImage2D = glcanvasc.glTexSubImage2D

glTranslated = glcanvasc.glTranslated

glTranslatef = glcanvasc.glTranslatef

glVertex2d = glcanvasc.glVertex2d

glVertex2dv = glcanvasc.glVertex2dv

glVertex2f = glcanvasc.glVertex2f

glVertex2fv = glcanvasc.glVertex2fv

glVertex2i = glcanvasc.glVertex2i

glVertex2iv = glcanvasc.glVertex2iv

glVertex2s = glcanvasc.glVertex2s

glVertex2sv = glcanvasc.glVertex2sv

glVertex3d = glcanvasc.glVertex3d

glVertex3dv = glcanvasc.glVertex3dv

glVertex3f = glcanvasc.glVertex3f

glVertex3fv = glcanvasc.glVertex3fv

glVertex3i = glcanvasc.glVertex3i

glVertex3iv = glcanvasc.glVertex3iv

glVertex3s = glcanvasc.glVertex3s

glVertex3sv = glcanvasc.glVertex3sv

glVertex4d = glcanvasc.glVertex4d

glVertex4dv = glcanvasc.glVertex4dv

glVertex4f = glcanvasc.glVertex4f

glVertex4fv = glcanvasc.glVertex4fv

glVertex4i = glcanvasc.glVertex4i

glVertex4iv = glcanvasc.glVertex4iv

glVertex4s = glcanvasc.glVertex4s

glVertex4sv = glcanvasc.glVertex4sv

glVertexPointer = glcanvasc.glVertexPointer

glViewport = glcanvasc.glViewport



#-------------- VARIABLE WRAPPERS ------------------

GL_2D = glcanvasc.GL_2D
GL_2_BYTES = glcanvasc.GL_2_BYTES
GL_3D = glcanvasc.GL_3D
GL_3D_COLOR = glcanvasc.GL_3D_COLOR
GL_3D_COLOR_TEXTURE = glcanvasc.GL_3D_COLOR_TEXTURE
GL_3_BYTES = glcanvasc.GL_3_BYTES
GL_4D_COLOR_TEXTURE = glcanvasc.GL_4D_COLOR_TEXTURE
GL_4_BYTES = glcanvasc.GL_4_BYTES
GL_ACCUM = glcanvasc.GL_ACCUM
GL_ACCUM_ALPHA_BITS = glcanvasc.GL_ACCUM_ALPHA_BITS
GL_ACCUM_BLUE_BITS = glcanvasc.GL_ACCUM_BLUE_BITS
GL_ACCUM_BUFFER_BIT = glcanvasc.GL_ACCUM_BUFFER_BIT
GL_ACCUM_CLEAR_VALUE = glcanvasc.GL_ACCUM_CLEAR_VALUE
GL_ACCUM_GREEN_BITS = glcanvasc.GL_ACCUM_GREEN_BITS
GL_ACCUM_RED_BITS = glcanvasc.GL_ACCUM_RED_BITS
GL_ADD = glcanvasc.GL_ADD
GL_ALL_ATTRIB_BITS = glcanvasc.GL_ALL_ATTRIB_BITS
GL_ALPHA = glcanvasc.GL_ALPHA
GL_ALPHA12 = glcanvasc.GL_ALPHA12
GL_ALPHA16 = glcanvasc.GL_ALPHA16
GL_ALPHA4 = glcanvasc.GL_ALPHA4
GL_ALPHA8 = glcanvasc.GL_ALPHA8
GL_ALPHA_BIAS = glcanvasc.GL_ALPHA_BIAS
GL_ALPHA_BITS = glcanvasc.GL_ALPHA_BITS
GL_ALPHA_SCALE = glcanvasc.GL_ALPHA_SCALE
GL_ALPHA_TEST = glcanvasc.GL_ALPHA_TEST
GL_ALPHA_TEST_FUNC = glcanvasc.GL_ALPHA_TEST_FUNC
GL_ALPHA_TEST_REF = glcanvasc.GL_ALPHA_TEST_REF
GL_ALWAYS = glcanvasc.GL_ALWAYS
GL_AMBIENT = glcanvasc.GL_AMBIENT
GL_AMBIENT_AND_DIFFUSE = glcanvasc.GL_AMBIENT_AND_DIFFUSE
GL_AND = glcanvasc.GL_AND
GL_AND_INVERTED = glcanvasc.GL_AND_INVERTED
GL_AND_REVERSE = glcanvasc.GL_AND_REVERSE
GL_ATTRIB_STACK_DEPTH = glcanvasc.GL_ATTRIB_STACK_DEPTH
GL_AUTO_NORMAL = glcanvasc.GL_AUTO_NORMAL
GL_AUX0 = glcanvasc.GL_AUX0
GL_AUX1 = glcanvasc.GL_AUX1
GL_AUX2 = glcanvasc.GL_AUX2
GL_AUX3 = glcanvasc.GL_AUX3
GL_AUX_BUFFERS = glcanvasc.GL_AUX_BUFFERS
GL_BACK = glcanvasc.GL_BACK
GL_BACK_LEFT = glcanvasc.GL_BACK_LEFT
GL_BACK_RIGHT = glcanvasc.GL_BACK_RIGHT
GL_BGRA_EXT = glcanvasc.GL_BGRA_EXT
GL_BGR_EXT = glcanvasc.GL_BGR_EXT
GL_BITMAP = glcanvasc.GL_BITMAP
GL_BITMAP_TOKEN = glcanvasc.GL_BITMAP_TOKEN
GL_BLEND = glcanvasc.GL_BLEND
GL_BLEND_DST = glcanvasc.GL_BLEND_DST
GL_BLEND_SRC = glcanvasc.GL_BLEND_SRC
GL_BLUE = glcanvasc.GL_BLUE
GL_BLUE_BIAS = glcanvasc.GL_BLUE_BIAS
GL_BLUE_BITS = glcanvasc.GL_BLUE_BITS
GL_BLUE_SCALE = glcanvasc.GL_BLUE_SCALE
GL_BYTE = glcanvasc.GL_BYTE
GL_C3F_V3F = glcanvasc.GL_C3F_V3F
GL_C4F_N3F_V3F = glcanvasc.GL_C4F_N3F_V3F
GL_C4UB_V2F = glcanvasc.GL_C4UB_V2F
GL_C4UB_V3F = glcanvasc.GL_C4UB_V3F
GL_CCW = glcanvasc.GL_CCW
GL_CLAMP = glcanvasc.GL_CLAMP
GL_CLEAR = glcanvasc.GL_CLEAR
GL_CLIENT_ALL_ATTRIB_BITS = glcanvasc.GL_CLIENT_ALL_ATTRIB_BITS
GL_CLIENT_ATTRIB_STACK_DEPTH = glcanvasc.GL_CLIENT_ATTRIB_STACK_DEPTH
GL_CLIENT_PIXEL_STORE_BIT = glcanvasc.GL_CLIENT_PIXEL_STORE_BIT
GL_CLIENT_VERTEX_ARRAY_BIT = glcanvasc.GL_CLIENT_VERTEX_ARRAY_BIT
GL_CLIP_PLANE0 = glcanvasc.GL_CLIP_PLANE0
GL_CLIP_PLANE1 = glcanvasc.GL_CLIP_PLANE1
GL_CLIP_PLANE2 = glcanvasc.GL_CLIP_PLANE2
GL_CLIP_PLANE3 = glcanvasc.GL_CLIP_PLANE3
GL_CLIP_PLANE4 = glcanvasc.GL_CLIP_PLANE4
GL_CLIP_PLANE5 = glcanvasc.GL_CLIP_PLANE5
GL_COEFF = glcanvasc.GL_COEFF
GL_COLOR = glcanvasc.GL_COLOR
GL_COLOR_ARRAY = glcanvasc.GL_COLOR_ARRAY
GL_COLOR_ARRAY_COUNT_EXT = glcanvasc.GL_COLOR_ARRAY_COUNT_EXT
GL_COLOR_ARRAY_EXT = glcanvasc.GL_COLOR_ARRAY_EXT
GL_COLOR_ARRAY_POINTER = glcanvasc.GL_COLOR_ARRAY_POINTER
GL_COLOR_ARRAY_POINTER_EXT = glcanvasc.GL_COLOR_ARRAY_POINTER_EXT
GL_COLOR_ARRAY_SIZE = glcanvasc.GL_COLOR_ARRAY_SIZE
GL_COLOR_ARRAY_SIZE_EXT = glcanvasc.GL_COLOR_ARRAY_SIZE_EXT
GL_COLOR_ARRAY_STRIDE = glcanvasc.GL_COLOR_ARRAY_STRIDE
GL_COLOR_ARRAY_STRIDE_EXT = glcanvasc.GL_COLOR_ARRAY_STRIDE_EXT
GL_COLOR_ARRAY_TYPE = glcanvasc.GL_COLOR_ARRAY_TYPE
GL_COLOR_ARRAY_TYPE_EXT = glcanvasc.GL_COLOR_ARRAY_TYPE_EXT
GL_COLOR_BUFFER_BIT = glcanvasc.GL_COLOR_BUFFER_BIT
GL_COLOR_CLEAR_VALUE = glcanvasc.GL_COLOR_CLEAR_VALUE
GL_COLOR_INDEX = glcanvasc.GL_COLOR_INDEX
GL_COLOR_INDEX12_EXT = glcanvasc.GL_COLOR_INDEX12_EXT
GL_COLOR_INDEX16_EXT = glcanvasc.GL_COLOR_INDEX16_EXT
GL_COLOR_INDEX1_EXT = glcanvasc.GL_COLOR_INDEX1_EXT
GL_COLOR_INDEX2_EXT = glcanvasc.GL_COLOR_INDEX2_EXT
GL_COLOR_INDEX4_EXT = glcanvasc.GL_COLOR_INDEX4_EXT
GL_COLOR_INDEX8_EXT = glcanvasc.GL_COLOR_INDEX8_EXT
GL_COLOR_INDEXES = glcanvasc.GL_COLOR_INDEXES
GL_COLOR_LOGIC_OP = glcanvasc.GL_COLOR_LOGIC_OP
GL_COLOR_MATERIAL = glcanvasc.GL_COLOR_MATERIAL
GL_COLOR_MATERIAL_FACE = glcanvasc.GL_COLOR_MATERIAL_FACE
GL_COLOR_MATERIAL_PARAMETER = glcanvasc.GL_COLOR_MATERIAL_PARAMETER
GL_COLOR_TABLE_ALPHA_SIZE_EXT = glcanvasc.GL_COLOR_TABLE_ALPHA_SIZE_EXT
GL_COLOR_TABLE_BLUE_SIZE_EXT = glcanvasc.GL_COLOR_TABLE_BLUE_SIZE_EXT
GL_COLOR_TABLE_FORMAT_EXT = glcanvasc.GL_COLOR_TABLE_FORMAT_EXT
GL_COLOR_TABLE_GREEN_SIZE_EXT = glcanvasc.GL_COLOR_TABLE_GREEN_SIZE_EXT
GL_COLOR_TABLE_INTENSITY_SIZE_EXT = glcanvasc.GL_COLOR_TABLE_INTENSITY_SIZE_EXT
GL_COLOR_TABLE_LUMINANCE_SIZE_EXT = glcanvasc.GL_COLOR_TABLE_LUMINANCE_SIZE_EXT
GL_COLOR_TABLE_RED_SIZE_EXT = glcanvasc.GL_COLOR_TABLE_RED_SIZE_EXT
GL_COLOR_TABLE_WIDTH_EXT = glcanvasc.GL_COLOR_TABLE_WIDTH_EXT
GL_COLOR_WRITEMASK = glcanvasc.GL_COLOR_WRITEMASK
GL_COMPILE = glcanvasc.GL_COMPILE
GL_COMPILE_AND_EXECUTE = glcanvasc.GL_COMPILE_AND_EXECUTE
GL_CONSTANT_ATTENUATION = glcanvasc.GL_CONSTANT_ATTENUATION
GL_COPY = glcanvasc.GL_COPY
GL_COPY_INVERTED = glcanvasc.GL_COPY_INVERTED
GL_COPY_PIXEL_TOKEN = glcanvasc.GL_COPY_PIXEL_TOKEN
GL_CULL_FACE = glcanvasc.GL_CULL_FACE
GL_CULL_FACE_MODE = glcanvasc.GL_CULL_FACE_MODE
GL_CURRENT_BIT = glcanvasc.GL_CURRENT_BIT
GL_CURRENT_COLOR = glcanvasc.GL_CURRENT_COLOR
GL_CURRENT_INDEX = glcanvasc.GL_CURRENT_INDEX
GL_CURRENT_NORMAL = glcanvasc.GL_CURRENT_NORMAL
GL_CURRENT_RASTER_COLOR = glcanvasc.GL_CURRENT_RASTER_COLOR
GL_CURRENT_RASTER_DISTANCE = glcanvasc.GL_CURRENT_RASTER_DISTANCE
GL_CURRENT_RASTER_INDEX = glcanvasc.GL_CURRENT_RASTER_INDEX
GL_CURRENT_RASTER_POSITION = glcanvasc.GL_CURRENT_RASTER_POSITION
GL_CURRENT_RASTER_POSITION_VALID = glcanvasc.GL_CURRENT_RASTER_POSITION_VALID
GL_CURRENT_RASTER_TEXTURE_COORDS = glcanvasc.GL_CURRENT_RASTER_TEXTURE_COORDS
GL_CURRENT_TEXTURE_COORDS = glcanvasc.GL_CURRENT_TEXTURE_COORDS
GL_CW = glcanvasc.GL_CW
GL_DECAL = glcanvasc.GL_DECAL
GL_DECR = glcanvasc.GL_DECR
GL_DEPTH = glcanvasc.GL_DEPTH
GL_DEPTH_BIAS = glcanvasc.GL_DEPTH_BIAS
GL_DEPTH_BITS = glcanvasc.GL_DEPTH_BITS
GL_DEPTH_BUFFER_BIT = glcanvasc.GL_DEPTH_BUFFER_BIT
GL_DEPTH_CLEAR_VALUE = glcanvasc.GL_DEPTH_CLEAR_VALUE
GL_DEPTH_COMPONENT = glcanvasc.GL_DEPTH_COMPONENT
GL_DEPTH_FUNC = glcanvasc.GL_DEPTH_FUNC
GL_DEPTH_RANGE = glcanvasc.GL_DEPTH_RANGE
GL_DEPTH_SCALE = glcanvasc.GL_DEPTH_SCALE
GL_DEPTH_TEST = glcanvasc.GL_DEPTH_TEST
GL_DEPTH_WRITEMASK = glcanvasc.GL_DEPTH_WRITEMASK
GL_DIFFUSE = glcanvasc.GL_DIFFUSE
GL_DITHER = glcanvasc.GL_DITHER
GL_DOMAIN = glcanvasc.GL_DOMAIN
GL_DONT_CARE = glcanvasc.GL_DONT_CARE
GL_DOUBLE = glcanvasc.GL_DOUBLE
GL_DOUBLEBUFFER = glcanvasc.GL_DOUBLEBUFFER
GL_DOUBLE_EXT = glcanvasc.GL_DOUBLE_EXT
GL_DRAW_BUFFER = glcanvasc.GL_DRAW_BUFFER
GL_DRAW_PIXEL_TOKEN = glcanvasc.GL_DRAW_PIXEL_TOKEN
GL_DST_ALPHA = glcanvasc.GL_DST_ALPHA
GL_DST_COLOR = glcanvasc.GL_DST_COLOR
GL_EDGE_FLAG = glcanvasc.GL_EDGE_FLAG
GL_EDGE_FLAG_ARRAY = glcanvasc.GL_EDGE_FLAG_ARRAY
GL_EDGE_FLAG_ARRAY_COUNT_EXT = glcanvasc.GL_EDGE_FLAG_ARRAY_COUNT_EXT
GL_EDGE_FLAG_ARRAY_EXT = glcanvasc.GL_EDGE_FLAG_ARRAY_EXT
GL_EDGE_FLAG_ARRAY_POINTER = glcanvasc.GL_EDGE_FLAG_ARRAY_POINTER
GL_EDGE_FLAG_ARRAY_POINTER_EXT = glcanvasc.GL_EDGE_FLAG_ARRAY_POINTER_EXT
GL_EDGE_FLAG_ARRAY_STRIDE = glcanvasc.GL_EDGE_FLAG_ARRAY_STRIDE
GL_EDGE_FLAG_ARRAY_STRIDE_EXT = glcanvasc.GL_EDGE_FLAG_ARRAY_STRIDE_EXT
GL_EMISSION = glcanvasc.GL_EMISSION
GL_ENABLE_BIT = glcanvasc.GL_ENABLE_BIT
GL_EQUAL = glcanvasc.GL_EQUAL
GL_EQUIV = glcanvasc.GL_EQUIV
GL_EVAL_BIT = glcanvasc.GL_EVAL_BIT
GL_EXP = glcanvasc.GL_EXP
GL_EXP2 = glcanvasc.GL_EXP2
GL_EXTENSIONS = glcanvasc.GL_EXTENSIONS
GL_EXT_bgra = glcanvasc.GL_EXT_bgra
GL_EXT_paletted_texture = glcanvasc.GL_EXT_paletted_texture
GL_EXT_vertex_array = glcanvasc.GL_EXT_vertex_array
GL_EYE_LINEAR = glcanvasc.GL_EYE_LINEAR
GL_EYE_PLANE = glcanvasc.GL_EYE_PLANE
GL_FALSE = glcanvasc.GL_FALSE
GL_FASTEST = glcanvasc.GL_FASTEST
GL_FEEDBACK = glcanvasc.GL_FEEDBACK
GL_FEEDBACK_BUFFER_POINTER = glcanvasc.GL_FEEDBACK_BUFFER_POINTER
GL_FEEDBACK_BUFFER_SIZE = glcanvasc.GL_FEEDBACK_BUFFER_SIZE
GL_FEEDBACK_BUFFER_TYPE = glcanvasc.GL_FEEDBACK_BUFFER_TYPE
GL_FILL = glcanvasc.GL_FILL
GL_FLAT = glcanvasc.GL_FLAT
GL_FLOAT = glcanvasc.GL_FLOAT
GL_FOG = glcanvasc.GL_FOG
GL_FOG_BIT = glcanvasc.GL_FOG_BIT
GL_FOG_COLOR = glcanvasc.GL_FOG_COLOR
GL_FOG_DENSITY = glcanvasc.GL_FOG_DENSITY
GL_FOG_END = glcanvasc.GL_FOG_END
GL_FOG_HINT = glcanvasc.GL_FOG_HINT
GL_FOG_INDEX = glcanvasc.GL_FOG_INDEX
GL_FOG_MODE = glcanvasc.GL_FOG_MODE
GL_FOG_START = glcanvasc.GL_FOG_START
GL_FRONT = glcanvasc.GL_FRONT
GL_FRONT_AND_BACK = glcanvasc.GL_FRONT_AND_BACK
GL_FRONT_FACE = glcanvasc.GL_FRONT_FACE
GL_FRONT_LEFT = glcanvasc.GL_FRONT_LEFT
GL_FRONT_RIGHT = glcanvasc.GL_FRONT_RIGHT
GL_GEQUAL = glcanvasc.GL_GEQUAL
GL_GREATER = glcanvasc.GL_GREATER
GL_GREEN = glcanvasc.GL_GREEN
GL_GREEN_BIAS = glcanvasc.GL_GREEN_BIAS
GL_GREEN_BITS = glcanvasc.GL_GREEN_BITS
GL_GREEN_SCALE = glcanvasc.GL_GREEN_SCALE
GL_HINT_BIT = glcanvasc.GL_HINT_BIT
GL_INCR = glcanvasc.GL_INCR
GL_INDEX_ARRAY = glcanvasc.GL_INDEX_ARRAY
GL_INDEX_ARRAY_COUNT_EXT = glcanvasc.GL_INDEX_ARRAY_COUNT_EXT
GL_INDEX_ARRAY_EXT = glcanvasc.GL_INDEX_ARRAY_EXT
GL_INDEX_ARRAY_POINTER = glcanvasc.GL_INDEX_ARRAY_POINTER
GL_INDEX_ARRAY_POINTER_EXT = glcanvasc.GL_INDEX_ARRAY_POINTER_EXT
GL_INDEX_ARRAY_STRIDE = glcanvasc.GL_INDEX_ARRAY_STRIDE
GL_INDEX_ARRAY_STRIDE_EXT = glcanvasc.GL_INDEX_ARRAY_STRIDE_EXT
GL_INDEX_ARRAY_TYPE = glcanvasc.GL_INDEX_ARRAY_TYPE
GL_INDEX_ARRAY_TYPE_EXT = glcanvasc.GL_INDEX_ARRAY_TYPE_EXT
GL_INDEX_BITS = glcanvasc.GL_INDEX_BITS
GL_INDEX_CLEAR_VALUE = glcanvasc.GL_INDEX_CLEAR_VALUE
GL_INDEX_LOGIC_OP = glcanvasc.GL_INDEX_LOGIC_OP
GL_INDEX_MODE = glcanvasc.GL_INDEX_MODE
GL_INDEX_OFFSET = glcanvasc.GL_INDEX_OFFSET
GL_INDEX_SHIFT = glcanvasc.GL_INDEX_SHIFT
GL_INDEX_WRITEMASK = glcanvasc.GL_INDEX_WRITEMASK
GL_INT = glcanvasc.GL_INT
GL_INTENSITY = glcanvasc.GL_INTENSITY
GL_INTENSITY12 = glcanvasc.GL_INTENSITY12
GL_INTENSITY16 = glcanvasc.GL_INTENSITY16
GL_INTENSITY4 = glcanvasc.GL_INTENSITY4
GL_INTENSITY8 = glcanvasc.GL_INTENSITY8
GL_INVALID_ENUM = glcanvasc.GL_INVALID_ENUM
GL_INVALID_OPERATION = glcanvasc.GL_INVALID_OPERATION
GL_INVALID_VALUE = glcanvasc.GL_INVALID_VALUE
GL_INVERT = glcanvasc.GL_INVERT
GL_KEEP = glcanvasc.GL_KEEP
GL_LEFT = glcanvasc.GL_LEFT
GL_LEQUAL = glcanvasc.GL_LEQUAL
GL_LESS = glcanvasc.GL_LESS
GL_LIGHT0 = glcanvasc.GL_LIGHT0
GL_LIGHT1 = glcanvasc.GL_LIGHT1
GL_LIGHT2 = glcanvasc.GL_LIGHT2
GL_LIGHT3 = glcanvasc.GL_LIGHT3
GL_LIGHT4 = glcanvasc.GL_LIGHT4
GL_LIGHT5 = glcanvasc.GL_LIGHT5
GL_LIGHT6 = glcanvasc.GL_LIGHT6
GL_LIGHT7 = glcanvasc.GL_LIGHT7
GL_LIGHTING = glcanvasc.GL_LIGHTING
GL_LIGHTING_BIT = glcanvasc.GL_LIGHTING_BIT
GL_LIGHT_MODEL_AMBIENT = glcanvasc.GL_LIGHT_MODEL_AMBIENT
GL_LIGHT_MODEL_LOCAL_VIEWER = glcanvasc.GL_LIGHT_MODEL_LOCAL_VIEWER
GL_LIGHT_MODEL_TWO_SIDE = glcanvasc.GL_LIGHT_MODEL_TWO_SIDE
GL_LINE = glcanvasc.GL_LINE
GL_LINEAR = glcanvasc.GL_LINEAR
GL_LINEAR_ATTENUATION = glcanvasc.GL_LINEAR_ATTENUATION
GL_LINEAR_MIPMAP_LINEAR = glcanvasc.GL_LINEAR_MIPMAP_LINEAR
GL_LINEAR_MIPMAP_NEAREST = glcanvasc.GL_LINEAR_MIPMAP_NEAREST
GL_LINES = glcanvasc.GL_LINES
GL_LINE_BIT = glcanvasc.GL_LINE_BIT
GL_LINE_LOOP = glcanvasc.GL_LINE_LOOP
GL_LINE_RESET_TOKEN = glcanvasc.GL_LINE_RESET_TOKEN
GL_LINE_SMOOTH = glcanvasc.GL_LINE_SMOOTH
GL_LINE_SMOOTH_HINT = glcanvasc.GL_LINE_SMOOTH_HINT
GL_LINE_STIPPLE = glcanvasc.GL_LINE_STIPPLE
GL_LINE_STIPPLE_PATTERN = glcanvasc.GL_LINE_STIPPLE_PATTERN
GL_LINE_STIPPLE_REPEAT = glcanvasc.GL_LINE_STIPPLE_REPEAT
GL_LINE_STRIP = glcanvasc.GL_LINE_STRIP
GL_LINE_TOKEN = glcanvasc.GL_LINE_TOKEN
GL_LINE_WIDTH = glcanvasc.GL_LINE_WIDTH
GL_LINE_WIDTH_GRANULARITY = glcanvasc.GL_LINE_WIDTH_GRANULARITY
GL_LINE_WIDTH_RANGE = glcanvasc.GL_LINE_WIDTH_RANGE
GL_LIST_BASE = glcanvasc.GL_LIST_BASE
GL_LIST_BIT = glcanvasc.GL_LIST_BIT
GL_LIST_INDEX = glcanvasc.GL_LIST_INDEX
GL_LIST_MODE = glcanvasc.GL_LIST_MODE
GL_LOAD = glcanvasc.GL_LOAD
GL_LOGIC_OP = glcanvasc.GL_LOGIC_OP
GL_LOGIC_OP_MODE = glcanvasc.GL_LOGIC_OP_MODE
GL_LUMINANCE = glcanvasc.GL_LUMINANCE
GL_LUMINANCE12 = glcanvasc.GL_LUMINANCE12
GL_LUMINANCE12_ALPHA12 = glcanvasc.GL_LUMINANCE12_ALPHA12
GL_LUMINANCE12_ALPHA4 = glcanvasc.GL_LUMINANCE12_ALPHA4
GL_LUMINANCE16 = glcanvasc.GL_LUMINANCE16
GL_LUMINANCE16_ALPHA16 = glcanvasc.GL_LUMINANCE16_ALPHA16
GL_LUMINANCE4 = glcanvasc.GL_LUMINANCE4
GL_LUMINANCE4_ALPHA4 = glcanvasc.GL_LUMINANCE4_ALPHA4
GL_LUMINANCE6_ALPHA2 = glcanvasc.GL_LUMINANCE6_ALPHA2
GL_LUMINANCE8 = glcanvasc.GL_LUMINANCE8
GL_LUMINANCE8_ALPHA8 = glcanvasc.GL_LUMINANCE8_ALPHA8
GL_LUMINANCE_ALPHA = glcanvasc.GL_LUMINANCE_ALPHA
GL_MAP1_COLOR_4 = glcanvasc.GL_MAP1_COLOR_4
GL_MAP1_GRID_DOMAIN = glcanvasc.GL_MAP1_GRID_DOMAIN
GL_MAP1_GRID_SEGMENTS = glcanvasc.GL_MAP1_GRID_SEGMENTS
GL_MAP1_INDEX = glcanvasc.GL_MAP1_INDEX
GL_MAP1_NORMAL = glcanvasc.GL_MAP1_NORMAL
GL_MAP1_TEXTURE_COORD_1 = glcanvasc.GL_MAP1_TEXTURE_COORD_1
GL_MAP1_TEXTURE_COORD_2 = glcanvasc.GL_MAP1_TEXTURE_COORD_2
GL_MAP1_TEXTURE_COORD_3 = glcanvasc.GL_MAP1_TEXTURE_COORD_3
GL_MAP1_TEXTURE_COORD_4 = glcanvasc.GL_MAP1_TEXTURE_COORD_4
GL_MAP1_VERTEX_3 = glcanvasc.GL_MAP1_VERTEX_3
GL_MAP1_VERTEX_4 = glcanvasc.GL_MAP1_VERTEX_4
GL_MAP2_COLOR_4 = glcanvasc.GL_MAP2_COLOR_4
GL_MAP2_GRID_DOMAIN = glcanvasc.GL_MAP2_GRID_DOMAIN
GL_MAP2_GRID_SEGMENTS = glcanvasc.GL_MAP2_GRID_SEGMENTS
GL_MAP2_INDEX = glcanvasc.GL_MAP2_INDEX
GL_MAP2_NORMAL = glcanvasc.GL_MAP2_NORMAL
GL_MAP2_TEXTURE_COORD_1 = glcanvasc.GL_MAP2_TEXTURE_COORD_1
GL_MAP2_TEXTURE_COORD_2 = glcanvasc.GL_MAP2_TEXTURE_COORD_2
GL_MAP2_TEXTURE_COORD_3 = glcanvasc.GL_MAP2_TEXTURE_COORD_3
GL_MAP2_TEXTURE_COORD_4 = glcanvasc.GL_MAP2_TEXTURE_COORD_4
GL_MAP2_VERTEX_3 = glcanvasc.GL_MAP2_VERTEX_3
GL_MAP2_VERTEX_4 = glcanvasc.GL_MAP2_VERTEX_4
GL_MAP_COLOR = glcanvasc.GL_MAP_COLOR
GL_MAP_STENCIL = glcanvasc.GL_MAP_STENCIL
GL_MATRIX_MODE = glcanvasc.GL_MATRIX_MODE
GL_MAX_ATTRIB_STACK_DEPTH = glcanvasc.GL_MAX_ATTRIB_STACK_DEPTH
GL_MAX_CLIENT_ATTRIB_STACK_DEPTH = glcanvasc.GL_MAX_CLIENT_ATTRIB_STACK_DEPTH
GL_MAX_CLIP_PLANES = glcanvasc.GL_MAX_CLIP_PLANES
GL_MAX_EVAL_ORDER = glcanvasc.GL_MAX_EVAL_ORDER
GL_MAX_LIGHTS = glcanvasc.GL_MAX_LIGHTS
GL_MAX_LIST_NESTING = glcanvasc.GL_MAX_LIST_NESTING
GL_MAX_MODELVIEW_STACK_DEPTH = glcanvasc.GL_MAX_MODELVIEW_STACK_DEPTH
GL_MAX_NAME_STACK_DEPTH = glcanvasc.GL_MAX_NAME_STACK_DEPTH
GL_MAX_PIXEL_MAP_TABLE = glcanvasc.GL_MAX_PIXEL_MAP_TABLE
GL_MAX_PROJECTION_STACK_DEPTH = glcanvasc.GL_MAX_PROJECTION_STACK_DEPTH
GL_MAX_TEXTURE_SIZE = glcanvasc.GL_MAX_TEXTURE_SIZE
GL_MAX_TEXTURE_STACK_DEPTH = glcanvasc.GL_MAX_TEXTURE_STACK_DEPTH
GL_MAX_VIEWPORT_DIMS = glcanvasc.GL_MAX_VIEWPORT_DIMS
GL_MODELVIEW = glcanvasc.GL_MODELVIEW
GL_MODELVIEW_MATRIX = glcanvasc.GL_MODELVIEW_MATRIX
GL_MODELVIEW_STACK_DEPTH = glcanvasc.GL_MODELVIEW_STACK_DEPTH
GL_MODULATE = glcanvasc.GL_MODULATE
GL_MULT = glcanvasc.GL_MULT
GL_N3F_V3F = glcanvasc.GL_N3F_V3F
GL_NAME_STACK_DEPTH = glcanvasc.GL_NAME_STACK_DEPTH
GL_NAND = glcanvasc.GL_NAND
GL_NEAREST = glcanvasc.GL_NEAREST
GL_NEAREST_MIPMAP_LINEAR = glcanvasc.GL_NEAREST_MIPMAP_LINEAR
GL_NEAREST_MIPMAP_NEAREST = glcanvasc.GL_NEAREST_MIPMAP_NEAREST
GL_NEVER = glcanvasc.GL_NEVER
GL_NICEST = glcanvasc.GL_NICEST
GL_NONE = glcanvasc.GL_NONE
GL_NOOP = glcanvasc.GL_NOOP
GL_NOR = glcanvasc.GL_NOR
GL_NORMALIZE = glcanvasc.GL_NORMALIZE
GL_NORMAL_ARRAY = glcanvasc.GL_NORMAL_ARRAY
GL_NORMAL_ARRAY_COUNT_EXT = glcanvasc.GL_NORMAL_ARRAY_COUNT_EXT
GL_NORMAL_ARRAY_EXT = glcanvasc.GL_NORMAL_ARRAY_EXT
GL_NORMAL_ARRAY_POINTER = glcanvasc.GL_NORMAL_ARRAY_POINTER
GL_NORMAL_ARRAY_POINTER_EXT = glcanvasc.GL_NORMAL_ARRAY_POINTER_EXT
GL_NORMAL_ARRAY_STRIDE = glcanvasc.GL_NORMAL_ARRAY_STRIDE
GL_NORMAL_ARRAY_STRIDE_EXT = glcanvasc.GL_NORMAL_ARRAY_STRIDE_EXT
GL_NORMAL_ARRAY_TYPE = glcanvasc.GL_NORMAL_ARRAY_TYPE
GL_NORMAL_ARRAY_TYPE_EXT = glcanvasc.GL_NORMAL_ARRAY_TYPE_EXT
GL_NOTEQUAL = glcanvasc.GL_NOTEQUAL
GL_NO_ERROR = glcanvasc.GL_NO_ERROR
GL_OBJECT_LINEAR = glcanvasc.GL_OBJECT_LINEAR
GL_OBJECT_PLANE = glcanvasc.GL_OBJECT_PLANE
GL_ONE = glcanvasc.GL_ONE
GL_ONE_MINUS_DST_ALPHA = glcanvasc.GL_ONE_MINUS_DST_ALPHA
GL_ONE_MINUS_DST_COLOR = glcanvasc.GL_ONE_MINUS_DST_COLOR
GL_ONE_MINUS_SRC_ALPHA = glcanvasc.GL_ONE_MINUS_SRC_ALPHA
GL_ONE_MINUS_SRC_COLOR = glcanvasc.GL_ONE_MINUS_SRC_COLOR
GL_OR = glcanvasc.GL_OR
GL_ORDER = glcanvasc.GL_ORDER
GL_OR_INVERTED = glcanvasc.GL_OR_INVERTED
GL_OR_REVERSE = glcanvasc.GL_OR_REVERSE
GL_OUT_OF_MEMORY = glcanvasc.GL_OUT_OF_MEMORY
GL_PACK_ALIGNMENT = glcanvasc.GL_PACK_ALIGNMENT
GL_PACK_LSB_FIRST = glcanvasc.GL_PACK_LSB_FIRST
GL_PACK_ROW_LENGTH = glcanvasc.GL_PACK_ROW_LENGTH
GL_PACK_SKIP_PIXELS = glcanvasc.GL_PACK_SKIP_PIXELS
GL_PACK_SKIP_ROWS = glcanvasc.GL_PACK_SKIP_ROWS
GL_PACK_SWAP_BYTES = glcanvasc.GL_PACK_SWAP_BYTES
GL_PASS_THROUGH_TOKEN = glcanvasc.GL_PASS_THROUGH_TOKEN
GL_PERSPECTIVE_CORRECTION_HINT = glcanvasc.GL_PERSPECTIVE_CORRECTION_HINT
GL_PIXEL_MAP_A_TO_A = glcanvasc.GL_PIXEL_MAP_A_TO_A
GL_PIXEL_MAP_A_TO_A_SIZE = glcanvasc.GL_PIXEL_MAP_A_TO_A_SIZE
GL_PIXEL_MAP_B_TO_B = glcanvasc.GL_PIXEL_MAP_B_TO_B
GL_PIXEL_MAP_B_TO_B_SIZE = glcanvasc.GL_PIXEL_MAP_B_TO_B_SIZE
GL_PIXEL_MAP_G_TO_G = glcanvasc.GL_PIXEL_MAP_G_TO_G
GL_PIXEL_MAP_G_TO_G_SIZE = glcanvasc.GL_PIXEL_MAP_G_TO_G_SIZE
GL_PIXEL_MAP_I_TO_A = glcanvasc.GL_PIXEL_MAP_I_TO_A
GL_PIXEL_MAP_I_TO_A_SIZE = glcanvasc.GL_PIXEL_MAP_I_TO_A_SIZE
GL_PIXEL_MAP_I_TO_B = glcanvasc.GL_PIXEL_MAP_I_TO_B
GL_PIXEL_MAP_I_TO_B_SIZE = glcanvasc.GL_PIXEL_MAP_I_TO_B_SIZE
GL_PIXEL_MAP_I_TO_G = glcanvasc.GL_PIXEL_MAP_I_TO_G
GL_PIXEL_MAP_I_TO_G_SIZE = glcanvasc.GL_PIXEL_MAP_I_TO_G_SIZE
GL_PIXEL_MAP_I_TO_I = glcanvasc.GL_PIXEL_MAP_I_TO_I
GL_PIXEL_MAP_I_TO_I_SIZE = glcanvasc.GL_PIXEL_MAP_I_TO_I_SIZE
GL_PIXEL_MAP_I_TO_R = glcanvasc.GL_PIXEL_MAP_I_TO_R
GL_PIXEL_MAP_I_TO_R_SIZE = glcanvasc.GL_PIXEL_MAP_I_TO_R_SIZE
GL_PIXEL_MAP_R_TO_R = glcanvasc.GL_PIXEL_MAP_R_TO_R
GL_PIXEL_MAP_R_TO_R_SIZE = glcanvasc.GL_PIXEL_MAP_R_TO_R_SIZE
GL_PIXEL_MAP_S_TO_S = glcanvasc.GL_PIXEL_MAP_S_TO_S
GL_PIXEL_MAP_S_TO_S_SIZE = glcanvasc.GL_PIXEL_MAP_S_TO_S_SIZE
GL_PIXEL_MODE_BIT = glcanvasc.GL_PIXEL_MODE_BIT
GL_POINT = glcanvasc.GL_POINT
GL_POINTS = glcanvasc.GL_POINTS
GL_POINT_BIT = glcanvasc.GL_POINT_BIT
GL_POINT_SIZE = glcanvasc.GL_POINT_SIZE
GL_POINT_SIZE_GRANULARITY = glcanvasc.GL_POINT_SIZE_GRANULARITY
GL_POINT_SIZE_RANGE = glcanvasc.GL_POINT_SIZE_RANGE
GL_POINT_SMOOTH = glcanvasc.GL_POINT_SMOOTH
GL_POINT_SMOOTH_HINT = glcanvasc.GL_POINT_SMOOTH_HINT
GL_POINT_TOKEN = glcanvasc.GL_POINT_TOKEN
GL_POLYGON = glcanvasc.GL_POLYGON
GL_POLYGON_BIT = glcanvasc.GL_POLYGON_BIT
GL_POLYGON_MODE = glcanvasc.GL_POLYGON_MODE
GL_POLYGON_OFFSET_FACTOR = glcanvasc.GL_POLYGON_OFFSET_FACTOR
GL_POLYGON_OFFSET_FILL = glcanvasc.GL_POLYGON_OFFSET_FILL
GL_POLYGON_OFFSET_LINE = glcanvasc.GL_POLYGON_OFFSET_LINE
GL_POLYGON_OFFSET_POINT = glcanvasc.GL_POLYGON_OFFSET_POINT
GL_POLYGON_OFFSET_UNITS = glcanvasc.GL_POLYGON_OFFSET_UNITS
GL_POLYGON_SMOOTH = glcanvasc.GL_POLYGON_SMOOTH
GL_POLYGON_SMOOTH_HINT = glcanvasc.GL_POLYGON_SMOOTH_HINT
GL_POLYGON_STIPPLE = glcanvasc.GL_POLYGON_STIPPLE
GL_POLYGON_STIPPLE_BIT = glcanvasc.GL_POLYGON_STIPPLE_BIT
GL_POLYGON_TOKEN = glcanvasc.GL_POLYGON_TOKEN
GL_POSITION = glcanvasc.GL_POSITION
GL_PROJECTION = glcanvasc.GL_PROJECTION
GL_PROJECTION_MATRIX = glcanvasc.GL_PROJECTION_MATRIX
GL_PROJECTION_STACK_DEPTH = glcanvasc.GL_PROJECTION_STACK_DEPTH
GL_PROXY_TEXTURE_1D = glcanvasc.GL_PROXY_TEXTURE_1D
GL_PROXY_TEXTURE_2D = glcanvasc.GL_PROXY_TEXTURE_2D
GL_Q = glcanvasc.GL_Q
GL_QUADRATIC_ATTENUATION = glcanvasc.GL_QUADRATIC_ATTENUATION
GL_QUADS = glcanvasc.GL_QUADS
GL_QUAD_STRIP = glcanvasc.GL_QUAD_STRIP
GL_R = glcanvasc.GL_R
GL_R3_G3_B2 = glcanvasc.GL_R3_G3_B2
GL_READ_BUFFER = glcanvasc.GL_READ_BUFFER
GL_RED = glcanvasc.GL_RED
GL_RED_BIAS = glcanvasc.GL_RED_BIAS
GL_RED_BITS = glcanvasc.GL_RED_BITS
GL_RED_SCALE = glcanvasc.GL_RED_SCALE
GL_RENDER = glcanvasc.GL_RENDER
GL_RENDERER = glcanvasc.GL_RENDERER
GL_RENDER_MODE = glcanvasc.GL_RENDER_MODE
GL_REPEAT = glcanvasc.GL_REPEAT
GL_REPLACE = glcanvasc.GL_REPLACE
GL_RETURN = glcanvasc.GL_RETURN
GL_RGB = glcanvasc.GL_RGB
GL_RGB10 = glcanvasc.GL_RGB10
GL_RGB10_A2 = glcanvasc.GL_RGB10_A2
GL_RGB12 = glcanvasc.GL_RGB12
GL_RGB16 = glcanvasc.GL_RGB16
GL_RGB4 = glcanvasc.GL_RGB4
GL_RGB5 = glcanvasc.GL_RGB5
GL_RGB5_A1 = glcanvasc.GL_RGB5_A1
GL_RGB8 = glcanvasc.GL_RGB8
GL_RGBA = glcanvasc.GL_RGBA
GL_RGBA12 = glcanvasc.GL_RGBA12
GL_RGBA16 = glcanvasc.GL_RGBA16
GL_RGBA2 = glcanvasc.GL_RGBA2
GL_RGBA4 = glcanvasc.GL_RGBA4
GL_RGBA8 = glcanvasc.GL_RGBA8
GL_RGBA_MODE = glcanvasc.GL_RGBA_MODE
GL_RIGHT = glcanvasc.GL_RIGHT
GL_S = glcanvasc.GL_S
GL_SCISSOR_BIT = glcanvasc.GL_SCISSOR_BIT
GL_SCISSOR_BOX = glcanvasc.GL_SCISSOR_BOX
GL_SCISSOR_TEST = glcanvasc.GL_SCISSOR_TEST
GL_SELECT = glcanvasc.GL_SELECT
GL_SELECTION_BUFFER_POINTER = glcanvasc.GL_SELECTION_BUFFER_POINTER
GL_SELECTION_BUFFER_SIZE = glcanvasc.GL_SELECTION_BUFFER_SIZE
GL_SET = glcanvasc.GL_SET
GL_SHADE_MODEL = glcanvasc.GL_SHADE_MODEL
GL_SHININESS = glcanvasc.GL_SHININESS
GL_SHORT = glcanvasc.GL_SHORT
GL_SMOOTH = glcanvasc.GL_SMOOTH
GL_SPECULAR = glcanvasc.GL_SPECULAR
GL_SPHERE_MAP = glcanvasc.GL_SPHERE_MAP
GL_SPOT_CUTOFF = glcanvasc.GL_SPOT_CUTOFF
GL_SPOT_DIRECTION = glcanvasc.GL_SPOT_DIRECTION
GL_SPOT_EXPONENT = glcanvasc.GL_SPOT_EXPONENT
GL_SRC_ALPHA = glcanvasc.GL_SRC_ALPHA
GL_SRC_ALPHA_SATURATE = glcanvasc.GL_SRC_ALPHA_SATURATE
GL_SRC_COLOR = glcanvasc.GL_SRC_COLOR
GL_STACK_OVERFLOW = glcanvasc.GL_STACK_OVERFLOW
GL_STACK_UNDERFLOW = glcanvasc.GL_STACK_UNDERFLOW
GL_STENCIL = glcanvasc.GL_STENCIL
GL_STENCIL_BITS = glcanvasc.GL_STENCIL_BITS
GL_STENCIL_BUFFER_BIT = glcanvasc.GL_STENCIL_BUFFER_BIT
GL_STENCIL_CLEAR_VALUE = glcanvasc.GL_STENCIL_CLEAR_VALUE
GL_STENCIL_FAIL = glcanvasc.GL_STENCIL_FAIL
GL_STENCIL_FUNC = glcanvasc.GL_STENCIL_FUNC
GL_STENCIL_INDEX = glcanvasc.GL_STENCIL_INDEX
GL_STENCIL_PASS_DEPTH_FAIL = glcanvasc.GL_STENCIL_PASS_DEPTH_FAIL
GL_STENCIL_PASS_DEPTH_PASS = glcanvasc.GL_STENCIL_PASS_DEPTH_PASS
GL_STENCIL_REF = glcanvasc.GL_STENCIL_REF
GL_STENCIL_TEST = glcanvasc.GL_STENCIL_TEST
GL_STENCIL_VALUE_MASK = glcanvasc.GL_STENCIL_VALUE_MASK
GL_STENCIL_WRITEMASK = glcanvasc.GL_STENCIL_WRITEMASK
GL_STEREO = glcanvasc.GL_STEREO
GL_SUBPIXEL_BITS = glcanvasc.GL_SUBPIXEL_BITS
GL_T = glcanvasc.GL_T
GL_T2F_C3F_V3F = glcanvasc.GL_T2F_C3F_V3F
GL_T2F_C4F_N3F_V3F = glcanvasc.GL_T2F_C4F_N3F_V3F
GL_T2F_C4UB_V3F = glcanvasc.GL_T2F_C4UB_V3F
GL_T2F_N3F_V3F = glcanvasc.GL_T2F_N3F_V3F
GL_T2F_V3F = glcanvasc.GL_T2F_V3F
GL_T4F_C4F_N3F_V4F = glcanvasc.GL_T4F_C4F_N3F_V4F
GL_T4F_V4F = glcanvasc.GL_T4F_V4F
GL_TEXTURE = glcanvasc.GL_TEXTURE
GL_TEXTURE_1D = glcanvasc.GL_TEXTURE_1D
GL_TEXTURE_2D = glcanvasc.GL_TEXTURE_2D
GL_TEXTURE_ALPHA_SIZE = glcanvasc.GL_TEXTURE_ALPHA_SIZE
GL_TEXTURE_BINDING_1D = glcanvasc.GL_TEXTURE_BINDING_1D
GL_TEXTURE_BINDING_2D = glcanvasc.GL_TEXTURE_BINDING_2D
GL_TEXTURE_BIT = glcanvasc.GL_TEXTURE_BIT
GL_TEXTURE_BLUE_SIZE = glcanvasc.GL_TEXTURE_BLUE_SIZE
GL_TEXTURE_BORDER = glcanvasc.GL_TEXTURE_BORDER
GL_TEXTURE_BORDER_COLOR = glcanvasc.GL_TEXTURE_BORDER_COLOR
GL_TEXTURE_COMPONENTS = glcanvasc.GL_TEXTURE_COMPONENTS
GL_TEXTURE_COORD_ARRAY = glcanvasc.GL_TEXTURE_COORD_ARRAY
GL_TEXTURE_COORD_ARRAY_COUNT_EXT = glcanvasc.GL_TEXTURE_COORD_ARRAY_COUNT_EXT
GL_TEXTURE_COORD_ARRAY_EXT = glcanvasc.GL_TEXTURE_COORD_ARRAY_EXT
GL_TEXTURE_COORD_ARRAY_POINTER = glcanvasc.GL_TEXTURE_COORD_ARRAY_POINTER
GL_TEXTURE_COORD_ARRAY_POINTER_EXT = glcanvasc.GL_TEXTURE_COORD_ARRAY_POINTER_EXT
GL_TEXTURE_COORD_ARRAY_SIZE = glcanvasc.GL_TEXTURE_COORD_ARRAY_SIZE
GL_TEXTURE_COORD_ARRAY_SIZE_EXT = glcanvasc.GL_TEXTURE_COORD_ARRAY_SIZE_EXT
GL_TEXTURE_COORD_ARRAY_STRIDE = glcanvasc.GL_TEXTURE_COORD_ARRAY_STRIDE
GL_TEXTURE_COORD_ARRAY_STRIDE_EXT = glcanvasc.GL_TEXTURE_COORD_ARRAY_STRIDE_EXT
GL_TEXTURE_COORD_ARRAY_TYPE = glcanvasc.GL_TEXTURE_COORD_ARRAY_TYPE
GL_TEXTURE_COORD_ARRAY_TYPE_EXT = glcanvasc.GL_TEXTURE_COORD_ARRAY_TYPE_EXT
GL_TEXTURE_ENV = glcanvasc.GL_TEXTURE_ENV
GL_TEXTURE_ENV_COLOR = glcanvasc.GL_TEXTURE_ENV_COLOR
GL_TEXTURE_ENV_MODE = glcanvasc.GL_TEXTURE_ENV_MODE
GL_TEXTURE_GEN_MODE = glcanvasc.GL_TEXTURE_GEN_MODE
GL_TEXTURE_GEN_Q = glcanvasc.GL_TEXTURE_GEN_Q
GL_TEXTURE_GEN_R = glcanvasc.GL_TEXTURE_GEN_R
GL_TEXTURE_GEN_S = glcanvasc.GL_TEXTURE_GEN_S
GL_TEXTURE_GEN_T = glcanvasc.GL_TEXTURE_GEN_T
GL_TEXTURE_GREEN_SIZE = glcanvasc.GL_TEXTURE_GREEN_SIZE
GL_TEXTURE_HEIGHT = glcanvasc.GL_TEXTURE_HEIGHT
GL_TEXTURE_INTENSITY_SIZE = glcanvasc.GL_TEXTURE_INTENSITY_SIZE
GL_TEXTURE_INTERNAL_FORMAT = glcanvasc.GL_TEXTURE_INTERNAL_FORMAT
GL_TEXTURE_LUMINANCE_SIZE = glcanvasc.GL_TEXTURE_LUMINANCE_SIZE
GL_TEXTURE_MAG_FILTER = glcanvasc.GL_TEXTURE_MAG_FILTER
GL_TEXTURE_MATRIX = glcanvasc.GL_TEXTURE_MATRIX
GL_TEXTURE_MIN_FILTER = glcanvasc.GL_TEXTURE_MIN_FILTER
GL_TEXTURE_PRIORITY = glcanvasc.GL_TEXTURE_PRIORITY
GL_TEXTURE_RED_SIZE = glcanvasc.GL_TEXTURE_RED_SIZE
GL_TEXTURE_RESIDENT = glcanvasc.GL_TEXTURE_RESIDENT
GL_TEXTURE_STACK_DEPTH = glcanvasc.GL_TEXTURE_STACK_DEPTH
GL_TEXTURE_WIDTH = glcanvasc.GL_TEXTURE_WIDTH
GL_TEXTURE_WRAP_S = glcanvasc.GL_TEXTURE_WRAP_S
GL_TEXTURE_WRAP_T = glcanvasc.GL_TEXTURE_WRAP_T
GL_TRANSFORM_BIT = glcanvasc.GL_TRANSFORM_BIT
GL_TRIANGLES = glcanvasc.GL_TRIANGLES
GL_TRIANGLE_FAN = glcanvasc.GL_TRIANGLE_FAN
GL_TRIANGLE_STRIP = glcanvasc.GL_TRIANGLE_STRIP
GL_TRUE = glcanvasc.GL_TRUE
GL_UNPACK_ALIGNMENT = glcanvasc.GL_UNPACK_ALIGNMENT
GL_UNPACK_LSB_FIRST = glcanvasc.GL_UNPACK_LSB_FIRST
GL_UNPACK_ROW_LENGTH = glcanvasc.GL_UNPACK_ROW_LENGTH
GL_UNPACK_SKIP_PIXELS = glcanvasc.GL_UNPACK_SKIP_PIXELS
GL_UNPACK_SKIP_ROWS = glcanvasc.GL_UNPACK_SKIP_ROWS
GL_UNPACK_SWAP_BYTES = glcanvasc.GL_UNPACK_SWAP_BYTES
GL_UNSIGNED_BYTE = glcanvasc.GL_UNSIGNED_BYTE
GL_UNSIGNED_INT = glcanvasc.GL_UNSIGNED_INT
GL_UNSIGNED_SHORT = glcanvasc.GL_UNSIGNED_SHORT
GL_V2F = glcanvasc.GL_V2F
GL_V3F = glcanvasc.GL_V3F
GL_VENDOR = glcanvasc.GL_VENDOR
GL_VERSION = glcanvasc.GL_VERSION
GL_VERTEX_ARRAY = glcanvasc.GL_VERTEX_ARRAY
GL_VERTEX_ARRAY_COUNT_EXT = glcanvasc.GL_VERTEX_ARRAY_COUNT_EXT
GL_VERTEX_ARRAY_EXT = glcanvasc.GL_VERTEX_ARRAY_EXT
GL_VERTEX_ARRAY_POINTER = glcanvasc.GL_VERTEX_ARRAY_POINTER
GL_VERTEX_ARRAY_POINTER_EXT = glcanvasc.GL_VERTEX_ARRAY_POINTER_EXT
GL_VERTEX_ARRAY_SIZE = glcanvasc.GL_VERTEX_ARRAY_SIZE
GL_VERTEX_ARRAY_SIZE_EXT = glcanvasc.GL_VERTEX_ARRAY_SIZE_EXT
GL_VERTEX_ARRAY_STRIDE = glcanvasc.GL_VERTEX_ARRAY_STRIDE
GL_VERTEX_ARRAY_STRIDE_EXT = glcanvasc.GL_VERTEX_ARRAY_STRIDE_EXT
GL_VERTEX_ARRAY_TYPE = glcanvasc.GL_VERTEX_ARRAY_TYPE
GL_VERTEX_ARRAY_TYPE_EXT = glcanvasc.GL_VERTEX_ARRAY_TYPE_EXT
GL_VIEWPORT = glcanvasc.GL_VIEWPORT
GL_VIEWPORT_BIT = glcanvasc.GL_VIEWPORT_BIT
GL_XOR = glcanvasc.GL_XOR
GL_ZERO = glcanvasc.GL_ZERO
GL_ZOOM_X = glcanvasc.GL_ZOOM_X
GL_ZOOM_Y = glcanvasc.GL_ZOOM_Y
