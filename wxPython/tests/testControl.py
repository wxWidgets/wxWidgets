import unittest
import wx
import sys

import testWindow

"""
This file contains classes and methods for unit testing the API of wx.Control.

Methods yet to test for wx.Control:
__init__, Command, Create, GetAlignment
"""

BaseClass = testWindow.WindowTest
if sys.platform.find('win32') != -1:
    BaseClass = testWindow.WindowWinTest
elif sys.platform.find('linux') != -1:
    BaseClass = testWindow.WindowLinuxTest
elif sys.platform.find('mac') != -1:
    BaseClass = testWindow.WindowMacTest

class ControlTest(BaseClass):
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.testControl = wx.Control(parent=self.frame, id=wx.ID_ANY)
    
    def tearDown(self):
        self.frame.Destroy()
        self.app.Destroy()
    
    def testDefaultAttributes(self):
        """GetClassDefaultAttributes"""
        attrs = wx.Control.GetClassDefaultAttributes()
        self.assert_(isinstance(attrs, wx.VisualAttributes))
        self.assert_(attrs.colBg.IsOk())
        self.assert_(attrs.colFg.IsOk())
        self.assert_(attrs.font.IsOk())

    # NOTE: only makes sense when called in ControlTest.
    #   otherwise the results are padded with meaningless tests.
    #   how to generalize this for inheritance?
    # TODO: does this only work on Windows? if so, why?
    def testLabelText(self):
        """GetLabelText"""
        name = 'Name of Control'
        ctrl = wx.Control(parent=self.frame, name=name)
        self.assertEquals(name, ctrl.GetLabelText())

# -----------------------------------------------------------

class ControlWinTest(ControlTest):
    # TODO: is this expected behavior? Why does it only happen
    # on Windows? It's a wrapped C++ assertion failure.
    def testAllControlsNeedParents(self):
        """
        All instances of wx.Control need to have a parent
        (at least on Windows)"""
        class_under_test = type(self.testControl)
        self.assertRaises(wx.PyAssertionError, class_under_test, None)

class ControlLinuxTest(ControlTest):
    def testLabelText(self):
        pass

class ControlMacTest(ControlTest):
    pass

# -----------------------------------------------------------

def suite():
    testclass = ControlTest
    if sys.platform.find('win32') != -1:
        testclass = ControlWinTest
    elif sys.platform.find('linux') != -1:
        testclass = ControlLinuxTest
    elif sys.platform.find('mac') != -1:
        testclass = ControlMacTest
    suite = unittest.makeSuite(testclass)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
