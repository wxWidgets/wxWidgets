#----------------------------------------------------------------------
# Name:        wxPython.lib.editor.wxEditor
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

import os, time

from wxPython.wx import *

import selection
import images

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

class wxEditor(wxScrolledWindow):

    def __init__(self, parent, id,
                 pos=wxDefaultPosition, size=wxDefaultSize, style=0):

        wxScrolledWindow.__init__(self, parent, id,
                                  pos, size,
                                  style|wxWANTS_CHARS)

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
        EVT_LEFT_DOWN(self, self.OnLeftDown)
        EVT_LEFT_UP(self, self.OnLeftUp)
        EVT_MOTION(self, self.OnMotion)
        EVT_SCROLLWIN(self, self.OnScroll)
        EVT_CHAR(self, self.OnChar)
        EVT_PAINT(self, self.OnPaint)
        EVT_SIZE(self, self.OnSize)
        EVT_WINDOW_DESTROY(self, self.OnDestroy)
        EVT_ERASE_BACKGROUND(self, self.OnEraseBackground)

##------------------- Platform-specific stuff

    def NiceFontForPlatform(self):
        if wxPlatform == "__WXMSW__":
            return wxFont(10, wxMODERN, wxNORMAL, wxNORMAL)
        else:
            return wxFont(12, wxMODERN, wxNORMAL, wxNORMAL, False)

    def UnixKeyHack(self, key):
        # this will be obsolete when we get the new wxWindows patch
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
        self.bw, self.bh = self.GetClientSizeTuple()

        if wxPlatform == "__WXMSW__":
            self.sh = self.bh / self.fh
            self.sw = (self.bw / self.fw) - 1
        else:
            self.sh = self.bh / self.fh
            if self.LinesInFile() >= self.sh:
                self.bw = self.bw - wxSystemSettings_GetSystemMetric(wxSYS_VSCROLL_X)
                self.sw = (self.bw / self.fw) - 1

            self.sw = (self.bw / self.fw) - 1
            if self.CalcMaxLineLen() >= self.sw:
                self.bh = self.bh - wxSystemSettings_GetSystemMetric(wxSYS_HSCROLL_Y)
                self.sh = self.bh / self.fh


    def UpdateView(self, dc = None):
        if dc is None:
            dc = wxClientDC(self)
        if dc.Ok():
            self.SetCharDimensions()
            self.KeepCursorOnScreen()
            self.DrawSimpleCursor(0,0,dc, True)
            self.Draw(dc)

    def OnPaint(self, event):
        dc = wxPaintDC(self)
        self.UpdateView(dc)
        self.AdjustScrollbars()

    def OnEraseBackground(self, evt):
        pass

##-------------------- Drawing code

    def InitFonts(self):
        dc = wxClientDC(self)
        self.font = self.NiceFontForPlatform()
        dc.SetFont(self.font)
        self.fw = dc.GetCharWidth()
        self.fh = dc.GetCharHeight()

    def SetColors(self):
        self.fgColor = wxNamedColour('black')
        self.bgColor = wxNamedColour('white')
        self.selectColor = wxColour(238, 220, 120)  # r, g, b = emacsOrange

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
            odc = wxClientDC(self)

        bmp = wxEmptyBitmap(max(1,self.bw), max(1,self.bh))
        dc = wxBufferedDC(odc, bmp)
        if dc.Ok():
            dc.SetFont(self.font)
            dc.SetBackgroundMode(wxSOLID)
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
            dc = wxClientDC(self)

        if (self.LinesInFile())<self.cy: #-1 ?
            self.cy = self.LinesInFile()-1
        s = self.lines[self.cy]

        x = self.cx - self.sx
        y = self.cy - self.sy
        self.DrawSimpleCursor(x, y, dc)


    def DrawSimpleCursor(self, xp, yp, dc = None, old=False):
        if not dc:
            dc = wxClientDC(self)

        if old:
            xp = self.sco_x
            yp = self.sco_y

        szx = self.fw
        szy = self.fh
        x = xp * szx
        y = yp * szy
        dc.Blit(x,y,szx,szy,dc,x,y,wxSRC_INVERT)
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
        self.scrollTimer = wxTimer(self)
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
        EVT_TIMER(self, -1, self.OnTimer)

    def OnTimer(self, event):
        screenX, screenY = wxGetMousePosition()
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

        if eventType == wxEVT_SCROLLWIN_LINEUP:
            self.sx -= 1
        elif eventType == wxEVT_SCROLLWIN_LINEDOWN:
            self.sx += 1
        elif eventType == wxEVT_SCROLLWIN_PAGEUP:
            self.sx -= self.sw
        elif eventType == wxEVT_SCROLLWIN_PAGEDOWN:
            self.sx += self.sw
        elif eventType == wxEVT_SCROLLWIN_TOP:
            self.sx = self.cx = 0
        elif eventType == wxEVT_SCROLLWIN_BOTTOM:
            self.sx = maxLineLen - self.sw
            self.cx = maxLineLen
        else:
            self.sx = event.GetPosition()

        self.HorizBoundaries()

    def VertScroll(self, event, eventType):
        if   eventType == wxEVT_SCROLLWIN_LINEUP:
            self.sy -= 1
        elif eventType == wxEVT_SCROLLWIN_LINEDOWN:
            self.sy += 1
        elif eventType == wxEVT_SCROLLWIN_PAGEUP:
            self.sy -= self.sh
        elif eventType == wxEVT_SCROLLWIN_PAGEDOWN:
            self.sy += self.sh
        elif eventType == wxEVT_SCROLLWIN_TOP:
            self.sy = self.cy = 0
        elif eventType == wxEVT_SCROLLWIN_BOTTOM:
            self.sy = self.LinesInFile() - self.sh
            self.cy = self.LinesInFile()
        else:
            self.sy = event.GetPosition()

        self.VertBoundaries()

    def OnScroll(self, event):
        dir = event.GetOrientation()
        eventType = event.GetEventType()
        if dir == wxHORIZONTAL:
            self.HorizScroll(event, eventType)
        else:
            self.VertScroll(event, eventType)
        self.UpdateView()


    def AdjustScrollbars(self):
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
                wxBell()

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
            wxBell()
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
        do = wxTextDataObject()
        do.SetText(os.linesep.join(linesOfText))
        wxTheClipboard.Open()
        wxTheClipboard.SetData(do)
        wxTheClipboard.Close()

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
        do = wxTextDataObject()
        wxTheClipboard.Open()
        success = wxTheClipboard.GetData(do)
        wxTheClipboard.Close()
        if success:
            pastedLines = LineSplitter(do.GetText())
        else:
            wxBell()
            return
        if len(pastedLines) == 0:
            wxBell()
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
                wxBell()
            else:
                self.cVert(-1)
                self.cx = self.CurrentLineLength()
        else:
            self.cx -= 1

    def MoveRight(self, event):
        linelen = self.CurrentLineLength()
        if self.cx == linelen:
            if self.cy == len(self.lines) - 1:
                wxBell()
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
        action[WXK_DOWN]  = self.MoveDown
        action[WXK_UP]    = self.MoveUp
        action[WXK_LEFT]  = self.MoveLeft
        action[WXK_RIGHT] = self.MoveRight
        action[WXK_NEXT]  = self.MovePageDown
        action[WXK_PRIOR] = self.MovePageUp
        action[WXK_HOME]  = self.MoveHome
        action[WXK_END]   = self.MoveEnd

    def SetMoveSpecialControlFuncs(self, action):
        action[WXK_HOME] = self.MoveStartOfFile
        action[WXK_END]  = self.MoveEndOfFile

    def SetAltFuncs(self, action):
        # subclass implements
        pass

    def SetControlFuncs(self, action):
        action['c'] = self.OnCopySelection
        action['d'] = self.OnDeleteSelection
        action['v'] = self.OnPaste
        action['x'] = self.OnCutSelection

    def SetSpecialControlFuncs(self, action):
        action[WXK_INSERT] = self.OnCopySelection

    def SetShiftFuncs(self, action):
        action[WXK_DELETE] = self.OnCutSelection
        action[WXK_INSERT] = self.OnPaste

    def SetSpecialFuncs(self, action):
        action[WXK_BACK]   = self.BackSpace
        action[WXK_DELETE] = self.Delete
        action[WXK_RETURN] = self.BreakLine
        action[WXK_ESCAPE] = self.Escape
        action[WXK_TAB]    = self.TabKey

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
            wxBell()
        return True

    def ControlKey(self, event, key):
        return self.ModifierKey(key, event, event.ControlDown(), self.SetControlFuncs)

    def AltKey(self, event, key):
        return self.ModifierKey(key, event, event.AltDown(), self.SetAltFuncs)

    def SpecialControlKey(self, event, key):
        if not event.ControlDown():
            return False
        if not self.Dispatch(self.SetSpecialControlFuncs, key, event):
            wxBell()
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
                wxBell()
                return
        self.UpdateView()
        self.AdjustScrollbars()

    def OnChar(self, event):
        key = event.KeyCode()
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

