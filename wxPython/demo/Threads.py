
import  random
import  time
import  thread

import  wx
import  wx.lib.newevent

#----------------------------------------------------------------------

# This creates a new Event class and a EVT binder function
(UpdateBarEvent, EVT_UPDATE_BARGRAPH) = wx.lib.newevent.NewEvent()


#----------------------------------------------------------------------

class CalcBarThread:
    def __init__(self, win, barNum, val):
        self.win = win
        self.barNum = barNum
        self.val = val

    def Start(self):
        self.keepGoing = self.running = True
        thread.start_new_thread(self.Run, ())

    def Stop(self):
        self.keepGoing = False

    def IsRunning(self):
        return self.running

    def Run(self):
        while self.keepGoing:
            evt = UpdateBarEvent(barNum = self.barNum, value = int(self.val))
            wx.PostEvent(self.win, evt)
 
            sleeptime = (random.random() * 2) + 0.5
            time.sleep(sleeptime/4)

            sleeptime = sleeptime * 5
            if int(random.random() * 2):
                self.val = self.val + sleeptime
            else:
                self.val = self.val - sleeptime

            if self.val < 0: self.val = 0
            if self.val > 300: self.val = 300

        self.running = False

#----------------------------------------------------------------------


class GraphWindow(wx.Window):
    def __init__(self, parent, labels):
        wx.Window.__init__(self, parent, -1)

        self.values = []
        for label in labels:
            self.values.append((label, 0))

        font = wx.Font(12, wx.SWISS, wx.NORMAL, wx.BOLD)
        self.SetFont(font)

        self.colors = [ wx.RED, wx.GREEN, wx.BLUE, wx.CYAN,
                        "Yellow", "Navy" ]

        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)
        self.Bind(wx.EVT_PAINT, self.OnPaint)


    def SetValue(self, index, value):
        assert index < len(self.values)
        cur = self.values[index]
        self.values[index:index+1] = [(cur[0], value)]


    def SetFont(self, font):
        wx.Window.SetFont(self, font)
        wmax = hmax = 0
        for label, val in self.values:
            w,h = self.GetTextExtent(label)
            if w > wmax: wmax = w
            if h > hmax: hmax = h
        self.linePos = wmax + 10
        self.barHeight = hmax


    def GetBestHeight(self):
        return 2 * (self.barHeight + 1) * len(self.values)


    def Draw(self, dc, size):
        dc.SetFont(self.GetFont())
        dc.SetTextForeground(wx.BLUE)
        dc.SetBackground(wx.Brush(self.GetBackgroundColour()))
        dc.Clear()
        dc.SetPen(wx.Pen(wx.BLACK, 3, wx.SOLID))
        dc.DrawLine(self.linePos, 0, self.linePos, size.height-10)

        bh = ypos = self.barHeight
        for x in range(len(self.values)):
            label, val = self.values[x]
            dc.DrawText(label, 5, ypos)

            if val:
                color = self.colors[ x % len(self.colors) ]
                dc.SetPen(wx.Pen(color))
                dc.SetBrush(wx.Brush(color))
                dc.DrawRectangle(self.linePos+3, ypos, val, bh)

            ypos = ypos + 2*bh
            if ypos > size[1]-10:
                break


    def OnPaint(self, evt):
        dc = wx.BufferedPaintDC(self)
        self.Draw(dc, self.GetSize())


    def OnEraseBackground(self, evt):
        pass




#----------------------------------------------------------------------

class TestFrame(wx.Frame):
    def __init__(self, parent, log):
        wx.Frame.__init__(self, parent, -1, "Thread Test", size=(450,300))
        self.log = log

        #self.CenterOnParent()

        panel = wx.Panel(self, -1)
        panel.SetFont(wx.Font(10, wx.SWISS, wx.NORMAL, wx.BOLD))
        wx.StaticText(panel, -1,
                     "This demo shows multiple threads interacting with this\n"
                     "window by sending events to it, one thread for each bar.",
                     (5,5))
        panel.Fit()

        self.graph = GraphWindow(self, ['Zero', 'One', 'Two', 'Three', 'Four',
                                        'Five', 'Six', 'Seven'])
        self.graph.SetSize((450, self.graph.GetBestHeight()))

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(panel, 0, wx.EXPAND)
        sizer.Add(self.graph, 1, wx.EXPAND)

        self.SetSizer(sizer)
        self.SetAutoLayout(True)
        sizer.Fit(self)

        self.Bind(EVT_UPDATE_BARGRAPH, self.OnUpdate)

        self.threads = []
        self.threads.append(CalcBarThread(self, 0, 50))
        self.threads.append(CalcBarThread(self, 1, 75))
        self.threads.append(CalcBarThread(self, 2, 100))
        self.threads.append(CalcBarThread(self, 3, 150))
        self.threads.append(CalcBarThread(self, 4, 225))
        self.threads.append(CalcBarThread(self, 5, 300))
        self.threads.append(CalcBarThread(self, 6, 250))
        self.threads.append(CalcBarThread(self, 7, 175))

        for t in self.threads:
            t.Start()

        self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)


    def OnUpdate(self, evt):
        self.graph.SetValue(evt.barNum, evt.value)
        self.graph.Refresh(False)


    def OnCloseWindow(self, evt):
        busy = wx.BusyInfo("One moment please, waiting for threads to die...")
        wx.Yield()

        for t in self.threads:
            t.Stop()

        running = 1

        while running:
            running = 0

            for t in self.threads:
                running = running + t.IsRunning()

            time.sleep(0.1)

        self.Destroy()



#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        b = wx.Button(self, -1, "Show Threads sample", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)


    def OnButton(self, evt):
        win = TestFrame(self, self.log)
        win.Show(True)


#---------------------------------------------------------------------------


def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------





overview = """\
The main issue with multi-threaded GUI programming is the thread safty
of the GUI itself.  On most platforms the GUI is not thread safe and
so any cross platform GUI Toolkit and applications written with it
need to take that into account.

The solution is to only allow interaction with the GUI from a single
thread, but this often severely limits what can be done in an
application and makes it difficult to use additional threads at all.

Since wxPython already makes extensive use of event handlers, it is a
logical extension to allow events to be sent to GUI objects from
alternate threads.  A function called wx.PostEvent allows you to do
this.  It accepts an event and an event handler (window) and instead
of sending the event immediately in the current context like
ProcessEvent does, it processes it later from the context of the GUI
thread.

"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

