#----------------------------------------------------------------------
# Name:        wxPython.lib.vtk
# Purpose:     Provides a wrapper around the vtkRenderWindow from the
#              VTK Visualization Toolkit.  Requires the VTK Python
#              extensions from http://www.kitware.com/
#
# Author:      Robin Dunn
#
# Created:     16-Nov-1999
# RCS-ID:      $Id$
# Copyright:   (c) 1999 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------


try:  # For Win32,
    from vtkpython import *
except ImportError:
    try:  # for Unix.  Why they do it this way is anybody's guess...
        from libVTKCommonPython import *
        from libVTKGraphicsPython import *
        from libVTKImagingPython import *
    except ImportError:
        raise ImportError, "VTK extension module not found"

from wxPython.wx import *
import math

#----------------------------------------------------------------------

class wxVTKRenderWindow(wxScrolledWindow):
    def __init__(self, parent, id, position=wxDefaultPosition,
                 size=wxDefaultSize, style=0):
        wxScrolledWindow.__init__(self, parent, id, position, size, style)

        self.renderWindow = vtkRenderWindow()

        if wxPlatform != '__WXMSW__':
            # We can't get the handle in wxGTK until after the widget
            # is created, the window create event happens later so we'll
            # catch it there
            EVT_WINDOW_CREATE(self, self.OnCreateWindow)
        else:
            # but in MSW, the window create event happens durring the above
            # call to __init__ so we have to do it here.
            hdl = self.GetHandle()
            self.renderWindow.SetWindowInfo(str(hdl))


        EVT_LEFT_DOWN  (self, self.SaveClick)
        EVT_MIDDLE_DOWN(self, self.SaveClick)
        EVT_RIGHT_DOWN (self, self.SaveClick)
        EVT_LEFT_UP    (self, self.Release)
        EVT_MIDDLE_UP  (self, self.Release)
        EVT_RIGHT_UP   (self, self.Release)
        EVT_MOTION     (self, self.MouseMotion)

        EVT_ERASE_BACKGROUND(self, self.OnEraseBackground)
        EVT_PAINT(self, self.OnPaint)


    def GetRenderer(self):
        self.renderWindow.GetRenderers().InitTraversal()
        return self.renderWindow.GetRenderers().GetNextItem()


    def GetRenderWindow(self):
        return self.renderWindow


    def OnPaint(self, event):
        dc = wxPaintDC(self)
        self.renderWindow.Render()

    def OnCreateWindow(self, event):
        hdl = self.GetHandle()
        self.renderWindow.SetWindowInfo(str(hdl))

    def OnEraseBackground(self, event):
        pass



    def SaveClick(self, event):
        self.prev_x, self.prev_y = event.GetPositionTuple()
        self.CaptureMouse()

    def Release(self, event):
        self.ReleaseMouse()

    def MouseMotion(self, event):
        event.x, event.y = event.GetPositionTuple()
        size = self.GetClientSize()
        if event.LeftIsDown():
            # rotate
            camera = self.GetRenderer().GetActiveCamera()
            camera.Azimuth(float(self.prev_x - event.x) / size.width * 360)
            camera.Elevation(float(event.y - self.prev_y) / size.width * 360)
            camera.OrthogonalizeViewUp()
            self.MotionUpdate(event)

        elif event.MiddleIsDown():
            # pan
            camera = self.GetRenderer().GetActiveCamera()
            camera.Yaw(-float(self.prev_x - event.x) / size.width * 30)
            camera.Pitch(float(event.y - self.prev_y) / size.width * 30)
            camera.OrthogonalizeViewUp()
            self.MotionUpdate(event)

        elif event.RightIsDown():
            # dolly
            camera = self.GetRenderer().GetActiveCamera()
            camera.Dolly(math.exp(float((event.x - self.prev_x) - \
                                        (event.y - self.prev_y))/ \
                                  size.width))
            self.MotionUpdate(event)


    def MotionUpdate(self,event):
        renderer = self.GetRenderer()
        renderer.GetLights().InitTraversal()
        light = renderer.GetLights().GetNextItem()
        camera = renderer.GetActiveCamera()
        light.SetPosition(camera.GetPosition())
        light.SetFocalPoint(camera.GetFocalPoint())
        self.renderWindow.Render()
        self.prev_x = event.x
        self.prev_y = event.y



