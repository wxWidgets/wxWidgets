#----------------------------------------------------------------------------
# Name:         printout.py
# Purpose:      preview and printing class -> table/grid printing
#
# Author:       Lorne White (email: lorne.white@telusplanet.net)
#
# Created:
# Version       0.72
# Date:         Sept 8, 2001
# Licence:      wxWindows license
#----------------------------------------------------------------------------

import os, sys, string, copy

from wxPython.wx import *
import copy

class PrintBase:
    def OutTextRegion(self, textout, txtdraw = TRUE):
        textlines = string.splitfields(textout, '\n')
        y = copy.copy(self.y) + self.pt_space_before
        for text in textlines:
            remain = 'X'
            while remain != "":
                vout, remain = self.SetFlow(text, self.region)
                if self.draw == TRUE and txtdraw == TRUE:
                    test_out = self.TestFull(vout)
                    if self.align == wxALIGN_LEFT:
                        self.DC.DrawText(test_out, self.indent+self.pcell_left_margin, y)

                    elif self.align == wxALIGN_CENTRE:
                        diff = self.GetCellDiff(test_out, self.region)
                        self.DC.DrawText(test_out, self.indent+diff/2, y)

                    elif self.align == wxALIGN_RIGHT:
                        diff = self.GetCellDiff(test_out, self.region)
                        self.DC.DrawText(test_out, self.indent+diff, y)

                    else:
                        self.DC.DrawText(test_out, self.indent+self.pcell_left_margin, y)
                text = remain
                y = y + self.space
        return y - self.space + self.pt_space_after

    def GetCellDiff(self, text, width):      # get the remaining cell size for adjustment
        w, h = self.DC.GetTextExtent(text)
        diff = width - w
        if diff < 0:
            diff = 0
        return diff

    def TestFull(self, text_test):
        w, h = self.DC.GetTextExtent(text_test)
        if w > self.region:     # trouble fitting into cell
            return self.SetChar(text_test, self.region)     # fit the text to the cell size
        else:
            return text_test

    def SetFlow(self, ln_text, width):
        width = width - self.pcell_right_margin
        text = ""
        split = string.split(ln_text)
        if len(split) == 1:
            return ln_text, ""

        cnt = 0
        for word in split:
            bword = " " + word	# blank + word
            length = len(bword)

            w, h = self.DC.GetTextExtent(text + bword)
            if w < width:
                text = text + bword
                cnt = cnt + 1
            else:
                remain = string.joinfields(split[cnt:],' ')
                text = string.strip(text)
                return text, remain

        remain = string.joinfields(split[cnt:],' ')
        vout = string.strip(text)
        return vout, remain

    def SetChar(self, ln_text, width):  # truncate string to fit into width
        width = width - self.pcell_right_margin - self.pcell_left_margin
        text = ""
        for val in ln_text:
            w, h = self.DC.GetTextExtent(text + val)
            if w > width:
                text = text + ".."
                return text     # fitted text value
            text = text + val
        return text

    def OutTextPageWidth(self, textout, y_out, align, indent, txtdraw = TRUE):
        textlines = string.splitfields(textout, '\n')
        y = copy.copy(y_out)

        pagew = self.parent.page_width * self.pwidth        # full page width
        w, h = self.DC.GetTextExtent(textout)
        y_line = h

        for text in textlines:
            remain = 'X'
            while remain != "":
                vout, remain = self.SetFlow(text, pagew)
                if self.draw == TRUE and txtdraw == TRUE:
                    test_out = vout
                    if align == wxALIGN_LEFT:
                        self.DC.DrawText(test_out, indent, y)

                    elif align == wxALIGN_CENTRE:
                        diff = self.GetCellDiff(test_out, pagew)
                        self.DC.DrawText(test_out, indent+diff/2, y)

                    elif align == wxALIGN_RIGHT:
                        diff = self.GetCellDiff(test_out, pagew)
                        self.DC.DrawText(test_out, indent+diff, y)

                    else:
                        self.DC.DrawText(test_out, indent, y_out)
                text = remain
                y = y + y_line
        return y - y_line

    def SetPreview(self, preview):
        self.preview = preview

    def SetPSize(self, width, height):
        self.pwidth = width/self.scale
        self.pheight = height/self.scale

    def SetScale(self, scale):
        self.scale = scale

    def SetPTSize(self, width, height):
        self.ptwidth = width
        self.ptheight = height

    def getWidth(self):
        return self.sizew

    def getHeight(self):
        return self.sizeh


class PrintTableDraw(wxScrolledWindow, PrintBase):
    def __init__(self, parent, DC, size):
        self.parent = parent
        self.DC = DC
        self.scale = parent.scale
        self.width = size[0]
        self.height = size[1]
        self.SetDefaults()

    def SetDefaults(self):
        self.page = 1
        self.total_pages = None

        self.page_width = self.parent.page_width
        self.page_height = self.parent.page_height

        self.left_margin = self.parent.left_margin
        self.right_margin = self.parent.right_margin

        self.top_margin  = self.parent.top_margin
        self.bottom_margin = self.parent.bottom_margin
        self.cell_left_margin = self.parent.cell_left_margin
        self.cell_right_margin = self.parent.cell_right_margin

        self.label_colour = self.parent.label_colour

        self.row_line_colour = self.parent.row_line_colour
        self.row_line_size = self.parent.row_line_size

        self.row_def_line_colour = self.parent.row_def_line_colour
        self.row_def_line_size = self.parent.row_def_line_size

        self.column_line_colour = self.parent.column_line_colour
        self.column_line_size = self.parent.column_line_size

        self.column_def_line_size = self.parent.column_def_line_size
        self.column_def_line_colour = self.parent.column_def_line_colour

        self.text_font = self.parent.text_font
        self.text_font_name = self.parent.text_font_name
        self.text_font_colour = self.parent.text_font_colour

        self.label_font = self.parent.label_font
        self.label_font_name = self.parent.label_font_name
        self.label_font_colour = self.parent.label_font_colour

    def AdjustValues(self):
        self.vertical_offset = self.pheight * self.parent.vertical_offset
        self.horizontal_offset = self.pheight * self.parent.horizontal_offset

        self.pcell_left_margin = self.pwidth * self.cell_left_margin
        self.pcell_right_margin = self.pwidth * self.cell_right_margin
        self.ptop_margin = self.pheight * self.top_margin
        self.pbottom_margin = self.pheight * self.bottom_margin

        self.pheader_margin = self.pheight * self.parent.header_margin
        self.pfooter_margin = self.pheight * self.parent.footer_margin

        self.cell_colour = self.parent.set_cell_colour
        self.cell_text = self.parent.set_cell_text

        self.column = []
        self.column_align = []
        self.column_bgcolour = []
        self.column_txtcolour = []

        set_column_align = self.parent.set_column_align
        set_column_bgcolour = self.parent.set_column_bgcolour
        set_column_txtcolour = self.parent.set_column_txtcolour

        pos_x = self.left_margin * self.pwidth + self.horizontal_offset     # left margin
        self.column.append(pos_x)

        if self.set_column == []:
            table_width = self.page_width - self.left_margin - self.right_margin
            width = table_width/(len(self.label))
            for val in self.label:
                column_width = width * self.pwidth
                pos_x = pos_x + column_width
                self.column.append(pos_x)   # position of each column
        else:
            for val in self.set_column:
                column_width = val * self.pwidth
                pos_x = pos_x + column_width
                self.column.append(pos_x)   # position of each column

        if pos_x > self.page_width * self.pwidth:    # check if it fits in page
            print "Warning, Too Wide for Page"
            return

        if self.label != []:
            if len(self.column) -1 != len(self.label):
                print "Column Settings Incorrect", "\nColumn Value: " + str(self.column), "\nLabel Value: " + str(self.label)
                return

        first_value = self.data[0]
        column_total = len(first_value)
        if column_total != len(self.column) -1:
            print "Column Settings Incorrect", first_value, self.column
            return

        col = 0
        for col in range(column_total):
            try:
                align = set_column_align[col]       # check if custom column alignment
            except:
                align = wxALIGN_LEFT
            self.column_align.append(align)

            try:
                colour = set_column_bgcolour[col]     # check if custom column background colour
            except:
                colour = self.parent.column_colour
            self.column_bgcolour.append(colour)

            try:
                colour = set_column_txtcolour[col]     # check if custom column text colour
            except:
                colour = self.parent.text_font_colour
            self.column_txtcolour.append(colour)

            col = col + 1

    def SetPointAdjust(self):
        f = wxFont(10, wxSWISS, wxNORMAL, wxNORMAL)     # setup using 10 point
        self.DC.SetFont(f)
        f.SetFaceName(self.text_font_name)
        x, y = self.DC.GetTextExtent("W")

        self.label_pt_space_before = self.parent.label_pt_adj_before * y/10        # extra spacing for label per point value
        self.label_pt_space_after = self.parent.label_pt_adj_after * y/10

        self.text_pt_space_before = self.parent.text_pt_adj_before * y/10        # extra spacing for row text per point value
        self.text_pt_space_after = self.parent.text_pt_adj_after * y/10

    def SetPage(self, page):
        self.page = page

    def SetColumns(self, col):
        self.column = col

    def OutCanvas(self):
        self.AdjustValues()
        self.SetPointAdjust()

        self.y_start = self.ptop_margin + self.vertical_offset
        self.y_end = self.parent.page_height * self.pheight - self.pbottom_margin + self.vertical_offset

        self.text_font.SetFaceName(self.label_font_name)
        self.DC.SetFont(self.label_font)
        x, y = self.DC.GetTextExtent("W")
        self.label_space = y

        self.text_font.SetFaceName(self.text_font_name)
        self.DC.SetFont(self.text_font)
        x, y = self.DC.GetTextExtent("W")
        self.space = y

        if self.total_pages == None:
            self.GetTotalPages()    # total pages for display/printing

        self.data_cnt = 0

        cnt = 1                 # get selected page
        if cnt == self.page:
            self.draw = TRUE
        else:
            self.draw = FALSE

        while cnt < self.page:
            self.OutPage()
            cnt = cnt + 1

        self.draw = TRUE
        self.PrintHeader()
        self.PrintFooter()
        self.OutPage()


    def GetTotalPages(self):
        self.data_cnt = 0
        self.draw = FALSE

        cnt = 0
        while 1:
            test = self.OutPage()
            if  test == FALSE:
                break
            cnt = cnt + 1

        self.total_pages = cnt + 1

    def OutPage(self):
        self.y = self.y_start
        self.end_x = self.column[-1]

        if self.data_cnt < len(self.data)-1:  # if there data for display on the page
            if self.label != []:        # check if header defined
                self.PrintLabel()
        else:
            return FALSE

        for val in self.data:
            try:
                row_val = self.data[self.data_cnt]
            except:
                self.FinishDraw()
                return FALSE

            max_y = self.PrintRow(row_val, FALSE)       # test to see if row will fit in remaining space
            test = max_y + self.space

            if test > self.y_end:
                break

            self.ColourRowCells(max_y-self.y+self.space)       # colour the row/column
            max_y = self.PrintRow(row_val, TRUE)      # row fits - print text
            self.DrawGridLine()     # top line of cell
            self.y = max_y + self.space

            if self.y > self.y_end:
                break

            self.data_cnt = self.data_cnt + 1

        self.FinishDraw()

        if self.data_cnt == len(self.data):    # last value in list
            return FALSE

        return TRUE


    def PrintLabel(self):
        self.pt_space_before = self.label_pt_space_before   # set the point spacing
        self.pt_space_after = self.label_pt_space_after

        self.LabelColorRow(self.label_colour)
        self.label_font.SetFaceName(self.label_font_name)
        self.DC.SetFont(self.label_font)
        self.DC.SetTextForeground(self.label_font_colour)

        self.col = 0
        max_y = 0
        for vtxt in self.label:
            self.region = self.column[self.col+1] - self.column[self.col]
            self.indent = self.column[self.col]

            self.align = wxALIGN_LEFT

            max_out = self.OutTextRegion(vtxt, TRUE)
            if max_out > max_y:
                max_y = max_out
            self.col = self.col + 1

        self.DrawGridLine()     # top line of label
        self.y = max_y + self.label_space

    def PrintHeader(self):      # print the header array
        if self.draw == FALSE:
            return

        for val in self.parent.header:
            f = wxFont(val["Size"], wxSWISS, wxNORMAL, val["Attr"])
            self.DC.SetFont(f)
            fontname = val["Name"]

            f.SetFaceName(fontname)
            self.DC.SetTextForeground(val["Colour"])

            header_indent = val["Indent"] * self.pwidth
            self.OutTextPageWidth(val["Text"], self.pheader_margin, val["Align"], header_indent, TRUE)

    def PrintFooter(self):      # print the header array
        if self.draw == FALSE:
            return

        footer_pos = self.parent.page_height * self.pheight - self.pfooter_margin + self.vertical_offset
        for val in self.parent.footer:
            f = wxFont(val["Size"], wxSWISS, wxNORMAL, val["Attr"])
            self.DC.SetFont(f)
            fontname = val["Name"]

            f.SetFaceName(fontname)
            self.DC.SetTextForeground(val["Colour"])

            footer_indent = val["Indent"] * self.pwidth
            ftype = val["Type"]
            if ftype == "Pageof":
                text = "Page " + str(self.page) + " of " + str(self.total_pages)
            elif ftype == "Page":
                text = "Page " + str(self.page)
            elif ftype == "Num":
                text = str(self.page)
            else:
                text = ""

            self.OutTextPageWidth(text, footer_pos, val["Align"], footer_indent, TRUE)

    def LabelColorRow(self, colour):
        brush = wxBrush(colour, wxSOLID)
        self.DC.SetBrush(brush)
        height = self.label_space + self.label_pt_space_before + self.label_pt_space_after
        self.DC.DrawRectangle(self.column[0], self.y, self.end_x-self.column[0]+1, height)

    def ColourRowCells(self, height):
        if self.draw == FALSE:
            return

        col = 0
        for colour in self.column_bgcolour:
            cellcolour = self.GetCellColour(self.data_cnt, col)
            if cellcolour != None:
                colour = cellcolour

            brush = wxBrush(colour, wxSOLID)
            self.DC.SetBrush(brush)
            self.DC.SetPen(wxPen(wxNamedColour('WHITE'), 0))

            start_x = self.column[col]
            width = self.column[col+1] - start_x + 2
            self.DC.DrawRectangle(start_x, self.y, width, height)
            col = col + 1

    def PrintRow(self, row_val, draw = TRUE, align = wxALIGN_LEFT):
        self.text_font.SetFaceName(self.text_font_name)
        self.DC.SetFont(self.text_font)

        self.pt_space_before = self.text_pt_space_before   # set the point spacing
        self.pt_space_after = self.text_pt_space_after

        self.col = 0
        max_y = 0
        self.DC.SetTextForeground(self.text_font_colour)
        for vtxt in row_val:
            self.region = self.column[self.col+1] - self.column[self.col]
            self.indent = self.column[self.col]
            self.align = self.column_align[self.col]

            fcolour = self.column_txtcolour[self.col]       # set font colour
            celltext = self.GetCellText(self.data_cnt, self.col)
            if celltext != None:
                fcolour = celltext      # override the column colour

            self.DC.SetTextForeground(fcolour)

            max_out = self.OutTextRegion(vtxt, draw)
            if max_out > max_y:
                max_y = max_out
            self.col = self.col + 1
        return max_y

    def GetCellColour(self, row, col):      # check if custom colour defined for the cell background
        try:
            set = self.cell_colour[row]
        except:
            return None
        try:
            colour = set[col]
            return colour
        except:
            return None

    def GetCellText(self, row, col):      # check if custom colour defined for the cell text
        try:
            set = self.cell_text[row]
        except:
            return None
        try:
            colour = set[col]
            return colour
        except:
            return None

    def FinishDraw(self):
        self.DrawGridLine()     # draw last row line
        self.DrawColumns()      # draw all vertical lines

    def DrawGridLine(self):
        if self.draw == TRUE:
            try:
                size = self.row_line_size[self.data_cnt]
            except:
                size = self.row_def_line_size

            try:
                colour = self.row_line_colour[self.data_cnt]
            except:
                colour = self.row_def_line_colour

            self.DC.SetPen(wxPen(colour, size))

            y_out = self.y
#            y_out = self.y + self.pt_space_before + self.pt_space_after     # adjust for extra spacing
            self.DC.DrawLine(self.column[0], y_out, self.end_x, y_out)

    def DrawColumns(self):
        if self.draw == TRUE:
            col = 0
            for val in self.column:
                try:
                    size = self.column_line_size[col]
                except:
                    size = self.column_def_line_size

                try:
                    colour = self.column_line_colour[col]
                except:
                    colour = self.column_def_line_colour

                indent = val

                self.DC.SetPen(wxPen(colour, size))
                self.DC.DrawLine(indent, self.y_start, indent, self.y)
                col = col + 1

    def DrawText(self):
        self.DoRefresh()

    def DoDrawing(self, DC):
        size = DC.GetSizeTuple()
        self.DC = DC

        DC.BeginDrawing()
        self.DrawText()
        DC.EndDrawing()

        self.sizew = DC.MaxY()
        self.sizeh = DC.MaxX()


class PrintTable:
    def __init__(self, parentFrame=None):
        self.data = []
        self.set_column = []
        self.label = []
        self.header = []
        self.footer = []

        self.set_column_align = {}
        self.set_column_bgcolour = {}
        self.set_column_txtcolour = {}
        self.set_cell_colour = {}
        self.set_cell_text = {}
        self.column_line_size = {}
        self.column_line_colour = {}
        self.row_line_size = {}
        self.row_line_colour = {}

        self.parentFrame = parentFrame
        self.SetPreviewSize()

        self.printData = wxPrintData()
        self.scale = 1.0

        self.SetParms()
        self.SetColors()
        self.SetFonts()
        self.TextSpacing()

        self.SetPrinterOffset()
        self.SetHeaderValue()
        self.SetFooterValue()
        self.SetMargins()
        self.SetPortrait()

    def SetPreviewSize(self, position = wxPoint(0, 0), size="Full"):
        if size == "Full":
            screenWidth = int(wx.wxSystemSettings_GetSystemMetric(wx.wxSYS_SCREEN_X))
            screenHeight = int(wx.wxSystemSettings_GetSystemMetric(wx.wxSYS_SCREEN_Y))
            self.preview_frame_size = wxSize(screenWidth, screenHeight)
            self.preview_frame_pos = position
        else:
            self.preview_frame_size = size     
            self.preview_frame_pos = position

    def SetPaperId(self, paper):
        self.printData.SetPaperId(paper)

    def SetOrientation(self, orient):
        self.printData.SetOrientation(orient)

    def SetColors(self):
        self.row_def_line_colour = wxNamedColour('BLACK')
        self.row_def_line_size = 1

        self.column_def_line_colour = wxNamedColour('BLACK')
        self.column_def_line_size = 1
        self.column_colour = wxNamedColour('WHITE')

        self.label_colour = wxNamedColour('LIGHT GREY')

    def SetFonts(self):
        self.label_font_size = 12
        self.label_font_attr = wxNORMAL
        self.label_font_name = "Arial"
        self.label_font_colour = wxNamedColour('BLACK')

        self.text_font_size = 10
        self.text_font_attr = wxNORMAL
        self.text_font_name = "Arial"
        self.text_font_colour = wxNamedColour('BLACK')

    def TextSpacing(self):
        self.label_pt_adj_before = 0     # point adjustment before and after the label text
        self.label_pt_adj_after = 0

        self.text_pt_adj_before = 0     # point adjustment before and after the row text
        self.text_pt_adj_after = 0

    def SetLabelSpacing(self, before, after):        # method to set the label space adjustment
        self.label_pt_adj_before = before
        self.label_pt_adj_after = after

    def SetRowSpacing(self, before, after):         # method to set the row space adjustment
        self.text_pt_adj_before = before
        self.text_pt_adj_after = after

    def SetPrinterOffset(self):        # offset to adjust for printer
        self.vertical_offset = -0.1
        self.horizontal_offset = -0.1

    def SetHeaderValue(self):
        self.header_margin = 0.25
        self.header_font_size = 12
        self.header_font_colour = wxNamedColour('BLACK')
        self.header_font_attr = wxBOLD
        self.header_font_name = self.text_font_name
        self.header_align = wxALIGN_CENTRE
        self.header_indent = 0
        self.header_type = None

    def SetFooterValue(self):
        self.footer_margin = 0.7
        self.footer_font_size = 10
        self.footer_font_colour = wxNamedColour('BLACK')
        self.footer_font_attr = wxNORMAL
        self.footer_font_name = self.text_font_name
        self.footer_align = wxALIGN_CENTRE
        self.footer_indent = 0
        self.footer_type = "Pageof"

    def SetMargins(self):
        self.left_margin = 1.0
        self.right_margin = 1.0     # only used if no column sizes

        self.top_margin  = 0.8
        self.bottom_margin = 1.0
        self.cell_left_margin = 0.1
        self.cell_right_margin = 0.1

    def SetPortrait(self):
        self.printData.SetPaperId(wxPAPER_LETTER)
        self.printData.SetOrientation(wxPORTRAIT)
        self.page_width = 8.5
        self.page_height = 11.0

    def SetLandscape(self):
        self.printData.SetOrientation(wxLANDSCAPE)
        self.page_width = 11.0
        self.page_height = 8.5

    def SetParms(self):
        self.ymax = 1
        self.xmax = 1
        self.page = 1
        self.total_pg = 100

        self.preview = None
        self.page = 0

    def SetColAlignment(self, col, align=wxALIGN_LEFT):
        self.set_column_align[col] = align

    def SetColBackgroundColour(self, col, colour):
        self.set_column_bgcolour[col] = colour

    def SetColTextColour(self, col, colour):
        self.set_column_txtcolour[col] = colour

    def SetCellColour(self, row, col, colour):      # cell background colour
        try:
            set = self.set_cell_colour[row]     # test if row already exists
            try:
                set[col] = colour       # test if column already exists
            except:
                set = { col: colour }       # create the column value
        except:
            set = { col: colour }           # create the column value

        self.set_cell_colour[row] = set    # create dictionary item for colour settings

    def SetCellText(self, row, col, colour):        # font colour for custom cells
        try:
            set = self.set_cell_text[row]     # test if row already exists
            try:
                set[col] = colour       # test if column already exists
            except:
                set = { col: colour }       # create the column value
        except:
            set = { col: colour }           # create the column value

        self.set_cell_text[row] = set    # create dictionary item for colour settings

    def SetColumnLineSize(self, col, size):      # column line size
        self.column_line_size[col] = size    # create dictionary item for column line settings

    def SetColumnLineColour(self, col, colour):
        self.column_line_colour[col] = colour

    def SetRowLineSize(self, row, size):
        self.row_line_size[row] = size

    def SetRowLineColour(self, row, colour):
        self.row_line_colour[row] = colour

    def SetHeader(self, text = "", type = None, name=None, size=None, colour = None, align = None, indent = None, attr=None):
        set = { "Text": text }

        if name == None:
            set["Name"] = self.header_font_name
        else:
            set["Name"] = name

        if size == None:
            set["Size"] = self.header_font_size
        else:
            set["Size"] = size

        if colour == None:
            set["Colour"] = self.header_font_colour
        else:
            set["Colour"] = colour

        if align == None:
            set["Align"] = self.header_align
        else:
            set["Align"] = align

        if indent == None:
            set["Indent"] = self.header_indent
        else:
            set["Indent"] = indent

        if attr == None:
            set["Attr"] = self.header_font_attr
        else:
            set["Attr"] = attr

        if type == None:
            set["Type"] = self.header_type
        else:
            set["Type"] = type

        self.header.append(set)

    def SetFooter(self, text = "", type = None, name=None, size=None, colour = None, align = None, indent = None, attr=None):
        set = { "Text": text }

        if name == None:
            set["Name"] = self.footer_font_name
        else:
            set["Name"] = name

        if size == None:
            set["Size"] = self.footer_font_size
        else:
            set["Size"] = size

        if colour == None:
            set["Colour"] = self.footer_font_colour
        else:
            set["Colour"] = colour

        if align == None:
            set["Align"] = self.footer_align
        else:
            set["Align"] = align

        if indent == None:
            set["Indent"] = self.footer_indent
        else:
            set["Indent"] = indent

        if type == None:
            set["Type"] = self.footer_type
        else:
            set["Type"] = type

        if attr == None:
            set["Attr"] = self.footer_font_attr
        else:
            set["Attr"] = attr

        self.footer.append(set)

    def Preview(self):
        printout = SetPrintout(self)
        printout2 = SetPrintout(self)
        self.preview = wxPrintPreview(printout, printout2, self.printData)
        if not self.preview.Ok():
            wxMessageBox("There was a problem printing!", "Printing", wxOK)
            return

        self.preview.SetZoom(60)        # initial zoom value

        frame = wxPreviewFrame(self.preview, self.parentFrame, "Print preview")

        frame.Initialize()
        if self.parentFrame:
            frame.SetPosition(self.preview_frame_pos)
            frame.SetSize(self.preview_frame_size)
        frame.Show(true)


    def Print(self):
        pdd = wxPrintDialogData()
        pdd.SetPrintData(self.printData)
        printer = wxPrinter(pdd)
        printout = SetPrintout(self)
        if not printer.Print(self.parentFrame, printout):
            wxMessageBox("There was a problem printing.\nPerhaps your current printer is not set correctly?", "Printing", wxOK)
        else:
            self.printData = printer.GetPrintDialogData().GetPrintData()
        printout.Destroy()

    def DoDrawing(self, DC):
        size = DC.GetSizeTuple()
        DC.BeginDrawing()

        self.text_font = wxFont(self.text_font_size, wxSWISS, self.text_font_attr, wxNORMAL)
        self.label_font = wxFont(self.label_font_size, wxSWISS, self.label_font_attr, wxNORMAL)

        table = PrintTableDraw(self, DC, size)
        table.data = self.data
        table.set_column = self.set_column
        table.label = self.label
        table.SetPage(self.page)

        if self.preview is None:
            table.SetPSize(size[0]/self.page_width, size[1]/self.page_height)
            table.SetPTSize(size[0], size[1])
            table.SetPreview(FALSE)
        else:
            if self.preview == 1:
                table.scale = self.scale
                table.SetPSize(size[0]/self.page_width, size[1]/self.page_height)
            else:
                table.SetPSize(self.pwidth, self.pheight)

            table.SetPTSize(self.ptwidth, self.ptheight)
            table.SetPreview(self.preview)

        table.OutCanvas()
        self.page_total = table.total_pages     # total display pages

        DC.EndDrawing()

        self.ymax = DC.MaxY()
        self.xmax = DC.MaxX()

        self.sizeh = size[0]
        self.sizew = size[1]

    def GetTotalPages(self):
        self.page_total = 100
        return self.page_total

    def HasPage(self, page):
        if page <= self.page_total:
            return true
        else:
            return false

    def SetPage(self, page):
        self.page = page

    def SetPageSize(self, width, height):
        self.pwidth, self.pheight = width, height

    def SetTotalSize(self, width, height):
        self.ptwidth, self.ptheight = width, height

    def SetPreview(self, preview, scale):
        self.preview = preview
        self.scale = scale

    def SetTotalSize(self, width, height):
        self.ptwidth = width
        self.ptheight = height

class PrintGrid:
    def __init__(self, parent, grid, format = [], total_col = None, total_row = None):
        if total_row == None:
            total_row = grid.GetNumberRows()
        if total_col == None:
            total_col = grid.GetNumberCols()

        self.total_row = total_row
        self.total_col = total_col
        self.grid = grid

        data = []
        for row in range(total_row):
            row_val = []
            value = grid.GetRowLabelValue(row)
            row_val.append(value)

            for col in range(total_col):
                value = grid.GetCellValue(row, col)
                row_val.append(value)
            data.append(row_val)

        label = [""]
        for col in range(total_col):
            value = grid.GetColLabelValue(col)
            label.append(value)

        self.table = PrintTable(parent)
        self.table.cell_left_margin = 0.0
        self.table.cell_right_margin = 0.0

        self.table.label = label
        self.table.set_column = format
        self.table.data = data

    def GetTable(self):
        return self.table

    def SetAttributes(self):
        for row in range(self.total_row):
            for col in range(self.total_col):
                colour = self.grid.GetCellTextColour(row, col-1)
                self.table.SetCellText(row, col, colour)

                colour = self.grid.GetCellBackgroundColour(row, col-1)
                self.table.SetCellColour(row, col, colour)

    def Preview(self):
        self.table.Preview()

    def Print(self):
        self.table.Print()


class SetPrintout(wxPrintout):
    def __init__(self, canvas):
        wxPrintout.__init__(self)
        self.canvas = canvas
        self.end_pg = 1000

    def OnBeginDocument(self, start, end):
        return self.base_OnBeginDocument(start, end)

    def OnEndDocument(self):
        self.base_OnEndDocument()

    def HasPage(self, page):
        try:
            end = self.canvas.HasPage(page)
            return end
        except:
            return true

    def GetPageInfo(self):
        try:
            self.end_pg = self.canvas.GetTotalPages()
        except:
            pass

        end_pg = self.end_pg
        str_pg = 1
        return (str_pg, end_pg, str_pg, end_pg)

    def OnPreparePrinting(self):
        self.base_OnPreparePrinting()

    def OnBeginPrinting(self):
        dc = self.GetDC()

        self.preview = self.IsPreview()
        if (self.preview):
            self.pixelsPerInch = self.GetPPIScreen()
        else:
            self.pixelsPerInch = self.GetPPIPrinter()

        (w, h) = dc.GetSizeTuple()
        scaleX = float(w) / 1000
        scaleY = float(h) / 1000
        self.printUserScale = min(scaleX, scaleY)

        self.base_OnBeginPrinting()

    def GetSize(self):
        self.psizew, self.psizeh = self.GetPPIPrinter()
        return self.psizew, self.psizeh

    def GetTotalSize(self):
        self.ptsizew, self.ptsizeh = self.GetPageSizePixels()
        return self.ptsizew, self.ptsizeh

    def OnPrintPage(self, page):
        dc = self.GetDC()
        (w, h) = dc.GetSizeTuple()
        scaleX = float(w) / 1000
        scaleY = float(h) / 1000
        self.printUserScale = min(scaleX, scaleY)
        dc.SetUserScale(self.printUserScale, self.printUserScale)

        self.preview = self.IsPreview()

        self.canvas.SetPreview(self.preview, self.printUserScale)
        self.canvas.SetPage(page)

        self.ptsizew, self.ptsizeh = self.GetPageSizePixels()
        self.canvas.SetTotalSize(self.ptsizew, self.ptsizeh)

        self.psizew, self.psizeh = self.GetPPIPrinter()
        self.canvas.SetPageSize(self.psizew, self.psizeh)

        self.canvas.DoDrawing(dc)
        return true






