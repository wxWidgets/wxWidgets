import unittest
import wx

import testTopLevelWindow

"""
This file contains classes and methods for unit testing the API of wx.Dialog.

Methods yet to test:
__init__, Create, CreateButtonSizer, CreateSeparatedButtonSizer, CreateStdDialogButtonSizer,
CreateTextSizer, EndModal, GetAffirmativeId, GetClassDefaultAttributes, GetEscapeId,
GetReturnCode, IsModal, SetAffirmativeId, SetEscapeId, SetReturnCode, ShowModal
"""

class DialogTest(testTopLevelWindow.TopLevelWindowBase):
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.testControl = wx.Dialog(parent=self.frame, id=wx.ID_ANY)
    
    
def suite():
    suite = unittest.makeSuite(DialogTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
