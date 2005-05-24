#----------------------------------------------------------------------------
# Name:         Calendar.py
# Purpose:      Calendar control display testing on panel for wxPython demo
#
# Author:       Lorne White (email: lwhite1@planet.eon.net)
#
# Version       0.9
# Date:         Feb 26, 2001
# Licence:      wxWindows license
#----------------------------------------------------------------------------
# 11/15/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
#
# 11/26/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Ugh. AFter updating to the Bind() method, things lock up
#   on various control clicks. Will have to debug. Only seems
#   to happen on windows with calendar controls, though. 
#
# 11/30/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Lockup issue clarification: it appears that the spinner is
#   the culprit.
#

import  os

import  wx
import  wx.lib.calendar 

import  images


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
        name = wx.lib.calendar.Month[i]

        if name != None:
            monthlist.append(name)

    return monthlist

class TestPanel(wx.Panel):
    def __init__(self, parent, log, frame):
        wx.Panel.__init__(self, parent, -1)

        self.log = log
        self.frame = frame

        self.calend = wx.lib.calendar.Calendar(self, -1, (100, 50), (200, 180))

#        start_month = 2        # preselect the date for calendar
#        start_year = 2001

        start_month = self.calend.GetMonth()        # get the current month & year
        start_year = self.calend.GetYear()

        # month list from DateTime module

        monthlist = GetMonthList()

        self.date = wx.ComboBox(self, -1, "",
                               (100, 20), (90, -1),
                               monthlist, wx.CB_DROPDOWN)

        self.date.SetSelection(start_month-1)
        self.Bind(wx.EVT_COMBOBOX, self.EvtComboBox, self.date)

        # set start month and year

        self.calend.SetMonth(start_month)
        self.calend.SetYear(start_year)

        # set attributes of calendar

        self.calend.hide_title = True
        self.calend.HideGrid()
        self.calend.SetWeekColor('WHITE', 'BLACK')

        # display routine

        self.ResetDisplay()

        # mouse click event
        self.Bind(wx.lib.calendar.EVT_CALENDAR, self.MouseClick, self.calend)

        # scroll bar for month selection
        self.scroll = wx.ScrollBar(self, -1, (100, 240), (200, 20), wx.SB_HORIZONTAL)
        self.scroll.SetScrollbar(start_month-1, 1, 12, 1, True)
        self.Bind(wx.EVT_COMMAND_SCROLL, self.Scroll, self.scroll)

        # spin control for year selection

        self.dtext = wx.TextCtrl(self, -1, str(start_year), (200, 20), (60, -1))
        h = self.dtext.GetSize().height

        self.spin = wx.SpinButton(self, -1, (270, 20), (h*2, h))
        self.spin.SetRange(1980, 2010)
        self.spin.SetValue(start_year)
        self.Bind(wx.EVT_SPIN, self.OnSpin, self.spin)

        # button for calendar dialog test
        self.but1 = wx.Button(self, -1, "Test Calendar Dialog", (380, 80))
        self.Bind(wx.EVT_BUTTON, self.TestDlg, self.but1)

        # button for calendar window test
        self.but2 = wx.Button(self, -1, "Test Calendar Window", (380, 180))
        self.Bind(wx.EVT_BUTTON, self.TestFrame, self.but2)

        self.but3 = wx.Button(self, -1, "Test Calendar Print", (380, 280))
        self.Bind(wx.EVT_BUTTON, self.OnPreview, self.but3)

        # calendar dialog

    def TestDlg(self, event):       # test the date dialog
        dlg = wx.lib.calendar.CalenDlg(self)
        dlg.Centre()

        if dlg.ShowModal() == wx.ID_OK:
            result = dlg.result
            day = result[1]
            month = result[2]
            year = result[3]
            new_date = str(month) + '/'+ str(day) + '/'+ str(year)
            self.log.WriteText('Date Selected: %s\n' % new_date)
        else:
            self.log.WriteText('No Date Selected')

        # calendar window test

    def TestFrame(self, event):
        frame = CalendFrame(self, -1, "Test Calendar", self.log)
        frame.Show(True)
        return True

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
        self.scroll.SetScrollbar(monthval, 1, 12, 1, True)

        self.calend.SetMonth(monthval+1)
        self.ResetDisplay()

    def Scroll(self, event):
        value = self.scroll.GetThumbPosition()
        monthval = int(value)+1
        self.calend.SetMonth(monthval)
        self.ResetDisplay()
        self.log.WriteText('Month: %s\n' % value)

        name = wx.lib.calendar.Month[monthval]
        self.date.SetValue(name)

    # log mouse events

    def MouseClick(self, evt):
        text = '%s CLICK   %02d/%02d/%d' % (evt.click, evt.day, evt.month, evt.year)  # format date
        self.log.WriteText('Date Selected: ' + text + '\n')


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

# test of full window calendar control functions

class CalendFrame(wx.Frame):
    def __init__(self, parent, id, title, log):
        wx.Frame.__init__(self, parent, id, title, size=(400, 400),
                         style=wx.DEFAULT_FRAME_STYLE|wx.NO_FULL_REPAINT_ON_RESIZE)

        self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)

        self.log = log
        self.CreateStatusBar()
        self.mainmenu = wx.MenuBar()
        menu = wx.Menu()

        menu = self.MakeFileMenu()
        self.mainmenu.Append(menu, '&File')

        self.MakeToolMenu()             # toolbar

        self.SetMenuBar(self.mainmenu)
        self.calend = wx.lib.calendar.Calendar(self, -1)
        self.calend.SetCurrentDay()
        self.calend.grid_color = 'BLUE'
        self.calend.SetBusType()
#        self.calend.ShowWeekEnd()

        self.ResetDisplay()

        self.Bind(wx.lib.calendar.EVT_CALENDAR, self.MouseClick, self.calend)

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
        menu = wx.Menu()

        mID = wx.NewId()
        menu.Append(mID, 'Decrement', 'Next')
        self.Bind(wx.EVT_MENU, self.OnDecMonth, id=mID)

        mID = wx.NewId()
        menu.Append(mID, 'Increment', 'Dec')
        self.Bind(wx.EVT_MENU, self.OnIncMonth, id=mID)

        menu.AppendSeparator()

        mID = wx.NewId()
        menu.Append(mID, 'E&xit', 'Exit')
        self.Bind(wx.EVT_MENU, self.OnCloseWindow, id=mID)

        return menu

    def MakeToolMenu(self):
        tb = self.CreateToolBar(wx.TB_HORIZONTAL|wx.NO_BORDER)

        mID = wx.NewId()
        SetToolPath(self, tb, mID, images.getDbDecBitmap(), 'Dec Year')
        self.Bind(wx.EVT_TOOL, self.OnDecYear, id=mID)

        mID = wx.NewId()
        SetToolPath(self, tb, mID, images.getDecBitmap(), 'Dec Month')
        self.Bind(wx.EVT_TOOL, self.OnDecMonth, id=mID)

        mID = wx.NewId()
        SetToolPath(self, tb, mID, images.getPtBitmap(), 'Current Month')
        self.Bind(wx.EVT_TOOL, self.OnCurrent, id=mID)

        mID = wx.NewId()
        SetToolPath(self, tb, mID, images.getIncBitmap(), 'Inc Month')
        self.Bind(wx.EVT_TOOL, self.OnIncMonth, id=mID)

        mID = wx.NewId()
        SetToolPath(self, tb, mID, images.getDbIncBitmap(), 'Inc Year')
        self.Bind(wx.EVT_TOOL, self.OnIncYear, id=mID)

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
        self.printData = wx.PrintData()

    def SetCal(self):
        self.grid_color = 'BLUE'
        self.back_color = 'WHITE'
        self.sel_color = 'RED'
        self.high_color = 'LIGHT BLUE'
        self.font = wx.SWISS
        self.bold = wx.NORMAL

        self.sel_key = None      # last used by
        self.sel_lst = []        # highlighted selected days

        self.size = None
        self.hide_title = False
        self.hide_grid = False
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
        self.preview = wx.PrintPreview(printout, printout2, self.printData)

        if not self.preview.Ok():
            wx.MessageBox("There was a problem printing!", "Printing", wx.OK)
            return

        self.preview.SetZoom(60)        # initial zoom value

        frame = wx.PreviewFrame(self.preview, self.frame, "Print preview")

        frame.Initialize()
        frame.SetPosition(self.frame.GetPosition())
        frame.SetSize(self.frame.GetSize())
        frame.Show(True)

    def Print(self):
        pdd = wx.PrintDialogData()
        pdd.SetPrintData(self.printData)
        printer = wx.Printer(pdd)
        printout = SetPrintout(self)
        frame = wx.Frame(None, -1, "Test")

        if not printer.Print(frame, printout):
            wx.MessageBox("There was a problem printing.\nPerhaps your current printer is not set correctly?", "Printing", wx.OK)
        else:
            self.printData = printer.GetPrintDialogData().GetPrintData()

        printout.Destroy()

    def DoDrawing(self, DC):
        size = DC.GetSize()
        DC.BeginDrawing()

        cal = wx.lib.calendar.PrtCalDraw(self)

        if self.preview is None:
            cal.SetPSize(size[0]/self.pagew, size[1]/self.pageh)
            cal.SetPreview(False)

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
        cal.outer_border = False
        cal.font = self.font
        cal.bold = self.bold

        cal_size = (3.0, 3.0)
        cal.SetSize(cal_size)

        year, month = self.year, self.month

        x = 0.5
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

            x = x + 4.0     # next column

        DC.EndDrawing()

        self.ymax = DC.MaxY()
        self.xmax = DC.MaxX()

    def IncMonth(self, year, month):     # next month
        month = month + 1

        if month > 12:
            month = 1
            year = year + 1

        return year, month

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
    tb.AddSimpleTool(id, bmp, title, title)

class SetPrintout(wx.Printout):
    def __init__(self, canvas):
        wx.Printout.__init__(self)
        self.canvas = canvas
        self.end_pg = 1

    def OnBeginDocument(self, start, end):
        return self.base_OnBeginDocument(start, end)

    def OnEndDocument(self):
        self.base_OnEndDocument()

    def HasPage(self, page):
        if page <= self.end_pg:
            return True
        else:
            return False

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

        (w, h) = dc.GetSize()
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
        (w, h) = dc.GetSize()
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
        return True

class MyApp(wx.App):
    def OnInit(self):
        frame = CalendFrame(None, -1, "Test Calendar", log)
        frame.Show(True)
        self.SetTopWindow(frame)
        return True

#---------------------------------------------------------------------------

def MessageDlg(self, message, type = 'Message'):
    dlg = wx.MessageDialog(self, message, type, wx.OK | wx.ICON_INFORMATION)
    dlg.ShowModal()
    dlg.Destroy()

#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log, frame)
    return win

#---------------------------------------------------------------------------


overview = """\
This control provides a Calendar control class for displaying and selecting dates.  
In addition, the class is extended and can be used for printing/previewing.

Additional features include weekend highlighting and business type Monday-Sunday 
format.

See example for various methods used to set display month, year, and highlighted 
dates (different font and background colours).

by Lorne White

"""




if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

