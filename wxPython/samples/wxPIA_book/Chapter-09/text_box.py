import wx

if __name__ == "__main__":
    app = wx.PySimpleApp()
    dialog = wx.TextEntryDialog(None, 
            "What kind of text would you like to enter?",
            "Text Entry", "Default Value", style=wx.OK|wx.CANCEL)
    if dialog.ShowModal() == wx.ID_OK:
        print "You entered: %s" % dialog.GetValue()

    dialog.Destroy()
