# 11/18/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 
# 11/28/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o iewin.py is missing
# 

import  wx

if wx.Platform == '__WXMSW__':
    import  wx.iewin    as  iewin

#----------------------------------------------------------------------

class TestPanel(wx.Window):
    def __init__(self, parent, log, frame=None):
        wx.Window.__init__(
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

        self.ie = iewin.IEHtmlWin(self, -1, style = wx.NO_FULL_REPAINT_ON_RESIZE)


        btn = wx.Button(self, wx.NewId(), "Open", style=wx.BU_EXACTFIT)
        wx.EVT_BUTTON(self, btn.GetId(), self.OnOpenButton)
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        btn = wx.Button(self, wx.NewId(), "Home", style=wx.BU_EXACTFIT)
        wx.EVT_BUTTON(self, btn.GetId(), self.OnHomeButton)
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        btn = wx.Button(self, wx.NewId(), "<--", style=wx.BU_EXACTFIT)
        wx.EVT_BUTTON(self, btn.GetId(), self.OnPrevPageButton)
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        btn = wx.Button(self, wx.NewId(), "-->", style=wx.BU_EXACTFIT)
        wx.EVT_BUTTON(self, btn.GetId(), self.OnNextPageButton)
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        btn = wx.Button(self, wx.NewId(), "Stop", style=wx.BU_EXACTFIT)
        wx.EVT_BUTTON(self, btn.GetId(), self.OnStopButton)
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        btn = wx.Button(self, wx.NewId(), "Search", style=wx.BU_EXACTFIT)
        wx.EVT_BUTTON(self, btn.GetId(), self.OnSearchPageButton)
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        btn = wx.Button(self, wx.NewId(), "Refresh", style=wx.BU_EXACTFIT)
        wx.EVT_BUTTON(self, btn.GetId(), self.OnRefreshPageButton)
        btnSizer.Add(btn, 0, wx.EXPAND|wx.ALL, 2)

        txt = wx.StaticText(self, -1, "Location:")
        btnSizer.Add(txt, 0, wx.CENTER|wx.ALL, 2)

        self.location = wx.ComboBox(
                            self, wx.NewId(), "", style=wx.CB_DROPDOWN|wx.PROCESS_ENTER
                            )
        
        wx.EVT_COMBOBOX(self, self.location.GetId(), self.OnLocationSelect)
        wx.EVT_KEY_UP(self.location, self.OnLocationKey)
        wx.EVT_CHAR(self.location, self.IgnoreReturn)
        btnSizer.Add(self.location, 1, wx.EXPAND|wx.ALL, 2)

        sizer.Add(btnSizer, 0, wx.EXPAND)
        sizer.Add(self.ie, 1, wx.EXPAND)

        self.ie.Navigate(self.current)
        self.location.Append(self.current)

        self.SetSizer(sizer)
        self.SetAutoLayout(True)
        wx.EVT_SIZE(self, self.OnSize)

        # Hook up the event handlers for the IE window
        iewin.EVT_MSHTML_BEFORENAVIGATE2(self, -1, self.OnBeforeNavigate2)
        iewin.EVT_MSHTML_NEWWINDOW2(self, -1, self.OnNewWindow2)
        iewin.EVT_MSHTML_DOCUMENTCOMPLETE(self, -1, self.OnDocumentComplete)
        #EVT_MSHTML_PROGRESSCHANGE(self, -1, self.OnProgressChange)
        iewin.EVT_MSHTML_STATUSTEXTCHANGE(self, -1, self.OnStatusTextChange)
        iewin.EVT_MSHTML_TITLECHANGE(self, -1, self.OnTitleChange)


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
        if evt.KeyCode() == wx.WXK_RETURN:
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
        self.ie.Refresh(iewin.IEHTML_REFRESH_COMPLETELY)


    def logEvt(self, name, event):
        self.log.write('%s: %s\n' %
                       (name, (event.GetLong1(), event.GetLong2(), event.GetText1())))

    def OnBeforeNavigate2(self, evt):
        self.logEvt('OnBeforeNavigate2', evt)

    def OnNewWindow2(self, evt):
        self.logEvt('OnNewWindow2', evt)
        evt.Veto() # don't allow it

    def OnDocumentComplete(self, evt):
        self.logEvt('OnDocumentComplete', evt)
        self.current = evt.GetText1()
        self.location.SetValue(self.current)

    def OnTitleChange(self, evt):
        self.logEvt('OnTitleChange', evt)
        if self.frame:
            self.frame.SetTitle(self.titleBase + ' -- ' + evt.GetText1())

    def OnStatusTextChange(self, evt):
        self.logEvt('OnStatusTextChange', evt)
        if self.frame:
            self.frame.SetStatusText(evt.GetText1())


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



overview = """\
<html><body>
<h2>wxIEHtmlWin</h2>

The wxIEHtmlWin class is the first example of using a contributed
wxActiveX class in wxWindows C++.  It is still experimental, but
I think it is useful.

<p> Using this class is simpler than ActiveXWrapper, doesn't rely on
the win32all extensions, and is more "wx\'ish", meaning that it uses
events and etc. as would be expected from any other wx window.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])


#----------------------------------------------------------------------

