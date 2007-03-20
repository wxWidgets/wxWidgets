# 12/02/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for 2.5 compatability.
#

"""
FancyText -- methods for rendering XML specified text

This module exports four main methods::

    def GetExtent(str, dc=None, enclose=True)
    def GetFullExtent(str, dc=None, enclose=True)
    def RenderToBitmap(str, background=None, enclose=True)
    def RenderToDC(str, dc, x, y, enclose=True)

In all cases, 'str' is an XML string. Note that start and end tags are
only required if *enclose* is set to False. In this case the text
should be wrapped in FancyText tags.

In addition, the module exports one class::

    class StaticFancyText(self, window, id, text, background, ...)

This class works similar to StaticText except it interprets its text
as FancyText.

The text can support superscripts and subscripts, text in different
sizes, colors, styles, weights and families. It also supports a
limited set of symbols, currently *times*, *infinity*, *angle* as well
as greek letters in both upper case (*Alpha* *Beta*... *Omega*) and
lower case (*alpha* *beta*... *omega*).

>>> frame = wx.Frame(wx.NULL, -1, "FancyText demo", wx.DefaultPosition)
>>> sft = StaticFancyText(frame, -1, testText, wx.Brush("light grey", wx.SOLID))
>>> frame.SetClientSize(sft.GetSize())
>>> didit = frame.Show()
>>> from guitest import PauseTests; PauseTests()

"""

# Copyright 2001-2003 Timothy Hochberg
# Use as you see fit. No warantees, I cannot be held responsible, etc.

import copy
import math
import sys

import wx
import xml.parsers.expat

__all__ = "GetExtent", "GetFullExtent", "RenderToBitmap", "RenderToDC", "StaticFancyText"

if sys.platform == "win32":
    _greekEncoding = str(wx.FONTENCODING_CP1253)
else:
    _greekEncoding = str(wx.FONTENCODING_ISO8859_7)

_families = {"fixed" : wx.FIXED, "default" : wx.DEFAULT, "decorative" : wx.DECORATIVE, "roman" : wx.ROMAN,
                "script" : wx.SCRIPT, "swiss" : wx.SWISS, "modern" : wx.MODERN}
_styles = {"normal" : wx.NORMAL, "slant" : wx.SLANT, "italic" : wx.ITALIC}
_weights = {"normal" : wx.NORMAL, "light" : wx.LIGHT, "bold" : wx.BOLD}

# The next three classes: Renderer, SizeRenderer and DCRenderer are
# what you will need to override to extend the XML language. All of
# the font stuff as well as the subscript and superscript stuff are in 
# Renderer.

_greek_letters = ("alpha", "beta", "gamma", "delta", "epsilon",  "zeta",
                 "eta", "theta", "iota", "kappa", "lambda", "mu", "nu",
                 "xi", "omnikron", "pi", "rho", "altsigma", "sigma", "tau", "upsilon",
                 "phi", "chi", "psi", "omega")

def iround(number):
    return int(round(number))
    
def iceil(number):
    return int(math.ceil(number))

class Renderer:
    """Class for rendering XML marked up text.

    See the module docstring for a description of the markup.

    This class must be subclassed and the methods the methods that do
    the drawing overridden for a particular output device.

    """
    defaultSize = None
    defaultFamily = wx.DEFAULT
    defaultStyle = wx.NORMAL
    defaultWeight = wx.NORMAL
    defaultEncoding = None
    defaultColor = "black"

    def __init__(self, dc=None, x=0, y=None):
        if dc == None:
            dc = wx.MemoryDC()
        self.dc = dc
        self.offsets = [0]
        self.fonts = [{}]
        self.width = self.height = 0
        self.x = x
        self.minY = self.maxY = self._y = y
        if Renderer.defaultSize is None:
            Renderer.defaultSize = wx.NORMAL_FONT.GetPointSize()
        if Renderer.defaultEncoding is None:
            Renderer.defaultEncoding = wx.Font_GetDefaultEncoding()
        
    def getY(self):
        if self._y is None:
            self.minY = self.maxY = self._y = self.dc.GetTextExtent("M")[1]
        return self._y
    def setY(self, value):
        self._y = y
    y = property(getY, setY)
        
    def startElement(self, name, attrs):
        method = "start_" + name
        if not hasattr(self, method):
            raise ValueError("XML tag '%s' not supported" % name)
        getattr(self, method)(attrs)

    def endElement(self, name):
        methname = "end_" + name
        if hasattr(self, methname):
            getattr(self, methname)()
        elif hasattr(self, "start_" + name):
            pass
        else:
            raise ValueError("XML tag '%s' not supported" % methname)
        
    def characterData(self, data):
        self.dc.SetFont(self.getCurrentFont())
        for i, chunk in enumerate(data.split('\n')):
            if i:
                self.x = 0
                self.y = self.mayY = self.maxY + self.dc.GetTextExtent("M")[1]
            if chunk:
                width, height, descent, extl = self.dc.GetFullTextExtent(chunk)
                self.renderCharacterData(data, iround(self.x), iround(self.y + self.offsets[-1] - height + descent))
            else:
                width = height = descent = extl = 0
            self.updateDims(width, height, descent, extl)

    def updateDims(self, width, height, descent, externalLeading):
        self.x += width
        self.width = max(self.x, self.width)
        self.minY = min(self.minY, self.y+self.offsets[-1]-height+descent)
        self.maxY = max(self.maxY, self.y+self.offsets[-1]+descent)
        self.height = self.maxY - self.minY

    def start_FancyText(self, attrs):
        pass
    start_wxFancyText = start_FancyText # For backward compatibility

    def start_font(self, attrs):
        for key, value in attrs.items():
            if key == "size":
                value = int(value)
            elif key == "family":
                value = _families[value]
            elif key == "style":
                value = _styles[value]
            elif key == "weight":
                value = _weights[value]
            elif key == "encoding":
                value = int(value)
            elif key == "color":
                pass
            else:
                raise ValueError("unknown font attribute '%s'" % key)
            attrs[key] = value
        font = copy.copy(self.fonts[-1])
        font.update(attrs)
        self.fonts.append(font)

    def end_font(self):
        self.fonts.pop()

    def start_sub(self, attrs):
        if attrs.keys():
            raise ValueError("<sub> does not take attributes")
        font = self.getCurrentFont()
        self.offsets.append(self.offsets[-1] + self.dc.GetFullTextExtent("M", font)[1]*0.5)
        self.start_font({"size" : font.GetPointSize() * 0.8})

    def end_sub(self):
        self.fonts.pop()
        self.offsets.pop()

    def start_sup(self, attrs):
        if attrs.keys():
            raise ValueError("<sup> does not take attributes")
        font = self.getCurrentFont()
        self.offsets.append(self.offsets[-1] - self.dc.GetFullTextExtent("M", font)[1]*0.3)
        self.start_font({"size" : font.GetPointSize() * 0.8})

    def end_sup(self):
        self.fonts.pop()
        self.offsets.pop()        

    def getCurrentFont(self):
        font = self.fonts[-1]
        return wx.Font(font.get("size", self.defaultSize),
                       font.get("family", self.defaultFamily),
                       font.get("style", self.defaultStyle),
                       font.get("weight",self.defaultWeight),
                       False, "",
                       font.get("encoding", self.defaultEncoding))

    def getCurrentColor(self):
        font = self.fonts[-1]
        return wx.TheColourDatabase.FindColour(font.get("color", self.defaultColor))
        
    def getCurrentPen(self):
        return wx.Pen(self.getCurrentColor(), 1, wx.SOLID)
        
    def renderCharacterData(self, data, x, y):
        raise NotImplementedError()


def _addGreek():
    alpha = 0xE1
    Alpha = 0xC1
    def end(self):
        pass
    for i, name in enumerate(_greek_letters):
        def start(self, attrs, code=chr(alpha+i)):
            self.start_font({"encoding" : _greekEncoding})
            self.characterData(code)
            self.end_font()
        setattr(Renderer, "start_%s" % name, start)
        setattr(Renderer, "end_%s" % name, end)
        if name == "altsigma":
            continue # There is no capital for altsigma
        def start(self, attrs, code=chr(Alpha+i)):
            self.start_font({"encoding" : _greekEncoding})
            self.characterData(code)
            self.end_font()
        setattr(Renderer, "start_%s" % name.capitalize(), start)
        setattr(Renderer, "end_%s" % name.capitalize(), end)
_addGreek()    



class SizeRenderer(Renderer):
    """Processes text as if rendering it, but just computes the size."""
    
    def __init__(self, dc=None):
        Renderer.__init__(self, dc, 0, 0)
    
    def renderCharacterData(self, data, x, y):
        pass
        
    def start_angle(self, attrs):
        self.characterData("M")

    def start_infinity(self, attrs):
        width, height = self.dc.GetTextExtent("M")
        width = max(width, 10)
        height = max(height, width / 2)
        self.updateDims(width, height, 0, 0)

    def start_times(self, attrs):
        self.characterData("M")

    def start_in(self, attrs):
        self.characterData("M")

    def start_times(self, attrs):
        self.characterData("M")        

    
class DCRenderer(Renderer):
    """Renders text to a wxPython device context DC."""

    def renderCharacterData(self, data, x, y):
        self.dc.SetTextForeground(self.getCurrentColor())
        self.dc.DrawText(data, x, y)

    def start_angle(self, attrs):
        self.dc.SetFont(self.getCurrentFont())
        self.dc.SetPen(self.getCurrentPen())
        width, height, descent, leading = self.dc.GetFullTextExtent("M")
        y = self.y + self.offsets[-1]
        self.dc.DrawLine(iround(self.x), iround(y), iround( self.x+width), iround(y))
        self.dc.DrawLine(iround(self.x), iround(y), iround(self.x+width), iround(y-width))
        self.updateDims(width, height, descent, leading)
      

    def start_infinity(self, attrs):
        self.dc.SetFont(self.getCurrentFont())
        self.dc.SetPen(self.getCurrentPen())
        width, height, descent, leading = self.dc.GetFullTextExtent("M")
        width = max(width, 10)
        height = max(height, width / 2)
        self.dc.SetPen(wx.Pen(self.getCurrentColor(), max(1, width/10)))
        self.dc.SetBrush(wx.TRANSPARENT_BRUSH)
        y = self.y + self.offsets[-1]
        r = iround( 0.95 * width / 4)
        xc = (2*self.x + width) / 2
        yc = iround(y-1.5*r)
        self.dc.DrawCircle(xc - r, yc, r)
        self.dc.DrawCircle(xc + r, yc, r)
        self.updateDims(width, height, 0, 0)

    def start_times(self, attrs):
        self.dc.SetFont(self.getCurrentFont())
        self.dc.SetPen(self.getCurrentPen())
        width, height, descent, leading = self.dc.GetFullTextExtent("M")
        y = self.y + self.offsets[-1]
        width *= 0.8
        width = iround(width+.5)
        self.dc.SetPen(wx.Pen(self.getCurrentColor(), 1))
        self.dc.DrawLine(iround(self.x), iround(y-width), iround(self.x+width-1), iround(y-1))
        self.dc.DrawLine(iround(self.x), iround(y-2), iround(self.x+width-1), iround(y-width-1))
        self.updateDims(width, height, 0, 0)


def RenderToRenderer(str, renderer, enclose=True):
    try:
        if enclose:
            str = '<?xml version="1.0"?><FancyText>%s</FancyText>' % str
        p = xml.parsers.expat.ParserCreate()
        p.returns_unicode = 0
        p.StartElementHandler = renderer.startElement
        p.EndElementHandler = renderer.endElement
        p.CharacterDataHandler = renderer.characterData
        p.Parse(str, 1)
    except xml.parsers.expat.error, err:
        raise ValueError('error parsing text text "%s": %s' % (str, err)) 


# Public interface


def GetExtent(str, dc=None, enclose=True):
    "Return the extent of str"
    renderer = SizeRenderer(dc)
    RenderToRenderer(str, renderer, enclose)
    return iceil(renderer.width), iceil(renderer.height) # XXX round up


def GetFullExtent(str, dc=None, enclose=True):
    renderer = SizeRenderer(dc)
    RenderToRenderer(str, renderer, enclose)
    return iceil(renderer.width), iceil(renderer.height), -iceil(renderer.minY) # XXX round up


def RenderToBitmap(str, background=None, enclose=1):
    "Return str rendered on a minumum size bitmap"
    dc = wx.MemoryDC()
    # Chicken and egg problem, we need a bitmap in the DC in order to
    # measure how big the bitmap should be...
    dc.SelectObject(wx.EmptyBitmap(1,1))
    width, height, dy = GetFullExtent(str, dc, enclose)
    bmp = wx.EmptyBitmap(width, height)
    dc.SelectObject(bmp)
    if background is None:
        dc.SetBackground(wx.WHITE_BRUSH)
    else:
        dc.SetBackground(background) 
    dc.Clear()
    renderer = DCRenderer(dc, y=dy)
    dc.BeginDrawing()
    RenderToRenderer(str, renderer, enclose)
    dc.EndDrawing()
    dc.SelectObject(wx.NullBitmap)
    if background is None:
        img = wx.ImageFromBitmap(bmp)
        bg = dc.GetBackground().GetColour()
        img.SetMaskColour(bg.Red(), bg.Green(), bg.Blue())
        bmp = img.ConvertToBitmap()
    return bmp


def RenderToDC(str, dc, x, y, enclose=1):
    "Render str onto a wxDC at (x,y)"
    width, height, dy = GetFullExtent(str, dc)
    renderer = DCRenderer(dc, x, y+dy)
    RenderToRenderer(str, renderer, enclose)
    
    
class StaticFancyText(wx.StaticBitmap):
    def __init__(self, window, id, text, *args, **kargs):
        args = list(args)
        kargs.setdefault('name', 'staticFancyText')
        if 'background' in kargs:
            background = kargs.pop('background')
        elif args:
            background = args.pop(0)
        else:
            background = wx.Brush(window.GetBackgroundColour(), wx.SOLID)
        
        bmp = RenderToBitmap(text, background)
        wx.StaticBitmap.__init__(self, window, id, bmp, *args, **kargs)


# Old names for backward compatibiliry
getExtent = GetExtent
renderToBitmap = RenderToBitmap
renderToDC = RenderToDC


# Test Driver

def test():
    testText = \
"""<font weight="bold" size="16">FancyText</font> -- <font style="italic" size="16">methods for rendering XML specified text</font>
<font family="swiss" size="12">
This module exports four main methods::
<font family="fixed" style="slant">
    def GetExtent(str, dc=None, enclose=True)
    def GetFullExtent(str, dc=None, enclose=True)
    def RenderToBitmap(str, background=None, enclose=True)
    def RenderToDC(str, dc, x, y, enclose=True)
</font>
In all cases, 'str' is an XML string. Note that start and end tags
are only required if *enclose* is set to False. In this case the 
text should be wrapped in FancyText tags.

In addition, the module exports one class::
<font family="fixed" style="slant">
    class StaticFancyText(self, window, id, text, background, ...)
</font>
This class works similar to StaticText except it interprets its text 
as FancyText.

The text can support<sup>superscripts</sup> and <sub>subscripts</sub>, text
in different <font size="20">sizes</font>, <font color="blue">colors</font>, <font style="italic">styles</font>, <font weight="bold">weights</font> and
<font family="script">families</font>. It also supports a limited set of symbols,
currently <times/>, <infinity/>, <angle/> as well as greek letters in both
upper case (<Alpha/><Beta/>...<Omega/>) and lower case (<alpha/><beta/>...<omega/>).

We can use doctest/guitest to display this string in all its marked up glory.
<font family="fixed">
>>> frame = wx.Frame(wx.NULL, -1, "FancyText demo", wx.DefaultPosition)
>>> sft = StaticFancyText(frame, -1, __doc__, wx.Brush("light grey", wx.SOLID))
>>> frame.SetClientSize(sft.GetSize())
>>> didit = frame.Show()
>>> from guitest import PauseTests; PauseTests()

</font></font>
The End"""

    app = wx.PySimpleApp()
    box = wx.BoxSizer(wx.VERTICAL)
    frame = wx.Frame(None, -1, "FancyText demo", wx.DefaultPosition)
    frame.SetBackgroundColour("light grey")
    sft = StaticFancyText(frame, -1, testText)
    box.Add(sft, 1, wx.EXPAND)
    frame.SetSizer(box)
    frame.SetAutoLayout(True)
    box.Fit(frame)
    box.SetSizeHints(frame)
    frame.Show()
    app.MainLoop()

if __name__ == "__main__":    
    test()


