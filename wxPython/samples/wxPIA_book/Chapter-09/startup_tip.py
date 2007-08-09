import wx

if __name__ == "__main__":
    app = wx.PySimpleApp()
    provider = wx.CreateFileTipProvider("tips.txt", 0)
    wx.ShowTip(None, provider, True)
    

