import unittest
import wx

'''
This file contains classes and methods for unit testing the API of wx.Button
'''

class ButtonTestFrame(wx.Frame):
    def __init__(self, parent, id):
        wx.Frame.__init__(self, parent, id, 'TestFrame',
                size=(340, 200))
        self.testControl = wx.Button(self, -1)

class ButtonTest(unittest.TestCase):
    #####################
    ## Fixture Methods ##
    #####################
    def setUp(self):
        self.app = wx.PySimpleApp()
        self.frame = ButtonTestFrame(parent=None, id=-1)
        # we just do this to shorten typing :-)
        self.testControl = self.frame.testControl

    def tearDown(self):
        self.frame.Destroy()

    ##################
    ## Test Methods ##
    ##################
    
    # tests go here

def suite():
    suite = unittest.makeSuite(ButtonTest)
    return unittest.TestSuite(suite)
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
