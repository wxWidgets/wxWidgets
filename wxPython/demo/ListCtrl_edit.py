#----------------------------------------------------------------------------
# Name:         ListCtrl_edit.py
# Purpose:      Testing editing a ListCtrl
#
# Author:       Pim van Heuven
#
# Created:      2004/10/15
# Copyright:    (c) Pim Van Heuven
# Licence:      wxWindows license
#----------------------------------------------------------------------------

import sys
import  wx
import  wx.lib.mixins.listctrl  as  listmix

#---------------------------------------------------------------------------

listctrldata = {
1 : ("Hey!", "You can edit", "me!"),
2 : ("Try changing the contents", "by", "clicking"),
3 : ("in", "a", "cell"),
4 : ("See how the length columns", "change", "?"),
5 : ("You can use", "TAB,", "cursor down,"),
6 : ("and cursor up", "to", "navigate"),
}

#---------------------------------------------------------------------------

class TestListCtrl(wx.ListCtrl,
                   listmix.ListCtrlAutoWidthMixin,
                   listmix.TextEditMixin):

    def __init__(self, parent, ID, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=0):
        wx.ListCtrl.__init__(self, parent, ID, pos, size, style)

        listmix.ListCtrlAutoWidthMixin.__init__(self)
        self.Populate()
        listmix.TextEditMixin.__init__(self)

    def Populate(self):
        # for normal, simple columns, you can add them like this:
        self.InsertColumn(0, "Column 1")
        self.InsertColumn(1, "Column 2")
        self.InsertColumn(2, "Column 3")
        self.InsertColumn(3, "Len 1", wx.LIST_FORMAT_RIGHT)
        self.InsertColumn(4, "Len 2", wx.LIST_FORMAT_RIGHT)
        self.InsertColumn(5, "Len 3", wx.LIST_FORMAT_RIGHT)

        items = listctrldata.items()
        for key, data in items:
            index = self.InsertStringItem(sys.maxint, data[0])
            self.SetStringItem(index, 0, data[0])
            self.SetStringItem(index, 1, data[1])
            self.SetStringItem(index, 2, data[2])
            self.SetItemData(index, key)

        self.SetColumnWidth(0, wx.LIST_AUTOSIZE)
        self.SetColumnWidth(1, wx.LIST_AUTOSIZE)
        self.SetColumnWidth(2, 100)

        self.currentItem = 0


    def SetStringItem(self, index, col, data):
        if col in range(3):
            wx.ListCtrl.SetStringItem(self, index, col, data)
            wx.ListCtrl.SetStringItem(self, index, 3+col, str(len(data)))
        else:
            try:
                datalen = int(data)
            except:
                return

            wx.ListCtrl.SetStringItem(self, index, col, data)

            data = self.GetItem(index, col-3).GetText()
            wx.ListCtrl.SetStringItem(self, index, col-3, data[0:datalen])




class TestListCtrlPanel(wx.Panel):
    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent, -1, style=wx.WANTS_CHARS)

        self.log = log
        tID = wx.NewId()

        sizer = wx.BoxSizer(wx.VERTICAL)
        
        if wx.Platform == "__WXMAC__" and \
               hasattr(wx.GetApp().GetTopWindow(), "LoadDemo"):
            self.useNative = wx.CheckBox(self, -1, "Use native listctrl")
            self.useNative.SetValue( 
                not wx.SystemOptions.GetOptionInt("mac.listctrl.always_use_generic") )
            self.Bind(wx.EVT_CHECKBOX, self.OnUseNative, self.useNative)
            sizer.Add(self.useNative, 0, wx.ALL | wx.ALIGN_RIGHT, 4)

        self.list = TestListCtrl(self, tID,
                                 style=wx.LC_REPORT
                                 | wx.BORDER_NONE
                                 | wx.LC_SORT_ASCENDING
                                 )

        sizer.Add(self.list, 1, wx.EXPAND)
        self.SetSizer(sizer)
        self.SetAutoLayout(True)


    def OnUseNative(self, event):
        wx.SystemOptions.SetOptionInt("mac.listctrl.always_use_generic", not event.IsChecked())
        wx.GetApp().GetTopWindow().LoadDemo("ListCtrl_edit")



#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestListCtrlPanel(nb, log)
    return win

#---------------------------------------------------------------------------


overview = """\
<html>
<body>

This demo demonstrates direct editing of all cells of a ListCtrl. To
enable it just include the <b>TextEditMixin</b>. The ListCtrl can be
navigated with the TAB and up/down cursor keys.

<p>Another facet of this demo is that the remaining space of the
ListCtrls is divided over the first three columns. This is achieved
with the extended syntax of <b>ListCtrlAutoWidthMixin</b>:
<code>listmix.ListCtrlAutoWidthMixin.__init__(self, startcol, endcol)</code>
(Look at the general ListCtrl demo for more information about the
ListCtrlAutoWidthMixin)

<p>Finally, the ListCtrl is automatically scrolled, if needed, when
TAB is pressed consecutively (Windows only).

</body>
</html>
"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

