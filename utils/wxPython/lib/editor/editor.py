#----------------------------------------------------------------------
# Name:        wxPython.lib.editor.wxEditor
# Purpose:     An intelligent text editor with colorization capabilities.
#
# Author:      Dirk Holtwic, Robin Dunn
#
# Created:     15-Dec-1999
# RCS-ID:      $Id$
# Copyright:   (c) 1999 by Dirk Holtwick, 1999
# Licence:     wxWindows license
#----------------------------------------------------------------------


# PLEASE NOTE:  This is experimental code.  It needs an overhall in the
#               drawing and update code, and there is occasionally a
#               mysteriously disappearing line...
#
#               I am working on a StyledTextEditor that will likely
#               render this editor obsolete...  But this one is at
#               least somewhat functional now while the other is still
#               vapor.
#
#               - Robin


from wxPython.wx import *
from string import *
from keyword import *
from regsub import *
from tokenizer import *

#---------------------------------------------------------------------------


class Line:
    def __init__(self, text=""):
        self.text       = text      # the string itself
        self.syntax     = []        # the colors of the line
        self.editable   = true      # edit?
        self.visible    = 0         # will be incremented if not
        self.indent     = 0         # not used yet

#----------------------------------------------------------------------

class wxEditor(wxScrolledWindow):

    def __init__(self, parent, id=-1):
    ###############################################################
        """
        Alles hat einen Anfang
        """

        wxScrolledWindow.__init__(self, parent, id,
                                  wxDefaultPosition, wxSize(500,400),
                                  wxSUNKEN_BORDER|wxWANTS_CHARS)

        # the syntax informations, if they don't exist,
        # all syntax stuff will be ignored

        # cursor pos
        self.cx = 0
        self.cy = 0

        # the lines that are visible
        self.lines = []
        self.line = 0
        self.len = 0

        self.ocy = 0

        # border pos
        #self.bx = 0
        #self.by = 0

        # screen
        self.sx = 0
        self.sy = 0
        self.sw = 0
        self.sh = 0
        self.osx= 0
        self.osy= 0

        # font
        dc = wxClientDC(self)

        if wxPlatform == "__WXMSW__":
            self.font = wxFont(10, wxMODERN, wxNORMAL, wxNORMAL)
        else:
            self.font = wxFont(12, wxMODERN, wxNORMAL, wxNORMAL, false)
        dc.SetFont(self.font)

        # font weight, height
        self.fw = dc.GetCharWidth()
        self.fh = dc.GetCharHeight()

        #  back, for colour
        self.bcol = wxNamedColour('white')
        self.fcol = wxNamedColour('black')

        self.cfcol = wxNamedColour('black')
        self.cbcol = wxNamedColour('red')

        # nicht edierbare zeile (hintergrund)
        self.nedcol = wxNamedColour('grey')

        self.SetBackgroundColour(self.bcol)
        #dc.SetForegroundColour(self.fcol)

        # events
        EVT_LEFT_DOWN(self, self.OnMouseClick)
        EVT_RIGHT_DOWN(self, self.OnMouseClick)
        EVT_SCROLLWIN(self, self.OnScroll)

        self.o_cx = self.cx
        self.o_cy = self.cy
        self.o_sx = self.sx
        self.o_sy = self.sy
        self.o_line = self.line
        self.sco_x = 0
        self.sco_y = 0

        self.tabsize = 4

        self.update = true
        self.in_scroll =FALSE
        self.inUpdate = FALSE


        bw,bh = self.GetSizeTuple()
        # double buffering
        self.mdc = wxMemoryDC()
        self.mdc.SelectObject(wxEmptyBitmap(bw,bh))
        # disable physical scrolling because invisible parts are not drawn
        self.EnableScrolling(FALSE, FALSE)

        # the ordinary text as it is
        self.SetText()
        self.SetFocus()


#---------------------------------------------------------------------------

    def CalcLines(self):
    ###############################################################
        self.lines = []
        x =maxlen =0
        for line in self.text:
            if line.visible==0:
                self.lines.append(x)
            else:
                if len(line.text) >maxlen:
                    maxlen =len(line.text)
            x = x + 1
        self.len = len(self.lines)
        self.max_linelength =maxlen


    def SetFontTab(self, fonttab):
    ###############################################################
        """ Fonttabelle zum schnellen Zugriff """
        self.ftab = fonttab


    def SetText(self, text = [""]):
    ###############################################################
        """ Text mittels Liste setzen """
        self.cx = 0
        self.cy = 0
        self.text = []

        for t in text:
            self.text.append(Line(t))

        for l in range(0,len(text)-1):
            #self.UpdateSyntax(l)
            self.OnUpdateHighlight(l)

        self.OnInit()

        self.update = true
        self.UpdateView(None, true)


    # show new text
    def GetText(self):
    ###############################################################
        """ Der gesamte Text als Liste """
        text = []
        for line in self.text:
            text.append(line.text)
        return text


    def IsEmpty(self):
    ###############################################################
        """see if at least one text line is not empty"""
        for line in self.text:
            if line.text: return 0
        return 1


    def IsLine(self, line):
    ###############################################################
        """ Schauen, ob alles im gr¸nen Bereich ist """
        return (line>=0) and (line<self.len)


    def IsEditable(self, line):
    ###############################################################
        return self.text[self.GetLine(line)].editable


    def GetLine(self, line):
    ###############################################################
        return self.lines[line]


    def GetTextLine(self, line):
    ###############################################################
        """ Text holen """
        if self.IsLine(line):
            return self.text[self.GetLine(line)].text
        return ""


    def SetTextLine(self, line, text):
    ###############################################################
        """ Nur den Text ‰ndern """
        if self.IsLine(line):
            l = self.GetLine(line)
            self.text[l].text = text
            #self.UpdateSyntax(l)
            self.OnUpdateHighlight(l)
            self.update = true


#---------------------------------------------------------------------------

    def OnMouseClick(self, event):
    ###############################################################
        """
        Wenn es Click gemacht hat => Cursor setzen
        """
        self.SetFocus()

        self.cy = self.sy + (event.GetY() / self.fh)
        if self.cy >= self.len: self.cy =max(self.len -1, 0)
        linelen =len(self.text[self.GetLine(self.cy)].text)
        self.cx = self.sx + (event.GetX() / self.fw)
        # allow positioning right behind the last character
        if self.cx > linelen: self.cx =linelen
        if event.GetEventType() ==wxEVT_RIGHT_DOWN:
            self.update = true
            self.OnFold()
        self.UpdateView()


    def DrawCursor(self, dc = None):
    ###############################################################
        """
        Auch der Cursor muﬂ ja irgendwie gezeichnet werden
        """
        if not dc:
            dc = wxClientDC(self)

        if (self.len)<self.cy: #-1 ?
            self.cy = self.len-1
        s = self.text[self.GetLine(self.cy)].text

        x = self.cx - self.sx
        y = self.cy - self.sy
        self.DrawSimpleCursor(x, y, dc)


    def DrawSimpleCursor(self, xp, yp, dc = None, old=false):
    ###############################################################
        """
        Auch der Cursor muﬂ ja irgendwie gezeichnet werden
        """
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


    def OnScroll(self, event):
        dir =event.GetOrientation()
        evt =event.GetEventType()
        if dir ==wxHORIZONTAL:
            if evt ==wxEVT_SCROLLWIN_LINEUP:     self.sx =self.sx -1
            elif evt ==wxEVT_SCROLLWIN_LINEDOWN: self.sx =self.sx +1
            elif evt ==wxEVT_SCROLLWIN_PAGEUP:   self.sx =self.sx -self.sw
            elif evt ==wxEVT_SCROLLWIN_PAGEDOWN: self.sx =self.sx +self.sw
            elif evt ==wxEVT_SCROLLWIN_TOP:      self.sx =self.cx =0
            elif evt ==wxEVT_SCROLLWIN_BOTTOM:
                self.sx =self.max_linelength -self.sw
                self.cx =self.max_linelength
            else:
                self.sx =event.GetPosition()

            if self.sx >(self.max_linelength -self.sw +1):
                self.sx =self.max_linelength -self.sw +1
            if self.sx <0:   self.sx =0
            if self.cx >(self.sx +self.sw -1): self.cx =self.sx +self.sw -1
            if self.cx <self.sx: self.cx =self.sx

        else:
            if evt ==wxEVT_SCROLLWIN_LINEUP:     self.sy =self.sy -1
            elif evt ==wxEVT_SCROLLWIN_LINEDOWN: self.sy =self.sy +1
            elif evt ==wxEVT_SCROLLWIN_PAGEUP:   self.sy =self.sy -self.sh
            elif evt ==wxEVT_SCROLLWIN_PAGEDOWN: self.sy =self.sy +self.sh
            elif evt ==wxEVT_SCROLLWIN_TOP:      self.sy =self.cy =0
            elif evt ==wxEVT_SCROLLWIN_BOTTOM:
                self.sy =self.len -self.sh
                self.cy =self.len
            else:
                self.sy =event.GetPosition()

            if self.sy >(self.len -self.sh +1):
                self.sy =self.len -self.sh +1
            if self.sy <0: self.sy =0
            if self.cy >(self.sy +self.sh -1): self.cy =self.sy +self.sh -1
            if self.cy <self.sy: self.cy =self.sy

        self.UpdateView()


    def AdjustScrollbars(self):
        # there appears to be endless recursion:
        # SetScrollbars issue EvtPaint which calls UpdateView
        # which calls AdjustScrollbars
        if not self.in_scroll:
            self.in_scroll =TRUE
            self.SetScrollbars(self.fw, self.fh, self.max_linelength +1,
                               # it seem to be a bug in scrollbars:
                               # the scrollbar is hidden
                               # even if current position >0
                               max(self.len +1, self.sy +self.sh),
                               self.sx, self.sy)
            self.osx, self.osy = self.sx, self.sy
            self.in_scroll =FALSE


    # adapts the output to what it should be
    def UpdateView(self, dc = None, doup=false):
    ###############################################################
        """
        Diese Routine wird immer dann aufgerufen, wenn
        sich etwas ver‰ndert hat
        """
        if self.inUpdate:
            return
        self.inUpdate = true

        self.CalcLines()

        if not dc:
            dc = wxClientDC(self)

        self.bw,self.bh = self.GetSizeTuple()
        self.sw = self.bw / self.fw
        self.sh = self.bh / self.fh

        if self.cy<self.sy:
            self.sy = self.cy
        elif self.cy>(self.sy+self.sh-1):
            self.sy = self.cy-self.sh+1

        if self.cx<self.sx:
            self.sx = self.cx
        elif self.cx>(self.sx+self.sw-1):
            self.sx = self.cx-self.sw+1

        # left line? change syntax!
        if self.ocy!=self.cy:
            self.OnUpdateSyntax(self.ocy)
        self.ocy = self.cy

        # alles beim alten
        if self.osx != self.sx or self.osy != self.sy:
            self.AdjustScrollbars()

        self.DrawSimpleCursor(0,0,dc, true)
        # [als] i don't really understand how the following condition works
        #if self.update or doup:
        self.Draw(dc)
        #    self.update = false
        #else:
        #    self.DrawCursor(dc)

        self.o_cx = self.cx
        self.o_cy = self.cy
        self.o_sx = self.sx
        self.o_sy = self.sy
        self.o_line = self.line
        self.inUpdate = false




    def DrawEditText(self, t, x, y, dc = None):
    ###############################################################
        """ Einfache Hilfsroutine um Text zu schreiben
        """
        if not dc:
            dc = wxClientDC(self)
        dc.SetFont(self.font)
        dc.DrawText(t, x * self.fw, y * self.fh)


    def DrawLine(self, line, dc=None):
    ###############################################################
        """
        Hier wird einfach die Ansicht der ganzen Seite
        wiederhergestellt.
        !!! Kann modifiziert werden !!!
        """

        if not dc:
            dc = wxClientDC(self)

        dc.SetBackgroundMode(wxSOLID)
        dc.SetTextBackground(self.bcol)
        dc.SetTextForeground(self.fcol)
        #dc.Clear()

        # delimiter
        ll = self.sx
        lr = self.sx + self.sw
        y = line - self.sy

        # text + syntax
        if self.IsLine(line):
            l   = self.GetLine(line)
            t   = self.text[l].text
            syn = self.text[l].syntax

            if not self.text[l].editable:
                dc.SetTextBackground(self.nedcol)
            else:
                dc.SetTextBackground(self.bcol)

            dc.SetTextForeground(self.fcol)

            pos = ll
            for h in syn:
                xp, col = h
                if xp>=ll:
                    self.DrawEditText(t[pos:xp], (pos-ll), y, dc)
                    pos = xp
                dc.SetTextForeground(self.ftab[col])
            self.DrawEditText(t[pos:], (pos-ll), y, dc)


    def Draw(self, odc=None):
    ###############################################################
        """
        Hier wird einfach die Ansicht der ganzen Seite
        wiederhergestellt.
        !!! Kann modifiziert werden !!!
        """

        if not odc:
            odc = wxClientDC(self)

        dc = self.mdc
        dc.SelectObject(wxEmptyBitmap(self.bw,self.bh))
        dc.SetBackgroundMode(wxSOLID)
        dc.SetTextBackground(self.bcol)
        dc.SetTextForeground(self.fcol)
        dc.Clear()
        for line in range(self.sy, self.sy + self.sh): self.DrawLine(line, dc)
        odc.Blit(0,0,self.bw,self.bh,dc,0,0,wxCOPY)
        self.DrawCursor(odc)


    def cVert(self, num):
    ###############################################################
        """ Vertikale Cursorverschiebung
        """
        cy = self.cy + num
        if cy <0: cy =0
        elif cy >(self.len -1): cy =self.len -1
        # scroll when edge hit
        if cy >(self.sy +self.sh -1): self.sy =cy -self.sh +1
        elif cy <self.sy: self.sy =cy
        self.cy =cy
        # disallow positioning behind the end of the line
        linelen =len(self.text[self.GetLine(cy)].text)
        if self.cx >linelen: self.cx =linelen


    def cHoriz(self, num):
    ###############################################################
        """ Horizontale Cursorverschiebung
        """
        cx = self.cx + num
        linelen =len(self.text[self.GetLine(self.cy)].text)
        if cx <0: cx =0
        elif cx >linelen: cx =linelen
        # scroll when edge hit
        if cx >(self.sx +self.sw -2): self.sx =cx -self.sw +2
        elif cx <self.sx: self.sx =cx
        self.cx =cx


    def InsertText(self, text):
    ###############################################################
        """
        Simple Routine um Text - auch ¸ber mehrere
        Zeilen - einzuf¸gen
        """

        if self.IsEditable(self.cy):
            tis = split(text, "\n")

            t = self.GetTextLine(self.cy)

            if len(tis)==1:
                t = t[:self.cx] + text + t[self.cx:]
                self.SetTextLine(self.cy, t)
                self.cHoriz(len(text))
            else:
                rest = t[self.cx:]
                t = t[:self.cx] + tis[0]
                self.SetTextLine(self.cy, t)
                for i in range(1,len(tis)):
                    self.text.insert(self.GetLine(self.cy)+1, Line())
                    self.lines.insert(self.cy+1,self.GetLine(self.cy)+1)
                    self.cVert(+1)
                    self.SetTextLine(self.cy, tis[i])
                t = self.GetTextLine(self.cy)
                self.cx = len(t)
                t = t + rest
                self.SetTextLine(self.cy, t)
            self.update = true
            #self.UpdateView()

#-----------------------------------------------------------------------------------------

    def RemoveLine(self, line):
        pass


    def OnChar(self, event):
    ###############################################################
        """
        Wenn eine Taste gedr¸ckt wird,
        kann an dieser Stelle die Auswertung stattfinden
        """

        # get code
        key = event.KeyCode()

#        if event.ControlDown:
#            if chr(key)=="k":
#                print "weg"


        # movements
        if key==WXK_DOWN:
            self.cVert(+1)
        elif key==WXK_UP:
            self.cVert(-1)
        elif key==WXK_LEFT:
            self.cHoriz(-1)
        elif key==WXK_RIGHT:
            self.cHoriz(+1)

        elif key==WXK_NEXT:
            self.cVert(self.sh)
        elif key==WXK_PRIOR:
            self.cVert(-self.sh)

        elif key==WXK_HOME:
            self.cx = 0
        elif key==WXK_END:
            self.cx = len(self.GetTextLine(self.cy))

        elif key==WXK_BACK:
            t = self.GetTextLine(self.cy)
            if self.cx>0:
                t = t[:self.cx-1] + t[self.cx:]
                self.SetTextLine(self.cy, t)
                self.cHoriz(-1)

        elif key==WXK_DELETE:
            t = self.GetTextLine(self.cy)
            if self.cx<len(t):
                t = t[:self.cx] + t[self.cx+1:]
            self.SetTextLine(self.cy, t)

        elif key==WXK_RETURN:
            self.InsertText("\n")

        elif key==WXK_TAB:
            self.OnTabulator(event)

        # clipboard (buggy)
        elif key==WXK_F10:
            if wxTheClipboard.Open():
                data = wxTheClipboard.GetData()
                wxTheClipboard.Close()
                print data

        # folding (buggy)
        elif key==WXK_F12:
            self.update = true
            self.OnFold()

        # regular ascii
        elif (key>31) and (key<256):
            self.InsertText(chr(key))

        self.UpdateView()
        return 0


    def OnPaint(self, event):
        dc = wxPaintDC(self)
        self.bw,self.bh = self.GetSizeTuple()
        self.UpdateView(dc, true)


#-----------------------------------------------------------------------------------------

    def GetIndent(self, line):
        p = 0
        for c in line:
            if c==" ": p = p + 1
            elif c=="\t": p =(p /self.tabsize +1) *self.tabsize
            else: break
        return p


    def Goto(self, pos):
        self.cVert(pos-self.cy-1)
        self.UpdateView()

# --------------------------------------------------------

    # to be overloaded
    def OnUpdateHighlight(self, line = -1):
        pass

    def OnUpdateSyntax(self, line = -1):
        pass

    def OnTabulator(self, event):
        pass

    def OnInit(self):
        pass

    def OnFold(self):
        pass

