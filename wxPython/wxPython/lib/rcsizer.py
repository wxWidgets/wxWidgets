#----------------------------------------------------------------------
# Name:        wxPython.lib.rcsizer
# Purpose:     RowColSizer:
#
# Author:      Robin Dunn, adapted from code by Niki Spahiev
#
# Created:     26-Feb-2002
# RCS-ID:      $Id$
# Copyright:   (c) 2002 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------

"""
A pure-Python wxSizer that lays out items in a grid similar to
wxFlexGridSizer but item position is not implicit but explicitly
specified by row and col, and row/col spanning is supported.

Adapted from code by Niki Spahiev.

If anyone is interested, it would be nice to have this ported to C++.
"""



from wxPython.wx import *
import operator

#----------------------------------------------------------------------

class RowColSizer(wxPySizer):

    # default sizes for cells with no item
    col_w = 10
    row_h = 22

    def __init__(self):
        wxPySizer.__init__(self)
        self.growableRows = []
        self.growableCols = []


    def AddGrowableRow(self, idx):
        self.growableRows.append(idx)

    def AddGrowableCol(self, idx):
        self.growableCols.append(idx)



    #--------------------------------------------------
    def Add(self, item, option=0, flag=0, border=0,
            row=-1, col=-1,       # row, col and spanning can be specified individually...
            rowspan=1, colspan=1,
            pos=None, size=None,  # or as tuples (row,col) and (rowspan,colspan)
            ):

        if pos is not None:
            row, col = pos
        if size is not None:
            rowspan, colspan = size

        assert row != -1, "Row must be specified"
        assert col != -1, "Column must be specified"

        # Do I really want to do this?  Probably not...
        #if rowspan > 1 or colspan > 1:
        #    flag = flag | wxEXPAND

        wxPySizer.Add(self, item, option, flag, border,
                      userData=(row, col, row+rowspan, col+colspan))

    #AddWindow = Add
    #AddSizer  = Add

    def AddSpacer(self, width, height, option=0, flag=0, border=0,
                  row=-1, col=-1,
                  rowspan=1, colspan=1,
                  pos=None, size=None,
                  ):
        if pos is not None:
            row, col = pos
        if size is not None:
            rowspan, colspan = size

        assert row != -1, "Row must be specified"
        assert col != -1, "Column must be specified"

        wxPySizer.AddSpacer(self, width, height, option, flag, border,
                            userData=(row, col, row+rowspan, col+colspan))

    #--------------------------------------------------
    def _add( self, size, dim ):
        r, c, r2, c2 = dim   # unpack coords and spanning

        # are the widths and heights lists long enough?
        if r2 > len(self.rowHeights):
            x = [self.row_h] * (r2-len(self.rowHeights))
            self.rowHeights.extend( x )
        if c2 > len(self.colWidths):
            x = [self.col_w] * (c2-len(self.colWidths))
            self.colWidths.extend( x )

        # set the widths and heights lists for this item
        scale = (r2 - r)
        for i in range(r, r2):
            self.rowHeights[i] = max( self.rowHeights[i], size.height / scale )
        scale = (c2 - c)
        for i in range(c, c2):
            self.colWidths[i] = max( self.colWidths[i], size.width / scale )


    #--------------------------------------------------
    def CalcMin( self ):
        self.rowHeights = []
        self.colWidths = []

        items = self.GetChildren()
        if not items:
            return wxSize(10, 10)

        for item in items:
            self._add( item.CalcMin(), item.GetUserData() )

        size = wxSize( reduce( operator.add, self.colWidths),
                       reduce( operator.add, self.rowHeights) )
        return size


    #--------------------------------------------------
    def RecalcSizes( self ):
        # save current dimensions, etc.
        curWidth  = self.GetSize().width
        curHeight = self.GetSize().height
        px = self.GetPosition().x
        py = self.GetPosition().y
        minWidth  = self.CalcMin().width
        minHeight = self.CalcMin().height

        # Check for growables
        if self.growableRows and curHeight > minHeight:
            delta = (curHeight - minHeight) / len(self.growableRows)
            extra = (curHeight - minHeight) % len(self.growableRows)
            for idx in self.growableRows:
                self.rowHeights[idx] += delta
            self.rowHeights[self.growableRows[0]] += extra

        if self.growableCols and curWidth > minWidth:
            delta = (curWidth - minWidth) / len(self.growableCols)
            extra = (curWidth - minWidth) % len(self.growableCols)
            for idx in self.growableCols:
                self.colWidths[idx] += delta
            self.colWidths[self.growableCols[0]] += extra

        rpos = [0] * len(self.rowHeights)
        cpos = [0] * len(self.colWidths)

        for i in range(len(self.rowHeights)):
            height = self.rowHeights[i]
            rpos[i] = py
            py += height

        for i in range(len(self.colWidths)):
            width = self.colWidths[i]
            cpos[i] = px
            px += width

        # iterate children and set dimensions...
        for item in self.GetChildren():
            r, c, r2, c2 = item.GetUserData()
            width = reduce( operator.add, self.colWidths[c:c2] )
            height = reduce( operator.add, self.rowHeights[r:r2] )
            self.SetItemBounds( item, cpos[c], rpos[r], width, height )


    #--------------------------------------------------
    def SetItemBounds(self, item, x, y, w, h):
        # calculate the item's actual size and position within
        # its grid cell
        ipt = wxPoint(x, y)
        isz = item.CalcMin()
        flag = item.GetFlag()

        if flag & wxEXPAND or flag & wxSHAPED:
            isz = wxSize(w, h)
        else:
            if flag & wxALIGN_CENTER_HORIZONTAL:
                ipt.x = x + (w - isz.width) / 2
            elif flag & wxALIGN_RIGHT:
                ipt.x = x + (w - isz.width)

            if flag & wxALIGN_CENTER_VERTICAL:
                ipt.y = y + (h - isz.height) / 2
            elif flag & wxALIGN_BOTTOM:
                ipt.y = y + (h - isz.height)

        item.SetDimension(ipt, isz)


#----------------------------------------------------------------------
#----------------------------------------------------------------------



