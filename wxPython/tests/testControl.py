import unittest
import wx

import testWindow
import testItemContainer

"""
This file contains classes and methods for unit testing the API of wx.Control,
as well as a base class for testing subclasses of wx.ControlWithItems (and
a few tests for wx.ControlWithItems itself).

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
    
    # TODO: affirm that this is expected behavior
    def testAllControlsNeedParents(self):
        """
        All instances of wx.Control need to have a parent"""
        class_under_test = type(self.testControl)
        self.assertRaises(wx.PyAssertionError, class_under_test, None)
    
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
    def testLabelText(self):
        """GetLabelText"""
        name = 'Name of Control'
        ctrl = wx.Control(parent=self.frame, name=name)
        self.assertEquals(name, ctrl.GetLabelText())

# -----------------------------------------------------------

class ControlWithItemsTest(unittest.TestCase):
    def setUp(self):
        self.app = wx.PySimpleApp()
    
    def tearDown(self):
        self.app.Destroy()
    
    def testConstructorFails(self):
        self.assertRaises(AttributeError, wx.ControlWithItems)


class ControlWithItemsBase(ControlTest, testItemContainer.ItemContainerBase):
    """Mixing wx.Control with wx.ItemContainer """
    pass

# -----------------------------------------------------------

def suite():
    suite = unittest.makeSuite(ControlTest)
    suite2 = unittest.makeSuite(ControlWithItemsTest)
    return unittest.TestSuite((suite,suite2))
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
