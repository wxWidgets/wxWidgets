#----------------------------------------------------------------------------
# Name:         wxCalendar.py
# Purpose:      Calendar control display testing on panel for wxPython demo
#
# Author:       Lorne White (email: lwhite1@planet.eon.net)
#
# Created:
# Version       0.8 2000/04/16
# Licence:      wxWindows license
#----------------------------------------------------------------------------

from wxPython.wx           import *
from wxPython.lib.calendar import wxCalendar, Month, PrtCalDraw

import os
dir_path = os.getcwd()


# highlighted days in month

test_days ={ 0: [],
            1: [3, 7, 9, 21],
            2: [2, 10, 4, 9],
            3: [4, 20, 29],
            4: [1, 12, 22],
            5: [2, 10, 15],
            6: [4, 8, 17],
            7: [6, 7, 8],
            8: [5, 10, 20],
            9: [1, 2, 5, 29],
           10: [2, 4, 6, 22],
           11: [6, 9, 12, 28, 29],
           12: [8, 9, 10, 11, 20] }

# test of full window calendar control functions

def GetMonthList():
    monthlist = []
    for i in range(13):
        name = Month[i]
        if name != None:
            monthlist.append(name)
    return monthlist

class TestPanel(wxPanel):
    def __init__(self, parent, log, frame):
        wxPanel.__init__(self, parent, -1)

        self.log = log
        self.frame = frame

        self.calend = wxCalendar(self, -1, wxPoint(100, 50), wxSize(200, 180))

        start_month = 11
        start_year = 1999

# month list from DateTime module

        monthlist = GetMonthList()

        self.date = wxComboBox(self, 10, Month[start_month], wxPoint(100, 20), wxSize(90, -1), monthlist, wxCB_DROPDOWN)
        EVT_COMBOBOX(self, 10, self.EvtComboBox)

# set start month and year

        self.calend.SetMonth(start_month)
        self.calend.SetYear(start_year)

# set attributes of calendar

        self.calend.hide_title = TRUE
        self.calend.HideGrid()
        self.calend.SetWeekColor('WHITE', 'BLACK')

# display routine

        self.ResetDisplay()

# mouse click event

        self.Connect(self.calend.GetId(), -1, 2100, self.MouseClick)

# scroll bar for month selection

        self.scroll = wxScrollBar(self, 40, wxPoint(100, 240), wxSize(200, 20), wxSB_HORIZONTAL)
        self.scroll.SetScrollbar(start_month-1, 1, 12, 1, TRUE)
        EVT_COMMAND_SCROLL(self, 40, self.Scroll)

# spin control for year selection

        self.dtext = wxTextCtrl(self, -1, str(start_year), wxPoint(200, 20), wxSize(60, -1))
        h = self.dtext.GetSize().height

        self.spin = wxSpinButton(self, 20, wxPoint(270, 20), wxSize(h*2, h))
        self.spin.SetRange(1980, 2010)
        self.spin.SetValue(start_year)
        EVT_SPIN(self, 20, self.OnSpin)

# button for calendar dialog test

        wxStaticText(self, -1, "Test Calendar Dialog", wxPoint(350, 50), wxSize(150, -1))

        bmp = wxBitmap('bitmaps/Calend.bmp', wxBITMAP_TYPE_BMP)
        self.but = wxBitmapButton(self, 60, bmp, wxPoint(380, 80))#, wxSize(30, 30))
        EVT_BUTTON(self, 60, self.TestDlg)

# button for calendar window test

        wxStaticText(self, -1, "Test Calendar Window", wxPoint(350, 150), wxSize(150, -1))

        bmp = wxBitmap('bitmaps/Calend.bmp', wxBITMAP_TYPE_BMP)
        self.but = wxBitmapButton(self, 160, bmp, wxPoint(380, 180))#, wxSize(30, 30))
        EVT_BUTTON(self, 160, self.TestFrame)

        wxStaticText(self, -1, "Test Calendar Print", wxPoint(350, 250), wxSize(150, -1))

        bmp = wxBitmap('bitmaps/Calend.bmp', wxBITMAP_TYPE_BMP)
        self.but = wxBitmapButton(self, 170, bmp, wxPoint(380, 280))#, wxSize(30, 30))
        EVT_BUTTON(self, 170, self.OnPreview)

# calendar dialog

    def TestDlg(self, event):
        dlg = CalenDlg(self, self.log)
        dlg.Centre()
        dlg.ShowModal()
        dlg.Destroy()

# calendar window test

    def TestFrame(self, event):
        frame = CalendFrame(self, -1, "Test Calendar", self.log)
        frame.Show(true)
        return true

# calendar print preview

    def OnPreview(self, event):
        month = self.calend.GetMonth()
        year = self.calend.GetYear()

        prt = PrintCalend(self.frame, month, year)
        prt.Preview()

# month and year control events

    def OnSpin(self, event):
        year = event.GetPosition()
        self.dtext.SetValue(str(year))
        self.calend.SetYear(year)
        self.calend.Refresh()

    def EvtComboBox(self, event):
        name = event.GetString()
        self.log.WriteText('EvtComboBox: %s\n' % name)
        monthval = self.date.FindString(name)
        self.scroll.SetScrollbar(monthval, 1, 12, 1, TRUE)

        self.calend.SetMonth(monthval+1)
        self.ResetDisplay()

    def Scroll(self, event):
        value = self.scroll.GetThumbPosition()
        monthval = int(value)+1
        self.calend.SetMonth(monthval)
        self.ResetDisplay()
        self.log.WriteText('Month: %s\n' % value)

        name = Month[monthval]
        self.date.SetValue(name)

# log mouse events

    def MouseClick(self, evt):
        text = '%s CLICK   %02d/%02d/%d' % (evt.click, evt.day, evt.month, evt.year)  # format date
        self.log.WriteText('Date Selected: ' + text + '\n')

    def OnCloseWindow(self, event):
        self.Destroy()

# set the highlighted days for the calendar

    def ResetDisplay(self):
        month = self.calend.GetMonth()
        try:
            set_days = test_days[month]
        except:
            set_days = [1, 5, 12]

        self.calend.AddSelect([4, 11], 'BLUE', 'WHITE')
        self.calend.SetSelDay(set_days)
        self.calend.Refresh()

# increment and decrement toolbar controls

    def OnIncYear(self, event):
        self.calend.IncYear()
        self.ResetDisplay()

    def OnDecYear(self, event):
        self.calend.DecYear()
        self.ResetDisplay()

    def OnIncMonth(self, event):
        self.calend.IncMonth()
        self.ResetDisplay()

    def OnDecMonth(self, event):
        self.calend.DecMonth()
        self.ResetDisplay()

    def OnCurrent(self, event):
        self.calend.SetCurrentDay()
        self.ResetDisplay()

# test the calendar control in a dialog

class CalenDlg(wxDialog):
    def __init__(self, parent, log):
        self.log = log
        wxDialog.__init__(self, parent, -1, "Test Calendar", wxPyDefaultPosition, wxSize(280, 300))

        start_month = 2
        start_year = 1999

# get month list from DateTime

        monthlist = GetMonthList()

# select the month

        self.date = wxComboBox(self, 100, Month[start_month], wxPoint(20, 20), wxSize(90, -1), monthlist, wxCB_DROPDOWN)
        EVT_COMBOBOX(self, 100, self.EvtComboBox)

# alternate spin button to control the month

        h = self.date.GetSize().height
        self.m_spin = wxSpinButton(self, 120, wxPoint(120, 20), wxSize(h*2, h), wxSP_VERTICAL)
        self.m_spin.SetRange(1, 12)
        self.m_spin.SetValue(start_month)

        EVT_SPIN(self, 120, self.OnMonthSpin)

# spin button to conrol the year

        self.dtext = wxTextCtrl(self, -1, str(start_year), wxPoint(160, 20), wxSize(60, -1))
        h = self.dtext.GetSize().height

        self.y_spin = wxSpinButton(self, 20, wxPoint(220, 20), wxSize(h*2, h), wxSP_VERTICAL)
        self.y_spin.SetRange(1980, 2010)
        self.y_spin.SetValue(start_year)

        EVT_SPIN(self, 20, self.OnYrSpin)

# set the calendar and attributes

        self.calend = wxCalendar(self, -1, wxPoint(20, 60), wxSize(240, 200))
        self.calend.SetMonth(start_month)
        self.calend.SetYear(start_year)

        self.calend.HideTitle()
        self.calend.ShowWeekEnd()

        self.ResetDisplay()

        self.Connect(self.calend.GetId(), -1, 2100, self.MouseClick)

# log the mouse clicks

    def MouseClick(self, evt):
        text = '%s CLICK   %02d/%02d/%d' % (evt.click, evt.day, evt.month, evt.year)  # format date
        self.log.WriteText('Date Selected: ' + text + '\n')

        if evt.click == 'DLEFT':
            self.EndModal(wxID_OK)

# month and year spin selection routines

    def OnMonthSpin(self, event):
        month = event.GetPosition()
        if month >= 0 and month <= 12:
            self.date.SetValue(Month[month])
            self.calend.SetMonth(month)
            self.calend.Refresh()

    def OnYrSpin(self, event):
        year = event.GetPosition()
        self.dtext.SetValue(str(year))
        self.calend.SetYear(year)
        self.calend.Refresh()

    def EvtComboBox(self, event):
        name = event.GetString()
        self.log.WriteText('EvtComboBox: %s\n' % name)
        monthval = self.date.FindString(name)
        self.m_spin.SetValue(monthval+1)

        self.calend.SetMonth(monthval+1)
        self.ResetDisplay()

# set the calendar for highlighted days

    def ResetDisplay(self):
        month = self.calend.GetMonth()
        try:
            set_days = test_days[month]
        except:
            set_days = [1, 5, 12]

        self.calend.AddSelect([4, 11], 'BLUE', 'WHITE')

        self.calend.SetSelDay(set_days)
        self.calend.Refresh()

# test of full window calendar control functions

class CalendFrame(wxFrame):
    def __init__(self, parent, id, title, log):
        wxFrame.__init__(self, parent, id, title, wxPyDefaultPosition, wxSize(400, 400))

        self.log = log
        self.CreateStatusBar()
        self.mainmenu = wxMenuBar()
        menu = wxMenu()

        menu = self.MakeFileMenu()
        self.mainmenu.Append(menu, '&File')

        self.MakeToolMenu()             # toolbar

        self.SetMenuBar(self.mainmenu)
        self.calend = wxCalendar(self, -1)
        self.calend.SetCurrentDay()
        self.calend.grid_color = 'BLUE'
        self.calend.SetBusType()
#        self.calend.ShowWeekEnd()

        self.ResetDisplay()

        self.Connect(self.calend.GetId(), -1, 2100, self.MouseClick)

    def MouseClick(self, evt):
        text = '%s CLICK   %02d/%02d/%d' % (evt.click, evt.day, evt.month, evt.year)  # format date
        self.log.WriteText('Date Selected: ' + text + '\n')

    def OnCloseWindow(self, event):
        self.Destroy()

    def ResetDisplay(self):
        month = self.calend.GetMonth()
        try:
            set_days = test_days[month]
        except:
            set_days = [1, 5, 12]

        self.calend.AddSelect([2, 16], 'GREEN', 'WHITE')

        self.calend.SetSelDay(set_days)
        self.calend.Refresh()

    def OnIncYear(self, event):
        self.calend.IncYear()
        self.ResetDisplay()

    def OnDecYear(self, event):
        self.calend.DecYear()
        self.ResetDisplay()

    def OnIncMonth(self, event):
        self.calend.IncMonth()
        self.ResetDisplay()

    def OnDecMonth(self, event):
        self.calend.DecMonth()
        self.ResetDisplay()

    def OnCurrent(self, event):
        self.calend.SetCurrentDay()
        self.ResetDisplay()

    def MakeFileMenu(self):
        menu = wxMenu()

        mID = NewId()
        menu.Append(mID, 'Decrement', 'Next')
        EVT_MENU(self, mID, self.OnDecMonth)

        mID = NewId()
        menu.Append(mID, 'Increment', 'Dec')
        EVT_MENU(self, mID, self.OnIncMonth)

        menu.AppendSeparator()

        mID = NewId()
        menu.Append(mID, 'E&xit', 'Exit')
        EVT_MENU(self, mID, self.OnCloseWindow)

        return menu

    def MakeToolMenu(self):
        tb = self.CreateToolBar(wxTB_HORIZONTAL|wxNO_BORDER)

        bmp_path = 'bitmaps/'
        SetToolPath(self, tb, 10, bmp_path + 'DbDec.bmp', 'Dec Year')
        EVT_TOOL(self, 10, self.OnDecYear)

        SetToolPath(self, tb, 15, bmp_path + 'Dec.bmp', 'Dec Month')
        EVT_TOOL(self, 15, self.OnDecMonth)

        SetToolPath(self, tb, 30, bmp_path + 'Pt.bmp', 'Current Month')
        EVT_TOOL(self, 30, self.OnCurrent)

        SetToolPath(self, tb, 40, bmp_path + 'Inc.bmp', 'Inc Month')
        EVT_TOOL(self, 40, self.OnIncMonth)

        SetToolPath(self, tb, 45, bmp_path + 'DbInc.bmp', 'Inc Year')
        EVT_TOOL(self, 45, self.OnIncYear)

        tb.Realize()

#---------------------------------------------------------------------------

# example class for printing/previewing calendars

class PrintCalend:
    def __init__(self, parent, month, year):
        self.frame = parent
        self.month = month
        self.year = year

        self.SetParms()
        self.SetCal()
        self.printData = wxPrintData()

    def SetCal(self):
        self.grid_color = 'BLUE'
        self.back_color = 'WHITE'
        self.sel_color = 'RED'
        self.high_color = 'LIGHT BLUE'
        self.font = wxSWISS
        self.bold = wxNORMAL

        self.sel_key = None      #  last used by
        self.sel_lst = []        # highlighted selected days

        self.size = None
        self.hide_title = FALSE
        self.hide_grid = FALSE
        self.set_day = None

    def SetParms(self):
        self.ymax = 1
        self.xmax = 1
        self.page = 1
        self.total_pg = 1

        self.preview = None
        self.scale = 1.0

        self.pagew = 8.5
        self.pageh = 11.0

        self.txt_marg = 0.1
        self.lf_marg = 0
        self.top_marg = 0

        self.page = 0

    def SetDates(self, month, year):
        self.month = month
        self.year = year

    def SetStyleDef(self, desc):
        self.style = desc

    def SetCopies(self, copies):        # number of copies of label
        self.copies = copies

    def SetStart(self, start):          # start position of label
        self.start = start

    def Preview(self):
        printout = SetPrintout(self)
        printout2 = SetPrintout(self)
        self.preview = wxPrintPreview(printout, printout2, self.printData)
        if not self.preview.Ok():
            wxMessageBox("There was a problem printing!", "Printing", wxOK)
            return

        self.preview.SetZoom(60)        # initial zoom value

        frame = wxPreviewFrame(self.preview, self.frame, "Print preview")

        frame.Initialize()
        frame.SetPosition(self.frame.GetPosition())
        frame.SetSize(self.frame.GetSize())
        frame.Show(true)

    def Print(self):
        pdd = wxPrintDialogData()
        pdd.SetPrintData(self.printData)
        printer = wxPrinter(pdd)
        printout = SetPrintout(self)
        frame = wxFrame(NULL, -1, "Test")
        if not printer.Print(frame, printout):
            wxMessageBox("There was a problem printing.\nPerhaps your current printer is not set correctly?", "Printing", wxOK)
        else:
            self.printData = printer.GetPrintDialogData().GetPrintData()
        printout.Destroy()

    def DoDrawing(self, DC):
        size = DC.GetSizeTuple()
        DC.BeginDrawing()

        cal = PrtCalDraw(self)

        if self.preview is None:
            cal.SetPSize(size[0]/self.pagew, size[1]/self.pageh)
            cal.SetPreview(FALSE)

        else:
            if self.preview == 1:
                cal.SetPSize(size[0]/self.pagew, size[1]/self.pageh)
            else:
                cal.SetPSize(self.pwidth, self.pheight)

            cal.SetPreview(self.preview)

        cal.hide_title = self.hide_title        # set the calendar parameters
        cal.hide_grid = self.hide_grid

        cal.grid_color = self.grid_color
        cal.high_color = self.high_color
        cal.back_color = self.back_color
        cal.outer_border = FALSE
        cal.font = self.font
        cal.bold = self.bold

        cal_size = wxSize(3.0, 3.0)
        cal.SetSize(cal_size)

        year, month = self.year, self.month

        x = 1.0
        for i in range(2):
            y = 0.5
            for j in range(3):
                cal.SetCal(year, month)       # current month
                cal.SetPos(x, y)

                try:
                    set_days = test_days[month]
                except:
                    set_days = [1, 5, 12]

                cal.AddSelect([2, 16], 'GREEN', 'WHITE')

                cal.DrawCal(DC, set_days)

                year, month = self.IncMonth(year, month)
                y = y + 3.5
            x = x + 4.0     # next colum

        DC.EndDrawing()

        self.ymax = DC.MaxY()
        self.xmax = DC.MaxX()

    def IncMonth(self, year, month):     # next month
        month = month + 1
        if month > 12:
            month = 1
            year = year + 1

        return year, month

    def OnCloseWindow(self, event):
        self.Destroy()

    def GetTotalPages(self):
        self.pg_cnt = 1
        return self.pg_cnt

    def SetPage(self, page):
        self.page = page

    def SetPageSize(self, width, height):
        self.pwidth, self.pheight = width, height

    def SetTotalSize(self, width, height):
        self.ptwidth, self.ptheight = width, height

    def SetPreview(self, preview, scale):
        self.preview = preview
        self.scale = scale

    def SetTotalSize(self, width, height):
        self.ptwidth = width
        self.ptheight = height

def SetToolPath(self, tb, id, bmp, title):
    global dir_path
    tb.AddSimpleTool(id, wxBitmap(os.path.join(dir_path, bmp), wxBITMAP_TYPE_BMP),
                     title, title)

class SetPrintout(wxPrintout):
    def __init__(self, canvas):
        wxPrintout.__init__(self)
        self.canvas = canvas
        self.end_pg = 1

    def OnBeginDocument(self, start, end):
        return self.base_OnBeginDocument(start, end)

    def OnEndDocument(self):
        self.base_OnEndDocument()

    def HasPage(self, page):
        if page <= self.end_pg:
            return true
        else:
            return false

    def GetPageInfo(self):
        self.end_pg = self.canvas.GetTotalPages()
        str_pg = 1
        try:
            end_pg = self.end_pg
        except:
            end_pg = 1
        return (str_pg, end_pg, str_pg, end_pg)

    def OnPreparePrinting(self):
        self.base_OnPreparePrinting()

    def OnBeginPrinting(self):
        dc = self.GetDC()

        self.preview = self.IsPreview()
        if (self.preview):
            self.pixelsPerInch = self.GetPPIScreen()
        else:
            self.pixelsPerInch = self.GetPPIPrinter()

        (w, h) = dc.GetSizeTuple()
        scaleX = float(w) / 1000
        scaleY = float(h) / 1000
        self.printUserScale = min(scaleX, scaleY)

        self.base_OnBeginPrinting()

    def GetSize(self):
        self.psizew, self.psizeh = self.GetPPIPrinter()
        return self.psizew, self.psizeh

    def GetTotalSize(self):
        self.ptsizew, self.ptsizeh = self.GetPageSizePixels()
        return self.ptsizew, self.ptsizeh

    def OnPrintPage(self, page):
        dc = self.GetDC()
        (w, h) = dc.GetSizeTuple()
        scaleX = float(w) / 1000
        scaleY = float(h) / 1000
        self.printUserScale = min(scaleX, scaleY)
        dc.SetUserScale(self.printUserScale, self.printUserScale)

        self.preview = self.IsPreview()

        self.canvas.SetPreview(self.preview, self.printUserScale)
        self.canvas.SetPage(page)

        self.ptsizew, self.ptsizeh = self.GetPageSizePixels()
        self.canvas.SetTotalSize(self.ptsizew, self.ptsizeh)

        self.psizew, self.psizeh = self.GetPPIPrinter()
        self.canvas.SetPageSize(self.psizew, self.psizeh)

        self.canvas.DoDrawing(dc)
        return true

class MyApp(wxApp):
    def OnInit(self):
        frame = CalendFrame(NULL, -1, "Test Calendar")
        frame.Show(true)
        self.SetTopWindow(frame)
        return true

#---------------------------------------------------------------------------

def MessageDlg(self, message, type = 'Message'):
    dlg = wxMessageDialog(self, message, type, wxOK | wxICON_INFORMATION)
    dlg.ShowModal()
    dlg.Destroy()

#---------------------------------------------------------------------------

def main():
    app = MyApp(0)
    app.MainLoop()


if __name__ == '__main__':
    main()


#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log, frame)
    return win

#---------------------------------------------------------------------------


overview = """\
This control provides a calendar control class for displaying and selecting dates.  In addition, the class is extended and can now be used for printing/previewing.
Additional features include weekend highlighting and business type Monday-Sunday format.

See example for various methods used to set display month, year, and highlighted dates (different font and background colours).

by Lorne White

"""
