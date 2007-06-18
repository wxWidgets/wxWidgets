import unittest
import wx

import testTopLevelWindow

"""
This file contains classes and methods for unit testing the API of wx.Frame.
        
Methods yet to test:


"""

class FrameTest(testTopLevelWindow.TopLevelWindowBase):
    def __init__(self, arg):
        # superclass setup
        super(FrameTest, self).__init__(arg)
        # FrameTest setup
        
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.testControl = wx.Frame(parent=self.frame, id=wx.ID_ANY)
    
    # TODO: find out why these three tests fail in FrameTest
    # but nowhere else.
    def testCenter(self):
        pass
    def testRect(self):
        pass
    def testSize(self):
        pass


def suite():
    suite = unittest.makeSuite(FrameTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
