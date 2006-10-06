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
# Latest Revision: 02 Oct 2006, 17.00 GMT
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

          titleBar.SetColor(BP_TEXT_COLOR, wx.WHITE) 
          titleBar.SetColor(BP_CAPTION_BORDER_COLOR, wx.WHITE) 
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

Latest Revision: Andrea Gavana @ 02 Oct 2006, 17.00 GMT
Version 0.1.

"""


import wx

# Some constants
BP_CAPTION_COLOR = 0, 
BP_CAPTION_GRADIENT_COLOR = 1 
BP_CAPTION_BORDER_COLOR = 2
BP_TEXT_COLOR = 3

# Buttons states 
BP_BTN_NONE = 100
BP_BTN_PRESSED = 101
BP_BTN_HOVER = 102

# Flags for HitTest() method
BP_HT_BUTTON = 200
BP_HT_NONE = 201

# Alignment of buttons in the panel
BP_ALIGN_RIGHT = 1 
BP_ALIGN_LEFT = 2 

# ButtonPanel default style
BP_DEFAULT_STYLE = 2 

 
def BrightenColor(color, factor): 
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


# -- ButtonInfo class implementation ----------------------------------------
# This class holds information about every button that is added to
# ButtonPanel.  It is an auxiliary class that you should use
# every time you add a button.

class ButtonInfo:

    def __init__(self, id=wx.ID_ANY, bmp=wx.NullBitmap, status=-1):
        """
        Default class constructor.

        Parameters:
        - id: the button id;
        - bmp: the associated bitmap;
        - status: button status (pressed, hovered, None).
        """
        if id == wx.ID_ANY:
            id = wx.NewId()
        self._id = id
        self._bmp = bmp
        self._status = status
        self._rect = wx.Rect()
        

    def GetBitmap(self):
        """ Returns the associated bitmap. """

        return self._bmp


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


    def SetBitmap(self, bmp):
        """ Sets the associated bitmap. """

        self._bmp = bmp
        

    def SetStatus(self, status):
        """ Sets the button status. """

        self._status = status


    def SetId(self, id):
        """ Sets the button id. """

        self._id = id

    Bitmap = property(GetBitmap, SetBitmap)
    Id     = property(GetId, SetId)
    Rect   = property(GetRect, SetRect)
    Status = property(GetStatus, SetStatus)
    

    

# -- ButtonPanel class implementation ----------------------------------
# This is the main class.

class ButtonPanel(wx.PyPanel):

    def __init__(self, parent, id=wx.ID_ANY, text="", style=BP_DEFAULT_STYLE,
                 alignment=BP_ALIGN_RIGHT, name="buttonPanel"):
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

        self._text = text
        self._nStyle = style
        self._alignment = alignment
        self._nPadding = 6
        self._nBmpSize = 16
        self._colorFrom = wx.SystemSettings.GetColour(wx.SYS_COLOUR_ACTIVECAPTION)
        self._colorTo = wx.WHITE
        self._colorBorder = wx.SystemSettings.GetColour(wx.SYS_COLOUR_3DFACE)
        self._colorText = wx.SystemSettings.GetColour(wx.SYS_COLOUR_WINDOWTEXT)
        self._firsttime = True
        self._borderPenWidth = 3
        
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)
        self.Bind(wx.EVT_MOTION, self.OnMouseMove)
        self.Bind(wx.EVT_LEFT_DOWN, self.OnLeftDown)
        self.Bind(wx.EVT_LEFT_UP, self.OnLeftUp)
        self.Bind(wx.EVT_LEAVE_WINDOW, self.OnMouseLeave)
        self.Bind(wx.EVT_ENTER_WINDOW, self.OnMouseEnterWindow)
        

    def AddButton(self, btnInfo):
        """
        Adds a button to ButtonPanel. Remember to pass a ButtonInfo instance to
        this method. See the demo for details.
        """

        self._vButtons.append(btnInfo)
        self.Refresh()
 

    def RemoveAllButtons(self):
        """ Remove all the buttons from ButtonPanel. """

        self._vButtons = []
        self.Refresh()
        

    def GetAlignment(self):
        """ Returns the button alignment (left, right). """

        return self._alignment
    

    def SetAlignment(self, alignment):
        """ Sets the button alignment (left, right). """

        self._alignment = alignment


    def DoGetBestSize(self):
        w = h = 0
        if self._text:
            dc = wx.ClientDC(self)
            boldFont = wx.SystemSettings.GetFont(wx.SYS_DEFAULT_GUI_FONT) 
            boldFont.SetWeight(wx.FONTWEIGHT_BOLD)
            dc.SetFont(boldFont)
            tw, th = dc.GetTextExtent(self._text)
            h = max(h, th)
            w += tw + self._nPadding

        if self._vButtons:
            bh = self._vButtons[0].GetBitmap().GetHeight()
            bw = self._vButtons[0].GetBitmap().GetWidth()
            
            bh += 2*self._nPadding + 2*self._borderPenWidth
            h = max(h, bh)

            bw = (len(self._vButtons)+1) * (bw + 2*self._nPadding)
            w += bw
            
        return (w, h)



    def OnPaint(self, event):
        """ Handles the wx.EVT_PAINT event for ButtonPanel. """
        
        dc = wx.BufferedPaintDC(self) 
        rect = self.GetClientRect()

        ##print rect, self.GetRect(), self.GetBestSize(), self.GetMinSize()
        
        # Draw gradient color in the background of the panel 
        self.FillGradientColor(dc, rect) 

        backBrush = wx.TRANSPARENT_BRUSH
        borderPen = wx.Pen(self._colorBorder) 
        size = self.GetSize() 
        borderPen.SetWidth(self._borderPenWidth) 

        # Draw a rectangle around the panel 
        dc.SetBrush(backBrush) 
        dc.SetPen(borderPen) 
        dc.DrawRectangleRect(rect) 

        # Draw the text 
        textWidth, textHeight = 0, 0
        
        if self._text != "":
         
            dc.SetTextForeground(self._colorText) 
            borderPen.SetWidth(2) 
            boldFont = wx.SystemSettings.GetFont(wx.SYS_DEFAULT_GUI_FONT) 
            boldFont.SetWeight(wx.FONTWEIGHT_BOLD)
            dc.SetFont(boldFont)
            
            textWidth, textHeight = dc.GetTextExtent(self._text)

            if self._alignment == BP_ALIGN_RIGHT:
                textX = self._nPadding
            else:
                textX = rect.width - textWidth - self._nPadding
                
            textY = (rect.GetHeight() - textHeight)/2 
            dc.DrawText(self._text, textX, textY) 
         
        if self._vButtons:
         
            height = self._vButtons[0].GetBitmap().GetHeight() 
            self._nBmpSize = self._vButtons[0].GetBitmap().GetWidth() 
            height += 2*self._nPadding + 2*self._borderPenWidth

            if self._firsttime:  # this probably isn't needed anymore now that DoGetBestSize is implemented...
                self.GetContainingSizer().Layout()
                self._firsttime = False
            
            # Draw all buttons 
            # [ Padding | Text | .. Buttons .. | Padding ]
            
            totalWidth = rect.width - self._nPadding*2 - textWidth 

            # The button is drawn inside a circle with padding of self._nPadding around it 
            # so the width of each image = imageWidth + 2 * self._nPadding 
            nImageWidth = self._nBmpSize + 2*self._nPadding 

            if self._alignment == BP_ALIGN_RIGHT:

                leftEndX = self._nPadding + textWidth 
                posx = rect.width - nImageWidth - self._nPadding
                
                for ii in xrange(len(self._vButtons)):
                 
                    # Make sure we can keep drawing 
                    if posx < leftEndX:
                        break 
                    
                    # Draw a rectangle around the buttons 
                    if self._vButtons[ii].GetStatus() == BP_BTN_HOVER:
                     
                        dc.SetBrush(wx.Brush(wx.Color(225, 225, 255))) 
                        dc.SetPen(wx.Pen(wx.SystemSettings.GetColour(wx.SYS_COLOUR_ACTIVECAPTION))) 
                        dc.DrawRectangle(posx, self._borderPenWidth, nImageWidth, nImageWidth) 
                        dc.DrawBitmap(self._vButtons[ii].GetBitmap(), posx + self._nPadding, self._nPadding + self._borderPenWidth, True) 
                     
                    elif self._vButtons[ii].GetStatus() == BP_BTN_PRESSED:
                     
                        dc.SetBrush(wx.Brush(wx.Color(225, 225, 255))) 
                        dc.SetPen(wx.Pen(wx.SystemSettings.GetColour(wx.SYS_COLOUR_ACTIVECAPTION))) 
                        dc.DrawRectangle(posx, self._borderPenWidth, nImageWidth, nImageWidth) 
                        dc.DrawBitmap(self._vButtons[ii].GetBitmap(), posx + self._nPadding, self._nPadding + self._borderPenWidth + 1, True) 
                     
                    else:
                     
                        dc.DrawBitmap(self._vButtons[ii].GetBitmap(), posx + self._nPadding, self._nPadding + self._borderPenWidth, True) 
                 
                    self._vButtons[ii].SetRect(wx.Rect(posx, self._borderPenWidth, nImageWidth, nImageWidth)) 
                    posx -= nImageWidth 

            else:

                rightStartX = textX - self._nPadding - nImageWidth
                posx = self._nPadding
                
                for ii in xrange(len(self._vButtons)):
                 
                    # Make sure we can keep drawing 
                    if posx > rightStartX:
                        break 
                    
                    # Draw a rectangle around the buttons 
                    if self._vButtons[ii].GetStatus() == BP_BTN_HOVER:
                     
                        dc.SetBrush(wx.Brush(wx.Color(225, 225, 255))) 
                        dc.SetPen(wx.Pen(wx.SystemSettings.GetColour(wx.SYS_COLOUR_ACTIVECAPTION))) 
                        dc.DrawRectangle(posx, self._borderPenWidth, nImageWidth, nImageWidth) 
                        dc.DrawBitmap(self._vButtons[ii].GetBitmap(), posx + self._nPadding, self._nPadding + self._borderPenWidth, True) 
                     
                    elif self._vButtons[ii].GetStatus() == BP_BTN_PRESSED:
                     
                        dc.SetBrush(wx.Brush(wx.Color(225, 225, 255))) 
                        dc.SetPen(wx.Pen(wx.SystemSettings.GetColour(wx.SYS_COLOUR_ACTIVECAPTION))) 
                        dc.DrawRectangle(posx, self._borderPenWidth, nImageWidth, nImageWidth) 
                        dc.DrawBitmap(self._vButtons[ii].GetBitmap(), posx + self._nPadding, self._nPadding + self._borderPenWidth + 1, True) 
                     
                    else:
                     
                        dc.DrawBitmap(self._vButtons[ii].GetBitmap(), posx + self._nPadding, self._nPadding + self._borderPenWidth, True) 
                 
                    self._vButtons[ii].SetRect(wx.Rect(posx, self._borderPenWidth, nImageWidth, nImageWidth)) 
                    posx += nImageWidth 
               
            # Update all other buttons that they are not drawn (by setting the rect to 0) 
            for cur in xrange(ii+1, len(self._vButtons)):
                self._vButtons[cur].SetRect(wx.Rect(0, 0, 0, 0)) 


    def OnEraseBackground(self, event):
        """ Handles the wx.EVT_ERASE_BACKGROUND event for ButtonPanel (does nothing). """
        
        pass
    
 
    def OnSize(self, event):
        """ Handles the wx.EVT_SIZE event for ButtonPanel. """
 
        self.Refresh() 
        event.Skip() 
 

    def SetColor(self, switch, color):
        """
        Sets the color depending on switch:
        - BP_CAPTION_COLOR: caption color;
        - BP_CAPTION_GRADIENT_COLOR: gradient color; 
        - BP_CAPTION_BORDER_COLOR; border color;
        - BP_TEXT_COLOR: text color. 
         """
         
        if switch == BP_CAPTION_COLOR:
            self._colorFrom = color  
        elif switch == BP_CAPTION_GRADIENT_COLOR:
            self._colorTo = color 
        elif switch == BP_CAPTION_BORDER_COLOR:
            self._colorBorder = color 
        elif switch == BP_TEXT_COLOR:
            self._colorText = color 
         
 
    def FillGradientColor(self, dc, rect):
        """ Gradient fill from colour 1 to colour 2 with top to bottom. """

        if rect.height < 1 or rect.width < 1: 
            return 

        size = rect.height 

        # calculate gradient coefficients 
        style = self.GetParent().GetWindowStyleFlag() 
        col2 = self._colorFrom 
        col1 = self._colorTo 

        rf, gf, bf = 0, 0, 0
        rstep = float((col2.Red() - col1.Red()))/float(size)
        gstep = float((col2.Green() - col1.Green()))/float(size)
        bstep = float((col2.Blue() - col1.Blue()))/float(size)

        for y in xrange(rect.y, rect.y + size):
         
            currCol = wx.Colour(col1.Red() + rf, col1.Green() + gf, col1.Blue() + bf)
            dc.SetBrush(wx.Brush(currCol, wx.SOLID)) 
            dc.SetPen(wx.Pen(currCol)) 
            dc.DrawLine(rect.x, y, rect.x + rect.width, y) 
            rf += rstep
            gf += gstep
            bf += bstep 

         
    def OnMouseMove(self, event):
        """ Handles the wx.EVT_MOTION event for ButtonPanel. """
        
        # Check to see if we are hovering a button
        for ii in xrange(len(self._vButtons)):
            if self._vButtons[ii].GetRect().Inside(event.GetPosition()):
                self._vButtons[ii].SetStatus(BP_BTN_HOVER) 
            else:
                self._vButtons[ii].SetStatus(BP_BTN_NONE) 
                 
        self.Refresh() 
        event.Skip() 
 

    def OnLeftDown(self, event):
        """ Handles the wx.EVT_LEFT_DOWN event for ButtonPanel. """
 
        tabId, hit = self.HitTest(event.GetPosition())

        if hit == BP_HT_BUTTON:
         
            self._vButtons[tabId].SetStatus(BP_BTN_PRESSED) 
            self.Refresh() 


    def OnLeftUp(self, event):
        """ Handles the wx.EVT_LEFT_UP event for ButtonPanel. """
        
        tabId, hit = self.HitTest(event.GetPosition())
        
        if hit == BP_HT_BUTTON:
            if self._vButtons[tabId].GetStatus() == BP_BTN_PRESSED:                 
                # Fire a button click event 
                btnEvent = wx.CommandEvent(wx.wxEVT_COMMAND_BUTTON_CLICKED, self._vButtons[tabId].GetId())
                self.GetEventHandler().ProcessEvent(btnEvent) 

                # Update the button status to be hovered 
                self._vButtons[tabId].SetStatus(BP_BTN_HOVER) 
                self.Refresh() 
                 

    def OnMouseLeave(self, event):
        """ Handles the wx.EVT_LEAVE_WINDOW event for ButtonPanel. """
        
        # Reset all buttons statuses 
        for ii in xrange(len(self._vButtons)):
            self._vButtons[ii].SetStatus(BP_BTN_NONE)
            
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
            if self._vButtons[ii].GetRect().Inside(pt):
                return ii, BP_HT_BUTTON

        return -1, BP_HT_NONE
 


