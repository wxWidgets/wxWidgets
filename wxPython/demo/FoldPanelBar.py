
import  wx
import wx.lib.foldpanelbar as fpb


# ----------------------------------------------------------------------------
# Extended Demo Implementation
#
# This demo shows how to use custom CaptionBar styles and custom icons for
# the caption bars. here i used the standard Windows XP icons for the
# collapsed and expanded state.
# ----------------------------------------------------------------------------

#----------------------------------------------------------------------
# different icons for the collapsed/expanded states.
# Taken from standard Windows XP collapsed/expanded states.
#----------------------------------------------------------------------

def GetCollapsedIconData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x01\x8eIDAT8\x8d\xa5\x93-n\xe4@\x10\x85?g\x03\n6lh)\xc4\xd2\x12\xc3\x81\
\xd6\xa2I\x90\x154\xb9\x81\x8f1G\xc8\x11\x16\x86\xcd\xa0\x99F\xb3A\x91\xa1\
\xc9J&\x96L"5lX\xcc\x0bl\xf7v\xb2\x7fZ\xa5\x98\xebU\xbdz\xf5\\\x9deW\x9f\xf8\
H\\\xbfO|{y\x9dT\x15P\x04\x01\x01UPUD\x84\xdb/7YZ\x9f\xa5\n\xce\x97aRU\x8a\
\xdc`\xacA\x00\x04P\xf0!0\xf6\x81\xa0\xf0p\xff9\xfb\x85\xe0|\x19&T)K\x8b\x18\
\xf9\xa3\xe4\xbe\xf3\x8c^#\xc9\xd5\n\xa8*\xc5?\x9a\x01\x8a\xd2b\r\x1cN\xc3\
\x14\t\xce\x97a\xb2F0Ks\xd58\xaa\xc6\xc5\xa6\xf7\xdfya\xe7\xbdR\x13M2\xf9\
\xf9qKQ\x1fi\xf6-\x00~T\xfac\x1dq#\x82,\xe5q\x05\x91D\xba@\xefj\xba1\xf0\xdc\
zzW\xcff&\xb8,\x89\xa8@Q\xd6\xaaf\xdfRm,\xee\xb1BDxr#\xae\xf5|\xddo\xd6\xe2H\
\x18\x15\x84\xa0q@]\xe54\x8d\xa3\xedf\x05M\xe3\xd8Uy\xc4\x15\x8d\xf5\xd7\x8b\
~\x82\x0fh\x0e"\xb0\xad,\xee\xb8c\xbb\x18\xe7\x8e;6\xa5\x89\x04\xde\xff\x1c\
\x16\xef\xe0p\xfa>\x19\x11\xca\x8d\x8d\xe0\x93\x1b\x01\xd8m\xf3(;x\xa5\xef=\
\xb7w\xf3\x1d$\x7f\xc1\xe0\xbd\xa7\xeb\xa0(,"Kc\x12\xc1+\xfd\xe8\tI\xee\xed)\
\xbf\xbcN\xc1{D\x04k\x05#\x12\xfd\xf2a\xde[\x81\x87\xbb\xdf\x9cr\x1a\x87\xd3\
0)\xba>\x83\xd5\xb97o\xe0\xaf\x04\xff\x13?\x00\xd2\xfb\xa9`z\xac\x80w\x00\
\x00\x00\x00IEND\xaeB`\x82' 

def GetCollapsedIconBitmap():
    return wx.BitmapFromImage(GetCollapsedIconImage())

def GetCollapsedIconImage():
    import cStringIO
    stream = cStringIO.StringIO(GetCollapsedIconData())
    return wx.ImageFromStream(stream)

#----------------------------------------------------------------------
def GetExpandedIconData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x01\x9fIDAT8\x8d\x95\x93\xa1\x8e\xdc0\x14EO\xb2\xc4\xd0\xd2\x12\xb7(mI\
\xa4%V\xd1lQT4[4-\x9a\xfe\xc1\xc2|\xc6\xc2~BY\x83:A3E\xd3\xa0*\xa4\xd2\x90H!\
\x95\x0c\r\r\x1fK\x81g\xb2\x99\x84\xb4\x0fY\xd6\xbb\xc7\xf7>=\'Iz\xc3\xbcv\
\xfbn\xb8\x9c\x15 \xe7\xf3\xc7\x0fw\xc9\xbc7\x99\x03\x0e\xfbn0\x99F+\x85R\
\x80RH\x10\x82\x08\xde\x05\x1ef\x90+\xc0\xe1\xd8\ryn\xd0Z-\\A\xb4\xd2\xf7\
\x9e\xfbwoF\xc8\x088\x1c\xbbae\xb3\xe8y&\x9a\xdf\xf5\xbd\xe7\xfem\x84\xa4\
\x97\xccYf\x16\x8d\xdb\xb2a]\xfeX\x18\xc9s\xc3\xe1\x18\xe7\x94\x12cb\xcc\xb5\
\xfa\xb1l8\xf5\x01\xe7\x84\xc7\xb2Y@\xb2\xcc0\x02\xb4\x9a\x88%\xbe\xdc\xb4\
\x9e\xb6Zs\xaa74\xadg[6\x88<\xb7]\xc6\x14\x1dL\x86\xe6\x83\xa0\x81\xba\xda\
\x10\x02x/\xd4\xd5\x06\r\x840!\x9c\x1fM\x92\xf4\x86\x9f\xbf\xfe\x0c\xd6\x9ae\
\xd6u\x8d \xf4\xf5\x165\x9b\x8f\x04\xe1\xc5\xcb\xdb$\x05\x90\xa97@\x04lQas\
\xcd*7\x14\xdb\x9aY\xcb\xb8\\\xe9E\x10|\xbc\xf2^\xb0E\x85\xc95_\x9f\n\xaa/\
\x05\x10\x81\xce\xc9\xa8\xf6><G\xd8\xed\xbbA)X\xd9\x0c\x01\x9a\xc6Q\x14\xd9h\
[\x04\xda\xd6c\xadFkE\xf0\xc2\xab\xd7\xb7\xc9\x08\x00\xf8\xf6\xbd\x1b\x8cQ\
\xd8|\xb9\x0f\xd3\x9a\x8a\xc7\x08\x00\x9f?\xdd%\xde\x07\xda\x93\xc3{\x19C\
\x8a\x9c\x03\x0b8\x17\xe8\x9d\xbf\x02.>\x13\xc0n\xff{PJ\xc5\xfdP\x11""<\xbc\
\xff\x87\xdf\xf8\xbf\xf5\x17FF\xaf\x8f\x8b\xd3\xe6K\x00\x00\x00\x00IEND\xaeB\
`\x82' 

def GetExpandedIconBitmap():
    return wx.BitmapFromImage(GetExpandedIconImage())

def GetExpandedIconImage():
    import cStringIO
    stream = cStringIO.StringIO(GetExpandedIconData())
    return wx.ImageFromStream(stream)

#----------------------------------------------------------------------
def GetMondrianData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00 \x00\x00\x00 \x08\x06\x00\
\x00\x00szz\xf4\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\x00\x00qID\
ATX\x85\xed\xd6;\n\x800\x10E\xd1{\xc5\x8d\xb9r\x97\x16\x0b\xad$\x8a\x82:\x16\
o\xda\x84pB2\x1f\x81Fa\x8c\x9c\x08\x04Z{\xcf\xa72\xbcv\xfa\xc5\x08 \x80r\x80\
\xfc\xa2\x0e\x1c\xe4\xba\xfaX\x1d\xd0\xde]S\x07\x02\xd8>\xe1wa-`\x9fQ\xe9\
\x86\x01\x04\x10\x00\\(Dk\x1b-\x04\xdc\x1d\x07\x14\x98;\x0bS\x7f\x7f\xf9\x13\
\x04\x10@\xf9X\xbe\x00\xc9 \x14K\xc1<={\x00\x00\x00\x00IEND\xaeB`\x82' 

def GetMondrianBitmap():
    return wx.BitmapFromImage(GetMondrianImage())

def GetMondrianImage():
    import cStringIO
    stream = cStringIO.StringIO(GetMondrianData())
    return wx.ImageFromStream(stream)

def GetMondrianIcon():
    icon = wx.EmptyIcon()
    icon.CopyFromBitmap(GetMondrianBitmap())
    return icon

# ----------------------------------------------------------------------------
# Beginning Of Extended Demo
# ----------------------------------------------------------------------------

class Extended(wx.Frame):
    
    def __init__(self, parent, id=wx.ID_ANY, title="", pos=wx.DefaultPosition,
                 size=(700,650), style=wx.DEFAULT_FRAME_STYLE):

        wx.Frame.__init__(self, parent, id, title, pos, size, style)

        self._flags = 0
        
        self.SetIcon(GetMondrianIcon())
        self.SetMenuBar(self.CreateMenuBar())

        self.statusbar = self.CreateStatusBar(2, wx.ST_SIZEGRIP)
        self.statusbar.SetStatusWidths([-4, -3])
        self.statusbar.SetStatusText("Andrea Gavana @ 23 Mar 2005", 0)
        self.statusbar.SetStatusText("Welcome to wxPython!", 1)

        self._leftWindow1 = wx.SashLayoutWindow(self, 101, wx.DefaultPosition,
                                                wx.Size(200, 1000), wx.NO_BORDER |
                                                wx.SW_3D | wx.CLIP_CHILDREN)

        self._leftWindow1.SetDefaultSize(wx.Size(220, 1000))
        self._leftWindow1.SetOrientation(wx.LAYOUT_VERTICAL)
        self._leftWindow1.SetAlignment(wx.LAYOUT_LEFT)
        self._leftWindow1.SetSashVisible(wx.SASH_RIGHT, True)
        self._leftWindow1.SetExtraBorderSize(10)

        self._pnl = 0

        # will occupy the space not used by the Layout Algorithm
        self.remainingSpace = wx.Panel(self, -1, style=wx.SUNKEN_BORDER)
        wx.StaticText(self.remainingSpace, -1,
                      "Use your imagination for what kinds of things to put in this window...",
                      (15,30))

        self.ID_WINDOW_TOP = 100
        self.ID_WINDOW_LEFT1 = 101
        self.ID_WINDOW_RIGHT1 = 102
        self.ID_WINDOW_BOTTOM = 103
    
        self._leftWindow1.Bind(wx.EVT_SASH_DRAGGED_RANGE, self.OnFoldPanelBarDrag,
                               id=100, id2=103)
        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_SCROLL, self.OnSlideColour)
        
        self.ReCreateFoldPanel(0)
        

    def OnSize(self, event):

        wx.LayoutAlgorithm().LayoutWindow(self, self.remainingSpace)
        event.Skip()
        

    def OnQuit(self, event):
 
        self.Destroy()


    def OnAbout(self, event):

        msg = "This is the about dialog of the FoldPanelBar demo.\n\n" + \
              "Author: Andrea Gavana @ 23 Mar 2005\n\n" + \
              "Please report any bug/requests or improvements\n" + \
              "To me at the following adresses:\n\n" + \
              "andrea.gavana@agip.it\n" + "andrea_gavana@tin.it\n\n" + \
              "Based On Julian Smart C++ demo implementation.\n\n" + \
              "Welcome To wxPython " + wx.VERSION_STRING + "!!"
              
        dlg = wx.MessageDialog(self, msg, "FoldPanelBar Extended Demo",
                               wx.OK | wx.ICON_INFORMATION)
        dlg.SetFont(wx.Font(8, wx.NORMAL, wx.NORMAL, wx.NORMAL, False, "Verdana"))
        dlg.ShowModal()
        dlg.Destroy()


    def OnToggleWindow(self, event):
        
        self._leftWindow1.Show(not self._leftWindow1.IsShown())
        # Leaves bits of itself behind sometimes
        wx.LayoutAlgorithm().LayoutWindow(self, self.remainingSpace)
        self.remainingSpace.Refresh()

        event.Skip()
        

    def OnFoldPanelBarDrag(self, event):

        if event.GetDragStatus() == wx.SASH_STATUS_OUT_OF_RANGE:
            return

        if event.GetId() == self.ID_WINDOW_LEFT1:
            self._leftWindow1.SetDefaultSize(wx.Size(event.GetDragRect().width, 1000))


        # Leaves bits of itself behind sometimes
        wx.LayoutAlgorithm().LayoutWindow(self, self.remainingSpace)
        self.remainingSpace.Refresh()

        event.Skip()
        

    def ReCreateFoldPanel(self, fpb_flags):

        # delete earlier panel
        self._leftWindow1.DestroyChildren()

        # recreate the foldpanelbar

        self._pnl = fpb.FoldPanelBar(self._leftWindow1, -1, wx.DefaultPosition,
                                     wx.Size(-1,-1), fpb.FPB_DEFAULT_STYLE, fpb_flags)

        Images = wx.ImageList(16,16)
        Images.Add(GetExpandedIconBitmap())
        Images.Add(GetCollapsedIconBitmap())
            
        item = self._pnl.AddFoldPanel("Caption Colours", collapsed=False,
                                      foldIcons=Images)

        self._pnl.AddFoldPanelWindow(item, wx.StaticText(item, -1, "Adjust The First Colour"),
                                     fpb.FPB_ALIGN_WIDTH, 5, 20) 

        # RED color spin control
        self._rslider1 = wx.Slider(item, -1, 0, 0, 255)
        self._pnl.AddFoldPanelWindow(item, self._rslider1, fpb.FPB_ALIGN_WIDTH, 2, 20) 

        # GREEN color spin control
        self._gslider1 = wx.Slider(item, -1, 0, 0, 255)
        self._pnl.AddFoldPanelWindow(item, self._gslider1, fpb.FPB_ALIGN_WIDTH, 0, 20) 

        # BLUE color spin control
        self._bslider1 = wx.Slider(item, -1, 0, 0, 255)
        self._pnl.AddFoldPanelWindow(item, self._bslider1, fpb.FPB_ALIGN_WIDTH,  0, 20) 
        
        self._pnl.AddFoldPanelSeparator(item)

        self._pnl.AddFoldPanelWindow(item, wx.StaticText(item, -1, "Adjust The Second Colour"),
                                     fpb.FPB_ALIGN_WIDTH, 5, 20) 

        # RED color spin control
        self._rslider2 = wx.Slider(item, -1, 0, 0, 255)
        self._pnl.AddFoldPanelWindow(item, self._rslider2, fpb.FPB_ALIGN_WIDTH, 2, 20) 

        # GREEN color spin control
        self._gslider2 = wx.Slider(item, -1, 0, 0, 255)
        self._pnl.AddFoldPanelWindow(item, self._gslider2, fpb.FPB_ALIGN_WIDTH, 0, 20) 

        # BLUE color spin control
        self._bslider2 = wx.Slider(item, -1, 0, 0, 255)
        self._pnl.AddFoldPanelWindow(item, self._bslider2, fpb.FPB_ALIGN_WIDTH, 0, 20) 

        self._pnl.AddFoldPanelSeparator(item)
        
        button1 = wx.Button(item, wx.ID_ANY, "Apply To All")
        button1.Bind(wx.EVT_BUTTON, self.OnExpandMe)
        self._pnl.AddFoldPanelWindow(item, button1)

        # read back current gradients and set the sliders
        # for the colour which is now taken as default

        style = self._pnl.GetCaptionStyle(item)
        col = style.GetFirstColour()

        self._rslider1.SetValue(col.Red())
        self._gslider1.SetValue(col.Green())
        self._bslider1.SetValue(col.Blue())

        col = style.GetSecondColour()
        self._rslider2.SetValue(col.Red())
        self._gslider2.SetValue(col.Green())
        self._bslider2.SetValue(col.Blue())

        # put down some caption styles from which the user can
        # select to show how the current or all caption bars will look like

        item = self._pnl.AddFoldPanel("Caption Style", False, foldIcons=Images)

        self.ID_USE_VGRADIENT = wx.NewId()
        self.ID_USE_HGRADIENT = wx.NewId()
        self.ID_USE_SINGLE = wx.NewId()
        self.ID_USE_RECTANGLE = wx.NewId()
        self.ID_USE_FILLED_RECTANGLE = wx.NewId()
        
        currStyle =  wx.RadioButton(item, self.ID_USE_VGRADIENT, "&Vertical Gradient")
        self._pnl.AddFoldPanelWindow(item, currStyle, fpb.FPB_ALIGN_WIDTH,
                                     fpb.FPB_DEFAULT_SPACING, 10)
        
        currStyle.SetValue(True)

        radio1 = wx.RadioButton(item, self.ID_USE_HGRADIENT, "&Horizontal Gradient")
        radio2 = wx.RadioButton(item, self.ID_USE_SINGLE, "&Single Colour")
        radio3 = wx.RadioButton(item, self.ID_USE_RECTANGLE, "&Rectangle Box")
        radio4 = wx.RadioButton(item, self.ID_USE_FILLED_RECTANGLE, "&Filled Rectangle Box")

        currStyle.Bind(wx.EVT_RADIOBUTTON, self.OnStyleChange)
        radio1.Bind(wx.EVT_RADIOBUTTON, self.OnStyleChange)
        radio2.Bind(wx.EVT_RADIOBUTTON, self.OnStyleChange)
        radio3.Bind(wx.EVT_RADIOBUTTON, self.OnStyleChange)
        radio4.Bind(wx.EVT_RADIOBUTTON, self.OnStyleChange)
        
        self._pnl.AddFoldPanelWindow(item, radio1, fpb.FPB_ALIGN_WIDTH, fpb.FPB_DEFAULT_SPACING, 10) 
        self._pnl.AddFoldPanelWindow(item, radio2, fpb.FPB_ALIGN_WIDTH, fpb.FPB_DEFAULT_SPACING, 10) 
        self._pnl.AddFoldPanelWindow(item, radio3, fpb.FPB_ALIGN_WIDTH, fpb.FPB_DEFAULT_SPACING, 10) 
        self._pnl.AddFoldPanelWindow(item, radio4, fpb.FPB_ALIGN_WIDTH, fpb.FPB_DEFAULT_SPACING, 10) 

        self._pnl.AddFoldPanelSeparator(item)

        self._single = wx.CheckBox(item, -1, "&Only This Caption")
        self._pnl.AddFoldPanelWindow(item, self._single, fpb.FPB_ALIGN_WIDTH,
                                     fpb.FPB_DEFAULT_SPACING, 10) 

        # one more panel to finish it

        cs = fpb.CaptionBarStyle()
        cs.SetCaptionStyle(fpb.CAPTIONBAR_RECTANGLE)

        item = self._pnl.AddFoldPanel("Misc Stuff", collapsed=True, foldIcons=Images,
                                      cbstyle=cs)

        button2 = wx.Button(item, wx.NewId(), "Collapse All")        
        self._pnl.AddFoldPanelWindow(item, button2) 
        self._pnl.AddFoldPanelWindow(item, wx.StaticText(item, -1, "Enter Some Comments"),
                                     fpb.FPB_ALIGN_WIDTH, 5, 20) 
        self._pnl.AddFoldPanelWindow(item, wx.TextCtrl(item, -1, "Comments"),
                                     fpb.FPB_ALIGN_WIDTH, fpb.FPB_DEFAULT_SPACING, 10)

        button2.Bind(wx.EVT_BUTTON, self.OnCollapseMe)
        self.radiocontrols = [currStyle, radio1, radio2, radio3, radio4]
        
        self._leftWindow1.SizeWindows()
        

    def OnCreateBottomStyle(self, event):

        # recreate with style collapse to bottom, which means
        # all panels that are collapsed are placed at the bottom,
        # or normal
        
        if event.IsChecked():
            self._flags = self._flags | fpb.FPB_COLLAPSE_TO_BOTTOM
        else:
            self._flags = self._flags & ~fpb.FPB_COLLAPSE_TO_BOTTOM

        self.ReCreateFoldPanel(self._flags)


    def OnCreateNormalStyle(self, event):

        # recreate with style where only one panel at the time is
        # allowed to be opened
        
        # TODO: Not yet implemented even in the C++ class!!!!

        if event.IsChecked():
            self._flags = self._flags | fpb.FPB_SINGLE_FOLD
        else:
            self._flags = self._flags & ~fpb.FPB_SINGLE_FOLD

        self.ReCreateFoldPanel(self._flags)


    def OnCollapseMe(self, event):

        for i in range(0, self._pnl.GetCount()):
            item = self._pnl.GetFoldPanel(i)
            self._pnl.Collapse(item)


    def OnExpandMe(self, event):

        col1 = wx.Colour(self._rslider1.GetValue(), self._gslider1.GetValue(),
                         self._bslider1.GetValue())
        col2 = wx.Colour(self._rslider2.GetValue(), self._gslider2.GetValue(),
                         self._bslider2.GetValue())

        style = fpb.CaptionBarStyle()

        style.SetFirstColour(col1)
        style.SetSecondColour(col2)

        counter = 0
        for items in self.radiocontrols:
            if items.GetValue():
                break
            counter = counter + 1
            
        if counter == 0:
            mystyle = fpb.CAPTIONBAR_GRADIENT_V
        elif counter == 1:
            mystyle = fpb.CAPTIONBAR_GRADIENT_H
        elif counter == 2:
            mystyle = fpb.CAPTIONBAR_SINGLE
        elif counter == 3:
            mystyle = fpb.CAPTIONBAR_RECTANGLE
        else:
            mystyle = fpb.CAPTIONBAR_FILLED_RECTANGLE

        style.SetCaptionStyle(mystyle)
        self._pnl.ApplyCaptionStyleAll(style)


    def OnSlideColour(self, event):

        col1 = wx.Colour(self._rslider1.GetValue(), self._gslider1.GetValue(),
                         self._bslider1.GetValue())
        col2 = wx.Colour(self._rslider2.GetValue(), self._gslider2.GetValue(),
                         self._bslider2.GetValue())

        style = fpb.CaptionBarStyle()

        counter = 0
        for items in self.radiocontrols:
            if items.GetValue():
                break
            
            counter = counter + 1

        if counter == 0:
            mystyle = fpb.CAPTIONBAR_GRADIENT_V
        elif counter == 1:
            mystyle = fpb.CAPTIONBAR_GRADIENT_H
        elif counter == 2:
            mystyle = fpb.CAPTIONBAR_SINGLE
        elif counter == 3:
            mystyle = fpb.CAPTIONBAR_RECTANGLE
        else:
            mystyle = fpb.CAPTIONBAR_FILLED_RECTANGLE
            
        style.SetFirstColour(col1)
        style.SetSecondColour(col2)
        style.SetCaptionStyle(mystyle)

        item = self._pnl.GetFoldPanel(0)
        self._pnl.ApplyCaptionStyle(item, style)
        

    def OnStyleChange(self, event):

        style = fpb.CaptionBarStyle()
        
        eventid = event.GetId()
        
        if eventid == self.ID_USE_HGRADIENT:
            style.SetCaptionStyle(fpb.CAPTIONBAR_GRADIENT_H)
            
        elif eventid == self.ID_USE_VGRADIENT:
            style.SetCaptionStyle(fpb.CAPTIONBAR_GRADIENT_V)
            
        elif eventid == self.ID_USE_SINGLE:
            style.SetCaptionStyle(fpb.CAPTIONBAR_SINGLE)
            
        elif eventid == self.ID_USE_RECTANGLE:
            style.SetCaptionStyle(fpb.CAPTIONBAR_RECTANGLE)
            
        elif eventid == self.ID_USE_FILLED_RECTANGLE:
            style.SetCaptionStyle(fpb.CAPTIONBAR_FILLED_RECTANGLE)
                
        else:
            raise "ERROR: Undefined Style Selected For CaptionBar: " + repr(eventid)

        col1 = wx.Colour(self._rslider1.GetValue(), self._gslider1.GetValue(),
                         self._bslider1.GetValue())
        col2 = wx.Colour(self._rslider2.GetValue(), self._gslider2.GetValue(),
                         self._bslider2.GetValue())

        style.SetFirstColour(col1)
        style.SetSecondColour(col2)

        if self._single.GetValue():
            item = self._pnl.GetFoldPanel(1)
            self._pnl.ApplyCaptionStyle(item, style)
        else:
            self._pnl.ApplyCaptionStyleAll(style)
    

    def CreateMenuBar(self, with_window=False):

        # Make a menubar
        file_menu = wx.Menu()

        FPBTEST_QUIT = wx.NewId()
        FPBTEST_REFRESH = wx.NewId()
        FPB_BOTTOM_STICK = wx.NewId()
        FPB_SINGLE_FOLD = wx.NewId()
        FPBTEST_TOGGLE_WINDOW = wx.NewId()
        FPBTEST_ABOUT = wx.NewId()
        
        file_menu.Append(FPBTEST_QUIT, "&Exit")

        option_menu = None

        if with_window:
            # Dummy option
            option_menu = wx.Menu()
            option_menu.Append(FPBTEST_REFRESH, "&Refresh picture")

        # make fold panel menu
        
        fpb_menu = wx.Menu()
        fpb_menu.AppendCheckItem(FPB_BOTTOM_STICK, "Create with &fpb.FPB_COLLAPSE_TO_BOTTOM")
        
        # Not Yet Implemented In The C++ class!!!
        # fpb_menu.AppendCheckItem(FPB_SINGLE_FOLD, _T("Create with &FPB_SINGLE_FOLD"))

        fpb_menu.AppendSeparator()
        fpb_menu.Append(FPBTEST_TOGGLE_WINDOW, "&Toggle FoldPanelBar")

        help_menu = wx.Menu()
        help_menu.Append(FPBTEST_ABOUT, "&About")

        menu_bar = wx.MenuBar()

        menu_bar.Append(file_menu, "&File")
        menu_bar.Append(fpb_menu, "&FoldPanel")
        
        if option_menu:
            menu_bar.Append(option_menu, "&Options")
            
        menu_bar.Append(help_menu, "&Help")

        self.Bind(wx.EVT_MENU, self.OnAbout, id=FPBTEST_ABOUT)
        self.Bind(wx.EVT_MENU, self.OnQuit, id=FPBTEST_QUIT)
        self.Bind(wx.EVT_MENU, self.OnToggleWindow, id=FPBTEST_TOGGLE_WINDOW)
        self.Bind(wx.EVT_MENU, self.OnCreateBottomStyle, id=FPB_BOTTOM_STICK)
        self.Bind(wx.EVT_MENU, self.OnCreateNormalStyle, id=FPB_SINGLE_FOLD)

        return menu_bar


# ----------------------------------------------------------------------------
# Collapsed Demo Implementation
# ----------------------------------------------------------------------------

class FoldTestPanel(wx.Panel):

    def __init__(self, parent, id, pos=wx.DefaultPosition, size=wx.DefaultSize,
                 style=wx.NO_BORDER | wx.TAB_TRAVERSAL):
        
        wx.Panel.__init__(self, parent, id, pos, size, style)

        self.CreateControls()
        self.GetSizer().Fit(self)
        self.GetSizer().SetSizeHints(self)
        self.GetSizer().Layout()

        self.Bind(fpb.EVT_CAPTIONBAR, self.OnPressCaption)


    def OnPressCaption(self, event):
        event.Skip()

    def CreateControls(self):

        sizer = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(sizer)
        
        subpanel = wx.Panel(self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize,
                            wx.NO_BORDER | wx.TAB_TRAVERSAL)
        sizer.Add(subpanel, 1, wx.GROW | wx.ADJUST_MINSIZE, 5)

        subsizer = wx.BoxSizer(wx.VERTICAL)
        subpanel.SetSizer(subsizer)
        itemstrings = ["One", "Two", "Three"]

        item5 = wx.Choice(subpanel, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize,
                          itemstrings, 0)
        
        subsizer.Add(item5, 0, wx.GROW | wx.ALL, 5)
        
        item6 = wx.TextCtrl(subpanel, wx.ID_ANY, "", wx.DefaultPosition, wx.DefaultSize,
                            wx.TE_MULTILINE)
        subsizer.Add(item6, 1, wx.GROW | wx.ALL, 5)

        item7 = wx.RadioButton(subpanel, wx.ID_ANY, "I Like This", wx.DefaultPosition,
                               wx.DefaultSize, 0)
        item7.SetValue(True)
        subsizer.Add(item7, 0, wx.ALIGN_LEFT | wx.ALL, 5)

        item8 = wx.RadioButton(subpanel, wx.ID_ANY, "I Hate It", wx.DefaultPosition,
                               wx.DefaultSize, 0)
        item8.SetValue(False)
        subsizer.Add(item8, 0, wx.ALIGN_LEFT | wx.ALL, 5)


# ----------------------------------------------------------------------------

class Collapsed(wx.Frame):

    def __init__(self, parent, id=wx.ID_ANY, title="", pos=wx.DefaultPosition,
                 size=(400,300), style=wx.DEFAULT_FRAME_STYLE):

        wx.Frame.__init__(self, parent, id, title, pos, size, style)
        
        self.SetIcon(GetMondrianIcon())
        self.SetMenuBar(self.CreateMenuBar())

        self.statusbar = self.CreateStatusBar(2, wx.ST_SIZEGRIP)
        self.statusbar.SetStatusWidths([-4, -3])
        self.statusbar.SetStatusText("Andrea Gavana @ 23 Mar 2005", 0)
        self.statusbar.SetStatusText("Welcome to wxPython!", 1)

        self.CreateFoldBar()
        

    def CreateFoldBar(self, vertical=True):
        
        if vertical:
            self.SetSize((500,600))
        else:
            self.SetSize((700,300))

        newstyle = (vertical and [fpb.FPB_VERTICAL] or [fpb.FPB_HORIZONTAL])[0]

        bar = fpb.FoldPanelBar(self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize,
                           fpb.FPB_DEFAULT_STYLE | newstyle, fpb.FPB_COLLAPSE_TO_BOTTOM)

        item = bar.AddFoldPanel("Test me", collapsed=False)
        button1 = wx.Button(item, wx.ID_ANY, "Collapse Me")
        button1.Bind(wx.EVT_BUTTON, self.OnCollapseMe)
        
        bar.AddFoldPanelWindow(item, button1, fpb.FPB_ALIGN_LEFT)

        item = bar.AddFoldPanel("Test me too!", collapsed=True)

        button2 = wx.Button(item, wx.ID_ANY, "Expand First One")
        button2.Bind(wx.EVT_BUTTON, self.OnExpandMe)
        
        bar.AddFoldPanelWindow(item, button2)
        bar.AddFoldPanelSeparator(item)

        newfoldpanel = FoldTestPanel(item, wx.ID_ANY)
        bar.AddFoldPanelWindow(item, newfoldpanel)

        bar.AddFoldPanelSeparator(item)

        bar.AddFoldPanelWindow(item, wx.TextCtrl(item, wx.ID_ANY, "Comment"),
                               fpb.FPB_ALIGN_LEFT, fpb.FPB_DEFAULT_SPACING, 20)

        item = bar.AddFoldPanel("Some Opinions ...", collapsed=False)
        bar.AddFoldPanelWindow(item, wx.CheckBox(item, wx.ID_ANY, "I Like This"))

        if vertical:
            # do not add this for horizontal for better presentation
            bar.AddFoldPanelWindow(item, wx.CheckBox(item, wx.ID_ANY, "And also this"))
            bar.AddFoldPanelWindow(item, wx.CheckBox(item, wx.ID_ANY, "And gimme this too"))

        bar.AddFoldPanelSeparator(item)

        bar.AddFoldPanelWindow(item, wx.CheckBox(item, wx.ID_ANY, "Check this too if you like"))

        if vertical:
            # do not add this for horizontal for better presentation
            bar.AddFoldPanelWindow(item, wx.CheckBox(item, wx.ID_ANY, "What about this"))

        item = bar.AddFoldPanel("Choose one ...", collapsed=False)
        bar.AddFoldPanelWindow(item, wx.StaticText(item, wx.ID_ANY, "Enter your comment"))
        bar.AddFoldPanelWindow(item, wx.TextCtrl(item, wx.ID_ANY, "Comment"),
                               fpb.FPB_ALIGN_WIDTH, fpb.FPB_DEFAULT_SPACING, 20)

        if hasattr(self, "pnl"):
            self.pnl.Destroy()

        self.pnl = bar

        size = self.GetClientSize()
        self.pnl.SetDimensions(0, 0, size.GetWidth(), size.GetHeight())


    def CreateMenuBar(self):

        FoldPanelBarTest_Quit = wx.NewId()
        FoldPanelBarTest_About = wx.NewId()
        FoldPanelBarTest_Horizontal = wx.NewId()
        FoldPanelBarTest_Vertical = wx.NewId()
        
        menuFile = wx.Menu()
        menuFile.Append(FoldPanelBarTest_Horizontal, "&Horizontal\tAlt-H")
        menuFile.Append(FoldPanelBarTest_Vertical, "&Vertical\tAlt-V")
        menuFile.AppendSeparator()
        menuFile.Append(FoldPanelBarTest_Quit, "E&xit\tAlt-X", "Quit This Program")

        helpMenu = wx.Menu()
        helpMenu.Append(FoldPanelBarTest_About, "&About...\tF1", "Show About Dialog")

        self.FoldPanelBarTest_Vertical = FoldPanelBarTest_Vertical
        self.FoldPanelBarTest_Horizontal = FoldPanelBarTest_Horizontal

        self.Bind(wx.EVT_MENU, self.OnQuit, id=FoldPanelBarTest_Quit)
        self.Bind(wx.EVT_MENU, self.OnAbout, id=FoldPanelBarTest_About)
        self.Bind(wx.EVT_MENU, self.OnOrientation, id=FoldPanelBarTest_Horizontal)
        self.Bind(wx.EVT_MENU, self.OnOrientation, id=FoldPanelBarTest_Vertical)

        value = wx.MenuBar()
        value.Append(menuFile, "&File")
        value.Append(helpMenu, "&Help")

        return value


    def OnOrientation(self, event):
        self.CreateFoldBar(event.GetId() == self.FoldPanelBarTest_Vertical)


    def OnQuit(self, event):
        # True is to force the frame to close
        self.Close(True)


    def OnAbout(self, event):

        msg = "This is the about dialog of the FoldPanelBarTest application.\n\n" + \
              "Welcome To wxPython " + wx.VERSION_STRING + "!!"
        dlg = wx.MessageDialog(self, msg, "About FoldPanelBarTest",
                               wx.OK | wx.ICON_INFORMATION)
        dlg.ShowModal()
        dlg.Destroy()


    def OnCollapseMe(self, event):

        item = self.pnl.GetFoldPanel(0)
        self.pnl.Collapse(item)

    def OnExpandMe(self, event):

        self.pnl.Expand(self.pnl.GetFoldPanel(0))
        self.pnl.Collapse(self.pnl.GetFoldPanel(1))


    
# ----------------------------------------------------------------------------
# NotCollapsed Implementation
# ----------------------------------------------------------------------------

class NotCollapsed(wx.Frame):
    def __init__(self, parent, id=wx.ID_ANY, title="", pos=wx.DefaultPosition,
                 size=(400,300), style=wx.DEFAULT_FRAME_STYLE):

        wx.Frame.__init__(self, parent, id, title, pos, size, style)
        
        self.SetIcon(GetMondrianIcon())
        self.SetMenuBar(self.CreateMenuBar())

        self.statusbar = self.CreateStatusBar(2, wx.ST_SIZEGRIP)
        self.statusbar.SetStatusWidths([-4, -3])
        self.statusbar.SetStatusText("Andrea Gavana @ 23 Mar 2005", 0)
        self.statusbar.SetStatusText("Welcome to wxPython!", 1)

        pnl = fpb.FoldPanelBar(self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize,
                           fpb.FPB_DEFAULT_STYLE | fpb.FPB_VERTICAL)

        item = pnl.AddFoldPanel("Test Me", collapsed=False)
        
        button1 = wx.Button(item, wx.ID_ANY, "Collapse Me")
        
        pnl.AddFoldPanelWindow(item, button1, fpb.FPB_ALIGN_LEFT)
        pnl.AddFoldPanelSeparator(item)
        
        button1.Bind(wx.EVT_BUTTON, self.OnCollapseMe)

        item = pnl.AddFoldPanel("Test Me Too!", collapsed=True)
        button2 = wx.Button(item, wx.ID_ANY, "Expand First One")
        pnl.AddFoldPanelWindow(item, button2, fpb.FPB_ALIGN_LEFT)
        pnl.AddFoldPanelSeparator(item)

        button2.Bind(wx.EVT_BUTTON, self.OnExpandMe)
        
        newfoldpanel = FoldTestPanel(item, wx.ID_ANY)
        pnl.AddFoldPanelWindow(item, newfoldpanel)

        pnl.AddFoldPanelSeparator(item)

        pnl.AddFoldPanelWindow(item, wx.TextCtrl(item, wx.ID_ANY, "Comment"),
                               fpb.FPB_ALIGN_LEFT, fpb.FPB_DEFAULT_SPACING, 20)

        item = pnl.AddFoldPanel("Some Opinions ...", collapsed=False)
        pnl.AddFoldPanelWindow(item, wx.CheckBox(item, wx.ID_ANY, "I Like This"))
        pnl.AddFoldPanelWindow(item, wx.CheckBox(item, wx.ID_ANY, "And Also This"))
        pnl.AddFoldPanelWindow(item, wx.CheckBox(item, wx.ID_ANY, "And Gimme This Too"))

        pnl.AddFoldPanelSeparator(item)

        pnl.AddFoldPanelWindow(item, wx.CheckBox(item, wx.ID_ANY, "Check This Too If You Like"))
        pnl.AddFoldPanelWindow(item, wx.CheckBox(item, wx.ID_ANY, "What About This"))

        item = pnl.AddFoldPanel("Choose One ...", collapsed=False)
        pnl.AddFoldPanelWindow(item, wx.StaticText(item, wx.ID_ANY, "Enter Your Comment"))
        pnl.AddFoldPanelWindow(item, wx.TextCtrl(item, wx.ID_ANY, "Comment"),
                               fpb.FPB_ALIGN_WIDTH, fpb.FPB_DEFAULT_SPACING, 20, 20)
        self.pnl = pnl


    def CreateMenuBar(self):

        FoldPanelBarTest_Quit = wx.NewId()
        FoldPanelBarTest_About = wx.NewId()
        
        menuFile = wx.Menu()
        menuFile.Append(FoldPanelBarTest_Quit, "E&xit\tAlt-X", "Quit This Program")

        helpMenu = wx.Menu()
        helpMenu.Append(FoldPanelBarTest_About, "&About...\tF1", "Show About Dialog")

        self.Bind(wx.EVT_MENU, self.OnQuit, id=FoldPanelBarTest_Quit)
        self.Bind(wx.EVT_MENU, self.OnAbout, id=FoldPanelBarTest_About)

        value = wx.MenuBar()
        value.Append(menuFile, "&File")
        value.Append(helpMenu, "&Help")

        return value


    # Event Handlers

    def OnQuit(self, event):

        # True is to force the frame to close
        self.Close(True)


    def OnAbout(self, event):

        msg = "This is the about dialog of the FoldPanelBarTest application.\n\n" + \
              "Welcome To wxPython " + wx.VERSION_STRING + "!!"
        dlg = wx.MessageDialog(self, msg, "About FoldPanelBarTest",
                               wx.OK | wx.ICON_INFORMATION)
        dlg.ShowModal()
        dlg.Destroy()


    def OnCollapseMe(self, event):

        item = self.pnl.GetFoldPanel(0)
        self.pnl.Collapse(item)

    def OnExpandMe(self, event):

        self.pnl.Expand(self.pnl.GetFoldPanel(0))
        self.pnl.Collapse(self.pnl.GetFoldPanel(1))


#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        vsizer = wx.BoxSizer(wx.VERTICAL)
        b = wx.Button(self, -1, "FoldPanelBar Extended Demo")
        vsizer.Add(b, 0, wx.ALL, 5)
        self.Bind(wx.EVT_BUTTON, self.OnButton1, b)

        b = wx.Button(self, -1, "FoldPanelBar Collapsed Demo")
        vsizer.Add(b, 0, wx.ALL, 5)
        self.Bind(wx.EVT_BUTTON, self.OnButton2, b)

        b = wx.Button(self, -1, "FoldPanelBar NotCollapsed Demo")
        vsizer.Add(b, 0, wx.ALL, 5)
        self.Bind(wx.EVT_BUTTON, self.OnButton3, b)

        bdr = wx.BoxSizer()
        bdr.Add(vsizer, 0, wx.ALL, 50)
        self.SetSizer(bdr)

    def OnButton1(self, evt):
        frame = Extended(self, title="FoldPanelBar Extended Demo")
        frame.Show()

    def OnButton2(self, evt):
        frame = Collapsed(self, title="FoldPanelBar Collapsed Demo")
        frame.Show()

    def OnButton3(self, evt):
        frame = NotCollapsed(self, title="FoldPanelBar NotCollapsed Demo")
        frame.Show()
    

#---------------------------------------------------------------------------


def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win


#---------------------------------------------------------------------------


overview = fpb.__doc__


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

