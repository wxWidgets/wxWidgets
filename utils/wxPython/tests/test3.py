#!/bin/env python
#----------------------------------------------------------------------------
# Name:         test3.py
# Purpose:      Testing menus and status lines
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
    def OnPaint(self, event):
        dc = wxPaintDC(self)
        dc.BeginDrawing()
        w, h = self.GetClientSize()
        font = wxFont(42, wxSWISS, wxNORMAL, wxNORMAL)
        dc.SetFont(font)
        st = "Python Rules!"
        tw,th, d,e = dc.GetTextExtent(st)
        dc.DrawText(st, (w-tw)/2, (h-th)/2)
        dc.EndDrawing()

#---------------------------------------------------------------------------

if wxPlatform == '__WXMSW__':
    class MyMiniFrame(wxMiniFrame):
        def __init__(self, parent, ID, title, pos, size, style):
            wxMiniFrame.__init__(self, parent, ID, title, pos, size, style)
            panel = wxPanel(self, -1)
            ID = NewId()
            button = wxButton(panel, ID, "Close Me")
            button.SetPosition(wxPoint(15, 15))
            self.Connect(ID, -1, wxEVT_COMMAND_BUTTON_CLICKED, self.OnCloseMe)

        def OnCloseMe(self, event):
            self.Close(true)

        def OnCloseWindow(self, event):
            self.Destroy()

#---------------------------------------------------------------------------

class MyFrame(wxFrame):
    def __init__(self, parent, id, title):
        wxFrame.__init__(self, parent, id, title, wxPyDefaultPosition,
                         wxSize(420, 200))
        self.canvas = MyCanvas(self, -1)
        self.CreateStatusBar(3)
        mainmenu = wxMenuBar()
        menu = wxMenu()
        menu.Append(100, 'A &Menu Item', 'the help text')
        menu.Append(101, '&Another', 'Grok!')
        menu.AppendSeparator()
        menu.Append(200, 'E&xit', 'Get the heck outta here!')
        mainmenu.Append(menu, "&It's a menu!")
        self.SetMenuBar(mainmenu)
        if wxPlatform == '__WXMSW__':
            print menu.GetHelpString(100)
            print mainmenu.GetHelpString(101)
            print mainmenu.GetHelpString(200)
            self.DragAcceptFiles(true)

        self.Connect(-1, -1, wxEVT_COMMAND_MENU_SELECTED, self.OnMenuCommand)
        self.Connect(-1, -1, wxEVT_DROP_FILES, self.OnDropFiles)



    def OnCloseWindow(self, event):
        print 'OnCloseWindow'
        self.Destroy()


    def OnSize(self, event):
        w,h = self.GetClientSize()
        self.canvas.SetSize(wxSize(w, h))
        self.SetStatusText("hello, this is a test: (%d, %d)" % (w,h))

##     def OnMenuHighlight(self, event):
##         mainmenu = self.GetMenuBar()
##         st = mainmenu.GetHelpString(event.GetMenuId())
##         self.SetStatusText('['+st+']', 0)

    def OnMenuCommand(self, event):
        # why isn't this a wxMenuEvent???
        print event, event.GetInt()
        if event.GetInt() == 200:
            self.Close()
        elif event.GetInt() == 101:
            if wxPlatform == '__WXMSW__':
                win = MyMiniFrame(self, -1, "This is a Mini...",
                              wxPoint(-1, -1), #wxPyDefaultPosition,
                              wxSize(150, 150),
                              wxMINIMIZE_BOX | wxMAXIMIZE_BOX |
                              wxTHICK_FRAME | wxSYSTEM_MENU |
                              wxTINY_CAPTION_HORIZ)
                win.Show(true)
            else:
                print 'Sorry, can\'t do mini\'s...'



    def OnDropFiles(self, event):
        fileList = event.GetFiles()
        for file in fileList:
            print file


#---------------------------------------------------------------------------


class MyApp(wxApp):
    def OnInit(self):
        frame = MyFrame(NULL, -1, "Test 3")
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
# Revision 1.2  1998/08/22 19:51:17  RD
# some tweaks for wxGTK
#
# Revision 1.1  1998/08/09 08:28:05  RD
# Initial version
#
#
