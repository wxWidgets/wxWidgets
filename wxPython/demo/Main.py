#!/bin/env python
#----------------------------------------------------------------------------
# Name:         Main.py
# Purpose:      Testing lots of stuff, controls, window types, etc.
#
# Author:       Robin Dunn
#
# Created:      A long time ago, in a galaxy far, far away...
# RCS-ID:       $Id$
# Copyright:    (c) 1999 by Total Control Software
# Licence:      wxWindows license
#----------------------------------------------------------------------------

import sys, os, time
import wx
import wx.html

import images


#---------------------------------------------------------------------------


_treeList = [
    # new stuff
    ('Recent Additions', [
        'wxScrolledPanel',
        'ShapedWindow',
        'NewNamespace',
        'PopupMenu',
        'AnalogClockWindow',
        'MaskedEditControls',
        'wxTreeListCtrl',
        'wxGrid_MegaExample',
        ]),

    # managed windows == things with a (optional) caption you can close
    ('Base Frames and Dialogs', [
        'wxDialog',
        'wxFrame',
        'wxMDIWindows',
        'wxMiniFrame',
        'wxWizard',
        ]),

    # the common dialogs
    ('Common Dialogs', [
        'wxColourDialog',
        'wxDirDialog',
        'wxFileDialog',
        'wxFindReplaceDialog',
        'wxFontDialog',
        'wxMessageDialog',
        'wxPageSetupDialog',
        'wxPrintDialog',
        'wxProgressDialog',
        'wxSingleChoiceDialog',
        'wxTextEntryDialog',
        ]),

    # dialogs from libraries
    ('More Dialogs', [
        'ErrorDialogs',
        'ImageBrowser',
        'wxMultipleChoiceDialog',
        'wxScrolledMessageDialog',
        ]),

    # core controls
    ('Core Windows/Controls', [
        'PopupMenu',
        'wxButton',
        'wxCheckBox',
        'wxCheckListBox',
        'wxChoice',
        'wxComboBox',
        'wxGauge',
        'wxGenericDirCtrl',
        'wxGrid',
        'wxGrid_MegaExample',
        'wxListBox',
        'wxListCtrl',
        'wxListCtrl_virtual',
        'wxMenu',
        'wxNotebook',
        'wxPopupWindow',
        'wxRadioBox',
        'wxRadioButton',
        'wxSashWindow',
        'wxScrolledWindow',
        'wxSlider',
        'wxSpinButton',
        'wxSpinCtrl',
        'wxSplitterWindow',
        'wxStaticBitmap',
        'wxStaticText',
        'wxStatusBar',
        'wxTextCtrl',
        'wxToggleButton',
        'wxToolBar',
        'wxTreeCtrl',
        'wxValidator',
        ]),

    # controls coming from other librairies
    ('More Windows/Controls', [
        #'wxFloatBar',          deprecated
        #'wxMVCTree',           deprecated
        #'wxRightTextCtrl',     deprecated as we have wxTE_RIGHT now.
        'AnalogClockWindow',
        'ColourSelect',
        'ContextHelp',
        'FancyText',
        'FileBrowseButton',
        'GenericButtons',
        'MaskedEditControls',
        'PyShell',
        'PyCrust',
        'SplitTree',
        'TablePrint',
        'Throbber',
        'wxCalendar',
        'wxCalendarCtrl',
        'wxPyColourChooser',
        'wxDynamicSashWindow',
        'wxEditableListBox',
        'wxEditor',
        'wxHtmlWindow',
        'wxIEHtmlWin',
        'wxIntCtrl',
        'wxLEDNumberCtrl',
        'wxMimeTypesManager',
        'wxMultiSash',
        'wxPopupControl',
        'wxStyledTextCtrl_1',
        'wxStyledTextCtrl_2',
        'wxTimeCtrl',
        'wxTreeListCtrl',
        ]),

    # How to lay out the controls in a frame/dialog
    ('Window Layout', [
        'LayoutAnchors',
        'Layoutf',
        'RowColSizer',
        'Sizers',
        'wxLayoutConstraints',
        'wxScrolledPanel',
        'wxXmlResource',
        'wxXmlResourceHandler',
        ]),

    # ditto
    ('Process and Events', [
        'EventManager',
        'infoframe',
        'OOR',
        'PythonEvents',
        'Threads',
        'wxKeyEvents',
        'wxProcess',
        'wxTimer',
        ]),

    # Clipboard and DnD
    ('Clipboard and DnD', [
        'CustomDragAndDrop',
        'DragAndDrop',
        'URLDragAndDrop',
        ]),

    # Images
    ('Using Images', [
        'Throbber',
        'wxArtProvider',
        'wxDragImage',
        'wxImage',
        'wxImageFromStream',
        'wxMask',
        ]),

    # Other stuff
    ('Miscellaneous', [
        'ColourDB',
        'DialogUnits',
        'DrawXXXList',
        'FontEnumerator',
        'PrintFramework',
        'Throbber',
        'Unicode',
        'wxFileHistory',
        'wxJoystick',
        'wxOGL',
        'wxWave',
        ]),

    # need libs not coming with the demo
    ('Objects using an external library', [
        'ActiveXWrapper_Acrobat',
        'ActiveXWrapper_IE',
        'wxGLCanvas',
        'wxPlotCanvas',
        ]),


    ('Check out the samples dir too', [
        ]),

]



#---------------------------------------------------------------------------

class MyLog(wx.PyLog):
    def __init__(self, textCtrl, logTime=0):
        wx.PyLog.__init__(self)
        self.tc = textCtrl
        self.logTime = logTime

    def DoLogString(self, message, timeStamp):
        if self.logTime:
            message = time.strftime("%X", time.localtime(timeStamp)) + \
                      ": " + message
        if self.tc:
            self.tc.AppendText(message + '\n')


class MyTP(wx.PyTipProvider):
    def GetTip(self):
        return "This is my tip"

#---------------------------------------------------------------------------

def opj(path):
    """Convert paths to the platform-specific separator"""
    return apply(os.path.join, tuple(path.split('/')))


#---------------------------------------------------------------------------

class wxPythonDemo(wx.Frame):
    overviewText = "wxPython Overview"

    def __init__(self, parent, id, title):
        wx.Frame.__init__(self, parent, -1, title, size = (800, 600),
                         style=wx.DEFAULT_FRAME_STYLE|wx.NO_FULL_REPAINT_ON_RESIZE)

        self.cwd = os.getcwd()
        self.curOverview = ""
        self.window = None

        icon = images.getMondrianIcon()
        self.SetIcon(icon)

        if wx.Platform == '__WXMSW__':
            # setup a taskbar icon, and catch some events from it
            self.tbicon = wx.TaskBarIcon()
            self.tbicon.SetIcon(icon, "wxPython Demo")
            wx.EVT_TASKBAR_LEFT_DCLICK(self.tbicon, self.OnTaskBarActivate)
            wx.EVT_TASKBAR_RIGHT_UP(self.tbicon, self.OnTaskBarMenu)
            wx.EVT_MENU(self.tbicon, self.TBMENU_RESTORE, self.OnTaskBarActivate)
            wx.EVT_MENU(self.tbicon, self.TBMENU_CLOSE, self.OnTaskBarClose)

        wx.CallAfter(self.ShowTip)

        self.otherWin = None
        wx.EVT_IDLE(self, self.OnIdle)
        wx.EVT_CLOSE(self, self.OnCloseWindow)
        wx.EVT_ICONIZE(self, self.OnIconfiy)
        wx.EVT_MAXIMIZE(self, self.OnMaximize)

        self.Centre(wx.BOTH)
        self.CreateStatusBar(1, wx.ST_SIZEGRIP)

        splitter = wx.SplitterWindow(self, -1, style=wx.NO_3D|wx.SP_3D)
        splitter2 = wx.SplitterWindow(splitter, -1, style=wx.NO_3D|wx.SP_3D)

        def EmptyHandler(evt): pass
        wx.EVT_ERASE_BACKGROUND(splitter, EmptyHandler)
        wx.EVT_ERASE_BACKGROUND(splitter2, EmptyHandler)

        # Prevent TreeCtrl from displaying all items after destruction when True
        self.dying = False

        # Make a File menu
        self.mainmenu = wx.MenuBar()
        menu = wx.Menu()
        exitID = wx.NewId()
        menu.Append(exitID, 'E&xit\tAlt-X', 'Get the heck outta here!')
        wx.EVT_MENU(self, exitID, self.OnFileExit)
        wx.App_SetMacExitMenuItemId(exitID)
        self.mainmenu.Append(menu, '&File')

        # Make a Demo menu
        menu = wx.Menu()
        for item in _treeList:
            submenu = wx.Menu()
            for childItem in item[1]:
                mID = wx.NewId()
                submenu.Append(mID, childItem)
                wx.EVT_MENU(self, mID, self.OnDemoMenu)
            menu.AppendMenu(wx.NewId(), item[0], submenu)
        self.mainmenu.Append(menu, '&Demo')


        # Make a Help menu
        helpID = wx.NewId()
        findID = wx.NewId()
        findnextID = wx.NewId()
        menu = wx.Menu()
        menu.Append(findID, '&Find\tCtrl-F', 'Find in the Demo Code')
        menu.Append(findnextID, 'Find &Next\tF3', 'Find Next')
        menu.AppendSeparator()
        menu.Append(helpID, '&About\tCtrl-H', 'wxPython RULES!!!')
        wx.App_SetMacAboutMenuItemId(helpID)
        wx.EVT_MENU(self, helpID, self.OnHelpAbout)
        wx.EVT_MENU(self, findID, self.OnHelpFind)
        wx.EVT_MENU(self, findnextID, self.OnFindNext)
        wx.EVT_COMMAND_FIND(self, -1, self.OnFind)
        wx.EVT_COMMAND_FIND_NEXT(self, -1, self.OnFind)
        wx.EVT_COMMAND_FIND_CLOSE(self, -1 , self.OnFindClose)
        self.mainmenu.Append(menu, '&Help')
        self.SetMenuBar(self.mainmenu)

        self.finddata = wx.FindReplaceData()

        if 0:
            # This is another way to set Accelerators, in addition to
            # using the '\t<key>' syntax in the menu items.
            aTable = wx.AcceleratorTable([(wx.ACCEL_ALT,  ord('X'), exitID),
                                          (wx.ACCEL_CTRL, ord('H'), helpID),
                                          (wx.ACCEL_CTRL, ord('F'), findID),
                                          (wx.ACCEL_NORMAL, WXK_F3, findnextID)
                                          ])
            self.SetAcceleratorTable(aTable)


        # Create a TreeCtrl
        tID = wx.NewId()
        self.treeMap = {}
        self.tree = wx.TreeCtrl(splitter, tID,
                                style=wx.TR_HAS_BUTTONS |
                                wx.TR_HAS_VARIABLE_ROW_HEIGHT
                               )

        root = self.tree.AddRoot("wxPython Overview")
        firstChild = None
        for item in _treeList:
            child = self.tree.AppendItem(root, item[0])
            if not firstChild: firstChild = child
            for childItem in item[1]:
                theDemo = self.tree.AppendItem(child, childItem)
                self.treeMap[childItem] = theDemo

        self.tree.Expand(root)
        self.tree.Expand(firstChild)
        wx.EVT_TREE_ITEM_EXPANDED   (self.tree, tID, self.OnItemExpanded)
        wx.EVT_TREE_ITEM_COLLAPSED  (self.tree, tID, self.OnItemCollapsed)
        wx.EVT_TREE_SEL_CHANGED     (self.tree, tID, self.OnSelChanged)
        wx.EVT_LEFT_DOWN            (self.tree,      self.OnTreeLeftDown)

        # Create a Notebook
        self.nb = wx.Notebook(splitter2, -1, style=wx.CLIP_CHILDREN)

        # Set up a wx.html.HtmlWindow on the Overview Notebook page
        # we put it in a panel first because there seems to be a
        # refresh bug of some sort (wxGTK) when it is directly in
        # the notebook...
        if 0:  # the old way
            self.ovr = wx.html.HtmlWindow(self.nb, -1, size=(400, 400))
            self.nb.AddPage(self.ovr, self.overviewText)

        else:  # hopefully I can remove this hacky code soon, see SF bug #216861
            panel = wx.Panel(self.nb, -1, style=wx.CLIP_CHILDREN)
            self.ovr = wx.html.HtmlWindow(panel, -1, size=(400, 400))
            self.nb.AddPage(panel, self.overviewText)

            def OnOvrSize(evt, ovr=self.ovr):
                ovr.SetSize(evt.GetSize())

            wx.EVT_SIZE(panel, OnOvrSize)
            wx.EVT_ERASE_BACKGROUND(panel, EmptyHandler)


        self.SetOverview(self.overviewText, overview)


        # Set up a TextCtrl on the Demo Code Notebook page
        self.txt = wx.TextCtrl(self.nb, -1,
                              style = wx.TE_MULTILINE|wx.TE_READONLY|
                              wx.HSCROLL|wx.TE_RICH2|wx.TE_NOHIDESEL)
        self.nb.AddPage(self.txt, "Demo Code")


        # Set up a log on the View Log Notebook page
        self.log = wx.TextCtrl(splitter2, -1,
                              style = wx.TE_MULTILINE|wx.TE_READONLY|wx.HSCROLL)

        # Set the wxWindows log target to be this textctrl
        #wx.Log_SetActiveTarget(wx.LogTextCtrl(self.log))

        # But instead of the above we want to show how to use our own wx.Log class
        wx.Log_SetActiveTarget(MyLog(self.log))

        # for serious debugging
        #wx.Log_SetActiveTarget(wx.LogStderr())
        #wx.Log_SetTraceMask(wx.TraceMessages)

        self.Show(True)


        # add the windows to the splitter and split it.
        splitter2.SplitHorizontally(self.nb, self.log, 450)
        splitter.SplitVertically(self.tree, splitter2, 180)

        splitter.SetMinimumPaneSize(20)
        splitter2.SetMinimumPaneSize(20)



        # select initial items
        self.nb.SetSelection(0)
        self.tree.SelectItem(root)

        if len(sys.argv) == 2:
            try:
                selectedDemo = self.treeMap[sys.argv[1]]
            except:
                selectedDemo = None
            if selectedDemo:
                self.tree.SelectItem(selectedDemo)
                self.tree.EnsureVisible(selectedDemo)


        wx.LogMessage('window handle: %s' % self.GetHandle())


    #---------------------------------------------
    def WriteText(self, text):
        if text[-1:] == '\n':
            text = text[:-1]
        wx.LogMessage(text)


    def write(self, txt):
        self.WriteText(txt)

    #---------------------------------------------
    def OnItemExpanded(self, event):
        item = event.GetItem()
        wx.LogMessage("OnItemExpanded: %s" % self.tree.GetItemText(item))
        event.Skip()

    #---------------------------------------------
    def OnItemCollapsed(self, event):
        item = event.GetItem()
        wx.LogMessage("OnItemCollapsed: %s" % self.tree.GetItemText(item))
        event.Skip()

    #---------------------------------------------
    def OnTreeLeftDown(self, event):
        pt = event.GetPosition();
        item, flags = self.tree.HitTest(pt)
        if item == self.tree.GetSelection():
            self.SetOverview(self.tree.GetItemText(item)+" Overview", self.curOverview)
        event.Skip()

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
            # inform the window that it's time to quit if it cares
            if self.window is not None:
                if hasattr(self.window, "ShutdownDemo"):
                    self.window.ShutdownDemo()
            wx.SafeYield() # in case the page has pending events
            self.nb.DeletePage(2)

        if itemText == self.overviewText:
            self.GetDemoFile('Main.py')
            self.SetOverview(self.overviewText, overview)
            self.nb.Refresh();
            self.window = None

        else:
            if os.path.exists(itemText + '.py'):
                wx.BeginBusyCursor()
                wx.LogMessage("Running demo %s.py..." % itemText)
                try:
                    self.GetDemoFile(itemText + '.py')
                    module = __import__(itemText, globals())
                    self.SetOverview(itemText + " Overview", module.overview)
                finally:
                    wx.EndBusyCursor()
                self.tree.Refresh()

                # in case runTest is modal, make sure things look right...
                self.nb.Refresh();
                wx.SafeYield()

                self.window = module.runTest(self, self.nb, self) ###
                if self.window is not None:
                    self.nb.AddPage(self.window, 'Demo')
                    self.nb.SetSelection(2)
                    self.nb.Refresh()  # without this wxMac has troubles showing the just added page

            else:
                self.ovr.SetPage("")
                self.txt.Clear()
                self.window = None



    #---------------------------------------------
    # Get the Demo files
    def GetDemoFile(self, filename):
        self.txt.Clear()
        try:
            self.txt.SetValue(open(filename).read())
        except IOError:
            self.txt.WriteText("Cannot open %s file." % filename)

        self.txt.SetInsertionPoint(0)
        self.txt.ShowPosition(0)

    #---------------------------------------------
    def SetOverview(self, name, text):
        self.curOverview = text
        lead = text[:6]
        if lead != '<html>' and lead != '<HTML>':
            text = '<br>'.join(text.split('\n'))
        self.ovr.SetPage(text)
        self.nb.SetPageText(0, name)

    #---------------------------------------------
    # Menu methods
    def OnFileExit(self, *event):
        self.Close()

    def OnHelpAbout(self, event):
        from About import MyAboutBox
        about = MyAboutBox(self)
        about.ShowModal()
        about.Destroy()

    def OnHelpFind(self, event):
        self.nb.SetSelection(1)
        self.finddlg = wx.FindReplaceDialog(self, self.finddata, "Find",
                        wx.FR_NOUPDOWN |
                        wx.FR_NOMATCHCASE |
                        wx.FR_NOWHOLEWORD)
        self.finddlg.Show(True)

    def OnFind(self, event):
        self.nb.SetSelection(1)
        end = self.txt.GetLastPosition()
        textstring = self.txt.GetRange(0, end).lower()
        start = self.txt.GetSelection()[1]
        findstring = self.finddata.GetFindString().lower()
        loc = textstring.find(findstring, start)
        if loc == -1 and start != 0:
            # string not found, start at beginning
            start = 0
            loc = textstring.find(findstring, start)
        if loc == -1:
            dlg = wx.MessageDialog(self, 'Find String Not Found',
                          'Find String Not Found in Demo File',
                          wx.OK | wx.ICON_INFORMATION)
            dlg.ShowModal()
            dlg.Destroy()
        if self.finddlg:
            if loc == -1:
                self.finddlg.SetFocus()
                return
            else:
                self.finddlg.Destroy()
        self.txt.SetSelection(loc, loc + len(findstring))
        self.txt.ShowPosition(loc)



    def OnFindNext(self, event):
        if self.finddata.GetFindString():
            self.OnFind(event)
        else:
            self.OnHelpFind(event)

    def OnFindClose(self, event):
        event.GetDialog().Destroy()


    #---------------------------------------------
    def OnCloseWindow(self, event):
        self.dying = True
        self.window = None
        self.mainmenu = None
        if hasattr(self, "tbicon"):
            del self.tbicon
        self.Destroy()


    #---------------------------------------------
    def OnIdle(self, event):
        if self.otherWin:
            self.otherWin.Raise()
            self.window = self.otherWin
            self.otherWin = None


    #---------------------------------------------
    def ShowTip(self):
        try:
            showTipText = open(opj("data/showTips")).read()
            showTip, index = eval(showTipText)
        except IOError:
            showTip, index = (1, 0)
        if showTip:
            tp = wx.CreateFileTipProvider(opj("data/tips.txt"), index)
            ##tp = MyTP(0)
            showTip = wx.ShowTip(self, tp)
            index = tp.GetCurrentTip()
            open(opj("data/showTips"), "w").write(str( (showTip, index) ))


    #---------------------------------------------
    def OnDemoMenu(self, event):
        try:
            selectedDemo = self.treeMap[self.mainmenu.GetLabel(event.GetId())]
        except:
            selectedDemo = None
        if selectedDemo:
            self.tree.SelectItem(selectedDemo)
            self.tree.EnsureVisible(selectedDemo)


    #---------------------------------------------
    def OnTaskBarActivate(self, evt):
        if self.IsIconized():
            self.Iconize(False)
        if not self.IsShown():
            self.Show(True)
        self.Raise()

    #---------------------------------------------

    TBMENU_RESTORE = 1000
    TBMENU_CLOSE   = 1001

    def OnTaskBarMenu(self, evt):
        menu = wx.Menu()
        menu.Append(self.TBMENU_RESTORE, "Restore wxPython Demo")
        menu.Append(self.TBMENU_CLOSE,   "Close")
        self.tbicon.PopupMenu(menu)
        menu.Destroy()

    #---------------------------------------------
    def OnTaskBarClose(self, evt):
        self.Close()

        # because of the way wx.TaskBarIcon.PopupMenu is implemented we have to
        # prod the main idle handler a bit to get the window to actually close
        wx.GetApp().ProcessIdle()


    #---------------------------------------------
    def OnIconfiy(self, evt):
        wx.LogMessage("OnIconfiy")
        evt.Skip()

    #---------------------------------------------
    def OnMaximize(self, evt):
        wx.LogMessage("OnMaximize")
        evt.Skip()




#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

class MySplashScreen(wx.SplashScreen):
    def __init__(self):
        bmp = wx.Image(opj("bitmaps/splash.gif")).ConvertToBitmap()
        wx.SplashScreen.__init__(self, bmp,
                                wx.SPLASH_CENTRE_ON_SCREEN|wx.SPLASH_TIMEOUT,
                                4000, None, -1,
                                style = wx.SIMPLE_BORDER|wx.FRAME_NO_TASKBAR|wx.STAY_ON_TOP)
        wx.EVT_CLOSE(self, self.OnClose)

    def OnClose(self, evt):
        frame = wxPythonDemo(None, -1, "wxPython: (A Demonstration)")
        frame.Show()
        evt.Skip()  # Make sure the default handler runs too...


class MyApp(wx.App):
    def OnInit(self):
        """
        Create and show the splash screen.  It will then create and show
        the main frame when it is time to do so.
        """

        #import locale
        #self.locale = wx.Locale(wx.LANGUAGE_FRENCH)
        #locale.setlocale(locale.LC_ALL, 'fr')

        wx.InitAllImageHandlers()
        splash = MySplashScreen()
        splash.Show()
        return True



#---------------------------------------------------------------------------

def main():
    try:
        demoPath = os.path.dirname(__file__)
        os.chdir(demoPath)
    except:
        pass
    app = MyApp(wx.Platform == "__WXMAC__")
    app.MainLoop()


#---------------------------------------------------------------------------



overview = """<html><body>
<h2>wxPython</h2>

<p> wxPython is a <b>GUI toolkit</b> for the <a
href="http://www.python.org/">Python</a> programming language.  It
allows Python programmers to create programs with a robust, highly
functional graphical user interface, simply and easily.  It is
implemented as a Python extension module (native code) that wraps the
popular <a href="http://wxwindows.org/front.htm">wxWindows</a> cross
platform GUI library, which is written in C++.

<p> Like Python and wxWindows, wxPython is <b>Open Source</b> which
means that it is free for anyone to use and the source code is
available for anyone to look at and modify.  Or anyone can contribute
fixes or enhnacments to the project.

<p> wxPython is a <b>cross-platform</b> toolkit.  This means that the
same program will run on multiple platforms without modification.
Currently supported platforms are 32-bit Microsoft Windows, most Unix
or unix-like systems, and Macintosh OS X. Since the language is
Python, wxPython programs are <b>simple, easy</b> to write and easy to
understand.

<p> <b>This demo</b> is not only a collection of test cases for
wxPython, but is also designed to help you learn about and how to use
wxPython.  Each sample is listed in the tree control on the left.
When a sample is selected in the tree then a module is loaded and run
(usually in a tab of this notebook,) and the source code of the module
is loaded in another tab for you to browse and learn from.

"""


#----------------------------------------------------------------------------
#----------------------------------------------------------------------------

if __name__ == '__main__':
    main()

#----------------------------------------------------------------------------







