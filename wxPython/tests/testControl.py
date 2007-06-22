import unittest
import wx

import wxtest
import testWindow

"""
This file contains classes and methods for unit testing the API of wx.Control.

Methods yet to test for wx.Control:
__init__, Command, Create, GetAlignment
"""

class ControlTest(testWindow.WindowTest):
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.testControl = wx.Control(parent=self.frame, id=wx.ID_ANY)
    
    def tearDown(self):
        self.frame.Destroy()
        self.app.Destroy()
    
    # TODO: is this expected behavior? Why does it only happen
    # on Windows? It's a wrapped C++ assertion failure.
    def testAllControlsNeedParents(self):
        """
        All instances of wx.Control need to have a parent
        (at least on Windows)"""
        class_under_test = type(self.testControl)
        if wxtest.PlatformIsWindows():
            self.assertRaises(wx.PyAssertionError, class_under_test, None)
        else:
            # if not windows, doesn't raise exception
            class_under_test(None)
    
    def testDefaultAttributes(self):
        """GetClassDefaultAttributes"""
        attrs = wx.Control.GetClassDefaultAttributes()
        self.assert_(isinstance(attrs, wx.VisualAttributes))
        self.assert_(attrs.colBg.IsOk())
        self.assert_(attrs.colFg.IsOk())
        self.assert_(attrs.font.IsOk())

    # TODO: does this only work on Windows? if so, why?
    def testLabelText(self):
        """GetLabelText"""
        name = 'Name of Control'
        class_under_test = type(self.testControl)
        ctrl = wx.Control(parent=self.frame, name=name)
        self.assertEquals(name, ctrl.GetLabelText())
    

def suite():
    suite = unittest.makeSuite(ControlTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
