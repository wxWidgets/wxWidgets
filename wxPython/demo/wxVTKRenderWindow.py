
from wxPython.wx  import *
try:
    from wxPython.lib import vtk
    haveVTK = true
except ImportError:
    haveVTK = false

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    if haveVTK:
        f = wxFrame(frame, -1, "wxVTKRenderWindow", size=(450, 300))
        win = vtk.wxVTKRenderWindow(f, -1)

        # Get the render window
        renWin = win.GetRenderWindow()

        # Next, do the VTK stuff
        ren = vtk.vtkRenderer()
        renWin.AddRenderer(ren)
        cone = vtk.vtkConeSource()
        cone.SetResolution(80)
        coneMapper = vtk.vtkPolyDataMapper()
        coneMapper.SetInput(cone.GetOutput())
        coneActor = vtk.vtkActor()
        coneActor.SetMapper(coneMapper)
        ren.AddActor(coneActor)
        coneMapper.GetLookupTable().Build()

        # Create a scalar bar
        scalarBar = vtk.vtkScalarBarActor()
        scalarBar.SetLookupTable(coneMapper.GetLookupTable())
        scalarBar.SetTitle("Temperature")
        scalarBar.GetPositionCoordinate().SetCoordinateSystemToNormalizedViewport()
        scalarBar.GetPositionCoordinate().SetValue(0.1, 0.01)
        scalarBar.SetOrientationToHorizontal()
        scalarBar.SetWidth(0.8)
        scalarBar.SetHeight(0.17)
        ren.AddActor2D(scalarBar)

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
