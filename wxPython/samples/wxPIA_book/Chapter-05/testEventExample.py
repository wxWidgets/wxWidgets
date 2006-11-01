import unittest
import modelExample
import wx

class TestExample(unittest.TestCase):

    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = modelExample.ModelExample(parent=None, id=-1)

    def tearDown(self):
        self.frame.Destroy()

    def testModel(self):
        self.frame.OnBarney(None)
        self.assertEqual("Barney", self.frame.model.first,
                msg="First is wrong")
        self.assertEqual("Rubble", self.frame.model.last)

    def testEvent(self):
        panel = self.frame.GetChildren()[0]
        for each in panel.GetChildren():
            if each.GetLabel() == "Wilmafy":
                wilma = each
                break
        event = wx.CommandEvent(wx.wxEVT_COMMAND_BUTTON_CLICKED, wilma.GetId())
        wilma.GetEventHandler().ProcessEvent(event)
        self.assertEqual("Wilma", self.frame.model.first)
        self.assertEqual("Flintstone", self.frame.model.last)

def suite():
    suite = unittest.makeSuite(TestExample, 'test')
    return suite

if __name__ == '__main__':
    unittest.main(defaultTest='suite')

