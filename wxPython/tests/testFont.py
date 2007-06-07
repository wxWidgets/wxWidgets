import unittest
import wx

"""
This file contains classes and methods for unit testing the API of wx.Font

NOTE: The documentation for font currently omits a large number of constants.
    I had to look through the wx module itself to find them, and I'll copy
    that information here.

TODO: fix the fact that wx.FONTSTYLE_MAX and wx.FONTWEIGHT_MAX aren't
    implemented on any modern platforms
    
CONSTANTS:
    wx.FONTFAMILY_xxx constants:
        wx.FONTFAMILY_DECORATIVE
        wx.FONTFAMILY_DEFAULT
        wx.FONTFAMILY_MAX       # aliased to FONTFAMILY_UNKNOWN
        wx.FONTFAMILY_MODERN
        wx.FONTFAMILY_ROMAN
        wx.FONTFAMILY_SCRIPT
        wx.FONTFAMILY_SWISS
        wx.FONTFAMILY_TELETYPE
        wx.FONTFAMILY_UNKNOWN

    wx.FONTSTYLE_xxx constants:
        wx.FONTSTYLE_ITALIC
        wx.FONTSTYLE_MAX
        wx.FONTSTYLE_NORMAL
        wx.FONTSTYLE_SLANT
        
    wx.FONTWEIGHT_xxx constants:
        wx.FONTWEIGHT_BOLD
        wx.FONTWEIGHT_LIGHT
        wx.FONTWEIGHT_MAX
        wx.FONTWEIGHT_NORMAL
"""

class FontTest(unittest.TestCase):
    def setUp(self):
        self.app = wx.PySimpleApp()
    
    def tearDown(self):
        pass
    
    
    def testFontstyle(self):
        # wx.FONTSTYLE_MAX dies
        self.assertRaises(wx.PyAssertionError, wx.Font, 12, wx.FONTFAMILY_DEFAULT,
                                            wx.FONTSTYLE_MAX, wx.FONTWEIGHT_NORMAL)
                                            
        
    def testFontweight(self):
        # wx.FONTWEIGHT_MAX dies
        self.assertRaises(wx.PyAssertionError, wx.Font, 12, wx.FONTFAMILY_DEFAULT,
                                            wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_MAX)


def suite():
    suite = unittest.makeSuite(FontTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
