import unittest
import wx

"""
This file contains classes and methods for unit testing the API of wx.Colour
(as well as wx.Color, because they're the same thing)
        
Methods yet to test:
__del__, __eq__, __getitem__, __len__, __ne__, __nonzero__, __reduce__, 
__repr__, __str__, GetAsString, GetPixel
"""
            
def getColourEquivalents():
    # doesn't include wx.Colour instances, only equivalents
    return ( getColourEquivalentNames() +
                getColourEquivalentHexValues() +
                getColourEquivalentTuples() )
    
def getColourEquivalentNames():
    return tuple((name, wx.TheColourDatabase.FindColour(name)) for name in getColourNames())

def getColourEquivalentHexValues():
    return tuple((hexify(col), col) for name,col in getColourEquivalentNames())
    
def getColourEquivalentTuples():
    return tuple((col.Get(), col) for name,col in getColourEquivalentNames())

def hexify(col):
    (r,g,b) = col.Get()
    rhex, ghex, bhex = hex(r)[2:], hex(g)[2:], hex(b)[2:]
    if len(rhex) == 1:
        rhex = '0' + rhex
    if len(ghex) == 1:
        ghex = '0' + ghex
    if len(bhex) == 1:
        bhex = '0' + bhex
    return '#' + rhex + ghex + bhex

# -----------------------------------------------------------

def getColourData():
    return tuple( wx.Colour(*rgba) for rgba in getColourTuples() )

def getColourTuples():
    return (
                (0,0,0,0), (1,1,1), (255,0,0), (0,255,0), (0,0,255),
                (100,100,100), (32,64,128,254), (254,121,61), (9,12,81),
                (4,1,7,99), (255,0,0,23), (0,255,0,94), (0,0,255,102),
                (100,100,100,128), (32,64,128,2), (254,121,61,0), (9,12,81,255),
        )

def getColourNames():
    """from inspection of wx.TheColourDatabase"""
    return ('BLACK','BLUE','SLATE BLUE','GREEN','SPRING GREEN','CYAN','NAVY',
            'STEEL BLUE','FOREST GREEN','SEA GREEN','DARK GREY','MIDNIGHT BLUE',
            'DARK GREEN','DARK SLATE GREY','MEDIUM BLUE','SKY BLUE','LIME GREEN',
            'MEDIUM AQUAMARINE','CORNFLOWER BLUE','MEDIUM SEA GREEN','INDIAN RED',
            'VIOLET','DARK OLIVE GREEN','DIM GREY','CADET BLUE','MEDIUM GREY',
            'DARK SLATE BLUE','MEDIUM FOREST GREEN','SALMON','DARK TURQUOISE',
            'AQUAMARINE','MEDIUM TURQUOISE','MEDIUM SLATE BLUE','MEDIUM SPRING GREEN',
            'GREY','FIREBRICK','MAROON','SIENNA','LIGHT STEEL BLUE','PALE GREEN',
            'MEDIUM ORCHID','GREEN YELLOW','DARK ORCHID','YELLOW GREEN','BLUE VIOLET',
            'KHAKI','BROWN','TURQUOISE','PURPLE','LIGHT BLUE','LIGHT GREY','ORANGE',
            'VIOLET RED','GOLD','THISTLE','WHEAT','MEDIUM VIOLET RED','ORCHID',
            'TAN','GOLDENROD','PLUM','MEDIUM GOLDENROD','RED','ORANGE RED',
            'LIGHT MAGENTA','CORAL','PINK','YELLOW','WHITE')

# -----------------------------------------------------------

class ColorTest(unittest.TestCase):
    def setUp(self):
        self.app = wx.PySimpleApp()
    
    def tearDown(self):
        self.app.Destroy()
    
    def testColorColourAlias(self):
        self.assertEquals(wx.Color, wx.Colour)
    
    def testSetFromName(self):
        """SetFromName"""
        for name,colour in getColourEquivalentNames():
            newcol = wx.Colour()
            newcol.SetFromName(name)
            self.assertEquals(colour, newcol)
    
    def testConstructor(self):
        """__init__"""
        self.assertRaises(OverflowError, wx.Colour, -1)
        self.assertRaises(OverflowError, wx.Colour, 256)
        
    def testGetSetRGB(self):
        """SetRGB, GetRGB"""
        for color in getColourData():
            sludge = color.GetRGB()
            del color
            color = wx.Colour()
            color.SetRGB(sludge)
            self.assertEquals(sludge, color.GetRGB())
            
    def testMultipleAccessors(self):
        """Get, Set"""
        for i in range(256):
            color = wx.Color()
            color.Set(i,i,i,i)
            self.assertEquals((i,i,i), color.Get())
            self.assertEquals(i, color.Alpha())
    
    def testOk(self):
        """IsOk, Ok"""
        c1 = wx.Colour(255,255,255,255)
        c2 = wx.Colour(0,0,0,0)
        c3 = wx.Colour()
        for color in (c1, c2, c3):
            self.assert_(color.IsOk())
            self.assert_(color.Ok())
        # HACK: to generate an invalid wx.Colour instance
        # NOTE: cannot access colBg directly without crashing the interpreter
        attr = wx.VisualAttributes()
        self.assert_(not attr.colBg.Ok())
        self.assert_(not attr.colBg.IsOk())
    
    def testSingleAccessors(self):
        """Red, Green, Blue, Alpha"""
        for i in range(256):
            colour = wx.Colour(i,i,i,i)
            self.assertEquals(i, colour.Red())
            self.assertEquals(i, colour.Green())
            self.assertEquals(i, colour.Blue())
            self.assertEquals(i, colour.Alpha())
            

def suite():
    suite = unittest.makeSuite(ColorTest)
    return suite
    
if __name__ == '__main__':
    # test getColourEquivalents
    print "Testing getColourEquivalents... ",
    app = wx.PySimpleApp()
    f = wx.Frame(None)
    for test, colour in getColourEquivalents():
        f.SetBackgroundColour(test)
        #print 'Test: ', test
        #print 'Colour: ', colour
        assert colour == f.GetBackgroundColour()
    print "Done"
    unittest.main(defaultTest='suite')
