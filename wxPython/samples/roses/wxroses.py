#----------------------------------------------------------------------------
# Name:         wxroses.py
# Purpose:      wxPython GUI using clroses.py to display a classic graphics
#               hack.
#
# Author:       Ric Werme, Robin Dunn.
# WWW:          http://WermeNH.com/roses
#
# Created:      June 2007
# CVS-ID:       $Id$
# Copyright:    Public Domain, please give credit where credit is due.
# License:      Sorry, no EULA.
#----------------------------------------------------------------------------

# This module is responsible for everything involving GUI usage
# as clroses knows nothing about wxpython, tkintr, etc.

# There are some notes about how the Roses algorithm works in clroses.py,
# but the best reference should be at http://WermeNH.com/roses/index.html .

# There are a number of enhancements that could be done to wxRoses, and
# contributions are welcome as long as you don't destory the general
# structure, flavor, and all that.  The following list is in the order
# I'd like to see done.  Some are easy, some aren't, some are easy if
# you have experience in the right parts of external code.

# Brighter crossing points.
# Where many vectors cross, the display becomes washed out as a solid shape
# of light.  On (antique) refresh vector graphics systems, crossing points
# are brighter because the electron beam paints the pixels multiple times.
# This gives complex patterns a lacy feel to some, and a 3-D sense to
# fluted shapes where vectors lie tangent to some curve.  It would be
# nice to do the same in a bitmap buffer, the code to draw a vector is
# pretty simple, adding something that adds brightness to it via math or
# a lookup table ought to be a simple addition.

# Monochrome is so 20th century.
# There are a number of things that could be done with color.  The simplest
# is to step through colors in a color list, better things to do would be
# for clroses.py to determine the length of an interesting "generator pattern,"
# e.g. the square in the opening display.  Then it could change colors either
# every four vectors or cycle through the first four colors in the list.

# Bookmark that Rose!
# As you play with wxRoses, you'll come across some patterns that are
# "keepers."  A bookmark mechanism would be handy.

# Save that Rose!
# It would be nice to have a Menu-bar/File/Save-as dialog to save a pattern
# as a jpg/png/gif file.

# Themes
# A pulldown option to select various themes is worthwhile.  E.g.:
#  Start an interesting animation,
#  Select complex, lacy Roses,
#  Select the author's favorites,
#  Return to the initial Rose.
# Actually, all that's necessary are some pre-loaded bookmarks.

# Help text
# Standard fare, or:

# Slide show
# At CMU I created an interactive slide show that walked people through
# all the options and made suggestions about how to choose Style and Petal.
# I forget exactly what I said and may not have listings for it.  At any rate,
# making the help mechanism start one of several "lessons" where it could
# control the display (without blocking the user's control) would be pretty
# straightforward.

import wx
import clroses
import wx.lib.colourselect as cs


# Class SpinPanel creates a control that includes both a StaticText widget
# which holds the the name of a parameter and a SpinCtrl widget which
# displays the current value.  Values are set at initialization and can
# change via the SpinCtrl widget or by the program.  So that the program
# can easily access the SpinCtrl, the SpinPanel handles are saved in the
# spin_panels dictionary.
class SpinPanel(wx.Panel):
    def __init__(self, parent, name, min_value, value, max_value, callback):
        wx.Panel.__init__(self, parent, -1)
        if "wxMac" in wx.PlatformInfo:
            self.SetWindowVariant(wx.WINDOW_VARIANT_SMALL)
        self.st = wx.StaticText(self, -1, name)
        self.sc = wx.SpinCtrl(self, -1, "", size = (70, -1))
        self.sc.SetRange(min_value, max_value)
        self.sc.SetValue(value)
        self.sc.Bind(wx.EVT_SPINCTRL, self.OnSpin)
        self.callback = callback

        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(self.st, 0, wx.ALIGN_CENTER_VERTICAL)
        sizer.Add((1,1), 1)
        sizer.Add(self.sc)
        self.SetSizer(sizer)
        
        global spin_panels
        spin_panels[name] = self

    # Called (generally through spin_panels{}) to set the SpinCtrl value.
    def SetValue(self, value):
        self.sc.SetValue(value)

    # Called when user changes the SpinCtrl value.
    def OnSpin(self, event):
        name = self.st.GetLabel()
        value = self.sc.GetValue()
        if verbose:
            print 'OnSpin', name, '=', value
        self.callback(name, value)	# Call MyFrame.OnSpinback to call clroses


# This class is used to display the current rose diagram.  It keeps a
# buffer bitmap of the current display, which it uses to refresh the
# screen with when needed.  When it is told to draw some lines it does
# so to the buffer in order for it to always be up to date.
class RosePanel(wx.Panel):
    def __init__(self, *args, **kw):
        wx.Panel.__init__(self, *args, **kw)
        self.InitBuffer()
        self.resizeNeeded = False
        self.useGCDC = False
        self.useBuffer = True

        # set default colors
        self.SetBackgroundColour((51, 51, 51))    # gray20
        self.SetForegroundColour((164, 211, 238)) # lightskyblue2
        
        # connect the size and paint events to handlers
        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_IDLE, self.OnIdle)


    def InitBuffer(self):
        size = self.GetClientSize()
        self.buffer = wx.EmptyBitmap(max(1, size.width),
                                     max(1, size.height))
        
    def Clear(self):
        dc = self.useBuffer and wx.MemoryDC(self.buffer) or wx.ClientDC(self)
        dc.SetBackground(wx.Brush(self.GetBackgroundColour()))
        dc.Clear()
        if self.useBuffer:
            self.Refresh(False)

    def DrawLines(self, lines):
        if len(lines) <= 1:
            return
        dc = self.useBuffer and wx.MemoryDC(self.buffer) or wx.ClientDC(self)
        if self.useGCDC:
            dc = wx.GCDC(dc)
        dc.SetPen(wx.Pen(self.GetForegroundColour(), 1))
        dc.DrawLines(lines)
        if self.useBuffer:
            self.Refresh(False)

    def TriggerResize(self):
        self.GetParent().TriggerResize(self.buffer.GetSize())

    def TriggerRedraw(self):
        self.GetParent().TriggerRedraw()
    
    def OnSize(self, evt):
        self.resizeNeeded = True

    def OnIdle(self, evt):
        if self.resizeNeeded:
            self.InitBuffer()
            self.TriggerResize()
            if self.useBuffer:
                self.Refresh()
            self.resizeNeeded = False

    def OnPaint(self, evt):
        dc = wx.PaintDC(self)
        if self.useBuffer:
            dc.DrawBitmap(self.buffer, 0,0)
        else:
            self.TriggerRedraw()


# A panel used to collect options on how the rose is drawn
class OptionsPanel(wx.Panel):
    def __init__(self, parent, rose):
        wx.Panel.__init__(self, parent)
        self.rose = rose
        sizer = wx.StaticBoxSizer(wx.StaticBox(self, label='Options'),
                                  wx.VERTICAL)
        self.useGCDC = wx.CheckBox(self, label="Use GCDC")
        sizer.Add(self.useGCDC, 0, wx.BOTTOM|wx.LEFT, 2)
        self.useBuffer = wx.CheckBox(self, label="Use buffering")
        sizer.Add(self.useBuffer, 0, wx.BOTTOM|wx.LEFT, 2)

        def makeCButton(label):
            btn = cs.ColourSelect(self, size=(20,22))
            lbl = wx.StaticText(self, -1, label)
            sizer = wx.BoxSizer(wx.HORIZONTAL)
            sizer.Add(btn)
            sizer.Add((4,4))
            sizer.Add(lbl, 0, wx.ALIGN_CENTER_VERTICAL)
            return sizer, btn
        
        s, self.fg = makeCButton('Foreground')
        sizer.Add(s)        
        s, self.bg = makeCButton('Background')
        sizer.Add(s)
        self.SetSizer(sizer)

        self.Bind(wx.EVT_CHECKBOX, self.OnUseGCDC, self.useGCDC)
        self.Bind(wx.EVT_CHECKBOX, self.OnUseBuffer, self.useBuffer)
        self.Bind(wx.EVT_IDLE, self.OnIdle)
        self.Bind(cs.EVT_COLOURSELECT, self.OnSetFG, self.fg)
        self.Bind(cs.EVT_COLOURSELECT, self.OnSetBG, self.bg)

    def OnIdle(self, evt):
        if self.useGCDC.GetValue() != self.rose.useGCDC:
            self.useGCDC.SetValue(self.rose.useGCDC)
        if self.useBuffer.GetValue() != self.rose.useBuffer:
            self.useBuffer.SetValue(self.rose.useBuffer)
        if self.fg.GetValue() != self.rose.GetForegroundColour():
            self.fg.SetValue(self.rose.GetForegroundColour())
        if self.bg.GetValue() != self.rose.GetBackgroundColour():
            self.bg.SetValue(self.rose.GetBackgroundColour())

    def OnUseGCDC(self, evt):
        self.rose.useGCDC = evt.IsChecked()
        self.rose.TriggerRedraw()

    def OnUseBuffer(self, evt):
        self.rose.useBuffer = evt.IsChecked()
        self.rose.TriggerRedraw()
        
    def OnSetFG(self, evt):
        self.rose.SetForegroundColour(evt.GetValue())
        self.rose.TriggerRedraw()

    def OnSetBG(self, evt):
        self.rose.SetBackgroundColour(evt.GetValue())
        self.rose.TriggerRedraw()


# MyFrame is the traditional class name to create and populate the
# application's frame.  The general GUI has control/status panels on
# the right side and a panel on the left side that draws the rose
#
# This class also derives from clroses.rose so it can implement the
# required interfaces to connect the GUI to the rose engine.
class MyFrame(wx.Frame, clroses.rose):
    # Color matching dictionary, convert label name to color:
    # Stop and Go ala traffic lights,
    # Skip and Forward look ahead to the purple mountain majesties (really bluish),
    # Reverse and Backward look morosely behind to maroon memories,
    # Redraw looks at the brown earth right below your feet.
    # Yeah, so it's lame.  All I really wanted was to color Stop and Go.
    labelColours = {
        'Go': 'dark green', 'Stop': 'red',
        'Redraw': 'brown', 'Skip': 'dark slate blue',
        'Backward': 'maroon', 'Forward': 'dark slate blue', 'Reverse': 'maroon'
        }

    def __init__(self):
        def makeSP(name, labels, statictexts = None):
            panel = wx.Panel(self.side_panel, -1)
            box = wx.StaticBox(panel, -1, name)
            sizer = wx.StaticBoxSizer(box, wx.VERTICAL)
            for name, min_value, value, max_value in labels:
                sp = SpinPanel(panel, name, min_value, value, max_value, self.OnSpinback)
                sizer.Add(sp, 0, wx.EXPAND)
            if statictexts:
                for name, text in statictexts:
                    st = wx.StaticText(panel, -1, text)
                    spin_panels[name] = st	# Supposed to be a SpinPanel....
                    sizer.Add(st, 0, wx.EXPAND)
            panel.SetSizer(sizer)
            return panel
    
        wx.Frame.__init__(self, None, title="Roses in wxPython")

        self.rose_panel = RosePanel(self)
        self.side_panel = wx.Panel(self)
        
        # The cmd panel is four buttons whose names and foreground colors
        # change.  Plop them in a StaticBox like the SpinPanels.  Use
        # a 2x2 grid, but StaticBoxSizer can't handle that.  Therefore,
        # create a sub panel, layout the buttons there, then give that to
        # a higher panel that has the static box stuff.
        self.cmd_panel = wx.Panel(self.side_panel, -1)
        self.sub_panel = wx.Panel(self.cmd_panel, -1)
        sizer = wx.GridSizer(rows = 2, cols = 2)
        global ctrl_buttons
        border = 'wxMac' in wx.PlatformInfo and 3 or 1
        for name, handler in (
                ('Go',       self.OnGoStop),
                ('Redraw',   self.OnRedraw),
                ('Backward', self.OnBackward),
                ('Forward',  self.OnForward)):
            button = wx.Button(self.sub_panel, -1, name)
            button.SetForegroundColour(self.labelColours[name])
            ctrl_buttons[name] = button
            button.Bind(wx.EVT_BUTTON, handler)
            sizer.Add(button, 0, wx.EXPAND|wx.ALL, border)
        self.sub_panel.SetSizer(sizer)

        # Set up cmd_panel with StaticBox stuff
        box = wx.StaticBox(self.cmd_panel, -1, 'Command')
        sizer = wx.StaticBoxSizer(box, wx.VERTICAL)
        sizer.Add(self.sub_panel)
        self.cmd_panel.SetSizer(sizer)

        # Now make the rest of the control panels...
        # The order of creation of SpinCtrls and Buttons is the order that
        # the tab key will step through, so the order of panel creation is
        # important.
        # In the SpinPanel data (name, min, value, max), value will be
        # overridden by clroses.py defaults.
        self.coe_panel = makeSP('Coefficient',
                               (('Style',     0, 100, 3600),
                                ('Sincr', -3600,  -1, 3600),
                                ('Petal',     0,   2, 3600),
                                ('Pincr', -3600,   1, 3600)))

        self.vec_panel = makeSP('Vector',
                                (('Vectors'   , 1,  399, 3600),
                                 ('Minimum'   , 1,    1, 3600),
                                 ('Maximum'   , 1, 3600, 3600),
                                 ('Skip first', 0,    0, 3600),
                                 ('Draw only' , 1, 3600, 3600)),
                                (('Takes', 'Takes 0000 vectors'), ))
        
        self.tim_panel = makeSP('Timing',
                               (('Vec/tick' , 1,   20, 3600),
                                ('msec/tick', 1,   50, 1000),
                                ('Delay'    , 1, 2000, 9999)))

        self.opt_panel = OptionsPanel(self.side_panel, self.rose_panel)
            
        # put them all on in a sizer attached to the side_panel
        panelSizer = wx.BoxSizer(wx.VERTICAL)
        panelSizer.Add(self.cmd_panel, 0, wx.EXPAND|wx.TOP|wx.LEFT|wx.RIGHT, 5)
        panelSizer.Add(self.coe_panel, 0, wx.EXPAND|wx.TOP|wx.LEFT|wx.RIGHT, 5)
        panelSizer.Add(self.vec_panel, 0, wx.EXPAND|wx.TOP|wx.LEFT|wx.RIGHT, 5)
        panelSizer.Add(self.tim_panel, 0, wx.EXPAND|wx.TOP|wx.LEFT|wx.RIGHT, 5)
        panelSizer.Add(self.opt_panel, 0, wx.EXPAND|wx.TOP|wx.LEFT|wx.RIGHT, 5)
        self.side_panel.SetSizer(panelSizer)

        # and now arrange the two main panels in another sizer for the frame
        mainSizer = wx.BoxSizer(wx.HORIZONTAL)
        mainSizer.Add(self.rose_panel, 1, wx.EXPAND)
        mainSizer.Add(self.side_panel, 0, wx.EXPAND)
        self.SetSizer(mainSizer)

        # bind event handlers
        self.timer = wx.Timer(self)
        self.Bind(wx.EVT_TIMER, self.OnTimer, self.timer)

        # Determine appropriate image size.
        # At this point, the rose_panel and side_panel will both report
        # size (20, 20).  After mainSizer.Fit(self) they will report the
        # same, but the Frame size, self.GetSize(), will report the desired
        # side panel dimensions plus an extra 20 on the width.  That lets
        # us determine the frame size that will display the side panel and
        # a square space for the diagram.  Only after Show() will the two
        # panels report the accurate sizes.
        mainSizer.Fit(self)
        rw, rh = self.rose_panel.GetSize()
        sw, sh = self.side_panel.GetSize()
        fw, fh = self.GetSize()
        h = max(600, fh)	# Change 600 to desired minimum size
        w = h + fw - rw
        if verbose:
            print 'rose panel size', (rw, rh)
            print 'side panel size', (sw, sh)
            print '     frame size', (fw, fh)
            print 'Want size', (w,h)
        self.SetSize((w, h))
        self.SupplyControlValues()	# Ask clroses to tell us all the defaults
        self.Show()

    # Command button event handlers.  These are relabled when changing between auto
    # and manual modes.  They simply reflect the call to a method in the base class.
    #
    # Go/Stop button
    def OnGoStop(self, event):
        if verbose:
            print 'OnGoStop'
        self.cmd_go_stop()

    # Redraw/Redraw
    def OnRedraw(self, event):
        if verbose:
            print 'OnRedraw'
        self.cmd_redraw()

    # Backward/Reverse
    def OnBackward(self, event):
        if verbose:
            print 'OnBackward'
        self.cmd_backward()

    # Forward/Skip
    def OnForward(self, event):
        if verbose:
            print 'OnForward'
        self.cmd_step()


    # The clroses.roses class expects to have methods available that
    # implement the missing parts of the functionality needed to do
    # the actual work of getting the diagram to the screen and etc.
    # Those are implemented here as the App* methods.
    
    def AppClear(self):
        if verbose:
            print 'AppClear: clear screen'
        self.rose_panel.Clear()
        
    def AppCreateLine(self, line):
        # print 'AppCreateLine, len', len(line), 'next', self.nextpt
        self.rose_panel.DrawLines(line)

    # Here when clroses has set a new style and/or petal value, update
    # strings on display.
    def AppSetParam(self, style, petals, vectors):
        spin_panels['Style'].SetValue(style)
        spin_panels['Petal'].SetValue(petals)
        spin_panels['Vectors'].SetValue(vectors)

    def AppSetIncrs(self, sincr, pincr):
        spin_panels['Sincr'].SetValue(sincr)
        spin_panels['Pincr'].SetValue(pincr)

    def AppSetVectors(self, vectors, minvec, maxvec, skipvec, drawvec):
        spin_panels['Vectors'].SetValue(vectors)
        spin_panels['Minimum'].SetValue(minvec)
        spin_panels['Maximum'].SetValue(maxvec)
        spin_panels['Skip first'].SetValue(skipvec)
        spin_panels['Draw only'].SetValue(drawvec)
        
    def AppSetTakesVec(self, takes):
        spin_panels['Takes'].SetLabel('Takes %d vectors' % takes)

    # clroses doesn't change this data, so it's not telling us something
    # we don't already know.
    def AppSetTiming(self, vecPtick, msecPtick, delay):
        spin_panels['Vec/tick'].SetValue(vecPtick)
        spin_panels['msec/tick'].SetValue(msecPtick)
        spin_panels['Delay'].SetValue(delay)

    # Command buttons change their names based on the whether we're in auto
    # or manual mode.
    def AppCmdLabels(self, labels):
        for name, label in map(None, ('Go', 'Redraw', 'Backward', 'Forward'), labels):
            ctrl_buttons[name].SetLabel(label)
            ctrl_buttons[name].SetForegroundColour(self.labelColours[label])


    # Timer methods.  The paranoia about checking up on the callers is
    # primarily because it's easier to check here.  We expect that calls to
    # AppAfter and OnTimer alternate, but don't verify that AppCancelTimer()
    # is canceling anything as callers of that may be uncertain about what's
    # happening.
    
    # Method to provide a single callback after some amount of time.
    def AppAfter(self, msec, callback):
        if self.timer_callback:
            print 'AppAfter: timer_callback already set!',
        # print 'AppAfter:', callback
        self.timer_callback = callback
        self.timer.Start(msec, True)

    # Method to cancel something we might be waiting for but have lost
    # interest in.
    def AppCancelTimer(self):
        self.timer.Stop()
        # print 'AppCancelTimer'
        self.timer_callback = None

    # When the timer happens, we come here and jump off to clroses internal code.
    def OnTimer(self, evt):
        callback = self.timer_callback
        self.timer_callback = None
        # print 'OnTimer,', callback
        if callback:
            callback()		# Often calls AppAfter() and sets the callback
        else:
            print 'OnTimer: no callback!'


    resize_delay = 300
    
    def TriggerResize(self, size):
        self.resize(size, self.resize_delay)
        self.resize_delay = 100

    def TriggerRedraw(self):
        self.repaint(10)

    # Called when data in spin boxes changes.
    def OnSpinback(self, name, value):
        if verbose:
            print 'OnSpinback', name, value
        if name == 'Style':
            self.SetStyle(value)
        elif name == 'Sincr':
            self.SetSincr(value)
        elif name == 'Petal':
            self.SetPetals(value)
        elif name == 'Pincr':
            self.SetPincr(value)

        elif name == 'Vectors':
            self.SetVectors(value)
        elif name == 'Minimum':
            self.SetMinVec(value)
        elif name == 'Maximum':
            self.SetMaxVec(value)
        elif name == 'Skip first':
            self.SetSkipFirst(value)
        elif name == 'Draw only':
            self.SetDrawOnly(value)

        elif name == 'Vec/tick':
            self.SetStep(value)
        elif name == 'msec/tick':
            self.SetDrawDelay(value)
        elif name == 'Delay':
            self.SetWaitDelay(value)
        else:
            print 'OnSpinback: Don\'t recognize', name

verbose = 0			# Need some command line options...
spin_panels = {}		# Hooks to get from rose to panel labels
ctrl_buttons = {}		# Button widgets for command (NE) panel

app = wx.App(False)
MyFrame()
if verbose:
    print 'spin_panels', spin_panels.keys()
    print 'ctrl_buttons', ctrl_buttons.keys()
app.MainLoop()
