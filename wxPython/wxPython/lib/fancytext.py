"""wxFancyText -- methods for rendering XML specified text

This module has four main methods:

def getExtent(str, dc=None, enclose=1):
def renderToBitmap(str, background=None, enclose=1)
def renderToDC(str, dc, x, y, enclose=1)

In all cases, 'str' is an XML string. The tags in the string can
currently specify the font, subscripts, superscripts, and the angle
sign. The allowable properties of font are size, family, style, weght,
encoding, and color. See the example on the bottom for a better idea
of how this works.

Note that start and end tags for the string are provided if enclose is
true, so for instance, renderToBitmap("X<sub>1</sub>") will work.

"""
# Copyright 2001 Timothy Hochberg
# Use as you see fit. No warantees, I cannot be held responsible, etc.



# TODO:  Make a wxFancyTextCtrl class that derives from wxControl.
#        Add support for line breaks
#        etc.
#        - Robin



from wxPython.wx import *
import xml.parsers.expat, copy

_families = {"default" : wxDEFAULT, "decorative" : wxDECORATIVE, "roman" : wxROMAN,
                "swiss" : wxSWISS, "modern" : wxMODERN}
_styles = {"normal" : wxNORMAL, "slant" : wxSLANT, "italic" : wxITALIC}
_weights = {"normal" : wxNORMAL, "light" : wxLIGHT, "bold" : wxBOLD}

# The next three classes: Renderer, SizeRenderer and DCRenderer are
# what you will need to override to extend the XML language. All of
# the font stuff as well as the subscript and superscript stuff are in
# Renderer.

class Renderer:

    defaultSize = wxNORMAL_FONT.GetPointSize()
    defaultFamily = wxDEFAULT
    defaultStyle = wxNORMAL
    defaultWeight = wxNORMAL
    defaultEncoding = wxFont_GetDefaultEncoding()
    defaultColor = "black"

    def __init__(self, dc=None):
        if dc == None:
            dc = wxMemoryDC()
        self.dc = dc
        self.offsets = [0]
        self.fonts = [{}]

    def startElement(self, name, attrs):
        method = "start_" + name
        if not hasattr(self, method):
            raise ValueError("XML tag '%s' not supported" % name)
        getattr(self, method)(attrs)

    def endElement(self, name):
        method = "end_" + name
        if not hasattr(self, method):
            raise ValueError("XML tag '%s' not supported" % name)
        getattr(self, method)()

    def start_wxFancyString(self, attrs):
        pass

    def end_wxFancyString(self):
        pass

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
                pass
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
        return wxFont(font.get("size", self.defaultSize),
                             font.get("family", self.defaultFamily),
                             font.get("style", self.defaultStyle),
                             font.get("weight", self.defaultWeight),
                             encoding = font.get("encoding", self.defaultEncoding))

    def getCurrentColor(self):
        font = self.fonts[-1]
        return wxNamedColour(font.get("color", self.defaultColor))


class SizeRenderer(Renderer):

    def __init__(self, dc=None):
        Renderer.__init__(self, dc)
        self.width = self.height = 0
        self.minY = self.maxY = 0

    def characterData(self, data):
        self.dc.SetFont(self.getCurrentFont())
        width, height = self.dc.GetTextExtent(data)
        self.width = self.width +  width
        self.minY = min(self.minY, self.offsets[-1])
        self.maxY = max(self.maxY, self.offsets[-1] + height)
        self.height = self.maxY - self.minY

    def start_angle(self, attrs):
        self.characterData("M")

    def end_angle(self):
        pass

class DCRenderer(Renderer):

    def __init__(self, dc=None, x=0, y=0):
        Renderer.__init__(self, dc)
        self.x = x
        self.y = y

    def characterData(self, data):
        self.dc.SetFont(self.getCurrentFont())
        self.dc.SetTextForeground(self.getCurrentColor())
        width, height = self.dc.GetTextExtent(data)
        self.dc.DrawText(data, self.x, self.y + self.offsets[-1])
        self.x = self.x + width

    def start_angle(self, attrs):
        self.dc.SetFont(self.getCurrentFont())
        self.dc.SetPen(wxPen(self.getCurrentColor(), 1))
        width, height, descent, leading = self.dc.GetFullTextExtent("M")
        y = self.y + self.offsets[-1] + height - descent
        self.dc.DrawLine(self.x, y, self.x+width, y)
        self.dc.DrawLine(self.x, y, self.x+width, y-width)
        self.x = self.x + width

    def end_angle(self):
        pass

# This is a rendering function that is primarily used internally,
# although it could be used externally if one had overridden the
# Renderer classes.

def renderToRenderer(str, renderer, enclose=1):
    if enclose:
        str = '<?xml version="1.0"?><wxFancyString>%s</wxFancyString>' % str
    p = xml.parsers.expat.ParserCreate()
    p.returns_unicode = 0
    p.StartElementHandler = renderer.startElement
    p.EndElementHandler = renderer.endElement
    p.CharacterDataHandler = renderer.characterData
    p.Parse(str, 1)


def getExtent(str, dc=None, enclose=1):
    "Return the extent of str"
    renderer = SizeRenderer(dc)
    renderToRenderer(str, renderer, enclose)
    return wxSize(renderer.width, renderer.height)

# This should probably only be used internally....

def getFullExtent(str, dc=None, enclose=1):
    renderer = SizeRenderer(dc)
    renderToRenderer(str, renderer, enclose)
    return renderer.width, renderer.height, -renderer.minY

def renderToBitmap(str, background=None, enclose=1):
    "Return str rendered on a minumum size bitmap"
    dc = wxMemoryDC()
    width, height, dy = getFullExtent(str, dc)
    bmp = wxEmptyBitmap(width, height)
    dc.SelectObject(bmp)
    if background is not None:
        dc.SetBackground(background)
    dc.Clear()
    renderer = DCRenderer(dc, y=dy)
    dc.BeginDrawing()
    renderToRenderer(str, renderer, enclose)
    dc.EndDrawing()
    dc.SelectObject(wxNullBitmap)
    return bmp

def renderToDC(str, dc, x, y, enclose=1):
    "Render str onto a wxDC at (x,y)"
    width, height, dy = getFullExtent(str, dc)
    renderer = DCRenderer(dc, x, y+dy)
    renderToRenderer(str, renderer, enclose)


if __name__ == "__main__":
    str = ('<font style="italic" family="swiss" color="red" weight="bold" >some  |<sup>23</sup> <angle/>text<sub>with <angle/> subscript</sub> </font> some other text'
            '<font family="swiss" color="green" size="40">big green text</font>')
    ID_EXIT  = 102
    class myApp(wxApp):
        def OnInit(self):
            return 1
    app = myApp()
    frame = wxFrame(NULL, -1, "wxFancyText demo", wxDefaultPosition)
    frame.SetClientSize(getExtent(str))
    bmp = renderToBitmap(str, wxCYAN_BRUSH)
    sb = wxStaticBitmap(frame, -1, bmp)
    EVT_MENU(frame, ID_EXIT, frame.Destroy)
    frame.Show(1)
    app.MainLoop()
