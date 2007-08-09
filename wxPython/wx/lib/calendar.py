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
# 06/02/2004 - Joerg "Adi" Sieker adi@sieker.info
#
# o Changed color handling, use dictionary instead of members.
#   This causes all color changes to be ignored if they manipluate the members directly.
#   SetWeekColor and other method color methods were adapted to use the new dictionary.
# o Added COLOR_* constants
# o Added SetColor method for Calendar class
# o Added 3D look of week header
# o Added colors for 3D look of header
# o Fixed width calculation.
#   Because of rounding difference the total width and height of the
#   calendar could be up to 6 pixels to small. The last column and row
#   are now wider/taller by the missing amount.
# o Added SetTextAlign method to wxCalendar. This exposes logic
#   which was already there.
# o Fixed CalDraw.SetMarg which set set_x_st and set_y_st which don't get used anywhere.
#   Instead set set_x_mrg and set_y_mrg
# o Changed default X and Y Margin to 0.
# o Added wxCalendar.SetMargin.
#
# 17/03/2004 - Joerg "Adi" Sieker adi@sieker.info
# o Added keyboard navigation to the control.
#   Use the cursor keys to navigate through the ages. :)
#   The Home key function as go to today
# o select day is now a filled rect instead of just an outline
#
# 15/04/2005 - Joe "shmengie" Brown joebrown@podiatryfl.com
# o Adjusted spin control size/placement (On Windows ctrls were overlapping).
# o Set Ok/Cancel buttons to wx.ID_OK & wx.ID_CANCEL to provide default dialog
#   behaviour.
# o If no date has been clicked clicked, OnOk set the result to calend's date,
#   important if keyboard only navigation is used.
#
# 12/10/2006 - Walter Barnes walter_barnes05@yahoo.com
# o Fixed CalDraw to properly render months that start on a Sunday.
#
# 21/10/2006 - Walter Barnes walter_barnes05@yahoo.com
# o Fixed a bug in Calendar: Shift and Control key status was only recorded for
#   left-down events.
# o Added handlers for wxEVT_MIDDLE_DOWN and wxEVT_MIDDLE_DCLICK to generate
#   EVT_CALENDAR for these mouse events.


import wx

from CDate import *

CalDays = [6, 0, 1, 2, 3, 4, 5]
AbrWeekday = {6:"Sun", 0:"Mon", 1:"Tue", 2:"Wed", 3:"Thu", 4:"Fri", 5:"Sat"}
_MIDSIZE = 180

COLOR_GRID_LINES = "grid_lines"
COLOR_BACKGROUND = "background"
COLOR_SELECTION_FONT = "selection_font"
COLOR_SELECTION_BACKGROUND = "selection_background"
COLOR_BORDER = "border"
COLOR_HEADER_BACKGROUND = "header_background"
COLOR_HEADER_FONT = "header_font"
COLOR_WEEKEND_BACKGROUND = "weekend_background"
COLOR_WEEKEND_FONT = "weekend_font"
COLOR_FONT = "font"
COLOR_3D_LIGHT = "3d_light"
COLOR_3D_DARK = "3d_dark"
COLOR_HIGHLIGHT_FONT = "highlight_font"
COLOR_HIGHLIGHT_BACKGROUND = "highlight_background"

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

def MakeColor(in_color):
    try:
        color = wxNamedColour(in_color)
    except:
        color = in_color
    return color

def DefaultColors():
    colors = {}
    colors[COLOR_GRID_LINES] = 'BLACK'
    colors[COLOR_BACKGROUND] = 'WHITE'
    colors[COLOR_SELECTION_FONT]  =  wx.SystemSettings_GetColour(wx.SYS_COLOUR_WINDOWTEXT)
    colors[COLOR_SELECTION_BACKGROUND] =wx.Colour(255,255,225)
    colors[COLOR_BORDER] = 'BLACK'
    colors[COLOR_HEADER_BACKGROUND] = wx.SystemSettings_GetColour(wx.SYS_COLOUR_3DFACE)
    colors[COLOR_HEADER_FONT] = wx.SystemSettings_GetColour(wx.SYS_COLOUR_WINDOWTEXT)
    colors[COLOR_WEEKEND_BACKGROUND] = 'LIGHT GREY'
    colors[COLOR_WEEKEND_FONT] = wx.SystemSettings_GetColour(wx.SYS_COLOUR_WINDOWTEXT)
    colors[COLOR_FONT] = wx.SystemSettings_GetColour(wx.SYS_COLOUR_WINDOWTEXT)
    colors[COLOR_3D_LIGHT] = wx.SystemSettings_GetColour(wx.SYS_COLOUR_BTNHIGHLIGHT)
    colors[COLOR_3D_DARK] = wx.SystemSettings_GetColour(wx.SYS_COLOUR_BTNSHADOW)
    colors[COLOR_HIGHLIGHT_FONT]  =  wx.SystemSettings_GetColour(wx.SYS_COLOUR_HIGHLIGHTTEXT)
    colors[COLOR_HIGHLIGHT_BACKGROUND] = wx.SystemSettings_GetColour(wx.SYS_COLOUR_HIGHLIGHT)
    return colors
# calendar drawing routing

class CalDraw:
    def __init__(self, parent):
        self.pwidth = 1
        self.pheight = 1
        try:
            self.scale = parent.scale
        except:
            self.scale = 1

        self.gridx = []
        self.gridy = []

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

        self.colors = DefaultColors()

        self.font = wx.SWISS
        self.bold = wx.NORMAL

        self.hide_title = False
        self.hide_grid = False
        self.outer_border = True

        self.title_offset = 0
        self.cal_week_scale = 0.7
        self.show_weekend = False
        self.cal_type = "NORMAL"

    def SetWeekColor(self, font_color, week_color):
        # set font and background color for week title
        self.colors[COLOR_HEADER_FONT] = MakeColor(font_color)
        self.colors[COLOR_HEADER_BACKGROUND] = MakeColor(week_color)
        self.colors[COLOR_3D_LIGHT] = MakeColor(week_color)
        self.colors[COLOR_3D_DARK] = MakeColor(week_color)

    def SetSize(self, size):
        self.set_sizew = size[0]
        self.set_sizeh = size[1]

    def InitValues(self):       # default dimensions of various elements of the calendar
        self.rg = {}
        self.cal_sel = {}
        self.set_cy_st = 0      # start position
        self.set_cx_st = 0

        self.set_y_mrg = 1      # start of vertical draw default
        self.set_x_mrg = 1
        self.set_y_end = 1
    def SetPos(self, xpos, ypos):
        self.set_cx_st = xpos
        self.set_cy_st = ypos

    def SetMarg(self, xmarg, ymarg):
        self.set_x_mrg = xmarg
        self.set_y_mrg = ymarg
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
        self.InitScale()

        self.DrawBorder(DC)

        if self.hide_title is False:
            self.DrawMonth(DC)

        self.Center()

        self.DrawGrid(DC)
        self.GetRect()
        if self.show_weekend is True:       # highlight weekend dates
            self.SetWeekEnd()

        self.AddSelect(sel_lst)     # overrides the weekend highlight

        self.DrawSel(DC)      # highlighted days
        self.DrawWeek(DC)
        self.DrawNum(DC)

    def AddSelect(self, list, cfont=None, cbackgrd = None):
        if cfont is None:
            cfont = self.colors[COLOR_SELECTION_FONT]      # font digit color
        if cbackgrd is None:
            cbackgrd = self.colors[COLOR_SELECTION_BACKGROUND]     # select background color

        for val in list:
            self.cal_sel[val] = (cfont, cbackgrd)

    # draw border around the outside of the main display rectangle
    def DrawBorder(self, DC, transparent = False):   
        if self.outer_border is True:
            if transparent == False:
                brush = wx.Brush(MakeColor(self.colors[COLOR_BACKGROUND]), wx.SOLID)
            else:
                brush = wx.TRANSPARENT_BRUSH
            DC.SetBrush(brush)
            DC.SetPen(wx.Pen(MakeColor(self.colors[COLOR_BORDER])))
            # full display window area
            rect = wx.Rect(self.cx_st, self.cy_st, self.sizew, self.sizeh)  
            DC.DrawRectangleRect(rect)

    def DrawFocusIndicator(self, DC):
        if self.outer_border is True:
            DC.SetBrush(wx.TRANSPARENT_BRUSH)
            DC.SetPen(wx.Pen(MakeColor(self.colors[COLOR_HIGHLIGHT_BACKGROUND]), style=wx.DOT))
            # full display window area
            rect = wx.Rect(self.cx_st, self.cy_st, self.sizew, self.sizeh)  
            DC.DrawRectangleRect(rect)

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

        if start_pos > 6:
            start_pos = 0

        self.st_pos = start_pos

        self.cal_days = []
        for i in range(start_pos):
            self.cal_days.append('')

        i = 1
        while i <= dim:
            self.cal_days.append(str(i))
            i = i + 1

        self.end_pos = start_pos + dim - 1

        return start_pos

    def SetWeekEnd(self, font_color=None, backgrd = None):
        if font_color != None:
            self.SetColor(COLOR_WEEKEND_FONT, MakeColor(font_color))
        if backgrd != None:
            self.SetColor(COLOR_WEEKEND_BACKGROUND, MakeColor(backgrd))

        date = 6 - int(self.dow)     # start day of first saturday
        if date == 0:                #...unless we start on Sunday
            self.cal_sel[1] = (self.GetColor(COLOR_WEEKEND_FONT), self.GetColor(COLOR_WEEKEND_BACKGROUND))
            date = 7

        while date <= self.dim:
            self.cal_sel[date] = (self.GetColor(COLOR_WEEKEND_FONT), self.GetColor(COLOR_WEEKEND_BACKGROUND))  # Saturday
            date = date + 1

            if date <= self.dim:
                self.cal_sel[date] = (self.GetColor(COLOR_WEEKEND_FONT), self.GetColor(COLOR_WEEKEND_BACKGROUND))      # Sunday
                date = date + 6
            else:
                date = date + 7

    # get the display rectange list of the day grid
    def GetRect(self):      
        cnt = 0
        h = 0
        w = 0
        for y in self.gridy[1:-1]:
            if y == self.gridy[-2]:
                h = h + self.restH

            for x in self.gridx[:-1]:
                assert type(y) == int
                assert type(x) == int

                w = self.cellW
                h = self.cellH

                if x == self.gridx[-2]:
                    w = w + self.restW

                rect = wx.Rect(x, y, w+1, h+1)  # create rect region

                self.rg[cnt] = rect
                cnt = cnt + 1

        return self.rg

    def GetCal(self):
        return self.cal_days

    def GetOffset(self):
        return self.st_pos

    # month and year title
    def DrawMonth(self, DC):
        month = Month[self.month]

        sizef = 11
        if self.sizeh < _MIDSIZE:
            sizef = 10

        f = wx.Font(sizef, self.font, wx.NORMAL, self.bold)
        DC.SetFont(f)

        tw,th = DC.GetTextExtent(month)
        adjust = self.cx_st + (self.sizew-tw)/2
        DC.DrawText(month, adjust, self.cy_st + th)

        year = str(self.year)
        tw,th = DC.GetTextExtent(year)
        adjust =  self.sizew - tw - self.x_mrg

        self.title_offset = th * 2

        f = wx.Font(sizef, self.font, wx.NORMAL, self.bold)
        DC.SetFont(f)
        DC.DrawText(year, self.cx_st + adjust, self.cy_st + th)

    def DrawWeek(self, DC):     # draw the week days
        # increase by 1 to include all gridlines
        width  = self.gridx[1] - self.gridx[0] + 1
        height = self.gridy[1] - self.gridy[0] + 1
        rect_w = self.gridx[-1] - self.gridx[0]

        f = wx.Font(10, self.font, wx.NORMAL, self.bold)      # initial font setting

        if self.week_auto == True:
            test_size = self.max_week_size      # max size
            test_day = ' Sun '
            while test_size > 2:
                f.SetPointSize(test_size)
                DC.SetFont(f)
                tw,th = DC.GetTextExtent(test_day)

                if tw < width and th < height:
                    break

                test_size = test_size - 1
        else:
            f.SetPointSize(self.week_size)   # set fixed size
            DC.SetFont(f)

        DC.SetTextForeground(MakeColor(self.colors[COLOR_HEADER_FONT]))

        cnt_x = 0
        cnt_y = 0

        brush = wx.Brush(MakeColor(self.colors[COLOR_HEADER_BACKGROUND]), wx.SOLID)
        DC.SetBrush(brush)

        if self.cal_type == "NORMAL":
            cal_days = CalDays
        else:
            cal_days = BusCalDays

        for val in cal_days:
            if val == cal_days[-1]:
                width = width + self.restW

            day = AbrWeekday[val]

            if self.sizew < 200:
                day = day[0]

            dw,dh = DC.GetTextExtent(day)

            diffx = (width-dw)/2
            diffy = (height-dh)/2

            x = self.gridx[cnt_x]
            y = self.gridy[cnt_y]
            pointXY = (x, y)
            pointWH = (width, height)
            if self.hide_grid == False:
                pen = wx.Pen(MakeColor(self.GetColor(COLOR_GRID_LINES)), 1, wx.SOLID)
            else:
                pen = wx.Pen(MakeColor(self.GetColor(COLOR_BACKGROUND)), 1, wx.SOLID)
            DC.SetPen(pen)
            DC.DrawRectanglePointSize( pointXY, pointWH)
            
            old_pen = DC.GetPen()

            pen = wx.Pen(MakeColor(self.colors[COLOR_3D_LIGHT]), 1, wx.SOLID)
            DC.SetPen(pen)
            # draw the horizontal hilight
            startPoint = wx.Point(x + 1 , y + 1)
            endPoint   = wx.Point(x + width - 1, y + 1)
            DC.DrawLinePoint(startPoint, endPoint )

            # draw the vertical hilight
            startPoint = wx.Point(x + 1 , y + 1)
            endPoint   = wx.Point(x + 1, y + height - 2)
            DC.DrawLinePoint(startPoint, endPoint )

            pen = wx.Pen(MakeColor(self.colors[COLOR_3D_DARK]), 1, wx.SOLID)
            DC.SetPen(pen)
            
            # draw the horizontal lowlight
            startPoint = wx.Point(x + 1, y + height - 2)
            endPoint   = wx.Point(x + width - 1, y + height - 2)
            DC.DrawLinePoint(startPoint, endPoint )
            
            # draw the vertical lowlight
            startPoint = wx.Point(x + width - 2 , y + 2)
            endPoint   = wx.Point(x + width - 2, y + height - 2)
            DC.DrawLinePoint(startPoint, endPoint )

            pen = wx.Pen(MakeColor(self.colors[COLOR_FONT]), 1, wx.SOLID)
            
            DC.SetPen(pen)
                
            point = (x+diffx, y+diffy)
            DC.DrawTextPoint(day, point)
            cnt_x = cnt_x + 1

    def _CalcFontSize(self, DC, f):
        if self.num_auto == True:
            test_size = self.max_num_size      # max size
            test_day = ' 99 '

            while test_size > 2:
                f.SetPointSize(test_size)
                DC.SetFont(f)
                tw,th = DC.GetTextExtent(test_day)

                if tw < self.cellW and th < self.cellH:
                    sizef = test_size
                    break
                test_size = test_size - 1
        else:
            f.SetPointSize(self.num_size)   # set fixed size
            DC.SetFont(f)

    # draw the day numbers
    def DrawNum(self, DC):      
        f = wx.Font(10, self.font, wx.NORMAL, self.bold)      # initial font setting
        self._CalcFontSize(DC, f)

        cnt_x = 0
        cnt_y = 1
        for val in self.cal_days:
            x = self.gridx[cnt_x]
            y = self.gridy[cnt_y]

            self._DrawDayText(x, y, val, f, DC)

            if cnt_x < 6:
                cnt_x = cnt_x + 1
            else:
                cnt_x = 0
                cnt_y = cnt_y + 1

    def _DrawDayText(self, x, y, text, font, DC):

        try:
            num_val = int(text)
            num_color = self.cal_sel[num_val][0]
        except:
            num_color = self.colors[COLOR_FONT]
            
        DC.SetTextForeground(MakeColor(num_color))
        DC.SetFont(font)

        tw,th = DC.GetTextExtent(text)
        
        if self.num_align_horz == wx.ALIGN_CENTRE:
            adj_h = (self.cellW - tw)/2
        elif self.num_align_horz == wx.ALIGN_RIGHT:
            adj_h = self.cellW - tw
        else:
            adj_h = 0   # left alignment

        adj_h = adj_h + self.num_indent_horz

        if self.num_align_vert == wx.ALIGN_CENTRE:
            adj_v = (self.cellH - th)/2
        elif self.num_align_vert == wx.ALIGN_BOTTOM:
            adj_v = self.cellH - th
        else:
            adj_v = 0   # left alignment

        adj_v = adj_v + self.num_indent_vert

        DC.DrawTextPoint(text, (x+adj_h, y+adj_v))
        
    def DrawDayText(self, DC, key):
        f = wx.Font(10, self.font, wx.NORMAL, self.bold)      # initial font setting
        self._CalcFontSize(DC, f)

        if key > self.end_pos: 
            key = self.end_pos

        val = self.cal_days[key]
        cnt_x = key % 7
        cnt_y = int(key / 7)+1
        x = self.gridx[cnt_x]
        y = self.gridy[cnt_y]
        self._DrawDayText(x, y, val, f, DC)


    # calculate the dimensions in the center of the drawing area
    def Center(self):       
        borderW = self.x_mrg * 2
        borderH = self.y_mrg + self.y_end + self.title_offset

        self.cellW = int((self.sizew - borderW)/7)
        self.cellH = int((self.sizeh - borderH)/7)

        self.restW = ((self.sizew - borderW)%7 ) - 1

        # week title adjustment
        self.weekHdrCellH = int(self.cellH * self.cal_week_scale)
        # recalculate the cell height exkl. the week header and
        # subtracting the size
        self.cellH = int((self.sizeh - borderH - self.weekHdrCellH)/6)

        self.restH = ((self.sizeh - borderH - self.weekHdrCellH)%6 ) - 1
        self.calW = self.cellW * 7
        self.calH = self.cellH * 6 + self.weekHdrCellH

    # highlighted selected days
    def DrawSel(self, DC):

        for key in self.cal_sel.keys():
            sel_color = self.cal_sel[key][1]
            brush = wx.Brush(MakeColor(sel_color), wx.SOLID)
            DC.SetBrush(brush)

            if self.hide_grid is False:
                DC.SetPen(wx.Pen(MakeColor(self.colors[COLOR_GRID_LINES]), 0))
            else:
                DC.SetPen(wx.Pen(MakeColor(self.colors[COLOR_BACKGROUND]), 0))
            
            nkey = key + self.st_pos -1
            rect = self.rg[nkey]

            DC.DrawRectangleRect(rect)

    # calculate and draw the grid lines
    def DrawGrid(self, DC):
        DC.SetPen(wx.Pen(MakeColor(self.colors[COLOR_GRID_LINES]), 0))

        self.gridx = []
        self.gridy = []

        self.x_st = self.cx_st + self.x_mrg
        # start postion of draw
        self.y_st = self.cy_st + self.y_mrg + self.title_offset
        
        x1 = self.x_st
        y1 = self.y_st
        y2 = y1 + self.calH + self.restH

        for i in range(8):
            if i == 7:
                x1 = x1 + self.restW

            if self.hide_grid is False:
                DC.DrawLinePoint((x1, y1), (x1, y2))

            self.gridx.append(x1)

            x1 = x1 + self.cellW

        x1 = self.x_st
        y1 = self.y_st
        x2 = x1 + self.calW + self.restW

        for i in range(8):
            if i == 7:
                y1 = y1 + self.restH

            if self.hide_grid is False:
                DC.DrawLinePoint((x1, y1), (x2, y1))

            self.gridy.append(y1)

            if i == 0:
                y1 = y1 + self.weekHdrCellH
            else:
                y1 = y1 + self.cellH
    
    def GetColor(self, name):
        return MakeColor(self.colors[name])

    def SetColor(self, name, value):
        self.colors[name] = MakeColor(value)

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

class Calendar( wx.PyControl ):
    def __init__(self, parent, id, pos=wx.DefaultPosition, size=wx.Size(200,200),
                   style= 0, validator=wx.DefaultValidator,
                   name= "calendar"):
        wx.PyControl.__init__(self, parent, id, pos, size, style | wx.WANTS_CHARS, validator, name)

        self.hasFocus = False
        # set the calendar control attributes

        self.hide_grid = False
        self.hide_title = False
        self.show_weekend = False
        self.cal_type = "NORMAL"
        self.outer_border = True
        self.num_align_horz = wx.ALIGN_CENTRE
        self.num_align_vert = wx.ALIGN_CENTRE
        self.colors = DefaultColors()
        self.set_x_mrg = 1
        self.set_y_mrg = 1
        self.set_y_end = 1

        self.select_list = []

        self.SetBackgroundColour(MakeColor(self.colors[COLOR_BACKGROUND]))
        self.Bind(wx.EVT_LEFT_DOWN, self.OnLeftEvent)
        self.Bind(wx.EVT_LEFT_DCLICK, self.OnLeftDEvent)
        self.Bind(wx.EVT_RIGHT_DOWN, self.OnRightEvent)
        self.Bind(wx.EVT_RIGHT_DCLICK, self.OnRightDEvent)
        self.Bind(wx.EVT_MIDDLE_DOWN, self.OnMiddleEvent)
        self.Bind(wx.EVT_MIDDLE_DCLICK, self.OnMiddleDEvent)
        self.Bind(wx.EVT_SET_FOCUS, self.OnSetFocus)
        self.Bind(wx.EVT_KILL_FOCUS, self.OnKillFocus)
        self.Bind(wx.EVT_KEY_DOWN, self.OnKeyDown)

        self.sel_key = None      #  last used by
        self.sel_lst = []        # highlighted selected days

        # default calendar for current month
        self.SetNow()       

        self.size = None
        self.set_day = None

        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_SIZE, self.OnSize)
        
    def AcceptsFocus(self):
        return self.IsShown() and self.IsEnabled()
    
    def GetColor(self, name):
        return MakeColor(self.colors[name])

    def SetColor(self, name, value):
        self.colors[name] = MakeColor(value)

    # control some of the main calendar attributes

    def HideTitle(self):
        self.hide_title = True

    def HideGrid(self):
        self.hide_grid = True

    # determine the calendar rectangle click area and draw a selection

    def ProcessClick(self, event):
        self.SetFocus()
        self.x, self.y = event.GetX(), event.GetY()
        self.shiftkey = event.ShiftDown()
        self.ctrlkey = event.ControlDown()
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

    def OnMiddleEvent(self, event):
        self.click = 'MIDDLE'
        self.ProcessClick(event)

    def OnMiddleDEvent(self, event):
        self.click = 'DMIDDLE'
        self.ProcessClick(event)

    def OnSetFocus(self, event):
        self.hasFocus = True
        self.DrawFocusIndicator(True)

    def OnKillFocus(self, event):
        self.hasFocus = False
        self.DrawFocusIndicator(False)

    def OnKeyDown(self, event):
        if not self.hasFocus:
            event.Skip()
            return
        
        key_code = event.GetKeyCode()
        
        if key_code == wx.WXK_TAB:
            forward = not event.ShiftDown()
            ne = wx.NavigationKeyEvent()
            ne.SetDirection(forward)
            ne.SetCurrentFocus(self)
            ne.SetEventObject(self)
            self.GetParent().GetEventHandler().ProcessEvent(ne)
            event.Skip()
            return

        delta = None

        if key_code == wx.WXK_UP:
            delta = -7 
        elif key_code == wx.WXK_DOWN:
            delta = 7 
        elif key_code == wx.WXK_LEFT:
            delta = -1 
        elif key_code == wx.WXK_RIGHT:
            delta = 1 
        elif key_code == wx.WXK_HOME:
            curDate = wx.DateTimeFromDMY(int(self.cal_days[self.sel_key]),self.month - 1,self.year)
            newDate = wx.DateTime_Now()
            ts = newDate - curDate
            delta =  ts.GetDays()

        if delta <> None:
            curDate = wx.DateTimeFromDMY(int(self.cal_days[self.sel_key]),self.month - 1,self.year)
            timeSpan = wx.TimeSpan_Days(delta)
            newDate = curDate + timeSpan

            if curDate.GetMonth() == newDate.GetMonth():
                self.set_day = newDate.GetDay()
                key = self.sel_key + delta
                self.SelectDay(key)
            else:
                self.month = newDate.GetMonth() + 1
                self.year = newDate.GetYear()
                self.set_day = newDate.GetDay()
                self.sel_key = None
                self.DoDrawing(wx.ClientDC(self))

        event.Skip()

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
        self.day = day

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
            self.day = int(self.cal_days[key])
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
        self.colors[COLOR_HEADER_FONT] = MakeColor(font_color)
        self.colors[COLOR_HEADER_BACKGROUND] = MakeColor(week_color)
        self.colors[COLOR_3D_LIGHT] = MakeColor(week_color)
        self.colors[COLOR_3D_DARK] = MakeColor(week_color)

    def SetTextAlign(self, vert, horz):
        self.num_align_horz = horz
        self.num_align_vert = vert
        
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
        #DC = wx.PaintDC(self)
        DC.BeginDrawing()

        try:
            cal = self.caldraw
        except:
            self.caldraw = CalDraw(self)
            cal = self.caldraw

        cal.hide_grid = self.hide_grid
        cal.hide_title = self.hide_title
        cal.show_weekend = self.show_weekend
        cal.cal_type = self.cal_type
        cal.outer_border = self.outer_border
        cal.num_align_horz = self.num_align_horz
        cal.num_align_vert = self.num_align_vert
        cal.colors = self.colors

        if self.size is None:
            size = self.GetClientSize()
        else:
            size = self.size

        # drawing attributes

        cal.SetSize(size)
        cal.SetCal(self.year, self.month)

        # these have to set after SetCal as SetCal would overwrite them again.
        cal.set_x_mrg = self.set_x_mrg 
        cal.set_y_mrg = self.set_y_mrg 
        cal.set_y_end = self.set_y_end 

        for val in self.select_list:
            cal.AddSelect(val[0], val[1], val[2])

        cal.DrawCal(DC, self.sel_lst)

        self.rg = cal.GetRect()
        self.cal_days = cal.GetCal()
        self.st_pos = cal.GetOffset()
        self.ymax = DC.MaxY()

        if self.set_day != None:
            self.SetDay(self.set_day)

        DC.EndDrawing()

    # draw the selection rectangle
    def DrawFocusIndicator(self, draw):
        DC = wx.ClientDC(self)
        try:
            if draw == True:
                self.caldraw.DrawFocusIndicator(DC)
            else:
                self.caldraw.DrawBorder(DC,True)
        except:
            pass

    def DrawRect(self, key, bgcolor = 'WHITE', fgcolor= 'PINK',width = 0):
        if key == None:
            return

        DC = wx.ClientDC(self)
        DC.BeginDrawing()

        brush = wx.Brush(MakeColor(bgcolor))
        DC.SetBrush(brush)

        DC.SetPen(wx.TRANSPARENT_PEN)

        rect = self.rg[key]
        DC.DrawRectangle(rect.x+1, rect.y+1, rect.width-2, rect.height-2)

        self.caldraw.DrawDayText(DC,key)

        DC.EndDrawing()
        
    def DrawRectOrg(self, key, fgcolor = 'BLACK', width = 0):
        if key == None:
            return

        DC = wx.ClientDC(self)
        DC.BeginDrawing()

        brush = wx.Brush(wx.Colour(0, 0xFF, 0x80), wx.TRANSPARENT)
        DC.SetBrush(brush)

        try:
            DC.SetPen(wx.Pen(MakeColor(fgcolor), width))
        except:
            DC.SetPen(wx.Pen(MakeColor(self.GetColor(COLOR_GRID_LINES)), width))

        rect = self.rg[key]
        DC.DrawRectangleRect(rect)

        DC.EndDrawing()

    # set the day selection

    def SetDay(self, day):
        d = day + self.st_pos - 1
        self.SelectDay(d)

    def IsDayInWeekend(self, key):
        try:
            t = Date(self.year, self.month, 1)
            
            day = self.cal_days[key]
            day = int(day) + t.day_of_week
            
            if day % 7 == 6 or day % 7 == 0:
                return True
        except:
            return False
        
    def SelectDay(self, key):
        sel_size = 1
        # clear large selection
        
        if self.sel_key != None:
            (cfont, bgcolor) = self.__GetColorsForDay(self.sel_key)
            self.DrawRect(self.sel_key, bgcolor,cfont, sel_size)

        self.DrawRect(key, self.GetColor(COLOR_HIGHLIGHT_BACKGROUND), self.GetColor(COLOR_HIGHLIGHT_FONT), sel_size)
        # store last used by
        self.sel_key = key

    def ClearDsp(self):
        self.Clear()
    def SetMargin(self, xmarg, ymarg):
        self.set_x_mrg = xmarg
        self.set_y_mrg = ymarg
        self.set_y_end = ymarg
    def __GetColorsForDay(self, key):
        cfont   = self.GetColor(COLOR_FONT)
        bgcolor = self.GetColor(COLOR_BACKGROUND)

        if self.IsDayInWeekend(key) is True and self.show_weekend is True:
            cfont   = self.GetColor(COLOR_WEEKEND_FONT)
            bgcolor = self.GetColor(COLOR_WEEKEND_BACKGROUND)

        try:
            dayIdx = int(self.cal_days[key])
            (cfont, bgcolor) = self.caldraw.cal_sel[dayIdx]
        except:
            pass

        return (cfont, bgcolor)

class CalenDlg(wx.Dialog):
    def __init__(self, parent, month=None, day = None, year=None):
        wx.Dialog.__init__(self, parent, -1, "Event Calendar", wx.DefaultPosition, (280, 360))
        self.result = None
        
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
        self.m_spin = wx.SpinButton(self, -1, (115, 20), (h*1.5, h), wx.SP_VERTICAL)
        self.m_spin.SetRange(1, 12)
        self.m_spin.SetValue(start_month)
        self.Bind(wx.EVT_SPIN, self.OnMonthSpin, self.m_spin)

        # spin button to control the year
        self.dtext = wx.TextCtrl(self, -1, str(start_year), (160, 20), (60, -1))
        h = self.dtext.GetSize().height

        self.y_spin = wx.SpinButton(self, -1, (225, 20), (h*1.5, h), wx.SP_VERTICAL)
        self.y_spin.SetRange(1980, 2010)
        self.y_spin.SetValue(start_year)

        self.Bind(wx.EVT_SPIN, self.OnYrSpin, self.y_spin)
        self.Bind(EVT_CALENDAR, self.MouseClick, self.calend)

        x_pos = 50
        y_pos = 280
        but_size = (60, 25)

        btn = wx.Button(self, wx.ID_OK, ' Ok ', (x_pos, y_pos), but_size)
        self.Bind(wx.EVT_BUTTON, self.OnOk, btn)

        btn = wx.Button(self, wx.ID_CANCEL, ' Close ', (x_pos + 120, y_pos), but_size)
        self.Bind(wx.EVT_BUTTON, self.OnCancel, btn)

    def OnOk(self, evt):
        self.result = ['None', str(self.calend.day), Month[self.calend.month], str(self.calend.year)]
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



