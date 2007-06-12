import unittest
import wx

import testControl

"""
This file contains classes and methods for unit testing the API of wx.Button

Methods yet to test:
__init__, Create, GetClassDefaultAttributes, GetDefaultSize, SetDefault
"""

class ButtonTestFrame(wx.Frame):
    def __init__(self, parent, id):
        wx.Frame.__init__(self, parent, id, 'TestFrame',
                size=(340, 200))
        self.testControl = wx.Button(parent=self, id=wx.ID_ANY)

class ButtonTest(testControl.ControlTest):
    def __init__(self, arg):
        # superclass setup
        super(ButtonTest, self).__init__(arg)
        # ButtonTest setup
        
    #####################
    ## Fixture Methods ##
    #####################
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = ButtonTestFrame(parent=None, id=wx.ID_ANY)
        # we just do this to shorten typing :-)
        self.testControl = self.frame.testControl
        self.children = []
        self.children_ids = []
        self.children_names = []
    
    def tearDown(self):
        self.frame.Destroy()
        self.app.Destroy()

    ##################
    ## Test Methods ##
    ##################
    
    # additional tests go here
    pass

def suite():
    suite = unittest.makeSuite(ButtonTest)
    return unittest.TestSuite(suite)
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
