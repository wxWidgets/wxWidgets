# --------------------------------------------------------------------------- #
# FOLDPANELBAR wxPython IMPLEMENTATION
# Ported From Jorgen Bodde & Julian Smart (Extended Demo) C++ Code By:
#
# Andrea Gavana, @ 23 Mar 2005
# Latest Revision: 28 Mar 2005, 22.30 CET
#
#
# TODO List
#
# All The C++ TODOs Are Still Alive. I Am Not Able to Read Jorges's Mind
# So I Don't Really Know What Will Be The New Features/Additions He Will
# Make On His Code. At The Moment They Are:
#
# 1. OnPaint Function In CaptionBar Class:
# TODO: Maybe First A Memory Dc Should Draw All, And Then Paint It On The
# Caption. This Way A Flickering Arrow During Resize Is Not Visible.
#
# 2. OnChar Function In CaptionBar Class:
# TODO: This Is Easy To Do But I Don't Have Any Useful Idea On Which Kind
# Of Features To Add. Does Anyone Have An Intelligent Idea?
#
# 3. AddFoldPanelWindow Function In FoldPanelBar Class:
# TODO: Take Old And New Heights, And If Difference, Reposition All The
# Lower Panels. This Is Because The User Can Add New wxWindow Controls
# Somewhere In Between When Other Panels Are Already Present.
# Don't Know What It Means. Probably Is My Poor English...
#
# 4. OnSizePanel Function In FoldPanelBar Class:
# TODO: A Smart Way To Check Wether The Old - New Width Of The
# Panel Changed, If So No Need To Resize The Fold Panel Items
#
# 5. Implementing Styles Like FPB_SINGLE_FOLD and FPB_EXCLUSIVE_FOLD
# TODO: Jorgen Has Left Undone These Jobs. I Don't Really Get What They
# Should Supposed To Do, So If Someone Could Enlight Me, Please Let Me Know.
#
#
# For The Original TODO List From Jorgen, Please Refer To:
# http://www.solidsteel.nl/jorg/components/foldpanel/wxFoldPanelBar.php#todo_list
#
#
#
# For All Kind Of Problems, Requests Of Enhancements And Bug Reports, Please
# Write To Me At:
#
# andrea.gavana@agip.it
# andrea_gavan@tin.it
#
# Or, Obviously, To The wxPython Mailing List!!!
#
#
# End Of Comments
# --------------------------------------------------------------------------- #


"""Description:

The FoldPanelBar is a control that contains multiple panels (FoldPanel items)
that can be expanded or collapsed. The captionbar of the FoldPanel can be
customized by setting it to a horizontal gradient style, vertical gradient style,
a single color, a rectangle or filled rectangle. The FoldPanel items can be
collapsed in place or to the bottom of the control. The wxWindow derived controls
can be added dynamically, and separated by separator lines. 
FoldPanelBar is freeware and distributed under the wxPython license. 
 
   
- How does it work:

The internals of the FoldPanelBar is a list of FoldPanelItem objects. Through
the reference of FoldPanel these panels can be controlled by adding new controls
to a FoldPanel or adding new FoldPanels to the FoldPanelBar. 
The CaptionBar fires events to the parent (container of all panel items) when a
sub-panel needs resizing (either folding or expanding). The fold or expand process
is simply a resize of the panel so it looks like all controls on it are gone. All
controls are still child of the FoldPanel they are located on. If they don't
handle the event (and they won't) then the owner of the FoldPanelBar gets the
events. This is what you need to handle the controls. There isn't much to it just
a lot of calculations to see what panel belongs where. There are no sizers
involved in the panels, everything is purely x-y positioning. 


- What can it do and what not?

    a) What it can do:
        Run-time addition of panels (no deletion just yet) 
        Run time addition of controls to the panel (it will be resized accordingly)
        Creating panels in collapsed mode or expanded mode 
        Various modes of caption behaviour and filling to make it more appealing 
        Panels can be folded and collapsed (or all of them) to allow more space
        
    b) What it cannot do:

        Selection of a panel like in a list ctrl 
        Dragging and dropping the panels 
        Re-ordering the panels (not yet)  

 
- Supported platforms

FoldPanelBar is supported on the following platforms: 
Windows (Verified on Windows XP, 2000)
Linux/Unix (GTK2) (Thanks To Toni Brkic And Robin Dunn)
Mac OSX (Thanks To Robin Dunn For The CaptionBar Size Patch)


Latest Revision: Andrea Gavana @ 30 Mar 2005, 22.30 CET

"""

import wx

#----------------------------------------------------------------------
# Collapsed And Expanded Bitmap Images
# Created With img2py.py 
#----------------------------------------------------------------------

def GetCollapsedIconData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x007IDAT8\x8dcddbf\xa0\x040Q\xa4{\xf0\x1b\xf0\xff\xdf\xdf\xff\x03\xe7\
\x02\x98\xed\x84\\A\x1b\x17\xa0\xdb\x8a\xcf\x15\xd4w\x01.\xdbp\x89S\xec\x02\
\xc6\xd1\xbc\xc0\x00\x00\x9a\xf5\x1b\xfa\xf9m$?\x00\x00\x00\x00IEND\xaeB`\
\x82' 

def GetCollapsedIconBitmap():
    return wx.BitmapFromImage(GetCollapsedIconImage())

def GetCollapsedIconImage():
    import  cStringIO
    stream = cStringIO.StringIO(GetCollapsedIconData())
    return wx.ImageFromStream(stream)

#----------------------------------------------------------------------
def GetExpandedIconData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x00BIDAT8\x8dcddbf\xa0\x040Q\xa4{P\x18\xc0\x82.\xf0\xff\xdf\xdf\xff\xb8\
\x143213R\xdd\x05\x18\x06`\xb3\x05\x9f8m\x02\x11\xdd6\\\xb6\xd3\xce\x05\xc8\
\xb6\xe2\xb3\x9d*.`\x1c\xcd\x0b\x0c\x00\x9e\xbc\x04W\x19\xcfa\xb5\x00\x00\
\x00\x00IEND\xaeB`\x82' 

def GetExpandedIconBitmap():
    return wx.BitmapFromImage(GetExpandedIconImage())

def GetExpandedIconImage():
    import  cStringIO
    stream = cStringIO.StringIO(GetExpandedIconData())
    return wx.ImageFromStream(stream)

#----------------------------------------------------------------------

#----------------------------------------------------------------------
# FOLDPANELBAR Starts Here
#----------------------------------------------------------------------

# CAPTIONBAR STYLES
#
#- CAPTIONBAR_GRADIENT_V: Draws a vertical gradient from top to bottom
#- CAPTIONBAR_GRADIENT_H: Draws a horizontal gradient from left to right
#- CAPTIONBAR_SINGLE: Draws a single filled rectangle to draw the caption
#- CAPTIONBAR_RECTANGLE: Draws a single colour with a rectangle around the caption
#- CAPTIONBAR_FILLED_RECTANGLE: Draws a filled rectangle and a border around it

CAPTIONBAR_NOSTYLE            = 0
CAPTIONBAR_GRADIENT_V         = 1
CAPTIONBAR_GRADIENT_H         = 2
CAPTIONBAR_SINGLE             = 3
CAPTIONBAR_RECTANGLE          = 4
CAPTIONBAR_FILLED_RECTANGLE   = 5

FPB_EXTRA_X = 10
FPB_EXTRA_Y = 4

# pixels of the bmp to be aligned from the right filled with space
FPB_BMP_RIGHTSPACE = 2

# Not yet supported but added for future reference. Single fold forces
# other panels to close when they are open, and only opens the current panel.
# This will allow the open panel to gain the full size left in the client area
FPB_SINGLE_FOLD = 0x0001

# All panels are stacked to the bottom. When they are expanded again they
# show up at the top
FPB_COLLAPSE_TO_BOTTOM = 0x0002

# Not yet supported, but added for future reference. Single fold plus panels
# will be stacked at the bottom
FPB_EXCLUSIVE_FOLD = FPB_SINGLE_FOLD | FPB_COLLAPSE_TO_BOTTOM

# Orientation Flag 
FPB_HORIZONTAL = wx.HORIZONTAL
FPB_VERTICAL = wx.VERTICAL  

# Default Extrastyle of the FoldPanelBar 
FPB_DEFAULT_EXTRASTYLE = 0
# Default style of the FoldPanelBar 
FPB_DEFAULT_STYLE = wx.TAB_TRAVERSAL | wx.NO_BORDER

# FoldPanelItem default settings
FPB_ALIGN_LEFT = 0 
FPB_ALIGN_WIDTH = 1

FPB_DEFAULT_LEFTSPACING = 5
FPB_DEFAULT_RIGHTSPACING = 10
FPB_DEFAULT_SPACING = 8

FPB_DEFAULT_LEFTLINESPACING = 2
FPB_DEFAULT_RIGHTLINESPACING = 2


# ------------------------------------------------------------------------------ #
# class CaptionBarStyle
#    This class encapsulates the styles you wish to set for the CaptionBar
#    (this is the part of the FoldPanel where the caption is displayed). It can
#    either be applied at creation time be reapplied when styles need to be
#    changed.
#
#    At construction time, all styles are set to their default transparency.
#    This means none of the styles will be applied to the CaptionBar in question,
#    meaning it will be created using the default internals. When setting i.e
#    the color, font or panel style, these styles become active to be used.
# ------------------------------------------------------------------------------ #

class CaptionBarStyle:

    def __init__(self):
        """ Default constructor for this class."""
        
        self.ResetDefaults()


    def ResetDefaults(self):
        """ Resets Default CaptionBarStyle."""
        
        self._firstColourUsed = False
        self._secondColourUsed = False
        self._textColourUsed = False
        self._captionFontUsed = False
        self._captionStyleUsed = False
        self._captionStyle = CAPTIONBAR_GRADIENT_V


    # ------- CaptionBar Font -------
    
    def SetCaptionFont(self, font):
        """
        Sets font for the caption bar.

        If this is not set, the font property is undefined and will not
        be used. Use CaptionFontUsed() to check if this style is used.
        """
        
        self._captionFont = font
        self._captionFontUsed = True


    def CaptionFontUsed(self):
        """ Checks if the caption bar font is set. """
        
        return self._captionFontUsed


    def GetCaptionFont(self):
        """
        Returns the font for the caption bar.

        Please be warned this will result in an assertion failure when
        this property is not previously set.
        See also SetCaptionFont(), CaptionFontUsed()
        """
        
        return self._captionFont


    # ------- First Colour -------
    
    def SetFirstColour(self, colour):
        """
        Sets first colour for the caption bar.

        If this is not set, the colour property is undefined and will not
        be used. Use FirstColourUsed() to check if this style is used.
        """
        
        self._firstColour = colour
        self._firstColourUsed = True


    def FirstColourUsed(self):
        """ Checks if the first colour of the caption bar is set."""
        
        return self._firstColourUsed


    def GetFirstColour(self):
        """
        Returns the first colour for the caption bar.

        Please be warned this will result in an assertion failure
        when this property is not previously set.
        See also SetCaptionFirstColour(), CaptionFirstColourUsed()
        """
        
        return self._firstColour


    # ------- Second Colour -------
    
    def SetSecondColour(self, colour):
        """
        Sets second colour for the caption bar.

        If this is not set, the colour property is undefined and will not
        be used. Use SecondColourUsed() to check if this style is used.
        """
        
        self._secondColour = colour
        self._secondColourUsed = True


    def SecondColourUsed(self):
        """ Checks if the second colour of the caption bar is set."""
        
        return self._secondColourUsed

    def GetSecondColour(self):
        """
        Returns the second colour for the caption bar.

        Please be warned this will result in an assertion failure
        when this property is not previously set.
        See also SetCaptionSecondColour(), CaptionSecondColourUsed()
        """
        
        return self._secondColour


    # ------- Caption Text Colour -------
    
    def SetCaptionColour(self, colour):
        """
        Sets caption colour for the caption bar.

        If this is not set, the colour property is undefined and will not
        be used. Use CaptionColourUsed() to check if this style is used.
        """
        
        self._textColour = colour
        self._textColourUsed = True


    def CaptionColourUsed(self):
        """ Checks if the caption colour of the caption bar is set."""
        
        return self._textColourUsed


    def GetCaptionColour(self):
        """
        Returns the caption colour for the caption bar.

        Please be warned this will result in an assertion failure
        when this property is not previously set.
        See also SetCaptionColour(), CaptionColourUsed()
        """
        
        return self._textColour


    # ------- CaptionStyle  -------
    
    def SetCaptionStyle(self, style):
        """
        Sets caption style for the caption bar.

        If this is not set, the property is undefined and will not
        be used. Use CaptionStyleUsed() to check if this style is used.
        The following styles can be applied:
            - CAPTIONBAR_GRADIENT_V: Draws a vertical gradient from top to bottom
            - CAPTIONBAR_GRADIENT_H: Draws a horizontal gradient from left to right
            - CAPTIONBAR_SINGLE: Draws a single filled rectangle to draw the caption
            - CAPTIONBAR_RECTANGLE: Draws a single colour with a rectangle around the caption
            - CAPTIONBAR_FILLED_RECTANGLE: Draws a filled rectangle and a border around it
        """
    
        self._captionStyle = style
        self._captionStyleUsed = True
        

    def CaptionStyleUsed(self):
        """ Checks if the caption style of the caption bar is set."""
        
        return self._captionStyleUsed


    def GetCaptionStyle(self):
        """
        Returns the caption style for the caption bar.
        
        Please be warned this will result in an assertion failure
        when this property is not previously set.
        See also SetCaptionStyle(), CaptionStyleUsed()
        """
        
        return self._captionStyle


#-----------------------------------#
#        CaptionBarEvent
#-----------------------------------#
wxEVT_CAPTIONBAR = wx.NewEventType()
EVT_CAPTIONBAR = wx.PyEventBinder(wxEVT_CAPTIONBAR, 0)


# ---------------------------------------------------------------------------- #
# class CaptionBarEvent
#    This event will be sent when a EVT_CAPTIONBAR is mapped in the parent.
#    It is to notify the parent that the bar is now in collapsed or expanded
#    state. The parent should re-arrange the associated windows accordingly
# ---------------------------------------------------------------------------- #

class CaptionBarEvent(wx.PyCommandEvent):
    
    def __init__(self, evtType):
        """ Default Constructor For This Class."""
        
        wx.PyCommandEvent.__init__(self, evtType)

        
    def GetFoldStatus(self):
        """ Returns Wether The Bar Is Expanded Or Collapsed. True Means Expanded."""
        
        return not self._bar.IsCollapsed()


    def GetBar(self):
        """ Returns The CaptionBar Selected."""
        
        return self._bar


    def SetTag(self, tag):
        """ Assign A Tag To The Selected CaptionBar."""
        
        self._tag = tag


    def GetTag(self):
        """ Returns The Tag Assigned To The Selected CaptionBar."""
        
        return self._tag


    def SetBar(self, bar):
        """
        Sets The Bar Associated With This Event.

        Should Not Used By Any Other Then The Originator Of The Event.
        """
        
        self._bar = bar


# -------------------------------------------------------------------------------- #
# class CaptionBar
#    This class is a graphical caption component that consists of a caption and
#    a clickable arrow.
#
#    The CaptionBar fires an event EVT_CAPTIONBAR which is a CaptionBarEvent.
#    This event can be caught and the parent window can act upon the collapsed
#    or expanded state of the bar (which is actually just the icon which changed).
#    The parent panel can reduce size or expand again.
# -------------------------------------------------------------------------------- #

class CaptionBar(wx.Window):

    # Define Empty CaptionBar Style
    EmptyCaptionBarStyle = CaptionBarStyle()
    
    def __init__(self, parent, id, pos, size, caption="",
                 foldIcons=None, cbstyle=EmptyCaptionBarStyle,
                 rightIndent=FPB_BMP_RIGHTSPACE,
                 iconWidth=16, iconHeight=16, collapsed=False):
        """ Default Class Constructor."""
        
        wx.Window.__init__(self, parent, wx.ID_ANY, pos=wx.DefaultPosition,
                           size=(20,20), style=wx.NO_BORDER)

        self._controlCreated = False
        self._collapsed = collapsed
        self.ApplyCaptionStyle(cbstyle, True)

        if foldIcons is None:
            foldIcons = wx.ImageList(16, 16)

            bmp = GetExpandedIconBitmap()
            foldIcons.Add(bmp)
            bmp = GetCollapsedIconBitmap()
            foldIcons.Add(bmp)

        # set initial size
        if foldIcons:
            assert foldIcons.GetImageCount() > 1
            iconWidth, iconHeight = foldIcons.GetSize(0)

        self._caption = caption
        self._foldIcons = foldIcons
        self._style = cbstyle
        self._rightIndent = rightIndent
        self._iconWidth = iconWidth
        self._iconHeight = iconHeight
        self._oldSize = wx.Size(20,20)

        self._controlCreated = True

        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_MOUSE_EVENTS, self.OnMouseEvent)
        self.Bind(wx.EVT_CHAR, self.OnChar)
        

    def ApplyCaptionStyle(self, cbstyle=EmptyCaptionBarStyle, applyDefault=True):
        """ Applies the style defined in cbstyle to the CaptionBar."""
        
        newstyle = cbstyle
        
        if applyDefault:

            # get first colour from style or make it default
            if not newstyle.FirstColourUsed():
                newstyle.SetFirstColour(wx.WHITE)

            # get second colour from style or make it default
            if not newstyle.SecondColourUsed():
                # make the second colour slightly darker then the background
                color = self.GetParent().GetBackgroundColour()
                r, g, b = int(color.Red()), int(color.Green()), int(color.Blue())
                color = ((r >> 1) + 20, (g >> 1) + 20, (b >> 1) + 20)
                newstyle.SetSecondColour(wx.Colour(color[0], color[1], color[2]))

            # get text colour
            if not newstyle.CaptionColourUsed():
                newstyle.SetCaptionColour(wx.BLACK)

            # get font colour
            if not newstyle.CaptionFontUsed():
                newstyle.SetCaptionFont(self.GetParent().GetFont())

            # apply caption style
            if not newstyle.CaptionStyleUsed():
                newstyle.SetCaptionStyle(CAPTIONBAR_GRADIENT_V)

        self._style = newstyle
        

    def SetCaptionStyle(self, cbstyle=EmptyCaptionBarStyle, applyDefault=True):
        """
        Sets CaptionBar styles with CapionBarStyle class.

        All styles that are actually set, are applied. If you set applyDefault
        to True, all other (not defined) styles will be set to default. If it
        is False, the styles which are not set in the CaptionBarStyle will be
        ignored.
        """
        
        self.ApplyCaptionStyle(cbstyle, applyDefault)
        self.Refresh()

    
    def GetCaptionStyle(self):
        """
        Returns the current style of the captionbar in a CaptionBarStyle class.

        This can be used to change and set back the changes.
        """
        
        return self._style


    def IsCollapsed(self):
        """
        Returns wether the status of the bar is expanded or collapsed. """
        
        return self._collapsed
    

    def SetRightIndent(self, pixels):
        """
        Sets the amount of pixels on the right from which the bitmap is trailing.

        If this is 0, it will be drawn all the way to the right, default is
        equal to FPB_BMP_RIGHTSPACE. Assign this before assigning an image
        list to prevent a redraw.
        """

        assert pixels >= 0
        self._rightIndent = pixels
        if self._foldIcons:
            self.Refresh()


    def Collapse(self):
        """
        This sets the internal state / representation to collapsed.

        This does not trigger a CaptionBarEvent to be sent to the parent.
        """
        
        self._collapsed = True
        self.RedrawIconBitmap()


    def Expand(self):
        """
        This sets the internal state / representation to expanded.

        This does not trigger a CaptionBarEvent to be sent to the parent.
        """
        
        self._collapsed = False
        self.RedrawIconBitmap()
        

    def SetBoldFont(self):
        """ Sets the CaptionBarFont weight to BOLD."""
        
        self.GetFont().SetWeight(wx.BOLD)


    def SetNormalFont(self):
        """ Sets the CaptionBarFont weight to NORMAL."""
        
        self.GetFont().SetWeight(wx.NORMAL)


    def IsVertical(self):
        """
        Returns wether the CaptionBar Has Default Orientation Or Not.

        Default is vertical.
        """
        
        fld = self.GetParent().GetGrandParent()
        if isinstance(fld, FoldPanelBar):
            return self.GetParent().GetGrandParent().IsVertical()
        else:
            raise "ERROR: Wrong Parent " + repr(fld)

        
    def OnPaint(self, event):
        """ The paint event for flat or gradient fill. """
        
        if not self._controlCreated:
            event.Skip()
            return
        
        dc = wx.PaintDC(self)
        wndRect = self.GetRect()
        vertical = self.IsVertical()

        # TODO: Maybe first a memory DC should draw all, and then paint it on
        # the caption. This way a flickering arrow during resize is not visible
        
        self.FillCaptionBackground(dc)
        dc.SetFont(self._style.GetCaptionFont())

        if vertical:
            dc.DrawText(self._caption, 4, FPB_EXTRA_Y/2)
        else:
            dc.DrawRotatedText(self._caption, FPB_EXTRA_Y/2,
                               wndRect.GetBottom() - 4, 90)

        # draw small icon, either collapsed or expanded
        # based on the state of the bar. If we have any bmp's

        if self._foldIcons:

            index = self._collapsed
            
            if vertical:
                drw = wndRect.GetRight() - self._iconWidth - self._rightIndent
                self._foldIcons.Draw(index, dc, drw,
                                     (wndRect.GetHeight() - self._iconHeight)/2,
                                     wx.IMAGELIST_DRAW_TRANSPARENT)
            else:
                self._foldIcons.Draw(index, dc,
                                     (wndRect.GetWidth() - self._iconWidth)/2,
                                     self._rightIndent, wx.IMAGELIST_DRAW_TRANSPARENT)

##        event.Skip()
        

    def FillCaptionBackground(self, dc):
        """ Fills the background of the caption with either a gradient or a solid color. """

        style = self._style.GetCaptionStyle()

        if style == CAPTIONBAR_GRADIENT_V:
            if self.IsVertical():
                self.DrawVerticalGradient(dc, self.GetRect())
            else:
                self.DrawHorizontalGradient(dc, self.GetRect())

        elif style == CAPTIONBAR_GRADIENT_H:
            if self.IsVertical():
                self.DrawHorizontalGradient(dc, self.GetRect())
            else:
                self.DrawVerticalGradient(dc, self.GetRect())
            
        elif style == CAPTIONBAR_SINGLE:
            self.DrawSingleColour(dc, self.GetRect())
        elif style == CAPTIONBAR_RECTANGLE or style == CAPTIONBAR_FILLED_RECTANGLE:
            self.DrawSingleRectangle(dc, self.GetRect())
        else:
            raise "STYLE Error: Undefined Style Selected: " + repr(style)


    def OnMouseEvent(self, event):
        """
        Catches the mouse click-double click.
        
        If clicked on the arrow (single) or double on the caption we change state
        and an event must be fired to let this panel collapse or expand.
        """
        
        send_event = False

        if event.LeftDown() and self._foldIcons:

            pt = event.GetPosition()
            rect = self.GetRect()
            vertical = self.IsVertical()

            drw = (rect.GetWidth() - self._iconWidth - self._rightIndent)
            if vertical and pt.x > drw or not vertical and \
               pt.y < (self._iconHeight + self._rightIndent):
                send_event = True

        elif event.LeftDClick():
            send_event = True

        # send the collapse, expand event to the parent
        
        if send_event:
            event = CaptionBarEvent(wxEVT_CAPTIONBAR)
            event.SetBar(self)
            self.GetEventHandler().ProcessEvent(event)
            event.Skip()
        

    def OnChar(self, event):
        """ Unused Methods. Any Ideas?!?"""
        
        # TODO: Anything here?
        event.Skip()


    def DoGetBestSize(self):
        """ Returns the best size for this panel, based upon the font assigned
        to this window, and the caption string"""
        
        if self.IsVertical():
            x, y = self.GetTextExtent(self._caption)
        else:
            y, x = self.GetTextExtent(self._caption)

        if x < self._iconWidth:
            x = self._iconWidth

        if y < self._iconHeight:
            y = self._iconHeight

        # TODO: The extra FPB_EXTRA_X constants should be adjustable as well

        return wx.Size(x + FPB_EXTRA_X, y + FPB_EXTRA_Y)


    def DrawVerticalGradient(self, dc, rect):
        """ Gradient fill from colour 1 to colour 2 with top to bottom. """

        if  rect.height < 1 or rect.width < 1:
            return

        dc.SetPen(wx.TRANSPARENT_PEN)

        # calculate gradient coefficients
        col2 = self._style.GetSecondColour()
        col1 = self._style.GetFirstColour()

        r1, g1, b1 = int(col1.Red()), int(col1.Green()), int(col1.Blue())
        r2, g2, b2 = int(col2.Red()), int(col2.Green()), int(col2.Blue())

        flrect = float(rect.height)

        rstep = float((r2 - r1)) / flrect
        gstep = float((g2 - g1)) / flrect
        bstep = float((b2 - b1)) / flrect

        rf, gf, bf = 0, 0, 0

        for y in range(rect.y, rect.y + rect.height):
            currCol = (r1 + rf, g1 + gf, b1 + bf)
                
            dc.SetBrush(wx.Brush(currCol, wx.SOLID))
            dc.DrawRectangle(rect.x, rect.y + (y - rect.y), rect.width, rect.height)
            rf = rf + rstep
            gf = gf + gstep
            bf = bf + bstep


    def DrawHorizontalGradient(self, dc, rect):
        """ Gradient fill from colour 1 to colour 2 with left to right. """

        if rect.height < 1 or rect.width < 1:
            return

        dc.SetPen(wx.TRANSPARENT_PEN)

        # calculate gradient coefficients
        col2 = self._style.GetSecondColour()
        col1 = self._style.GetFirstColour()

        r1, g1, b1 = int(col1.Red()), int(col1.Green()), int(col1.Blue())
        r2, g2, b2 = int(col2.Red()), int(col2.Green()), int(col2.Blue())

        flrect = float(rect.width)

        rstep = float((r2 - r1)) / flrect
        gstep = float((g2 - g1)) / flrect
        bstep = float((b2 - b1)) / flrect

        rf, gf, bf = 0, 0, 0
        
        for x in range(rect.x, rect.x + rect.width):
            currCol = (r1 + rf, g1 + gf, b1 + bf)
                
            dc.SetBrush(wx.Brush(currCol, wx.SOLID))
            dc.DrawRectangle(rect.x + (x - rect.x), rect.y, 1, rect.height)
            rf = rf + rstep
            gf = gf + gstep
            bf = bf + bstep


    def DrawSingleColour(self, dc, rect):
        """ Single colour fill. This is the most easy one to find. """

        if rect.height < 1 or rect.width < 1:
            return

        dc.SetPen(wx.TRANSPARENT_PEN)

        # draw simple rectangle
        dc.SetBrush(wx.Brush(self._style.GetFirstColour(), wx.SOLID))
        dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height)
        

    def DrawSingleRectangle(self, dc, rect):
        """ Single rectangle. This is the most easy one to find. """
        
        if rect.height < 2 or rect.width < 1:
            return

        # single frame, set up internal fill colour

        if self._style.GetCaptionStyle() == CAPTIONBAR_RECTANGLE:
            color = self.GetParent().GetBackgroundColour()
            br = wx.Brush(color, wx.SOLID)
        else:
            color = self._style.GetFirstColour()
            br = wx.Brush(color, wx.SOLID)

        # setup the pen frame

        pen = wx.Pen(self._style.GetSecondColour())
        dc.SetPen(pen)
        dc.SetBrush(br)
        dc.DrawRectangle(rect.x, rect.y, rect.width, rect.height - 1)

        bgpen = wx.Pen(self.GetParent().GetBackgroundColour())
        dc.SetPen(bgpen)
        dc.DrawLine(rect.x, rect.y + rect.height - 1, rect.x + rect.width,
                    rect.y + rect.height - 1)


    def OnSize(self, event):
        """ Handles the size events for the CaptionBar."""

        if not self._controlCreated:
            event.Skip()
            return
        
        size = event.GetSize()

        if self._foldIcons:

            # What I am doing here is simply invalidating the part of the window
            # exposed. So when I make a rect with as width the newly exposed part,
            # and the x,y of the old window size origin, I don't need a bitmap
            # calculation in it, or do I ? The bitmap needs redrawing anyway. 
            # Leave it like this until I figured it out.

            # set rect to redraw as old bitmap area which is entitled to redraw

            rect = wx.Rect(size.GetWidth() - self._iconWidth - self._rightIndent, 0,
                           self._iconWidth + self._rightIndent,
                           self._iconWidth + self._rightIndent)
            
            # adjust rectangle when more is slided so we need to redraw all
            # the old stuff but not all (ugly flickering)

            diffX = size.GetWidth() - self._oldSize.GetWidth()
            
            if diffX > 1:
                
                # adjust the rect with all the crap to redraw

                rect.SetWidth(rect.GetWidth() + diffX + 10)
                rect.SetX(rect.GetX() - diffX - 10)

            self.RefreshRect(rect)
            
        else:
            
            rect = self.GetRect()
            self.RefreshRect(rect)

        self._oldSize = size
        

    def RedrawIconBitmap(self):
        """ Redraws the icons (if they exists). """

        if self._foldIcons:
        
            # invalidate the bitmap area and force a redraw

            rect = self.GetRect()

            rect.SetX(rect.GetWidth() - self._iconWidth - self._rightIndent)
            rect.SetWidth(self._iconWidth + self._rightIndent)
            self.RefreshRect(rect)


# ---------------------------------------------------------------------------------- #
# class FoldPanelBar
#    The FoldPanelBar is a class which can maintain a list of collapsable panels.
#    Once a panel is collapsed, only it's panel bar is visible to the user. This
#    will provide more space for the other panels, or allow the user to close
#    panels which are not used often to get the most out of the work area.
#
#    This control is easy to use. Simply create it as a child for a panel or
#    sash window, and populate panels with FoldPanelBar.AddFoldPanel(). Then use
#    the FoldPanelBar.AddFoldPanelWindow() to add wxWindow derived controls to the
#    current fold panel. Use FoldPanelBar.AddFoldPanelSeparator() to put separators
#    between the groups of controls that need a visual separator to group them
#    together. After all is constructed, the user can fold the panels by
#    doubleclicking on the bar or single click on the arrow, which will indicate
#    the collapsed or expanded state.
# ---------------------------------------------------------------------------------- #

class FoldPanelBar(wx.Panel):

    # Define Empty CaptionBar Style
    EmptyCaptionBarStyle = CaptionBarStyle()
    
    def __init__(self, parent, id=-1, pos=wx.DefaultPosition, size=wx.DefaultSize,
                 style=FPB_DEFAULT_STYLE, extraStyle=FPB_DEFAULT_EXTRASTYLE): 
        """ Default Class Constructor. """
        
        self._controlCreated = False
        self._extraStyle = extraStyle
        
        # make sure there is any orientation
        if style & FPB_HORIZONTAL != FPB_HORIZONTAL:
            style = style | FPB_VERTICAL

        if style & FPB_HORIZONTAL == 4:
            self._isVertical = False
        else:
            self._isVertical = True

        
        # create the panel (duh!). This causes a size event, which we are going
        # to skip when we are not initialised

        wx.Panel.__init__(self, parent, id, pos, size, style)

        # the fold panel area

        self._foldPanel = wx.Panel(self, wx.ID_ANY, pos, size,
                                   wx.NO_BORDER | wx.TAB_TRAVERSAL)

        self._controlCreated = True
        self._panels = []

        self.Bind(EVT_CAPTIONBAR, self.OnPressCaption)
        self.Bind(wx.EVT_SIZE, self.OnSizePanel)
        

    def AddFoldPanel(self, caption="", collapsed=False, foldIcons=None,
                     cbstyle=EmptyCaptionBarStyle):
        """
        Adds a fold panel to the list of panels.

        If the flag collapsed is set to True, the panel is collapsed initially.
        The FoldPanel item which is returned, can be used as a reference to
        perform actions upon the fold panel like collapsing it, expanding it,
        or deleting it from the list.

        Use this foldpanel to add windows to it. Please consult
        FoldPanelBar.AddFoldPanelWindow() and
        FoldPanelBar.AddFoldPanelSeparator() to know how to add wxWindow items
        to the panels.
        """

        # create a fold panel item, which is first only the caption.
        # the user can now add a panel area which will be folded in
        # when pressed.

        if foldIcons is None:
            foldIcons = wx.ImageList(16, 16)

            bmp = GetExpandedIconBitmap()
            foldIcons.Add(bmp)
            bmp = GetCollapsedIconBitmap()
            foldIcons.Add(bmp)
    
        item = FoldPanelItem(self._foldPanel, -1, caption=caption,
                             foldIcons=foldIcons,
                             collapsed=collapsed, cbstyle=cbstyle)
        
        pos = 0
        if len(self._panels) > 0:
            pos = self._panels[-1].GetItemPos() + self._panels[-1].GetPanelLength()

        item.Reposition(pos)
        self._panels.append(item)

        return item


    def AddFoldPanelWindow(self, panel, window, flags=FPB_ALIGN_WIDTH,
                           Spacing=FPB_DEFAULT_SPACING,
                           leftSpacing=FPB_DEFAULT_LEFTLINESPACING,
                           rightSpacing=FPB_DEFAULT_RIGHTLINESPACING):
        """
        Adds a wxWindow derived class to the referenced FoldPanel.

        IMPORTANT: Make the to be created window, child of the FoldPanel. See
        example that follows. The flags to be used are:
            - FPB_ALIGN_WIDTH: Which means the wxWindow to be added will be
              aligned to fit the width of the FoldPanel when it is resized.
              Very handy for sizer items, buttons and text boxes.
            - FPB_ALIGN_LEFT: Alligns left instead of fitting the width of
              the child window to be added. Use either this one or
              FPB_ALIGN_WIDTH.

        The wxWindow to be added can be slightly indented from left and right
        so it is more visibly placed in the FoldPanel. Use Spacing > 0 to give
        the control an y offset from the previous wxWindow added, use leftSpacing
        to give it a slight indent from the left, and rightSpacing also reserves
        a little space on the right so the wxWindow can be properly placed in
        the FoldPanel.

        The following example adds a FoldPanel to the FoldPanelBar and adds two
        wxWindow derived controls to the FoldPanel:

        # CODE

            # create the FoldPanelBar
            >>> m_pnl = FoldPanelBar(self, wx.ID_ANY, wx.DefaultPosition,
                                     wx.DefaultSize, FPB_DEFAULT_STYLE,
                                     FPB_COLLAPSE_TO_BOTTOM)

            # add a foldpanel to the control. "Test me" is the caption and it is
            # initially not collapsed.
            >>> item = m_pnl.AddFoldPanel("Test me", False)

            # now add a button to the fold panel. Mind that the button should be
            # made child of the FoldPanel and not of the main form.
            >>> m_pnl.AddFoldPanelWindow(item, wx.Button(item, ID_COLLAPSEME,
                                        "Collapse Me"))

            # add a separator between the two controls. This is purely a visual
            # line that can have a certain color and also the indents and width
            # aligning like a control.
            >>> m_pnl.AddFoldPanelSeparator(item)

            # now add a text ctrl. Also very easy. Align this on width so that
            # when the control gets wider the text control also sizes along.
            >>> m_pnl.AddFoldPanelWindow(item, wx.TextCtrl(item, wx.ID_ANY, "Comment"),
                                         FPB_ALIGN_WIDTH, FPB_DEFAULT_SPACING, 20)

        # ENDCODE
        """
        
        try:
            item = self._panels.index(panel)
        except:
            raise "ERROR: Invalid Panel Passed To AddFoldPanelWindow: " + repr(panel)
        
        panel.AddWindow(window, flags, Spacing, leftSpacing, rightSpacing)

        # TODO: Take old and new height, and if difference, reposition all the lower
        # panels this is because the user can add new wxWindow controls somewhere in
        # between when other panels are already present.
    
        return 0


    def AddFoldPanelSeparator(self, panel, colour=wx.BLACK,
                              Spacing=FPB_DEFAULT_SPACING,
                              leftSpacing=FPB_DEFAULT_LEFTLINESPACING,
                              rightSpacing=FPB_DEFAULT_RIGHTLINESPACING):
        """
        Adds a separator line to the current FoldPanel.
        
        The seperator is a simple line which is drawn and is no real component.
        It can be used to separate groups of controls which belong to each other.
        The colour is adjustable, and it takes the same Spacing, leftSpacing and
        rightSpacing as AddFoldPanelWindow().
        """
        
        try:
            item = self._panels.index(panel)
        except:
            raise "ERROR: Invalid Panel Passed To AddFoldPanelSeparator: " + repr(panel)
        
        panel.AddSeparator(colour, Spacing, leftSpacing, rightSpacing)
        return 0


    def OnSizePanel(self, event):
        """ Handles the EVT_SIZE method for the FoldPanelBar. """

        # skip all stuff when we are not initialised yet

        if not self._controlCreated:
            event.Skip()
            return

        foldrect = self.GetRect()

        # fold panel itself. If too little space,
        # don't show it

        foldrect.SetX(0)
        foldrect.SetY(0)

        self._foldPanel.SetSize(foldrect[2:])
        
        if self._extraStyle & FPB_COLLAPSE_TO_BOTTOM:
            rect = self.RepositionCollapsedToBottom()
            vertical = self.IsVertical()
            if vertical and rect.GetHeight() > 0 or not vertical and rect.GetWidth() > 0:
                self.RefreshRect(rect)

        # TODO: A smart way to check wether the old - new width of the
        # panel changed, if so no need to resize the fold panel items

        self.RedisplayFoldPanelItems()
    

    def OnPressCaption(self, event):
        """ Handles the EVT_CAPTIONBAR event in the FoldPanelBar. """

        # act upon the folding or expanding status of the bar
        # to expand or collapse the panel(s)

        if event.GetFoldStatus():
            self.Collapse(event.GetTag())
        else:
            self.Expand(event.GetTag())

        event.Skip()
        

    def RefreshPanelsFrom(self, item):
        """ Refreshes all the panels from given index down to last one. """
        
        try:
            i = self._panels.index(item)
        except:
            raise "ERROR: Invalid Panel Passed To RefreshPanelsFrom: " + repr(item)
        
        self.Freeze()

        # if collapse to bottom is on, the panels that are not expanded
        # should be drawn at the bottom. All panels that are expanded
        # are drawn on top. The last expanded panel gets all the extra space

        if self._extraStyle & FPB_COLLAPSE_TO_BOTTOM:
        
            offset = 0

            for panels in self._panels:
            
                if panels.IsExpanded():
                    offset = offset + panels.Reposition(offset)
        
            # put all non collapsed panels at the bottom where there is space, 
            # else put them right behind the expanded ones

            self.RepositionCollapsedToBottom()
        
        else:
        
            pos = self._panels[i].GetItemPos() + self._panels[i].GetPanelLength()
            for j in range(i+1, len(self._panels)):
                pos = pos + self._panels[j].Reposition(pos)

        
        self.Thaw()
##        self.Refresh()
        

    def RedisplayFoldPanelItems(self):
        """ Resizes the fold panels so they match the width. """
        
        # resize them all. No need to reposition

        for panels in self._panels:
            panels.ResizePanel()
            panels.Refresh()


    def RepositionCollapsedToBottom(self):
        """
        Repositions all the collapsed panels to the bottom.

        When it is not possible to align them to the bottom, stick them behind the
        visible panels. The Rect holds the slack area left between last repositioned
        panel and the bottom panels. This needs to get a refresh.
        """

        value = wx.Rect(0,0,0,0)
        vertical = self.IsVertical()

        # determine wether the number of panels left
        # times the size of their captions is enough
        # to be placed in the left over space

        expanded = 0
        collapsed = 0
        collapsed, expanded, values = self.GetPanelsLength(collapsed, expanded)

        # if no room stick them behind the normal ones, else
        # at the bottom

        if (vertical and [self.GetSize().GetHeight()] or \
            [self.GetSize().GetWidth()])[0] - expanded - collapsed < 0:
            offset = expanded
        else:

            # value is the region which is left unpainted
            # I will send it back as 'slack' so it does not need to
            # be recalculated.

            value.SetHeight(self.GetSize().GetHeight())
            value.SetWidth(self.GetSize().GetWidth())

            if vertical:
                value.SetY(expanded)
                value.SetHeight(value.GetHeight() - expanded)
            else:
                value.SetX(expanded)
                value.SetWidth(value.GetWidth() - expanded)

            offset = (vertical and [self.GetSize().GetHeight()] or \
                      [self.GetSize().GetWidth()])[0] - collapsed


        # go reposition

        for panels in self._panels:
            if not panels.IsExpanded():
                offset = offset + panels.Reposition(offset)

        return value


    def GetPanelsLength(self, collapsed, expanded):
        """
        Returns the length of the panels that are expanded and collapsed.

        This is useful to determine quickly what size is used to display,
        and what is left at the bottom (right) to align the collapsed panels.
        """
        
        value = 0

        # assumed here that all the panels that are expanded
        # are positioned after each other from 0,0 to end.

        for j in range(0, len(self._panels)):
            offset = self._panels[j].GetPanelLength()
            value = value + offset
            if self._panels[j].IsExpanded():
                expanded = expanded + offset
            else:
                collapsed = collapsed + offset

        return collapsed, expanded, value


    def Collapse(self, foldpanel):
        """
        Collapses the given FoldPanel reference, and updates the foldpanel bar.

        In the FPB_COLLAPSE_TO_BOTTOM style, all collapsed captions are put at
        the bottom of the control. In the normal mode, they stay where they are.
        """
        
        try:
            item = self._panels.index(foldpanel)
        except:
            raise "ERROR: Invalid Panel Passed To Collapse: " + repr(foldpanel)
        
        foldpanel.Collapse()
        self.RefreshPanelsFrom(foldpanel)


    def Expand(self, foldpanel):
        """
        Expands the given FoldPanel reference, and updates the foldpanel bar.
        
        In the FPB_COLLAPSE_TO_BOTTOM style, they will be removed from the bottom
        and the order where the panel originally was placed is restored.
        """
        
        foldpanel.Expand()
        self.RefreshPanelsFrom(foldpanel)


    def ApplyCaptionStyle(self, foldpanel, cbstyle):
        """
        Sets the style of the caption bar (called CaptionBar) of the FoldPanel.

        The changes are applied immediately. All styles not set in the CaptionBarStyle
        class are not applied. Use the CaptionBar reference to indicate what captionbar
        you want to apply the style to. To apply one style to all CaptionBar items, use
        ApplyCaptionStyleAll()
        """
        
        foldpanel.ApplyCaptionStyle(cbstyle)
        

    def ApplyCaptionStyleAll(self, cbstyle):
        """ Sets the style of all the caption bars of the FoldPanel.

        The changes are applied immediately. """
 
        for panels in self._panels:
            self.ApplyCaptionStyle(panels, cbstyle)
            

    def GetCaptionStyle(self, foldpanel):
        """
        Returns the currently used caption style for the FoldPanel.

        It is returned as a CaptionBarStyle class. After modifying it, it can be
        set again.
        """
        
        return foldpanel.GetCaptionStyle()


    def IsVertical(self):
        """
        Returns wether the CaptionBar Has Default Orientation Or Not.

        Default is vertical.
        """
        
        return self._isVertical


    def GetFoldPanel(self, item):
        """
        Returns the panel associated with the index "item".

        See the example at the bottom of the module, especially the events
        for the "Collapse Me" and "Expand Me" buttons.
        """

        try:
            ind = self._panels[item]
            return self._panels[item]
        except:
            raise "ERROR: List Index Out Of Range Or Bad Item Passed: " + repr(item) + \
                  ". Item Should Be An Integer Between " + repr(0) + " And " + \
                  repr(len(self._panels))


    def GetCount(self):
        """ Returns the number of panels in the FoldPanelBar. """

        try:
            return len(self._panels)
        except:
            raise "ERROR: No Panels Have Been Added To FoldPanelBar"

        

# --------------------------------------------------------------------------------- #
# class FoldPanelItem
#    This class is a child sibling of the FoldPanelBar class. It will contain a
#    CaptionBar class for receiving of events, and a the rest of the area can be
#    populated by a wxPanel derived class.
# --------------------------------------------------------------------------------- #

class FoldPanelItem(wx.Panel):

    # Define Empty CaptionBar Style
    EmptyCaptionBarStyle = CaptionBarStyle()
    
    def __init__(self, parent, id=wx.ID_ANY, caption="", foldIcons=None,
                 collapsed=False, cbstyle=EmptyCaptionBarStyle):
        """ Default Class Constructor. """
        
        wx.Panel.__init__(self, parent, id, style=wx.CLIP_CHILDREN)
        self._controlCreated = False
        self._UserSize = 0
        self._PanelSize = 0
        self._LastInsertPos = 0
        self._itemPos = 0
        self._userSized = False

        if foldIcons is None:
            foldIcons = wx.ImageList(16, 16)

            bmp = GetExpandedIconBitmap()
            foldIcons.Add(bmp)
            bmp = GetCollapsedIconBitmap()
            foldIcons.Add(bmp)
        
        self._foldIcons = foldIcons

        # create the caption bar, in collapsed or expanded state
         
        self._captionBar = CaptionBar(self, wx.ID_ANY, wx.Point(0,0),
                                      size=wx.DefaultSize, caption=caption,
                                      foldIcons=foldIcons, cbstyle=cbstyle)

        if collapsed:
            self._captionBar.Collapse()

        self._controlCreated = True

        # make initial size for component, if collapsed, the
        # size is determined on the panel height and won't change
    
        size = self._captionBar.GetSize()

        self._PanelSize = (self.IsVertical() and [size.GetHeight()] or \
                           [size.GetWidth()])[0]
        
        self._LastInsertPos = self._PanelSize
        self._items = []

        self.Bind(EVT_CAPTIONBAR, self.OnPressCaption)
        self.Bind(wx.EVT_PAINT, self.OnPaint)


    def AddWindow(self, window, flags=FPB_ALIGN_WIDTH, Spacing=FPB_DEFAULT_SPACING,
                  leftSpacing=FPB_DEFAULT_LEFTLINESPACING,
                  rightSpacing=FPB_DEFAULT_RIGHTLINESPACING):
        """
        Adds a window item to the list of items on this panel.

        The flags are FPB_ALIGN_LEFT for a non sizing window element, and
        FPB_ALIGN_WIDTH for a width aligned item. The Spacing parameter reserves
        a number of pixels before the window element, and leftSpacing is an indent.
        rightSpacing is only relevant when the style FPB_ALIGN_WIDTH is chosen.
        """
        
        wi = FoldWindowItem(self, window, Type="WINDOW", flags=flags, Spacing=Spacing,
                            leftSpacing=leftSpacing, rightSpacing=rightSpacing)
        
        self._items.append(wi)

        vertical = self.IsVertical()
        
        self._Spacing = Spacing
        self._leftSpacing = leftSpacing
        self._rightSpacing = rightSpacing

        xpos = (vertical and [leftSpacing] or [self._LastInsertPos + Spacing])[0]
        ypos = (vertical and [self._LastInsertPos + Spacing] or [leftSpacing])[0]

        window.SetDimensions(xpos, ypos, -1, -1, wx.SIZE_USE_EXISTING)

        self._LastInsertPos = self._LastInsertPos + wi.GetWindowLength(vertical)
        self.ResizePanel()
        

    def AddSeparator(self, colour=wx.BLACK, Spacing=FPB_DEFAULT_SPACING,
                     leftSpacing=FPB_DEFAULT_LEFTSPACING,
                     rightSpacing=FPB_DEFAULT_RIGHTSPACING):
        """
        Adds a separator item to the list of items on this panel. """
        
        wi = FoldWindowItem(self, window=None, Type="SEPARATOR",
                            flags=FPB_ALIGN_WIDTH, y=self._LastInsertPos,
                            colour=colour, Spacing=Spacing, leftSpacing=leftSpacing,
                            rightSpacing=rightSpacing)
        
        self._items.append(wi)
        self._LastInsertPos = self._LastInsertPos + \
                              wi.GetWindowLength(self.IsVertical())
        
        self.ResizePanel()


    def Reposition(self, pos):
        """ Repositions this FoldPanelBar and reports the length occupied for the
        next FoldPanelBar in the list. """
        
        # NOTE: Call Resize before Reposition when an item is added, because the new
        # size needed will be calculated by Resize. Of course the relative position
        # of the controls have to be correct in respect to the caption bar
    
        self.Freeze()

        vertical = self.IsVertical()
        xpos = (vertical and [-1] or [pos])[0]
        ypos = (vertical and [pos] or [-1])[0]

        self.SetDimensions(xpos, ypos, -1, -1, wx.SIZE_USE_EXISTING)
        self._itemPos = pos

        self.Thaw()
##        self.Refresh()

        return self.GetPanelLength()


    def OnPressCaption(self, event):
        """ Handles the EVT_CAPTIONBAR event in the FoldPanelItem. """
        
        # tell the upper container we are responsible
        # for this event, so it can fold the panel item
        # and do a refresh

        event.SetTag(self)
        event.Skip()


    def ResizePanel(self):
        """ Resizes the panel. """
        
        # prevent unnecessary updates by blocking repaints for a sec

        self.Freeze()

        vertical = self.IsVertical()
        # force this panel to take the width of the parent panel and the y of the
        # user or calculated width (which will be recalculated by the contents here)


        if self._captionBar.IsCollapsed():
            size = self._captionBar.GetSize()
            self._PanelSize = (vertical and [size.GetHeight()] or [size.GetWidth()])[0]
        else:
            size = self.GetBestSize()
            self._PanelSize = (vertical and [size.GetHeight()] or [size.GetWidth()])[0]

            if self._UserSize:
                if vertical:
                    size.SetHeight(self._UserSize)
                else:
                    size.SetWidth(self._UserSize)

        pnlsize = self.GetParent().GetSize()

        if vertical:
            size.SetWidth(pnlsize.GetWidth())
        else:
            size.SetHeight(pnlsize.GetHeight())
        
        # resize caption bar
        xsize = (vertical and [size.GetWidth()] or [-1])[0]
        ysize = (vertical and [-1] or [size.GetHeight()])[0]

        self._captionBar.SetSize((xsize, ysize))

        # resize the panel
        self.SetSize(size)

        # go by all the controls and call Layout

        for items in self._items:
            items.ResizeItem((vertical and [size.GetWidth()] or \
                              [size.GetHeight()])[0], vertical)

        self.Thaw()            
##        self.Refresh()
        

    def OnPaint(self, event):
        """ Handles the EVT_PAINT event in the FoldPanelItem. """
  
        # draw all the items that are lines

        dc = wx.PaintDC(self)
        vertical = self.IsVertical()

        for item in self._items:
            
            if item.GetType() == "SEPARATOR":
                pen = wx.Pen(item.GetLineColour(), 1, wx.SOLID)
                dc.SetPen(pen)
                a = item.GetLeftSpacing()
                b = item.GetLineY() + item.GetSpacing()
                c = item.GetLineLength()
                d = a + c
                
                if vertical:
                    dc.DrawLine(a, b, d, b)
                else:
                    dc.DrawLine(b, a, b, d)
        
        event.Skip()
        

    def IsVertical(self):
        """
        Returns wether the CaptionBar Has Default Orientation Or Not.

        Default is vertical.
        """
        
        # grandparent of FoldPanelItem is FoldPanelBar
        # default is vertical
        
        if isinstance(self.GetGrandParent(), FoldPanelBar):
            return self.GetGrandParent().IsVertical()
        else:
            raise "ERROR: Wrong Parent " + repr(self.GetGrandParent())


    def IsExpanded(self):
        """ Returns expanded or collapsed status.

        If the panel is expanded, True is returned. """
        
        return not self._captionBar.IsCollapsed()

    
    def GetItemPos(self):
        """ Returns item's position. """
        
        return self._itemPos


    def Collapse(self):
        # this should not be called by the user, because it doesn't trigger the
        # parent  to tell it that we are collapsed or expanded, it only changes
        # visual state
        
        self._captionBar.Collapse()
        self.ResizePanel()


    def Expand(self):
        # this should not be called by the user, because it doesn't trigger the
        # parent to tell it that we are collapsed or expanded, it only changes
        # visual state
        
        self._captionBar.Expand()
        self.ResizePanel()


    def GetPanelLength(self):
        """ Returns size of panel. """
        
        if self._captionBar.IsCollapsed():
            return self.GetCaptionLength()
        elif self._userSized:
            return self._UserSize
        
        return self._PanelSize


    def GetCaptionLength(self):
        """ Returns height of caption only. This is for folding calculation purposes. """
        
        size = self._captionBar.GetSize()
        return (self.IsVertical() and [size.GetHeight()] or [size.GetWidth()])[0]


    def ApplyCaptionStyle(self, cbstyle):
        """ Applies the style defined in cbstyle to the CaptionBar."""
        
        self._captionBar.SetCaptionStyle(cbstyle)


    def GetCaptionStyle(self):
        """
        Returns the current style of the captionbar in a CaptionBarStyle class.

        This can be used to change and set back the changes.
        """
        
        return self._captionBar.GetCaptionStyle()


# ----------------------------------------------------------------------------------- #
# class FoldWindowItem
#    This class is a child sibling of the FoldPanelItem class. It will contain
#    wxWindow that can be either a separator (a colored line simulated by a wxWindow)
#    or a wxPython controls (such as a wxButton, a wxListCtrl etc...).
# ----------------------------------------------------------------------------------- #

class FoldWindowItem:

    def __init__(self, parent, window=None, **kw):
        """
        Default Class Constructor
        
        Initialize with:

            Type = "WINDOW", flags = FPB_ALIGN_WIDTH,
            Spacing = FPB_DEFAULT_SPACING,
            leftSpacing = FPB_DEFAULT_LEFTSPACING,
            rightSpacing = FPB_DEFAULT_RIGHTSPACING

            or:

            Type = "SEPARATOR"
            y, lineColor = wx.BLACK,
            flags = FPB_ALIGN_WIDTH,
            Spacing = FPB_DEFAULT_SPACING,
            leftSpacing = FPB_DEFAULT_LEFTLINESPACING,
            rightSpacing = FPB_DEFAULT_RIGHTLINESPACING
        """


        if not kw.has_key("Type"):
            raise 'ERROR: Missing Window Type Information. This Should Be "WINDOW" Or "SEPARATOR"'

        if kw.get("Type") == "WINDOW":
            # Window constructor. This initialises the class as a wx.Window Type
            
            if kw.has_key("flags"):
                self._flags = kw.get("flags")
            else:
                self._flags = FPB_ALIGN_WIDTH
            if kw.has_key("Spacing"):
                self._Spacing = kw.get("Spacing")
            else:
                self._Spacing = FPB_DEFAULT_SPACING
            if kw.has_key("leftSpacing"):
                self._leftSpacing = kw.get("leftSpacing")
            else:
                self._leftSpacing = FPB_DEFAULT_LEFTSPACING
            if kw.has_key("rightSpacing"):
                self._rightSpacing = kw.get("rightSpacing")
            else:
                self._rightSpacing = FPB_DEFAULT_RIGHTSPACING

            self._lineY = 0
            self._sepLineColour = None
            self._wnd = window
            

        elif kw.get("Type") == "SEPARATOR":
            # separator constructor. This initialises the class as a separator type
            
            if kw.has_key("y"):
                self._lineY = kw.get("y")
            else:
                raise "ERROR: Undefined Y Position For The Separator"
            if kw.has_key("lineColour"):
                self._sepLineColour = kw.get("lineColour")
            else:
                self._sepLineColour = wx.BLACK
            if kw.has_key("flags"):
                self._flags = kw.get("flags")
            else:
                self._flags = FPB_ALIGN_WIDTH
            if kw.has_key("Spacing"):
                self._Spacing = kw.get("Spacing")
            else:
                self._Spacing = FPB_DEFAULT_SPACING
            if kw.has_key("leftSpacing"):
                self._leftSpacing = kw.get("leftSpacing")
            else:
                self._leftSpacing = FPB_DEFAULT_LEFTSPACING
            if kw.has_key("rightSpacing"):
                self._rightSpacing = kw.get("rightSpacing")
            else:
                self._rightSpacing = FPB_DEFAULT_RIGHTSPACING

            self._wnd = window

        else:
            raise "ERROR: Undefined Window Type Selected: " + repr(kw.get("Type"))

        self._type = kw.get("Type")
        self._lineLength = 0
        

    def GetType(self):
        return self._type

    def GetLineY(self):
        return self._lineY

    def GetLineLength(self):
        return self._lineLength

    def GetLineColour(self):
        return self._sepLineColour

    def GetLeftSpacing(self):
        return self._leftSpacing

    def GetRightSpacing(self):
        return self._rightSpacing

    def GetSpacing(self):
        return self._Spacing


    def GetWindowLength(self, vertical=True):
        """ Returns space needed by the window if type is FoldWindowItem "WINDOW"
        and returns the total size plus the extra spacing."""

        value = 0
        if self._type == "WINDOW":
            size = self._wnd.GetSize()
            value = (vertical and [size.GetHeight()] or [size.GetWidth()])[0] + \
                    self._Spacing
            
        elif self._type == "SEPARATOR":
            value = 1 + self._Spacing

        return value


    def ResizeItem(self, size, vertical=True):
        """
        Resizes the element, whatever it is.
        
        A separator or line will be always aligned by width or height
        depending on orientation of the whole panel.
        """
        
        if self._flags & FPB_ALIGN_WIDTH:
            # align by taking full width
            mySize = size - self._leftSpacing - self._rightSpacing

            if mySize < 0:
                mySize = 10  # can't have negative width

            if self._type == "SEPARATOR":
                self._lineLength = mySize
            else:
                xsize = (vertical and [mySize] or [-1])[0]
                ysize = (vertical and [-1] or [mySize])[0]

                self._wnd.SetSize((xsize, ysize))
        
