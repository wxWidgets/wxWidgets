#----------------------------------------------------------------------
# Name:        wxPython.lib.editor.Editor
# Purpose:     An intelligent text editor with colorization capabilities.
#
# Original
# Authors:      Dirk Holtwic, Robin Dunn
#
# New
# Authors:      Adam Feuer, Steve Howell
#
# History:
#   This code used to support a fairly complex subclass that did
#   syntax coloring and outliner collapse mode.  Adam and Steve
#   inherited the code, and added a lot of basic editor
#   functionality that had not been there before, such as cut-and-paste.
#
#
# Created:     15-Dec-1999
# RCS-ID:      $Id$
# Copyright:   (c) 1999 by Dirk Holtwick, 1999
# Licence:     wxWindows license
#----------------------------------------------------------------------
# 12/14/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o 2.5 compatability update.
#
# 12/21/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o wxEditor -> Editor
#

import  os
import  time

import  wx

import  selection
import  images

#----------------------------

def ForceBetween(min, val, max):
    if val  > max:
        return max
    if val < min:
        return min
    return val


def LineTrimmer(lineOfText):
    if len(lineOfText) == 0:
        return ""
    elif lineOfText[-1] == '\r':
        return lineOfText[:-1]
    else:
        return lineOfText

def LineSplitter(text):
    return map (LineTrimmer, text.split('\n'))


#----------------------------

class Scroller:
    def __init__(self, parent):
        self.parent = parent
        self.ow = 0
        self.oh = 0
        self.ox = 0
        self.oy = 0

    def SetScrollbars(self, fw, fh, w, h, x, y):
        if (self.ow != w or self.oh != h or self.ox != x or self.oy != y):
            self.parent.SetScrollbars(fw, fh, w, h, x, y)
            self.ow = w
            self.oh = h
            self.ox = x
            self.oy = y

#----------------------------------------------------------------------

class Editor(wx.ScrolledWindow):

    def __init__(self, parent, id,
                 pos=wx.DefaultPosition, size=wx.DefaultSize, style=0):

        wx.ScrolledWindow.__init__(self, parent, id,
                                  pos, size,
                                  style|wx.WANTS_CHARS)

        self.isDrawing = False
        
        self.InitCoords()
        self.InitFonts()
        self.SetColors()
        self.MapEvents()
        self.LoadImages()
        self.InitDoubleBuffering()
        self.InitScrolling()
        self.SelectOff()
        self.SetFocus()
        self.SetText([""])
        self.SpacesPerTab = 4

##------------------ Init stuff

    def InitCoords(self):
        self.cx = 0
        self.cy = 0
        self.oldCx = 0
        self.oldCy = 0
        self.sx = 0
        self.sy = 0
        self.sw = 0
        self.sh = 0
        self.sco_x = 0
        self.sco_y = 0

    def MapEvents(self):
        self.Bind(wx.EVT_LEFT_DOWN, self.OnLeftDown)
        self.Bind(wx.EVT_LEFT_UP, self.OnLeftUp)
        self.Bind(wx.EVT_MOTION, self.OnMotion)
        self.Bind(wx.EVT_SCROLLWIN, self.OnScroll)
        self.Bind(wx.EVT_CHAR, self.OnChar)
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.Bind(wx.EVT_SIZE, self.OnSize)
        self.Bind(wx.EVT_WINDOW_DESTROY, self.OnDestroy)
        self.Bind(wx.EVT_ERASE_BACKGROUND, self.OnEraseBackground)

##------------------- Platform-specific stuff

    def NiceFontForPlatform(self):
        if wx.Platform == "__WXMSW__":
            font = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL)
        else:
            font = wx.Font(12, wx.MODERN, wx.NORMAL, wx.NORMAL, False)
        if wx.Platform == "__WXMAC__":
            font.SetNoAntiAliasing()
        return font

    def UnixKeyHack(self, key):
        #
        # this will be obsolete when we get the new wxWindows patch
        #
        # 12/14/03 - jmg
        #
        # Which patch? I don't know if this is needed, but I don't know
        # why it's here either. Play it safe; leave it in.
        #
        if key <= 26:
            key += ord('a') - 1
        return key

##-------------------- UpdateView/Cursor code

    def OnSize(self, event):
        self.AdjustScrollbars()
        self.SetFocus()

    def SetCharDimensions(self):
        # TODO: We need a code review on this.  It appears that Linux
        # improperly reports window dimensions when the scrollbar's there.
        self.bw, self.bh = self.GetClientSize()

        if wx.Platform == "__WXMSW__":
            self.sh = self.bh / self.fh
            self.sw = (self.bw / self.fw) - 1
        else:
            self.sh = self.bh / self.fh
            if self.LinesInFile() >= self.sh:
                self.bw = self.bw - wx.SystemSettings_GetMetric(wx.SYS_VSCROLL_X)
                self.sw = (self.bw / self.fw) - 1

            self.sw = (self.bw / self.fw) - 1
            if self.CalcMaxLineLen() >= self.sw:
                self.bh = self.bh - wx.SystemSettings_GetMetric(wx.SYS_HSCROLL_Y)
                self.sh = self.bh / self.fh


    def UpdateView(self, dc = None):
        if dc is None:
            dc = wx.ClientDC(self)
        if dc.Ok():
            self.SetCharDimensions()
            self.KeepCursorOnScreen()
            self.DrawSimpleCursor(0,0, dc, True)
            self.Draw(dc)

    def OnPaint(self, event):
        dc = wx.PaintDC(self)
        if self.isDrawing:
            return
        self.isDrawing = True
        self.UpdateView(dc)
        wx.CallAfter(self.AdjustScrollbars)
        self.isDrawing = False

    def OnEraseBackground(self, evt):
        pass

##-------------------- Drawing code

    def InitFonts(self):
        dc = wx.ClientDC(self)
        self.font = self.NiceFontForPlatform()
        dc.SetFont(self.font)
        self.fw = dc.GetCharWidth()
        self.fh = dc.GetCharHeight()

    def SetColors(self):
        self.fgColor = wx.NamedColour('black')
        self.bgColor = wx.NamedColour('white')
        self.selectColor = wx.Colour(238, 220, 120)  # r, g, b = emacsOrange

    def InitDoubleBuffering(self):
        pass

    def DrawEditText(self, t, x, y, dc):
        dc.DrawText(t, x * self.fw, y * self.fh)

    def DrawLine(self, line, dc):
        if self.IsLine(line):
            l   = line
            t   = self.lines[l]
            dc.SetTextForeground(self.fgColor)
            fragments = selection.Selection(
                self.SelectBegin, self.SelectEnd,
                self.sx, self.sw, line, t)
            x = 0
            for (data, selected) in fragments:
                if selected:
                    dc.SetTextBackground(self.selectColor)
                    if x == 0 and len(data) == 0 and len(fragments) == 1:
                        data = ' '
                else:
                    dc.SetTextBackground(self.bgColor)
                self.DrawEditText(data, x, line - self.sy, dc)
                x += len(data)

    def Draw(self, odc=None):
        if not odc:
            odc = wx.ClientDC(self)

        bmp = wx.EmptyBitmap(max(1,self.bw), max(1,self.bh))
        dc = wx.BufferedDC(odc, bmp)
        if dc.Ok():
            dc.SetFont(self.font)
            dc.SetBackgroundMode(wx.SOLID)
            dc.SetTextBackground(self.bgColor)
            dc.SetTextForeground(self.fgColor)
            dc.Clear()
            for line in range(self.sy, self.sy + self.sh):
                self.DrawLine(line, dc)
            if len(self.lines) < self.sh + self.sy:
                self.DrawEofMarker(dc)
            self.DrawCursor(dc)

##------------------ eofMarker stuff

    def LoadImages(self):
        self.eofMarker = images.GetBitmap(images.EofImageData)

    def DrawEofMarker(self,dc):
        x = 0
        y = (len(self.lines) - self.sy) * self.fh
        hasTransparency = 1
        dc.DrawBitmap(self.eofMarker, x, y, hasTransparency)

##------------------ cursor-related functions

    def DrawCursor(self, dc = None):
        if not dc:
            dc = wx.ClientDC(self)

        if (self.LinesInFile())<self.cy: #-1 ?
            self.cy = self.LinesInFile()-1
        s = self.lines[self.cy]

        x = self.cx - self.sx
        y = self.cy - self.sy
        self.DrawSimpleCursor(x, y, dc)


    def DrawSimpleCursor(self, xp, yp, dc = None, old=False):
        if not dc:
            dc = wx.ClientDC(self)

        if old:
            xp = self.sco_x
            yp = self.sco_y

        szx = self.fw
        szy = self.fh
        x = xp * szx
        y = yp * szy
        dc.Blit(x,y, szx,szy, dc, x,y, wx.SRC_INVERT)
        self.sco_x = xp
        self.sco_y = yp

##-------- Enforcing screen boundaries, cursor movement

    def CalcMaxLineLen(self):
        """get length of longest line on screen"""
        maxlen = 0
        for line in self.lines[self.sy:self.sy+self.sh]:
            if len(line) >maxlen:
                maxlen = len(line)
        return maxlen

    def KeepCursorOnScreen(self):
        self.sy = ForceBetween(max(0, self.cy-self.sh), self.sy, self.cy)
        self.sx = ForceBetween(max(0, self.cx-self.sw), self.sx, self.cx)
        self.AdjustScrollbars()

    def HorizBoundaries(self):
        self.SetCharDimensions()
        maxLineLen = self.CalcMaxLineLen()
        self.sx = ForceBetween(0, self.sx, max(self.sw, maxLineLen - self.sw + 1))
        self.cx = ForceBetween(self.sx, self.cx, self.sx + self.sw - 1)

    def VertBoundaries(self):
        self.SetCharDimensions()
        self.sy = ForceBetween(0, self.sy, max(self.sh, self.LinesInFile() - self.sh + 1))
        self.cy = ForceBetween(self.sy, self.cy, self.sy + self.sh - 1)

    def cVert(self, num):
        self.cy = self.cy + num
        self.cy = ForceBetween(0, self.cy, self.LinesInFile() - 1)
        self.sy = ForceBetween(self.cy - self.sh + 1, self.sy, self.cy)
        self.cx = min(self.cx, self.CurrentLineLength())

    def cHoriz(self, num):
        self.cx = self.cx + num
        self.cx = ForceBetween(0, self.cx, self.CurrentLineLength())
        self.sx = ForceBetween(self.cx - self.sw + 1, self.sx, self.cx)

    def AboveScreen(self, row):
        return row < self.sy

    def BelowScreen(self, row):
        return row >= self.sy + self.sh

    def LeftOfScreen(self, col):
        return col < self.sx

    def RightOfScreen(self, col):
        return col >= self.sx + self.sw

##----------------- data structure helper functions

    def GetText(self):
        return self.lines

    def SetText(self, lines):
        self.InitCoords()
        self.lines = lines
        self.UnTouchBuffer()
        self.SelectOff()
        self.AdjustScrollbars()
        self.UpdateView(None)

    def IsLine(self, lineNum):
        return (0<=lineNum) and (lineNum<self.LinesInFile())

    def GetTextLine(self, lineNum):
        if self.IsLine(lineNum):
            return self.lines[lineNum]
        return ""

    def SetTextLine(self, lineNum, text):
        if self.IsLine(lineNum):
            self.lines[lineNum] = text

    def CurrentLineLength(self):
        return len(self.lines[self.cy])

    def LinesInFile(self):
        return len(self.lines)

    def UnTouchBuffer(self):
        self.bufferTouched = False

    def BufferWasTouched(self):
        return self.bufferTouched

    def TouchBuffer(self):
        self.bufferTouched = True


##-------------------------- Mouse scroll timing functions

    def InitScrolling(self):
        # we don't rely on the windows system to scroll for us; we just
        # redraw the screen manually every time
        self.EnableScrolling(False, False)
        self.nextScrollTime = 0
        self.SCROLLDELAY = 0.050 # seconds
        self.scrollTimer = wx.Timer(self)
        self.scroller = Scroller(self)

    def CanScroll(self):
       if time.time() >  self.nextScrollTime:
           self.nextScrollTime = time.time() + self.SCROLLDELAY
           return True
       else:
           return False

    def SetScrollTimer(self):
        oneShot = True
        self.scrollTimer.Start(1000*self.SCROLLDELAY/2, oneShot)
        self.Bind(wx.EVT_TIMER, self.OnTimer)

    def OnTimer(self, event):
        screenX, screenY = wx.GetMousePosition()
        x, y = self.ScreenToClientXY(screenX, screenY)
        self.MouseToRow(y)
        self.MouseToCol(x)
        self.SelectUpdate()

##-------------------------- Mouse off screen functions

    def HandleAboveScreen(self, row):
        self.SetScrollTimer()
        if self.CanScroll():
            row = self.sy - 1
            row = max(0, row)
            self.cy = row

    def HandleBelowScreen(self, row):
        self.SetScrollTimer()
        if self.CanScroll():
            row = self.sy + self.sh
            row  = min(row, self.LinesInFile() - 1)
            self.cy = row

    def HandleLeftOfScreen(self, col):
        self.SetScrollTimer()
        if self.CanScroll():
            col = self.sx - 1
            col = max(0,col)
            self.cx = col

    def HandleRightOfScreen(self, col):
        self.SetScrollTimer()
        if self.CanScroll():
            col = self.sx + self.sw
            col = min(col, self.CurrentLineLength())
            self.cx = col

##------------------------ mousing functions

    def MouseToRow(self, mouseY):
        row  = self.sy + (mouseY/ self.fh)
        if self.AboveScreen(row):
            self.HandleAboveScreen(row)
        elif self.BelowScreen(row):
            self.HandleBelowScreen(row)
        else:
            self.cy  = min(row, self.LinesInFile() - 1)

    def MouseToCol(self, mouseX):
        col = self.sx + (mouseX / self.fw)
        if self.LeftOfScreen(col):
            self.HandleLeftOfScreen(col)
        elif self.RightOfScreen(col):
            self.HandleRightOfScreen(col)
        else:
            self.cx = min(col, self.CurrentLineLength())

    def MouseToCursor(self, event):
        self.MouseToRow(event.GetY())
        self.MouseToCol(event.GetX())

    def OnMotion(self, event):
        if event.LeftIsDown() and self.HasCapture():
            self.Selecting = True
            self.MouseToCursor(event)
            self.SelectUpdate()

    def OnLeftDown(self, event):
        self.MouseToCursor(event)
        self.SelectBegin = (self.cy, self.cx)
        self.SelectEnd = None
        self.UpdateView()
        self.CaptureMouse()
        self.SetFocus()

    def OnLeftUp(self, event):
        if not self.HasCapture():
            return

        if self.SelectEnd is None:
            self.OnClick()
        else:
            self.Selecting = False
            self.SelectNotify(False, self.SelectBegin, self.SelectEnd)

        self.ReleaseMouse()
        self.scrollTimer.Stop()


#------------------------- Scrolling

    def HorizScroll(self, event, eventType):
        maxLineLen = self.CalcMaxLineLen()

        if eventType == wx.EVT_SCROLLWIN_LINEUP:
            self.sx -= 1
        elif eventType == wx.EVT_SCROLLWIN_LINEDOWN:
            self.sx += 1
        elif eventType == wx.EVT_SCROLLWIN_PAGEUP:
            self.sx -= self.sw
        elif eventType == wx.EVT_SCROLLWIN_PAGEDOWN:
            self.sx += self.sw
        elif eventType == wx.EVT_SCROLLWIN_TOP:
            self.sx = self.cx = 0
        elif eventType == wx.EVT_SCROLLWIN_BOTTOM:
            self.sx = maxLineLen - self.sw
            self.cx = maxLineLen
        else:
            self.sx = event.GetPosition()

        self.HorizBoundaries()

    def VertScroll(self, event, eventType):
        if   eventType == wx.EVT_SCROLLWIN_LINEUP:
            self.sy -= 1
        elif eventType == wx.EVT_SCROLLWIN_LINEDOWN:
            self.sy += 1
        elif eventType == wx.EVT_SCROLLWIN_PAGEUP:
            self.sy -= self.sh
        elif eventType == wx.EVT_SCROLLWIN_PAGEDOWN:
            self.sy += self.sh
        elif eventType == wx.EVT_SCROLLWIN_TOP:
            self.sy = self.cy = 0
        elif eventType == wx.EVT_SCROLLWIN_BOTTOM:
            self.sy = self.LinesInFile() - self.sh
            self.cy = self.LinesInFile()
        else:
            self.sy = event.GetPosition()

        self.VertBoundaries()

    def OnScroll(self, event):
        dir = event.GetOrientation()
        eventType = event.GetEventType()
        if dir == wx.HORIZONTAL:
            self.HorizScroll(event, eventType)
        else:
            self.VertScroll(event, eventType)
        self.UpdateView()


    def AdjustScrollbars(self):
        if self:
            for i in range(2):
                self.SetCharDimensions()
                self.scroller.SetScrollbars(
                    self.fw, self.fh,
                    self.CalcMaxLineLen()+3, max(self.LinesInFile()+1, self.sh),
                    self.sx, self.sy)

#------------ backspace, delete, return

    def BreakLine(self, event):
        if self.IsLine(self.cy):
            t = self.lines[self.cy]
            self.lines = self.lines[:self.cy] + [t[:self.cx],t[self.cx:]] + self.lines[self.cy+1:]
            self.cVert(1)
            self.cx = 0
            self.TouchBuffer()

    def InsertChar(self,char):
        if self.IsLine(self.cy):
            t = self.lines[self.cy]
            t = t[:self.cx] + char + t[self.cx:]
            self.SetTextLine(self.cy, t)
            self.cHoriz(1)
            self.TouchBuffer()

    def JoinLines(self):
        t1 = self.lines[self.cy]
        t2 = self.lines[self.cy+1]
        self.cx = len(t1)
        self.lines = self.lines[:self.cy] + [t1 + t2] + self.lines[self.cy+2:]
        self.TouchBuffer()


    def DeleteChar(self,x,y,oldtext):
        newtext = oldtext[:x] + oldtext[x+1:]
        self.SetTextLine(y, newtext)
        self.TouchBuffer()


    def BackSpace(self, event):
        t = self.GetTextLine(self.cy)
        if self.cx>0:
            self.DeleteChar(self.cx-1,self.cy,t)
            self.cHoriz(-1)
            self.TouchBuffer()
        elif self.cx == 0:
            if self.cy > 0:
                self.cy -= 1
                self.JoinLines()
                self.TouchBuffer()
            else:
                wx.Bell()

    def Delete(self, event):
        t = self.GetTextLine(self.cy)
        if self.cx<len(t):
            self.DeleteChar(self.cx,self.cy,t)
            self.TouchBuffer()
        else:
            if self.cy < len(self.lines) - 1:
                self.JoinLines()
                self.TouchBuffer()

    def Escape(self, event):
        self.SelectOff()

    def TabKey(self, event):
        numSpaces = self.SpacesPerTab - (self.cx % self.SpacesPerTab)
        self.SingleLineInsert(' ' * numSpaces)

##----------- selection routines

    def SelectUpdate(self):
        self.SelectEnd = (self.cy, self.cx)
        self.SelectNotify(self.Selecting, self.SelectBegin, self.SelectEnd)
        self.UpdateView()

    def NormalizedSelect(self):
        (begin, end) = (self.SelectBegin, self.SelectEnd)
        (bRow, bCol) = begin
        (eRow, eCol) = end
        if (bRow < eRow):
            return (begin, end)
        elif (eRow < bRow):
            return (end, begin)
        else:
            if (bCol < eCol):
                return (begin, end)
            else:
                return (end, begin)

    def FindSelection(self):
        if self.SelectEnd is None or self.SelectBegin is None:
            wx.Bell()
            return None
        (begin, end) =  self.NormalizedSelect()
        (bRow, bCol) = begin
        (eRow, eCol) = end
        return (bRow, bCol, eRow, eCol)

    def SelectOff(self):
        self.SelectBegin = None
        self.SelectEnd = None
        self.Selecting = False
        self.SelectNotify(False,None,None)

    def CopySelection(self, event):
        selection = self.FindSelection()
        if selection is None:
            return
        (bRow, bCol, eRow, eCol) = selection

        if bRow == eRow:
            self.SingleLineCopy(bRow, bCol, eCol)
        else:
            self.MultipleLineCopy(bRow, bCol, eRow, eCol)

    def OnCopySelection(self, event):
        self.CopySelection(event)
        self.SelectOff()

    def CopyToClipboard(self, linesOfText):
        do = wx.TextDataObject()
        do.SetText(os.linesep.join(linesOfText))
        wx.TheClipboard.Open()
        wx.TheClipboard.SetData(do)
        wx.TheClipboard.Close()

    def SingleLineCopy(self, Row, bCol, eCol):
        Line = self.GetTextLine(Row)
        self.CopyToClipboard([Line[bCol:eCol]])

    def MultipleLineCopy(self, bRow, bCol, eRow, eCol):
        bLine = self.GetTextLine(bRow)[bCol:]
        eLine = self.GetTextLine(eRow)[:eCol]
        self.CopyToClipboard([bLine] + [l for l in self.lines[bRow + 1:eRow]] + [eLine])

    def OnDeleteSelection(self, event):
        selection = self.FindSelection()
        if selection is None:
            return
        (bRow, bCol, eRow, eCol) = selection

        if bRow == eRow:
            self.SingleLineDelete(bRow, bCol, eCol)
        else:
            self.MultipleLineDelete(bRow, bCol, eRow, eCol)

        self.TouchBuffer()

        self.cy = bRow
        self.cx = bCol
        self.SelectOff()
        self.UpdateView()


    def SingleLineDelete(self, Row, bCol, eCol):
        ModLine = self.GetTextLine(Row)
        ModLine = ModLine[:bCol] + ModLine[eCol:]
        self.SetTextLine(Row,ModLine)

    def MultipleLineDelete(self, bRow, bCol, eRow, eCol):
        bLine = self.GetTextLine(bRow)
        eLine = self.GetTextLine(eRow)
        ModLine = bLine[:bCol] + eLine[eCol:]
        self.lines[bRow:eRow + 1] = [ModLine]

    def OnPaste(self, event):
        do = wx.TextDataObject()
        wx.TheClipboard.Open()
        success = wx.TheClipboard.GetData(do)
        wx.TheClipboard.Close()
        if success:
            pastedLines = LineSplitter(do.GetText())
        else:
            wx.Bell()
            return
        if len(pastedLines) == 0:
            wx.Bell()
            return
        elif len(pastedLines) == 1:
            self.SingleLineInsert(pastedLines[0])
        else:
            self.MultipleLinePaste(pastedLines)

    def SingleLineInsert(self, newText):
        ModLine = self.GetTextLine(self.cy)
        ModLine = ModLine[:self.cx] + newText + ModLine[self.cx:]
        self.SetTextLine(self.cy, ModLine)
        self.cHoriz(len(newText))
        self.TouchBuffer()
        self.UpdateView()

    def MultipleLinePaste(self, pastedLines):
        FirstLine = LastLine = self.GetTextLine(self.cy)
        FirstLine = FirstLine[:self.cx] + pastedLines[0]
        LastLine = pastedLines[-1] + LastLine[self.cx:]

        NewSlice = [FirstLine]
        NewSlice += [l for l in pastedLines[1:-1]]
        NewSlice += [LastLine]
        self.lines[self.cy:self.cy + 1] = NewSlice

        self.cy = self.cy + len(pastedLines)-1
        self.cx = len(pastedLines[-1])
        self.TouchBuffer()
        self.UpdateView()

    def OnCutSelection(self,event):
        self.CopySelection(event)
        self.OnDeleteSelection(event)

#-------------- Keyboard movement implementations

    def MoveDown(self, event):
        self.cVert(+1)

    def MoveUp(self, event):
        self.cVert(-1)

    def MoveLeft(self, event):
        if self.cx == 0:
            if self.cy == 0:
                wx.Bell()
            else:
                self.cVert(-1)
                self.cx = self.CurrentLineLength()
        else:
            self.cx -= 1

    def MoveRight(self, event):
        linelen = self.CurrentLineLength()
        if self.cx == linelen:
            if self.cy == len(self.lines) - 1:
                wx.Bell()
            else:
                self.cx = 0
                self.cVert(1)
        else:
            self.cx += 1


    def MovePageDown(self, event):
        self.cVert(self.sh)

    def MovePageUp(self, event):
        self.cVert(-self.sh)

    def MoveHome(self, event):
        self.cx = 0

    def MoveEnd(self, event):
        self.cx = self.CurrentLineLength()

    def MoveStartOfFile(self, event):
        self.cy = 0
        self.cx = 0

    def MoveEndOfFile(self, event):
        self.cy = len(self.lines) - 1
        self.cx = self.CurrentLineLength()

#-------------- Key handler mapping tables

    def SetMoveSpecialFuncs(self, action):
        action[wx.WXK_DOWN]  = self.MoveDown
        action[wx.WXK_UP]    = self.MoveUp
        action[wx.WXK_LEFT]  = self.MoveLeft
        action[wx.WXK_RIGHT] = self.MoveRight
        action[wx.WXK_NEXT]  = self.MovePageDown
        action[wx.WXK_PRIOR] = self.MovePageUp
        action[wx.WXK_HOME]  = self.MoveHome
        action[wx.WXK_END]   = self.MoveEnd

    def SetMoveSpecialControlFuncs(self, action):
        action[wx.WXK_HOME] = self.MoveStartOfFile
        action[wx.WXK_END]  = self.MoveEndOfFile

    def SetAltFuncs(self, action):
        # subclass implements
        pass

    def SetControlFuncs(self, action):
        action['c'] = self.OnCopySelection
        action['d'] = self.OnDeleteSelection
        action['v'] = self.OnPaste
        action['x'] = self.OnCutSelection

    def SetSpecialControlFuncs(self, action):
        action[wx.WXK_INSERT] = self.OnCopySelection

    def SetShiftFuncs(self, action):
        action[wx.WXK_DELETE] = self.OnCutSelection
        action[wx.WXK_INSERT] = self.OnPaste

    def SetSpecialFuncs(self, action):
        action[wx.WXK_BACK]   = self.BackSpace
        action[wx.WXK_DELETE] = self.Delete
        action[wx.WXK_RETURN] = self.BreakLine
        action[wx.WXK_ESCAPE] = self.Escape
        action[wx.WXK_TAB]    = self.TabKey

##-------------- Logic for key handlers


    def Move(self, keySettingFunction, key, event):
        action = {}
        keySettingFunction(action)

        if not action.has_key(key):
            return False

        if event.ShiftDown():
            if not self.Selecting:
                self.Selecting = True
                self.SelectBegin = (self.cy, self.cx)
            action[key](event)
            self.SelectEnd = (self.cy, self.cx)
        else:
            action[key](event)
            if self.Selecting:
                self.Selecting = False

        self.SelectNotify(self.Selecting, self.SelectBegin, self.SelectEnd)
        self.UpdateView()
        return True

    def MoveSpecialKey(self, event, key):
        return self.Move(self.SetMoveSpecialFuncs, key, event)

    def MoveSpecialControlKey(self, event, key):
        if not event.ControlDown():
            return False
        return self.Move(self.SetMoveSpecialControlFuncs, key, event)

    def Dispatch(self, keySettingFunction, key, event):
        action = {}
        keySettingFunction(action)
        if action.has_key(key):
            action[key](event)
            self.UpdateView()
            return True
        return False

    def ModifierKey(self, key, event, modifierKeyDown, MappingFunc):
        if not modifierKeyDown:
            return False

        key = self.UnixKeyHack(key)
        try:
            key = chr(key)
        except:
            return False
        if not self.Dispatch(MappingFunc, key, event):
            wx.Bell()
        return True

    def ControlKey(self, event, key):
        return self.ModifierKey(key, event, event.ControlDown(), self.SetControlFuncs)

    def AltKey(self, event, key):
        return self.ModifierKey(key, event, event.AltDown(), self.SetAltFuncs)

    def SpecialControlKey(self, event, key):
        if not event.ControlDown():
            return False
        if not self.Dispatch(self.SetSpecialControlFuncs, key, event):
            wx.Bell()
        return True

    def ShiftKey(self, event, key):
        if not event.ShiftDown():
            return False
        return self.Dispatch(self.SetShiftFuncs, key, event)

    def NormalChar(self, event, key):
        self.SelectOff()

        # regular ascii
        if not self.Dispatch(self.SetSpecialFuncs, key, event):
            if (key>31) and (key<256):
                self.InsertChar(chr(key))
            else:
                wx.Bell()
                return
        self.UpdateView()
        self.AdjustScrollbars()

    def OnChar(self, event):
        key = event.GetKeyCode()
        filters = [self.AltKey,
                   self.MoveSpecialControlKey,
                   self.ControlKey,
                   self.SpecialControlKey,
                   self.MoveSpecialKey,
                   self.ShiftKey,
                   self.NormalChar]
        for filter in filters:
            if filter(event,key):
                break
        return 0

#----------------------- Eliminate memory leaks

    def OnDestroy(self, event):
        self.mdc = None
        self.odc = None
        self.bgColor = None
        self.fgColor = None
        self.font = None
        self.selectColor = None
        self.scrollTimer = None
        self.eofMarker = None

#--------------------  Abstract methods for subclasses

    def OnClick(self):
        pass

    def SelectNotify(self, Selecting, SelectionBegin, SelectionEnd):
        pass

