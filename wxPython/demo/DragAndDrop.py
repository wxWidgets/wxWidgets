
from wxPython.wx import *

#----------------------------------------------------------------------

class ClipTextPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log

        #self.SetFont(wxFont(10, wxSWISS, wxNORMAL, wxBOLD, false))

        sizer = wxBoxSizer(wxVERTICAL)
        sizer.Add(wxStaticText(self, -1,
                               "Copy/Paste text to/from\n"
                               "this window and other apps"), 0, wxEXPAND|wxALL, 2)

        self.text = wxTextCtrl(self, -1, "", style=wxTE_MULTILINE|wxHSCROLL)
        sizer.Add(self.text, 1, wxEXPAND)

        hsz = wxBoxSizer(wxHORIZONTAL)
        hsz.Add(wxButton(self, 6050, " Copy "), 1, wxEXPAND|wxALL, 2)
        hsz.Add(wxButton(self, 6051, " Paste "), 1, wxEXPAND|wxALL, 2)
        sizer.Add(hsz, 0, wxEXPAND)
        sizer.Add(wxButton(self, 6052, " Copy Bitmap "), 0, wxEXPAND|wxALL, 2)

        EVT_BUTTON(self, 6050, self.OnCopy)
        EVT_BUTTON(self, 6051, self.OnPaste)
        EVT_BUTTON(self, 6052, self.OnCopyBitmap)

        self.SetAutoLayout(true)
        self.SetSizer(sizer)


    def OnCopy(self, evt):
        self.do = wxTextDataObject()
        self.do.SetText(self.text.GetValue())
        wxTheClipboard.Open()
        wxTheClipboard.SetData(self.do)
        wxTheClipboard.Close()


    def OnPaste(self, evt):
        do = wxTextDataObject()
        wxTheClipboard.Open()
        success = wxTheClipboard.GetData(do)
        wxTheClipboard.Close()
        if success:
            self.text.SetValue(do.GetText())
        else:
            wxMessageBox("There is no data in the clipboard in the required format",
                         "Error")

    def OnCopyBitmap(self, evt):
        dlg = wxFileDialog(self, "Choose a bitmap to copy", wildcard="*.bmp")
        if dlg.ShowModal() == wxID_OK:
            bmp = wxBitmap(dlg.GetFilename(), wxBITMAP_TYPE_BMP)
            bmpdo = wxBitmapDataObject(bmp)
            wxTheClipboard.Open()
            wxTheClipboard.SetData(bmpdo)
            wxTheClipboard.Close()

            wxMessageBox("The bitmap is now in the Clipboard.  Switch to a graphics\n"
                         "editor and try pasting it in...")
        dlg.Destroy()

#----------------------------------------------------------------------

class OtherDropTarget(wxPyDropTarget):
    def __init__(self, window, log):
        wxPyDropTarget.__init__(self)
        self.log = log
        self.do = wxFileDataObject()
        self.SetDataObject(self.do)

    def OnEnter(self, x, y, d):
        self.log.WriteText("OnEnter: %d, %d, %d\n" % (x, y, d))
        return wxDragCopy

    #def OnDragOver(self, x, y, d):
    #    self.log.WriteText("OnDragOver: %d, %d, %d\n" % (x, y, d))
    #    return wxDragCopy

    def OnLeave(self):
        self.log.WriteText("OnLeave\n")

    def OnDrop(self, x, y):
        self.log.WriteText("OnDrop: %d %d\n" % (x, y))
        return true

    def OnData(self, x, y, d):
        self.log.WriteText("OnData: %d, %d, %d\n" % (x, y, d))
        self.GetData()
        self.log.WriteText("%s\n" % self.do.GetFilenames())
        return d




class MyFileDropTarget(wxFileDropTarget):
    def __init__(self, window, log):
        wxFileDropTarget.__init__(self)
        self.window = window
        self.log = log

    def OnDropFiles(self, x, y, filenames):
        self.window.SetInsertionPointEnd()
        self.window.WriteText("\n%d file(s) dropped at %d,%d:\n" %
                              (len(filenames), x, y))
        for file in filenames:
            self.window.WriteText(file + '\n')



class FileDropPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)

        #self.SetFont(wxFont(10, wxSWISS, wxNORMAL, wxBOLD, false))

        sizer = wxBoxSizer(wxVERTICAL)
        sizer.Add(wxStaticText(self, -1, " \nDrag some files here:"),
                  0, wxEXPAND|wxALL, 2)

        self.text = wxTextCtrl(self, -1, "",
                               style = wxTE_MULTILINE|wxHSCROLL|wxTE_READONLY)
        dt = MyFileDropTarget(self, log)
        self.text.SetDropTarget(dt)
        sizer.Add(self.text, 1, wxEXPAND)

        self.SetAutoLayout(true)
        self.SetSizer(sizer)


    def WriteText(self, text):
        self.text.WriteText(text)

    def SetInsertionPointEnd(self):
        self.text.SetInsertionPointEnd()


#----------------------------------------------------------------------
#----------------------------------------------------------------------

class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)

        self.SetAutoLayout(true)
        outsideSizer = wxBoxSizer(wxVERTICAL)

        msg = "Clipboard / Drag-And-Drop"
        text = wxStaticText(self, -1, "", style=wxALIGN_CENTRE)
        text.SetFont(wxFont(24, wxSWISS, wxNORMAL, wxBOLD, false))
        text.SetLabel(msg)
        w,h = text.GetTextExtent(msg)
        text.SetSize(wxSize(w,h+1))
        text.SetForegroundColour(wxBLUE)
        outsideSizer.Add(text, 0, wxEXPAND|wxALL, 5)
        outsideSizer.Add(wxStaticLine(self, -1), 0, wxEXPAND)

        inSizer = wxBoxSizer(wxHORIZONTAL)
        inSizer.Add(ClipTextPanel(self, log), 1, wxEXPAND)
        inSizer.Add(FileDropPanel(self, log), 1, wxEXPAND)

        outsideSizer.Add(inSizer, 1, wxEXPAND)
        self.SetSizer(outsideSizer)


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------












overview = """\
This demo shows some examples of data transfer through clipboard or drag and drop. In wxWindows, these two ways to transfer data (either between different applications or inside one and the same) are very similar which allows to implement both of them using almost the same code - or, in other words, if you implement drag and drop support for your application, you get clipboard support for free and vice versa.

At the heart of both clipboard and drag and drop operations lies the wxDataObject class. The objects of this class (or, to be precise, classes derived from it) represent the data which is being carried by the mouse during drag and drop operation or copied to or pasted from the clipboard. wxDataObject is a "smart" piece of data because it knows which formats it supports (see GetFormatCount and GetAllFormats) and knows how to render itself in any of them (see GetDataHere). It can also receive its value from the outside in a format it supports if it implements the SetData method. Please see the documentation of this class for more details.

Both clipboard and drag and drop operations have two sides: the source and target, the data provider and the data receiver. These which may be in the same application and even the same window when, for example, you drag some text from one position to another in a word processor. Let us describe what each of them should do.

"""
