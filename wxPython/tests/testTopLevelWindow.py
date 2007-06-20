import unittest
import wx
import sys

import testWindow

"""
This file contains classes and methods for unit testing the API of 
wx.TopLevelWindow.

Methods yet to test for wx.TopLevelWindow:
__init__, CenterOnScreen, CentreOnScreen, EnableCloseButton, GetDefaultItem, GetIcon,
GetTitle, GetTmpDefaultItem, Iconize, IsActive, IsAlwaysMaximized, IsFullScreen,
IsIconized, IsMaximized, MacGetMetalAppearance, MacSetMetalAppearance, Maximize,
RequestUserAttention, Restore, SetDefaultItem, SetIcon, SetIcons, SetShape, SetTitle,
SetTmpDefaultItem, ShowFullScreen
"""

BaseClass = testWindow.WindowTest
if sys.platform.find('win32') != -1:
    BaseClass = testWindow.WindowWinTest
elif sys.platform.find('linux') != -1:
    BaseClass = testWindow.WindowLinuxTest
elif sys.platform.find('mac') != -1:
    BaseClass = testWindow.WindowMacTest

class TopLevelWindowBase(BaseClass):
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.testControl = wx.Control(parent=self.frame, id=wx.ID_ANY)
    
    def tearDown(self):
        self.frame.Destroy()
        self.app.Destroy()
        
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
