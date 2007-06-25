import unittest
import wx

"""
This file contains classes and methods for unit testing the API of wx.Point
        
Methods yet to test:
__del__, __getitem__, __iadd__, __isub__, __len__,
__nonzero__, __reduce__, __repr__, __setitem__, __str__

asTuple(*args, **kwargs) -> Use Get instead
"""

# the delegation among the following methods is in order to expose
# "valid" data to the wx.Window tests while still being able to
# comprehensively test the point class (and, in the future, expose
# the full functionality to any other class)

def getPointData():
    return tuple( wx.Point(x,y) for x,y in getXYData() )

# ASSUME: "valid" points are positive
def getValidPointData():
    return tuple( wx.Point(x,y) for x,y in getPositiveXYData() )

# -----------------------------------------------------------

# TODO: what is the maximum value that wx.Point is capable of achieving?

def getXYData():
    return ( getPositiveXYData() +
            getNegativeXYData() +
            getMixedXYData() )

def getPositiveXYData():
    return (
                (0,0), (1,2), (2,1), (10,10),
                (20,30), (66,99), (100,200),
                (999,789), (1000,2000), (3141,2718)
        )

def getNegativeXYData():
    return (
                (-1,-1), (-2,-3), (-9,-9), (-10,-11), (-34,-56),
                (-64,-81), (-99,-99), (-121, -324), (-999, -999),
                (-1001,-1234), (-9999,-9999), (-99999,-99999)
        )

def getMixedXYData():
    return (
                (-1,1), (2,-3), (-9,9), (-10,11), (34,-56),
                (-64,81), (99,-99), (-121, 324), (-999, 999),
                (1001,-1234), (-9999,9999), (99999,-99999),
                (-1,2), (-2,1), (6,-7), (10,-10), (-20,30), (66,-99), 
                (-100,200), (999,-789), (1000,-2000), (3141,-2718)
        )

# -----------------------------------------------------------

NEG_TWO = wx.Point(-2,-2)
NEG_ONE = wx.Point(-1,-1)
ZERO    = wx.Point(0,0)
ONE     = wx.Point(1,1)
TWO     = wx.Point(2,2)
        
# -----------------------------------------------------------

class PointTest(unittest.TestCase):
    def setUp(self):
        self.app = wx.PySimpleApp()
    
    def tearDown(self):
        self.app.Destroy()
    
    def testConstructor(self):
        """__init__"""
        for x,y in getXYData():
            point = wx.Point(x,y)
            self.assertEquals((x,y), point.Get())
        point = wx.Point()
        self.assertEquals((0,0), point)
        
    def testGetSet(self):
        """Set, Get"""
        point = wx.Point()
        for x,y in getXYData():
            point.Set(x,y)
            self.assertEquals((x,y), point.Get())
            
    def testEquals(self):
        """__eq__"""
        self.assert_( ZERO == ZERO )
        self.assert_( ZERO == wx.Point(0,0) )
        self.assert_( ZERO == wx.Point() )
        self.assert_( NEG_ONE == NEG_ONE )
        self.assert_( NEG_ONE == wx.Point(-1,-1) )
        self.assert_( ONE == ONE )
        self.assert_( ONE == wx.Point(1,1) )
        
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


def suite():
    suite = unittest.makeSuite(PointTest)
    return suite
    
if __name__ == '__main__':
    unittest.main(defaultTest='suite')
