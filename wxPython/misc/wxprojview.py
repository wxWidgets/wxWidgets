#!/usr/bin/env python

import wx
import wx.lib.stattext as st
import os

class MyFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, title="wx Active Project",
                          style=wx.FRAME_NO_TASKBAR|wx.STAY_ON_TOP,
                          name="wxprojview"
                          )
        p = wx.Panel(self)#, style=wx.SIMPLE_BORDER)
        
        p.SetBackgroundColour("sky blue")
        self.label = st.GenStaticText(p, -1, "wx XXX")
        self.label.SetBackgroundColour("sky blue")
        self.label.SetFont(wx.Font(14, wx.SWISS, wx.NORMAL, wx.BOLD))
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(self.label, 1, wx.ALIGN_CENTER|wx.ALL, 2)
        p.SetSizerAndFit(sizer)
        self.SetClientSize(p.GetSize())

        for obj in [p, self.label]:
            obj.Bind(wx.EVT_LEFT_DOWN, self.OnLeftDown)
            obj.Bind(wx.EVT_LEFT_UP,   self.OnLeftUp)
            obj.Bind(wx.EVT_MOTION,    self.OnMouseMove)
            obj.Bind(wx.EVT_RIGHT_UP,  self.OnRightUp)

        cfg = wx.Config.Get()
        cfg.SetPath("/")
        if cfg.Exists("Pos"):
            pos = eval(cfg.Read("Pos"))
            # TODO: ensure this position is on-screen
            self.SetPosition(pos)

        self.Bind(wx.EVT_CLOSE, self.OnClose)
        self.Bind(wx.EVT_TIMER, self.OnUpdateVersion)
        self.timer = wx.Timer(self)
        self.timer.Start(5000)
        self.OnUpdateVersion(None)


    def OnUpdateVersion(self, evt):
        ver = '??'
        if 'wxMSW' in wx.PlatformInfo:
            pass
        else:
            link = '/opt/wx/current'
            if os.path.islink(link):
                rp = os.path.realpath(link)
                ver = os.path.split(rp)[1]
        label = 'wx %s' % ver
        if label != self.label.GetLabel():
            self.label.SetLabel(label)
            self.label.GetContainingSizer().Layout()


    def OnClose(self, evt):
        cfg = wx.Config.Get()
        cfg.SetPath("/")
        cfg.Write("Pos", str(self.GetPosition().Get()))
        self.timer.Stop()
        evt.Skip()
        
        

    def OnLeftDown(self, evt):
        win = evt.GetEventObject()
        win.CaptureMouse()
        self.capture = win
        pos = win.ClientToScreen(evt.GetPosition())
        origin = self.GetPosition()
        dx = pos.x - origin.x
        dy = pos.y - origin.y
        self.delta = wx.Point(dx, dy)

    def OnLeftUp(self, evt):
        if self.capture.HasCapture():
            self.capture.ReleaseMouse()

    def OnMouseMove(self, evt):
        if evt.Dragging() and evt.LeftIsDown():
            win = evt.GetEventObject()
            pos = win.ClientToScreen(evt.GetPosition())
            fp = (pos.x - self.delta.x, pos.y - self.delta.y)
            self.Move(fp)

    def OnRightUp(self, evt):
        self.Close()

        

app = wx.PySimpleApp()
app.SetAppName("wxprojview")
app.SetVendorName("Robin Dunn")
frm = MyFrame()
frm.Show()
app.MainLoop()
