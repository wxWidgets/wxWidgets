from wxPython.wx import *
from layoutf import Layoutf
import string



class wxScrolledMessageDialog(wxDialog):

    def __init__(self, parent, msg, caption, pos = None, size = None):
        if not pos:
            pos = wxDefaultPosition
        if not size:
            size = wxSize(500,300)
        wxDialog.__init__(self, parent, -1, caption, pos, size)
        text = wxTextCtrl(self, -1, msg, wxDefaultPosition,
                             wxDefaultSize,
                             wxTE_MULTILINE | wxTE_READONLY)
        ok = wxButton(self, wxID_OK, "OK")
        text.SetConstraints(Layoutf('t=t5#1;b=t5#2;l=l5#1;r=r5#1', (self,ok)))
        ok.SetConstraints(Layoutf('b=b5#1;x%w50#1;w!80;h!25', (self,)))
        self.SetAutoLayout(TRUE)
        self.Layout()


class wxMultipleChoiceDialog(wxDialog):

    def __init__(self, parent, msg, title, lst, pos = None, size = None):
        if not pos:
            pos = wxDefaultPosition
        if not size:
            size = wxSize(200,200)
        wxDialog.__init__(self, parent, -1, title, pos, size)
        dc = wxClientDC(self)
        height = 0
        for line in string.split(msg,'\n'):
            height = height + dc.GetTextExtent(msg)[1] + 4
        stat = wxStaticText(self, -1, msg)
        self.lbox = wxListBox(self, 100, wxDefaultPosition,
                                 wxDefaultSize, lst, wxLB_MULTIPLE)
        ok = wxButton(self, wxID_OK, "OK")
        cancel = wxButton(self, wxID_CANCEL, "Cancel")
        stat.SetConstraints(Layoutf('t=t10#1;l=l5#1;r=r5#1;h!%d' % (height,),
                                   (self,)))
        self.lbox.SetConstraints(Layoutf('t=b10#2;l=l5#1;r=r5#1;b=t5#3',
                                 (self, stat, ok)))
        ok.SetConstraints(Layoutf('b=b5#1;x%w25#1;w!80;h!25', (self,)))
        cancel.SetConstraints(Layoutf('b=b5#1;x%w75#1;w!80;h!25', (self,)))
        self.SetAutoLayout(TRUE)
        self.lst = lst
        self.Layout()

    def OnSize(self, event):
        self.Layout()

    def GetValue(self):
        return self.lbox.GetSelections()

    def GetValueString(self):
        sel = self.lbox.GetSelections()
        val = []
        for i in sel:
            val.append(self.lst[i])
        return tuple(val)

if __name__ == '__main__':
    class MyFrame(wxFrame):
        def __init__(self):
            wxFrame.__init__(self, NULL, -1, "hello",
                                wxDefaultPosition, wxSize(200,200))
            wxButton(self, 100, "Multiple Test",wxPoint(0,0))
            wxButton(self, 101, "Message Test", wxPoint(0,100))
            EVT_BUTTON(self, 100, self.OnMultipleTest)
            EVT_BUTTON(self, 101, self.OnMessageTest)

        def OnMultipleTest(self, event):
            self.lst = [ 'apple', 'pear', 'banana', 'coconut', 'orange',
                         'etc', 'etc..', 'etc...' ]
            dlg = wxMultipleChoiceDialog(self,
                                         "Pick some from\n this list\nblabla",
                                         "m.s.d.", self.lst)
            if (dlg.ShowModal() == wxID_OK):
                print "Selection:", dlg.GetValue(), " -> ", dlg.GetValueString()

        def OnMessageTest(self, event):
            import sys;
            f = open(sys.argv[0],"r")
            msg = f.read()
            dlg = wxScrolledMessageDialog(self, msg, "message test")
            dlg.ShowModal()


    class MyApp(wxApp):
        def OnInit(self):
            frame = MyFrame()
            frame.Show(TRUE)
            self.SetTopWindow(frame)
            return TRUE

    app = MyApp(0)
    app.MainLoop()







