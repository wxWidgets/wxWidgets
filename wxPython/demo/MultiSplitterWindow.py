
import wx
from wx.lib.splitter import MultiSplitterWindow

#----------------------------------------------------------------------

class SamplePane(wx.Panel):
    """
    Just a simple test window to put into the splitter.
    """
    def __init__(self, parent, colour, label):
        wx.Panel.__init__(self, parent, style=wx.BORDER_SUNKEN)
        self.SetBackgroundColour(colour)
        wx.StaticText(self, -1, label, (5,5))

    def SetOtherLabel(self, label):
        wx.StaticText(self, -1, label, (5, 30))



class ControlPane(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)

        hvBox = wx.RadioBox(self, -1, "Orientation",
                            choices=["Horizontal", "Vertical"],
                            style=wx.RA_SPECIFY_COLS,
                            majorDimension=1)
        hvBox.SetSelection(0)
        self.Bind(wx.EVT_RADIOBOX, self.OnSetHV, hvBox)
        
        luCheck = wx.CheckBox(self, -1, "Live Update")
        luCheck.SetValue(True)
        self.Bind(wx.EVT_CHECKBOX, self.OnSetLiveUpdate, luCheck)

        btn = wx.Button(self, -1, "Swap 2 && 4")
        self.Bind(wx.EVT_BUTTON, self.OnSwapButton, btn)

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(hvBox)
        sizer.Add(luCheck, 0, wx.TOP, 5)
        sizer.Add(btn, 0, wx.TOP, 5)
        border = wx.BoxSizer()
        border.Add(sizer, 1, wx.EXPAND|wx.ALL, 5)
        self.SetSizer(border)


    def OnSetHV(self, evt):
        rb = evt.GetEventObject()
        self.GetParent().SetOrientation(rb.GetSelection())
        

    def OnSetLiveUpdate(self, evt):
        check = evt.GetEventObject()
        self.GetParent().SetLiveUpdate(check.GetValue())


    def OnSwapButton(self, evt):
        self.GetParent().Swap2and4()
        


class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        cp = ControlPane(self)
        
        splitter = MultiSplitterWindow(self, style=wx.SP_LIVE_UPDATE)
        self.splitter = splitter
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        sizer.Add(cp)
        sizer.Add(splitter, 1, wx.EXPAND)
        self.SetSizer(sizer)


        p1 = SamplePane(splitter, "pink", "Panel One")
        p1.SetOtherLabel(
            "There are two sash\n"
            "drag modes. Try\n"
            "dragging with and\n"
            "without the Shift\n"
            "key held down."
            )
        splitter.AppendWindow(p1, 140)

        p2 = SamplePane(splitter, "sky blue", "Panel Two")
        p2.SetOtherLabel("This window\nhas a\nminsize.")
        p2.SetMinSize(p2.GetBestSize())
        splitter.AppendWindow(p2, 150)

        p3 = SamplePane(splitter, "yellow", "Panel Three")
        splitter.AppendWindow(p3, 125)

        p4 = SamplePane(splitter, "Lime Green", "Panel Four")
        splitter.AppendWindow(p4)

        self.Bind(wx.EVT_SPLITTER_SASH_POS_CHANGED, self.OnChanged)
        self.Bind(wx.EVT_SPLITTER_SASH_POS_CHANGING, self.OnChanging)


    def OnChanging(self, evt):
        self.log.write( "Changing sash:%d  %s\n" %
                        (evt.GetSashIdx(), evt.GetSashPosition()))
        # This is one way to control the sash limits
        #if evt.GetSashPosition() < 50:
        #    evt.Veto()

        # Or you can reset the sash position to whatever you want
        #if evt.GetSashPosition() < 5:
        #    evt.SetSashPosition(25)


    def OnChanged(self, evt):
        self.log.write( "Changed sash:%d  %s\n" %
                        (evt.GetSashIdx(), evt.GetSashPosition()))

            
    def SetOrientation(self, value):
        if value:
            self.splitter.SetOrientation(wx.VERTICAL)
        else:
            self.splitter.SetOrientation(wx.HORIZONTAL)
        self.splitter.SizeWindows()

            
    def SetLiveUpdate(self, enable):
        if enable:
            self.splitter.SetWindowStyle(wx.SP_LIVE_UPDATE)
        else:
            self.splitter.SetWindowStyle(0)
            

    def Swap2and4(self):
        win2 = self.splitter.GetWindow(1)
        win4 = self.splitter.GetWindow(3)
        self.splitter.ExchangeWindows(win2, win4)

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>MultiSplitterWindow</center></h2>

This class is very similar to wx.SplitterWindow except that it
allows for more than two windows and more than one sash.  Many of
the same styles, constants, and methods behave the same as in
wx.SplitterWindow.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

