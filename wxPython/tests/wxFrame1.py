#Boa:Frame:wxFrame1

from wxPython.wx import *

import sys


def create(parent):
    return wxFrame1(parent)

frame_count = 0

[wxID_WXFRAME1BUTTON1, wxID_WXFRAME1] = map(lambda _init_ctrls: wxNewId(), range(2))

class wxFrame1(wxFrame):
    def _init_utils(self):
        pass

    def _init_ctrls(self, prnt):
        wxFrame.__init__(self, size = wxSize(960, 662), id = wxID_WXFRAME1, title = 'wxFrame1', parent = prnt, name = '', style = wxDEFAULT_FRAME_STYLE, pos = (-1, -1))
        self._init_utils()

        self.button1 = wxButton(label = 'button1', id = wxID_WXFRAME1BUTTON1, parent = self, name = 'button1', size = wxSize(75, 23), style = 0, pos = wxPoint(216, 152))
        EVT_BUTTON(self.button1, wxID_WXFRAME1BUTTON1, self.OnButton1Button)

    def __init__(self, parent):
        self._init_ctrls(parent)
        global frame_count
        frame_count += 1
        self.SetTitle('Frame %d' % frame_count)
        print 'Frame %d' % frame_count
        self.shown = 0
        EVT_IDLE(self, self.OnIdle)

    def OnButton1Button(self, event):
        print 'Closing %s' % self.GetTitle()
        self.Close()

    def OnIdle(self, evt):
        if not self.shown:
            #print self.GetTitle(), "shown"
            #print self.Show(false), self.Show(true)
            #import win32gui, win32con
            #win32gui.ShowWindow(self.GetHandle(), win32con.SW_SHOW)
            self.shown = 1


class BoaApp(wxApp):
    def OnInit(self):
        frame1 = create(None)

##         import win32gui, win32con
##         win32gui.ShowWindow(frame1.GetHandle(), win32con.SW_SHOW)
##         frame1.Show(true)

        frame1.Show(true)
        frame1.Show(false)
        frame1.Show(true)

        frame2 = create(None)
        print frame2.Show(true)
        return true

def main():
    application = BoaApp(0)
    application.MainLoop()

if __name__ == '__main__':
    main()

##if __name__ == '__main__':
##    app = wxPySimpleApp()
##
##    frame1 = create(None)
##    frame1.Show(true)
##
##    frame2 = create(None)
##    frame2.Show(true)
##
##    app.MainLoop()
##
