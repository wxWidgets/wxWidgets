# Name:         params.py
# Purpose:      Classes for parameter introduction
# Author:       Roman Rolinsky <rolinsky@mema.ucl.ac.be>
# Created:      22.08.2001
# RCS-ID:       $Id$

from wxPython.wx import *
from wxPython.xrc import *
import string
import os.path
from types import *

# Object which is currently processed
currentXXX = None
def SetCurrentXXX(xxx):
    global currentXXX
    currentXXX = xxx

genericStyles = ['wxSIMPLE_BORDER', 'wxDOUBLE_BORDER',
                 'wxSUNKEN_BORDER', 'wxRAISED_BORDER',
                 'wxSTATIC_BORDER', 'wxNO_BORDER',
                 'wxTRANSPARENT_WINDOW', 'wxWANTS_CHARS',
                 'wxNO_FULL_REPAINT_ON_RESIZE']

buttonSize = (55,-1)

# Class that can properly disable children
class PPanel(wxPanel):
    def __init__(self, parent, name):
        wxPanel.__init__(self, parent, -1, name=name)
        self.modified = self.freeze = false
    def Enable(self, value):
        # Something strange is going on with enable so we make sure...
        for w in self.GetChildren():
            w.Enable(value)
        wxPanel.Enable(self, value)
    def SetModified(self):
        self.modified = true
        panel.SetModified(true)

class ParamBinaryOr(PPanel):
    def __init__(self, parent, name):
        PPanel.__init__(self, parent, name)
        self.ID_TEXT_CTRL = wxNewId()
        self.ID_BUTTON_CHOICES = wxNewId()
        self.SetBackgroundColour(panel.GetBackgroundColour())
        sizer = wxBoxSizer()
        self.text = wxTextCtrl(self, self.ID_TEXT_CTRL, size=wxSize(200,-1))
        sizer.Add(self.text, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5)
        self.button = wxButton(self, self.ID_BUTTON_CHOICES, 'Edit...', size=buttonSize)
        sizer.Add(self.button, 0, wxALIGN_CENTER_VERTICAL)
        self.SetAutoLayout(true)
        self.SetSizer(sizer)
        sizer.Fit(self)
        EVT_BUTTON(self, self.ID_BUTTON_CHOICES, self.OnButtonChoices)
        EVT_TEXT(self, self.ID_TEXT_CTRL, self.OnChange)
    def OnChange(self, evt):
        if self.freeze: return
        self.SetModified()
        evt.Skip()
    def GetValue(self):
        return self.text.GetValue()
    def SetValue(self, value):
        self.freeze = true
        self.text.SetValue(value)
        self.freeze = false
    def OnButtonChoices(self, evt):
        dlg = wxDialog(self, -1, 'Choices')
        topSizer = wxBoxSizer(wxVERTICAL)
        listBox = wxCheckListBox(dlg, -1, choices=self.values, size=(250,200))
        value = map(string.strip, string.split(self.text.GetValue(), '|'))
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
        topSizer.Add(listBox, 1, wxEXPAND)
        sizer = wxBoxSizer()
        buttonOk = wxButton(dlg, wxID_OK, 'OK')
        buttonOk.SetDefault()
        sizer.Add(buttonOk, 0, wxRIGHT, 10)
        sizer.Add(0, 0, 1)
        sizer.Add(wxButton(dlg, wxID_CANCEL, 'Cancel'))
        topSizer.Add(sizer, 0, wxALL | wxEXPAND, 10)
        dlg.SetAutoLayout(true)
        dlg.SetSizer(topSizer)
        topSizer.Fit(dlg)
        dlg.Center()
        if dlg.ShowModal() == wxID_OK: 
            value = []
            for i in range(listBox.Number()):
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
              'wxEXPAND', 'wxSHAPED', 'wxALIGN_CENTRE', 'wxALIGN_RIGHT',
              'wxALIGN_BOTTOM', 'wxALIGN_CENTRE_VERTICAL',
              'wxALIGN_CENTRE_HORIZONTAL']
    equal = {'wxALIGN_CENTER': 'wxALIGN_CENTRE',
             'wxALIGN_CENTER_VERTICAL': 'wxALIGN_CENTRE_VERTICAL',
             'wxALIGN_CENTER_HORIZONTAL': 'wxALIGN_CENTRE_HORIZONTAL'}
    def __init__(self, parent, name):
        ParamBinaryOr.__init__(self, parent, name)

class ParamStyle(ParamBinaryOr):
    equal = {'wxALIGN_CENTER': 'wxALIGN_CENTRE'}
    def __init__(self, parent, name):
        self.values = currentXXX.winStyles + genericStyles
        ParamBinaryOr.__init__(self, parent, name)

class ParamNonGenericStyle(ParamBinaryOr):
    def __init__(self, parent, name):
        self.values = currentXXX.winStyles
        ParamBinaryOr.__init__(self, parent, name)

class ParamExStyle(ParamBinaryOr):
    def __init__(self, parent, name):
        if currentXXX:
            self.values = currentXXX.exStyles # constant at the moment
        else:
            self.values = []
        ParamBinaryOr.__init__(self, parent, name)

class ParamColour(PPanel):
    def __init__(self, parent, name):
        PPanel.__init__(self, parent, name)
        self.ID_TEXT_CTRL = wxNewId()
        self.ID_BUTTON = wxNewId()
        self.SetBackgroundColour(panel.GetBackgroundColour())
        sizer = wxBoxSizer()
        self.text = wxTextCtrl(self, self.ID_TEXT_CTRL, size=buttonSize)
        sizer.Add(self.text, 0, wxRIGHT, 5)
        self.button = wxPanel(self, self.ID_BUTTON, wxDefaultPosition, wxSize(40, -1))
        sizer.Add(self.button, 0, wxGROW)
        self.SetAutoLayout(true)
        self.SetSizer(sizer)
        sizer.Fit(self)
        self.textModified = false
        EVT_PAINT(self.button, self.OnPaintButton)
        EVT_TEXT(self, self.ID_TEXT_CTRL, self.OnChange)
        EVT_LEFT_DOWN(self.button, self.OnLeftDown)
    def GetValue(self):
        return self.value
    def SetValue(self, value):
        self.freeze = true
        if not value: value = '#FFFFFF'
        self.value = value
        self.text.SetValue(str(value))  # update text ctrl
        colour = wxColour(int(value[1:3], 16), int(value[3:5], 16), int(value[5:7], 16))
        self.button.SetBackgroundColour(colour)
        self.button.Refresh()
        self.freeze = false
    def OnChange(self, evt):
        if self.freeze: return
        self.SetModified()
        self.textModified = true
        evt.Skip()
    def OnPaintButton(self, evt):
        dc = wxPaintDC(self.button)
        dc.SetBrush(wxTRANSPARENT_BRUSH)
        if self.IsEnabled(): dc.SetPen(wxBLACK_PEN)
        else: dc.SetPen(wxGREY_PEN)
        size = self.button.GetSize()
        dc.DrawRectangle(0, 0, size.x, size.y)
    def OnLeftDown(self, evt):
        dlg = wxColourDialog(self)
        if dlg.ShowModal() == wxID_OK:
            self.SetValue('#%02X%02X%02X' % dlg.GetColourData().GetColour().Get())
            self.SetModified()
            self.textModified = false
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
        self.SetBackgroundColour(panel.GetBackgroundColour())
        sizer = wxBoxSizer()
        self.text = wxTextCtrl(self, self.ID_TEXT_CTRL, size=wxSize(200,-1))
        sizer.Add(self.text, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5)
        self.button = wxButton(self, self.ID_BUTTON_SELECT, 'Select...', size=buttonSize)
        sizer.Add(self.button, 0, wxALIGN_CENTER_VERTICAL)
        self.SetAutoLayout(true)
        self.SetSizer(sizer)
        sizer.Fit(self)
        self.textModified = false
        EVT_BUTTON(self, self.ID_BUTTON_SELECT, self.OnButtonSelect)
        EVT_TEXT(self, self.ID_TEXT_CTRL, self.OnChange)
    def OnChange(self, evt):
        if self.freeze: return
        self.SetModified()
        self.textModified = true
        evt.Skip()
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
        self.freeze = true              # disable other handlers
        if not value: value = self._defaultValue()
        self.value = value
        self.text.SetValue(str(value))  # update text ctrl
        self.freeze = false
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
        error = false
        try:
            try: size = int(self.value[0])
            except ValueError: error = true
            try: family = fontFamiliesXml2wx[self.value[1]]
            except KeyError: error = true
            try: style = fontStylesXml2wx[self.value[2]]
            except KeyError: error = true
            try: weight = fontWeightsXml2wx[self.value[3]]
            except KeyError: error = true
            try: underlined = int(self.value[4])
            except ValueError: error = true
            face = self.value[5]
            mapper = wxFontMapper()
            if not self.value[6]: enc = mapper.CharsetToEncoding(self.value[6])
        except IndexError:
            error = true
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
                     font.GetFaceName(),
                     wxFontMapper_GetEncodingName(font.GetEncoding())
                     ]
            # Add ignored flags
            self.SetValue(value)
            self.SetModified()
            self.textModified = false
        dlg.Destroy()

################################################################################

class ParamInt(PPanel):
    def __init__(self, parent, name):
        PPanel.__init__(self, parent, name)
        self.ID_SPIN_CTRL = wxNewId()
        sizer = wxBoxSizer()
        self.spin = wxSpinCtrl(self, self.ID_SPIN_CTRL, size=wxSize(50,-1))
        self.SetBackgroundColour(panel.GetBackgroundColour())
        sizer.Add(self.spin)
        self.SetAutoLayout(true)
        self.SetSizer(sizer)
        sizer.Fit(self)
        EVT_SPINCTRL(self, self.ID_SPIN_CTRL, self.OnChange)
    def GetValue(self):
        return str(self.spin.GetValue())
    def SetValue(self, value):
        self.freeze = true
        if not value: value = 0
        self.spin.SetValue(int(value))
        self.freeze = false
    def OnChange(self, evt):
        if self.freeze: return
        self.SetModified()
        evt.Skip()

class ParamText(PPanel):
    def __init__(self, parent, name, textWidth=200):
        PPanel.__init__(self, parent, name)
        self.ID_TEXT_CTRL = wxNewId()
        # We use sizer even here to have the same size of text control
        sizer = wxBoxSizer()
        self.SetBackgroundColour(panel.GetBackgroundColour())
        self.text = wxTextCtrl(self, self.ID_TEXT_CTRL, size=wxSize(textWidth,-1))
        sizer.Add(self.text, 0, wxALIGN_CENTER_VERTICAL)
        self.SetAutoLayout(true)
        self.SetSizer(sizer)
        sizer.Fit(self)
        EVT_TEXT(self, self.ID_TEXT_CTRL, self.OnChange)
    def GetValue(self):
        return self.text.GetValue()
    def SetValue(self, value):
        self.freeze = true              # disable other handlers
        self.text.SetValue(value)
        self.freeze = false             # disable other handlers
    def OnChange(self, evt):
        if self.freeze: return
        self.SetModified()
        evt.Skip()

class ParamAccel(ParamText):
    def __init__(self, parent, name):
        ParamText.__init__(self, parent, name, 50)

class ParamPosSize(ParamText):
    def __init__(self, parent, name):
        ParamText.__init__(self, parent, name, 80)

class ContentDialog(wxDialogPtr):
    def __init__(self, parent, value):
        # Is this normal???
        w = frame.res.LoadDialog(parent, 'DIALOG_CONTENT')
        wxDialogPtr.__init__(self, w.this)
        self.thisown = 1
        self.Center()
        self.list = self.FindWindowByName('LIST')
        # Set list items
        for v in value:
            self.list.Append(v)
        self.SetAutoLayout(true)
        self.GetSizer().Fit(self)
        # Callbacks
        self.ID_BUTTON_APPEND = XMLID('BUTTON_APPEND')
        self.ID_BUTTON_REMOVE = XMLID('BUTTON_REMOVE')
        self.ID_BUTTON_UP = XMLID('BUTTON_UP')
        self.ID_BUTTON_DOWN = XMLID('BUTTON_DOWN')
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
                       self.list.GetSelection() < self.list.Number() - 1)

class ContentCheckListDialog(wxDialogPtr):
    def __init__(self, parent, value):
        # Is this normal???
        w = frame.res.LoadDialog(parent, 'DIALOG_CONTENT_CHECK_LIST')
        wxDialogPtr.__init__(self, w.this)
        self.thisown = 1
        self.Center()
        self.list = self.FindWindowByName('CHECK_LIST')
        # Set list items
        i = 0
        for v,ch in value:
            self.list.Append(v)
            self.list.Check(i, ch)
            i += 1
        self.SetAutoLayout(true)
        self.GetSizer().Fit(self)
        # Callbacks
        self.ID_BUTTON_APPEND = XMLID('BUTTON_APPEND')
        self.ID_BUTTON_REMOVE = XMLID('BUTTON_REMOVE')
        self.ID_BUTTON_UP = XMLID('BUTTON_UP')
        self.ID_BUTTON_DOWN = XMLID('BUTTON_DOWN')
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
                       self.list.GetSelection() < self.list.Number() - 1)

class ParamContent(PPanel):
    def __init__(self, parent, name):
        PPanel.__init__(self, parent, name)
        self.ID_TEXT_CTRL = wxNewId()
        self.ID_BUTTON_EDIT = wxNewId()
        self.SetBackgroundColour(panel.GetBackgroundColour())
        sizer = wxBoxSizer()
        self.text = wxTextCtrl(self, self.ID_TEXT_CTRL, size=wxSize(200,-1))
        sizer.Add(self.text, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5)
        self.button = wxButton(self, self.ID_BUTTON_EDIT, 'Edit...', size=buttonSize)
        sizer.Add(self.button, 0, wxALIGN_CENTER_VERTICAL)
        self.SetAutoLayout(true)
        self.SetSizer(sizer)
        sizer.Fit(self)
        self.textModified = false
        EVT_BUTTON(self, self.ID_BUTTON_EDIT, self.OnButtonEdit)
        EVT_TEXT(self, self.ID_TEXT_CTRL, self.OnChange)
    def OnChange(self, evt):
        if self.freeze: return
        self.SetModified()
        self.textModified = true
        evt.Skip()
    def GetValue(self):
        if self.textModified:           # text has newer value
            try:
                return eval(self.text.GetValue())
            except SyntaxError:
                wxLogError('Syntax error in parameter value: ' + self.GetName())
                return []
        return self.value
    def SetValue(self, value):
        self.freeze = true
        if not value: value = []
        self.value = value
        self.text.SetValue(str(value))  # update text ctrl
        self.freeze = false
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
            for i in range(dlg.list.Number()):
                value.append(dlg.list.GetString(i))
            # Add ignored flags
            self.SetValue(value)
            self.SetModified()
            self.textModified = false
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
            for i in range(dlg.list.Number()):
                value.append((dlg.list.GetString(i), dlg.list.IsChecked(i)))
            # Add ignored flags
            self.SetValue(value)
            self.SetModified()
            self.textModified = false
        dlg.Destroy()

class IntListDialog(wxDialogPtr):
    def __init__(self, parent, value):
        # Is this normal???
        w = frame.res.LoadDialog(parent, 'DIALOG_INTLIST')
        wxDialogPtr.__init__(self, w.this)
        self.thisown = 1
        self.Center()
        self.list = self.FindWindowByName('LIST')
        # Set list items
        value.sort()
        for v in value:
            if type(v) != IntType:
                wxLogError('Invalid item type')
            else:
                self.list.Append(str(v))
        self.SetAutoLayout(true)
        self.GetSizer().Fit(self)
        # Callbacks
        self.ID_BUTTON_ADD = XMLID('BUTTON_ADD')
        self.ID_BUTTON_REMOVE = XMLID('BUTTON_REMOVE')
        EVT_BUTTON(self, self.ID_BUTTON_ADD, self.OnButtonAppend)
        EVT_BUTTON(self, self.ID_BUTTON_REMOVE, self.OnButtonRemove)
        EVT_UPDATE_UI(self, self.ID_BUTTON_REMOVE, self.OnUpdateUI)
    def OnButtonAppend(self, evt):
        s = wxGetTextFromUser('Enter new number:', 'Add', '', self)
        # Check that it's unique
        try:
            v = int(s)
            s = str(v)                  # to be sure
            i = self.list.FindString(s)
            if i == -1:                 # ignore non-unique
                # Find place to insert
                found = false
                for i in range(self.list.Number()):
                    if int(self.list.GetString(i)) > v:
                        found = true
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
            for i in range(dlg.list.Number()):
                value.append(int(dlg.list.GetString(i)))
            # Add ignored flags
            self.SetValue(value)
            self.SetModified()
            self.textModified = false
        dlg.Destroy()

# Boxless radiobox 
class RadioBox(PPanel):
    def __init__(self, parent, id, choices,
                 pos=wxDefaultPosition, name='radiobox'):
        PPanel.__init__(self, parent, name)
        self.SetBackgroundColour(panel.GetBackgroundColour())
        self.choices = choices
        topSizer = wxBoxSizer()
        for i in choices:
            button = wxRadioButton(self, -1, i, name=i)
            topSizer.Add(button)
            EVT_RADIOBUTTON(self, button.GetId(), self.OnRadioChoice)
        self.SetAutoLayout(true)
        self.SetSizer(topSizer)
        topSizer.Fit(self)
    def SetStringSelection(self, value):
        self.freeze = true
        for i in self.choices:
            self.FindWindowByName(i).SetValue(i == value)
        self.value = value
        self.freeze = false
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
        RadioBox.__init__(self, parent, -1, choices = self.values.keys(), name=name)
    def GetValue(self):
        return self.values[self.GetStringSelection()]
    def SetValue(self, value):
        if not value: value = '1'
        self.SetStringSelection(self.seulav[value])

class ParamOrient(RadioBox):
    values = {'horizontal': 'wxHORIZONTAL', 'vertical': 'wxVERTICAL'}
    seulav = {'wxHORIZONTAL': 'horizontal', 'wxVERTICAL': 'vertical'}
    def __init__(self, parent, name):
        RadioBox.__init__(self, parent, -1, choices = self.values.keys(), name=name)
    def GetValue(self):
        return self.values[self.GetStringSelection()]
    def SetValue(self, value):
        if not value: value = 'wxHORIZONTAL'
        self.SetStringSelection(self.seulav[value])

class ParamFile(PPanel):
    def __init__(self, parent, name):    
        PPanel.__init__(self, parent, name)
        self.ID_TEXT_CTRL = wxNewId()
        self.ID_BUTTON_BROWSE = wxNewId()
        self.SetBackgroundColour(panel.GetBackgroundColour())
        sizer = wxBoxSizer()
        self.text = wxTextCtrl(self, self.ID_TEXT_CTRL, size=wxSize(200,-1))
        sizer.Add(self.text, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5)
        self.button = wxButton(self, self.ID_BUTTON_BROWSE, 'Browse...',size=buttonSize)
        sizer.Add(self.button, 0, wxALIGN_CENTER_VERTICAL)
        self.SetAutoLayout(true)
        self.SetSizer(sizer)
        sizer.Fit(self)
        self.textModified = false
        EVT_BUTTON(self, self.ID_BUTTON_BROWSE, self.OnButtonBrowse)
        EVT_TEXT(self, self.ID_TEXT_CTRL, self.OnChange)
    def OnChange(self, evt):
        if self.freeze: return        
        self.SetModified()
        self.textModified = true
        evt.Skip()
    def GetValue(self):
        if self.textModified:           # text has newer value
            return self.text.GetValue()
        return self.value
    def SetValue(self, value):
        self.freeze = true
        self.value = value
        self.text.SetValue(value)  # update text ctrl
        self.freeze = false
    def OnButtonBrowse(self, evt):
        if self.textModified:           # text has newer value
            self.value = self.text.GetValue()
        dlg = wxFileDialog(self,
                           defaultDir = os.path.dirname(self.value),
                           defaultFile = os.path.basename(self.value))
        if dlg.ShowModal() == wxID_OK:
            # Make relative
            common = os.path.commonprefix([os.path.abspath(frame.dataFile),
                                           dlg.GetPath()])
            self.SetValue(dlg.GetPath()[len(common):])
            self.SetModified()
            self.textModified = false
        dlg.Destroy()

paramDict = {
    'flag': ParamFlag,
    'style': ParamStyle, 'exstyle': ParamExStyle,
    'pos': ParamPosSize, 'size': ParamPosSize,
    'border': ParamInt, 'cols': ParamInt, 'rows': ParamInt,
    'vgap': ParamInt, 'hgap': ParamInt,
    'checkable': ParamBool, 'accel': ParamAccel,
    'label': ParamText, 'title': ParamText, 'value': ParamText,
    'content': ParamContent, 'selection': ParamInt,
    'min': ParamInt, 'max': ParamInt,
    'fg': ParamColour, 'bg': ParamColour, 'font': ParamFont,
    'enabled': ParamBool, 'focused': ParamBool, 'hidden': ParamBool,
    'tooltip': ParamText, 'bitmap': ParamFile, 'icon': ParamFile,
    }

