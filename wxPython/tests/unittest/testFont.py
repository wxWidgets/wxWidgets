import unittest
import wx

import wxtest

"""
This file contains classes and methods for unit testing the API of wx.Font

NOTE: The documentation for font currently omits a large number of constants.
    I had to look through the wx module itself to find them, and I'll copy
    that information here.
    
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

Methods yet to test:
__init__, __del__, __eq__, __ne__, __nonzero__, GetDefaultEncoding, GetEncoding, GetFaceName
GetFamily, GetFamilyString, GetNativeFontInfo, GetNativeFontInfoDesc, GetNativeFontInfoUserDesc,
GetNoAntiAliasing, GetPixelSize, GetPointSize, GetStyle, GetStyleString, GetUnderlined,
GetWeight, GetWeightString, IsFixedWidth, IsUsingSizeInPixels, SetDefaultEncoding,
SetEncoding, SetFaceName, SetFamily, SetNativeFontInfo, SetNativeFontInfoFromString,
SetNativeFontInfoUserDesc, SetNoAntiAliasing, SetPixelSize, SetPointSize, SetStyle, SetUnderlined
SetWeight
"""

# TODO:
#   Verify completeness of this list. Flesh it out, perhaps automate the font creation process,
#   perhaps return a generator.
def getFontData():
    return (
                wx.Font(1, wx.FONTFAMILY_DECORATIVE, wx.FONTSTYLE_SLANT, wx.FONTWEIGHT_NORMAL),
                wx.Font(8, wx.FONTFAMILY_ROMAN, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_BOLD),
                wx.Font(10, wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_SLANT, wx.FONTWEIGHT_NORMAL),
                wx.Font(12, wx.FONTFAMILY_MODERN, wx.FONTSTYLE_ITALIC, wx.FONTWEIGHT_LIGHT),
                wx.Font(16, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_BOLD),
                wx.Font(18, wx.FONTFAMILY_SCRIPT, wx.FONTSTYLE_SLANT, wx.FONTWEIGHT_LIGHT),
                wx.Font(24, wx.FONTFAMILY_SWISS, wx.FONTSTYLE_ITALIC, wx.FONTWEIGHT_NORMAL),
                wx.Font(32, wx.FONTFAMILY_DECORATIVE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_BOLD),
                wx.Font(36, wx.FONTFAMILY_UNKNOWN, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_LIGHT),
                wx.Font(48, wx.FONTFAMILY_MODERN, wx.FONTSTYLE_SLANT, wx.FONTWEIGHT_BOLD),
                wx.Font(72, wx.FONTFAMILY_SWISS, wx.FONTSTYLE_ITALIC, wx.FONTWEIGHT_NORMAL),
                wx.Font(96, wx.FONTFAMILY_MAX, wx.FONTSTYLE_SLANT, wx.FONTWEIGHT_BOLD),
                wx.Font(128, wx.FONTFAMILY_SCRIPT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_LIGHT),
                wx.Font(256, wx.FONTFAMILY_MAX, wx.FONTSTYLE_ITALIC, wx.FONTWEIGHT_BOLD)
        )

# -----------------------------------------------------------

class FontTest(unittest.TestCase):
    def setUp(self):
        self.app = wx.PySimpleApp()
    
    def tearDown(self):
        pass
    
    def testFontstyle(self):
        # wx.FONTSTYLE_MAX dies (on Windows)
        if wxtest.PlatformIsWindows():
            self.assertRaises(wx.PyAssertionError, wx.Font, 12, wx.FONTFAMILY_DEFAULT,
                                            wx.FONTSTYLE_MAX, wx.FONTWEIGHT_NORMAL)
        else:
            wx.Font(12, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_MAX, wx.FONTWEIGHT_NORMAL)
                                            
    def testFontweight(self):
        # wx.FONTWEIGHT_MAX dies (on Windows)
        if wxtest.PlatformIsWindows():
            self.assertRaises(wx.PyAssertionError, wx.Font, 12, wx.FONTFAMILY_DEFAULT,
                                            wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_MAX)
        else:
            wx.Font(12, wx.FONTFAMILY_DEFAULT, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_MAX)
                                            
    def testOk(self):
        """IsOk, Ok"""
        for font in getFontData():
            self.assert_(font.IsOk())
            self.assert_(font.Ok())
        # HACK: to generate an invalid wx.Font instance
        # NOTE: cannot access font directly without crashing the interpreter
        attr = wx.VisualAttributes()
        self.assert_(not attr.font.Ok())
        self.assert_(not attr.font.IsOk())
        

def suite():
    suite = unittest.makeSuite(FontTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
