#!/bin/env python
#----------------------------------------------------------------------------
# Name:         test2.py
# Purpose:      Testing GDI stuff and events.
#
# Author:       Robin Dunn
#
# Created:
# RCS-ID:       $Id$
# Copyright:    (c) 1998 by Total Control Software
# Licence:      wxWindows license
#----------------------------------------------------------------------------


from wxPython.wx import *


#---------------------------------------------------------------------------



class MyCanvas(wxScrolledWindow):
    def __init__(self, parent):
        wxScrolledWindow.__init__(self, parent, -1, wxPoint(0, 0), wxPyDefaultSize, wxSUNKEN_BORDER)

        self.SetBackgroundColour(wxNamedColor("WHITE"))
        self.Connect(-1, -1, wxEVT_LEFT_DOWN, self.OnLeftButtonEvent)
        self.Connect(-1, -1, wxEVT_LEFT_UP,   self.OnLeftButtonEvent)
        self.Connect(-1, -1, wxEVT_MOTION,    self.OnLeftButtonEvent)

        self.SetCursor(wxStockCursor(wxCURSOR_PENCIL))
        bmp = wxBitmap('bitmaps/test2.bmp', wxBITMAP_TYPE_BMP)
        self.bmp = bmp

        self.SetScrollbars(20, 20, 50, 50)

        self.lines = []



    def OnPaint(self, event):
        dc = wxPaintDC(self)
        self.PrepareDC(dc)
        self.DoDrawing(dc)



    def DoDrawing(self, dc):
        dc.BeginDrawing()
        #dc.Clear()
        pen1 = wxPen(wxNamedColour('RED'))
        dc.SetPen(pen1)
        dc.DrawRectangle(5, 5, 50, 50)

        dc.SetBrush(wxLIGHT_GREY_BRUSH)
        dc.SetPen(wxPen(wxNamedColour('BLUE'), 4))
        dc.DrawRectangle(15, 15, 50, 50)

        font = wxFont(14, wxSWISS, wxNORMAL, wxNORMAL)
        dc.SetFont(font)
        dc.SetTextForeground(wxColour(0xFF, 0x20, 0xFF))
        te = dc.GetTextExtent("Hello World")
        dc.DrawText("Hello World", 60, 65)

        dc.SetPen(wxPen(wxNamedColour('VIOLET'), 4))
        dc.DrawLine(5, 65+te[1], 60+te[0], 65+te[1])

        lst = [(100,110), (150,110), (150,160), (100,160)]
        dc.DrawLines(lst, -60)
        dc.SetPen(wxGREY_PEN)
        dc.DrawPolygon(lst, 75)
        dc.SetPen(wxGREEN_PEN)
        dc.DrawSpline(lst+[(100,100)])

        dc.DrawBitmap(self.bmp, 200, 20)
        dc.SetTextForeground(wxColour(0, 0xFF, 0x80))
        dc.DrawText("a bitmap", 200, 80)

        self.DrawSavedLines(dc)
        dc.EndDrawing()


    def DrawSavedLines(self, dc):
        dc.SetPen(wxPen(wxNamedColour('MEDIUM FOREST GREEN'), 4))
        for line in self.lines:
            for coords in line:
                apply(dc.DrawLine, coords)



    def OnLeftButtonEvent(self, event):
        if event.LeftDown():
            self.x, self.y = event.GetX(), event.GetY()
            self.curLine = []
        elif event.Dragging():
            dc = wxClientDC(self)
            dc.BeginDrawing()
            dc.SetPen(wxPen(wxNamedColour('MEDIUM FOREST GREEN'), 4))
            coords = (self.x, self.y, event.GetX(), event.GetY())
            self.curLine.append(coords)
            apply(dc.DrawLine, coords)
            self.x, self.y = event.GetX(), event.GetY()
            dc.EndDrawing()
        elif event.LeftUp():
            self.lines.append(self.curLine)
            self.curLine = []





#---------------------------------------------------------------------------

class MyFrame(wxFrame):
    def __init__(self, parent, id, title):
        wxFrame.__init__(self, parent, id, title, wxPyDefaultPosition, wxSize(320, 200))
        self.canvas = MyCanvas(self)

    def OnCloseWindow(self, event):
        self.Destroy()

    def OnSize(self, event):
        size = self.GetClientSize()
        self.canvas.SetDimensions(5, 5, size.width-10, size.height-10)


#---------------------------------------------------------------------------


class MyApp(wxApp):
    def OnInit(self):
        frame = MyFrame(NULL, -1, "Test 2")
        frame.Show(true)
        self.SetTopWindow(frame)
        return true


#---------------------------------------------------------------------------


def main():
    app = MyApp(0)
    app.MainLoop()


def t():
    import pdb
    pdb.run('main()')

if __name__ == '__main__':
    main()


#----------------------------------------------------------------------------
#
# $Log$
# Revision 1.1.2.2  2001/01/30 20:54:16  robind
# Gobs of changes move from the main trunk to the 2.2 branch in
# preparataion for 2.2.5 release.  See CHANGES.txt for details.
#
# Revision 1.3  2000/10/30 21:05:22  robind
#
# Merged wxPython 2.2.2 over to the main branch
#
# Revision 1.1.2.1  2000/05/16 02:07:01  RD
#
# Moved and reorganized wxPython directories
#
# Now builds into an intermediate wxPython package directory before
# installing
#
# Revision 1.3  1999/04/30 03:29:53  RD
#
# wxPython 2.0b9, first phase (win32)
# Added gobs of stuff, see wxPython/README.txt for details
#
# Revision 1.2.4.1  1999/03/27 23:30:00  RD
#
# wxPython 2.0b8
#     Python thread support
#     various minor additions
#     various minor fixes
#
# Revision 1.2  1998/12/15 20:44:34  RD
# Changed the import semantics from "from wxPython import *" to "from
# wxPython.wx import *"  This is for people who are worried about
# namespace pollution, they can use "from wxPython import wx" and then
# prefix all the wxPython identifiers with "wx."
#
# Added wxTaskbarIcon for wxMSW.
#
# Made the events work for wxGrid.
#
# Added wxConfig.
#
# Added wxMiniFrame for wxGTK, (untested.)
#
# Changed many of the args and return values that were pointers to gdi
# objects to references to reflect changes in the wxWindows API.
#
# Other assorted fixes and additions.
#
# Revision 1.1  1998/08/09 08:28:05  RD
# Initial version
#
#
