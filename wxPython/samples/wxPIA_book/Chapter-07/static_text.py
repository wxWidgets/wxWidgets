import wx

class StaticTextFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, -1, 'Static Text Example', 
                size=(400, 300))
        panel = wx.Panel(self, -1)
        wx.StaticText(panel, -1, "This is an example of static text", 
                (100, 10))
        rev = wx.StaticText(panel, -1, "Static Text With Reversed Colors", 
                (100, 30))
        rev.SetForegroundColour('white')
        rev.SetBackgroundColour('black')
        center = wx.StaticText(panel, -1, "align center", (100, 50), 
                (160, -1), wx.ALIGN_CENTER)
        center.SetForegroundColour('white')
        center.SetBackgroundColour('black')
        right = wx.StaticText(panel, -1, "align right", (100, 70), 
                (160, -1), wx.ALIGN_RIGHT)
        right.SetForegroundColour('white')
        right.SetBackgroundColour('black')
        str = "You can also change the font."
        text = wx.StaticText(panel, -1, str, (20, 100))
        font = wx.Font(18, wx.DECORATIVE, wx.ITALIC, wx.NORMAL)
        text.SetFont(font)
        wx.StaticText(panel, -1, "Your text\ncan be split\n"
                "over multiple lines\n\neven blank ones", (20,150))
        wx.StaticText(panel, -1, "Multi-line text\ncan also\n"
                "be right aligned\n\neven with a blank", (220,150), 
                style=wx.ALIGN_RIGHT)


if __name__ == '__main__':
    app = wx.PySimpleApp()
    frame = StaticTextFrame()
    frame.Show()
    app.MainLoop()

