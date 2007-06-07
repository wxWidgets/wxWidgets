# Name:         view.py
# Purpose:      View classes
# Author:       Roman Rolinsky <rolinsky@femagsoft.com>
# Created:      07.06.2007
# RCS-ID:       $Id$

from XMLTree import *
from component import Manager
from presenter import Presenter

class Frame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, -1, '')
        global frame
        frame = self
        global tree
        tree = XMLTree(self)
        wx.EVT_MENU_RANGE(self, ID.FIRST_COMPONENT, ID.LAST_COMPONENT, 
                          self.OnComponent)

    def OnComponent(self, evt):
        component = Manager.findById(evt.GetId())
        print component
        item = tree.GetSelection()
        tree.AppendItem(item, component.name, component.imageId)
        tree.Expand(item)
        # Notify Presenter
        Presenter.setModified()
