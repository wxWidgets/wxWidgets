
from wxPython.wx  import *
try:
    from wxPython.lib import vtk
    haveVTK = true
except ImportError:
    haveVTK = false

#----------------------------------------------------------------------

def MakeCone(vtkwin):
    cone = vtk.vtkConeSource()
    cone.SetResolution(80)
    coneMapper = vtk.vtkPolyDataMapper()
    coneMapper.SetInput(cone.GetOutput())
    coneActor = vtk.vtkActor()
    coneActor.SetMapper(coneMapper)

    ren = vtk.vtkRenderer()
    vtkwin.GetRenderWindow().AddRenderer(ren)
    ren.AddActor(coneActor)


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    if haveVTK:
        f = wxFrame(frame, -1, "wxVTKRenderWindow", size=(450, 300),
                    style=wxDEFAULT_FRAME_STYLE|wxNO_FULL_REPAINT_ON_RESIZE)
        win = vtk.wxVTKRenderWindow(f, -1)
        MakeCone(win)

        f.Show(true)
        frame.otherWin = f
        return None


    else:
        wxMessageBox("Unable to import VTK, which is a required component\n"
                     "of this demo.  You need to download and install the\n"
                     "Python extension module for VTK from http://www.kitware.com/",
                     "Import Error")
        return None

#----------------------------------------------------------------------


overview = """\
wxVTKRenderWindow is a wrapper around the vtkRenderWindow from the
VTK Visualization Toolkit.  The VTK Python extensions are required,
they can be obtained from http://www.kitware.com/ where you can also
find some nifty pictures and stuff.

"""
