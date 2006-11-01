import wx

if __name__ == "__main__":
    app = wx.PySimpleApp()
    dialog = wx.FontDialog(None, wx.FontData())
    if dialog.ShowModal() == wx.ID_OK:
        data = dialog.GetFontData()
        font = data.GetChosenFont()
        colour = data.GetColour()
        print 'You selected: "%s", %d points\n' % (
                font.GetFaceName(), font.GetPointSize())
    dialog.Destroy()




