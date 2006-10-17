import wx
import wx.lib.buttonpanel as bp
import images

import random

#----------------------------------------------------------------------

ID_BackgroundColour = wx.NewId()
ID_GradientFrom = wx.NewId()
ID_GradientTo = wx.NewId()
ID_BorderColour = wx.NewId()
ID_CaptionColour = wx.NewId()
ID_ButtonTextColour = wx.NewId()
ID_SelectionBrush = wx.NewId()
ID_SelectionPen = wx.NewId()
ID_SeparatorColour = wx.NewId()


#----------------------------------------------------------------------
# Some icons for the demo
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

#----------------------------------------------------------------------

class SettingsPanel(wx.MiniFrame):

    def __init__(self, parent, id=wx.ID_ANY, title="Settings Panel", pos=wx.DefaultPosition,
                 size=wx.DefaultSize,
                 style=wx.SYSTEM_MENU | wx.CAPTION | wx.CLOSE_BOX | wx.FRAME_NO_TASKBAR
                 | wx.FRAME_FLOAT_ON_PARENT | wx.CLIP_CHILDREN):

        wx.MiniFrame.__init__(self, parent, id, title, pos, size, style)

        self.targetTitleBar = parent.titleBar
        self.parent = parent
        self.panel = wx.Panel(self, -1)
        
        self.coloursizer_staticbox = wx.StaticBox(self.panel, -1, "Colour Options")
        self.bottomsizer_staticbox = wx.StaticBox(self.panel, -1, "Size Options")
        self.stylesizer_staticbox = wx.StaticBox(self.panel, -1, "ButtonPanel Styles")
        self.defaultstyle = wx.RadioButton(self.panel, -1, "Default Style", style=wx.RB_GROUP)
        self.gradientstyle = wx.RadioButton(self.panel, -1, "Gradient Style")
        self.verticalgradient = wx.RadioButton(self.panel, -1, "Vertical Gradient", style=wx.RB_GROUP)
        self.horizontalgradient = wx.RadioButton(self.panel, -1, "Horizontal Gradient")

        b = self.CreateColorBitmap(wx.BLACK)
        
        self.bakbrush = wx.BitmapButton(self.panel, ID_BackgroundColour, b, size=wx.Size(50,25))
        self.gradientfrom = wx.BitmapButton(self.panel, ID_GradientFrom, b, size=wx.Size(50,25))
        self.gradientto = wx.BitmapButton(self.panel, ID_GradientTo, b, size=wx.Size(50,25))
        self.bordercolour = wx.BitmapButton(self.panel, ID_BorderColour, b, size=wx.Size(50,25))
        self.captioncolour = wx.BitmapButton(self.panel, ID_CaptionColour, b, size=wx.Size(50,25))
        self.textbuttoncolour = wx.BitmapButton(self.panel, ID_ButtonTextColour, b, size=wx.Size(50,25))
        self.selectionbrush = wx.BitmapButton(self.panel, ID_SelectionBrush, b, size=wx.Size(50,25))
        self.selectionpen = wx.BitmapButton(self.panel, ID_SelectionPen, b, size=wx.Size(50,25))
        self.separatorcolour = wx.BitmapButton(self.panel, ID_SeparatorColour, b, size=wx.Size(50,25))

        self.separatorspin = wx.SpinCtrl(self.panel, -1, "7", min=3, max=20,
                                         style=wx.SP_ARROW_KEYS)
        self.marginspin = wx.SpinCtrl(self.panel, -1, "6", min=3, max=20,
                                      style=wx.SP_ARROW_KEYS)
        self.paddingspin = wx.SpinCtrl(self.panel, -1, "6", min=3, max=20,
                                       style=wx.SP_ARROW_KEYS)
        self.borderspin = wx.SpinCtrl(self.panel, -1, "3", min=3, max=7,
                                      style=wx.SP_ARROW_KEYS)

        self.__set_properties()
        self.__do_layout()

        self.Bind(wx.EVT_RADIOBUTTON, self.OnDefaultStyle, self.defaultstyle)
        self.Bind(wx.EVT_RADIOBUTTON, self.OnGradientStyle, self.gradientstyle)
        self.Bind(wx.EVT_RADIOBUTTON, self.OnVerticalGradient, self.verticalgradient)
        self.Bind(wx.EVT_RADIOBUTTON, self.OnHorizontalGradient, self.horizontalgradient)
        self.Bind(wx.EVT_BUTTON, self.OnSetColour, id=ID_BackgroundColour)
        self.Bind(wx.EVT_BUTTON, self.OnSetColour, id=ID_GradientFrom)
        self.Bind(wx.EVT_BUTTON, self.OnSetColour, id=ID_GradientTo)
        self.Bind(wx.EVT_BUTTON, self.OnSetColour, id=ID_BorderColour)
        self.Bind(wx.EVT_BUTTON, self.OnSetColour, id=ID_CaptionColour)
        self.Bind(wx.EVT_BUTTON, self.OnSetColour, id=ID_ButtonTextColour)
        self.Bind(wx.EVT_BUTTON, self.OnSetColour, id=ID_SelectionBrush)
        self.Bind(wx.EVT_BUTTON, self.OnSetColour, id=ID_SelectionPen)
        self.Bind(wx.EVT_BUTTON, self.OnSetColour, id=ID_SeparatorColour)
        
        self.Bind(wx.EVT_SPINCTRL, self.OnSeparator, self.separatorspin)
        self.Bind(wx.EVT_SPINCTRL, self.OnMargins, self.marginspin)
        self.Bind(wx.EVT_SPINCTRL, self.OnPadding, self.paddingspin)
        self.Bind(wx.EVT_SPINCTRL, self.OnBorder, self.borderspin)

        self.Bind(wx.EVT_CLOSE, self.OnClose)        


    def __set_properties(self):

        self.defaultstyle.SetFont(wx.Font(8, wx.DEFAULT, wx.NORMAL, wx.BOLD, 0, ""))
        self.defaultstyle.SetValue(1)
        self.gradientstyle.SetFont(wx.Font(8, wx.DEFAULT, wx.NORMAL, wx.BOLD, 0, ""))
        self.verticalgradient.SetValue(1)

        if self.targetTitleBar.GetStyle() & bp.BP_DEFAULT_STYLE:
            self.verticalgradient.Enable(False)
            self.horizontalgradient.Enable(False)
            self.defaultstyle.SetValue(1)
        else:
            self.gradientstyle.SetValue(1)

        self.borderspin.SetValue(self.targetTitleBar.GetBPArt().GetMetric(bp.BP_BORDER_SIZE))
        self.separatorspin.SetValue(self.targetTitleBar.GetBPArt().GetMetric(bp.BP_SEPARATOR_SIZE))
        self.marginspin.SetValue(self.targetTitleBar.GetBPArt().GetMetric(bp.BP_MARGINS_SIZE).x)
        self.paddingspin.SetValue(self.targetTitleBar.GetBPArt().GetMetric(bp.BP_PADDING_SIZE).x)

        self.UpdateColors()        
        

    def __do_layout(self):

        mainsizer = wx.BoxSizer(wx.VERTICAL)
        buttonsizer = wx.BoxSizer(wx.HORIZONTAL)
        bottomsizer = wx.StaticBoxSizer(self.bottomsizer_staticbox, wx.VERTICAL)
        sizer_13 = wx.BoxSizer(wx.HORIZONTAL)
        sizer_12 = wx.BoxSizer(wx.HORIZONTAL)
        sizer_11 = wx.BoxSizer(wx.HORIZONTAL)
        sizer_10 = wx.BoxSizer(wx.HORIZONTAL)
        coloursizer = wx.StaticBoxSizer(self.coloursizer_staticbox, wx.HORIZONTAL)
        rightsizer = wx.BoxSizer(wx.VERTICAL)
        sizer_9 = wx.BoxSizer(wx.HORIZONTAL)
        sizer_8 = wx.BoxSizer(wx.HORIZONTAL)
        sizer_7 = wx.BoxSizer(wx.HORIZONTAL)
        sizer_6 = wx.BoxSizer(wx.HORIZONTAL)
        leftsizer = wx.BoxSizer(wx.VERTICAL)
        sizer_5 = wx.BoxSizer(wx.HORIZONTAL)
        sizer_4 = wx.BoxSizer(wx.HORIZONTAL)
        sizer_3 = wx.BoxSizer(wx.HORIZONTAL)
        sizer_2 = wx.BoxSizer(wx.HORIZONTAL)
        sizer_1 = wx.BoxSizer(wx.HORIZONTAL)
        stylesizer = wx.StaticBoxSizer(self.stylesizer_staticbox, wx.VERTICAL)
        tophsizer = wx.BoxSizer(wx.HORIZONTAL)
        tophsizer2 = wx.BoxSizer(wx.VERTICAL)
        
        stylesizer.Add(self.defaultstyle, 0, wx.ALL|wx.EXPAND|wx.ADJUST_MINSIZE, 5)

        tophsizer.Add(self.gradientstyle, 0, wx.LEFT|wx.RIGHT|wx.EXPAND|
                      wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)

        tophsizer2.Add(self.verticalgradient, 0, wx.BOTTOM|wx.ADJUST_MINSIZE, 3)
        tophsizer2.Add(self.horizontalgradient, 0, wx.ADJUST_MINSIZE, 0)
        
        tophsizer.Add(tophsizer2, 1, wx.LEFT|wx.EXPAND|wx.ALIGN_CENTER_VERTICAL, 10)

        stylesizer.Add(tophsizer, 1, wx.EXPAND, 0)

        mainsizer.Add(stylesizer, 0, wx.ALL|wx.EXPAND, 5)

        label_1 = wx.StaticText(self.panel, -1, "Background Brush Colour:")
        sizer_1.Add(label_1, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        sizer_1.Add((0, 0), 1, wx.EXPAND|wx.ADJUST_MINSIZE, 0)
        sizer_1.Add(self.bakbrush, 0, wx.ADJUST_MINSIZE, 0)

        leftsizer.Add(sizer_1, 1, wx.EXPAND, 0)

        label_2 = wx.StaticText(self.panel, -1, "Gradient Colour From:")
        sizer_2.Add(label_2, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        sizer_2.Add((0, 0), 1, wx.EXPAND|wx.ADJUST_MINSIZE, 0)
        sizer_2.Add(self.gradientfrom, 0, wx.ADJUST_MINSIZE, 0)

        leftsizer.Add(sizer_2, 1, wx.EXPAND, 0)

        label_3 = wx.StaticText(self.panel, -1, "Gradient Colour To:")
        sizer_3.Add(label_3, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        sizer_3.Add((0, 0), 1, wx.EXPAND|wx.ADJUST_MINSIZE, 0)
        sizer_3.Add(self.gradientto, 0, wx.ADJUST_MINSIZE, 0)

        leftsizer.Add(sizer_3, 1, wx.EXPAND, 0)

        label_4 = wx.StaticText(self.panel, -1, "Border Colour:")
        sizer_4.Add(label_4, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        sizer_4.Add((0, 0), 1, wx.EXPAND|wx.ADJUST_MINSIZE, 0)
        sizer_4.Add(self.bordercolour, 0, wx.ADJUST_MINSIZE, 0)

        leftsizer.Add(sizer_4, 1, wx.EXPAND, 0)

        label_5 = wx.StaticText(self.panel, -1, "Main Caption Colour:")
        sizer_5.Add(label_5, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        sizer_5.Add((0, 0), 1, wx.EXPAND|wx.ADJUST_MINSIZE, 0)
        sizer_5.Add(self.captioncolour, 0, wx.ADJUST_MINSIZE, 0)

        leftsizer.Add(sizer_5, 1, wx.EXPAND, 0)

        coloursizer.Add(leftsizer, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5)
        coloursizer.Add((20, 20), 1, wx.EXPAND|wx.ADJUST_MINSIZE, 0)
        
        label_6 = wx.StaticText(self.panel, -1, "Text Button Colour:")
        sizer_6.Add(label_6, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        sizer_6.Add((0, 0), 1, wx.EXPAND|wx.ADJUST_MINSIZE, 0)
        sizer_6.Add(self.textbuttoncolour, 0, wx.ADJUST_MINSIZE, 0)

        rightsizer.Add(sizer_6, 1, wx.EXPAND, 0)

        label_7 = wx.StaticText(self.panel, -1, "Selection Brush Colour:")
        sizer_7.Add(label_7, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        sizer_7.Add((0, 0), 1, wx.EXPAND|wx.ADJUST_MINSIZE, 0)
        sizer_7.Add(self.selectionbrush, 0, wx.ADJUST_MINSIZE, 0)

        rightsizer.Add(sizer_7, 1, wx.EXPAND, 0)

        label_8 = wx.StaticText(self.panel, -1, "Selection Pen Colour:")
        sizer_8.Add(label_8, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        sizer_8.Add((0, 0), 1, wx.EXPAND|wx.ADJUST_MINSIZE, 0)
        sizer_8.Add(self.selectionpen, 0, wx.ADJUST_MINSIZE, 0)

        rightsizer.Add(sizer_8, 1, wx.EXPAND, 0)

        label_9 = wx.StaticText(self.panel, -1, "Separator Colour:")
        sizer_9.Add(label_9, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        sizer_9.Add((0, 0), 1, wx.EXPAND|wx.ADJUST_MINSIZE, 0)
        sizer_9.Add(self.separatorcolour, 0, wx.ADJUST_MINSIZE, 0)

        rightsizer.Add(sizer_9, 1, wx.EXPAND, 0)

        coloursizer.Add(rightsizer, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5)

        mainsizer.Add(coloursizer, 0, wx.ALL|wx.EXPAND, 5)

        label_10 = wx.StaticText(self.panel, -1, "Separator Size:")
        sizer_10.Add(label_10, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        sizer_10.Add((0, 0), 1, wx.EXPAND|wx.ADJUST_MINSIZE, 0)
        sizer_10.Add(self.separatorspin, 0, wx.ALL|wx.ADJUST_MINSIZE, 5)

        bottomsizer.Add(sizer_10, 1, wx.EXPAND, 0)

        label_11 = wx.StaticText(self.panel, -1, "Margins Size:")
        sizer_11.Add(label_11, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        sizer_11.Add((0, 0), 1, wx.EXPAND|wx.ADJUST_MINSIZE, 0)
        sizer_11.Add(self.marginspin, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ADJUST_MINSIZE, 5)

        bottomsizer.Add(sizer_11, 1, wx.EXPAND, 0)

        label_12 = wx.StaticText(self.panel, -1, "Padding Size:")
        sizer_12.Add(label_12, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        sizer_12.Add((0, 0), 1, wx.EXPAND|wx.ADJUST_MINSIZE, 0)
        sizer_12.Add(self.paddingspin, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ADJUST_MINSIZE, 5)

        bottomsizer.Add(sizer_12, 1, wx.EXPAND, 0)

        label_13 = wx.StaticText(self.panel, -1, "Border Size:")
        sizer_13.Add(label_13, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        sizer_13.Add((0, 0), 1, wx.EXPAND|wx.ADJUST_MINSIZE, 0)
        sizer_13.Add(self.borderspin, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ADJUST_MINSIZE, 5)

        bottomsizer.Add(sizer_13, 1, wx.EXPAND, 0)

        mainsizer.Add(bottomsizer, 0, wx.ALL|wx.EXPAND, 5)

        self.panel.SetSizer(mainsizer)
        sizer = wx.BoxSizer()
        sizer.Add(self.panel, 1, wx.EXPAND)
        self.SetSizer(sizer)
        self.Fit()


    def CreateColorBitmap(self, c):
    
        image = wx.EmptyImage(25, 14)
        
        for x in xrange(25):
            for y in xrange(14):
                pixcol = c
                if x == 0 or x == 24 or y == 0 or y == 13:
                    pixcol = wx.BLACK
                    
                image.SetRGB(x, y, pixcol.Red(), pixcol.Green(), pixcol.Blue())
            
        return image.ConvertToBitmap()


    def UpdateColors(self):
    
        bk = self.targetTitleBar.GetBPArt().GetColor(bp.BP_BACKGROUND_COLOR)
        self.bakbrush.SetBitmapLabel(self.CreateColorBitmap(bk))
        
        capfrom = self.targetTitleBar.GetBPArt().GetColor(bp.BP_GRADIENT_COLOR_FROM)
        self.gradientfrom.SetBitmapLabel(self.CreateColorBitmap(capfrom))

        capto = self.targetTitleBar.GetBPArt().GetColor(bp.BP_GRADIENT_COLOR_TO)
        self.gradientto.SetBitmapLabel(self.CreateColorBitmap(capto))

        captxt = self.targetTitleBar.GetBPArt().GetColor(bp.BP_TEXT_COLOR)
        self.captioncolour.SetBitmapLabel(self.CreateColorBitmap(captxt))

        bor = self.targetTitleBar.GetBPArt().GetColor(bp.BP_BORDER_COLOR)
        self.bordercolour.SetBitmapLabel(self.CreateColorBitmap(bor))
        
        btntext = self.targetTitleBar.GetBPArt().GetColor(bp.BP_BUTTONTEXT_COLOR)
        self.textbuttoncolour.SetBitmapLabel(self.CreateColorBitmap(btntext))

        selb = self.targetTitleBar.GetBPArt().GetColor(bp.BP_SELECTION_BRUSH_COLOR)
        self.selectionbrush.SetBitmapLabel(self.CreateColorBitmap(selb))

        selp = self.targetTitleBar.GetBPArt().GetColor(bp.BP_SELECTION_PEN_COLOR)
        self.selectionpen.SetBitmapLabel(self.CreateColorBitmap(selp))
        
        sepc = self.targetTitleBar.GetBPArt().GetColor(bp.BP_SEPARATOR_COLOR)
        self.separatorcolour.SetBitmapLabel(self.CreateColorBitmap(sepc))


    def OnDefaultStyle(self, event):

        self.verticalgradient.Enable(False)
        self.horizontalgradient.Enable(False)
        self.targetTitleBar.SetStyle(bp.BP_DEFAULT_STYLE)

        self.targetTitleBar.Refresh()

        event.Skip()
        

    def OnGradientStyle(self, event): 

        self.verticalgradient.Enable(True)
        self.horizontalgradient.Enable(True)
        self.targetTitleBar.SetStyle(bp.BP_USE_GRADIENT)
        self.targetTitleBar.Refresh()
        
        event.Skip()


    def OnVerticalGradient(self, event):

        self.targetTitleBar.GetBPArt().SetGradientType(bp.BP_GRADIENT_VERTICAL)
        self.targetTitleBar.SetStyle(bp.BP_USE_GRADIENT)
        self.targetTitleBar.Refresh()
        
        event.Skip()
        

    def OnHorizontalGradient(self, event):

        self.targetTitleBar.GetBPArt().SetGradientType(bp.BP_GRADIENT_HORIZONTAL)
        self.targetTitleBar.SetStyle(bp.BP_USE_GRADIENT)
        self.targetTitleBar.Refresh()
        
        event.Skip()
        

    def OnSetColour(self, event):

        dlg = wx.ColourDialog(self.parent)
        
        dlg.SetTitle("Color Picker")
        
        if dlg.ShowModal() != wx.ID_OK:
            return
        
        var = 0
        if event.GetId() == ID_BackgroundColour:
            var = bp.BP_BACKGROUND_COLOR
        elif event.GetId() == ID_GradientFrom:
            var = bp.BP_GRADIENT_COLOR_FROM
        elif event.GetId() == ID_GradientTo:
            var = bp.BP_GRADIENT_COLOR_TO
        elif event.GetId() == ID_BorderColour:
            var = bp.BP_BORDER_COLOR
        elif event.GetId() == ID_CaptionColour:
            var = bp.BP_TEXT_COLOR
        elif event.GetId() == ID_ButtonTextColour:
            var = bp.BP_BUTTONTEXT_COLOR
        elif event.GetId() == ID_SelectionBrush:
            var = bp.BP_SELECTION_BRUSH_COLOR
        elif event.GetId() == ID_SelectionPen:
            var = bp.BP_SELECTION_PEN_COLOR
        elif event.GetId() == ID_SeparatorColour:
            var = bp.BP_SEPARATOR_COLOR
        else:
            return        
        
        self.targetTitleBar.GetBPArt().SetColor(var, dlg.GetColourData().GetColour())
        self.targetTitleBar.Refresh()
        self.UpdateColors()

        self.parent.useredited = True


    def OnSeparator(self, event):

        self.targetTitleBar.GetBPArt().SetMetric(bp.BP_SEPARATOR_SIZE,
                                                 event.GetInt())

        self.targetTitleBar.DoLayout()
        self.parent.mainPanel.Layout()
        self.parent.useredited = True

        event.Skip()


    def OnMargins(self, event):

        self.targetTitleBar.GetBPArt().SetMetric(bp.BP_MARGINS_SIZE,
                                                 wx.Size(event.GetInt(), event.GetInt()))

        self.targetTitleBar.DoLayout()
        self.parent.mainPanel.Layout()

        self.parent.useredited = True
        
        event.Skip()


    def OnPadding(self, event):

        self.targetTitleBar.GetBPArt().SetMetric(bp.BP_PADDING_SIZE,
                                                 wx.Size(event.GetInt(), event.GetInt()))

        self.targetTitleBar.DoLayout()
        self.parent.mainPanel.Layout()
        self.parent.useredited = True
        
        event.Skip()


    def OnBorder(self, event):

        self.targetTitleBar.GetBPArt().SetMetric(bp.BP_BORDER_SIZE,
                                                 event.GetInt())

        self.targetTitleBar.DoLayout()
        self.parent.mainPanel.Layout()

        self.parent.useredited = True
        
        event.Skip()


    def OnClose(self, event):

        self.parent.hassettingpanel = False
        self.Destroy()



#----------------------------------------------------------------------

class ButtonPanelDemo(wx.Frame):

    def __init__(self, parent, id=wx.ID_ANY, title="ButtonPanel wxPython Demo ;-)",
                 pos=wx.DefaultPosition, size=(640, 400), style=wx.DEFAULT_FRAME_STYLE):
        
        wx.Frame.__init__(self, parent, id, title, pos, size, style)

        self.useredited = False
        self.hassettingpanel = False

        self.SetIcon(GetMondrianIcon())
        self.CreateMenuBar()

        self.statusbar = self.CreateStatusBar(2, wx.ST_SIZEGRIP)
        self.statusbar.SetStatusWidths([-2, -1])
        # statusbar fields
        statusbar_fields = [("ButtonPanel wxPython Demo, Andrea Gavana @ 02 Oct 2006"),
                            ("Welcome To wxPython!")]

        for i in range(len(statusbar_fields)):
            self.statusbar.SetStatusText(statusbar_fields[i], i)
        
        self.mainPanel = wx.Panel(self, -1)
        self.logtext = wx.TextCtrl(self.mainPanel, -1, "", style=wx.TE_MULTILINE|wx.TE_READONLY)
        
        vSizer = wx.BoxSizer(wx.VERTICAL) 
        self.mainPanel.SetSizer(vSizer) 

        self.alignments = [bp.BP_ALIGN_LEFT, bp.BP_ALIGN_RIGHT, bp.BP_ALIGN_TOP, bp.BP_ALIGN_BOTTOM]
        
        self.alignment = bp.BP_ALIGN_LEFT
        self.style = bp.BP_USE_GRADIENT
        
        self.titleBar = bp.ButtonPanel(self.mainPanel, -1, "A Simple Test & Demo",
                                       style=self.style, alignment=self.alignment)

        self.created = False
        self.pngs = [ (images.get_bp_btn1Bitmap(), 'label1'),
                      (images.get_bp_btn2Bitmap(), 'label2'),
                      (images.get_bp_btn3Bitmap(), 'label3'),
                      (images.get_bp_btn4Bitmap(), 'label4'),
                      ]
        self.CreateButtons()
        self.SetProperties()
                
        
    def CreateMenuBar(self):

        mb = wx.MenuBar()
        
        file_menu = wx.Menu()
        
        item = wx.MenuItem(file_menu, wx.ID_ANY, "&Quit")
        file_menu.AppendItem(item)
        self.Bind(wx.EVT_MENU, self.OnClose, item)

        edit_menu = wx.Menu()

        item = wx.MenuItem(edit_menu, wx.ID_ANY, "Set Bar Text")
        edit_menu.AppendItem(item)
        self.Bind(wx.EVT_MENU, self.OnSetBarText, item)

        edit_menu.AppendSeparator()        

        submenu = wx.Menu()
        
        item = wx.MenuItem(submenu, wx.ID_ANY, "BP_ALIGN_LEFT", kind=wx.ITEM_RADIO)
        submenu.AppendItem(item)
        item.Check(True)
        self.Bind(wx.EVT_MENU, self.OnAlignment, item)
        
        item = wx.MenuItem(submenu, wx.ID_ANY, "BP_ALIGN_RIGHT", kind=wx.ITEM_RADIO)
        submenu.AppendItem(item)
        self.Bind(wx.EVT_MENU, self.OnAlignment, item)
        
        item = wx.MenuItem(submenu, wx.ID_ANY, "BP_ALIGN_TOP", kind=wx.ITEM_RADIO)
        submenu.AppendItem(item)
        self.Bind(wx.EVT_MENU, self.OnAlignment, item)
        
        item = wx.MenuItem(submenu, wx.ID_ANY, "BP_ALIGN_BOTTOM", kind=wx.ITEM_RADIO)
        submenu.AppendItem(item)
        self.Bind(wx.EVT_MENU, self.OnAlignment, item)

        edit_menu.AppendMenu(wx.ID_ANY, "&Alignment", submenu)
                
        submenu = wx.Menu()

        item = wx.MenuItem(submenu, wx.ID_ANY, "Default Style", kind=wx.ITEM_RADIO)
        submenu.AppendItem(item)
        self.Bind(wx.EVT_MENU, self.OnDefaultStyle, item)
        
        item = wx.MenuItem(submenu, wx.ID_ANY, "Gradient Style", kind=wx.ITEM_RADIO)
        submenu.AppendItem(item)
        item.Check(True)
        self.Bind(wx.EVT_MENU, self.OnGradientStyle, item)
        
        edit_menu.AppendMenu(wx.ID_ANY, "&Styles", submenu)

        edit_menu.AppendSeparator()
        
        item = wx.MenuItem(submenu, wx.ID_ANY, "Settings Panel")
        edit_menu.AppendItem(item)
        self.Bind(wx.EVT_MENU, self.OnSettingsPanel, item)

        demo_menu = wx.Menu()
        
        item = wx.MenuItem(demo_menu, wx.ID_ANY, "Default Demo", kind=wx.ITEM_RADIO)
        demo_menu.AppendItem(item)
        self.Bind(wx.EVT_MENU, self.OnDefaultDemo, item)

        item = wx.MenuItem(demo_menu, wx.ID_ANY, "Button Only Demo", kind=wx.ITEM_RADIO)
        demo_menu.AppendItem(item)
        self.Bind(wx.EVT_MENU, self.OnButtonOnly, item)
        
        help_menu = wx.Menu()

        item = wx.MenuItem(help_menu, wx.ID_ANY, "&About...")
        help_menu.AppendItem(item)
        self.Bind(wx.EVT_MENU, self.OnAbout, item)
      
        mb.Append(file_menu, "&File")
        mb.Append(edit_menu, "&Edit")
        mb.Append(demo_menu, "&Demo")
        mb.Append(help_menu, "&Help")
        
        self.SetMenuBar(mb)


    def CreateButtons(self):

        # Here we (re)create the buttons for the default startup demo
        self.Freeze()

        if self.created:
            sizer = self.mainPanel.GetSizer()
            sizer.Detach(0)
            self.titleBar.Hide()
            wx.CallAfter(self.titleBar.Destroy)
            self.titleBar = bp.ButtonPanel(self.mainPanel, -1, "A Simple Test & Demo",
                                           style=self.style, alignment=self.alignment)
            self.SetProperties()
                    
        self.indices = []
        for count, png in enumerate(self.pngs):

            if count < 2:
                # First 2 buttons are togglebuttons
                kind = wx.ITEM_CHECK
            else:
                kind = wx.ITEM_NORMAL
                
            btn = bp.ButtonInfo(self.titleBar, wx.NewId(),
                                png[0],
                                kind=kind)
            
            self.titleBar.AddButton(btn)
            self.Bind(wx.EVT_BUTTON, self.OnButton, id=btn.GetId())
            
            self.indices.append(btn.GetId())
            
            if count < 2:
                # First 2 buttons have also a text
                btn.SetText(png[1])

            if count == 2:
                # Append a separator after the second button
                self.titleBar.AddSeparator()
            
            if count == 1:
                # Add a wx.TextCtrl to ButtonPanel
                self.titleBar.AddControl(wx.TextCtrl(self.titleBar, -1, "Hello wxPython!"))
                btn.SetTextAlignment(bp.BP_BUTTONTEXT_ALIGN_RIGHT)

        # Add a wx.Choice to ButtonPanel                        
        self.titleBar.AddControl(wx.Choice(self.titleBar, -1,
                                           choices=["Hello", "From", "wxPython!"]))
        
        self.strings = ["First", "Second", "Third", "Fourth"]

        self.ChangeLayout()              
        self.Thaw()
        self.titleBar.DoLayout()

        self.created = True
        

    def ButtonOnly(self):
        
        # Here we (re)create the buttons for the button-only demo
        self.Freeze()
        
        if self.created:
            sizer = self.mainPanel.GetSizer()
            sizer.Detach(0)
            self.titleBar.Destroy()
            self.titleBar = bp.ButtonPanel(self.mainPanel, -1, "A Simple Test & Demo",
                                           style=self.style, alignment=self.alignment)
            self.SetProperties()

        # Buttons are created completely random, with random images, toggle behavior
        # and text
        
        self.indices = []
        for count in xrange(8):

            itemImage = random.randint(0, 3)
            hasText = random.randint(0, 1)
            itemKind = random.randint(0, 1)
                            
            btn = bp.ButtonInfo(self.titleBar, wx.NewId(), self.pngs[itemImage][0],
                                kind=itemKind)

            if hasText:
                btn.SetText(self.pngs[itemImage][1])
                rightText = random.randint(0, 1)
                if rightText:
                    btn.SetTextAlignment(bp.BP_BUTTONTEXT_ALIGN_RIGHT)

            self.titleBar.AddButton(btn)
            self.Bind(wx.EVT_BUTTON, self.OnButton, id=btn.GetId())
            
            self.indices.append(btn.GetId())

            if count in [0, 3, 5]:
                self.titleBar.AddSeparator()
                    
        self.strings = ["First", "Second", "Third", "Fourth", "Fifth", "Sixth", "Seventh", "Eighth"]

        self.ChangeLayout()              
        self.Thaw()
        self.titleBar.DoLayout()
        

    def ChangeLayout(self):
        
        # Change the layout after a switch in ButtonPanel alignment
        self.Freeze()
        
        if self.alignment in [bp.BP_ALIGN_LEFT, bp.BP_ALIGN_RIGHT]:
            vSizer = wx.BoxSizer(wx.VERTICAL)
        else:
            vSizer = wx.BoxSizer(wx.HORIZONTAL)
            
        self.mainPanel.SetSizer(vSizer) 

        vSizer.Add(self.titleBar, 0, wx.EXPAND)
        vSizer.Add((20, 20))
        vSizer.Add(self.logtext, 1, wx.EXPAND|wx.ALL, 5)

        vSizer.Layout()
        self.mainPanel.Layout()
        self.Thaw()
                

    def SetProperties(self):

        # No resetting if the user is using the Settings Panel
        if self.useredited:
            return
        
        # Sets the colours for the two demos: called only if the user didn't
        # modify the colours and sizes using the Settings Panel
        bpArt = self.titleBar.GetBPArt()
        
        if self.style & bp.BP_USE_GRADIENT:
            # set the color the text is drawn with
            bpArt.SetColor(bp.BP_TEXT_COLOR, wx.WHITE)

            # These default to white and whatever is set in the system
            # settings for the wx.SYS_COLOUR_ACTIVECAPTION.  We'll use
            # some specific settings to ensure a consistent look for the
            # demo.
            bpArt.SetColor(bp.BP_BORDER_COLOR, wx.Colour(120,23,224))
            bpArt.SetColor(bp.BP_GRADIENT_COLOR_FROM, wx.Colour(60,11,112))
            bpArt.SetColor(bp.BP_GRADIENT_COLOR_TO, wx.Colour(120,23,224))
            bpArt.SetColor(bp.BP_BUTTONTEXT_COLOR, wx.Colour(70,143,255))
            bpArt.SetColor(bp.BP_SEPARATOR_COLOR,
                           bp.BrightenColour(wx.Colour(60, 11, 112), 0.85))
            bpArt.SetColor(bp.BP_SELECTION_BRUSH_COLOR, wx.Color(225, 225, 255))
            bpArt.SetColor(bp.BP_SELECTION_PEN_COLOR, wx.SystemSettings_GetColour(wx.SYS_COLOUR_ACTIVECAPTION))

        else:

            background = self.titleBar.GetBackgroundColour()            
            bpArt.SetColor(bp.BP_TEXT_COLOR, wx.BLUE)
            bpArt.SetColor(bp.BP_BORDER_COLOR,
                           bp.BrightenColour(background, 0.85))
            bpArt.SetColor(bp.BP_SEPARATOR_COLOR,
                           bp.BrightenColour(background, 0.85))
            bpArt.SetColor(bp.BP_BUTTONTEXT_COLOR, wx.BLACK)
            bpArt.SetColor(bp.BP_SELECTION_BRUSH_COLOR, wx.Colour(242, 242, 235))
            bpArt.SetColor(bp.BP_SELECTION_PEN_COLOR, wx.Colour(206, 206, 195))

        self.titleBar.SetStyle(self.style)
        
        
    def OnAlignment(self, event):
        
        # Here we change the alignment property of ButtonPanel
        current = event.GetId()
        edit_menu = self.GetMenuBar().FindMenu("Edit")
        edit_menu = self.GetMenuBar().GetMenu(edit_menu)
        menu = edit_menu.FindItem("BP_ALIGN_LEFT")
        
        alignment = self.alignments[current - menu]
        self.alignment = alignment

        self.ChangeLayout()    
        self.titleBar.SetAlignment(alignment)
        self.mainPanel.Layout()
        
        event.Skip()


    def OnDefaultStyle(self, event):
        
        # Restore the ButtonPanel default style (no gradient)
        self.style = bp.BP_DEFAULT_STYLE
        self.SetProperties()

        event.Skip()        


    def OnGradientStyle(self, event):

        # Use gradients to paint ButtonPanel background
        self.style = bp.BP_USE_GRADIENT
        self.SetProperties()

        event.Skip()        


    def OnDefaultDemo(self, event):
        
        # Reload the default startup demo
        self.CreateButtons()
        event.Skip()


    def OnButtonOnly(self, event):

        # Reload the button-only demo
        self.ButtonOnly()
        event.Skip()
        
        
    def OnButton(self, event):

        btn = event.GetId()
        indx = self.indices.index(btn)
        
        self.logtext.AppendText("Event Fired From " + self.strings[indx] + " Button\n")
        event.Skip()


    def OnSetBarText(self, event):

        dlg = wx.TextEntryDialog(self, "Enter The Text You Wish To Display On The Bar (Clear If No Text):",
                                 "Set Text", self.titleBar.GetBarText())
        
        if dlg.ShowModal() == wx.ID_OK:
        
            val = dlg.GetValue()
            self.titleBar.SetBarText(val)
            self.titleBar.DoLayout()
            self.mainPanel.Layout()


    def OnSettingsPanel(self, event):

        if self.hassettingpanel:
            self.settingspanel.Raise()
            return

        self.settingspanel = SettingsPanel(self, -1)
        self.settingspanel.Show()
        self.hassettingpanel = True


    def OnClose(self, event):
        
        self.Destroy()
        event.Skip()


    def OnAbout(self, event):

        msg = "This Is The About Dialog Of The ButtonPanel Demo.\n\n" + \
              "Author: Andrea Gavana @ 02 Oct 2006\n\n" + \
              "Please Report Any Bug/Requests Of Improvements\n" + \
              "To Me At The Following Adresses:\n\n" + \
              "andrea.gavana@gmail.com\n" + "gavana@kpo.kz\n\n" + \
              "Based On Eran C++ Implementation (wxWidgets Forum).\n\n" + \
              "Welcome To wxPython " + wx.VERSION_STRING + "!!"
              
        dlg = wx.MessageDialog(self, msg, "ButtonPanel wxPython Demo",
                               wx.OK | wx.ICON_INFORMATION)
        dlg.ShowModal()
        dlg.Destroy()        



#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        b = wx.Button(self, -1, " Test ButtonPanel ", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)


    def OnButton(self, evt):
        self.win = ButtonPanelDemo(self)
        self.win.Show(True)


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = bp.__doc__



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])


  
    
