"""
ActiveXControl sample usage.

Based on original code by Willy Heineman, wheineman@uconect.net, 14 February 2000

ActiveXControl provides a simplified wrapper for creating activeX
controls within wxPython GUIs.  It allows you to include the activeX
control in a sizer, and includes a flag to prevent redrawing the
background behind the control (reduces the annoying flicker on
Win32 systems).
"""
from wxPython.wx import *
from wxPython.lib.activexwrapper import ActiveXWrapper

import pythoncom
import pywin.mfc.activex
import win32com.client.gencache
import win32con
import win32ui

acrobatOCX = win32com.client.gencache.EnsureModule('{CA8A9783-280D-11CF-A24D-444553540000}', 0x0, 1, 3)
if acrobatOCX is None:
    win32ui.MessageBox("Can't load PDF.OCX, install Acrobat 4.0",
                       'ActiveX Demo',
                       win32con.MB_OK | win32con.MB_ICONSTOP)
    raise ImportError


#----------------------------------------------------------------------
# NOTE:
# Your ActiveX control _must_ be an instance of pywin.mfc.activex.Control,
# as we use the CreateControl method to bind the GUI systems.

class PDFControl(pywin.mfc.activex.Control, acrobatOCX.Pdf):
    def __init__(self):
        pywin.mfc.activex.Control.__init__(self)
        acrobatOCX.Pdf.__init__(self)


#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log

        sizer = wxBoxSizer(wxVERTICAL)
        btnSizer = wxBoxSizer(wxHORIZONTAL)

        # Build the wxPython wrapper window and put the Acrobat
        # control in it
        self.axw = ActiveXWrapper( self, -1, style=wxSUNKEN_BORDER, eraseBackground=0)
        self.axw.SetControl( PDFControl() )

        sizer.Add(self.axw, 1, wxEXPAND)

        btn = wxButton(self, wxNewId(), "Open PDF File")
        EVT_BUTTON(self, btn.GetId(), self.OnOpenButton)
        btnSizer.Add(btn, 1, wxEXPAND|wxALL, 5)

        btn = wxButton(self, wxNewId(), "<-- Previous Page")
        EVT_BUTTON(self, btn.GetId(), self.OnPrevPageButton)
        btnSizer.Add(btn, 1, wxEXPAND|wxALL, 5)

        btn = wxButton(self, wxNewId(), "Next Page -->")
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
        dlg = wxFileDialog(self, wildcard="*.pdf")
        if dlg.ShowModal() == wxID_OK:
            self.axw.control.LoadFile(dlg.GetPath())

        dlg.Destroy()


    def OnPrevPageButton(self, event):
        self.axw.control.gotoPreviousPage()


    def OnNextPageButton(self, event):
        self.axw.control.gotoNextPage()



#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win


overview = __doc__

#----------------------------------------------------------------------


if __name__ == '__main__':
    class TestFrame(wxFrame):
        def __init__(self):
            wxFrame.__init__(self, None, -1, "ActiveX test -- Acrobat", size=(640, 480))
            self.tp = TestPanel(self, sys.stdout)

        def OnCloseWindow(self, event):
            self.tp.axw.Cleanup()
            self.Destroy()


    app = wxPySimpleApp()
    frame = TestFrame()
    frame.Show(true)
    app.MainLoop()



