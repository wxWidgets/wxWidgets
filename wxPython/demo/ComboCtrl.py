
import wx
import wx.combo
import os

#----------------------------------------------------------------------

class NullLog:
    def write(*args):
        pass


# This class is used to provide an interface between a ComboCtrl and a
# ListCtrl that is used as the popoup for the combo widget.  In this
# case we use multiple inheritance to derive from both wx.ListCtrl and
# wx.ComboPopup, but it also works well when deriving from just
# ComboPopup and using a has-a relationship with the popup control,
# you just need to be sure to return the control itself from the
# GetControl method.

class ListCtrlComboPopup(wx.ListCtrl, wx.combo.ComboPopup):
        
    def __init__(self, log=None):
        if log:
            self.log = log
        else:
            self.log = NullLog()
            
        
        # Since we are using multiple inheritance, and don't know yet
        # which window is to be the parent, we'll do 2-phase create of
        # the ListCtrl instead, and call its Create method later in
        # our Create method.  (See Create below.)
        self.PostCreate(wx.PreListCtrl())

        # Also init the ComboPopup base class.
        wx.combo.ComboPopup.__init__(self)
        

    def AddItem(self, txt):
        self.InsertStringItem(self.GetItemCount(), txt)

    def OnMotion(self, evt):
        item, flags = self.HitTest(evt.GetPosition())
        if item >= 0:
            self.Select(item)
            self.curitem = item

    def OnLeftDown(self, evt):
        self.value = self.curitem
        self.Dismiss()


    # The following methods are those that are overridable from the
    # ComboPopup base class.  Most of them are not required, but all
    # are shown here for demonstration purposes.


    # This is called immediately after construction finishes.  You can
    # use self.GetCombo if needed to get to the ComboCtrl instance.
    def Init(self):
        self.log.write("ListCtrlComboPopup.Init")
        self.value = -1
        self.curitem = -1


    # Create the popup child control.  Return true for success.
    def Create(self, parent):
        self.log.write("ListCtrlComboPopup.Create")
        wx.ListCtrl.Create(self, parent,
                           style=wx.LC_LIST|wx.LC_SINGLE_SEL|wx.SIMPLE_BORDER)
        self.Bind(wx.EVT_MOTION, self.OnMotion)
        self.Bind(wx.EVT_LEFT_DOWN, self.OnLeftDown)
        return True


    # Return the widget that is to be used for the popup
    def GetControl(self):
        #self.log.write("ListCtrlComboPopup.GetControl")
        return self

    # Called just prior to displaying the popup, you can use it to
    # 'select' the current item.
    def SetStringValue(self, val):
        self.log.write("ListCtrlComboPopup.SetStringValue")
        idx = self.FindItem(-1, val)
        if idx != wx.NOT_FOUND:
            self.Select(idx)

    # Return a string representation of the current item.
    def GetStringValue(self):
        self.log.write("ListCtrlComboPopup.GetStringValue")
        if self.value >= 0:
            return self.GetItemText(self.value)
        return ""

    # Called immediately after the popup is shown
    def OnPopup(self):
        self.log.write("ListCtrlComboPopup.OnPopup")
        wx.combo.ComboPopup.OnPopup(self)

    # Called when popup is dismissed
    def OnDismiss(self):
        self.log.write("ListCtrlComboPopup.OnDismiss")
        wx.combo.ComboPopup.OnDismiss(self)

    # This is called to custom paint in the combo control itself
    # (ie. not the popup).  Default implementation draws value as
    # string.
    def PaintComboControl(self, dc, rect):
        self.log.write("ListCtrlComboPopup.PaintComboControl")
        wx.combo.ComboPopup.PaintComboControl(self, dc, rect)

    # Receives key events from the parent ComboCtrl.  Events not
    # handled should be skipped, as usual.
    def OnComboKeyEvent(self, event):
        self.log.write("ListCtrlComboPopup.OnComboKeyEvent")
        wx.combo.ComboPopup.OnComboKeyEvent(self, event)

    # Implement if you need to support special action when user
    # double-clicks on the parent wxComboCtrl.
    def OnComboDoubleClick(self):
        self.log.write("ListCtrlComboPopup.OnComboDoubleClick")
        wx.combo.ComboPopup.OnComboDoubleClick(self)

    # Return final size of popup. Called on every popup, just prior to OnPopup.
    # minWidth = preferred minimum width for window
    # prefHeight = preferred height. Only applies if > 0,
    # maxHeight = max height for window, as limited by screen size
    #   and should only be rounded down, if necessary.
    def GetAdjustedSize(self, minWidth, prefHeight, maxHeight):
        self.log.write("ListCtrlComboPopup.GetAdjustedSize: %d, %d, %d" % (minWidth, prefHeight, maxHeight))
        return wx.combo.ComboPopup.GetAdjustedSize(self, minWidth, prefHeight, maxHeight)

    # Return true if you want delay the call to Create until the popup
    # is shown for the first time. It is more efficient, but note that
    # it is often more convenient to have the control created
    # immediately.    
    # Default returns false.
    def LazyCreate(self):
        self.log.write("ListCtrlComboPopup.LazyCreate")
        return wx.combo.ComboPopup.LazyCreate(self)
        


#----------------------------------------------------------------------
# This class is a popup containing a TreeCtrl.  This time we'll use a
# has-a style (instead of is-a like above.)

class TreeCtrlComboPopup(wx.combo.ComboPopup):

    # overridden ComboPopup methods
    
    def Init(self):
        self.value = None
        self.curitem = None

        
    def Create(self, parent):
        self.tree = wx.TreeCtrl(parent, style=wx.TR_HIDE_ROOT
                                |wx.TR_HAS_BUTTONS
                                |wx.TR_SINGLE
                                |wx.TR_LINES_AT_ROOT
                                |wx.SIMPLE_BORDER)
        self.tree.Bind(wx.EVT_MOTION, self.OnMotion)
        self.tree.Bind(wx.EVT_LEFT_DOWN, self.OnLeftDown)
        

    def GetControl(self):
        return self.tree


    def GetStringValue(self):
        if self.value:
            return self.tree.GetItemText(self.value)
        return ""


    def OnPopup(self):
        if self.value:
            self.tree.EnsureVisible(self.value)
            self.tree.SelectItem(self.value)


    def SetStringValue(self, value):
        # this assumes that item strings are unique...
        root = self.tree.GetRootItem()
        if not root:
            return
        found = self.FindItem(root, value)
        if found:
            self.value = found
            self.tree.SelectItem(found)


    def GetAdjustedSize(self, minWidth, prefHeight, maxHeight):
        return wx.Size(minWidth, min(200, maxHeight))
                       

    # helpers
    
    def FindItem(self, parentItem, text):        
        item, cookie = self.tree.GetFirstChild(parentItem)
        while item:
            if self.tree.GetItemText(item) == text:
                return item
            if self.tree.ItemHasChildren(item):
                item = self.FindItem(item, text)
            item, cookie = self.tree.GetNextChild(parentItem, cookie)
        return wx.TreeItemId();


    def AddItem(self, value, parent=None):
        if not parent:
            root = self.tree.GetRootItem()
            if not root:
                root = self.tree.AddRoot("<hidden root>")
            parent = root

        item = self.tree.AppendItem(parent, value)
        return item

    
    def OnMotion(self, evt):
        # have the selection follow the mouse, like in a real combobox
        item, flags = self.tree.HitTest(evt.GetPosition())
        if item and flags & wx.TREE_HITTEST_ONITEMLABEL:
            self.tree.SelectItem(item)
            self.curitem = item
        evt.Skip()


    def OnLeftDown(self, evt):
        # do the combobox selection
        item, flags = self.tree.HitTest(evt.GetPosition())
        if item and flags & wx.TREE_HITTEST_ONITEMLABEL:
            self.curitem = item
            self.value = item
            self.Dismiss()
        evt.Skip()
        
        
#----------------------------------------------------------------------
# Here we subclass wx.combo.ComboCtrl to do some custom popup animation

CUSTOM_COMBOBOX_ANIMATION_DURATION = 200

class ComboCtrlWithCustomPopupAnim(wx.combo.ComboCtrl):
    def __init__(self, *args, **kw):
        wx.combo.ComboCtrl.__init__(self, *args, **kw)
        self.Bind(wx.EVT_TIMER, self.OnTimer)
        self.aniTimer = wx.Timer(self)


    def AnimateShow(self, rect, flags):
        self.aniStart = wx.GetLocalTimeMillis()
        self.aniRect = wx.Rect(*rect)
        self.aniFlags = flags

        dc = wx.ScreenDC()
        bmp = wx.EmptyBitmap(rect.width, rect.height)
        mdc = wx.MemoryDC(bmp)
        if "wxMac" in wx.PlatformInfo:
            pass
        else:
            mdc.Blit(0, 0, rect.width, rect.height, dc, rect.x, rect.y)
        del mdc
        self.aniBackBitmap = bmp

        self.aniTimer.Start(10, wx.TIMER_CONTINUOUS)
        self.OnTimer(None)
        return False
        

    def OnTimer(self, evt):
        stopTimer = False
        popup = self.GetPopupControl().GetControl()
        rect = self.aniRect
        dc = wx.ScreenDC()

        if self.IsPopupWindowState(self.Hidden):
            stopTimer = True
        else:
            pos = wx.GetLocalTimeMillis() - self.aniStart
            if pos < CUSTOM_COMBOBOX_ANIMATION_DURATION:
                # Actual animation happens here
                width = rect.width
                height = rect.height

                center_x = rect.x + (width/2)
                center_y = rect.y + (height/2)

                dc.SetPen( wx.BLACK_PEN )
                dc.SetBrush( wx.TRANSPARENT_BRUSH )

                w = (((pos*256)/CUSTOM_COMBOBOX_ANIMATION_DURATION)*width)/256
                ratio = float(w) / float(width)
                h = int(height * ratio)
                
                dc.DrawBitmap( self.aniBackBitmap, rect.x, rect.y )
                dc.DrawRectangle( center_x - w/2, center_y - h/2, w, h )
            else:
                stopTimer = True

        if stopTimer:
            dc.DrawBitmap( self.aniBackBitmap, rect.x, rect.y )
            popup.Move( (0, 0) )
            self.aniTimer.Stop()
            self.DoShowPopup( rect, self.aniFlags )

                
#----------------------------------------------------------------------
# FileSelectorCombo displays a dialog instead of a popup control, it
# also uses a custom bitmap on the combo button.

class FileSelectorCombo(wx.combo.ComboCtrl):
    def __init__(self, *args, **kw):
        wx.combo.ComboCtrl.__init__(self, *args, **kw)

        # make a custom bitmap showing "..."
        bw, bh = 14, 16
        bmp = wx.EmptyBitmap(bw,bh)
        dc = wx.MemoryDC(bmp)

        # clear to a specific background colour
        bgcolor = wx.Colour(255,254,255)
        dc.SetBackground(wx.Brush(bgcolor))
        dc.Clear()

        # draw the label onto the bitmap
        label = "..."
        font = wx.SystemSettings.GetFont(wx.SYS_DEFAULT_GUI_FONT)
        font.SetWeight(wx.FONTWEIGHT_BOLD)
        dc.SetFont(font)
        tw,th = dc.GetTextExtent(label)
        dc.DrawText(label, (bw-tw)/2, (bw-tw)/2)
        del dc

        # now apply a mask using the bgcolor
        bmp.SetMaskColour(bgcolor)

        # and tell the ComboCtrl to use it
        self.SetButtonBitmaps(bmp, True)
        

    # Overridden from ComboCtrl, called when the combo button is clicked
    def OnButtonClick(self):
        path = ""
        name = ""
        if self.GetValue():
            path, name = os.path.split(self.GetValue())
        
        dlg = wx.FileDialog(self, "Choose File", path, name,
                            "All files (*.*)|*.*", wx.FD_OPEN)
        if dlg.ShowModal() == wx.ID_OK:
            self.SetValue(dlg.GetPath())
        dlg.Destroy()
        self.SetFocus()

    # Overridden from ComboCtrl to avoid assert since there is no ComboPopup
    def DoSetPopupControl(self, popup):
        pass
    

#----------------------------------------------------------------------


class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        fgs = wx.FlexGridSizer(cols=3, hgap=10, vgap=10)

        cc = self.MakeLCCombo(log=self.log)
        fgs.Add(cc)
        fgs.Add((10,10))
        fgs.Add(wx.StaticText(self, -1, "wx.ComboCtrl with a ListCtrl popup"))

        cc = self.MakeLCCombo(style=wx.CB_READONLY)
        fgs.Add(cc)
        fgs.Add((10,10))
        fgs.Add(wx.StaticText(self, -1, "        Read-only"))

        cc = self.MakeLCCombo()
        cc.SetButtonPosition(side=wx.LEFT)
        fgs.Add(cc)
        fgs.Add((10,10))
        fgs.Add(wx.StaticText(self, -1, "        Button on the left"))

        cc = self.MakeLCCombo()
        cc.SetPopupMaxHeight(250)
        fgs.Add(cc)
        fgs.Add((10,10))
        fgs.Add(wx.StaticText(self, -1, "        Max height of popup set"))

        cc = wx.combo.ComboCtrl(self, size=(250,-1))
        tcp = TreeCtrlComboPopup()
        cc.SetPopupControl(tcp)
        fgs.Add(cc)
        fgs.Add((10,10))
        fgs.Add(wx.StaticText(self, -1, "TreeCtrl popup"))
        # add some items to the tree
        for i in range(5):
            item = tcp.AddItem('Item %d' % (i+1))
            for j in range(15):
                tcp.AddItem('Subitem %d-%d' % (i+1, j+1), parent=item)
                
        cc = ComboCtrlWithCustomPopupAnim(self, size=(250, -1))
        popup = ListCtrlComboPopup()
        cc.SetPopupMaxHeight(150)
        cc.SetPopupControl(popup)
        fgs.Add(cc)
        fgs.Add((10,10))
        fgs.Add(wx.StaticText(self, -1, "Custom popup animation"))
        for word in "How cool was that!?  Way COOL!".split():
            popup.AddItem(word)
        if "wxMac" in wx.PlatformInfo:
            cc.SetValue("Sorry, animation not working yet on Mac")


        cc = FileSelectorCombo(self, size=(250, -1))
        fgs.Add(cc)
        fgs.Add((10,10))
        fgs.Add(wx.StaticText(self, -1, "Custom popup action, and custom button bitmap"))
        

        box = wx.BoxSizer()
        box.Add(fgs, 1, wx.EXPAND|wx.ALL, 20)
        self.SetSizer(box)


    def MakeLCCombo(self, log=None, style=0):
        # Create a ComboCtrl
        cc = wx.combo.ComboCtrl(self, style=style, size=(250,-1))
        
        # Create a Popup
        popup = ListCtrlComboPopup(log)

        # Associate them with each other.  This also triggers the
        # creation of the ListCtrl.
        cc.SetPopupControl(popup)

        # Add some items to the listctrl.
        for x in range(75):
            popup.AddItem("Item-%02d" % x)

        return cc
        

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>wx.combo.ComboCtrl</center></h2>

A combo control is a generic combobox that allows a totally custom
popup. In addition it has other customization features. For instance,
position and size of the dropdown button can be changed.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])
