
import wx
import wx.lib.flatnotebook as fnb
import random
import images

#----------------------------------------------------------------------
def GetMondrianData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00 \x00\x00\x00 \x08\x06\x00\
\x00\x00szz\xf4\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\x00\x00qID\
ATX\x85\xed\xd6;\n\x800\x10E\xd1{\xc5\x8d\xb9r\x97\x16\x0b\xad$\x8a\x82:\x16\
o\xda\x84pB2\x1f\x81Fa\x8c\x9c\x08\x04Z{\xcf\xa72\xbcv\xfa\xc5\x08 \x80r\x80\
\xfc\xa2\x0e\x1c\xe4\xba\xfaX\x1d\xd0\xde]S\x07\x02\xd8>\xe1wa-`\x9fQ\xe9\
\x86\x01\x04\x10\x00\\(Dk\x1b-\x04\xdc\x1d\x07\x14\x98;\x0bS\x7f\x7f\xf9\x13\
\x04\x10@\xf9X\xbe\x00\xc9 \x14K\xc1<={\x00\x00\x00\x00IEND\xaeB`\x82' 


def GetMondrianBitmap():
    return wx.BitmapFromImage(GetMondrianImage())


def GetMondrianImage():
    import cStringIO
    stream = cStringIO.StringIO(GetMondrianData())
    return wx.ImageFromStream(stream)


def GetMondrianIcon():
    icon = wx.EmptyIcon()
    icon.CopyFromBitmap(GetMondrianBitmap())
    return icon
#----------------------------------------------------------------------


MENU_EDIT_DELETE_ALL = wx.ID_HIGHEST + 1000
MENU_EDIT_ADD_PAGE = MENU_EDIT_DELETE_ALL + 1
MENU_EDIT_DELETE_PAGE = MENU_EDIT_DELETE_ALL + 2
MENU_EDIT_SET_SELECTION = MENU_EDIT_DELETE_ALL + 3
MENU_EDIT_ADVANCE_SELECTION_FWD = MENU_EDIT_DELETE_ALL + 4
MENU_EDIT_ADVANCE_SELECTION_BACK = MENU_EDIT_DELETE_ALL + 5
MENU_SET_ALL_TABS_SHAPE_ANGLE = MENU_EDIT_DELETE_ALL + 6
MENU_SHOW_IMAGES = MENU_EDIT_DELETE_ALL + 7
MENU_USE_VC71_STYLE = MENU_EDIT_DELETE_ALL + 8
MENU_USE_DEFAULT_STYLE = MENU_EDIT_DELETE_ALL + 9
MENU_USE_FANCY_STYLE = MENU_EDIT_DELETE_ALL + 10
MENU_SELECT_GRADIENT_COLOR_FROM = MENU_EDIT_DELETE_ALL + 11
MENU_SELECT_GRADIENT_COLOR_TO = MENU_EDIT_DELETE_ALL + 12
MENU_SELECT_GRADIENT_COLOR_BORDER = MENU_EDIT_DELETE_ALL + 13
MENU_SET_PAGE_IMAGE_INDEX = MENU_EDIT_DELETE_ALL + 14
MENU_HIDE_X = MENU_EDIT_DELETE_ALL + 15
MENU_HIDE_NAV_BUTTONS = MENU_EDIT_DELETE_ALL + 16
MENU_USE_MOUSE_MIDDLE_BTN = MENU_EDIT_DELETE_ALL + 17
MENU_DRAW_BORDER = MENU_EDIT_DELETE_ALL + 18
MENU_USE_BOTTOM_TABS = MENU_EDIT_DELETE_ALL + 19
MENU_ENABLE_TAB = MENU_EDIT_DELETE_ALL + 20
MENU_DISABLE_TAB = MENU_EDIT_DELETE_ALL + 21
MENU_ENABLE_DRAG_N_DROP = MENU_EDIT_DELETE_ALL + 22
MENU_DCLICK_CLOSES_TAB = MENU_EDIT_DELETE_ALL + 23
MENU_USE_VC8_STYLE = MENU_EDIT_DELETE_ALL + 24

MENU_SET_ACTIVE_TEXT_COLOR = MENU_EDIT_DELETE_ALL + 27
MENU_DRAW_TAB_X = MENU_EDIT_DELETE_ALL + 28
MENU_SET_ACTIVE_TAB_COLOR = MENU_EDIT_DELETE_ALL + 29
MENU_SET_TAB_AREA_COLOR = MENU_EDIT_DELETE_ALL + 30
MENU_SELECT_NONACTIVE_TEXT_COLOR = MENU_EDIT_DELETE_ALL + 31
MENU_GRADIENT_BACKGROUND = MENU_EDIT_DELETE_ALL + 32
MENU_COLORFUL_TABS = MENU_EDIT_DELETE_ALL + 33


class FlatNotebookDemo(wx.Frame):

    def __init__(self, parent, log):

        wx.Frame.__init__(self, parent, title="FlatNotebook Demo", size=(800,600))
        self.log = log

        self._bShowImages = False
        self._bVCStyle = False
        self._newPageCounter = 0

        self._ImageList = wx.ImageList(16, 16)
        self._ImageList.Add(images.get_book_redBitmap())
        self._ImageList.Add(images.get_book_greenBitmap())
        self._ImageList.Add(images.get_book_blueBitmap())

        self.statusbar = self.CreateStatusBar(2, wx.ST_SIZEGRIP)
        self.statusbar.SetStatusWidths([-2, -1])
        # statusbar fields
        statusbar_fields = [("FlatNotebook wxPython Demo, Andrea Gavana @ 02 Oct 2006"),
                            ("Welcome To wxPython!")]

        for i in range(len(statusbar_fields)):
            self.statusbar.SetStatusText(statusbar_fields[i], i)

        self.SetIcon(GetMondrianIcon())
        self.CreateMenuBar()
        self.CreateRightClickMenu()
        self.LayoutItems()

        self.Bind(fnb.EVT_FLATNOTEBOOK_PAGE_CHANGING, self.OnPageChanging)
        self.Bind(fnb.EVT_FLATNOTEBOOK_PAGE_CHANGED, self.OnPageChanged)
        self.Bind(fnb.EVT_FLATNOTEBOOK_PAGE_CLOSING, self.OnPageClosing)



    def CreateMenuBar(self):

        self._menuBar = wx.MenuBar(wx.MB_DOCKABLE)
        self._fileMenu = wx.Menu()
        item = wx.MenuItem(self._fileMenu, wx.ID_ANY, "&Close\tCtrl-Q", "Close demo window")
        self.Bind(wx.EVT_MENU, self.OnQuit, item)
        self._fileMenu.AppendItem(item)
        
        self._editMenu = wx.Menu()
        item = wx.MenuItem(self._editMenu, MENU_EDIT_ADD_PAGE, "New Page\tCtrl+N", "Add New Page")
        self.Bind(wx.EVT_MENU, self.OnAddPage, item)
        self._editMenu.AppendItem(item)

        item = wx.MenuItem(self._editMenu, MENU_EDIT_DELETE_PAGE, "Delete Page\tCtrl+F4", "Delete Page")
        self.Bind(wx.EVT_MENU, self.OnDeletePage, item)
        self._editMenu.AppendItem(item)

        item = wx.MenuItem(self._editMenu, MENU_EDIT_DELETE_ALL, "Delete All Pages", "Delete All Pages")
        self.Bind(wx.EVT_MENU, self.OnDeleteAll, item)
        self._editMenu.AppendItem(item)

        item = wx.MenuItem(self._editMenu, MENU_EDIT_SET_SELECTION, "Set Selection", "Set Selection")
        self.Bind(wx.EVT_MENU, self.OnSetSelection, item)
        self._editMenu.AppendItem(item)

        item = wx.MenuItem(self._editMenu, MENU_EDIT_ADVANCE_SELECTION_FWD, "Advance Selection Forward",
                           "Advance Selection Forward")
        self.Bind(wx.EVT_MENU, self.OnAdvanceSelectionFwd, item)
        self._editMenu.AppendItem(item)

        item = wx.MenuItem(self._editMenu, MENU_EDIT_ADVANCE_SELECTION_BACK, "Advance Selection Backward",
                           "Advance Selection Backward")
        self.Bind(wx.EVT_MENU, self.OnAdvanceSelectionBack, item)
        self._editMenu.AppendItem(item)

        item = wx.MenuItem(self._editMenu, MENU_SET_ALL_TABS_SHAPE_ANGLE, "Set an inclination of tab header borders",
                           "Set the shape of tab header")
        self.Bind(wx.EVT_MENU, self.OnSetAllPagesShapeAngle, item)
        self._editMenu.AppendItem(item)

        item = wx.MenuItem(self._editMenu, MENU_SET_PAGE_IMAGE_INDEX, "Set image index of selected page",
                           "Set image index")
        self.Bind(wx.EVT_MENU, self.OnSetPageImageIndex, item)
        self._editMenu.AppendItem(item)

        item = wx.MenuItem(self._editMenu, MENU_SHOW_IMAGES, "Show Images", "Show Images", wx.ITEM_CHECK)
        self.Bind(wx.EVT_MENU, self.OnShowImages, item)
        self._editMenu.AppendItem(item)

        styleMenu = wx.Menu()
        item = wx.MenuItem(styleMenu, MENU_USE_DEFAULT_STYLE, "Use Default Style", "Use VC71 Style", wx.ITEM_RADIO)
        self.Bind(wx.EVT_MENU, self.OnDefaultStyle, item)
        styleMenu.AppendItem(item)

        item = wx.MenuItem(styleMenu, MENU_USE_VC71_STYLE, "Use VC71 Style", "Use VC71 Style", wx.ITEM_RADIO)
        self.Bind(wx.EVT_MENU, self.OnVC71Style, item)
        styleMenu.AppendItem(item)

        item = wx.MenuItem(styleMenu, MENU_USE_VC8_STYLE, "Use VC8 Style", "Use VC8 Style", wx.ITEM_RADIO)
        self.Bind(wx.EVT_MENU, self.OnVC8Style, item)
        styleMenu.AppendItem(item)

        item = wx.MenuItem(styleMenu, MENU_USE_FANCY_STYLE, "Use Fancy Style", "Use Fancy Style", wx.ITEM_RADIO)
        self.Bind(wx.EVT_MENU, self.OnFancyStyle, item)
        styleMenu.AppendItem(item)

        self._editMenu.AppendMenu(wx.ID_ANY, "Tabs Style", styleMenu)

        item = wx.MenuItem(styleMenu, MENU_SELECT_GRADIENT_COLOR_FROM, "Select fancy tab style 'from' color",
                           "Select fancy tab style 'from' color")
        self._editMenu.AppendItem(item)

        item = wx.MenuItem(styleMenu, MENU_SELECT_GRADIENT_COLOR_TO, "Select fancy tab style 'to' color",
                           "Select fancy tab style 'to' color")
        self._editMenu.AppendItem(item)

        item = wx.MenuItem(styleMenu, MENU_SELECT_GRADIENT_COLOR_BORDER, "Select fancy tab style 'border' color",
                           "Select fancy tab style 'border' color")
        self._editMenu.AppendItem(item)

        self.Bind(wx.EVT_MENU_RANGE, self.OnSelectColor, id=MENU_SELECT_GRADIENT_COLOR_FROM,
                  id2=MENU_SELECT_GRADIENT_COLOR_BORDER)        

        item = wx.MenuItem(self._editMenu, MENU_HIDE_NAV_BUTTONS, "Hide Navigation Buttons",
                           "Hide Navigation Buttons", wx.ITEM_CHECK)
        self._editMenu.AppendItem(item)

        item = wx.MenuItem(self._editMenu, MENU_HIDE_X, "Hide X Button", "Hide X Button", wx.ITEM_CHECK)
        self._editMenu.AppendItem(item)

        item = wx.MenuItem(self._editMenu, MENU_USE_MOUSE_MIDDLE_BTN, "Use Mouse Middle Button as 'X' button",
                           "Use Mouse Middle Button as 'X' button", wx.ITEM_CHECK)
        self._editMenu.AppendItem(item)

        item = wx.MenuItem(self._editMenu, MENU_USE_BOTTOM_TABS, "Use Bottoms Tabs", "Use Bottoms Tabs",
                           wx.ITEM_CHECK)
        self._editMenu.AppendItem(item)

        self.Bind(wx.EVT_MENU_RANGE, self.OnStyle, id=MENU_HIDE_X, id2=MENU_USE_BOTTOM_TABS)

        item = wx.MenuItem(self._editMenu, MENU_ENABLE_TAB, "Enable Tab", "Enable Tab")
        self.Bind(wx.EVT_MENU, self.OnEnableTab, item)
        self._editMenu.AppendItem(item)

        item = wx.MenuItem(self._editMenu, MENU_DISABLE_TAB, "Disable Tab", "Disable Tab")
        self.Bind(wx.EVT_MENU, self.OnDisableTab, item)
        self._editMenu.AppendItem(item)

        item = wx.MenuItem(self._editMenu, MENU_ENABLE_DRAG_N_DROP, "Enable Drag And Drop of Tabs",
                           "Enable Drag And Drop of Tabs", wx.ITEM_CHECK)
        self.Bind(wx.EVT_MENU, self.OnEnableDrag, item)        
        self._editMenu.AppendItem(item)
        item.Check(True)

        item = wx.MenuItem(self._editMenu, MENU_DRAW_BORDER, "Draw Border around tab area",
                           "Draw Border around tab area", wx.ITEM_CHECK)
        self.Bind(wx.EVT_MENU, self.OnStyle, item)
        self._editMenu.AppendItem(item)
        item.Check(True)

        item = wx.MenuItem(self._editMenu, MENU_DRAW_TAB_X, "Draw X button On Active Tab",
                           "Draw X button On Active Tab", wx.ITEM_CHECK)
        self.Bind(wx.EVT_MENU, self.OnDrawTabX, item)
        self._editMenu.AppendItem(item)
        
        item = wx.MenuItem(styleMenu, MENU_SET_ACTIVE_TAB_COLOR, "Select Active Tab Color",
                           "Select Active Tab Color")
        self.Bind(wx.EVT_MENU, self.OnSelectColor, item)
        self._editMenu.AppendItem(item)

        item = wx.MenuItem(styleMenu, MENU_SET_TAB_AREA_COLOR, "Select Tab Area Color",
                           "Select Tab Area Color")
        self.Bind(wx.EVT_MENU, self.OnSelectColor, item)
        self._editMenu.AppendItem(item)

        item = wx.MenuItem(styleMenu, MENU_SET_ACTIVE_TEXT_COLOR, "Select active tab text color",
                           "Select active tab text color")
        self.Bind(wx.EVT_MENU, self.OnSelectColor, item)
        self._editMenu.AppendItem(item)

        item = wx.MenuItem(styleMenu, MENU_SELECT_NONACTIVE_TEXT_COLOR,
                           "Select NON-active tab text color", "Select NON-active tab text color")
        self.Bind(wx.EVT_MENU, self.OnSelectColor, item)
        self._editMenu.AppendItem(item)

        item = wx.MenuItem(self._editMenu, MENU_DCLICK_CLOSES_TAB, "Mouse double click closes tab",
                           "Mouse double click closes tab", wx.ITEM_CHECK)
        self.Bind(wx.EVT_MENU, self.OnDClickCloseTab, item)
        self._editMenu.AppendItem(item)
        item.Check(False)
        
        item = wx.MenuItem(self._editMenu, MENU_GRADIENT_BACKGROUND, "Use Gradient Coloring for tab area",
                           "Use Gradient Coloring for tab area", wx.ITEM_CHECK)
        self.Bind(wx.EVT_MENU, self.OnGradientBack, item)
        self._editMenu.AppendItem(item)
        item.Check(False)

        item = wx.MenuItem(self._editMenu, MENU_COLORFUL_TABS, "Colorful tabs", "Colorful tabs", wx.ITEM_CHECK)
        self.Bind(wx.EVT_MENU, self.OnColorfulTabs, item)
        self._editMenu.AppendItem(item)
        item.Check(False)

        help_menu = wx.Menu()
        item = wx.MenuItem(help_menu, wx.ID_ANY, "About...", "Shows The About Dialog")
        self.Bind(wx.EVT_MENU, self.OnAbout, item)
        help_menu.AppendItem(item)

        self._menuBar.Append(self._fileMenu, "&File")
        self._menuBar.Append(self._editMenu, "&Edit")
        self._menuBar.Append(help_menu, "&Help")

        self.SetMenuBar(self._menuBar)


    def CreateRightClickMenu(self):

        self._rmenu = wx.Menu()
        item = wx.MenuItem(self._rmenu, MENU_EDIT_DELETE_PAGE, "Close Tab\tCtrl+F4", "Close Tab")
        self._rmenu.AppendItem(item)
        

    def LayoutItems(self):
        
        mainSizer = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer(mainSizer)

        bookStyle = fnb.FNB_TABS_BORDER_SIMPLE

        self.book = fnb.StyledNotebook(self, wx.ID_ANY, style=bookStyle)
        self.secondBook = fnb.StyledNotebook(self, wx.ID_ANY, style=bookStyle)

        # Set right click menu to the notebook
        self.book.SetRightClickMenu(self._rmenu)

        # Set the image list 
        self.book.SetImageList(self._ImageList)
        mainSizer.Add(self.book, 6, wx.EXPAND)

        # Add spacer between the books
        spacer = wx.Panel(self, -1)
        spacer.SetBackgroundColour(wx.SystemSettings_GetColour(wx.SYS_COLOUR_3DFACE))
        mainSizer.Add(spacer, 0, wx.ALL | wx.EXPAND)

        mainSizer.Add(self.secondBook, 2, wx.EXPAND)

        # Add some pages to the second notebook
        self.Freeze()

        text = wx.TextCtrl(self.secondBook, -1, "Second Book Page 1", style=wx.TE_MULTILINE)  
        self.secondBook.AddPage(text, "Second Book Page 1")

        text = wx.TextCtrl(self.secondBook, -1, "Second Book Page 2", style=wx.TE_MULTILINE)
        self.secondBook.AddPage(text,  "Second Book Page 2")

        self.Thaw()	

        mainSizer.Layout()
        self.SendSizeEvent()


    def OnStyle(self, event):

        style = self.book.GetWindowStyleFlag()
        eventid = event.GetId()
        
        if eventid == MENU_HIDE_NAV_BUTTONS:
            if event.IsChecked():            
                # Hide the navigation buttons
                style |= fnb.FNB_NO_NAV_BUTTONS
            else:
                if style & fnb.FNB_NO_NAV_BUTTONS:
                    style ^= fnb.FNB_NO_NAV_BUTTONS
            
            self.book.SetWindowStyleFlag(style)

        elif eventid == MENU_HIDE_X:
            if event.IsChecked():
                # Hide the X button
                style |= fnb.FNB_NO_X_BUTTON
            else:
                if style & fnb.FNB_NO_X_BUTTON:
                    style ^= fnb.FNB_NO_X_BUTTON
            
            self.book.SetWindowStyleFlag(style)
            
        elif eventid == MENU_DRAW_BORDER:
            if event.IsChecked():
                style |= fnb.FNB_TABS_BORDER_SIMPLE
            else:
                if style & fnb.FNB_TABS_BORDER_SIMPLE:
                    style ^= fnb.FNB_TABS_BORDER_SIMPLE
            
            self.book.SetWindowStyleFlag(style)

        elif eventid == MENU_USE_MOUSE_MIDDLE_BTN:
            if event.IsChecked():
                style |= fnb.FNB_MOUSE_MIDDLE_CLOSES_TABS            
            else:
                if style & fnb.FNB_MOUSE_MIDDLE_CLOSES_TABS:
                    style ^= fnb.FNB_MOUSE_MIDDLE_CLOSES_TABS
            
            self.book.SetWindowStyleFlag(style)

        elif eventid == MENU_USE_BOTTOM_TABS:
            if event.IsChecked():
                style |= fnb.FNB_BOTTOM
            else:
                if style & fnb.FNB_BOTTOM:
                    style ^= fnb.FNB_BOTTOM
            
            self.book.SetWindowStyleFlag(style)
            self.book.Refresh()


    def OnQuit(self, event):

    	self.Destroy()


    def OnDeleteAll(self, event):

    	self.book.DeleteAllPages()


    def OnShowImages(self, event):

        self._bShowImages = event.IsChecked()


    def OnVC71Style(self, event):

        style = self.book.GetWindowStyleFlag()

        # remove old tabs style
        mirror = ~(fnb.FNB_VC71 | fnb.FNB_VC8 | fnb.FNB_FANCY_TABS)
        style &= mirror

        style |= fnb.FNB_VC71

        self.book.SetWindowStyleFlag(style)


    def OnVC8Style(self, event):

        style = self.book.GetWindowStyleFlag()

        # remove old tabs style
        mirror = ~(fnb.FNB_VC71 | fnb.FNB_VC8 | fnb.FNB_FANCY_TABS)
        style &= mirror

        # set new style
        style |= fnb.FNB_VC8

        self.book.SetWindowStyleFlag(style)


    def OnDefaultStyle(self, event):

        style = self.book.GetWindowStyleFlag()

        # remove old tabs style
        mirror = ~(fnb.FNB_VC71 | fnb.FNB_VC8 | fnb.FNB_FANCY_TABS)
        style &= mirror

        self.book.SetWindowStyleFlag(style)


    def OnFancyStyle(self, event):

        style = self.book.GetWindowStyleFlag()

        # remove old tabs style
        mirror = ~(fnb.FNB_VC71 | fnb.FNB_VC8 | fnb.FNB_FANCY_TABS)
        style &= mirror

        style |= fnb.FNB_FANCY_TABS
        self.book.SetWindowStyleFlag(style)


    def OnSelectColor(self, event):

        eventid = event.GetId()
        
        # Open a color dialog
        data = wx.ColourData()
        
        dlg = wx.ColourDialog(self, data)
        
        if dlg.ShowModal() == wx.ID_OK:
        
            if eventid == MENU_SELECT_GRADIENT_COLOR_BORDER:
                self.book.SetGradientColorBorder(dlg.GetColourData().GetColour())
            elif eventid == MENU_SELECT_GRADIENT_COLOR_FROM:
                self.book.SetGradientColorFrom(dlg.GetColourData().GetColour())
            elif eventid == MENU_SELECT_GRADIENT_COLOR_TO:
                self.book.SetGradientColorTo(dlg.GetColourData().GetColour())
            elif eventid == MENU_SET_ACTIVE_TEXT_COLOR:
                self.book.SetActiveTabTextColour(dlg.GetColourData().GetColour())
            elif eventid == MENU_SELECT_NONACTIVE_TEXT_COLOR:
                self.book.SetNonActiveTabTextColour(dlg.GetColourData().GetColour())
            elif eventid == MENU_SET_ACTIVE_TAB_COLOR:
                self.book.SetActiveTabColour(dlg.GetColourData().GetColour())
            elif eventid == MENU_SET_TAB_AREA_COLOR:
                self.book.SetTabAreaColour(dlg.GetColourData().GetColour())
            
            self.book.Refresh()
        

    def OnAddPage(self, event):

        caption = "New Page Added #" + str(self._newPageCounter)
        
        self.Freeze()

        image = -1
        if self._bShowImages:
            image = random.randint(0, self._ImageList.GetImageCount()-1)
        
        self.book.AddPage(self.CreatePage(caption), caption, True, image)
        self.book.SetSelection(self.book.GetPageCount()-1)
        self.Thaw()
        self._newPageCounter = self._newPageCounter + 1


    def CreatePage(self, caption):

        p = wx.Panel(self.book)
        wx.StaticText(p, -1, caption, (20,20))
        wx.TextCtrl(p, -1, "", (20,40), (150,-1))
        #wx.TextCtrl(p, -1, "", (20,75), (150,-1), style=wx.TE_MULTILINE)
        return p
    


    def OnDeletePage(self, event):

    	self.book.DeletePage(self.book.GetSelection())


    def OnSetSelection(self, event):

        dlg = wx.TextEntryDialog(self, "Enter Tab Number to select:", "Set Selection")
        
        if dlg.ShowModal() == wx.ID_OK:
        
            val = dlg.GetValue()
            self.book.SetSelection(int(val))


    def OnEnableTab(self, event):

        dlg = wx.TextEntryDialog(self, "Enter Tab Number to enable:", "Enable Tab")
        
        if dlg.ShowModal() == wx.ID_OK:
        
            val = dlg.GetValue()
            self.book.Enable(int(val))	


    def OnDisableTab(self, event):

        dlg = wx.TextEntryDialog(self, "Enter Tab Number to disable:", "Disable Tab")
        
        if dlg.ShowModal() == wx.ID_OK:
        
            val = dlg.GetValue()
            self.book.Enable(int(val), False)	


    def OnEnableDrag(self, event):

        style = self.book.GetWindowStyleFlag()
        
        if event.IsChecked():        
            if style & fnb.FNB_NODRAG:
                style ^= fnb.FNB_NODRAG
        else:
            style |= fnb.FNB_NODRAG
        
        self.book.SetWindowStyleFlag(style)


    def OnSetAllPagesShapeAngle(self, event):

        
        dlg = wx.TextEntryDialog(self, "Enter an inclination of header borders (0-45):", "Set Angle")
        if dlg.ShowModal() == wx.ID_OK:
        
            val = dlg.GetValue()
            self.book.SetAllPagesShapeAngle(int(val))


    def OnSetPageImageIndex(self, event):

        dlg = wx.TextEntryDialog(self, "Enter an image index (0-%i):"%(self.book.GetImageList().GetImageCount()-1), "Set Image Index")
        if dlg.ShowModal() == wx.ID_OK:
            val = dlg.GetValue()
            self.book.SetPageImageIndex(self.book.GetSelection(), int(val))


    def OnAdvanceSelectionFwd(self, event):

    	self.book.AdvanceSelection(True)


    def OnAdvanceSelectionBack(self, event):

    	self.book.AdvanceSelection(False)


    def OnPageChanging(self, event):
        self.log.write("Page Changing From %d To %d" % (event.GetOldSelection(), event.GetSelection()))
        event.Skip()


    def OnPageChanged(self, event):
        self.log.write("Page Changed To %d" % event.GetSelection())
        event.Skip()


    def OnPageClosing(self, event):
        self.log.write("Page Closing, Selection: %d" % event.GetSelection())
        event.Skip()


    def OnDrawTabX(self, event):

        style = self.book.GetWindowStyleFlag()
        if event.IsChecked():
            style |= fnb.FNB_X_ON_TAB
        else:
            if style & fnb.FNB_X_ON_TAB:
                style ^= fnb.FNB_X_ON_TAB		
        
        self.book.SetWindowStyleFlag(style)


    def OnDClickCloseTab(self, event):

        style = self.book.GetWindowStyleFlag()
        if event.IsChecked():       
            style |= fnb.FNB_DCLICK_CLOSES_TABS
        else:
            style &= ~(fnb.FNB_DCLICK_CLOSES_TABS)		
        
        self.book.SetWindowStyleFlag(style)


    def OnGradientBack(self, event):

        style = self.book.GetWindowStyleFlag()
        if event.IsChecked():
            style |= fnb.FNB_BACKGROUND_GRADIENT
        else:
            style &= ~(fnb.FNB_BACKGROUND_GRADIENT)
            
        self.book.SetWindowStyleFlag(style)
        self.book.Refresh()


    def OnColorfulTabs(self, event):

        style = self.book.GetWindowStyleFlag()
        if event.IsChecked():
            style |= fnb.FNB_COLORFUL_TABS
        else:
            style &= ~(fnb.FNB_COLORFUL_TABS)

        self.book.SetWindowStyleFlag(style)
        self.book.Refresh()


    def OnAbout(self, event):

        msg = "This Is The About Dialog Of The FlatNotebook Demo.\n\n" + \
              "Author: Andrea Gavana @ 02 Oct 2006\n\n" + \
              "Please Report Any Bug/Requests Of Improvements\n" + \
              "To Me At The Following Adresses:\n\n" + \
              "andrea.gavana@gmail.com\n" + "gavana@kpo.kz\n\n" + \
              "Based On Eran C++ Implementation (wxWidgets Forum).\n\n" + \
              "Welcome To wxPython " + wx.VERSION_STRING + "!!"
              
        dlg = wx.MessageDialog(self, msg, "FlatNotebook wxPython Demo",
                               wx.OK | wx.ICON_INFORMATION)
        dlg.ShowModal()
        dlg.Destroy()        


#---------------------------------------------------------------------------


class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        b = wx.Button(self, -1, " Test FlatNotebook ", (50,50))
        self.Bind(wx.EVT_BUTTON, self.OnButton, b)


    def OnButton(self, evt):
        self.win = FlatNotebookDemo(self, self.log)
        self.win.Show(True)

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------


overview = fnb.__doc__



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

