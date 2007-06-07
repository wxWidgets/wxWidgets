# Test module for future super xrced version :)
from globals import *
import view
from presenter import Presenter
import plugin
from XMLTreeMenu import ID

app = wx.PySimpleApp()

plugin.load_plugins('plugins')
ID.LAST_COMPONENT = wx.NewId()

frame = view.Frame()
Presenter.init()
frame.Show()
app.MainLoop()
