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

from wxPython.wx import *

if wxPlatform == '__WXMSW__':
    from wxPython.lib.activexwrapper import MakeActiveXClass
    import win32com.client.gencache

    try:
        acrobat = win32com.client.gencache.EnsureModule('{CA8A9783-280D-11CF-A24D-444553540000}', 0x0, 1, 3)
    except:
        raise ImportError("Can't load PDF.OCX, install Acrobat 4.0")



#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log

        sizer = wxBoxSizer(wxVERTICAL)
        btnSizer = wxBoxSizer(wxHORIZONTAL)

        # this function creates a new class that can be used as
        # a wxWindow, but contains the given ActiveX control.
        ActiveXWrapper = MakeActiveXClass(acrobat.Pdf)

        # create an instance of the new class
        self.pdf = ActiveXWrapper( self, -1, style=wxSUNKEN_BORDER)

        sizer.Add(self.pdf, 1, wxEXPAND)

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
        self.pdf.Cleanup()
        self.pdf = None


    def OnOpenButton(self, event):
        dlg = wxFileDialog(self, wildcard="*.pdf")
        if dlg.ShowModal() == wxID_OK:
            wxBeginBusyCursor()
            self.pdf.LoadFile(dlg.GetPath())
            wxEndBusyCursor()

        dlg.Destroy()


    def OnPrevPageButton(self, event):
        self.pdf.gotoPreviousPage()


    def OnNextPageButton(self, event):
        self.pdf.gotoNextPage()



#----------------------------------------------------------------------

def runTest(frame, nb, log):
    if wxPlatform == '__WXMSW__':
        win = TestPanel(nb, log)
        return win
    else:
        dlg = wxMessageDialog(frame, 'This demo only works on MSW.',
                          'Sorry', wxOK | wxICON_INFORMATION)
        dlg.ShowModal()
        dlg.Destroy()


overview = __doc__

#----------------------------------------------------------------------


if __name__ == '__main__':
    class TestFrame(wxFrame):
        def __init__(self):
            wxFrame.__init__(self, None, -1, "ActiveX test -- Acrobat", size=(640, 480),
                             style=wxDEFAULT_FRAME_STYLE|wxNO_FULL_REPAINT_ON_RESIZE)
            self.tp = TestPanel(self, sys.stdout)

        def OnCloseWindow(self, event):
            self.tp.pdf.Cleanup()
            self.Destroy()


    app = wxPySimpleApp()
    frame = TestFrame()
    frame.Show(true)
    app.MainLoop()



