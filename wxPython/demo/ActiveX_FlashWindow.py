import  os
import  wx

if wx.Platform == '__WXMSW__':
    from wx.lib.flashwin import FlashWindow

from Main import opj

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)
        self.pdf = None

        sizer = wx.BoxSizer(wx.VERTICAL)
        btnSizer = wx.BoxSizer(wx.HORIZONTAL)

        self.flash = FlashWindow(self, style=wx.SUNKEN_BORDER)
        self.flash.LoadMovie(0, 'file://' + os.path.abspath('data/Asteroid_blaster.swf'))

        sizer.Add(self.flash, proportion=1, flag=wx.EXPAND)

        btn = wx.Button(self, wx.NewId(), "Open Flash File")
        self.Bind(wx.EVT_BUTTON, self.OnOpenFileButton, btn)
        btnSizer.Add(btn, proportion=1, flag=wx.EXPAND|wx.ALL, border=5)

        btn = wx.Button(self, wx.NewId(), "Open Flash URL")
        self.Bind(wx.EVT_BUTTON, self.OnOpenURLButton, btn)
        btnSizer.Add(btn, proportion=1, flag=wx.EXPAND|wx.ALL, border=5)

        btnSizer.Add((50,-1), proportion=2, flag=wx.EXPAND)
        sizer.Add(btnSizer, proportion=0, flag=wx.EXPAND)

        self.SetSizer(sizer)
        self.SetAutoLayout(True)



    def OnOpenFileButton(self, event):
        dlg = wx.FileDialog(self, wildcard="*.swf")

        if dlg.ShowModal() == wx.ID_OK:
            wx.BeginBusyCursor()
            self.flash.LoadMovie(0, 'file://' + dlg.GetPath())
            wx.EndBusyCursor()

        dlg.Destroy()


    def OnOpenURLButton(self, event):
        dlg = wx.TextEntryDialog(self, "Enter a URL of a .swf file", "Enter URL")

        if dlg.ShowModal() == wx.ID_OK:
            wx.BeginBusyCursor()
            # setting the movie property works too
            self.flash.movie = dlg.GetValue() 
            wx.EndBusyCursor()

        dlg.Destroy()



#----------------------------------------------------------------------

def runTest(frame, nb, log):
    if wx.Platform == '__WXMSW__':
        win = TestPanel(nb, log)
        return win
    else:
        from Main import MessagePanel
        win = MessagePanel(nb, 'This demo only works on Microsoft Windows.',
                           'Sorry', wx.ICON_WARNING)
        return win


overview = """\
<html><body>
<h2>wx.lib.flashwin.FlashWindow</h2>

The wx.lib.pdfwin.FlashWindow class is yet another example of using
ActiveX controls from wxPython using the new wx.activex module.  This
allows you to use an ActiveX control as if it is a wx.Window, you can
call its methods, set/get properties, and receive events from the
ActiveX control in a very intuitive way.

<p> Using this class is simpler than ActiveXWrapper, doesn't rely on
the win32all extensions, and is more "wx\'ish", meaning that it uses
events and etc. as would be expected from any other wx window.

<p> This demo embeds the Shockwave Flash control, and lets you play a game.

</body></html>
"""

#----------------------------------------------------------------------



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])


