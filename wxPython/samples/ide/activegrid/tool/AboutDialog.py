#----------------------------------------------------------------------------
# Name:         AboutDialog.py
# Purpose:      AboutBox which has copyright notice, license information, and credits
#
# Author:       Morgan Hua
#
# Created:      3/22/05
# Copyright:    (c) 2005 ActiveGrid, Inc.
# CVS-ID:       $Id$
# License:      wxWindows License
#----------------------------------------------------------------------------

import wx
from IDE import ACTIVEGRID_BASE_IDE, getSplashBitmap
_ = wx.GetTranslation

#----------------------------------------------------------------------------
# Package License Data for AboutDialog
#   Package, License, URL
#   If no information is available, put a None as a place holder.
#----------------------------------------------------------------------------


licenseData = [
    ("ActiveGrid", "ASL 2.0", "http://apache.org/licenses/LICENSE-2.0"),
    ("Python 2.3", "Python Software Foundation License", "http://www.python.org/2.3/license.html"),
    ("wxPython 2.5", "wxWidgets 2 - LGPL", "http://wxwidgets.org/newlicen.htm"),
    ("wxWidgets", "wxWindows Library License 3", "http://www.wxwidgets.org/manuals/2.5.4/wx_wxlicense.html"),
    ("pychecker", "MetaSlash - BSD", "http://pychecker.sourceforge.net/COPYRIGHT"), 
    ("process.py", "See file", "http://starship.python.net/~tmick/"),
]

if not ACTIVEGRID_BASE_IDE:    # add licenses for database connections only if not the base IDE
    licenseData += [
        ("pydb2", "LGPL", "http://sourceforge.net/projects/pydb2"), 
        ("pysqlite", "Python License (CNRI)", "http://sourceforge.net/projects/pysqlite"),
        ("mysql-python", "GPL, Python License (CNRI), Zope Public License", "http://sourceforge.net/projects/mysql-python"), 
        ("cx_Oracle", "Computronix", "http://http://www.computronix.com/download/License(cxOracle).txt"), 
        ("SQLite", "Public Domain", "http://www.sqlite.org/copyright.html"),
        ("PyGreSQL", "BSD", "http://www.pygresql.org"),
    ]

if wx.Platform == '__WXMSW__':
    licenseData += [("pywin32", "Python Software Foundation License", "http://sourceforge.net/projects/pywin32/")]

class AboutDialog(wx.Dialog):

    def __init__(self, parent):
        """
        Initializes the about dialog.
        """
        wx.Dialog.__init__(self, parent, -1, _("About ") + wx.GetApp().GetAppName(), style = wx.DEFAULT_DIALOG_STYLE)

        nb = wx.Notebook(self, -1)

        aboutPage = wx.Panel(nb, -1)
        sizer = wx.BoxSizer(wx.VERTICAL)
        splash_bmp = getSplashBitmap()
        image = wx.StaticBitmap(aboutPage, -1, splash_bmp, (0,0), (splash_bmp.GetWidth(), splash_bmp.GetHeight()))
        sizer.Add(image, 0, wx.ALIGN_CENTER|wx.ALL, 0)
        sizer.Add(wx.StaticText(aboutPage, -1, wx.GetApp().GetAppName() + _("\nVersion 0.6 Early Access\n\nCopyright (c) 2003-2005 ActiveGrid Incorporated and Contributors.  All rights reserved.")), 0, wx.ALIGN_LEFT|wx.ALL, 10)
        sizer.Add(wx.StaticText(aboutPage, -1, _("http://www.activegrid.com")), 0, wx.ALIGN_LEFT|wx.LEFT|wx.BOTTOM, 10)
        aboutPage.SetSizer(sizer)
        nb.AddPage(aboutPage, _("Copyright"))

        licensePage = wx.Panel(nb, -1)
        grid = wx.grid.Grid(licensePage, -1)
        grid.CreateGrid(len(licenseData), 2)
        
        dc = wx.ClientDC(grid)
        dc.SetFont(grid.GetLabelFont())
        grid.SetColLabelValue(0, _("License"))
        grid.SetColLabelValue(1, _("URL"))
        w, maxHeight = dc.GetTextExtent(_("License"))
        w, h = dc.GetTextExtent(_("URL"))
        if h > maxHeight:
            maxHeight = h
        grid.SetColLabelSize(maxHeight + 6)  # add a 6 pixel margin

        for row, data in enumerate(licenseData):
            package = data[0]
            license = data[1]
            url = data[2]
            if package:
                grid.SetRowLabelValue(row, package)
            if license:
                grid.SetCellValue(row, 0, license)
            if url:
                grid.SetCellValue(row, 1, url)
        
        grid.EnableEditing(False)
        grid.EnableDragGridSize(False)
        grid.EnableDragColSize(False)
        grid.EnableDragRowSize(False)
        grid.SetRowLabelAlignment(wx.ALIGN_LEFT, wx.ALIGN_CENTRE)
        grid.SetLabelBackgroundColour(wx.WHITE)
        grid.AutoSizeColumn(0, 100)
        grid.AutoSizeColumn(1, 100)
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(grid, 1, wx.EXPAND|wx.ALL, 10)
        licensePage.SetSizer(sizer)
        nb.AddPage(licensePage, _("Licenses"))

        creditsPage = wx.Panel(nb, -1)
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(wx.StaticText(creditsPage, -1, _("ActiveGrid Development Team:\n\nLawrence Bruhmuller\nEric Chu\nMatt Fryer\nJoel Hare\nMorgan Hua\nAlan Mullendore\nJeff Norton\nKevin Wang\nPeter Yared")), 0, wx.ALIGN_LEFT|wx.ALL, 10)
        creditsPage.SetSizer(sizer)
        nb.AddPage(creditsPage, _("Credits"))
            
        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(nb, 0, wx.ALIGN_CENTRE|wx.ALL, 5)
        btn = wx.Button(self, wx.ID_OK)
        sizer.Add(btn, 0, wx.ALIGN_CENTRE|wx.ALL, 5)

        self.SetSizer(sizer)
        self.SetAutoLayout(True)
        sizer.Fit(self)
        

