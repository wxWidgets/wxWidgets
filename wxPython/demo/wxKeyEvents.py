
from wxPython.wx import *

#----------------------------------------------------------------------

keyMap = {
    WXK_BACK : "WXK_BACK",
    WXK_TAB : "WXK_TAB",
    WXK_RETURN : "WXK_RETURN",
    WXK_ESCAPE : "WXK_ESCAPE",
    WXK_SPACE : "WXK_SPACE",
    WXK_DELETE : "WXK_DELETE",
    WXK_START : "WXK_START",
    WXK_LBUTTON : "WXK_LBUTTON",
    WXK_RBUTTON : "WXK_RBUTTON",
    WXK_CANCEL : "WXK_CANCEL",
    WXK_MBUTTON : "WXK_MBUTTON",
    WXK_CLEAR : "WXK_CLEAR",
    WXK_SHIFT : "WXK_SHIFT",
    WXK_ALT : "WXK_ALT",
    WXK_CONTROL : "WXK_CONTROL",
    WXK_MENU : "WXK_MENU",
    WXK_PAUSE : "WXK_PAUSE",
    WXK_CAPITAL : "WXK_CAPITAL",
    WXK_PRIOR : "WXK_PRIOR",
    WXK_NEXT : "WXK_NEXT",
    WXK_END : "WXK_END",
    WXK_HOME : "WXK_HOME",
    WXK_LEFT : "WXK_LEFT",
    WXK_UP : "WXK_UP",
    WXK_RIGHT : "WXK_RIGHT",
    WXK_DOWN : "WXK_DOWN",
    WXK_SELECT : "WXK_SELECT",
    WXK_PRINT : "WXK_PRINT",
    WXK_EXECUTE : "WXK_EXECUTE",
    WXK_SNAPSHOT : "WXK_SNAPSHOT",
    WXK_INSERT : "WXK_INSERT",
    WXK_HELP : "WXK_HELP",
    WXK_NUMPAD0 : "WXK_NUMPAD0",
    WXK_NUMPAD1 : "WXK_NUMPAD1",
    WXK_NUMPAD2 : "WXK_NUMPAD2",
    WXK_NUMPAD3 : "WXK_NUMPAD3",
    WXK_NUMPAD4 : "WXK_NUMPAD4",
    WXK_NUMPAD5 : "WXK_NUMPAD5",
    WXK_NUMPAD6 : "WXK_NUMPAD6",
    WXK_NUMPAD7 : "WXK_NUMPAD7",
    WXK_NUMPAD8 : "WXK_NUMPAD8",
    WXK_NUMPAD9 : "WXK_NUMPAD9",
    WXK_MULTIPLY : "WXK_MULTIPLY",
    WXK_ADD : "WXK_ADD",
    WXK_SEPARATOR : "WXK_SEPARATOR",
    WXK_SUBTRACT : "WXK_SUBTRACT",
    WXK_DECIMAL : "WXK_DECIMAL",
    WXK_DIVIDE : "WXK_DIVIDE",
    WXK_F1 : "WXK_F1",
    WXK_F2 : "WXK_F2",
    WXK_F3 : "WXK_F3",
    WXK_F4 : "WXK_F4",
    WXK_F5 : "WXK_F5",
    WXK_F6 : "WXK_F6",
    WXK_F7 : "WXK_F7",
    WXK_F8 : "WXK_F8",
    WXK_F9 : "WXK_F9",
    WXK_F10 : "WXK_F10",
    WXK_F11 : "WXK_F11",
    WXK_F12 : "WXK_F12",
    WXK_F13 : "WXK_F13",
    WXK_F14 : "WXK_F14",
    WXK_F15 : "WXK_F15",
    WXK_F16 : "WXK_F16",
    WXK_F17 : "WXK_F17",
    WXK_F18 : "WXK_F18",
    WXK_F19 : "WXK_F19",
    WXK_F20 : "WXK_F20",
    WXK_F21 : "WXK_F21",
    WXK_F22 : "WXK_F22",
    WXK_F23 : "WXK_F23",
    WXK_F24 : "WXK_F24",
    WXK_NUMLOCK : "WXK_NUMLOCK",
    WXK_SCROLL : "WXK_SCROLL",
    WXK_PAGEUP : "WXK_PAGEUP",
    WXK_PAGEDOWN : "WXK_PAGEDOWN",
    WXK_NUMPAD_SPACE : "WXK_NUMPAD_SPACE",
    WXK_NUMPAD_TAB : "WXK_NUMPAD_TAB",
    WXK_NUMPAD_ENTER : "WXK_NUMPAD_ENTER",
    WXK_NUMPAD_F1 : "WXK_NUMPAD_F1",
    WXK_NUMPAD_F2 : "WXK_NUMPAD_F2",
    WXK_NUMPAD_F3 : "WXK_NUMPAD_F3",
    WXK_NUMPAD_F4 : "WXK_NUMPAD_F4",
    WXK_NUMPAD_HOME : "WXK_NUMPAD_HOME",
    WXK_NUMPAD_LEFT : "WXK_NUMPAD_LEFT",
    WXK_NUMPAD_UP : "WXK_NUMPAD_UP",
    WXK_NUMPAD_RIGHT : "WXK_NUMPAD_RIGHT",
    WXK_NUMPAD_DOWN : "WXK_NUMPAD_DOWN",
    WXK_NUMPAD_PRIOR : "WXK_NUMPAD_PRIOR",
    WXK_NUMPAD_PAGEUP : "WXK_NUMPAD_PAGEUP",
    WXK_NUMPAD_NEXT : "WXK_NUMPAD_NEXT",
    WXK_NUMPAD_PAGEDOWN : "WXK_NUMPAD_PAGEDOWN",
    WXK_NUMPAD_END : "WXK_NUMPAD_END",
    WXK_NUMPAD_BEGIN : "WXK_NUMPAD_BEGIN",
    WXK_NUMPAD_INSERT : "WXK_NUMPAD_INSERT",
    WXK_NUMPAD_DELETE : "WXK_NUMPAD_DELETE",
    WXK_NUMPAD_EQUAL : "WXK_NUMPAD_EQUAL",
    WXK_NUMPAD_MULTIPLY : "WXK_NUMPAD_MULTIPLY",
    WXK_NUMPAD_ADD : "WXK_NUMPAD_ADD",
    WXK_NUMPAD_SEPARATOR : "WXK_NUMPAD_SEPARATOR",
    WXK_NUMPAD_SUBTRACT : "WXK_NUMPAD_SUBTRACT",
    WXK_NUMPAD_DECIMAL : "WXK_NUMPAD_DECIMAL",
    WXK_NUMPAD_DIVIDE : "WXK_NUMPAD_DIVIDE",
}


#----------------------------------------------------------------------

class KeySink(wxWindow):
    def __init__(self, parent):
        wxWindow.__init__(self, parent, -1, style =
                          wxWANTS_CHARS
                          #| wxRAISED_BORDER
                          #| wxSUNKEN_BORDER
                          )
        self.SetBackgroundColour(wxBLUE)
        self.haveFocus = False
        self.callSkip = False
        self.logKeyDn = True
        self.logKeyUp = True
        self.logChar = True

        EVT_PAINT(self, self.OnPaint)
        EVT_SET_FOCUS(self, self.OnSetFocus)
        EVT_KILL_FOCUS(self, self.OnKillFocus)
        EVT_MOUSE_EVENTS(self, self.OnMouse)

        EVT_KEY_DOWN(self, self.OnKeyDown)
        EVT_KEY_UP(self, self.OnKeyUp)
        EVT_CHAR(self, self.OnChar)


    def SetCallSkip(self, skip):
        self.callSkip = skip

    def SetLogKeyUp(self, val):
        self.logKeyUp = val

    def SetLogKeyDn(self, val):
        self.logKeyDn = val

    def SetLogChar(self, val):
        self.logChar = val


    def OnPaint(self, evt):
        dc = wxPaintDC(self)
        rect = self.GetClientRect()
        dc.SetTextForeground(wxWHITE)
        dc.DrawLabel("Click here and then press some keys",
                     rect, wxALIGN_CENTER | wxALIGN_TOP)
        if self.haveFocus:
            dc.SetTextForeground(wxGREEN)
            dc.DrawLabel("Have Focus", rect, wxALIGN_RIGHT | wxALIGN_BOTTOM)
        else:
            dc.SetTextForeground(wxRED)
            dc.DrawLabel("Need Focus!", rect, wxALIGN_RIGHT | wxALIGN_BOTTOM)


    def OnSetFocus(self, evt):
        self.haveFocus = True
        self.Refresh()

    def OnKillFocus(self, evt):
        self.haveFocus = False
        self.Refresh()

    def OnMouse(self, evt):
        if evt.ButtonDown():
            self.SetFocus()


    def OnKeyDown(self, evt):
        if self.logKeyDn:
            self.GetParent().keylog.LogKeyEvent("KeyDown", evt)
        if self.callSkip:
            evt.Skip()

    def OnKeyUp(self, evt):
        if self.logKeyUp:
            self.GetParent().keylog.LogKeyEvent("KeyUp", evt)
        if self.callSkip:
            evt.Skip()

    def OnChar(self, evt):
        if self.logChar:
            self.GetParent().keylog.LogKeyEvent("Char", evt)


#----------------------------------------------------------------------

from wxPython.lib.mixins.listctrl import wxListCtrlAutoWidthMixin

class KeyLog(wxListCtrl, wxListCtrlAutoWidthMixin):

    def __init__(self, parent):
        wxListCtrl.__init__(self, parent, -1,
                            style = wxLC_REPORT|wxLC_VRULES|wxLC_HRULES)
        wxListCtrlAutoWidthMixin.__init__(self)

        self.InsertColumn(0, "Event Type")
        self.InsertColumn(1, "Key Name")
        self.InsertColumn(2, "Key Code")
        self.InsertColumn(3, "Modifiers")
        self.InsertColumn(4, "RawKeyCode")
        self.InsertColumn(5, "RawKeyFlags")
        self.InsertColumn(6, "")

        for x in range(6):
            self.SetColumnWidth(x, wxLIST_AUTOSIZE_USEHEADER)

        self.SetColumnWidth(1, 125)


    def LogKeyEvent(self, evType, evt):
        keycode = evt.GetKeyCode()
        keyname = keyMap.get(keycode, None)
        if keyname is None:
            if keycode < 256:
                if keycode == 0:
                    keyname = "NUL"
                elif keycode < 27:
                    keyname = "Ctrl-%s" % chr(ord('A') + keycode-1)
                else:
                    keyname = "\"%s\"" % chr(keycode)
            else:
                keyname = "unknown (%s)" % keycode

        modifiers = ""
        for mod, ch in [(evt.ControlDown(), 'C'),
                        (evt.AltDown(),     'A'),
                        (evt.ShiftDown(),   'S'),
                        (evt.MetaDown(),    'M')]:
            if mod:
                modifiers += ch
            else:
                modifiers += '-'

        id = self.InsertStringItem(self.GetItemCount(), evType)
        self.SetStringItem(id, 1, keyname)
        self.SetStringItem(id, 2, str(keycode))
        self.SetStringItem(id, 3, modifiers)
        self.SetStringItem(id, 4, str(evt.GetRawKeyCode()))
        self.SetStringItem(id, 5, str(evt.GetRawKeyFlags()))

        #print ( id, evType, keyname, str(keycode), modifiers, str(evt.GetRawKeyCode()), str(evt.GetRawKeyFlags()))

        self.EnsureVisible(id)


    def ClearLog(self):
        self.DeleteAllItems()


#----------------------------------------------------------------------


class TestPanel(wxPanel):
    def __init__(self, parent, log):
        self.log = log
        wxPanel.__init__(self, parent, -1, style=0)
        self.keysink = KeySink(self)
        self.keysink.SetSize((100, 65))
        self.keylog = KeyLog(self)

        btn = wxButton(self, -1, "Clear Log")
        EVT_BUTTON(self, btn.GetId(), self.OnClearBtn)

        cb1 = wxCheckBox(self, -1, "Call evt.Skip for Key Up/Dn events")
        EVT_CHECKBOX(self, cb1.GetId(), self.OnSkipCB)

        cb2 = wxCheckBox(self, -1, "EVT_KEY_UP")
        EVT_CHECKBOX(self, cb2.GetId(), self.OnKeyUpCB)
        cb2.SetValue(True)

        cb3 = wxCheckBox(self, -1, "EVT_KEY_DOWN")
        EVT_CHECKBOX(self, cb3.GetId(), self.OnKeyDnCB)
        cb3.SetValue(True)

        cb4 = wxCheckBox(self, -1, "EVT_CHAR")
        EVT_CHECKBOX(self, cb4.GetId(), self.OnCharCB)
        cb4.SetValue(True)

        buttons = wxBoxSizer(wxHORIZONTAL)
        buttons.Add(btn, 0, wxALL, 4)
        buttons.Add(cb1, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 6)
        buttons.Add(cb2, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 6)
        buttons.Add(cb3, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 6)
        buttons.Add(cb4, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 6)

        sizer = wxBoxSizer(wxVERTICAL)
        sizer.Add(self.keysink, 0, wxGROW)
        sizer.Add(buttons)
        sizer.Add(self.keylog, 1, wxGROW)

        self.SetSizer(sizer)


    def OnClearBtn(self, evt):
        self.keylog.ClearLog()

    def OnSkipCB(self, evt):
        self.keysink.SetCallSkip(evt.GetInt())

    def OnKeyUpCB(self, evt):
        self.keysink.SetLogKeyUp(evt.GetInt())

    def OnKeyDnCB(self, evt):
        self.keysink.SetLogKeyDn(evt.GetInt())

    def OnCharCB(self, evt):
        self.keysink.SetLogChar(evt.GetInt())


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>wxKeyEvents</center></h2>

This demo simply lets catches all key events and prints info about them.
It is meant to be used as a compatibility test for cross platform work.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

