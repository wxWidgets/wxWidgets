"""
ActiveXControl sample usage.

Based on original code by Willy Heineman, wheineman@uconect.net, 14 February 2000

ActiveXControl provides a simplified wrapper for creating activeX
controls within wxPython GUIs.
"""

from wxPython.wx import *
from wxPython.lib.activexwrapper import ActiveXWrapper

import pythoncom
import pywin.mfc.activex
import win32com.client.gencache
import win32con
import win32ui

browserModule = win32com.client.gencache.EnsureModule("{EAB22AC0-30C1-11CF-A7EB-0000C05BAE0B}", 0, 1, 1)
if browserModule is None:
    win32ui.MessageBox("IE4 or greater does not appear to be installed.",
                       'ActiveX Demo',
                       win32con.MB_OK | win32con.MB_ICONSTOP)
    raise ImportError


#----------------------------------------------------------------------
# NOTE:
# Your ActiveX control _must_ be an instance of pywin.mfc.activex.Control,
# as we use the CreateControl method to bind the GUI systems.

class WebBrowserControl(pywin.mfc.activex.Control,
                        browserModule.WebBrowser):

    def __init__(self):
        pywin.mfc.activex.Control.__init__(self)
        browserModule.WebBrowser.__init__(self)



#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log
        self.current = "http://alldunn.com/"

        sizer = wxBoxSizer(wxVERTICAL)
        btnSizer = wxBoxSizer(wxHORIZONTAL)

        # Build the wxPython wrapper window and put the Acrobat
        # control in it
        self.axw = ActiveXWrapper( self, -1, style=wxSUNKEN_BORDER, eraseBackground=0)
        self.axw.SetControl( WebBrowserControl() )
        sizer.Add(self.axw, 1, wxEXPAND)

        self.axw.control.Navigate(self.current)

        btn = wxButton(self, wxNewId(), "Open Location")
        EVT_BUTTON(self, btn.GetId(), self.OnOpenButton)
        btnSizer.Add(btn, 1, wxEXPAND|wxALL, 5)

        btn = wxButton(self, wxNewId(), "<-- Back Page")
        EVT_BUTTON(self, btn.GetId(), self.OnPrevPageButton)
        btnSizer.Add(btn, 1, wxEXPAND|wxALL, 5)

        btn = wxButton(self, wxNewId(), "Forward Page -->")
        EVT_BUTTON(self, btn.GetId(), self.OnNextPageButton)
        btnSizer.Add(btn, 1, wxEXPAND|wxALL, 5)

        btnSizer.Add(50, -1, 2, wxEXPAND)
        sizer.Add(btnSizer, 0, wxEXPAND)

        self.SetSizer(sizer)
        self.SetAutoLayout(true)


    def __del__(self):
        self.axw.Cleanup()
        self.axw = None


    def OnOpenButton(self, event):
        dlg = wxTextEntryDialog(self, "Open Location",
                                "Enter a full URL or local path",
                                self.current, wxOK|wxCANCEL)
        dlg.CentreOnParent()
        if dlg.ShowModal() == wxID_OK:
            self.current = dlg.GetValue()
            self.axw.control.Navigate(self.current)
        dlg.Destroy()


    def OnPrevPageButton(self, event):
        self.axw.control.GoBack()


    def OnNextPageButton(self, event):
        self.axw.control.GoForward()


#----------------------------------------------------------------------
# for the demo framework...

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win


overview = __doc__

#----------------------------------------------------------------------


if __name__ == '__main__':
    class TestFrame(wxFrame):
        def __init__(self):
            wxFrame.__init__(self, None, -1, "ActiveX test -- Internet Explorer", size=(640, 480))
            self.tp = TestPanel(self, sys.stdout)

        def OnCloseWindow(self, event):
            self.tp.axw.Cleanup()
            self.Destroy()


    app = wxPySimpleApp()
    frame = TestFrame()
    frame.Show(true)
    app.MainLoop()



