import unittest
import wx

import testControlWithItems

"""
This file contains classes and methods for unit testing the API of wx.ListBox.
        
Methods yet to test:
__init__, AppendAndEnsureVisible, Create, Deselect, DeselectAll, EnsureVisible,
GetClassDefaultAttributes, GetSelections, HitTest, Insert, InsertItems, IsSelected,
IsSorted, Select, Set, SetFirstItem, SetFirstItemStr, SetItemBackgroundColour,
SetItemFont, SetItemForegroundColour, SetSelection, SetStringSelection
"""

class ListBoxTest(testControlWithItems.ControlWithItemsBase):
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.testControl = wx.ListBox(parent=self.frame, id=wx.ID_ANY)
    
    def tearDown(self):
        self.app.Destroy()
        self.frame.Destroy()
        

def suite():
    suite = unittest.makeSuite(ListBoxTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
