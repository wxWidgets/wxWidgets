import wx

if __name__ == "__main__":
    app = wx.PySimpleApp()
    choices = ["Alpha", "Baker", "Charlie", "Delta"]
    dialog = wx.SingleChoiceDialog(None, "Pick A Word", "Choices",
            choices)
    if dialog.ShowModal() == wx.ID_OK:
        print "You selected: %s\n" % dialog.GetStringSelection()

    dialog.Destroy()
    
