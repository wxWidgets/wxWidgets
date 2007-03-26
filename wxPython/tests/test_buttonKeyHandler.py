import wx

BIND_HANDLERS = False
SKIP_EVENT    = True

def OnKeyDown(evt):
    print "OnKeyDown", evt.KeyCode
    if SKIP_EVENT:
        evt.Skip()

def OnKeyUp(evt):
    print "OnKeyUp", evt.KeyCode
    if SKIP_EVENT:
        evt.Skip()

def OnChar(evt):
    print "OnChar", evt.KeyCode
    if SKIP_EVENT:
        evt.Skip()


app = wx.App(False)
frm = wx.Frame(None, title="test_buttonKeyHandler")
pnl = wx.Panel(frm)

btn = wx.Button(pnl, label="Focus me and press a key", pos=(20,20))
if BIND_HANDLERS:
    btn.Bind(wx.EVT_KEY_DOWN, OnKeyDown)
    btn.Bind(wx.EVT_KEY_UP, OnKeyUp)
    btn.Bind(wx.EVT_CHAR, OnChar)

frm.Show()
app.MainLoop()
