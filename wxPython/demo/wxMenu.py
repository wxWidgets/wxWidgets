#-------------------------------------------------------------------
# essaimenu.py
#
# menus in wxPython 2.3.3
#
#-------------------------------------------------------------------

from wxPython.wx import *

#-------------------------------------------------------------------

class MyFrame(wxFrame):

    def __init__(self, parent, id, log):
        wxFrame.__init__(self, parent, id, 'Playing with menus', size=(400, 200))
        self.log = log
        self.CenterOnScreen()

        self.CreateStatusBar()
        self.SetStatusText("This is the statusbar")

        tc = wxTextCtrl(self, -1, """
A bunch of bogus menus have been created for this frame.  You
can play around with them to see how they behave and then
check the source for this sample to see how to implement them.
""", style=wxTE_READONLY|wxTE_MULTILINE)

        # Prepare the menu bar
        menuBar = wxMenuBar()

        # 1st menu from left
        menu1 = wxMenu()
        menu1.Append(101, "&Mercury", "This the text in the Statusbar")
        menu1.Append(102, "&Venus", "")
        menu1.Append(103, "&Earth", "You may select Earth too")
        menu1.AppendSeparator()
        menu1.Append(104, "&Close", "Close this frame")
        # Add menu to the menu bar
        menuBar.Append(menu1, "&Planets")

        # 2nd menu from left
        menu2 = wxMenu()
        menu2.Append(201, "Hydrogen")
        menu2.Append(202, "Helium")
        # a submenu in the 2nd menu
        submenu = wxMenu()
        submenu.Append(2031,"Lanthanium")
        submenu.Append(2032,"Cerium")
        submenu.Append(2033,"Praseodymium")
        menu2.AppendMenu(203, "Lanthanides", submenu)
        # Append 2nd menu
        menuBar.Append(menu2, "&Elements")

        menu3 = wxMenu()
        menu3.Append(301, "IDLE", "a Python shell using tcl/tk as GUI", wxITEM_RADIO)
        menu3.Append(302, "PyCrust", "a Python shell using wxPython as GUI", wxITEM_RADIO)
        menu3.Append(303, "psi", "a simple Python shell using wxPython as GUI", wxITEM_RADIO)
        menu3.AppendSeparator()
        menu3.Append(304, "project1", "", wxITEM_NORMAL)
        menu3.Append(305, "project2", "", wxITEM_NORMAL)
        menuBar.Append(menu3, "&Shells")

        menu4 = wxMenu()
        menu4.Append(401, "letters", "abcde...", wxITEM_CHECK)
        menu4.Append(402, "digits", "123...", wxITEM_CHECK)
        menu4.Append(403, "letters and digits", "abcd... + 123...", wxITEM_CHECK)
        menuBar.Append(menu4, "Chec&k")

        menu5 = wxMenu()
        menu5.Append(501, "Interesting thing\tCtrl+A", "Note the shortcut!")
        menu5.AppendSeparator()
        menu5.Append(502, "Hello\tShift+H")
        menu5.AppendSeparator()
        menu5.Append(503, "remove the submenu")
        menu6 = wxMenu()
        menu6.Append(601, "Submenu Item")
        menu5.AppendMenu(504, "submenu", menu6)
        menu5.Append(505, "remove this menu")
        menuBar.Append(menu5, "&Fun")

        self.SetMenuBar(menuBar)

        # Menu events
        EVT_MENU_HIGHLIGHT_ALL(self, self.OnMenuHighlight)

        EVT_MENU(self, 101, self.Menu101)
        EVT_MENU(self, 102, self.Menu102)
        EVT_MENU(self, 103, self.Menu103)
        EVT_MENU(self, 104, self.CloseWindow)

        EVT_MENU(self, 201, self.Menu201)
        EVT_MENU(self, 202, self.Menu202)
        EVT_MENU(self, 2031, self.Menu2031)
        EVT_MENU(self, 2032, self.Menu2032)
        EVT_MENU(self, 2033, self.Menu2033)

        EVT_MENU(self, 301, self.Menu301To303)
        EVT_MENU(self, 302, self.Menu301To303)
        EVT_MENU(self, 303, self.Menu301To303)
        EVT_MENU(self, 304, self.Menu304)
        EVT_MENU(self, 305, self.Menu305)

        EVT_MENU_RANGE(self, 401, 403, self.Menu401To403)

        EVT_MENU(self, 501, self.Menu501)
        EVT_MENU(self, 502, self.Menu502)
        EVT_MENU(self, 503, self.TestRemove)
        EVT_MENU(self, 505, self.TestRemove2)


    # Methods

    def OnMenuHighlight(self, event):
        # Show how to get menu item imfo from this event handler
        id = event.GetMenuId()
        item = self.GetMenuBar().FindItemById(id)
        text = item.GetText()
        help = item.GetHelp()
        #print text, help
        event.Skip() # but in this case just call Skip so the default is done


    def Menu101(self, event):
        self.log.write('Welcome to Mercury\n')

    def Menu102(self, event):
        self.log.write('Welcome to Venus\n')

    def Menu103(self, event):
        self.log.write('Welcome to the Earth\n')

    def CloseWindow(self, event):
        self.Close()

    def Menu201(self, event):
        self.log.write('Chemical element number 1\n')

    def Menu202(self, event):
        self.log.write('Chemical element number 2\n')

    def Menu2031(self, event):
        self.log.write('Element number 57\n')

    def Menu2032(self, event):
        self.log.write('Element number 58\n')

    def Menu2033(self, event):
        self.log.write('Element number 59\n')

    def Menu301To303(self, event):
        id = event.GetId()
        self.log.write('Event id: %d\n' % id)

    def Menu304(self, event):
        self.log.write('Not yet available\n')

    def Menu305(self, event):
        self.log.write('Still vapour\n')

    def Menu401To403(self, event):
        self.log.write('From a EVT_MENU_RANGE event\n')

    def Menu501(self, event):
        self.log.write('Look in the code how the shortcut has been realized\n')

    def Menu502(self, event):
        self.log.write('Hello from Jean-Michel\n')


    def TestRemove(self, evt):
        mb = self.GetMenuBar()
        submenuItem = mb.FindItemById(601)
        if not submenuItem:
            return
        submenu = submenuItem.GetMenu()
        menu = submenu.GetParent()

        #menu.Remove(504)               # works
        menu.RemoveItem(mb.FindItemById(504))  # this also works
        #menu.RemoveItem(submenuItem)   # doesn't work, as expected since submenuItem is not on menu


    def TestRemove2(self, evt):
        mb = self.GetMenuBar()
        mb.Remove(4)


#-------------------------------------------------------------------

def runTest(frame, nb, log):
    win = MyFrame(frame, -1, log)
    frame.otherWin = win
    win.Show(True)


#-------------------------------------------------------------------


overview = """\
A demo of using wxMenuBar and wxMenu in various ways.
"""




if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])

