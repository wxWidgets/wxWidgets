# 11/18/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 

import  wx

if wx.Platform == '__WXMSW__':
    import  wx.lib.iewin    as  iewin

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log, frame=None):
        wx.Panel.__init__(
            self, parent, -1,
            style=wx.TAB_TRAVERSAL|wx.CLIP_CHILDREN|wx.NO_FULL_REPAINT_ON_RESIZE
            )
            
        self.log = log
        self.current = "http://wxPython.org/"
        self.frame = frame

        if frame:
            self.titleBase = frame.GetTitle()

        sizer = wx.BoxSizer(wx.VERTICAL)
        btnSizer = wx.BoxSizer(wx.HORIZONTAL)

        self.ie = iewin.IEHtmlWindow(self, -1, style = wx.NO_FULL_REPAINT_ON_RESIZE)


        btn = wx.Button(self, -1, "Open", style=wx.BU_EXACTFIT)
        self.Bind(wx.EVT_BUTTON, self.OnOpenButton, btn)
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        btn = wx.Button(self, -1, "Home", style=wx.BU_EXACTFIT)
        self.Bind(wx.EVT_BUTTON, self.OnHomeButton, btn)
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        btn = wx.Button(self, -1, "<--", style=wx.BU_EXACTFIT)
        self.Bind(wx.EVT_BUTTON, self.OnPrevPageButton, btn)
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        btn = wx.Button(self, -1, "-->", style=wx.BU_EXACTFIT)
        self.Bind(wx.EVT_BUTTON, self.OnNextPageButton, btn)
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        btn = wx.Button(self, -1, "Stop", style=wx.BU_EXACTFIT)
        self.Bind(wx.EVT_BUTTON, self.OnStopButton, btn)
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        btn = wx.Button(self, -1, "Search", style=wx.BU_EXACTFIT)
        self.Bind(wx.EVT_BUTTON, self.OnSearchPageButton, btn)
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        btn = wx.Button(self, -1, "Refresh", style=wx.BU_EXACTFIT)
        self.Bind(wx.EVT_BUTTON, self.OnRefreshPageButton, btn)
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        txt = wx.StaticText(self, -1, "Location:")
        btnSizer.Add(txt, 0, wx.CENTER|wx.ALL, 2)

        self.location = wx.ComboBox(
                            self, -1, "", style=wx.CB_DROPDOWN|wx.PROCESS_ENTER
                            )
        
        self.Bind(wx.EVT_COMBOBOX, self.OnLocationSelect, self.location)
        self.location.Bind(wx.EVT_KEY_UP, self.OnLocationKey)
        self.location.Bind(wx.EVT_CHAR, self.IgnoreReturn)
        btnSizer.Add(self.location, 1, wx.EXPAND|wx.ALL, 2)

        sizer.Add(btnSizer, 0, wx.EXPAND)
        sizer.Add(self.ie, 1, wx.EXPAND)

        self.ie.LoadUrl(self.current)
        self.location.Append(self.current)

        self.SetSizer(sizer)
        # Since this is a wxWindow we have to call Layout ourselves
        self.Bind(wx.EVT_SIZE, self.OnSize)

        # Hook up the event handlers for the IE window
        self.Bind(iewin.EVT_BeforeNavigate2, self.OnBeforeNavigate2, self.ie)
        self.Bind(iewin.EVT_NewWindow2, self.OnNewWindow2, self.ie)
        self.Bind(iewin.EVT_DocumentComplete, self.OnDocumentComplete, self.ie)
        ##self.Bind(iewin.EVT_ProgressChange,  self.OnProgressChange, self.ie)
        self.Bind(iewin.EVT_StatusTextChange, self.OnStatusTextChange, self.ie)
        self.Bind(iewin.EVT_TitleChange, self.OnTitleChange, self.ie)


    def ShutdownDemo(self):
        # put the frame title back
        if self.frame:
            self.frame.SetTitle(self.titleBase)


    def OnSize(self, evt):
        self.Layout()


    def OnLocationSelect(self, evt):
        url = self.location.GetStringSelection()
        self.log.write('OnLocationSelect: %s\n' % url)
        self.ie.Navigate(url)

    def OnLocationKey(self, evt):
        if evt.GetKeyCode() == wx.WXK_RETURN:
            URL = self.location.GetValue()
            self.location.Append(URL)
            self.ie.Navigate(URL)
        else:
            evt.Skip()


    def IgnoreReturn(self, evt):
        if evt.GetKeyCode() != wx.WXK_RETURN:
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
        self.ie.Refresh(iewin.REFRESH_COMPLETELY)


    def logEvt(self, evt):
        pst = ""
        for name in evt.paramList:
            pst += " %s:%s " % (name, repr(getattr(evt, name)))
        self.log.write('%s: %s\n' % (evt.eventName, pst))


    def OnBeforeNavigate2(self, evt):
        self.logEvt(evt)

    def OnNewWindow2(self, evt):
        self.logEvt(evt)
        # Veto the new window.  Cancel is defined as an "out" param
        # for this event.  See iewin.py
        evt.Cancel = True   

    def OnProgressChange(self, evt):
        self.logEvt(evt)
        
    def OnDocumentComplete(self, evt):
        self.logEvt(evt)
        self.current = evt.URL
        self.location.SetValue(self.current)

    def OnTitleChange(self, evt):
        self.logEvt(evt)
        if self.frame:
            self.frame.SetTitle(self.titleBase + ' -- ' + evt.Text)

    def OnStatusTextChange(self, evt):
        self.logEvt(evt)
        if self.frame:
            self.frame.SetStatusText(evt.Text)


#----------------------------------------------------------------------
# for the demo framework...

def runTest(frame, nb, log):
    if wx.Platform == '__WXMSW__':
        win = TestPanel(nb, log, frame)
        return win
    else:
        from Main import MessagePanel
        win = MessagePanel(nb, 'This demo only works on Microsoft Windows.',
                           'Sorry', wx.ICON_WARNING)
        return win



overview = """\
<html><body>
<h2>wx.lib.iewin.IEHtmlWindow</h2>

The wx.lib.iewin.IEHtmlWindow class is one example of using ActiveX
controls from wxPython using the new wx.activex module.  This allows
you to use an ActiveX control as if it is a wx.Window, you can call
its methods, set/get properties, and receive events from the ActiveX
control in a very intuitive way.

<p> Using this class is simpler than ActiveXWrapper, doesn't rely on
the win32all extensions, and is more "wx\'ish", meaning that it uses
events and etc. as would be expected from any other wx window.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])


#----------------------------------------------------------------------

