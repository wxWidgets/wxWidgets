import unittest
import wx

import testControl

"""
This file contains classes and methods for unit testing the API of wx.Slider

TODO:
    should it be legal for min value to be greater than max value?

TODO:
    Fill in the docs!

Methods yet to test:
__init__, ClearSel, ClearTicks,Create, GetClassDefaultAttributes, GetSelEnd,
GetSelStart, GetThumbLength, GetTickFreq, SetSelection, SetThumbLength,
SetTick, SetTickFreq
"""

class SliderTest(testControl.ControlTest):
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.testControl = wx.Slider(parent=self.frame, id=wx.ID_ANY)
        
    def testGetRange(self):
        """GetRange
        'wxPython in Action' pg.207 says wx.Sliders have GetRange"""
        self.assert_(hasattr(self.testControl,'GetRange'))
    
    def testLineSize(self):
        """SetLineSize, GetLineSize"""
        for i in (-1,1,5,10,100,1000):
            self.testControl.SetLineSize(i)
            self.assertEquals(i, self.testControl.GetLineSize())
        
    def testMinMax(self):
        """SetMin, SetMax, GetMin, GetMax"""
        for min,max in ((1,10),(100,10000),(-20,-10)):
            self.testControl.SetMin(min)
            self.testControl.SetMax(max)
            self.assertEquals(min, self.testControl.GetMin())
            self.assertEquals(max, self.testControl.GetMax())
    
    def testPageSize(self):
        """SetPageSize, GetPageSize"""
        for i in (-1,1,10,100,1000):
            self.testControl.SetPageSize(i)
            self.assertEquals(i, self.testControl.GetPageSize())
    
    def testSetRange(self):
        """SetRange"""
        self.testControl.SetRange(999,1001)
        self.assertEquals(999, self.testControl.GetMin())
        self.assertEquals(1001, self.testControl.GetMax())
    
    def testTickFreq(self):
        """SetTickFreq, GetTickFreq"""
        for i in (-1,1,5,10,100,1000):
            self.testControl.SetTickFreq(i)
            self.assertEquals(i, self.testControl.GetTickFreq())
    
    def testValue(self):
        """SetValue, GetValue"""
        min = self.testControl.GetMin()
        max = self.testControl.GetMax()
        for i in range(min,max+1):
            self.testControl.SetValue(i)
            self.assertEquals(i, self.testControl.GetValue())
        for j in range(min-100,min+1):
            self.testControl.SetValue(j)
            self.assertEquals(min, self.testControl.GetValue())
        for k in range(max,max+100):
            self.testControl.SetValue(k)
            self.assertEquals(max, self.testControl.GetValue())
        

def suite():
    suite = unittest.makeSuite(SliderTest)
    return unittest.TestSuite(suite)
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
