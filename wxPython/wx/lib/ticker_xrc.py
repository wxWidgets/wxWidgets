#----------------------------------------------------------------------
# Name:        wx.lib.ticker_xrc
# Purpose:     A XRC handler for wx.lib.ticker
#
# Author:      Chris Mellon
#
# Created:     17-May-2005
# RCS-ID:      $Id$
# Copyright:   (c) 2005 by Chris Mellon
# Licence:     wxWindows license
#----------------------------------------------------------------------

import wx
import wx.xrc as xrc
from wx.lib.ticker import Ticker

class wxTickerXmlHandler(xrc.XmlResourceHandler):
    def __init__(self):
        xrc.XmlResourceHandler.__init__(self)
        self.AddWindowStyles()
        
    def CanHandle(self, node):
        return self.IsOfClass(node, "wxTicker")
    
    def DoCreateResource(self):
        t = Ticker(
            self.GetParentAsWindow(),
            self.GetID(),
            pos = self.GetPosition(),
            size = self.GetSize(),
            style=self.GetStyle()
        )
        if self.HasParam("text"):
            t.SetText(self.GetText("text"))
        if self.HasParam("start"):
            if self.GetBool("start"):
                t.Start()
            else:
                t.Stop()
        if self.HasParam("ppf"):
            t.SetPPF(self.GetLong("ppf"))
        if self.HasParam("fps"):
            t.SetFPS(self.GetLong("fps"))
        if self.HasParam("direction"):
            t.SetDirection(self.GetText("direction"))
        
        self.SetupWindow(t)     # handles font, bg/fg color
        return t

