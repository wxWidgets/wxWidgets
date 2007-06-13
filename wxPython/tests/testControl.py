import unittest
import wx

import testWindow

"""
This file contains classes and methods for unit testing the API of wx.Control

Methods yet to test:
__init__, Command, Create, GetAlignment, GetLabelText
"""

class ControlTest(testWindow.WindowTest):
    def __init__(self, arg):
        # superclass setup
        super(ControlTest, self).__init__(arg)
        # ControlTest setup
        
    #####################
    ## Fixture Methods ##
    #####################
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.testControl = wx.Control(parent=self.frame, id=wx.ID_ANY)
        self.children = []
        self.children_ids = []
        self.children_names = []
    
    def tearDown(self):
        self.frame.Destroy()
        self.app.Destroy()

    ##################
    ## Test Methods ##
    ##################
    
    # TODO: affirm that this is expected behavior
    def testAllControlsNeedParents(self):
        """
        All instances of wx.Control need to have a parent"""
        class_under_test = type(self.testControl)
        self.assertRaises(wx.PyAssertionError, class_under_test, None)
    
    def testDefaultAttributes(self):
        """GetClassDefaultAttributes"""
        attrs = wx.Control.GetClassDefaultAttributes()
        self.assert_(isinstance(attrs, wx.VisualAttributes))
        self.assert_(attrs.colBg.IsOk())
        self.assert_(attrs.colFg.IsOk())
        self.assert_(attrs.font.IsOk())

def suite():
    suite = unittest.makeSuite(ControlTest)
    return unittest.TestSuite(suite)
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
