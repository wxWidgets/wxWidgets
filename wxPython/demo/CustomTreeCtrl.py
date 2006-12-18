import wx
import string
import os
import wx.lib.colourselect as csel

import wx.lib.customtreectrl as CT

from Main import opj
import images

#---------------------------------------------------------------------------


penstyle = ["wx.SOLID", "wx.TRANSPARENT", "wx.DOT", "wx.LONG_DASH", "wx.DOT_DASH", "wx.USER_DASH",
           "wx.BDIAGONAL_HATCH", "wx.CROSSDIAG_HATCH", "wx.FDIAGONAL_HATCH", "wx.CROSS_HATCH",
           "wx.HORIZONTAL_HATCH", "wx.VERTICAL_HATCH"]

ArtIDs = [ "None",
           "wx.ART_ADD_BOOKMARK",
           "wx.ART_DEL_BOOKMARK",
           "wx.ART_HELP_SIDE_PANEL",
           "wx.ART_HELP_SETTINGS",
           "wx.ART_HELP_BOOK",
           "wx.ART_HELP_FOLDER",
           "wx.ART_HELP_PAGE",
           "wx.ART_GO_BACK",
           "wx.ART_GO_FORWARD",
           "wx.ART_GO_UP",
           "wx.ART_GO_DOWN",
           "wx.ART_GO_TO_PARENT",
           "wx.ART_GO_HOME",
           "wx.ART_FILE_OPEN",
           "wx.ART_PRINT",
           "wx.ART_HELP",
           "wx.ART_TIP",
           "wx.ART_REPORT_VIEW",
           "wx.ART_LIST_VIEW",
           "wx.ART_NEW_DIR",
           "wx.ART_HARDDISK",
           "wx.ART_FLOPPY",
           "wx.ART_CDROM",
           "wx.ART_REMOVABLE",
           "wx.ART_FOLDER",
           "wx.ART_FOLDER_OPEN",
           "wx.ART_GO_DIR_UP",
           "wx.ART_EXECUTABLE_FILE",
           "wx.ART_NORMAL_FILE",
           "wx.ART_TICK_MARK",
           "wx.ART_CROSS_MARK",
           "wx.ART_ERROR",
           "wx.ART_QUESTION",
           "wx.ART_WARNING",
           "wx.ART_INFORMATION",
           "wx.ART_MISSING_IMAGE",
           "SmileBitmap"
           ]

keyMap = {
    wx.WXK_BACK : "WXK_BACK",
    wx.WXK_TAB : "WXK_TAB",
    wx.WXK_RETURN : "WXK_RETURN",
    wx.WXK_ESCAPE : "WXK_ESCAPE",
    wx.WXK_SPACE : "WXK_SPACE",
    wx.WXK_DELETE : "WXK_DELETE",
    wx.WXK_START : "WXK_START",
    wx.WXK_LBUTTON : "WXK_LBUTTON",
    wx.WXK_RBUTTON : "WXK_RBUTTON",
    wx.WXK_CANCEL : "WXK_CANCEL",
    wx.WXK_MBUTTON : "WXK_MBUTTON",
    wx.WXK_CLEAR : "WXK_CLEAR",
    wx.WXK_SHIFT : "WXK_SHIFT",
    wx.WXK_ALT : "WXK_ALT",
    wx.WXK_CONTROL : "WXK_CONTROL",
    wx.WXK_MENU : "WXK_MENU",
    wx.WXK_PAUSE : "WXK_PAUSE",
    wx.WXK_CAPITAL : "WXK_CAPITAL",
    wx.WXK_PRIOR : "WXK_PRIOR",
    wx.WXK_NEXT : "WXK_NEXT",
    wx.WXK_END : "WXK_END",
    wx.WXK_HOME : "WXK_HOME",
    wx.WXK_LEFT : "WXK_LEFT",
    wx.WXK_UP : "WXK_UP",
    wx.WXK_RIGHT : "WXK_RIGHT",
    wx.WXK_DOWN : "WXK_DOWN",
    wx.WXK_SELECT : "WXK_SELECT",
    wx.WXK_PRINT : "WXK_PRINT",
    wx.WXK_EXECUTE : "WXK_EXECUTE",
    wx.WXK_SNAPSHOT : "WXK_SNAPSHOT",
    wx.WXK_INSERT : "WXK_INSERT",
    wx.WXK_HELP : "WXK_HELP",
    wx.WXK_NUMPAD0 : "WXK_NUMPAD0",
    wx.WXK_NUMPAD1 : "WXK_NUMPAD1",
    wx.WXK_NUMPAD2 : "WXK_NUMPAD2",
    wx.WXK_NUMPAD3 : "WXK_NUMPAD3",
    wx.WXK_NUMPAD4 : "WXK_NUMPAD4",
    wx.WXK_NUMPAD5 : "WXK_NUMPAD5",
    wx.WXK_NUMPAD6 : "WXK_NUMPAD6",
    wx.WXK_NUMPAD7 : "WXK_NUMPAD7",
    wx.WXK_NUMPAD8 : "WXK_NUMPAD8",
    wx.WXK_NUMPAD9 : "WXK_NUMPAD9",
    wx.WXK_MULTIPLY : "WXK_MULTIPLY",
    wx.WXK_ADD : "WXK_ADD",
    wx.WXK_SEPARATOR : "WXK_SEPARATOR",
    wx.WXK_SUBTRACT : "WXK_SUBTRACT",
    wx.WXK_DECIMAL : "WXK_DECIMAL",
    wx.WXK_DIVIDE : "WXK_DIVIDE",
    wx.WXK_F1 : "WXK_F1",
    wx.WXK_F2 : "WXK_F2",
    wx.WXK_F3 : "WXK_F3",
    wx.WXK_F4 : "WXK_F4",
    wx.WXK_F5 : "WXK_F5",
    wx.WXK_F6 : "WXK_F6",
    wx.WXK_F7 : "WXK_F7",
    wx.WXK_F8 : "WXK_F8",
    wx.WXK_F9 : "WXK_F9",
    wx.WXK_F10 : "WXK_F10",
    wx.WXK_F11 : "WXK_F11",
    wx.WXK_F12 : "WXK_F12",
    wx.WXK_F13 : "WXK_F13",
    wx.WXK_F14 : "WXK_F14",
    wx.WXK_F15 : "WXK_F15",
    wx.WXK_F16 : "WXK_F16",
    wx.WXK_F17 : "WXK_F17",
    wx.WXK_F18 : "WXK_F18",
    wx.WXK_F19 : "WXK_F19",
    wx.WXK_F20 : "WXK_F20",
    wx.WXK_F21 : "WXK_F21",
    wx.WXK_F22 : "WXK_F22",
    wx.WXK_F23 : "WXK_F23",
    wx.WXK_F24 : "WXK_F24",
    wx.WXK_NUMLOCK : "WXK_NUMLOCK",
    wx.WXK_SCROLL : "WXK_SCROLL",
    wx.WXK_PAGEUP : "WXK_PAGEUP",
    wx.WXK_PAGEDOWN : "WXK_PAGEDOWN",
    wx.WXK_NUMPAD_SPACE : "WXK_NUMPAD_SPACE",
    wx.WXK_NUMPAD_TAB : "WXK_NUMPAD_TAB",
    wx.WXK_NUMPAD_ENTER : "WXK_NUMPAD_ENTER",
    wx.WXK_NUMPAD_F1 : "WXK_NUMPAD_F1",
    wx.WXK_NUMPAD_F2 : "WXK_NUMPAD_F2",
    wx.WXK_NUMPAD_F3 : "WXK_NUMPAD_F3",
    wx.WXK_NUMPAD_F4 : "WXK_NUMPAD_F4",
    wx.WXK_NUMPAD_HOME : "WXK_NUMPAD_HOME",
    wx.WXK_NUMPAD_LEFT : "WXK_NUMPAD_LEFT",
    wx.WXK_NUMPAD_UP : "WXK_NUMPAD_UP",
    wx.WXK_NUMPAD_RIGHT : "WXK_NUMPAD_RIGHT",
    wx.WXK_NUMPAD_DOWN : "WXK_NUMPAD_DOWN",
    wx.WXK_NUMPAD_PRIOR : "WXK_NUMPAD_PRIOR",
    wx.WXK_NUMPAD_PAGEUP : "WXK_NUMPAD_PAGEUP",
    wx.WXK_NUMPAD_NEXT : "WXK_NUMPAD_NEXT",
    wx.WXK_NUMPAD_PAGEDOWN : "WXK_NUMPAD_PAGEDOWN",
    wx.WXK_NUMPAD_END : "WXK_NUMPAD_END",
    wx.WXK_NUMPAD_BEGIN : "WXK_NUMPAD_BEGIN",
    wx.WXK_NUMPAD_INSERT : "WXK_NUMPAD_INSERT",
    wx.WXK_NUMPAD_DELETE : "WXK_NUMPAD_DELETE",
    wx.WXK_NUMPAD_EQUAL : "WXK_NUMPAD_EQUAL",
    wx.WXK_NUMPAD_MULTIPLY : "WXK_NUMPAD_MULTIPLY",
    wx.WXK_NUMPAD_ADD : "WXK_NUMPAD_ADD",
    wx.WXK_NUMPAD_SEPARATOR : "WXK_NUMPAD_SEPARATOR",
    wx.WXK_NUMPAD_SUBTRACT : "WXK_NUMPAD_SUBTRACT",
    wx.WXK_NUMPAD_DECIMAL : "WXK_NUMPAD_DECIMAL",
    wx.WXK_NUMPAD_DIVIDE : "WXK_NUMPAD_DIVIDE"
    }



#---------------------------------------------------------------------------
# Just A Dialog To Select Pen Styles
#---------------------------------------------------------------------------
class PenDialog(wx.Dialog):
    
    def __init__(self, parent=None, id=-1, title="", pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.DEFAULT_DIALOG_STYLE, oldpen=None,
                 pentype=0):

        wx.Dialog.__init__(self, parent, id, title, pos, size, style)
        
        self.colourbutton = csel.ColourSelect(self)
        self.spinwidth = wx.SpinCtrl(self, -1, "1", min=1, max=3, style=wx.SP_ARROW_KEYS)
        
        self.combostyle = wx.ComboBox(self, -1, choices=penstyle, style=wx.CB_DROPDOWN|wx.CB_READONLY)
        
        choices = ["[1, 1]", "[2, 2]", "[3, 3]", "[4, 4]"]
        self.combodash = wx.ComboBox(self, -1, choices=choices, style=wx.CB_DROPDOWN|wx.CB_READONLY)
        
        self.okbutton = wx.Button(self, wx.ID_OK)
        self.cancelbutton = wx.Button(self, wx.ID_CANCEL)

        self.oldpen = oldpen
        self.parent = parent
        self.pentype = pentype

        self.__set_properties()
        self.__do_layout()

        self.Bind(wx.EVT_COMBOBOX, self.OnStyle, self.combostyle)
        self.Bind(wx.EVT_BUTTON, self.OnOk, self.okbutton)
        self.Bind(wx.EVT_BUTTON, self.OnCancel, self.cancelbutton)


    def __set_properties(self):

        self.SetTitle("Pen Dialog Selector")
        self.colourbutton.SetMinSize((25, 25))
        self.colourbutton.SetColour(self.oldpen.GetColour())

        style = self.oldpen.GetStyle()
        for count, st in enumerate(penstyle):
            if eval(st) == style:                
                self.combostyle.SetSelection(count)
                if count == 5:
                    self.combodash.Enable(True)
                else:
                    self.combodash.Enable(False)
                break

        if self.combodash.IsEnabled():
            dashes = repr(self.oldpen.GetDashes())
            self.combodash.SetValue(dashes)

        self.spinwidth.SetValue(self.oldpen.GetWidth())
        self.okbutton.SetDefault()

        if self.pentype == 1:
            self.spinwidth.Enable(False)


    def __do_layout(self):

        mainsizer = wx.BoxSizer(wx.VERTICAL)
        bottomsizer = wx.BoxSizer(wx.HORIZONTAL)
        middlesizer = wx.BoxSizer(wx.VERTICAL)
        stylesizer = wx.BoxSizer(wx.HORIZONTAL)
        widthsizer = wx.BoxSizer(wx.HORIZONTAL)
        coloursizer = wx.BoxSizer(wx.HORIZONTAL)
        label_1 = wx.StaticText(self, -1, "Please Choose The Pen Settings:")
        label_1.SetFont(wx.Font(8, wx.DEFAULT, wx.NORMAL, wx.BOLD, 0, ""))
        mainsizer.Add(label_1, 0, wx.ALL|wx.ADJUST_MINSIZE, 10)
        label_2 = wx.StaticText(self, -1, "Pen Colour")
        coloursizer.Add(label_2, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        coloursizer.Add((5, 5), 1, wx.ADJUST_MINSIZE, 0)
        coloursizer.Add(self.colourbutton, 0, wx.ALL|wx.ADJUST_MINSIZE, 5)
        middlesizer.Add(coloursizer, 0, wx.EXPAND, 0)
        label_3 = wx.StaticText(self, -1, "Pen Width")
        widthsizer.Add(label_3, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        widthsizer.Add((5, 5), 1, wx.ADJUST_MINSIZE, 0)
        widthsizer.Add(self.spinwidth, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        middlesizer.Add(widthsizer, 0, wx.EXPAND, 0)
        label_4 = wx.StaticText(self, -1, "Pen Style")
        stylesizer.Add(label_4, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        stylesizer.Add((5, 5), 1, wx.ADJUST_MINSIZE, 0)
        stylesizer.Add(self.combostyle, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        stylesizer.Add(self.combodash, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        middlesizer.Add(stylesizer, 0, wx.BOTTOM|wx.EXPAND, 5)
        mainsizer.Add(middlesizer, 1, wx.EXPAND, 0)
        bottomsizer.Add(self.okbutton, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 20)
        bottomsizer.Add((20, 20), 1, wx.ADJUST_MINSIZE, 0)
        bottomsizer.Add(self.cancelbutton, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 20)
        mainsizer.Add(bottomsizer, 0, wx.EXPAND, 0)
        self.SetAutoLayout(True)
        self.SetSizer(mainsizer)
        mainsizer.Fit(self)
        mainsizer.SetSizeHints(self)
        self.Layout()
        self.Centre()




    def OnStyle(self, event):

        choice = event.GetEventObject().GetValue()
        self.combodash.Enable(choice==5)
        event.Skip()


    def OnOk(self, event):

        colour = self.colourbutton.GetColour()
        style = eval(self.combostyle.GetValue())
        width = int(self.spinwidth.GetValue())
        
        dashes = None
        if self.combostyle.GetSelection() == 5:
            dashes = eval(self.combodash.GetValue())

        pen = wx.Pen(colour, width, style)
        
        if dashes:
            pen.SetDashes(dashes)

        pen.SetCap(wx.CAP_BUTT)

        if self.pentype == 0:
            self.parent.SetConnectionPen(pen)
        else:
            self.parent.SetBorderPen(pen)
            
        self.Destroy()
        event.Skip()


    def OnCancel(self, event): 

        self.Destroy()    
        event.Skip()


#---------------------------------------------------------------------------
# Just A Dialog To Select Tree Buttons Icons
#---------------------------------------------------------------------------
class TreeButtonsDialog(wx.Dialog):
    
    def __init__(self, parent=None, id=-1, title="", pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.DEFAULT_DIALOG_STYLE, oldicons=None):
        
        wx.Dialog.__init__(self, parent, id, title, pos, size, style)

        self.listicons = wx.ListBox(self, -1, choices=["Set 1", "Set 2", "Set 3", "Set 4", "Set 5"], style=wx.LB_SINGLE)

        bitmap_plus = opj("bitmaps/plus" + str(oldicons+1) + ".ico")
        bitmap_minus = opj("bitmaps/minus" + str(oldicons+1) + ".ico")

        bitmap_plus = wx.Image(bitmap_plus, wx.BITMAP_TYPE_ICO)
        bitmap_plus.Rescale(24, 24)
        bitmap_plus = bitmap_plus.ConvertToBitmap()
        bitmap_minus = wx.Image(bitmap_minus, wx.BITMAP_TYPE_ICO)
        bitmap_minus.Rescale(24, 24)
        bitmap_minus = bitmap_minus.ConvertToBitmap()
        
        self.bitmap_plus = wx.StaticBitmap(self, -1, bitmap_plus)
        self.bitmap_minus = wx.StaticBitmap(self, -1, bitmap_minus)

        self.okbutton = wx.Button(self, wx.ID_OK)
        self.cancelbutton = wx.Button(self, wx.ID_CANCEL)

        self.parent = parent        

        self.__set_properties()
        self.__do_layout()

        self.Bind(wx.EVT_BUTTON, self.OnOk, self.okbutton)
        self.Bind(wx.EVT_BUTTON, self.OnCancel, self.cancelbutton)
        self.Bind(wx.EVT_LISTBOX, self.OnListBox, self.listicons)


    def __set_properties(self):

        self.SetTitle("Tree Buttons Selector")
        self.listicons.SetSelection(0)
        self.okbutton.SetDefault()


    def __do_layout(self):

        mainsizer = wx.BoxSizer(wx.VERTICAL)
        bottomsizer = wx.BoxSizer(wx.HORIZONTAL)
        topsizer = wx.BoxSizer(wx.HORIZONTAL)
        rightsizer = wx.BoxSizer(wx.VERTICAL)
        sizer_2 = wx.BoxSizer(wx.HORIZONTAL)
        sizer_1 = wx.BoxSizer(wx.HORIZONTAL)
        label_1 = wx.StaticText(self, -1, "Please Choose One Of These Sets Of Icons:")
        label_1.SetFont(wx.Font(8, wx.DEFAULT, wx.NORMAL, wx.BOLD, 0, ""))
        mainsizer.Add(label_1, 0, wx.ALL|wx.ADJUST_MINSIZE, 10)
        topsizer.Add(self.listicons, 0, wx.ALL|wx.EXPAND|wx.ADJUST_MINSIZE, 5)
        label_2 = wx.StaticText(self, -1, "Collapsed")
        sizer_1.Add(label_2, 0, wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 10)
        sizer_1.Add((20, 20), 1, wx.ALIGN_RIGHT|wx.ADJUST_MINSIZE, 0)
        sizer_1.Add(self.bitmap_plus, 1, wx.RIGHT|wx.EXPAND|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 10)
        rightsizer.Add(sizer_1, 0, wx.EXPAND, 0)
        label_3 = wx.StaticText(self, -1, "Expanded")
        sizer_2.Add(label_3, 0, wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 10)
        sizer_2.Add((20, 20), 1, wx.ADJUST_MINSIZE, 0)
        sizer_2.Add(self.bitmap_minus, 1, wx.RIGHT|wx.EXPAND|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 10)
        rightsizer.Add(sizer_2, 0, wx.EXPAND, 0)
        topsizer.Add(rightsizer, 0, wx.ALL|wx.EXPAND, 5)
        mainsizer.Add(topsizer, 1, wx.EXPAND, 0)
        bottomsizer.Add(self.okbutton, 0, wx.ALL|wx.ADJUST_MINSIZE, 20)
        bottomsizer.Add((20, 20), 1, wx.ADJUST_MINSIZE, 0)
        bottomsizer.Add(self.cancelbutton, 0, wx.ALL|wx.ADJUST_MINSIZE, 20)
        mainsizer.Add(bottomsizer, 0, wx.EXPAND, 0)
        self.SetAutoLayout(True)
        self.SetSizer(mainsizer)
        mainsizer.Fit(self)
        mainsizer.SetSizeHints(self)
        self.Layout()


    def OnListBox(self, event):

        selection = self.listicons.GetSelection()
        bitmap_plus = opj("bitmaps/plus" + str(selection+1) + ".ico")
        bitmap_minus = opj("bitmaps/minus" + str(selection+1) + ".ico")

        bitmap_plus = wx.Image(bitmap_plus, wx.BITMAP_TYPE_ICO)
        bitmap_plus.Rescale(24, 24)
        bitmap_plus = bitmap_plus.ConvertToBitmap()
        bitmap_minus = wx.Image(bitmap_minus, wx.BITMAP_TYPE_ICO)
        bitmap_minus.Rescale(24, 24)
        bitmap_minus = bitmap_minus.ConvertToBitmap()
        
        self.bitmap_plus.SetBitmap(bitmap_plus)        
        self.bitmap_minus.SetBitmap(bitmap_minus)

        self.bitmap_plus.Refresh()
        self.bitmap_minus.Refresh()
        event.Skip()
        

    def OnOk(self, event):

        selection = self.listicons.GetSelection()
        self.parent.SetTreeButtons(selection)
        self.Destroy()
        event.Skip()


    def OnCancel(self, event):

        self.Destroy()
        event.Skip()


#---------------------------------------------------------------------------
# Just A Dialog To Select Tree Check/Radio Item Icons
#---------------------------------------------------------------------------
class CheckDialog(wx.Dialog):
    
    def __init__(self, parent=None, id=-1, title="", pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.DEFAULT_DIALOG_STYLE):
        
        wx.Dialog.__init__(self, parent, id, title, pos, size, style)

        self.listicons = wx.ListBox(self, -1, choices=["Set 1", "Set 2"], style=wx.LB_SINGLE)

        bitmap_check = wx.Bitmap(opj("bitmaps/checked.ico"), wx.BITMAP_TYPE_ICO)
        bitmap_uncheck = wx.Bitmap(opj("bitmaps/notchecked.ico"), wx.BITMAP_TYPE_ICO)
        bitmap_flag = wx.Bitmap(opj("bitmaps/flagged.ico"), wx.BITMAP_TYPE_ICO)
        bitmap_unflag = wx.Bitmap(opj("bitmaps/notflagged.ico"), wx.BITMAP_TYPE_ICO)

        self.bitmap_check = wx.StaticBitmap(self, -1, bitmap_check)
        self.bitmap_uncheck = wx.StaticBitmap(self, -1, bitmap_uncheck)
        self.bitmap_flag = wx.StaticBitmap(self, -1, bitmap_flag)
        self.bitmap_unflag = wx.StaticBitmap(self, -1, bitmap_unflag)

        self.okbutton = wx.Button(self, wx.ID_OK)
        self.cancelbutton = wx.Button(self, wx.ID_CANCEL)

        self.parent = parent        

        self.__set_properties()
        self.__do_layout()

        self.Bind(wx.EVT_BUTTON, self.OnOk, self.okbutton)
        self.Bind(wx.EVT_BUTTON, self.OnCancel, self.cancelbutton)
        self.Bind(wx.EVT_LISTBOX, self.OnListBox, self.listicons)


    def __set_properties(self):

        self.SetTitle("Check/Radio Icon Selector")
        self.listicons.SetSelection(0)
        self.okbutton.SetDefault()


    def __do_layout(self):

        mainsizer = wx.BoxSizer(wx.VERTICAL)
        bottomsizer = wx.BoxSizer(wx.HORIZONTAL)
        topsizer = wx.BoxSizer(wx.HORIZONTAL)
        rightsizer = wx.BoxSizer(wx.VERTICAL)
        sizer_2 = wx.BoxSizer(wx.HORIZONTAL)
        sizer_1 = wx.BoxSizer(wx.HORIZONTAL)
        sizer_3 = wx.BoxSizer(wx.HORIZONTAL)
        sizer_4 = wx.BoxSizer(wx.HORIZONTAL)
        label_1 = wx.StaticText(self, -1, "Please Choose One Of These Sets Of Icons:")
        label_1.SetFont(wx.Font(8, wx.DEFAULT, wx.NORMAL, wx.BOLD, 0, ""))
        mainsizer.Add(label_1, 0, wx.ALL|wx.ADJUST_MINSIZE, 10)
        topsizer.Add(self.listicons, 0, wx.ALL|wx.EXPAND|wx.ADJUST_MINSIZE, 5)
        label_2 = wx.StaticText(self, -1, "Checked")
        sizer_1.Add(label_2, 0, wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 10)
        sizer_1.Add((20, 20), 1, wx.ALIGN_RIGHT|wx.ADJUST_MINSIZE, 0)
        sizer_1.Add(self.bitmap_check, 0, wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 10)
        rightsizer.Add(sizer_1, 0, wx.EXPAND, 0)
        label_3 = wx.StaticText(self, -1, "Not Checked")
        sizer_2.Add(label_3, 0, wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 10)
        sizer_2.Add((20, 20), 1, wx.ADJUST_MINSIZE, 0)
        sizer_2.Add(self.bitmap_uncheck, 0, wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 10)
        rightsizer.Add(sizer_2, 0, wx.EXPAND, 0)
        label_4 = wx.StaticText(self, -1, "Flagged")
        sizer_3.Add(label_4, 0, wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 10)
        sizer_3.Add((20, 20), 1, wx.ADJUST_MINSIZE, 0)
        sizer_3.Add(self.bitmap_flag, 0, wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 10)
        rightsizer.Add(sizer_3, 0, wx.EXPAND, 0)
        label_5 = wx.StaticText(self, -1, "Not Flagged")
        sizer_4.Add(label_5, 0, wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 10)
        sizer_4.Add((20, 20), 1, wx.ADJUST_MINSIZE, 0)
        sizer_4.Add(self.bitmap_unflag, 0, wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 10)
        rightsizer.Add(sizer_4, 0, wx.EXPAND, 0)
        
        topsizer.Add(rightsizer, 0, wx.ALL|wx.EXPAND, 5)
        mainsizer.Add(topsizer, 1, wx.EXPAND, 0)
        bottomsizer.Add(self.okbutton, 0, wx.ALL|wx.ADJUST_MINSIZE, 20)
        bottomsizer.Add((20, 20), 1, wx.ADJUST_MINSIZE, 0)
        bottomsizer.Add(self.cancelbutton, 0, wx.ALL|wx.ADJUST_MINSIZE, 20)
        mainsizer.Add(bottomsizer, 0, wx.EXPAND, 0)
        self.SetAutoLayout(True)
        self.SetSizer(mainsizer)
        mainsizer.Fit(self)
        mainsizer.SetSizeHints(self)
        self.Layout()


    def OnListBox(self, event):

        selection = self.listicons.GetSelection()

        if selection == 0:
            bitmap_check = opj("bitmaps/checked.ico")
            bitmap_uncheck = opj("bitmaps/notchecked.ico")
            bitmap_flag = opj("bitmaps/flagged.ico")
            bitmap_unflag = opj("bitmaps/notflagged.ico")
        else:
            bitmap_check = opj("bitmaps/aquachecked.ico")
            bitmap_uncheck = opj("bitmaps/aquanotchecked.ico")
            bitmap_flag = opj("bitmaps/aquaflagged.ico")
            bitmap_unflag = opj("bitmaps/aquanotflagged.ico")

        bitmap_check = wx.Bitmap(bitmap_check, wx.BITMAP_TYPE_ICO)
        bitmap_uncheck = wx.Bitmap(bitmap_uncheck, wx.BITMAP_TYPE_ICO)
        bitmap_flag = wx.Bitmap(bitmap_flag, wx.BITMAP_TYPE_ICO)
        bitmap_unflag = wx.Bitmap(bitmap_unflag, wx.BITMAP_TYPE_ICO)
        
        self.bitmap_uncheck.SetBitmap(bitmap_uncheck)
        self.bitmap_check.SetBitmap(bitmap_check)
        self.bitmap_unflag.SetBitmap(bitmap_unflag)
        self.bitmap_flag.SetBitmap(bitmap_flag)
        
        self.bitmap_check.Refresh()
        self.bitmap_uncheck.Refresh()
        self.bitmap_flag.Refresh()
        self.bitmap_unflag.Refresh()
        
        event.Skip()
        

    def OnOk(self, event):

        selection = self.listicons.GetSelection()
        self.parent.SetCheckRadio(selection)
        self.Destroy()
        event.Skip()


    def OnCancel(self, event):

        self.Destroy()
        event.Skip()


#---------------------------------------------------------------------------
# Just A Dialog To Select Tree Items Icons
#---------------------------------------------------------------------------
class TreeIcons(wx.Dialog):

    def __init__(self, parent=None, id=-1, title="", pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.DEFAULT_DIALOG_STYLE, oldpen=None,
                 bitmaps=None):

        wx.Dialog.__init__(self, parent, id, title, pos, size, style)

        self.bitmaps = [None, None, None, None]
        empty = wx.EmptyBitmap(16, 16)
        self.parent = parent
        
        self.bitmaps[0] = wx.StaticBitmap(self, -1, empty)
        self.combonormal = wx.ComboBox(self, -1, choices=ArtIDs, style=wx.CB_DROPDOWN|wx.CB_READONLY)
        self.bitmaps[1] = wx.StaticBitmap(self, -1, empty)
        self.comboselected = wx.ComboBox(self, -1, choices=ArtIDs, style=wx.CB_DROPDOWN|wx.CB_READONLY)
        self.bitmaps[2] = wx.StaticBitmap(self, -1, empty)
        self.comboexpanded = wx.ComboBox(self, -1, choices=ArtIDs, style=wx.CB_DROPDOWN|wx.CB_READONLY)
        self.bitmaps[3] = wx.StaticBitmap(self, -1, empty)
        self.comboselectedexpanded = wx.ComboBox(self, -1, choices=ArtIDs, style=wx.CB_DROPDOWN|wx.CB_READONLY)
        self.okbutton = wx.Button(self, wx.ID_OK)
        self.cancelbutton = wx.Button(self, wx.ID_CANCEL)

        self.combonormal.SetSelection(bitmaps[0] >= 0 and bitmaps[0]+1 or 0)
        self.comboselected.SetSelection(bitmaps[1] >= 0 and bitmaps[1]+1 or 0)
        self.comboexpanded.SetSelection(bitmaps[2] >= 0 and bitmaps[2]+1 or 0)
        self.comboselectedexpanded.SetSelection(bitmaps[3] >= 0 and bitmaps[3]+1 or 0)

        self.GetBitmaps(bitmaps)
    
        self.__set_properties()
        self.__do_layout()

        self.Bind(wx.EVT_COMBOBOX, self.OnComboNormal, self.combonormal)
        self.Bind(wx.EVT_COMBOBOX, self.OnComboSelected, self.comboselected)
        self.Bind(wx.EVT_COMBOBOX, self.OnComboExpanded, self.comboexpanded)
        self.Bind(wx.EVT_COMBOBOX, self.OnComboSelectedExpanded, self.comboselectedexpanded)
        self.Bind(wx.EVT_BUTTON, self.OnOk, self.okbutton)
        self.Bind(wx.EVT_BUTTON, self.OnCancel, self.cancelbutton)


    def __set_properties(self):

        self.SetTitle("Item Icon Selector")
        self.okbutton.SetDefault()


    def __do_layout(self):

        mainsizer = wx.BoxSizer(wx.VERTICAL)
        sizer_2 = wx.BoxSizer(wx.HORIZONTAL)
        gridsizer = wx.FlexGridSizer(4, 3, 5, 5)
        label_1 = wx.StaticText(self, -1, "Please Choose The Icons For This Item (All Are Optional):")
        label_1.SetFont(wx.Font(8, wx.DEFAULT, wx.NORMAL, wx.BOLD, 0, ""))
        mainsizer.Add(label_1, 0, wx.ALL|wx.ADJUST_MINSIZE, 10)
        label_2 = wx.StaticText(self, -1, "TreeIcon_Normal:")
        gridsizer.Add(label_2, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        gridsizer.Add(self.bitmaps[0], 0, wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        gridsizer.Add(self.combonormal, 0, wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 0)
        label_3 = wx.StaticText(self, -1, "TreeIcon_Selected:")
        gridsizer.Add(label_3, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        gridsizer.Add(self.bitmaps[1], 0, wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        gridsizer.Add(self.comboselected, 0, wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 0)
        label_4 = wx.StaticText(self, -1, "TreeIcon_Expanded:")
        gridsizer.Add(label_4, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        gridsizer.Add(self.bitmaps[2], 0, wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        gridsizer.Add(self.comboexpanded, 0, wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 0)
        label_5 = wx.StaticText(self, -1, "TreeIcon_SelectedExpanded:")
        gridsizer.Add(label_5, 0, wx.LEFT|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        gridsizer.Add(self.bitmaps[3], 0, wx.LEFT|wx.RIGHT|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 5)
        gridsizer.Add(self.comboselectedexpanded, 0, wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 0)
        gridsizer.AddGrowableCol(0)
        gridsizer.AddGrowableCol(1)
        gridsizer.AddGrowableCol(2)
        mainsizer.Add(gridsizer, 0, wx.ALL|wx.EXPAND, 5)
        sizer_2.Add(self.okbutton, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 20)
        sizer_2.Add((20, 20), 1, wx.ADJUST_MINSIZE, 0)
        sizer_2.Add(self.cancelbutton, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL|wx.ADJUST_MINSIZE, 20)
        mainsizer.Add(sizer_2, 1, wx.EXPAND, 0)
        self.SetAutoLayout(True)
        self.SetSizer(mainsizer)
        mainsizer.Fit(self)
        mainsizer.SetSizeHints(self)
        self.Layout()
        self.Centre()


    def OnComboNormal(self, event):

        input = event.GetSelection()
        self.GetBitmap(input, 0)
        event.Skip()


    def OnComboSelected(self, event):

        input = event.GetSelection()
        self.GetBitmap(input, 1)
        event.Skip()


    def OnComboExpanded(self, event):

        input = event.GetSelection()
        self.GetBitmap(input, 2)
        event.Skip()


    def OnComboSelectedExpanded(self, event):

        input = event.GetSelection()
        self.GetBitmap(input, 3)
        event.Skip()


    def OnOk(self, event):

        bitmaps = [-1, -1, -1, -1]
        normal = self.combonormal.GetSelection()
        selected = self.comboselected.GetSelection()
        expanded = self.comboexpanded.GetSelection()
        selexp = self.comboselectedexpanded.GetSelection()

        bitmaps = [(normal > 0 and normal or -1), (selected > 0 and selected or -1),
                   (expanded > 0 and expanded or -1), (selexp > 0 and selexp or -1)]

        newbitmaps = []
        
        for bmp in bitmaps:
            if bmp > 0:
                newbitmaps.append(bmp-1)
            else:
                newbitmaps.append(bmp)

        self.parent.SetNewIcons(newbitmaps)

        self.Destroy()
        event.Skip()


    def OnCancel(self, event): 

        self.Destroy()
        event.Skip()


    def GetBitmap(self, input, which):

        if input == 0:
            bmp = wx.EmptyBitmap(16,16)
            self.ClearBmp(bmp)
        elif input > 36:
            bmp = images.getSmilesBitmap()
        else:
            bmp = wx.ArtProvider_GetBitmap(eval(ArtIDs[input]), wx.ART_OTHER, (16,16))
            if not bmp.Ok():
                bmp = wx.EmptyBitmap(16,16)
                self.ClearBmp(bmp)

        self.bitmaps[which].SetBitmap(bmp)
        self.bitmaps[which].Refresh()
        

    def GetBitmaps(self, bitmaps):

        output = []

        for count, input in enumerate(bitmaps):
            if input < 0:
                bmp = wx.EmptyBitmap(16,16)
                self.ClearBmp(bmp)
            elif input > 35:
                bmp = images.getSmilesBitmap()
            else:
                bmp = wx.ArtProvider_GetBitmap(eval(ArtIDs[input+1]), wx.ART_OTHER, (16,16))
                if not bmp.Ok():
                    bmp = wx.EmptyBitmap(16,16)
                    self.ClearBmp(bmp)

            self.bitmaps[count].SetBitmap(bmp)


    def ClearBmp(self, bmp):

        dc = wx.MemoryDC()
        dc.SelectObject(bmp)
        dc.SetBackground(wx.Brush("white"))
        dc.Clear()

                
#---------------------------------------------------------------------------
# CustomTreeCtrl Demo Implementation
#---------------------------------------------------------------------------
class CustomTreeCtrlDemo(wx.Panel):

    def __init__(self, parent, log):
        wx.Panel.__init__(self, parent)
        
        self.log = log
        self.oldicons = 0        

        splitter = wx.SplitterWindow(self, -1, style=wx.CLIP_CHILDREN | wx.SP_LIVE_UPDATE | wx.SP_3D)

        # Create the CustomTreeCtrl, using a derived class defined below
        self.tree = CustomTreeCtrl(splitter, -1, log=self.log, style=wx.SUNKEN_BORDER)           

        self.leftpanel = wx.ScrolledWindow(splitter, -1, style=wx.SUNKEN_BORDER)
        self.leftpanel.SetScrollRate(20,20)
        width = self.PopulateLeftPanel(self.tree.styles, self.tree.events)
        
        # add the windows to the splitter and split it.
        splitter.SplitVertically(self.leftpanel, self.tree, 300)
        splitter.SetMinimumPaneSize(width+5)
        
        sizer = wx.BoxSizer()
        sizer.Add(splitter, 1, wx.EXPAND)
        self.SetSizer(sizer)
        

    def PopulateLeftPanel(self, styles, events):
        pnl = wx.Panel(self.leftpanel)
        mainsizer = wx.BoxSizer(wx.VERTICAL)
        recreatetree = wx.Button(pnl, -1, "Recreate CustomTreeCtrl")
        mainsizer.Add(recreatetree, 0, wx.ALL|wx.ALIGN_CENTER, 10)
        recreatetree.Bind(wx.EVT_BUTTON, self.OnRecreateTree)

        staticboxstyles = wx.StaticBox(pnl, -1, "CustomTreeCtrl Styles")
        stylesizer = wx.StaticBoxSizer(staticboxstyles, wx.VERTICAL)
        staticboxevents = wx.StaticBox(pnl, -1, "CustomTreeCtrl Events")
        eventssizer = wx.StaticBoxSizer(staticboxevents, wx.VERTICAL)
        staticboxcolours = wx.StaticBox(pnl, -1, "CustomTreeCtrl Images/Colours")
        colourssizer = wx.StaticBoxSizer(staticboxcolours, wx.VERTICAL)
        staticboxthemes = wx.StaticBox(pnl, -1, "CustomTreeCtrl Themes/Gradients")
        themessizer = wx.StaticBoxSizer(staticboxthemes, wx.VERTICAL)

        self.treestyles = []
        self.treeevents = []
        
        for count, style in enumerate(styles):
            
            if count == 0:
                tags = wx.ALL
            else:
                tags = wx.LEFT|wx.RIGHT|wx.BOTTOM

            if style != "TR_DEFAULT_STYLE":
                check = wx.CheckBox(pnl, -1, style)
                stylesizer.Add(check, 0, tags, 3)
                        
                if style in ["TR_HAS_BUTTONS", "TR_HAS_VARIABLE_ROW_HEIGHT"]:
                    check.SetValue(1)

                if style == "TR_HAS_VARIABLE_ROW_HEIGHT":
                    check.Enable(False)

                check.Bind(wx.EVT_CHECKBOX, self.OnCheckStyle)
                self.treestyles.append(check)


        for count, event in enumerate(events):
            
            if count == 0:
                tags = wx.ALL
            else:
                tags = wx.LEFT|wx.RIGHT|wx.BOTTOM

            if count not in [6, 17, 22, 23]:
                check = wx.CheckBox(pnl, -1, event)
                eventssizer.Add(check, 0, tags, 3)
                        
                if event in ["EVT_TREE_ITEM_EXPANDED", "EVT_TREE_ITEM_COLLAPSED",
                             "EVT_TREE_SEL_CHANGED", "EVT_TREE_SEL_CHANGING"]:
                    
                    check.SetValue(1)

                check.Bind(wx.EVT_CHECKBOX, self.OnCheckEvent)
                self.treeevents.append(check)

        sizer1 = wx.BoxSizer(wx.HORIZONTAL)
        label = wx.StaticText(pnl, -1, "Connection Pen")
        font = label.GetFont()
        font.SetWeight(wx.FONTWEIGHT_BOLD)
        label.SetFont(font)
        buttonconnection = wx.Button(pnl, -1, "Choose...")
        buttonconnection.Bind(wx.EVT_BUTTON, self.OnButtonConnection)
        sizer1.Add(label, 0, wx.ALL|wx.ALIGN_CENTER, 5)
        sizer1.Add((1,0), 1, wx.EXPAND)
        sizer1.Add(buttonconnection, 0, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_RIGHT, 5)
        
        sizer2 = wx.BoxSizer(wx.HORIZONTAL)
        label = wx.StaticText(pnl, -1, "Border Pen")
        font = label.GetFont()
        font.SetWeight(wx.FONTWEIGHT_BOLD)
        label.SetFont(font)
        buttonborder = wx.Button(pnl, -1, "Choose...")
        buttonborder.Bind(wx.EVT_BUTTON, self.OnButtonBorder)
        sizer2.Add(label, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ALIGN_CENTER, 5)
        sizer2.Add((1,0), 1, wx.EXPAND)
        sizer2.Add(buttonborder, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_RIGHT, 5)

        sizer3 = wx.BoxSizer(wx.HORIZONTAL)
        label = wx.StaticText(pnl, -1, "Tree Buttons")
        font = label.GetFont()
        font.SetWeight(wx.FONTWEIGHT_BOLD)
        label.SetFont(font)
        buttontree = wx.Button(pnl, -1, "Choose...")
        buttontree.Bind(wx.EVT_BUTTON, self.OnButtonTree)
        sizer3.Add(label, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ALIGN_CENTER, 5)
        sizer3.Add((1,0), 1, wx.EXPAND)
        sizer3.Add(buttontree, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_RIGHT, 5)

        sizer4 = wx.BoxSizer(wx.HORIZONTAL)
        label = wx.StaticText(pnl, -1, "Check/Radio Buttons")
        font = label.GetFont()
        font.SetWeight(wx.FONTWEIGHT_BOLD)
        label.SetFont(font)
        buttoncr = wx.Button(pnl, -1, "Choose...")
        buttoncr.Bind(wx.EVT_BUTTON, self.OnButtonCheckRadio)
        sizer4.Add(label, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ALIGN_CENTER, 5)
        sizer4.Add((1,0), 1, wx.EXPAND)
        sizer4.Add(buttoncr, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_RIGHT, 5)

        sizer5 = wx.BoxSizer(wx.HORIZONTAL)
        radioimage = wx.RadioButton(pnl, -1, "Image Background", style=wx.RB_GROUP)
        radioimage.Bind(wx.EVT_RADIOBUTTON, self.OnBackgroundImage)
        self.imagebutton = wx.Button(pnl, -1, "Choose...")
        self.imagebutton.Bind(wx.EVT_BUTTON, self.OnChooseImage)
        sizer5.Add(radioimage, 0, wx.ALL|wx.ALIGN_CENTER, 5)
        sizer5.Add((1,0), 1, wx.EXPAND)
        sizer5.Add(self.imagebutton, 0, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.ALIGN_RIGHT, 5)

        sizer6 = wx.BoxSizer(wx.HORIZONTAL)
        radiobackground = wx.RadioButton(pnl, -1, "Background Colour")
        radiobackground.Bind(wx.EVT_RADIOBUTTON, self.OnBackgroundColour)
        self.backbutton = csel.ColourSelect(pnl, -1, "Choose...", wx.WHITE)
        self.backbutton.Bind(csel.EVT_COLOURSELECT, self.OnChooseBackground)
        sizer6.Add(radiobackground, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ALIGN_CENTER, 5)
        sizer6.Add((1,0), 1, wx.EXPAND)
        sizer6.Add(self.backbutton, 0, wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.ALIGN_CENTER, 5)        

        colourssizer.Add(sizer1, 0, wx.EXPAND)
        colourssizer.Add(sizer2, 0, wx.EXPAND)
        colourssizer.Add(sizer3, 0, wx.EXPAND)
        colourssizer.Add(sizer4, 0, wx.EXPAND)
        colourssizer.Add(sizer5, 0, wx.EXPAND)
        colourssizer.Add(sizer6, 0, wx.EXPAND)

        sizera = wx.BoxSizer(wx.HORIZONTAL)
        self.checknormal = wx.CheckBox(pnl, -1, "Standard Colours")
        self.focus = csel.ColourSelect(pnl, -1, "Focus",
                                       self.tree.GetHilightFocusColour())
        self.unfocus = csel.ColourSelect(pnl, -1, "Non-Focus",
                                         self.tree.GetHilightNonFocusColour())
        self.checknormal.Bind(wx.EVT_CHECKBOX, self.OnCheckNormal)
        self.focus.Bind(csel.EVT_COLOURSELECT, self.OnFocusColour)
        self.unfocus.Bind(csel.EVT_COLOURSELECT, self.OnNonFocusColour)
        sizera1 = wx.BoxSizer(wx.VERTICAL)
        sizera1.Add(self.focus, 0, wx.BOTTOM, 2)
        sizera1.Add(self.unfocus, 0)
        sizera.Add(self.checknormal, 0, wx.ALL, 3)
        sizera.Add((1, 0), 1, wx.EXPAND)
        sizera.Add(sizera1, 0, wx.ALL|wx.ALIGN_CENTER_HORIZONTAL|wx.EXPAND, 3)

        sizerb = wx.BoxSizer(wx.VERTICAL)
        self.checkgradient = wx.CheckBox(pnl, -1, "Gradient Theme")
        self.checkgradient.Bind(wx.EVT_CHECKBOX, self.OnCheckGradient)
        sizerb1 = wx.BoxSizer(wx.HORIZONTAL)
        sizerb1.Add((10, 0))
        self.radiohorizontal = wx.RadioButton(pnl, -1, "Horizontal", style=wx.RB_GROUP)
        self.radiohorizontal.Bind(wx.EVT_RADIOBUTTON, self.OnHorizontal)
        sizerb1.Add(self.radiohorizontal, 0, wx.TOP|wx.BOTTOM, 3)
        sizerb2 = wx.BoxSizer(wx.HORIZONTAL)
        sizerb2.Add((10, 0))
        self.radiovertical = wx.RadioButton(pnl, -1, "Vertical")
        self.radiovertical.Bind(wx.EVT_RADIOBUTTON, self.OnVertical)
        sizerb2.Add(self.radiovertical, 0, wx.BOTTOM, 3)
        sizerb3 = wx.BoxSizer(wx.HORIZONTAL)
        self.firstcolour = csel.ColourSelect(pnl, -1, "First Colour",
                                             self.tree.GetFirstGradientColour())
        self.secondcolour = csel.ColourSelect(pnl, -1, "Second Colour",
                                              self.tree.GetSecondGradientColour())
        self.firstcolour.Bind(csel.EVT_COLOURSELECT, self.OnFirstColour)
        self.secondcolour.Bind(csel.EVT_COLOURSELECT, self.OnSecondColour)
        sizerb3.Add(self.firstcolour, 0, wx.TOP|wx.BOTTOM|wx.ALIGN_CENTER_HORIZONTAL, 3)
        sizerb3.Add(self.secondcolour, 0, wx.LEFT|wx.TOP|wx.BOTTOM|wx.ALIGN_CENTER_HORIZONTAL, 3)
        sizerb.Add(self.checkgradient, 0, wx.ALL, 3)
        sizerb.Add(sizerb1, 0)
        sizerb.Add(sizerb2, 0)
        sizerb.Add(sizerb3, 0, wx.ALIGN_CENTER)

        self.checkvista = wx.CheckBox(pnl, -1, "Windows Vista Theme")
        self.checkvista.Bind(wx.EVT_CHECKBOX, self.OnVista)
        
        themessizer.Add(sizera, 0, wx.EXPAND)
        themessizer.Add(sizerb, 0, wx.EXPAND)
        themessizer.Add((0, 5))
        themessizer.Add(self.checkvista, 0, wx.EXPAND|wx.ALL, 3)

        mainsizer.Add(stylesizer, 0, wx.EXPAND|wx.ALL, 5)
        mainsizer.Add(colourssizer, 0, wx.EXPAND|wx.ALL, 5)
        mainsizer.Add(themessizer, 0, wx.EXPAND|wx.ALL, 5)
        mainsizer.Add(eventssizer, 0, wx.EXPAND|wx.ALL, 5)

        pnl.SetSizer(mainsizer)
        pnl.Fit()

        swsizer = wx.BoxSizer(wx.VERTICAL)
        swsizer.Add(pnl, 0, wx.EXPAND)
        self.leftpanel.SetSizer(swsizer)
        swsizer.Layout()

        radiobackground.SetValue(1)
        self.checknormal.SetValue(1)
        self.radiohorizontal.Enable(False)
        self.radiovertical.Enable(False)
        self.firstcolour.Enable(False)
        self.secondcolour.Enable(False)
        self.imagebutton.Enable(False)

        return mainsizer.CalcMin().width + wx.SystemSettings.GetMetric(wx.SYS_VSCROLL_X)
    

    def OnRecreateTree(self, event):

        splitter = self.tree.GetParent()
        newtree = CustomTreeCtrl(splitter, -1, log=self.log)
        splitter.ReplaceWindow(self.tree, newtree)
        self.tree.Destroy()
        self.tree = newtree
        # Todo:  The settings in the leftpanel should be reset too
        

    def OnCheckStyle(self, event):

        self.tree.ChangeStyle(self.treestyles)
        event.Skip()


    def OnCheckEvent(self, event):

        obj = event.GetEventObject()
        self.tree.BindEvents(obj)
        
        event.Skip()


    def OnButtonConnection(self, event):

        pen = self.tree.GetConnectionPen()
        dlg = PenDialog(self, -1, oldpen=pen, pentype=0)

        dlg.ShowModal()
                
        event.Skip()


    def SetConnectionPen(self, pen):

        self.tree.SetConnectionPen(pen)
        

    def OnButtonBorder(self, event):

        pen = self.tree.GetBorderPen()
        dlg = PenDialog(self, -1, oldpen=pen, pentype=1)

        dlg.ShowModal()
        event.Skip()


    def SetBorderPen(self, pen):

        self.tree.SetBorderPen(pen)
        

    def OnButtonTree(self, event):

        dlg = TreeButtonsDialog(self, -1, oldicons=self.oldicons)
        dlg.ShowModal()
        
        event.Skip()


    def OnButtonCheckRadio(self, event):

        dlg = CheckDialog(self, -1)
        dlg.ShowModal()
        
        event.Skip()
    

    def SetTreeButtons(self, selection):

        bitmap_plus = opj("bitmaps/plus" + str(selection+1) + ".ico")
        bitmap_minus = opj("bitmaps/minus" + str(selection+1) + ".ico")
        
        bitmap = wx.Bitmap(bitmap_plus, wx.BITMAP_TYPE_ICO)
        width = bitmap.GetWidth()
        
        il = wx.ImageList(width, width)
        
        il.Add(wx.Bitmap(bitmap_plus, wx.BITMAP_TYPE_ICO))
        il.Add(wx.Bitmap(bitmap_plus, wx.BITMAP_TYPE_ICO))
        il.Add(wx.Bitmap(bitmap_minus, wx.BITMAP_TYPE_ICO))
        il.Add(wx.Bitmap(bitmap_minus, wx.BITMAP_TYPE_ICO))

        self.il = il                
        self.tree.SetButtonsImageList(il)


    def SetCheckRadio(self, selection):

        if selection == 0:
            self.tree.SetImageListCheck(13, 13)
        else:
            bitmap_check = opj("bitmaps/aquachecked.ico")
            bitmap_uncheck = opj("bitmaps/aquanotchecked.ico")
            bitmap_flag = opj("bitmaps/aquaflagged.ico")
            bitmap_unflag = opj("bitmaps/aquanotflagged.ico")

            il = wx.ImageList(16, 16)
        
            il.Add(wx.Bitmap(bitmap_check, wx.BITMAP_TYPE_ICO))
            il.Add(wx.Bitmap(bitmap_uncheck, wx.BITMAP_TYPE_ICO))
            il.Add(wx.Bitmap(bitmap_flag, wx.BITMAP_TYPE_ICO))
            il.Add(wx.Bitmap(bitmap_unflag, wx.BITMAP_TYPE_ICO))
            self.tree.SetImageListCheck(16, 16, il)        


    def OnBackgroundImage(self, event):

        if hasattr(self, "backgroundimage"):
            self.tree.SetBackgroundImage(self.backgroundimage)

        self.backbutton.Enable(False)
        self.imagebutton.Enable(True)
        
        event.Skip()


    def OnChooseImage(self, event):

        wildcard = "JPEG Files (*.jpg)|*.jpg|"     \
                   "Bitmap Files (*.bmp)|*.bmp|" \
                   "PNG Files (*.png)|*.png|"    \
                   "Icon Files (*.ico)|*.ico|"        \
                   "GIF Files (*.gif)|*.gif|"        \
                   "All files (*.*)|*.*"
        
        dlg = wx.FileDialog(self, "Choose An Image File", ".", "", wildcard, wx.OPEN)

        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
        else:
            dlg.Destroy()
            return

        dlg.Destroy()
        bitmap = wx.Bitmap(path, wx.BITMAP_TYPE_ANY)
        self.tree.SetBackgroundImage(bitmap)
        self.backgroundimage = bitmap

        event.Skip()


    def OnBackgroundColour(self, event):
        
        self.imagebutton.Enable(False)
        self.backbutton.Enable(True)
        self.tree.SetBackgroundImage(None)
        
        event.Skip()


    def OnChooseBackground(self, event):

        col1 = event.GetValue()        
        self.tree.SetBackgroundColour(col1)        
        event.Skip()


    def OnCheckNormal(self, event):

        self.radiohorizontal.Enable(False)
        self.radiovertical.Enable(False)
        self.firstcolour.Enable(False)
        self.secondcolour.Enable(False)
        self.focus.Enable(True)
        self.unfocus.Enable(True)
        self.checkgradient.SetValue(0)
        self.checkvista.SetValue(0)
        self.tree.EnableSelectionGradient(False)
        self.tree.EnableSelectionVista(False)
        event.Skip()


    def OnFocusColour(self, event):

        col1 = event.GetValue()  
        self.tree.SetHilightFocusColour(col1)
        event.Skip()


    def OnNonFocusColour(self, event):

        col1 = event.GetValue()  
        self.tree.SetHilightNonFocusColour(col1)
        event.Skip()
        

    def OnCheckGradient(self, event):

        self.radiohorizontal.Enable(True)
        self.radiovertical.Enable(True)
        self.firstcolour.Enable(True)
        self.secondcolour.Enable(True)
        self.checknormal.SetValue(0)
        self.checkvista.SetValue(0)
        self.focus.Enable(False)
        self.unfocus.Enable(False)
        self.tree.SetGradientStyle(self.radiovertical.GetValue())
        self.tree.EnableSelectionVista(False)
        self.tree.EnableSelectionGradient(True)
        
        event.Skip()
        
        
    def OnHorizontal(self, event):

        self.tree.SetGradientStyle(self.radiovertical.GetValue())
        event.Skip()


    def OnVertical(self, event):

        self.tree.SetGradientStyle(self.radiovertical.GetValue())
        event.Skip()


    def OnFirstColour(self, event):

        col1 = event.GetValue()  
        self.tree.SetFirstGradientColour(wx.Colour(col1[0], col1[1], col1[2]))
        event.Skip()


    def OnSecondColour(self, event):

        col1 = event.GetValue()  
        self.tree.SetSecondGradientColour(wx.Colour(col1[0], col1[1], col1[2]))
        event.Skip()


    def OnVista(self, event):

        self.radiohorizontal.Enable(False)
        self.radiovertical.Enable(False)
        self.firstcolour.Enable(False)
        self.secondcolour.Enable(False)
        self.checknormal.SetValue(0)
        self.checkgradient.SetValue(0)
        self.focus.Enable(False)
        self.unfocus.Enable(False)
        self.tree.EnableSelectionGradient(False)
        self.tree.EnableSelectionVista(True)
        
        event.Skip()





#---------------------------------------------------------------------------
# CustomTreeCtrl Demo Implementation
#---------------------------------------------------------------------------
class CustomTreeCtrl(CT.CustomTreeCtrl):

    def __init__(self, parent, id=wx.ID_ANY, pos=wx.DefaultPosition,
                 size=wx.DefaultSize,
                 style=wx.SUNKEN_BORDER,
                 ctstyle=CT.TR_HAS_BUTTONS | CT.TR_HAS_VARIABLE_ROW_HEIGHT,
                 log=None):

        CT.CustomTreeCtrl.__init__(self, parent, id, pos, size, style, ctstyle)

        alldata = dir(CT)

        treestyles = []
        events = []
        for data in alldata:
            if data.startswith("TR_"):
                treestyles.append(data)
            elif data.startswith("EVT_"):
                events.append(data)

        self.events = events
        self.styles = treestyles
        self.item = None
        
        il = wx.ImageList(16, 16)

        for items in ArtIDs[1:-1]:
            bmp = wx.ArtProvider_GetBitmap(eval(items), wx.ART_TOOLBAR, (16, 16))
            il.Add(bmp)

        smileidx = il.Add(images.getSmilesBitmap())
        numicons = il.GetImageCount()

        self.AssignImageList(il)
        self.count = 0
        self.log = log

        # NOTE:  For some reason tree items have to have a data object in
        #        order to be sorted.  Since our compare just uses the labels
        #        we don't need any real data, so we'll just use None below for
        #        the item data.

        self.root = self.AddRoot("The Root Item")

        if not(self.GetTreeStyle() & CT.TR_HIDE_ROOT):
            self.SetPyData(self.root, None)
            self.SetItemImage(self.root, 24, CT.TreeItemIcon_Normal)
            self.SetItemImage(self.root, 13, CT.TreeItemIcon_Expanded)

        textctrl = wx.TextCtrl(self, -1, "I Am A Simple\nMultiline wx.TexCtrl", style=wx.TE_MULTILINE)
        self.gauge = wx.Gauge(self, -1, 50, style=wx.GA_HORIZONTAL|wx.GA_SMOOTH)
        self.gauge.SetValue(0)
        combobox = wx.ComboBox(self, -1, choices=["That", "Was", "A", "Nice", "Holyday!"], style=wx.CB_READONLY|wx.CB_DROPDOWN)

        textctrl.Bind(wx.EVT_CHAR, self.OnTextCtrl)
        combobox.Bind(wx.EVT_COMBOBOX, self.OnComboBox)

        for x in range(15):
            if x == 1:
                child = self.AppendItem(self.root, "Item %d" % x + "\nHello World\nHappy wxPython-ing!")
                self.SetItemBold(child, True)
            else:
                child = self.AppendItem(self.root, "Item %d" % x)
            self.SetPyData(child, None)
            self.SetItemImage(child, 24, CT.TreeItemIcon_Normal)
            self.SetItemImage(child, 13, CT.TreeItemIcon_Expanded)

            for y in range(5):
                if y == 0 and x == 1:
                    last = self.AppendItem(child, "item %d-%s" % (x, chr(ord("a")+y)), ct_type=2, wnd=self.gauge)
                elif y == 1 and x == 2:
                    last = self.AppendItem(child, "Item %d-%s" % (x, chr(ord("a")+y)), ct_type=1, wnd=textctrl)
                elif 2 < y < 4:
                    last = self.AppendItem(child, "item %d-%s" % (x, chr(ord("a")+y)))
                elif y == 4 and x == 1:
                    last = self.AppendItem(child, "item %d-%s" % (x, chr(ord("a")+y)), wnd=combobox)
                else:
                    last = self.AppendItem(child, "item %d-%s" % (x, chr(ord("a")+y)), ct_type=2)
                    
                self.SetPyData(last, None)
                self.SetItemImage(last, 24, CT.TreeItemIcon_Normal)
                self.SetItemImage(last, 13, CT.TreeItemIcon_Expanded)
                    
                for z in range(5):
                    if z > 2:
                        item = self.AppendItem(last,  "item %d-%s-%d" % (x, chr(ord("a")+y), z), ct_type=1)
                    elif 0 < z <= 2:
                        item = self.AppendItem(last,  "item %d-%s-%d" % (x, chr(ord("a")+y), z), ct_type=2)
                    elif z == 0:
                        item = self.AppendItem(last,  "item %d-%s-%d" % (x, chr(ord("a")+y), z))
                        self.SetItemHyperText(item, True)
                    self.SetPyData(item, None)
                    self.SetItemImage(item, 28, CT.TreeItemIcon_Normal)
                    self.SetItemImage(item, numicons-1, CT.TreeItemIcon_Selected)

        self.Bind(wx.EVT_LEFT_DCLICK, self.OnLeftDClick)
        self.Bind(wx.EVT_IDLE, self.OnIdle)

        self.eventdict = {'EVT_TREE_BEGIN_DRAG': self.OnBeginDrag, 'EVT_TREE_BEGIN_LABEL_EDIT': self.OnBeginEdit,
                          'EVT_TREE_BEGIN_RDRAG': self.OnBeginRDrag, 'EVT_TREE_DELETE_ITEM': self.OnDeleteItem,
                          'EVT_TREE_END_DRAG': self.OnEndDrag, 'EVT_TREE_END_LABEL_EDIT': self.OnEndEdit,
                          'EVT_TREE_ITEM_ACTIVATED': self.OnActivate, 'EVT_TREE_ITEM_CHECKED': self.OnItemCheck,
                          'EVT_TREE_ITEM_CHECKING': self.OnItemChecking, 'EVT_TREE_ITEM_COLLAPSED': self.OnItemCollapsed,
                          'EVT_TREE_ITEM_COLLAPSING': self.OnItemCollapsing, 'EVT_TREE_ITEM_EXPANDED': self.OnItemExpanded,
                          'EVT_TREE_ITEM_EXPANDING': self.OnItemExpanding, 'EVT_TREE_ITEM_GETTOOLTIP': self.OnToolTip,
                          'EVT_TREE_ITEM_MENU': self.OnItemMenu, 'EVT_TREE_ITEM_RIGHT_CLICK': self.OnRightDown,
                          'EVT_TREE_KEY_DOWN': self.OnKey, 'EVT_TREE_SEL_CHANGED': self.OnSelChanged,
                          'EVT_TREE_SEL_CHANGING': self.OnSelChanging, "EVT_TREE_ITEM_HYPERLINK": self.OnHyperLink}

        mainframe = wx.GetTopLevelParent(self)
        
        if not hasattr(mainframe, "leftpanel"):
            self.Bind(CT.EVT_TREE_ITEM_EXPANDED, self.OnItemExpanded)
            self.Bind(CT.EVT_TREE_ITEM_COLLAPSED, self.OnItemCollapsed)
            self.Bind(CT.EVT_TREE_SEL_CHANGED, self.OnSelChanged)
            self.Bind(CT.EVT_TREE_SEL_CHANGING, self.OnSelChanging)
            self.Bind(wx.EVT_RIGHT_DOWN, self.OnRightDown)
            self.Bind(wx.EVT_RIGHT_UP, self.OnRightUp)
        else:
            for combos in mainframe.treeevents:
                self.BindEvents(combos)

        if hasattr(mainframe, "leftpanel"):
            self.ChangeStyle(mainframe.treestyles)

        if not(self.GetTreeStyle() & CT.TR_HIDE_ROOT):
            self.SelectItem(self.root)
            self.Expand(self.root)


    def BindEvents(self, choice, recreate=False):

        value = choice.GetValue()
        text = choice.GetLabel()
        
        evt = "CT." + text
        binder = self.eventdict[text]

        if value == 1:
            if evt == "CT.EVT_TREE_BEGIN_RDRAG":
                self.Bind(wx.EVT_RIGHT_DOWN, None)
                self.Bind(wx.EVT_RIGHT_UP, None)
            self.Bind(eval(evt), binder)
        else:
            self.Bind(eval(evt), None)
            if evt == "CT.EVT_TREE_BEGIN_RDRAG":
                self.Bind(wx.EVT_RIGHT_DOWN, self.OnRightDown)
                self.Bind(wx.EVT_RIGHT_UP, self.OnRightUp)


    def ChangeStyle(self, combos):

        style = 0
        for combo in combos:
            if combo.GetValue() == 1:
                style = style | eval("CT." + combo.GetLabel())

        if self.GetTreeStyle() != style:
            self.SetTreeStyle(style)
            

    def OnCompareItems(self, item1, item2):
        
        t1 = self.GetItemText(item1)
        t2 = self.GetItemText(item2)
        
        self.log.write('compare: ' + t1 + ' <> ' + t2 + "\n")

        if t1 < t2:
            return -1
        if t1 == t2:
            return 0

        return 1

    
    def OnIdle(self, event):

        if self.gauge:
            try:
                if self.gauge.IsEnabled() and self.gauge.IsShown():
                    self.count = self.count + 1

                    if self.count >= 50:
                        self.count = 0

                    self.gauge.SetValue(self.count)

            except:
                self.gauge = None

        event.Skip()


    def OnRightDown(self, event):
        
        pt = event.GetPosition()
        item, flags = self.HitTest(pt)

        if item:
            self.item = item
            self.log.write("OnRightClick: %s, %s, %s" % (self.GetItemText(item), type(item), item.__class__) + "\n")
            self.SelectItem(item)


    def OnRightUp(self, event):

        item = self.item
        
        if not item:
            event.Skip()
            return

        if not self.IsEnabled(item):
            event.Skip()
            return

        # Item Text Appearance
        ishtml = self.IsItemHyperText(item)
        back = self.GetItemBackgroundColour(item)
        fore = self.GetItemTextColour(item)
        isbold = self.IsBold(item)
        font = self.GetItemFont(item)

        # Icons On Item
        normal = self.GetItemImage(item, CT.TreeItemIcon_Normal)
        selected = self.GetItemImage(item, CT.TreeItemIcon_Selected)
        expanded = self.GetItemImage(item, CT.TreeItemIcon_Expanded)
        selexp = self.GetItemImage(item, CT.TreeItemIcon_SelectedExpanded)

        # Enabling/Disabling Windows Associated To An Item
        haswin = self.GetItemWindow(item)

        # Enabling/Disabling Items
        enabled = self.IsEnabled(item)

        # Generic Item's Info
        children = self.GetChildrenCount(item)
        itemtype = self.GetItemType(item)
        text = self.GetItemText(item)
        pydata = self.GetPyData(item)
        
        self.current = item
        self.itemdict = {"ishtml": ishtml, "back": back, "fore": fore, "isbold": isbold,
                         "font": font, "normal": normal, "selected": selected, "expanded": expanded,
                         "selexp": selexp, "haswin": haswin, "children": children,
                         "itemtype": itemtype, "text": text, "pydata": pydata, "enabled": enabled}
        
        menu = wx.Menu()

        item1 = menu.Append(wx.ID_ANY, "Change Item Background Colour")
        item2 = menu.Append(wx.ID_ANY, "Modify Item Text Colour")
        menu.AppendSeparator()
        if isbold:
            strs = "Make Item Text Not Bold"
        else:
            strs = "Make Item Text Bold"
        item3 = menu.Append(wx.ID_ANY, strs)
        item4 = menu.Append(wx.ID_ANY, "Change Item Font")
        menu.AppendSeparator()
        if ishtml:
            strs = "Set Item As Non-Hyperlink"
        else:
            strs = "Set Item As Hyperlink"
        item5 = menu.Append(wx.ID_ANY, strs)
        menu.AppendSeparator()
        if haswin:
            enabled = self.GetItemWindowEnabled(item)
            if enabled:
                strs = "Disable Associated Widget"
            else:
                strs = "Enable Associated Widget"
        else:
            strs = "Enable Associated Widget"
        item6 = menu.Append(wx.ID_ANY, strs)

        if not haswin:
            item6.Enable(False)

        item7 = menu.Append(wx.ID_ANY, "Disable Item")
        
        menu.AppendSeparator()
        item8 = menu.Append(wx.ID_ANY, "Change Item Icons")
        menu.AppendSeparator()
        item9 = menu.Append(wx.ID_ANY, "Get Other Information For This Item")
        menu.AppendSeparator()

        item10 = menu.Append(wx.ID_ANY, "Delete Item")
        if item == self.GetRootItem():
            item10.Enable(False)
        item11 = menu.Append(wx.ID_ANY, "Prepend An Item")
        item12 = menu.Append(wx.ID_ANY, "Append An Item")

        self.Bind(wx.EVT_MENU, self.OnItemBackground, item1)
        self.Bind(wx.EVT_MENU, self.OnItemForeground, item2)
        self.Bind(wx.EVT_MENU, self.OnItemBold, item3)
        self.Bind(wx.EVT_MENU, self.OnItemFont, item4)
        self.Bind(wx.EVT_MENU, self.OnItemHyperText, item5)
        self.Bind(wx.EVT_MENU, self.OnEnableWindow, item6)
        self.Bind(wx.EVT_MENU, self.OnDisableItem, item7)
        self.Bind(wx.EVT_MENU, self.OnItemIcons, item8)
        self.Bind(wx.EVT_MENU, self.OnItemInfo, item9)
        self.Bind(wx.EVT_MENU, self.OnItemDelete, item10)
        self.Bind(wx.EVT_MENU, self.OnItemPrepend, item11)
        self.Bind(wx.EVT_MENU, self.OnItemAppend, item12)
        
        self.PopupMenu(menu)
        menu.Destroy()
        

    def OnItemBackground(self, event):

        colourdata = wx.ColourData()
        colourdata.SetColour(self.itemdict["back"])
        dlg = wx.ColourDialog(self, colourdata)
        
        dlg.GetColourData().SetChooseFull(True)

        if dlg.ShowModal() == wx.ID_OK:
            data = dlg.GetColourData()
            col1 = data.GetColour().Get()
            self.SetItemBackgroundColour(self.current, col1)
        dlg.Destroy()


    def OnItemForeground(self, event):

        colourdata = wx.ColourData()
        colourdata.SetColour(self.itemdict["fore"])
        dlg = wx.ColourDialog(self, colourdata)
        
        dlg.GetColourData().SetChooseFull(True)

        if dlg.ShowModal() == wx.ID_OK:
            data = dlg.GetColourData()
            col1 = data.GetColour().Get()
            self.SetItemTextColour(self.current, col1)
        dlg.Destroy()


    def OnItemBold(self, event):

        self.SetItemBold(self.current, not self.itemdict["isbold"])


    def OnItemFont(self, event):

        data = wx.FontData()
        font = self.itemdict["font"]
        
        if font is None:
            font = wx.SystemSettings_GetFont(wx.SYS_DEFAULT_GUI_FONT)
            
        data.SetInitialFont(font)

        dlg = wx.FontDialog(self, data)
        
        if dlg.ShowModal() == wx.ID_OK:
            data = dlg.GetFontData()
            font = data.GetChosenFont()
            self.SetItemFont(self.current, font)

        dlg.Destroy()
        

    def OnItemHyperText(self, event):

        self.SetItemHyperText(self.current, not self.itemdict["ishtml"])


    def OnEnableWindow(self, event):

        enable = self.GetItemWindowEnabled(self.current)
        self.SetItemWindowEnabled(self.current, not enable)


    def OnDisableItem(self, event):

        self.EnableItem(self.current, False)
        

    def OnItemIcons(self, event):

        bitmaps = [self.itemdict["normal"], self.itemdict["selected"],
                   self.itemdict["expanded"], self.itemdict["selexp"]]

        wx.BeginBusyCursor()        
        dlg = TreeIcons(self, -1, bitmaps=bitmaps)
        wx.EndBusyCursor()
        dlg.ShowModal()


    def SetNewIcons(self, bitmaps):

        self.SetItemImage(self.current, bitmaps[0], CT.TreeItemIcon_Normal)
        self.SetItemImage(self.current, bitmaps[1], CT.TreeItemIcon_Selected)
        self.SetItemImage(self.current, bitmaps[2], CT.TreeItemIcon_Expanded)
        self.SetItemImage(self.current, bitmaps[3], CT.TreeItemIcon_SelectedExpanded)


    def OnItemInfo(self, event):

        itemtext = self.itemdict["text"]
        numchildren = str(self.itemdict["children"])
        itemtype = self.itemdict["itemtype"]
        pydata = repr(type(self.itemdict["pydata"]))

        if itemtype == 0:
            itemtype = "Normal"
        elif itemtype == 1:
            itemtype = "CheckBox"
        else:
            itemtype = "RadioButton"

        strs = "Information On Selected Item:\n\n" + "Text: " + itemtext + "\n" \
               "Number Of Children: " + numchildren + "\n" \
               "Item Type: " + itemtype + "\n" \
               "Item Data Type: " + pydata + "\n"

        dlg = wx.MessageDialog(self, strs, "CustomTreeCtrlDemo Info", wx.OK | wx.ICON_INFORMATION)
        dlg.ShowModal()
        dlg.Destroy()
                
        

    def OnItemDelete(self, event):

        strs = "Are You Sure You Want To Delete Item " + self.GetItemText(self.current) + "?"
        dlg = wx.MessageDialog(None, strs, 'Deleting Item', wx.YES_NO | wx.NO_DEFAULT | wx.CANCEL | wx.ICON_QUESTION)

        if dlg.ShowModal() in [wx.ID_NO, wx.ID_CANCEL]:
            dlg.Destroy()
            return

        dlg.Destroy()

        self.DeleteChildren(self.current)
        self.Delete(self.current)
        self.current = None
        


    def OnItemPrepend(self, event):

        dlg = wx.TextEntryDialog(self, "Please Enter The New Item Name", 'Item Naming', 'Python')

        if dlg.ShowModal() == wx.ID_OK:
            newname = dlg.GetValue()
            newitem = self.PrependItem(self.current, newname)
            self.EnsureVisible(newitem)

        dlg.Destroy()


    def OnItemAppend(self, event):

        dlg = wx.TextEntryDialog(self, "Please Enter The New Item Name", 'Item Naming', 'Python')

        if dlg.ShowModal() == wx.ID_OK:
            newname = dlg.GetValue()
            newitem = self.AppendItem(self.current, newname)
            self.EnsureVisible(newitem)

        dlg.Destroy()
        

    def OnBeginEdit(self, event):
        
        self.log.write("OnBeginEdit" + "\n")
        # show how to prevent edit...
        item = event.GetItem()
        if item and self.GetItemText(item) == "The Root Item":
            wx.Bell()
            self.log.write("You can't edit this one..." + "\n")

            # Lets just see what's visible of its children
            cookie = 0
            root = event.GetItem()
            (child, cookie) = self.GetFirstChild(root)

            while child:
                self.log.write("Child [%s] visible = %d" % (self.GetItemText(child), self.IsVisible(child)) + "\n")
                (child, cookie) = self.GetNextChild(root, cookie)

            event.Veto()


    def OnEndEdit(self, event):
        
        self.log.write("OnEndEdit: %s %s" %(event.IsEditCancelled(), event.GetLabel()))
        # show how to reject edit, we'll not allow any digits
        for x in event.GetLabel():
            if x in string.digits:
                self.log.write(", You can't enter digits..." + "\n")
                event.Veto()
                return
            
        self.log.write("\n")


    def OnLeftDClick(self, event):
        
        pt = event.GetPosition()
        item, flags = self.HitTest(pt)
        if item and (flags & CT.TREE_HITTEST_ONITEMLABEL):
            if self.GetTreeStyle() & CT.TR_EDIT_LABELS:
                self.log.write("OnLeftDClick: %s (manually starting label edit)"% self.GetItemText(item) + "\n")
                self.EditLabel(item)
            else:
                self.log.write("OnLeftDClick: Cannot Start Manual Editing, Missing Style TR_EDIT_LABELS\n")

        event.Skip()                
        

    def OnItemExpanded(self, event):
        
        item = event.GetItem()
        if item:
            self.log.write("OnItemExpanded: %s" % self.GetItemText(item) + "\n")


    def OnItemExpanding(self, event):
        
        item = event.GetItem()
        if item:
            self.log.write("OnItemExpanding: %s" % self.GetItemText(item) + "\n")
            
        event.Skip()

        
    def OnItemCollapsed(self, event):

        item = event.GetItem()
        if item:
            self.log.write("OnItemCollapsed: %s" % self.GetItemText(item) + "\n")
            

    def OnItemCollapsing(self, event):

        item = event.GetItem()
        if item:
            self.log.write("OnItemCollapsing: %s" % self.GetItemText(item) + "\n")
    
        event.Skip()

        
    def OnSelChanged(self, event):

        self.item = event.GetItem()
        if self.item:
            self.log.write("OnSelChanged: %s" % self.GetItemText(self.item))
            if wx.Platform == '__WXMSW__':
                self.log.write(", BoundingRect: %s" % self.GetBoundingRect(self.item, True) + "\n")
            else:
                self.log.write("\n")
                
        event.Skip()


    def OnSelChanging(self, event):

        item = event.GetItem()
        olditem = event.GetOldItem()
        
        if item:
            if not olditem:
                olditemtext = "None"
            else:
                olditemtext = self.GetItemText(olditem)
            self.log.write("OnSelChanging: From %s" % olditemtext + " To %s" % self.GetItemText(item) + "\n")
                
        event.Skip()


    def OnBeginDrag(self, event):

        self.item = event.GetItem()
        if self.item:
            self.log.write("Beginning Drag..." + "\n")

            event.Allow()


    def OnBeginRDrag(self, event):

        self.item = event.GetItem()
        if self.item:
            self.log.write("Beginning Right Drag..." + "\n")

            event.Allow()
        

    def OnEndDrag(self, event):

        self.item = event.GetItem()
        if self.item:
            self.log.write("Ending Drag!" + "\n")

        event.Skip()            


    def OnDeleteItem(self, event):

        item = event.GetItem()

        if not item:
            return

        self.log.write("Deleting Item: %s" % self.GetItemText(item) + "\n")
        event.Skip()
        

    def OnItemCheck(self, event):

        item = event.GetItem()
        self.log.write("Item " + self.GetItemText(item) + " Has Been Checked!\n")
        event.Skip()


    def OnItemChecking(self, event):

        item = event.GetItem()
        self.log.write("Item " + self.GetItemText(item) + " Is Being Checked...\n")
        event.Skip()
        

    def OnToolTip(self, event):

        item = event.GetItem()
        if item:
            event.SetToolTip(wx.ToolTip(self.GetItemText(item)))


    def OnItemMenu(self, event):

        item = event.GetItem()
        if item:
            self.log.write("OnItemMenu: %s" % self.GetItemText(item) + "\n")
    
        event.Skip()


    def OnKey(self, event):

        keycode = event.GetKeyCode()
        keyname = keyMap.get(keycode, None)
                
        if keycode == wx.WXK_BACK:
            self.log.write("OnKeyDown: HAHAHAHA! I Vetoed Your Backspace! HAHAHAHA\n")
            return

        if keyname is None:
            if "unicode" in wx.PlatformInfo:
                keycode = event.GetUnicodeKey()
                if keycode <= 127:
                    keycode = event.GetKeyCode()
                keyname = "\"" + unichr(event.GetUnicodeKey()) + "\""
                if keycode < 27:
                    keyname = "Ctrl-%s" % chr(ord('A') + keycode-1)
                
            elif keycode < 256:
                if keycode == 0:
                    keyname = "NUL"
                elif keycode < 27:
                    keyname = "Ctrl-%s" % chr(ord('A') + keycode-1)
                else:
                    keyname = "\"%s\"" % chr(keycode)
            else:
                keyname = "unknown (%s)" % keycode
                
        self.log.write("OnKeyDown: You Pressed '" + keyname + "'\n")

        event.Skip()
        
        
    def OnActivate(self, event):
        
        if self.item:
            self.log.write("OnActivate: %s" % self.GetItemText(self.item) + "\n")

        event.Skip()

        
    def OnHyperLink(self, event):

        item = event.GetItem()
        if item:
            self.log.write("OnHyperLink: %s" % self.GetItemText(self.item) + "\n")
            

    def OnTextCtrl(self, event):

        char = chr(event.GetKeyCode())
        self.log.write("EDITING THE TEXTCTRL: You Wrote '" + char + \
                       "' (KeyCode = " + str(event.GetKeyCode()) + ")\n")
        event.Skip()


    def OnComboBox(self, event):

        selection = event.GetEventObject().GetValue()
        self.log.write("CHOICE FROM COMBOBOX: You Chose '" + selection + "'\n")
        event.Skip()



#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = CustomTreeCtrlDemo(nb, log)
    return win

#----------------------------------------------------------------------



overview = CT.__doc__


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

