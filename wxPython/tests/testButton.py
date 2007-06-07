import unittest
import wx

import testWindow

'''
This file contains classes and methods for unit testing the API of wx.Button
'''

class ButtonTestFrame(wx.Frame):
    def __init__(self, parent, id):
        wx.Frame.__init__(self, parent, id, 'TestFrame',
                size=(340, 200))
        self.testControl = wx.Button(self, -1)

class ButtonTest(testWindow.WindowTest):
    #####################
    ## Fixture Methods ##
    #####################
    
    # modified setUp and tearDown go here

    ##################
    ## Test Methods ##
    ##################
    
    # additional tests go here
    pass

def suite():
    suite = unittest.makeSuite(ButtonTest)
    return unittest.TestSuite(suite)
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
