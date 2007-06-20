import unittest
import wx
import sys

import testControl
import testItemContainer

"""
This file contains classes and methods for unit testing the API of wx.ControlWithItems.
"""

BaseControlClass = testControl.ControlTest
BaseItemContainerClass = testItemContainer.ItemContainerBase
if sys.platform.find('win32') != -1:
    BaseControlClass = testControl.ControlWinTest
    BaseItemContainerClass = testItemContainer.ItemContainerWinBase
elif sys.platform.find('linux') != -1:
    BaseClass = testControl.ControlLinuxTest
    BaseItemContainerClass = testItemContainer.ItemContainerLinuxBase
elif sys.platform.find('mac') != -1:
    BaseClass = testControl.ControlMacTest
    BaseItemContainerClass = testItemContainer.ItemContainerMacBase


class ControlWithItemsTest(unittest.TestCase):
    def setUp(self):
        self.app = wx.PySimpleApp()
    
    def tearDown(self):
        self.app.Destroy()
    
    def testConstructorFails(self):
        self.assertRaises(AttributeError, wx.ControlWithItems)

# ControlWithItemsBase doesn't need platform-specific subclasses, because its behavior
# is contained entirely within its parent classes.
class ControlWithItemsBase(BaseControlClass, BaseItemContainerClass):
    """Mixing wx.Control with wx.ItemContainer """
    pass

# -----------------------------------------------------------

class ControlWithItemsWinTest(ControlWithItemsTest):
    pass

class ControlWithItemsLinuxTest(ControlWithItemsTest):
    pass

class ControlWithItemsMacTest(ControlWithItemsTest):
    pass

# -----------------------------------------------------------

def suite():
    testclass = ControlWithItemsTest
    if sys.platform.find('win32') != -1:
        testclass = ControlWithItemsWinTest
    elif sys.platform.find('linux') != -1:
        testclass = ControlWithItemsLinuxTest
    elif sys.platform.find('mac') != -1:
        testclass = ControlWithItemsMacTest
    suite = unittest.makeSuite(testclass)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
