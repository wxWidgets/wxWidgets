#! /usr/local/bin/python
#----------------------------------------------------------------------------
# Name:         CalendPanel.py
# Purpose:      Calendar control display testing on panel
#
# Author:       Lorne White (email: lwhite1@planet.eon.net)
#
# Created:
# Version       0.5 1999/11/03
# Licence:      wxWindows license
#----------------------------------------------------------------------------

from wxPython.wx           import *
from wxPython.lib.calendar import wxCalendar, Month


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
    def __init__(self, parent, log):
        wxPanel.__init__(self, parent, -1)

        self.log = log

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

        self.calend.HideTitle()
        self.calend.HideGrid()

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

        wxStaticText(self, -1, "Test Calendar Dialog", wxPoint(350, 50)).SetBackgroundColour(wxNamedColour('Red'))

        bmp = wxBitmap('bitmaps/Calend.bmp', wxBITMAP_TYPE_BMP)
        self.but = wxBitmapButton(self, 60, bmp, wxPoint(380, 80))#, wxSize(30, 30))
        EVT_BUTTON(self, 60, self.TestDlg)

# button for calendar window test

        wxStaticText(self, -1, "Test Calendar Window", wxPoint(350, 150)).SetBackgroundColour(wxNamedColour('Blue'))

        bmp = wxBitmap('bitmaps/Calend.bmp', wxBITMAP_TYPE_BMP)
        self.but = wxBitmapButton(self, 160, bmp, wxPoint(380, 180))#, wxSize(30, 30))
        EVT_BUTTON(self, 160, self.TestFrame)

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
        self.m_spin = wxSpinButton(self, 120, wxPoint(130, 20), wxSize(h*2, h), wxSP_VERTICAL)
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

def SetToolPath(self, tb, id, bmp, title):
    global dir_path
    tb.AddTool(id, wxBitmap(os.path.join(dir_path, bmp), wxBITMAP_TYPE_BMP), wxNullBitmap, false, -1, -1, title, title)


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
    win = TestPanel(nb, log)
    return win

#---------------------------------------------------------------------------


overview = """\
This control provides a calendar control class for displaying and selecting dates.

See example for various methods used to set display month, year, and highlighted dates (different colour).

by Lorne White

"""
