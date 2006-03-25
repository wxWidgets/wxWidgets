from wxPython.wx import *
import string


class floatValidator(wxPyValidator):

    def __init__(self, obj=None, attrName=""):
        wxPyValidator.__init__(self)
        self.numList = ['1','2','3','4','5','6','7','8','9','0','.']
        EVT_CHAR(self, self.OnChar)
        self.obj = obj
        self.attrName = attrName

    def Clone(self):
        return floatValidator(self.obj, self.attrName)

    def TransferToWindow(self):
        if self.obj and hasattr(self.obj, self.attrName):
            tc = wxPyTypeCast(self.GetWindow(), "wxTextCtrl")
            tc.SetValue(str(getattr(self.obj, self.attrName)))
        return true

    def TransferFromWindow(self):
        if self.obj and self.attrName:
            tc = wxPyTypeCast(self.GetWindow(), "wxTextCtrl")
            text = tc.GetValue()
            setattr(self.obj, self.attrName, string.atof(text))
        return true


    def Validate(self, win):
        tc = wxPyTypeCast(self.GetWindow(), "wxTextCtrl")
        val = tc.GetValue()

        for x in val:
            if x not in self.numList:
                return false

        return true

    def OnChar(self, event):
        key = event.KeyCode()
        if key < WXK_SPACE or key == WXK_DELETE or key > 255:
            event.Skip()
            return

        if chr(key) in self.numList:
            event.Skip()
            return

        if not wxValidator_IsSilent():
            wxBell()

        # Returning without calling even.Skip eats the event before it
        # gets to the text control
        return



class MyDialog(wxDialog):
    def __init__(self, parent):
        wxDialog.__init__(self, parent, -1, "hello")

        self.theValue =  555.12
        fltValidator = floatValidator(self, "theValue")

        Vbox = wxBoxSizer(wxVERTICAL)
        Tbox = wxBoxSizer(wxHORIZONTAL)
        Tbox.Add(wxStaticText(self, -1, "Initial Balance"), 0, wxALL,5)
        Tbox.Add(wxTextCtrl(self, 13, "123.45", validator = fltValidator,
                            size=(100, -1)), 0, wxALL,5)

        Vbox.Add(Tbox, 0, 0)

        Tbox = wxBoxSizer(wxHORIZONTAL)
        Tbox.Add(wxButton(self, wxID_OK, "Ok"), 0, wxALL,5)
        Tbox.Add(wxButton(self, wxID_CANCEL, "Cancel"), 0, wxALL,5)

        Vbox.Add(Tbox, 0, 0)

        self.SetAutoLayout(true)
        self.SetSizer(Vbox)
        Vbox.Fit(self)


class TestFrame(wxFrame):
    def __init__(self, parent):
        wxFrame.__init__(self, parent, -1, "Testing...", size=(150,75))
        wxButton(self, 25, "Click Me")
        EVT_BUTTON(self, 25, self.OnClick)


    def OnClick(self, event):
        dlg = MyDialog(self)
        dlg.ShowModal()
        print dlg.theValue
        dlg.Destroy()



app = wxPySimpleApp()
frame = TestFrame(None)
frame.Show(true)
app.MainLoop()




