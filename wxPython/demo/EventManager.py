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

from wxPython.wx import *
from wxPython.lib.evtmgr import eventManager

#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log

        fsize    = self.GetFont().GetPointSize()
        f1 = wxFont(fsize+0, wxSWISS, wxNORMAL, wxNORMAL)
        f2 = wxFont(fsize+2, wxSWISS, wxNORMAL, wxBOLD)
        f3 = wxFont(fsize+6, wxSWISS, wxNORMAL, wxBOLD)

        title1 = wxStaticText(self, -1, 'EventManager')
        title1.SetFont(f3)
        txt = """\
        This demo shows  (1) basic uses and features of the EventManager, as well
        as  (2) how it helps with a real-world task: creating independent, object-
        oriented components."""
        message0 = wxStaticText(self, -1, txt)
        message0.SetFont(f1)

        title2 = wxStaticText(self, -1, 'Event Listeners')
        title2.SetFont(f2)

        txt = """\
        These objects listen to motion events from the target window, using the ability
        to register one event with multiple listeners.  They also register for mouse events
        on themselves to implement toggle-button functionality."""
        message1 = wxStaticText(self, -1, txt)
        message1.SetFont(f1)

        title3 = wxStaticText(self, -1, 'Target Window')
        title3.SetFont(f2)

        txt = """\
        A passive window that's used as an event generator.  Move the mouse over it to
        send events to the listeners above."""
        message2 = wxStaticText(self, -1, txt)
        message2.SetFont(f1)

        targetPanel = Tile(self, log, bgColor=wxColor(80,10,10), active=0)
        buttonPanel = wxPanel(self ,-1)
        sizer       = wxBoxSizer(wxHORIZONTAL)
        target      = targetPanel.tile

        sizer.Add(0,0,1)
        for factor in [0.2, 0.3, 0.4, 0.5, 0.6, 0.7]:
            sizer.Add(Tile(buttonPanel, log, factor-0.05, target), 0, wxALIGN_CENTER)
            sizer.Add(0,0,1)
            sizer.Add(Tile(buttonPanel, log, factor,      target), 0, wxALIGN_CENTER)
            sizer.Add(0,0,1)

        buttonPanel.SetAutoLayout(1)
        buttonPanel.SetSizer(sizer)
        sizer.Fit(buttonPanel)

        sizer = wxBoxSizer(wxVERTICAL)
        sizer.Add(title1,      0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 6)
        sizer.Add(message0,    0, wxALIGN_CENTER | wxALL, 6)
        sizer.Add(title2,      0, wxALIGN_CENTER | wxLEFT | wxTOP    | wxRIGHT, 16)
        sizer.Add(message1,    0, wxALIGN_CENTER | wxALL, 6)
        sizer.Add(buttonPanel, 0, wxEXPAND       | wxLEFT | wxBOTTOM | wxRIGHT, 16)
        sizer.Add(title3,      0, wxALIGN_CENTER | wxLEFT | wxRIGHT, 16)
        sizer.Add(message2,    0, wxALIGN_CENTER | wxALL, 6)
        sizer.Add(targetPanel, 2, wxEXPAND       | wxLEFT | wxBOTTOM | wxRIGHT, 16)
        self.SetAutoLayout(1)
        self.SetSizer(sizer)



class Tile(wxPanel):
    """
    This outer class is responsible for changing
    its border color in response to certain mouse
    events over its contained 'InnerTile'.
    """
    normal = wxColor(150,150,150)
    active = wxColor(250,245,245)
    hover  = wxColor(210,220,210)

    def __init__(self, parent, log, factor=1, thingToWatch=None, bgColor=None, active=1, size=(38,38), borderWidth=3):
        wxPanel.__init__(self, parent, -1, size=size, style=wxCLIP_CHILDREN)
        self.tile = InnerTile(self, log, factor, thingToWatch, bgColor)
        self.log  = log
        sizer = wxBoxSizer(wxHORIZONTAL)
        sizer.Add(self.tile, 1, wxEXPAND | wxALL, borderWidth)
        self.SetAutoLayout(1)
        self.SetSizer(sizer)
        self.Layout()
        self.SetBackgroundColour(Tile.normal)
        if active:
            # Register myself for mouse events over self.tile in order to
            # create typical button/hyperlink visual effects.
            eventManager.Register(self.setHover,  EVT_ENTER_WINDOW, self.tile)
            eventManager.Register(self.setNormal, EVT_LEAVE_WINDOW, self.tile)
            eventManager.Register(self.setActive, EVT_LEFT_DOWN,    self.tile)
            eventManager.Register(self.setHover,  EVT_LEFT_UP,      self.tile)


    def setHover(self, event):
        self.SetBackgroundColour(Tile.hover)
        self.Refresh()


    def setActive(self, event):
        self.SetBackgroundColour(Tile.active)
        self.Refresh()


    def setNormal(self, event):
        self.SetBackgroundColour(Tile.normal)
        self.Refresh()



class InnerTile(wxPanel):
    IDLE_COLOR  = wxColor( 80, 10, 10)
    START_COLOR = wxColor(200, 70, 50)
    FINAL_COLOR = wxColor( 20, 80,240)
    OFF_COLOR   = wxColor(185,190,185)
    # Some pre-computation.
    DELTAS            = map(lambda a,b: b-a, START_COLOR.Get(), FINAL_COLOR.Get())
    START_COLOR_TUPLE = START_COLOR.Get()

    """
    This inner panel changes its color in reaction to mouse
    events over the 'thingToWatch'.
    """
    def __init__(self, parent, log, factor, thingToWatch=None, bgColor=None):
        wxPanel.__init__(self, parent, -1)
        self.log=log
        if bgColor:
            self.SetBackgroundColour(bgColor)
        if thingToWatch:
            self.factor       = factor
            self.thingToWatch = thingToWatch
            self.state        = 0
            self.toggleOnOff()
            # Watch for the mouse click to enable/disable myself.
            eventManager.Register(self.toggleOnOff, EVT_LEFT_UP, self)


    def toggleOnOff(self, event=None):
        # Implement being on or off by registering and
        # de-registering self.makeColor() from the event manager.
        if self.state:
            eventManager.DeregisterListener(self.makeColor)
        else:
            eventManager.Register(self.makeColor, EVT_MOTION, self.thingToWatch)
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
        self.setColor(wxColor(r,g,b))




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
    run.main(['', os.path.basename(sys.argv[0])])

