# superdoodle.py

"""
This module implements the SuperDoodle demo application.  It takes the
DoodleWindow previously presented and reuses it in a much more
intelligent Frame.  This one has a menu and a statusbar, is able to
save and reload doodles, clear the workspace, and has a simple control
panel for setting color and line thickness in addition to the popup
menu that DoodleWindow provides.  There is also a nice About dialog
implmented using an wx.html.HtmlWindow.
"""

import wx                  # This module uses the new wx namespace
import wx.html
from wx.lib import buttons # for generic button classes
from doodle import DoodleWindow

import os, cPickle


#----------------------------------------------------------------------

wx.RegisterId(5000)  # Give a high starting value for the IDs, just for kicks

idNEW    = wx.NewId()
idOPEN   = wx.NewId()
idSAVE   = wx.NewId()
idSAVEAS = wx.NewId()
idCLEAR  = wx.NewId()
idEXIT   = wx.NewId()
idABOUT  = wx.NewId()



class DoodleFrame(wx.Frame):
    """
    A DoodleFrame contains a DoodleWindow and a ControlPanel and manages
    their layout with a wx.BoxSizer.  A menu and associated event handlers
    provides for saving a doodle to a file, etc.
    """
    title = "Do a doodle"
    def __init__(self, parent):
        wx.Frame.__init__(self, parent, -1, self.title, size=(800,600),
                         style=wx.DEFAULT_FRAME_STYLE | wx.NO_FULL_REPAINT_ON_RESIZE)
        self.CreateStatusBar()
        self.MakeMenu()
        self.filename = None

        self.doodle = DoodleWindow(self, -1)
        cPanel = ControlPanel(self, -1, self.doodle)

        # Create a sizer to layout the two windows side-by-side.
        # Both will grow vertically, the doodle window will grow
        # horizontally as well.
        box = wx.BoxSizer(wx.HORIZONTAL)
        box.Add(cPanel, 0, wx.EXPAND)
        box.Add(self.doodle, 1, wx.EXPAND)

        # Tell the frame that it should layout itself in response to
        # size events.
        self.SetAutoLayout(True)
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
                wx.MessageBox("%s is not a doodle file." % self.filename,
                             "oops!", style=wx.OK|wx.ICON_EXCLAMATION)


    def MakeMenu(self):
        # create the file menu
        menu1 = wx.Menu()

        # Using the "\tKeyName" syntax automatically creates a
        # wx.AcceleratorTable for this frame and binds the keys to
        # the menu items.
        menu1.Append(idOPEN, "&Open\tCtrl-O", "Open a doodle file")
        menu1.Append(idSAVE, "&Save\tCtrl-S", "Save the doodle")
        menu1.Append(idSAVEAS, "Save &As", "Save the doodle in a new file")
        menu1.AppendSeparator()
        menu1.Append(idCLEAR, "&Clear", "Clear the current doodle")
        menu1.AppendSeparator()
        menu1.Append(idEXIT, "E&xit", "Terminate the application")

        # and the help menu
        menu2 = wx.Menu()
        menu2.Append(idABOUT, "&About\tCtrl-H", "Display the gratuitous 'about this app' thingamajig")

        # and add them to a menubar
        menuBar = wx.MenuBar()
        menuBar.Append(menu1, "&File")
        menuBar.Append(menu2, "&Help")
        self.SetMenuBar(menuBar)

        self.Bind(wx.EVT_MENU,   self.OnMenuOpen, id=idOPEN)
        self.Bind(wx.EVT_MENU,   self.OnMenuSave, id=idSAVE)
        self.Bind(wx.EVT_MENU, self.OnMenuSaveAs, id=idSAVEAS)
        self.Bind(wx.EVT_MENU,  self.OnMenuClear, id=idCLEAR)
        self.Bind(wx.EVT_MENU,   self.OnMenuExit, id=idEXIT)
        self.Bind(wx.EVT_MENU,  self.OnMenuAbout, id=idABOUT)



    wildcard = "Doodle files (*.ddl)|*.ddl|All files (*.*)|*.*"

    def OnMenuOpen(self, event):
        dlg = wx.FileDialog(self, "Open doodle file...", os.getcwd(),
                           style=wx.OPEN, wildcard = self.wildcard)
        if dlg.ShowModal() == wx.ID_OK:
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
        dlg = wx.FileDialog(self, "Save doodle as...", os.getcwd(),
                           style=wx.SAVE | wx.OVERWRITE_PROMPT,
                           wildcard = self.wildcard)
        if dlg.ShowModal() == wx.ID_OK:
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


class ControlPanel(wx.Panel):
    """
    This class implements a very simple control panel for the DoodleWindow.
    It creates buttons for each of the colours and thickneses supported by
    the DoodleWindow, and event handlers to set the selected values.  There is
    also a little window that shows an example doodleLine in the selected
    values.  Nested sizers are used for layout.
    """

    BMP_SIZE = 16
    BMP_BORDER = 3

    def __init__(self, parent, ID, doodle):
        wx.Panel.__init__(self, parent, ID, style=wx.RAISED_BORDER, size=(20,20))

        numCols = 4
        spacing = 4

        btnSize = wx.Size(self.BMP_SIZE + 2*self.BMP_BORDER,
                          self.BMP_SIZE + 2*self.BMP_BORDER)

        # Make a grid of buttons for each colour.  Attach each button
        # event to self.OnSetColour.  The button ID is the same as the
        # key in the colour dictionary.
        self.clrBtns = {}
        colours = doodle.menuColours
        keys = colours.keys()
        keys.sort()
        cGrid = wx.GridSizer(cols=numCols, hgap=2, vgap=2)
        for k in keys:
            bmp = self.MakeBitmap(colours[k])
            b = buttons.GenBitmapToggleButton(self, k, bmp, size=btnSize )
            b.SetBezelWidth(1)
            b.SetUseFocusIndicator(False)
            self.Bind(wx.EVT_BUTTON, self.OnSetColour, b)
            cGrid.Add(b, 0)
            self.clrBtns[colours[k]] = b
        self.clrBtns[colours[keys[0]]].SetToggle(True)


        # Make a grid of buttons for the thicknesses.  Attach each button
        # event to self.OnSetThickness.  The button ID is the same as the
        # thickness value.
        self.thknsBtns = {}
        tGrid = wx.GridSizer(cols=numCols, hgap=2, vgap=2)
        for x in range(1, doodle.maxThickness+1):
            b = buttons.GenToggleButton(self, x, str(x), size=btnSize)
            b.SetBezelWidth(1)
            b.SetUseFocusIndicator(False)
            self.Bind(wx.EVT_BUTTON, self.OnSetThickness, b)
            tGrid.Add(b, 0)
            self.thknsBtns[x] = b
        self.thknsBtns[1].SetToggle(True)

        # Make a colour indicator window, it is registerd as a listener
        # with the doodle window so it will be notified when the settings
        # change
        ci = ColourIndicator(self)
        doodle.AddListener(ci)
        doodle.Notify()
        self.doodle = doodle

        # Make a box sizer and put the two grids and the indicator
        # window in it.
        box = wx.BoxSizer(wx.VERTICAL)
        box.Add(cGrid, 0, wx.ALL, spacing)
        box.Add(tGrid, 0, wx.ALL, spacing)
        box.Add(ci, 0, wx.EXPAND|wx.ALL, spacing)
        self.SetSizer(box)
        self.SetAutoLayout(True)

        # Resize this window so it is just large enough for the
        # minimum requirements of the sizer.
        box.Fit(self)



    def MakeBitmap(self, colour):
        """
        We can create a bitmap of whatever we want by simply selecting
        it into a wx.MemoryDC and drawing on it.  In this case we just set
        a background brush and clear the dc.
        """
        bmp = wx.EmptyBitmap(self.BMP_SIZE, self.BMP_SIZE)
        dc = wx.MemoryDC()
        dc.SelectObject(bmp)
        dc.SetBackground(wx.Brush(colour))
        dc.Clear()
        dc.SelectObject(wx.NullBitmap)
        return bmp


    def OnSetColour(self, event):
        """
        Use the event ID to get the colour, set that colour in the doodle.
        """
        colour = self.doodle.menuColours[event.GetId()]
        if colour != self.doodle.colour:
            # untoggle the old colour button
            self.clrBtns[self.doodle.colour].SetToggle(False)
        # set the new colour
        self.doodle.SetColour(colour)


    def OnSetThickness(self, event):
        """
        Use the event ID to set the thickness in the doodle.
        """
        thickness = event.GetId()
        if thickness != self.doodle.thickness:
            # untoggle the old thickness button
            self.thknsBtns[self.doodle.thickness].SetToggle(False)
        # set the new colour
        self.doodle.SetThickness(thickness)



#----------------------------------------------------------------------

class ColourIndicator(wx.Window):
    """
    An instance of this class is used on the ControlPanel to show
    a sample of what the current doodle line will look like.
    """
    def __init__(self, parent):
        wx.Window.__init__(self, parent, -1, style=wx.SUNKEN_BORDER)
        self.SetBackgroundColour(wx.WHITE)
        self.SetSize( (45, 45) )
        self.colour = self.thickness = None
        self.Bind(wx.EVT_PAINT, self.OnPaint)


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
        dc = wx.PaintDC(self)
        if self.colour:
            sz = self.GetClientSize()
            pen = wx.Pen(self.colour, self.thickness)
            dc.BeginDrawing()
            dc.SetPen(pen)
            dc.DrawLine(10, sz.height/2, sz.width-10, sz.height/2)
            dc.EndDrawing()


#----------------------------------------------------------------------

class DoodleAbout(wx.Dialog):
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
&copy; 1997-2003.</p>
</body>
</html>
'''

    def __init__(self, parent):
        wx.Dialog.__init__(self, parent, -1, 'About SuperDoodle',
                          size=(420, 380) )

        html = wx.html.HtmlWindow(self, -1)
        html.SetPage(self.text)
        button = wx.Button(self, wx.ID_OK, "Okay")

        # constraints for the html window
        lc = wx.LayoutConstraints()
        lc.top.SameAs(self, wx.Top, 5)
        lc.left.SameAs(self, wx.Left, 5)
        lc.bottom.SameAs(button, wx.Top, 5)
        lc.right.SameAs(self, wx.Right, 5)
        html.SetConstraints(lc)

        # constraints for the button
        lc = wx.LayoutConstraints()
        lc.bottom.SameAs(self, wx.Bottom, 5)
        lc.centreX.SameAs(self, wx.CentreX)
        lc.width.AsIs()
        lc.height.AsIs()
        button.SetConstraints(lc)

        self.SetAutoLayout(True)
        self.Layout()
        self.CentreOnParent(wx.BOTH)


#----------------------------------------------------------------------

class DoodleApp(wx.App):
    def OnInit(self):
        frame = DoodleFrame(None)
        frame.Show(True)
        self.SetTopWindow(frame)
        return True


#----------------------------------------------------------------------

if __name__ == '__main__':
    app = DoodleApp(redirect=True)
    app.MainLoop()
