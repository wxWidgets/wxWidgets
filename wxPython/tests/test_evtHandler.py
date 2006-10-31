
import wx


# This class is just an experiment to see how easy it would be to
# handle simulating transfer of ownership of object to a 'parent'
# object, and then automatically calling Destroy on those when the
# parent is destroyed.  Conclusion: It's not too hard at all. Now,
# what should I do with it...
class DestroyWrapper(object):
    def __init__(self):
        import weakref
        self.items = weakref.WeakValueDictionary()

    def AddItem(self, obj):
        self.items[len(self.items)+1] = obj

    def __del__(self):
        for item in self.items.values():
            item.Destroy()



            

class MyEvtHandler(wx.EvtHandler):
    instCount = 0
    
    def __init__(self):
        wx.EvtHandler.__init__(self)
        MyEvtHandler.instCount += 1
        self.cnt = MyEvtHandler.instCount
        self.Bind(wx.EVT_CHECKBOX, self.OnCheckBox)

    def __del__(self):
        print "%02d: deleted" % self.cnt

    def OnCheckBox(self, evt):
        print "%02d: %s" % (self.cnt, evt.IsChecked())
        evt.Skip()



class MyFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, title="wx.EvtHandler Test")
        p = wx.Panel(self)

        pushBtn = wx.Button(p, -1, "Push EvtHandler", (20,20))
        popBtn  = wx.Button(p, -1, "Pop EvtHandler", (20,60))

        checkBox = wx.CheckBox(p, -1, "Test EvtHandler", (200, 25))

        self.Bind(wx.EVT_BUTTON, self.OnPushBtn, pushBtn)
        self.Bind(wx.EVT_BUTTON, self.OnPopBtn, popBtn)

##        self.dw = DestroyWrapper()


    def OnPushBtn(self, evt):
        eh = MyEvtHandler()
        self.PushEventHandler(eh)
##        self.dw.AddItem(eh)
        print "%02d: pushed" % eh.cnt


    def OnPopBtn(self, evt):
        eh = self.GetEventHandler()
        if eh.this == self.this:
            print "All already popped!"
        else:
            eh = self.PopEventHandler()
            print "%02d: popped( %s )" % (eh.cnt, eh.__class__)
            eh.Destroy()
            
    

app = wx.App(False)
frm = MyFrame()
frm.Show()
app.MainLoop()
