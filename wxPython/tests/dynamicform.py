

from wxPython.wx import *

class FieldData:
    def __init__(self, name, label, shortHelp="", defValue="",
                 size=(-1, -1), style=0, ID=-1):
        self.name = name
        self.label = label
        self.shortHelp = shortHelp
        self.defValue = defValue
        self.size = size
        self.style = style
        self.ID = ID


class DynamicForm(wxPanel):
    def __init__(self, parent, ID, fieldData):
        wxPanel.__init__(self, parent, ID)

        sizer = wxFlexGridSizer(cols=2, vgap=5, hgap=5)
        for field in fieldData:
            label = wxStaticText(self, -1, field.label)
            sizer.Add(label, 0, wxALIGN_RIGHT)
            text = wxTextCtrl(self, field.ID, field.defValue,
                              size=field.size, style=field.style)
            if field.shortHelp:
                text.SetToolTip(wxToolTip(field.shortHelp))
            self.__dict__["get_"+field.name] = text.GetValue
            self.__dict__["set_"+field.name] = text.SetValue
            sizer.Add(text, 0, wxEXPAND)

        sizer.Fit(self)
        self.SetAutoLayout(true)
        self.SetSizer(sizer)


class TestFrame(wxFrame):
    testFields = [
        FieldData("fname", "First name:", "Enter someone's first name"),
        FieldData("lname", "Last name:",  "Enter someone's last name"),
        FieldData("email", "Email address:", "just figure it out..."),
        ]
    def __init__(self):
        wxFrame.__init__(self, None, -1, "This is a test")
        form = DynamicForm(self, -1, self.testFields)
        form.set_fname("Robin")
        form.set_lname("Dunn")
        self.form = form
        self.Fit()
        EVT_CLOSE(self, self.OnCloseWindow)

    def OnCloseWindow(self, evt):
        print self.form.get_email()
        self.Destroy()


app = wxPySimpleApp()
frame = TestFrame()
frame.Show(true)
app.MainLoop()

