
from   wxPython.wx import *

import ColorPanel
import wxGrid
import wxListCtrl
import wxScrolledWindow

#----------------------------------------------------------------------------

def runTest(frame, nb, log):

    testWin = wxNotebook(nb, -1, style=wxNB_BOTTOM)

    win = ColorPanel.ColoredPanel(testWin, wxBLUE)
    testWin.AddPage(win, "Blue")
    st = wxStaticText(win, -1,
                      "You can put nearly any type of window here,\n"
                      "and the tabs can be on any side... (look below.)",
                      wxPoint(10, 10))
    st.SetForegroundColour(wxWHITE)
    st.SetBackgroundColour(wxBLUE)

    win = ColorPanel.ColoredPanel(testWin, wxRED)
    testWin.AddPage(win, "Red")

    win = wxScrolledWindow.MyCanvas(testWin)
    testWin.AddPage(win, 'ScrolledWindow')

    win = ColorPanel.ColoredPanel(testWin, wxGREEN)
    testWin.AddPage(win, "Green")

    win = wxGrid.TestGrid(testWin, log)
    testWin.AddPage(win, "Grid")

    win = wxListCtrl.TestListCtrlPanel(testWin, log)
    testWin.AddPage(win, 'List')

    win = ColorPanel.ColoredPanel(testWin, wxCYAN)
    testWin.AddPage(win, "Cyan")

    win = ColorPanel.ColoredPanel(testWin, wxWHITE)
    testWin.AddPage(win, "White")

    win = ColorPanel.ColoredPanel(testWin, wxBLACK)
    testWin.AddPage(win, "Black")

    win = ColorPanel.ColoredPanel(testWin, wxNamedColour('MIDNIGHT BLUE'))
    testWin.AddPage(win, "MIDNIGHT BLUE")

    win = ColorPanel.ColoredPanel(testWin, wxNamedColour('INDIAN RED'))
    testWin.AddPage(win, "INDIAN RED")

    return testWin

#----------------------------------------------------------------------------















overview = """\
This class represents a notebook control, which manages multiple windows with associated tabs.

To use the class, create a wxNotebook object and call AddPage or InsertPage, passing a window to be used as the page. Do not explicitly delete the window for a page that is currently managed by wxNotebook.

wxNotebook()
-------------------------

Default constructor.

wxNotebook(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size, long style = 0, const wxString& name = "notebook")

Constructs a notebook control.

Parameters
-------------------

parent = The parent window. Must be non-NULL.

id = The window identifier.

pos = The window position.

size = The window size.

style = The window style. Its value is a bit list of zero or more of wxTC_MULTILINE, wxTC_RIGHTJUSTIFY, wxTC_FIXEDWIDTH and wxTC_OWNERDRAW.
"""
