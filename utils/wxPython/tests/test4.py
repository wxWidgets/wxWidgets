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


from wxPython import *

import time

#---------------------------------------------------------------------------

class TestSimpleControlsDlg(wxDialog):
    def __init__(self, parent, log):
        self.log = log
        wxDialog.__init__(self, parent, -1, "Test Simple Controls",
                          wxPyDefaultPosition, wxSize(350, 350))


        sampleList = ['zero', 'one', 'two', 'three', 'four', 'five',
                      'six', 'seven', 'eight']

        y_pos = 5
        delta = 25

        wxStaticText(self, -1, "wxTextCtrl", wxPoint(5, y_pos), wxSize(75, 20))
        wxTextCtrl(self, 10, "", wxPoint(80, y_pos), wxSize(150, 20))
        EVT_TEXT(self, 10, self.EvtText)
        y_pos = y_pos + delta

        wxCheckBox(self, 20, "wxCheckBox", wxPoint(80, y_pos), wxSize(150, 20), wxNO_BORDER)
        EVT_CHECKBOX(self, 20, self.EvtCheckBox)
        y_pos = y_pos + delta

        rb = wxRadioBox(self, 30, "wxRadioBox", wxPoint(80, y_pos), wxPyDefaultSize,
                        sampleList, 3, wxRA_HORIZONTAL| wxNO_BORDER)
        EVT_RADIOBOX(self, 30, self.EvtRadioBox)
        width, height = rb.GetSize()
        y_pos = y_pos + height + 5

        wxStaticText(self, -1, "wxChoice", wxPoint(5, y_pos), wxSize(75, 20))
        wxChoice(self, 40, wxPoint(80, y_pos), wxSize(95, 20), #wxPyDefaultSize,
                 sampleList)
        EVT_CHOICE(self, 40, self.EvtChoice)
        y_pos = y_pos + delta

        wxStaticText(self, -1, "wxComboBox", wxPoint(5, y_pos), wxSize(75, 18))
        wxComboBox(self, 50, "default value", wxPoint(80, y_pos), wxSize(95, 20),
                   sampleList, wxCB_DROPDOWN)
        EVT_COMBOBOX(self, 50, self.EvtComboBox)
        y_pos = y_pos + delta

        wxStaticText(self, -1, "wxListBox", wxPoint(5, y_pos), wxSize(75, 18))
        lb = wxListBox(self, 60, wxPoint(80, y_pos), wxPyDefaultSize,
                       sampleList, wxLB_SINGLE)
        EVT_LISTBOX(self, 60, self.EvtListBox)
        EVT_LISTBOX_DCLICK(self, 60, self.EvtListBoxDClick)
        lb.SetSelection(0)
        width, height = lb.GetSize()
        y_pos = y_pos + height + 5



        y_pos = y_pos + 15
        wxButton(self, wxID_OK, ' OK ', wxPoint(80, y_pos), wxPyDefaultSize).SetDefault()
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
                         wxPyDefaultPosition, wxSize(500, 300))

        self.SetAutoLayout(true)
        EVT_BUTTON(self, 100, self.OnButton)

        self.panelA = wxWindow(self, -1, wxPyDefaultPosition, wxPyDefaultSize,
                               wxSIMPLE_BORDER)
        self.panelA.SetBackgroundColour(wxBLUE)
        lc = wxLayoutConstraints()
        lc.top.SameAs(self, wxTop, 10)
        lc.left.SameAs(self, wxLeft, 10)
        lc.bottom.SameAs(self, wxBottom, 10)
        lc.right.PercentOf(self, wxRight, 50)
        self.panelA.SetConstraints(lc)

        self.panelB = wxWindow(self, -1, wxPyDefaultPosition, wxPyDefaultSize,
                               wxSIMPLE_BORDER)
        self.panelB.SetBackgroundColour(wxRED)
        lc = wxLayoutConstraints()
        lc.top.SameAs(self, wxTop, 10)
        lc.right.SameAs(self, wxRight, 10)
        lc.bottom.PercentOf(self, wxBottom, 30)
        lc.left.RightOf(self.panelA, 10)
        self.panelB.SetConstraints(lc)

        self.panelC = wxWindow(self, -1, wxPyDefaultPosition, wxPyDefaultSize,
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

        self.panelD = wxWindow(self.panelC, -1, wxPyDefaultPosition, wxPyDefaultSize,
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
    def __init__(self, parent):
        wxFrame.__init__(self, parent, -1, 'Test Grid',
                         wxPyDefaultPosition, wxSize(500, 300))

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


    def OnCloseWindow(self, event):
        self.Destroy()


#---------------------------------------------------------------------------


class ColoredPanel(wxWindow):
    def __init__(self, parent, color):
        wxWindow.__init__(self, parent, -1,
                          wxPyDefaultPosition, wxPyDefaultSize, wxRAISED_BORDER)
        self.SetBackgroundColour(color)


class TestNotebookWindow(wxFrame):
    def __init__(self, parent):
        wxFrame.__init__(self, parent, -1, 'Test wxNotebook',
                         wxPyDefaultPosition, wxPyDefaultSize)

        nb = wxNotebook(self, -1)

        win = ColoredPanel(nb, wxBLUE)
        nb.AddPage(win, "Blue")
        st = wxStaticText(win, -1, "You can put any type of window here!", wxPoint(10, 10))
        st.SetForegroundColour(wxWHITE)
        st.SetBackgroundColour(wxBLUE)

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
        self.SetSize(wxSize(300, 300))  # force a redraw so the notebook will draw


    def OnCloseWindow(self, event):
        self.Destroy()

#---------------------------------------------------------------------------

class TestSplitterWindow(wxFrame):
    def __init__(self, parent):
        wxFrame.__init__(self, parent, -1, 'Test wxSplitterWindow',
                         wxPyDefaultPosition, wxSize(500, 300))

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
        (w,h, d,e) = dc.GetTextExtent('X')
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
        self.cb.SetPosition(wxPoint(rect.x+2, rect.y+2))
        self.cb.SetSize(wxSize(rect.width-4, rect.height-4))



class TestCustomStatusBar(wxFrame):
    def __init__(self, parent):
        wxFrame.__init__(self, parent, -1, 'Test Custom StatusBar',
                         wxPyDefaultPosition, wxSize(500, 300))
        wxWindow(self, -1)

        self.sb = CustomStatusBar(self)
        self.SetStatusBar(self.sb)

    def OnCloseWindow(self, event):
        self.sb.timer.Stop()
        self.Destroy()


#---------------------------------------------------------------------------

class TestToolBar(wxFrame):
    def __init__(self, parent, log):
        wxFrame.__init__(self, parent, -1, 'Test ToolBar',
                         wxPyDefaultPosition, wxSize(500, 300))
        self.log = log

        wxWindow(self, -1)

        tb = self.CreateToolBar()
        #tb = wxToolBar(self, -1, wxPyDefaultPosition, wxPyDefaultSize,
        #               wxTB_HORIZONTAL | wxNO_BORDER | wxTB_FLAT)
        #self.SetToolBar(tb)

        tb.AddTool(10, wxNoRefBitmap('bitmaps/new.bmp',   wxBITMAP_TYPE_BMP),
                        NULL, false, -1, -1, "New")
        EVT_TOOL(self, 10, self.OnToolClick)
        EVT_TOOL_RCLICKED(self, 10, self.OnToolRClick)

        tb.AddTool(20, wxNoRefBitmap('bitmaps/open.bmp',  wxBITMAP_TYPE_BMP),
                        NULL, false, -1, -1, "Open")
        EVT_TOOL(self, 20, self.OnToolClick)
        EVT_TOOL_RCLICKED(self, 20, self.OnToolRClick)

        tb.AddSeparator()
        tb.AddTool(30, wxNoRefBitmap('bitmaps/copy.bmp',  wxBITMAP_TYPE_BMP),
                        NULL, false, -1, -1, "Copy")
        EVT_TOOL(self, 30, self.OnToolClick)
        EVT_TOOL_RCLICKED(self, 30, self.OnToolRClick)

        tb.AddTool(40, wxNoRefBitmap('bitmaps/paste.bmp', wxBITMAP_TYPE_BMP),
                        NULL, false, -1, -1, "Paste")
        EVT_TOOL(self, 40, self.OnToolClick)
        EVT_TOOL_RCLICKED(self, 40, self.OnToolRClick)

        tb.AddSeparator()

        tb.AddTool(50, wxNoRefBitmap('bitmaps/tog1.bmp', wxBITMAP_TYPE_BMP),
                        NULL, true, -1, -1, "Toggle this")
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
#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

class AppFrame(wxFrame):
    def __init__(self, parent, id, title):
        wxFrame.__init__(self, parent, id, title, wxPyDefaultPosition,
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

        self.log = wxTextCtrl(self, -1, '', wxPyDefaultPosition, wxPyDefaultSize,
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

        return menu




    def WriteText(self, str):
        self.log.WriteText(str)
        if wxPlatform == '__WXMSW__':
            w, h = self.log.GetClientSize()
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
        win = TestGrid(self)
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
        data = wxPageSetupData()
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
        data = wxPrintData()
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
        win = TestNotebookWindow(self)
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
