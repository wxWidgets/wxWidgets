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

import wx                  # This module uses the new wx namespace
import wx.html

import images

# For debugging
##wx.Trap();
##print "wx.VERSION_STRING = ", wx.VERSION_STRING
##print os.getpid();
##raw_input("Press Enter...")


#---------------------------------------------------------------------------


_treeList = [
    # new stuff
    ('Recent Additions/Updates', [
        'OGL',
        'FloatCanvas',
        ]),

    # managed windows == things with a (optional) caption you can close
    ('Base Frames and Dialogs', [
        'Dialog',
        'Frame',
        'MDIWindows',
        'MiniFrame',
        'Wizard',
        ]),

    # the common dialogs
    ('Common Dialogs', [
        'ColourDialog',
        'DirDialog',
        'FileDialog',
        'FileDialog_Save',
        'FindReplaceDialog',
        'FontDialog',
        'MessageDialog',
        'PageSetupDialog',
        'PrintDialog',
        'ProgressDialog',
        'SingleChoiceDialog',
        'TextEntryDialog',
        ]),

    # dialogs from libraries
    ('More Dialogs', [
        'ImageBrowser',
        'MultipleChoiceDialog',
        'ScrolledMessageDialog',
        ]),

    # core controls
    ('Core Windows/Controls', [
        'BitmapButton',
        'Button',
        'CheckBox',
        'CheckListBox',
        'Choice',
        'ComboBox',
        'Gauge',
        'Grid',
        'Grid_MegaExample',
        'ListBox',
        'ListCtrl',
        'ListCtrl_virtual',
        'Listbook',
        'Menu',
        'Notebook',
        'PopupMenu',
        'PopupWindow',
        'RadioBox',
        'RadioButton',
        'SashWindow',
        'ScrolledWindow',
        'Slider',
        'SpinButton',
        'SpinCtrl',
        'SplitterWindow',
        'StaticBitmap',
        'StaticText',
        'StatusBar',
        'TextCtrl',
        'ToggleButton',
        'ToolBar',
        'TreeCtrl',
        'Validator',
        ]),

    ('Custom Controls', [
        'AnalogClockWindow',
        'ColourSelect',
        'Editor',
        'GenericButtons',
        'GenericDirCtrl',
        'LEDNumberCtrl',
        'MultiSash',
        'PopupControl',
        'PyColourChooser',
        'TreeListCtrl',
    ]),
    
    # controls coming from other libraries
    ('More Windows/Controls', [
        'ActiveX_FlashWindow',
        'ActiveX_IEHtmlWindow',
        'ActiveX_PDFWindow',
        #'RightTextCtrl',     deprecated as we have wxTE_RIGHT now.
        'Calendar',
        'CalendarCtrl',
        'ContextHelp',
        'DynamicSashWindow',
        'EditableListBox',
        'FancyText',
        'FileBrowseButton',
        'FloatBar',  
        'FloatCanvas',
        'HtmlWindow',
        'IntCtrl',
        'MVCTree',   
        'MaskedEditControls',
        'MaskedNumCtrl',
        'MimeTypesManager',
        'PyCrust',
        'PyPlot',
        'PyShell',
        'ScrolledPanel',
        'SplitTree',
        'StyledTextCtrl_1',
        'StyledTextCtrl_2',
        'TablePrint',
        'Throbber',
        'TimeCtrl',
        'VListBox',
        ]),

    # How to lay out the controls in a frame/dialog
    ('Window Layout', [
        'GridBagSizer',
        'LayoutAnchors',
        'LayoutConstraints',
        'Layoutf',
        'RowColSizer',
        'ScrolledPanel',
        'Sizers',
        'XmlResource',
        'XmlResourceHandler',
        'XmlResourceSubclass',
        ]),

    # ditto
    ('Process and Events', [
        'EventManager',
        'KeyEvents',
        'Process',
        'PythonEvents',
        'Threads',
        'Timer',
        'infoframe',
        ]),

    # Clipboard and DnD
    ('Clipboard and DnD', [
        'CustomDragAndDrop',
        'DragAndDrop',
        'URLDragAndDrop',
        ]),

    # Images
    ('Using Images', [
        'ArtProvider',
        'Cursor',
        'DragImage',
        'Image',
        'ImageAlpha',
        'ImageFromStream',
        'Mask',
        'Throbber',
        ]),

    # Other stuff
    ('Miscellaneous', [
        'ColourDB',
        'DialogUnits',
        'DrawXXXList',
        'FileHistory',
        'FontEnumerator',
        'Joystick',
        'OGL',
        'PrintFramework',
        'ShapedWindow',
        'Sound',
        'Unicode',
        ]),

    # need libs not coming with the demo
    ('Samples using an external library', [
        'GLCanvas',
        ]),


    ('Check out the samples dir too', [
        ]),

]



#---------------------------------------------------------------------------
# Show how to derive a custom wxLog class

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
# A class to be used to display source code in the demo.  Try using the
# wxSTC in the StyledTextCtrl_2 sample first, fall back to wxTextCtrl
# if there is an error, such as the stc module not being present.
#

try:
    ##raise ImportError     # for testing the alternate implementation
    from wx import stc
    from StyledTextCtrl_2 import PythonSTC
    class DemoCodeViewer(PythonSTC):
        def __init__(self, parent, ID):
            PythonSTC.__init__(self, parent, ID, wx.BORDER_NONE)
            self.SetUpEditor()

        # Some methods to make it compatible with how the wxTextCtrl is used
        def SetValue(self, value):
            self.SetReadOnly(False)
            self.SetText(value)
            self.SetReadOnly(True)

        def Clear(self):
            self.ClearAll()

        def SetInsertionPoint(self, pos):
            self.SetCurrentPos(pos)

        def ShowPosition(self, pos):
            self.GotoPos(pos)

        def GetLastPosition(self):
            return self.GetLength()

        def GetRange(self, start, end):
            return self.GetTextRange(start, end)

        def GetSelection(self):
            return self.GetAnchor(), self.GetCurrentPos()

        def SetSelection(self, start, end):
            self.SetSelectionStart(start)
            self.SetSelectionEnd(end)

        def SetUpEditor(self):
            """
            This method carries out the work of setting up the demo editor.            
            It's seperate so as not to clutter up the init code.
            """
            import keyword
            
            self.SetLexer(stc.STC_LEX_PYTHON)
            self.SetKeyWords(0, " ".join(keyword.kwlist))
    
            # Enable folding
            self.SetProperty("fold", "1" ) 

            # Highlight tab/space mixing (shouldn't be any)
            self.SetProperty("tab.timmy.whinge.level", "1")

            # Set left and right margins
            self.SetMargins(2,2)
    
            # Set up the numbers in the margin for margin #1
            self.SetMarginType(1, wx.stc.STC_MARGIN_NUMBER)
            # Reasonable value for, say, 4-5 digits using a mono font (40 pix)
            self.SetMarginWidth(1, 40)
    
            # Indentation and tab stuff
            self.SetIndent(4)               # Proscribed indent size for wx
            self.SetIndentationGuides(True) # Show indent guides
            self.SetBackSpaceUnIndents(True)# Backspace unindents rather than delete 1 space
            self.SetTabIndents(True)        # Tab key indents
            self.SetTabWidth(4)             # Proscribed tab size for wx
            self.SetUseTabs(False)          # Use spaces rather than tabs, or
                                            # TabTimmy will complain!    
            # White space
            self.SetViewWhiteSpace(False)   # Don't view white space
    
            # EOL
            #self.SetEOLMode(wx.stc.STC_EOL_CRLF)  # Just leave it at the default (autosense)
            self.SetViewEOL(False)    
            # No right-edge mode indicator
            self.SetEdgeMode(stc.STC_EDGE_NONE)
    
            # Setup a margin to hold fold markers
            self.SetMarginType(2, stc.STC_MARGIN_SYMBOL)
            self.SetMarginMask(2, stc.STC_MASK_FOLDERS)
            self.SetMarginSensitive(2, True)
            self.SetMarginWidth(2, 12)
    
            # and now set up the fold markers
            self.MarkerDefine(stc.STC_MARKNUM_FOLDEREND,     stc.STC_MARK_BOXPLUSCONNECTED,  "white", "black")
            self.MarkerDefine(stc.STC_MARKNUM_FOLDEROPENMID, stc.STC_MARK_BOXMINUSCONNECTED, "white", "black")
            self.MarkerDefine(stc.STC_MARKNUM_FOLDERMIDTAIL, stc.STC_MARK_TCORNER,  "white", "black")
            self.MarkerDefine(stc.STC_MARKNUM_FOLDERTAIL,    stc.STC_MARK_LCORNER,  "white", "black")
            self.MarkerDefine(stc.STC_MARKNUM_FOLDERSUB,     stc.STC_MARK_VLINE,    "white", "black")
            self.MarkerDefine(stc.STC_MARKNUM_FOLDER,        stc.STC_MARK_BOXPLUS,  "white", "black")
            self.MarkerDefine(stc.STC_MARKNUM_FOLDEROPEN,    stc.STC_MARK_BOXMINUS, "white", "black")
    
            # Global default style
            if wx.Platform == '__WXMSW__':
                self.StyleSetSpec(stc.STC_STYLE_DEFAULT, 
                                  'fore:#000000,back:#FFFFFF,face:Courier New,size:9')
            else:
                self.StyleSetSpec(stc.STC_STYLE_DEFAULT, 
                                  'fore:#000000,back:#FFFFFF,face:Courier,size:12')
    
            # Clear styles and revert to default.
            self.StyleClearAll()

            # Following style specs only indicate differences from default.
            # The rest remains unchanged.

            # Line numbers in margin
            self.StyleSetSpec(wx.stc.STC_STYLE_LINENUMBER,'fore:#000000,back:#99A9C2')
    
            # Highlighted brace
            self.StyleSetSpec(wx.stc.STC_STYLE_BRACELIGHT,'fore:#00009D,back:#FFFF00')
            # Unmatched brace
            self.StyleSetSpec(wx.stc.STC_STYLE_BRACEBAD,'fore:#00009D,back:#FF0000')
            # Indentation guide
            self.StyleSetSpec(wx.stc.STC_STYLE_INDENTGUIDE, "fore:#CDCDCD")
    
            # Python styles
            self.StyleSetSpec(wx.stc.STC_P_DEFAULT, 'fore:#000000')
            # Comments
            self.StyleSetSpec(wx.stc.STC_P_COMMENTLINE,  'fore:#008000,back:#F0FFF0')
            self.StyleSetSpec(wx.stc.STC_P_COMMENTBLOCK, 'fore:#008000,back:#F0FFF0')
            # Numbers
            self.StyleSetSpec(wx.stc.STC_P_NUMBER, 'fore:#008080')
            # Strings and characters
            self.StyleSetSpec(wx.stc.STC_P_STRING, 'fore:#800080')
            self.StyleSetSpec(wx.stc.STC_P_CHARACTER, 'fore:#800080')
            # Keywords
            self.StyleSetSpec(wx.stc.STC_P_WORD, 'fore:#000080,bold')
            # Triple quotes
            self.StyleSetSpec(wx.stc.STC_P_TRIPLE, 'fore:#800080,back:#FFFFEA')
            self.StyleSetSpec(wx.stc.STC_P_TRIPLEDOUBLE, 'fore:#800080,back:#FFFFEA')
            # Class names
            self.StyleSetSpec(wx.stc.STC_P_CLASSNAME, 'fore:#0000FF,bold')
            # Function names
            self.StyleSetSpec(wx.stc.STC_P_DEFNAME, 'fore:#008080,bold')
            # Operators
            self.StyleSetSpec(wx.stc.STC_P_OPERATOR, 'fore:#800000,bold')
            # Identifiers. I leave this as not bold because everything seems
            # to be an identifier if it doesn't match the above criterae
            self.StyleSetSpec(wx.stc.STC_P_IDENTIFIER, 'fore:#000000')

            # Caret color
            self.SetCaretForeground("BLUE")
            # Selection background
            self.SetSelBackground(1, '#66CCFF')

            self.SetSelBackground(True, wx.SystemSettings_GetColour(wx.SYS_COLOUR_HIGHLIGHT))
            self.SetSelForeground(True, wx.SystemSettings_GetColour(wx.SYS_COLOUR_HIGHLIGHTTEXT))


except ImportError:
    class DemoCodeViewer(wx.TextCtrl):
        def __init__(self, parent, ID):
            wx.TextCtrl.__init__(self, parent, ID, style =
                                 wx.TE_MULTILINE | wx.TE_READONLY |
                                 wx.HSCROLL | wx.TE_RICH2 | wx.TE_NOHIDESEL)


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

        if wx.Platform != '__WXMAC__':
            # setup a taskbar icon, and catch some events from it
            dim = 16  # (may want to use 22 on wxGTK...)
            icon = wx.IconFromBitmap(
                images.getMondrianImage().Scale(dim,dim).ConvertToBitmap() )
            #icon = wx.Icon('bmp_source/mondrian.ico', wx.BITMAP_TYPE_ICO)
            #icon = images.getMondrianIcon()
            self.tbicon = wx.TaskBarIcon()
            self.tbicon.SetIcon(icon, "wxPython Demo")
            self.tbicon.Bind(wx.EVT_TASKBAR_LEFT_DCLICK, self.OnTaskBarActivate)
            self.tbicon.Bind(wx.EVT_TASKBAR_RIGHT_UP, self.OnTaskBarMenu)
            self.tbicon.Bind(wx.EVT_MENU, self.OnTaskBarActivate, id=self.TBMENU_RESTORE)
            self.tbicon.Bind(wx.EVT_MENU, self.OnTaskBarClose, id=self.TBMENU_CLOSE)

        wx.CallAfter(self.ShowTip)

        self.otherWin = None
        self.Bind(wx.EVT_IDLE, self.OnIdle)
        self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)
        self.Bind(wx.EVT_ICONIZE, self.OnIconfiy)
        self.Bind(wx.EVT_MAXIMIZE, self.OnMaximize)

        self.Centre(wx.BOTH)
        self.CreateStatusBar(1, wx.ST_SIZEGRIP)

        splitter = wx.SplitterWindow(self, -1, style=wx.CLIP_CHILDREN | wx.SP_LIVE_UPDATE | wx.SP_3D)
        splitter2 = wx.SplitterWindow(splitter, -1, style=wx.CLIP_CHILDREN | wx.SP_LIVE_UPDATE | wx.SP_3D)

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



        def EmptyHandler(evt): pass
        #splitter.Bind(wx.EVT_ERASE_BACKGROUND, EmptyHandler)
        #splitter2.Bind(wx.EVT_ERASE_BACKGROUND, EmptyHandler)

        # Prevent TreeCtrl from displaying all items after destruction when True
        self.dying = False

        # Make a File menu
        self.mainmenu = wx.MenuBar()
        menu = wx.Menu()
        item = menu.Append(-1, '&Redirect Output',
                           'Redirect print statements to a window',
                           wx.ITEM_CHECK)
        self.Bind(wx.EVT_MENU, self.OnToggleRedirect, item)
        
        item = menu.Append(-1, 'E&xit\tAlt-X', 'Get the heck outta here!')
        self.Bind(wx.EVT_MENU, self.OnFileExit, item)
        wx.App_SetMacExitMenuItemId(item.GetId())

        self.mainmenu.Append(menu, '&File')

        # Make a Demo menu
        menu = wx.Menu()
        for item in _treeList:
            submenu = wx.Menu()
            for childItem in item[1]:
                mi = submenu.Append(-1, childItem)
                self.Bind(wx.EVT_MENU, self.OnDemoMenu, mi)
            menu.AppendMenu(wx.NewId(), item[0], submenu)
        self.mainmenu.Append(menu, '&Demo')


        # Make a Help menu
        helpID = wx.NewId()
        findID = wx.NewId()
        findnextID = wx.NewId()
        menu = wx.Menu()
        findItem = menu.Append(-1, '&Find\tCtrl-F', 'Find in the Demo Code')
        findnextItem = menu.Append(-1, 'Find &Next\tF3', 'Find Next')
        menu.AppendSeparator()
        helpItem = menu.Append(-1, '&About\tCtrl-H', 'wxPython RULES!!!')
        wx.App_SetMacAboutMenuItemId(helpItem.GetId())
        self.Bind(wx.EVT_MENU, self.OnHelpAbout, helpItem)
        self.Bind(wx.EVT_MENU, self.OnHelpFind,  findItem)
        self.Bind(wx.EVT_MENU, self.OnFindNext,  findnextItem)
        self.Bind(wx.EVT_COMMAND_FIND, self.OnFind)
        self.Bind(wx.EVT_COMMAND_FIND_NEXT, self.OnFind)
        self.Bind(wx.EVT_COMMAND_FIND_CLOSE, self.OnFindClose)
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
        self.tree = wx.TreeCtrl(splitter, tID, style =
                                wx.TR_DEFAULT_STYLE #| wx.TR_HAS_VARIABLE_ROW_HEIGHT
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
        self.tree.Bind(wx.EVT_TREE_ITEM_EXPANDED, self.OnItemExpanded, id=tID)
        self.tree.Bind(wx.EVT_TREE_ITEM_COLLAPSED, self.OnItemCollapsed, id=tID)
        self.tree.Bind(wx.EVT_TREE_SEL_CHANGED, self.OnSelChanged, id=tID)
        self.tree.Bind(wx.EVT_LEFT_DOWN, self.OnTreeLeftDown)

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

            panel.Bind(wx.EVT_SIZE, OnOvrSize)
            panel.Bind(wx.EVT_ERASE_BACKGROUND, EmptyHandler)


        self.SetOverview(self.overviewText, overview)


        # Set up a notebook page for viewing the source code of each sample
        self.txt = DemoCodeViewer(self.nb, -1)
        self.nb.AddPage(self.txt, "Demo Code")
        self.LoadDemoSource('Main.py')


        # add the windows to the splitter and split it.
        splitter2.SplitHorizontally(self.nb, self.log, -120)
        splitter.SplitVertically(self.tree, splitter2, 180)

        splitter.SetMinimumPaneSize(20)
        splitter2.SetMinimumPaneSize(20)


        # Make the splitter on the right expand the top window when resized
        def SplitterOnSize(evt):
            splitter = evt.GetEventObject()
            sz = splitter.GetSize()
            splitter.SetSashPosition(sz.height - 120, False)
            evt.Skip()

        splitter2.Bind(wx.EVT_SIZE, SplitterOnSize)


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


##        wx.LogMessage('window handle: %s' % self.GetHandle())


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
            self.LoadDemoSource('Main.py')
            self.SetOverview(self.overviewText, overview)
            self.window = None

        else:
            if os.path.exists(itemText + '.py'):
                wx.BeginBusyCursor()
                wx.LogMessage("Running demo %s.py..." % itemText)
                try:
                    self.LoadDemoSource(itemText + '.py')

                    if (sys.modules.has_key(itemText)):
                       reload(sys.modules[itemText])
                    
                    module = __import__(itemText, globals())
                    self.SetOverview(itemText + " Overview", module.overview)
                finally:
                    wx.EndBusyCursor()
                self.tree.Refresh()

                self.window = module.runTest(self, self.nb, self) ###
                if self.window is not None:
                    self.nb.AddPage(self.window, 'Demo')
                    self.nb.SetSelection(2)

            else:
                self.ovr.SetPage("")
                self.txt.Clear()
                self.window = None

        self.tree.SetFocus()


    #---------------------------------------------
    # Get the Demo files
    def LoadDemoSource(self, filename):
        self.txt.Clear()
        try:
            self.txt.SetValue(open(filename).read())
        except IOError:
            self.txt.SetValue("Cannot open %s file." % filename)

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

    def OnToggleRedirect(self, event):
        app = wx.GetApp()
        if event.Checked():
            app.RedirectStdio()
            print "Print statements and other standard output will now be directed to this window."
        else:
            app.RestoreStdio()
            print "Print statements and other standard output will now be sent to the usual location."
        
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
        self.txt.ShowPosition(loc)
        self.txt.SetSelection(loc, loc + len(findstring))



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
                                 wx.SPLASH_CENTRE_ON_SCREEN | wx.SPLASH_TIMEOUT,
                                 3000, None, -1)
        self.Bind(wx.EVT_CLOSE, self.OnClose)

    def OnClose(self, evt):
        self.Hide()
        frame = wxPythonDemo(None, -1, "wxPython: (A Demonstration)")
        frame.Show()
        evt.Skip()  # Make sure the default handler runs too...


class MyApp(wx.App):
    def OnInit(self):
        """
        Create and show the splash screen.  It will then create and show
        the main frame when it is time to do so.
        """

        # Normally when using a SplashScreen you would create it, show
        # it and then continue on with the applicaiton's
        # initialization, finally creating and showing the main
        # application window(s).  In this case we have nothing else to
        # do so we'll delay showing the main frame until later (see
        # OnClose above) so the users can see the SplashScreen effect.        
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
    app = MyApp(0) ##wx.Platform == "__WXMAC__")
    app.MainLoop()


#---------------------------------------------------------------------------



overview = """<html><body>
<h2>wxPython</h2>

<p> wxPython is a <b>GUI toolkit</b> for the Python programming
language.  It allows Python programmers to create programs with a
robust, highly functional graphical user interface, simply and easily.
It is implemented as a Python extension module (native code) that
wraps the popular wxWindows cross platform GUI library, which is
written in C++.

<p> Like Python and wxWindows, wxPython is <b>Open Source</b> which
means that it is free for anyone to use and the source code is
available for anyone to look at and modify.  Or anyone can contribute
fixes or enhancements to the project.

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







