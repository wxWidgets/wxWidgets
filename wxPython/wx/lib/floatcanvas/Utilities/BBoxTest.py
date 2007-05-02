
"""
Test code for the BBox Object

"""

import unittest

from BBox import *

class testCreator(unittest.TestCase):
    def testCreates(self):
        B = BBox(((0,0),(5,5)))
        self.failUnless(isinstance(B, BBox))

    def testType(self):
        B = N.array(((0,0),(5,5)))
        self.failIf(isinstance(B, BBox))

    def testDataType(self):
        B = BBox(((0,0),(5,5)))
        self.failUnless(B.dtype == N.float)

    def testShape(self):
        B = BBox((0,0,5,5))
        self.failUnless(B.shape == (2,2))
        
    def testShape2(self):
        self.failUnlessRaises(ValueError, BBox, (0,0,5) )
        
    def testShape3(self):
        self.failUnlessRaises(ValueError, BBox, (0,0,5,6,7) )

    def testArrayConstruction(self):
        A = N.array(((4,5),(10,12)), N.float_)
        B = BBox(A)
        self.failUnless(isinstance(B, BBox))
        
    def testMinMax(self):
        self.failUnlessRaises(ValueError, BBox, (0,0,-1,6) )

    def testMinMax2(self):
        self.failUnlessRaises(ValueError, BBox, (0,0,1,-6) )

    def testMinMax(self):
        # OK to have a zero-sized BB
        B = BBox(((0,0),(0,5)))
        self.failUnless(isinstance(B, BBox))

    def testMinMax2(self):
        # OK to have a zero-sized BB
        B = BBox(((10.0,-34),(10.0,-34.0)))
        self.failUnless(isinstance(B, BBox))

    def testMinMax3(self):
        # OK to have a tiny BB
        B = BBox(((0,0),(1e-20,5)))
        self.failUnless(isinstance(B, BBox))

    def testMinMax4(self):
        # Should catch tiny difference
        self.failUnlessRaises(ValueError, BBox, ((0,0), (-1e-20,5)) )

class testAsBBox(unittest.TestCase):

    def testPassThrough(self):
        B = BBox(((0,0),(5,5)))
        C = asBBox(B)
        self.failUnless(B is C)

    def testPassThrough2(self):
        B = (((0,0),(5,5)))
        C = asBBox(B)
        self.failIf(B is C)
    
    def testPassArray(self):
        # Different data type
        A = N.array( (((0,0),(5,5))) )
        C = asBBox(A)
        self.failIf(A is C)
    
    def testPassArray2(self):
        # same data type -- should be a view
        A = N.array( (((0,0),(5,5))), N.float_ )
        C = asBBox(A)
        A[0,0] = -10
        self.failUnless(C[0,0] == A[0,0])
    
class testIntersect(unittest.TestCase):

    def testSame(self):
        B = BBox(((-23.5, 456),(56, 532.0)))
        C = BBox(((-23.5, 456),(56, 532.0)))
        self.failUnless(B.Overlaps(C) )
    
    def testUpperLeft(self):
        B = BBox( ( (5, 10),(15, 25) ) )
        C = BBox( ( (0, 12),(10, 32.0) ) )
        self.failUnless(B.Overlaps(C) )
    
    def testUpperRight(self):
        B = BBox( ( (5, 10),(15, 25) ) )
        C = BBox( ( (12, 12),(25, 32.0) ) )
        self.failUnless(B.Overlaps(C) )
    
    def testLowerRight(self):
        B = BBox( ( (5, 10),(15, 25) ) )
        C = BBox( ( (12, 5),(25, 15) ) )
        self.failUnless(B.Overlaps(C) )
    
    def testLowerLeft(self):
        B = BBox( ( (5, 10),(15, 25) ) )
        C = BBox( ( (-10, 5),(8.5, 15) ) )
        self.failUnless(B.Overlaps(C) )
        
    def testBelow(self):
        B = BBox( ( (5, 10),(15, 25) ) )
        C = BBox( ( (-10, 5),(8.5, 9.2) ) )
        self.failIf(B.Overlaps(C) )
        
    def testAbove(self):
        B = BBox( ( (5, 10),(15, 25) ) )
        C = BBox( ( (-10, 25.001),(8.5, 32) ) )
        self.failIf(B.Overlaps(C) )
        
    def testLeft(self):
        B = BBox( ( (5, 10),(15, 25) ) )
        C = BBox( ( (4, 8),(4.95, 32) ) )
        self.failIf(B.Overlaps(C) )
        
    def testRight(self):
        B = BBox( ( (5, 10),(15, 25) ) )
        C = BBox( ( (17.1, 8),(17.95, 32) ) )
        self.failIf(B.Overlaps(C) )

    def testInside(self):
        B = BBox( ( (-15, -25),(-5, -10) ) )
        C = BBox( ( (-12, -22), (-6, -8) ) )
        self.failUnless(B.Overlaps(C) )
        
    def testOutside(self):
        B = BBox( ( (-15, -25),(-5, -10) ) )
        C = BBox( ( (-17, -26), (3, 0) ) )
        self.failUnless(B.Overlaps(C) )
    
    def testTouch(self):
        B = BBox( ( (5, 10),(15, 25) ) )
        C = BBox( ( (15, 8),(17.95, 32) ) )
        self.failUnless(B.Overlaps(C) )
        
    def testCorner(self):
        B = BBox( ( (5, 10),(15, 25) ) )
        C = BBox( ( (15, 25),(17.95, 32) ) )
        self.failUnless(B.Overlaps(C) )
        
    def testZeroSize(self):
        B = BBox( ( (5, 10),(15, 25) ) )
        C = BBox( ( (15, 25),(15, 25) ) )
        self.failUnless(B.Overlaps(C) )
        
    def testZeroSize2(self):
        B = BBox( ( (5, 10),(5, 10) ) )
        C = BBox( ( (15, 25),(15, 25) ) )
        self.failIf(B.Overlaps(C) )
        
    def testZeroSize3(self):
        B = BBox( ( (5, 10),(5, 10) ) )
        C = BBox( ( (0, 8),(10, 12) ) )
        self.failUnless(B.Overlaps(C) )

    def testZeroSize4(self):
        B = BBox( ( (5, 1),(10, 25) ) )
        C = BBox( ( (8, 8),(8, 8) ) )
        self.failUnless(B.Overlaps(C) )



class testEquality(unittest.TestCase):
    def testSame(self):
        B = BBox( ( (1.0, 2.0), (5.0, 10.0) ) )
        C = BBox( ( (1.0, 2.0), (5.0, 10.0) ) )
        self.failUnless(B == C)
        
    def testIdentical(self):
        B = BBox( ( (1.0, 2.0), (5.0, 10.0) ) )
        self.failUnless(B == B)
        
    def testNotSame(self):
        B = BBox( ( (1.0, 2.0), (5.0, 10.0) ) )
        C = BBox( ( (1.0, 2.0), (5.0, 10.1) ) )
        self.failIf(B == C)
        
    def testWithArray(self):
        B = BBox( ( (1.0, 2.0), (5.0, 10.0) ) )
        C = N.array( ( (1.0, 2.0), (5.0, 10.0) ) )
        self.failUnless(B == C)
        
    def testWithArray2(self):
        B = BBox( ( (1.0, 2.0), (5.0, 10.0) ) )
        C = N.array( ( (1.0, 2.0), (5.0, 10.0) ) )
        self.failUnless(C == B)
        
    def testWithArray2(self):
        B = BBox( ( (1.0, 2.0), (5.0, 10.0) ) )
        C = N.array( ( (1.01, 2.0), (5.0, 10.0) ) )
        self.failIf(C == B)
        
class testInside(unittest.TestCase):
    def testSame(self):
        B = BBox( ( (1.0, 2.0), (5.0, 10.0) ) )
        C = BBox( ( (1.0, 2.0), (5.0, 10.0) ) )
        self.failUnless(B.Inside(C))

    def testPoint(self):
        B = BBox( ( (1.0, 2.0), (5.0, 10.0) ) )
        C = BBox( ( (3.0, 4.0), (3.0, 4.0) ) )
        self.failUnless(B.Inside(C))

    def testPointOutside(self):
        B = BBox( ( (1.0, 2.0), (5.0, 10.0) ) )
        C = BBox( ( (-3.0, 4.0), (0.10, 4.0) ) )
        self.failIf(B.Inside(C))

    def testUpperLeft(self):
        B = BBox( ( (5, 10),(15, 25) ) )
        C = BBox( ( (0, 12),(10, 32.0) ) )
        self.failIf(B.Inside(C) )
    
    def testUpperRight(self):
        B = BBox( ( (5, 10),(15, 25) ) )
        C = BBox( ( (12, 12),(25, 32.0) ) )
        self.failIf(B.Inside(C) )
    
    def testLowerRight(self):
        B = BBox( ( (5, 10),(15, 25) ) )
        C = BBox( ( (12, 5),(25, 15) ) )
        self.failIf(B.Inside(C) )
    
    def testLowerLeft(self):
        B = BBox( ( (5, 10),(15, 25) ) )
        C = BBox( ( (-10, 5),(8.5, 15) ) )
        self.failIf(B.Inside(C) )
        
    def testBelow(self):
        B = BBox( ( (5, 10),(15, 25) ) )
        C = BBox( ( (-10, 5),(8.5, 9.2) ) )
        self.failIf(B.Inside(C) )
        
    def testAbove(self):
        B = BBox( ( (5, 10),(15, 25) ) )
        C = BBox( ( (-10, 25.001),(8.5, 32) ) )
        self.failIf(B.Inside(C) )
        
    def testLeft(self):
        B = BBox( ( (5, 10),(15, 25) ) )
        C = BBox( ( (4, 8),(4.95, 32) ) )
        self.failIf(B.Inside(C) )
        
    def testRight(self):
        B = BBox( ( (5, 10),(15, 25) ) )
        C = BBox( ( (17.1, 8),(17.95, 32) ) )
        self.failIf(B.Inside(C) )

class testFromPoints(unittest.TestCase):

    def testCreate(self):
        Pts = N.array( ((5,2),
                (3,4),
                (1,6),
                ), N.float_ )
        B = fromPoints(Pts)
        #B = BBox( ( (1.0, 2.0), (5.0, 10.0) ) )
        self.failUnless(B[0,0] == 1.0 and
                        B[0,1] == 2.0 and
                        B[1,0] == 5.0 and
                        B[1,1] == 6.0
                        )
    def testCreateInts(self):
        Pts = N.array( ((5,2),
                (3,4),
                (1,6),
                ) )
        B = fromPoints(Pts)
        self.failUnless(B[0,0] == 1.0 and
                        B[0,1] == 2.0 and
                        B[1,0] == 5.0 and
                        B[1,1] == 6.0
                        )

    def testSinglePoint(self):
        Pts = N.array( (5,2), N.float_ )
        B = fromPoints(Pts)
        self.failUnless(B[0,0] == 5.0 and
                        B[0,1] == 2.0 and
                        B[1,0] == 5.0 and
                        B[1,1] == 2.0
                        )

    def testListTuples(self):
        Pts = [ (3, 6.5),
                (13, 43.2),
                (-4.32, -4),
                (65, -23),
                (-0.0001, 23.432),
                ]
        B = fromPoints(Pts)       
        self.failUnless(B[0,0] == -4.32 and
                        B[0,1] == -23.0 and
                        B[1,0] == 65.0 and
                        B[1,1] == 43.2
                        )
class testMerge(unittest.TestCase):
    A = BBox( ((-23.5, 456), (56, 532.0)) )
    B = BBox( ((-20.3, 460), (54, 465  )) )# B should be completely inside A
    C = BBox( ((-23.5, 456), (58, 540.0)) )# up and to the right or A
    D = BBox( ((-26.5, 12), (56, 532.0)) )

    def testInside(self):
        C = self.A.copy()
        C.Merge(self.B)
        self.failUnless(C == self.A)

    def testFullOutside(self):
        C = self.B.copy()
        C.Merge(self.A)
        self.failUnless(C == self.A)

    def testUpRight(self):
        A = self.A.copy()
        A.Merge(self.C)
        self.failUnless(A[0] == self.A[0] and A[1] == self.C[1])

    def testDownLeft(self):
        A = self.A.copy()
        A.Merge(self.D)
        self.failUnless(A[0] == self.D[0] and A[1] == self.A[1])

class testBBarray(unittest.TestCase):
    BBarray = N.array( ( ((-23.5, 456), (56, 532.0)),
                         ((-20.3, 460), (54, 465  )),
                         ((-23.5, 456), (58, 540.0)),
                         ((-26.5,  12), (56, 532.0)),
                       ),
                       dtype=N.float)
    print BBarray
    BB = asBBox( ((-26.5,  12.), ( 58. , 540.)) )

    def testJoin(self):
        BB = fromBBArray(self.BBarray)
        self.failUnless(BB == self.BB, "Wrong BB was created. It was:\n%s \nit should have been:\n%s"%(BB, self.BB))


if __name__ == "__main__":
    unittest.main()
