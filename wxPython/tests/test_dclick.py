import wx

def OnClick(evt):
    print 'Click'

def OnDClick(evt):
    print 'DClick'

def OnMouse(evt):
    if evt.LeftDClick():
        print 'DClick'
    elif evt.LeftDown():
        print 'Click'
    
app = wx.App(redirect=False)
frame = wx.Frame(None, title="Test mouse clicks")
panel = wx.Panel(frame)

if True:
    # try separate
    panel.Bind(wx.EVT_LEFT_DOWN, OnClick)
    panel.Bind(wx.EVT_LEFT_DCLICK, OnDClick)
else:
    # or together
    panel.Bind(wx.EVT_MOUSE_EVENTS, OnMouse)

frame.Show()
app.MainLoop()
