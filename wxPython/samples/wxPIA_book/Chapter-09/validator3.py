import wx
import string

about_txt = """\
The validator used in this example will validate the input on the fly
instead of waiting until the okay button is pressed.  The first field
will not allow digits to be typed, the second will allow anything
and the third will not allow alphabetic characters to be entered.
"""


class CharValidator(wx.PyValidator):
    def __init__(self, flag):
         wx.PyValidator.__init__(self)
         self.flag = flag
         self.Bind(wx.EVT_CHAR, self.OnChar)

    def Clone(self):
         """
         Note that every validator must implement the Clone() method.
         """
         return CharValidator(self.flag)

    def Validate(self, win):
         return True

    def TransferToWindow(self):
         return True 

    def TransferFromWindow(self):
         return True

    def OnChar(self, evt):
         key = chr(evt.GetKeyCode())
         if self.flag == "no-alpha" and key in string.letters:
              return
         if self.flag == "no-digit" and key in string.digits:
              return
         evt.Skip()


class MyDialog(wx.Dialog):
    def __init__(self):
        wx.Dialog.__init__(self, None, -1, "Validators: behavior modification")

        # Create the text controls
        about   = wx.StaticText(self, -1, about_txt)
        name_l  = wx.StaticText(self, -1, "Name:")
        email_l = wx.StaticText(self, -1, "Email:")
        phone_l = wx.StaticText(self, -1, "Phone:")
        
        name_t  = wx.TextCtrl(self, validator=CharValidator("no-digit"))
        email_t = wx.TextCtrl(self, validator=CharValidator("any"))
        phone_t = wx.TextCtrl(self, validator=CharValidator("no-alpha"))

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
