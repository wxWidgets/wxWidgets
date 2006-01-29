
import  wx

hadImportError = False
try:
    import  wx.lib.plot
except ImportError:
    hadImportError = True
    

################################################################\
# Where's the code???                                           |
#                                                               |
# wx.lib.plot.py came with its own excellent demo built in,     |
# for testing purposes, but it serves quite well to demonstrate |
# the code and classes within, so we are simply borrowing that  |
# code for the demo. Please load up wx.lib.plot.py for a review |
# of the code itself. The demo/test is at the bottom of         |
# the file, as expected.                                        |
################################################################/

#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        b = wx.Button(self, -1, "Show the PyPlot sample", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)


    def OnButton(self, evt):
        win = wx.lib.plot.TestFrame(self, -1, "PlotCanvas Demo")
        win.Show()

#---------------------------------------------------------------------------


def runTest(frame, nb, log):
    if not hadImportError:
        win = TestPanel(nb, log)
    else:
        from Main import MessagePanel
        win = MessagePanel(nb, """\
This demo requires the Numeric or numarray module,
which could not be imported.  It probably is not installed
(it's not part of the standard Python distribution). See the
Python site (http://www.python.org) for information on
downloading source or binaries.""",
                           'Sorry', wx.ICON_WARNING)
    
    return win


#----------------------------------------------------------------------

if hadImportError:
    overview = ""
else:
    overview = """\
<html><body>
<center><h2>PyPlot</h2></center>

This demo illustrates the features of the new PyPlot modules, found
in wx.lib.plot.py. All methods and functions are documented clearly
therein; only the overview is included here.

<p>
PyPlot is an improvement over wxPlotCanvas, which is now deprecated.
If you are using wxPlotCanvas now, please make plans to migrate in
anticipation of the time that wxPlotCanvas disappears completely.

<p>
The demo illustrates four different plot styles (with appropriate
variations on fonts, etc, to show how flexible it is) as well as
provides you with a means to tinker with all the features that
come with the class itself.

<p><ul>
<li>File Menu

    <ul>
    <li>Page Setup

        <p>This allows you to set up how the plot will be printed. This
        is built into the library itself.
       
    <li>Print Preview

        <p>As you might expect, this allows you to preview how the plot
        will look when printed, in light of the page setup you may
        have selected above.
        
    <li>Print
    
        <p>Surprise! It prints the current plot to your printer! :-)
        
    <li>Save Plot
    
        <p>That's right, the library even provides you with the means
        to export the plotted data out to a graphics file. Several
        formats are allowed for, basically any image class that
        supports saving.
    </ul>
    
<li>Plot Menu

    <ul>
    <li>Plot 1 ... Plot 5
     
        <p>Different data with different plot formats, including one empty
        plot.
         
    <li>Enable Zoom
    
        <p>If Zoom is enabled, you can rubber-band select an area of the
        plot to examine it in detail using the left mouse button. Right
        mouse button zooms back out. This is automatically supported
        by the library, all you have to do is turn it on.
        
    <li>Enable Grid
    
        <p>Plots can have different styles of grids, and and these grids can
        be turned on or off as needed.
        
    <li>Enable Legend
    
        <p>Plot can have legends or not, the contents which are definable 
        by you.
    </ul>
</ul>
        
<HR><pre>
%s</pre>
</body></html>
""" % wx.lib.plot.__doc__


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

