import unittest
import wx

import testColour

"""
This file contains classes and methods for unit testing the API of wx.TextAttr.
        
Methods yet to test:
Combine, GetAlignment, GetFlags, GetLeftIndent, GetLeftSubIndent, GetRightIndent,
GetTabs, HasAlignment, HasFlag, HasLeftIndent, HasRightIndent, HasTabs, Init,
Merge, SetAlignment, SetFlags, SetLeftIndent, SetRightIndent, SetTabs
"""

class TextAttrTest(unittest.TestCase):
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.testControl = wx.TextAttr()
    
    def tearDown(self):
        """__del__"""
        self.app.Destroy()
        del self.testControl
        
    def testBackgroundColour(self):
        """SetBackgroundColour, GetBackgroundColour, HasBackgroundColour"""
        self.assert_(not self.testControl.HasBackgroundColour())
        self.assertEquals(wx.NullColour, self.testControl.GetBackgroundColour())
        for test,colour in testColour.getColourEquivalents():
            self.testControl.SetBackgroundColour(test)
            self.assert_(self.testControl.HasBackgroundColour())
            self.assertEquals(colour, self.testControl.GetBackgroundColour())
    
    def testIsDefault(self):
        """IsDefault"""
        a,b,c = wx.TextAttr(), wx.TextAttr(), wx.TextAttr()
        for ctrl in (a,b,c,self.testControl):
            self.assert_(ctrl.IsDefault())
        a.SetBackgroundColour(wx.Colour(255,0,0))
        b.SetTextColour(wx.Colour(0,0,255))
        c.SetFont(wx.Font(8, wx.FONTFAMILY_ROMAN, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_BOLD))
        for ctrl in (a,b,c):
            self.assert_(not ctrl.IsDefault())
    
    def testConstructor(self):
        """__init__"""
        # a trivial test as yet
        # TODO: exercise constructor more fully
        a = wx.TextAttr()
    
    def testFont(self):
        """SetFont, GetFont, HasFont"""
        self.assert_(not self.testControl.HasFont())
        self.assertEquals(wx.NullFont, self.testControl.GetFont())
        fn = wx.Font(12, wx.FONTFAMILY_ROMAN, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL)
        self.testControl.SetFont(fn)
        self.assert_(self.testControl.HasFont())
        self.assertEquals(fn, self.testControl.GetFont())
    
    def testTextColour(self):
        """SetTextColour, GetTextColour, HasTextColour"""
        self.assert_(not self.testControl.HasTextColour())
        self.assertEquals(wx.NullColour, self.testControl.GetTextColour())
        for test,colour in testColour.getColourEquivalents():
            self.testControl.SetTextColour(test)
            self.assert_(self.testControl.HasTextColour())
            self.assertEquals(colour, self.testControl.GetTextColour())
    

def suite():
    suite = unittest.makeSuite(TextAttrTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
