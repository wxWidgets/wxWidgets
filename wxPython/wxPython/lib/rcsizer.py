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
        items = self.GetChildren()
        if not items:
            return wxSize(10, 10)

        self.rowHeights = []
        self.colWidths = []

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
            rpos[i] = px
            px += height

        for i in range(len(self.colWidths)):
            width = self.colWidths[i]
            cpos[i] = py
            py += width

        # iterate children and set dimensions...
        for item in self.GetChildren():
            r, c, r2, c2 = item.GetUserData()
            width = reduce( operator.add, self.colWidths[c:c2] )
            height = reduce( operator.add, self.rowHeights[r:r2] )
            #item.SetDimension( (cpos[c], rpos[r]), (width, height))
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




##     #--------------------------------------------------
##     def _add( self, size, opt, dim ):
##         r,c,r2,c2 = dim
##         if r2 > len(self.rows0):
##             x = [self.row_h] * (r2-len(self.rows0))
##             self.rows0.extend( x )
##             self.rows1.extend( x )
##         if c2 > len(self.cols0):
##             x = [self.col_w] * (c2-len(self.cols0))
##             self.cols0.extend( x )
##             self.cols1.extend( x )
##         if opt == 0:    # fixed
##             scale = (r2-r)
##             for i in range(r,r2):
##                 self.rows1[i] = self.rows0[i] = max( self.rows0[i], size.y/scale )
##             scale = (c2-c)
##             for i in range(c,c2):
##                 self.cols1[i] = self.cols0[i] = max( self.cols0[i], size.x/scale )
##         else:
##             scale = (r2-r)
##             for i in range(r,r2):
##                 self.rows0[i] = max( self.rows0[i], size.y/scale )
##                 self.rows1[i] = self.rows0[i] * opt
##             scale = (c2-c)
##             for i in range(c,c2):
##                 self.cols0[i] = max( self.cols0[i], size.x/scale )
##                 self.cols1[i] = self.cols0[i] * opt


##     #--------------------------------------------------
##     def CalcMin( self ):
##         children = self.GetChildren()
##         if not children:
##                 return wxSize(10, 10)

##         self.rows0 = []
##         self.cols0 = []
##         self.rows1 = []
##         self.cols1 = []

##         for cell in children:
##             self._add( cell.CalcMin(), cell.GetOption(), cell.GetUserData() )

##         self.minWidth    = reduce( operator.add, self.cols1 )
##         self.minHeight   = reduce( operator.add, self.rows1 )
##         self.fixedWidth  = reduce( operator.add, self.cols0 )   # size without stretched widgets
##         self.fixedHeight = reduce( operator.add, self.rows0 )

##         return wxSize( self.minWidth, self.minHeight )


##     #--------------------------------------------------
##     def RecalcSizes( self ):
##         # get current dimensions, save for performance
##         myWidth  = self.GetSize().x
##         myHeight = self.GetSize().y

##         # relative recent positions
##         px = self.GetPosition().x
##         py = self.GetPosition().y

##         # calculate space for one stretched item
##         stretchC = 0
##         for i in range(len(self.cols0)):
##             if self.cols1[i] <> self.cols0[i]:
##                 stretchC = stretchC + self.cols1[i] / self.cols0[i]
##         if myWidth > self.fixedWidth and stretchC:
##             deltaw = (myWidth - self.fixedWidth) / stretchC
##             extraw = (myWidth - self.fixedWidth) % stretchC
##         else:
##             deltaw = extraw = 0

##         stretchR = 0
##         for i in range(len(self.rows0)):
##             if self.rows1[i] <> self.rows0[i]:
##                 stretchR = stretchR + self.rows1[i] / self.rows0[i]
##         if myHeight > self.fixedHeight and stretchR:
##             deltah = (myHeight - self.fixedHeight) / stretchR
##             extrah = (myHeight - self.fixedHeight) % stretchR
##         else:
##             deltah = extrah = 0

##         self.rpos = [0] * len( self.rows0 )
##         self.cpos = [0] * len( self.cols0 )

##         for i in range(len(self.rows0)):
##             newHeight = self.rows0[i]
##             if self.rows1[i] <> self.rows0[i]:
##                 weight = self.rows1[i] / self.rows0[i]
##                 # first stretchable gets extra pixels
##                 newHeight = newHeight + (deltah * weight) + extrah
##                 extrah = 0
##             self.rpos[i] = py
##             self.rows1[i] = newHeight
##             py = py + newHeight

##         for i in range(len(self.cols0)):
##             newWidth = self.cols0[i]
##             if self.cols1[i] <> self.cols0[i]:
##                 weight = self.cols1[i] / self.cols0[i]
##                 # first stretchable gets extra pixels
##                 newWidth = newWidth + (deltaw * weight) + extraw
##                 extraw = 0
##             self.cpos[i] = px
##             self.cols1[i] = newWidth
##             px = px + newWidth

##         # iterate children ...
##         for cell in self.GetChildren():
##             r,c,r2,c2 = cell.GetUserData()
##             newWidth = reduce( operator.add, self.cols1[c:c2] )
##             newHeight = reduce( operator.add, self.rows1[r:r2] )
##             cell.SetDimension( (self.cpos[c], self.rpos[r]), (newWidth, newHeight) )

