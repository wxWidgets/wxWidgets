"""Easy generation of new events classes and binder objects"""

__author__ = "Miki Tebeka <miki.tebeka@gmail.com>"

import wx

#---------------------------------------------------------------------------

def NewEvent():
    """Generate new (Event, Binder) tuple
        e.g. MooEvent, EVT_MOO = NewEvent()
    """
    evttype = wx.NewEventType()
    
    class _Event(wx.PyEvent):
        def __init__(self, **kw):
            wx.PyEvent.__init__(self)
            self.SetEventType(evttype)
            self.__dict__.update(kw)

    return _Event, wx.PyEventBinder(evttype)



def NewCommandEvent():
    """Generate new (CmdEvent, Binder) tuple
        e.g. MooCmdEvent, EVT_MOO = NewCommandEvent()
    """
    evttype = wx.NewEventType()

    class _Event(wx.PyCommandEvent):
        def __init__(self, id, **kw):
            wx.PyCommandEvent.__init__(self, evttype, id)
            self.__dict__.update(kw)
    
    return _Event, wx.PyEventBinder(evttype, 1)


#---------------------------------------------------------------------------

def _test():
    """A little smoke test"""
    from threading import Thread
    from time import sleep

    MooEvent, EVT_MOO = NewEvent()
    GooEvent, EVT_GOO = NewCommandEvent()

    DELAY = 0.7

    def evt_thr(win):
        sleep(DELAY)
        wx.PostEvent(win, MooEvent(moo=1))

    def cmd_thr(win, id):
        sleep(DELAY)
        wx.PostEvent(win, GooEvent(id, goo=id))

    ID_CMD1 = wx.NewId()
    ID_CMD2 = wx.NewId()

    class Frame(wx.Frame):
        def __init__(self):
            wx.Frame.__init__(self, None, -1, "MOO")
            sizer = wx.BoxSizer(wx.VERTICAL)
            EVT_MOO(self, self.on_moo)
            b = wx.Button(self, -1, "Generate MOO")
            sizer.Add(b, 1, wx.EXPAND)
            wx.EVT_BUTTON(self, b.GetId(), self.on_evt_click)
            b = wx.Button(self, ID_CMD1, "Generate GOO with %d" % ID_CMD1)
            sizer.Add(b, 1, wx.EXPAND)
            wx.EVT_BUTTON(self, ID_CMD1, self.on_cmd_click)
            b = wx.Button(self, ID_CMD2, "Generate GOO with %d" % ID_CMD2)
            sizer.Add(b, 1, wx.EXPAND)
            wx.EVT_BUTTON(self, ID_CMD2, self.on_cmd_click)

            EVT_GOO(self, ID_CMD1, self.on_cmd1)
            EVT_GOO(self, ID_CMD2, self.on_cmd2)

            self.SetSizer(sizer)
            self.SetAutoLayout(True)
            sizer.Fit(self)

        def on_evt_click(self, e):
            t = Thread(target=evt_thr, args=(self, ))
            t.setDaemon(True)
            t.start()

        def on_cmd_click(self, e):
            t = Thread(target=cmd_thr, args=(self, e.GetId()))
            t.setDaemon(True)
            t.start()

        def show(self, msg, title):
            dlg = wx.MessageDialog(self, msg, title, wx.OK)
            dlg.ShowModal()
            dlg.Destroy()

        def on_moo(self, e):
            self.show("MOO = %s" % e.moo, "Got Moo")

        def on_cmd1(self, e):
            self.show("goo = %s" % e.goo, "Got Goo (cmd1)")

        def on_cmd2(self, e):
            self.show("goo = %s" % e.goo, "Got Goo (cmd2)")
            

    app = wx.PySimpleApp()
    f = Frame()
    f.Show(True)
    app.MainLoop()

if __name__ == "__main__":
    _test()
