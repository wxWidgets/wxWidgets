
import wx
import images

#----------------------------------------------------------------------

CUSTOMID = 1111

cursors = {
    "wx.CURSOR_ARROW" : wx.CURSOR_ARROW,
    "wx.CURSOR_RIGHT_ARROW" : wx.CURSOR_RIGHT_ARROW,
    "wx.CURSOR_BULLSEYE" : wx.CURSOR_BULLSEYE,
    "wx.CURSOR_CHAR" : wx.CURSOR_CHAR,
    "wx.CURSOR_CROSS" : wx.CURSOR_CROSS,
    "wx.CURSOR_HAND" : wx.CURSOR_HAND,
    "wx.CURSOR_IBEAM" : wx.CURSOR_IBEAM,
    "wx.CURSOR_LEFT_BUTTON" : wx.CURSOR_LEFT_BUTTON,
    "wx.CURSOR_MAGNIFIER" : wx.CURSOR_MAGNIFIER,
    "wx.CURSOR_MIDDLE_BUTTON" : wx.CURSOR_MIDDLE_BUTTON,
    "wx.CURSOR_NO_ENTRY" : wx.CURSOR_NO_ENTRY,
    "wx.CURSOR_PAINT_BRUSH" : wx.CURSOR_PAINT_BRUSH,
    "wx.CURSOR_PENCIL" : wx.CURSOR_PENCIL,
    "wx.CURSOR_POINT_LEFT" : wx.CURSOR_POINT_LEFT,
    "wx.CURSOR_POINT_RIGHT" : wx.CURSOR_POINT_RIGHT,
    "wx.CURSOR_QUESTION_ARROW" : wx.CURSOR_QUESTION_ARROW,
    "wx.CURSOR_RIGHT_BUTTON" : wx.CURSOR_RIGHT_BUTTON,
    "wx.CURSOR_SIZENESW" : wx.CURSOR_SIZENESW,
    "wx.CURSOR_SIZENS" : wx.CURSOR_SIZENS,
    "wx.CURSOR_SIZENWSE" : wx.CURSOR_SIZENWSE,
    "wx.CURSOR_SIZEWE" : wx.CURSOR_SIZEWE,
    "wx.CURSOR_SIZING" : wx.CURSOR_SIZING,
    "wx.CURSOR_SPRAYCAN" : wx.CURSOR_SPRAYCAN,
    "wx.CURSOR_WAIT" : wx.CURSOR_WAIT,
    "wx.CURSOR_WATCH" : wx.CURSOR_WATCH,
    "wx.CURSOR_BLANK" : wx.CURSOR_BLANK,
    "wx.CURSOR_DEFAULT" : wx.CURSOR_DEFAULT,
    "wx.CURSOR_COPY_ARROW" : wx.CURSOR_COPY_ARROW,
    "wx.CURSOR_ARROWWAIT" : wx.CURSOR_ARROWWAIT,

    "zz [custom cursor]"  : CUSTOMID,
}


class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        # create a list of choices from the dictionary above
        choices = cursors.keys()
        choices.sort()

        # create the controls
        self.cb = wx.ComboBox(self, -1, "wx.CURSOR_DEFAULT", choices=choices,
                              style=wx.CB_READONLY)
        self.tx = wx.StaticText(self, -1,                                
             "This sample allows you to see all the stock cursors \n"
             "available to wxPython.  Simply select a name from the \n"
             "wx.Choice and then move the mouse into the window \n"
             "below to see the cursor.  NOTE: not all stock cursors \n"
             "have a specific representaion on all platforms.")
        
        self.win = wx.Window(self, -1, size=(200,200), style=wx.SIMPLE_BORDER)
        self.win.SetBackgroundColour("white")

        # bind an event or two
        self.Bind(wx.EVT_COMBOBOX, self.OnChooseCursor, self.cb)
        self.win.Bind(wx.EVT_LEFT_DOWN, self.OnDrawDot)
        

        # Setup the layout
        gbs = wx.GridBagSizer()
        gbs.Add(self.cb, (2,1))
        gbs.Add(self.tx, (2,3))
        gbs.Add(self.win, (5,0), (1, 6), wx.ALIGN_CENTER)
        self.SetSizer(gbs)


    def OnChooseCursor(self, evt):
        # clear the dots
        self.win.Refresh()
        
        choice = self.cb.GetStringSelection()
        self.log.write("Selecting the %s cursor\n" % choice)

        cnum = cursors[choice]
        
        if cnum == CUSTOMID:
            image = images.getBlom12Image()
            image.SetMaskColour(255, 255, 255)

            # since this image didn't come from a .cur file, tell it where the hotspot is
            image.SetOptionInt(wx.IMAGE_OPTION_CUR_HOTSPOT_X, 0)
            image.SetOptionInt(wx.IMAGE_OPTION_CUR_HOTSPOT_Y, 22)

            # make the image into a cursor
            cursor = wx.CursorFromImage(image)

        else:
            # create one of the stock (built-in) cursors
            cursor = wx.StockCursor(cnum)

        # set the cursor for the window
        self.win.SetCursor(cursor)


    def OnDrawDot(self, evt):
        # Draw a dot so the user can see where the hotspot is
        dc = wx.ClientDC(self.win)
        dc.SetPen(wx.Pen("RED"))
        dc.SetBrush(wx.Brush("RED"))
        pos = evt.GetPosition()
        dc.DrawCircle(pos.x, pos.y, 4)
        

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>wx.Cursor</center></h2>

This demo shows the stock mouse cursors that are available to wxPython.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

