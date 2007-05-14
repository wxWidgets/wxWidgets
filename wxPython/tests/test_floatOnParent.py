import wx

app = wx.App(False)

mf = wx.Frame(None, title="MainFrame")
p = wx.Panel(mf)
txt = wx.TextCtrl(p, value="Hello", pos=(10,10))
mf.Show()

pf = wx.Frame(parent=mf, size=(150,150), #title="popup frame",
              style=0
              #| wx.DEFAULT_FRAME_STYLE
              | wx.FRAME_NO_TASKBAR
              #| wx.FRAME_TOOL_WINDOW | wx.CAPTION | wx.TINY_CAPTION_VERT
              | wx.FRAME_FLOAT_ON_PARENT
              #| wx.STAY_ON_TOP
              | wx.POPUP_WINDOW
              | wx.NO_BORDER
              )
#pf.MacSetMetalAppearance(True)
p = wx.Panel(pf)
p.SetBackgroundColour("pink")
pf.Show()
pf.Move(mf.GetPosition() + (50,50))
mf.Raise()

app.MainLoop()
