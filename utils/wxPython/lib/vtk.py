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

class wxVTKRenderWindow(wxWindow):
    def __init__(self, parent, id, position=wxDefaultPosition,
                 size=wxDefaultSize, style=0):
        wxWindow.__init__(self, parent, id, position, size, style)

        self.renderWindow = vtkRenderWindow()

        hdl = self.GetHandle()
        self.renderWindow.SetWindowInfo(str(hdl))

        EVT_LEFT_DOWN  (self, self.SaveClick)
        EVT_MIDDLE_DOWN(self, self.SaveClick)
        EVT_RIGHT_DOWN (self, self.SaveClick)
        EVT_LEFT_UP    (self, self.Release)
        EVT_MIDDLE_UP  (self, self.Release)
        EVT_RIGHT_UP   (self, self.Release)
        EVT_MOTION(self, self.MouseMotion)


    def GetRenderer(self):
        self.renderWindow.GetRenderers().InitTraversal()
        return self.renderWindow.GetRenderers().GetNextItem()


    def GetRenderWindow(self):
        return self.renderWindow


    def OnPaint(self, event):
        dc = wxPaintDC(self)
        self.renderWindow.Render()


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



## testcode is now in the demo ##

## if __name__ == '__main__':
##     class TestFrame(wxFrame):
##         def __init__(self, parent):
##             wxFrame.__init__(self, parent, -1, "VTK Test", size=(450,450))

##             rw = wxVTKRenderWindow(self, -1)

##             # Get the render window
##             renWin = rw.GetRenderWindow()

##             # Next, do the VTK stuff
##             ren = vtkRenderer()
##             renWin.AddRenderer(ren)
##             cone = vtkConeSource()
##             cone.SetResolution(80)
##             coneMapper = vtkPolyDataMapper()
##             coneMapper.SetInput(cone.GetOutput())
##             coneActor = vtkActor()
##             coneActor.SetMapper(coneMapper)
##             ren.AddActor(coneActor)
##             coneMapper.GetLookupTable().Build()

##             # Create a scalar bar
##             scalarBar = vtkScalarBarActor()
##             scalarBar.SetLookupTable(coneMapper.GetLookupTable())
##             scalarBar.SetTitle("Temperature")
##             scalarBar.GetPositionCoordinate().SetCoordinateSystemToNormalizedViewport()
##             scalarBar.GetPositionCoordinate().SetValue(0.1, 0.01)
##             scalarBar.SetOrientationToHorizontal()
##             scalarBar.SetWidth(0.8)
##             scalarBar.SetHeight(0.17)
##             ren.AddActor2D(scalarBar)



##     class TestApp(wxApp):
##         def OnInit(self):
##             f = TestFrame(None)
##             self.SetTopWindow(f)
##             f.Show(true)
##             return true

##     app = TestApp(0)
##     app.MainLoop()

