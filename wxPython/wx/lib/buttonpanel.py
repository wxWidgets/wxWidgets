# --------------------------------------------------------------------------- #
# FANCYBUTTONPANEL Widget wxPython IMPLEMENTATION
#
# Original C++ Code From Eran. You Can Find It At:
#
# http://wxforum.shadonet.com/viewtopic.php?t=6619
#
# License: wxWidgets license
#
#
# Python Code By:
#
# Andrea Gavana, @ 02 Oct 2006
# Latest Revision: 16 Oct 2006, 17.00 GMT
#
#
# For All Kind Of Problems, Requests Of Enhancements And Bug Reports, Please
# Write To Me At:
#
# andrea.gavana@gmail.com
# gavana@kpo.kz
#
# Or, Obviously, To The wxPython Mailing List!!!
#
#
# End Of Comments
# --------------------------------------------------------------------------- #

"""
With `ButtonPanel` class you have a panel with gradient coloring
on it and with the possibility to place some buttons on it. Using a
standard panel with normal wx.Buttons leads to an ugly result: the
buttons are placed correctly on the panel - but with grey area around
them.  Gradient coloring is kept behind the images - this was achieved
due to the PNG format and the transparency of the bitmaps.

The image are functioning like a buttons and can be caught in your
code using the usual self.Bind(wx.EVT_BUTTON, self.OnButton) method.

The control is generic, and support theming (well, I tested it under
Windows with the three defauls themes: grey, blue, silver and the
classic look).


Usage
-----

The following example shows a simple implementation that uses ButtonPanel
inside a very simple frame::

  class MyFrame(wx.Frame):

      def __init__(self, parent, id=-1, title="ButtonPanel", pos=wx.DefaultPosition,
                   size=(800, 600), style=wx.DEFAULT_FRAME_STYLE):
                 
          wx.Frame.__init__(self, parent, id, title, pos, size, style)

          mainPanel = wx.Panel(self, -1)
          self.logtext = wx.TextCtrl(mainPanel, -1, "", style=wx.TE_MULTILINE)

          vSizer = wx.BoxSizer(wx.VERTICAL) 
          mainPanel.SetSizer(vSizer) 

          alignment = BP_ALIGN_RIGHT 

          titleBar = ButtonPanel(mainPanel, -1, "A Simple Test & Demo")

          btn1 = ButtonInfo(wx.NewId(), wx.Bitmap("png4.png", wx.BITMAP_TYPE_PNG))
          titleBar.AddButton(btn1)
          self.Bind(wx.EVT_BUTTON, self.OnButton, btn1)

          btn2 = ButtonInfo(wx.NewId(), wx.Bitmap("png3.png", wx.BITMAP_TYPE_PNG))
          titleBar.AddButton(btn2)
          self.Bind(wx.EVT_BUTTON, self.OnButton, btn2)

          btn3 = ButtonInfo(wx.NewId(), wx.Bitmap("png2.png", wx.BITMAP_TYPE_PNG))
          titleBar.AddButton(btn3)
          self.Bind(wx.EVT_BUTTON, self.OnButton, btn3)

          btn4 = ButtonInfo(wx.NewId(), wx.Bitmap("png1.png", wx.BITMAP_TYPE_PNG))
          titleBar.AddButton(btn4)
          self.Bind(wx.EVT_BUTTON, self.OnButton, btn4)

          vSizer.Add(titleBar, 0, wx.EXPAND)
          vSizer.Add((20, 20))
          vSizer.Add(self.logtext, 1, wx.EXPAND|wx.ALL, 5)
  
          vSizer.Layout()
  
  # our normal wxApp-derived class, as usual

  app = wx.PySimpleApp()
  
  frame = MyFrame(None)
  app.SetTopWindow(frame)
  frame.Show()
  
  app.MainLoop()


License And Version:

ButtonPanel Is Freeware And Distributed Under The wxPython License. 

Latest Revision: Andrea Gavana @ 12 Oct 2006, 17.00 GMT
Version 0.3.

"""


import wx

# Some constants to tune the BPArt class
BP_BACKGROUND_COLOR = 0
""" Background brush colour when no gradient shading exists. """
BP_GRADIENT_COLOR_FROM = 1
""" Starting gradient colour, used only when BP_USE_GRADIENT style is applied. """
BP_GRADIENT_COLOR_TO = 2
""" Ending gradient colour, used only when BP_USE_GRADIENT style is applied. """
BP_BORDER_COLOR = 3
""" Pen colour to paint the border of ButtonPanel. """
BP_TEXT_COLOR = 4
""" Main ButtonPanel caption colour. """
BP_BUTTONTEXT_COLOR = 5
""" Text colour for buttons with text. """
BP_BUTTONTEXT_INACTIVE_COLOR = 6
""" Text colour for inactive buttons with text. """
BP_SELECTION_BRUSH_COLOR = 7
""" Brush colour to be used when hovering or selecting a button. """
BP_SELECTION_PEN_COLOR = 8
""" Pen colour to be used when hovering or selecting a button. """
BP_SEPARATOR_COLOR = 9
""" Pen colour used to paint the separators. """
BP_TEXT_FONT = 10
""" Font of the ButtonPanel main caption. """
BP_BUTTONTEXT_FONT = 11
""" Text font for the buttons with text. """

BP_BUTTONTEXT_ALIGN_BOTTOM = 12
""" Flag that indicates the image and text in buttons is stacked. """
BP_BUTTONTEXT_ALIGN_RIGHT = 13
""" Flag that indicates the text is shown alongside the image in buttons with text. """

BP_SEPARATOR_SIZE = 14
"""
Separator size. NB: This is not the line width, but the sum of the space before
and after the separator line plus the width of the line.
"""
BP_MARGINS_SIZE = 15
"""
Size of the left/right margins in ButtonPanel (top/bottom for vertically
aligned ButtonPanels).
"""
BP_BORDER_SIZE = 16
""" Size of the border. """
BP_PADDING_SIZE = 17
""" Inter-tool separator size. """

# Caption Gradient Type
BP_GRADIENT_NONE = 0
""" No gradient shading should be used to paint the background. """
BP_GRADIENT_VERTICAL = 1
""" Vertical gradient shading should be used to paint the background. """
BP_GRADIENT_HORIZONTAL = 2
""" Horizontal gradient shading should be used to paint the background. """

# Flags for HitTest() method
BP_HT_BUTTON = 200
BP_HT_NONE = 201

# Alignment of buttons in the panel
BP_ALIGN_RIGHT = 1
BP_ALIGN_LEFT = 2
BP_ALIGN_TOP = 4
BP_ALIGN_BOTTOM = 8

# ButtonPanel styles
BP_DEFAULT_STYLE = 1
BP_USE_GRADIENT = 2


# Check for the new method in 2.7 (not present in 2.6.3.3)
if wx.VERSION_STRING < "2.7":
    wx.Rect.Contains = lambda self, point: wx.Rect.Inside(self, point)

 
def BrightenColour(color, factor): 
    """ Bright the input colour by a factor."""

    val = color.Red()*factor
    if val > 255:
        red = 255
    else:
        red = val
        
    val = color.Green()*factor
    if val > 255:
        green = 255
    else:
        green = val

    val = color.Blue()*factor
    if val > 255:
        blue = 255
    else:
        blue = val

    return wx.Color(red, green, blue) 


def GrayOut(anImage):
    """
    Convert the given image (in place) to a grayed-out version,
    appropriate for a 'Disabled' appearance.
    """
    
    factor = 0.7        # 0 < f < 1.  Higher Is Grayer

    anImage = anImage.ConvertToImage()
    if anImage.HasAlpha():
        anImage.ConvertAlphaToMask(1)
    
    if anImage.HasMask():
        maskColor = (anImage.GetMaskRed(), anImage.GetMaskGreen(), anImage.GetMaskBlue())
    else:
        maskColor = None
        
    data = map(ord, list(anImage.GetData()))
    
    for i in range(0, len(data), 3):

        pixel = (data[i], data[i+1], data[i+2])
        pixel = MakeGray(pixel, factor, maskColor)

        for x in range(3):
            data[i+x] = pixel[x]

    anImage.SetData(''.join(map(chr, data)))

    anImage = anImage.ConvertToBitmap()

    return anImage


def MakeGray((r,g,b), factor, maskColor):
    """
    Make a pixel grayed-out. If the pixel matches the maskColor, it won't be
    changed.
    """
    
    if (r,g,b) != maskColor:
        return map(lambda x: int((230 - x) * factor) + x, (r,g,b))
    else:
        return (r,g,b)


# ---------------------------------------------------------------------------- #
# Class BPArt
# Handles all the drawings for buttons, separators and text and allows the
# programmer to set colours, sizes and gradient shadings for ButtonPanel
# ---------------------------------------------------------------------------- #

class BPArt:
    """
    BPArt is an art provider class which does all of the drawing for ButtonPanel.
    This allows the library caller to customize the BPArt or to completely replace
    all drawing with custom BPArts.
    """

    def __init__(self, parentStyle):
        """ Default class constructor. """

        base_color = wx.SystemSettings_GetColour(wx.SYS_COLOUR_3DFACE)

        self._background_brush = wx.Brush(base_color, wx.SOLID)
        self._gradient_color_to = wx.WHITE
        self._gradient_color_from = wx.SystemSettings_GetColour(wx.SYS_COLOUR_ACTIVECAPTION)

        if parentStyle & BP_USE_GRADIENT:
            self._border_pen = wx.Pen(wx.WHITE, 3)
            self._caption_text_color = wx.WHITE
            self._buttontext_color = wx.Colour(70, 143, 255)
            self._separator_pen = wx.Pen(BrightenColour(self._gradient_color_from, 1.4))
            self._gradient_type = BP_GRADIENT_VERTICAL
        else:
            self._border_pen = wx.Pen(BrightenColour(base_color, 0.9), 3)
            self._caption_text_color = wx.BLACK
            self._buttontext_color = wx.SystemSettings_GetColour(wx.SYS_COLOUR_BTNTEXT)
            self._separator_pen = wx.Pen(BrightenColour(base_color, 0.9))
            self._gradient_type = BP_GRADIENT_NONE
            
        self._buttontext_inactive_color = wx.SystemSettings_GetColour(wx.SYS_COLOUR_GRAYTEXT)
        self._selection_brush = wx.Brush(wx.Color(225, 225, 255))
        self._selection_pen = wx.Pen(wx.SystemSettings_GetColour(wx.SYS_COLOUR_ACTIVECAPTION))

        sysfont = wx.SystemSettings.GetFont(wx.SYS_DEFAULT_GUI_FONT)
        self._caption_font = wx.Font(sysfont.GetPointSize(), wx.DEFAULT, wx.NORMAL, wx.BOLD,
                                     False, sysfont.GetFaceName())
        self._buttontext_font = wx.Font(sysfont.GetPointSize(), wx.DEFAULT, wx.NORMAL, wx.NORMAL,
                                        False, sysfont.GetFaceName())
    
        self._separator_size = 7
        self._margins_size = wx.Size(6, 6)
        self._caption_border_size = 3
        self._padding_size = wx.Size(6, 6)


    def GetMetric(self, id):
        """ Returns sizes of customizable options. """

        if id == BP_SEPARATOR_SIZE:
            return self._separator_size
        elif id == BP_MARGINS_SIZE:
            return self._margins_size
        elif id == BP_BORDER_SIZE:
            return self._caption_border_size
        elif id == BP_PADDING_SIZE:
            return self._padding_size
        else:
            raise "\nERROR: Invalid Metric Ordinal. "


    def SetMetric(self, id, new_val):
        """ Sets sizes for customizable options. """

        if id == BP_SEPARATOR_SIZE:
            self._separator_size = new_val
        elif id == BP_MARGINS_SIZE:
            self._margins_size = new_val
        elif id == BP_BORDER_SIZE:
            self._caption_border_size = new_val
            self._border_pen.SetWidth(new_val)
        elif id == BP_PADDING_SIZE:
            self._padding_size = new_val
        else:
            raise "\nERROR: Invalid Metric Ordinal. "


    def GetColor(self, id):
        """ Returns colours of customizable options. """

        if id == BP_BACKGROUND_COLOR:
            return self._background_brush.GetColour()
        elif id == BP_GRADIENT_COLOR_FROM:
            return self._gradient_color_from
        elif id == BP_GRADIENT_COLOR_TO:
            return self._gradient_color_to
        elif id == BP_BORDER_COLOR:
            return self._border_pen.GetColour()
        elif id == BP_TEXT_COLOR:
            return self._caption_text_color
        elif id == BP_BUTTONTEXT_COLOR:
            return self._buttontext_color
        elif id == BP_BUTTONTEXT_INACTIVE_COLOR:
            return self._buttontext_inactive_color
        elif id == BP_SELECTION_BRUSH_COLOR:
            return self._selection_brush.GetColour()
        elif id == BP_SELECTION_PEN_COLOR:
            return self._selection_pen.GetColour()
        elif id == BP_SEPARATOR_COLOR:
            return self._separator_pen.GetColour()
        else:
            raise "\nERROR: Invalid Colour Ordinal. "


    def SetColor(self, id, colour):
        """ Sets colours for customizable options. """

        if id == BP_BACKGROUND_COLOR:
            self._background_brush.SetColour(colour)
        elif id == BP_GRADIENT_COLOR_FROM:
            self._gradient_color_from = colour
        elif id == BP_GRADIENT_COLOR_TO:
            self._gradient_color_to = colour
        elif id == BP_BORDER_COLOR:
            self._border_pen.SetColour(colour)
        elif id == BP_TEXT_COLOR:
            self._caption_text_color = colour
        elif id == BP_BUTTONTEXT_COLOR:
            self._buttontext_color = colour
        elif id == BP_BUTTONTEXT_INACTIVE_COLOR:
            self._buttontext_inactive_color = colour
        elif id == BP_SELECTION_BRUSH_COLOR:
            self._selection_brush.SetColour(colour)
        elif id == BP_SELECTION_PEN_COLOR:
            self._selection_pen.SetColour(colour)
        elif id == BP_SEPARATOR_COLOR:
            self._separator_pen.SetColour(colour)
        else:
            raise "\nERROR: Invalid Colour Ordinal. "
        

    GetColour = GetColor
    SetColour = SetColor


    def SetFont(self, id, font):
        """ Sets font for customizable options. """

        if id == BP_TEXT_FONT:
            self._caption_font = font
        elif id == BP_BUTTONTEXT_FONT:
            self._buttontext_font = font


    def GetFont(self, id):
        """ Returns font of customizable options. """

        if id == BP_TEXT_FONT:
            return self._caption_font
        elif id == BP_BUTTONTEXT_FONT:
            return self._buttontext_font
        
        return wx.NoneFont


    def SetGradientType(self, gradient):
        """ Sets the gradient type for BPArt drawings. """

        self._gradient_type = gradient
        

    def GetGradientType(self):
        """ Returns the gradient type for BPArt drawings. """

        return self._gradient_type        

        
    def DrawSeparator(self, dc, rect, isVertical):
        """ Draws a separator in ButtonPanel. """
                    
        dc.SetPen(self._separator_pen)

        if isVertical:
            ystart = yend = rect.y + rect.height/2
            xstart = int(rect.x + 1.5*self._caption_border_size)
            xend = int(rect.x + rect.width - 1.5*self._caption_border_size)
            dc.DrawLine(xstart, ystart, xend, yend)
        else:
            xstart = xend = rect.x + rect.width/2
            ystart = int(rect.y + 1.5*self._caption_border_size)
            yend = int(rect.y + rect.height - 1.5*self._caption_border_size)
            dc.DrawLine(xstart, ystart, xend, yend)


    def DrawCaption(self, dc, rect, captionText):
        """ Draws the main caption text in ButtonPanel. """

        textColour = self._caption_text_color
        textFont = self._caption_font
        padding = self._padding_size
            
        dc.SetTextForeground(textColour) 
        dc.SetFont(textFont)

        dc.DrawText(captionText, rect.x + padding.x, rect.y+padding.y)
            

    def DrawButton(self, dc, rect, parentSize, buttonBitmap, isVertical,
                   buttonStatus, isToggled, textAlignment, text=""):
        """ Draws a button in ButtonPanel, together with its text (if any). """
        
        bmpxsize, bmpysize = buttonBitmap.GetWidth(), buttonBitmap.GetHeight()
        dx = dy = focus = 0
        
        borderw = self._caption_border_size
        padding = self._padding_size

        buttonFont = self._buttontext_font
        dc.SetFont(buttonFont)
        
        if isVertical:
            
            rect = wx.Rect(borderw, rect.y, rect.width-2*borderw, rect.height)
            
            if text != "":

                textW, textH = dc.GetTextExtent(text)
                
                if textAlignment == BP_BUTTONTEXT_ALIGN_RIGHT:
                    fullExtent = bmpxsize + padding.x/2 + textW
                    bmpypos = rect.y + (rect.height - bmpysize)/2
                    bmpxpos = rect.x + (rect.width - fullExtent)/2
                    textxpos = bmpxpos + padding.x/2 + bmpxsize
                    textypos = bmpypos + (bmpysize - textH)/2
                else:
                    bmpxpos = rect.x + (rect.width - bmpxsize)/2
                    bmpypos = rect.y + padding.y
                    textxpos = rect.x + (rect.width - textW)/2
                    textypos = bmpypos + bmpysize + padding.y/2
            else:
                bmpxpos = rect.x + (rect.width - bmpxsize)/2
                bmpypos = rect.y + (rect.height - bmpysize)/2
                
                
        else:

            rect = wx.Rect(rect.x, borderw, rect.width, rect.height-2*borderw)

            if text != "":

                textW, textH = dc.GetTextExtent(text)
                
                if textAlignment == BP_BUTTONTEXT_ALIGN_RIGHT:
                    fullExtent = bmpxsize + padding.x/2 + textW
                    bmpypos = rect.y + (rect.height - bmpysize)/2
                    bmpxpos = rect.x + (rect.width - fullExtent)/2
                    textxpos = bmpxpos + padding.x/2 + bmpxsize
                    textypos = bmpypos + (bmpysize - textH)/2
                else:
                    fullExtent = bmpysize + padding.y/2 + textH
                    bmpxpos = rect.x + (rect.width - bmpxsize)/2
                    bmpypos = rect.y + (rect.height - fullExtent)/2
                    textxpos = rect.x + (rect.width - textW)/2
                    textypos = bmpypos + bmpysize + padding.y/2
            else:
                bmpxpos = rect.x + (rect.width - bmpxsize)/2
                bmpypos = rect.y + (rect.height - bmpysize)/2
                    
        # Draw a button 
        # [ Padding | Text | .. Buttons .. | Padding ]

        if buttonStatus in ["Pressed", "Toggled", "Hover"]:                
            dc.SetBrush(self._selection_brush) 
            dc.SetPen(self._selection_pen)
            dc.DrawRoundedRectangleRect(rect, 4)

        if buttonStatus == "Pressed" or isToggled:
            dx = dy = 1
            
        dc.DrawBitmap(buttonBitmap, bmpxpos+dx, bmpypos+dy, True)

        if text != "":
            isEnabled = buttonStatus != "Disabled"
            self.DrawLabel(dc, text, isEnabled, textxpos+dx, textypos+dy)

                
    def DrawLabel(self, dc, text, isEnabled, xpos, ypos):
        """ Draws the label for a button. """

        if not isEnabled:
            dc.SetTextForeground(self._buttontext_inactive_color)
        else:
            dc.SetTextForeground(self._buttontext_color)
            
        dc.DrawText(text, xpos, ypos)


    def DrawButtonPanel(self, dc, rect, style):
        """ Paint the ButtonPanel's background. """

        if style & BP_USE_GRADIENT:
            # Draw gradient color in the backgroud of the panel 
            self.FillGradientColor(dc, rect)

        # Draw a rectangle around the panel
        backBrush = (style & BP_USE_GRADIENT and [wx.TRANSPARENT_BRUSH] or \
                     [self._background_brush])[0]
        
        dc.SetBrush(backBrush) 
        dc.SetPen(self._border_pen)
        dc.DrawRectangleRect(rect) 
        

    def FillGradientColor(self, dc, rect):
        """ Gradient fill from colour 1 to colour 2 with top to bottom or left to right. """

        if rect.height < 1 or rect.width < 1: 
            return 

        isVertical = self._gradient_type == BP_GRADIENT_VERTICAL
        size = (isVertical and [rect.height] or [rect.width])[0]
        start = (isVertical and [rect.y] or [rect.x])[0]

        # calculate gradient coefficients

        col2 = self._gradient_color_from
        col1 = self._gradient_color_to

        rf, gf, bf = 0, 0, 0
        rstep = float((col2.Red() - col1.Red()))/float(size)
        gstep = float((col2.Green() - col1.Green()))/float(size)
        bstep = float((col2.Blue() - col1.Blue()))/float(size)

        for coord in xrange(start, start + size):
         
            currCol = wx.Colour(col1.Red() + rf, col1.Green() + gf, col1.Blue() + bf)
            dc.SetBrush(wx.Brush(currCol, wx.SOLID)) 
            dc.SetPen(wx.Pen(currCol))
            if isVertical:
                dc.DrawLine(rect.x, coord, rect.x + rect.width, coord) 
            else:
                dc.DrawLine(coord, rect.y, coord, rect.y + rect.width)
                
            rf += rstep
            gf += gstep
            bf += bstep 
                
        
class Control(wx.EvtHandler):

    def __init__(self, parent, size=wx.Size(-1, -1)):
        """
        Default class constructor.
        
        Base class for all pseudo controls
        parent = parent object
        size = (width, height)
        """

        wx.EvtHandler.__init__(self)

        self._parent = parent
        self._id = wx.NewId()
        self._size = size
        self._isshown = True
        self._focus = False


    def Show(self, show=True):
        """ Shows or hide the control. """

        self._isshown = show


    def Hide(self):
        """ Hides the control. """

        self.Show(False)


    def IsShown(self):
        """ Returns whether the control is shown or not. """

        return self._isshown        
        

    def GetId(self):
        """ Returns the control id. """

        return self._id
    

    def GetBestSize(self):
        """ Returns the control best size. """

        return self._size


    def Disable(self):
        """ Disables the control. """

        self.Enable(False)


    def Enable(self, value=True):
        """ Enables or disables the control. """

        self.disabled = not value


    def SetFocus(self, focus=True):
        """ Sets or kills the focus on the control. """

        self._focus = focus

        
    def HasFocus(self):
        """ Returns whether the control has the focus or not. """

        return self._focus
    
                
    def OnMouseEvent(self, x, y, event):
        pass

    def Draw(self, rect):
        pass



class Sizer(object):
    """
    Sizer

    This is a mix-in class to add pseudo support to a wx sizer.  Just create
    a new class that derives from this class and the wx sizer and intercepts
    any methods that add to the wx sizer.
    """
    def __init__(self):
        self.children = [] # list of child Pseudo Controls
    
    # Sizer doesn't use the x1,y1,x2,y2 so allow it to 
    # be called with or without the coordinates
    def Draw(self, dc, x1=0, y1=0, x2=0, y2=0):
        for item in self.children:
            # use sizer coordinates rather than
            # what is passed in
            c = item.GetUserData()
            c.Draw(dc, item.GetRect())
            
    def GetBestSize(self):
        # this should be handled by the wx.Sizer based class
        return self.GetMinSize()


# Pseudo BoxSizer
class BoxSizer(Sizer, wx.BoxSizer):
    def __init__(self, orient=wx.HORIZONTAL):
        wx.BoxSizer.__init__(self, orient)
        Sizer.__init__(self)

    #-------------------------------------------
    # sizer overrides (only called from Python)
    #-------------------------------------------
    # no support for user data if it's a pseudocontrol
    # since that is already used
    def Add(self, item, proportion=0, flag=0, border=0, userData=None):
        # check to see if it's a pseudo object or sizer
        if isinstance(item, Sizer):
            szitem = wx.BoxSizer.Add(self, item, proportion, flag, border, item)
            self.children.append(szitem)
        elif isinstance(item, Control): # Control should be what ever class your controls come from
            sz = item.GetBestSize()
            # add a spacer to track this object
            szitem = wx.BoxSizer.Add(self, sz, proportion, flag, border, item)
            self.children.append(szitem)
        else:
            wx.BoxSizer.Add(self, item, proportion, flag, border, userData)

    def Prepend(self, item, proportion=0, flag=0, border=0, userData=None):
        # check to see if it's a pseudo object or sizer
        if isinstance(item, Sizer):
            szitem = wx.BoxSizer.Prepend(self, item, proportion, flag, border, item)
            self.children.append(szitem)
        elif isinstance(item, Control): # Control should be what ever class your controls come from
            sz = item.GetBestSize()
            # add a spacer to track this object
            szitem = wx.BoxSizer.Prepend(self, sz, proportion, flag, border, item)
            self.children.insert(0,szitem)
        else:
            wx.BoxSizer.Prepend(self, item, proportion, flag, border, userData)

    def Insert(self, before, item, proportion=0, flag=0, border=0, userData=None, realIndex=None):
        # check to see if it's a pseudo object or sizer
        if isinstance(item, Sizer):
            szitem = wx.BoxSizer.Insert(self, before, item, proportion, flag, border, item)
            self.children.append(szitem)
        elif isinstance(item, Control): # Control should be what ever class your controls come from
            sz = item.GetBestSize()
            # add a spacer to track this object
            szitem = wx.BoxSizer.Insert(self, before, sz, proportion, flag, border, item)
            if realIndex is not None:
                self.children.insert(realIndex,szitem)
            else:
                self.children.insert(before,szitem)
                
        else:
            wx.BoxSizer.Insert(self, before, item, proportion, flag, border, userData)


    def Remove(self, indx, pop=-1):

        if pop >= 0:
            self.children.pop(pop)

        wx.BoxSizer.Remove(self, indx)
        

    def Layout(self):

        for ii, child in enumerate(self.GetChildren()):
            item = child.GetUserData()
            if item and child.IsShown():
                self.SetItemMinSize(ii, *item.GetBestSize())

        wx.BoxSizer.Layout(self)

        
    def Show(self, item, show=True):

        child = self.GetChildren()[item]
        if child and child.GetUserData():
            child.GetUserData().Show(show)

        wx.BoxSizer.Show(self, item, show)
    

# ---------------------------------------------------------------------------- #
# Class Separator
# This class holds all the information to size and draw a separator inside
# ButtonPanel
# ---------------------------------------------------------------------------- #

class Separator(Control):

    def __init__(self, parent):
        """ Default class constructor. """

        self._isshown = True
        self._parent = parent
        Control.__init__(self, parent)

    
    def GetBestSize(self):
        """ Returns the separator best size. """

        # 10 is completely arbitrary, but it works anyhow
        if self._parent.IsVertical():
            return wx.Size(10, self._parent._art.GetMetric(BP_SEPARATOR_SIZE))
        else:
            return wx.Size(self._parent._art.GetMetric(BP_SEPARATOR_SIZE), 10)
        
    
    def Draw(self, dc, rect):
        """ Draws the separator. Actually the drawing is done in BPArt. """

        if not self.IsShown():
            return

        isVertical = self._parent.IsVertical()        
        self._parent._art.DrawSeparator(dc, rect, isVertical)
        

# ---------------------------------------------------------------------------- #
# Class ButtonPanelText
# This class is used to hold data about the main caption in ButtonPanel
# ---------------------------------------------------------------------------- #

class ButtonPanelText(Control):

    def __init__(self, parent, text=""):
        """ Default class constructor. """

        self._text = text
        self._isshown = True
        self._parent = parent
        
        Control.__init__(self, parent)


    def GetText(self):
        """ Returns the caption text. """

        return self._text


    def SetText(self, text=""):
        """ Sets the caption text. """

        self._text = text


    def CreateDC(self):
        """ Convenience function to create a DC. """

        dc = wx.ClientDC(self._parent)
        textFont = self._parent._art.GetFont(BP_TEXT_FONT)
        dc.SetFont(textFont)

        return dc        

        
    def GetBestSize(self):
        """ Returns the best size for the main caption in ButtonPanel. """

        if self._text == "":
            return wx.Size(0, 0)

        dc = self.CreateDC()
        rect = self._parent.GetClientRect()
        
        tw, th = dc.GetTextExtent(self._text)
        padding = self._parent._art.GetMetric(BP_PADDING_SIZE)
        self._size = wx.Size(tw+2*padding.x, th+2*padding.y)

        return self._size

    
    def Draw(self, dc, rect):
        """ Draws the main caption. Actually the drawing is done in BPArt. """

        if not self.IsShown():
            return

        captionText = self.GetText()
        self._parent._art.DrawCaption(dc, rect, captionText)
        
            
# -- ButtonInfo class implementation ----------------------------------------
# This class holds information about every button that is added to
# ButtonPanel.  It is an auxiliary class that you should use
# every time you add a button.

class ButtonInfo(Control):

    def __init__(self, parent, id=wx.ID_ANY, bmp=wx.NullBitmap,
                 status="Normal", text="", kind=wx.ITEM_NORMAL):
        """
        Default class constructor.

        Parameters:
        - parent: the parent window (ButtonPanel);
        - id: the button id;
        - bmp: the associated bitmap;
        - status: button status (pressed, hovered, None).
        - text to be displayed either below of to the right of the button
        """
        
        if id == wx.ID_ANY:
            id = wx.NewId()

        self._status = status
        self._rect = wx.Rect()
        self._text = text
        self._kind = kind
        self._toggle = False
        self._textAlignment = BP_BUTTONTEXT_ALIGN_BOTTOM

        disabledbmp = GrayOut(bmp)

        self._bitmaps = {"Normal": bmp, "Toggled": None, "Disabled": disabledbmp,
                         "Hover": None, "Pressed": None}        

        Control.__init__(self, parent)
        

    def GetBestSize(self):
        """ Returns the best size for the button. """

        xsize = self.GetBitmap().GetWidth()
        ysize = self.GetBitmap().GetHeight()
        
        if self.HasText():
            # We have text in the button
            dc = wx.ClientDC(self._parent)
            normalFont = self._parent._art.GetFont(BP_BUTTONTEXT_FONT)
            dc.SetFont(normalFont)
            tw, th = dc.GetTextExtent(self.GetText())

            if self.GetTextAlignment() == BP_BUTTONTEXT_ALIGN_BOTTOM:
                xsize = max(xsize, tw)
                ysize = ysize + th
            else:
                xsize = xsize + tw
                ysize = max(ysize, th)

        border = self._parent._art.GetMetric(BP_BORDER_SIZE)
        padding = self._parent._art.GetMetric(BP_PADDING_SIZE)
        
        if self._parent.IsVertical():
            xsize = xsize + 2*border
        else:
            ysize = ysize + 2*border

        self._size = wx.Size(xsize+2*padding.x, ysize+2*padding.y)

        return self._size

    
    def Draw(self, dc, rect):
        """ Draws the button on ButtonPanel. Actually the drawing is done in BPArt. """

        if not self.IsShown():
            return

        buttonBitmap = self.GetBitmap()
        isVertical = self._parent.IsVertical()
        text = self.GetText()
        parentSize = self._parent.GetSize()[not isVertical]
        buttonStatus = self.GetStatus()
        isToggled = self.GetToggled()
        textAlignment = self.GetTextAlignment()

        self._parent._art.DrawButton(dc, rect, parentSize, buttonBitmap, isVertical,
                                     buttonStatus, isToggled, textAlignment, text)

        self.SetRect(rect)
        
        
    def CheckRefresh(self, status):
        """ Checks whether a ButtonPanel repaint is needed or not. Convenience function. """

        if status == self._status:
            self._parent.Refresh()

        
    def SetBitmap(self, bmp, status="Normal"):
        """ Sets the associated bitmap. """

        self._bitmaps[status] = bmp
        self.CheckRefresh(status)


    def GetBitmap(self, status=None):
        """ Returns the associated bitmap. """

        if status is None:
            status = self._status

        if not self.IsEnabled():
            status = "Disabled"
            
        if self._bitmaps[status] is None:
            return self._bitmaps["Normal"]
            
        return self._bitmaps[status]


    def GetRect(self):
        """ Returns the button rect. """

        return self._rect


    def GetStatus(self):
        """ Returns the button status. """

        return self._status


    def GetId(self):
        """ Returns the button id. """
        
        return self._id


    def SetRect(self, rect):
        """ Sets the button rect. """

        self._rect = rect
        

    def SetStatus(self, status):
        """ Sets the button status. """

        if status == self._status:
            return
        
        if self.GetToggled() and status == "Normal":
            status = "Toggled"
            
        self._status = status
        self._parent.Refresh()


    def GetTextAlignment(self):
        """ Returns the text alignment in the button (bottom or right). """

        return self._textAlignment


    def SetTextAlignment(self, alignment):
        """ Sets the text alignment in the button (bottom or right). """

        if alignment == self._textAlignment:
            return

        self._alignment = alignment
        

    def GetToggled(self):
        """ Returns whether a wx.ITEM_CHECK button is toggled or not. """

        if self._kind == wx.ITEM_NORMAL:
            return False

        return self._toggle
    

    def SetToggled(self, toggle=True):
        """ Sets a wx.ITEM_CHECK button toggled/not toggled. """

        if self._kind == wx.ITEM_NORMAL:
            return

        self._toggle = toggle


    def SetId(self, id):
        """ Sets the button id. """

        self._id = id


    def AddStatus(self, name="Custom", bmp=wx.NullBitmap):
        """
        Add a programmer-defined status in addition to the 5 default status:
        - Normal;
        - Disabled;
        - Hover;
        - Pressed;
        - Toggled.
        """

        self._bitmaps.update({name: bmp})


    def Enable(self, enable=True):

        if enable:
            self._status = "Normal"
        else:
            self._status = "Disabled"
        

    def IsEnabled(self):

        return self._status != "Disabled"
        
    
    def SetText(self, text=""):
        """ Sets the text of the button. """

        self._text = text


    def GetText(self):
        """ Returns the text associated to the button. """

        return self._text


    def HasText(self):
        """ Returns whether the button has text or not. """

        return self._text != ""
    

    def SetKind(self, kind=wx.ITEM_NORMAL):
        """ Sets the button type (standard or toggle). """

        self._kind = kind


    def GetKind(self):
        """ Returns the button type (standard or toggle). """

        return self._kind
    
                
    Bitmap = property(GetBitmap, SetBitmap)
    Id     = property(GetId, SetId)
    Rect   = property(GetRect, SetRect)
    Status = property(GetStatus, SetStatus)
    

# -- ButtonPanel class implementation ----------------------------------
# This is the main class.

class ButtonPanel(wx.PyPanel):

    def __init__(self, parent, id=wx.ID_ANY, text="", style=BP_DEFAULT_STYLE,
                 alignment=BP_ALIGN_LEFT, name="buttonPanel"):
        """
        Default class constructor.

        - parent: parent window 
        - id: window ID 
        - text: text to draw 
        - style: window style
        - alignment: alignment of buttons (left or right)
        - name: window class name 
        """        
        
        wx.PyPanel.__init__(self, parent, id, wx.DefaultPosition, wx.DefaultSize,
                          wx.NO_BORDER, name=name)
        
        self._vButtons = []
        self._vSeparators = []

        self._nStyle = style
        self._alignment = alignment

        self._art = BPArt(style)

        self._controlCreated = False

        direction = (self.IsVertical() and [wx.VERTICAL] or [wx.HORIZONTAL])[0]            
        self._mainsizer = BoxSizer(direction)
        self.SetSizer(self._mainsizer)

        margins = self._art.GetMetric(BP_MARGINS_SIZE)
        
        # First spacer to create some room before the first text/button/control
        self._mainsizer.Add((margins.x, margins.y), 0)
        
        # Last spacer to create some room before the last text/button/control
        self._mainsizer.Add((margins.x, margins.y), 0)        

        self.Bind(wx.EVT_SIZE, self.OnSize)        
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)
        self.Bind(wx.EVT_MOTION, self.OnMouseMove)
        self.Bind(wx.EVT_LEFT_DOWN, self.OnLeftDown)
        self.Bind(wx.EVT_LEFT_UP, self.OnLeftUp)
        self.Bind(wx.EVT_LEAVE_WINDOW, self.OnMouseLeave)
        self.Bind(wx.EVT_ENTER_WINDOW, self.OnMouseEnterWindow)
        
        self.SetBarText(text)
        self.LayoutItems()
        
    
    def SetBarText(self, text):
        """ Sets the main caption text (leave text="" for no text). """

        self.Freeze()
        
        text = text.strip()

        if self._controlCreated:
            self.RemoveText()

        self._text = ButtonPanelText(self, text)
        lenChildren = len(self._mainsizer.GetChildren())
        
        if text == "":
            # Even if we have no text, we insert it an empty spacer anyway
            # it is easier to handle if you have to recreate the sizer after.
            if self.IsStandard():
                self._mainsizer.Insert(1, self._text, 0, wx.ALIGN_CENTER,
                                       userData=self._text, realIndex=0)
            else:
                self._mainsizer.Insert(lenChildren-1, self._text, 0, wx.ALIGN_CENTER,
                                       userData=self._text, realIndex=lenChildren)

            return

        # We have text, so insert the text and an expandable spacer
        # alongside it. "Standard" ButtonPanel are left or top aligned.
        if self.IsStandard():
            self._mainsizer.Insert(1, self._text, 0, wx.ALIGN_CENTER,
                                    userData=self._text, realIndex=0)
            self._mainsizer.Insert(2, (0, 0), 1, wx.EXPAND)
            
        else:
            self._mainsizer.Insert(lenChildren-1, self._text, 0, wx.ALIGN_CENTER,
                                   userData=self._text, realIndex=lenChildren)
            self._mainsizer.Insert(lenChildren-1, (0, 0), 1, wx.EXPAND)
                

    def RemoveText(self):
        """ Removes the main caption text. """
        
        lenChildren = len(self._mainsizer.GetChildren())
        lenCustom = len(self._vButtons) + len(self._vSeparators) + 1
        
        if self.IsStandard():
            # Detach the text
            self._mainsizer.Remove(1, 0)
            if self.HasBarText():
                # Detach the expandable spacer
                self._mainsizer.Remove(1, -1)
        else:
            # Detach the text
            self._mainsizer.Remove(lenChildren-2, lenCustom-1)
            if self.HasBarText():
                # Detach the expandable spacer            
                self._mainsizer.Remove(lenChildren-3, -1)

                    
    def GetBarText(self):
        """ Returns the main caption text. """

        return self._text.GetText()


    def HasBarText(self):
        """ Returns whether ButtonPanel has a main caption text or not. """

        return hasattr(self, "_text") and self._text.GetText() != ""

            
    def AddButton(self, btnInfo):
        """
        Adds a button to ButtonPanel. Remember to pass a ButtonInfo instance to
        this method. See the demo for details.
        """

        lenChildren = len(self._mainsizer.GetChildren())
        self._mainsizer.Insert(lenChildren-1, btnInfo, 0, wx.ALIGN_CENTER|wx.EXPAND, userData=btnInfo)
            
        self._vButtons.append(btnInfo)


    def AddSpacer(self, size=(0, 0), proportion=1, flag=wx.EXPAND):
        """ Adds a spacer (stretchable or fixed-size) to ButtonPanel. """

        lenChildren = len(self._mainsizer.GetChildren())
        self._mainsizer.Insert(lenChildren-1, size, proportion, flag)
            

    def AddControl(self, control, proportion=0, flag=wx.ALIGN_CENTER|wx.ALL, border=None):
        """ Adds a wxPython control to ButtonPanel. """

        lenChildren = len(self._mainsizer.GetChildren())
        
        if border is None:
            border = self._art.GetMetric(BP_PADDING_SIZE)
            border = max(border.x, border.y)

        self._mainsizer.Insert(lenChildren-1, control, proportion, flag, border)
        

    def AddSeparator(self):
        """ Adds a separator line to ButtonPanel. """

        lenChildren = len(self._mainsizer.GetChildren())
        separator = Separator(self)
        
        self._mainsizer.Insert(lenChildren-1, separator, 0, wx.EXPAND)
        self._vSeparators.append(separator)
        

    def RemoveAllButtons(self):
        """ Remove all the buttons from ButtonPanel. """

        self._vButtons = []

        
    def RemoveAllSeparators(self):
        """ Remove all the separators from ButtonPanel. """

        self._vSeparators = []

        
    def GetAlignment(self):
        """ Returns the button alignment (left, right, top, bottom). """

        return self._alignment
    

    def SetAlignment(self, alignment):
        """ Sets the button alignment (left, right, top, bottom). """

        if alignment == self._alignment:
            return

        self.Freeze()
        
        text = self.GetBarText()
        
        # Remove the text in any case
        self.RemoveText()

        # Remove the first and last spacers
        self._mainsizer.Remove(0, -1)
        self._mainsizer.Remove(len(self._mainsizer.GetChildren())-1, -1)
        
        self._alignment = alignment

        # Recreate the sizer accordingly to the new alignment
        self.ReCreateSizer(text)


    def IsVertical(self):
        """ Returns whether ButtonPanel is vertically aligned or not. """

        return self._alignment not in [BP_ALIGN_RIGHT, BP_ALIGN_LEFT]
        

    def IsStandard(self):
        """ Returns whether ButtonPanel is aligned "Standard" (left/top) or not. """

        return self._alignment in [BP_ALIGN_LEFT, BP_ALIGN_TOP]


    def DoLayout(self):
        """
        Do the Layout for ButtonPanel.
        NB: Call this method every time you make a modification to the layout
        or to the customizable sizes of the pseudo controls.
        """

        margins = self._art.GetMetric(BP_MARGINS_SIZE)
        lenChildren = len(self._mainsizer.GetChildren())

        self._mainsizer.SetItemMinSize(0, (margins.x, margins.y))
        self._mainsizer.SetItemMinSize(lenChildren-1, (margins.x, margins.y))
        
        self._controlCreated = True
        self.LayoutItems()

        # *VERY* WEIRD: the sizer seems not to respond to any layout until I
        # change the ButtonPanel size and restore it back
        size = self.GetSize()
        self.SetSize((size.x+1, size.y+1))
        self.SetSize((size.x, size.y))
        
        if self.IsFrozen():
            self.Thaw()
        

    def ReCreateSizer(self, text):
        """ Recreates the ButtonPanel sizer accordingly to the alignment specified. """
        
        children = self._mainsizer.GetChildren()
        self.RemoveAllButtons()
        self.RemoveAllSeparators()

        # Create a new sizer depending on the alignment chosen
        direction = (self.IsVertical() and [wx.VERTICAL] or [wx.HORIZONTAL])[0]            
        self._mainsizer = BoxSizer(direction)

        margins = self._art.GetMetric(BP_MARGINS_SIZE)
        # First spacer to create some room before the first text/button/control
        self._mainsizer.Add((margins.x, margins.y), 0)
        
        # Last spacer to create some room before the last text/button/control
        self._mainsizer.Add((margins.x, margins.y), 0)
                
        # This is needed otherwise SetBarText goes mad        
        self._controlCreated = False

        for child in children:
            userData = child.GetUserData()
            if userData:
                if isinstance(userData, ButtonInfo):
                    # It is a ButtonInfo, can't be anything else
                    self.AddButton(child.GetUserData())
                elif isinstance(userData, Separator):
                    self.AddSeparator()
                    
            else:
                if child.IsSpacer():
                    # This is a spacer, expandable or not
                    self.AddSpacer(child.GetSize(), child.GetProportion(),
                                   child.GetFlag())
                else:
                    # This is a wxPython control
                    self.AddControl(child.GetWindow(), child.GetProportion(),
                                    child.GetFlag(), child.GetBorder())

        self.SetSizer(self._mainsizer)

        # Now add the text. It doesn't matter if there is no text        
        self.SetBarText(text)
        
        self.DoLayout()
        
        self.Thaw()
        

    def DoGetBestSize(self):
        """ Returns the best size of ButtonPanel. """

        w = h = btnWidth = btnHeight = 0
        isVertical = self.IsVertical()

        padding = self._art.GetMetric(BP_PADDING_SIZE)
        border = self._art.GetMetric(BP_BORDER_SIZE)
        margins = self._art.GetMetric(BP_MARGINS_SIZE)
        separator_size = self._art.GetMetric(BP_SEPARATOR_SIZE)

        # Add the space required for the main caption        
        if self.HasBarText():
            w, h = self._text.GetBestSize()
            if isVertical:
                h += padding.y
            else:
                w += padding.x
        else:
            w = h = border

        # Add the button's sizes
        for btn in self._vButtons:
            
            bw, bh = btn.GetBestSize()            
            btnWidth = max(btnWidth, bw)
            btnHeight = max(btnHeight, bh)

            if isVertical:            
                w = max(w, btnWidth)
                h += bh
            else:
                h = max(h, btnHeight)
                w += bw

        # Add the control's sizes
        for control in self.GetControls():
            cw, ch = control.GetSize()
            if isVertical:
                h += ch
                w = max(w, cw)
            else:
                w += cw
                h = max(h, ch)

        # Add the separator's sizes and the 2 SizerItems at the beginning
        # and at the end
        if self.IsVertical():
            h += 2*margins.y + len(self._vSeparators)*separator_size
        else:
            w += 2*margins.x + len(self._vSeparators)*separator_size

        return wx.Size(w, h)


    def OnPaint(self, event):
        """ Handles the wx.EVT_PAINT event for ButtonPanel. """
        
        dc = wx.BufferedPaintDC(self) 
        rect = self.GetClientRect()

        self._art.DrawButtonPanel(dc, rect, self._nStyle)
        self._mainsizer.Draw(dc)
                

    def OnEraseBackground(self, event):
        """ Handles the wx.EVT_ERASE_BACKGROUND event for ButtonPanel (does nothing). """

        pass
    
 
    def OnSize(self, event):
        """ Handles the wx.EVT_SIZE event for ButtonPanel. """

        # NOTE: It seems like LayoutItems number of calls can be optimized in some way.
        # Currently every DoLayout (or every parent Layout()) calls about 3 times
        # the LayoutItems method. Any idea on how to improve it?
        self.LayoutItems()
        self.Refresh()

        event.Skip() 


    def LayoutItems(self):
        """
        Layout the items using a different algorithm depending on the existance
        of the main caption.
        """

        nonspacers, allchildren = self.GetNonFlexibleChildren()
        
        if self.HasBarText():
            self.FlexibleLayout(nonspacers, allchildren)
        else:
            self.SizeLayout(nonspacers, allchildren)
            
        self._mainsizer.Layout()


    def SizeLayout(self, nonspacers, children):
        """ Layout the items when no main caption exists. """

        size = self.GetSize()
        isVertical = self.IsVertical()
        
        corner = 0
        indx1 = len(nonspacers)

        for item in nonspacers:
            corner += self.GetItemSize(item, isVertical)
            if corner > size[isVertical]:
                indx1 = nonspacers.index(item)
                break

        # Leave out the last spacer, it has to be there always        
        for ii in xrange(len(nonspacers)-1):
            indx = children.index(nonspacers[ii])
            self._mainsizer.Show(indx, ii < indx1)
                

    def GetItemSize(self, item, isVertical):
        """ Returns the size of an item in the main ButtonPanel sizer. """
        
        if item.GetUserData():
            return item.GetUserData().GetBestSize()[isVertical]
        else:
            return item.GetSize()[isVertical]


    def FlexibleLayout(self, nonspacers, allchildren):
        """ Layout the items when the main caption exists. """

        if len(nonspacers) < 2:
            return
        
        isVertical = self.IsVertical()
        isStandard = self.IsStandard()
        
        size = self.GetSize()[isVertical]
        padding = self._art.GetMetric(BP_PADDING_SIZE)
        
        fixed = (isStandard and [nonspacers[1]] or [nonspacers[-2]])[0]
        
        if isStandard:
            nonspacers.reverse()
            leftendx = fixed.GetSize()[isVertical] + padding.x
        else:
            rightstartx = size - fixed.GetSize()[isVertical]
            size = 0

        count = lennonspacers = len(nonspacers)
        
        for item in nonspacers:
            if isStandard:
                size -= self.GetItemSize(item, isVertical)
                if size < leftendx:
                    break
            else:
                size += self.GetItemSize(item, isVertical)
                if size > rightstartx:
                    break
                
            count = count - 1

        nonspacers.reverse()
        
        for jj in xrange(2, lennonspacers):
            indx = allchildren.index(nonspacers[jj])
            self._mainsizer.Show(indx, jj >= count)

        
    def GetNonFlexibleChildren(self):
        """
        Returns all the ButtonPanel main sizer's children that are not
        flexible spacers.
        """

        children1 = []
        children2 = self._mainsizer.GetChildren()
        
        for child in children2:
            if child.IsSpacer():
                if child.GetUserData() or child.GetProportion() == 0:
                    children1.append(child)
            else:
                children1.append(child)

        return children1, children2


    def GetControls(self):
        """ Returns the wxPython controls that belongs to ButtonPanel. """
    
        children2 = self._mainsizer.GetChildren()
        children1 = [child for child in children2 if not child.IsSpacer()]

        return children1


    def SetStyle(self, style):
        """ Sets ButtonPanel style. """

        if style == self._nStyle:
            return

        self._nStyle = style
        self.Refresh()


    def GetStyle(self):
        """ Returns the ButtonPanel style. """

        return self._nStyle

        
    def OnMouseMove(self, event):
        """ Handles the wx.EVT_MOTION event for ButtonPanel. """
        
        # Check to see if we are hovering a button
        for btn in self._vButtons:

            if not btn.IsEnabled():
                continue

            if btn.GetRect().Contains(event.GetPosition()):
                btn.SetStatus("Hover")
            else:
                btn.SetStatus("Normal") 
                 
        self.Refresh() 
        event.Skip() 
 

    def OnLeftDown(self, event):
        """ Handles the wx.EVT_LEFT_DOWN event for ButtonPanel. """
 
        tabId, hit = self.HitTest(event.GetPosition())

        if hit == BP_HT_BUTTON and self._vButtons[tabId].IsEnabled():
             
            self._vButtons[tabId].SetStatus("Pressed")
            self.Refresh() 


    def OnLeftUp(self, event):
        """ Handles the wx.EVT_LEFT_UP event for ButtonPanel. """
        
        tabId, flags = self.HitTest(event.GetPosition())
        hit = self._vButtons[tabId]
        
        if flags == BP_HT_BUTTON:

            if hit.GetStatus() == "Disabled":
                return

            for btn in self._vButtons:
                if btn != hit:
                    btn.SetFocus(False)
                    
            if hit.GetStatus() == "Pressed": 
                # Fire a button click event 
                btnEvent = wx.CommandEvent(wx.wxEVT_COMMAND_BUTTON_CLICKED, hit.GetId())
                self.GetEventHandler().ProcessEvent(btnEvent) 

                hit.SetToggled(not hit.GetToggled())
                
                # Update the button status to be hovered 
                hit.SetStatus("Hover")
                hit.SetFocus()

                self.Refresh() 
                 

    def OnMouseLeave(self, event):
        """ Handles the wx.EVT_LEAVE_WINDOW event for ButtonPanel. """
        
        # Reset all buttons statuses
        for btn in self._vButtons:
            if not btn.IsEnabled():
                continue
            btn.SetStatus("Normal")
            
        self.Refresh() 
        event.Skip() 
 

    def OnMouseEnterWindow(self, event):
        """ Handles the wx.EVT_ENTER_WINDOW event for ButtonPanel. """
         
        event.Skip() 
 

    def HitTest(self, pt):
        """
        HitTest method for ButtonPanel. Returns the button (if any) and
        a flag (if any).
        """
         
        btnIdx = -1 

        for ii in xrange(len(self._vButtons)):
            if not self._vButtons[ii].IsEnabled():
                continue
            if self._vButtons[ii].GetRect().Contains(pt):
                return ii, BP_HT_BUTTON

        return -1, BP_HT_NONE
 

    def GetBPArt(self):
        """ Returns the associated BPArt art provider. """

        return self._art
    
