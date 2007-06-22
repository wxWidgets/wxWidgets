import unittest
import wx

import testListBox

"""
This file contains classes and methods for unit testing the API of wx.CheckListBox.
        
Methods yet to test:
__init__, Check, Create, GetItemHeight, IsChecked
"""

class CheckListBoxTest(testListBox.ListBoxTest):
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.testControl = wx.CheckListBox(parent=self.frame, id=wx.ID_ANY)
    
    def tearDown(self):
        self.app.Destroy()
        self.frame.Destroy()
        

def suite():
    suite = unittest.makeSuite(CheckListBoxTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
