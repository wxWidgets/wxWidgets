import unittest
import wx

import testWindow

"""
This file contains classes and methods for unit testing the API of wx.Control

Methods yet to test:
__init__, Command, Create, GetAlignment, GetClassDefaultAttributes, GetLabelText
"""

class ControlTestFrame(wx.Frame):
    def __init__(self, parent, id):
        wx.Frame.__init__(self, parent, id, 'TestFrame',
                size=(340, 200))
        self.testControl = wx.Control(parent=self, id=wx.ID_ANY)

class ControlTest(testWindow.WindowTest):
    def __init__(self, arg):
        # superclass setup
        super(ControlTest, self).__init__(arg)
        # ControlTest setup
        
    #####################
    ## Fixture Methods ##
    #####################
    def setUp(self):
        self.frame = ControlTestFrame(parent=None, id=wx.ID_ANY)
        # we just do this to shorten typing :-)
        self.testControl = self.frame.testControl
        self.children = []
        self.children_ids = []
        self.children_names = []

    ##################
    ## Test Methods ##
    ##################
    
    # additional tests go here

def suite():
    suite = unittest.makeSuite(ControlTest)
    return unittest.TestSuite(suite)
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
