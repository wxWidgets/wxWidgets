#!/bin/env python
#----------------------------------------------------------------------------
# Name:         test4.py
# Purpose:      Testing lots of stuff, controls, window types, etc.
#
# Author:       Robin Dunn
#
# Created:
# RCS-ID:       $Id$
# Copyright:    (c) 1998 by Total Control Software
# Licence:      wxWindows license
#----------------------------------------------------------------------------


from wxPython.wx import *

import time

#---------------------------------------------------------------------------

class TestSimpleControlsDlg(wxDialog):
    def __init__(self, parent, log):
        self.log = log
        wxDialog.__init__(self, parent, -1, "Test Simple Controls",
                          wxDefaultPosition, wxSize(350, 400))


        sampleList = ["zero", "one", "two", "three", "four", "five",
                      "six", "seven", "eight", "nine", "ten"]

        y_pos = 5
        delta = 25

        wxStaticText(self, -1, "wxTextCtrl", wxPoint(5, y_pos), wxSize(75, 20))
        wxTextCtrl(self, 10, "", wxPoint(80, y_pos), wxSize(150, 20))
        EVT_TEXT(self, 10, self.EvtText)
        y_pos = y_pos + delta

        wxCheckBox(self, 20, "wxCheckBox", wxPoint(80, y_pos), wxSize(150, 20), wxNO_BORDER)
        EVT_CHECKBOX(self, 20, self.EvtCheckBox)
        y_pos = y_pos + delta

        rb = wxRadioBox(self, 30, "wxRadioBox", wxPoint(80, y_pos), wxDefaultSize,
                        sampleList, 3, wxRA_SPECIFY_COLS | wxNO_BORDER)
        EVT_RADIOBOX(self, 30, self.EvtRadioBox)
        width, height = rb.GetSizeTuple()
        y_pos = y_pos + height + 5

        wxStaticText(self, -1, "wxChoice", wxPoint(5, y_pos), wxSize(75, 20))
        wxChoice(self, 40, wxPoint(80, y_pos), wxSize(95, 20), #wxDefaultSize,
                 sampleList)
        EVT_CHOICE(self, 40, self.EvtChoice)
        y_pos = y_pos + delta

        wxStaticText(self, -1, "wxComboBox", wxPoint(5, y_pos), wxSize(75, 18))
        wxComboBox(self, 50, "default value", wxPoint(80, y_pos), wxSize(95, -1),
                   sampleList, wxCB_DROPDOWN)
        EVT_COMBOBOX(self, 50, self.EvtComboBox)
        y_pos = y_pos + delta

        wxStaticText(self, -1, "wxListBox", wxPoint(5, y_pos), wxSize(75, 18))
        lb = wxListBox(self, 60, wxPoint(80, y_pos), wxSize(95, 80),
                       sampleList, wxLB_SINGLE)
        EVT_LISTBOX(self, 60, self.EvtListBox)
        EVT_LISTBOX_DCLICK(self, 60, self.EvtListBoxDClick)
        lb.SetSelection(0)
        width, height = lb.GetSizeTuple()
        y_pos = y_pos + height + 5



        y_pos = y_pos + 15
        wxButton(self, wxID_OK, ' OK ', wxPoint(80, y_pos), wxDefaultSize).SetDefault()
        wxButton(self, wxID_CANCEL, ' Cancel ', wxPoint(140, y_pos))


    def EvtText(self, event):
        self.log.WriteText('EvtText: %s\n' % event.GetString())

    def EvtCheckBox(self, event):
        self.log.WriteText('EvtCheckBox: %d\n' % event.GetInt())

    def EvtRadioBox(self, event):
        self.log.WriteText('EvtRadioBox: %d\n' % event.GetInt())

    def EvtChoice(self, event):
        self.log.WriteText('EvtChoice: %s\n' % event.GetString())

    def EvtComboBox(self, event):
        self.log.WriteText('EvtComboBox: %s\n' % event.GetString())

    def EvtListBox(self, event):
        self.log.WriteText('EvtListBox: %s\n' % event.GetString())

    def EvtListBoxDClick(self, event):
        self.log.WriteText('EvtListBoxDClick:\n')



#---------------------------------------------------------------------------

class TestTimer(wxTimer):
    def __init__(self, log):
        wxTimer.__init__(self)
        self.log = log

    def Notify(self):
        wxBell()
        self.log.WriteText('beep!\n')


#---------------------------------------------------------------------------

class TestLayoutConstraints(wxFrame):
    def __init__(self, parent):
        wxFrame.__init__(self, parent, -1, 'Test Layout Constraints',
                         wxDefaultPosition, wxSize(500, 300))

        self.SetAutoLayout(true)
        EVT_BUTTON(self, 100, self.OnButton)

        self.panelA = wxWindow(self, -1, wxDefaultPosition, wxDefaultSize,
                               wxSIMPLE_BORDER)
        self.panelA.SetBackgroundColour(wxBLUE)
        lc = wxLayoutConstraints()
        lc.top.SameAs(self, wxTop, 10)
        lc.left.SameAs(self, wxLeft, 10)
        lc.bottom.SameAs(self, wxBottom, 10)
        lc.right.PercentOf(self, wxRight, 50)
        self.panelA.SetConstraints(lc)

        self.panelB = wxWindow(self, -1, wxDefaultPosition, wxDefaultSize,
                               wxSIMPLE_BORDER)
        self.panelB.SetBackgroundColour(wxRED)
        lc = wxLayoutConstraints()
        lc.top.SameAs(self, wxTop, 10)
        lc.right.SameAs(self, wxRight, 10)
        lc.bottom.PercentOf(self, wxBottom, 30)
        lc.left.RightOf(self.panelA, 10)
        self.panelB.SetConstraints(lc)

        self.panelC = wxWindow(self, -1, wxDefaultPosition, wxDefaultSize,
                               wxSIMPLE_BORDER)
        self.panelC.SetBackgroundColour(wxWHITE)
        lc = wxLayoutConstraints()
        lc.top.Below(self.panelB, 10)
        lc.right.SameAs(self, wxRight, 10)
        lc.bottom.SameAs(self, wxBottom, 10)
        lc.left.RightOf(self.panelA, 10)
        self.panelC.SetConstraints(lc)

        b = wxButton(self.panelA, 100, ' Panel A ')
        lc = wxLayoutConstraints()
        lc.centreX.SameAs   (self.panelA, wxCentreX)
        lc.centreY.SameAs   (self.panelA, wxCentreY)
        lc.height.AsIs      ()
        lc.width.PercentOf  (self.panelA, wxWidth, 50)
        b.SetConstraints(lc);

        b = wxButton(self.panelB, 100, ' Panel B ')
        lc = wxLayoutConstraints()
        lc.top.SameAs       (self.panelB, wxTop, 2)
        lc.right.SameAs     (self.panelB, wxRight, 4)
        lc.height.AsIs      ()
        lc.width.AsIs       ()
        b.SetConstraints(lc);

        self.panelD = wxWindow(self.panelC, -1, wxDefaultPosition, wxDefaultSize,
                            wxSIMPLE_BORDER)
        self.panelD.SetBackgroundColour(wxGREEN)
        wxStaticText(self.panelD, -1, "Panel D", wxPoint(4, 4)).SetBackgroundColour(wxGREEN)

        b = wxButton(self.panelC, 100, ' Panel C ')
        lc = wxLayoutConstraints()
        lc.top.Below        (self.panelD)
        lc.left.RightOf     (self.panelD)
        lc.height.AsIs      ()
        lc.width.AsIs       ()
        b.SetConstraints(lc);

        lc = wxLayoutConstraints()
        lc.bottom.PercentOf (self.panelC, wxHeight, 50)
        lc.right.PercentOf  (self.panelC, wxWidth, 50)
        lc.height.SameAs    (b, wxHeight)
        lc.width.SameAs     (b, wxWidth)
        self.panelD.SetConstraints(lc);


    def OnButton(self, event):
        self.Close(true)


    def OnCloseWindow(self, event):
        self.Destroy()


#---------------------------------------------------------------------------

class TestGrid(wxFrame):
    def __init__(self, parent, log):
        wxFrame.__init__(self, parent, -1, 'Test Grid',
                         wxDefaultPosition, wxSize(500, 300))
        self.log = log

        grid = wxGrid(self, -1)

        grid.CreateGrid(16, 16)
        grid.SetColumnWidth(3, 200)
        grid.SetRowHeight(4, 45)
        grid.SetCellValue("First cell", 0, 0)
        grid.SetCellValue("Another cell", 1, 1)
        grid.SetCellValue("Yet another cell", 2, 2)
        grid.SetCellTextFont(wxFont(12, wxROMAN, wxITALIC, wxNORMAL), 0, 0)
        grid.SetCellTextColour(wxRED, 1, 1)
        grid.SetCellBackgroundColour(wxCYAN, 2, 2)
        grid.UpdateDimensions()
        grid.AdjustScrollbars()

        EVT_GRID_SELECT_CELL(grid, self.OnSelectCell)
        EVT_GRID_CELL_CHANGE(grid, self.OnCellChange)
        EVT_GRID_CELL_LCLICK(grid, self.OnCellClick)
        EVT_GRID_LABEL_LCLICK(grid, self.OnLabelClick)



    def OnCloseWindow(self, event):
        self.Destroy()

    def OnSelectCell(self, event):
        self.log.WriteText("OnSelectCell: (%d, %d)\n" % (event.m_row, event.m_col))

    def OnCellChange(self, event):
        self.log.WriteText("OnCellChange: (%d, %d)\n" % (event.m_row, event.m_col))

    def OnCellClick(self, event):
        self.log.WriteText("OnCellClick: (%d, %d)\n" % (event.m_row, event.m_col))

    def OnLabelClick(self, event):
        self.log.WriteText("OnLabelClick: (%d, %d)\n" % (event.m_row, event.m_col))

#---------------------------------------------------------------------------


class ColoredPanel(wxWindow):
    def __init__(self, parent, color):
        wxWindow.__init__(self, parent, -1,
                          wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER)
        self.SetBackgroundColour(color)


class TestNotebookWindow(wxFrame):
    def __init__(self, parent, log):
        wxFrame.__init__(self, parent, -1, 'Test wxNotebook')

        nb = wxNotebook(self, -1, wxPoint(0,0), self.GetClientSize())

        win = ColoredPanel(nb, wxBLUE)
        nb.AddPage(win, "Blue")
        st = wxStaticText(win, -1,
                          "You can put nearly any type of window here!",
                          wxPoint(10, 10))
        st.SetForegroundColour(wxWHITE)
        st.SetBackgroundColour(wxBLUE)
        st = wxStaticText(win, -1,
                          "Check the next tab for an example...",
                          wxPoint(10, 30))
        st.SetForegroundColour(wxWHITE)
        st.SetBackgroundColour(wxBLUE)

        win = TestTreeCtrlPanel(nb, log)
        nb.AddPage(win, "TreeCtrl")

        win = TestListCtrlPanel(nb, log)
        nb.AddPage(win, "ListCtrl")

        win = ColoredPanel(nb, wxRED)
        nb.AddPage(win, "Red")

        win = ColoredPanel(nb, wxGREEN)
        nb.AddPage(win, "Green")

        win = ColoredPanel(nb, wxCYAN)
        nb.AddPage(win, "Cyan")

        win = ColoredPanel(nb, wxWHITE)
        nb.AddPage(win, "White")

        win = ColoredPanel(nb, wxBLACK)
        nb.AddPage(win, "Black")

        win = ColoredPanel(nb, wxNamedColour('MIDNIGHT BLUE'))
        nb.AddPage(win, "MIDNIGHT BLUE")

        win = ColoredPanel(nb, wxNamedColour('INDIAN RED'))
        nb.AddPage(win, "INDIAN RED")

        nb.SetSelection(0)
        self.SetSize(wxSize(350, 300))


    def OnCloseWindow(self, event):
        self.Destroy()

#---------------------------------------------------------------------------

class TestSplitterWindow(wxFrame):
    def __init__(self, parent):
        wxFrame.__init__(self, parent, -1, 'Test wxSplitterWindow',
                         wxDefaultPosition, wxSize(500, 300))

        splitter = wxSplitterWindow(self, -1)

        p1 = ColoredPanel(splitter, wxRED)
        wxStaticText(p1, -1, "Panel One", wxPoint(5,5)).SetBackgroundColour(wxRED)

        p2 = ColoredPanel(splitter, wxBLUE)
        wxStaticText(p2, -1, "Panel Two", wxPoint(5,5)).SetBackgroundColour(wxBLUE)

        splitter.SplitVertically(p1, p2)


    def OnCloseWindow(self, event):
        self.Destroy()


#---------------------------------------------------------------------------

class CustomStatusBar(wxStatusBar):
    def __init__(self, parent):
        wxStatusBar.__init__(self, parent, -1)
        self.SetFieldsCount(3)

        self.SetStatusText("A Custom StatusBar...", 0)

        self.cb = wxCheckBox(self, 1001, "toggle clock")
        EVT_CHECKBOX(self, 1001, self.OnToggleClock)
        self.cb.SetValue(true)

        # figure out how tall to make it.
        dc = wxClientDC(self)
        dc.SetFont(self.GetFont())
        (w,h) = dc.GetTextExtent('X')
        h = int(h * 1.8)
        self.SetSize(wxSize(100, h))

        # start our timer
        self.timer = wxPyTimer(self.Notify)
        self.timer.Start(1000)
        self.Notify()


    # Time-out handler
    def Notify(self):
        t = time.localtime(time.time())
        st = time.strftime("%d-%b-%Y   %I:%M:%S", t)
        self.SetStatusText(st, 2)

    # the checkbox was clicked
    def OnToggleClock(self, event):
        if self.cb.GetValue():
            self.timer.Start(1000)
            self.Notify()
        else:
            self.timer.Stop()

    # reposition the checkbox
    def OnSize(self, event):
        rect = self.GetFieldRect(1)
        print "%s, %s" % (rect.x, rect.y)
        self.cb.SetPosition(wxPoint(rect.x+2, rect.y+2))
        self.cb.SetSize(wxSize(rect.width-4, rect.height-4))



class TestCustomStatusBar(wxFrame):
    def __init__(self, parent):
        wxFrame.__init__(self, parent, -1, 'Test Custom StatusBar',
                         wxDefaultPosition, wxSize(500, 300))
        wxWindow(self, -1).SetBackgroundColour(wxNamedColour("WHITE"))

        self.sb = CustomStatusBar(self)
        self.SetStatusBar(self.sb)

    def OnCloseWindow(self, event):
        self.sb.timer.Stop()
        self.Destroy()


#---------------------------------------------------------------------------

class TestToolBar(wxFrame):
    def __init__(self, parent, log):
        wxFrame.__init__(self, parent, -1, 'Test ToolBar',
                         wxDefaultPosition, wxSize(500, 300))
        self.log = log

        wxWindow(self, -1).SetBackgroundColour(wxNamedColour("WHITE"))

        tb = self.CreateToolBar(wxTB_HORIZONTAL|wxNO_BORDER)
        #tb = wxToolBar(self, -1, wxDefaultPosition, wxDefaultSize,
        #               wxTB_HORIZONTAL | wxNO_BORDER | wxTB_FLAT)
        #self.SetToolBar(tb)

        self.CreateStatusBar()

        tb.AddTool(10, wxNoRefBitmap('bitmaps/new.bmp',   wxBITMAP_TYPE_BMP),
                        wxNullBitmap, false, -1, -1, "New", "Long help for 'New'")
        EVT_TOOL(self, 10, self.OnToolClick)
        EVT_TOOL_RCLICKED(self, 10, self.OnToolRClick)

        tb.AddTool(20, wxNoRefBitmap('bitmaps/open.bmp',  wxBITMAP_TYPE_BMP),
                        wxNullBitmap, false, -1, -1, "Open")
        EVT_TOOL(self, 20, self.OnToolClick)
        EVT_TOOL_RCLICKED(self, 20, self.OnToolRClick)

        tb.AddSeparator()
        tb.AddTool(30, wxNoRefBitmap('bitmaps/copy.bmp',  wxBITMAP_TYPE_BMP),
                        wxNullBitmap, false, -1, -1, "Copy")
        EVT_TOOL(self, 30, self.OnToolClick)
        EVT_TOOL_RCLICKED(self, 30, self.OnToolRClick)

        tb.AddTool(40, wxNoRefBitmap('bitmaps/paste.bmp', wxBITMAP_TYPE_BMP),
                        wxNullBitmap, false, -1, -1, "Paste")
        EVT_TOOL(self, 40, self.OnToolClick)
        EVT_TOOL_RCLICKED(self, 40, self.OnToolRClick)

        tb.AddSeparator()

        tb.AddTool(50, wxNoRefBitmap('bitmaps/tog1.bmp', wxBITMAP_TYPE_BMP),
                        wxNullBitmap, true, -1, -1, "Toggle this")
        EVT_TOOL(self, 50, self.OnToolClick)
        EVT_TOOL_RCLICKED(self, 50, self.OnToolRClick)

        tb.AddTool(60, wxNoRefBitmap('bitmaps/tog1.bmp', wxBITMAP_TYPE_BMP),
                        wxNoRefBitmap('bitmaps/tog2.bmp', wxBITMAP_TYPE_BMP),
                        true, -1, -1, "Toggle with 2 bitmaps")
        EVT_TOOL(self, 60, self.OnToolClick)
        EVT_TOOL_RCLICKED(self, 60, self.OnToolRClick)

        tb.Realize()


    def OnCloseWindow(self, event):
        self.Destroy()

    def OnToolClick(self, event):
        self.log.WriteText("tool %s clicked\n" % event.GetId())

    def OnToolRClick(self, event):
        self.log.WriteText("tool %s right-clicked\n" % event.GetId())


#---------------------------------------------------------------------------

class TestTreeCtrlPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)

        self.log = log
        tID = 1101

        self.tree = wxTreeCtrl(self, tID)
        root = self.tree.AddRoot("The Root Item")
        for x in range(10):
            child = self.tree.AppendItem(root, "Item %d" % x)
            for y in range(5):
                last = self.tree.AppendItem(child, "item %d-%s" % (x, chr(ord("a")+y)))

        self.tree.Expand(root)
        EVT_TREE_ITEM_EXPANDED  (self, tID, self.OnItemExpanded)
        EVT_TREE_ITEM_COLLAPSED (self, tID, self.OnItemCollapsed)
        EVT_TREE_SEL_CHANGED    (self, tID, self.OnSelChanged)


    def OnSize(self, event):
        w,h = self.GetClientSizeTuple()
        self.tree.SetDimensions(0, 0, w, h)


    def OnItemExpanded(self, event):
        item = event.GetItem()
        self.log.WriteText("OnItemExpanded: %s\n" % self.tree.GetItemText(item))

    def OnItemCollapsed(self, event):
        item = event.GetItem()
        self.log.WriteText("OnItemCollapsed: %s\n" % self.tree.GetItemText(item))

    def OnSelChanged(self, event):
        item = event.GetItem()
        self.log.WriteText("OnSelChanged: %s\n" % self.tree.GetItemText(item))




class TestTreeCtrl(wxFrame):
    def __init__(self, parent, log):
        wxFrame.__init__(self, parent, -1, 'Test TreeCtrl',
                         wxDefaultPosition, wxSize(250, 300))

        p = TestTreeCtrlPanel(self, log)


#---------------------------------------------------------------------------

class TestListCtrlPanel(wxPanel):
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)

        self.log = log
        tID = 1101

        self.il = wxImageList(16, 16)
        idx1 = self.il.Add(wxNoRefBitmap('bitmaps/smiles.bmp', wxBITMAP_TYPE_BMP))

        self.list = wxListCtrl(self, tID, wxDefaultPosition, wxDefaultSize,
                               wxLC_REPORT|wxSUNKEN_BORDER)
        self.list.SetImageList(self.il, wxIMAGE_LIST_SMALL)

        #self.list.SetToolTip(wxToolTip("This is a ToolTip!"))
        #wxToolTip_Enable(true)

        self.list.InsertColumn(0, "Column 0")
        self.list.InsertColumn(1, "Column 1")
        self.list.InsertColumn(2, "One More Column (2)")
        for x in range(50):
            self.list.InsertImageStringItem(x, "This is item %d" % x, idx1)
            self.list.SetStringItem(x, 1, "Col 1, item %d" % x)
            self.list.SetStringItem(x, 2, "item %d in column 2" % x)

        self.list.SetColumnWidth(0, wxLIST_AUTOSIZE)
        self.list.SetColumnWidth(1, wxLIST_AUTOSIZE)
        self.list.SetColumnWidth(2, wxLIST_AUTOSIZE)


    def OnSize(self, event):
        w,h = self.GetClientSizeTuple()
        self.list.SetDimensions(0, 0, w, h)




class TestListCtrl(wxFrame):
    def __init__(self, parent, log):
        wxFrame.__init__(self, parent, -1, 'Test ListCtrl',
                         wxDefaultPosition, wxSize(250, 300))

        p = TestListCtrlPanel(self, log)


#---------------------------------------------------------------------------

class TestSashWindow(wxMDIParentFrame):
    NEW_WINDOW       = 5000
    TOGGLE_WINDOW    = 5001
    QUIT             = 5002
    ID_WINDOW_TOP    = 5100
    ID_WINDOW_LEFT1  = 5101
    ID_WINDOW_LEFT2  = 5102
    ID_WINDOW_BOTTOM = 5103


    def __init__(self, parent, log):
        wxMDIParentFrame.__init__(self, parent, -1, 'Test Sash Window',
                                  wxDefaultPosition, wxSize(250, 300))

        self.log = log
        menu = wxMenu()
        menu.Append(self.NEW_WINDOW, "&New Window")
        menu.Append(self.TOGGLE_WINDOW, "&Toggle window")
        menu.Append(self.QUIT, "E&xit")

        menubar = wxMenuBar()
        menubar.Append(menu, "&File")

        self.SetMenuBar(menubar)
        self.CreateStatusBar()

        EVT_MENU(self, self.NEW_WINDOW,    self.OnNewWindow)
        EVT_MENU(self, self.TOGGLE_WINDOW, self.OnToggleWindow)
        EVT_MENU(self, self.QUIT,          self.OnQuit)

        EVT_SASH_DRAGGED_RANGE(self, self.ID_WINDOW_TOP,
                               self.ID_WINDOW_BOTTOM, self.OnSashDrag)


        # Create some layout windows
        # A window like a toolbar
        win = wxSashLayoutWindow(self, self.ID_WINDOW_TOP, wxDefaultPosition,
                                 wxSize(200, 30), wxNO_BORDER|wxSW_3D)
        win.SetDefaultSize(wxSize(1000, 30))
        win.SetOrientation(wxLAYOUT_HORIZONTAL)
        win.SetAlignment(wxLAYOUT_TOP)
        win.SetBackgroundColour(wxColour(255, 0, 0))
        win.SetSashVisible(wxSASH_BOTTOM, true)

        self.topWindow = win


        # A window like a statusbar
        win = wxSashLayoutWindow(self, self.ID_WINDOW_BOTTOM,
                                 wxDefaultPosition, wxSize(200, 30),
                                 wxNO_BORDER|wxSW_3D)
        win.SetDefaultSize(wxSize(1000, 30))
        win.SetOrientation(wxLAYOUT_HORIZONTAL)
        win.SetAlignment(wxLAYOUT_BOTTOM)
        win.SetBackgroundColour(wxColour(0, 0, 255))
        win.SetSashVisible(wxSASH_TOP, true)

        self.bottomWindow = win


        # A window to the left of the client window
        win =  wxSashLayoutWindow(self, self.ID_WINDOW_LEFT1,
                                  wxDefaultPosition, wxSize(200, 30),
                                  wxNO_BORDER|wxSW_3D)
        win.SetDefaultSize(wxSize(120, 1000))
        win.SetOrientation(wxLAYOUT_VERTICAL)
        win.SetAlignment(wxLAYOUT_LEFT)
        win.SetBackgroundColour(wxColour(0, 255, 0))
        win.SetSashVisible(wxSASH_RIGHT, TRUE)
        win.SetExtraBorderSize(10)

        textWindow = wxTextCtrl(win, -1, "", wxDefaultPosition, wxDefaultSize,
                                wxTE_MULTILINE|wxSUNKEN_BORDER)
        textWindow.SetValue("A help window")

        self.leftWindow1 = win


        # Another window to the left of the client window
        win = wxSashLayoutWindow(self, self.ID_WINDOW_LEFT2,
                                 wxDefaultPosition, wxSize(200, 30),
                                 wxNO_BORDER|wxSW_3D)
        win.SetDefaultSize(wxSize(120, 1000))
        win.SetOrientation(wxLAYOUT_VERTICAL)
        win.SetAlignment(wxLAYOUT_LEFT)
        win.SetBackgroundColour(wxColour(0, 255, 255))
        win.SetSashVisible(wxSASH_RIGHT, TRUE)

        self.leftWindow2 = win


    def OnNewWindow(self, event):
        pass

    def OnToggleWindow(self, event):
        pass

    def OnQuit(self, event):
        self.Close(true)

    def OnSashDrag(self, event):
        if event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE:
            return

        eID = event.GetId()
        if eID == self.ID_WINDOW_TOP:
            self.topWindow.SetDefaultSize(wxSize(1000, event.GetDragRect().height))

        elif eID == self.ID_WINDOW_LEFT1:
            self.leftWindow1.SetDefaultSize(wxSize(event.GetDragRect().width, 1000))


        elif eID == self.ID_WINDOW_LEFT2:
            self.leftWindow2.SetDefaultSize(wxSize(event.GetDragRect().width, 1000))

        elif eID == self.ID_WINDOW_BOTTOM:
            self.bottomWindow.SetDefaultSize(wxSize(1000, event.GetDragRect().height))

        wxLayoutAlgorithm().LayoutMDIFrame(self)

        # Leaves bits of itself behind sometimes
        self.GetClientWindow().Refresh()


    def OnSize(self, event):
        wxLayoutAlgorithm().LayoutMDIFrame(self)

#---------------------------------------------------------------------------
#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

class AppFrame(wxFrame):
    def __init__(self, parent, id, title):
        wxFrame.__init__(self, parent, id, title, wxDefaultPosition,
                         wxSize(420, 200))
        if wxPlatform == '__WXMSW__':
            self.icon = wxIcon('bitmaps/mondrian.ico', wxBITMAP_TYPE_ICO)
            self.SetIcon(self.icon)

        self.mainmenu = wxMenuBar()
        menu = wxMenu()
        menu.Append(200, 'E&xit', 'Get the heck outta here!')
        EVT_MENU(self, 200, self.OnFileExit)
        self.mainmenu.Append(menu, '&File')

        menu = self.MakeTestsMenu()
        self.mainmenu.Append(menu, '&Tests')
        self.SetMenuBar(self.mainmenu)

        self.log = wxTextCtrl(self, -1, '', wxDefaultPosition, wxDefaultSize,
                              wxTE_MULTILINE|wxTE_READONLY)
        self.log.WriteText('Test 4:\n')
        (w, self.charHeight) = self.log.GetTextExtent('X')


    def MakeTestsMenu(self):
        menu = wxMenu()

        mID = NewId()
        menu.Append(mID, '&Simple Controls')
        EVT_MENU(self, mID, self.OnTestSimpleControls)

        mID = NewId()
        menu.Append(mID, '&Timer', '', true)
        EVT_MENU(self, mID, self.OnTestTimer)
        self.timerID = mID
        self.timer = None

        mID = NewId()
        menu.Append(mID, '&Layout Constraints')
        EVT_MENU(self, mID, self.OnTestLayoutConstraints)

        mID = NewId()
        menu.Append(mID, '&Grid')
        EVT_MENU(self, mID, self.OnTestGrid)


        smenu = wxMenu()   # make a sub-menu

        mID = NewId()
        smenu.Append(mID, '&Colour')
        EVT_MENU(self, mID, self.OnTestColourDlg)

        mID = NewId()
        smenu.Append(mID, '&Directory')
        EVT_MENU(self, mID, self.OnTestDirDlg)

        mID = NewId()
        smenu.Append(mID, '&File')
        EVT_MENU(self, mID, self.OnTestFileDlg)

        mID = NewId()
        smenu.Append(mID, '&Single Choice')
        EVT_MENU(self, mID, self.OnTestSingleChoiceDlg)

        mID = NewId()
        smenu.Append(mID, '&TextEntry')
        EVT_MENU(self, mID, self.OnTestTextEntryDlg)

        mID = NewId()
        smenu.Append(mID, '&Font')
        EVT_MENU(self, mID, self.OnTestFontDlg)

        mID = NewId()
        smenu.Append(mID, '&PageSetup')
        EVT_MENU(self, mID, self.OnTestPageSetupDlg)

        mID = NewId()
        smenu.Append(mID, '&Print')
        EVT_MENU(self, mID, self.OnTestPrintDlg)

        mID = NewId()
        smenu.Append(mID, '&Message')
        EVT_MENU(self, mID, self.OnTestMessageDlg)


        menu.AppendMenu(NewId(), '&Common Dialogs', smenu)


        mID = NewId()
        menu.Append(mID, '&Notebook')
        EVT_MENU(self, mID, self.OnTestNotebook)

        mID = NewId()
        menu.Append(mID, '&Splitter Window')
        EVT_MENU(self, mID, self.OnTestSplitter)

        mID = NewId()
        menu.Append(mID, '&Custom StatusBar')
        EVT_MENU(self, mID, self.OnTestCustomStatusBar)

        mID = NewId()
        menu.Append(mID, '&ToolBar')
        EVT_MENU(self, mID, self.OnTestToolBar)

        mID = NewId()
        menu.Append(mID, 'T&ree Control')
        EVT_MENU(self, mID, self.OnTestTreeCtrl)

        mID = NewId()
        menu.Append(mID, '&List Control')
        EVT_MENU(self, mID, self.OnTestListCtrl)

        mID = NewId()
        menu.Append(mID, 'S&ash Window and Layout Algorithm')
        EVT_MENU(self, mID, self.OnTestSashWindow)

        return menu




    def WriteText(self, str):
        self.log.WriteText(str)
        if wxPlatform == '__WXMSW__':
            w, h = self.log.GetClientSizeTuple()
            numLines = h/self.charHeight
            x, y = self.log.PositionToXY(self.log.GetLastPosition())
            self.log.ShowPosition(self.log.XYToPosition(x, y-numLines+1))

    def OnFileExit(self, event):
        self.Close()

    def OnCloseWindow(self, event):
        self.Destroy()




    def OnTestSimpleControls(self, event):
        dlg = TestSimpleControlsDlg(self, self)
        dlg.Centre()
        dlg.ShowModal()
        dlg.Destroy()

    def OnTestTimer(self, event):
        if self.timer:
            self.mainmenu.Check(self.timerID, false)
            self.timer.Stop()
            self.timer = None
        else:
            self.mainmenu.Check(self.timerID, true)
            self.timer = TestTimer(self)
            self.timer.Start(1000)

    def OnTestLayoutConstraints(self, event):
        win = TestLayoutConstraints(self)
        win.Show(true)

    def OnTestGrid(self, event):
        win = TestGrid(self, self)
        win.Show(true)
        win.SetSize(wxSize(505, 300))  # have to force a resize, or the grid doesn't
                                       # show up for some reason....

    def OnTestColourDlg(self, event):
        data = wxColourData()
        data.SetChooseFull(true)
        dlg = wxColourDialog(self, data)
        if dlg.ShowModal() == wxID_OK:
            data = dlg.GetColourData()
            self.log.WriteText('You selected: %s\n' % str(data.GetColour().Get()))
        dlg.Destroy()

    def OnTestDirDlg(self, event):
        dlg = wxDirDialog(self)
        if dlg.ShowModal() == wxID_OK:
            self.log.WriteText('You selected: %s\n' % dlg.GetPath())
        dlg.Destroy()

    def OnTestFileDlg(self, event):
        dlg = wxFileDialog(self, "Choose a file", ".", "", "*.*", wxOPEN)
        if dlg.ShowModal() == wxID_OK:
            self.log.WriteText('You selected: %s\n' % dlg.GetPath())
        dlg.Destroy()

    def OnTestSingleChoiceDlg(self, event):
        dlg = wxSingleChoiceDialog(self, 'Test Single Choice', 'The Caption',
                                   ['zero', 'one', 'two', 'three', 'four', 'five',
                                    'six', 'seven', 'eight'])
        if dlg.ShowModal() == wxID_OK:
            self.log.WriteText('You selected: %s\n' % dlg.GetStringSelection())
        dlg.Destroy()

    def OnTestTextEntryDlg(self, event):
        dlg = wxTextEntryDialog(self, 'What is your favorite programming language?',
                                'Duh??', 'Python')
        #dlg.SetValue("Python is the best!")  #### this doesn't work?
        if dlg.ShowModal() == wxID_OK:
            self.log.WriteText('You entered: %s\n' % dlg.GetValue())
        dlg.Destroy()


    def OnTestFontDlg(self, event):
        dlg = wxFontDialog(self)
        if dlg.ShowModal() == wxID_OK:
            data = dlg.GetFontData()
            font = data.GetChosenFont()
            self.log.WriteText('You selected: "%s", %d points, color %s\n' %
                               (font.GetFaceName(), font.GetPointSize(),
                                data.GetColour().Get()))
        dlg.Destroy()


    def OnTestPageSetupDlg(self, event):
        data = wxPageSetupDialogData()
        data.SetMarginTopLeft(wxPoint(50,50))
        data.SetMarginBottomRight(wxPoint(50,50))
        dlg = wxPageSetupDialog(self, data)
        if dlg.ShowModal() == wxID_OK:
            data = dlg.GetPageSetupData()
            tl = data.GetMarginTopLeft()
            br = data.GetMarginBottomRight()
            self.log.WriteText('Margins are: %s %s\n' % (str(tl), str(br)))
        dlg.Destroy()

    def OnTestPrintDlg(self, event):
        data = wxPrintDialogData()
        data.EnablePrintToFile(true)
        data.EnablePageNumbers(true)
        data.EnableSelection(true)
        dlg = wxPrintDialog(self, data)
        if dlg.ShowModal() == wxID_OK:
            self.log.WriteText('\n')
        dlg.Destroy()

    def OnTestMessageDlg(self, event):
        dlg = wxMessageDialog(self, 'Hello from Python and wxWindows!',
                              'A Message Box', wxOK | wxICON_INFORMATION)
        dlg.ShowModal()
        dlg.Destroy()


    def OnTestNotebook(self, event):
        win = TestNotebookWindow(self, self)
        win.Show(true)

    def OnTestSplitter(self, event):
        win = TestSplitterWindow(self)
        win.Show(true)

    def OnTestCustomStatusBar(self, event):
        win = TestCustomStatusBar(self)
        win.Show(true)

    def OnTestToolBar(self, event):
        win = TestToolBar(self, self)
        win.Show(true)

    def OnTestTreeCtrl(self, event):
        win = TestTreeCtrl(self, self)
        win.Show(true)

    def OnTestListCtrl(self, event):
        win = TestListCtrl(self, self)
        win.Show(true)

    def OnTestSashWindow(self, event):
        win = TestSashWindow(self, self)
        win.Show(true)

#---------------------------------------------------------------------------


class MyApp(wxApp):
    def OnInit(self):
        frame = AppFrame(NULL, -1, "Test 4: (lots of little tests...)")
        frame.Show(true)
        self.SetTopWindow(frame)
        return true

#---------------------------------------------------------------------------


def main():
    app = MyApp(0)
    app.MainLoop()


def t():
    import pdb
    pdb.run('main()')


# for focused testing...
def main2():
    class T2App(wxApp):
        def OnInit(self):
            frame = TestLayoutConstraints(NULL)
            frame.Show(true)
            self.SetTopWindow(frame)
            return true

    app = T2App(0)
    app.MainLoop()

def t2():
    import pdb
    pdb.run('main2()')



if __name__ == '__main__':
    main()


#----------------------------------------------------------------------------
#
# $Log$
# Revision 1.1.2.2  2001/01/30 20:54:16  robind
# Gobs of changes move from the main trunk to the 2.2 branch in
# preparataion for 2.2.5 release.  See CHANGES.txt for details.
#
# Revision 1.3  2000/10/30 21:05:22  robind
#
# Merged wxPython 2.2.2 over to the main branch
#
# Revision 1.1.2.1  2000/05/16 02:07:02  RD
#
# Moved and reorganized wxPython directories
#
# Now builds into an intermediate wxPython package directory before
# installing
#
# Revision 1.17  1999/08/05 05:06:50  RD
#
# Some minor tweaks
#
# Revision 1.16  1999/04/30 03:29:54  RD
#
# wxPython 2.0b9, first phase (win32)
# Added gobs of stuff, see wxPython/README.txt for details
#
# Revision 1.15.2.1  1999/03/16 06:05:50  RD
#
# wxPython 2.0b7
#
# Revision 1.15  1999/03/05 07:23:42  RD
#
# Minor wxPython changes for wxWin 2.0
#
# Revision 1.14  1999/02/27 04:20:50  RD
#
# minor tweaks for testing
#
# Revision 1.13  1999/02/20 09:04:44  RD
# Added wxWindow_FromHWND(hWnd) for wxMSW to construct a wxWindow from a
# window handle.  If you can get the window handle into the python code,
# it should just work...  More news on this later.
#
# Added wxImageList, wxToolTip.
#
# Re-enabled wxConfig.DeleteAll() since it is reportedly fixed for the
# wxRegConfig class.
#
# As usual, some bug fixes, tweaks, etc.
#
# Revision 1.12  1999/01/30 07:31:33  RD
#
# Added wxSashWindow, wxSashEvent, wxLayoutAlgorithm, etc.
#
# Various cleanup, tweaks, minor additions, etc. to maintain
# compatibility with the current wxWindows.
#
# Revision 1.11  1999/01/29 16:17:59  HH
# In test4's toolbar sample, changed NULL to wxNullBitmap to prevent SIGSEVS
# with wxGTK. The sample works now.
#
# Revision 1.10  1998/12/16 22:12:47  RD
#
# Tweaks needed to be able to build wxPython with wxGTK.
#
# Revision 1.9  1998/12/15 20:44:35  RD
# Changed the import semantics from "from wxPython import *" to "from
# wxPython.wx import *"  This is for people who are worried about
# namespace pollution, they can use "from wxPython import wx" and then
# prefix all the wxPython identifiers with "wx."
#
# Added wxTaskbarIcon for wxMSW.
#
# Made the events work for wxGrid.
#
# Added wxConfig.
#
# Added wxMiniFrame for wxGTK, (untested.)
#
# Changed many of the args and return values that were pointers to gdi
# objects to references to reflect changes in the wxWindows API.
#
# Other assorted fixes and additions.
#
# Revision 1.8  1998/11/25 08:47:11  RD
#
# Added wxPalette, wxRegion, wxRegionIterator, wxTaskbarIcon
# Added events for wxGrid
# Other various fixes and additions
#
# Revision 1.7  1998/11/11 03:13:19  RD
#
# Additions for wxTreeCtrl
#
# Revision 1.6  1998/10/20 06:45:33  RD
# New wxTreeCtrl wrappers (untested)
# some changes in helpers
# etc.
#
# Revision 1.5  1998/10/02 06:42:28  RD
#
# Version 0.4 of wxPython for MSW.
#
# Revision 1.4  1998/08/27 21:59:51  RD
# Some chicken-and-egg problems solved for wxPython on wxGTK
#
# Revision 1.3  1998/08/27 00:01:17  RD
# - more tweaks
# - have discovered some problems but not yet discovered solutions...
#
# Revision 1.2  1998/08/22 19:51:18  RD
# some tweaks for wxGTK
#
# Revision 1.1  1998/08/09 08:28:05  RD
# Initial version
#
#
