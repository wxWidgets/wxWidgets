import unittest
import wx

import testControl

"""
This file contains classes and methods for unit testing the API of wx.Gauge

TODO:
    Fill in the docs!

NOTE:
    SetBezelFace, SetShadowWidth don't seem to work (on Windows at least)

Methods yet to test:
__init__, Create, GetClassDefaultAttributes, Pulse
"""

class GaugeTest(testControl.ControlTest):
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = wx.Frame(parent=None)
        self.testControl = wx.Gauge(parent=self.frame)
    
    def testBezelFace(self):
        """SetBezelFace, GetBezelFace"""
        for i in range(self.testControl.GetRange()):
            self.testControl.SetBezelFace(i)
            self.assertEquals(i, self.testControl.GetBezelFace())
    
    def testIsVertical(self):
        """IsVertical"""
        vert  = wx.Gauge(self.frame, style=wx.GA_VERTICAL)
        horiz = wx.Gauge(self.frame, style=wx.GA_HORIZONTAL)
        self.assert_(not self.testControl.IsVertical()) # default
        self.assert_(vert.IsVertical())
        self.assert_(not horiz.IsVertical())
    
    def testRange(self):
        """SetRange, GetRange"""
        for i in range(1000):
            self.testControl.SetRange(i)
            self.assertEquals(i, self.testControl.GetRange())
    
    def testShadowWidth(self):
        """SetShadowWidth, GetShadowWidth"""
        for i in range(self.testControl.GetRange()):
            self.testControl.SetShadowWidth(i)
            self.assertEquals(i, self.testControl.GetShadowWidth())
    
    def testValue(self):
        """SetValue, GetValue"""
        for i in range(self.testControl.GetRange()):
            self.testControl.SetValue(i)
            self.assertEquals(i, self.testControl.GetValue())

def suite():
    suite = unittest.makeSuite(GaugeTest)
    return unittest.TestSuite(suite)
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
