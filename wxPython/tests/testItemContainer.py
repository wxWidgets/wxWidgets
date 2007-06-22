import unittest
import wx

"""
This file contains classes and methods for unit testing the API of 
wx.ItemContainer.  Since wx.ItemContainer is uninstantiable, the tests
for subclasses are implemented with a "test base-class" of sorts.
        
Methods yet to test:
__init__, Append, AppendItems, Clear, Delete, FindString, GetClientData, GetCount,
GetItems, GetSelection, GetString, GetStrings, GetStringSelection, Insert,
IsEmpty, Select, SetClientData, SetItems, SetSelection, SetString, SetStringSelection
"""

class ItemContainerTest(unittest.TestCase):
    def setUp(self):
        self.app = wx.PySimpleApp()
    
    def tearDown(self):
        self.app.Destroy()
    
    def testConstructorFails(self):
        self.assertRaises(AttributeError, wx.ItemContainer)
        
# -----------------------------------------------------------
    
class ItemContainerBase(unittest.TestCase):
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.testControl = None
    
    def tearDown(self):
        self.app.Destroy()
        self.frame.Destroy()
        

def suite():
    suite = unittest.makeSuite(ItemContainerTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
