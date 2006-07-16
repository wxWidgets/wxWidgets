# AnalogClock's base classes
#   E. A. Tacao <e.a.tacao |at| estadao.com.br>
#   http://j.domaindlx.com/elements28/wxpython/
#   15 Fev 2006, 22:00 GMT-03:00
# Distributed under the wxWidgets license.

from time import strftime, localtime
import math
import wx

from styles import *

#----------------------------------------------------------------------

_targets = [HOUR, MINUTE, SECOND]

#----------------------------------------------------------------------

class Element:
    """Base class for face, hands and tick marks."""

    def __init__(self, idx=0, pos=None, size=None, offset=0, clocksize=None,
                 scale=1, rotate=False, kind=""):

        self.idx = idx
        self.pos = pos
        self.size = size
        self.offset = offset
        self.clocksize = clocksize
        self.scale = scale
        self.rotate = rotate
        self.kind = kind

        self.text = None
        self.angfac = [6, 30][self.kind == "hours"]


    def _pol2rect(self, m, t):
        return m * math.cos(math.radians(t)), m * math.sin(math.radians(t))


    def _rect2pol(self, x, y):
        return math.hypot(x, y), math.degrees(math.atan2(y, x))


    def DrawRotated(self, dc, offset=0):
        pass


    def DrawStraight(self, dc, offset=0):
        pass


    def Draw(self, dc, offset=0):
        if self.rotate:
            self.DrawRotated(dc, offset)
        else:
            self.DrawStraight(dc, offset)


    def RecalcCoords(self, clocksize, centre, scale):
        pass
        
        
    def GetSize(self):
        return self.size


    def GetOffset(self):
        return self.offset


    def GetIsRotated(self, rotate):
        return self.rotate


    def GetMaxSize(self, scale=1):
        return self.size * scale
        
        
    def GetScale(self):
        return self.scale


    def SetIsRotated(self, rotate):
        self.rotate = rotate


    def GetMaxSize(self, scale=1):
        return self.size * scale


    def GetPolygon(self):
        return self.polygon


    def SetPosition(self, pos):
        self.pos = pos


    def SetSize(self, size):
        self.size = size


    def SetOffset(self, offset):
        self.offset = offset


    def SetClockSize(self, clocksize):
        self.clocksize = clocksize


    def SetScale(self, scale):
        self.scale = scale


    def SetIsRotated(self, rotate):
        self.rotate = rotate


    def SetPolygon(self, polygon):
        self.polygon = polygon

#----------------------------------------------------------------------

class ElementWithDyer(Element):
    """Base class for clock face and hands."""

    def __init__(self, **kwargs):
        self.dyer = kwargs.pop("dyer", Dyer())
        Element.__init__(self, **kwargs)


    def GetFillColour(self):
        return self.dyer.GetFillColour()


    def GetBorderColour(self):
        return self.dyer.GetBorderColour()


    def GetBorderWidth(self):
        return self.dyer.GetBorderWidth()


    def GetShadowColour(self):
        return self.dyer.GetShadowColour()

        
    def SetFillColour(self, colour):
        self.dyer.SetFillColour(colour)


    def SetBorderColour(self, colour):
        self.dyer.SetBorderColour(colour)


    def SetBorderWidth(self, width):
        self.dyer.SetBorderWidth(width)
        
        
    def SetShadowColour(self, colour):
        self.dyer.SetShadowColour(colour)

#----------------------------------------------------------------------

class Face(ElementWithDyer):
    """Holds info about the clock face."""

    def __init__(self, **kwargs):
        ElementWithDyer.__init__(self, **kwargs)


    def Draw(self, dc):
        self.dyer.Select(dc)
        dc.DrawCircle(self.pos.x, self.pos.y, self.radius)


    def RecalcCoords(self, clocksize, centre, scale):
        self.radius = min(clocksize.Get()) / 2. - self.dyer.width / 2.
        self.pos = centre

#----------------------------------------------------------------------

class Hand(ElementWithDyer):
    """Holds info about a clock hand."""

    def __init__(self, **kwargs):
        self.lenfac = kwargs.pop("lenfac")
        ElementWithDyer.__init__(self, **kwargs)

        self.SetPolygon([[-1, 0], [0, -1], [1, 0], [0, 4]])


    def Draw(self, dc, end, offset=0):
        radius, centre, r = end
        angle = math.degrees(r)
        polygon = self.polygon[:]
        vscale = radius / max([y for x, y in polygon])

        for i, (x, y) in enumerate(polygon):
            x *= self.scale * self.size
            y *= vscale * self.lenfac
            m, t = self._rect2pol(x, y)
            polygon[i] = self._pol2rect(m, t - angle)

        dc.DrawPolygon(polygon, centre.x + offset, centre.y + offset)


    def RecalcCoords(self, clocksize, centre, scale):
        self.pos = centre
        self.scale = scale

#----------------------------------------------------------------------

class TickSquare(Element):
    """Holds info about a tick mark."""

    def __init__(self, **kwargs):
        Element.__init__(self, **kwargs)


    def Draw(self, dc, offset=0):
        width = height = self.size * self.scale
        x = self.pos.x - width / 2.
        y = self.pos.y - height / 2.

        dc.DrawRectangle(x + offset, y + offset, width, height)

#----------------------------------------------------------------------

class TickCircle(Element):
    """Holds info about a tick mark."""

    def __init__(self, **kwargs):
        Element.__init__(self, **kwargs)


    def Draw(self, dc, offset=0):
        radius = self.size * self.scale / 2.
        x = self.pos.x
        y = self.pos.y

        dc.DrawCircle(x + offset, y + offset, radius)

#----------------------------------------------------------------------

class TickPoly(Element):
    """Holds info about a tick mark."""

    def __init__(self, **kwargs):
        Element.__init__(self, **kwargs)

        self.SetPolygon([[0, 1], [1, 0], [2, 1], [1, 5]])


    def _calcPolygon(self):
        width = max([x for x, y in self.polygon])
        height = max([y for x, y in self.polygon])
        tscale = self.size / max(width, height) * self.scale
        polygon = [(x * tscale, y * tscale) for x, y in self.polygon]

        width = max([x for x, y in polygon])
        height = max([y for x, y in polygon])
        
        return polygon, width, height


    def DrawStraight(self, dc, offset=0):
        polygon, width, height = self._calcPolygon()

        x = self.pos.x - width / 2.
        y = self.pos.y - height / 2.

        dc.DrawPolygon(polygon, x + offset, y + offset)


    def DrawRotated(self, dc, offset=0):
        polygon, width, height = self._calcPolygon()

        angle = 360 - self.angfac * (self.idx + 1)
        r = math.radians(angle)

        for i in range(len(polygon)):
            m, t = self._rect2pol(*polygon[i])
            t -= angle
            polygon[i] = self._pol2rect(m, t)

        x = self.pos.x - math.cos(r) * width / 2. - math.sin(r) * height / 2.
        y = self.pos.y - math.cos(r) * height / 2. + math.sin(r) * width / 2.

        dc.DrawPolygon(polygon, x + offset, y + offset)

#----------------------------------------------------------------------

class TickDecimal(Element):
    """Holds info about a tick mark."""

    def __init__(self, **kwargs):
        Element.__init__(self, **kwargs)

        self.text = "%s" % (self.idx + 1)


    def DrawStraight(self, dc, offset=0):
        width, height = dc.GetTextExtent(self.text)

        x = self.pos.x - width / 2.
        y = self.pos.y - height / 2.

        dc.DrawText(self.text, x + offset, y + offset)


    def DrawRotated(self, dc, offset=0):
        width, height = dc.GetTextExtent(self.text)

        angle = 360 - self.angfac * (self.idx + 1)
        r = math.radians(angle)

        x = self.pos.x - math.cos(r) * width / 2. - math.sin(r) * height / 2.
        y = self.pos.y - math.cos(r) * height / 2. + math.sin(r) * width / 2.

        dc.DrawRotatedText(self.text, x + offset, y + offset, angle)


#----------------------------------------------------------------------

class TickRoman(TickDecimal):
    """Holds info about a tick mark."""

    def __init__(self, **kwargs):
        TickDecimal.__init__(self, **kwargs)

        self.text = ["I","II","III","IV","V",                 \
                     "VI","VII","VIII","IX","X",              \
                     "XI","XII","XIII","XIV","XV",            \
                     "XVI","XVII","XVIII","XIX","XX",         \
                     "XXI","XXII","XXIII","XXIV","XXV",       \
                     "XXVI","XXVII","XXVIII","XXIX","XXX",    \
                     "XXXI","XXXII","XXXIII","XXXIV","XXXV",  \
                     "XXXVI","XXXVII","XXXVIII","XXXIX","XL", \
                     "XLI","XLII","XLIII","XLIV","XLV",       \
                     "XLVI","XLVII","XLVIII","XLIX","L",      \
                     "LI","LII","LIII","LIV","LV",            \
                     "LVI","LVII","LVIII","LIX","LX"][self.idx]

#----------------------------------------------------------------------

class TickBinary(TickDecimal):
    """Holds info about a tick mark."""

    def __init__(self, **kwargs):
        TickDecimal.__init__(self, **kwargs)

        def d2b(n, b=""):
            while n > 0:
                b = str(n % 2) + b; n = n >> 1
            return b.zfill(4)

        self.text = d2b(self.idx + 1)

#----------------------------------------------------------------------

class TickHex(TickDecimal):
    """Holds info about a tick mark."""

    def __init__(self, **kwargs):
        TickDecimal.__init__(self, **kwargs)

        self.text = hex(self.idx + 1)[2:].upper()

#----------------------------------------------------------------------

class TickNone(Element):
    """Holds info about a tick mark."""

    def __init__(self, **kwargs):
        Element.__init__(self, **kwargs)


    def Draw(self, dc, offset=0):
        pass
        
#----------------------------------------------------------------------

class Dyer:
    """Stores info about colours and borders of clock Elements."""

    def __init__(self, border=None, width=0, fill=None, shadow=None):
        """
        self.border (wx.Colour)  border colour
        self.width  (int)        border width
        self.fill   (wx.Colour)  fill colour
        self.shadow (wx.Colour)  shadow colour
        """

        self.border = border or \
                      wx.SystemSettings.GetColour(wx.SYS_COLOUR_WINDOWTEXT)
        self.fill   = fill or \
                      wx.SystemSettings.GetColour(wx.SYS_COLOUR_WINDOWTEXT)
        self.shadow = shadow or \
                      wx.SystemSettings.GetColour(wx.SYS_COLOUR_3DSHADOW)
        self.width  = width


    def Select(self, dc, shadow=False):
        """Selects the current settings into the dc."""

        if not shadow:
            dc.SetPen(wx.Pen(self.border, self.width, wx.SOLID))
            dc.SetBrush(wx.Brush(self.fill, wx.SOLID))
            dc.SetTextForeground(self.fill)
        else:
            dc.SetPen(wx.Pen(self.shadow, self.width, wx.SOLID))
            dc.SetBrush(wx.Brush(self.shadow, wx.SOLID))
            dc.SetTextForeground(self.shadow)


    def GetFillColour(self):
        return self.fill


    def GetBorderColour(self):
        return self.border


    def GetBorderWidth(self):
        return self.width


    def GetShadowColour(self):
        return self.shadow


    def SetFillColour(self, colour):
        self.fill = colour


    def SetBorderColour(self, colour):
        self.border = colour


    def SetBorderWidth(self, width):
        self.width = width


    def SetShadowColour(self, colour):
        self.shadow = colour

#----------------------------------------------------------------------

class HandSet:
    """Manages the set of hands."""

    def __init__(self, parent, h, m, s):
        self.parent = parent

        self.hands = [h, m, s]
        self.radius = 1
        self.centre = wx.Point(1, 1)


    def _draw(self, dc, shadow=False):
        ends = [int(x) for x in strftime("%I %M %S", localtime()).split()]

        flags = [self.parent.clockStyle & flag \
                 for flag in self.parent.allHandStyles]

        a_hand = self.hands[0]

        if shadow:
            offset = self.parent.shadowOffset * a_hand.GetScale()
        else:
            offset = 0

        for i, hand in enumerate(self.hands):
            # Is this hand supposed to be drawn?
            if flags[i]:
                idx = ends[i]
                # Is this the hours hand?
                if i == 0:
                    idx = idx * 5 + ends[1] / 12 - 1
                # else prevent exceptions on leap seconds
                elif idx <= 0 or idx > 60:
                    idx = 59
                # and adjust idx offset for minutes and non-leap seconds 
                else:
                    idx = idx - 1
                angle = math.radians(180 - 6 * (idx + 1))

                hand.dyer.Select(dc, shadow)
                hand.Draw(dc, (self.radius, self.centre, angle), offset)


    def Draw(self, dc):
        if self.parent.clockStyle & SHOW_SHADOWS:
            self._draw(dc, True)
        self._draw(dc)


    def RecalcCoords(self, clocksize, centre, scale):
        self.centre = centre
        [hand.RecalcCoords(clocksize, centre, scale) for hand in self.hands]


    def SetMaxRadius(self, radius):
        self.radius = radius


    def GetSize(self, target):
        r = []
        for i, hand in enumerate(self.hands):
            if _targets[i] & target:
                r.append(hand.GetSize())
        return tuple(r)


    def GetFillColour(self, target):
        r = []
        for i, hand in enumerate(self.hands):
            if _targets[i] & target:
                r.append(hand.GetFillColour())
        return tuple(r)


    def GetBorderColour(self, target):
        r = []
        for i, hand in enumerate(self.hands):
            if _targets[i] & target:
                r.append(hand.GetBorderColour())
        return tuple(r)


    def GetBorderWidth(self, target):
        r = []
        for i, hand in enumerate(self.hands):
            if _targets[i] & target:
                r.append(hand.GetBorderWidth())
        return tuple(r)


    def GetShadowColour(self):
        r = []
        for i, hand in enumerate(self.hands):
            if _targets[i] & target:
                r.append(hand.GetShadowColour())
        return tuple(r)


    def SetSize(self, size, target):
        for i, hand in enumerate(self.hands):
            if _targets[i] & target:
                hand.SetSize(size)


    def SetFillColour(self, colour, target):
        for i, hand in enumerate(self.hands):
            if _targets[i] & target:
                hand.SetFillColour(colour)


    def SetBorderColour(self, colour, target):
        for i, hand in enumerate(self.hands):
            if _targets[i] & target:
                hand.SetBorderColour(colour)


    def SetBorderWidth(self, width, target):
        for i, hand in enumerate(self.hands):
            if _targets[i] & target:
                hand.SetBorderWidth(width)


    def SetShadowColour(self, colour):
        for i, hand in enumerate(self.hands):
            hand.SetShadowColour(colour)

#----------------------------------------------------------------------

class TickSet:
    """Manages a set of tick marks."""

    def __init__(self, parent, **kwargs):
        self.parent = parent
        self.dyer = Dyer()
        self.noe = {"minutes": 60, "hours": 12}[kwargs["kind"]]
        self.font = wx.SystemSettings.GetFont(wx.SYS_DEFAULT_GUI_FONT)

        style = kwargs.pop("style")
        self.kwargs = kwargs
        self.SetStyle(style)


    def _draw(self, dc, shadow=False):
        dc.SetFont(self.font)
        
        a_tick = self.ticks[0]

        if shadow:
            offset = self.parent.shadowOffset * a_tick.GetScale()
        else:
            offset = 0

        clockStyle = self.parent.clockStyle

        for idx, tick in self.ticks.items():
            draw = False

            # Are we a set of hours?
            if self.noe == 12:
                # Should we show all hours ticks?
                if clockStyle & SHOW_HOURS_TICKS:
                    draw = True
                # Or is this tick a quarter and should we show only quarters?
                elif clockStyle & SHOW_QUARTERS_TICKS and not (idx + 1) % 3.:
                    draw = True
            # Are we a set of minutes and minutes should be shown?
            elif self.noe == 60 and clockStyle & SHOW_MINUTES_TICKS:
                # If this tick occupies the same position of an hour/quarter
                # tick, should we still draw it anyway?
                if clockStyle & OVERLAP_TICKS:
                    draw = True
                # Right, sir. I promise I won't overlap any tick.
                else:
                    # Ensure that this tick won't overlap an hour tick.
                    if clockStyle & SHOW_HOURS_TICKS:
                        if (idx + 1) % 5.:
                            draw = True
                    # Ensure that this tick won't overlap a quarter tick.
                    elif clockStyle & SHOW_QUARTERS_TICKS:
                        if (idx + 1) % 15.:
                            draw = True
                    # We're not drawing quarters nor hours, so we can draw all
                    # minutes ticks.
                    else:
                        draw = True

            if draw:
                tick.Draw(dc, offset)


    def Draw(self, dc):
        if self.parent.clockStyle & SHOW_SHADOWS:
            self.dyer.Select(dc, True)
            self._draw(dc, True)
        self.dyer.Select(dc)
        self._draw(dc)


    def RecalcCoords(self, clocksize, centre, scale):
        a_tick = self.ticks[0]

        size = a_tick.GetMaxSize(scale)
        maxsize = size

        # Try to find a 'good' max size for text-based ticks.
        if a_tick.text is not None:
            self.font.SetPointSize(size)
            dc = wx.MemoryDC()
            dc.SelectObject(wx.EmptyBitmap(*clocksize.Get()))
            dc.SetFont(self.font)
            maxsize = size
            for tick in self.ticks.values():
                maxsize = max(*(dc.GetTextExtent(tick.text) + (maxsize,)))

        radius = self.radius = min(clocksize.Get()) / 2. - \
                               self.dyer.width / 2. - \
                               maxsize / 2. - \
                               a_tick.GetOffset() * scale - \
                               self.parent.shadowOffset * scale

        # If we are a set of hours, the number of elements of this tickset is 
        # 12 and ticks are separated by a distance of 30 degrees;
        # if we are a set of minutes, the number of elements of this tickset is
        # 60 and ticks are separated by a distance of 6 degrees.
        angfac = [6, 30][self.noe == 12]

        for i, tick in self.ticks.items():
            tick.SetClockSize(clocksize)
            tick.SetScale(scale)

            deg = 180 - angfac * (i + 1)
            angle = math.radians(deg)

            x = centre.x + radius * math.sin(angle)
            y = centre.y + radius * math.cos(angle)

            tick.SetPosition(wx.Point(x, y))


    def GetSize(self):
        return self.kwargs["size"]


    def GetFillColour(self):
        return self.dyer.GetFillColour()


    def GetBorderColour(self):
        return self.dyer.GetBorderColour()


    def GetBorderWidth(self):
        return self.dyer.GetBorderWidth()


    def GetPolygon(self):
        a_tick = self.ticks.values()[0]
        return a_tick.GetPolygon()


    def GetFont(self):
        return self.font


    def GetOffset(self):
        a_tick = self.ticks[0]
        return a_tick.GetOffset()


    def GetShadowColour(self):
        return self.dyer.GetShadowColour()


    def GetIsRotated(self):
        a_tick = self.ticks[0]
        return a_tick.GetIsRotated()


    def GetStyle(self):
        return self.style


    def SetSize(self, size):
        self.kwargs["size"] = size
        [tick.SetSize(size) for tick in self.ticks.values()]


    def SetFillColour(self, colour):
        self.dyer.SetFillColour(colour)


    def SetBorderColour(self, colour):
        self.dyer.SetBorderColour(colour)


    def SetBorderWidth(self, width):
        self.dyer.SetBorderWidth(width)


    def SetPolygon(self, polygon):
        [tick.SetPolygon(polygon) for tick in self.ticks.values()]


    def SetFont(self, font):
        self.font = font


    def SetOffset(self, offset):
        self.kwargs["offset"] = offset
        [tick.SetOffset(offset) for tick in self.ticks.values()]


    def SetShadowColour(self, colour):
        self.dyer.SetShadowColour(colour)


    def SetIsRotated(self, rotate):
        self.kwargs["rotate"] = rotate
        [tick.SetIsRotated(rotate) for tick in self.ticks.values()]


    def SetStyle(self, style):
        self.style = style
        tickclass = allTickStyles[style]
        self.kwargs["rotate"] = self.parent.clockStyle & ROTATE_TICKS

        self.ticks = {}
        for i in range(self.noe):
            self.kwargs["idx"] = i
            self.ticks[i] = tickclass(**self.kwargs)

#----------------------------------------------------------------------

class Box:
    """Gathers info about the clock face and tick sets."""

    def __init__(self, parent, Face, TicksM, TicksH):
        self.parent = parent
        self.Face   = Face
        self.TicksH = TicksH
        self.TicksM = TicksM


    def GetNiceRadiusForHands(self, centre):
        a_tick = self.TicksM.ticks[0]
        scale = a_tick.GetScale()
        bw = max(self.TicksH.dyer.width / 2. * scale,
                 self.TicksM.dyer.width / 2. * scale)

        mgt = self.TicksM.ticks[59]
        my = mgt.pos.y + mgt.GetMaxSize(scale) + bw

        hgt = self.TicksH.ticks[11]
        hy = hgt.pos.y + hgt.GetMaxSize(scale) + bw

        niceradius = centre.y - max(my, hy)
        return niceradius


    def Draw(self, dc):
        [getattr(self, attr).Draw(dc) \
         for attr in ["Face", "TicksM", "TicksH"]]


    def RecalcCoords(self, size, centre, scale):
        [getattr(self, attr).RecalcCoords(size, centre, scale) \
         for attr in ["Face", "TicksH", "TicksM"]]


    def GetTickSize(self, target):
        r = []
        for i, attr in enumerate(["TicksH", "TicksM"]):
            if _targets[i] & target:
                tick = getattr(self, attr)
                r.append(tick.GetSize())
        return tuple(r)


    def GetTickFillColour(self, target):
        r = []
        for i, attr in enumerate(["TicksH", "TicksM"]):
            if _targets[i] & target:
                tick = getattr(self, attr)
                r.append(tick.GetFillColour())
        return tuple(r)


    def GetTickBorderColour(self, target):
        r = []
        for i, attr in enumerate(["TicksH", "TicksM"]):
            if _targets[i] & target:
                tick = getattr(self, attr)
                r.append(tick.GetBorderColour())
        return tuple(r)


    def GetTickBorderWidth(self, target):
        r = []
        for i, attr in enumerate(["TicksH", "TicksM"]):
            if _targets[i] & target:
                tick = getattr(self, attr)
                r.append(tick.GetBorderWidth())
        return tuple(r)


    def GetTickPolygon(self, target):
        r = []
        for i, attr in enumerate(["TicksH", "TicksM"]):
            if _targets[i] & target:
                tick = getattr(self, attr)
                r.append(tick.GetPolygon())
        return tuple(r)


    def GetTickFont(self, target):
        r = []
        for i, attr in enumerate(["TicksH", "TicksM"]):
            if _targets[i] & target:
                tick = getattr(self, attr)
                r.append(tick.GetFont())
        return tuple(r)


    def GetIsRotated(self):
        a_tickset = self.TicksH
        return a_tickset.GetIsRotated()


    def GetTickOffset(self, target):
        r = []
        for i, attr in enumerate(["TicksH", "TicksM"]):
            if _targets[i] & target:
                tick = getattr(self, attr)
                r.append(tick.GetOffset())
        return tuple(r)


    def GetShadowColour(self):
        a_tickset = self.TicksH
        return a_tickset.GetShadowColour()


    def GetTickStyle(self, target):
        r = []
        for i, attr in enumerate(["TicksH", "TicksM"]):
            if _targets[i] & target:
                tick = getattr(self, attr)
                r.append(tick.GetStyle())
        return tuple(r)


    def SetTickSize(self, size, target):
        for i, attr in enumerate(["TicksH", "TicksM"]):
            if _targets[i] & target:
                tick = getattr(self, attr)
                tick.SetSize(size)


    def SetTickFillColour(self, colour, target):
        for i, attr in enumerate(["TicksH", "TicksM"]):
            if _targets[i] & target:
                tick = getattr(self, attr)
                tick.SetFillColour(colour)


    def SetTickBorderColour(self, colour, target):
        for i, attr in enumerate(["TicksH", "TicksM"]):
            if _targets[i] & target:
                tick = getattr(self, attr)
                tick.SetBorderColour(colour)


    def SetTickBorderWidth(self, width, target):
        for i, attr in enumerate(["TicksH", "TicksM"]):
            if _targets[i] & target:
                tick = getattr(self, attr)
                tick.SetBorderWidth(width)


    def SetTickPolygon(self, polygon, target):
        for i, attr in enumerate(["TicksH", "TicksM"]):
            if _targets[i] & target:
                tick = getattr(self, attr)
                tick.SetPolygon(polygon)


    def SetTickFont(self, font, target):
        fs = font.GetNativeFontInfoDesc()
        for i, attr in enumerate(["TicksH", "TicksM"]):
            if _targets[i] & target:
                tick = getattr(self, attr)
                tick.SetFont(wx.FontFromNativeInfoString(fs))


    def SetIsRotated(self, rotate):
        [getattr(self, attr).SetIsRotated(rotate) \
         for attr in ["TicksH", "TicksM"]]


    def SetTickOffset(self, offset, target):
        for i, attr in enumerate(["TicksH", "TicksM"]):
            if _targets[i] & target:
                tick = getattr(self, attr)
                tick.SetOffset(offset)


    def SetShadowColour(self, colour):
        for attr in ["TicksH", "TicksM"]:
            tick = getattr(self, attr)
            tick.SetShadowColour(colour)


    def SetTickStyle(self, style, target):
        for i, attr in enumerate(["TicksH", "TicksM"]):
            if _targets[i] & target:
                tick = getattr(self, attr)
                tick.SetStyle(style)

#----------------------------------------------------------------------

# Relationship between styles and ticks class names.
allTickStyles = {TICKS_BINARY:  TickBinary,
                 TICKS_CIRCLE:  TickCircle,
                 TICKS_DECIMAL: TickDecimal,
                 TICKS_HEX:     TickHex,
                 TICKS_NONE:    TickNone,
                 TICKS_POLY:    TickPoly,
                 TICKS_ROMAN:   TickRoman,
                 TICKS_SQUARE:  TickSquare}


#
##
### eof
