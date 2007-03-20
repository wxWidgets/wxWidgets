__author__  = "E. A. Tacao <e.a.tacao |at| estadao.com.br>"
__date__    = "12 Fev 2006, 22:00 GMT-03:00"
__version__ = "0.03"
__doc__     = """
ButtonTreeCtrlPanel is a widget where one can place check buttons, tri-state
check buttons, radio buttons, both, and the ability to display them
hierarchically.


About:

ButtonTreeCtrlPanel is distributed under the wxWidgets license.

For all kind of problems, requests, enhancements, bug reports, etc,
please drop me an e-mail.

For updates please visit <http://j.domaindlx.com/elements28/wxpython/>.
"""

import cStringIO

import wx
from wx.lib.newevent import NewEvent

#----------------------------------------------------------------------------

(ButtonTreeCtrlPanelEvent, EVT_BUTTONTREECTRLPANEL) = NewEvent()
EVT_CHANGED = EVT_BUTTONTREECTRLPANEL

#----------------------------------------------------------------------------

class ButtonTreeCtrlPanel(wx.Panel):
    def __init__(self, parent, id=wx.ID_ANY, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.WANTS_CHARS):
        wx.Panel.__init__(self, parent, id, pos, size, style)

        self.tree = wx.TreeCtrl(self, style=wx.TR_NO_LINES|wx.TR_HIDE_ROOT)

        il = self.il = wx.ImageList(16, 16)
        self.tree.SetImageList(il)

        for bl in ["checkbox_checked", "checkbox_unchecked", "checkbox_tri",
                   "radiobox_checked", "radiobox_unchecked"]:
            setattr(self, bl, il.Add(getattr(self, "get%sBitmap" % bl)()))

        bmp = wx.ArtProvider.GetBitmap(wx.ART_FOLDER, wx.ART_TOOLBAR, (16, 16))
        self.empty_bitmap = il.Add(bmp)

        self.root = self.tree.AddRoot("Root Item for ButtonTreeCtrlPanel")

        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.tree.Bind(wx.EVT_LEFT_DCLICK, self.OnLeftClicks)
        self.tree.Bind(wx.EVT_LEFT_DOWN, self.OnLeftClicks)
        self.tree.Bind(wx.EVT_RIGHT_DOWN, self.OnRightClick)

        self.allitems = []

        wx.CallAfter(self.OnSize)


    def _doLogicTest(self, style, value, item):
        if style in [wx.CHK_2STATE, wx.CHK_3STATE]:
            n = [self.checkbox_unchecked, self.checkbox_checked, \
                 self.checkbox_tri][value]

            self.tree.SetPyData(item, (value, style))
            self.tree.SetItemImage(item, n, wx.TreeItemIcon_Normal)

        elif style == wx.RB_SINGLE:
            if value:
                parent = self.tree.GetItemParent(item)
                (child, cookie) = self.tree.GetFirstChild(parent)

                if self.tree.GetPyData(child):
                    self.tree.SetPyData(child, (False, wx.RB_SINGLE))
                    self.tree.SetItemImage(child, self.radiobox_unchecked, \
                                           wx.TreeItemIcon_Normal)

                for x in range(1, self.tree.GetChildrenCount(parent, False)):
                    (child, cookie) = self.tree.GetNextChild(parent, cookie)

                    if self.tree.GetPyData(child):
                        self.tree.SetPyData(child, (False, wx.RB_SINGLE))
                        self.tree.SetItemImage(child, self.radiobox_unchecked, \
                                               wx.TreeItemIcon_Normal)

                self.tree.SetPyData(item, (True, wx.RB_SINGLE))
                self.tree.SetItemImage(item, self.radiobox_checked, \
                                       wx.TreeItemIcon_Normal)

            else:
                self.tree.SetPyData(item, (False, wx.RB_SINGLE))
                self.tree.SetItemImage(item, self.radiobox_unchecked, \
                                       wx.TreeItemIcon_Normal)


    def _getItems(self, parent=None, value=None):
        if not parent:
            parent = self.root
        cil = []
        (child, cookie) = self.tree.GetFirstChild(parent)
        if child.IsOk():
            d = self.tree.GetPyData(child)
            if value is None or (d and d[0] == value):
                cil.append(child)
            for x in range(1, self.tree.GetChildrenCount(parent, False)):
                (child, cookie) = self.tree.GetNextChild(parent, cookie)
                if child.IsOk():
                    d = self.tree.GetPyData(child)
                    if value is None or (d and d[0] == value):
                        cil.append(child)
        return cil


    def AddItem(self, label, bmp=None, parent=None, style=None, value=False):
        v = None

        if bmp:
            n = self.il.Add(bmp)
        if not parent:
            parent = self.root
        if style is not None:
            v = (value, style)

        this_item = self.tree.AppendItem(parent, label)
        self.tree.SetPyData(this_item, v)

        if v:
            self._doLogicTest(style, value, this_item)
        else:
            if bmp is None:
                bmp = self.empty_bitmap
            else:
                bmp = self.il.Add(bmp)

            self.tree.SetItemImage(this_item, bmp, wx.TreeItemIcon_Normal)

        self.allitems.append(this_item)
        [self.tree.Expand(x) for x in self.allitems]

        return this_item


    def ExpandItem(self, item):
        self.tree.Expand(item)


    def CollapseItem(self, item):
        self.tree.Collapse(item)


    def EnsureFirstVisible(self):
        (child, cookie) = self.tree.GetFirstChild(self.root)
        if child.IsOk():
            self.tree.SelectItem(child)
            self.tree.EnsureVisible(child)


    def SetItemValue(self, item, value):
        data = self.tree.GetPyData(item)
        if data:
            self._doLogicTest(data[1], value, item)


    def GetItemValue(self, item):
        data = self.tree.GetPyData(item)
        if data:
            return data[0]
        else:
            return None


    def GetItemByLabel(self, label, parent=None):
        r = None
        for item in self._getItems(parent):
            if self.tree.GetItemText(item) == label:
                r = item; break
        return r


    def GetAllItems(self):
        return self.allitems


    def GetRootItems(self):
        cil = []
        for x in range(0, len(self.allitems)):
            d = self.tree.GetPyData(self.allitems[x])
            if not d:
                cil.append(self.allitems[x])
        return cil


    def GetStringRootItems(self):
        return [self.tree.GetItemText(x) for x in self.GetRootItems]


    def GetItemsUnchecked(self, parent=None):
        return self._getItems(parent, 0)


    def GetItemsChecked(self, parent=None):
        return self._getItems(parent, 1)


    def GetItemsTri(self, parent=None):
        return self._getItems(parent, 2)


    def GetStringItemsUnchecked(self, parent=None):
        return [self.tree.GetItemText(x) \
                for x in self.GetItemsUnchecked(parent)]


    def GetStringItemsChecked(self, parent=None):
        return [self.tree.GetItemText(x) for x in self.GetItemsChecked(parent)]


    def GetStringItemsTri(self, parent=None):
        return [self.tree.GetItemText(x) for x in self.GetItemsTri(parent)]


    def OnRightClick(self, evt):
        item, flags = self.tree.HitTest(evt.GetPosition())
        self.tree.SelectItem(item)


    def OnLeftClicks(self, evt):
        item, flags = self.tree.HitTest(evt.GetPosition())
        if item:
            text, data = self.tree.GetItemText(item), self.tree.GetPyData(item)
            if data:
                style = data[1]
                if style == wx.CHK_2STATE:
                    value = not data[0]
                elif style == wx.CHK_3STATE:
                    value = data[0] + 1
                    if value == 3: value = 0
                else:
                    value = True

                self._doLogicTest(style, value, item)

                if value <> data[0]:
                    nevt = ButtonTreeCtrlPanelEvent(obj=self, id=self.GetId(),
                                                    item=item, val=value)
                    wx.PostEvent(self, nevt)

        evt.Skip()


    def OnSize(self, evt=None):
        self.tree.SetSize(self.GetClientSize())

    # # Images generated by encode_bitmaps.py -----------------------------

    def getcheckbox_uncheckedData(self):
        return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x00AIDAT8\x8d\xed\x93;\x0e\x00 \x14\xc2\xca\xd3\xfb\xdf\xd8\xcf\xac\x83\
\x89\xe2dd\xa7\xe9\x00R$\x9c\x84\xd5\x062@\xab\xa5\xed\x16\x15I\xf7\x0cf\xea\
*\xb3\xadm\xf0\x01O\x00\x86!\x9dL\xda6\x90{\xe7\x0e\xbf\x98\x0c*\xf3\x13\xe2\
e\x00\x00\x00\x00IEND\xaeB`\x82'

    def getcheckbox_uncheckedBitmap(self):
        return wx.BitmapFromImage(self.getcheckbox_uncheckedImage())

    def getcheckbox_uncheckedImage(self):
        stream = cStringIO.StringIO(self.getcheckbox_uncheckedData())
        return wx.ImageFromStream(stream)

    #----------------------------------------------------------------------
    def getradiobox_checkedData(self):
        return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x00qIDAT8\x8d\xadR\xd1\x12\x80 \x08\x03\xed\xff\xffX\xe9\xa9n\xd1\xe6\
\xd9\x11o\xca6\x18\xe0\xde\xbaU\xa2\x95\xd8fv\xa8D\xcc\x11\xf8\xf6\xd6\x9d\
\xe1<[\xc8\xc4\x17!\t=\x04\x90\x9c\x81*Gg\xc0\xdaU\x16n\x81\xab\x02\x02c\x8e\
`\x95\xf1On\x01A\xab\xb9\x94\xd7(\x05\xd0\x8a\xf2o&\xb6\xa0\x08,O;`\x9e\xd5\
\x1c\xfe=\xa4\x95\xd0\xf6)\x7f\x8d\xf2\x1aO_y@8\xb08r\xf9\x00\x00\x00\x00IEN\
D\xaeB`\x82'

    def getradiobox_checkedBitmap(self):
        return wx.BitmapFromImage(self.getradiobox_checkedImage())

    def getradiobox_checkedImage(self):
        stream = cStringIO.StringIO(self.getradiobox_checkedData())
        return wx.ImageFromStream(stream)

    #----------------------------------------------------------------------
    def getradiobox_uncheckedData(self):
        return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x00gIDAT8\x8d\xcd\x92K\x12\x800\x08C\x81z\xff\x1bk\xbb\xd2\xc1\x98t\xea\
\xe0Bv\x1d\xc8k\xf8\xb8G\xb3JDImf\x9bJ\xf4c\xef\xf9\xed\xd1\x9c\xd59\xb6\x80\
\xc2\x87\x00@7@\x16c\xa1\xca\xd1\x190\xbb\xaa\x85\x0bp\xfe\xa0\ns.\xbb)o\xe1\
G\x00\xd6\x1f\x06\x9b\x13u\xc0 \n\xfc\xed!\xcd@\xcb\xa7\xfc6\xca[\x18\xdf\
\x0e0Ao\x7fq\xe8\x00\x00\x00\x00IEND\xaeB`\x82'

    def getradiobox_uncheckedBitmap(self):
        return wx.BitmapFromImage(self.getradiobox_uncheckedImage())

    def getradiobox_uncheckedImage(self):
        stream = cStringIO.StringIO(self.getradiobox_uncheckedData())
        return wx.ImageFromStream(stream)

    #----------------------------------------------------------------------
    def getcheckbox_checkedData(self):
        return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x00gIDAT8\x8d\xad\x93Q\x0e\x800\x08C[\xe6\xfdo\xac\xf3k\x89\xa9\xb01\
\x91o^S\xa0\x90\xd6P)+\xd1\x00\x0e\x00\xe8\xd7\xd9wAZ\xe3\x7f\x0eTuV\xea6\
\xed \x1a3%0\xdb\xd1K@\x9bW\x0bv\x1d\x0c\xe8\tG\xfb\tG\xc8\xc0\xae\x806\xaf.\
\xe3:\x18P\xe6\xac\xe1\x08\x19\x18\x90 }\x89t9\xca\xac\xbe\xf3\r?\xc9(*%&\
\xec\x9b\x00\x00\x00\x00IEND\xaeB`\x82'

    def getcheckbox_checkedBitmap(self):
        return wx.BitmapFromImage(self.getcheckbox_checkedImage())

    def getcheckbox_checkedImage(self):
        stream = cStringIO.StringIO(self.getcheckbox_checkedData())
        return wx.ImageFromStream(stream)

    #----------------------------------------------------------------------
    def getcheckbox_triData(self):
        return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x00pIDAT8\x8d\xad\x93A\x0e\x800\x08\x04\x97\xea\xab\xd4\xa3\xfah\xf5h\
\xfdV{jb\x08 \x8a\x9c\x99\xcd\xd0l\x89R\x87\xc8\xa4\x10\r\xa0\x07\x80i\x1c\
\xca[\xf0\xcc\x17\xfdg\xc0S\xad\xe1\xb6n\x83c\xdf\xc43]\x01\x1a,\x06\xf0e\
\x0bV\r\x1at\x87\xe7e\x15\xdfG=\xc1\x03\x8b\x01|\xd9\x82U\x83\x06=\xc1j\x80\
\x17\x06X\x91\xbeT:\\e\x8a~\xe7\nM\x85%\xe1\xceT8v\x00\x00\x00\x00IEND\xaeB`\
\x82'

    def getcheckbox_triBitmap(self):
        return wx.BitmapFromImage(self.getcheckbox_triImage())

    def getcheckbox_triImage(self):
        stream = cStringIO.StringIO(self.getcheckbox_triData())
        return wx.ImageFromStream(stream)


#
##
### eof
