import wx
from component import manager
from XMLTreeMenu import ID
import XMLTree
import plugin

class Frame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, -1, '')
        tree = XMLTree.XMLTree(self)
        wx.EVT_MENU_RANGE(self, ID.FIRST_COMPONENT, ID.LAST_COMPONENT, 
                          self.OnComponent)
        self.tree = tree

    def OnComponent(self, evt):
        component = manager.findById(evt.GetId())
        print component
        tree = self.tree
        item = tree.GetSelection()
        tree.AppendItem(item, component.name, component.imageId)
        tree.Expand(item)

app = wx.PySimpleApp()

ID.FIRST_COMPONENT = wx.NewId()
plugin.load_plugins('plugins')
ID.LAST_COMPONENT = wx.NewId()

frame = Frame()
frame.Show()
app.MainLoop()
