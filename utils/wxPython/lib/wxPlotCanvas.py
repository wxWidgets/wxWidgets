"""
This is a port of Konrad Hinsen's tkPlotCanvas.py plotting module.
After thinking long and hard I came up with the name "wxPlotCanvas.py".

This file contains two parts; first the re-usable library stuff, then, after
a "if __name__=='__main__'" test, a simple frame and a few default plots
for testing.

Harm van der Heijden, feb 1999

Original comment follows below:
# This module defines a plot widget for Tk user interfaces.
# It supports only elementary line plots at the moment.
# See the example at the end for documentation...
#
# Written by Konrad Hinsen <hinsen@cnrs-orleans.fr>
# With contributions from RajGopal Srinivasan <raj@cherubino.med.jhmi.edu>
# Last revision: 1998-7-28
#
"""

from wxPython import wx
import string

# Not everybody will have Numeric, so let's be cool about it...
try:
    import Numeric
except:
    # bummer!
    d = wx.wxMessageDialog(wx.NULL,
    """This module requires the Numeric module, which could not be imported.
It probably is not installed (it's not part of the standard Python
distribution). See the Python site (http://www.python.org) for
information on downloading source or binaries.""",
    "Numeric not found")
    if d.ShowModal() == wx.wxID_CANCEL:
	d = wx.wxMessageDialog(wx.NULL, "I kid you not! Pressing Cancel won't help you!", "Not a joke", wx.wxOK)
	d.ShowModal()
    raise ImportError

#
# Plotting classes...
#
class PolyPoints:

    def __init__(self, points, attr):
        self.points = Numeric.array(points)
	self.scaled = self.points
        self.attributes = {}
        for name, value in self._attributes.items():
            try:
                value = attr[name]
            except KeyError: pass
            self.attributes[name] = value

    def boundingBox(self):
        return Numeric.minimum.reduce(self.points), \
               Numeric.maximum.reduce(self.points)

    def scaleAndShift(self, scale=1, shift=0):
        self.scaled = scale*self.points+shift


class PolyLine(PolyPoints):

    def __init__(self, points, **attr):
        PolyPoints.__init__(self, points, attr)

    _attributes = {'color': 'black',
                   'width': 1}

    def draw(self, dc):
	color = self.attributes['color']
	width = self.attributes['width']
	arguments = []
       	dc.SetPen(wx.wxPen(wx.wxNamedColour(color), width))
	dc.DrawLines(map(tuple,self.scaled))


class PolyMarker(PolyPoints):

    def __init__(self, points, **attr):

        PolyPoints.__init__(self, points, attr)

    _attributes = {'color': 'black',
                   'width': 1,
                   'fillcolor': None,
                   'size': 2,
                   'fillstyle': wx.wxSOLID,
                   'outline': 'black',
                   'marker': 'circle'}

    def draw(self, dc):
	color = self.attributes['color']
	width = self.attributes['width']
        size = self.attributes['size']
        fillcolor = self.attributes['fillcolor']
        fillstyle = self.attributes['fillstyle']
        marker = self.attributes['marker']

	dc.SetPen(wx.wxPen(wx.wxNamedColour(color),width))
	if fillcolor:
	    dc.SetBrush(wx.wxBrush(wx.wxNamedColour(fillcolor),fillstyle))
	else:
	    dc.SetBrush(wx.wxBrush(wx.wxNamedColour('black'), wx.wxTRANSPARENT))

	self._drawmarkers(dc, self.scaled, marker, size)

    def _drawmarkers(self, dc, coords, marker,size=1):
        f = eval('self._' +marker)
        for xc, yc in coords:
            f(dc, xc, yc, size)

    def _circle(self, dc, xc, yc, size=1):
	dc.DrawEllipse(xc-2.5*size,yc-2.5*size,5.*size,5.*size)

    def _dot(self, dc, xc, yc, size=1):
	dc.DrawPoint(xc,yc)

    def _square(self, dc, xc, yc, size=1):
	dc.DrawRectangle(xc-2.5*size,yc-2.5*size,5.*size,5.*size)

    def _triangle(self, dc, xc, yc, size=1):
	dc.DrawPolygon([(-0.5*size*5,0.2886751*size*5),
		       (0.5*size*5,0.2886751*size*5),
		       (0.0,-0.577350*size*5)],xc,yc)

    def _triangle_down(self, dc, xc, yc, size=1):
	dc.DrawPolygon([(-0.5*size*5,-0.2886751*size*5),
		       (0.5*size*5,-0.2886751*size*5),
		       (0.0,0.577350*size*5)],xc,yc)

    def _cross(self, dc, xc, yc, size=1):
	dc.DrawLine(xc-2.5*size,yc-2.5*size,xc+2.5*size,yc+2.5*size)
	dc.DrawLine(xc-2.5*size,yc+2.5*size,xc+2.5*size,yc-2.5*size)

    def _plus(self, dc, xc, yc, size=1):
	dc.DrawLine(xc-2.5*size,yc,xc+2.5*size,yc)
	dc.DrawLine(xc,yc-2.5*size,xc,yc+2.5*size)

class PlotGraphics:

    def __init__(self, objects):
        self.objects = objects

    def boundingBox(self):
	p1, p2 = self.objects[0].boundingBox()
	for o in self.objects[1:]:
	    p1o, p2o = o.boundingBox()
	    p1 = Numeric.minimum(p1, p1o)
	    p2 = Numeric.maximum(p2, p2o)
	return p1, p2

    def scaleAndShift(self, scale=1, shift=0):
	for o in self.objects:
	    o.scaleAndShift(scale, shift)

    def draw(self, canvas):
	for o in self.objects:
	    o.draw(canvas)

    def __len__(self):
	return len(self.objects)

    def __getitem__(self, item):
	return self.objects[item]


class PlotCanvas(wx.wxWindow):

    def __init__(self, parent, id = -1):
	wx.wxWindow.__init__(self, parent, id, wx.wxPyDefaultPosition, wx.wxPyDefaultSize)
	self.border = (1,1)
	self.SetClientSizeWH(400,400)
	self.SetBackgroundColour(wx.wxNamedColour("white"))

	wx.EVT_SIZE(self,self.reconfigure)
	self._setsize()
	self.last_draw = None
#	self.font = self._testFont(font)

    def OnPaint(self, event):
	pdc = wx.wxPaintDC(self)
	if self.last_draw is not None:
	    apply(self.draw, self.last_draw + (pdc,))

    def reconfigure(self, event):
	(new_width,new_height) = self.GetClientSizeTuple()
        if new_width == self.width and new_height == self.height:
            return
        self._setsize()
        # self.redraw()

    def _testFont(self, font):
	if font is not None:
	    bg = self.canvas.cget('background')
	    try:
		item = CanvasText(self.canvas, 0, 0, anchor=NW,
				  text='0', fill=bg, font=font)
		self.canvas.delete(item)
	    except TclError:
		font = None
	return font

    def _setsize(self):
	(self.width,self.height) = self.GetClientSizeTuple();
	self.plotbox_size = 0.97*Numeric.array([self.width, -self.height])
	xo = 0.5*(self.width-self.plotbox_size[0])
	yo = self.height-0.5*(self.height+self.plotbox_size[1])
	self.plotbox_origin = Numeric.array([xo, yo])

    def draw(self, graphics, xaxis = None, yaxis = None, dc = None):
	if dc == None: dc = wx.wxClientDC(self)
	dc.BeginDrawing()
	dc.Clear()
	self.last_draw = (graphics, xaxis, yaxis)
	p1, p2 = graphics.boundingBox()
	xaxis = self._axisInterval(xaxis, p1[0], p2[0])
	yaxis = self._axisInterval(yaxis, p1[1], p2[1])
	text_width = [0., 0.]
	text_height = [0., 0.]
	if xaxis is not None:
	    p1[0] = xaxis[0]
	    p2[0] = xaxis[1]
	    xticks = self._ticks(xaxis[0], xaxis[1])
	    bb = dc.GetTextExtent(xticks[0][1])
	    text_height[1] = bb[1]
	    text_width[0] = 0.5*bb[0]
	    bb = dc.GetTextExtent(xticks[-1][1])
	    text_width[1] = 0.5*bb[0]
	else:
	    xticks = None
	if yaxis is not None:
	    p1[1] = yaxis[0]
	    p2[1] = yaxis[1]
	    yticks = self._ticks(yaxis[0], yaxis[1])
	    for y in yticks:
		bb = dc.GetTextExtent(y[1])
		text_width[0] = max(text_width[0],bb[0])
	    h = 0.5*bb[1]
	    text_height[0] = h
	    text_height[1] = max(text_height[1], h)
	else:
	    yticks = None
	text1 = Numeric.array([text_width[0], -text_height[1]])
	text2 = Numeric.array([text_width[1], -text_height[0]])
	scale = (self.plotbox_size-text1-text2) / (p2-p1)
	shift = -p1*scale + self.plotbox_origin + text1
	self._drawAxes(dc, xaxis, yaxis, p1, p2,
                       scale, shift, xticks, yticks)
	graphics.scaleAndShift(scale, shift)
	graphics.draw(dc)
	dc.EndDrawing()

    def _axisInterval(self, spec, lower, upper):
	if spec is None:
	    return None
	if spec == 'minimal':
	    if lower == upper:
		return lower-0.5, upper+0.5
	    else:
		return lower, upper
	if spec == 'automatic':
	    range = upper-lower
	    if range == 0.:
		return lower-0.5, upper+0.5
	    log = Numeric.log10(range)
	    power = Numeric.floor(log)
	    fraction = log-power
	    if fraction <= 0.05:
		power = power-1
	    grid = 10.**power
	    lower = lower - lower % grid
	    mod = upper % grid
	    if mod != 0:
		upper = upper - mod + grid
	    return lower, upper
	if type(spec) == type(()):
	    lower, upper = spec
	    if lower <= upper:
		return lower, upper
	    else:
		return upper, lower
	raise ValueError, str(spec) + ': illegal axis specification'

    def _drawAxes(self, dc, xaxis, yaxis,
                  bb1, bb2, scale, shift, xticks, yticks):
	dc.SetPen(wx.wxPen(wx.wxNamedColour('BLACK'),1))
	if xaxis is not None:
	    lower, upper = xaxis
	    text = 1
	    for y, d in [(bb1[1], -3), (bb2[1], 3)]:
		p1 = scale*Numeric.array([lower, y])+shift
		p2 = scale*Numeric.array([upper, y])+shift
		dc.DrawLine(p1[0],p1[1],p2[0],p2[1])
		for x, label in xticks:
		    p = scale*Numeric.array([x, y])+shift
		    dc.DrawLine(p[0],p[1],p[0],p[1]+d)
		    if text:
			dc.DrawText(label,p[0],p[1])
		text = 0

	if yaxis is not None:
	    lower, upper = yaxis
	    text = 1
	    h = dc.GetCharHeight()
	    for x, d in [(bb1[0], -3), (bb2[0], 3)]:
		p1 = scale*Numeric.array([x, lower])+shift
		p2 = scale*Numeric.array([x, upper])+shift
		dc.DrawLine(p1[0],p1[1],p2[0],p2[1])
		for y, label in yticks:
		    p = scale*Numeric.array([x, y])+shift
		    dc.DrawLine(p[0],p[1],p[0]-d,p[1])
		    if text:
			dc.DrawText(label,p[0]-dc.GetTextExtent(label)[0],
				    p[1]-0.5*h)
		text = 0

    def _ticks(self, lower, upper):
	ideal = (upper-lower)/7.
	log = Numeric.log10(ideal)
	power = Numeric.floor(log)
	fraction = log-power
	factor = 1.
	error = fraction
	for f, lf in self._multiples:
	    e = Numeric.fabs(fraction-lf)
	    if e < error:
		error = e
		factor = f
	grid = factor * 10.**power
        if power > 3 or power < -3:
            format = '%+7.0e'
        elif power >= 0:
            digits = max(1, int(power))
            format = '%' + `digits`+'.0f'
        else:
            digits = -int(power)
            format = '%'+`digits+2`+'.'+`digits`+'f'
	ticks = []
	t = -grid*Numeric.floor(-lower/grid)
	while t <= upper:
	    ticks.append(t, format % (t,))
	    t = t + grid
	return ticks

    _multiples = [(2., Numeric.log10(2.)), (5., Numeric.log10(5.))]

    def redraw(self,dc=None):
	if self.last_draw is not None:
	    apply(self.draw, self.last_draw + (dc,))

    def clear(self):
        self.canvas.delete('all')

#---------------------------------------------------------------------------
# if running standalone...
#
#     ...a sample implementation using the above
#


if __name__ == '__main__':
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


    class AppFrame(wx.wxFrame):
	def __init__(self, parent, id, title):
	    wx.wxFrame.__init__(self, parent, id, title,
				wx.wxPyDefaultPosition, wx.wxSize(400, 400))

	    # Now Create the menu bar and items
	    self.mainmenu = wx.wxMenuBar()

	    menu = wx.wxMenu()
	    menu.Append(200, '&Print...', 'Print the current plot')
	    wx.EVT_MENU(self, 200, self.OnFilePrint)
	    menu.Append(209, 'E&xit', 'Enough of this already!')
	    wx.EVT_MENU(self, 209, self.OnFileExit)
	    self.mainmenu.Append(menu, '&File')

	    menu = wx.wxMenu()
	    menu.Append(210, '&Draw', 'Draw plots')
	    wx.EVT_MENU(self,210,self.OnPlotDraw)
	    menu.Append(211, '&Redraw', 'Redraw plots')
	    wx.EVT_MENU(self,211,self.OnPlotRedraw)
	    menu.Append(212, '&Clear', 'Clear canvas')
	    wx.EVT_MENU(self,212,self.OnPlotClear)
	    self.mainmenu.Append(menu, '&Plot')

	    menu = wx.wxMenu()
	    menu.Append(220, '&About', 'About this thing...')
	    wx.EVT_MENU(self, 220, self.OnHelpAbout)
	    self.mainmenu.Append(menu, '&Help')

	    self.SetMenuBar(self.mainmenu)

	    # A status bar to tell people what's happening
	    self.CreateStatusBar(1)

	    self.client = PlotCanvas(self)

	def OnFilePrint(self, event):
	    d = wx.wxMessageDialog(self,
"""As of this writing, printing support in wxPython is shaky at best.
Are you sure you want to do this?""", "Danger!", wx.wxYES_NO)
            if d.ShowModal() == wx.wxID_YES:
		psdc = wx.wxPostScriptDC("out.ps", wx.TRUE, self)
		self.client.redraw(psdc)

	def OnFileExit(self, event):
	    self.Close()

	def OnPlotDraw(self, event):
	    self.client.draw(_InitObjects(),'automatic','automatic');

	def OnPlotRedraw(self,event):
	    self.client.redraw()

	def OnPlotClear(self,event):
	    self.client.last_draw = None
	    dc = wx.wxClientDC(self.client)
	    dc.Clear()

	def OnHelpAbout(self, event):
	    about = wx.wxMessageDialog(self, __doc__, "About...", wx.wxOK)
	    about.ShowModal()

	def OnCloseWindow(self, event):
	    self.Destroy()


    class MyApp(wx.wxApp):
	def OnInit(self):
	    frame = AppFrame(wx.NULL, -1, "wxPlotCanvas")
	    frame.Show(wx.TRUE)
	    self.SetTopWindow(frame)
	    return wx.TRUE


    app = MyApp(0)
    app.MainLoop()




#----------------------------------------------------------------------------
