#!/usr/bin/env python

# simple text editor
#
# Copyright 2001 Adam Feuer and Steve Howell
#
# License: Python

import os, string, re
import sys
from wxPython.wx         import *
from StatusBar           import *
from FrogEditor          import FrogEditor


ABOUT_TEXT = """FrogEdit : Copyright 2001 Adam Feuer and Steve Howell
wxEditor component : Copyright 1999 - 2001 Dirk Holtwic, Robin Dunn, Adam Feuer, Steve Howell

FrogEdit was built using Python, wxPython, and wxWindows."""


##---------------------------------------------------------------------

def chomp(line):
    line = string.split(line,'\n')[0]
    return string.split(line,'\r')[0]

##---------------------------------------------------------------------

class OutlinerPanel(wxPanel):

    def Close(self, event):
        self.parent.Close()
        wxPanel.Close(self)

##----------------------------------------------------------------------


class FrogEditFrame(wxFrame):
    def __init__(self, parent, ID, title, pos=wxDefaultPosition,
                 size=wxDefaultSize, style=wxDEFAULT_FRAME_STYLE):

        wxFrame.__init__(self, parent, ID, title, pos, size, style)

        splitter = wxSplitterWindow(self, -1, style=wxNO_3D|wxSP_3D)
        win = OutlinerPanel(splitter, -1, style=wxCLIP_CHILDREN)
        win.parent = self
        log = self.MakeLogWindow(splitter)

        self.MakeStatusbar()
        self.MakeEditorWindow(win, log)
        self.SetUpSplitter(splitter, win, log)
        self.MakeMenus()
        self.MakeMainWindow(win)
        self.RegisterEventHandlers()
        self.InitVariables()

        win.Layout()


##------------- Init Misc

    def RegisterEventHandlers(self):
        EVT_CLOSE(self,self.OnCloseWindow)

    def InitVariables(self):
        self.fileName = None
        self.edl.UnTouchBuffer()

    def MakeMenus(self):
        self.MainMenu = wxMenuBar()
        self.AddMenus(self.MainMenu)
        self.SetMenuBar(self.MainMenu)

##------------- Init Subwindows

    def MakeEditorWindow(self, win, log):
        self.edl = FrogEditor(win, -1, style=wxSUNKEN_BORDER, statusBar = self.sb)
        self.edl.SetControlFuncs = self.SetControlFuncs
        self.edl.SetAltFuncs = self.SetAltFuncs
        self.edl.SetStatus(log)

    def MakeStatusbar(self):
        self.sb = CustomStatusBar(self)
        self.SetStatusBar(self.sb)

    def MakeLogWindow(self, container):
        log = wxTextCtrl(container, -1,
                         style = wxTE_MULTILINE|wxTE_READONLY|wxHSCROLL)
        wxLog_SetActiveTarget(wxLogTextCtrl(log))
        wxLogMessage('window handle: %s' % self.GetHandle())
        return log

    def SetUpSplitter(self, splitter, win, log):
        splitter.SplitHorizontally(win, log)
        splitter.SetSashPosition(360, True)
        splitter.SetMinimumPaneSize(40)

    def MakeToolbar(self, win):
        toolbarBox = wxBoxSizer(wxHORIZONTAL)
        self.AddButtons(win, toolbarBox)
        return toolbarBox

    def MakeMainWindow(self, win):
        mainBox = wxBoxSizer(wxVERTICAL)
        mainBox.Add(self.MakeToolbar(win))
        borderWidth = 5
        mainBox.Add(self.edl, 1, wxALL|wxGROW, borderWidth)
        win.SetSizer(mainBox)
        win.SetAutoLayout(True)

##-------------- Init Menus

    # override this to add more menus
    def AddMenus(self, menu):
        self.AddFileMenu(menu)
        self.AddEditMenu(menu)
        self.AddHelpMenu(menu)

    def AddMenuItem(self, menu, itemText, itemDescription, itemHandler):
        menuId = wxNewId()
        menu.Append(menuId, itemText, itemDescription)
        EVT_MENU(self, menuId, itemHandler)
        return menuId

    def AddFileMenu(self, menu):
        fileMenu = wxMenu()
        self.AddMenuItem(fileMenu, '&New File\tCtrl-N', 'New File', self.OnNewFile)
        self.AddMenuItem(fileMenu, '&Open File\tCtrl-O', 'Open File', self.OnOpenFile)
        self.AddMenuItem(fileMenu, '&Save File\tCtrl-S', 'Save File', self.OnSaveFile)
        self.AddMenuItem(fileMenu, 'Save File &As\tCtrl-A', 'Save File As',self.OnSaveFileAs)
        self.AddMenuItem(fileMenu, 'E&xit\tAlt-X', 'Exit', self.OnFileExit)
        menu.Append(fileMenu, 'File')

    def AddEditMenu(self, menu):
        editMenu = wxMenu()
        self.AddMenuItem(editMenu, 'Cut\tCtrl-X', 'Cut', self.edl.OnCutSelection)
        self.AddMenuItem(editMenu, '&Copy\tCtrl-C', 'Copy', self.edl.OnCopySelection)
        self.AddMenuItem(editMenu, 'Paste\tCtrl-V', 'Paste', self.edl.OnPaste)
        self.AddMenuItem(editMenu, 'Edit preferences', 'Edit Preferences', self.OnEditPreferences)
        menu.Append(editMenu, 'Edit')

    def AddHelpMenu(self, menu):
        helpMenu = wxMenu()
        self.AddMenuItem(helpMenu, 'About', 'About the program', self.OnHelpAbout)
        menu.Append(helpMenu, 'Help')

##---------------- Init Buttons


    def NewButton(self, window, container, name, pos, size, handler):
        buttonId = wxNewId()
        if pos == None or size == None:
            container.Add(wxButton(window, buttonId, name), 0, 0)
        else:
            container.Add(wxButton(window, buttonId, name, pos, size), 0, 0)
        EVT_BUTTON(self, buttonId, handler)
        return buttonId

    # override this to make more buttons
    def AddButtons(self, window, container):
        buttonPos = None
        buttonSize = None
        self.NewButton(window, container, "New", buttonPos, buttonSize, self.OnNewFile)
        self.NewButton(window, container, "Open", buttonPos, buttonSize, self.OnOpenFile)
        self.NewButton(window, container, "Save", buttonPos, buttonSize, self.OnSaveFile)


##-------------- Init Dialogs

    def MessageDialog(self, text, title):
        messageDialog = wxMessageDialog(self, text, title, wxOK | wxICON_INFORMATION)
        messageDialog.ShowModal()
        messageDialog.Destroy()

    def OkCancelDialog(self, text, title):
        dialog = wxMessageDialog(self, text, title, wxOK | wxCANCEL | wxICON_INFORMATION)
        result = dialog.ShowModal()
        dialog.Destroy()
        if result == wxID_OK:
            return True
        else:
            return False

    def SelectFileDialog(self, defaultDir=None, defaultFile=None, wildCard=None):
        if defaultDir == None:
            defaultDir = "."
        if defaultFile == None:
            defaultFile = ""
        if wildCard == None:
            wildCard = "*.*"
        fileName = None
        fileDialog = wxFileDialog(self, "Choose a file", defaultDir, defaultFile, wildCard, wxOPEN|wxMULTIPLE)
        result = fileDialog.ShowModal()
        if result == wxID_OK:
            fileName = fileDialog.GetPath()
            wxLogMessage('You selected: %s\n' % fileName)
        fileDialog.Destroy()
        return fileName

    def OpenFileError(self, fileName):
        wxLogMessage('Open file error.')
        self.MessageDialog("Error opening file '%s'!" % fileName, "Error")


    def SaveFileError(self, fileName):
        wxLogMessage('Save file error.')
        self.MessageDialog("Error saving file '%s'!" % fileName, "Error")

##---------------- Utility functions


    def SetControlFuncs(self, action):
        "for overriding editor's keys"
        FrogEditor.SetControlFuncs(self.edl, action)
        action['a'] = self.OnSaveFileAs
        action['o'] = self.OnOpenFile
        action['n'] = self.OnNewFile
        action['s'] = self.OnSaveFile

    def SetAltFuncs(self, action):
        FrogEditor.SetAltFuncs(self.edl, action)
        action['x'] = self.OnFileExit

    def GetCurrentDir(self):
        if self.fileName is not None:
            return os.path.split(self.fileName)[0]
        return "."

    def GetFileName(self):
        if self.fileName is not None:
            return os.path.split(self.fileName)[1]
        return ""

    def NewFile(self):
        self.edl.SetText([""])
        self.fileName = None
        self.sb.setFileName("")

    def SaveFile(self, fileName):
        try:
            contents = string.join(self.edl.GetText(), '\n')
            f = open(fileName, 'w')
            f.write(contents)
            f.close()
            self.edl.UnTouchBuffer()
            self.sb.setFileName(fileName)
            return True
        except:
            return False

    def OpenFile(self, fileName):
        try:
            f = open(fileName, 'r')
            contents = f.readlines()
            f.close()
            contents = [chomp(line) for line in contents]
            if len(contents) == 0:
                contents = [""]
            self.edl.SetText(contents)
            self.fileName = fileName
            self.sb.setFileName(fileName)
            return True
        except:
            return False



##---------------- Event handlers


    def OnCloseWindow(self, event):
        self.edl.OnCloseWindow(event)
        self.Destroy()

    def OnNewFile(self, event):
        if self.edl.BufferWasTouched():
            if not self.OkCancelDialog("New file - abandon changes?", "New File"):
                return
        self.NewFile()
        self.edl.SetFocus()

    def OnOpenFile(self, event):
        if self.edl.BufferWasTouched():
            if not self.OkCancelDialog("Open file - abandon changes?", "Open File"):
                return
        fileName = self.SelectFileDialog(self.GetCurrentDir())
        if fileName is not None:
            if self.OpenFile(fileName) is False:
                self.OpenFileError(fileName)
        self.edl.SetFocus()

    def OnSaveFile(self, event):
        if self.fileName is None:
            return self.OnSaveFileAs(event)
        wxLogMessage("Saving %s..." % self.fileName)
        if self.SaveFile(self.fileName) is not True:
            self.SaveFileError(self.fileName)
        self.edl.SetFocus()

    def OnSaveFileAs(self, event):
        fileName = self.SelectFileDialog(self.GetCurrentDir(),self.GetFileName())
        if fileName is not None:
            self.fileName = fileName
            wxLogMessage("Saving %s..." % self.fileName)
            if self.SaveFile(self.fileName) is not True:
                self.SaveFileError(self.fileName)
        self.edl.SetFocus()

    def OnFileExit(self, event):
        if self.edl.BufferWasTouched():
            if not self.OkCancelDialog("Exit program - abandon changes?", "Exit"):
                return
        self.OnCloseWindow(event)

    def OnEditPreferences(self, event):
        self.MessageDialog("Edit preferences is not implemented yet.", "Not implemented.")
        pass

    def OnHelpAbout(self, event):
        self.MessageDialog(ABOUT_TEXT, "About FrogEdit")
        pass

    def Show(self, show):
        wxFrame.Show(self, show)
        self.edl.SetFocus()

##------------- Startup stuff

    def LoadInitialFile(self, fileName):
        if fileName is not None:
            if self.OpenFile(fileName) is False:
                self.OpenFileError(fileName)




##-------------- Application Launcher utility class

class FrogEditLauncher:

    def MakeAppFrame(self):
        return FrogEditFrame(None, -1, "FrogEdit", size=(640, 480),
                             style=wxDEFAULT_FRAME_STYLE|wxNO_FULL_REPAINT_ON_RESIZE)

    def GetArgvFilename(self):
        if len(sys.argv) > 1:
            return sys.argv[1]
        else:
            return None

    def Main(self):
        app = wxPySimpleApp()
        win = self.MakeAppFrame()
        win.Show(True)
        win.LoadInitialFile(self.GetArgvFilename())
        app.MainLoop()


##-------------- Main program


if __name__ == '__main__':

    launcher = FrogEditLauncher()
    launcher.Main()
