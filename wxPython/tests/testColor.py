import unittest
import wx

'''
This file contains classes and methods for unit testing the API of wx.Color
(as well as wx.Colour, because they're the same thing)

IDEA: implement all the tests for just wx.Color, then abstract the tests
        to run against a particular symbol that can be substituted into.
        then sub both wx.Color and wx.Colour into it.
        (yes i know that they refer to the same thing.  but let's be thorough.)
'''

class ColorTest(unittest.TestCase):
    def testBlue(self):
        one = wx.Color('BLUE')
        two = wx.Color('#0000FF')
        three = wx.Color(0,0,255)
        self.assertEquals(one, two)
        self.assertEquals(one,three)
        self.assertEquals(two,three)
    
    def testRed(self):
        one = wx.Color('RED')
        two = wx.Color('#FF0000')
        three = wx.Color(255,0,0)
        self.assertEquals(one, two)
        self.assertEquals(one,three)
        self.assertEquals(two,three)
        
    def testGreen(self):
        one = wx.Color('GREEN')
        two = wx.Color('#00FF00')
        three = wx.Color(0,0,255)
        self.assertEquals(one, two)
        self.assertEquals(one,three)
        self.assertEquals(two,three)


def suite():
    suite = unittest.makeSuite(ColorTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
