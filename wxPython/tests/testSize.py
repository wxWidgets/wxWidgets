import unittest
import wx

"""
This file contains classes and methods for unit testing the API of wx.Size
        
Methods yet to test:
__del__, __getitem__, __len__, __nonzero__,
__reduce__, __repr__, __setitem__, __str__, __sub__, DecBy, DecTo,
GetHeight, GetWidth, IncBy, IncTo, IsFullySpecified, Scale, SetDefaults,
SetHeight, SetWidth

asTuple(*args, **kwargs) -> Use Get instead
"""

def getSizeData():
    return tuple( wx.Size(w,h) for w,h in getSizeTuples() )

def getValidSizeData():
    return tuple( wx.Size(w,h) for w,h in getPositiveSizeTuples() )

def getValidSizeHints():
    return ( # minW, minH, maxW, maxH
                (0,0,1000,1000), (0,0,1,1), (9,99,999,9999),
                (0,0,0,0), (10,10,100,100), (64, 64, 64, 64),
                (1000,1000,9999,9999), (1, 10, 100, 1000),
                (1,5,1,5), (100,1,100,1), (99,1000,100,1001)
        )

def getInvalidSizeHints():
    return (
                (10,10,1,1), (99,99,98,98), (100,100,99,99),
                (1,1,0,0), (2,2,1,1), (9999,9999,9998,9998),
                (10,9,100,1), (100,8,7,1000), (100,1,99,1),
                (999,1001,1001,999)
        )

# -----------------------------------------------------------

def getSizeTuples():
    return ( getPositiveSizeTuples() +
                getNegativeSizeTuples() +
                getMixedSizeTuples() )

def getPositiveSizeTuples():
    return (
                (1,1), (5,5), (10,10), (10,1000), (9,9),
                (100,100), (100,500), (200,100), (500,500),
                (1000,1000), (1,1000), (1000,1), (31415, 27182),
                (32767, 32767), (0,0), (999,999)
        )

def getNegativeSizeTuples():
    return (
                (-1,-1), (-5,-5), (-10,-10), (-10,-1000), (-9,-9),
                (-100,-100), (-100,-500), (-200,-100), (-500,-500),
                (-1000,-1000), (-1,-1000), (-1000,-1), (-31415, -27182),
                (-32767, -32767), (-999,-999)
        )

def getMixedSizeTuples():
    return (
                (1,-1), (-5,5), (-10,10), (10,-1000), (-9,9),
                (100,-100), (-100,500), (-200,100), (-500,500),
                (1000,-1000), (-1,1000), (-1000,1), (31415, -27182),
                (32767, -32767), (-999,999)
        )

# -----------------------------------------------------------

NEG_TWO = wx.Size(-2,-2)
NEG_ONE = wx.Size(-1,-1)
ZERO    = wx.Size(0,0)
ONE     = wx.Size(1,1)
TWO     = wx.Size(2,2)

# -----------------------------------------------------------

class SizeTest(unittest.TestCase):
    def setUp(self):
        self.app = wx.PySimpleApp()
    
    def tearDown(self):
        self.app.Destroy()
    
    def testConstructor(self):
        """__init__,
        A trivial test."""
        for (w,h),size in zip(getSizeTuples(),getSizeData()):
            self.assertEquals(size, wx.Size(w,h))
    
    # NOTE:
    #   testEquals, testNotEquals, testAddition, testSubtraction
    #   were copied from testPoint class.  In the future, possibly
    #   refactor out code duplication.
    def testEquals(self):
        """__eq__"""
        self.assert_( ZERO == ZERO )
        self.assert_( ZERO == wx.Size(0,0) )
        self.assert_( ZERO == wx.Size() )
        self.assert_( NEG_ONE == NEG_ONE )
        self.assert_( NEG_ONE == wx.Size(-1,-1) )
        self.assert_( ONE == ONE )
        self.assert_( ONE == wx.Size(1,1) )
        
    def testNotEquals(self):
        """__ne__"""
        self.assert_( ZERO != ONE )
        self.assert_( ONE != NEG_ONE )
        self.assert_( TWO != NEG_TWO )
        self.assert_( NEG_TWO != ZERO )
    
    def testAddition(self):
        """__add__"""
        self.assertEquals( ONE+NEG_ONE, ZERO )
        self.assertEquals( TWO+NEG_TWO, ZERO )
        self.assertEquals( ONE+ONE, TWO )
        self.assertEquals( ZERO+ZERO, ZERO )
        self.assertEquals( NEG_ONE+NEG_ONE, NEG_TWO )
        self.assertEquals( ONE+ZERO, ONE )
        self.assertEquals( TWO+NEG_ONE, ONE )
    
    def testSubtraction(self):
        """__sub__"""
        self.assertEquals( ONE-ONE, ZERO )
        self.assertEquals( NEG_ONE-NEG_ONE, ZERO )
        self.assertEquals( ONE-NEG_ONE, TWO )
        self.assertEquals( NEG_ONE-ONE, NEG_TWO )
        self.assertEquals( ZERO-ONE, NEG_ONE )
        self.assertEquals( ZERO-NEG_ONE, ONE )
        self.assertEquals( NEG_ONE-ZERO, NEG_ONE )
        self.assertEquals( ONE-ZERO, ONE )
    
    def testGetSet(self):
        """Set, Get"""
        size = wx.Size()
        for w,h in getSizeTuples():
            size.Set(w,h)
            self.assertEquals((w,h), size.Get())

def suite():
    suite = unittest.makeSuite(SizeTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
