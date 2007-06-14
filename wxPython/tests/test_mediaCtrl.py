import wx
import wx.media
from wx.lib.mixins.inspection import InspectableApp

import os; print os.getpid(); raw_input("press enter...")

class TestFrame(wx.Frame):
    def __init__(self, *args, **kw):
        wx.Frame.__init__(self, *args, **kw)
        self.CreateStatusBar()
        pnl = wx.Panel(self)
        
        self.mc = wx.media.MediaCtrl(pnl, size=(-1,200))
        #self.fn = "/home/robind/smb4k/BEAST/MyVideos/SageTV/48HoursMystery-6199728-0.mpg"

        self.Bind(wx.media.EVT_MEDIA_PLAY, self.OnMediaPlay, self.mc)
        self.Bind(wx.media.EVT_MEDIA_PAUSE, self.OnMediaPause, self.mc)
        self.Bind(wx.media.EVT_MEDIA_STOP, self.OnMediaStop, self.mc)
        self.Bind(wx.media.EVT_MEDIA_FINISHED, self.OnMediaFinished, self.mc)
        self.Bind(wx.media.EVT_MEDIA_LOADED, self.OnMediaLoaded, self.mc)

        selectBtn = wx.Button(pnl, -1, "Select")
        loadBtn = wx.Button(pnl, -1, "Load")
        playBtn = wx.Button(pnl, -1, "Play")
        pauseBtn= wx.Button(pnl, -1, "Pause")
        stopBtn = wx.Button(pnl, -1, "Stop")

        self.Bind(wx.EVT_BUTTON, self.OnBtnSelect, selectBtn)
        self.Bind(wx.EVT_BUTTON, self.OnBtnLoad, loadBtn)
        self.Bind(wx.EVT_BUTTON, self.OnBtnPlay, playBtn)
        self.Bind(wx.EVT_BUTTON, self.OnBtnPause, pauseBtn)
        self.Bind(wx.EVT_BUTTON, self.OnBtnStop, stopBtn)
        

        vbox = wx.BoxSizer(wx.VERTICAL)
        vbox.AddF(self.mc, wx.SizerFlags(1).Expand().Border(wx.ALL, 5))
        hbox = wx.BoxSizer(wx.HORIZONTAL)
        hbox.AddF(selectBtn, wx.SizerFlags().Border())
        hbox.AddF(loadBtn, wx.SizerFlags().Border())
        hbox.AddF(playBtn, wx.SizerFlags().Border())
        hbox.AddF(pauseBtn, wx.SizerFlags().Border())
        hbox.AddF(stopBtn, wx.SizerFlags().Border())
        vbox.Add(hbox)        
        pnl.SetSizer(vbox)

        sizer = wx.BoxSizer()
        sizer.AddF(pnl, wx.SizerFlags(1).Expand())
        self.SetSizer(sizer)
        wx.CallLater(200, self.SetClientSize, pnl.GetBestSize())

        self.timer = wx.Timer(self)
        self.Bind(wx.EVT_TIMER, self.OnShowStatus)
        self.timer.Start(500)
        

    def OnBtnSelect(self, evt):
        name = wx.FileSelector("Choose media file")
        if name:
            self.fn = name

    def OnBtnLoad(self, evt):
        print "Loading %s..." % self.fn
        print "Load:", self.mc.Load(self.fn)
        
    def OnBtnPlay(self, evt):
        print "Play:", self.mc.Play()
        
    def OnBtnPause(self, evt):
        print "Pause:", self.mc.Pause()
        
    def OnBtnStop(self, evt):
        print "Stop:", self.mc.Stop()
        

    def OnShowStatus(self, evt):
        current = self.mc.Tell()
        length = self.mc.Length()

        dl_current = self.mc.GetDownloadProgress()
        dl_length = self.mc.GetDownloadTotal()

        cw,ch = self.mc.GetSize()
        bw,bh = self.mc.GetBestSize()

        vol = self.mc.GetVolume()
        
        stateMap = { wx.media.MEDIASTATE_PLAYING : "Playing",
                     wx.media.MEDIASTATE_PAUSED  : "Paused",
                     wx.media.MEDIASTATE_STOPPED : "Stopped", }
        state = stateMap.get(self.mc.GetState(), "Other")

        st = "Size:%d,%d  BSize:%d,%d  Pos:%d/%d  DL:%d/%d  V:%d  %s" % (
            cw,ch, bw,bh, current, length, dl_current, dl_length, vol*100, state)
        
        self.SetStatusText(st)

        

    def OnMediaPlay(self, evt):
        print "OnMediaPlay"

    def OnMediaPause(self, evt):
        print "OnMediaPause"

    def OnMediaStop(self, evt):
        print "OnMediaStop"

    def OnMediaFinished(self, evt):
        print "OnMediaFinished"

    def OnMediaLoaded(self, evt):
        print "OnMediaLoaded"

        

app = InspectableApp(False)
frm = TestFrame(None, title="simple wxMediaCtrl test")
frm.Show()
app.MainLoop()

