
from wxPython.lib.wxPlotCanvas import *
from wxPython.lib              import wxPlotCanvas

#---------------------------------------------------------------------------

def _InitObjects():
    # 100 points sin function, plotted as green circles
    data1 = 2.*Numeric.pi*Numeric.arange(200)/200.
    data1.shape = (100, 2)
    data1[:,1] = Numeric.sin(data1[:,0])
    markers1 = PolyMarker(data1, color='green', marker='circle',size=1)

    # 50 points cos function, plotted as red line
    data1 = 2.*Numeric.pi*Numeric.arange(100)/100.
    data1.shape = (50,2)
    data1[:,1] = Numeric.cos(data1[:,0])
    lines = PolyLine(data1, color='red')

    # A few more points...
    pi = Numeric.pi
    markers2 = PolyMarker([(0., 0.), (pi/4., 1.), (pi/2, 0.),
                          (3.*pi/4., -1)], color='blue',
                          fillcolor='green', marker='cross')

    return PlotGraphics([markers1, lines, markers2])


#---------------------------------------------------------------------------


def runTest(frame, nb, log):
    win = PlotCanvas(nb)
    win.draw(_InitObjects(),'automatic','automatic');
    return win

overview = wxPlotCanvas.__doc__

#---------------------------------------------------------------------------
