
from wxPython.wx        import *
from wxPython.lib.grids import wxFlexGridSizer

import string

#----------------------------------------------------------------------

ALPHA_ONLY = 1
DIGIT_ONLY = 2

class MyValidator(wxPyValidator):
    def __init__(self, flag=None, pyVar=None):
        wxPyValidator.__init__(self)
        self.flag = flag
        EVT_CHAR(self, self.OnChar)

    def Clone(self):
        return MyValidator(self.flag)

    def Validate(self, win):
        print 'validate'
        tc = wxPyTypeCast(win, "wxTextCtrl")
        val = tc.GetValue()
        if self.flag == ALPHA_ONLY:
            for x in val:
                if x not in string.letters:
                    return false

        elif self.flag == DIGIT_ONLY:
            for x in val:
                if x not in string.digits:
                    return false

        return true


    def OnChar(self, event):
        key = event.KeyCode()
        if key < WXK_SPACE or key == WXK_DELETE or key > 255:
            event.Skip()
            return
        if self.flag == ALPHA_ONLY and chr(key) in string.letters:
            event.Skip()
            return
        if self.flag == DIGIT_ONLY and chr(key) in string.digits:
            event.Skip()
            return

        if not wxValidator_IsSilent():
            wxBell()

        # Returning without calling even.Skip eats the event before it
        # gets to the text control
        return

#----------------------------------------------------------------------

class TestValidatorPanel(wxPanel):
    def __init__(self, parent):
        wxPanel.__init__(self, parent, -1)
        self.SetAutoLayout(true)
        VSPACE = 10

        fgs = wxFlexGridSizer(0, 2)

        fgs.Add(1,1);
        fgs.Add(wxStaticText(self, -1, "These controls have validators that limit\n"
                             "the type of characters that can be entered."))

        fgs.Add(1,VSPACE); fgs.Add(1,VSPACE)

        label = wxStaticText(self, -1, "Alpha Only: ")
        fgs.Add(label, 0, wxALIGN_RIGHT|wxCENTER)

        fgs.Add(wxTextCtrl(self, -1, "", validator = MyValidator(ALPHA_ONLY))

        fgs.Add(1,VSPACE); fgs.Add(1,VSPACE)

        label = wxStaticText(self, -1, "Digits Only: ")
        fgs.Add(label, 0, wxALIGN_RIGHT|wxCENTER)
        fgs.Add(wxTextCtrl(self, -1, "", validator = MyValidator(DIGITS_ONLY)))

        border = wxBoxSizer()
        border.Add(fgs, 1, wxGROW|wxALL, 25)
        self.SetSizer(border)
        self.Layout()


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestValidatorPanel(nb)
    return win

#----------------------------------------------------------------------









overview = """\
wxValidator is the base class for a family of validator classes that mediate between a class of control, and application data.

A validator has three major roles:

1. to transfer data from a C++ variable or own storage to and from a control;

2. to validate data in a control, and show an appropriate error message;

3. to filter events (such as keystrokes), thereby changing the behaviour of the associated control.

Validators can be plugged into controls dynamically.

"""
