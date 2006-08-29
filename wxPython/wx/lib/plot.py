#-----------------------------------------------------------------------------
# Name:        wx.lib.plot.py
# Purpose:     Line, Bar and Scatter Graphs
#
# Author:      Gordon Williams
#
# Created:     2003/11/03
# RCS-ID:      $Id$
# Copyright:   (c) 2002
# Licence:     Use as you wish.
#-----------------------------------------------------------------------------
# 12/15/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o 2.5 compatability update.
# o Renamed to plot.py in the wx.lib directory.
# o Reworked test frame to work with wx demo framework. This saves a bit
#   of tedious cut and paste, and the test app is excellent.
#
# 12/18/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o wxScrolledMessageDialog -> ScrolledMessageDialog
#
# Oct 6, 2004  Gordon Williams (g_will@cyberus.ca)
#   - Added bar graph demo
#   - Modified line end shape from round to square.
#   - Removed FloatDCWrapper for conversion to ints and ints in arguments
#
# Oct 15, 2004  Gordon Williams (g_will@cyberus.ca)
#   - Imported modules given leading underscore to name.
#   - Added Cursor Line Tracking and User Point Labels. 
#   - Demo for Cursor Line Tracking and Point Labels.
#   - Size of plot preview frame adjusted to show page better.
#   - Added helper functions PositionUserToScreen and PositionScreenToUser in PlotCanvas.
#   - Added functions GetClosestPoints (all curves) and GetClosestPoint (only closest curve)
#       can be in either user coords or screen coords.
#   
#

"""
This is a simple light weight plotting module that can be used with
Boa or easily integrated into your own wxPython application.  The
emphasis is on small size and fast plotting for large data sets.  It
has a reasonable number of features to do line and scatter graphs
easily as well as simple bar graphs.  It is not as sophisticated or 
as powerful as SciPy Plt or Chaco.  Both of these are great packages 
but consume huge amounts of computer resources for simple plots.
They can be found at http://scipy.com

This file contains two parts; first the re-usable library stuff, then,
after a "if __name__=='__main__'" test, a simple frame and a few default
plots for examples and testing.

Based on wxPlotCanvas
Written by K.Hinsen, R. Srinivasan;
Ported to wxPython Harm van der Heijden, feb 1999

Major Additions Gordon Williams Feb. 2003 (g_will@cyberus.ca)
    -More style options
    -Zooming using mouse "rubber band"
    -Scroll left, right
    -Grid(graticule)
    -Printing, preview, and page set up (margins)
    -Axis and title labels
    -Cursor xy axis values
    -Doc strings and lots of comments
    -Optimizations for large number of points
    -Legends
    
Did a lot of work here to speed markers up. Only a factor of 4
improvement though. Lines are much faster than markers, especially
filled markers.  Stay away from circles and triangles unless you
only have a few thousand points.

Times for 25,000 points
Line - 0.078 sec
Markers
Square -                   0.22 sec
dot -                      0.10
circle -                   0.87
cross,plus -               0.28
triangle, triangle_down -  0.90

Thanks to Chris Barker for getting this version working on Linux.

Zooming controls with mouse (when enabled):
    Left mouse drag - Zoom box.
    Left mouse double click - reset zoom.
    Right mouse click - zoom out centred on click location.
"""

import  string as _string
import  time as _time
import  wx

# Needs Numeric or numarray or NumPy
try:
    import numpy as _Numeric
except:
    try:
        import numarray as _Numeric  #if numarray is used it is renamed Numeric
    except:
        try:
            import Numeric as _Numeric
        except:
            msg= """
            This module requires the Numeric/numarray or NumPy module,
            which could not be imported.  It probably is not installed
            (it's not part of the standard Python distribution). See the
            Numeric Python site (http://numpy.scipy.org) for information on
            downloading source or binaries."""
            raise ImportError, "Numeric,numarray or NumPy not found. \n" + msg



#
# Plotting classes...
#
class PolyPoints:
    """Base Class for lines and markers
        - All methods are private.
    """

    def __init__(self, points, attr):
        self._points = _Numeric.array(points).astype(_Numeric.Float64)
        self._logscale = (False, False)
        self.currentScale= (1,1)
        self.currentShift= (0,0)
        self.scaled = self.points
        self.attributes = {}
        self.attributes.update(self._attributes)
        for name, value in attr.items():   
            if name not in self._attributes.keys():
                raise KeyError, "Style attribute incorrect. Should be one of %s" % self._attributes.keys()
            self.attributes[name] = value
        
    def setLogScale(self, logscale):
        self._logscale = logscale
    
    def __getattr__(self, name):
        if name == 'points':
            if len(self._points)>0:
                data = _Numeric.array(self._points,copy=True)
                if self._logscale[0]:
                    data = self.log10(data, 0)
                if self._logscale[1]:
                    data = self.log10(data, 1)
                return data
            else:
                return self._points
        else:
            raise AttributeError, name
            
    def log10(self, data, ind):
        data = _Numeric.compress(data[:,ind]>0,data,0)
        data[:,ind] = _Numeric.log10(data[:,ind])
        return data

    def boundingBox(self):
        if len(self.points) == 0:
            # no curves to draw
            # defaults to (-1,-1) and (1,1) but axis can be set in Draw
            minXY= _Numeric.array([-1.0,-1.0])
            maxXY= _Numeric.array([ 1.0, 1.0])
        else:
            minXY= _Numeric.minimum.reduce(self.points)
            maxXY= _Numeric.maximum.reduce(self.points)
        return minXY, maxXY

    def scaleAndShift(self, scale=(1,1), shift=(0,0)):
        if len(self.points) == 0:
            # no curves to draw
            return
        if (scale is not self.currentScale) or (shift is not self.currentShift):
            # update point scaling
            self.scaled = scale*self.points+shift
            self.currentScale= scale
            self.currentShift= shift
        # else unchanged use the current scaling
        
    def getLegend(self):
        return self.attributes['legend']

    def getClosestPoint(self, pntXY, pointScaled= True):
        """Returns the index of closest point on the curve, pointXY, scaledXY, distance
            x, y in user coords
            if pointScaled == True based on screen coords
            if pointScaled == False based on user coords
        """
        if pointScaled == True:
            #Using screen coords
            p = self.scaled
            pxy = self.currentScale * _Numeric.array(pntXY)+ self.currentShift
        else:
            #Using user coords
            p = self.points
            pxy = _Numeric.array(pntXY)
        #determine distance for each point
        d= _Numeric.sqrt(_Numeric.add.reduce((p-pxy)**2,1)) #sqrt(dx^2+dy^2)
        pntIndex = _Numeric.argmin(d)
        dist = d[pntIndex]
        return [pntIndex, self.points[pntIndex], self.scaled[pntIndex], dist]
        
        
class PolyLine(PolyPoints):
    """Class to define line type and style
        - All methods except __init__ are private.
    """
    
    _attributes = {'colour': 'black',
                   'width': 1,
                   'style': wx.SOLID,
                   'legend': ''}

    def __init__(self, points, **attr):
        """Creates PolyLine object
            points - sequence (array, tuple or list) of (x,y) points making up line
            **attr - key word attributes
                Defaults:
                    'colour'= 'black',          - wx.Pen Colour any wx.NamedColour
                    'width'= 1,                 - Pen width
                    'style'= wx.SOLID,          - wx.Pen style
                    'legend'= ''                - Line Legend to display
        """
        PolyPoints.__init__(self, points, attr)

    def draw(self, dc, printerScale, coord= None):
        colour = self.attributes['colour']
        width = self.attributes['width'] * printerScale
        style= self.attributes['style']
        if not isinstance(colour, wx.Colour):
            colour = wx.NamedColour(colour)
        pen = wx.Pen(colour, width, style)
        pen.SetCap(wx.CAP_BUTT)
        dc.SetPen(pen)
        if coord == None:
            dc.DrawLines(self.scaled)
        else:
            dc.DrawLines(coord) # draw legend line

    def getSymExtent(self, printerScale):
        """Width and Height of Marker"""
        h= self.attributes['width'] * printerScale
        w= 5 * h
        return (w,h)


class PolyMarker(PolyPoints):
    """Class to define marker type and style
        - All methods except __init__ are private.
    """
  
    _attributes = {'colour': 'black',
                   'width': 1,
                   'size': 2,
                   'fillcolour': None,
                   'fillstyle': wx.SOLID,
                   'marker': 'circle',
                   'legend': ''}

    def __init__(self, points, **attr):
        """Creates PolyMarker object
        points - sequence (array, tuple or list) of (x,y) points
        **attr - key word attributes
            Defaults:
                'colour'= 'black',          - wx.Pen Colour any wx.NamedColour
                'width'= 1,                 - Pen width
                'size'= 2,                  - Marker size
                'fillcolour'= same as colour,      - wx.Brush Colour any wx.NamedColour
                'fillstyle'= wx.SOLID,      - wx.Brush fill style (use wx.TRANSPARENT for no fill)
                'marker'= 'circle'          - Marker shape
                'legend'= ''                - Marker Legend to display
              
            Marker Shapes:
                - 'circle'
                - 'dot'
                - 'square'
                - 'triangle'
                - 'triangle_down'
                - 'cross'
                - 'plus'
        """
      
        PolyPoints.__init__(self, points, attr)

    def draw(self, dc, printerScale, coord= None):
        colour = self.attributes['colour']
        width = self.attributes['width'] * printerScale
        size = self.attributes['size'] * printerScale
        fillcolour = self.attributes['fillcolour']
        fillstyle = self.attributes['fillstyle']
        marker = self.attributes['marker']

        if colour and not isinstance(colour, wx.Colour):
            colour = wx.NamedColour(colour)
        if fillcolour and not isinstance(fillcolour, wx.Colour):
            fillcolour = wx.NamedColour(fillcolour)
            
        dc.SetPen(wx.Pen(colour, width))
        if fillcolour:
            dc.SetBrush(wx.Brush(fillcolour,fillstyle))
        else:
            dc.SetBrush(wx.Brush(colour, fillstyle))
        if coord == None:
            self._drawmarkers(dc, self.scaled, marker, size)
        else:
            self._drawmarkers(dc, coord, marker, size) # draw legend marker

    def getSymExtent(self, printerScale):
        """Width and Height of Marker"""
        s= 5*self.attributes['size'] * printerScale
        return (s,s)

    def _drawmarkers(self, dc, coords, marker,size=1):
        f = eval('self._' +marker)
        f(dc, coords, size)

    def _circle(self, dc, coords, size=1):
        fact= 2.5*size
        wh= 5.0*size
        rect= _Numeric.zeros((len(coords),4),_Numeric.Float)+[0.0,0.0,wh,wh]
        rect[:,0:2]= coords-[fact,fact]
        dc.DrawEllipseList(rect.astype(_Numeric.Int32))

    def _dot(self, dc, coords, size=1):
        dc.DrawPointList(coords)

    def _square(self, dc, coords, size=1):
        fact= 2.5*size
        wh= 5.0*size
        rect= _Numeric.zeros((len(coords),4),_Numeric.Float)+[0.0,0.0,wh,wh]
        rect[:,0:2]= coords-[fact,fact]
        dc.DrawRectangleList(rect.astype(_Numeric.Int32))

    def _triangle(self, dc, coords, size=1):
        shape= [(-2.5*size,1.44*size), (2.5*size,1.44*size), (0.0,-2.88*size)]
        poly= _Numeric.repeat(coords,3)
        poly.shape= (len(coords),3,2)
        poly += shape
        dc.DrawPolygonList(poly.astype(_Numeric.Int32))

    def _triangle_down(self, dc, coords, size=1):
        shape= [(-2.5*size,-1.44*size), (2.5*size,-1.44*size), (0.0,2.88*size)]
        poly= _Numeric.repeat(coords,3)
        poly.shape= (len(coords),3,2)
        poly += shape
        dc.DrawPolygonList(poly.astype(_Numeric.Int32))
      
    def _cross(self, dc, coords, size=1):
        fact= 2.5*size
        for f in [[-fact,-fact,fact,fact],[-fact,fact,fact,-fact]]:
            lines= _Numeric.concatenate((coords,coords),axis=1)+f
            dc.DrawLineList(lines.astype(_Numeric.Int32))

    def _plus(self, dc, coords, size=1):
        fact= 2.5*size
        for f in [[-fact,0,fact,0],[0,-fact,0,fact]]:
            lines= _Numeric.concatenate((coords,coords),axis=1)+f
            dc.DrawLineList(lines.astype(_Numeric.Int32))

class PlotGraphics:
    """Container to hold PolyXXX objects and graph labels
        - All methods except __init__ are private.
    """

    def __init__(self, objects, title='', xLabel='', yLabel= ''):
        """Creates PlotGraphics object
        objects - list of PolyXXX objects to make graph
        title - title shown at top of graph
        xLabel - label shown on x-axis
        yLabel - label shown on y-axis
        """
        if type(objects) not in [list,tuple]:
            raise TypeError, "objects argument should be list or tuple"
        self.objects = objects
        self.title= title
        self.xLabel= xLabel
        self.yLabel= yLabel

    def setLogScale(self, logscale):
        if type(logscale) != tuple:
            raise TypeError, 'logscale must be a tuple of bools, e.g. (False, False)'
        if len(self.objects) == 0:
            return
        for o in self.objects:
            o.setLogScale(logscale)
        
    def boundingBox(self):
        p1, p2 = self.objects[0].boundingBox()
        for o in self.objects[1:]:
            p1o, p2o = o.boundingBox()
            p1 = _Numeric.minimum(p1, p1o)
            p2 = _Numeric.maximum(p2, p2o)
        return p1, p2

    def scaleAndShift(self, scale=(1,1), shift=(0,0)):
        for o in self.objects:
            o.scaleAndShift(scale, shift)

    def setPrinterScale(self, scale):
        """Thickens up lines and markers only for printing"""
        self.printerScale= scale

    def setXLabel(self, xLabel= ''):
        """Set the X axis label on the graph"""
        self.xLabel= xLabel

    def setYLabel(self, yLabel= ''):
        """Set the Y axis label on the graph"""
        self.yLabel= yLabel
        
    def setTitle(self, title= ''):
        """Set the title at the top of graph"""
        self.title= title

    def getXLabel(self):
        """Get x axis label string"""
        return self.xLabel

    def getYLabel(self):
        """Get y axis label string"""
        return self.yLabel

    def getTitle(self, title= ''):
        """Get the title at the top of graph"""
        return self.title

    def draw(self, dc):
        for o in self.objects:
            #t=_time.clock()          # profile info
            o.draw(dc, self.printerScale)
            #dt= _time.clock()-t
            #print o, "time=", dt

    def getSymExtent(self, printerScale):
        """Get max width and height of lines and markers symbols for legend"""
        symExt = self.objects[0].getSymExtent(printerScale)
        for o in self.objects[1:]:
            oSymExt = o.getSymExtent(printerScale)
            symExt = _Numeric.maximum(symExt, oSymExt)
        return symExt
    
    def getLegendNames(self):
        """Returns list of legend names"""
        lst = [None]*len(self)
        for i in range(len(self)):
            lst[i]= self.objects[i].getLegend()
        return lst
            
    def __len__(self):
        return len(self.objects)

    def __getitem__(self, item):
        return self.objects[item]


#-------------------------------------------------------------------------------
# Main window that you will want to import into your application.

class PlotCanvas(wx.Panel):
    """
    Subclass of a wx.Panel which holds two scrollbars and the actual
    plotting canvas (self.canvas). It allows for simple general plotting
    of data with zoom, labels, and automatic axis scaling."""

    def __init__(self, parent, id=wx.ID_ANY, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=0, name="plotCanvas"):
        """Constructs a panel, which can be a child of a frame or
        any other non-control window"""
    
        wx.Panel.__init__(self, parent, id, pos, size, style, name)

        sizer = wx.FlexGridSizer(2,2,0,0)
        self.canvas = wx.Window(self, -1)
        self.sb_vert = wx.ScrollBar(self, -1, style=wx.SB_VERTICAL)
        self.sb_vert.SetScrollbar(0,1000,1000,1000)
        self.sb_hor = wx.ScrollBar(self, -1, style=wx.SB_HORIZONTAL)
        self.sb_hor.SetScrollbar(0,1000,1000,1000)

        sizer.Add(self.canvas, 1, wx.EXPAND)
        sizer.Add(self.sb_vert, 0, wx.EXPAND)
        sizer.Add(self.sb_hor, 0, wx.EXPAND)
        sizer.Add((0,0))
        
        sizer.AddGrowableRow(0, 1)
        sizer.AddGrowableCol(0, 1)

        self.sb_vert.Show(False)
        self.sb_hor.Show(False)
        
        self.SetSizer(sizer)
        self.Fit()

        self.border = (1,1)

        self.SetBackgroundColour("white")
        
        # Create some mouse events for zooming
        self.canvas.Bind(wx.EVT_LEFT_DOWN, self.OnMouseLeftDown)
        self.canvas.Bind(wx.EVT_LEFT_UP, self.OnMouseLeftUp)
        self.canvas.Bind(wx.EVT_MOTION, self.OnMotion)
        self.canvas.Bind(wx.EVT_LEFT_DCLICK, self.OnMouseDoubleClick)
        self.canvas.Bind(wx.EVT_RIGHT_DOWN, self.OnMouseRightDown)

        # scrollbar events
        self.Bind(wx.EVT_SCROLL_THUMBTRACK, self.OnScroll)
        self.Bind(wx.EVT_SCROLL_PAGEUP, self.OnScroll)
        self.Bind(wx.EVT_SCROLL_PAGEDOWN, self.OnScroll)
        self.Bind(wx.EVT_SCROLL_LINEUP, self.OnScroll)
        self.Bind(wx.EVT_SCROLL_LINEDOWN, self.OnScroll)

        # set curser as cross-hairs
        self.canvas.SetCursor(wx.CROSS_CURSOR)
        self.HandCursor = wx.CursorFromImage(getHandImage())
        self.GrabHandCursor = wx.CursorFromImage(getGrabHandImage())
        self.MagCursor = wx.CursorFromImage(getMagPlusImage())
            
        # Things for printing
        self.print_data = wx.PrintData()
        self.print_data.SetPaperId(wx.PAPER_LETTER)
        self.print_data.SetOrientation(wx.LANDSCAPE)
        self.pageSetupData= wx.PageSetupDialogData()
        self.pageSetupData.SetMarginBottomRight((25,25))
        self.pageSetupData.SetMarginTopLeft((25,25))
        self.pageSetupData.SetPrintData(self.print_data)
        self.printerScale = 1
        self.parent= parent

        # scrollbar variables
        self._sb_ignore = False
        self._adjustingSB = False
        self._sb_xfullrange = 0
        self._sb_yfullrange = 0
        self._sb_xunit = 0
        self._sb_yunit = 0
        
        self._dragEnabled = False
        self._screenCoordinates = _Numeric.array([0.0, 0.0])
        
        self._logscale = (False, False)

        # Zooming variables
        self._zoomInFactor =  0.5
        self._zoomOutFactor = 2
        self._zoomCorner1= _Numeric.array([0.0, 0.0]) # left mouse down corner
        self._zoomCorner2= _Numeric.array([0.0, 0.0])   # left mouse up corner
        self._zoomEnabled= False
        self._hasDragged= False
        
        # Drawing Variables
        self.last_draw = None
        self._pointScale= 1
        self._pointShift= 0
        self._xSpec= 'auto'
        self._ySpec= 'auto'
        self._gridEnabled= False
        self._legendEnabled= False
        
        # Fonts
        self._fontCache = {}
        self._fontSizeAxis= 10
        self._fontSizeTitle= 15
        self._fontSizeLegend= 7

        # pointLabels
        self._pointLabelEnabled= False
        self.last_PointLabel= None
        self._pointLabelFunc= None
        self.canvas.Bind(wx.EVT_LEAVE_WINDOW, self.OnLeave)

        self.canvas.Bind(wx.EVT_PAINT, self.OnPaint)
        self.canvas.Bind(wx.EVT_SIZE, self.OnSize)
        # OnSize called to make sure the buffer is initialized.
        # This might result in OnSize getting called twice on some
        # platforms at initialization, but little harm done.
        self.OnSize(None) # sets the initial size based on client size

        self._gridColour = wx.NamedColour('black')

    def SetCursor(self, cursor):
        self.canvas.SetCursor(cursor)
        
    def GetGridColour(self):
        return self._gridColour

    def SetGridColour(self, colour):
        if isinstance(colour, wx.Colour):
            self._gridColour = colour
        else:
            self._gridColour = wx.NamedColour(colour)

        
    # SaveFile
    def SaveFile(self, fileName= ''):
        """Saves the file to the type specified in the extension. If no file
        name is specified a dialog box is provided.  Returns True if sucessful,
        otherwise False.
        
        .bmp  Save a Windows bitmap file.
        .xbm  Save an X bitmap file.
        .xpm  Save an XPM bitmap file.
        .png  Save a Portable Network Graphics file.
        .jpg  Save a Joint Photographic Experts Group file.
        """
        if _string.lower(fileName[-3:]) not in ['bmp','xbm','xpm','png','jpg']:
            dlg1 = wx.FileDialog(
                    self, 
                    "Choose a file with extension bmp, gif, xbm, xpm, png, or jpg", ".", "",
                    "BMP files (*.bmp)|*.bmp|XBM files (*.xbm)|*.xbm|XPM file (*.xpm)|*.xpm|PNG files (*.png)|*.png|JPG files (*.jpg)|*.jpg",
                    wx.SAVE|wx.OVERWRITE_PROMPT
                    )
            try:
                while 1:
                    if dlg1.ShowModal() == wx.ID_OK:
                        fileName = dlg1.GetPath()
                        # Check for proper exension
                        if _string.lower(fileName[-3:]) not in ['bmp','xbm','xpm','png','jpg']:
                            dlg2 = wx.MessageDialog(self, 'File name extension\n'
                            'must be one of\n'
                            'bmp, xbm, xpm, png, or jpg',
                              'File Name Error', wx.OK | wx.ICON_ERROR)
                            try:
                                dlg2.ShowModal()
                            finally:
                                dlg2.Destroy()
                        else:
                            break # now save file
                    else: # exit without saving
                        return False
            finally:
                dlg1.Destroy()

        # File name has required extension
        fType = _string.lower(fileName[-3:])
        if fType == "bmp":
            tp= wx.BITMAP_TYPE_BMP       # Save a Windows bitmap file.
        elif fType == "xbm":
            tp= wx.BITMAP_TYPE_XBM       # Save an X bitmap file.
        elif fType == "xpm":
            tp= wx.BITMAP_TYPE_XPM       # Save an XPM bitmap file.
        elif fType == "jpg":
            tp= wx.BITMAP_TYPE_JPEG      # Save a JPG file.
        else:
            tp= wx.BITMAP_TYPE_PNG       # Save a PNG file.
        # Save Bitmap
        res= self._Buffer.SaveFile(fileName, tp)
        return res

    def PageSetup(self):
        """Brings up the page setup dialog"""
        data = self.pageSetupData
        data.SetPrintData(self.print_data)
        dlg = wx.PageSetupDialog(self.parent, data)
        try:
            if dlg.ShowModal() == wx.ID_OK:
                data = dlg.GetPageSetupData() # returns wx.PageSetupDialogData
                # updates page parameters from dialog
                self.pageSetupData.SetMarginBottomRight(data.GetMarginBottomRight())
                self.pageSetupData.SetMarginTopLeft(data.GetMarginTopLeft())
                self.pageSetupData.SetPrintData(data.GetPrintData())
                self.print_data=wx.PrintData(data.GetPrintData()) # updates print_data
        finally:
            dlg.Destroy()
                
    def Printout(self, paper=None):
        """Print current plot."""
        if paper != None:
            self.print_data.SetPaperId(paper)
        pdd = wx.PrintDialogData(self.print_data)
        printer = wx.Printer(pdd)
        out = PlotPrintout(self)
        print_ok = printer.Print(self.parent, out)
        if print_ok:
            self.print_data = wx.PrintData(printer.GetPrintDialogData().GetPrintData())
        out.Destroy()

    def PrintPreview(self):
        """Print-preview current plot."""
        printout = PlotPrintout(self)
        printout2 = PlotPrintout(self)
        self.preview = wx.PrintPreview(printout, printout2, self.print_data)
        if not self.preview.Ok():
            wx.MessageDialog(self, "Print Preview failed.\n" \
                               "Check that default printer is configured\n", \
                               "Print error", wx.OK|wx.CENTRE).ShowModal()
        self.preview.SetZoom(40)
        # search up tree to find frame instance
        frameInst= self
        while not isinstance(frameInst, wx.Frame):
            frameInst= frameInst.GetParent()
        frame = wx.PreviewFrame(self.preview, frameInst, "Preview")
        frame.Initialize()
        frame.SetPosition(self.GetPosition())
        frame.SetSize((600,550))
        frame.Centre(wx.BOTH)
        frame.Show(True)

    def setLogScale(self, logscale):
        if type(logscale) != tuple:
            raise TypeError, 'logscale must be a tuple of bools, e.g. (False, False)'
        if self.last_draw is not None:
            graphics, xAxis, yAxis= self.last_draw
            graphics.setLogScale(logscale)
            self.last_draw = (graphics, None, None)
        self.SetXSpec('min')
        self.SetYSpec('min')
        self._logscale = logscale

    def getLogScale(self):
        return self._logscale
    
    def SetFontSizeAxis(self, point= 10):
        """Set the tick and axis label font size (default is 10 point)"""
        self._fontSizeAxis= point
        
    def GetFontSizeAxis(self):
        """Get current tick and axis label font size in points"""
        return self._fontSizeAxis
    
    def SetFontSizeTitle(self, point= 15):
        """Set Title font size (default is 15 point)"""
        self._fontSizeTitle= point

    def GetFontSizeTitle(self):
        """Get current Title font size in points"""
        return self._fontSizeTitle
    
    def SetFontSizeLegend(self, point= 7):
        """Set Legend font size (default is 7 point)"""
        self._fontSizeLegend= point
        
    def GetFontSizeLegend(self):
        """Get current Legend font size in points"""
        return self._fontSizeLegend

    def SetShowScrollbars(self, value):
        """Set True to show scrollbars"""
        if value not in [True,False]:
            raise TypeError, "Value should be True or False"
        if value == self.GetShowScrollbars():
            return
        self.sb_vert.Show(value)
        self.sb_hor.Show(value)
        wx.CallAfter(self.Layout)

    def GetShowScrollbars(self):
        """Set True to show scrollbars"""
        return self.sb_vert.IsShown()

    def SetEnableDrag(self, value):
        """Set True to enable drag."""
        if value not in [True,False]:
            raise TypeError, "Value should be True or False"
        if value:
            if self.GetEnableZoom():
                self.SetEnableZoom(False)
            self.SetCursor(self.HandCursor)
        else:
            self.SetCursor(wx.CROSS_CURSOR)
        self._dragEnabled = value
    
    def GetEnableDrag(self):
        return self._dragEnabled
    
    def SetEnableZoom(self, value):
        """Set True to enable zooming."""
        if value not in [True,False]:
            raise TypeError, "Value should be True or False"
        if value:
            if self.GetEnableDrag():
                self.SetEnableDrag(False)
            self.SetCursor(self.MagCursor)
        else:
            self.SetCursor(wx.CROSS_CURSOR)
        self._zoomEnabled= value

    def GetEnableZoom(self):
        """True if zooming enabled."""
        return self._zoomEnabled

    def SetEnableGrid(self, value):
        """Set True to enable grid."""
        if value not in [True,False,'Horizontal','Vertical']:
            raise TypeError, "Value should be True, False, Horizontal or Vertical"
        self._gridEnabled= value
        self.Redraw()

    def GetEnableGrid(self):
        """True if grid enabled."""
        return self._gridEnabled

    def SetEnableLegend(self, value):
        """Set True to enable legend."""
        if value not in [True,False]:
            raise TypeError, "Value should be True or False"
        self._legendEnabled= value 
        self.Redraw()

    def GetEnableLegend(self):
        """True if Legend enabled."""
        return self._legendEnabled

    def SetEnablePointLabel(self, value):
        """Set True to enable pointLabel."""
        if value not in [True,False]:
            raise TypeError, "Value should be True or False"
        self._pointLabelEnabled= value 
        self.Redraw()  #will erase existing pointLabel if present
        self.last_PointLabel = None

    def GetEnablePointLabel(self):
        """True if pointLabel enabled."""
        return self._pointLabelEnabled

    def SetPointLabelFunc(self, func):
        """Sets the function with custom code for pointLabel drawing
            ******** more info needed ***************
        """
        self._pointLabelFunc= func

    def GetPointLabelFunc(self):
        """Returns pointLabel Drawing Function"""
        return self._pointLabelFunc

    def Reset(self):
        """Unzoom the plot."""
        self.last_PointLabel = None        #reset pointLabel
        if self.last_draw is not None:
            self._Draw(self.last_draw[0])
        
    def ScrollRight(self, units):          
        """Move view right number of axis units."""
        self.last_PointLabel = None        #reset pointLabel
        if self.last_draw is not None:
            graphics, xAxis, yAxis= self.last_draw
            xAxis= (xAxis[0]+units, xAxis[1]+units)
            self._Draw(graphics,xAxis,yAxis)

    def ScrollUp(self, units):
        """Move view up number of axis units."""
        self.last_PointLabel = None        #reset pointLabel
        if self.last_draw is not None:
             graphics, xAxis, yAxis= self.last_draw
             yAxis= (yAxis[0]+units, yAxis[1]+units)
             self._Draw(graphics,xAxis,yAxis)

    def GetXY(self, event):
        """Wrapper around _getXY, which handles log scales"""
        x,y = self._getXY(event)
        if self.getLogScale()[0]:
            x = _Numeric.power(10,x)
        if self.getLogScale()[1]:
            y = _Numeric.power(10,y)
        return x,y
        
    def _getXY(self,event):
        """Takes a mouse event and returns the XY user axis values."""
        x,y= self.PositionScreenToUser(event.GetPosition())
        return x,y

    def PositionUserToScreen(self, pntXY):
        """Converts User position to Screen Coordinates"""
        userPos= _Numeric.array(pntXY)
        x,y= userPos * self._pointScale + self._pointShift
        return x,y
        
    def PositionScreenToUser(self, pntXY):
        """Converts Screen position to User Coordinates"""
        screenPos= _Numeric.array(pntXY)
        x,y= (screenPos-self._pointShift)/self._pointScale
        return x,y
        
    def SetXSpec(self, type= 'auto'):
        """xSpec- defines x axis type. Can be 'none', 'min' or 'auto'
        where:
            'none' - shows no axis or tick mark values
            'min' - shows min bounding box values
            'auto' - rounds axis range to sensible values
        """
        self._xSpec= type
        
    def SetYSpec(self, type= 'auto'):
        """ySpec- defines x axis type. Can be 'none', 'min' or 'auto'
        where:
            'none' - shows no axis or tick mark values
            'min' - shows min bounding box values
            'auto' - rounds axis range to sensible values
        """
        self._ySpec= type

    def GetXSpec(self):
        """Returns current XSpec for axis"""
        return self._xSpec
    
    def GetYSpec(self):
        """Returns current YSpec for axis"""
        return self._ySpec
    
    def GetXMaxRange(self):
        xAxis = self._getXMaxRange()
        if self.getLogScale()[0]:
            xAxis = _Numeric.power(10,xAxis)
        return xAxis

    def _getXMaxRange(self):
        """Returns (minX, maxX) x-axis range for displayed graph"""
        graphics= self.last_draw[0]
        p1, p2 = graphics.boundingBox()     # min, max points of graphics
        xAxis = self._axisInterval(self._xSpec, p1[0], p2[0]) # in user units
        return xAxis

    def GetYMaxRange(self):
        yAxis = self._getYMaxRange()
        if self.getLogScale()[1]:
            yAxis = _Numeric.power(10,yAxis)
        return yAxis

    def _getYMaxRange(self):
        """Returns (minY, maxY) y-axis range for displayed graph"""
        graphics= self.last_draw[0]
        p1, p2 = graphics.boundingBox()     # min, max points of graphics
        yAxis = self._axisInterval(self._ySpec, p1[1], p2[1])
        return yAxis

    def GetXCurrentRange(self):
        xAxis = self._getXCurrentRange()
        if self.getLogScale()[0]:
            xAxis = _Numeric.power(10,xAxis)
        return xAxis

    def _getXCurrentRange(self):
        """Returns (minX, maxX) x-axis for currently displayed portion of graph"""
        return self.last_draw[1]
    
    def GetYCurrentRange(self):
        yAxis = self._getYCurrentRange()
        if self.getLogScale()[1]:
            yAxis = _Numeric.power(10,yAxis)
        return yAxis

    def _getYCurrentRange(self):
        """Returns (minY, maxY) y-axis for currently displayed portion of graph"""
        return self.last_draw[2]

    def Draw(self, graphics, xAxis = None, yAxis = None, dc = None):
        """Wrapper around _Draw, which handles log axes"""
        
        graphics.setLogScale(self.getLogScale())
        
        # check Axis is either tuple or none
        if type(xAxis) not in [type(None),tuple]:
            raise TypeError, "xAxis should be None or (minX,maxX)"+str(type(xAxis))
        if type(yAxis) not in [type(None),tuple]:
            raise TypeError, "yAxis should be None or (minY,maxY)"+str(type(xAxis))
             
        # check case for axis = (a,b) where a==b caused by improper zooms
        if xAxis != None:
            if xAxis[0] == xAxis[1]:
                return
            if self.getLogScale()[0]:
                xAxis = _Numeric.log10(xAxis)
        if yAxis != None:
            if yAxis[0] == yAxis[1]:
                return
            if self.getLogScale()[1]:
                yAxis = _Numeric.log10(yAxis)
        self._Draw(graphics, xAxis, yAxis, dc)
        
    def _Draw(self, graphics, xAxis = None, yAxis = None, dc = None):
        """\
        Draw objects in graphics with specified x and y axis.
        graphics- instance of PlotGraphics with list of PolyXXX objects
        xAxis - tuple with (min, max) axis range to view
        yAxis - same as xAxis
        dc - drawing context - doesn't have to be specified.    
        If it's not, the offscreen buffer is used
        """

        if dc == None:
            # sets new dc and clears it 
            dc = wx.BufferedDC(wx.ClientDC(self.canvas), self._Buffer)
            dc.Clear()
            
        dc.BeginDrawing()
        # dc.Clear()
        
        # set font size for every thing but title and legend
        dc.SetFont(self._getFont(self._fontSizeAxis))

        # sizes axis to axis type, create lower left and upper right corners of plot
        if xAxis == None or yAxis == None:
            # One or both axis not specified in Draw
            p1, p2 = graphics.boundingBox()     # min, max points of graphics
            if xAxis == None:
                xAxis = self._axisInterval(self._xSpec, p1[0], p2[0]) # in user units
            if yAxis == None:
                yAxis = self._axisInterval(self._ySpec, p1[1], p2[1])
            # Adjust bounding box for axis spec
            p1[0],p1[1] = xAxis[0], yAxis[0]     # lower left corner user scale (xmin,ymin)
            p2[0],p2[1] = xAxis[1], yAxis[1]     # upper right corner user scale (xmax,ymax)
        else:
            # Both axis specified in Draw
            p1= _Numeric.array([xAxis[0], yAxis[0]])    # lower left corner user scale (xmin,ymin)
            p2= _Numeric.array([xAxis[1], yAxis[1]])     # upper right corner user scale (xmax,ymax)

        self.last_draw = (graphics, _Numeric.array(xAxis), _Numeric.array(yAxis))       # saves most recient values

        # Get ticks and textExtents for axis if required
        if self._xSpec is not 'none':        
            xticks = self._xticks(xAxis[0], xAxis[1])
            xTextExtent = dc.GetTextExtent(xticks[-1][1])# w h of x axis text last number on axis
        else:
            xticks = None
            xTextExtent= (0,0) # No text for ticks
        if self._ySpec is not 'none':
            yticks = self._yticks(yAxis[0], yAxis[1])
            if self.getLogScale()[1]:
                yTextExtent = dc.GetTextExtent('-2e-2')
            else:
                yTextExtentBottom = dc.GetTextExtent(yticks[0][1])
                yTextExtentTop = dc.GetTextExtent(yticks[-1][1])
                yTextExtent= (max(yTextExtentBottom[0],yTextExtentTop[0]),
                              max(yTextExtentBottom[1],yTextExtentTop[1]))
        else:
            yticks = None
            yTextExtent= (0,0) # No text for ticks

        # TextExtents for Title and Axis Labels
        titleWH, xLabelWH, yLabelWH= self._titleLablesWH(dc, graphics)

        # TextExtents for Legend
        legendBoxWH, legendSymExt, legendTextExt = self._legendWH(dc, graphics)

        # room around graph area
        rhsW= max(xTextExtent[0], legendBoxWH[0]) # use larger of number width or legend width
        lhsW= yTextExtent[0]+ yLabelWH[1]
        bottomH= max(xTextExtent[1], yTextExtent[1]/2.)+ xLabelWH[1]
        topH= yTextExtent[1]/2. + titleWH[1]
        textSize_scale= _Numeric.array([rhsW+lhsW,bottomH+topH]) # make plot area smaller by text size
        textSize_shift= _Numeric.array([lhsW, bottomH])          # shift plot area by this amount

        # drawing title and labels text
        dc.SetFont(self._getFont(self._fontSizeTitle))
        titlePos= (self.plotbox_origin[0]+ lhsW + (self.plotbox_size[0]-lhsW-rhsW)/2.- titleWH[0]/2.,
                 self.plotbox_origin[1]- self.plotbox_size[1])
        dc.DrawText(graphics.getTitle(),titlePos[0],titlePos[1])
        dc.SetFont(self._getFont(self._fontSizeAxis))
        xLabelPos= (self.plotbox_origin[0]+ lhsW + (self.plotbox_size[0]-lhsW-rhsW)/2.- xLabelWH[0]/2.,
                 self.plotbox_origin[1]- xLabelWH[1])
        dc.DrawText(graphics.getXLabel(),xLabelPos[0],xLabelPos[1])
        yLabelPos= (self.plotbox_origin[0],
                 self.plotbox_origin[1]- bottomH- (self.plotbox_size[1]-bottomH-topH)/2.+ yLabelWH[0]/2.)
        if graphics.getYLabel():  # bug fix for Linux
            dc.DrawRotatedText(graphics.getYLabel(),yLabelPos[0],yLabelPos[1],90)

        # drawing legend makers and text
        if self._legendEnabled:
            self._drawLegend(dc,graphics,rhsW,topH,legendBoxWH, legendSymExt, legendTextExt)

        # allow for scaling and shifting plotted points
        scale = (self.plotbox_size-textSize_scale) / (p2-p1)* _Numeric.array((1,-1))
        shift = -p1*scale + self.plotbox_origin + textSize_shift * _Numeric.array((1,-1))
        self._pointScale= scale  # make available for mouse events
        self._pointShift= shift        
        self._drawAxes(dc, p1, p2, scale, shift, xticks, yticks)
        
        graphics.scaleAndShift(scale, shift)
        graphics.setPrinterScale(self.printerScale)  # thicken up lines and markers if printing
        
        # set clipping area so drawing does not occur outside axis box
        ptx,pty,rectWidth,rectHeight= self._point2ClientCoord(p1, p2)
        dc.SetClippingRegion(ptx,pty,rectWidth,rectHeight)
        # Draw the lines and markers
        #start = _time.clock()
        graphics.draw(dc)
        # print "entire graphics drawing took: %f second"%(_time.clock() - start)
        # remove the clipping region
        dc.DestroyClippingRegion()
        dc.EndDrawing()

        self._adjustScrollbars()
        
    def Redraw(self, dc=None):
        """Redraw the existing plot."""
        if self.last_draw is not None:
            graphics, xAxis, yAxis= self.last_draw
            self._Draw(graphics,xAxis,yAxis,dc)

    def Clear(self):
        """Erase the window."""
        self.last_PointLabel = None        #reset pointLabel
        dc = wx.BufferedDC(wx.ClientDC(self.canvas), self._Buffer)
        dc.Clear()
        self.last_draw = None

    def Zoom(self, Center, Ratio):
        """ Zoom on the plot
            Centers on the X,Y coords given in Center
            Zooms by the Ratio = (Xratio, Yratio) given
        """
        self.last_PointLabel = None   #reset maker
        x,y = Center
        if self.last_draw != None:
            (graphics, xAxis, yAxis) = self.last_draw
            w = (xAxis[1] - xAxis[0]) * Ratio[0]
            h = (yAxis[1] - yAxis[0]) * Ratio[1]
            xAxis = ( x - w/2, x + w/2 )
            yAxis = ( y - h/2, y + h/2 )
            self._Draw(graphics, xAxis, yAxis)
        
    def GetClosestPoints(self, pntXY, pointScaled= True):
        """Returns list with
            [curveNumber, legend, index of closest point, pointXY, scaledXY, distance]
            list for each curve.
            Returns [] if no curves are being plotted.
            
            x, y in user coords
            if pointScaled == True based on screen coords
            if pointScaled == False based on user coords
        """
        if self.last_draw == None:
            #no graph available
            return []
        graphics, xAxis, yAxis= self.last_draw
        l = []
        for curveNum,obj in enumerate(graphics):
            #check there are points in the curve
            if len(obj.points) == 0:
                continue  #go to next obj
            #[curveNumber, legend, index of closest point, pointXY, scaledXY, distance]
            cn = [curveNum]+ [obj.getLegend()]+ obj.getClosestPoint( pntXY, pointScaled)
            l.append(cn)
        return l

    def GetClosetPoint(self, pntXY, pointScaled= True):
        """Returns list with
            [curveNumber, legend, index of closest point, pointXY, scaledXY, distance]
            list for only the closest curve.
            Returns [] if no curves are being plotted.
            
            x, y in user coords
            if pointScaled == True based on screen coords
            if pointScaled == False based on user coords
        """
        #closest points on screen based on screen scaling (pointScaled= True)
        #list [curveNumber, index, pointXY, scaledXY, distance] for each curve
        closestPts= self.GetClosestPoints(pntXY, pointScaled)
        if closestPts == []:
            return []  #no graph present
        #find one with least distance
        dists = [c[-1] for c in closestPts]
        mdist = min(dists)  #Min dist
        i = dists.index(mdist)  #index for min dist
        return closestPts[i]  #this is the closest point on closest curve

    def UpdatePointLabel(self, mDataDict):
        """Updates the pointLabel point on screen with data contained in
            mDataDict.

            mDataDict will be passed to your function set by
            SetPointLabelFunc.  It can contain anything you
            want to display on the screen at the scaledXY point
            you specify.

            This function can be called from parent window with onClick,
            onMotion events etc.            
        """
        if self.last_PointLabel != None:
            #compare pointXY
            if _Numeric.sometrue(mDataDict["pointXY"] != self.last_PointLabel["pointXY"]):
                #closest changed
                self._drawPointLabel(self.last_PointLabel) #erase old
                self._drawPointLabel(mDataDict) #plot new
        else:
            #just plot new with no erase
            self._drawPointLabel(mDataDict) #plot new
        #save for next erase
        self.last_PointLabel = mDataDict

    # event handlers **********************************
    def OnMotion(self, event):
        if self._zoomEnabled and event.LeftIsDown():
            if self._hasDragged:
                self._drawRubberBand(self._zoomCorner1, self._zoomCorner2) # remove old
            else:
                self._hasDragged= True
            self._zoomCorner2[0], self._zoomCorner2[1] = self._getXY(event)
            self._drawRubberBand(self._zoomCorner1, self._zoomCorner2) # add new
        elif self._dragEnabled and event.LeftIsDown():
            coordinates = event.GetPosition()
            newpos, oldpos = map(_Numeric.array, map(self.PositionScreenToUser, [coordinates, self._screenCoordinates]))
            dist = newpos-oldpos
            self._screenCoordinates = coordinates

            if self.last_draw is not None:
                graphics, xAxis, yAxis= self.last_draw
                yAxis -= dist[1]
                xAxis -= dist[0]
                self._Draw(graphics,xAxis,yAxis)
            
    def OnMouseLeftDown(self,event):
        self._zoomCorner1[0], self._zoomCorner1[1]= self._getXY(event)
        self._screenCoordinates = _Numeric.array(event.GetPosition())
        if self._dragEnabled:
            self.SetCursor(self.GrabHandCursor)
            self.canvas.CaptureMouse()

    def OnMouseLeftUp(self, event):
        if self._zoomEnabled:
            if self._hasDragged == True:
                self._drawRubberBand(self._zoomCorner1, self._zoomCorner2) # remove old
                self._zoomCorner2[0], self._zoomCorner2[1]= self._getXY(event)
                self._hasDragged = False  # reset flag
                minX, minY= _Numeric.minimum( self._zoomCorner1, self._zoomCorner2)
                maxX, maxY= _Numeric.maximum( self._zoomCorner1, self._zoomCorner2)
                self.last_PointLabel = None        #reset pointLabel
                if self.last_draw != None:
                    self._Draw(self.last_draw[0], xAxis = (minX,maxX), yAxis = (minY,maxY), dc = None)
            #else: # A box has not been drawn, zoom in on a point
            ## this interfered with the double click, so I've disables it.
            #    X,Y = self._getXY(event)
            #    self.Zoom( (X,Y), (self._zoomInFactor,self._zoomInFactor) )
        if self._dragEnabled:
            self.SetCursor(self.HandCursor)
            if self.canvas.HasCapture():
                self.canvas.ReleaseMouse()

    def OnMouseDoubleClick(self,event):
        if self._zoomEnabled:
            # Give a little time for the click to be totally finished
            # before (possibly) removing the scrollbars and trigering
            # size events, etc.
            wx.FutureCall(200,self.Reset)
        
    def OnMouseRightDown(self,event):
        if self._zoomEnabled:
            X,Y = self._getXY(event)
            self.Zoom( (X,Y), (self._zoomOutFactor, self._zoomOutFactor) )

    def OnPaint(self, event):
        # All that is needed here is to draw the buffer to screen
        if self.last_PointLabel != None:
            self._drawPointLabel(self.last_PointLabel) #erase old
            self.last_PointLabel = None
        dc = wx.BufferedPaintDC(self.canvas, self._Buffer)

    def OnSize(self,event):
        # The Buffer init is done here, to make sure the buffer is always
        # the same size as the Window
        Size  = self.canvas.GetClientSize()
        Size.width = max(1, Size.width)
        Size.height = max(1, Size.height)
        
        # Make new offscreen bitmap: this bitmap will always have the
        # current drawing in it, so it can be used to save the image to
        # a file, or whatever.
        self._Buffer = wx.EmptyBitmap(Size.width, Size.height)
        self._setSize()

        self.last_PointLabel = None        #reset pointLabel

        if self.last_draw is None:
            self.Clear()
        else:
            graphics, xSpec, ySpec = self.last_draw
            self._Draw(graphics,xSpec,ySpec)

    def OnLeave(self, event):
        """Used to erase pointLabel when mouse outside window"""
        if self.last_PointLabel != None:
            self._drawPointLabel(self.last_PointLabel) #erase old
            self.last_PointLabel = None

    def OnScroll(self, evt):
        if not self._adjustingSB:
            self._sb_ignore = True
            sbpos = evt.GetPosition()
        
            if evt.GetOrientation() == wx.VERTICAL:
                fullrange,pagesize = self.sb_vert.GetRange(),self.sb_vert.GetPageSize()
                sbpos = fullrange-pagesize-sbpos
                dist = sbpos*self._sb_yunit-(self._getYCurrentRange()[0]-self._sb_yfullrange[0])
                self.ScrollUp(dist)
            
            if evt.GetOrientation() == wx.HORIZONTAL:
                dist = sbpos*self._sb_xunit-(self._getXCurrentRange()[0]-self._sb_xfullrange[0])
                self.ScrollRight(dist)
               
    # Private Methods **************************************************
    def _setSize(self, width=None, height=None):
        """DC width and height."""
        if width == None:
            (self.width,self.height) = self.canvas.GetClientSize()
        else:
            self.width, self.height= width,height    
        self.plotbox_size = 0.97*_Numeric.array([self.width, self.height])
        xo = 0.5*(self.width-self.plotbox_size[0])
        yo = self.height-0.5*(self.height-self.plotbox_size[1])
        self.plotbox_origin = _Numeric.array([xo, yo])
    
    def _setPrinterScale(self, scale):
        """Used to thicken lines and increase marker size for print out."""
        # line thickness on printer is very thin at 600 dot/in. Markers small
        self.printerScale= scale
     
    def _printDraw(self, printDC):
        """Used for printing."""
        if self.last_draw != None:
            graphics, xSpec, ySpec= self.last_draw
            self._Draw(graphics,xSpec,ySpec,printDC)

    def _drawPointLabel(self, mDataDict):
        """Draws and erases pointLabels"""
        width = self._Buffer.GetWidth()
        height = self._Buffer.GetHeight()
        tmp_Buffer = wx.EmptyBitmap(width,height)
        dcs = wx.MemoryDC()
        dcs.SelectObject(tmp_Buffer)
        dcs.Clear()
        dcs.BeginDrawing()
        self._pointLabelFunc(dcs,mDataDict)  #custom user pointLabel function
        dcs.EndDrawing()

        dc = wx.ClientDC( self.canvas )
        #this will erase if called twice
        dc.Blit(0, 0, width, height, dcs, 0, 0, wx.EQUIV)  #(NOT src) XOR dst
        

    def _drawLegend(self,dc,graphics,rhsW,topH,legendBoxWH, legendSymExt, legendTextExt):
        """Draws legend symbols and text"""
        # top right hand corner of graph box is ref corner
        trhc= self.plotbox_origin+ (self.plotbox_size-[rhsW,topH])*[1,-1]
        legendLHS= .091* legendBoxWH[0]  # border space between legend sym and graph box
        lineHeight= max(legendSymExt[1], legendTextExt[1]) * 1.1 #1.1 used as space between lines
        dc.SetFont(self._getFont(self._fontSizeLegend))
        for i in range(len(graphics)):
            o = graphics[i]
            s= i*lineHeight
            if isinstance(o,PolyMarker):
                # draw marker with legend
                pnt= (trhc[0]+legendLHS+legendSymExt[0]/2., trhc[1]+s+lineHeight/2.)
                o.draw(dc, self.printerScale, coord= _Numeric.array([pnt]))
            elif isinstance(o,PolyLine):
                # draw line with legend
                pnt1= (trhc[0]+legendLHS, trhc[1]+s+lineHeight/2.)
                pnt2= (trhc[0]+legendLHS+legendSymExt[0], trhc[1]+s+lineHeight/2.)
                o.draw(dc, self.printerScale, coord= _Numeric.array([pnt1,pnt2]))
            else:
                raise TypeError, "object is neither PolyMarker or PolyLine instance"
            # draw legend txt
            pnt= (trhc[0]+legendLHS+legendSymExt[0], trhc[1]+s+lineHeight/2.-legendTextExt[1]/2)
            dc.DrawText(o.getLegend(),pnt[0],pnt[1])
        dc.SetFont(self._getFont(self._fontSizeAxis)) # reset

    def _titleLablesWH(self, dc, graphics):
        """Draws Title and labels and returns width and height for each"""
        # TextExtents for Title and Axis Labels
        dc.SetFont(self._getFont(self._fontSizeTitle))
        title= graphics.getTitle()
        titleWH= dc.GetTextExtent(title)
        dc.SetFont(self._getFont(self._fontSizeAxis))
        xLabel, yLabel= graphics.getXLabel(),graphics.getYLabel()
        xLabelWH= dc.GetTextExtent(xLabel)
        yLabelWH= dc.GetTextExtent(yLabel)
        return titleWH, xLabelWH, yLabelWH
    
    def _legendWH(self, dc, graphics):
        """Returns the size in screen units for legend box"""
        if self._legendEnabled != True:
            legendBoxWH= symExt= txtExt= (0,0)
        else:
            # find max symbol size
            symExt= graphics.getSymExtent(self.printerScale)
            # find max legend text extent
            dc.SetFont(self._getFont(self._fontSizeLegend))
            txtList= graphics.getLegendNames()
            txtExt= dc.GetTextExtent(txtList[0])
            for txt in graphics.getLegendNames()[1:]:
                txtExt= _Numeric.maximum(txtExt,dc.GetTextExtent(txt))
            maxW= symExt[0]+txtExt[0]    
            maxH= max(symExt[1],txtExt[1])
            # padding .1 for lhs of legend box and space between lines
            maxW= maxW* 1.1
            maxH= maxH* 1.1 * len(txtList)
            dc.SetFont(self._getFont(self._fontSizeAxis))
            legendBoxWH= (maxW,maxH)
        return (legendBoxWH, symExt, txtExt)

    def _drawRubberBand(self, corner1, corner2):
        """Draws/erases rect box from corner1 to corner2"""
        ptx,pty,rectWidth,rectHeight= self._point2ClientCoord(corner1, corner2)
        # draw rectangle
        dc = wx.ClientDC( self.canvas )
        dc.BeginDrawing()                 
        dc.SetPen(wx.Pen(wx.BLACK))
        dc.SetBrush(wx.Brush( wx.WHITE, wx.TRANSPARENT ) )
        dc.SetLogicalFunction(wx.INVERT)
        dc.DrawRectangle( ptx,pty, rectWidth,rectHeight)
        dc.SetLogicalFunction(wx.COPY)
        dc.EndDrawing()

    def _getFont(self,size):
        """Take font size, adjusts if printing and returns wx.Font"""
        s = size*self.printerScale
        of = self.GetFont()
        # Linux speed up to get font from cache rather than X font server
        key = (int(s), of.GetFamily (), of.GetStyle (), of.GetWeight ())
        font = self._fontCache.get (key, None)
        if font:
            return font                 # yeah! cache hit
        else:
            font =  wx.Font(int(s), of.GetFamily(), of.GetStyle(), of.GetWeight())
            self._fontCache[key] = font
            return font


    def _point2ClientCoord(self, corner1, corner2):
        """Converts user point coords to client screen int coords x,y,width,height"""
        c1= _Numeric.array(corner1)
        c2= _Numeric.array(corner2)
        # convert to screen coords
        pt1= c1*self._pointScale+self._pointShift
        pt2= c2*self._pointScale+self._pointShift
        # make height and width positive
        pul= _Numeric.minimum(pt1,pt2) # Upper left corner
        plr= _Numeric.maximum(pt1,pt2) # Lower right corner
        rectWidth, rectHeight= plr-pul
        ptx,pty= pul
        return ptx, pty, rectWidth, rectHeight 
    
    def _axisInterval(self, spec, lower, upper):
        """Returns sensible axis range for given spec"""
        if spec == 'none' or spec == 'min':
            if lower == upper:
                return lower-0.5, upper+0.5
            else:
                return lower, upper
        elif spec == 'auto':
            range = upper-lower
            if range == 0.:
                return lower-0.5, upper+0.5
            log = _Numeric.log10(range)
            power = _Numeric.floor(log)
            fraction = log-power
            if fraction <= 0.05:
                power = power-1
            grid = 10.**power
            lower = lower - lower % grid
            mod = upper % grid
            if mod != 0:
                upper = upper - mod + grid
            return lower, upper
        elif type(spec) == type(()):
            lower, upper = spec
            if lower <= upper:
                return lower, upper
            else:
                return upper, lower
        else:
            raise ValueError, str(spec) + ': illegal axis specification'

    def _drawAxes(self, dc, p1, p2, scale, shift, xticks, yticks):
        
        penWidth= self.printerScale        # increases thickness for printing only
        dc.SetPen(wx.Pen(self._gridColour, penWidth))
        
        # set length of tick marks--long ones make grid
        if self._gridEnabled:
            x,y,width,height= self._point2ClientCoord(p1,p2)
            if self._gridEnabled == 'Horizontal':
                yTickLength= width/2.0 +1
                xTickLength= 3 * self.printerScale
            elif self._gridEnabled == 'Vertical':
                yTickLength= 3 * self.printerScale
                xTickLength= height/2.0 +1
            else:
                yTickLength= width/2.0 +1
                xTickLength= height/2.0 +1
        else:
            yTickLength= 3 * self.printerScale  # lengthens lines for printing
            xTickLength= 3 * self.printerScale
        
        if self._xSpec is not 'none':
            lower, upper = p1[0],p2[0]
            text = 1
            for y, d in [(p1[1], -xTickLength), (p2[1], xTickLength)]:   # miny, maxy and tick lengths
                a1 = scale*_Numeric.array([lower, y])+shift
                a2 = scale*_Numeric.array([upper, y])+shift
                dc.DrawLine(a1[0],a1[1],a2[0],a2[1])  # draws upper and lower axis line
                for x, label in xticks:
                    pt = scale*_Numeric.array([x, y])+shift
                    dc.DrawLine(pt[0],pt[1],pt[0],pt[1] + d) # draws tick mark d units
                    if text:
                        dc.DrawText(label,pt[0],pt[1])
                text = 0  # axis values not drawn on top side

        if self._ySpec is not 'none':
            lower, upper = p1[1],p2[1]
            text = 1
            h = dc.GetCharHeight()
            for x, d in [(p1[0], -yTickLength), (p2[0], yTickLength)]:
                a1 = scale*_Numeric.array([x, lower])+shift
                a2 = scale*_Numeric.array([x, upper])+shift
                dc.DrawLine(a1[0],a1[1],a2[0],a2[1])
                for y, label in yticks:
                    pt = scale*_Numeric.array([x, y])+shift
                    dc.DrawLine(pt[0],pt[1],pt[0]-d,pt[1])
                    if text:
                        dc.DrawText(label,pt[0]-dc.GetTextExtent(label)[0],
                                    pt[1]-0.5*h)
                text = 0    # axis values not drawn on right side

    def _xticks(self, *args):
        if self._logscale[0]:
            return self._logticks(*args)
        else:
            return self._ticks(*args)
    
    def _yticks(self, *args):
        if self._logscale[1]:
            return self._logticks(*args)
        else:
            return self._ticks(*args)
        
    def _logticks(self, lower, upper):
        #lower,upper = map(_Numeric.log10,[lower,upper])
        #print 'logticks',lower,upper
        ticks = []
        mag = _Numeric.power(10,_Numeric.floor(lower))
        if upper-lower > 6:
            t = _Numeric.power(10,_Numeric.ceil(lower))
            base = _Numeric.power(10,_Numeric.floor((upper-lower)/6))
            def inc(t):
                return t*base-t
        else:
            t = _Numeric.ceil(_Numeric.power(10,lower)/mag)*mag
            def inc(t):
                return 10**int(_Numeric.floor(_Numeric.log10(t)+1e-16))
        majortick = int(_Numeric.log10(mag))
        while t <= pow(10,upper):
            if majortick != int(_Numeric.floor(_Numeric.log10(t)+1e-16)):
                majortick = int(_Numeric.floor(_Numeric.log10(t)+1e-16))
                ticklabel = '1e%d'%majortick
            else:
                if upper-lower < 2:
                    minortick = int(t/pow(10,majortick)+.5)
                    ticklabel = '%de%d'%(minortick,majortick)
                else:
                    ticklabel = ''
            ticks.append((_Numeric.log10(t), ticklabel))
            t += inc(t)
        if len(ticks) == 0:
            ticks = [(0,'')]
        return ticks
    
    def _ticks(self, lower, upper):
        ideal = (upper-lower)/7.
        log = _Numeric.log10(ideal)
        power = _Numeric.floor(log)
        fraction = log-power
        factor = 1.
        error = fraction
        for f, lf in self._multiples:
            e = _Numeric.fabs(fraction-lf)
            if e < error:
                error = e
                factor = f
        grid = factor * 10.**power
        if power > 4 or power < -4:
            format = '%+7.1e'        
        elif power >= 0:
            digits = max(1, int(power))
            format = '%' + `digits`+'.0f'
        else:
            digits = -int(power)
            format = '%'+`digits+2`+'.'+`digits`+'f'
        ticks = []
        t = -grid*_Numeric.floor(-lower/grid)
        while t <= upper:
            ticks.append( (t, format % (t,)) )
            t = t + grid
        return ticks

    _multiples = [(2., _Numeric.log10(2.)), (5., _Numeric.log10(5.))]


    def _adjustScrollbars(self):
        if self._sb_ignore:
            self._sb_ignore = False
            return

        self._adjustingSB = True
        needScrollbars = False
        
        # horizontal scrollbar
        r_current = self._getXCurrentRange()
        r_max = list(self._getXMaxRange())
        sbfullrange = float(self.sb_hor.GetRange())

        r_max[0] = min(r_max[0],r_current[0])
        r_max[1] = max(r_max[1],r_current[1])
            
        self._sb_xfullrange = r_max

        unit = (r_max[1]-r_max[0])/float(self.sb_hor.GetRange())
        pos = int((r_current[0]-r_max[0])/unit)
        
        if pos >= 0:
            pagesize = int((r_current[1]-r_current[0])/unit)

            self.sb_hor.SetScrollbar(pos, pagesize, sbfullrange, pagesize)
            self._sb_xunit = unit
            needScrollbars = needScrollbars or (pagesize != sbfullrange)
        else:
            self.sb_hor.SetScrollbar(0, 1000, 1000, 1000)

        # vertical scrollbar
        r_current = self._getYCurrentRange()
        r_max = list(self._getYMaxRange())
        sbfullrange = float(self.sb_vert.GetRange())

        r_max[0] = min(r_max[0],r_current[0])
        r_max[1] = max(r_max[1],r_current[1])
            
        self._sb_yfullrange = r_max
        
        unit = (r_max[1]-r_max[0])/sbfullrange
        pos = int((r_current[0]-r_max[0])/unit)
        
        if pos >= 0:
            pagesize = int((r_current[1]-r_current[0])/unit)
            pos = (sbfullrange-1-pos-pagesize)
            self.sb_vert.SetScrollbar(pos, pagesize, sbfullrange, pagesize)
            self._sb_yunit = unit
            needScrollbars = needScrollbars or (pagesize != sbfullrange)
        else:
            self.sb_vert.SetScrollbar(0, 1000, 1000, 1000)

        self.SetShowScrollbars(needScrollbars)
        self._adjustingSB = False

#-------------------------------------------------------------------------------
# Used to layout the printer page

class PlotPrintout(wx.Printout):
    """Controls how the plot is made in printing and previewing"""
    # Do not change method names in this class,
    # we have to override wx.Printout methods here!
    def __init__(self, graph):
        """graph is instance of plotCanvas to be printed or previewed"""
        wx.Printout.__init__(self)
        self.graph = graph

    def HasPage(self, page):
        if page == 1:
            return True
        else:
            return False

    def GetPageInfo(self):
        return (1, 1, 1, 1)  # disable page numbers

    def OnPrintPage(self, page):
        dc = self.GetDC()  # allows using floats for certain functions
##        print "PPI Printer",self.GetPPIPrinter()
##        print "PPI Screen", self.GetPPIScreen()
##        print "DC GetSize", dc.GetSize()
##        print "GetPageSizePixels", self.GetPageSizePixels()
        # Note PPIScreen does not give the correct number
        # Calulate everything for printer and then scale for preview
        PPIPrinter= self.GetPPIPrinter()        # printer dots/inch (w,h)
        #PPIScreen= self.GetPPIScreen()          # screen dots/inch (w,h)
        dcSize= dc.GetSize()                    # DC size
        pageSize= self.GetPageSizePixels() # page size in terms of pixcels
        clientDcSize= self.graph.GetClientSize()
        
        # find what the margins are (mm)
        margLeftSize,margTopSize= self.graph.pageSetupData.GetMarginTopLeft()
        margRightSize, margBottomSize= self.graph.pageSetupData.GetMarginBottomRight()

        # calculate offset and scale for dc
        pixLeft= margLeftSize*PPIPrinter[0]/25.4  # mm*(dots/in)/(mm/in)
        pixRight= margRightSize*PPIPrinter[0]/25.4    
        pixTop= margTopSize*PPIPrinter[1]/25.4
        pixBottom= margBottomSize*PPIPrinter[1]/25.4

        plotAreaW= pageSize[0]-(pixLeft+pixRight)
        plotAreaH= pageSize[1]-(pixTop+pixBottom)

        # ratio offset and scale to screen size if preview
        if self.IsPreview():
            ratioW= float(dcSize[0])/pageSize[0]
            ratioH= float(dcSize[1])/pageSize[1]
            pixLeft *= ratioW
            pixTop *= ratioH
            plotAreaW *= ratioW
            plotAreaH *= ratioH
        
        # rescale plot to page or preview plot area
        self.graph._setSize(plotAreaW,plotAreaH)
        
        # Set offset and scale
        dc.SetDeviceOrigin(pixLeft,pixTop)

        # Thicken up pens and increase marker size for printing
        ratioW= float(plotAreaW)/clientDcSize[0]
        ratioH= float(plotAreaH)/clientDcSize[1]
        aveScale= (ratioW+ratioH)/2
        self.graph._setPrinterScale(aveScale)  # tickens up pens for printing

        self.graph._printDraw(dc)
        # rescale back to original
        self.graph._setSize()
        self.graph._setPrinterScale(1)
        self.graph.Redraw()     #to get point label scale and shift correct

        return True


#----------------------------------------------------------------------
from wx import ImageFromStream, BitmapFromImage
import cStringIO, zlib


def getMagPlusData():
    return zlib.decompress(
'x\xda\x01*\x01\xd5\xfe\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x18\
\x00\x00\x00\x18\x08\x06\x00\x00\x00\xe0w=\xf8\x00\x00\x00\x04sBIT\x08\x08\
\x08\x08|\x08d\x88\x00\x00\x00\xe1IDATx\x9c\xb5U\xd1\x0e\xc4 \x08\xa3n\xff\
\xff\xc5\xdb\xb8\xa7\xee<\x04\x86gFb\xb2\x88\xb6\x14\x90\x01m\x937m\x8f\x1c\
\xd7yh\xe4k\xdb\x8e*\x01<\x05\x04\x07F\x1cU\x9d"\x19\x14\\\xe7\xa1\x1e\xf07"\
\x90H+$?\x04\x16\x9c\xd1z\x04\x00J$m\x06\xdc\xee\x03Hku\x13\xd8C\x16\x84+"O\
\x1b\xa2\x07\xca"\xb7\xc6sY\xbdD\x926\xf5.\xce\x06!\xd2)x\xcb^\'\x08S\xe4\
\xe5x&5\xb4[A\xb5h\xb4j=\x9a\xc8\xf8\xecm\xd4\\\x9e\xdf\xbb?\x10\xf0P\x06\
\x12\xed?=\xb6a\xd8=\xcd\xa2\xc8T\xd5U2t\x11\x95d\xa3"\x9aQ\x9e\x12\xb7M\x19\
I\x9f\xff\x1e\xd8\xa63#q\xff\x07U\x8b\xd2\xd9\xa7k\xe9\xa1U\x94,\xbf\xe4\x88\
\xe4\xf6\xaf\x12x$}\x8a\xc2Q\xf1\'\x89\xf2\x9b\xfbKE\xae\xd8\x07+\xd2\xa7c\
\xdf\x0e\xc3D\x00\x00\x00\x00IEND\xaeB`\x82\xe2ovy' )

def getMagPlusBitmap():
    return BitmapFromImage(getMagPlusImage())

def getMagPlusImage():
    stream = cStringIO.StringIO(getMagPlusData())
    return ImageFromStream(stream)

#----------------------------------------------------------------------
def getGrabHandData():
    return zlib.decompress(
'x\xda\x01Z\x01\xa5\xfe\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x18\
\x00\x00\x00\x18\x08\x06\x00\x00\x00\xe0w=\xf8\x00\x00\x00\x04sBIT\x08\x08\
\x08\x08|\x08d\x88\x00\x00\x01\x11IDATx\x9c\xb5U\xd1\x12\x830\x08Kh\xff\xff\
\x8b7\xb3\x97\xd1C\xa4Zw\x93;\x1fJ1\t\x98VJ\x92\xb5N<\x14\x04 I\x00\x80H\xb4\
\xbd_\x8a9_{\\\x89\xf2z\x02\x18/J\x82\xb5\xce\xed\xfd\x12\xc9\x91\x03\x00_\
\xc7\xda\x8al\x00{\xfdW\xfex\xf2zeO\x92h\xed\x80\x05@\xa45D\xc5\xb3\x98u\x12\
\xf7\xab.\xa9\xd0k\x1eK\x95\xbb\x1a]&0\x92\xf0\'\xc6]gI\xda\tsr\xab\x8aI\x1e\
\\\xe3\xa4\x0e\xb4*`7"\x07\x8f\xaa"x\x05\xe0\xdfo6B\xf3\x17\xe3\x98r\xf1\xaf\
\x07\xd1Z\'%\x95\x0erW\xac\x8c\xe3\xe0\xfd\xd8AN\xae\xb8\xa3R\x9as>\x11\x8bl\
yD\xab\x1f\xf3\xec\x1cY\x06\x89$\xbf\x80\xfb\x14\\dw\x90x\x12\xa3+\xeeD\x16%\
I\xe3\x1c\xb8\xc7c\'\xd5Y8S\x9f\xc3Zg\xcf\x89\xe8\xaao\'\xbbk{U\xfd\xc0\xacX\
\xab\xbb\xe8\xae\xfa)AEr\x15g\x86(\t\xfe\x19\xa4\xb5\xe9f\xfem\xde\xdd\xbf$\
\xf8G<>\xa2\xc7\t>\tE\xfc\x8a\xf6\x8dqc\x00\x00\x00\x00IEND\xaeB`\x82\xdb\
\xd0\x8f\n' )

def getGrabHandBitmap():
    return BitmapFromImage(getGrabHandImage())

def getGrabHandImage():
    stream = cStringIO.StringIO(getGrabHandData())
    return ImageFromStream(stream)

#----------------------------------------------------------------------
def getHandData():
    return zlib.decompress(
'x\xda\x01Y\x01\xa6\xfe\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x18\
\x00\x00\x00\x18\x08\x06\x00\x00\x00\xe0w=\xf8\x00\x00\x00\x04sBIT\x08\x08\
\x08\x08|\x08d\x88\x00\x00\x01\x10IDATx\x9c\xad\x96\xe1\x02\xc2 \x08\x849\
\xf5\xfd\x9fx\xdb\xf5\'\x8c!\xa8\xab\xee\x975\xe5\x83\x0b\\@\xa9\xb2\xab\xeb\
<\xa8\xebR\x1bv\xce\xb4\'\xc1\x81OL\x92\xdc\x81\x0c\x00\x1b\x88\xa4\x94\xda\
\xe0\x83\x8b\x88\x00\x10\x92\xcb\x8a\xca,K\x1fT\xa1\x1e\x04\xe0f_\n\x88\x02\
\xf1:\xc3\x83>\x81\x0c\x92\x02v\xe5+\xba\xce\x83\xb7f\xb8\xd1\x9c\x8fz8\xb2*\
\x93\xb7l\xa8\xe0\x9b\xa06\xb8]_\xe7\xc1\x01\x10U\xe1m\x98\xc9\xefm"ck\xea\
\x1a\x80\xa0Th\xb9\xfd\x877{V*Qk\xda,\xb4\x8b\xf4;[\xa1\xcf6\xaa4\x9cd\x85X\
\xb0\r\\j\x83\x9dd\x92\xc3 \xf6\xbd\xab\x0c2\x05\xc0p\x9a\xa7]\xf4\x14\x18]3\
7\x80}h?\xff\xa2\xa2\xe5e\x90\xact\xaf\xe8B\x14y[4\x83|\x13\xdc\x9e\xeb\x16e\
\x90\xa7\xf2I\rw\x91\x87d\xd7p\x96\xbd\xd70\x07\xda\xe3v\x9a\xf5\xc5\xb2\xb2\
+\xb24\xbc\xaew\xedZe\x9f\x02"\xc8J\xdb\x83\xf6oa\xf5\xb7\xa5\xbf8\x12\xffW\
\xcf_\xbd;\xe4\x8c\x03\x10\xdb^\x00\x00\x00\x00IEND\xaeB`\x82\xd1>\x97B' )

def getHandBitmap():
    return BitmapFromImage(getHandImage())

def getHandImage():
    stream = cStringIO.StringIO(getHandData())
    return ImageFromStream(stream)



#---------------------------------------------------------------------------
# if running standalone...
#
#     ...a sample implementation using the above
#

def _draw1Objects():
    # 100 points sin function, plotted as green circles
    data1 = 2.*_Numeric.pi*_Numeric.arange(200)/200.
    data1.shape = (100, 2)
    data1[:,1] = _Numeric.sin(data1[:,0])
    markers1 = PolyMarker(data1, legend='Green Markers', colour='green', marker='circle',size=1)

    # 50 points cos function, plotted as red line
    data1 = 2.*_Numeric.pi*_Numeric.arange(100)/100.
    data1.shape = (50,2)
    data1[:,1] = _Numeric.cos(data1[:,0])
    lines = PolyLine(data1, legend= 'Red Line', colour='red')

    # A few more points...
    pi = _Numeric.pi
    markers2 = PolyMarker([(0., 0.), (pi/4., 1.), (pi/2, 0.),
                          (3.*pi/4., -1)], legend='Cross Legend', colour='blue',
                          marker='cross')
    
    return PlotGraphics([markers1, lines, markers2],"Graph Title", "X Axis", "Y Axis")

def _draw2Objects():
    # 100 points sin function, plotted as green dots
    data1 = 2.*_Numeric.pi*_Numeric.arange(200)/200.
    data1.shape = (100, 2)
    data1[:,1] = _Numeric.sin(data1[:,0])
    line1 = PolyLine(data1, legend='Green Line', colour='green', width=6, style=wx.DOT)

    # 50 points cos function, plotted as red dot-dash
    data1 = 2.*_Numeric.pi*_Numeric.arange(100)/100.
    data1.shape = (50,2)
    data1[:,1] = _Numeric.cos(data1[:,0])
    line2 = PolyLine(data1, legend='Red Line', colour='red', width=3, style= wx.DOT_DASH)

    # A few more points...
    pi = _Numeric.pi
    markers1 = PolyMarker([(0., 0.), (pi/4., 1.), (pi/2, 0.),
                          (3.*pi/4., -1)], legend='Cross Hatch Square', colour='blue', width= 3, size= 6,
                          fillcolour= 'red', fillstyle= wx.CROSSDIAG_HATCH,
                          marker='square')

    return PlotGraphics([markers1, line1, line2], "Big Markers with Different Line Styles")

def _draw3Objects():
    markerList= ['circle', 'dot', 'square', 'triangle', 'triangle_down',
                'cross', 'plus', 'circle']
    m=[]
    for i in range(len(markerList)):
        m.append(PolyMarker([(2*i+.5,i+.5)], legend=markerList[i], colour='blue',
                          marker=markerList[i]))
    return PlotGraphics(m, "Selection of Markers", "Minimal Axis", "No Axis")

def _draw4Objects():
    # 25,000 point line
    data1 = _Numeric.arange(5e5,1e6,10)
    data1.shape = (25000, 2)
    line1 = PolyLine(data1, legend='Wide Line', colour='green', width=5)

    # A few more points...
    markers2 = PolyMarker(data1, legend='Square', colour='blue',
                          marker='square')
    return PlotGraphics([line1, markers2], "25,000 Points", "Value X", "")

def _draw5Objects():
    # Empty graph with axis defined but no points/lines
    points=[]
    line1 = PolyLine(points, legend='Wide Line', colour='green', width=5)
    return PlotGraphics([line1], "Empty Plot With Just Axes", "Value X", "Value Y")

def _draw6Objects():
    # Bar graph
    points1=[(1,0), (1,10)]
    line1 = PolyLine(points1, colour='green', legend='Feb.', width=10)
    points1g=[(2,0), (2,4)]
    line1g = PolyLine(points1g, colour='red', legend='Mar.', width=10)
    points1b=[(3,0), (3,6)]
    line1b = PolyLine(points1b, colour='blue', legend='Apr.', width=10)

    points2=[(4,0), (4,12)]
    line2 = PolyLine(points2, colour='Yellow', legend='May', width=10)
    points2g=[(5,0), (5,8)]
    line2g = PolyLine(points2g, colour='orange', legend='June', width=10)
    points2b=[(6,0), (6,4)]
    line2b = PolyLine(points2b, colour='brown', legend='July', width=10)

    return PlotGraphics([line1, line1g, line1b, line2, line2g, line2b],
                        "Bar Graph - (Turn on Grid, Legend)", "Months", "Number of Students")
def _draw7Objects():
    # Empty graph with axis defined but no points/lines
    x = _Numeric.arange(1,1000,1)
    y1 = 4.5*x**2
    y2 = 2.2*x**3
    points1 = _Numeric.transpose([x,y1])
    points2 = _Numeric.transpose([x,y2])
    line1 = PolyLine(points1, legend='quadratic', colour='blue', width=1)
    line2 = PolyLine(points2, legend='cubic', colour='red', width=1)
    return PlotGraphics([line1,line2], "double log plot", "Value X", "Value Y")


class TestFrame(wx.Frame):
    def __init__(self, parent, id, title):
        wx.Frame.__init__(self, parent, id, title,
                          wx.DefaultPosition, (600, 400))

        # Now Create the menu bar and items
        self.mainmenu = wx.MenuBar()

        menu = wx.Menu()
        menu.Append(200, 'Page Setup...', 'Setup the printer page')
        self.Bind(wx.EVT_MENU, self.OnFilePageSetup, id=200)
        
        menu.Append(201, 'Print Preview...', 'Show the current plot on page')
        self.Bind(wx.EVT_MENU, self.OnFilePrintPreview, id=201)
        
        menu.Append(202, 'Print...', 'Print the current plot')
        self.Bind(wx.EVT_MENU, self.OnFilePrint, id=202)
        
        menu.Append(203, 'Save Plot...', 'Save current plot')
        self.Bind(wx.EVT_MENU, self.OnSaveFile, id=203)
        
        menu.Append(205, 'E&xit', 'Enough of this already!')
        self.Bind(wx.EVT_MENU, self.OnFileExit, id=205)
        self.mainmenu.Append(menu, '&File')

        menu = wx.Menu()
        menu.Append(206, 'Draw1', 'Draw plots1')
        self.Bind(wx.EVT_MENU,self.OnPlotDraw1, id=206)
        menu.Append(207, 'Draw2', 'Draw plots2')
        self.Bind(wx.EVT_MENU,self.OnPlotDraw2, id=207)
        menu.Append(208, 'Draw3', 'Draw plots3')
        self.Bind(wx.EVT_MENU,self.OnPlotDraw3, id=208)
        menu.Append(209, 'Draw4', 'Draw plots4')
        self.Bind(wx.EVT_MENU,self.OnPlotDraw4, id=209)
        menu.Append(210, 'Draw5', 'Draw plots5')
        self.Bind(wx.EVT_MENU,self.OnPlotDraw5, id=210)
        menu.Append(260, 'Draw6', 'Draw plots6')
        self.Bind(wx.EVT_MENU,self.OnPlotDraw6, id=260)
        menu.Append(261, 'Draw7', 'Draw plots7')
        self.Bind(wx.EVT_MENU,self.OnPlotDraw7, id=261)

        menu.Append(211, '&Redraw', 'Redraw plots')
        self.Bind(wx.EVT_MENU,self.OnPlotRedraw, id=211)
        menu.Append(212, '&Clear', 'Clear canvas')
        self.Bind(wx.EVT_MENU,self.OnPlotClear, id=212)
        menu.Append(213, '&Scale', 'Scale canvas')
        self.Bind(wx.EVT_MENU,self.OnPlotScale, id=213) 
        menu.Append(214, 'Enable &Zoom', 'Enable Mouse Zoom', kind=wx.ITEM_CHECK)
        self.Bind(wx.EVT_MENU,self.OnEnableZoom, id=214) 
        menu.Append(215, 'Enable &Grid', 'Turn on Grid', kind=wx.ITEM_CHECK)
        self.Bind(wx.EVT_MENU,self.OnEnableGrid, id=215)
        menu.Append(217, 'Enable &Drag', 'Activates dragging mode', kind=wx.ITEM_CHECK)
        self.Bind(wx.EVT_MENU,self.OnEnableDrag, id=217)
        menu.Append(220, 'Enable &Legend', 'Turn on Legend', kind=wx.ITEM_CHECK)
        self.Bind(wx.EVT_MENU,self.OnEnableLegend, id=220)
        menu.Append(222, 'Enable &Point Label', 'Show Closest Point', kind=wx.ITEM_CHECK)
        self.Bind(wx.EVT_MENU,self.OnEnablePointLabel, id=222)
       
        menu.Append(225, 'Scroll Up 1', 'Move View Up 1 Unit')
        self.Bind(wx.EVT_MENU,self.OnScrUp, id=225) 
        menu.Append(230, 'Scroll Rt 2', 'Move View Right 2 Units')
        self.Bind(wx.EVT_MENU,self.OnScrRt, id=230)
        menu.Append(235, '&Plot Reset', 'Reset to original plot')
        self.Bind(wx.EVT_MENU,self.OnReset, id=235)

        self.mainmenu.Append(menu, '&Plot')

        menu = wx.Menu()
        menu.Append(300, '&About', 'About this thing...')
        self.Bind(wx.EVT_MENU, self.OnHelpAbout, id=300)
        self.mainmenu.Append(menu, '&Help')

        self.SetMenuBar(self.mainmenu)

        # A status bar to tell people what's happening
        self.CreateStatusBar(1)
        
        self.client = PlotCanvas(self)
        #define the function for drawing pointLabels
        self.client.SetPointLabelFunc(self.DrawPointLabel)
        # Create mouse event for showing cursor coords in status bar
        self.client.canvas.Bind(wx.EVT_LEFT_DOWN, self.OnMouseLeftDown)
        # Show closest point when enabled
        self.client.canvas.Bind(wx.EVT_MOTION, self.OnMotion)

        self.Show(True)

    def DrawPointLabel(self, dc, mDataDict):
        """This is the fuction that defines how the pointLabels are plotted
            dc - DC that will be passed
            mDataDict - Dictionary of data that you want to use for the pointLabel

            As an example I have decided I want a box at the curve point
            with some text information about the curve plotted below.
            Any wxDC method can be used.
        """
        # ----------
        dc.SetPen(wx.Pen(wx.BLACK))
        dc.SetBrush(wx.Brush( wx.BLACK, wx.SOLID ) )
        
        sx, sy = mDataDict["scaledXY"] #scaled x,y of closest point
        dc.DrawRectangle( sx-5,sy-5, 10, 10)  #10by10 square centered on point
        px,py = mDataDict["pointXY"]
        cNum = mDataDict["curveNum"]
        pntIn = mDataDict["pIndex"]
        legend = mDataDict["legend"]
        #make a string to display
        s = "Crv# %i, '%s', Pt. (%.2f,%.2f), PtInd %i" %(cNum, legend, px, py, pntIn)
        dc.DrawText(s, sx , sy+1)
        # -----------

    def OnMouseLeftDown(self,event):
        s= "Left Mouse Down at Point: (%.4f, %.4f)" % self.client._getXY(event)
        self.SetStatusText(s)
        event.Skip()            #allows plotCanvas OnMouseLeftDown to be called

    def OnMotion(self, event):
        #show closest point (when enbled)
        if self.client.GetEnablePointLabel() == True:
            #make up dict with info for the pointLabel
            #I've decided to mark the closest point on the closest curve
            dlst= self.client.GetClosetPoint( self.client._getXY(event), pointScaled= True)
            if dlst != []:    #returns [] if none
                curveNum, legend, pIndex, pointXY, scaledXY, distance = dlst
                #make up dictionary to pass to my user function (see DrawPointLabel) 
                mDataDict= {"curveNum":curveNum, "legend":legend, "pIndex":pIndex,\
                            "pointXY":pointXY, "scaledXY":scaledXY}
                #pass dict to update the pointLabel
                self.client.UpdatePointLabel(mDataDict)
        event.Skip()           #go to next handler

    def OnFilePageSetup(self, event):
        self.client.PageSetup()
        
    def OnFilePrintPreview(self, event):
        self.client.PrintPreview()
        
    def OnFilePrint(self, event):
        self.client.Printout()
        
    def OnSaveFile(self, event):
        self.client.SaveFile()

    def OnFileExit(self, event):
        self.Close()

    def OnPlotDraw1(self, event):
        self.resetDefaults()
        self.client.Draw(_draw1Objects())
    
    def OnPlotDraw2(self, event):
        self.resetDefaults()
        self.client.Draw(_draw2Objects())
    
    def OnPlotDraw3(self, event):
        self.resetDefaults()
        self.client.SetFont(wx.Font(10,wx.SCRIPT,wx.NORMAL,wx.NORMAL))
        self.client.SetFontSizeAxis(20)
        self.client.SetFontSizeLegend(12)
        self.client.SetXSpec('min')
        self.client.SetYSpec('none')
        self.client.Draw(_draw3Objects())

    def OnPlotDraw4(self, event):
        self.resetDefaults()
        drawObj= _draw4Objects()
        self.client.Draw(drawObj)
##        # profile
##        start = _time.clock()            
##        for x in range(10):
##            self.client.Draw(drawObj)
##        print "10 plots of Draw4 took: %f sec."%(_time.clock() - start)
##        # profile end

    def OnPlotDraw5(self, event):
        # Empty plot with just axes
        self.resetDefaults()
        drawObj= _draw5Objects()
        # make the axis X= (0,5), Y=(0,10)
        # (default with None is X= (-1,1), Y= (-1,1))
        self.client.Draw(drawObj, xAxis= (0,5), yAxis= (0,10))

    def OnPlotDraw6(self, event):
        #Bar Graph Example
        self.resetDefaults()
        #self.client.SetEnableLegend(True)   #turn on Legend
        #self.client.SetEnableGrid(True)     #turn on Grid
        self.client.SetXSpec('none')        #turns off x-axis scale
        self.client.SetYSpec('auto')
        self.client.Draw(_draw6Objects(), xAxis= (0,7))

    def OnPlotDraw7(self, event):
        #log scale example
        self.resetDefaults()
        self.client.setLogScale((True,True))
        self.client.Draw(_draw7Objects())

    def OnPlotRedraw(self,event):
        self.client.Redraw()

    def OnPlotClear(self,event):
        self.client.Clear()
        
    def OnPlotScale(self, event):
        if self.client.last_draw != None:
            graphics, xAxis, yAxis= self.client.last_draw
            self.client.Draw(graphics,(1,3.05),(0,1))

    def OnEnableZoom(self, event):
        self.client.SetEnableZoom(event.IsChecked())
        self.mainmenu.Check(217, not event.IsChecked())
        
    def OnEnableGrid(self, event):
        self.client.SetEnableGrid(event.IsChecked())
        
    def OnEnableDrag(self, event):
        self.client.SetEnableDrag(event.IsChecked())
        self.mainmenu.Check(214, not event.IsChecked())
        
    def OnEnableLegend(self, event):
        self.client.SetEnableLegend(event.IsChecked())

    def OnEnablePointLabel(self, event):
        self.client.SetEnablePointLabel(event.IsChecked())

    def OnScrUp(self, event):
        self.client.ScrollUp(1)
        
    def OnScrRt(self,event):
        self.client.ScrollRight(2)

    def OnReset(self,event):
        self.client.Reset()

    def OnHelpAbout(self, event):
        from wx.lib.dialogs import ScrolledMessageDialog
        about = ScrolledMessageDialog(self, __doc__, "About...")
        about.ShowModal()

    def resetDefaults(self):
        """Just to reset the fonts back to the PlotCanvas defaults"""
        self.client.SetFont(wx.Font(10,wx.SWISS,wx.NORMAL,wx.NORMAL))
        self.client.SetFontSizeAxis(10)
        self.client.SetFontSizeLegend(7)
        self.client.setLogScale((False,False))
        self.client.SetXSpec('auto')
        self.client.SetYSpec('auto')
        
        

def __test():

    class MyApp(wx.App):
        def OnInit(self):
            wx.InitAllImageHandlers()
            frame = TestFrame(None, -1, "PlotCanvas")
            #frame.Show(True)
            self.SetTopWindow(frame)
            return True


    app = MyApp(0)
    app.MainLoop()

if __name__ == '__main__':
    __test()
