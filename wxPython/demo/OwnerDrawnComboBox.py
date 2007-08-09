
import wx
import wx.combo

#----------------------------------------------------------------------
# This ComboBox class graphically displays the various pen styles that
# are available, making it easy for the user to choose the style they
# want.

class PenStyleComboBox(wx.combo.OwnerDrawnComboBox):

    # Overridden from OwnerDrawnComboBox, called to draw each
    # item in the list
    def OnDrawItem(self, dc, rect, item, flags):
        if item == wx.NOT_FOUND:
            # painting the control, but there is no valid item selected yet
            return

        r = wx.Rect(*rect)  # make a copy
        r.Deflate(3, 5)

        penStyle = wx.SOLID
        if item == 1:
            penStyle = wx.TRANSPARENT
        elif item == 2:
            penStyle = wx.DOT
        elif item == 3:
            penStyle = wx.LONG_DASH
        elif item == 4:
            penStyle = wx.SHORT_DASH
        elif item == 5:
            penStyle = wx.DOT_DASH
        elif item == 6:
            penStyle = wx.BDIAGONAL_HATCH
        elif item == 7:
            penStyle = wx.CROSSDIAG_HATCH
        elif item == 8:
            penStyle = wx.FDIAGONAL_HATCH
        elif item == 9:
            penStyle = wx.CROSS_HATCH
        elif item == 10:
            penStyle = wx.HORIZONTAL_HATCH
        elif item == 11:
            penStyle = wx.VERTICAL_HATCH
            
        pen = wx.Pen(dc.GetTextForeground(), 3, penStyle)
        dc.SetPen(pen)

        if flags & wx.combo.ODCB_PAINTING_CONTROL:
           # for painting the control itself
           dc.DrawLine( r.x+5, r.y+r.height/2, r.x+r.width - 5, r.y+r.height/2 )

        else:
            # for painting the items in the popup
            dc.DrawText(self.GetString( item ),
                        r.x + 3,
                        (r.y + 0) + ( (r.height/2) - dc.GetCharHeight() )/2
                        )
            dc.DrawLine( r.x+5, r.y+((r.height/4)*3)+1, r.x+r.width - 5, r.y+((r.height/4)*3)+1 )

           
    # Overridden from OwnerDrawnComboBox, called for drawing the
    # background area of each item.
    def OnDrawBackground(self, dc, rect, item, flags):
        # If the item is selected, or its item # iseven, or we are painting the
        # combo control itself, then use the default rendering.
        if (item & 1 == 0 or flags & (wx.combo.ODCB_PAINTING_CONTROL |
                                      wx.combo.ODCB_PAINTING_SELECTED)):
            wx.combo.OwnerDrawnComboBox.OnDrawBackground(self, dc, rect, item, flags)
            return

        # Otherwise, draw every other background with different colour.
        bgCol = wx.Colour(240,240,250)
        dc.SetBrush(wx.Brush(bgCol))
        dc.SetPen(wx.Pen(bgCol))
        dc.DrawRectangleRect(rect);



    # Overridden from OwnerDrawnComboBox, should return the height
    # needed to display an item in the popup, or -1 for default
    def OnMeasureItem(self, item):
        # Simply demonstrate the ability to have variable-height items
        if item & 1:
            return 36
        else:
            return 24

    # Overridden from OwnerDrawnComboBox.  Callback for item width, or
    # -1 for default/undetermined
    def OnMeasureItemWidth(self, item):
        return -1; # default - will be measured from text width
    

        

#----------------------------------------------------------------------

class TestPanel(wx.Panel):
    def __init__(self, parent, log):
        self.log = log
        wx.Panel.__init__(self, parent, -1)

        penStyles = [
            "Solid",
            "Transparent",
            "Dot",
            "Long Dash",
            "Short Dash",
            "Dot Dash",
            "Backward Diagonal Hatch",
            "Cross-diagonal Hatch",
            "Forward Diagonal Hatch",
            "Cross Hatch",
            "Horizontal Hatch",
            "Vertical Hatch",
            ]

        wx.StaticText(self, -1, "Pen Styles:", (20, 20))
        pscb = PenStyleComboBox(self, choices=penStyles, style=wx.CB_READONLY,
                                pos=(20,40), size=(250, -1))

        self.pscb = pscb

#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TestPanel(nb, log)
    return win

#----------------------------------------------------------------------



overview = """<html><body>
<h2><center>wx.combo.OwnerDrawnComboBox</center></h2>

wx.combo.OwnerDrawnComboBox is a combobox with owner-drawn list
items. In essence, it is a wx.combo.ComboCtrl with wx.VListBox popup and
wx.ControlWithItems interface.

</body></html>
"""



if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

