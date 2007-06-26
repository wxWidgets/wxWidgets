import unittest
import wx

import wxtest
import testControl

"""
This file contains classes and methods for unit testing the API of wx.SpinCtrl

TODO:
    should it be legal for min value to be greater than max value?

Methods yet to test:

__init__
Create
GetClassDefaultAttributes
GetMax
GetMin
GetValue
SetRange
SetSelection
SetValue
SetValueString

"""

class SpinCtrlTest(testControl.ControlTest):
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.testControl = wx.SpinCtrl(parent=self.frame)
    
    # crashes interpreter on Windows for some reason
    def testAllControlsNeedParents(self):
        if wxtest.PlatformIsNotWindows():
            super(SpinCtrlTest,self).testAllControlsNeedParents()
        

def suite():
    suite = unittest.makeSuite(SpinCtrlTest)
    return unittest.TestSuite(suite)
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
