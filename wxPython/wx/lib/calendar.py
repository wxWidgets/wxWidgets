#----------------------------------------------------------------------------
# Name:         calendar.py
# Purpose:      Calendar display control
#
# Author:       Lorne White (email: lorne.white@telusplanet.net)
#
# Created:
# Version       0.92
# Date:         Nov 26, 2001
# Licence:      wxWindows license
#----------------------------------------------------------------------------
# 12/01/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# o Tested with updated demo
# o Added new event type EVT_CALENDAR. The reason for this is that the original
#   library used a hardcoded ID of 2100 for generating events. This makes it
#   very difficult to fathom when trying to decode the code since there's no
#   published API. Creating the new event binder might seem like overkill - 
#   after all, you might ask, why not just use a new event ID and be done with
#   it? However, a consistent interface is very useful at times; also it makes
#   it clear that we're not just hunting for mouse clicks -- we're hunting
#   wabbit^H^H^H^H (sorry bout that) for calender-driven mouse clicks. So
#   that's my sad story. Shoot me if you must :-)
# o There's still one deprecation warning buried in here somewhere, but I 
#   haven't been able to find it yet. It only occurs when displaying a 
#   print preview, and only the first time. It *could* be an error in the
#   demo, I suppose.
#
#   Here's the traceback:
#
#   C:\Python\lib\site-packages\wx\core.py:949: DeprecationWarning: 
#       integer argument expected, got float
#   newobj = _core.new_Rect(*args, **kwargs)
# 
# 12/17/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o A few style-guide nips and tucks
# o Renamed wxCalendar to Calendar
# o Couple of bugfixes
#

import  wx

from CDate import *

CalDays = [6, 0, 1, 2, 3, 4, 5]
AbrWeekday = {6:"Sun", 0:"Mon", 1:"Tue", 2:"Wed", 3:"Thu", 4:"Fri", 5:"Sat"}
_MIDSIZE = 180

BusCalDays = [0, 1, 2, 3, 4, 5, 6]

# Calendar click event - added 12/1/03 by jmg (see above)
wxEVT_COMMAND_PYCALENDAR_DAY_CLICKED = wx.NewEventType()
EVT_CALENDAR = wx.PyEventBinder(wxEVT_COMMAND_PYCALENDAR_DAY_CLICKED, 1)

def GetMonthList():
    monthlist = []
    for i in range(13):
        name = Month[i]
        if name != None:
            monthlist.append(name)
    return monthlist

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
        self.num_auto = True    # auto scale of the cal number day size
        self.num_size = 12      # default size of calendar if no auto size
        self.max_num_size = 12  # maximum size for calendar number

        self.num_align_horz = wx.ALIGN_CENTRE    # alignment of numbers
        self.num_align_vert = wx.ALIGN_CENTRE
        self.num_indent_horz = 0     # points indent from position, used to offset if not centered
        self.num_indent_vert = 0

        self.week_auto = True       # auto scale of week font text
        self.week_size = 10
        self.max_week_size = 12

        self.grid_color = 'BLACK'       # grid and selection colors
        self.back_color = 'WHITE'
        self.sel_color = 'RED'

        self.high_color = 'LIGHT BLUE'
        self.border_color = 'BLACK'
        self.week_color = 'LIGHT GREY'

        self.week_font_color = 'BLACK'      # font colors
        self.day_font_color = 'BLACK'

        self.font = wx.SWISS
        self.bold = wx.NORMAL

        self.hide_title = False
        self.hide_grid = False
        self.outer_border = True

        self.title_offset = 0
        self.cal_week_scale = 0.7
        self.show_weekend = False
        self.cal_type = "NORMAL"

    def SetWeekColor(self, font_color, week_color):     # set font and background color for week title
        self.week_font_color = font_color
        self.week_color = week_color

    def SetSize(self, size):
        self.set_sizew = size[0]
        self.set_sizeh = size[1]

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
        self.sizew = int(self.set_sizew * self.pwidth)
        self.sizeh = int(self.set_sizeh * self.pheight)

        self.cx_st = int(self.set_cx_st * self.pwidth)       # draw start position
        self.cy_st = int(self.set_cy_st * self.pheight)

        self.x_mrg = int(self.set_x_mrg * self.pwidth)         # calendar draw margins
        self.y_mrg = int(self.set_y_mrg * self.pheight)
        self.y_end = int(self.set_y_end * self.pheight)

    def DrawCal(self, DC, sel_lst=[]):
        self.DC = DC
        self.InitScale()

        self.DrawBorder()

        if self.hide_title is False:
            self.DrawMonth()

        self.Center()

        self.DrawGrid()
        self.GetRect()

        if self.show_weekend is True:       # highlight weekend dates
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

    # draw border around the outside of the main display rectangle
    def DrawBorder(self):   
        brush = wx.Brush(wx.NamedColour(self.back_color), wx.SOLID)
        self.DC.SetBrush(brush)
        self.DC.SetPen(wx.Pen(wx.NamedColour(self.border_color), 1))

        if self.outer_border is True:
            # full display window area
            rect = wx.Rect(self.cx_st, self.cy_st, self.sizew, self.sizeh)  
            self.DC.DrawRectangleRect(rect)

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

    # get the display rectange list of the day grid
    def GetRect(self):      
        cnt = 0
        for y in self.gridy[1:-1]:
            for x in self.gridx[:-1]:
                assert type(y) == int
                assert type(x) == int
                rect = wx.Rect(x, y, self.dl_w, self.dl_h)  # create rect region
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

        sizef = 11
        if self.sizeh < _MIDSIZE:
            sizef = 10

        f = wx.Font(sizef, self.font, wx.NORMAL, self.bold)
        self.DC.SetFont(f)

        tw,th = self.DC.GetTextExtent(month)
        adjust = self.cx_st + (self.sizew-tw)/2
        self.DC.DrawText(month, (adjust, self.cy_st + th))

        year = str(self.year)
        tw,th = self.DC.GetTextExtent(year)
        adjust =  self.sizew - tw - self.x_mrg

        self.title_offset = th * 2

        f = wx.Font(sizef, self.font, wx.NORMAL, self.bold)
        self.DC.SetFont(f)
        self.DC.DrawText(year, (self.cx_st + adjust, self.cy_st + th))

    def DrawWeek(self):     # draw the week days
        width = self.gridx[1]-self.gridx[0]
        height = self.gridy[1] - self.gridy[0]
        rect_w = self.gridx[7]-self.gridx[0]

        f = wx.Font(10, self.font, wx.NORMAL, self.bold)      # initial font setting

        if self.week_auto == True:
            test_size = self.max_week_size      # max size
            test_day = ' Sun '
            while test_size > 2:
                f.SetPointSize(test_size)
                self.DC.SetFont(f)
                tw,th = self.DC.GetTextExtent(test_day)

                if tw < width and th < height:
                    break

                test_size = test_size - 1
        else:
            f.SetPointSize(self.week_size)   # set fixed size
            self.DC.SetFont(f)

        self.DC.SetTextForeground(wx.NamedColour(self.week_font_color))

        cnt_x = 0
        cnt_y = 0

        brush = wx.Brush(wx.NamedColour(self.week_color), wx.SOLID)
        self.DC.SetBrush(brush)
        self.DC.DrawRectangle((self.gridx[0], self.gridy[0]), (rect_w+1, height))

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
            self.DC.DrawRectangle((self.gridx[cnt_x], self.gridy[0]), (width+1, height))
            self.DC.DrawText(day, (x+diffx, y+diffy))
            cnt_x = cnt_x + 1

    # draw the day numbers
    def DrawNum(self):      
        f = wx.Font(10, self.font, wx.NORMAL, self.bold)      # initial font setting

        if self.num_auto == True:
            test_size = self.max_num_size      # max size
            test_day = ' 99 '

            while test_size > 2:
                f.SetPointSize(test_size)
                self.DC.SetFont(f)
                tw,th = self.DC.GetTextExtent(test_day)

                if tw < self.dl_w and th < self.dl_h:
                    sizef = test_size
                    break
                test_size = test_size - 1
        else:
            f.SetPointSize(self.num_size)   # set fixed size
            self.DC.SetFont(f)

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

            self.DC.SetTextForeground(wx.NamedColour(num_color))
            self.DC.SetFont(f)

            tw,th = self.DC.GetTextExtent(val)

            if self.num_align_horz == wx.ALIGN_CENTRE:
                adj_h = (self.dl_w - tw)/2
            elif self.num_align_horz == wx.ALIGN_RIGHT:
                adj_h = self.dl_w - tw
            else:
                adj_h = 0   # left alignment

            adj_h = adj_h + self.num_indent_horz

            if self.num_align_vert == wx.ALIGN_CENTRE:
                adj_v = (self.dl_h - th)/2
            elif self.num_align_horz == wx.ALIGN_RIGHT:
                adj_v = self.dl_h - th
            else:
                adj_v = 0   # left alignment

            adj_v = adj_v + self.num_indent_vert

            self.DC.DrawText(val, (x+adj_h, y+adj_v))

            if cnt_x < 6:
                cnt_x = cnt_x + 1
            else:
                cnt_x = 0
                cnt_y = cnt_y + 1

    # calculate the dimensions in the center of the drawing area
    def Center(self):       
        bdw = self.x_mrg * 2
        bdh = self.y_mrg + self.y_end + self.title_offset

        self.dl_w = int((self.sizew-bdw)/7)
        self.dl_h = int((self.sizeh-bdh)/7)

        # week title adjustment
        self.dl_th = int(self.dl_h*self.cal_week_scale)
        self.cwidth = self.dl_w * 7
        self.cheight = self.dl_h * 6 + self.dl_th

    # highlighted selected days
    def DrawSel(self):
        for key in self.cal_sel.keys():
            sel_color = self.cal_sel[key][1]
            brush = wx.Brush(wx.NamedColour(sel_color), wx.SOLID)
            self.DC.SetBrush(brush)

            if self.hide_grid is False:
                self.DC.SetPen(wx.Pen(wx.NamedColour(self.grid_color), 0))
            else:
                self.DC.SetPen(wx.Pen(wx.NamedColour(self.back_color), 0))

            nkey = key + self.st_pos -1
            rect = self.rg[nkey]
            self.DC.DrawRectangle((rect.x, rect.y), (rect.width+1, rect.height+1))

    # calculate and draw the grid lines
    def DrawGrid(self):
        self.DC.SetPen(wx.Pen(wx.NamedColour(self.grid_color), 0))

        self.gridx = []
        self.gridy = []

        self.x_st = self.cx_st + self.x_mrg
        # start postion of draw
        self.y_st = self.cy_st + self.y_mrg + self.title_offset

        x1 = self.x_st
        y1 = self.y_st
        y2 = y1 + self.cheight

        for i in range(8):
            if self.hide_grid is False:
                self.DC.DrawLine((x1, y1), (x1, y2))
            self.gridx.append(x1)
            x1 = x1 + self.dl_w

        x1 = self.x_st
        y1 = self.y_st
        x2 = x1 + self.cwidth

        for i in range(8):
            if self.hide_grid is False:
                self.DC.DrawLine((x1, y1), (x2, y1))

            self.gridy.append(y1)

            if i == 0:
                y1 = y1 + self.dl_th
            else:
                y1 = y1 + self.dl_h


class PrtCalDraw(CalDraw):
    def InitValues(self):
        self.rg = {}
        self.cal_sel = {}
        # start draw border location
        self.set_cx_st = 1.0        
        self.set_cy_st = 1.0

        # draw offset position
        self.set_y_mrg = 0.2      
        self.set_x_mrg = 0.2
        self.set_y_end = 0.2

    # calculate the dimensions in the center of the drawing area
    def SetPSize(self, pwidth, pheight):    
        self.pwidth = int(pwidth)/self.scale
        self.pheight = int(pheight)/self.scale

    def SetPreview(self, preview):
        self.preview = preview

class Calendar(wx.Window):
    def __init__(self, parent, id, pos=wx.DefaultPosition, size=wx.DefaultSize):
        wx.Window.__init__(self, parent, id, pos, size)

        # set the calendar control attributes

        self.grid_color = 'BLACK'
        self.back_color = 'WHITE'
        self.hide_grid = False
        self.sel_color = 'RED'
        self.hide_title = False
        self.show_weekend = False
        self.cal_type = "NORMAL"

        # font colors
        self.week_color = 'LIGHT GREY'
        self.week_font_color = 'BLACK'      

        self.select_list = []

        self.SetBackgroundColour(self.back_color)
        self.Bind(wx.EVT_LEFT_DOWN, self.OnLeftEvent)
        self.Bind(wx.EVT_LEFT_DCLICK, self.OnLeftDEvent)
        self.Bind(wx.EVT_RIGHT_DOWN, self.OnRightEvent)
        self.Bind(wx.EVT_RIGHT_DCLICK, self.OnRightDEvent)

        self.sel_key = None      #  last used by
        self.sel_lst = []        # highlighted selected days

        # default calendar for current month
        self.SetNow()       

        self.size = None
        self.set_day = None

        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_SIZE, self.OnSize)

    # control some of the main calendar attributes

    def HideTitle(self):
        self.hide_title = True

    def HideGrid(self):
        self.hide_grid = True

    # determine the calendar rectangle click area and draw a selection

    def ProcessClick(self, event):
        self.x, self.y = event.GetX(), event.GetY()
        key = self.GetDayHit(self.x, self.y)
        self.SelectDay(key)

    # tab mouse click events and process

    def OnLeftEvent(self, event):
        self.click = 'LEFT'
        self.shiftkey = event.ShiftDown()
        self.ctrlkey = event.ControlDown()
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
        # list of highlighted days
        self.sel_lst = sel  

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
            # Changed 12/1/03 by jmg (see above) to support 2.5 event binding
            evt = wx.PyCommandEvent(wxEVT_COMMAND_PYCALENDAR_DAY_CLICKED, self.GetId())
            evt.click, evt.day, evt.month, evt.year = self.click, self.day, self.month, self.year
            evt.shiftkey = self.shiftkey
            evt.ctrlkey = self.ctrlkey
            self.GetEventHandler().ProcessEvent(evt)

            self.set_day = self.day
            return key

    # find the clicked area rectangle

    def GetDayHit(self, mx, my):
        for key in self.rg.keys():
            val = self.rg[key]
            ms_rect = wx.Rect(mx, my, 1, 1)
            if wx.IntersectRect(ms_rect, val) is not None:
                result = self.TestDay(key)
                return result

        return None

    # calendar drawing

    def SetWeekColor(self, font_color, week_color):     
        # set font and background color for week title
        self.week_font_color = font_color
        self.week_color = week_color

    def AddSelect(self, list, font_color, back_color):
        list_val = [list, font_color, back_color]
        self.select_list.append(list_val)

    def ShowWeekEnd(self):
        # highlight weekend
        self.show_weekend = True

    def SetBusType(self):
        self.cal_type = "BUS"

    def OnSize(self, evt):
        self.Refresh(False)
        evt.Skip()

    def OnPaint(self, event):
        DC = wx.PaintDC(self)
        self.DoDrawing(DC)

    def DoDrawing(self, DC):
        DC = wx.PaintDC(self)
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

        DC = wx.ClientDC(self)
        DC.BeginDrawing()

        brush = wx.Brush(wx.Colour(0, 0xFF, 0x80), wx.TRANSPARENT)
        DC.SetBrush(brush)
        DC.SetPen(wx.Pen(wx.NamedColour(color), width))

        rect = self.rg[key]
        DC.DrawRectangle((rect.x, rect.y), (rect.width+1, rect.height+1))

        DC.EndDrawing()

    # set the day selection

    def SetDay(self, day):
        day = day + self.st_pos - 1
        self.SelectDay(day)

    def SelectDay(self, key):
        sel_size = 1
        # clear large selection
        self.DrawRect(self.sel_key, self.back_color, sel_size)

        if self.hide_grid is False:
            self.DrawRect(self.sel_key, self.grid_color)

        self.DrawRect(key, self.sel_color, sel_size)
        # store last used by
        self.sel_key = key
        self.select_day = None

    def ClearDsp(self):
        self.Clear()

class CalenDlg(wx.Dialog):
    def __init__(self, parent, month=None, day = None, year=None):
        wx.Dialog.__init__(self, parent, -1, "Event Calendar", wx.DefaultPosition, (280, 360))

        # set the calendar and attributes
        self.calend = Calendar(self, -1, (20, 60), (240, 200))

        if month == None:
            self.calend.SetCurrentDay()
            start_month = self.calend.GetMonth()
            start_year = self.calend.GetYear()
        else:
            self.calend.month = start_month = month
            self.calend.year = start_year = year
            self.calend.SetDayValue(day)

        self.calend.HideTitle()
        self.ResetDisplay()

        # get month list from DateTime
        monthlist = GetMonthList()

        # select the month
        self.date = wx.ComboBox(self, -1, Month[start_month], (20, 20), (90, -1), 
                                monthlist, wx.CB_DROPDOWN)
        self.Bind(wx.EVT_COMBOBOX, self.EvtComboBox, self.date)

        # alternate spin button to control the month
        h = self.date.GetSize().height
        self.m_spin = wx.SpinButton(self, -1, (130, 20), (h*2, h), wx.SP_VERTICAL)
        self.m_spin.SetRange(1, 12)
        self.m_spin.SetValue(start_month)
        self.Bind(wx.EVT_SPIN, self.OnMonthSpin, self.m_spin)

        # spin button to control the year
        self.dtext = wx.TextCtrl(self, -1, str(start_year), (160, 20), (60, -1))
        h = self.dtext.GetSize().height

        self.y_spin = wx.SpinButton(self, -1, (220, 20), (h*2, h), wx.SP_VERTICAL)
        self.y_spin.SetRange(1980, 2010)
        self.y_spin.SetValue(start_year)

        self.Bind(wx.EVT_SPIN, self.OnYrSpin, self.y_spin)
        self.Bind(EVT_CALENDAR, self.MouseClick, self.calend)

        x_pos = 50
        y_pos = 280
        but_size = (60, 25)

        btn = wx.Button(self, -1, ' Ok ', (x_pos, y_pos), but_size)
        self.Bind(wx.EVT_BUTTON, self.OnOk, btn)

        btn = wx.Button(self, -1, ' Close ', (x_pos + 120, y_pos), but_size)
        self.Bind(wx.EVT_BUTTON, self.OnCancel, btn)

    def OnOk(self, event):
        self.EndModal(wx.ID_OK)

    def OnCancel(self, event):
        self.EndModal(wx.ID_CANCEL)

    # log the mouse clicks
    def MouseClick(self, evt):
        self.month = evt.month
        # result click type and date
        self.result = [evt.click, str(evt.day), Month[evt.month], str(evt.year)]  

        if evt.click == 'DLEFT':
            self.EndModal(wx.ID_OK)

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
        monthval = self.date.FindString(name)
        self.m_spin.SetValue(monthval+1)

        self.calend.SetMonth(monthval+1)
        self.ResetDisplay()

    # set the calendar for highlighted days

    def ResetDisplay(self):
        month = self.calend.GetMonth()
        self.calend.Refresh()



