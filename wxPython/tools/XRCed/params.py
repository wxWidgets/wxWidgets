# Name:         params.py
# Purpose:      Classes for parameter introduction
# Author:       Roman Rolinsky <rolinsky@mema.ucl.ac.be>
# Created:      22.08.2001

from wxPython.wx import *
from wxPython.xrc import *
import string

# Object which is currently processed
currentXXX = None
def SetCurrentXXX(xxx):
    global currentXXX
    currentXXX = xxx

# Register objects in the view
registered = {}
def Register(param, obj):
    registered[param] = obj
def GetRegistered(param):
    return registered[param]
def ClearRegister():
    registered.clear()

genericStyles = ['wxSIMPLE_BORDER', 'wxSUNKEN_BORDER', 'wxRAISED_BORDER',
                 'wxTAB_TRAVERSAL', 'wxWANTS_CHARS', 'wxVSCROLL', 'wxHSCROLL']

class ParamBinaryOr(wxPanel):
    ID_TEXT_CTRL = wxNewId()
    ID_BUTTON_CHOICES = wxNewId()
    def __init__(self, parent, id, value, size, name, param):
        wxPanel.__init__(self, parent, id, name=name)
        self.SetBackgroundColour(panel.GetBackgroundColour())
        Register(param, self)
        sizer = wxBoxSizer()
        self.text = wxTextCtrl(self, self.ID_TEXT_CTRL, value, size=wxSize(200,-1))
        sizer.Add(self.text, 0, wxRIGHT, 10)
        self.button = wxButton(self, self.ID_BUTTON_CHOICES, 'Edit...')
        sizer.Add(self.button)
        self.SetAutoLayout(true)
        self.SetSizer(sizer)
        sizer.Fit(self)
        EVT_BUTTON(self, self.ID_BUTTON_CHOICES, self.OnButtonChoices)
        EVT_TEXT(self, self.ID_TEXT_CTRL, self.OnChange)
    def OnChange(self, evt):
        panel.SetModified(true)
        evt.Skip()
    def GetValue(self):
        return self.text.GetValue()
    def SetValue(self, value):
        self.text.SetValue(value)
    def OnButtonChoices(self, evt):
        dlg = wxDialog(self, -1, 'Choices', size=wxSize(250,300))
        topSizer = wxBoxSizer(wxVERTICAL)
        listBox = wxCheckListBox(dlg, -1, choices=self.values)
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
                    print 'Unknown flag: %s: ignored.' % i
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
        dlg.Center()
        if dlg.ShowModal() != wxID_OK: return
        value = []
        for i in range(listBox.Number()):
            if listBox.IsChecked(i):
                value.append(self.values[i])
        # Add ignored flags
        value.extend(ignored)
        self.SetValue(reduce(lambda a,b: a+'|'+b, value))
        panel.SetModified(true)

class ParamFlag(ParamBinaryOr):
    values = ['wxTOP', 'wxBOTTOM', 'wxLEFT', 'wxRIGHT', 'wxALL',
              'wxEXPAND', 'wxSHAPED', 'wxALIGN_CENTRE', 'wxALIGN_RIGHT',
              'wxALIGN_BOTTOM', 'wxALIGN_CENTRE_VERTICAL',
              'wxALIGN_CENTRE_HORIZONTAL']
    equal = {'wxALIGN_CENTER': 'wxALIGN_CENTRE',
             'wxALIGN_CENTER_VERTICAL': 'wxALIGN_CENTRE_VERTICAL',
             'wxALIGN_CENTER_HORIZONTAL': 'wxALIGN_CENTRE_HORIZONTAL'}
    def __init__(self, parent, id, value, size, name, param):
        ParamBinaryOr.__init__(self, parent, id, value, size, name, param)

class ParamStyle(ParamBinaryOr):
    equal = {'wxALIGN_CENTER': 'wxALIGN_CENTRE'}
    def __init__(self, parent, id, value, size, name, param):
        self.values = currentXXX.styles + genericStyles
        ParamBinaryOr.__init__(self, parent, id, value, size, name, param)

class ParamInt(wxPanel):
    ID_SPIN_CTRL = wxNewId()
    def __init__(self, parent, id, value, size, name, param):
        wxPanel.__init__(self, parent, id, name=name)
        sizer = wxBoxSizer()
        self.spin = wxSpinCtrl(self, self.ID_SPIN_CTRL, value, size=wxSize(50,-1))
        self.SetBackgroundColour(panel.GetBackgroundColour())
        sizer.Add(self.spin)
        self.SetAutoLayout(true)
        self.SetSizer(sizer)
        sizer.Fit(self)
        Register(param, self)
        EVT_SPINCTRL(self, self.ID_SPIN_CTRL, self.OnChange)
    def GetValue(self):
        return str(self.spin.GetValue())
    def SetValue(self, value):
        if not value: value = 0
        self.spin.SetValue(int(value))
    def OnChange(self, evt):
        panel.SetModified(true)
        evt.Skip()

class ParamText(wxTextCtrl):
    def __init__(self, parent, id, value, size, name, param):
        wxTextCtrl.__init__(self, parent, id, value, size=wxSize(200,-1), name=name)
        Register(param, self)
        EVT_TEXT(self, id, self.OnChange)
    def OnChange(self, evt):
        panel.SetModified(true)
        evt.Skip()

class ParamAccel(wxTextCtrl):
    def __init__(self, parent, id, value, size, name, param):
        wxTextCtrl.__init__(self, parent, id, value, size=wxSize(50,-1), name=name)
        Register(param, self)
        EVT_TEXT(self, id, self.OnChange)
    def OnChange(self, evt):
        panel.SetModified(true)
        evt.Skip()

class ParamPosSize(wxTextCtrl):
    def __init__(self, parent, id, value, size, name, param):
        wxTextCtrl.__init__(self, parent, id, value, size=wxSize(80,-1), name=name)
        Register(param, self)
        EVT_TEXT(self, id, self.OnChange)
    def OnChange(self, evt):
        panel.SetModified(true)
        evt.Skip()

class ContentDialog(wxDialog):
    def __init__(self, parent, value):
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
        # !!! Bug in XRC or wxWin listbox select random items
        self.FindWindowByName('wxID_OK').SetFocus()
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
            evt.Enable(self.list.GetSelection() < self.list.Number() - 1)

class ParamContent(wxPanel):
    ID_TEXT_CTRL = wxNewId()
    ID_BUTTON_EDIT = wxNewId()
    def __init__(self, parent, id, value, size, name, param):
        wxPanel.__init__(self, parent, id, name=name)
        self.SetBackgroundColour(panel.GetBackgroundColour())
        Register(param, self)
        sizer = wxBoxSizer()
        self.text = wxTextCtrl(self, self.ID_TEXT_CTRL, str(value), size=wxSize(200,-1))
        sizer.Add(self.text, 0, wxRIGHT, 10)
        self.button = wxButton(self, self.ID_BUTTON_EDIT, 'Edit...')
        sizer.Add(self.button)
        self.SetAutoLayout(true)
        self.SetSizer(sizer)
        sizer.Fit(self)
        self.textModified = false
        EVT_BUTTON(self, self.ID_BUTTON_EDIT, self.OnButtonEdit)
        EVT_TEXT(self, self.ID_TEXT_CTRL, self.OnChange)
    def OnChange(self, evt):
        panel.SetModified(true)
        self.textModified = true
        evt.Skip()
    def GetValue(self):
        if self.textModified:           # text has newer value
            return eval(self.text.GetValue())
        return self.value
    def SetValue(self, value):
        self.value = value
        self.text.SetValue(str(value))  # update text ctrl
    def OnButtonEdit(self, evt):
        if self.textModified:           # text has newer value
            self.value = eval(self.text.GetValue())
        dlg = ContentDialog(self, self.value)
        if dlg.ShowModal() != wxID_OK: return
        value = []
        for i in range(dlg.list.Number()):
            value.append(dlg.list.GetString(i))
        # Add ignored flags
        self.SetValue(value)
        panel.SetModified(true)
        self.textModified = false

# Boxless radiobox 
class RadioBox(wxPanel):
    ID_RADIO_CHOICE = wxNewId()         # economize on IDs, use names
    def __init__(self, parent, id, choices,
                 pos=wxDefaultPosition, size=wxDefaultSize, name='radiobox'):
        wxPanel.__init__(self, parent, id, pos, size, name=name)
        self.SetBackgroundColour(panel.GetBackgroundColour())
        self.choices = choices
        topSizer = wxBoxSizer()
        for i in choices:
            topSizer.Add(wxRadioButton(self, self.ID_RADIO_CHOICE, i, name=i))
        self.SetAutoLayout(true)
        self.SetSizer(topSizer)
        topSizer.Fit(self)
        EVT_RADIOBUTTON(self, self.ID_RADIO_CHOICE, self.OnRadioChoice)
    def SetStringSelection(self, value):
        self.value = None
        for i in self.choices:
            w = self.FindWindowByName(i)
            w.SetValue(i == value)
        self.value = value
    def OnRadioChoice(self, evt):
        if not self.value:              # procedure call
            evt.Skip()
            return
        self.FindWindowByName(self.value).SetValue(false)
        self.value = evt.GetEventObject().GetName()
        panel.SetModified(true)
    def GetStringSelection(self):
        return self.value

class ParamBool(RadioBox):
    values = {'yes': '1', 'no': '0'}
    seulav = {'1': 'yes', '0': 'no'}
    def __init__(self, parent, id, value, size, name, param):
        RadioBox.__init__(self, parent, id, choices = self.values.keys(), name=name)
        Register(param, self)
        self.SetValue(value)
    def GetValue(self):
        return self.values[self.GetStringSelection()]
    def SetValue(self, value):
        if not value: value = '1'
        self.SetStringSelection(self.seulav[value])

class ParamOrient(RadioBox):
    values = {'horizontal': 'wxHORIZONTAL', 'vertical': 'wxVERTICAL'}
    seulav = {'wxHORIZONTAL': 'horizontal', 'wxVERTICAL': 'vertical'}
    def __init__(self, parent, id, value, size, name, param):
        RadioBox.__init__(self, parent, id, choices = self.values.keys(), name=name)
        Register(param, self)
        self.SetValue(value)
    def GetValue(self):
        return self.values[self.GetStringSelection()]
    def SetValue(self, value):
        if not value: value = 'wxHORIZONTAL'
        self.SetStringSelection(self.seulav[value])

paramDict = {
    'flag': ParamFlag,
    'style': ParamStyle,
    'pos': ParamPosSize, 'size': ParamPosSize,
    'border': ParamInt, 'cols': ParamInt, 'rows': ParamInt,
    'vgap': ParamInt, 'hgap': ParamInt,
    'checkable': ParamBool, 'accel': ParamAccel,
    'label': ParamText, 'title': ParamText, 'value': ParamText,
    'content': ParamContent, 'selection': ParamInt,
    'min': ParamInt, 'max': ParamInt,
    }
