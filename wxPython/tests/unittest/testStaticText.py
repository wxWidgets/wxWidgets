import unittest
import wx

import testControl

"""
This file contains classes and methods for unit testing the API of wx.StaticText

TODO: test setting different fonts and styles.
    are there other methods to test or is that it?
"""

class StaticTextTest(testControl.ControlTest):
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.name = "Static Text Control"
        self.testControl = wx.StaticText(parent=self.frame, id=wx.ID_ANY,
                                            name=self.name)
                                            
                                            
def suite():
    suite = unittest.makeSuite(StaticTextTest)
    return unittest.TestSuite(suite)
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
