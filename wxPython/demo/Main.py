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
from   wxPython.wx import *
from   wxPython.html import wxHtmlWindow

##from wxPython.stc import *

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

class MyLog(wxPyLog):
    def __init__(self, textCtrl, logTime=0):
        wxPyLog.__init__(self)
        self.tc = textCtrl
        self.logTime = logTime

    def DoLogString(self, message, timeStamp):
        if self.logTime:
            message = time.strftime("%X", time.localtime(timeStamp)) + \
                      ": " + message
        if self.tc:
            self.tc.AppendText(message + '\n')


class MyTP(wxPyTipProvider):
    def GetTip(self):
        return "This is my tip"

#---------------------------------------------------------------------------

def opj(path):
    """Convert paths to the platform-specific separator"""
    return apply(os.path.join, tuple(path.split('/')))


#---------------------------------------------------------------------------

class wxPythonDemo(wxFrame):
    overviewText = "wxPython Overview"

    def __init__(self, parent, id, title):
        wxFrame.__init__(self, parent, -1, title, size = (800, 600),
                         style=wxDEFAULT_FRAME_STYLE|wxNO_FULL_REPAINT_ON_RESIZE)

        self.cwd = os.getcwd()
        self.curOverview = ""
        self.window = None

        icon = images.getMondrianIcon()
        self.SetIcon(icon)

        if wxPlatform == '__WXMSW__':
            # setup a taskbar icon, and catch some events from it
            self.tbicon = wxTaskBarIcon()
            self.tbicon.SetIcon(icon, "wxPython Demo")
            EVT_TASKBAR_LEFT_DCLICK(self.tbicon, self.OnTaskBarActivate)
            EVT_TASKBAR_RIGHT_UP(self.tbicon, self.OnTaskBarMenu)
            EVT_MENU(self.tbicon, self.TBMENU_RESTORE, self.OnTaskBarActivate)
            EVT_MENU(self.tbicon, self.TBMENU_CLOSE, self.OnTaskBarClose)

        wxCallAfter(self.ShowTip)

        self.otherWin = None
        EVT_IDLE(self, self.OnIdle)
        EVT_CLOSE(self, self.OnCloseWindow)
        EVT_ICONIZE(self, self.OnIconfiy)
        EVT_MAXIMIZE(self, self.OnMaximize)

        self.Centre(wxBOTH)
        self.CreateStatusBar(1, wxST_SIZEGRIP)

        splitter = wxSplitterWindow(self, -1, style=wxNO_3D|wxSP_3D)
        splitter2 = wxSplitterWindow(splitter, -1, style=wxNO_3D|wxSP_3D)

        def EmptyHandler(evt): pass
        EVT_ERASE_BACKGROUND(splitter, EmptyHandler)
        EVT_ERASE_BACKGROUND(splitter2, EmptyHandler)

        # Prevent TreeCtrl from displaying all items after destruction when True
        self.dying = False

        # Make a File menu
        self.mainmenu = wxMenuBar()
        menu = wxMenu()
        exitID = wxNewId()
        menu.Append(exitID, 'E&xit\tAlt-X', 'Get the heck outta here!')
        EVT_MENU(self, exitID, self.OnFileExit)
        wxApp_SetMacExitMenuItemId(exitID)
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
        findID = wxNewId()
        findnextID = wxNewId()
        menu = wxMenu()
        menu.Append(findID, '&Find\tCtrl-F', 'Find in the Demo Code')
        menu.Append(findnextID, 'Find &Next\tF3', 'Find Next')
        menu.AppendSeparator()
        menu.Append(helpID, '&About\tCtrl-H', 'wxPython RULES!!!')
        wxApp_SetMacAboutMenuItemId(helpID)
        EVT_MENU(self, helpID, self.OnHelpAbout)
        EVT_MENU(self, findID, self.OnHelpFind)
        EVT_MENU(self, findnextID, self.OnFindNext)
        EVT_COMMAND_FIND(self, -1, self.OnFind)
        EVT_COMMAND_FIND_NEXT(self, -1, self.OnFind)
        EVT_COMMAND_FIND_CLOSE(self, -1 , self.OnFindClose)
        self.mainmenu.Append(menu, '&Help')
        self.SetMenuBar(self.mainmenu)

        self.finddata = wxFindReplaceData()

        if 0:
            # This is another way to set Accelerators, in addition to
            # using the '\t<key>' syntax in the menu items.
            aTable = wxAcceleratorTable([(wxACCEL_ALT,  ord('X'), exitID),
                                         (wxACCEL_CTRL, ord('H'), helpID),
                                         (wxACCEL_CTRL, ord('F'), findID),
                                         (wxACCEL_NORMAL, WXK_F3, findnextID)
                                         ])
            self.SetAcceleratorTable(aTable)


        # Create a TreeCtrl
        tID = wxNewId()
        self.treeMap = {}
        self.tree = wxTreeCtrl(splitter, tID,
                               style=wxTR_HAS_BUTTONS |
                               wxTR_HAS_VARIABLE_ROW_HEIGHT
                               )

        #self.tree.SetBackgroundColour(wxNamedColour("Pink"))
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
        EVT_TREE_ITEM_EXPANDED   (self.tree, tID, self.OnItemExpanded)
        EVT_TREE_ITEM_COLLAPSED  (self.tree, tID, self.OnItemCollapsed)
        EVT_TREE_SEL_CHANGED     (self.tree, tID, self.OnSelChanged)
        EVT_LEFT_DOWN            (self.tree,      self.OnTreeLeftDown)

        # Create a Notebook
        self.nb = wxNotebook(splitter2, -1, style=wxCLIP_CHILDREN)

        # Set up a wxHtmlWindow on the Overview Notebook page
        # we put it in a panel first because there seems to be a
        # refresh bug of some sort (wxGTK) when it is directly in
        # the notebook...
        if 0:  # the old way
            self.ovr = wxHtmlWindow(self.nb, -1, size=(400, 400))
            self.nb.AddPage(self.ovr, self.overviewText)

        else:  # hopefully I can remove this hacky code soon, see SF bug #216861
            panel = wxPanel(self.nb, -1, style=wxCLIP_CHILDREN)
            self.ovr = wxHtmlWindow(panel, -1, size=(400, 400))
            self.nb.AddPage(panel, self.overviewText)

            def OnOvrSize(evt, ovr=self.ovr):
                ovr.SetSize(evt.GetSize())

            EVT_SIZE(panel, OnOvrSize)
            EVT_ERASE_BACKGROUND(panel, EmptyHandler)


        self.SetOverview(self.overviewText, overview)


        # Set up a TextCtrl on the Demo Code Notebook page
        self.txt = wxTextCtrl(self.nb, -1,
                              style = wxTE_MULTILINE|wxTE_READONLY|
                              wxHSCROLL|wxTE_RICH2|wxTE_NOHIDESEL)
        self.nb.AddPage(self.txt, "Demo Code")


        # Set up a log on the View Log Notebook page
        self.log = wxTextCtrl(splitter2, -1,
                              style = wxTE_MULTILINE|wxTE_READONLY|wxHSCROLL)

        # Set the wxWindows log target to be this textctrl
        #wxLog_SetActiveTarget(wxLogTextCtrl(self.log))

        # But instead of the above we want to show how to use our own wxLog class
        wxLog_SetActiveTarget(MyLog(self.log))

        # for serious debugging
        #wxLog_SetActiveTarget(wxLogStderr())
        #wxLog_SetTraceMask(wxTraceMessages)

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


        wxLogMessage('window handle: %s' % self.GetHandle())


    #---------------------------------------------
    def WriteText(self, text):
        if text[-1:] == '\n':
            text = text[:-1]
        wxLogMessage(text)


    def write(self, txt):
        self.WriteText(txt)

    #---------------------------------------------
    def OnItemExpanded(self, event):
        item = event.GetItem()
        wxLogMessage("OnItemExpanded: %s" % self.tree.GetItemText(item))
        event.Skip()

    #---------------------------------------------
    def OnItemCollapsed(self, event):
        item = event.GetItem()
        wxLogMessage("OnItemCollapsed: %s" % self.tree.GetItemText(item))
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
            wxSafeYield() # in case the page has pending events
            self.nb.DeletePage(2)

        if itemText == self.overviewText:
            self.GetDemoFile('Main.py')
            self.SetOverview(self.overviewText, overview)
            self.nb.Refresh();
            self.window = None

        else:
            if os.path.exists(itemText + '.py'):
                wxBeginBusyCursor()
                wxLogMessage("Running demo %s.py..." % itemText)
                try:
                    self.GetDemoFile(itemText + '.py')
                    module = __import__(itemText, globals())
                    self.SetOverview(itemText + " Overview", module.overview)
                finally:
                    wxEndBusyCursor()
                self.tree.Refresh()

                # in case runTest is modal, make sure things look right...
                self.nb.Refresh();
                wxSafeYield()

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
        self.finddlg = wxFindReplaceDialog(self, self.finddata, "Find",
                        wxFR_NOUPDOWN |
                        wxFR_NOMATCHCASE |
                        wxFR_NOWHOLEWORD)
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
            dlg = wxMessageDialog(self, 'Find String Not Found',
                          'Find String Not Found in Demo File',
                          wxOK | wxICON_INFORMATION)
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
            tp = wxCreateFileTipProvider(opj("data/tips.txt"), index)
            ##tp = MyTP(0)
            showTip = wxShowTip(self, tp)
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
        menu = wxMenu()
        menu.Append(self.TBMENU_RESTORE, "Restore wxPython Demo")
        menu.Append(self.TBMENU_CLOSE,   "Close")
        self.tbicon.PopupMenu(menu)
        menu.Destroy()

    #---------------------------------------------
    def OnTaskBarClose(self, evt):
        self.Close()

        # because of the way wxTaskBarIcon.PopupMenu is implemented we have to
        # prod the main idle handler a bit to get the window to actually close
        wxGetApp().ProcessIdle()


    #---------------------------------------------
    def OnIconfiy(self, evt):
        wxLogMessage("OnIconfiy")
        evt.Skip()

    #---------------------------------------------
    def OnMaximize(self, evt):
        wxLogMessage("OnMaximize")
        evt.Skip()




#---------------------------------------------------------------------------
#---------------------------------------------------------------------------

class MySplashScreen(wxSplashScreen):
    def __init__(self):
        bmp = wxImage(opj("bitmaps/splash.gif")).ConvertToBitmap()
        wxSplashScreen.__init__(self, bmp,
                                wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
                                4000, None, -1,
                                style = wxSIMPLE_BORDER|wxFRAME_NO_TASKBAR|wxSTAY_ON_TOP)
        EVT_CLOSE(self, self.OnClose)

    def OnClose(self, evt):
        frame = wxPythonDemo(None, -1, "wxPython: (A Demonstration)")
        frame.Show()
        evt.Skip()  # Make sure the default handler runs too...


class MyApp(wxApp):
    def OnInit(self):
        """
        Create and show the splash screen.  It will then create and show
        the main frame when it is time to do so.
        """

        #import locale
        #self.locale = wxLocale(wxLANGUAGE_FRENCH)
        #locale.setlocale(locale.LC_ALL, 'fr')

        wxInitAllImageHandlers()
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
    app = MyApp(wxPlatform == "__WXMAC__")
    app.MainLoop()


#---------------------------------------------------------------------------



overview = """<html><body>
 <h2>Python</h2>

 Python is an interpreted, interactive, object-oriented programming
 language often compared to Tcl, Perl, Scheme, or Java.

 <p> Python combines remarkable power with very clear syntax. It has
 modules, classes, exceptions, very high level dynamic data types, and
 dynamic typing.  There are interfaces to many system calls and
 libraries, and new built-in modules are easily written in C or
 C++. Python is also usable as an extension language for applications
 that need a programmable interface.  <p>

 <h2>wxWindows</h2>

 wxWindows is a free C++ framework designed to make cross-platform
 programming child's play. Well, almost. wxWindows 2 supports Windows
 3.1/95/98/NT, Unix with GTK/Motif/Lesstif, with a Mac version
 underway. Other ports are under consideration.  <p>

 wxWindows is a set of libraries that allows C++ applications to
 compile and run on several different types of computers, with minimal
 source code changes.  There is one library per supported GUI (such as
 Motif, or Windows). As well as providing a common API (Application
 Programming Interface) for GUI functionality, it provides
 functionality for accessing some commonly-used operating system
 facilities, such as copying or deleting files. wxWindows is a
 'framework' in the sense that it provides a lot of built-in
 functionality, which the application can use or replace as required,
 thus saving a great deal of coding effort. Basic data structures such
 as strings, linked lists and hash tables are also supported.

 <p>
 <h2>wxPython</h2>

 wxPython is a Python extension module that encapsulates the wxWindows
 GUI classes. Currently it is only available for the Win32 and GTK
 ports of wxWindows, but as soon as the other ports are brought up to
 the same level as Win32 and GTK, it should be fairly trivial to
 enable wxPython to be used with the new GUI.

 <p>

 The wxPython extension module attempts to mirror the class heiarchy
 of wxWindows as closely as possible. This means that there is a
 wxFrame class in wxPython that looks, smells, tastes and acts almost
 the same as the wxFrame class in the C++ version. Unfortunately,
 because of differences in the languages, wxPython doesn't match
 wxWindows exactly, but the differences should be easy to absorb
 because they are natural to Python. For example, some methods that
 return multiple values via argument pointers in C++ will return a
 tuple of values in Python.

 <p>

 There is still much to be done for wxPython, many classes still need
 to be mirrored. Also, wxWindows is still somewhat of a moving target
 so it is a bit of an effort just keeping wxPython up to date. On the
 other hand, there are enough of the core classes completed that
 useful applications can be written.

 <p>

 wxPython is close enough to the C++ version that the majority of
 the wxPython documentation is actually just notes attached to the C++
 documents that describe the places where wxPython is different. There
 is also a series of sample programs included, and a series of
 documentation pages that assist the programmer in getting started
 with wxPython.

 """


#----------------------------------------------------------------------------
#----------------------------------------------------------------------------

if __name__ == '__main__':
    main()

#----------------------------------------------------------------------------







