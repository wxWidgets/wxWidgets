# AnalogClock setup dialog
#   E. A. Tacao <e.a.tacao |at| estadao.com.br>
#   http://j.domaindlx.com/elements28/wxpython/
#   15 Fev 2006, 22:00 GMT-03:00
# Distributed under the wxWidgets license.

import wx

import styles
import lib_setup.colourselect as csel
import lib_setup.fontselect as fsel
import lib_setup.buttontreectrlpanel as bt

#----------------------------------------------------------------------

_window_styles = ['wx.SIMPLE_BORDER', 'wx.DOUBLE_BORDER', 'wx.SUNKEN_BORDER',
                  'wx.RAISED_BORDER', 'wx.STATIC_BORDER', 'wx.NO_BORDER']

#----------------------------------------------------------------------

class _GroupBase(wx.Panel):
    def __init__(self, parent, title, group):
        wx.Panel.__init__(self, parent)

        self.parent = parent
        self.clock  = self.parent.clock
        self.group  = group
        self.target = {"Hours":   styles.HOUR,
                       "Minutes": styles.MINUTE,
                       "Seconds": styles.SECOND}.get(title)

        self.Bind(fsel.EVT_FONTSELECT, self.OnSelectFont)
        self.Bind(csel.EVT_COLOURSELECT, self.OnSelectColour)
        self.Bind(wx.EVT_SPINCTRL, self.OnSpin)
        self.Bind(wx.EVT_TEXT_ENTER, self.OnSpin)
        self.Bind(wx.EVT_CHOICE, self.OnChoice)


    def OnSelectFont(self, evt):
        self.clock.SetTickFont(evt.val, self.target)


    def OnSelectColour(self, evt):
        obj = evt.obj; val = evt.val

        if hasattr(self, "fc") and obj == self.fc:
            if self.group == "Hands":
                self.clock.SetHandFillColour(val, self.target)
            elif self.group == "Ticks":
                self.clock.SetTickFillColour(val, self.target)
            elif self.group == "Face":
                self.clock.SetFaceFillColour(val)

        elif hasattr(self, "bc") and obj == self.bc:
            if self.group == "Hands":
                self.clock.SetHandBorderColour(val, self.target)
            elif self.group == "Ticks":
                self.clock.SetTickBorderColour(val, self.target)
            elif self.group == "Face":
                self.clock.SetFaceBorderColour(val)

        elif hasattr(self, "sw") and obj == self.sw:
            self.clock.SetShadowColour(val)

        elif hasattr(self, "bg") and obj == self.bg:
            self.clock.SetBackgroundColour(val)

        elif hasattr(self, "fg") and obj == self.fg:
            self.clock.SetForegroundColour(val)
            self.parent.GetGrandParent().UpdateControls()


    def OnSpin(self, evt):
        obj = evt.GetEventObject(); val = evt.GetInt()

        if hasattr(self, "bw") and obj == self.bw:
            if self.group == "Hands":
                self.clock.SetHandBorderWidth(val, self.target)
            elif self.group == "Ticks":
                self.clock.SetTickBorderWidth(val, self.target)
            elif self.group == "Face":
                self.clock.SetFaceBorderWidth(val)

        elif hasattr(self, "sz") and obj == self.sz:
            if self.group == "Hands":
                self.clock.SetHandSize(val, self.target)
            elif self.group == "Ticks":
                self.clock.SetTickSize(val, self.target)

        elif hasattr(self, "of") and obj == self.of:
            self.clock.SetTickOffset(val, self.target)


    def OnChoice(self, evt):
        self.clock.SetWindowStyle(eval(evt.GetString()))

                              
    def UpdateControls(self):
        if hasattr(self, "ft"):
            self.ft.SetValue(self.clock.GetTickFont(self.target)[0])

        if hasattr(self, "fc"):
            if self.group == "Hands":
                self.fc.SetValue(self.clock.GetHandFillColour(self.target)[0])
            elif self.group == "Ticks":
                self.fc.SetValue(self.clock.GetTickFillColour(self.target)[0])
            elif self.group == "Face":
                self.fc.SetValue(self.clock.GetFaceFillColour())

        if hasattr(self, "bc"):
            if self.group == "Hands":
                self.bc.SetValue(self.clock.GetHandBorderColour(self.target)[0])
            elif self.group == "Ticks":
                self.bc.SetValue(self.clock.GetTickBorderColour(self.target)[0])
            elif self.group == "Face":
                self.bc.SetValue(self.clock.GetFaceBorderColour())

        if hasattr(self, "sw"):
            self.sw.SetValue(self.clock.GetShadowColour())

        if hasattr(self, "bg"):
            self.bg.SetValue(self.clock.GetBackgroundColour())

        if hasattr(self, "fg"):
            self.fg.SetValue(self.clock.GetForegroundColour())

        if hasattr(self, "bw"):
            if self.group == "Hands":
                self.bw.SetValue(self.clock.GetHandBorderWidth(self.target)[0])
            elif self.group == "Ticks":
                self.bw.SetValue(self.clock.GetTickBorderWidth(self.target)[0])
            elif self.group == "Face":
                self.bw.SetValue(self.clock.GetFaceBorderWidth())

        if hasattr(self, "sz"):
            if self.group == "Hands":
                self.sz.SetValue(self.clock.GetHandSize(self.target)[0])
            elif self.group == "Ticks":
                self.sz.SetValue(self.clock.GetTickSize(self.target)[0])

        if hasattr(self, "of"):
            self.of.SetValue(self.clock.GetTickOffset(self.target)[0])

        if hasattr(self, "ws"):
            for style in _window_styles:
                if self.clock.GetWindowStyleFlag() & eval(style):
                    self.ws.SetStringSelection(style)
                    break

#----------------------------------------------------------------------

class _Group_1(_GroupBase):
    def __init__(self, parent, title, group="Hands"):
        _GroupBase.__init__(self, parent, title, group)

        box = wx.StaticBoxSizer(wx.StaticBox(self, label=title), wx.VERTICAL)

        sizer = self.sizer = wx.GridBagSizer(2, 6)

        p = wx.StaticText(self, label="Border:")
        sizer.Add(p, pos=(0, 0), flag=wx.ALIGN_CENTRE_VERTICAL)

        p = self.bc = csel.ColourSelect(self)
        sizer.Add(p, pos=(0, 1), flag=wx.ALIGN_CENTRE_VERTICAL)

        p = self.bw = wx.SpinCtrl(self, size=(75, 21),
                                  min=0, max=100, value="75")
        sizer.Add(p, pos=(0, 2), span=(1, 2), flag=wx.ALIGN_CENTRE_VERTICAL)

        p = wx.StaticText(self, label="Fill:")
        sizer.Add(p, pos=(1, 0), flag=wx.ALIGN_CENTRE_VERTICAL)

        p = self.fc = csel.ColourSelect(self)
        sizer.Add(p, pos=(1, 1), flag=wx.ALIGN_CENTRE_VERTICAL)

        p = self.ls = wx.StaticText(self, label="Size:")
        sizer.Add(p, pos=(2, 0), flag=wx.ALIGN_CENTRE_VERTICAL)

        p = self.sz = wx.SpinCtrl(self, size=(75, 21),
                                  min=0, max=100, value="75")
        sizer.Add(p, pos=(2, 1), span=(1, 3), flag=wx.ALIGN_CENTRE_VERTICAL)

        box.Add(sizer)

        self.SetSizer(box)

#----------------------------------------------------------------------

class _Group_2(_Group_1):
    def __init__(self, parent, title, group="Ticks"):
        _Group_1.__init__(self, parent, title, group)

        sizer = self.sizer

        p = wx.StaticText(self, label="Offset:")
        sizer.Add(p, pos=(3, 0), flag=wx.ALIGN_CENTRE_VERTICAL)

        p = self.of = wx.SpinCtrl(self, size=(75, 21),
                                  min=0, max=100, value="75")
        sizer.Add(p, pos=(3, 1), span=(1, 3), flag=wx.ALIGN_CENTRE_VERTICAL)

        p = wx.StaticText(self, label="Font:")
        sizer.Add(p, pos=(4, 0), flag=wx.ALIGN_CENTRE_VERTICAL)

        p = self.ft = fsel.FontSelect(self)
        sizer.Add(p, pos=(4, 1), span=(1, 3), flag=wx.ALIGN_CENTRE_VERTICAL)

        self.GetSizer().Layout()

#----------------------------------------------------------------------

class _Group_3(_Group_1):
    def __init__(self, parent, title, group="Face"):
        _Group_1.__init__(self, parent, title, group)

        sizer = self.sizer

        for widget in [self.ls, self.sz]:
            sizer.Detach(widget)
            widget.Destroy()
        sizer.Layout()
        
        p = wx.StaticText(self, label="Shadow:")
        sizer.Add(p, pos=(2, 0), flag=wx.ALIGN_CENTRE_VERTICAL)

        p = self.sw = csel.ColourSelect(self)
        sizer.Add(p, pos=(2, 1), span=(1, 3), flag=wx.ALIGN_CENTRE_VERTICAL)

        self.GetSizer().Layout()

#----------------------------------------------------------------------

class _Group_4(_GroupBase):
    def __init__(self, parent, title, group="Window"):
        _GroupBase.__init__(self, parent, title, group)

        box = wx.StaticBoxSizer(wx.StaticBox(self, label=title), wx.VERTICAL)

        sizer = self.sizer = wx.GridBagSizer(2, 6)

        p = wx.StaticText(self, label="Foreground:")
        sizer.Add(p, pos=(0, 0), flag=wx.ALIGN_CENTRE_VERTICAL)

        p = self.fg = csel.ColourSelect(self)
        sizer.Add(p, pos=(0, 1), span=(1, 3), flag=wx.ALIGN_CENTRE_VERTICAL)

        p = wx.StaticText(self, label="Background:")
        sizer.Add(p, pos=(1, 0), flag=wx.ALIGN_CENTRE_VERTICAL)

        p = self.bg = csel.ColourSelect(self)
        sizer.Add(p, pos=(1, 1), span=(1, 3), flag=wx.ALIGN_CENTRE_VERTICAL)

        p = wx.StaticText(self, label="Style:")
        sizer.Add(p, pos=(2, 0), flag=wx.ALIGN_CENTRE_VERTICAL)

        p = self.ws = wx.Choice(self, choices=_window_styles)
        sizer.Add(p, pos=(2, 1), span=(1, 3), flag=wx.ALIGN_CENTRE_VERTICAL)

        box.Add(sizer)

        self.SetSizer(box)

#----------------------------------------------------------------------

class _PageBase(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)

        self.clock = self.GetGrandParent().GetParent()
        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(self.sizer)


    def UpdateControls(self):
        [group.UpdateControls() for group in self.GetChildren()]

#----------------------------------------------------------------------

class StylesPanel(bt.ButtonTreeCtrlPanel):
    def __init__(self, parent):
        bt.ButtonTreeCtrlPanel.__init__(self, parent)

        self.clock = self.GetGrandParent().GetParent()

        root = self.AddItem("Styles")
        g1 = self.AddItem("General", parent=root)
        g2 = self.AddItem("Hours",   parent=root)
        g3 = self.AddItem("Minutes", parent=root)
        self.groups = [g1, g2, g3]

        clockStyle = self.clock.GetClockStyle()
        hourStyle, minuteStyle = self.clock.GetTickStyle()

        for label in dir(styles):
            if label.startswith("TICKS_"):
                value = bool(getattr(styles, label) & hourStyle)
                self.AddItem(label, parent=g2, style=wx.RB_SINGLE, value=value)

                value = bool(getattr(styles, label) & minuteStyle)
                self.AddItem(label, parent=g3, style=wx.RB_SINGLE, value=value)

            elif not (label.startswith("DEFAULT_") or \
                      label.startswith("_") or \
                      label in ["HOUR", "MINUTE", "SECOND", "ALL"]):
                value = bool(getattr(styles, label) & clockStyle)
                self.AddItem(label, parent=g1, style=wx.CHK_2STATE, value=value)

        self.EnsureFirstVisible()

        self.Bind(bt.EVT_CHANGED, self.OnChanged)


    def OnChanged(self, evt):
        clockStyle, hourStyle, minuteStyle = \
          [reduce(lambda x, y: x | y,
           [getattr(styles, item) \
            for item in self.GetStringItemsChecked(group)], 0) \
            for group in self.groups]

        self.clock.SetClockStyle(clockStyle)
        self.clock.SetTickStyle(hourStyle, styles.HOUR)
        self.clock.SetTickStyle(minuteStyle, styles.MINUTE)


    def UpdateControls(self):
        clockStyle = self.clock.GetClockStyle()
        hourStyle, minuteStyle = self.clock.GetTickStyle()

        [g1, g2, g3] = self.groups

        for label in dir(styles):
            if label.startswith("TICKS_"):
                item = self.GetItemByLabel(label, g2)
                value = bool(getattr(styles, label) & hourStyle)
                self.SetItemValue(item, value)

                item = self.GetItemByLabel(label, g3)
                value = bool(getattr(styles, label) & minuteStyle)
                self.SetItemValue(item, value)

            elif not (label.startswith("DEFAULT_") or \
                      label.startswith("_") or \
                      label in ["HOUR", "MINUTE", "SECOND", "ALL"]):
                item = self.GetItemByLabel(label, g1)
                value = bool(getattr(styles, label) & clockStyle)
                self.SetItemValue(item, value)

#----------------------------------------------------------------------

class HandsPanel(_PageBase):
    def __init__(self, parent):
        _PageBase.__init__(self, parent)

        [self.sizer.Add(_Group_1(self, title), 1,
                        flag=wx.EXPAND|wx.ALL, border=6) \
         for title in ["Hours", "Minutes", "Seconds"]]

#----------------------------------------------------------------------

class TicksPanel(_PageBase):
    def __init__(self, parent):
        _PageBase.__init__(self, parent)

        [self.sizer.Add(_Group_2(self, title), 1,
                        flag=wx.EXPAND|wx.ALL, border=6) \
         for title in ["Hours", "Minutes"]]

#----------------------------------------------------------------------

class MiscPanel(_PageBase):
    def __init__(self, parent):
        _PageBase.__init__(self, parent)

        self.sizer.Add(_Group_3(self, "Face"), 1,
                       flag=wx.EXPAND|wx.ALL, border=6)
        self.sizer.Add(_Group_4(self, "Window"), 1,
                       flag=wx.EXPAND|wx.ALL, border=6)

#----------------------------------------------------------------------

class Setup(wx.Dialog):
    """AnalogClock customization dialog."""

    def __init__(self, parent):
        wx.Dialog.__init__(self, parent, title="AnalogClock Setup")

        sizer = wx.BoxSizer(wx.VERTICAL)

        nb = self.nb = wx.Notebook(self)
        for s in ["Styles", "Hands", "Ticks", "Misc"]:
            page = eval(s + "Panel(nb)"); page.Fit()
            nb.AddPage(page, s)
        nb.Fit()
        sizer.Add(nb, 1, flag = wx.EXPAND|wx.ALL, border=6)

        bsizer = wx.BoxSizer(wx.HORIZONTAL)
        bsizer.Add(wx.Button(self, label="Reset"),
                   flag = wx.LEFT|wx.RIGHT, border=6)
        bsizer.Add(wx.Button(self, wx.ID_OK),
                   flag = wx.LEFT|wx.RIGHT, border=6)
        sizer.Add(bsizer, 0, flag=wx.ALIGN_RIGHT|wx.ALL, border=6)

        self.Bind(wx.EVT_CLOSE, self.OnClose)
        self.Bind(wx.EVT_BUTTON, self.OnButton)

        self.customcolours = [None] * 16

        self.SetSizerAndFit(sizer)
        wx.CallAfter(self.UpdateControls)
        

    def OnClose(self, evt):
        self.Hide()


    def OnButton(self, evt):
        if evt.GetEventObject().GetLabel() == "Reset":
            self.ResetClock()
        evt.Skip()


    def UpdateControls(self):
        wx.BeginBusyCursor()
        for i in range(self.nb.GetPageCount()):
            self.nb.GetPage(i).UpdateControls()
        wx.EndBusyCursor()


    def ResetClock(self):
        clock = self.GetParent()

        wx.BeginBusyCursor()

        clock.SetClockStyle(styles.DEFAULT_CLOCK_STYLE)
        clock.SetTickStyle(styles.TICKS_POLY, styles.HOUR)
        clock.SetTickStyle(styles.TICKS_CIRCLE, styles.MINUTE)

        clock.SetTickFont(wx.SystemSettings.GetFont(wx.SYS_DEFAULT_GUI_FONT))

        clock.SetHandBorderWidth(0)
        clock.SetTickBorderWidth(0)
        clock.SetFaceBorderWidth(0)

        clock.SetHandSize(7, styles.HOUR)
        clock.SetHandSize(5, styles.MINUTE)
        clock.SetHandSize(1, styles.SECOND)

        clock.SetTickSize(25, styles.HOUR)
        clock.SetTickSize(5,  styles.MINUTE)

        clock.SetTickOffset(0)

        clock.SetWindowStyle(wx.NO_BORDER)

        sw = wx.SystemSettings.GetColour(wx.SYS_COLOUR_3DSHADOW)
        clock.SetShadowColour(sw)

        no_color = wx.SystemSettings.GetColour(wx.SYS_COLOUR_3DFACE)
        clock.SetFaceFillColour(no_color)
        clock.SetFaceBorderColour(no_color)
        clock.SetBackgroundColour(no_color)

        fg = wx.SystemSettings.GetColour(wx.SYS_COLOUR_WINDOWTEXT)
        clock.SetForegroundColour(fg)

        self.UpdateControls()

        wx.EndBusyCursor()


#
#
### eof
