import unittest
import wx

import wxtest
import testButton

"""
This file contains classes and methods for unit testing the API of 
wx.BitmapButton

Methods yet to test:
__init__, Create, GetBitmapDisabled, GetBitmapFocus, GetBitmapHover,
GetBitmapLabel, GetBitmapSelected, GetMarginX, GetMarginY,
SetBitmapDisabled, SetBitmapFocus, SetBitmapHover, SetBitmapLabel,
SetBitmapSelected, SetMargins
"""


class BitmapButtonTest(testButton.ButtonTest):
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.testControl = wx.BitmapButton(parent=self.frame, bitmap=wx.NullBitmap)
    
    def tearDown(self):
        self.app.Destroy()
        self.frame.Destroy()
    
    # crashes interpreter on Windows for some reason
    def testAllControlsNeedParents(self):
        if wxtest.PlatformIsNotWindows():
            super(BitmapButtonTest,self).testAllControlsNeedParents()
        
        

def suite():
    suite = unittest.makeSuite(BitmapButtonTest)
    return unittest.TestSuite(suite)
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
