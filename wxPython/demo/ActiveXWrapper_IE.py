"""
<html><body>
This demo shows how to embed an ActiveX control in a wxPython
application, (Win32 only.)

<p>
The MakeActiveXClass function dynamically builds a new Class on the
fly, that has the same signature and semantics as wxWindow.  This
means that when you call the function you get back a new class that
you can use just like wxWindow, (set the size and position, use in a
sizer, etc.) except its contents will be the COM control.

<p>
This demo embeds the Internet Explorer WebBrowser control, and shows
how to receive events from the COM control.  (The title bar and status
bar are updated as pages change, in addition to the log messages being
shown.)
</body></html>
"""

# 11/24/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for V2.5
# 

import  sys
import  wx

if wx.Platform == '__WXMSW__':
    import  wx.lib.activexwrapper   as  ax
    import  win32com.client.gencache

    try:
        browserModule = win32com.client.gencache.EnsureModule(
            "{EAB22AC0-30C1-11CF-A7EB-0000C05BAE0B}", 0, 1, 1
            )
    except:
        raise ImportError("IE4 or greater does not appear to be installed.")


#----------------------------------------------------------------------

class TestPanel(wx.Window):
    def __init__(self, parent, log, frame=None):
        wx.Window.__init__(
            self, parent, -1,
            style=wx.CLIP_CHILDREN|wx.NO_FULL_REPAINT_ON_RESIZE
            )

        self.ie = None
        self.log = log
        self.current = "http://wxPython.org/"
        self.frame = frame

        if frame:
            self.titleBase = frame.GetTitle()

        sizer = wx.BoxSizer(wx.VERTICAL)
        btnSizer = wx.BoxSizer(wx.HORIZONTAL)

        # Make a new class that derives from the WebBrowser class in the
        # COM module imported above.  This class also derives from wxWindow and
        # implements the machinery needed to integrate the two worlds.
        theClass = ax.MakeActiveXClass(
                        browserModule.WebBrowser, eventObj = self
                        )

        # Create an instance of that class
        self.ie = theClass(self, -1) ##, style=wxSUNKEN_BORDER)


        btn = wx.Button(self, wx.NewId(), "Open", style=wx.BU_EXACTFIT)
        self.Bind(wx.EVT_BUTTON, self.OnOpenButton, id=btn.GetId())
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        btn = wx.Button(self, wx.NewId(), "Home", style=wx.BU_EXACTFIT)
        self.Bind(wx.EVT_BUTTON, self.OnHomeButton, id=btn.GetId())
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        btn = wx.Button(self, wx.NewId(), "<--", style=wx.BU_EXACTFIT)
        self.Bind(wx.EVT_BUTTON, self.OnPrevPageButton, id=btn.GetId())
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        btn = wx.Button(self, wx.NewId(), "-->", style=wx.BU_EXACTFIT)
        self.Bind(wx.EVT_BUTTON, self.OnNextPageButton, id=btn.GetId())
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        btn = wx.Button(self, wx.NewId(), "Stop", style=wx.BU_EXACTFIT)
        self.Bind(wx.EVT_BUTTON, self.OnStopButton, id=btn.GetId())
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        btn = wx.Button(self, wx.NewId(), "Search", style=wx.BU_EXACTFIT)
        self.Bind(wx.EVT_BUTTON, self.OnSearchPageButton, id=btn.GetId())
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        btn = wx.Button(self, wx.NewId(), "Refresh", style=wx.BU_EXACTFIT)
        self.Bind(wx.EVT_BUTTON, self.OnRefreshPageButton, id=btn.GetId())
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        txt = wx.StaticText(self, -1, "Location:")
        btnSizer.Add(txt, 0, wx.CENTER|wx.ALL, 2)

        self.location = wx.ComboBox(self, wx.NewId(), "", style=wx.CB_DROPDOWN)
        self.Bind(wx.EVT_COMBOBOX, self.OnLocationSelect, id=self.location.GetId())
        self.Bind(wx.EVT_KEY_UP, self.OnLocationKey, self.location)
        self.Bind(wx.EVT_CHAR, self.IgnoreReturn, self.location)
        btnSizer.Add(self.location, 1, wx.EXPAND|wx.ALL, 2)

        sizer.Add(btnSizer, 0, wx.EXPAND)
        sizer.Add(self.ie, 1, wx.EXPAND)

        self.ie.Navigate(self.current)
        self.location.Append(self.current)

        self.SetSizer(sizer)
        self.SetAutoLayout(True)
        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_WINDOW_DESTROY, self.OnDestroy)


    def ShutdownDemo(self):
        # put the frame title back
        if self.frame:
            self.frame.SetTitle(self.titleBase)


    def OnDestroy(self, evt):
        if self.ie:
            self.ie.Cleanup()
            self.ie = None
            self.frame = None


    def OnSize(self, evt):
        self.Layout()


    def OnLocationSelect(self, evt):
        url = self.location.GetStringSelection()
        self.log.write('OnLocationSelect: %s\n' % url)
        self.ie.Navigate(url)

    def OnLocationKey(self, evt):
        if evt.KeyCode() == wx.WXK_RETURN:
            URL = self.location.GetValue()
            self.location.Append(URL)
            self.ie.Navigate(URL)
        else:
            evt.Skip()

    def IgnoreReturn(self, evt):
        print 'IgnoreReturn'
        if evt.KeyCode() != wx.WXK_RETURN:
            evt.Skip()

    def OnOpenButton(self, event):
        dlg = wx.TextEntryDialog(self, "Open Location",
                                "Enter a full URL or local path",
                                self.current, wx.OK|wx.CANCEL)

        dlg.CentreOnParent()

        if dlg.ShowModal() == wx.ID_OK:
            self.current = dlg.GetValue()
            self.ie.Navigate(self.current)

        dlg.Destroy()

    def OnHomeButton(self, event):
        self.ie.GoHome()    ## ET Phone Home!

    def OnPrevPageButton(self, event):
        self.ie.GoBack()

    def OnNextPageButton(self, event):
        self.ie.GoForward()

    def OnStopButton(self, evt):
        self.ie.Stop()

    def OnSearchPageButton(self, evt):
        self.ie.GoSearch()

    def OnRefreshPageButton(self, evt):
        self.ie.Refresh2(3)


    # The following event handlers are called by the web browser COM
    # control since  we passed self to MakeActiveXClass.  It will look
    # here for matching attributes and call them if they exist.  See the
    # module generated by makepy for details of method names, etc.
    def OnBeforeNavigate2(self, pDisp, URL, *args):
        self.log.write('OnBeforeNavigate2: %s\n' % URL)

    def OnNavigateComplete2(self, pDisp, URL):
        self.log.write('OnNavigateComplete2: %s\n' % URL)
        self.current = URL
        self.location.SetValue(URL)

    def OnTitleChange(self, text):
        self.log.write('OnTitleChange: %s\n' % text)
        if self.frame:
            self.frame.SetTitle(self.titleBase + ' -- ' + text)

    def OnStatusTextChange(self, text):
        self.log.write('OnStatusTextChange: %s\n' % text)
        if self.frame:
            self.frame.SetStatusText(text)


#----------------------------------------------------------------------
# for the demo framework...

def runTest(frame, nb, log):
    if wx.Platform == '__WXMSW__':
        win = TestPanel(nb, log, frame)
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
                self, None, -1, "ActiveX test -- Internet Explorer",
                size=(640, 480),
                style=wx.DEFAULT_FRAME_STYLE|wx.NO_FULL_REPAINT_ON_RESIZE
                )

            self.CreateStatusBar()
            self.tp = TestPanel(self, sys.stdout, self)
            self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)

        def OnCloseWindow(self, evt):
            self.tp.Destroy()
            self.Destroy()

    app = wx.PySimpleApp()
    frame = TestFrame()
    frame.Show(True)
    app.MainLoop()

