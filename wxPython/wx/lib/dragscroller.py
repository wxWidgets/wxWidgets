#-----------------------------------------------------------------------------
# Name:        dragscroller.py
# Purpose:     Scrolls a wx.ScrollWindow by dragging
#
# Author:      Riaan Booysen
#
# Created:     2006/09/05
# Copyright:   (c) 2006
# Licence:     wxPython
#-----------------------------------------------------------------------------

import wx

class DragScroller:
    """ Scrolls a wx.ScrollWindow in the direction and speed of a mouse drag.
    
        Call Start with the position of the drag start.
        Call Stop on the drag release. """

    def __init__(self, scrollwin, rate=30, sensitivity=0.75):
        self.scrollwin = scrollwin
        self.rate = rate
        self.sensitivity = sensitivity

        self.pos = None
        self.timer = None
    
    def GetScrollWindow(self):
        return self.scrollwin
    def SetScrollWindow(self, scrollwin):
        self.scrollwin = scrollwin
    
    def GetUpdateRate(self):
        return self.rate
    def SetUpdateRate(self, rate):
        self.rate = rate

    def GetSensitivity(self):
        return self.sensitivity
    def SetSensitivity(self, sensitivity):
        self.sensitivity = sensitivity

    def Start(self, pos):
        """ Start a drag scroll operation """
        if not self.scrollwin:
            raise Exception, 'No ScrollWindow defined'
        
        self.pos = pos
        self.scrollwin.SetCursor(wx.StockCursor(wx.CURSOR_SIZING))
        self.scrollwin.CaptureMouse()

        self.timer = wx.Timer(self.scrollwin)
        self.scrollwin.Bind(wx.EVT_TIMER, self.OnTimerDoScroll, id=self.timer.GetId())
        self.timer.Start(self.rate)
    
    def Stop(self):
        """ Stops a drag scroll operation """
        if self.timer and self.scrollwin:
            self.timer.Stop()
            self.scrollwin.Disconnect(self.timer.GetId())
            self.timer.Destroy()
            self.timer = None

            self.scrollwin.SetCursor(wx.STANDARD_CURSOR)
            self.scrollwin.ReleaseMouse()

    def OnTimerDoScroll(self, event):
        if self.pos is None or not self.timer or not self.scrollwin: 
            return
        
        new = self.scrollwin.ScreenToClient(wx.GetMousePosition())
        dx = int((new.x-self.pos.x)*self.sensitivity)
        dy = int((new.y-self.pos.y)*self.sensitivity)
        spx = self.scrollwin.GetScrollPos(wx.HORIZONTAL)
        spy = self.scrollwin.GetScrollPos(wx.VERTICAL)

        self.scrollwin.Scroll(spx+dx, spy+dy)
