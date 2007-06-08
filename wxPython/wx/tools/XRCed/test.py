# Test module for future super xrced version :)
from globals import *
import view
from presenter import Presenter
import plugin

app = wx.PySimpleApp()
conf = view.conf = wx.Config(style = wx.CONFIG_USE_LOCAL_FILE)
conf.localconf = None
conf.autoRefresh = conf.ReadInt('autorefresh', True)
#pos = conf.ReadInt('x', -1), conf.ReadInt('y', -1)
#size = conf.ReadInt('width', 800), conf.ReadInt('height', 600)
conf.embedPanel = conf.ReadInt('embedPanel', True)
conf.showTools = conf.ReadInt('showTools', True)
conf.sashPos = conf.ReadInt('sashPos', 200)

g.fileHistory = wx.FileHistory()
g.fileHistory.Load(conf)

plugin.load_plugins('plugins')

frame = view.Frame()
Presenter.init()
frame.Show()
app.MainLoop()
