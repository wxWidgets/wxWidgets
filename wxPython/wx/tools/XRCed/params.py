# Name:         params.py
# Purpose:      Classes for parameter introduction
# Author:       Roman Rolinsky <rolinsky@mema.ucl.ac.be>
# Created:      22.08.2001
# RCS-ID:       $Id$

import string
import os.path
from globals import *
from types import *
from wxPython.xrc import *

genericStyles = [
    'wxCLIP_CHILDREN',
    'wxSIMPLE_BORDER',
    'wxSUNKEN_BORDER',
    'wxDOUBLE_BORDER',
    'wxRAISED_BORDER',
    'wxSTATIC_BORDER',
    'wxNO_BORDER',
    'wxTRANSPARENT_WINDOW',
    'wxWANTS_CHARS',
    'wxNO_FULL_REPAINT_ON_RESIZE',
    'wxFULL_REPAINT_ON_RESIZE',
    'wxWS_EX_BLOCK_EVENTS',
]

buttonSize = (35,-1)    # in dialog units, transformed to pixels in panel ctor

# Class that can properly disable children
class PPanel(wxPanel):
    def __init__(self, parent, name):
        wxPanel.__init__(self, parent, -1, name=name)
        self.SetBackgroundColour(parent.GetBackgroundColour())
        self.SetForegroundColour(parent.GetForegroundColour())
        self.modified = self.freeze = False
    def Enable(self, value):
        # Something strange is going on with enable so we make sure...
        for w in self.GetChildren():
            w.Enable(value)
        wxPanel.Enable(self, value)
    def SetModified(self):
        self.modified = True
        g.panel.SetModified(True)
    # Common method to set modified state
    def OnChange(self, evt):
        if self.freeze: return
        self.SetModified()
        evt.Skip()

class ParamBinaryOr(PPanel):
    def __init__(self, parent, name):
        PPanel.__init__(self, parent, name)
        self.ID_TEXT_CTRL = wxNewId()
        self.ID_BUTTON_CHOICES = wxNewId()
        sizer = wxBoxSizer()
        self.text = wxTextCtrl(self, self.ID_TEXT_CTRL, size=wxSize(200,-1))
        sizer.Add(self.text, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5)
        self.button = wxButton(self, self.ID_BUTTON_CHOICES, 'Edit...', size=buttonSize)
        sizer.Add(self.button, 0, wxALIGN_CENTER_VERTICAL)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)
        EVT_BUTTON(self, self.ID_BUTTON_CHOICES, self.OnButtonChoices)
        EVT_TEXT(self, self.ID_TEXT_CTRL, self.OnChange)
    def GetValue(self):
        return self.text.GetValue()
    def SetValue(self, value):
        self.freeze = True
        self.text.SetValue(value)
        self.freeze = False
    def OnButtonChoices(self, evt):
        dlg = g.frame.res.LoadDialog(self, 'DIALOG_CHOICES')
        listBox = XRCCTRL(dlg, 'CHECK_LIST')
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
        if dlg.ShowModal() == wxID_OK:
            value = []
            for i in range(listBox.GetCount()):
                if listBox.IsChecked(i):
                    value.append(self.values[i])
            # Add ignored flags
            value.extend(ignored)
            if value:
                self.SetValue(reduce(lambda a,b: a+'|'+b, value))
            else:
                self.SetValue('')
            self.SetModified()
        dlg.Destroy()

class ParamFlag(ParamBinaryOr):
    values = ['wxTOP', 'wxBOTTOM', 'wxLEFT', 'wxRIGHT', 'wxALL',
              'wxEXPAND', 'wxGROW', 'wxSHAPED', 'wxALIGN_CENTRE', 'wxALIGN_RIGHT',
              'wxFIXED_MINSIZE',
              'wxALIGN_BOTTOM', 'wxALIGN_CENTRE_VERTICAL',
              'wxALIGN_CENTRE_HORIZONTAL',
              ]
    equal = {'wxALIGN_CENTER': 'wxALIGN_CENTRE',
             'wxALIGN_CENTER_VERTICAL': 'wxALIGN_CENTRE_VERTICAL',
             'wxALIGN_CENTER_HORIZONTAL': 'wxALIGN_CENTRE_HORIZONTAL'}
    def __init__(self, parent, name):
        ParamBinaryOr.__init__(self, parent, name)

class ParamStyle(ParamBinaryOr):
    equal = {'wxALIGN_CENTER': 'wxALIGN_CENTRE'}
    def __init__(self, parent, name):
        self.values = g.currentXXX.winStyles + genericStyles
        ParamBinaryOr.__init__(self, parent, name)

class ParamNonGenericStyle(ParamBinaryOr):
    def __init__(self, parent, name):
        self.values = g.currentXXX.winStyles
        ParamBinaryOr.__init__(self, parent, name)

class ParamExStyle(ParamBinaryOr):
    def __init__(self, parent, name):
        if g.currentXXX:
            self.values = g.currentXXX.exStyles # constant at the moment
        else:
            self.values = []
        ParamBinaryOr.__init__(self, parent, name)

class ParamColour(PPanel):
    def __init__(self, parent, name):
        PPanel.__init__(self, parent, name)
        self.ID_TEXT_CTRL = wxNewId()
        self.ID_BUTTON = wxNewId()
        sizer = wxBoxSizer()
        self.text = wxTextCtrl(self, self.ID_TEXT_CTRL, size=(65,-1))
        sizer.Add(self.text, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5)
        self.button = wxPanel(self, self.ID_BUTTON, wxDefaultPosition, wxSize(20, 1))
        sizer.Add(self.button, 0, wxGROW | wxALIGN_CENTER_VERTICAL)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)
        self.textModified = False
        EVT_PAINT(self.button, self.OnPaintButton)
        EVT_TEXT(self, self.ID_TEXT_CTRL, self.OnChange)
        EVT_LEFT_DOWN(self.button, self.OnLeftDown)
    def GetValue(self):
        return self.text.GetValue()
    def SetValue(self, value):
        self.freeze = True
        if not value: value = '#FFFFFF'
        self.text.SetValue(str(value))  # update text ctrl
        colour = wxColour(int(value[1:3], 16), int(value[3:5], 16), int(value[5:7], 16))
        self.button.SetBackgroundColour(colour)
        self.button.Refresh()
        self.freeze = False
    def OnPaintButton(self, evt):
        dc = wxPaintDC(self.button)
        dc.SetBrush(wxTRANSPARENT_BRUSH)
        if self.IsEnabled(): dc.SetPen(wxBLACK_PEN)
        else: dc.SetPen(wxGREY_PEN)
        size = self.button.GetSize()
        dc.DrawRectangle(0, 0, size.width, size.height)
    def OnLeftDown(self, evt):
        data = wxColourData()
        data.SetColour(self.GetValue())
        dlg = wxColourDialog(self, data)
        if dlg.ShowModal() == wxID_OK:
            self.SetValue('#%02X%02X%02X' % dlg.GetColourData().GetColour().Get())
            self.SetModified()
        dlg.Destroy()

################################################################################

# Mapping from wx constants ro XML strings
fontFamiliesWx2Xml = {wxDEFAULT: 'default', wxDECORATIVE: 'decorative',
                wxROMAN: 'roman', wxSCRIPT: 'script', wxSWISS: 'swiss',
                wxMODERN: 'modern'}
fontStylesWx2Xml = {wxNORMAL: 'normal', wxSLANT: 'slant', wxITALIC: 'italic'}
fontWeightsWx2Xml = {wxNORMAL: 'normal', wxLIGHT: 'light', wxBOLD: 'bold'}
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
        self.ID_TEXT_CTRL = wxNewId()
        self.ID_BUTTON_SELECT = wxNewId()
        sizer = wxBoxSizer()
        self.text = wxTextCtrl(self, self.ID_TEXT_CTRL, size=(200,-1))
        sizer.Add(self.text, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5)
        self.button = wxButton(self, self.ID_BUTTON_SELECT, 'Select...', size=buttonSize)
        sizer.Add(self.button, 0, wxALIGN_CENTER_VERTICAL)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)
        self.textModified = False
        EVT_BUTTON(self, self.ID_BUTTON_SELECT, self.OnButtonSelect)
        EVT_TEXT(self, self.ID_TEXT_CTRL, self.OnChange)
    def OnChange(self, evt):
        PPanel.OnChange(self, evt)
        self.textModified = True
    def _defaultValue(self):
        return ['12', 'default', 'normal', 'normal', '0', '', '']
    def GetValue(self):
        if self.textModified:           # text has newer value
            try:
                return eval(self.text.GetValue())
            except SyntaxError:
                wxLogError('Syntax error in parameter value: ' + self.GetName())
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
                wxLogError('Syntax error in parameter value: ' + self.GetName())
                self.value = self._defaultValue()
        # Make initial font
        # Default values
        size = 12
        family = wxDEFAULT
        style = weight = wxNORMAL
        underlined = 0
        face = ''
        enc = wxFONTENCODING_DEFAULT
        # Fall back to default if exceptions
        error = False
        try:
            try: size = int(self.value[0])
            except ValueError: error = True
            try: family = fontFamiliesXml2wx[self.value[1]]
            except KeyError: error = True
            try: style = fontStylesXml2wx[self.value[2]]
            except KeyError: error = True
            try: weight = fontWeightsXml2wx[self.value[3]]
            except KeyError: error = True
            try: underlined = int(self.value[4])
            except ValueError: error = True
            face = self.value[5]
            mapper = wxFontMapper()
            if not self.value[6]: enc = mapper.CharsetToEncoding(self.value[6])
        except IndexError:
            error = True
        if error: wxLogError('Invalid font specification')
        if enc == wxFONTENCODING_DEFAULT: enc = wxFONTENCODING_SYSTEM
        font = wxFont(size, family, style, weight, underlined, face, enc)
        data = wxFontData()
        data.SetInitialFont(font)
        dlg = wxFontDialog(self, data)
        if dlg.ShowModal() == wxID_OK:
            font = dlg.GetFontData().GetChosenFont()
            value = [str(font.GetPointSize()),
                     fontFamiliesWx2Xml.get(font.GetFamily(), "default"),
                     fontStylesWx2Xml.get(font.GetStyle(), "normal"),
                     fontWeightsWx2Xml.get(font.GetWeight(), "normal"),
                     str(font.GetUnderlined()),
                     font.GetFaceName().encode(),
                     wxFontMapper_GetEncodingName(font.GetEncoding()).encode()
                     ]
            # Add ignored flags
            self.SetValue(value)
            self.SetModified()
            self.textModified = False
        dlg.Destroy()

################################################################################

class ParamInt(PPanel):
    def __init__(self, parent, name):
        PPanel.__init__(self, parent, name)
        self.ID_SPIN_CTRL = wxNewId()
        sizer = wxBoxSizer()
        self.spin = wxSpinCtrl(self, self.ID_SPIN_CTRL, size=(50,-1))
        sizer.Add(self.spin)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)
        EVT_SPINCTRL(self, self.ID_SPIN_CTRL, self.OnChange)
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
        self.ID_TEXT_CTRL = wxNewId()
        self.ID_SPIN_BUTTON = wxNewId()
        sizer = wxBoxSizer()
        self.text = wxTextCtrl(self, self.ID_TEXT_CTRL, size=(35,-1))
        self.spin = wxSpinButton(self, self.ID_SPIN_BUTTON, style = wxSP_VERTICAL, size=(-1,1))
        self.spin.SetRange(-10000, 10000)
        sizer.Add(self.text, 0, wxEXPAND | wxRIGHT, 2)
        sizer.Add(self.spin, 0, wxEXPAND)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)
        EVT_SPIN_UP(self, self.ID_SPIN_BUTTON, self.OnSpinUp)
        EVT_SPIN_DOWN(self, self.ID_SPIN_BUTTON, self.OnSpinDown)
    def GetValue(self):
        return self.text.GetValue()
    def SetValue(self, value):
        self.freeze = True
        if not value: value = '0'
        self.text.SetValue(value)
        self.freeze = False
    def Change(self, x):
        # Check if we are working with dialog units
        value = self.text.GetValue()
        units = ''
        if value[-1].upper() == 'D':
            units = value[-1]
            value = value[:-1]
        try:
            intValue = int(value) + x
            self.spin.SetValue(intValue)
            self.text.SetValue(str(intValue) + units)
            self.SetModified()
        except:
            # !!! Strange, if I use wxLogWarning, event is re-generated
            print 'incorrect unit format'
    def OnSpinUp(self, evt):
        self.Change(1)
    def OnSpinDown(self, evt):
        self.Change(-1)

class ParamMultilineText(PPanel):
    def __init__(self, parent, name, textWidth=-1):
        PPanel.__init__(self, parent, name)
        self.ID_TEXT_CTRL = wxNewId()
        self.ID_BUTTON_EDIT = wxNewId()
        sizer = wxBoxSizer()
        self.text = wxTextCtrl(self, self.ID_TEXT_CTRL, size=wxSize(200,-1))
        sizer.Add(self.text, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5)
        self.button = wxButton(self, self.ID_BUTTON_EDIT, 'Edit...', size=buttonSize)
        sizer.Add(self.button, 0, wxALIGN_CENTER_VERTICAL)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)
        EVT_BUTTON(self, self.ID_BUTTON_EDIT, self.OnButtonEdit)
        EVT_TEXT(self, self.ID_TEXT_CTRL, self.OnChange)
    def GetValue(self):
        return self.text.GetValue()
    def SetValue(self, value):
        self.freeze = True              # disable other handlers
        self.text.SetValue(value)
        self.freeze = False             # disable other handlers
    def OnButtonEdit(self, evt):
        dlg = g.frame.res.LoadDialog(self, 'DIALOG_TEXT')
        textCtrl = XRCCTRL(dlg, 'TEXT')
        textCtrl.SetValue(self.text.GetValue())
        if dlg.ShowModal() == wxID_OK:
            self.text.SetValue(textCtrl.GetValue())
            self.SetModified()
        dlg.Destroy()

class ParamText(PPanel):
    def __init__(self, parent, name, textWidth=-1):
        PPanel.__init__(self, parent, name)
        self.ID_TEXT_CTRL = wxNewId()
        # We use sizer even here to have the same size of text control
        sizer = wxBoxSizer()
        self.text = wxTextCtrl(self, self.ID_TEXT_CTRL, size=wxSize(textWidth,-1))
        if textWidth == -1: option = 1
        else: option = 0
        sizer.Add(self.text, option, wxALIGN_CENTER_VERTICAL)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)
        EVT_TEXT(self, self.ID_TEXT_CTRL, self.OnChange)
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

class ContentDialog(wxDialog):
    def __init__(self, parent, value):
        # Load from resource
	pre = wxPreDialog()
        g.frame.res.LoadOnDialog(pre, parent, 'DIALOG_CONTENT')
        self.this = pre.this
	self._setOORInfo(self)
        self.list = XRCCTRL(self, 'LIST')
        # Set list items
        for v in value:
            self.list.Append(v)
        self.SetAutoLayout(True)
        self.GetSizer().Fit(self)
        # Callbacks
        self.ID_BUTTON_APPEND = XRCID('BUTTON_APPEND')
        self.ID_BUTTON_REMOVE = XRCID('BUTTON_REMOVE')
        self.ID_BUTTON_UP = XRCID('BUTTON_UP')
        self.ID_BUTTON_DOWN = XRCID('BUTTON_DOWN')
        EVT_BUTTON(self, self.ID_BUTTON_UP, self.OnButtonUp)
        EVT_BUTTON(self, self.ID_BUTTON_DOWN, self.OnButtonDown)
        EVT_BUTTON(self, self.ID_BUTTON_APPEND, self.OnButtonAppend)
        EVT_BUTTON(self, self.ID_BUTTON_REMOVE, self.OnButtonRemove)
        EVT_UPDATE_UI(self, self.ID_BUTTON_UP, self.OnUpdateUI)
        EVT_UPDATE_UI(self, self.ID_BUTTON_DOWN, self.OnUpdateUI)
        EVT_UPDATE_UI(self, self.ID_BUTTON_REMOVE, self.OnUpdateUI)
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
        str = wxGetTextFromUser('Enter new item:', 'Append', '', self)
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

class ContentCheckListDialog(wxDialog):
    def __init__(self, parent, value):
	pre = wxPreDialog()
        g.frame.res.LoadOnDialog(pre, parent, 'DIALOG_CONTENT_CHECK_LIST')
        self.this = pre.this
	self._setOORInfo(self)
        self.list = XRCCTRL(self, 'CHECK_LIST')
        # Set list items
        i = 0
        for v,ch in value:
            self.list.Append(v)
            self.list.Check(i, ch)
            i += 1
        self.SetAutoLayout(True)
        self.GetSizer().Fit(self)
        # Callbacks
        self.ID_BUTTON_APPEND = XRCID('BUTTON_APPEND')
        self.ID_BUTTON_REMOVE = XRCID('BUTTON_REMOVE')
        self.ID_BUTTON_UP = XRCID('BUTTON_UP')
        self.ID_BUTTON_DOWN = XRCID('BUTTON_DOWN')
        EVT_CHECKLISTBOX(self, self.list.GetId(), self.OnCheck)
        EVT_BUTTON(self, self.ID_BUTTON_UP, self.OnButtonUp)
        EVT_BUTTON(self, self.ID_BUTTON_DOWN, self.OnButtonDown)
        EVT_BUTTON(self, self.ID_BUTTON_APPEND, self.OnButtonAppend)
        EVT_BUTTON(self, self.ID_BUTTON_REMOVE, self.OnButtonRemove)
        EVT_UPDATE_UI(self, self.ID_BUTTON_UP, self.OnUpdateUI)
        EVT_UPDATE_UI(self, self.ID_BUTTON_DOWN, self.OnUpdateUI)
        EVT_UPDATE_UI(self, self.ID_BUTTON_REMOVE, self.OnUpdateUI)
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
        str = wxGetTextFromUser('Enter new item:', 'Append', '', self)
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
        self.ID_TEXT_CTRL = wxNewId()
        self.ID_BUTTON_EDIT = wxNewId()
        sizer = wxBoxSizer()
        self.text = wxTextCtrl(self, self.ID_TEXT_CTRL, size=wxSize(200,-1))
        sizer.Add(self.text, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5)
        self.button = wxButton(self, self.ID_BUTTON_EDIT, 'Edit...', size=buttonSize)
        sizer.Add(self.button, 0, wxALIGN_CENTER_VERTICAL)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)
        self.textModified = False
        EVT_BUTTON(self, self.ID_BUTTON_EDIT, self.OnButtonEdit)
        EVT_TEXT(self, self.ID_TEXT_CTRL, self.OnChange)
    def OnChange(self, evt):
        PPanel.OnChange(self, evt)
        self.textModified = True
    def GetValue(self):
        if self.textModified:           # text has newer value
            try:
                return eval(self.text.GetValue())
            except SyntaxError:
                wxLogError('Syntax error in parameter value: ' + self.GetName())
                return []
        return self.value
    def SetValue(self, value):
        self.freeze = True
        if not value: value = []
        self.value = value
        self.text.SetValue(str(value))  # update text ctrl
        self.freeze = False
    def OnButtonEdit(self, evt):
        if self.textModified:           # text has newer value
            try:
                self.value = eval(self.text.GetValue())
            except SyntaxError:
                wxLogError('Syntax error in parameter value: ' + self.GetName())
                self.value = []
        dlg = ContentDialog(self, self.value)
        if dlg.ShowModal() == wxID_OK:
            value = []
            for i in range(dlg.list.GetCount()):
                value.append(dlg.list.GetString(i))
            # Add ignored flags
            self.SetValue(value)
            self.SetModified()
            self.textModified = False
        dlg.Destroy()

# CheckList content
class ParamContentCheckList(ParamContent):
    def __init__(self, parent, name):
        ParamContent.__init__(self, parent, name)
    def OnButtonEdit(self, evt):
        if self.textModified:           # text has newer value
            try:
                self.value = eval(self.text.GetValue())
            except SyntaxError:
                wxLogError('Syntax error in parameter value: ' + self.GetName())
                self.value = []
        dlg = ContentCheckListDialog(self, self.value)
        if dlg.ShowModal() == wxID_OK:
            value = []
            for i in range(dlg.list.GetCount()):
                value.append((dlg.list.GetString(i), dlg.list.IsChecked(i)))
            # Add ignored flags
            self.SetValue(value)
            self.SetModified()
            self.textModified = False
        dlg.Destroy()

class IntListDialog(wxDialog):
    def __init__(self, parent, value):
	pre = wxPreDialog()
        g.frame.res.LoadOnDialog(pre, parent, 'DIALOG_INTLIST')
        self.this = pre.this
	self._setOORInfo(self)
        self.list = XRCCTRL(self, 'LIST')
        # Set list items
        value.sort()
        for v in value:
            if type(v) != IntType:
                wxLogError('Invalid item type')
            else:
                self.list.Append(str(v))
        self.SetAutoLayout(True)
        self.GetSizer().Fit(self)
        # Callbacks
        self.ID_BUTTON_ADD = XRCID('BUTTON_ADD')
        self.ID_BUTTON_REMOVE = XRCID('BUTTON_REMOVE')
        EVT_BUTTON(self, self.ID_BUTTON_ADD, self.OnButtonAppend)
        EVT_BUTTON(self, self.ID_BUTTON_REMOVE, self.OnButtonRemove)
        EVT_UPDATE_UI(self, self.ID_BUTTON_REMOVE, self.OnUpdateUI)
    def OnButtonAppend(self, evt):
        s = wxGetTextFromUser('Enter new number:', 'Add', '', self)
        if not s: return
        # Check that it's unique
        try:
            v = int(s)
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
            wxLogError('List item is not an int!')
    def OnButtonRemove(self, evt):
        self.list.Delete(self.list.GetSelection())
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
                self.value = eval(self.text.GetValue())
            except SyntaxError:
                wxLogError('Syntax error in parameter value: ' + self.GetName())
                self.value = []
        dlg = IntListDialog(self, self.value)
        if dlg.ShowModal() == wxID_OK:
            value = []
            for i in range(dlg.list.GetCount()):
                value.append(int(dlg.list.GetString(i)))
            # Add ignored flags
            self.SetValue(value)
            self.SetModified()
            self.textModified = False
        dlg.Destroy()

# Boxless radiobox
class RadioBox(PPanel):
    def __init__(self, parent, id, choices,
                 pos=wxDefaultPosition, name='radiobox'):
        PPanel.__init__(self, parent, name)
        self.choices = choices
        topSizer = wxBoxSizer()
        for i in choices:
            button = wxRadioButton(self, -1, i, size=(-1,buttonSize[1]), name=i)
            topSizer.Add(button, 0, wxRIGHT, 5)
            EVT_RADIOBUTTON(self, button.GetId(), self.OnRadioChoice)
        self.SetAutoLayout(True)
        self.SetSizer(topSizer)
        topSizer.Fit(self)
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
        self.ID_TEXT_CTRL = wxNewId()
        self.ID_BUTTON_BROWSE = wxNewId()
        sizer = wxBoxSizer()
        self.text = wxTextCtrl(self, self.ID_TEXT_CTRL, size=wxSize(200,-1))
        sizer.Add(self.text, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5)
        self.button = wxButton(self, self.ID_BUTTON_BROWSE, 'Browse...',size=buttonSize)
        sizer.Add(self.button, 0, wxALIGN_CENTER_VERTICAL)
        self.SetAutoLayout(True)
        self.SetSizer(sizer)
        sizer.Fit(self)
        self.textModified = False
        EVT_BUTTON(self, self.ID_BUTTON_BROWSE, self.OnButtonBrowse)
        EVT_TEXT(self, self.ID_TEXT_CTRL, self.OnChange)
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
        dlg = wxFileDialog(self,
                           defaultDir = os.path.abspath(os.path.dirname(self.value)),
                           defaultFile = os.path.basename(self.value))
        if dlg.ShowModal() == wxID_OK:
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
	pre = wxPrePanel()
        g.frame.res.LoadOnPanel(pre, parent, 'PANEL_BITMAP')
        self.this = pre.this
	self._setOORInfo(self)
        self.SetBackgroundColour(parent.GetBackgroundColour())
        self.SetForegroundColour(parent.GetForegroundColour())
        self.modified = self.freeze = False
        self.radio_std = XRCCTRL(self, 'RADIO_STD')
        self.radio_std.SetBackgroundColour(parent.GetBackgroundColour())
        self.radio_std.SetForegroundColour(parent.GetForegroundColour())
        self.radio_file = XRCCTRL(self, 'RADIO_FILE')
        self.radio_file.SetBackgroundColour(parent.GetBackgroundColour())
        self.radio_file.SetForegroundColour(parent.GetForegroundColour())
        self.combo = XRCCTRL(self, 'COMBO_STD')
        self.text = XRCCTRL(self, 'TEXT_FILE')
        self.button = XRCCTRL(self, 'BUTTON_BROWSE')
        self.textModified = False
        self.SetAutoLayout(True)
        self.GetSizer().SetMinSize((260, -1))
        self.GetSizer().Fit(self)
        EVT_RADIOBUTTON(self, XRCID('RADIO_STD'), self.OnRadioStd)
        EVT_RADIOBUTTON(self, XRCID('RADIO_FILE'), self.OnRadioFile)
        EVT_BUTTON(self, XRCID('BUTTON_BROWSE'), self.OnButtonBrowse)
        EVT_COMBOBOX(self, XRCID('COMBO_STD'), self.OnCombo)
        EVT_TEXT(self, XRCID('COMBO_STD'), self.OnChange)
        EVT_TEXT(self, XRCID('TEXT_FILE'), self.OnChange)
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
        dlg = wxFileDialog(self,
                           defaultDir = os.path.abspath(os.path.dirname(self.value[1])),
                           defaultFile = os.path.basename(self.value[1]))
        if dlg.ShowModal() == wxID_OK:
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
    'border': ParamUnit, 'cols': ParamInt, 'rows': ParamInt,
    'vgap': ParamUnit, 'hgap': ParamUnit,
    'checkable': ParamBool, 'checked': ParamBool, 'radio': ParamBool,
    'accel': ParamAccel,
    'label': ParamMultilineText, 'title': ParamText, 'value': ParamText,
    'content': ParamContent, 'selection': ParamInt,
    'min': ParamInt, 'max': ParamInt,
    'fg': ParamColour, 'bg': ParamColour, 'font': ParamFont,
    'enabled': ParamBool, 'focused': ParamBool, 'hidden': ParamBool,
    'tooltip': ParamText, 'bitmap': ParamBitmap, 'icon': ParamBitmap,
    'encoding': ParamEncoding
    }
