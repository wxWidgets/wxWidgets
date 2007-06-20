import unittest
import wx
import sys

import testTopLevelWindow

"""
This file contains classes and methods for unit testing the API of wx.Frame.
        
Methods yet to test:
__init__, Command, Create, CreateStatusBar, CreateToolBar, DoGiveHelp, DoMenuUpdates,
GetClassDefaultAttributes, GetMenuBar, GetStatusBar, GetStatusBarPane, GetToolBar,
PopStatusText, ProcessCommand, PushStatusText, SendSizeEvent, SetMenuBar, SetStatusBar,
SetStatusBarPane, SetStatusText, SetStatusWidths, SetToolBar
"""

class FrameTest(testTopLevelWindow.TopLevelWindowBase):
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.testControl = wx.Frame(parent=self.frame, id=wx.ID_ANY)

# -----------------------------------------------------------

class FrameWinTest(FrameTest):
    # Overridden tests:
    # TODO: Why do these wx.Window tests fail on wx.Frame
    def testCenterFails(self):
        self.testControl.Center(wx.CENTER_ON_SCREEN)
    
    # it looks as though wx.Frame has a minimum size of 123,34.
    def testSize(self):
        self.testControl.SetSize(wx.Size(1,1))
        self.assertEquals(wx.Size(123,34),self.testControl.GetSize())
    
    # and the same goes for GetRect
    def testRect(self):
        self.testControl.SetRect(wx.Rect(0,0,0,0))
        self.assertEquals(wx.Rect(0,0,123,34),self.testControl.GetRect())

class FrameLinuxTest(FrameTest):
    pass

class FrameMacTest(FrameTest):
    pass

# -----------------------------------------------------------

def suite():
    testclass = FrameTest
    if sys.platform.find('win32') != -1:
        testclass = FrameWinTest
    elif sys.platform.find('linux') != -1:
        testclass = FrameLinuxTest
    elif sys.platform.find('mac') != -1:
        testclass = FrameMacTest
    suite = unittest.makeSuite(FrameTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
