import unittest
import wx

import testControlWithItems

"""
This file contains classes and methods for unit testing the API of wx.Choice.
        
Methods yet to test:
__init__, Create, GetClassDefaultAttributes, GetCurrentSelection
"""

class ChoiceTest(testControlWithItems.ControlWithItemsBase):
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.testControl = wx.Choice(parent=self.frame, id=wx.ID_ANY)
    
    def tearDown(self):
        self.app.Destroy()
        self.frame.Destroy()
        

def suite():
    suite = unittest.makeSuite(ChoiceTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
