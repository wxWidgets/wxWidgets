"""
VTK is now including a package for using VTK with wxPython, so this
module is now officially nothing but ancient history.  If for some
strange reason you really need this code (I don't know why, it didn't
work all that well anyway,) then just remove the triple quotes below.
I'm told that the module from Kitware is excellent and so you should
really use it.  See the URL below to get a copy from CVS.

http://public.kitware.com/cgi-bin/cvsweb.cgi/VTK/Wrapping/Python/vtk/wx/
"""

print __doc__

'''
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

# This class has been rewritten and improved by Prabhu Ramachandran
# <prabhu@aero.iitm.ernet.in>.  It has been tested under Win32 and
# Linux.  Many thanks to Eric Boix <frog@creatis.insa-lyon.fr> for
# testing it under Windows and finding and fixing many errors.
# Thanks also to Sebastien BARRE <sebastien@barre.nom.fr> for his
# suggestions.


try:
    from vtkpython import *
except ImportError:
    raise ImportError, "VTK extension module not found"

from wxPython.wx import *
import math

#----------------------------------------------------------------------

DEBUG = 0

def debug(msg):
    if DEBUG:
        print msg


class wxVTKRenderWindowBase(wxWindow):
    """
    A base class that enables one to embed a vtkRenderWindow into
    a wxPython widget.  This class embeds the RenderWindow correctly
    under different platforms.  Provided are some empty methods that
    can be overloaded to provide a user defined interaction behaviour.
    The event handling functions have names that are similar to the
    ones in the vtkInteractorStyle class included with VTK.
    """

    def __init__(self, parent, id, position=wxDefaultPosition,
                 size=wxDefaultSize, style=0):
        wxWindow.__init__(self, parent, id, position, size, style | wxWANTS_CHARS)
        self._RenderWindow = vtkRenderWindow()

        self.__InExpose = 0
        self.__Created = 0

        if wxPlatform != '__WXMSW__':
            # We can't get the handle in wxGTK until after the widget
            # is created, the window create event happens later so we'll
            # catch it there
            EVT_WINDOW_CREATE(self, self.OnCreateWindow)
            EVT_PAINT       (self, self.OnExpose)
        else:
            # but in MSW, the window create event happens durring the above
            # call to __init__ so we have to do it here.
            hdl = self.GetHandle()
            self._RenderWindow.SetWindowInfo(str(hdl))
            EVT_PAINT       (self, self.OnExpose)
            self.__Created = 1

        # common for all platforms
        EVT_SIZE        (self, self.OnConfigure)

        # setup the user defined events.
        self.SetupEvents()


    def SetupEvents(self):
        "Setup the user defined event bindings."
        # Remember to bind everything to self.box and NOT self
        EVT_LEFT_DOWN    (self, self.OnLeftButtonDown)
        EVT_MIDDLE_DOWN  (self, self.OnMiddleButtonDown)
        EVT_RIGHT_DOWN   (self, self.OnRightButtonDown)
        EVT_LEFT_UP      (self, self.OnLeftButtonUp)
        EVT_MIDDLE_UP    (self, self.OnMiddleButtonUp)
        EVT_RIGHT_UP     (self, self.OnRightButtonUp)
        EVT_MOTION       (self, self.OnMouseMove)
        EVT_ENTER_WINDOW (self, self.OnEnter)
        EVT_LEAVE_WINDOW (self, self.OnLeave)
        EVT_CHAR         (self, self.OnChar)
        # Add your bindings if you want them in the derived class.


    def GetRenderer(self):
        self._RenderWindow.GetRenderers().InitTraversal()
        return self._RenderWindow.GetRenderers().GetNextItem()


    def GetRenderWindow(self):
        return self._RenderWindow


    def Render(self):
        if self.__Created:
            # if block needed because calls to render before creation
            # will prevent the renderwindow from being embedded into a
            # wxPython widget.
            self._RenderWindow.Render()


    def OnExpose(self, event):
        # I need this for the MDIDemo.  Somehow OnCreateWindow was
        # not getting called.
        if not self.__Created:
            self.OnCreateWindow(event)
        if (not self.__InExpose):
            self.__InExpose = 1
            dc = wxPaintDC(self)
            self._RenderWindow.Render()
            self.__InExpose = 0


    def OnCreateWindow(self, event):
        hdl = self.GetHandle()
        try:
            self._RenderWindow.SetParentInfo(str(hdl))
        except:
            self._RenderWindow.SetWindowInfo(str(hdl))
            msg = "Warning:\n "\
                  "Unable to call vtkRenderWindow.SetParentInfo\n\n"\
                  "Using the SetWindowInfo method instead.  This\n"\
                  "is likely to cause a lot of flicker when\n"\
                  "rendering in the vtkRenderWindow.  Please\n"\
                  "use a recent Nightly VTK release (later than\n"\
                  "March 10 2001) to eliminate this problem."
            dlg = wxMessageDialog(NULL, msg, "Warning!",
                                   wxOK |wxICON_INFORMATION)
            dlg.ShowModal()
            dlg.Destroy()
        self.__Created = 1


    def OnConfigure(self, event):
        sz = self.GetSize()
        self.SetSize(sz)
        # Ugly hack that according to Eric Boix is necessary under
        # Windows.  If possible Try commenting this out and test under
        # Windows.
        #self._RenderWindow.GetSize()
        #
        self._RenderWindow.SetSize(sz.width, sz.height)


    def OnLeftButtonDown(self, event):
        "Left mouse button pressed."
        pass


    def OnMiddleButtonDown(self, event):
        "Middle mouse button pressed."
        pass


    def OnRightButtonDown(self, event):
        "Right mouse button pressed."
        pass


    def OnLeftButtonUp(self, event):
        "Left mouse button released."
        pass


    def OnMiddleButtonUp(self, event):
        "Middle mouse button released."
        pass


    def OnRightButtonUp(self, event):
        "Right mouse button released."
        pass


    def OnMouseMove(self, event):
        "Mouse has moved."
        pass


    def OnEnter(self, event):
        "Entering the vtkRenderWindow."
        pass


    def OnLeave(self, event):
        "Leaving the vtkRenderWindow."
        pass


    def OnChar(self, event):
        "Process Key events."
        pass


    def OnKeyDown(self, event):
        "Key pressed down."
        pass


    def OnKeyUp(self, event):
        "Key released."
        pass




class wxVTKRenderWindow(wxVTKRenderWindowBase):
    """
    An example of a fully functional wxVTKRenderWindow that is
    based on the vtkRenderWidget.py provided with the VTK sources.
    """

    def __init__(self, parent, id, position=wxDefaultPosition,
                 size=wxDefaultSize, style=0):
        wxVTKRenderWindowBase.__init__(self, parent, id, position, size,
                                       style)

        self._CurrentRenderer = None
        self._CurrentCamera = None
        self._CurrentZoom = 1.0
        self._CurrentLight = None

        self._ViewportCenterX = 0
        self._ViewportCenterY = 0

        self._Picker = vtkCellPicker()
        self._PickedAssembly = None
        self._PickedProperty = vtkProperty()
        self._PickedProperty.SetColor(1,0,0)
        self._PrePickedProperty = None

        self._OldFocus = None

        # these record the previous mouse position
        self._LastX = 0
        self._LastY = 0


    def OnLeftButtonDown(self, event):
        "Left mouse button pressed."
        self.StartMotion(event)


    def OnMiddleButtonDown(self, event):
        "Middle mouse button pressed."
        self.StartMotion(event)


    def OnRightButtonDown(self, event):
        "Right mouse button pressed."
        self.StartMotion(event)


    def OnLeftButtonUp(self, event):
        "Left mouse button released."
        self.EndMotion(event)


    def OnMiddleButtonUp(self, event):
        "Middle mouse button released."
        self.EndMotion(event)


    def OnRightButtonUp(self, event):
        "Right mouse button released."
        self.EndMotion(event)


    def OnMouseMove(self, event):
        event.x, event.y = event.GetPositionTuple()
        if event.LeftIsDown():
            if event.ShiftDown():
                self.Pan(event.x, event.y)
            else:
                self.Rotate(event.x, event.y)

        elif event.MiddleIsDown():
            self.Pan(event.x, event.y)

        elif event.RightIsDown():
            self.Zoom(event.x, event.y)


    def OnEnter(self, event):
        self.__OldFocus = wxWindow_FindFocus()
        self.SetFocus()
        x, y = event.GetPositionTuple()
        self.UpdateRenderer(x,y)


    def OnLeave(self, event):
        if (self._OldFocus != None):
            self.__OldFocus.SetFocus()


    def OnChar(self, event):
        key = event.GetKeyCode()
        if (key == ord('r')) or (key == ord('R')):
            self.Reset()
        elif (key == ord('w')) or (key == ord('W')):
            self.Wireframe()
        elif (key == ord('s')) or (key == ord('S')):
            self.Surface()
        elif (key == ord('p')) or (key == ord('P')):
            x, y = event.GetPositionTuple()
            self.PickActor(x, y)
        else:
            event.Skip()


    # Start of internal functions
    def GetZoomFactor(self):
        return self._CurrentZoom


    def SetZoomFactor(self, zf):
        self._CurrentZoom = zf


    def GetPicker(self):
        return self._Picker


    def Render(self):
        if (self._CurrentLight):
            light = self._CurrentLight
            light.SetPosition(self._CurrentCamera.GetPosition())
            light.SetFocalPoint(self._CurrentCamera.GetFocalPoint())

        wxVTKRenderWindowBase.Render(self)


    def UpdateRenderer(self, x, y):
        """
        UpdateRenderer will identify the renderer under the mouse and set
        up _CurrentRenderer, _CurrentCamera, and _CurrentLight.
        """
        sz = self.GetSize()
        windowX = sz.width
        windowY = sz.height

        renderers = self._RenderWindow.GetRenderers()
        numRenderers = renderers.GetNumberOfItems()

        self._CurrentRenderer = None
        renderers.InitTraversal()
        for i in range(0,numRenderers):
            renderer = renderers.GetNextItem()
            vx,vy = (0,0)
            if (windowX > 1):
                vx = float (x)/(windowX-1)
            if (windowY > 1):
                vy = (windowY-float(y)-1)/(windowY-1)
            (vpxmin,vpymin,vpxmax,vpymax) = renderer.GetViewport()

            if (vx >= vpxmin and vx <= vpxmax and
                vy >= vpymin and vy <= vpymax):
                self._CurrentRenderer = renderer
                self._ViewportCenterX = float(windowX)*(vpxmax-vpxmin)/2.0\
                                        +vpxmin
                self._ViewportCenterY = float(windowY)*(vpymax-vpymin)/2.0\
                                        +vpymin
                self._CurrentCamera = self._CurrentRenderer.GetActiveCamera()
                lights = self._CurrentRenderer.GetLights()
                lights.InitTraversal()
                self._CurrentLight = lights.GetNextItem()
                break

        self._LastX = x
        self._LastY = y


    def GetCurrentRenderer(self):
        return self._CurrentRenderer


    def StartMotion(self, event):
        x, y = event.GetPositionTuple()
        self.UpdateRenderer(x,y)
        self.CaptureMouse()


    def EndMotion(self, event=None):
        if self._CurrentRenderer:
            self.Render()
        self.ReleaseMouse()


    def Rotate(self,x,y):
        if self._CurrentRenderer:

            self._CurrentCamera.Azimuth(self._LastX - x)
            self._CurrentCamera.Elevation(y - self._LastY)
            self._CurrentCamera.OrthogonalizeViewUp()

            self._LastX = x
            self._LastY = y

            self._CurrentRenderer.ResetCameraClippingRange()
            self.Render()


    def PanAbsolute(self, x_vec, y_vec):
        if self._CurrentRenderer:

            renderer = self._CurrentRenderer
            camera = self._CurrentCamera
            (pPoint0,pPoint1,pPoint2) = camera.GetPosition()
            (fPoint0,fPoint1,fPoint2) = camera.GetFocalPoint()

            if (camera.GetParallelProjection()):
                renderer.SetWorldPoint(fPoint0,fPoint1,fPoint2,1.0)
                renderer.WorldToDisplay()
                fx,fy,fz = renderer.GetDisplayPoint()
                renderer.SetDisplayPoint(fx+x_vec,
                                         fy+y_vec,
                                         fz)
                renderer.DisplayToWorld()
                fx,fy,fz,fw = renderer.GetWorldPoint()
                camera.SetFocalPoint(fx,fy,fz)

                renderer.SetWorldPoint(pPoint0,pPoint1,pPoint2,1.0)
                renderer.WorldToDisplay()
                fx,fy,fz = renderer.GetDisplayPoint()
                renderer.SetDisplayPoint(fx+x_vec,
                                         fy+y_vec,
                                         fz)
                renderer.DisplayToWorld()
                fx,fy,fz,fw = renderer.GetWorldPoint()
                camera.SetPosition(fx,fy,fz)

            else:
                (fPoint0,fPoint1,fPoint2) = camera.GetFocalPoint()
                # Specify a point location in world coordinates
                renderer.SetWorldPoint(fPoint0,fPoint1,fPoint2,1.0)
                renderer.WorldToDisplay()
                # Convert world point coordinates to display coordinates
                dPoint = renderer.GetDisplayPoint()
                focalDepth = dPoint[2]

                aPoint0 = self._ViewportCenterX + x_vec
                aPoint1 = self._ViewportCenterY + y_vec

                renderer.SetDisplayPoint(aPoint0,aPoint1,focalDepth)
                renderer.DisplayToWorld()

                (rPoint0,rPoint1,rPoint2,rPoint3) = renderer.GetWorldPoint()
                if (rPoint3 != 0.0):
                    rPoint0 = rPoint0/rPoint3
                    rPoint1 = rPoint1/rPoint3
                    rPoint2 = rPoint2/rPoint3

                camera.SetFocalPoint((fPoint0 - rPoint0) + fPoint0,
                                     (fPoint1 - rPoint1) + fPoint1,
                                     (fPoint2 - rPoint2) + fPoint2)

                camera.SetPosition((fPoint0 - rPoint0) + pPoint0,
                                   (fPoint1 - rPoint1) + pPoint1,
                                   (fPoint2 - rPoint2) + pPoint2)

            self.Render()


    def Pan(self, x, y):
        self.PanAbsolute(x - self._LastX, - y + self._LastY)
        self._LastX = x
        self._LastY = y


    def Zoom(self,x,y):
        if self._CurrentRenderer:

            renderer = self._CurrentRenderer
            camera = self._CurrentCamera

            zoomFactor = math.pow(1.02,(0.5*(self._LastY - y)))
            self._CurrentZoom = self._CurrentZoom * zoomFactor

            if camera.GetParallelProjection():
                parallelScale = camera.GetParallelScale()/zoomFactor
                camera.SetParallelScale(parallelScale)
            else:
                camera.Dolly(zoomFactor)
                renderer.ResetCameraClippingRange()

            self._LastX = x
            self._LastY = y

            self.Render()


    def Reset(self):
        if self._CurrentRenderer:
            self._CurrentRenderer.ResetCamera()

        self.Render()


    def Wireframe(self):
        actors = self._CurrentRenderer.GetActors()
        numActors = actors.GetNumberOfItems()
        actors.InitTraversal()
        for i in range(0,numActors):
            actor = actors.GetNextItem()
            actor.GetProperty().SetRepresentationToWireframe()

        self.Render()


    def Surface(self):
        actors = self._CurrentRenderer.GetActors()
        numActors = actors.GetNumberOfItems()
        actors.InitTraversal()
        for i in range(0,numActors):
            actor = actors.GetNextItem()
            actor.GetProperty().SetRepresentationToSurface()

        self.Render()


    def PickActor(self,x,y):
        if self._CurrentRenderer:
            renderer = self._CurrentRenderer
            picker = self._Picker

            windowY = self.GetSize().height
            picker.Pick(x,(windowY - y - 1),0.0,renderer)
            assembly = picker.GetAssembly()

            if (self._PickedAssembly != None and
                self._PrePickedProperty != None):
                self._PickedAssembly.SetProperty(self._PrePickedProperty)
                # release hold of the property
                self._PrePickedProperty.UnRegister(self._PrePickedProperty)
                self._PrePickedProperty = None

            if (assembly != None):
                self._PickedAssembly = assembly
                self._PrePickedProperty = self._PickedAssembly.GetProperty()
                # hold onto the property
                self._PrePickedProperty.Register(self._PrePickedProperty)
                self._PickedAssembly.SetProperty(self._PickedProperty)

            self.Render()
'''
