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

def suite():
    suite = unittest.makeSuite(TestExample, 'test')
    return suite

if __name__ == '__main__':
    unittest.main(defaultTest='suite')

