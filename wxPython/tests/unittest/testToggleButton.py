import unittest
import wx

import testControl

"""
This file contains classes and methods for unit testing the API of 
wx.ToggleButton.

This class is virtually fully-tested.
"""


class ToggleButtonTest(testControl.ControlTest):
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.testControl = wx.ToggleButton(parent=self.frame)
    
    def tearDown(self):
        self.frame.Destroy()
        self.app.Destroy()
        
    def testSetValueFails(self):
        """
        That SetValue must be called with one and only one argument"""
        self.assertRaises(TypeError, self.testControl.SetValue)
        self.assertRaises(TypeError, self.testControl.SetValue, True, True)
    
    def testValue(self):
        """SetValue, GetValue"""
        self.assert_(not self.testControl.GetValue())
        self.testControl.SetValue(True)
        self.assert_(self.testControl.GetValue())
        self.testControl.SetValue(False)
        self.assert_(not self.testControl.GetValue())
        

def suite():
    suite = unittest.makeSuite(ToggleButtonTest)
    return unittest.TestSuite(suite)
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
