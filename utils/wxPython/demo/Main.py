#!/bin/env python
#----------------------------------------------------------------------------
# Name:         Main.py
# Purpose:      Testing lots of stuff, controls, window types, etc.
#
# Author:       Robin Dunn & Gary Dumer
#
# Created:
# RCS-ID:       $Id$
# Copyright:    (c) 1999 by Total Control Software
# Licence:      wxWindows license
#----------------------------------------------------------------------------

import sys, os
from   wxPython.wx import *
from   wxPython.lib.splashscreen import SplashScreen

#---------------------------------------------------------------------------

_useSplitter       = true
_useNestedSplitter = true

_treeList = [
    ('New since last release', ['wxMVCTree', 'wxVTKRenderWindow',
                                'FileBrowseButton', #'wxToggleButton',
                                'GenericButtons']),

    ('Managed Windows', ['wxFrame', 'wxDialog', 'wxMiniFrame']),

    ('Non-Managed Windows', ['wxGrid', 'wxSashWindow',
                               'wxScrolledWindow', 'wxSplitterWindow',
                               'wxStatusBar', 'wxToolBar', 'wxNotebook',
                               'wxHtmlWindow']),

    ('Common Dialogs', ['wxColourDialog', 'wxDirDialog', 'wxFileDialog',
                        'wxSingleChoiceDialog', 'wxTextEntryDialog',
                        'wxFontDialog', 'wxPageSetupDialog', 'wxPrintDialog',
                        'wxMessageDialog', 'wxProgressDialog']),

    ('Controls', ['wxButton', 'wxCheckBox', 'wxCheckListBox', 'wxChoice',
                  'wxComboBox', 'wxGauge', 'wxListBox', 'wxListCtrl', 'wxTextCtrl',
                  'wxTreeCtrl', 'wxSpinButton', 'wxStaticText', 'wxStaticBitmap',
                  'wxRadioBox', 'wxSlider', #'wxToggleButton'
                  ]),

    ('Window Layout', ['wxLayoutConstraints', 'Sizers', 'OldSizers']),

    ('Miscellaneous', [ 'DragAndDrop', 'CustomDragAndDrop', 'FontEnumerator',
                        'wxTimer', 'wxValidator', 'wxGLCanvas', 'DialogUnits',
                        'wxImage', 'PrintFramework', 'wxOGL', 'PythonEvents',
                        'Threads']),

    ('wxPython Library', ['Layoutf', 'wxScrolledMessageDialog',
                          'wxMultipleChoiceDialog', 'wxPlotCanvas', 'wxFloatBar',
                          'PyShell', 'wxCalendar', 'wxMVCTree', 'wxVTKRenderWindow',
                          'FileBrowseButton', 'GenericButtons']),

    ('Cool Contribs', ['pyTree', 'hangman', 'SlashDot', 'XMLtreeview']),

    ]

#---------------------------------------------------------------------------

class wxPythonDemo(wxFrame):
    def __init__(self, parent, id, title):
        wxFrame.__init__(self, parent, -1, title, size = (725, 550))

        self.cwd = os.getcwd()

        if wxPlatform == '__WXMSW__':
            self.icon = wxIcon('bitmaps/mondrian.ico', wxBITMAP_TYPE_ICO)
            self.SetIcon(self.icon)

        self.otherWin = None
        EVT_IDLE(self, self.OnIdle)

        self.Centre(wxBOTH)
        self.CreateStatusBar(1, wxST_SIZEGRIP)

        if _useSplitter:
            splitter = wxSplitterWindow(self, -1)
            if _useNestedSplitter:
                splitter2 = wxSplitterWindow(splitter, -1)
                logParent = nbParent = splitter2
            else:
                nbParent = splitter
                logParent = wxFrame(self, -1, "wxPython Demo: log window",
                                (0,0), (500, 150))
                logParent.Show(true)
        else:
            nbParent = self
            logParent = wxFrame(self, -1, "wxPython Demo: log window",
                                (0,0), (500, 150))
            logParent.Show(true)



        # Prevent TreeCtrl from displaying all items after destruction
        self.dying = false

        # Make a File menu
        self.mainmenu = wxMenuBar()
        menu = wxMenu()
        exitID = wxNewId()
        menu.Append(exitID, 'E&xit\tAlt-X', 'Get the heck outta here!')
        EVT_MENU(self, exitID, self.OnFileExit)
        self.mainmenu.Append(menu, '&File')

        # Make a Demo menu
        menu = wxMenu()
        for item in _treeList:
            submenu = wxMenu()
            for childItem in item[1]:
                mID = wxNewId()
                submenu.Append(mID, childItem)
                EVT_MENU(self, mID, self.OnDemoMenu)
            menu.AppendMenu(wxNewId(), item[0], submenu)
        self.mainmenu.Append(menu, '&Demo')


        # Make a Help menu
        helpID = wxNewId()
        menu = wxMenu()
        menu.Append(helpID, '&About\tCtrl-H', 'wxPython RULES!!!')
        EVT_MENU(self, helpID, self.OnHelpAbout)
        self.mainmenu.Append(menu, '&Help')
        self.SetMenuBar(self.mainmenu)

        # set the menu accellerator table...
        aTable = wxAcceleratorTable([(wxACCEL_ALT,  ord('X'), exitID),
                                     (wxACCEL_CTRL, ord('H'), helpID)])
        self.SetAcceleratorTable(aTable)


        # Create a TreeCtrl
        if _useSplitter:
            tID = wxNewId()
            self.treeMap = {}
            self.tree = wxTreeCtrl(splitter, tID)
            root = self.tree.AddRoot("Overview")
            firstChild = None
            for item in _treeList:
                child = self.tree.AppendItem(root, item[0])
                if not firstChild: firstChild = child
                for childItem in item[1]:
                    theDemo = self.tree.AppendItem(child, childItem)
                    self.treeMap[childItem] = theDemo

            self.tree.Expand(root)
            self.tree.Expand(firstChild)
            EVT_TREE_ITEM_EXPANDED   (self.tree, tID, self.OnItemExpanded)
            EVT_TREE_ITEM_COLLAPSED  (self.tree, tID, self.OnItemCollapsed)
            EVT_TREE_SEL_CHANGED     (self.tree, tID, self.OnSelChanged)

        # Create a Notebook
        self.nb = wxNotebook(nbParent, -1)

        # Set up a TextCtrl on the Overview Notebook page
        self.ovr = wxTextCtrl(self.nb, -1, style = wxTE_MULTILINE|wxTE_READONLY)
        self.nb.AddPage(self.ovr, "Overview")


        # Set up a TextCtrl on the Demo Code Notebook page
        self.txt = wxTextCtrl(self.nb, -1,
                              style = wxTE_MULTILINE|wxTE_READONLY|wxHSCROLL)
        self.txt.SetFont(wxFont(9, wxMODERN, wxNORMAL, wxNORMAL, false))
        self.nb.AddPage(self.txt, "Demo Code")


        # Set up a log on the View Log Notebook page
        self.log = wxTextCtrl(logParent, -1,
                              style = wxTE_MULTILINE|wxTE_READONLY|wxHSCROLL)
        (w, self.charHeight) = self.log.GetTextExtent('X')
        self.WriteText('wxPython Demo Log:\n')

        self.Show(true)

        # add the windows to the splitter and split it.
        if _useSplitter:
            if _useNestedSplitter:
                splitter2.SplitHorizontally(self.nb, self.log)
                splitter2.SetSashPosition(360, true)
                splitter2.SetMinimumPaneSize(20)

                splitter.SplitVertically(self.tree, splitter2)
            else:
                splitter.SplitVertically(self.tree, self.nb)

            splitter.SetSashPosition(180, true)
            splitter.SetMinimumPaneSize(20)


        # make our log window be stdout
        #sys.stdout = self

        # select initial items
        self.nb.SetSelection(0)
        if _useSplitter:
            self.tree.SelectItem(root)

        if len(sys.argv) == 2:
            try:
                selectedDemo = self.treeMap[sys.argv[1]]
            except:
                selectedDemo = None
            if selectedDemo and _useSplitter:
                self.tree.SelectItem(selectedDemo)
                self.tree.EnsureVisible(selectedDemo)


        self.WriteText('window handle: %s\n' % self.GetHandle())


    #---------------------------------------------
    def WriteText(self, text):
        self.log.WriteText(text)
        w, h = self.log.GetClientSizeTuple()
        numLines = h/self.charHeight
        x, y = self.log.PositionToXY(self.log.GetLastPosition())
        if y > numLines:
            self.log.ShowPosition(self.log.XYToPosition(x, y-numLines))
            ##self.log.ShowPosition(self.log.GetLastPosition())
        self.log.SetInsertionPointEnd()

    def write(self, txt):
        self.WriteText(txt)

    #---------------------------------------------
    def OnItemExpanded(self, event):
        item = event.GetItem()
        self.log.WriteText("OnItemExpanded: %s\n" % self.tree.GetItemText(item))

    #---------------------------------------------
    def OnItemCollapsed(self, event):
        item = event.GetItem()
        self.log.WriteText("OnItemCollapsed: %s\n" % self.tree.GetItemText(item))

    #---------------------------------------------
    def OnSelChanged(self, event):
        if self.dying:
            return

        item = event.GetItem()
        itemText = self.tree.GetItemText(item)
        self.RunDemo(itemText)


    #---------------------------------------------
    def RunDemo(self, itemText):
        os.chdir(self.cwd)
        if self.nb.GetPageCount() == 3:
            if self.nb.GetSelection() == 2:
                self.nb.SetSelection(0)
            self.nb.DeletePage(2)

        if itemText == 'Overview':
            self.GetDemoFile('Main.py')
            self.SetOverview('Overview', overview)
            self.nb.Refresh();
            self.window = None

        else:
            if os.path.exists(itemText + '.py'):
                wxBeginBusyCursor()
                self.GetDemoFile(itemText + '.py')
                module = __import__(itemText, globals())
                self.SetOverview(itemText, module.overview)
                wxEndBusyCursor()

                # in case runTest is modal, make sure things look right...
                self.nb.Refresh();
                wxYield()

                self.window = module.runTest(self, self.nb, self)
                if self.window:
                    self.nb.AddPage(self.window, 'Demo')
                    #self.nb.ResizeChildren()
                    self.nb.SetSelection(2)
                    #self.nb.ResizeChildren()
                    #if self.window.GetAutoLayout():
                    #    self.window.Layout()

            else:
                self.ovr.Clear()
                self.txt.Clear()
                self.window = None



    #---------------------------------------------
    # Get the Demo files
    def GetDemoFile(self, filename):
        self.txt.Clear()
        #if not self.txt.LoadFile(filename):
        #    self.txt.WriteText("Cannot open %s file." % filename)
        try:
            self.txt.SetValue(open(filename).read())
        except IOError:
            self.txt.WriteText("Cannot open %s file." % filename)


        self.txt.SetInsertionPoint(0)
        self.txt.ShowPosition(0)

    #---------------------------------------------
    def SetOverview(self, name, text):
        self.ovr.Clear()
        self.ovr.WriteText(text)
        self.nb.SetPageText(0, name)
        self.ovr.SetInsertionPoint(0)
        self.ovr.ShowPosition(0)

    #---------------------------------------------
    # Menu methods
    def OnFileExit(self, event):
        self.Close()


    def OnHelpAbout(self, event):
        #about = wxMessageDialog(self,
        #                        "wxPython is a Python extension module that\n"
        #                        "encapsulates the wxWindows GUI classes.\n\n"
        #                        "This demo shows off some of the capabilities\n"
        #                        "of wxPython.\n\n"
        #                        "          Developed by Robin Dunn",
        #                       "About wxPython", wxOK)
        from About import MyAboutBox
        about = MyAboutBox(self)
        about.ShowModal()
        about.Destroy()


    #---------------------------------------------
    def OnCloseWindow(self, event):
        self.dying = true
        self.window = None
        self.mainmenu = None
        self.Destroy()

    #---------------------------------------------
    def OnIdle(self, event):
        if self.otherWin:
            self.otherWin.Raise()
            self.window = self.otherWin
            self.otherWin = None

    #---------------------------------------------
    def OnDemoMenu(self, event):
        if _useSplitter:
            try:
                selectedDemo = self.treeMap[self.mainmenu.GetLabel(event.GetId())]
            except:
                selectedDemo = None
            if selectedDemo:
                self.tree.SelectItem(selectedDemo)
                self.tree.EnsureVisible(selectedDemo)
        else:
            self.RunDemo(self.mainmenu.GetLabel(event.GetId()))

#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

class MyApp(wxApp):
    def OnInit(self):
        wxImage_AddHandler(wxJPEGHandler())
        wxImage_AddHandler(wxPNGHandler())
        wxImage_AddHandler(wxGIFHandler())

        self.splash = SplashScreen(None, bitmapfile='bitmaps/splash.gif',
                              duration=4000, callback=self.AfterSplash)
        self.splash.Show(true)
        wxYield()
        return true

    def AfterSplash(self):
        self.splash.Close(true)
        frame = wxPythonDemo(None, -1, "wxPython: (A Demonstration)")
        frame.Show(true)
        self.SetTopWindow(frame)
        return true

#---------------------------------------------------------------------------

def main():
    app = MyApp(0)
    app.MainLoop()


#---------------------------------------------------------------------------



overview = """\
Python
------------

Python is an interpreted, interactive, object-oriented programming language often compared to Tcl, Perl, Scheme, or Java.

Python combines remarkable power with very clear syntax. It has modules, classes, exceptions, very high level dynamic data types, and dynamic typing. There are interfaces to many system calls and libraries, and new built-in modules are easily written in C or C++. Python is also usable as an extension language for applications that need a programmable interface.

wxWindows
--------------------

wxWindows is a free C++ framework designed to make cross-platform programming child's play. Well, almost. wxWindows 2 supports Windows 3.1/95/98/NT, Unix with GTK/Motif/Lesstif, with a Mac version underway. Other ports are under consideration.

wxWindows is a set of libraries that allows C++ applications to compile and run on several different types of computers, with minimal source code changes. There is one library per supported GUI (such as Motif, or Windows). As well as providing a common API (Application Programming Interface) for GUI functionality, it provides functionality for accessing some commonly-used operating system facilities, such as copying or deleting files. wxWindows is a 'framework' in the sense that it provides a lot of built-in functionality, which the application can use or replace as required, thus saving a great deal of coding effort. Basic data structures such as strings, linked lists and hash tables are also supported.

wxPython
----------------

wxPython is a Python extension module that encapsulates the wxWindows GUI classes. Currently it is only available for the Win32 and GTK ports of wxWindows, but as soon as the other ports are brought up to the same level as Win32 and GTK, it should be fairly trivial to enable wxPython to be used with the new GUI.

The wxPython extension module attempts to mirror the class heiarchy of wxWindows as closely as possible. This means that there is a wxFrame class in wxPython that looks, smells, tastes and acts almost the same as the wxFrame class in the C++ version. Unfortunately, because of differences in the languages, wxPython doesn't match wxWindows exactly, but the differences should be easy to absorb because they are natural to Python. For example, some methods that return multiple values via argument pointers in C++ will return a tuple of values in Python.

There is still much to be done for wxPython, many classes still need to be mirrored. Also, wxWindows is still somewhat of a moving target so it is a bit of an effort just keeping wxPython up to date. On the other hand, there are enough of the core classes completed that useful applications can be written.

wxPython is close enough to the C++ version that the majority of the wxPython documentation is actually just notes attached to the C++ documents that describe the places where wxPython is different. There is also a series of sample programs included, and a series of documentation pages that assist the programmer in getting started with wxPython.
"""







#----------------------------------------------------------------------------
#----------------------------------------------------------------------------

if __name__ == '__main__':
    main()

#----------------------------------------------------------------------------







