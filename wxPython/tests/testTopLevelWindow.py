import unittest
import wx

import testWindow

"""
This file contains classes and methods for unit testing the API of 
wx.TopLevelWindow.

Methods yet to test for wx.TopLevelWindow:
__init__, CenterOnScreen, CentreOnScreen, EnableCloseButton, GetDefaultItem, GetIcon,
GetTmpDefaultItem, Iconize, IsActive, IsAlwaysMaximized, IsFullScreen, IsIconized,
MacGetMetalAppearance, MacSetMetalAppearance, RequestUserAttention, Restore, SetDefaultItem,
SetIcon, SetIcons, SetShape, SetTmpDefaultItem, ShowFullScreen
"""

class TopLevelWindowBase(testWindow.WindowTest):
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.testControl = None
    
    def tearDown(self):
        self.frame.Destroy()
        self.app.Destroy()
    
    def testMaximize(self):
        """Maximize, IsMaximized"""
        self.testControl.Maximize()
        self.assert_(self.testControl.IsMaximized())
        self.testControl.Maximize(False)
        self.assert_(not self.testControl.IsMaximized())
        self.testControl.Maximize(True)
        self.assert_(self.testControl.IsMaximized())
    
    def testTitle(self):
        """SetTitle, GetTitle"""
        class_under_test = type(self.testControl)
        title = "Hello, World!"
        t = class_under_test(parent=self.frame, id=wx.ID_ANY, title=title)
        self.assertEquals(title, t.GetTitle())
        self.testControl.SetTitle(title)
        self.assertEquals(title, self.testControl.GetTitle())
        title2 = "The quick brown fox jumps over the lazy dog"
        self.testControl.SetTitle(title2)
        self.assertEquals(title2, self.testControl.GetTitle())
        
    def testTopLevel(self):
        """IsTopLevel"""
        self.assert_(self.testControl.IsTopLevel())

# -----------------------------------------------------------

class TopLevelWindowTest(unittest.TestCase):
    def setUp(self):
        self.app = wx.PySimpleApp()
    
    def tearDown(self):
        self.app.Destroy()
    
    def testConstructorFails(self):
        self.assertRaises(AttributeError, wx.TopLevelWindow)

# -----------------------------------------------------------

def suite():
    suite = unittest.makeSuite(TopLevelWindowTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
