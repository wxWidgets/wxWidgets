# Name:         params.py
# Purpose:      Classes for parameter introduction
# Author:       Roman Rolinsky <rolinsky@mema.ucl.ac.be>
# Created:      22.08.2001
# RCS-ID:       $Id$

import string
import os.path
from globals import *
from types import *

genericStyles = [
    'wxSIMPLE_BORDER', 'wxSUNKEN_BORDER', 'wxDOUBLE_BORDER',
    'wxRAISED_BORDER', 'wxSTATIC_BORDER', 'wxNO_BORDER',
    'wxCLIP_CHILDREN', 'wxTRANSPARENT_WINDOW', 'wxWANTS_CHARS',
    'wxNO_FULL_REPAINT_ON_RESIZE', 'wxFULL_REPAINT_ON_RESIZE'
    ]

genericExStyles = [
    'wxWS_EX_VALIDATE_RECURSIVELY',
    'wxWS_EX_BLOCK_EVENTS',
    'wxWS_EX_TRANSIENT',
    'wxFRAME_EX_CONTEXTHELP',
    'wxWS_EX_PROCESS_IDLE',
    'wxWS_EX_PROCESS_UI_UPDATES'
    ]

# Global vars initialized in Panel.__init__ for button and textbox size in screen pixels
buttonSize = textSise = None
# Default Button size in dialog units
buttonSizeD = (35,-1)


# Class that can properly disable children
class PPanel(wx.Panel):
    def __init__(self, parent, name):
        wx.Panel.__init__(self, parent, -1, name=name)
        self.modified = self.freeze = False
    def Enable(self, value):
        self.enabled = value
        # Something strange is going on with enable so we make sure...
        for w in self.GetChildren():
            w.Enable(value)
        #wx.Panel.Enable(self, value)
    def SetModified(self, state=True):
        self.modified = state
        if state: g.panel.SetModified(True)
    # Common method to set modified state
    def OnChange(self, evt):
        if self.freeze: return
        self.SetModified()
        evt.Skip()

class ParamBinaryOr(PPanel):
    def __init__(self, parent, name):
        PPanel.__init__(self, parent, name)
        self.ID_TEXT_CTRL = wx.NewId()
        self.ID_BUTTON_CHOICES = wx.NewId()
        sizer = wx.BoxSizer()
        self.text = wx.TextCtrl(self, self.ID_TEXT_CTRL, size=wx.Size(200,-1))
        sizer.Add(self.text, 0, wx.RIGHT | wx.ALIGN_CENTER_VERTICAL, 5)
        self.button = wx.Button(self, self.ID_BUTTON_CHOICES, 'Edit...', 
                                size=(buttonSize[0], textSize[1]))
        sizer.Add(self.button, 0, wx.EXPAND)
        self.SetSizerAndFit(sizer)
        wx.EVT_BUTTON(self, self.ID_BUTTON_CHOICES, self.OnButtonChoices)
        wx.EVT_TEXT(self, self.ID_TEXT_CTRL, self.OnChange)
    def GetValue(self):
        return self.text.GetValue()
    def SetValue(self, value):
        self.freeze = True
        self.text.SetValue(value)
        self.freeze = False
    def OnButtonChoices(self, evt):
        dlg = g.frame.res.LoadDialog(self, 'DIALOG_CHOICES')
        if self.GetName() == 'flag':  dlg.SetTitle('Sizer item flags')
        elif self.GetName() == 'style':  dlg.SetTitle('Window styles')
        elif self.GetName() == 'exstyle':  dlg.SetTitle('Extended window styles')
        listBox = xrc.XRCCTRL(dlg, 'CHECKLIST')
        listBox.InsertItems(self.values, 0)
        value = map(string.strip, self.text.GetValue().split('|'))
        if value == ['']: value = []
        ignored = []
        for i in value:
            try:
                listBox.Check(self.values.index(i))
            except ValueError:
                # Try to find equal
                if self.equal.has_key(i):
                    listBox.Check(self.values.index(self.equal[i]))
                else:
                    print 'WARNING: unknown flag: %s: ignored.' % i
                    ignored.append(i)
        if dlg.ShowModal() == wx.ID_OK:
            value = []
            for i in range(listBox.GetCount()):
                if listBox.IsChecked(i):
                    value.append(self.values[i])
            # Add ignored flags
            value.extend(ignored)
            self.SetValue('|'.join(value))
            self.SetModified()
        dlg.Destroy()

class ParamFlag(ParamBinaryOr):
    values = ['wxTOP', 'wxBOTTOM', 'wxLEFT', 'wxRIGHT', 'wxALL',
              'wxEXPAND', 'wxGROW', 'wxSHAPED', 'wxSTRETCH_NOT',
              'wxALIGN_CENTRE', 'wxALIGN_LEFT', 'wxALIGN_RIGHT',
              'wxALIGN_TOP', 'wxALIGN_BOTTOM', 
              'wxALIGN_CENTRE_VERTICAL', 'wxALIGN_CENTRE_HORIZONTAL', 
              'wxADJUST_MINSIZE', 'wxFIXED_MINSIZE'
              ]
    equal = {'wxALIGN_CENTER': 'wxALIGN_CENTRE',
             'wxALIGN_CENTER_VERTICAL': 'wxALIGN_CENTRE_VERTICAL',
             'wxALIGN_CENTER_HORIZONTAL': 'wxALIGN_CENTRE_HORIZONTAL',
             'wxUP': 'wxTOP', 'wxDOWN': 'wxBOTTOM', 'wxNORTH': 'wxTOP',
             'wxSOUTH': 'wxBOTTOM', 'wxWEST': 'wxLEFT', 'wxEAST': 'wxRIGHT'}
    def __init__(self, parent, name):
        ParamBinaryOr.__init__(self, parent, name)

class ParamNonGenericStyle(ParamBinaryOr):
    def __init__(self, parent, name):
        self.values = g.currentXXX.winStyles
        ParamBinaryOr.__init__(self, parent, name)

class ParamStyle(ParamBinaryOr):
    equal = {'wxALIGN_CENTER': 'wxALIGN_CENTRE'}
    def __init__(self, parent, name):
        ParamBinaryOr.__init__(self, parent, name)
        self.valuesSpecific = g.currentXXX.winStyles
        if self.valuesSpecific:         # override if using specific styles
            # Remove duplicates
            self.valuesGeneric = [s for s in genericStyles
                                  if s not in self.valuesSpecific]
            wx.EVT_BUTTON(self, self.ID_BUTTON_CHOICES, self.OnButtonChoicesBoth)
        else:
            self.values = genericStyles
    def OnButtonChoicesBoth(self, evt):
        dlg = g.frame.res.LoadDialog(self, 'DIALOG_STYLES')
        listBoxSpecific = xrc.XRCCTRL(dlg, 'CHECKLIST_SPECIFIC')
        listBoxSpecific.InsertItems(self.valuesSpecific, 0)
        listBoxGeneric = xrc.XRCCTRL(dlg, 'CHECKLIST_GENERIC')
        listBoxGeneric.InsertItems(self.valuesGeneric, 0)
        value = map(string.strip, self.text.GetValue().split('|'))
        if value == ['']: value = []
        # Set specific styles
        value2 = []                     # collect generic and ignored here
        for i in value:
            try:
                listBoxSpecific.Check(self.valuesSpecific.index(i))
            except ValueError:
                # Try to find equal
                if self.equal.has_key(i):
                    listBoxSpecific.Check(self.valuesSpecific.index(self.equal[i]))
                else:
                    value2.append(i)
        ignored = []
        # Set generic styles, collect non-standart values
        for i in value2:
            try:
                listBoxGeneric.Check(self.valuesGeneric.index(i))
            except ValueError:
                # Try to find equal
                if self.equal.has_key(i):
                    listBoxGeneric.Check(self.valuesGeneric.index(self.equal[i]))
                else:
                    print 'WARNING: unknown flag: %s: ignored.' % i
                    ignored.append(i)
        if dlg.ShowModal() == wx.ID_OK:
            value = [self.valuesSpecific[i]
                     for i in range(listBoxSpecific.GetCount())
                     if listBoxSpecific.IsChecked(i)] + \
                     [self.valuesGeneric[i]
                      for i in range(listBoxGeneric.GetCount())
                      if listBoxGeneric.IsChecked(i)] + ignored
            self.SetValue('|'.join(value))
            self.SetModified()
        dlg.Destroy()

class ParamExStyle(ParamBinaryOr):
    def __init__(self, parent, name):
        if g.currentXXX:
            self.values = g.currentXXX.exStyles + genericExStyles
        else:
            self.values = []
        ParamBinaryOr.__init__(self, parent, name)

class ParamColour(PPanel):
    def __init__(self, parent, name):
        PPanel.__init__(self, parent, name)
        self.ID_TEXT_CTRL = wx.NewId()
        self.ID_BUTTON = wx.NewId()
        sizer = wx.BoxSizer()
        self.text = wx.TextCtrl(self, self.ID_TEXT_CTRL, size=(80,-1))
        sizer.Add(self.text, 0, wx.ALIGN_CENTER_VERTICAL | wx.TOP | wx.BOTTOM, 2)
        self.button = wx.Panel(self, self.ID_BUTTON, wx.DefaultPosition, wx.Size(20, 20))
        sizer.Add(self.button, 0, wx.ALIGN_CENTER_VERTICAL | wx.LEFT, 5)
        self.SetSizer(sizer)
        self.textModified = False
        wx.EVT_PAINT(self.button, self.OnPaintButton)
        wx.EVT_TEXT(self, self.ID_TEXT_CTRL, self.OnChange)
        wx.EVT_LEFT_DOWN(self.button, self.OnLeftDown)
    def GetValue(self):
        return self.text.GetValue()
    def SetValue(self, value):
        self.freeze = True
        if not value: value = '#FFFFFF'
        self.text.SetValue(str(value))  # update text ctrl
        try:
            colour = wx.Colour(int(value[1:3], 16), int(value[3:5], 16), int(value[5:7], 16))
            self.button.SetBackgroundColour(colour)
        except:                         # ignore errors
            pass
        self.button.Refresh()
        self.freeze = False
    def OnPaintButton(self, evt):
        dc = wx.PaintDC(self.button)
        dc.SetBrush(wx.TRANSPARENT_BRUSH)
        if self.IsEnabled(): dc.SetPen(wx.BLACK_PEN)
        else: dc.SetPen(wx.GREY_PEN)
        size = self.button.GetSize()
        dc.DrawRectangle(0, 0, size.width, size.height)
    def OnLeftDown(self, evt):
        data = wx.ColourData()
        data.SetColour(self.GetValue())
        dlg = wx.ColourDialog(self, data)
        if dlg.ShowModal() == wx.ID_OK:
            self.SetValue('#%02X%02X%02X' % dlg.GetColourData().GetColour().Get())
            self.SetModified()
        dlg.Destroy()

################################################################################

# Mapping from wx constants to XML strings
fontFamiliesWx2Xml = {wx.DEFAULT: 'default', wx.DECORATIVE: 'decorative',
                wx.ROMAN: 'roman', wx.SCRIPT: 'script', wx.SWISS: 'swiss',
                wx.MODERN: 'modern'}
fontStylesWx2Xml = {wx.NORMAL: 'normal', wx.SLANT: 'slant', wx.ITALIC: 'italic'}
fontWeightsWx2Xml = {wx.NORMAL: 'normal', wx.LIGHT: 'light', wx.BOLD: 'bold'}
def ReverseMap(m):
    rm = {}
    for k,v in m.items(): rm[v] = k
    return rm
fontFamiliesXml2wx = ReverseMap(fontFamiliesWx2Xml)
fontStylesXml2wx = ReverseMap(fontStylesWx2Xml)
fontWeightsXml2wx = ReverseMap(fontWeightsWx2Xml)

class ParamFont(PPanel):
    def __init__(self, parent, name):
        PPanel.__init__(self, parent, name)
        self.ID_TEXT_CTRL = wx.NewId()
        self.ID_BUTTON_SELECT = wx.NewId()
        sizer = wx.BoxSizer()
        self.text = wx.TextCtrl(self, self.ID_TEXT_CTRL, size=(200,-1))
        sizer.Add(self.text, 0, wx.RIGHT | wx.ALIGN_CENTER_VERTICAL, 5)
        self.button = wx.Button(self, self.ID_BUTTON_SELECT, 'Select...', size=buttonSize)
        sizer.Add(self.button, 0, wx.ALIGN_CENTER_VERTICAL)
        self.SetSizer(sizer)
        self.textModified = False
        wx.EVT_BUTTON(self, self.ID_BUTTON_SELECT, self.OnButtonSelect)
        wx.EVT_TEXT(self, self.ID_TEXT_CTRL, self.OnChange)
    def OnChange(self, evt):
        PPanel.OnChange(self, evt)
        self.textModified = True
    def _defaultValue(self):
        return [`g._sysFont.GetPointSize()`, 'default', 'normal', 'normal', '0', '', '']
    def GetValue(self):
        if self.textModified:           # text has newer value
            try:
                return eval(self.text.GetValue())
            except SyntaxError:
                wx.LogError('Syntax error in parameter value: ' + self.GetName())
                return self._defaultValue()
        return self.value
    def SetValue(self, value):
        self.freeze = True              # disable other handlers
        if not value: value = self._defaultValue()
        self.value = value
        self.text.SetValue(str(value))  # update text ctrl
        self.freeze = False
    def OnButtonSelect(self, evt):
        if self.textModified:           # text has newer value
            try:
                self.value = eval(self.text.GetValue())
            except SyntaxError:
                wx.LogError('Syntax error in parameter value: ' + self.GetName())
                self.value = self._defaultValue()
        # Make initial font
        # Default values
        size = g._sysFont.GetPointSize()
        family = wx.DEFAULT
        style = weight = wx.NORMAL
        underlined = 0
        face = ''
        enc = wx.FONTENCODING_DEFAULT
        # Fall back to default if exceptions
        error = False
        try:
            try: size = int(self.value[0])
            except ValueError: error = True; wx.LogError('Invalid size specification')
            try: family = fontFamiliesXml2wx[self.value[1]]
            except KeyError: error = True; wx.LogError('Invalid family specification')
            try: style = fontStylesXml2wx[self.value[2]]
            except KeyError: error = True; wx.LogError('Invalid style specification')
            try: weight = fontWeightsXml2wx[self.value[3]]
            except KeyError: error = True; wx.LogError('Invalid weight specification')
            try: underlined = bool(self.value[4])
            except ValueError: error = True; wx.LogError('Invalid underlined flag specification')
            face = self.value[5]
        except IndexError:
            error = True
        mapper = wx.FontMapper()
        if not self.value[6]: enc = mapper.CharsetToEncoding(self.value[6])
            
        if error: wx.LogError('Invalid font specification')
        if enc == wx.FONTENCODING_DEFAULT: enc = wx.FONTENCODING_SYSTEM
        font = wx.Font(size, family, style, weight, underlined, face, enc)
        data = wx.FontData()
        data.SetInitialFont(font)
        dlg = wx.FontDialog(self, data)
        if dlg.ShowModal() == wx.ID_OK:
            font = dlg.GetFontData().GetChosenFont()
            if font.GetEncoding() == wx.FONTENCODING_SYSTEM:
                encName = ''
            else:
                encName = wx.FontMapper.GetEncodingName(font.GetEncoding()).encode()
            value = [str(font.GetPointSize()),
                     fontFamiliesWx2Xml.get(font.GetFamily(), "default"),
                     fontStylesWx2Xml.get(font.GetStyle(), "normal"),
                     fontWeightsWx2Xml.get(font.GetWeight(), "normal"),
                     str(int(font.GetUnderlined())),
                     font.GetFaceName().encode(),
                     encName
                     ]
            self.SetValue(value)
            self.SetModified()
            self.textModified = False
        dlg.Destroy()

################################################################################

class ParamInt(PPanel):
    def __init__(self, parent, name):
        PPanel.__init__(self, parent, name)
        self.ID_SPIN_CTRL = wx.NewId()
        sizer = wx.BoxSizer()
        self.spin = wx.SpinCtrl(self, self.ID_SPIN_CTRL, size=(60,-1))
        self.spin.SetRange(-2147483648, 2147483647) # min/max integers
        sizer.Add(self.spin)
        self.SetSizer(sizer)
        wx.EVT_SPINCTRL(self, self.ID_SPIN_CTRL, self.OnChange)
    def GetValue(self):
        return str(self.spin.GetValue())
    def SetValue(self, value):
        self.freeze = True
        if not value: value = 0
        self.spin.SetValue(int(value))
        self.freeze = False

# Non-negative number
class ParamIntNN(PPanel):
    def __init__(self, parent, name):
        PPanel.__init__(self, parent, name)
        self.ID_SPIN_CTRL = wx.NewId()
        sizer = wx.BoxSizer()
        self.spin = wx.SpinCtrl(self, self.ID_SPIN_CTRL, size=(60,-1))
        self.spin.SetRange(0, 10000) # min/max integers
        sizer.Add(self.spin)
        self.SetSizer(sizer)
        wx.EVT_SPINCTRL(self, self.ID_SPIN_CTRL, self.OnChange)
    def GetValue(self):
        return str(self.spin.GetValue())
    def SetValue(self, value):
        self.freeze = True
        if not value: value = 0
        self.spin.SetValue(int(value))
        self.freeze = False

# Same as int but allows dialog units (XXXd)
class ParamUnit(PPanel):
    def __init__(self, parent, name):
        PPanel.__init__(self, parent, name)
        self.ID_TEXT_CTRL = wx.NewId()
        self.ID_SPIN_BUTTON = wx.NewId()
        sizer = wx.BoxSizer(wx.HORIZONTAL)
        self.spin = wx.SpinButton(self, self.ID_SPIN_BUTTON, style = wx.SP_VERTICAL, size=(-1,0))
        textW = 60 - self.spin.GetSize()[0]
        self.text = wx.TextCtrl(self, self.ID_TEXT_CTRL, size=(textW,-1))
        self.spin.SetRange(-10000, 10000)
        sizer.Add(self.text, 0, wx.EXPAND)
        sizer.Add(self.spin, 0, wx.EXPAND)
        self.SetSizer(sizer)
        self.spin.Bind(wx.EVT_SPIN_UP, self.OnSpinUp)
        self.spin.Bind(wx.EVT_SPIN_DOWN, self.OnSpinDown)
        self.text.Bind(wx.EVT_TEXT, self.OnChange)
        
    def GetValue(self):
        return self.text.GetValue()
    def SetValue(self, value):
        self.freeze = True
        if not value: value = '0'        
        self.text.SetValue(value)
        self.Change(0)
        self.freeze = False
    def Change(self, x):
        self.freeze = True
        # Check if we are working with dialog units
        value = self.text.GetValue()
        units = ''
        if value[-1].upper() == 'D':
            units = value[-1]
            value = value[:-1]
        try:
            intValue = int(value) + x
            self.spin.SetValue(intValue)
            if x:                       # 0 can be passed to update spin value only
                self.text.SetValue(str(intValue) + units)
                self.SetModified()
        except:
            # !!! Strange, if I use wx.LogWarning, event is re-generated
            print 'ERROR: incorrect unit format'
        self.freeze = False
    def OnSpinUp(self, evt):
        self.freeze = True
        self.Change(1)
    def OnSpinDown(self, evt):
        if self.freeze: return
        self.freeze = True
        self.Change(-1)

class ParamMultilineText(PPanel):
    def __init__(self, parent, name, textWidth=-1):
        PPanel.__init__(self, parent, name)
        self.ID_TEXT_CTRL = wx.NewId()
        self.ID_BUTTON_EDIT = wx.NewId()
        sizer = wx.BoxSizer()
        self.text = wx.TextCtrl(self, self.ID_TEXT_CTRL, size=wx.Size(200,-1))
        sizer.Add(self.text, 0, wx.RIGHT | wx.ALIGN_CENTER_VERTICAL, 5)
        self.button = wx.Button(self, self.ID_BUTTON_EDIT, 'Edit...', size=buttonSize)
        sizer.Add(self.button, 0, wx.ALIGN_CENTER_VERTICAL)
        self.SetSizerAndFit(sizer)
        wx.EVT_BUTTON(self, self.ID_BUTTON_EDIT, self.OnButtonEdit)
        wx.EVT_TEXT(self, self.ID_TEXT_CTRL, self.OnChange)
    def GetValue(self):
        return self.text.GetValue()
    def SetValue(self, value):
        self.freeze = True              # disable other handlers
        self.text.SetValue(value)
        self.freeze = False             # disable other handlers
    def OnButtonEdit(self, evt):
        dlg = g.frame.res.LoadDialog(self, 'DIALOG_TEXT')
        textCtrl = xrc.XRCCTRL(dlg, 'TEXT')
        textCtrl.SetValue(self.text.GetValue())
        if dlg.ShowModal() == wx.ID_OK:
            self.text.SetValue(textCtrl.GetValue())
            self.SetModified()
        dlg.Destroy()

class ParamText(PPanel):
    def __init__(self, parent, name, textWidth=-1, style=0):
        PPanel.__init__(self, parent, name)
        self.ID_TEXT_CTRL = wx.NewId()
        # We use sizer even here to have the same size of text control
        sizer = wx.BoxSizer()
        self.text = wx.TextCtrl(self, self.ID_TEXT_CTRL, size=wx.Size(textWidth,-1), style=style)
        if textWidth == -1: option = 1
        else: option = 0
        sizer.Add(self.text, option, wx.ALIGN_CENTER_VERTICAL | wx.TOP | wx.BOTTOM, 2)
        self.SetSizer(sizer)
        wx.EVT_TEXT(self, self.ID_TEXT_CTRL, self.OnChange)
    def GetValue(self):
        return self.text.GetValue()
    def SetValue(self, value):
        self.freeze = True              # disable other handlers
        self.text.SetValue(value)
        self.freeze = False             # disable other handlers

class ParamAccel(ParamText):
    def __init__(self, parent, name):
        ParamText.__init__(self, parent, name, 100)

class ParamPosSize(ParamText):
    def __init__(self, parent, name):
        ParamText.__init__(self, parent, name, 80)

class ParamLabel(ParamText):
    def __init__(self, parent, name):
        ParamText.__init__(self, parent, name, 200)

class ParamEncoding(ParamText):
    def __init__(self, parent, name):
        ParamText.__init__(self, parent, name, 100)

class ParamComment(ParamText):
    def __init__(self, parent, name):
        ParamText.__init__(self, parent, name, 330 + buttonSize[0],
                           style=wx.TE_PROCESS_ENTER)

class ContentDialog(wx.Dialog):
    def __init__(self, parent, value):
        # Load from resource
        pre = wx.PreDialog()
        g.frame.res.LoadOnDialog(pre, parent, 'DIALOG_CONTENT')
        self.PostCreate(pre)
        self.list = xrc.XRCCTRL(self, 'LIST')
        # Set list items
        for v in value:
            self.list.Append(v)
        self.SetAutoLayout(True)
        self.GetSizer().Fit(self)
        # Callbacks
        self.ID_BUTTON_APPEND = xrc.XRCID('BUTTON_APPEND')
        self.ID_BUTTON_REMOVE = xrc.XRCID('BUTTON_REMOVE')
        self.ID_BUTTON_UP = xrc.XRCID('BUTTON_UP')
        self.ID_BUTTON_DOWN = xrc.XRCID('BUTTON_DOWN')
        wx.EVT_BUTTON(self, self.ID_BUTTON_UP, self.OnButtonUp)
        wx.EVT_BUTTON(self, self.ID_BUTTON_DOWN, self.OnButtonDown)
        wx.EVT_BUTTON(self, self.ID_BUTTON_APPEND, self.OnButtonAppend)
        wx.EVT_BUTTON(self, self.ID_BUTTON_REMOVE, self.OnButtonRemove)
        wx.EVT_UPDATE_UI(self, self.ID_BUTTON_UP, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(self, self.ID_BUTTON_DOWN, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(self, self.ID_BUTTON_REMOVE, self.OnUpdateUI)
    def OnButtonUp(self, evt):
        i = self.list.GetSelection()
        str = self.list.GetString(i)
        self.list.Delete(i)
        self.list.InsertItems([str], i-1)
        self.list.SetSelection(i-1)
    def OnButtonDown(self, evt):
        i = self.list.GetSelection()
        str = self.list.GetString(i)
        self.list.Delete(i)
        self.list.InsertItems([str], i+1)
        self.list.SetSelection(i+1)
    def OnButtonAppend(self, evt):
        str = wx.GetTextFromUser('Enter new item:', 'Append', '', self)
        self.list.Append(str)
    def OnButtonRemove(self, evt):
        self.list.Delete(self.list.GetSelection())
    def OnUpdateUI(self, evt):
        if evt.GetId() == self.ID_BUTTON_REMOVE:
            evt.Enable(self.list.GetSelection() != -1)
        elif evt.GetId() == self.ID_BUTTON_UP:
            evt.Enable(self.list.GetSelection() > 0)
        elif evt.GetId() == self.ID_BUTTON_DOWN:
            evt.Enable(self.list.GetSelection() != -1 and \
                       self.list.GetSelection() < self.list.GetCount() - 1)

class ContentCheckListDialog(wx.Dialog):
    def __init__(self, parent, value):
        pre = wx.PreDialog()
        g.frame.res.LoadOnDialog(pre, parent, 'DIALOG_CONTENT_CHECKLIST')
        self.PostCreate(pre)
        self.list = xrc.XRCCTRL(self, 'CHECK_LIST')
        # Set list items
        i = 0
        for v,ch in value:
            self.list.Append(v)
            self.list.Check(i, ch)
            i += 1
        self.SetAutoLayout(True)
        self.GetSizer().Fit(self)
        # Callbacks
        self.ID_BUTTON_APPEND = xrc.XRCID('BUTTON_APPEND')
        self.ID_BUTTON_REMOVE = xrc.XRCID('BUTTON_REMOVE')
        self.ID_BUTTON_UP = xrc.XRCID('BUTTON_UP')
        self.ID_BUTTON_DOWN = xrc.XRCID('BUTTON_DOWN')
        wx.EVT_CHECKLISTBOX(self, self.list.GetId(), self.OnCheck)
        wx.EVT_BUTTON(self, self.ID_BUTTON_UP, self.OnButtonUp)
        wx.EVT_BUTTON(self, self.ID_BUTTON_DOWN, self.OnButtonDown)
        wx.EVT_BUTTON(self, self.ID_BUTTON_APPEND, self.OnButtonAppend)
        wx.EVT_BUTTON(self, self.ID_BUTTON_REMOVE, self.OnButtonRemove)
        wx.EVT_UPDATE_UI(self, self.ID_BUTTON_UP, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(self, self.ID_BUTTON_DOWN, self.OnUpdateUI)
        wx.EVT_UPDATE_UI(self, self.ID_BUTTON_REMOVE, self.OnUpdateUI)
    def OnCheck(self, evt):
        # !!! Wrong wxGTK (wxMSW?) behavior: toggling selection if checking
        self.list.Deselect(evt.GetSelection())
    def OnButtonUp(self, evt):
        i = self.list.GetSelection()
        str, ch = self.list.GetString(i), self.list.IsChecked(i)
        self.list.Delete(i)
        self.list.InsertItems([str], i-1)
        self.list.Check(i-1, ch)
        self.list.SetSelection(i-1)
    def OnButtonDown(self, evt):
        i = self.list.GetSelection()
        str, ch = self.list.GetString(i), self.list.IsChecked(i)
        self.list.Delete(i)
        self.list.InsertItems([str], i+1)
        self.list.Check(i+1, ch)
        self.list.SetSelection(i+1)
    def OnButtonAppend(self, evt):
        str = wx.GetTextFromUser('Enter new item:', 'Append', '', self)
        self.list.Append(str)
    def OnButtonRemove(self, evt):
        self.list.Delete(self.list.GetSelection())
    def OnUpdateUI(self, evt):
        if evt.GetId() == self.ID_BUTTON_REMOVE:
            evt.Enable(self.list.GetSelection() != -1)
        elif evt.GetId() == self.ID_BUTTON_UP:
            evt.Enable(self.list.GetSelection() > 0)
        elif evt.GetId() == self.ID_BUTTON_DOWN:
            evt.Enable(self.list.GetSelection() != -1 and \
                       self.list.GetSelection() < self.list.GetCount() - 1)

class ParamContent(PPanel):
    def __init__(self, parent, name):
        PPanel.__init__(self, parent, name)
        self.ID_TEXT_CTRL = wx.NewId()
        self.ID_BUTTON_EDIT = wx.NewId()
        sizer = wx.BoxSizer()
        self.text = wx.TextCtrl(self, self.ID_TEXT_CTRL, size=wx.Size(200,-1))
        sizer.Add(self.text, 0, wx.RIGHT | wx.ALIGN_CENTER_VERTICAL, 5)
        self.button = wx.Button(self, self.ID_BUTTON_EDIT, 'Edit...', size=buttonSize)
        sizer.Add(self.button, 0, wx.ALIGN_CENTER_VERTICAL)
        self.SetSizer(sizer)
        self.textModified = False
        wx.EVT_BUTTON(self, self.ID_BUTTON_EDIT, self.OnButtonEdit)
        wx.EVT_TEXT(self, self.ID_TEXT_CTRL, self.OnChange)
    def OnChange(self, evt):
        PPanel.OnChange(self, evt)
        self.textModified = True
    def GetValue(self):
        if self.textModified:           # text has newer value
            try:
                return self.text.GetValue().split('|')
            except ValueError:
                return []
        return self.value
    def SetValue(self, value):
        self.freeze = True
        if not value: value = []
        self.value = value
        repr_ = '|'.join(map(str, value))
        self.text.SetValue(repr_)  # update text ctrl
        self.freeze = False
    def OnButtonEdit(self, evt):
        if self.textModified:           # text has newer value
            self.value = self.GetValue()
        dlg = ContentDialog(self, self.value)
        if dlg.ShowModal() == wx.ID_OK:
            value = []
            for i in range(dlg.list.GetCount()):
                value.append(dlg.list.GetString(i))
            self.SetValue(value)
            self.SetModified()
            self.textModified = False
        dlg.Destroy()
    def SetModified(self, state=True):
        PPanel.SetModified(self, state)
        self.textModified = False

# CheckList content
class ParamContentCheckList(ParamContent):
    def __init__(self, parent, name):
        ParamContent.__init__(self, parent, name)
    def OnButtonEdit(self, evt):
        if self.textModified:           # text has newer value
            self.value = self.GetValue()
        dlg = ContentCheckListDialog(self, self.value)
        if dlg.ShowModal() == wx.ID_OK:
            value = []
            for i in range(dlg.list.GetCount()):
                value.append((dlg.list.GetString(i), int(dlg.list.IsChecked(i))))
            self.SetValue(value)
            self.SetModified()
            self.textModified = False
        dlg.Destroy()
    def SetValue(self, value):
        self.freeze = True
        if not value: value = []
        self.value = value
        repr_ = '|'.join(map(str,value))
        self.text.SetValue(repr_)  # update text ctrl
        self.freeze = False        

class IntListDialog(wx.Dialog):
    def __init__(self, parent, value):
        pre = wx.PreDialog()
        g.frame.res.LoadOnDialog(pre, parent, 'DIALOG_INTLIST')
        self.PostCreate(pre)
        self.list = xrc.XRCCTRL(self, 'LIST')
        # Set list items
        value.sort()
        for v in value:
            if type(v) != IntType:
                wx.LogError('Invalid item type')
            else:
                self.list.Append(str(v))
        self.SetAutoLayout(True)
        self.GetSizer().Fit(self)
        # Callbacks
        self.spinCtrl = xrc.XRCCTRL(self, 'SPIN')
        wx.EVT_BUTTON(self, xrc.XRCID('BUTTON_ADD'), self.OnButtonAdd)
        self.ID_BUTTON_REMOVE = xrc.XRCID('BUTTON_REMOVE')
        wx.EVT_BUTTON(self, self.ID_BUTTON_REMOVE, self.OnButtonRemove)
        wx.EVT_BUTTON(self, xrc.XRCID('BUTTON_CLEAR'), self.OnButtonClear)
        wx.EVT_UPDATE_UI(self, self.ID_BUTTON_REMOVE, self.OnUpdateUI)
    def OnButtonAdd(self, evt):
        # Check that it's unique
        try:
            v = self.spinCtrl.GetValue()
            s = str(v)                  # to be sure
            i = self.list.FindString(s)
            if i == -1:                 # ignore non-unique
                # Find place to insert
                found = False
                for i in range(self.list.GetCount()):
                    if int(self.list.GetString(i)) > v:
                        found = True
                        break
                if found: self.list.InsertItems([s], i)
                else: self.list.Append(s)
        except ValueError:
            wx.LogError('List item is not an int!')
    def OnButtonRemove(self, evt):
        self.list.Delete(self.list.GetSelection())
    def OnButtonClear(self, evt):
        self.list.Clear()
    def OnUpdateUI(self, evt):
        if evt.GetId() == self.ID_BUTTON_REMOVE:
            evt.Enable(self.list.GetSelection() != -1)

# For growable list
class ParamIntList(ParamContent):
    def __init__(self, parent, name):
        ParamContent.__init__(self, parent, name)
    def OnButtonEdit(self, evt):
        if self.textModified:           # text has newer value
            try:
                self.value = map(int, self.text.GetValue().split('|'))
            except ValueError:
                self.value = []
        dlg = IntListDialog(self, self.value)
        if dlg.ShowModal() == wx.ID_OK:
            value = []
            for i in range(dlg.list.GetCount()):
                value.append(int(dlg.list.GetString(i)))
            self.SetValue(value)
            self.SetModified()
            self.textModified = False
        dlg.Destroy()

# Boxless radiobox
class RadioBox(PPanel):
    def __init__(self, parent, id, choices,
                 pos=wx.DefaultPosition, name='radiobox'):
        PPanel.__init__(self, parent, name)
        self.choices = choices
        topSizer = wx.BoxSizer()
        for i in choices:
            button = wx.RadioButton(self, -1, i, size=(-1,buttonSize[1]), name=i)
            topSizer.Add(button, 0, wx.RIGHT, 5)
            wx.EVT_RADIOBUTTON(self, button.GetId(), self.OnRadioChoice)
        self.SetSizer(topSizer)
    def SetStringSelection(self, value):
        self.freeze = True
        for i in self.choices:
            self.FindWindowByName(i).SetValue(i == value)
        self.value = value
        self.freeze = False
    def OnRadioChoice(self, evt):
        if self.freeze: return
        if evt.GetSelection():
            self.value = evt.GetEventObject().GetName()
            self.SetModified()
    def GetStringSelection(self):
        return self.value

class ParamBool(RadioBox):
    values = {'yes': '1', 'no': '0'}
    seulav = {'1': 'yes', '0': 'no'}
    def __init__(self, parent, name):
        RadioBox.__init__(self, parent, -1, choices=self.values.keys(), name=name)
    def GetValue(self):
        return self.values[self.GetStringSelection()]
    def SetValue(self, value):
        if not value: value = '1'
        self.SetStringSelection(self.seulav[value])

class ParamOrient(RadioBox):
    values = {'horizontal': 'wxHORIZONTAL', 'vertical': 'wxVERTICAL'}
    seulav = {'wxHORIZONTAL': 'horizontal', 'wxVERTICAL': 'vertical'}
    def __init__(self, parent, name):
        RadioBox.__init__(self, parent, -1, choices=self.values.keys(), name=name)
    def GetValue(self):
        return self.values[self.GetStringSelection()]
    def SetValue(self, value):
        if not value: value = 'wxHORIZONTAL'
        self.SetStringSelection(self.seulav[value])

class ParamOrientation(RadioBox):
    values = {'horizontal': 'horizontal', 'vertical': 'vertical'}
    seulav = {'horizontal': 'horizontal', 'vertical': 'vertical'}
    def __init__(self, parent, name):
        RadioBox.__init__(self, parent, -1, choices=self.values.keys(), name=name)
    def GetValue(self):
        return self.values[self.GetStringSelection()]
    def SetValue(self, value):
        if not value: value = 'vertical'
        self.SetStringSelection(self.seulav[value])

class ParamFile(PPanel):
    def __init__(self, parent, name):
        PPanel.__init__(self, parent, name)
        self.ID_TEXT_CTRL = wx.NewId()
        self.ID_BUTTON_BROWSE = wx.NewId()
        sizer = wx.BoxSizer()
        self.text = wx.TextCtrl(self, self.ID_TEXT_CTRL, size=wx.Size(200,-1))
        sizer.Add(self.text, 0, wx.RIGHT | wx.ALIGN_CENTER_VERTICAL, 5)
        self.button = wx.Button(self, self.ID_BUTTON_BROWSE, 'Browse...',size=buttonSize)
        sizer.Add(self.button, 0, wx.ALIGN_CENTER_VERTICAL)
        self.SetSizer(sizer)
        self.textModified = False
        wx.EVT_BUTTON(self, self.ID_BUTTON_BROWSE, self.OnButtonBrowse)
        wx.EVT_TEXT(self, self.ID_TEXT_CTRL, self.OnChange)
    def OnChange(self, evt):
        PPanel.OnChange(self, evt)
        self.textModified = True
    def GetValue(self):
        if self.textModified:           # text has newer value
            return self.text.GetValue()
        return self.value
    def SetValue(self, value):
        self.freeze = True
        self.value = value
        self.text.SetValue(value)  # update text ctrl
        self.freeze = False
    def OnButtonBrowse(self, evt):
        if self.textModified:           # text has newer value
            self.value = self.text.GetValue()
        dlg = wx.FileDialog(self,
                           defaultDir = os.path.abspath(os.path.dirname(self.value)),
                           defaultFile = os.path.basename(self.value))
        if dlg.ShowModal() == wx.ID_OK:
            # Get common part of selected path and current
            if g.frame.dataFile:
                curpath = os.path.abspath(g.frame.dataFile)
            else:
                curpath = os.path.join(os.getcwd(), '')
            common = os.path.commonprefix([curpath, dlg.GetPath()])
            self.SetValue(dlg.GetPath()[len(common):])
            self.SetModified()
            self.textModified = False
        dlg.Destroy()

class ParamBitmap(PPanel):
    def __init__(self, parent, name):
        pre = wx.PrePanel()
        g.frame.res.LoadOnPanel(pre, parent, 'PANEL_BITMAP')
        self.PostCreate(pre)
        self.modified = self.freeze = False
        self.radio_std = xrc.XRCCTRL(self, 'RADIO_STD')
        self.radio_file = xrc.XRCCTRL(self, 'RADIO_FILE')
        self.combo = xrc.XRCCTRL(self, 'COMBO_STD')
        self.text = xrc.XRCCTRL(self, 'TEXT_FILE')
        self.button = xrc.XRCCTRL(self, 'BUTTON_BROWSE')
        self.textModified = False
        self.SetAutoLayout(True)
        self.GetSizer().SetMinSize((260, -1))
        self.GetSizer().Fit(self)
        wx.EVT_RADIOBUTTON(self, xrc.XRCID('RADIO_STD'), self.OnRadioStd)
        wx.EVT_RADIOBUTTON(self, xrc.XRCID('RADIO_FILE'), self.OnRadioFile)
        wx.EVT_BUTTON(self, xrc.XRCID('BUTTON_BROWSE'), self.OnButtonBrowse)
        wx.EVT_COMBOBOX(self, xrc.XRCID('COMBO_STD'), self.OnCombo)
        wx.EVT_TEXT(self, xrc.XRCID('COMBO_STD'), self.OnChange)
        wx.EVT_TEXT(self, xrc.XRCID('TEXT_FILE'), self.OnChange)
    def OnRadioStd(self, evt):
        self.SetModified()
        self.SetValue(['wxART_MISSING_IMAGE',''])
    def OnRadioFile(self, evt):
        self.SetModified()
        self.SetValue(['',''])
    def updateRadios(self):
        if self.value[0]:
            self.radio_std.SetValue(True)
            self.radio_file.SetValue(False)
            self.text.Enable(False)
            self.button.Enable(False)
            self.combo.Enable(True)
        else:
            self.radio_std.SetValue(False)
            self.radio_file.SetValue(True)
            self.text.Enable(True)
            self.button.Enable(True)
            self.combo.Enable(False)
    def OnChange(self, evt):
        PPanel.OnChange(self, evt)
        self.textModified = True
    def OnCombo(self, evt):
        PPanel.OnChange(self, evt)
        self.value[0] = self.combo.GetValue()
    def GetValue(self):
        if self.textModified:           # text has newer value
            return [self.combo.GetValue(), self.text.GetValue()]
        return self.value
    def SetValue(self, value):
        self.freeze = True
        if not value:
            self.value = ['', '']
        else:
            self.value = value
        self.combo.SetValue(self.value[0])
        self.text.SetValue(self.value[1])  # update text ctrl
        self.updateRadios()
        self.freeze = False
    def OnButtonBrowse(self, evt):
        if self.textModified:           # text has newer value
            self.value[1] = self.text.GetValue()
        dlg = wx.FileDialog(self,
                           defaultDir = os.path.abspath(os.path.dirname(self.value[1])),
                           defaultFile = os.path.basename(self.value[1]))
        if dlg.ShowModal() == wx.ID_OK:
            # Get common part of selected path and current
            if g.frame.dataFile:
                curpath = os.path.abspath(g.frame.dataFile)
            else:
                curpath = os.path.join(os.getcwd(), '')
            common = os.path.commonprefix([curpath, dlg.GetPath()])
            self.SetValue(['', dlg.GetPath()[len(common):]])
            self.SetModified()
            self.textModified = False
        dlg.Destroy()

paramDict = {
    'flag': ParamFlag,
    'style': ParamStyle, 'exstyle': ParamExStyle,
    'pos': ParamPosSize, 'size': ParamPosSize,
    'cellpos': ParamPosSize, 'cellspan': ParamPosSize,
    'border': ParamUnit, 'cols': ParamIntNN, 'rows': ParamIntNN,
    'vgap': ParamUnit, 'hgap': ParamUnit,
    'checkable': ParamBool, 'checked': ParamBool, 'radio': ParamBool,
    'accel': ParamAccel,
    'label': ParamMultilineText, 'title': ParamText, 'value': ParamText,
    'content': ParamContent, 'selection': ParamIntNN,
    'min': ParamInt, 'max': ParamInt,
    'fg': ParamColour, 'bg': ParamColour, 'font': ParamFont,
    'enabled': ParamBool, 'focused': ParamBool, 'hidden': ParamBool,
    'tooltip': ParamText, 'bitmap': ParamBitmap, 'icon': ParamBitmap,
    'encoding': ParamEncoding, 'borders': ParamUnit,
    'comment': ParamComment
    }
