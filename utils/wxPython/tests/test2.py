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


from wxPython import *


#---------------------------------------------------------------------------



class MyCanvas(wxWindow):
    def __init__(self, parent):
        wxWindow.__init__(self, parent, -1, wxPoint(0, 0), wxPyDefaultSize, wxSUNKEN_BORDER)

        self.Connect(-1, -1, wxEVT_LEFT_DOWN, self.OnLeftButtonEvent)
        self.Connect(-1, -1, wxEVT_LEFT_UP,   self.OnLeftButtonEvent)
        self.Connect(-1, -1, wxEVT_MOTION,    self.OnLeftButtonEvent)

        self.SetCursor(wxStockCursor(wxCURSOR_PENCIL))
        bmp = wxBitmap('bitmaps/test2.bmp', wxBITMAP_TYPE_BMP)
        print 'bmp OK:', bmp.Ok()
        print 'bmp: (%dx%dx%d)' % (bmp.GetWidth(), bmp.GetHeight(), bmp.GetDepth())
        self.bmp = bmp

        self.lines = []



    def OnPaint(self, event):
        dc = wxPaintDC(self)
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
        w,h = self.GetClientSize()
        #self.canvas.SetSize(5, 5, w-10, h-10)
        self.canvas.SetDimensions(0, 0, w, h)


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
# Revision 1.1  1998/08/09 08:28:05  RD
# Initial version
#
#
