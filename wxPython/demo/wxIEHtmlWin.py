
from wxPython.wx import *

if wxPlatform == '__WXMSW__':
    from wxPython.iewin import *

#----------------------------------------------------------------------

class TestPanel(wxWindow):
    def __init__(self, parent, log, frame=None):
        wxWindow.__init__(self, parent, -1,
                          style=wxTAB_TRAVERSAL|wxCLIP_CHILDREN|wxNO_FULL_REPAINT_ON_RESIZE)
        self.log = log
        self.current = "http://wxWindows.org/"
        self.frame = frame
        if frame:
            self.titleBase = frame.GetTitle()


        sizer = wxBoxSizer(wxVERTICAL)
        btnSizer = wxBoxSizer(wxHORIZONTAL)

        self.ie = wxIEHtmlWin(self, -1, style = wxNO_FULL_REPAINT_ON_RESIZE)


        btn = wxButton(self, wxNewId(), "Open", style=wxBU_EXACTFIT)
        EVT_BUTTON(self, btn.GetId(), self.OnOpenButton)
        btnSizer.Add(btn, 0, wxEXPAND|wxALL, 2)

        btn = wxButton(self, wxNewId(), "Home", style=wxBU_EXACTFIT)
        EVT_BUTTON(self, btn.GetId(), self.OnHomeButton)
        btnSizer.Add(btn, 0, wxEXPAND|wxALL, 2)

        btn = wxButton(self, wxNewId(), "<--", style=wxBU_EXACTFIT)
        EVT_BUTTON(self, btn.GetId(), self.OnPrevPageButton)
        btnSizer.Add(btn, 0, wxEXPAND|wxALL, 2)

        btn = wxButton(self, wxNewId(), "-->", style=wxBU_EXACTFIT)
        EVT_BUTTON(self, btn.GetId(), self.OnNextPageButton)
        btnSizer.Add(btn, 0, wxEXPAND|wxALL, 2)

        btn = wxButton(self, wxNewId(), "Stop", style=wxBU_EXACTFIT)
        EVT_BUTTON(self, btn.GetId(), self.OnStopButton)
        btnSizer.Add(btn, 0, wxEXPAND|wxALL, 2)

        btn = wxButton(self, wxNewId(), "Search", style=wxBU_EXACTFIT)
        EVT_BUTTON(self, btn.GetId(), self.OnSearchPageButton)
        btnSizer.Add(btn, 0, wxEXPAND|wxALL, 2)

        btn = wxButton(self, wxNewId(), "Refresh", style=wxBU_EXACTFIT)
        EVT_BUTTON(self, btn.GetId(), self.OnRefreshPageButton)
        btnSizer.Add(btn, 0, wxEXPAND|wxALL, 2)

        txt = wxStaticText(self, -1, "Location:")
        btnSizer.Add(txt, 0, wxCENTER|wxALL, 2)

        self.location = wxComboBox(self, wxNewId(), "", style=wxCB_DROPDOWN|wxPROCESS_ENTER)
        EVT_COMBOBOX(self, self.location.GetId(), self.OnLocationSelect)
        EVT_KEY_UP(self.location, self.OnLocationKey)
        EVT_CHAR(self.location, self.IgnoreReturn)
        btnSizer.Add(self.location, 1, wxEXPAND|wxALL, 2)


        sizer.Add(btnSizer, 0, wxEXPAND)
        sizer.Add(self.ie, 1, wxEXPAND)

        self.ie.Navigate(self.current)
        self.location.Append(self.current)

        self.SetSizer(sizer)
        self.SetAutoLayout(True)
        EVT_SIZE(self, self.OnSize)

        # Hook up the event handlers for the IE window
        EVT_MSHTML_BEFORENAVIGATE2(self, -1, self.OnBeforeNavigate2)
        EVT_MSHTML_NEWWINDOW2(self, -1, self.OnNewWindow2)
        EVT_MSHTML_DOCUMENTCOMPLETE(self, -1, self.OnDocumentComplete)
        #EVT_MSHTML_PROGRESSCHANGE(self, -1, self.OnProgressChange)
        EVT_MSHTML_STATUSTEXTCHANGE(self, -1, self.OnStatusTextChange)
        EVT_MSHTML_TITLECHANGE(self, -1, self.OnTitleChange)


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
        if evt.KeyCode() == WXK_RETURN:
            URL = self.location.GetValue()
            self.location.Append(URL)
            self.ie.Navigate(URL)
        else:
            evt.Skip()


    def IgnoreReturn(self, evt):
        if evt.GetKeyCode() != WXK_RETURN:
            evt.Skip()

    def OnOpenButton(self, event):
        dlg = wxTextEntryDialog(self, "Open Location",
                                "Enter a full URL or local path",
                                self.current, wxOK|wxCANCEL)
        dlg.CentreOnParent()
        if dlg.ShowModal() == wxID_OK:
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
        self.ie.Refresh(wxIEHTML_REFRESH_COMPLETELY)



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
    if wxPlatform == '__WXMSW__':
        win = TestPanel(nb, log, frame)
        return win
    else:
        dlg = wxMessageDialog(frame, 'This demo only works on MSW.',
                          'Sorry', wxOK | wxICON_INFORMATION)
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

