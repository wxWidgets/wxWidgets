from wxPython.wx import *

havePopupWindow = 1
try:
    wxPopupWindow
except NameError:
    havePopupWindow = 0
    wxPopupWindow = wxPopupTransientWindow = wxWindow

#---------------------------------------------------------------------------

class TestPopup(wxPopupWindow):
    """Adds a bit of text and mouse movement to the wxPopupWindow"""
    def __init__(self, parent, style):
        wxPopupWindow.__init__(self, parent, style)
        self.SetBackgroundColour("CADET BLUE")
        st = wxStaticText(self, -1,
                          "This is a special kind of top level\n"
                          "window that can be used for\n"
                          "popup menus, combobox popups\n"
                          "and such.\n\n"
                          "Try positioning the demo near\n"
                          "the bottom of the screen and \n"
                          "hit the button again.\n\n"
                          "In this demo this window can\n"
                          "be dragged with the left button\n"
                          "and closed with the right."
                          ,
                          pos=(10,10))
        sz = st.GetBestSize()
        self.SetSize( (sz.width+20, sz.height+20) )

        EVT_LEFT_DOWN(self, self.OnMouseLeftDown)
        EVT_MOTION(self, self.OnMouseMotion)
        EVT_LEFT_UP(self, self.OnMouseLeftUp)
        EVT_RIGHT_UP(self, self.OnRightUp)
        EVT_LEFT_DOWN(st, self.OnMouseLeftDown)
        EVT_MOTION(st, self.OnMouseMotion)
        EVT_LEFT_UP(st, self.OnMouseLeftUp)
        EVT_RIGHT_UP(st, self.OnRightUp)

    def OnMouseLeftDown(self, evt):
        self.ldPos = evt.GetEventObject().ClientToScreen(evt.GetPosition())
        self.wPos = self.GetPosition()
        self.CaptureMouse()

    def OnMouseMotion(self, evt):
        if evt.Dragging() and evt.LeftIsDown():
            dPos = evt.GetEventObject().ClientToScreen(evt.GetPosition())
            nPos = (self.wPos.x + (dPos.x - self.ldPos.x),
                    self.wPos.y + (dPos.y - self.ldPos.y))
            self.Move(nPos)

    def OnMouseLeftUp(self, evt):
        self.ReleaseMouse()

    def OnRightUp(self, evt):
        self.Show(False)
        self.Destroy()


class TestTransientPopup(wxPopupTransientWindow):
    """Adds a bit of text and mouse movement to the wxPopupWindow"""
    def __init__(self, parent, style, log):
        wxPopupTransientWindow.__init__(self, parent, style)
        self.log = log
        panel = wxPanel(self, -1)
        panel.SetBackgroundColour("#FFB6C1")
        st = wxStaticText(panel, -1,
                          "wxPopupTransientWindow is a\n"
                          "wxPopupWindow which disappears\n"
                          "automatically when the user\n"
                          "clicks the mouse outside it or if it\n"
                          "loses focus in any other way."
                          ,
                          pos=(10,10))
        sz = st.GetBestSize()
        panel.SetSize( (sz.width+20, sz.height+20) )
        self.SetSize(panel.GetSize())

    def ProcessLeftDown(self, evt):
        self.log.write("ProcessLeftDown\n")
        return False

    def OnDismiss(self):
        self.log.write("OnDismiss\n")



class TestPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)
        self.log = log

        b = wxButton(self, -1, "Show wxPopupWindow", (25, 50))
        EVT_BUTTON(self, b.GetId(), self.OnShowPopup)

        b = wxButton(self, -1, "Show wxPopupTransientWindow", (25, 95))
        EVT_BUTTON(self, b.GetId(), self.OnShowPopupTransient)

        if 0:
            b = wxButton(self, -1, "Show wxPopupWindow with listbox", (25, 140))
            EVT_BUTTON(self, b.GetId(), self.OnShowPopupListbox)


    def OnShowPopup(self, evt):
        win = TestPopup(self, wxSIMPLE_BORDER)

        # Show the popup right below or above the button
        # depending on available screen space...
        btn = evt.GetEventObject()
        pos = btn.ClientToScreen( (0,0) )
        sz =  btn.GetSize()
        win.Position(pos, (0, sz.height))

        win.Show(True)


    def OnShowPopupTransient(self, evt):
        win = TestTransientPopup(self, wxSIMPLE_BORDER, self.log)

        # Show the popup right below or above the button
        # depending on available screen space...
        btn = evt.GetEventObject()
        pos = btn.ClientToScreen( (0,0) )
        sz =  btn.GetSize()
        win.Position(pos, (0, sz.height))

        win.Popup()


    def OnShowPopupListbox(self, evt):
        win = TestPopupWithListbox(self, wxNO_BORDER, self.log)

        # Show the popup right below or above the button
        # depending on available screen space...
        btn = evt.GetEventObject()
        pos = btn.ClientToScreen( (0,0) )
        sz =  btn.GetSize()
        win.Position(pos, (0, sz.height))

        win.Show(True)

class TestPopupWithListbox(wxPopupWindow):
    def __init__(self, parent, style, log):
        wxPopupWindow.__init__(self, parent, style)
        import keyword
        self.lb = wxListBox(self, -1, choices = keyword.kwlist)
        #sz = self.lb.GetBestSize()
        self.SetSize((150, 75)) #sz)
        self.lb.SetSize(self.GetClientSize())
        self.lb.SetFocus()
        EVT_LEFT_DOWN(self.lb, self.OnLeft)
        EVT_LISTBOX(self, -1, self.OnListBox)

    def OnLeft(self, evt):
        print "OnLeft", evt.GetEventObject()
        evt.Skip()
    def OnListBox(self, evt):
        print "OnListBox", evt.GetEventObject()
        evt.Skip()



#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    if havePopupWindow:
        win = TestPanel(nb, log)
        return win
    else:
        dlg = wxMessageDialog(frame, 'wxPopupWindow is not available on this platform.',
                          'Sorry', wxOK | wxICON_INFORMATION)
        dlg.ShowModal()
        dlg.Destroy()

#---------------------------------------------------------------------------




overview = """\
"""




if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

