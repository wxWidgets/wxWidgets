
import  os
import  wx
import  wx.lib.printout as  printout

#---------------------------------------------------------------------------

buttonDefs = {
    814 : ('PreviewWide',      'Preview print of a wide table'),
    815 : ('PreviewNarrow',   'Preview print of a narrow table with color highlights'),
    816 : ('PreviewText',    'Preview print of a text file'),
    818 : ('OnPreviewMatrix',   'Preview print of a narrow column grid without a table header'),
    817 : ('PreviewLine',   'Preview print to demonstrate the use of line breaks'),
    819 : ('PrintWide', 'Direct print (no preview) of a wide table'),
    }


class TablePanel(wx.Panel):
    def __init__(self, parent, log, frame):
        wx.Panel.__init__(self, parent, -1)
        self.log = log
        self.frame = frame

        box = wx.BoxSizer(wx.VERTICAL)
        box.Add((20, 30))
        keys = buttonDefs.keys()
        keys.sort()

        for k in keys:
            text = buttonDefs[k][1]
            btn = wx.Button(self, k, text)
            box.Add(btn, 0, wx.ALIGN_CENTER|wx.ALL, 15)
            self.Bind(wx.EVT_BUTTON, self.OnButton, btn)

        self.SetAutoLayout(True)
        self.SetSizer(box)

    def OnButton(self, evt):
        funct = buttonDefs[evt.GetId()][0]
        code = 'self.' + funct + '()'
        eval(code)

    def ReadData(self):
        test_file = "./data/testtable.txt"
        file = open(test_file,'r',1)
        i = 0

        data = []
        while 1:
            text = file.readline()
            text = text.strip()
            if not text:
                break

            list_val = text.split('\t')
            data.append(list_val)
        file.close()

        self.header = data[0]
        self.data = data[1:]

    def PreviewWide(self):
        self.ReadData()
        prt = printout.PrintTable(self.frame)
        prt.data = self.data
        prt.left_margin = 0.5
        prt.set_column = [1.0, 1.0, 1.0, 1.5, 1.0, 3.0]
        prt.label = self.header
        prt.SetLandscape()

        prt.SetColumnLineSize(2, 3)
        prt.SetColumnLineColour(3, wx.NamedColour('RED'))

        prt.SetRowLineSize(1, 3)
        prt.SetRowLineColour(5, wx.NamedColour('RED'))

        prt.SetHeader("wx.Windows Applications")
        prt.SetFooter()
        prt.SetFooter("Date: ", type = "Date", align=wx.ALIGN_RIGHT, indent = -1, colour = wx.NamedColour('RED'))
        prt.Preview()

    def PreviewNarrow(self):
        self.ReadData()
        new_data = []
        for val in self.data:
            new_data.append([val[0], val[1], val[2], val[4], val[5]])

        val = self.header
        new_header = [val[0], val[1], val[2], val[4], val[5]]

        prt = printout.PrintTable(self.frame)
        prt.data = new_data
        prt.set_column = [ 1, 1, 1, 1, 2]
        prt.label = new_header
        prt.SetColAlignment(1, wx.ALIGN_CENTRE)
        prt.SetColBackgroundColour(0, wx.NamedColour('RED'))
        prt.SetColTextColour(0, wx.NamedColour('WHITE'))
        prt.SetCellColour(4, 0, wx.NamedColour('LIGHT BLUE'))
        prt.SetCellColour(4, 1, wx.NamedColour('LIGHT BLUE'))
        prt.SetCellColour(17, 1, wx.NamedColour('LIGHT BLUE'))

        prt.SetColBackgroundColour(2, wx.NamedColour('LIGHT BLUE'))
        prt.SetCellText(4, 2, wx.NamedColour('RED'))

        prt.SetColTextColour(3, wx.NamedColour('RED'))
        prt.label_font_colour = wx.NamedColour('WHITE')
        prt.SetHeader("wxWindows Applications", colour = wx.NamedColour('RED'))

        prt.SetHeader("Printed: ", type = "Date & Time", align=wx.ALIGN_RIGHT, indent = -1, colour = wx.NamedColour('BLUE'))
        prt.SetFooter("Page No", colour = wx.NamedColour('RED'), type ="Num")
        prt.Preview()

    def OnPreviewMatrix(self):
        total_col = 45
        total_row = 10
        hsize = 0.2
        vsize = 0.2

        data = []
        startx = 1.0
        columns = []
        for val in range(total_col):
            columns.append(hsize)

        prt = printout.PrintTable(self.frame)

        for row in range(total_row):
            value = []
            for col in range(total_col):
                value.append(str(col))
            data.append(value)

        for col in range(total_col):
            prt.SetColAlignment(col, wx.ALIGN_CENTRE)

        prt.SetLandscape()
        prt.text_font_size = 8
        prt.cell_left_margin = 0

        prt.data = data
        prt.set_column = columns
        prt.SetHeader("Test of Small Grid Size")
        prt.Preview()

    def PreviewLine(self):
        prt = printout.PrintTable(self.frame)
        prt.label = ["Header 1", "Header 2", "Header 3"]
        prt.set_column = []
        prt.data = [["Row 1", "1", "2"], ["Row 2", "3", "4\nNew Line to see if it also can wrap around the cell region properly\nAnother new line"]]
        prt.SetFooter()
        prt.Preview()

    def PreviewText(self):
        prt = printout.PrintTable(self.frame)
        prt.SetHeader("PROCLAMATION")
        file = open('data/proclamation.txt')
        data = []
        for txt in file:
            data.append(txt.strip())
        file.close()
        prt.data = data
        prt.Preview()

    def PrintWide(self):
        self.ReadData()
        prt = printout.PrintTable(self.frame)
        prt.data = self.data

        prt.left_margin = 0.5
        prt.set_columns = [ 1, 1, 1, 1, 2, 1, 3 ]
        prt.label = self.header
        prt.SetLandscape()
        prt.Print()


#---------------------------------------------------------------------------

def runTest(frame, nb, log):
    win = TablePanel(nb, log, frame)
    return win

#---------------------------------------------------------------------------





overview = """\
<html><body>
<h2>Table Printing</h2>

This demo shows various ways of using the <b><i>new
</i></b> PrintOut class.  To understand the class you need to examine the demo examples
and the library <a href="%s">printout.py</a> module classes.
<p>
The initial class primarily contains a Table preview/printing class.  There is a lot of flexibility
in manipulating the placement, sizing, colours, alignment of the table text and cell background colors.
There are also a number of options for printing Header and Footer information on the page.
<p>
There is also a class to extract the parameters from a wxGrid and easily recreate a Table printout.
<p>
The data is printed from a list object containing the column and row values.  The label or table header
can be defined and will be repeated for all pages.
<p>
The correct "Total Page" does get calculated and used in the print out Footer.
<p>
There is still problems with the print framework to properly get the total pages in the preview unless
the program knows it before trying to parse through the available pages.  This will be fixed
when the framework allows for it.


""" % os.path.join(os.path.dirname(printout.__file__), "printout.py")





if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

