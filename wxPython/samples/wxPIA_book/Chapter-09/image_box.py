import wx
import wx.lib.imagebrowser as imagebrowser

if __name__ == "__main__":
    app = wx.PySimpleApp()
    dialog = imagebrowser.ImageDialog(None)   
    if dialog.ShowModal() == wx.ID_OK:
        print "You Selected File: " + dialog.GetFile()  
    dialog.Destroy()


