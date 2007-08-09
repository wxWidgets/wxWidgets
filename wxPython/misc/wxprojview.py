#!/usr/bin/env python
#----------------------------------------------------------------------
#
#  A little app I use on my system to help me remember which version of
#  wx I am corrently working on.  I don't expect this to work for anybody
#  else as it uses things that are specific to my setup.
#
#----------------------------------------------------------------------

import wx
import wx.lib.stattext as st
import os
import sys

class MyFrame(wx.Frame):
    def __init__(self, style=0):
        wx.Frame.__init__(self, None, title="wx Active Project",
                          style=wx.FRAME_NO_TASKBAR | style
                          ,name="wxprojview"
                          )
        p = wx.Panel(self)
        
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
        ver = '?.?'
        if 'wxMSW' in wx.PlatformInfo:
            info = open("c:/wxcurenv").read()
            p1 = info.find("WXCUR=") + 6
            p2 = info.find("\n", p1)
            ver = info[p1:p2]
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
style = wx.STAY_ON_TOP
if len(sys.argv) > 1 and sys.argv[1] == 'nostayontop':
    style=0
frm = MyFrame(style)
frm.Show()
app.MainLoop()
