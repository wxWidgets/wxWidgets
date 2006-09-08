
import  string
import  wx

#----------------------------------------------------------------------

ALPHA_ONLY = 1
DIGIT_ONLY = 2

class MyValidator(wx.PyValidator):
    def __init__(self, flag=None, pyVar=None):
        wx.PyValidator.__init__(self)
        self.flag = flag
        self.Bind(wx.EVT_CHAR, self.OnChar)

    def Clone(self):
        return MyValidator(self.flag)

    def Validate(self, win):
        tc = self.GetWindow()
        val = tc.GetValue()
        
        if self.flag == ALPHA_ONLY:
            for x in val:
                if x not in string.letters:
                    return False

        elif self.flag == DIGIT_ONLY:
            for x in val:
                if x not in string.digits:
                    return False

        return True


    def OnChar(self, event):
        key = event.GetKeyCode()

        if key < wx.WXK_SPACE or key == wx.WXK_DELETE or key > 255:
            event.Skip()
            return

        if self.flag == ALPHA_ONLY and chr(key) in string.letters:
            event.Skip()
            return

        if self.flag == DIGIT_ONLY and chr(key) in string.digits:
            event.Skip()
            return

        if not wx.Validator_IsSilent():
            wx.Bell()

        # Returning without calling even.Skip eats the event before it
        # gets to the text control
        return

#----------------------------------------------------------------------

class TestValidatorPanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, -1)
        self.SetAutoLayout(True)
        VSPACE = 10

        fgs = wx.FlexGridSizer(0, 2)

        fgs.Add((1,1))
        fgs.Add(wx.StaticText(self, -1, "These controls have validators that limit\n"
                             "the type of characters that can be entered."))

        fgs.Add((1,VSPACE)); fgs.Add((1,VSPACE))

        label = wx.StaticText(self, -1, "Alpha Only: ")
        fgs.Add(label, 0, wx.ALIGN_RIGHT|wx.CENTER)

        fgs.Add(wx.TextCtrl(self, -1, "", validator = MyValidator(ALPHA_ONLY)))

        fgs.Add((1,VSPACE)); fgs.Add((1,VSPACE))

        label = wx.StaticText(self, -1, "Digits Only: ")
        fgs.Add(label, 0, wx.ALIGN_RIGHT|wx.CENTER)
        fgs.Add(wx.TextCtrl(self, -1, "", validator = MyValidator(DIGIT_ONLY)))

        fgs.Add((1,VSPACE)); fgs.Add((1,VSPACE))
        fgs.Add((1,VSPACE)); fgs.Add((1,VSPACE))
        fgs.Add((0,0))
        b = wx.Button(self, -1, "Test Dialog Validation")
        self.Bind(wx.EVT_BUTTON, self.OnDoDialog, b)
        fgs.Add(b)

        border = wx.BoxSizer()
        border.Add(fgs, 1, wx.GROW|wx.ALL, 25)
        self.SetSizer(border)
        self.Layout()

    def OnDoDialog(self, evt):
        dlg = TestValidateDialog(self)
        dlg.ShowModal()
        dlg.Destroy()


#----------------------------------------------------------------------

class TextObjectValidator(wx.PyValidator):
     """ This validator is used to ensure that the user has entered something
         into the text object editor dialog's text field.
     """
     def __init__(self):
         """ Standard constructor.
         """
         wx.PyValidator.__init__(self)



     def Clone(self):
         """ Standard cloner.

             Note that every validator must implement the Clone() method.
         """
         return TextObjectValidator()


     def Validate(self, win):
         """ Validate the contents of the given text control.
         """
         textCtrl = self.GetWindow()
         text = textCtrl.GetValue()

         if len(text) == 0:
             wx.MessageBox("A text object must contain some text!", "Error")
             textCtrl.SetBackgroundColour("pink")
             textCtrl.SetFocus()
             textCtrl.Refresh()
             return False
         else:
             textCtrl.SetBackgroundColour(
                 wx.SystemSettings_GetColour(wx.SYS_COLOUR_WINDOW))
             textCtrl.Refresh()
             return True


     def TransferToWindow(self):
         """ Transfer data from validator to window.

             The default implementation returns False, indicating that an error
             occurred.  We simply return True, as we don't do any data transfer.
         """
         return True # Prevent wxDialog from complaining.


     def TransferFromWindow(self):
         """ Transfer data from window to validator.

             The default implementation returns False, indicating that an error
             occurred.  We simply return True, as we don't do any data transfer.
         """
         return True # Prevent wxDialog from complaining.

#----------------------------------------------------------------------

class TestValidateDialog(wx.Dialog):
    def __init__(self, parent):
        wx.Dialog.__init__(self, parent, -1, "Validated Dialog")

        self.SetAutoLayout(True)
        VSPACE = 10

        fgs = wx.FlexGridSizer(0, 2)

        fgs.Add((1,1));
        fgs.Add(wx.StaticText(self, -1,
                             "These controls must have text entered into them.  Each\n"
                             "one has a validator that is checked when the Okay\n"
                             "button is clicked."))

        fgs.Add((1,VSPACE)); fgs.Add((1,VSPACE))

        label = wx.StaticText(self, -1, "First: ")
        fgs.Add(label, 0, wx.ALIGN_RIGHT|wx.CENTER)

        fgs.Add(wx.TextCtrl(self, -1, "", validator = TextObjectValidator()))

        fgs.Add((1,VSPACE)); fgs.Add((1,VSPACE))

        label = wx.StaticText(self, -1, "Second: ")
        fgs.Add(label, 0, wx.ALIGN_RIGHT|wx.CENTER)
        fgs.Add(wx.TextCtrl(self, -1, "", validator = TextObjectValidator()))


        buttons = wx.StdDialogButtonSizer() #wx.BoxSizer(wx.HORIZONTAL)
        b = wx.Button(self, wx.ID_OK, "OK")
        b.SetDefault()
        buttons.AddButton(b)
        buttons.AddButton(wx.Button(self, wx.ID_CANCEL, "Cancel"))
        buttons.Realize()

        border = wx.BoxSizer(wx.VERTICAL)
        border.Add(fgs, 1, wx.GROW|wx.ALL, 25)
        border.Add(buttons)
        self.SetSizer(border)
        border.Fit(self)
        self.Layout()


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestValidatorPanel(nb)
    return win

#----------------------------------------------------------------------



overview = """\
<html>
<body>
wx.Validator is the base class for a family of validator classes that mediate 
between a class of control, and application data.

<p>A validator has three major roles:

<p><ol>
<li>to transfer data from a C++ variable or own storage to and from a control;
<li>to validate data in a control, and show an appropriate error message;
<li>to filter events (such as keystrokes), thereby changing the behaviour of the associated control.
</ol>
<p>Validators can be plugged into controls dynamically.
</body>
</html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

