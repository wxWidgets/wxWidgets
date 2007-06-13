import unittest
import wx

import testControl

"""
This file contains classes and methods for unit testing the API of wx.TextCtrl
        
Methods yet to test:
__init__, AppendText, CanCopy, CanCut, CanPaste, CanRedo, CanUndo, ChangeValue,
Create, DiscardEdits, EmulateKeyPress, GetClassDefaultAttributes,
GetDefaultStyle, GetInsertionPoint, GetLastPosition, GetLineLength, GetLineText,
GetNumberOfLines, GetRange, GetSelection, GetString, GetStringSelection,
GetStyle, HitTest, HitTestPos, IsEditable, IsModified,
IsMultiLine, IsSingleLine, LoadFile,  MacCheckSpelling, MarkDirty, PositionToXY,
Redo, Remove, Replace, SaveFile, SelectAll, SendTextUpdatedEvent, SetDefaultStyle,
SetEditable, SetInsertionPoint, SetInsertionPointEnd, SetMaxLength, SetModified,
SetSelection, SetStyle, ShowPosition, Undo, write, WriteText, XYToPosition
"""

class TextCtrlTest(testControl.ControlTest):
    def __init__(self, arg):
        # superclass setup
        super(TextCtrlTest, self).__init__(arg)
        # TextCtrlTest setup
        
    #####################
    ## Fixture Methods ##
    #####################
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.testControl = wx.TextCtrl(parent=self.frame, id=wx.ID_ANY, value="")
        self.children = []
        self.children_ids = []
        self.children_names = []
    
    def tearDown(self):
        self.app.Destroy()
        self.frame.Destroy()
        
    ##################
    ## Test Methods ##
    ##################
    
    def testClear(self):
        """Clear"""
        # TODO: ensure wx.wxEVT_COMMAND_TEXT_UPDATED event generated
        self.testControl.SetValue("Text text text")
        self.assert_(not self.testControl.IsEmpty())
        self.testControl.Clear()
        self.assert_(self.testControl.IsEmpty())
    
    def testIsEmpty(self):
        """IsEmpty"""
        self.assert_(self.testControl.IsEmpty())
        self.testControl.SetValue("Not Empty")
        self.assert_(not self.testControl.IsEmpty())
        self.testControl.SetValue("")
        self.assert_(self.testControl.IsEmpty())
    
    def testValue(self):
        """SetValue, GetValue"""
        val = "Hello, World!\nLorem Ipsum!"
        self.assertEquals("", self.testControl.GetValue())
        self.testControl.SetValue(val)
        self.assertEquals(val, self.testControl.GetValue())
    
    # NOTE: Uncomment the last line to observe some weird behavior
    def testCopyCutPaste(self):
        """Copy, Cut, Paste"""
        txt = "Yet Another TextControl"
        more = "Here is some more text!"
        otherControl = wx.TextCtrl(self.frame, id=wx.ID_ANY, value=txt)
        # sanity checks
        self.assert_(self.testControl.IsEmpty())
        self.assertEquals(txt, otherControl.GetValue())
        # copy/paste
        otherControl.Copy()
        self.testControl.Paste()
        self.assertEquals(txt, otherControl.GetValue())
        #self.assertEquals(txt, self.testControl.GetValue())
        

def suite():
    suite = unittest.makeSuite(TextCtrlTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
