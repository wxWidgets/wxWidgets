import wx
import pprint

about_txt = """\
The validator used in this example shows how the validator
can be used to transfer data to and from each text control
automatically when the dialog is shown and dismissed."""


class DataXferValidator(wx.PyValidator):
     def __init__(self, data, key):
         wx.PyValidator.__init__(self)
         self.data = data
         self.key = key

     def Clone(self):
         """
         Note that every validator must implement the Clone() method.
         """
         return DataXferValidator(self.data, self.key)

     def Validate(self, win):
         return True

     def TransferToWindow(self):
         textCtrl = self.GetWindow()
         textCtrl.SetValue(self.data.get(self.key, ""))
         return True 

     def TransferFromWindow(self):
         textCtrl = self.GetWindow()
         self.data[self.key] = textCtrl.GetValue()
         return True



class MyDialog(wx.Dialog):
    def __init__(self, data):
        wx.Dialog.__init__(self, None, -1, "Validators: data transfer")

        # Create the text controls
        about   = wx.StaticText(self, -1, about_txt)
        name_l  = wx.StaticText(self, -1, "Name:")
        email_l = wx.StaticText(self, -1, "Email:")
        phone_l = wx.StaticText(self, -1, "Phone:")
        
        name_t  = wx.TextCtrl(self, validator=DataXferValidator(data, "name"))
        email_t = wx.TextCtrl(self, validator=DataXferValidator(data, "email"))
        phone_t = wx.TextCtrl(self, validator=DataXferValidator(data, "phone"))

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

data = { "name" : "Jordyn Dunn" }
dlg = MyDialog(data)
dlg.ShowModal()
dlg.Destroy()

wx.MessageBox("You entered these values:\n\n" +
              pprint.pformat(data))

app.MainLoop()
