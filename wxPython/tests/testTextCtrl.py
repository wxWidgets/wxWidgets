import unittest
import wx

import testControl

"""
This file contains classes and methods for unit testing the API of wx.TextCtrl
        
Methods yet to test:
__init__, CanCopy, CanCut, CanPaste, CanRedo, CanUndo, ChangeValue, Create, 
DiscardEdits, EmulateKeyPress, GetClassDefaultAttributes, GetDefaultStyle,
GetLastPosition, GetLineLength, GetLineText, GetNumberOfLines, GetString,
GetStyle, HitTest, HitTestPos, IsEditable, IsModified, LoadFile, MacCheckSpelling,
MarkDirty, PositionToXY, Redo, SaveFile, SelectAll, SendTextUpdatedEvent,
SetDefaultStyle, SetEditable, SetMaxLength, SetModified, SetStyle, ShowPosition,
Undo, write, XYToPosition
"""

class TextCtrlTest(testControl.ControlTest):
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.testControl = wx.TextCtrl(parent=self.frame, id=wx.ID_ANY, value="")
    
    def tearDown(self):
        self.app.Destroy()
        self.frame.Destroy()
    
    # TODO: break up this method into smaller tests
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
    
    def testIsEmpty(self):
        """IsEmpty"""
        self.assert_(self.testControl.IsEmpty())
        self.testControl.SetValue("Not Empty")
        self.assert_(not self.testControl.IsEmpty())
        self.testControl.SetValue("")
        self.assert_(self.testControl.IsEmpty())
        
    # how can a wx.TextCtrl be both single and multi line simultaneously?
    def testMultiLine(self):
        """IsMultiLine"""
        multi = wx.TextCtrl(parent=self.frame, id=wx.ID_ANY,
                                    style=wx.TE_MULTILINE)
        self.assert_(multi.IsMultiLine())
        self.assert_(not self.testControl.IsSingleLine())
        
    def testSingleLine(self):
        """IsSingleLine"""
        self.assert_(self.testControl.IsSingleLine())
        self.assert_(not self.testControl.IsMultiLine())
    
    # Text manipulation methods of wx.TextCtrl (Table 7.4 in "wxPython in Action")
    # AppendText, Clear, EmulateKeyPress, GetInsertionPoint, SetInsertionPoint,
    # SetInsertionPointEnd, GetRange, GetSelection, GetStringSelection, SetSelection,
    # GetValue, SetValue, Remove, Replace, WriteText
    
    def testAppendText(self):
        """AppendText"""
        txt1 = "The quick brown fox"
        txt2 = " jumps over the lazy dog."
        txt = txt1+txt2
        self.testControl.SetValue(txt1)
        self.testControl.AppendText(txt2)
        self.assertEquals(txt, self.testControl.GetValue())
        self.assertEquals(len(txt), self.testControl.GetInsertionPoint())
    
    def testClear(self):
        """Clear"""
        # TODO: ensure wx.wxEVT_COMMAND_TEXT_UPDATED event generated
        self.testControl.SetValue("Text text text")
        self.assert_(not self.testControl.IsEmpty())
        self.testControl.Clear()
        self.assert_(self.testControl.IsEmpty())
    
    def testGetRange(self):
        """GetRange"""
        txt = "The quick brown fox jumps over the lazy dog."
        self.testControl.SetValue(txt)
        for i,j in ((0,2),(1,9),(7,len(txt))):
            self.assert_(txt[i:j], self.testControl.GetRange(i,j))
        
    def testInsertionPoint(self):
        """SetInsertionPoint, GetInsertionPoint"""
        txt = "The quick brown fox jumps over the lazy dog."
        self.testControl.SetValue(txt)
        # Is 1 the lowest possible value for GetInsertionPoint()?
        self.testControl.SetInsertionPoint(0)
        self.assert_(0, self.testControl.GetInsertionPoint())
        self.testControl.SetInsertionPoint(10)
        self.assert_(10, self.testControl.GetInsertionPoint())
        self.testControl.SetInsertionPoint(999)
        self.assert_(len(txt), self.testControl.GetInsertionPoint())
    
    def testInsertionPointEnd(self):
        """SetInsertionPointEnd"""
        txt = "The quick brown fox jumps over the lazy dog."
        self.testControl.SetValue(txt)
        self.testControl.SetInsertionPointEnd()
        self.assert_(len(txt), self.testControl.GetInsertionPoint())
    
    def testRemove(self):
        """Remove"""
        txt = "The quick brown fox jumps over the lazy dog."
        self.testControl.SetValue(txt)
        self.testControl.Remove(0,len(txt))
        self.assert_(self.testControl.IsEmpty())
        for i,j in ((0,2),(2,10),(7,len(txt))):
            self.testControl.SetValue(txt)
            removed = txt[:i]+txt[j:]
            self.testControl.Remove(i,j)
            self.assertEquals(removed, self.testControl.GetValue())
    
    def testReplace(self):
        """Replace"""
        txt = "The quick brown fox jumps over the lazy dog."
        rep = "asdf jkl;"
        for i,j in ((0,2),(2,10),(7,len(txt))):
            self.testControl.SetValue(txt)
            replaced = txt[:i]+rep+txt[j:]
            self.testControl.Replace(i,j,rep)
            self.assertEquals(replaced, self.testControl.GetValue())
        
    def testSelection(self):
        """SetSelection, GetSelection, GetStringSelection"""
        txt = "The quick brown fox jumps over the lazy dog."
        self.testControl.SetValue(txt)
        for i,j in ((0,2),(1,9),(7,len(txt))):
            self.testControl.SetSelection(i,j)
            self.assert_((i,j), self.testControl.GetSelection())
            self.assert_(txt[i:j], self.testControl.GetStringSelection())
    
    def testValue(self):
        """SetValue, GetValue"""
        val = "Hello, World!\nLorem Ipsum!"
        self.assertEquals("", self.testControl.GetValue())
        self.testControl.SetValue(val)
        self.assertEquals(val, self.testControl.GetValue())
        self.testControl.SetValue("")
        self.assertEquals("", self.testControl.GetValue())
        

def suite():
    suite = unittest.makeSuite(TextCtrlTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
