import unittest
import wx

import wxtest
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
    
    # Overridden tests:
    # TODO: Why do these wx.Window tests fail on wx.Frame?
    def testCenterFails(self):
        if wxtest.PlatformIsWindows():
            self.testControl.Center(wx.CENTER_ON_SCREEN)
        else:
            super(FrameTest,self).testCenterFails(self)
            
    # it looks as though wx.Frame has a minimum size of 123,34 (on Windows)
    def testSize(self):
        if wxtest.PlatformIsWindows():
            self.testControl.SetSize(wx.Size(1,1))
            self.assertEquals(wx.Size(123,34),self.testControl.GetSize())
        else:
            super(FrameTest,self).testSize(self)
    
    # and the same goes for GetRect(again, on Windows)
    def testRect(self):
        if wxtest.PlatformIsWindows():
            self.testControl.SetRect(wx.Rect(0,0,0,0))
            self.assertEquals(wx.Rect(0,0,123,34),self.testControl.GetRect())
        else:
            super(FrameTest,self).testSize(self)
            

def suite():
    suite = unittest.makeSuite(FrameTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
