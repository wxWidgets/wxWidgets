# 11/20/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 
# 11/30/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o wxPlotCanvas must be updated with new draw mechanics (tuples) before
#   it can be used with 2.5.
# 

import  wx
import  wx.lib.wxPlotCanvas as  plot

import  Numeric

#---------------------------------------------------------------------------

def _InitObjects():
    # 100 points sin function, plotted as green circles
    data1 = 2.*Numeric.pi*Numeric.arange(200)/200.
    data1.shape = (100, 2)
    data1[:,1] = Numeric.sin(data1[:,0])
    markers1 = plot.PolyMarker(data1, color='green', marker='circle',size=1)

    # 50 points cos function, plotted as red line
    data1 = 2.*Numeric.pi*Numeric.arange(100)/100.
    data1.shape = (50,2)
    data1[:,1] = Numeric.cos(data1[:,0])
    lines = plot.PolyLine(data1, color='red')

    # A few more points...
    pi = Numeric.pi
    markers2 = plot.PolyMarker([(0., 0.), (pi/4., 1.), (pi/2, 0.),
                          (3.*pi/4., -1)], color='blue',
                          fillcolor='green', marker='cross')

    return plot.PlotGraphics([markers1, lines, markers2])


#---------------------------------------------------------------------------


def runTest(frame, nb, log):
    win = plot.PlotCanvas(nb)
    win.draw(_InitObjects(),'automatic','automatic');
    return win

overview = plot.__doc__

#---------------------------------------------------------------------------


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
