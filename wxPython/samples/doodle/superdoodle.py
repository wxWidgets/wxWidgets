# superdoodle.py

"""
This module implements the SuperDoodle demo application.  It takes the
DoodleWindow previously presented and reuses it in a much more
intelligent Frame.  This one has a menu and a statusbar, is able to
save and reload doodles, clear the workspace, and has a simple control
panel for setting color and line thickness in addition to the popup
menu that DoodleWindow provides.  There is also a nice About dialog
implmented using an wxHtmlWindow.
"""

from wxPython.wx import *
from doodle import DoodleWindow

import os, cPickle


#----------------------------------------------------------------------

idNEW    = 11001
idOPEN   = 11002
idSAVE   = 11003
idSAVEAS = 11004
idCLEAR  = 11005
idEXIT   = 11006
idABOUT  = 11007


class DoodleFrame(wxFrame):
    """
    A DoodleFrame contains a DoodleWindow and a ControlPanel and manages
    their layout with a wxBoxSizer.  A menu and associated event handlers
    provides for saving a doodle to a file, etc.
    """
    title = "Do a doodle"
    def __init__(self, parent):
        wxFrame.__init__(self, parent, -1, self.title, size=(800,600))
        self.CreateStatusBar()
        self.MakeMenu()
        self.filename = None

        self.doodle = DoodleWindow(self, -1)
        cPanel = ControlPanel(self, -1, self.doodle)

        # Create a sizer to layout the two windows side-by-side.
        # Both will grow vertically, the doodle window will grow
        # horizontally as well.
        box = wxBoxSizer(wxHORIZONTAL)
        box.Add(cPanel, 0, wxEXPAND)
        box.Add(self.doodle, 1, wxEXPAND)

        # Tell the frame that it should layout itself in response to
        # size events.
        self.SetAutoLayout(true)
        self.SetSizer(box)


    def SaveFile(self):
        if self.filename:
            data = self.doodle.GetLinesData()
            f = open(self.filename, 'w')
            cPickle.dump(data, f)
            f.close()


    def ReadFile(self):
        if self.filename:
            try:
                f = open(self.filename, 'r')
                data = cPickle.load(f)
                f.close()
                self.doodle.SetLinesData(data)
            except cPickle.UnpicklingError:
                wxMessageBox("%s is not a doodle file." % self.filename,
                             "oops!", style=wxOK|wxICON_EXCLAMATION)


    def MakeMenu(self):
        # create the file menu
        menu1 = wxMenu()
        menu1.Append(idOPEN, "&Open", "Open a doodle file")
        menu1.Append(idSAVE, "&Save", "Save the doodle")
        menu1.Append(idSAVEAS, "Save &As", "Save the doodle in a new file")
        menu1.AppendSeparator()
        menu1.Append(idCLEAR, "&Clear", "Clear the current doodle")
        menu1.AppendSeparator()
        menu1.Append(idEXIT, "E&xit", "Terminate the application")

        # and the help menu
        menu2 = wxMenu()
        menu2.Append(idABOUT, "&About", "Display the gratuitous 'about this app' thingamajig")

        # and add them to a menubar
        menuBar = wxMenuBar()
        menuBar.Append(menu1, "&File")
        menuBar.Append(menu2, "&Help")
        self.SetMenuBar(menuBar)

        EVT_MENU(self, idOPEN,   self.OnMenuOpen)
        EVT_MENU(self, idSAVE,   self.OnMenuSave)
        EVT_MENU(self, idSAVEAS, self.OnMenuSaveAs)
        EVT_MENU(self, idCLEAR,  self.OnMenuClear)
        EVT_MENU(self, idEXIT,   self.OnMenuExit)
        EVT_MENU(self, idABOUT,  self.OnMenuAbout)



    wildcard = "Doodle files (*.ddl)|*.ddl|All files (*.*)|*.*"

    def OnMenuOpen(self, event):
        dlg = wxFileDialog(self, "Open doodle file...",
                           style=wxOPEN, wildcard = self.wildcard)
        if dlg.ShowModal() == wxID_OK:
            self.filename = dlg.GetPath()
            self.ReadFile()
            self.SetTitle(self.title + ' -- ' + self.filename)
        dlg.Destroy()


    def OnMenuSave(self, event):
        if not self.filename:
            self.OnMenuSaveAs(event)
        else:
            self.SaveFile()


    def OnMenuSaveAs(self, event):
        dlg = wxFileDialog(self, "Save doodle as...",
                           style=wxSAVE | wxOVERWRITE_PROMPT,
                           wildcard = self.wildcard)
        if dlg.ShowModal() == wxID_OK:
            filename = dlg.GetPath()
            if not os.path.splitext(filename)[1]:
                filename = filename + '.ddl'
            self.filename = filename
            self.SaveFile()
            self.SetTitle(self.title + ' -- ' + self.filename)
        dlg.Destroy()


    def OnMenuClear(self, event):
        self.doodle.SetLinesData([])
        self.SetTitle(self.title)


    def OnMenuExit(self, event):
        self.Close()


    def OnMenuAbout(self, event):
        dlg = DoodleAbout(self)
        dlg.ShowModal()
        dlg.Destroy()



#----------------------------------------------------------------------


class ControlPanel(wxPanel):
    """
    This class implements a very simple control panel for the DoodleWindow.
    It creates buttons for each of the colours and thickneses supported by
    the DoodleWindow, and event handlers to set the selected values.  There is
    also a little window that shows an example doodleLine in the selected
    values.  Nested sizers are used for layout.
    """
    def __init__(self, parent, ID, doodle):
        wxPanel.__init__(self, parent, ID, style=wxRAISED_BORDER)

        numCols = 4
        spacing = 4

        # Make a grid of buttons for each colour.  Attach each button
        # event to self.OnSetColour.  The button ID is the same as the
        # key in the colour dictionary.
        colours = doodle.menuColours
        keys = colours.keys()
        keys.sort()
        cGrid = wxGridSizer(cols=numCols, hgap=2, vgap=2)
        for k in keys:
            bmp = self.MakeBitmap(wxNamedColour(colours[k]))
            b = wxBitmapButton(self, k, bmp)
            EVT_BUTTON(self, k, self.OnSetColour)
            cGrid.Add(b, 0)

        # Save the button size so we can use it for the number buttons
        btnSize = b.GetSize()

        # Make a grid of buttons for the thicknesses.  Attach each button
        # event to self.OnSetThickness.  The button ID is the same as the
        # thickness value.
        tGrid = wxGridSizer(cols=numCols, hgap=2, vgap=2)
        for x in range(1, doodle.maxThickness+1):
            b = wxButton(self, x, str(x), size=btnSize)
            EVT_BUTTON(self, x, self.OnSetThickness)
            tGrid.Add(b, 0)

        # Make a colour indicator window, it is registerd as a listener
        # with the doodle window so it will be notified when the settings
        # change
        ci = ColourIndicator(self)
        doodle.AddListener(ci)
        doodle.Notify()
        self.doodle = doodle

        # Make a box sizer and put the two grids and the indicator
        # window in it.
        box = wxBoxSizer(wxVERTICAL)
        box.Add(cGrid, 0, wxALL, spacing)
        box.Add(tGrid, 0, wxALL, spacing)
        box.Add(ci, 0, wxEXPAND|wxALL, spacing)
        self.SetSizer(box)
        self.SetAutoLayout(true)

        # Resize this window so it is just large enough for the
        # minimum requirements of the sizer.
        box.Fit(self)



    def MakeBitmap(self, colour):
        """
        We can create a bitmap of whatever we want by simply selecting
        it into a wxMemoryDC and drawing on it.  In this case we just set
        a background brush and clear the dc.
        """
        bmp = wxEmptyBitmap(16,16)
        dc = wxMemoryDC()
        dc.SelectObject(bmp)
        dc.SetBackground(wxBrush(colour))
        dc.Clear()
        dc.SelectObject(wxNullBitmap)
        return bmp


    def OnSetColour(self, event):
        """
        Use the event ID to get the colour, set that colour in the doodle.
        """
        colour = self.doodle.menuColours[event.GetId()]
        self.doodle.SetColour(colour)


    def OnSetThickness(self, event):
        """
        Use the event ID to set the thickness in the doodle.
        """
        self.doodle.SetThickness(event.GetId())


#----------------------------------------------------------------------

class ColourIndicator(wxWindow):
    """
    An instance of this class is used on the ControlPanel to show
    a sample of what the current doodle line will look like.
    """
    def __init__(self, parent):
        wxWindow.__init__(self, parent, -1, style=wxSUNKEN_BORDER)
        self.SetBackgroundColour(wxWHITE)
        self.SetSize(wxSize(-1, 40))
        self.colour = self.thickness = None
        EVT_PAINT(self, self.OnPaint)


    def Update(self, colour, thickness):
        """
        The doodle window calls this method any time the colour
        or line thickness changes.
        """
        self.colour = colour
        self.thickness = thickness
        self.Refresh()  # generate a paint event


    def OnPaint(self, event):
        """
        This method is called when all or part of the window needs to be
        redrawn.
        """
        dc = wxPaintDC(self)
        if self.colour:
            sz = self.GetClientSize()
            pen = wxPen(wxNamedColour(self.colour), self.thickness)
            dc.BeginDrawing()
            dc.SetPen(pen)
            dc.DrawLine(10, sz.height/2, sz.width-10, sz.height/2)
            dc.EndDrawing()


#----------------------------------------------------------------------

class DoodleAbout(wxDialog):
    """ An about box that uses an HTML window """

    text = '''
<html>
<body bgcolor="#ACAA60">
<center><table bgcolor="#455481" width="100%" cellspacing="0"
cellpadding="0" border="1">
<tr>
    <td align="center"><h1>SuperDoodle</h1></td>
</tr>
</table>
</center>
<p><b>SuperDoodle</b> is a demonstration program for <b>wxPython</b> that
will hopefully teach you a thing or two.  Just follow these simple
instructions: </p>
<p>
<ol>
  <li><b>Read</b> the Source...
  <li><b>Learn</b>...
  <li><b>Do!</b>
</ol>

<p><b>SuperDoodle</b> and <b>wxPython</b> are brought to you by
<b>Robin Dunn</b> and <b>Total Control Software</b>, Copyright
&copy; 1997-2001.</p>
</body>
</html>
'''

    def __init__(self, parent):
        wxDialog.__init__(self, parent, -1, 'About SuperDoodle',
                          size=wxSize(420, 380))
        from wxPython.html import wxHtmlWindow

        html = wxHtmlWindow(self, -1)
        html.SetPage(self.text)
        button = wxButton(self, wxID_OK, "Okay")

        # constraints for the html window
        lc = wxLayoutConstraints()
        lc.top.SameAs(self, wxTop, 5)
        lc.left.SameAs(self, wxLeft, 5)
        lc.bottom.SameAs(button, wxTop, 5)
        lc.right.SameAs(self, wxRight, 5)
        html.SetConstraints(lc)

        # constraints for the button
        lc = wxLayoutConstraints()
        lc.bottom.SameAs(self, wxBottom, 5)
        lc.centreX.SameAs(self, wxCentreX)
        lc.width.AsIs()
        lc.height.AsIs()
        button.SetConstraints(lc)

        self.SetAutoLayout(true)
        self.Layout()
        self.CentreOnParent(wxBOTH)


#----------------------------------------------------------------------

class DoodleApp(wxApp):
    def OnInit(self):
        frame = DoodleFrame(None)
        frame.Show(true)
        self.SetTopWindow(frame)
        return true


#----------------------------------------------------------------------

if __name__ == '__main__':
    app = DoodleApp(0)
    app.MainLoop()
