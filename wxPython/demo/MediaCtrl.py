
import wx
import wx.media
import os

#----------------------------------------------------------------------

class StaticText(wx.StaticText):
    """
    A StaticText that only updates the label if it has changed, to
    help reduce potential flicker since these controls would be
    updated very frequently otherwise.
    """
    def SetLabel(self, label):
        if label <> self.GetLabel():
            wx.StaticText.SetLabel(self, label)

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1,
                          style=wx.TAB_TRAVERSAL|wx.CLIP_CHILDREN)

        # Create some controls
        try:
            self.mc = wx.media.MediaCtrl(self, style=wx.SIMPLE_BORDER,
                                         #szBackend=wx.media.MEDIABACKEND_DIRECTSHOW
                                         #szBackend=wx.media.MEDIABACKEND_QUICKTIME
                                         #szBackend=wx.media.MEDIABACKEND_WMP10
                                         )
        except NotImplementedError:
            self.Destroy()
            raise

        self.Bind(wx.media.EVT_MEDIA_LOADED, self.OnMediaLoaded)

        btn1 = wx.Button(self, -1, "Load File")
        self.Bind(wx.EVT_BUTTON, self.OnLoadFile, btn1)
        
        btn2 = wx.Button(self, -1, "Play")
        self.Bind(wx.EVT_BUTTON, self.OnPlay, btn2)
        self.playBtn = btn2
        
        btn3 = wx.Button(self, -1, "Pause")
        self.Bind(wx.EVT_BUTTON, self.OnPause, btn3)
        
        btn4 = wx.Button(self, -1, "Stop")
        self.Bind(wx.EVT_BUTTON, self.OnStop, btn4)

        slider = wx.Slider(self, -1, 0, 0, 0)
        self.slider = slider
        slider.SetMinSize((150, -1))
        self.Bind(wx.EVT_SLIDER, self.OnSeek, slider)

        self.st_size = StaticText(self, -1, size=(100,-1))
        self.st_len  = StaticText(self, -1, size=(100,-1))
        self.st_pos  = StaticText(self, -1, size=(100,-1))
        
        
        # setup the layout
        sizer = wx.GridBagSizer(5,5)
        sizer.Add(self.mc, (1,1), span=(5,1))#, flag=wx.EXPAND)
        sizer.Add(btn1, (1,3))
        sizer.Add(btn2, (2,3))
        sizer.Add(btn3, (3,3))
        sizer.Add(btn4, (4,3))
        sizer.Add(slider, (6,1), flag=wx.EXPAND)
        sizer.Add(self.st_size, (1, 5))
        sizer.Add(self.st_len,  (2, 5))
        sizer.Add(self.st_pos,  (3, 5))
        self.SetSizer(sizer)

        #self.DoLoadFile(os.path.abspath("data/testmovie.mpg"))
        wx.CallAfter(self.DoLoadFile, os.path.abspath("data/testmovie.mpg"))

        self.timer = wx.Timer(self)
        self.Bind(wx.EVT_TIMER, self.OnTimer)
        self.timer.Start(100)
        
        

    def OnLoadFile(self, evt):
        dlg = wx.FileDialog(self, message="Choose a media file",
                            defaultDir=os.getcwd(), defaultFile="",
                            style=wx.OPEN | wx.CHANGE_DIR )
        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
            self.DoLoadFile(path)
        dlg.Destroy()


    def DoLoadFile(self, path):
        self.playBtn.Disable()
        #noLog = wx.LogNull()
        if not self.mc.Load(path):
            wx.MessageBox("Unable to load %s: Unsupported format?" % path,
                          "ERROR",
                          wx.ICON_ERROR | wx.OK)
        else:
            self.mc.SetInitialSize()
            self.GetSizer().Layout()
            self.slider.SetRange(0, self.mc.Length())

    def OnMediaLoaded(self, evt):
        self.playBtn.Enable()
    
    def OnPlay(self, evt):
        if not self.mc.Play():
            wx.MessageBox("Unable to Play media : Unsupported format?",
                          "ERROR",
                          wx.ICON_ERROR | wx.OK)
        else:
            self.mc.SetInitialSize()
            self.GetSizer().Layout()
            self.slider.SetRange(0, self.mc.Length())

    def OnPause(self, evt):
        self.mc.Pause()
    
    def OnStop(self, evt):
        self.mc.Stop()
    

    def OnSeek(self, evt):
        offset = self.slider.GetValue()
        self.mc.Seek(offset)

    def OnTimer(self, evt):
        offset = self.mc.Tell()
        self.slider.SetValue(offset)
        self.st_size.SetLabel('size: %s' % self.mc.GetBestSize())
        self.st_len.SetLabel('length: %d seconds' % (self.mc.Length()/1000))
        self.st_pos.SetLabel('position: %d' % offset)

    def ShutdownDemo(self):
        self.timer.Stop()
        del self.timer

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    try:
        win = TestPanel(nb, log)
        return win
    except NotImplementedError:
        from Main import MessagePanel
        win = MessagePanel(nb, 'wx.MediaCtrl is not available on this platform.',
                           'Sorry', wx.ICON_WARNING)
        return win
    

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>wx.MediaCtrl</center></h2>

wx.MediaCtrl is a class that allows a way to convieniently display
various types of media, such as videos, audio files, natively through
native codecs.  Several different formats of audio and video files are
supported, but some formats may not be playable on all platforms or
may require specific codecs to be installed.

<p>
wx.MediaCtrl uses native backends to render media, for example on Windows
there is a ActiveMovie/DirectShow backend, and on Macintosh there is a 
QuickTime backend.
<p>
wx.MediaCtrl is not currently available on unix systems. 

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
