#! /usr/local/bin/python
#----------------------------------------------------------------------------
# Name:         wxCalend.py
# Purpose:      Calendar display control
#
# Author:       Lorne White (email: lwhite1@planet.eon.net)
#
# Created:
# Version       0.5 1999/11/03
# Licence:      wxWindows license
#----------------------------------------------------------------------------

from wxPython.wx import *

try:
    from DateTime import *
    using_mxDateTime = true
except ImportError:
    from CDate import *
    using_mxDateTime = false

import string, time

CalDays = [6, 0, 1, 2, 3, 4, 5]
AbrWeekday = {6:"Sun", 0:"Mon", 1:"Tue", 2:"Wed", 3:"Thu", 4:"Fri", 5:"Sat"}


# calendar drawing routing

class CalDraw:
    def __init__(self):
        self.rg = {}
        self.y_st = 15      # start of vertical draw default

    def SetSize(self, size):
        self.sizew = size.width
        self.sizeh = size.height

# draw the various elements of the calendar

    def DrawCal(self, DC, sel_lst):
        self.DC = DC

        self.DrawBorder()

        if self.hide_title is FALSE:
            self.DrawMonth()

        self.Center()

        self.DrawGrid()
        self.GetRect()

        self.DrawSel(sel_lst)   # highlighted days
        self.DrawWeek()
        self.DrawNum()

# draw border around the outside of the main display rectangle

    def DrawBorder(self):
        rect = wxRect(0, 0, self.sizew, self.sizeh)  # full display window area
        self.DC.DrawRectangle(rect.x, rect.y, rect.width, rect.height)

    def DrawNumVal(self):
        self.DrawNum()

# calculate the calendar days and offset position

    def SetCal(self, year, month):
        self.year = year
        self.month = month

        day = 1
        t = Date(year, month, day)
        dow = t.day_of_week     # start day in month
        dim = t.days_in_month   # number of days in month
        start_pos = dow+1
        self.st_pos = start_pos

        self.cal = []
        for i in range(start_pos):
            self.cal.append('')
        i = 1
        while i <= dim:
            self.cal.append(str(i))
            i = i + 1
        return start_pos

# get the display rectange list of the day grid

    def GetRect(self):
        cnt = 0
        for y in self.gridy[1:-1]:
            for x in self.gridx[:-1]:
                rect = wxRect(x, y, self.dl_w, self.dl_h)  # create rect region
                self.rg[cnt] = rect
                cnt = cnt + 1
        return self.rg

    def GetCal(self):
        return self.cal

    def GetOffset(self):
        return self.st_pos

# month and year title

    def DrawMonth(self):
        month = Month[self.month]

        sizef = 12
        if self.sizeh < 200:
            sizef = 8

        f = wxFont(sizef, self.font, wxNORMAL, wxNORMAL)
        self.DC.SetFont(f)

        tw,th = self.DC.GetTextExtent(month)
        adjust = (self.sizew-tw)/2
        self.DC.DrawText(month, adjust, 10)

        year = str(self.year)
        tw,th = self.DC.GetTextExtent(year)
        adjust = self.sizew-tw-20

        self.y_st = th * 3

        f = wxFont(sizef, self.font, wxNORMAL, wxNORMAL)
        self.DC.SetFont(f)
        self.DC.DrawText(year, adjust, 10)

# draw the week days

    def DrawWeek(self):
        sizef = 8
        if self.sizeh < 300:
            sizef = 6

        f = wxFont(sizef, self.font, wxNORMAL, wxNORMAL)
        self.DC.SetFont(f)

        cnt_x = 0
        cnt_y = 0
        width = self.gridx[1]-self.gridx[0]
        height = self.gridy[1] - self.gridy[0]

        for val in CalDays:
            day = AbrWeekday[val]
            if self.sizew < 200:
                day = day[0]
            dw,dh = self.DC.GetTextExtent(day)
            diffx = (width-dw)/2
            diffy = (height-dh)/2

            x = self.gridx[cnt_x]
            y = self.gridy[cnt_y]
            self.DC.DrawText(day, x+diffx, y+diffy)
            cnt_x = cnt_x + 1

# draw the day numbers

    def DrawNum(self):
        sizef = 9
        if self.sizeh < 260:
            sizef = 6
        f = wxFont(sizef, self.font, wxNORMAL, wxNORMAL)
        self.DC.SetFont(f)

        cnt_x = 0
        cnt_y = 1
        for val in self.cal:
            x = self.gridx[cnt_x]
            y = self.gridy[cnt_y]
            self.DC.DrawText(val, x+5, y+5)
            if cnt_x < 6:
                cnt_x = cnt_x + 1
            else:
                cnt_x = 0
                cnt_y = cnt_y + 1

# calculate the dimensions in the center of the drawing area

    def Center(self):
        self.x_st = 10
        self.y_end = 10

        bdw = self.x_st * 2
        bdh = self.y_st + self.y_end

        self.dl_w = (self.sizew-bdw)/7
        self.dl_h = (self.sizeh-bdh)/7

        self.cwidth = self.dl_w * 7
        self.cheight = self.dl_h * 6 + self.dl_h/2

# highlighted selectioned days

    def DrawSel(self, sel_lst):
        for key in sel_lst:
            brush = wxBrush(wxNamedColour(self.high_color), wxSOLID)
            self.DC.SetBrush(brush)
            if self.hide_grid is FALSE:
                self.DC.SetPen(wxPen(wxNamedColour(self.grid_color), 0))
            else:
                self.DC.SetPen(wxPen(wxNamedColour(self.back_color), 0))
            nkey = key + self.st_pos -1
            rect = self.rg[nkey]
            self.DC.DrawRectangle(rect.x, rect.y, rect.width+1, rect.height+1)

# calculate and draw the grid lines

    def DrawGrid(self):
        self.DC.SetPen(wxPen(wxNamedColour(self.grid_color), 0))

        self.gridx = []
        self.gridy = []

        x1 = self.x_st
        y1 = self.y_st
        y1 = self.y_st
        y2 = self.y_st + self.cheight
        for i in range(8):
            if self.hide_grid is FALSE:
                self.DC.DrawLine(x1, y1, x1, y2)
            self.gridx.append(x1)
            x1 = x1 + self.dl_w

        x1 = self.x_st
        y1 = self.y_st
        x2 = self.x_st + self.cwidth
        for i in range(8):
            if self.hide_grid is FALSE:
                self.DC.DrawLine(x1, y1, x2, y1)
            self.gridy.append(y1)
            if i == 0:
                y1 = y1 + self.dl_h/2
            else:
                y1 = y1 + self.dl_h


class Calendar(wxWindow):
    def __init__(self, parent, id, pos=wxDefaultPosition, size=wxDefaultSize):
        wxWindow.__init__(self, parent, id, pos, size)

    # set the calendar control attributes

        self.grid_color = 'BLACK'
        self.back_color = 'WHITE'
        self.sel_color = 'RED'
        self.high_color = 'LIGHT BLUE'
        self.font = wxSWISS

        self.SetBackgroundColour(wxNamedColor(self.back_color))
        self.Connect(-1, -1, wxEVT_LEFT_DOWN, self.OnLeftEvent)
        self.Connect(-1, -1, wxEVT_LEFT_DCLICK, self.OnLeftDEvent)
        self.Connect(-1, -1, wxEVT_RIGHT_DOWN, self.OnRightEvent)
        self.Connect(-1, -1, wxEVT_RIGHT_DCLICK, self.OnRightDEvent)

        self.sel_key = None      #  last used by
        self.sel_lst = []        # highlighted selected days

        self.SetNow()       # default calendar for current month

        self.size = None
        self.hide_title = FALSE
        self.hide_grid = FALSE
        self.set_day = None

# control some of the main calendar attributes

    def HideTitle(self):
        self.hide_title = TRUE

    def HideGrid(self):
        self.hide_grid = TRUE

# determine the calendar rectangle click area and draw a selection

    def ProcessClick(self, event):
        self.x, self.y = event.GetX(), event.GetY()
        key = self.GetDayHit(self.x, self.y)
        self.SelectDay(key)

# tab mouse click events and process

    def OnLeftEvent(self, event):
        self.click = 'LEFT'
        self.ProcessClick(event)

    def OnLeftDEvent(self, event):
        self.click = 'DLEFT'
        self.ProcessClick(event)

    def OnRightEvent(self, event):
        self.click = 'RIGHT'
        self.ProcessClick(event)

    def OnRightDEvent(self, event):
        self.click = 'DRIGHT'
        self.ProcessClick(event)

    def SetSize(self, set_size):
        self.size = set_size

    def SetSelDay(self, sel):
        self.sel_lst = sel  # list of highlighted days

# get the current date

    def SetNow(self):
        dt = now()
        self.month = dt.month
        self.year = dt.year
        self.day = dt.day

# set the current day

    def SetCurrentDay(self):
        self.SetNow()
        self.set_day = self.day

# get the date, day, month, year set in calendar

    def GetDate(self):
        return self.day, self.month, self.year

    def GetDay(self):
        return self.day

    def GetMonth(self):
        return self.month

    def GetYear(self):
        return self.year

# set the day, month, and year

    def SetDayValue(self, day):
        self.set_day = day

    def SetMonth(self, month):
        if month >= 1 and month <= 12:
            self.month = month
        else:
            self.month = 1
        self.set_day = None

    def SetYear(self, year):
        self.year = year

# increment year and month

    def IncYear(self):
        self.year = self.year + 1
        self.set_day = None

    def DecYear(self):
        self.year = self.year - 1
        self.set_day = None

    def IncMonth(self):
        self.month = self.month + 1
        if self.month > 12:
            self.month = 1
            self.year = self.year + 1
        self.set_day = None

    def DecMonth(self):
        self.month = self.month - 1
        if self.month < 1:
            self.month = 12
            self.year = self.year - 1
        self.set_day = None

# test to see if the selection has a date and create event

    def TestDay(self, key):
        try:
            self.day = int(self.cal[key])
        except:
            return None
        if self.day == "":
            return None
        else:
            evt = wxPyCommandEvent(2100, self.GetId())
            evt.click, evt.day, evt.month, evt.year = self.click, self.day, self.month, self.year
            self.GetEventHandler().ProcessEvent(evt)

            self.set_day = self.day
            return key

# find the clicked area rectangle

    def GetDayHit(self, mx, my):
        for key in self.rg.keys():
            val = self.rg[key]
            rt = wxRegion()
            rt.Union(val)
            if rt.Contains(mx, my) != 0:
                result = self.TestDay(key)
                return result
        return None

# calendar drawing

    def OnPaint(self, event):
        DC = wxPaintDC(self)
        self.DoDrawing(DC)

    def DoDrawing(self, DC):
        DC = wxPaintDC(self)
        DC.BeginDrawing()

        self.cal = cal = CalDraw()
        if self.size is None:
            size = self.GetClientSize()
        else:
            size = self.size

# drawing attributes

        cal.hide_title = self.hide_title
        cal.hide_grid = self.hide_grid

        cal.grid_color = self.grid_color
        cal.high_color = self.high_color
        cal.back_color = self.back_color
        cal.font = self.font

        cal.SetSize(size)
        cal.SetCal(self.year, self.month)
        cal.DrawCal(DC, self.sel_lst)

        self.rg = cal.GetRect()
        self.cal = cal.GetCal()
        self.st_pos = cal.GetOffset()
        self.ymax = DC.MaxY()

        if self.set_day != None:
            self.SetDay(self.set_day)
        DC.EndDrawing()

# draw the selection rectangle

    def DrawRect(self, key, color = 'BLACK', width = 0):
        if key == None:
            return
        DC = wxClientDC(self)
        DC.BeginDrawing()

        brush = wxBrush(wxColour(0, 0xFF, 0x80), wxTRANSPARENT)
        DC.SetBrush(brush)
        DC.SetPen(wxPen(wxNamedColour(color), width))

        rect = self.rg[key]
        DC.DrawRectangle(rect.x, rect.y, rect.width+1, rect.height+1)

        DC.EndDrawing()

# set the day selection

    def SetDay(self, day):
        day = day + self.st_pos - 1
        self.SelectDay(day)

    def SelectDay(self, key):
        sel_size = 1
        self.DrawRect(self.sel_key, self.back_color, sel_size)     # clear large selection
        if self.hide_grid is FALSE:
            self.DrawRect(self.sel_key, self.grid_color)

        self.DrawRect(key, self.sel_color, sel_size)
        self.sel_key = key      # store last used by
        self.select_day = None

    def ClearDsp(self):
        self.Clear()


