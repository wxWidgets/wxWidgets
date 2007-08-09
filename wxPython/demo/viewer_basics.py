# 11/15/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# o No idea what this does.
#

import  wx
import  wx.lib.vtk  as vtk

#---------------------------------------------------------------------------
class VtkFrame(wx.Frame):
    """
        Simple example VTK window that contains a cone.
    """
    def __init__(self, parent, id, title):
        wx.Frame.__init__(self, parent, id, title, size=(450, 300))
        win = vtk.VTKRenderWindow(self, -1)

        renWin = win.GetRenderWindow()

        ren = vtk.vtkRenderer()
        renWin.AddRenderer(ren)
        cone = vtk.vtkConeSource()
        coneMapper = vtk.vtkPolyDataMapper()
        coneMapper.SetInput(cone.GetOutput())
        coneActor = vtk.vtkActor()
        coneActor.SetMapper(coneMapper)
        ren.AddActor(coneActor)

#---------------------------------------------------------------------------
# Using new event binder
wx_EVT_ADD_CONE = wx.NewEventType()
EVT_ADD_CONE = wx.PyEventBinder(wx_EVT_ADD_CONE, 1)

class AddCone(wx.PyEvent):
    def __init__(self):
        wx.PyEvent.__init__(self)
        self.SetEventType(wx_EVT_ADD_CONE)


class HiddenCatcher(wx.Frame):
    """
        The "catcher" frame in the second thread.
        It is invisible.  It's only job is to receive
        Events from the main thread, and create
        the appropriate windows.
    """
    def __init__(self):
        wx.Frame.__init__(self, None, -1, '')
        self.Bind(EVT_ADD_CONE, self.AddCone)

    def AddCone(self,evt):
        add_cone()


#---------------------------------------------------------------------------

class SecondThreadApp(wx.App):
    """
        wxApp that lives in the second thread.
    """
    def OnInit(self):
        catcher = HiddenCatcher()
        #self.SetTopWindow(catcher)
        self.catcher = catcher
        return True

#---------------------------------------------------------------------------

def add_cone():
   frame = VtkFrame(None, -1, "Cone")
   frame.Show(True)

