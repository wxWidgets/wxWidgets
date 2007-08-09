import wx

about_txt = """\
The validator used in this example will ensure that the text
controls are not empty when you press the Ok button, and
will not let you leave if any of the Validations fail."""


class NotEmptyValidator(wx.PyValidator):
     def __init__(self):
         wx.PyValidator.__init__(self)

     def Clone(self):
         """
         Note that every validator must implement the Clone() method.
         """
         return NotEmptyValidator()

     def Validate(self, win):
         textCtrl = self.GetWindow()
         text = textCtrl.GetValue()

         if len(text) == 0:
             wx.MessageBox("This field must contain some text!", "Error")
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
         return True 

     def TransferFromWindow(self):
         return True



class MyDialog(wx.Dialog):
    def __init__(self):
        wx.Dialog.__init__(self, None, -1, "Validators: validating")

        # Create the text controls
        about   = wx.StaticText(self, -1, about_txt)
        name_l  = wx.StaticText(self, -1, "Name:")
        email_l = wx.StaticText(self, -1, "Email:")
        phone_l = wx.StaticText(self, -1, "Phone:")
        
        name_t  = wx.TextCtrl(self, validator=NotEmptyValidator())
        email_t = wx.TextCtrl(self, validator=NotEmptyValidator())
        phone_t = wx.TextCtrl(self, validator=NotEmptyValidator())

        # Use standard button IDs
        okay   = wx.Button(self, wx.ID_OK)
        okay.SetDefault()
        cancel = wx.Button(self, wx.ID_CANCEL)

        # Layout with sizers
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(about, 0, wx.ALL, 5)
        sizer.Add(wx.StaticLine(self), 0, wx.EXPAND|wx.ALL, 5)
        
        fgs = wx.FlexGridSizer(3, 2, 5, 5)
        fgs.Add(name_l, 0, wx.ALIGN_RIGHT)
        fgs.Add(name_t, 0, wx.EXPAND)
        fgs.Add(email_l, 0, wx.ALIGN_RIGHT)
        fgs.Add(email_t, 0, wx.EXPAND)
        fgs.Add(phone_l, 0, wx.ALIGN_RIGHT)
        fgs.Add(phone_t, 0, wx.EXPAND)
        fgs.AddGrowableCol(1)
        sizer.Add(fgs, 0, wx.EXPAND|wx.ALL, 5)

        btns = wx.StdDialogButtonSizer()
        btns.AddButton(okay)
        btns.AddButton(cancel)
        btns.Realize()
        sizer.Add(btns, 0, wx.EXPAND|wx.ALL, 5)

        self.SetSizer(sizer)
        sizer.Fit(self)
        

app = wx.PySimpleApp()

dlg = MyDialog()
dlg.ShowModal()
dlg.Destroy()

app.MainLoop()
