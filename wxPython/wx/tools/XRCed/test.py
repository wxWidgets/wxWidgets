# Test module for future super xrced version :)
import os
from globals import *
import params
from presenter import Presenter
from listener import Listener
from component import Manager
import view
import undo
import plugin

# Set application path for loading resources
if __name__ == '__main__':
    g.basePath = os.path.dirname(sys.argv[0])
else:
    g.basePath = os.path.dirname(__file__)

app = wx.PySimpleApp()

toolArtProvider = view.ToolArtProvider()
wx.ArtProvider.Push(toolArtProvider)

conf = g.conf = wx.Config(style = wx.CONFIG_USE_LOCAL_FILE)
conf.localconf = None
conf.autoRefresh = conf.ReadInt('autorefresh', True)
#pos = conf.ReadInt('x', -1), conf.ReadInt('y', -1)
#size = conf.ReadInt('width', 800), conf.ReadInt('height', 600)
conf.embedPanel = conf.ReadInt('embedPanel', True)
conf.showTools = conf.ReadInt('showTools', True)
conf.sashPos = conf.ReadInt('sashPos', 200)
if not conf.embedPanel:
    conf.panelX = conf.ReadInt('panelX', -1)
    conf.panelY = conf.ReadInt('panelY', -1)
else:
    conf.panelX = conf.panelY = -1
conf.panelWidth = conf.ReadInt('panelWidth', 200)
conf.panelHeight = conf.ReadInt('panelHeight', 200)
# Load file history from config
g.fileHistory = wx.FileHistory()
g.fileHistory.Load(conf)

g.undoMan = undo.UndoManager()

Manager.init()
# Load standard plugins
plugin.load_plugins('plugins')

# Setup MVP
view.CreateView()
Presenter.init()
Listener.Install(view.frame, view.tree, view.panel)

if len(sys.argv) > 1:
    Presenter.open(sys.argv[1])
view.frame.Show()
app.MainLoop()
