"""
<html><body>
This demo shows how to embed an ActiveX control in a wxPython application, (Win32 only.)
<p>
The MakeActiveXClass function dynamically builds a new Class on the fly, that has the
same signature and semantics as wxWindow.  This means that when you call the function
you get back a new class that you can use just like wxWindow, (set the size and position,
use in a sizer, etc.) except its contents will be the COM control.
<p>
This demo embeds the Adobe Acrobat Reader, and gives you some buttons for opening a PDF
file, changing pages, etc. that show how to call methods on the COM object.  If you don't
have Acrobat Reader 4.0 installed it won't work.
</body></html>
"""

# 11/24/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for 2.5
#

import  sys
import  wx

if wx.Platform == '__WXMSW__':
    import  wx.lib.activexwrapper   as  ax
    import  win32com.client.gencache

    try:
        acrobat = win32com.client.gencache.EnsureModule(
                    '{CA8A9783-280D-11CF-A24D-444553540000}', 0x0, 1, 3
                    )
    except:
        raise ImportError("Can't load PDF.OCX, install Acrobat 4.0")


#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1)
        self.pdf = None

        sizer = wx.BoxSizer(wx.VERTICAL)
        btnSizer = wx.BoxSizer(wx.HORIZONTAL)

        # this function creates a new class that can be used as
        # a wx.Window, but contains the given ActiveX control.
        ActiveXWrapper = ax.MakeActiveXClass(acrobat.Pdf)

        # create an instance of the new class
        self.pdf = ActiveXWrapper( self, -1, style=wx.SUNKEN_BORDER)

        sizer.Add(self.pdf, proportion=1, flag=wx.EXPAND)

        btn = wx.Button(self, wx.NewId(), "Open PDF File")
        self.Bind(wx.EVT_BUTTON, self.OnOpenButton)
        btnSizer.Add(btn, proportion=1, flag=wx.EXPAND|wx.ALL, border=5)

        btn = wx.Button(self, wx.NewId(), "<-- Previous Page")
        self.Bind(wx.EVT_BUTTON, self.OnPrevPageButton, id=btn.GetId())
        btnSizer.Add(btn, proportion=1, flag=wx.EXPAND|wx.ALL, border=5)

        btn = wx.Button(self, wx.NewId(), "Next Page -->")
        self.Bind(wx.EVT_BUTTON, self.OnNextPageButton, id=btn.GetId())
        btnSizer.Add(btn, proportion=1, flag=wx.EXPAND|wx.ALL, border=5)


        btnSizer.Add((50,-1), proportion=2, flag=wx.EXPAND)
        sizer.Add(btnSizer, proportion=0, flag=wx.EXPAND)

        self.SetSizer(sizer)
        self.SetAutoLayout(True)

        self.Bind(wx.EVT_WINDOW_DESTROY, self.OnDestroy)


    def OnDestroy(self, evt):
        if self.pdf:
            self.pdf.Cleanup()
            self.pdf = None



    def OnOpenButton(self, event):
        dlg = wx.FileDialog(self, wildcard="*.pdf")

        if dlg.ShowModal() == wx.ID_OK:
            wx.BeginBusyCursor()
            self.pdf.LoadFile(dlg.GetPath())
            wx.EndBusyCursor()

        dlg.Destroy()


    def OnPrevPageButton(self, event):
        self.pdf.gotoPreviousPage()


    def OnNextPageButton(self, event):
        self.pdf.gotoNextPage()



#----------------------------------------------------------------------

def runTest(frame, nb, log):
    if wx.Platform == '__WXMSW__':
        win = TestPanel(nb, log)
        return win
    else:
        dlg = wx.MessageDialog(frame, 'This demo only works on MSW.',
                          'Sorry', wx.OK | wx.ICON_INFORMATION)
        dlg.ShowModal()
        dlg.Destroy()


overview = __doc__

#----------------------------------------------------------------------


if __name__ == '__main__':
    class TestFrame(wx.Frame):
        def __init__(self):
            wx.Frame.__init__(
                self, None, -1, "ActiveX test -- Acrobat", size=(640, 480),
                style=wx.DEFAULT_FRAME_STYLE|wx.NO_FULL_REPAINT_ON_RESIZE
                )

            self.tp = TestPanel(self, sys.stdout)


    app = wx.PySimpleApp()
    frame = TestFrame()
    frame.Show(True)
    app.MainLoop()


