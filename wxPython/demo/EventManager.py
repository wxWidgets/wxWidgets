#---------------------------------------------------------------------------
# Name:        EventManager.py
# Purpose:     A module to demonstrate wxPython.lib.evtmgr.EventManager.
#
# Author:      Robb Shecter (robb@acm.org)
#
# Created:     16-December-2002
# Copyright:   (c) 2002 by Robb Shecter (robb@acm.org)
# Licence:     wxWindows license
#---------------------------------------------------------------------------

import  wx
import  wx.lib.evtmgr   as  em

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)
        self.log = log

        fsize    = self.GetFont().GetPointSize()
        f1 = wx.Font(fsize+0, wx.SWISS, wx.NORMAL, wx.NORMAL)
        f2 = wx.Font(fsize+2, wx.SWISS, wx.NORMAL, wx.BOLD)
        f3 = wx.Font(fsize+6, wx.SWISS, wx.NORMAL, wx.BOLD)

        title1 = wx.StaticText(self, -1, 'EventManager')
        title1.SetFont(f3)
        txt = """\
        This demo shows  (1) basic uses and features of the EventManager, as well
        as  (2) how it helps with a real-world task: creating independent, object-
        oriented components."""
        message0 = wx.StaticText(self, -1, txt)
        message0.SetFont(f1)

        title2 = wx.StaticText(self, -1, 'Event Listeners')
        title2.SetFont(f2)

        txt = """\
        These objects listen to motion events from the target window, using the ability
        to register one event with multiple listeners.  They also register for mouse events
        on themselves to implement toggle-button functionality."""
        message1 = wx.StaticText(self, -1, txt)
        message1.SetFont(f1)

        title3 = wx.StaticText(self, -1, 'Target Window')
        title3.SetFont(f2)

        txt = """\
        A passive window that's used as an event generator.  Move the mouse over it to
        send events to the listeners above."""
        message2 = wx.StaticText(self, -1, txt)
        message2.SetFont(f1)

        targetPanel = Tile(self, log, bgColor=wx.Colour(80,10,10), active=0)
        buttonPanel = wx.Panel(self ,-1)
        sizer       = wx.BoxSizer(wx.HORIZONTAL)
        target      = targetPanel.tile

        sizer.Add((0,0), 1)
        for factor in [0.2, 0.3, 0.4, 0.5, 0.6, 0.7]:
            sizer.Add(Tile(buttonPanel, log, factor-0.05, target), 0, wx.ALIGN_CENTER)
            sizer.Add((0,0),1)
            sizer.Add(Tile(buttonPanel, log, factor,      target), 0, wx.ALIGN_CENTER)
            sizer.Add((0,0),1)

        buttonPanel.SetSizer(sizer)
        sizer.Fit(buttonPanel)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(title1,      0, wx.ALIGN_CENTER | wx.TOP | wx.BOTTOM, 6)
        sizer.Add(message0,    0, wx.ALIGN_CENTER | wx.ALL, 6)
        sizer.Add(title2,      0, wx.ALIGN_CENTER | wx.LEFT | wx.TOP    | wx.RIGHT, 16)
        sizer.Add(message1,    0, wx.ALIGN_CENTER | wx.ALL, 6)
        sizer.Add(buttonPanel, 0, wx.EXPAND       | wx.ALL, 16)
        sizer.Add(title3,      0, wx.ALIGN_CENTER | wx.LEFT | wx.RIGHT, 16)
        sizer.Add(message2,    0, wx.ALIGN_CENTER | wx.ALL, 6)
        sizer.Add(targetPanel, 2, wx.EXPAND       | wx.LEFT | wx.BOTTOM | wx.RIGHT, 16)
        self.SetSizer(sizer)



class Tile(wx.Window):
    """
    This outer class is responsible for changing
    its border color in response to certain mouse
    events over its contained 'InnerTile'.
    """
    normal = wx.Colour(150,150,150)
    active = wx.Colour(250,245,245)
    hover  = wx.Colour(210,220,210)

    def __init__(self, parent, log, factor=1, thingToWatch=None, bgColor=None, active=1, size=(38,38), borderWidth=3):
        wx.Window.__init__(self, parent, -1, size=size, style=wx.CLIP_CHILDREN)
        self.tile = InnerTile(self, log, factor, thingToWatch, bgColor)
        self.log  = log
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(self.tile, 1, wx.EXPAND | wx.ALL, borderWidth)
        self.SetSizer(sizer)
        self.Layout()
        em.eventManager.Register(self.doLayout, wx.EVT_SIZE, self)
        self.SetBackgroundColour(Tile.normal)
        if active:
            # Register myself for mouse events over self.tile in order to
            # create typical button/hyperlink visual effects.
            em.eventManager.Register(self.setHover,  wx.EVT_ENTER_WINDOW, self.tile)
            em.eventManager.Register(self.setNormal, wx.EVT_LEAVE_WINDOW, self.tile)
            em.eventManager.Register(self.setActive, wx.EVT_LEFT_DOWN,    self.tile)
            em.eventManager.Register(self.setHover,  wx.EVT_LEFT_UP,      self.tile)


    def doLayout(self, event):
        self.Layout()
        

    def setHover(self, event):
        self.SetBackgroundColour(Tile.hover)
        self.Refresh()


    def setActive(self, event):
        self.SetBackgroundColour(Tile.active)
        self.Refresh()


    def setNormal(self, event):
        self.SetBackgroundColour(Tile.normal)
        self.Refresh()



class InnerTile(wx.Window):
    IDLE_COLOR  = wx.Colour( 80, 10, 10)
    START_COLOR = wx.Colour(200, 70, 50)
    FINAL_COLOR = wx.Colour( 20, 80,240)
    OFF_COLOR   = wx.Colour(185,190,185)
    # Some pre-computation.
    DELTAS            = map(lambda a,b: b-a, START_COLOR.Get(), FINAL_COLOR.Get())
    START_COLOR_TUPLE = START_COLOR.Get()

    """
    This inner panel changes its color in reaction to mouse
    events over the 'thingToWatch'.
    """
    def __init__(self, parent, log, factor, thingToWatch=None, bgColor=None):
        wx.Window.__init__(self, parent, -1)
        self.SetMinSize((20,20))
        self.log=log
        if bgColor:
            self.SetBackgroundColour(bgColor)
        if thingToWatch:
            self.factor       = factor
            self.thingToWatch = thingToWatch
            self.state        = 0
            self.toggleOnOff()
            # Watch for the mouse click to enable/disable myself.
            em.eventManager.Register(self.toggleOnOff, wx.EVT_LEFT_UP, self)


    def toggleOnOff(self, event=None):
        # Implement being on or off by registering and
        # de-registering self.makeColor() from the event manager.
        if self.state:
            em.eventManager.DeregisterListener(self.makeColor)
        else:
            em.eventManager.Register(self.makeColor, wx.EVT_MOTION, self.thingToWatch)
        self.state = 1 - self.state
        self.resetColor()


    def resetColor(self, event=None):
        if self.state:
            self.setColor(InnerTile.IDLE_COLOR)
        else:
            self.setColor(InnerTile.OFF_COLOR)


    def setColor(self, color):
        self.SetBackgroundColour(color)
        self.Refresh()


    def makeColor(self, mouseEvent):
        self.makeColorFromTuple(mouseEvent.GetPositionTuple())


    def makeColorFromTuple(self, (x, y)):
        MAX     = 180.0
        scaled  = min((x + y) * self.factor, MAX)  # In range [0..MAX]
        percent = scaled / MAX
        r = InnerTile.START_COLOR_TUPLE[0] + (InnerTile.DELTAS[0] * percent)
        g = InnerTile.START_COLOR_TUPLE[1] + (InnerTile.DELTAS[1] * percent)
        b = InnerTile.START_COLOR_TUPLE[2] + (InnerTile.DELTAS[2] * percent)
        self.setColor(wx.Colour(int(r), int(g), int(b)))




#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2>EventManager</h2>

<p> The goal of the EventManager is to make wxWindows events more
'Pythonic' (ie. object-oriented) and easier to work with, without
impacting performance.  It offers these features:

<p>
<ul>

    <li> Allows any number of listeners to register for a single
    event.  (In addition to the standard wxPython feature of a single
    listener being able to respond to many events.)

    <li> Makes it easy to disconnect and reconnect listeners.  This
    has the effect of reducing the need for case-based branching in
    application code.

    <li> Has an object-oriented API.  Programmers register to get
    events directly from the objects that generate them, instead of
    using ID numbers.

</ul>

<h3>Usage</h3>

<p>The EventManager class has three public methods.  First get a
reference to it:

<PRE>
  from wxPython.lib.evtmgr import eventManager
</PRE>

<p>...and then invoke any of the following methods.  These methods are
'safe'; duplicate registrations or de-registrations will have no
effect.

<p><b>Registering a listener:</b>

<PRE>
  eventManager.Register(listener, event, event-source)
</PRE>


<p><b>De-registering by window:</b>

<PRE>
  eventManager.DeregisterWindow(event-source)
</PRE>


<p><b>De-registering by listener:</b>

<PRE>
  eventManager.DeregisterListener(listener)
</PRE>

<p><b>Simple Example:</b>

<PRE>
  from wxPython.lib.evtmgr import eventManager

  aButton = wxButton(somePanel, -1, 'Click me')
  eventManager.Register(self.someMethod, EVT_BUTTON, aButton)
</PRE>

<p> See the demo code as well as the documentation in the source of
<tt>wxPython.lib.evtmgr</tt> for more details.


<p>
by Robb Shecter (robb@acm.org)
</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

