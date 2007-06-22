import unittest
import wx

import wxtest
import testTopLevelWindow

"""
This file contains classes and methods for unit testing the API of wx.Frame.

Methods yet to test:
__init__, Command, Create, DoGiveHelp, DoMenuUpdates, GetClassDefaultAttributes,
GetStatusBarPane, ProcessCommand, SendSizeEvent, SetStatusBarPane, SetStatusWidths
"""

class FrameTest(testTopLevelWindow.TopLevelWindowBase):
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.testControl = wx.Frame(parent=self.frame, id=wx.ID_ANY)
    
    def testMenuBar(self):
        """SetMenuBar, GetMenuBar"""
        mb = wx.MenuBar()
        self.testControl.SetMenuBar(mb)
        self.assertEquals(mb, self.testControl.GetMenuBar())
        mb2 = wx.MenuBar()
        self.assertNotEquals(mb, mb2)
        self.testControl.SetMenuBar(mb2)
        self.assertEquals(mb2, self.testControl.GetMenuBar())
        self.assertNotEquals(mb, self.testControl.GetMenuBar())
    
    # see testSize method, below; probably the same issue
    def testRect(self):
        if wxtest.PlatformIsWindows():
            self.testControl.SetRect(wx.Rect(0,0,0,0))
            self.assertEquals(wx.Rect(0,0,123,34),self.testControl.GetRect())
        else:
            super(FrameTest,self).testSize(self)
    
    # wx.Frame's size has a minimum on Windows
    def testSize(self):
        if wxtest.PlatformIsWindows():
            self.testControl.SetSize(wx.Size(1,1))
            self.assertEquals(wx.Size(123,34),self.testControl.GetSize())
        else:
            super(FrameTest,self).testSize(self)
    
    def testStatusBar(self):
        """SetStatusBar, GetStatusBar"""
        sb1 = wx.StatusBar(parent=self.testControl, id=wx.ID_ANY)
        sb2 = wx.StatusBar(parent=self.testControl, id=wx.ID_ANY)
        self.assertNotEquals(sb1,sb2) # sanity check
        self.testControl.SetStatusBar(sb1)
        self.assertEquals(sb1, self.testControl.GetStatusBar())
        self.testControl.SetStatusBar(sb2)
        self.assertEquals(sb2, self.testControl.GetStatusBar())
        self.assertNotEquals(sb1, self.testControl.GetStatusBar())
        
    def testStatusBarCreation(self):
        """CreateStatusBar"""
        self.assertRaises(wx.PyAssertionError, self.testControl.PushStatusText, 'text')
        sb = self.testControl.CreateStatusBar()
        self.testControl.PushStatusText('text') # test that it doesn't blow up
        self.assert_(isinstance(self.testControl.GetStatusBar(), wx.StatusBar))
        self.assertEquals(sb, self.testControl.GetStatusBar())
    
    def testStatusBarPushPop(self):
        """PushStatusText, PopStatusText"""
        sb = self.testControl.CreateStatusBar()
        txts = ('one','two','three','four','five')
        for txt in txts:
            self.testControl.PushStatusText(txt)
        for t in txts[::-1]:
            self.assertEquals(t, sb.GetStatusText())
            self.testControl.PopStatusText()
    
    def testStatusBarText(self):
        """SetStatusText"""
        sb = self.testControl.CreateStatusBar()
        txts = ('lorem','ipsum','dolor','sit','amet')
        for txt in txts:
            self.testControl.SetStatusText(txt)
            self.assertEquals(txt, sb.GetStatusText())
    
    def testToolBar(self):
        """CreateToolBar"""
        tb = self.testControl.CreateToolBar()
        self.assertEquals(tb, self.testControl.GetToolBar())
    
    def testToolBarCreation(self):
        """SetToolBar, GetToolBar"""
        tb1 = wx.ToolBar(self.testControl)
        tb2 = wx.ToolBar(self.testControl)
        self.assertNotEquals(tb1,tb2) # sanity check
        self.testControl.SetToolBar(tb1)
        self.assertEquals(tb1, self.testControl.GetToolBar())
        self.testControl.SetToolBar(tb2)
        self.assertEquals(tb2, self.testControl.GetToolBar())
        self.assertNotEquals(tb1, self.testControl.GetToolBar())
    

def suite():
    suite = unittest.makeSuite(FrameTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
