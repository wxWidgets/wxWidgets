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
LoadFile,  MacCheckSpelling, MarkDirty, PositionToXY,
Redo, Remove, Replace, SaveFile, SelectAll, SendTextUpdatedEvent, SetDefaultStyle,
SetEditable, SetInsertionPoint, SetInsertionPointEnd, SetMaxLength, SetModified,
SetSelection, SetStyle, ShowPosition, Undo, write, WriteText, XYToPosition
"""

class TextCtrlTest(testControl.ControlTest):
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.testControl = wx.TextCtrl(parent=self.frame, id=wx.ID_ANY, value="")
    
    def tearDown(self):
        self.app.Destroy()
        self.frame.Destroy()
    
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
    
    def testCopyCutPaste(self):
        """Copy, Cut, Paste"""
        txt1 = "Yet Another TextControl"
        txt2 = "Here is some more text!"
        txt3 = "And here is the third."
        otherControl = wx.TextCtrl(self.frame, id=wx.ID_ANY, value=txt1)
        # sanity checks
        self.assert_(self.testControl.IsEmpty())
        self.assertEquals(txt1, otherControl.GetValue())
        # copy/paste
        otherControl.SelectAll() # need to select in order to copy!
        otherControl.Copy()
        self.testControl.Paste()
        self.assertEquals(txt1, otherControl.GetValue())
        self.assertEquals(txt1, self.testControl.GetValue())
        # cut/paste
        otherControl.SetValue(txt2)
        otherControl.SelectAll()
        otherControl.Cut()
        self.testControl.Paste()
        self.assert_(otherControl.IsEmpty())
        self.assertEquals(txt1+txt2, self.testControl.GetValue())
        self.testControl.SelectAll()
        self.testControl.Paste()
        self.assertEquals(txt2, self.testControl.GetValue())
        # you can't copy what isn't selected...
        otherControl.SetValue(txt3)
        self.testControl.Clear()
        otherControl.Copy()
        self.testControl.Paste()
        self.assertNotEquals(txt3, self.testControl.GetValue())
        self.assertEquals(txt2, self.testControl.GetValue())
        
    
    # TODO: what should IsMultiLine return True?
    def testSingleMultiLine(self):
        """IsMultiLine, IsSingleLine"""
        self.testControl.SetValue("this is a single line")
        self.assert_(self.testControl.IsSingleLine())
        self.assert_(not self.testControl.IsMultiLine())
        # this doesn't do it...
        #self.testControl.SetValue("this\nis\na\nmulti\nline\n")
        #self.assert_(self.testControl.IsMultiLine())
        #self.assert_(not self.testControl.IsSingleLine())
        

def suite():
    suite = unittest.makeSuite(TextCtrlTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
