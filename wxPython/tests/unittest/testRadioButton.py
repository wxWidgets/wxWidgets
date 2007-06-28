import unittest
import wx

import testControl

"""
This file contains classes and methods for unit testing the API of
wx.RadioButton
        
Methods yet to test:
__init__, Create, GetClassDefaultAttributes
"""

class RadioButtonTest(testControl.ControlTest):
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = wx.Frame(parent=None, id=wx.ID_ANY)
        self.LABEL = "Label of the RadioButton!"
        self.testControl = wx.RadioButton(parent=self.frame, label=self.LABEL,
                                style=wx.RB_GROUP)
        
    def testDifferentGroupsToggle(self):
        one = [ wx.RadioButton(self.frame, style=wx.RB_GROUP),
                wx.RadioButton(self.frame),
                wx.RadioButton(self.frame) ]
        two = [ wx.RadioButton(self.frame, style=wx.RB_GROUP),
                wx.RadioButton(self.frame),
                wx.RadioButton(self.frame) ]
        for i in range(len(one)):
            one[i].SetValue(True)
            for j in range(len(two)):
                two[j].SetValue(True)
                for rb in two:
                    if rb == two[j]:
                        self.assert_(rb.GetValue())
                    else:
                        self.assert_(not rb.GetValue())
                        
    def testLabel(self):
        """
        Overrides previous testLabel method"""
        self.assertEquals(self.LABEL, self.testControl.GetLabel())
    
    def testToggle(self):
        a = wx.RadioButton(self.frame, label="a")
        b = wx.RadioButton(self.frame, label="b")
        self.testControl.SetValue(True)
        self.assert_(self.testControl.GetValue())
        for rad in (a,b):
            self.assert_(not rad.GetValue())
        a.SetValue(True)
        self.assert_(a.GetValue())
        for rad in (b,self.testControl):
            self.assert_(not rad.GetValue())
        b.SetValue(True)
        self.assert_(b.GetValue())
        for rad in (a,self.testControl):
            self.assert_(not rad.GetValue())
        
    def testValue(self):
        """SetValue, GetValue"""
        self.testControl.SetValue(True)
        self.assert_(self.testControl.GetValue())
        self.testControl.SetValue(False)
        self.assert_(not self.testControl.GetValue())
        
        
def suite():
    suite = unittest.makeSuite(RadioButtonTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
