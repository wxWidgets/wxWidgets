import unittest
import wx

import wxtest
import testControl

"""
This file contains classes and methods for unit testing the API of wx.SpinCtrl

TODO:
    should it be legal for min value to be greater than max value?

Methods yet to test:
__init__, Create, GetClassDefaultAttributes, SetSelection
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
    
    def testConstructor(self):
        """__init__
        'Nearly all of the complexity of the spin ontrol is in the constructor'
            - pg.209, 1st paragraph, 'wxPython in Action' """
        a = wx.SpinCtrl(parent=self.frame, min=-1000, max=1000,
                            initial=42)
        self.assertEquals(-1000, a.GetMin())
        self.assertEquals(1000, a.GetMax())
        self.assertEquals(42, a.GetValue())
    
    def testMinMaxRange(self):
        """SetRange, GetMin, GetMax"""
        for min,max in ((1,10),(-100,-10),(100,1000)):
            self.testControl.SetRange(min,max)
            self.assertEquals(min, self.testControl.GetMin())
            self.assertEquals(max, self.testControl.GetMax())
    
    def testValue(self):
        """SetValue, GetValue"""
        min = self.testControl.GetMin()
        max = self.testControl.GetMax()
        for i in range(min,max+1):
            self.testControl.SetValue(i)
            self.assertEquals(i, self.testControl.GetValue())
        for j in range(min-100,min):
            self.testControl.SetValue(j)
            self.assertEquals(min, self.testControl.GetValue())
        for k in range(max,max+100):
            self.testControl.SetValue(k)
            self.assertEquals(max, self.testControl.GetValue())
    
    def testValueString(self):
        """SetValueString"""
        min = self.testControl.GetMin()
        max = self.testControl.GetMax()
        for i in range(min,max+1):
            si = str(i)
            self.testControl.SetValueString(si)
            self.assertEquals(i, self.testControl.GetValue())
        for j in range(min-100,min):
            sj = str(j)
            self.testControl.SetValueString(sj)
            self.assertEquals(min, self.testControl.GetValue())
        for k in range(max,max+100):
            sk = str(k)
            self.testControl.SetValueString(sk)
            self.assertEquals(max, self.testControl.GetValue())
            
            
def suite():
    suite = unittest.makeSuite(SpinCtrlTest)
    return unittest.TestSuite(suite)
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
