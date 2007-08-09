import wx

if __name__ == "__main__":
    app = wx.PySimpleApp()
    dialog = wx.ColourDialog(None)
    dialog.GetColourData().SetChooseFull(True)
    if dialog.ShowModal() == wx.ID_OK:
        data = dialog.GetColourData()
        print 'You selected: %s\n' % str(data.GetColour().Get())
    dialog.Destroy()
