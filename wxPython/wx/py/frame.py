"""Base frame with menu."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

import wx
import os
from version import VERSION
import editwindow
import dispatcher

ID_NEW = wx.ID_NEW
ID_OPEN = wx.ID_OPEN
ID_REVERT = wx.ID_REVERT
ID_CLOSE = wx.ID_CLOSE
ID_SAVE = wx.ID_SAVE
ID_SAVEAS = wx.ID_SAVEAS
ID_PRINT = wx.ID_PRINT
ID_EXIT = wx.ID_EXIT
ID_UNDO = wx.ID_UNDO
ID_REDO = wx.ID_REDO
ID_CUT = wx.ID_CUT
ID_COPY = wx.ID_COPY
ID_PASTE = wx.ID_PASTE
ID_CLEAR = wx.ID_CLEAR
ID_SELECTALL = wx.ID_SELECTALL
ID_EMPTYBUFFER = wx.NewId()
ID_ABOUT = wx.ID_ABOUT
ID_HELP = wx.NewId()
ID_AUTOCOMP = wx.NewId()
ID_AUTOCOMP_SHOW = wx.NewId()
ID_AUTOCOMP_MAGIC = wx.NewId()
ID_AUTOCOMP_SINGLE = wx.NewId()
ID_AUTOCOMP_DOUBLE = wx.NewId()
ID_CALLTIPS = wx.NewId()
ID_CALLTIPS_SHOW = wx.NewId()
ID_CALLTIPS_INSERT = wx.NewId()
ID_COPY_PLUS = wx.NewId()
ID_NAMESPACE = wx.NewId()
ID_PASTE_PLUS = wx.NewId()
ID_WRAP = wx.NewId()
ID_TOGGLE_MAXIMIZE = wx.NewId()
ID_USEAA = wx.NewId()
ID_SHOW_LINENUMBERS = wx.NewId()
ID_AUTO_SAVESETTINGS = wx.NewId()
ID_SAVEHISTORY = wx.NewId()
ID_SAVEHISTORYNOW = wx.NewId()
ID_CLEARHISTORY = wx.NewId()
ID_SAVESETTINGS = wx.NewId()
ID_DELSETTINGSFILE = wx.NewId()
ID_EDITSTARTUPSCRIPT = wx.NewId()
ID_EXECSTARTUPSCRIPT = wx.NewId()
ID_STARTUP = wx.NewId()
ID_SETTINGS = wx.NewId()
ID_FIND = wx.ID_FIND
ID_FINDNEXT = wx.NewId()
ID_SHOWTOOLS = wx.NewId()



class Frame(wx.Frame):
    """Frame with standard menu items."""

    revision = __revision__

    def __init__(self, parent=None, id=-1, title='Editor',
                 pos=wx.DefaultPosition, size=wx.DefaultSize, 
                 style=wx.DEFAULT_FRAME_STYLE):
        """Create a Frame instance."""
        wx.Frame.__init__(self, parent, id, title, pos, size, style)
        self.CreateStatusBar()
        self.SetStatusText('Frame')
        import images
        self.SetIcon(images.getPyIcon())
        self.__createMenus()

        self.iconized = False
        self.findDlg = None
        self.findData = wx.FindReplaceData()
        self.findData.SetFlags(wx.FR_DOWN)

        self.Bind(wx.EVT_CLOSE, self.OnClose)
        self.Bind(wx.EVT_ICONIZE, self.OnIconize)


    def OnIconize(self, event):
        """Event handler for Iconize."""
        self.iconized = event.Iconized()


    def OnClose(self, event):
        """Event handler for closing."""
        self.Destroy()


    def __createMenus(self):
        # File Menu
        m = self.fileMenu = wx.Menu()
        m.Append(ID_NEW, '&New \tCtrl+N',
                 'New file')
        m.Append(ID_OPEN, '&Open... \tCtrl+O',
                 'Open file')
        m.AppendSeparator()
        m.Append(ID_REVERT, '&Revert \tCtrl+R',
                 'Revert to last saved version')
        m.Append(ID_CLOSE, '&Close \tCtrl+W',
                 'Close file')
        m.AppendSeparator()
        m.Append(ID_SAVE, '&Save... \tCtrl+S',
                 'Save file')
        m.Append(ID_SAVEAS, 'Save &As \tCtrl+Shift+S',
                 'Save file with new name')
        m.AppendSeparator()
        m.Append(ID_PRINT, '&Print... \tCtrl+P',
                 'Print file')
        m.AppendSeparator()
        m.Append(ID_NAMESPACE, '&Update Namespace \tCtrl+Shift+N',
                 'Update namespace for autocompletion and calltips')
        m.AppendSeparator()
        m.Append(ID_EXIT, 'E&xit\tCtrl+Q', 'Exit Program')

        # Edit
        m = self.editMenu = wx.Menu()
        m.Append(ID_UNDO, '&Undo \tCtrl+Z',
                 'Undo the last action')
        m.Append(ID_REDO, '&Redo \tCtrl+Y',
                 'Redo the last undone action')
        m.AppendSeparator()
        m.Append(ID_CUT, 'Cu&t \tCtrl+X',
                 'Cut the selection')
        m.Append(ID_COPY, '&Copy \tCtrl+C',
                 'Copy the selection')
        m.Append(ID_COPY_PLUS, 'Cop&y Plus \tCtrl+Shift+C',
                 'Copy the selection - retaining prompts')
        m.Append(ID_PASTE, '&Paste \tCtrl+V', 'Paste from clipboard')
        m.Append(ID_PASTE_PLUS, 'Past&e Plus \tCtrl+Shift+V',
                 'Paste and run commands')
        m.AppendSeparator()
        m.Append(ID_CLEAR, 'Cle&ar',
                 'Delete the selection')
        m.Append(ID_SELECTALL, 'Select A&ll \tCtrl+A',
                 'Select all text')
        m.AppendSeparator()
        m.Append(ID_EMPTYBUFFER, 'E&mpty Buffer...',
                 'Delete all the contents of the edit buffer')
        m.Append(ID_FIND, '&Find Text... \tCtrl+F',
                 'Search for text in the edit buffer')
        m.Append(ID_FINDNEXT, 'Find &Next \tF3',
                 'Find next/previous instance of the search text')

        # View
        m = self.viewMenu = wx.Menu()
        m.Append(ID_WRAP, '&Wrap Lines\tCtrl+Shift+W',
                 'Wrap lines at right edge', wx.ITEM_CHECK)
        m.Append(ID_SHOW_LINENUMBERS, '&Show Line Numbers\tCtrl+Shift+L', 'Show Line Numbers', wx.ITEM_CHECK)
        m.Append(ID_TOGGLE_MAXIMIZE, '&Toggle Maximize\tF11', 'Maximize/Restore Application')
        if hasattr(self, 'ToggleTools'):
            m.Append(ID_SHOWTOOLS,
                     'Show &Tools\tF4',
                     'Show the filling and other tools', wx.ITEM_CHECK)

        # Options
        m = self.autocompMenu = wx.Menu()
        m.Append(ID_AUTOCOMP_SHOW, 'Show &Auto Completion\tCtrl+Shift+A',
                 'Show auto completion list', wx.ITEM_CHECK)
        m.Append(ID_AUTOCOMP_MAGIC, 'Include &Magic Attributes\tCtrl+Shift+M',
                 'Include attributes visible to __getattr__ and __setattr__',
                 wx.ITEM_CHECK)
        m.Append(ID_AUTOCOMP_SINGLE, 'Include Single &Underscores\tCtrl+Shift+U',
                 'Include attibutes prefixed by a single underscore', wx.ITEM_CHECK)
        m.Append(ID_AUTOCOMP_DOUBLE, 'Include &Double Underscores\tCtrl+Shift+D',
                 'Include attibutes prefixed by a double underscore', wx.ITEM_CHECK)
        m = self.calltipsMenu = wx.Menu()
        m.Append(ID_CALLTIPS_SHOW, 'Show Call &Tips\tCtrl+Shift+T',
                 'Show call tips with argument signature and docstring', wx.ITEM_CHECK)
        m.Append(ID_CALLTIPS_INSERT, '&Insert Call Tips\tCtrl+Shift+I',
                 '&Insert Call Tips', wx.ITEM_CHECK)

        m = self.optionsMenu = wx.Menu()
        m.AppendMenu(ID_AUTOCOMP, '&Auto Completion', self.autocompMenu,
                     'Auto Completion Options')
        m.AppendMenu(ID_CALLTIPS, '&Call Tips', self.calltipsMenu,
                     'Call Tip Options')
                
        if wx.Platform == "__WXMAC__":
            m.Append(ID_USEAA, '&Use AntiAliasing',
                     'Use anti-aliased fonts', wx.ITEM_CHECK)
            
        m.AppendSeparator()

        self.historyMenu = wx.Menu()
        self.historyMenu.Append(ID_SAVEHISTORY, '&Autosave History',
                 'Automatically save history on close', wx.ITEM_CHECK)
        self.historyMenu.Append(ID_SAVEHISTORYNOW, '&Save History Now',
                 'Save history')
        self.historyMenu.Append(ID_CLEARHISTORY, '&Clear History ',
                 'Clear history')
        m.AppendMenu(-1, "&History", self.historyMenu, "History Options")

        self.startupMenu = wx.Menu()
        self.startupMenu.Append(ID_EXECSTARTUPSCRIPT,
                                'E&xecute Startup Script',
                                'Execute Startup Script', wx.ITEM_CHECK)
        self.startupMenu.Append(ID_EDITSTARTUPSCRIPT,
                                '&Edit Startup Script...',
                                'Edit Startup Script')
        m.AppendMenu(ID_STARTUP, '&Startup', self.startupMenu, 'Startup Options')

        self.settingsMenu = wx.Menu()
        self.settingsMenu.Append(ID_AUTO_SAVESETTINGS,
                                 '&Auto Save Settings',
                                 'Automatically save settings on close', wx.ITEM_CHECK)
        self.settingsMenu.Append(ID_SAVESETTINGS,
                                 '&Save Settings',
                                 'Save settings now')
        self.settingsMenu.Append(ID_DELSETTINGSFILE,
                                 '&Revert to default',
                                 'Revert to the default settings')
        m.AppendMenu(ID_SETTINGS, '&Settings', self.settingsMenu, 'Settings Options')           

        m = self.helpMenu = wx.Menu()
        m.Append(ID_HELP, '&Help\tF1', 'Help!')
        m.AppendSeparator()
        m.Append(ID_ABOUT, '&About...', 'About this program')

        b = self.menuBar = wx.MenuBar()
        b.Append(self.fileMenu, '&File')
        b.Append(self.editMenu, '&Edit')
        b.Append(self.viewMenu, '&View')
        b.Append(self.optionsMenu, '&Options')
        b.Append(self.helpMenu, '&Help')
        self.SetMenuBar(b)

        self.Bind(wx.EVT_MENU, self.OnFileNew, id=ID_NEW)
        self.Bind(wx.EVT_MENU, self.OnFileOpen, id=ID_OPEN)
        self.Bind(wx.EVT_MENU, self.OnFileRevert, id=ID_REVERT)
        self.Bind(wx.EVT_MENU, self.OnFileClose, id=ID_CLOSE)
        self.Bind(wx.EVT_MENU, self.OnFileSave, id=ID_SAVE)
        self.Bind(wx.EVT_MENU, self.OnFileSaveAs, id=ID_SAVEAS)
        self.Bind(wx.EVT_MENU, self.OnFileUpdateNamespace, id=ID_NAMESPACE)
        self.Bind(wx.EVT_MENU, self.OnFilePrint, id=ID_PRINT)
        self.Bind(wx.EVT_MENU, self.OnExit, id=ID_EXIT)
        self.Bind(wx.EVT_MENU, self.OnUndo, id=ID_UNDO)
        self.Bind(wx.EVT_MENU, self.OnRedo, id=ID_REDO)
        self.Bind(wx.EVT_MENU, self.OnCut, id=ID_CUT)
        self.Bind(wx.EVT_MENU, self.OnCopy, id=ID_COPY)
        self.Bind(wx.EVT_MENU, self.OnCopyPlus, id=ID_COPY_PLUS)
        self.Bind(wx.EVT_MENU, self.OnPaste, id=ID_PASTE)
        self.Bind(wx.EVT_MENU, self.OnPastePlus, id=ID_PASTE_PLUS)
        self.Bind(wx.EVT_MENU, self.OnClear, id=ID_CLEAR)
        self.Bind(wx.EVT_MENU, self.OnSelectAll, id=ID_SELECTALL)
        self.Bind(wx.EVT_MENU, self.OnEmptyBuffer, id=ID_EMPTYBUFFER)
        self.Bind(wx.EVT_MENU, self.OnAbout, id=ID_ABOUT)
        self.Bind(wx.EVT_MENU, self.OnHelp, id=ID_HELP)
        self.Bind(wx.EVT_MENU, self.OnAutoCompleteShow, id=ID_AUTOCOMP_SHOW)
        self.Bind(wx.EVT_MENU, self.OnAutoCompleteMagic, id=ID_AUTOCOMP_MAGIC)
        self.Bind(wx.EVT_MENU, self.OnAutoCompleteSingle, id=ID_AUTOCOMP_SINGLE)
        self.Bind(wx.EVT_MENU, self.OnAutoCompleteDouble, id=ID_AUTOCOMP_DOUBLE)
        self.Bind(wx.EVT_MENU, self.OnCallTipsShow, id=ID_CALLTIPS_SHOW)
        self.Bind(wx.EVT_MENU, self.OnCallTipsInsert, id=ID_CALLTIPS_INSERT)
        self.Bind(wx.EVT_MENU, self.OnWrap, id=ID_WRAP)
        self.Bind(wx.EVT_MENU, self.OnUseAA, id=ID_USEAA)
        self.Bind(wx.EVT_MENU, self.OnToggleMaximize, id=ID_TOGGLE_MAXIMIZE)
        self.Bind(wx.EVT_MENU, self.OnShowLineNumbers, id=ID_SHOW_LINENUMBERS)
        self.Bind(wx.EVT_MENU, self.OnAutoSaveSettings, id=ID_AUTO_SAVESETTINGS)
        self.Bind(wx.EVT_MENU, self.OnSaveHistory, id=ID_SAVEHISTORY)
        self.Bind(wx.EVT_MENU, self.OnSaveHistoryNow, id=ID_SAVEHISTORYNOW)
        self.Bind(wx.EVT_MENU, self.OnClearHistory, id=ID_CLEARHISTORY)
        self.Bind(wx.EVT_MENU, self.OnSaveSettings, id=ID_SAVESETTINGS)
        self.Bind(wx.EVT_MENU, self.OnDelSettingsFile, id=ID_DELSETTINGSFILE)
        self.Bind(wx.EVT_MENU, self.OnEditStartupScript, id=ID_EDITSTARTUPSCRIPT)
        self.Bind(wx.EVT_MENU, self.OnExecStartupScript, id=ID_EXECSTARTUPSCRIPT)
        self.Bind(wx.EVT_MENU, self.OnFindText, id=ID_FIND)
        self.Bind(wx.EVT_MENU, self.OnFindNext, id=ID_FINDNEXT)
        self.Bind(wx.EVT_MENU, self.OnToggleTools, id=ID_SHOWTOOLS)

        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_NEW)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_OPEN)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_REVERT)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_CLOSE)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_SAVE)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_SAVEAS)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_NAMESPACE)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_PRINT)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_UNDO)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_REDO)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_CUT)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_COPY)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_COPY_PLUS)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_PASTE)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_PASTE_PLUS)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_CLEAR)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_SELECTALL)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_EMPTYBUFFER)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_AUTOCOMP_SHOW)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_AUTOCOMP_MAGIC)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_AUTOCOMP_SINGLE)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_AUTOCOMP_DOUBLE)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_CALLTIPS_SHOW)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_CALLTIPS_INSERT)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_WRAP)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_USEAA)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_SHOW_LINENUMBERS)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_AUTO_SAVESETTINGS)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_SAVESETTINGS)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_DELSETTINGSFILE)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_EXECSTARTUPSCRIPT)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_SAVEHISTORY)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_SAVEHISTORYNOW)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_CLEARHISTORY)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_EDITSTARTUPSCRIPT)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_FIND)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_FINDNEXT)
        self.Bind(wx.EVT_UPDATE_UI, self.OnUpdateMenu, id=ID_SHOWTOOLS)
        
        self.Bind(wx.EVT_ACTIVATE, self.OnActivate)
        self.Bind(wx.EVT_FIND, self.OnFindNext)
        self.Bind(wx.EVT_FIND_NEXT, self.OnFindNext)
        self.Bind(wx.EVT_FIND_CLOSE, self.OnFindClose)
        
    

    def OnShowLineNumbers(self, event):
        win = wx.Window.FindFocus()
        if hasattr(win, 'lineNumbers'):
            win.lineNumbers = event.IsChecked()
            win.setDisplayLineNumbers(win.lineNumbers)

    def OnToggleMaximize(self, event):
        self.Maximize(not self.IsMaximized())

    def OnFileNew(self, event):
        self.bufferNew()

    def OnFileOpen(self, event):
        self.bufferOpen()

    def OnFileRevert(self, event):
        self.bufferRevert()

    def OnFileClose(self, event):
        self.bufferClose()

    def OnFileSave(self, event):
        self.bufferSave()

    def OnFileSaveAs(self, event):
        self.bufferSaveAs()

    def OnFileUpdateNamespace(self, event):
        self.updateNamespace()

    def OnFilePrint(self, event):
        self.bufferPrint()

    def OnExit(self, event):
        self.Close(False)

    def OnUndo(self, event):
        win = wx.Window.FindFocus()
        win.Undo()

    def OnRedo(self, event):
        win = wx.Window.FindFocus()
        win.Redo()

    def OnCut(self, event):
        win = wx.Window.FindFocus()
        win.Cut()

    def OnCopy(self, event):
        win = wx.Window.FindFocus()
        win.Copy()

    def OnCopyPlus(self, event):
        win = wx.Window.FindFocus()
        win.CopyWithPrompts()

    def OnPaste(self, event):
        win = wx.Window.FindFocus()
        win.Paste()

    def OnPastePlus(self, event):
        win = wx.Window.FindFocus()
        win.PasteAndRun()

    def OnClear(self, event):
        win = wx.Window.FindFocus()
        win.Clear()
    
    def OnEmptyBuffer(self, event):
        win = wx.Window.FindFocus()
        d = wx.MessageDialog(self,
                             "Are you sure you want to clear the edit buffer,\n"
                             "deleting all the text?",
                             "Empty Buffer", wx.OK | wx.CANCEL | wx.ICON_QUESTION)
        answer = d.ShowModal()
        d.Destroy()
        if (answer == wx.ID_OK):
            win.ClearAll()
            if hasattr(win,'prompt'):
                win.prompt()

    def OnSelectAll(self, event):
        win = wx.Window.FindFocus()
        win.SelectAll()

    def OnAbout(self, event):
        """Display an About window."""
        title = 'About'
        text = 'Your message here.'
        dialog = wx.MessageDialog(self, text, title,
                                  wx.OK | wx.ICON_INFORMATION)
        dialog.ShowModal()
        dialog.Destroy()

    def OnHelp(self, event):
        """Display a Help window."""
        title = 'Help'        
        text = "Type 'shell.help()' in the shell window."
        dialog = wx.MessageDialog(self, text, title,
                                  wx.OK | wx.ICON_INFORMATION)
        dialog.ShowModal()
        dialog.Destroy()

    def OnAutoCompleteShow(self, event):
        win = wx.Window.FindFocus()
        win.autoComplete = event.IsChecked()

    def OnAutoCompleteMagic(self, event):
        win = wx.Window.FindFocus()
        win.autoCompleteIncludeMagic = event.IsChecked()

    def OnAutoCompleteSingle(self, event):
        win = wx.Window.FindFocus()
        win.autoCompleteIncludeSingle = event.IsChecked()

    def OnAutoCompleteDouble(self, event):
        win = wx.Window.FindFocus()
        win.autoCompleteIncludeDouble = event.IsChecked()

    def OnCallTipsShow(self, event):
        win = wx.Window.FindFocus()
        win.autoCallTip = event.IsChecked()

    def OnCallTipsInsert(self, event):
        win = wx.Window.FindFocus()
        win.callTipInsert = event.IsChecked()

    def OnWrap(self, event):
        win = wx.Window.FindFocus()
        win.SetWrapMode(event.IsChecked())
        wx.FutureCall(1, self.shell.EnsureCaretVisible)

    def OnUseAA(self, event):
        win = wx.Window.FindFocus()
        win.SetUseAntiAliasing(event.IsChecked())

    def OnSaveHistory(self, event):
        self.autoSaveHistory = event.IsChecked()

    def OnSaveHistoryNow(self, event):
        self.SaveHistory()

    def OnClearHistory(self, event):
        self.shell.clearHistory()

    def OnAutoSaveSettings(self, event):
        self.autoSaveSettings = event.IsChecked()

    def OnSaveSettings(self, event):
        self.DoSaveSettings()

    def OnDelSettingsFile(self, event):
        if self.config is not None:
            d = wx.MessageDialog(
                self, "Do you want to revert to the default settings?\n" + 
                "A restart is needed for the change to take effect",
                "Warning", wx.OK | wx.CANCEL | wx.ICON_QUESTION)
            answer = d.ShowModal()
            d.Destroy()
            if (answer == wx.ID_OK):
                self.config.DeleteAll()
                self.LoadSettings()


    def OnEditStartupScript(self, event):
        if hasattr(self, 'EditStartupScript'):
            self.EditStartupScript()
            
    def OnExecStartupScript(self, event):
        self.execStartupScript = event.IsChecked()


    def OnFindText(self, event):
        if self.findDlg is not None:
            return
        win = wx.Window.FindFocus()
        self.findDlg = wx.FindReplaceDialog(win, self.findData, "Find",
                                            wx.FR_NOWHOLEWORD)
        self.findDlg.Show()
        
    def OnFindNext(self, event):
        if not self.findData.GetFindString():
            self.OnFindText(event)
            return
        if isinstance(event, wx.FindDialogEvent):
            win = self.findDlg.GetParent()
        else:
            win = wx.Window.FindFocus()
        win.DoFindNext(self.findData, self.findDlg)
        if self.findDlg is not None:
            self.OnFindClose(None)

    def OnFindClose(self, event):
        self.findDlg.Destroy()
        self.findDlg = None
    
    def OnToggleTools(self, event):
        self.ToggleTools()
        

    def OnUpdateMenu(self, event):
        """Update menu items based on current status and context."""
        win = wx.Window.FindFocus()
        id = event.GetId()
        event.Enable(True)
        try:
            if id == ID_NEW:
                event.Enable(hasattr(self, 'bufferNew'))
            elif id == ID_OPEN:
                event.Enable(hasattr(self, 'bufferOpen'))
            elif id == ID_REVERT:
                event.Enable(hasattr(self, 'bufferRevert')
                             and self.hasBuffer())
            elif id == ID_CLOSE:
                event.Enable(hasattr(self, 'bufferClose')
                             and self.hasBuffer())
            elif id == ID_SAVE:
                event.Enable(hasattr(self, 'bufferSave')
                             and self.bufferHasChanged())
            elif id == ID_SAVEAS:
                event.Enable(hasattr(self, 'bufferSaveAs')
                             and self.hasBuffer())
            elif id == ID_NAMESPACE:
                event.Enable(hasattr(self, 'updateNamespace')
                             and self.hasBuffer())
            elif id == ID_PRINT:
                event.Enable(hasattr(self, 'bufferPrint')
                             and self.hasBuffer())
            elif id == ID_UNDO:
                event.Enable(win.CanUndo())
            elif id == ID_REDO:
                event.Enable(win.CanRedo())
            elif id == ID_CUT:
                event.Enable(win.CanCut())
            elif id == ID_COPY:
                event.Enable(win.CanCopy())
            elif id == ID_COPY_PLUS:
                event.Enable(win.CanCopy() and hasattr(win, 'CopyWithPrompts'))
            elif id == ID_PASTE:
                event.Enable(win.CanPaste())
            elif id == ID_PASTE_PLUS:
                event.Enable(win.CanPaste() and hasattr(win, 'PasteAndRun'))
            elif id == ID_CLEAR:
                event.Enable(win.CanCut())
            elif id == ID_SELECTALL:
                event.Enable(hasattr(win, 'SelectAll'))
            elif id == ID_EMPTYBUFFER:
                event.Enable(hasattr(win, 'ClearAll') and not win.GetReadOnly())
            elif id == ID_AUTOCOMP_SHOW:
                event.Check(win.autoComplete)
            elif id == ID_AUTOCOMP_MAGIC:
                event.Check(win.autoCompleteIncludeMagic)
            elif id == ID_AUTOCOMP_SINGLE:
                event.Check(win.autoCompleteIncludeSingle)
            elif id == ID_AUTOCOMP_DOUBLE:
                event.Check(win.autoCompleteIncludeDouble)
            elif id == ID_CALLTIPS_SHOW:
                event.Check(win.autoCallTip)
            elif id == ID_CALLTIPS_INSERT:
                event.Check(win.callTipInsert)
            elif id == ID_WRAP:
                event.Check(win.GetWrapMode())
            elif id == ID_USEAA:
                event.Check(win.GetUseAntiAliasing())

            elif id == ID_SHOW_LINENUMBERS:
                event.Check(win.lineNumbers)
            elif id == ID_AUTO_SAVESETTINGS:
                event.Check(self.autoSaveSettings)
                event.Enable(self.config is not None)
            elif id == ID_SAVESETTINGS:
                event.Enable(self.config is not None and
                             hasattr(self, 'DoSaveSettings'))
            elif id == ID_DELSETTINGSFILE:
                event.Enable(self.config is not None)
                
            elif id == ID_EXECSTARTUPSCRIPT:
                event.Check(self.execStartupScript)
                event.Enable(self.config is not None)

            elif id == ID_SAVEHISTORY:
                event.Check(self.autoSaveHistory)
                event.Enable(self.dataDir is not None)
            elif id == ID_SAVEHISTORYNOW:
                event.Enable(self.dataDir is not None and
                             hasattr(self, 'SaveHistory'))
            elif id == ID_CLEARHISTORY:
                event.Enable(self.dataDir is not None)
                
            elif id == ID_EDITSTARTUPSCRIPT:
                event.Enable(hasattr(self, 'EditStartupScript'))
                event.Enable(self.dataDir is not None)
                
            elif id == ID_FIND:
                event.Enable(hasattr(win, 'DoFindNext'))
            elif id == ID_FINDNEXT:
                event.Enable(hasattr(win, 'DoFindNext'))

            elif id == ID_SHOWTOOLS:
                event.Check(self.ToolsShown())
                                             
            else:
                event.Enable(False)
        except AttributeError:
            # This menu option is not supported in the current context.
            event.Enable(False)


    def OnActivate(self, event):
        """
        Event Handler for losing the focus of the Frame. Should close
        Autocomplete listbox, if shown.
        """
        if not event.GetActive():
            # If autocomplete active, cancel it.  Otherwise, the
            # autocomplete list will stay visible on top of the
            # z-order after switching to another application
            win = wx.Window.FindFocus()
            if hasattr(win, 'AutoCompActive') and win.AutoCompActive():
                win.AutoCompCancel()
        event.Skip()



    def LoadSettings(self, config):
        """Called by derived classes to load settings specific to the Frame"""
        pos  = wx.Point(config.ReadInt('Window/PosX', -1),
                        config.ReadInt('Window/PosY', -1))
                        
        size = wx.Size(config.ReadInt('Window/Width', -1),
                       config.ReadInt('Window/Height', -1))

        self.SetSize(size)
        self.Move(pos)


    def SaveSettings(self, config):
        """Called by derived classes to save Frame settings to a wx.Config object"""

        # TODO: track position/size so we can save it even if the
        # frame is maximized or iconized.
        if not self.iconized and not self.IsMaximized():
            w, h = self.GetSize()
            config.WriteInt('Window/Width', w)
            config.WriteInt('Window/Height', h)
            
            px, py = self.GetPosition()
            config.WriteInt('Window/PosX', px)
            config.WriteInt('Window/PosY', py)




class ShellFrameMixin:
    """
    A mix-in class for frames that will have a Shell or a Crust window
    and that want to add history, startupScript and other common
    functionality.
    """
    def __init__(self, config, dataDir):
        self.config = config
        self.dataDir = dataDir
        self.startupScript = os.environ.get('PYTHONSTARTUP')
        if not self.startupScript and self.dataDir:
            self.startupScript = os.path.join(self.dataDir, 'startup')
            
        self.autoSaveSettings = False
        self.autoSaveHistory = False

        # We need this one before we have a chance to load the settings...
        self.execStartupScript = True
        if self.config:
            self.execStartupScript = self.config.ReadBool('Options/ExecStartupScript', True)
            

    def OnHelp(self, event):
        """Display a Help window."""
        import  wx.lib.dialogs
        title = 'Help on key bindings'
        
        text = wx.py.shell.HELP_TEXT

        dlg = wx.lib.dialogs.ScrolledMessageDialog(self, text, title, size = ((700, 540)))
        fnt = wx.Font(10, wx.TELETYPE, wx.NORMAL, wx.NORMAL)
        dlg.GetChildren()[0].SetFont(fnt)
        dlg.GetChildren()[0].SetInsertionPoint(0)
        dlg.ShowModal()
        dlg.Destroy()


    def LoadSettings(self):
        if self.config is not None:
            self.autoSaveSettings = self.config.ReadBool('Options/AutoSaveSettings', False)
            self.execStartupScript = self.config.ReadBool('Options/ExecStartupScript', True)
            self.autoSaveHistory  = self.config.ReadBool('Options/AutoSaveHistory', False)
            self.LoadHistory()


    def SaveSettings(self):
        if self.config is not None:
            # always save this one
            self.config.WriteBool('Options/AutoSaveSettings', self.autoSaveSettings)
            if self.autoSaveSettings:
                self.config.WriteBool('Options/AutoSaveHistory', self.autoSaveHistory)
                self.config.WriteBool('Options/ExecStartupScript', self.execStartupScript)
            if self.autoSaveHistory:
                self.SaveHistory()



    def SaveHistory(self):
        if self.dataDir:
            try:
                name = os.path.join(self.dataDir, 'history')
                f = file(name, 'w')
                hist = '\x00\n'.join(self.shell.history)
                f.write(hist)
                f.close()
            except:
                d = wx.MessageDialog(self, "Error saving history file.",
                                     "Error", wx.ICON_EXCLAMATION)
                d.ShowModal()
                d.Destroy()
                raise

    def LoadHistory(self):
        if self.dataDir:
            name = os.path.join(self.dataDir, 'history')
            if os.path.exists(name):
                try:
                    f = file(name, 'U')
                    hist = f.read()
                    f.close()
                    self.shell.history = hist.split('\x00\n')
                    dispatcher.send(signal="Shell.loadHistory", history=self.shell.history)
                except:
                    d = wx.MessageDialog(self, "Error loading history file.",
                                         "Error", wx.ICON_EXCLAMATION)
                    d.ShowModal()
                    d.Destroy()


    def bufferHasChanged(self):
        # the shell buffers can always be saved
        return True

    def bufferSave(self):
        import time
        appname = wx.GetApp().GetAppName()
        default = appname + '-' + time.strftime("%Y%m%d-%H%M.py")
        fileName = wx.FileSelector("Save File As", "Saving",
                                  default_filename=default,
                                  default_extension="py",
                                  wildcard="*.py",
                                  flags = wx.SAVE | wx.OVERWRITE_PROMPT)
        if not fileName:
            return
        
        text = self.shell.GetText()

## This isn't working currently...
##         d = wx.MessageDialog(self,u'Save source code only?\nAnswering yes will only save lines starting with >>> and ...',u'Question', wx.YES_NO | wx.ICON_QUESTION)
##         yes_no = d.ShowModal()
##         if yes_no == wx.ID_YES:
##             m = re.findall('^[>\.]{3,3} (.*)\r', text, re.MULTILINE | re.LOCALE)
##             text = '\n'.join(m)
##         d.Destroy()

        try:
            f = open(fileName, "w")
            f.write(text)
            f.close()
        except:
            d = wx.MessageDialog(self, u'Error saving session',u'Error',
                                 wx.OK | wx.ICON_ERROR)
            d.ShowModal()
            d.Destroy()


    def EditStartupScript(self):
        if os.path.exists(self.startupScript):
            text = file(self.startupScript, 'U').read()
        else:
            text = ''

        dlg = EditStartupScriptDialog(self, self.startupScript, text)
        if dlg.ShowModal() == wx.ID_OK:
            text = dlg.GetText()
            try:
                f = file(self.startupScript, 'w')
                f.write(text)
                f.close()
            except:
                d = wx.MessageDialog(self, "Error saving startup file.",
                                     "Error", wx.ICON_EXCLAMATION)
                d.ShowModal()
                d.Destroy()



class EditStartupScriptDialog(wx.Dialog):
    def __init__(self, parent, fileName, text):
        wx.Dialog.__init__(self, parent, size=(425,350),
                           title="Edit Startup Script",
                           style=wx.DEFAULT_DIALOG_STYLE|wx.RESIZE_BORDER)
        
        pst = wx.StaticText(self, -1, "Path:")
        ptx = wx.TextCtrl(self, -1, fileName, style=wx.TE_READONLY)
        self.editor = editwindow.EditWindow(self)
        self.editor.SetText(text)
        wx.CallAfter(self.editor.SetFocus)
        
        ok = wx.Button(self, wx.ID_OK)
        cancel = wx.Button(self, wx.ID_CANCEL)

        mainSizer = wx.BoxSizer(wx.VERTICAL)

        pthSizer = wx.BoxSizer(wx.HORIZONTAL)
        pthSizer.Add(pst, flag=wx.ALIGN_CENTER_VERTICAL)
        pthSizer.Add((5,5))
        pthSizer.Add(ptx, 1)
        mainSizer.Add(pthSizer, 0, wx.EXPAND|wx.ALL, 10)

        mainSizer.Add(self.editor, 1, wx.EXPAND|wx.LEFT|wx.RIGHT, 10)
        
        btnSizer = wx.BoxSizer(wx.HORIZONTAL)
        btnSizer.Add((5,5), 1)
        btnSizer.Add(ok)
        btnSizer.Add((5,5), 1)
        btnSizer.Add(cancel)
        btnSizer.Add((5,5), 1)
        mainSizer.Add(btnSizer, 0, wx.EXPAND|wx.ALL, 10)
        
        self.SetSizer(mainSizer)
        self.Layout()


    def GetText(self):
        return self.editor.GetText()
