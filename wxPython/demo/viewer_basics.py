from wxPython.wx import *
from wxPython.lib import vtk

#---------------------------------------------------------------------------
class VtkFrame(wxFrame):
    """
        Simple example VTK window that contains a cone.
    """
    def __init__(self, parent, id, title):
        wxFrame.__init__(self, parent,id,title, size=(450, 300))
        win = vtk.wxVTKRenderWindow(self, -1)

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
wxEVT_ADD_CONE = 25015

def EVT_ADD_CONE(win, func):
    win.Connect(-1, -1, wxEVT_ADD_CONE, func)

class AddCone(wxPyEvent):
    def __init__(self):
        wxPyEvent.__init__(self)
        self.SetEventType(wxEVT_ADD_CONE)

class HiddenCatcher(wxFrame):
    """
        The "catcher" frame in the second thread.
        It is invisible.  It's only job is to receive
        Events from the main thread, and create 
        the appropriate windows.
    """ 
    def __init__(self):
        wxFrame.__init__(self, NULL,-1,'')        
        EVT_ADD_CONE(self, self.AddCone)

    def AddCone(self,evt): 
        add_cone()
#---------------------------------------------------------------------------
class SecondThreadApp(wxApp):
    """
        wxApp that lives in the second thread.
    """    
    def OnInit(self):
        catcher = HiddenCatcher()
        #self.SetTopWindow(catcher)
        self.catcher =catcher
        return true
#---------------------------------------------------------------------------

def add_cone(): 
   frame = VtkFrame(NULL, -1, "Cone")
   frame.Show(true)
