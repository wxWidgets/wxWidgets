import wx
import os

if __name__ == "__main__":
    app = wx.PySimpleApp()
    wildcard = "Python source (*.py)|*.py|" \
            "Compiled Python (*.pyc)|*.pyc|" \
            "All files (*.*)|*.*"
    dialog = wx.FileDialog(None, "Choose a file", os.getcwd(), 
            "", wildcard, wx.OPEN)
    if dialog.ShowModal() == wx.ID_OK:
        print dialog.GetPath() 

    dialog.Destroy()
    


