
import wx
import wx.media
import os

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1,
                          style=wx.TAB_TRAVERSAL|wx.CLIP_CHILDREN)

        # Create some controls
        self.mc = wx.media.MediaCtrl(self)
        self.mc.SetMinSize((320,200))

        btn1 = wx.Button(self, -1, "Load File")
        self.Bind(wx.EVT_BUTTON, self.OnLoadFile, btn1)
        
        btn2 = wx.Button(self, -1, "Load URL")
        self.Bind(wx.EVT_BUTTON, self.OnLoadURI, btn2)
        
        btn3 = wx.Button(self, -1, "Play")
        self.Bind(wx.EVT_BUTTON, self.OnPlay, btn3)
        
        btn4 = wx.Button(self, -1, "Pause")
        self.Bind(wx.EVT_BUTTON, self.OnPause, btn4)
        
        btn5 = wx.Button(self, -1, "Stop")
        self.Bind(wx.EVT_BUTTON, self.OnStop, btn5)

        # setup the layout
        sizer = wx.GridBagSizer(5,5)
        sizer.Add(self.mc, (1,1), span=(5,1))#, flag=wx.EXPAND)
        sizer.Add(btn1, (1,3))
        sizer.Add(btn2, (2,3))
        sizer.Add(btn3, (3,3))
        sizer.Add(btn4, (4,3))
        sizer.Add(btn5, (5,3))
        self.SetSizer(sizer)
        

    def OnLoadFile(self, evt):
        dlg = wx.FileDialog(self, message="Choose a media file",
                            defaultDir=os.getcwd(), defaultFile="",
                            style=wx.OPEN | wx.CHANGE_DIR )
        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
            if not self.mc.Load(path):
                wx.MessageBox("Unable to load %s." % path,
                              "ERROR: Unsupported format?",
                              wx.ICON_ERROR | wx.OK)
            else:
                self.mc.SetBestFittingSize()
                self.GetSizer().Layout()
        dlg.Destroy()
        
    
    def OnLoadURI(self, evt):
        dlg = wx.TextEntryDialog(self, "URL:", "Please enter the URL of a media file",
                                 "http://www.mwscomp.com/movies/grail/tim-the-enchanter.avi")
        if dlg.ShowModal() == wx.ID_OK:
            url = dlg.GetValue()
            if not self.mc.LoadFromURI(url):
                wx.MessageBox("Unable to load %s." % url,
                              "ERROR", wx.ICON_ERROR | wx.OK)
            else:
                self.mc.SetBestFittingSize()
                self.GetSizer().Layout()
        dlg.Destroy()
            
    
    def OnPlay(self, evt):
        self.mc.Play()
    
    def OnPause(self, evt):
        self.mc.Pause()
    
    def OnStop(self, evt):
        self.mc.Stop()
    



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

wx.MediaCtrl is a class that allows a way to convieniently display types of 
media, such as videos, audio files, natively through native codecs.

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

