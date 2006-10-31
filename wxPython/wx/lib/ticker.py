#----------------------------------------------------------------------
# Name:        wx.lib.ticker
# Purpose:     A news-ticker style scrolling text control
#
# Author:      Chris Mellon
#
# Created:     29-Aug-2004
# RCS-ID:      $Id$
# Copyright:   (c) 2004 by Chris Mellon
# Licence:     wxWindows license
#----------------------------------------------------------------------

"""News-ticker style scrolling text control

    * Can scroll from right to left or left to right.
    
    * Speed of the ticking is controlled by two parameters:
    
      - Frames per Second(FPS): How many times per second the ticker updates
      
      - Pixels per Frame(PPF): How many pixels the text moves each update

Low FPS with high PPF will result in "jumpy" text, lower PPF with higher FPS
is smoother (but blurrier and more CPU intensive) text.
"""

import wx

#----------------------------------------------------------------------

class Ticker(wx.PyControl):
    def __init__(self, 
            parent, 
            id=-1, 
            text=wx.EmptyString,        #text in the ticker
            fgcolor = wx.BLACK,         #text/foreground color
            bgcolor = wx.WHITE,         #background color
            start=True,                 #if True, the ticker starts immediately
            ppf=2,                      #pixels per frame
            fps=20,                     #frames per second
            direction="rtl",            #direction of ticking, rtl or ltr
            pos=wx.DefaultPosition, size=wx.DefaultSize, style=wx.NO_BORDER, 
            name="Ticker"
        ):
        wx.PyControl.__init__(self, parent, id=id, pos=pos, size=size, style=style, name=name)
        self.timer = wx.Timer(owner=self)
        self._extent = (-1, -1)  #cache value for the GetTextExtent call
        self._offset = 0
        self._fps = fps  #frames per second
        self._ppf = ppf  #pixels per frame
        self.SetDirection(direction)
        self.SetText(text)
        self.SetInitialSize(size)
        self.SetForegroundColour(fgcolor)
        self.SetBackgroundColour(bgcolor)
        wx.EVT_TIMER(self, -1, self.OnTick)
        wx.EVT_PAINT(self, self.OnPaint)
        wx.EVT_ERASE_BACKGROUND(self, self.OnErase)
        if start:
            self.Start()

            
    def Stop(self):
        """Stop moving the text"""
        self.timer.Stop()

        
    def Start(self):
        """Starts the text moving"""
        if not self.timer.IsRunning():
            self.timer.Start(1000 / self._fps)

    
    def IsTicking(self):
        """Is the ticker ticking? ie, is the text moving?"""
        return self.timer.IsRunning()

        
    def SetFPS(self, fps):
        """Adjust the update speed of the ticker"""
        self._fps = fps
        self.Stop()
        self.Start()

        
    def GetFPS(self):
        """Update speed of the ticker"""
        return self._fps

        
    def SetPPF(self, ppf):
        """Set the number of pixels per frame the ticker moves - ie, how "jumpy" it is"""
        self._ppf = ppf

        
    def GetPPF(self):
        """Pixels per frame"""
        return self._ppf

        
    def SetFont(self, font):
        self._extent = (-1, -1)
        wx.Control.SetFont(self, font)

        
    def SetDirection(self, dir):
        """Sets the direction of the ticker: right to left(rtl) or left to right (ltr)"""
        if dir == "ltr" or dir == "rtl":
            if self._offset <> 0:
                #Change the offset so it's correct for the new direction
                self._offset = self._extent[0] + self.GetSize()[0] - self._offset
            self._dir = dir
        else:
            raise TypeError

            
    def GetDirection(self):
        return self._dir

        
    def SetText(self, text):
        """Set the ticker text."""
        self._text = text
        self._extent = (-1, -1)
        if not self._text:
            self.Refresh() #Refresh here to clear away the old text.
            
            
    def GetText(self):
        return self._text

        
    def UpdateExtent(self, dc):
        """Updates the cached text extent if needed"""
        if not self._text:
            self._extent = (-1, -1)
            return
        if self._extent == (-1, -1):
            self._extent = dc.GetTextExtent(self.GetText())
            
            
    def DrawText(self, dc):
        """Draws the ticker text at the current offset using the provided DC"""
        dc.SetTextForeground(self.GetForegroundColour())
        dc.SetFont(self.GetFont())
        self.UpdateExtent(dc)
        if self._dir == "ltr":
            offx = self._offset - self._extent[0]
        else:
            offx = self.GetSize()[0] - self._offset
        offy = (self.GetSize()[1] - self._extent[1]) / 2 #centered vertically
        dc.DrawText(self._text, offx, offy)
        
        
    def OnTick(self, evt):
        self._offset += self._ppf
        w1 = self.GetSize()[0]
        w2 = self._extent[0]
        if self._offset >= w1+w2:
            self._offset = 0
        self.Refresh()
        
        
    def OnPaint(self, evt):
        dc = wx.BufferedPaintDC(self)
        brush = wx.Brush(self.GetBackgroundColour())
        dc.SetBackground(brush)
        dc.Clear()
        self.DrawText(dc)
        
        
    def OnErase(self, evt):
        """Noop because of double buffering"""
        pass
        

    def AcceptsFocus(self):
        """Non-interactive, so don't accept focus"""
        return False

        
    def DoGetBestSize(self):
        """Width we don't care about, height is either -1, or the character
        height of our text with a little extra padding
        """
        if self._extent == (-1, -1):
            if not self._text:
                h = self.GetCharHeight()
            else:
                h = self.GetTextExtent(self.GetText())[1]
        else:
            h = self._extent[1]
        return (100, h+5)


    def ShouldInheritColours(self): 
        """Don't get colours from our parent..."""
        return False
        


#testcase/demo
if __name__ == '__main__':
    app = wx.PySimpleApp()
    f = wx.Frame(None)
    p = wx.Panel(f)
    t = Ticker(p, text="Some sample ticker text")   
    #set ticker properties here if you want
    s = wx.BoxSizer(wx.VERTICAL)
    s.Add(t, flag=wx.GROW, proportion=0)
    p.SetSizer(s)
    f.Show()
    app.MainLoop()
    
    
