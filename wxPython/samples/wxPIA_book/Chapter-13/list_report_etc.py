import wx
import sys, glob, random
import data

class DemoFrame(wx.Frame):
    def __init__(self):
        wx.Frame.__init__(self, None, -1,
                          "Other wx.ListCtrl Stuff",
                          size=(700,500))
        self.list = None
        self.editable = False
        self.MakeMenu()
        self.MakeListCtrl()


    def MakeListCtrl(self, otherflags=0):
        # if we already have a listctrl then get rid of it
        if self.list:
            self.list.Destroy()

        if self.editable:
            otherflags |= wx.LC_EDIT_LABELS
            
        # load some images into an image list
        il = wx.ImageList(16,16, True)
        for name in glob.glob("smicon??.png"):
            bmp = wx.Bitmap(name, wx.BITMAP_TYPE_PNG)
            il_max = il.Add(bmp)

        # create the list control
        self.list = wx.ListCtrl(self, -1, style=wx.LC_REPORT|otherflags)

        # assign the image list to it
        self.list.AssignImageList(il, wx.IMAGE_LIST_SMALL)

        # Add some columns
        for col, text in enumerate(data.columns):
            self.list.InsertColumn(col, text)

        # add the rows
        for row, item in enumerate(data.rows):
            index = self.list.InsertStringItem(sys.maxint, item[0])
            for col, text in enumerate(item[1:]):
                self.list.SetStringItem(index, col+1, text)

            # give each item a random image
            img = random.randint(0, il_max)
            self.list.SetItemImage(index, img, img)

            # set the data value for each item to be its position in
            # the data list
            self.list.SetItemData(index, row)
            
                
        # set the width of the columns in various ways
        self.list.SetColumnWidth(0, 120)
        self.list.SetColumnWidth(1, wx.LIST_AUTOSIZE)
        self.list.SetColumnWidth(2, wx.LIST_AUTOSIZE)
        self.list.SetColumnWidth(3, wx.LIST_AUTOSIZE_USEHEADER)

        # bind some interesting events
        self.Bind(wx.EVT_LIST_ITEM_SELECTED, self.OnItemSelected, self.list)
        self.Bind(wx.EVT_LIST_ITEM_DESELECTED, self.OnItemDeselected, self.list)
        self.Bind(wx.EVT_LIST_ITEM_ACTIVATED, self.OnItemActivated, self.list)

        # in case we are recreating the list tickle the frame a bit so
        # it will redo the layout
        self.SendSizeEvent()
        

    def MakeMenu(self):
        mbar = wx.MenuBar()
        menu = wx.Menu()
        item = menu.Append(-1, "E&xit\tAlt-X")
        self.Bind(wx.EVT_MENU, self.OnExit, item)
        mbar.Append(menu, "&File")

        menu = wx.Menu()
        item = menu.Append(-1, "Sort ascending")
        self.Bind(wx.EVT_MENU, self.OnSortAscending, item)        
        item = menu.Append(-1, "Sort descending")
        self.Bind(wx.EVT_MENU, self.OnSortDescending, item)
        item = menu.Append(-1, "Sort by submitter")
        self.Bind(wx.EVT_MENU, self.OnSortBySubmitter, item)

        menu.AppendSeparator()
        item = menu.Append(-1, "Show selected")
        self.Bind(wx.EVT_MENU, self.OnShowSelected, item)        
        item = menu.Append(-1, "Select all")
        self.Bind(wx.EVT_MENU, self.OnSelectAll, item)
        item = menu.Append(-1, "Select none")
        self.Bind(wx.EVT_MENU, self.OnSelectNone, item)

        menu.AppendSeparator()
        item = menu.Append(-1, "Set item text colour")
        self.Bind(wx.EVT_MENU, self.OnSetTextColour, item)
        item = menu.Append(-1, "Set item background colour")
        self.Bind(wx.EVT_MENU, self.OnSetBGColour, item)

        menu.AppendSeparator()
        item = menu.Append(-1, "Enable item editing", kind=wx.ITEM_CHECK)
        self.Bind(wx.EVT_MENU, self.OnEnableEditing, item)
        item = menu.Append(-1, "Edit current item")
        self.Bind(wx.EVT_MENU, self.OnEditItem, item)
        mbar.Append(menu, "&Demo")

        self.SetMenuBar(mbar)



    def OnExit(self, evt):
        self.Close()


    def OnItemSelected(self, evt):
        item = evt.GetItem()
        print "Item selected:", item.GetText()
        
    def OnItemDeselected(self, evt):
        item = evt.GetItem()
        print "Item deselected:", item.GetText()

    def OnItemActivated(self, evt): 
        item = evt.GetItem()
        print "Item activated:", item.GetText()

    def OnSortAscending(self, evt):
        # recreate the listctrl with a sort style
        self.MakeListCtrl(wx.LC_SORT_ASCENDING)
        
    def OnSortDescending(self, evt):
        # recreate the listctrl with a sort style
        self.MakeListCtrl(wx.LC_SORT_DESCENDING)

    def OnSortBySubmitter(self, evt):
        def compare_func(row1, row2):
            # compare the values in the 4th col of the data
            val1 = data.rows[row1][3]
            val2 = data.rows[row2][3]
            if val1 < val2: return -1
            if val1 > val2: return 1
            return 0

        self.list.SortItems(compare_func)
        


    def OnShowSelected(self, evt):
        print "These items are selected:"
        index = self.list.GetFirstSelected()
        if index == -1:
            print "\tNone"
            return
        while index != -1:
            item = self.list.GetItem(index)
            print "\t%s" % item.GetText()
            index = self.list.GetNextSelected(index)
            
    def OnSelectAll(self, evt):
        for index in range(self.list.GetItemCount()):
            self.list.Select(index, True)
    
    def OnSelectNone(self, evt):
        index = self.list.GetFirstSelected()
        while index != -1:
            self.list.Select(index, False)
            index = self.list.GetNextSelected(index)

    
    def OnSetTextColour(self, evt):
        dlg = wx.ColourDialog(self)
        if dlg.ShowModal() == wx.ID_OK:
            colour = dlg.GetColourData().GetColour()
            index = self.list.GetFirstSelected()
            while index != -1:
                self.list.SetItemTextColour(index, colour)
                index = self.list.GetNextSelected(index)
        dlg.Destroy()

    def OnSetBGColour(self, evt):
        dlg = wx.ColourDialog(self)
        if dlg.ShowModal() == wx.ID_OK:
            colour = dlg.GetColourData().GetColour()
            index = self.list.GetFirstSelected()
            while index != -1:
                self.list.SetItemBackgroundColour(index, colour)
                index = self.list.GetNextSelected(index)
        dlg.Destroy()


    def OnEnableEditing(self, evt):
        self.editable = evt.IsChecked()
        self.MakeListCtrl()
        
    def OnEditItem(self, evt):
        index = self.list.GetFirstSelected()
        if index != -1:
            self.list.EditLabel(index)
            
    
class DemoApp(wx.App):
    def OnInit(self):
        frame = DemoFrame()
        self.SetTopWindow(frame)
        print "Program output appears here..."
        frame.Show()
        return True

app = DemoApp(redirect=True)
app.MainLoop()
