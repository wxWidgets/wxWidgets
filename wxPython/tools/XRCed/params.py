# Name:         params.py
# Purpose:      Classes for parameter introduction
# Author:       Roman Rolinsky <rolinsky@mema.ucl.ac.be>
# Created:      22.08.2001
# RCS-ID:       $Id$

from wxPython.wx import *
from wxPython.xrc import *
import string
import os.path

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

# Class that can properly disable children
class PPanel(wxPanel):
    def __init__(self, parent, id, name):
        wxPanel.__init__(self, parent, id, name=name)
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
    def __init__(self, parent, id, size, name):
        PPanel.__init__(self, parent, id, name)
        self.ID_TEXT_CTRL = wxNewId()
        self.ID_BUTTON_CHOICES = wxNewId()
        self.SetBackgroundColour(panel.GetBackgroundColour())
        sizer = wxBoxSizer()
        self.text = wxTextCtrl(self, self.ID_TEXT_CTRL, size=wxSize(200,-1))
        sizer.Add(self.text, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10)
        self.button = wxButton(self, self.ID_BUTTON_CHOICES, 'Edit...')
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
            self.SetValue(reduce(lambda a,b: a+'|'+b, value))
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
    def __init__(self, parent, id, size, name):
        ParamBinaryOr.__init__(self, parent, id, size, name)

class ParamStyle(ParamBinaryOr):
    equal = {'wxALIGN_CENTER': 'wxALIGN_CENTRE'}
    def __init__(self, parent, id = -1, size = wxDefaultSize, name = ''):
        self.values = currentXXX.winStyles + genericStyles
        ParamBinaryOr.__init__(self, parent, id, size, name)

class ParamNonGenericStyle(ParamBinaryOr):
    def __init__(self, parent, id = -1, size = wxDefaultSize, name = ''):
        self.values = currentXXX.winStyles
        ParamBinaryOr.__init__(self, parent, id, size, name)

class ParamExStyle(ParamBinaryOr):
    def __init__(self, parent, id = -1, size = wxDefaultSize, name = ''):
        if currentXXX:
            self.values = currentXXX.exStyles # constant at the moment
        else:
            self.values = []
        ParamBinaryOr.__init__(self, parent, id, size, name)

class ParamColour(PPanel):
    def __init__(self, parent, id = -1, size = wxDefaultSize, name = ''):
        PPanel.__init__(self, parent, id, name)
        self.ID_TEXT_CTRL = wxNewId()
        self.ID_BUTTON = wxNewId()
        self.SetBackgroundColour(panel.GetBackgroundColour())
        sizer = wxBoxSizer()
        self.text = wxTextCtrl(self, self.ID_TEXT_CTRL, size=wxSize(60,-1))
        sizer.Add(self.text, 0, wxRIGHT, 10)
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
    def __init__(self, parent, id = -1, size = wxDefaultSize, name = ''):
        PPanel.__init__(self, parent, id, name)
        self.ID_TEXT_CTRL = wxNewId()
        self.ID_BUTTON_SELECT = wxNewId()
        self.SetBackgroundColour(panel.GetBackgroundColour())
        sizer = wxBoxSizer()
        self.text = wxTextCtrl(self, self.ID_TEXT_CTRL, size=wxSize(200,-1))
        sizer.Add(self.text, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10)
        self.button = wxButton(self, self.ID_BUTTON_SELECT, 'Select...')
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
    def GetValue(self):
        if self.textModified:           # text has newer value
            return eval(self.text.GetValue())
        return self.value
    def SetValue(self, value):
        self.freeze = true              # disable other handlers
        if not value: value = [''] * 7
        self.value = value
        self.text.SetValue(str(value))  # update text ctrl
        self.freeze = false
    def OnButtonSelect(self, evt):
        if self.textModified:           # text has newer value
            self.value = eval(self.text.GetValue())
        # Make initial font
        try: size = int(self.value[0])
        except ValueError: size = 12
        try: family = fontFamiliesXml2wx[self.value[1]]
        except KeyError: family = wxDEFAULT
        try: style = fontStylesXml2wx[self.value[2]]
        except KeyError: style = wxNORMAL
        try: weight = fontWeightsXml2wx[self.value[3]]
        except KeyError: weight = wxNORMAL
        try: underlined = int(self.value[4])
        except ValueError: underlined = 0
        face = self.value[5]
        mapper = wxFontMapper()
        enc = wxFONTENCODING_DEFAULT
        if not self.value[6]: enc = mapper.CharsetToEncoding(self.value[6])
        if enc == wxFONTENCODING_DEFAULT: enc = wxFONTENCODING_SYSTEM
        font = wxFont(size, family, style, weight, underlined, face, enc)
        data = wxFontData()
        data.SetInitialFont(font)
        dlg = wxFontDialog(self, data)
        if dlg.ShowModal() == wxID_OK:
            font = dlg.GetFontData().GetChosenFont()
            value = [str(font.GetPointSize()), fontFamiliesWx2Xml[font.GetFamily()],
                     fontStylesWx2Xml[font.GetStyle()],
                     fontWeightsWx2Xml[font.GetWeight()],
                     str(font.GetUnderlined()), font.GetFaceName(),
                     wxFontMapper_GetEncodingName(font.GetEncoding())]
            # Add ignored flags
            self.SetValue(value)
            self.SetModified()
            self.textModified = false
        dlg.Destroy()

################################################################################

class ParamInt(PPanel):
    def __init__(self, parent, id = -1, size = wxDefaultSize, name = ''):
        PPanel.__init__(self, parent, id, name)
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
    def __init__(self, parent, id = -1, size = wxDefaultSize, name = '', textWidth=200):
        PPanel.__init__(self, parent, id, name)
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
    def __init__(self, parent, id = -1, size = wxDefaultSize, name = ''):
        ParamText.__init__(self, parent, id, size, name, 50)

class ParamPosSize(ParamText):
    def __init__(self, parent, id = -1, size = wxDefaultSize, name = ''):
        ParamText.__init__(self, parent, id, size, name, 80)

class ContentDialog(wxDialog):
    def __init__(self, parent, value):
        #wxPreDialog(self)
        # Use another constructor
        w = frame.res.LoadDialog(parent, 'ID_DIALOG_CONTENT')
        self.this = w.this
        w.thisown = 0
        self.thisown = 1
        #frame.res.LoadOnDialog(self, parent, 'ID_DIALOG_CONTENT')
        self.Center()
        self.list = self.FindWindowByName('ID_LIST')
        # Set list items
        for v in value:
            self.list.Append(v)
        self.SetAutoLayout(true)
        # !!! self.SetSizer(sizer)
        self.GetSizer().Fit(self)
        # Callbacks
        self.ID_BUTTON_APPEND = XMLID('ID_BUTTON_APPEND')
        self.ID_BUTTON_REMOVE = XMLID('ID_BUTTON_REMOVE')
        self.ID_BUTTON_UP = XMLID('ID_BUTTON_UP')
        self.ID_BUTTON_DOWN = XMLID('ID_BUTTON_DOWN')
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

class ParamContent(PPanel):
    def __init__(self, parent, id = -1, size = wxDefaultSize, name = ''):
        PPanel.__init__(self, parent, id, name)
        self.ID_TEXT_CTRL = wxNewId()
        self.ID_BUTTON_EDIT = wxNewId()
        self.SetBackgroundColour(panel.GetBackgroundColour())
        sizer = wxBoxSizer()
        self.text = wxTextCtrl(self, self.ID_TEXT_CTRL, size=wxSize(200,-1))
        sizer.Add(self.text, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10)
        self.button = wxButton(self, self.ID_BUTTON_EDIT, 'Edit...')
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
            return eval(self.text.GetValue())
        return self.value
    def SetValue(self, value):
        self.freeze = true
        self.value = value
        self.text.SetValue(str(value))  # update text ctrl
        self.freeze = false
    def OnButtonEdit(self, evt):
        if self.textModified:           # text has newer value
            self.value = eval(self.text.GetValue())
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

# Boxless radiobox 
class RadioBox(PPanel):
    def __init__(self, parent, id, choices,
                 pos=wxDefaultPosition, size=wxDefaultSize, name='radiobox'):
        PPanel.__init__(self, parent, id, name)
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
    def __init__(self, parent, id = -1, size = wxDefaultSize, name = ''):
        RadioBox.__init__(self, parent, id, choices = self.values.keys(), name=name)
    def GetValue(self):
        return self.values[self.GetStringSelection()]
    def SetValue(self, value):
        if not value: value = '1'
        self.SetStringSelection(self.seulav[value])

class ParamOrient(RadioBox):
    values = {'horizontal': 'wxHORIZONTAL', 'vertical': 'wxVERTICAL'}
    seulav = {'wxHORIZONTAL': 'horizontal', 'wxVERTICAL': 'vertical'}
    def __init__(self, parent, id = -1, size = wxDefaultSize, name = ''):
        RadioBox.__init__(self, parent, id, choices = self.values.keys(), name=name)
    def GetValue(self):
        return self.values[self.GetStringSelection()]
    def SetValue(self, value):
        if not value: value = 'wxHORIZONTAL'
        self.SetStringSelection(self.seulav[value])

class ParamFile(PPanel):
    def __init__(self, parent, id = -1, size = wxDefaultSize, name = ''):    
        PPanel.__init__(self, parent, id, name)
        self.ID_TEXT_CTRL = wxNewId()
        self.ID_BUTTON_BROWSE = wxNewId()
        self.SetBackgroundColour(panel.GetBackgroundColour())
        sizer = wxBoxSizer()
        self.text = wxTextCtrl(self, self.ID_TEXT_CTRL, size=wxSize(200,-1))
        sizer.Add(self.text, 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 10)
        self.button = wxButton(self, self.ID_BUTTON_BROWSE, 'Browse...')
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
            return eval(self.text.GetValue())
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
                           defaultDir = os.path.abspath(os.path.dirname(self.value)),
                           defaultFile = os.path.basename(self.value))
        if dlg.ShowModal() == wxID_OK:
            # Make relative
            common = os.path.commonprefix([frame.dataFile, dlg.GetPath()])
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

