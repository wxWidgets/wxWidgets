#----------------------------------------------------------------------------
# Name:         calendar.py
# Purpose:      Calendar display control
#
# Author:       Lorne White (email: lwhite1@planet.eon.net)
#
# Created:
# Version       0.6 2000/03/30
# Licence:      wxWindows license
#----------------------------------------------------------------------------

from wxPython.wx import *

from CDate import *
import string, time


CalDays = [6, 0, 1, 2, 3, 4, 5]
AbrWeekday = {6:"Sun", 0:"Mon", 1:"Tue", 2:"Wed", 3:"Thu", 4:"Fri", 5:"Sat"}
_MIDSIZE = 160

BusCalDays = [0, 1, 2, 3, 4, 5, 6]

# calendar drawing routing

class CalDraw:
    def __init__(self, parent):
        self.pwidth = 1
        self.pheight = 1
        try:
            self.scale = parent.scale
        except:
            self.scale = 1

        self.DefParms()

    def DefParms(self):
        self.grid_color = 'BLACK'       # grid and selection colors
        self.back_color = 'WHITE'
        self.sel_color = 'RED'

        self.high_color = 'LIGHT BLUE'
        self.border_color = 'BLACK'
        self.week_color = 'LIGHT GREY'

        self.week_font_color = 'BLACK'      # font colors
        self.day_font_color = 'BLACK'

        self.font = wxSWISS
        self.bold = wxNORMAL

        self.hide_title = FALSE
        self.hide_grid = FALSE
        self.outer_border = TRUE

        self.title_offset = 0
        self.cal_week_scale = 0.7
        self.show_weekend = FALSE
        self.cal_type = "NORMAL"

    def SetWeekColor(self, font_color, week_color):     # set font and background color for week title
        self.week_font_color = font_color
        self.week_color = week_color

    def SetSize(self, size):
        self.set_sizew = size.width
        self.set_sizeh = size.height

    def InitValues(self):       # default dimensions of various elements of the calendar
        self.rg = {}
        self.cal_sel = {}
        self.set_cy_st = 0      # start position
        self.set_cx_st = 0

        self.set_y_mrg = 15      # start of vertical draw default
        self.set_x_mrg = 10
        self.set_y_end = 10

    def SetPos(self, xpos, ypos):
        self.set_cx_st = xpos
        self.set_cy_st = ypos

    def SetMarg(self, xmarg, ymarg):
        self.set_x_st = xmarg
        self.set_y_st = ymarg
        self.set_y_end = ymarg

    def InitScale(self):        # scale position values
        self.sizew = self.set_sizew * self.pwidth
        self.sizeh = self.set_sizeh * self.pheight

        self.cx_st = self.set_cx_st * self.pwidth       # draw start position
        self.cy_st = self.set_cy_st * self.pheight

        self.x_mrg = self.set_x_mrg * self.pwidth         # calendar draw margins
        self.y_mrg = self.set_y_mrg * self.pheight
        self.y_end = self.set_y_end * self.pheight

    def DrawCal(self, DC, sel_lst=[]):
        self.DC = DC
        self.InitScale()

        self.DrawBorder()
        if self.hide_title is FALSE:
            self.DrawMonth()

        self.Center()

        self.DrawGrid()
        self.GetRect()

        if self.show_weekend is TRUE:       # highlight weekend dates
            self.SetWeekEnd()

        self.AddSelect(sel_lst)     # overrides the weekend highlight

        self.DrawSel()      # highlighted days
        self.DrawWeek()
        self.DrawNum()

    def AddSelect(self, list, cfont=None, cbackgrd = None):
        if cfont is None:
            cfont = self.sel_color      # font digit color
        if cbackgrd is None:
            cbackgrd = self.high_color     # select background color

        for val in list:
            self.cal_sel[val] = (cfont, cbackgrd)

    def DrawBorder(self):   # draw border around the outside of the main display rectangle
        brush = wxBrush(wxNamedColour(self.back_color), wxSOLID)
        self.DC.SetBrush(brush)
        self.DC.SetPen(wxPen(wxNamedColour(self.border_color), 1))

        if self.outer_border is TRUE:
            rect = wxRect(self.cx_st, self.cy_st, self.sizew, self.sizeh)  # full display window area
            self.DC.DrawRectangle(rect.x, rect.y, rect.width, rect.height)

    def DrawNumVal(self):
        self.DrawNum()

# calculate the calendar days and offset position

    def SetCal(self, year, month):
        self.InitValues()       # reset initial values

        self.year = year
        self.month = month

        day = 1
        t = Date(year, month, day)
        dow = self.dow = t.day_of_week     # start day in month
        dim = self.dim = t.days_in_month   # number of days in month
        if self.cal_type == "NORMAL":
            start_pos = dow+1
        else:
            start_pos = dow

        self.st_pos = start_pos

        self.cal = []
        for i in range(start_pos):
            self.cal.append('')
        i = 1
        while i <= dim:
            self.cal.append(str(i))
            i = i + 1
        return start_pos

    def SetWeekEnd(self, font_color='BLACK', backgrd = 'LIGHT GREY'):
        date = 6 - int(self.dow)     # start day of first saturday
        while date <= self.dim:
            self.cal_sel[date] = (font_color, backgrd)  # Saturday
            date = date + 1
            if date <= self.dim:
                self.cal_sel[date] = (font_color, backgrd)      # Sunday
                date = date + 6
            else:
                date = date + 7

    def GetRect(self):      # get the display rectange list of the day grid
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

    def DrawMonth(self):        # month and year title
        month = Month[self.month]

        sizef = 11
        if self.sizeh < _MIDSIZE:
            sizef = 10

        f = wxFont(sizef, self.font, wxNORMAL, self.bold)
        self.DC.SetFont(f)

        tw,th = self.DC.GetTextExtent(month)
        adjust = self.cx_st + (self.sizew-tw)/2
        self.DC.DrawText(month, adjust, self.cy_st + th)

        year = str(self.year)
        tw,th = self.DC.GetTextExtent(year)
        adjust =  self.sizew - tw - self.x_mrg

        self.title_offset = th * 2

        f = wxFont(sizef, self.font, wxNORMAL, self.bold)
        self.DC.SetFont(f)
        self.DC.DrawText(year, self.cx_st + adjust, self.cy_st + th)

    def DrawWeek(self):     # draw the week days
        sizef = 8
        if self.sizeh < _MIDSIZE:
            sizef = 7

        f = wxFont(sizef, self.font, wxNORMAL, self.bold)
        self.DC.SetFont(f)
        self.DC.SetTextForeground(wxNamedColour(self.week_font_color))

        cnt_x = 0
        cnt_y = 0
        width = self.gridx[1]-self.gridx[0]
        height = self.gridy[1] - self.gridy[0]

        rect_w = self.gridx[7]-self.gridx[0]

        brush = wxBrush(wxNamedColour(self.week_color), wxSOLID)
        self.DC.SetBrush(brush)
#        self.DC.DrawRectangle(self.gridx[0], self.gridy[0], rect_w+1, height)

        if self.cal_type == "NORMAL":
            cal_days = CalDays
        else:
            cal_days = BusCalDays

        for val in cal_days:
            day = AbrWeekday[val]
            if self.sizew < 200:
                day = day[0]
            dw,dh = self.DC.GetTextExtent(day)
            diffx = (width-dw)/2
            diffy = (height-dh)/2

            x = self.gridx[cnt_x]
            y = self.gridy[cnt_y]
            self.DC.DrawRectangle(self.gridx[cnt_x], self.gridy[0], width+1, height)
            self.DC.DrawText(day, x+diffx, y+diffy)
            cnt_x = cnt_x + 1


    def DrawNum(self):      # draw the day numbers
        sizef = 10
        if self.sizeh < _MIDSIZE:
            sizef = 8
        f = wxFont(sizef, self.font, wxNORMAL, self.bold)

        cnt_x = 0
        cnt_y = 1
        for val in self.cal:
            x = self.gridx[cnt_x]
            y = self.gridy[cnt_y]

            try:
                num_val = int(val)
                num_color = self.cal_sel[num_val][0]
            except:
                num_color = self.day_font_color

            self.DC.SetTextForeground(wxNamedColour(num_color))
            self.DC.SetFont(f)

            self.DC.DrawText(val, x+5, y+5)
            if cnt_x < 6:
                cnt_x = cnt_x + 1
            else:
                cnt_x = 0
                cnt_y = cnt_y + 1

    def Center(self):       # calculate the dimensions in the center of the drawing area
        bdw = self.x_mrg * 2
        bdh = self.y_mrg + self.y_end + self.title_offset

        self.dl_w = int((self.sizew-bdw)/7)
        self.dl_h = int((self.sizeh-bdh)/7)

        self.dl_th = int(self.dl_h*self.cal_week_scale)     # week title adjustment
        self.cwidth = self.dl_w * 7
        self.cheight = self.dl_h * 6 + self.dl_th

    def DrawSel(self):         # highlighted selected days
        for key in self.cal_sel.keys():
            sel_color = self.cal_sel[key][1]
            brush = wxBrush(wxNamedColour(sel_color), wxSOLID)
            self.DC.SetBrush(brush)

            if self.hide_grid is FALSE:
                self.DC.SetPen(wxPen(wxNamedColour(self.grid_color), 0))
            else:
                self.DC.SetPen(wxPen(wxNamedColour(self.back_color), 0))
            nkey = key + self.st_pos -1
            rect = self.rg[nkey]
            self.DC.DrawRectangle(rect.x, rect.y, rect.width+1, rect.height+1)

    def DrawGrid(self):         # calculate and draw the grid lines
        self.DC.SetPen(wxPen(wxNamedColour(self.grid_color), 0))

        self.gridx = []
        self.gridy = []

        self.x_st = self.cx_st + self.x_mrg
        self.y_st = self.cy_st + self.y_mrg + self.title_offset     # start postion of draw

        x1 = self.x_st
        y1 = self.y_st

        y2 = y1 + self.cheight
        for i in range(8):
            if self.hide_grid is FALSE:
                self.DC.DrawLine(x1, y1, x1, y2)
            self.gridx.append(x1)
            x1 = x1 + self.dl_w

        x1 = self.x_st
        y1 = self.y_st

        x2 = x1 + self.cwidth
        for i in range(8):
            if self.hide_grid is FALSE:
                self.DC.DrawLine(x1, y1, x2, y1)
            self.gridy.append(y1)
            if i == 0:
                y1 = y1 + self.dl_th
            else:
                y1 = y1 + self.dl_h


class PrtCalDraw(CalDraw):
    def InitValues(self):
        self.rg = {}
        self.cal_sel = {}
        self.set_cx_st = 1.0        # start draw border location
        self.set_cy_st = 1.0

        self.set_y_mrg = 0.2      # draw offset position
        self.set_x_mrg = 0.2
        self.set_y_end = 0.2

    def SetPSize(self, pwidth, pheight):    # calculate the dimensions in the center of the drawing area
        self.pwidth = int(pwidth)/self.scale
        self.pheight = int(pheight)/self.scale

    def SetPreview(self, preview):
        self.preview = preview

class wxCalendar(wxWindow):
    def __init__(self, parent, id, pos=wxDefaultPosition, size=wxDefaultSize):
        wxWindow.__init__(self, parent, id, pos, size)

    # set the calendar control attributes

        self.grid_color = 'BLACK'
        self.back_color = 'WHITE'
        self.hide_grid = FALSE
        self.sel_color = 'RED'
        self.hide_title = FALSE
        self.show_weekend = FALSE
        self.cal_type = "NORMAL"

        self.week_color = 'LIGHT GREY'
        self.week_font_color = 'BLACK'      # font colors

        self.select_list = []

        self.SetBackgroundColour(wxNamedColor(self.back_color))
        self.Connect(-1, -1, wxEVT_LEFT_DOWN, self.OnLeftEvent)
        self.Connect(-1, -1, wxEVT_LEFT_DCLICK, self.OnLeftDEvent)
        self.Connect(-1, -1, wxEVT_RIGHT_DOWN, self.OnRightEvent)
        self.Connect(-1, -1, wxEVT_RIGHT_DCLICK, self.OnRightDEvent)

        self.sel_key = None      #  last used by
        self.sel_lst = []        # highlighted selected days

        self.SetNow()       # default calendar for current month

        self.size = None
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
            ms_rect = wxRect(mx, my, 1, 1)
            if wxIntersectRect(ms_rect, val) != None:
                result = self.TestDay(key)
                return result
        return None

# calendar drawing

    def SetWeekColor(self, font_color, week_color):     # set font and background color for week title
        self.week_font_color = font_color
        self.week_color = week_color

    def AddSelect(self, list, font_color, back_color):
        list_val = [list, font_color, back_color]
        self.select_list.append(list_val)

    def ShowWeekEnd(self):
        self.show_weekend = TRUE    # highlight weekend

    def SetBusType(self):
        self.cal_type = "BUS"

    def OnPaint(self, event):
        DC = wxPaintDC(self)
        self.DoDrawing(DC)

    def DoDrawing(self, DC):
        DC = wxPaintDC(self)
        DC.BeginDrawing()

        self.cal = cal = CalDraw(self)

        cal.grid_color = self.grid_color
        cal.back_color = self.back_color
        cal.hide_grid = self.hide_grid
        cal.grid_color = self.grid_color
        cal.hide_title = self.hide_title
        cal.show_weekend = self.show_weekend
        cal.cal_type = self.cal_type

        if self.size is None:
            size = self.GetClientSize()
        else:
            size = self.size

# drawing attributes

        cal.week_font_color = self.week_font_color
        cal.week_color = self.week_color

        cal.SetSize(size)
        cal.SetCal(self.year, self.month)
        for val in self.select_list:
            cal.AddSelect(val[0], val[1], val[2])

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


