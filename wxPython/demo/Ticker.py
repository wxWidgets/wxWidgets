
import wx             
from wx.lib.ticker import Ticker
import wx.lib.colourselect as  csel     #for easy color selection

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)
        
        self.ticker = Ticker(self)
        
        #       Controls for ...controlling... the ticker.
        self.txt = wx.TextCtrl(self, value="I am a scrolling ticker!!!!", size=(200,-1))
        wx.CallAfter(self.txt.SetInsertionPoint, 0)
        txtl = wx.StaticText(self, label="Ticker text:")
        fgb = csel.ColourSelect(self, -1, colour=self.ticker.GetForegroundColour())
        fgl = wx.StaticText(self, label="Foreground Color:")
        bgb = csel.ColourSelect(self, -1, colour=self.ticker.GetBackgroundColour())
        bgl = wx.StaticText(self, label="Background Color:")
        fontb = wx.Button(self, label="Change")
        self.fontl = wx.StaticText(self)
        dirb = wx.Button(self, label="Switch")
        self.dirl = wx.StaticText(self)
        fpsl = wx.StaticText(self, label="Frames per Second:")
        fps = wx.Slider(self, value=self.ticker.GetFPS(), minValue=1, maxValue=100,
                        size=(150,-1),
                        style=wx.SL_HORIZONTAL|wx.SL_AUTOTICKS|wx.SL_LABELS)
        fps.SetTickFreq(5)
        ppfl = wx.StaticText(self, label="Pixels per frame:")
        ppf = wx.Slider(self, value=self.ticker.GetPPF(), minValue=1, maxValue=10,
                        size=(150,-1),
                        style=wx.SL_HORIZONTAL|wx.SL_AUTOTICKS|wx.SL_LABELS)
        
        #       Do layout
        sz = wx.FlexGridSizer(cols=2, hgap=4, vgap=4)
        
        sz.Add(txtl, flag=wx.ALIGN_CENTER_VERTICAL)
        sz.Add(self.txt, flag=wx.ALIGN_CENTER_VERTICAL)
        
        sz.Add(fgl, flag=wx.ALIGN_CENTER_VERTICAL)
        sz.Add(fgb, flag=wx.ALIGN_CENTER_VERTICAL)
        
        sz.Add(bgl, flag=wx.ALIGN_CENTER_VERTICAL)
        sz.Add(bgb, flag=wx.ALIGN_CENTER_VERTICAL)
        
        sz.Add(self.fontl, flag=wx.ALIGN_CENTER_VERTICAL)
        sz.Add(fontb, flag=wx.ALIGN_CENTER_VERTICAL)
        
        sz.Add(self.dirl, flag=wx.ALIGN_CENTER_VERTICAL)
        sz.Add(dirb, flag=wx.ALIGN_CENTER_VERTICAL)
        
        sz.Add(fpsl, flag=wx.ALIGN_CENTER_VERTICAL)
        sz.Add(fps, flag=wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_LEFT)
        
        sz.Add(ppfl, flag=wx.ALIGN_CENTER_VERTICAL)
        sz.Add(ppf, flag=wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_LEFT)
        
        sz2 = wx.BoxSizer(wx.VERTICAL)
        sz2.Add(self.ticker, flag=wx.EXPAND|wx.ALL, border=5)
        sz2.Add(sz, flag=wx.EXPAND|wx.ALL, proportion=1, border=25)
        self.SetSizer(sz2)
        sz2.SetSizeHints(self)
        
        #       Bind events
        self.Bind(wx.EVT_BUTTON, self.OnChangeTickDirection, dirb)
        self.Bind(wx.EVT_BUTTON, self.OnChangeTickFont, fontb)
        self.Bind(wx.EVT_TEXT, self.OnText, self.txt)
        self.Bind(csel.EVT_COLOURSELECT, self.ChangeTickFGColor, fgb)
        self.Bind(csel.EVT_COLOURSELECT, self.ChangeTickBGColor, bgb)
        self.Bind(wx.EVT_SCROLL, self.ChangeFPS, fps)
        self.Bind(wx.EVT_SCROLL, self.ChangePPF, ppf)
        
        #       Set defaults
        self.SetTickDirection("rtl")
        self.SetTickFont(self.ticker.GetFont())
        self.ticker.SetText(self.txt.GetValue())

        
    def SetTickFont(self, font):
        """Sets ticker font, updates label"""
        self.ticker.SetFont(font)
        self.fontl.SetLabel("Font: %s"%(self.ticker.GetFont().GetFaceName()))
        self.Layout()
        
        
    def OnChangeTickFont(self, evt):
        fd = wx.FontData()
        fd.EnableEffects(False)
        fd.SetInitialFont(self.ticker.GetFont())
        dlg = wx.FontDialog(wx.GetTopLevelParent(self), fd)
        if dlg.ShowModal() == wx.ID_OK:
            data = dlg.GetFontData()
            self.SetTickFont(data.GetChosenFont())
            
            
    def SetTickDirection(self, dir):
        """Sets tick direction, updates label"""
        self.ticker.SetDirection(dir)
        self.dirl.SetLabel("Direction: %s"%(self.ticker.GetDirection()))
        
        
    def OnChangeTickDirection(self, dir):
        if self.ticker.GetDirection() == "rtl":
            self.SetTickDirection("ltr")
        else:
            self.SetTickDirection("rtl")
            
            
    def OnText(self, evt):
        """Live update of the ticker text"""
        self.ticker.SetText(self.txt.GetValue())
        
    def ChangeTickFGColor(self, evt):
        self.ticker.SetForegroundColour(evt.GetValue())
        
    def ChangeTickBGColor(self, evt):
        self.ticker.SetBackgroundColour(evt.GetValue())
        
    def ChangeFPS(self, evt):
        self.ticker.SetFPS(evt.GetPosition())
        
    def ChangePPF(self, evt):
        self.ticker.SetPPF(evt.GetPosition())


    def ShutdownDemo(self):
        self.ticker.Stop()

        
#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = wx.lib.ticker.__doc__



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

