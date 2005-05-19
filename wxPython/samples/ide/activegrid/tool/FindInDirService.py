#----------------------------------------------------------------------------
# Name:         IDEFindService.py
# Purpose:      Find Service for pydocview
#
# Author:       Morgan Hua
#
# Created:      8/15/03
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import wx
import wx.lib.docview
import os
from os.path import join
import re
import ProjectEditor
import MessageService
import FindService
import OutlineService
_ = wx.GetTranslation


#----------------------------------------------------------------------------
# Constants
#----------------------------------------------------------------------------
FILENAME_MARKER = _("Found in file: ")
PROJECT_MARKER = _("Searching project: ")
FIND_MATCHDIR = "FindMatchDir"
FIND_MATCHDIRSUBFOLDERS = "FindMatchDirSubfolders"

SPACE = 10
HALF_SPACE = 5


class FindInDirService(FindService.FindService):

    #----------------------------------------------------------------------------
    # Constants
    #----------------------------------------------------------------------------
    FINDALL_ID = wx.NewId()         # for bringing up Find All dialog box
    FINDDIR_ID = wx.NewId()         # for bringing up Find Dir dialog box


    def InstallControls(self, frame, menuBar = None, toolBar = None, statusBar = None, document = None):
        FindService.FindService.InstallControls(self, frame, menuBar, toolBar, statusBar, document)

        editMenu = menuBar.GetMenu(menuBar.FindMenu(_("&Edit")))
        wx.EVT_MENU(frame, FindInDirService.FINDALL_ID, self.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, FindInDirService.FINDALL_ID, self.ProcessUpdateUIEvent)
        editMenu.Append(FindInDirService.FINDALL_ID, _("Find in Project...\tCtrl+Shift+F"), _("Searches for the specified text in all the files in the project"))
        wx.EVT_MENU(frame, FindInDirService.FINDDIR_ID, self.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, FindInDirService.FINDDIR_ID, self.ProcessUpdateUIEvent)
        editMenu.Append(FindInDirService.FINDDIR_ID, _("Find in Directory..."), _("Searches for the specified text in all the files in the directory"))


    def ProcessEvent(self, event):
        id = event.GetId()
        if id == FindInDirService.FINDALL_ID:
            view = wx.GetApp().GetDocumentManager().GetCurrentView()
            if hasattr(view, "GetCtrl") and view.GetCtrl() and hasattr(view.GetCtrl(), "GetSelectedText"):
                self.ShowFindAllDialog(view.GetCtrl().GetSelectedText())
            else:
                self.ShowFindAllDialog()
            return True
        elif id == FindInDirService.FINDDIR_ID:
            view = wx.GetApp().GetDocumentManager().GetCurrentView()
            if hasattr(view, "GetCtrl") and view.GetCtrl() and hasattr(view.GetCtrl(), "GetSelectedText"):
                self.ShowFindDirDialog(view.GetCtrl().GetSelectedText())
            else:
                self.ShowFindDirDialog()
            return True
        else:
            return FindService.FindService.ProcessEvent(self, event)


    def ProcessUpdateUIEvent(self, event):
        id = event.GetId()
        if id == FindInDirService.FINDALL_ID:
            projectService = wx.GetApp().GetService(ProjectEditor.ProjectService)
            view = projectService.GetView()
            if view and view.GetDocument() and view.GetDocument().GetFiles():
                event.Enable(True)
            else:
                event.Enable(False)
            return True
        elif id == FindInDirService.FINDDIR_ID:
            event.Enable(True)
        else:
            return FindService.FindService.ProcessUpdateUIEvent(self, event)


    def ShowFindDirDialog(self, findString=None):
        config = wx.ConfigBase_Get()

        frame = wx.Dialog(None, -1, _("Find in Directory"), size= (320,200))
        borderSizer = wx.BoxSizer(wx.HORIZONTAL)

        contentSizer = wx.BoxSizer(wx.VERTICAL)
        lineSizer = wx.BoxSizer(wx.HORIZONTAL)
        lineSizer.Add(wx.StaticText(frame, -1, _("Directory:")), 0, wx.ALIGN_CENTER | wx.RIGHT, HALF_SPACE)
        dirCtrl = wx.TextCtrl(frame, -1, config.Read(FIND_MATCHDIR, ""), size=(200,-1))
        dirCtrl.SetToolTipString(dirCtrl.GetValue())
        lineSizer.Add(dirCtrl, 0, wx.LEFT, HALF_SPACE)
        findDirButton = wx.Button(frame, -1, _("Browse..."))
        lineSizer.Add(findDirButton, 0, wx.LEFT, HALF_SPACE)
        contentSizer.Add(lineSizer, 0, wx.BOTTOM, SPACE)
        
        def OnBrowseButton(event):
            dlg = wx.DirDialog(frame, _("Choose a directory:"), style=wx.DD_DEFAULT_STYLE)
            dir = dirCtrl.GetValue()
            if len(dir):
                dlg.SetPath(dir)
            if dlg.ShowModal() == wx.ID_OK:
                dirCtrl.SetValue(dlg.GetPath())
                dirCtrl.SetToolTipString(dirCtrl.GetValue())
                dirCtrl.SetInsertionPointEnd()

            dlg.Destroy()
        wx.EVT_BUTTON(findDirButton, -1, OnBrowseButton)

        subfolderCtrl = wx.CheckBox(frame, -1, _("Search in subdirectories"))
        subfolderCtrl.SetValue(config.ReadInt(FIND_MATCHDIRSUBFOLDERS, True))
        contentSizer.Add(subfolderCtrl, 0, wx.BOTTOM, SPACE)

        lineSizer = wx.BoxSizer(wx.VERTICAL)    # let the line expand horizontally without vertical expansion
        lineSizer.Add(wx.StaticLine(frame, -1, size = (10,-1)), 0, flag=wx.EXPAND)
        contentSizer.Add(lineSizer, flag=wx.EXPAND|wx.ALIGN_CENTER_VERTICAL|wx.BOTTOM, border=HALF_SPACE)
        
        lineSizer = wx.BoxSizer(wx.HORIZONTAL)
        lineSizer.Add(wx.StaticText(frame, -1, _("Find what:")), 0, wx.ALIGN_CENTER | wx.RIGHT, HALF_SPACE)
        if not findString:
            findString = config.Read(FindService.FIND_MATCHPATTERN, "")
        findCtrl = wx.TextCtrl(frame, -1, findString, size=(200,-1))
        findCtrl.SetFocus()
        findCtrl.SetSelection(0,-1)
        lineSizer.Add(findCtrl, 0, wx.LEFT, HALF_SPACE)
        contentSizer.Add(lineSizer, 0, wx.BOTTOM, SPACE)
        wholeWordCtrl = wx.CheckBox(frame, -1, _("Match whole word only"))
        wholeWordCtrl.SetValue(config.ReadInt(FindService.FIND_MATCHWHOLEWORD, False))
        matchCaseCtrl = wx.CheckBox(frame, -1, _("Match case"))
        matchCaseCtrl.SetValue(config.ReadInt(FindService.FIND_MATCHCASE, False))
        regExprCtrl = wx.CheckBox(frame, -1, _("Regular expression"))
        regExprCtrl.SetValue(config.ReadInt(FindService.FIND_MATCHREGEXPR, False))
        contentSizer.Add(wholeWordCtrl, 0, wx.BOTTOM, SPACE)
        contentSizer.Add(matchCaseCtrl, 0, wx.BOTTOM, SPACE)
        contentSizer.Add(regExprCtrl, 0, wx.BOTTOM, SPACE)
        borderSizer.Add(contentSizer, 0, wx.TOP | wx.BOTTOM | wx.LEFT, SPACE)

        buttonSizer = wx.BoxSizer(wx.VERTICAL)
        findBtn = wx.Button(frame, wx.ID_OK, _("Find"))
        findBtn.SetDefault()
        buttonSizer.Add(findBtn, 0, wx.BOTTOM, HALF_SPACE)
        buttonSizer.Add(wx.Button(frame, wx.ID_CANCEL), 0)
        borderSizer.Add(buttonSizer, 0, wx.ALL, SPACE)

        frame.SetSizer(borderSizer)
        frame.Fit()

        status = frame.ShowModal()

        passedCheck = False
        while status == wx.ID_OK and not passedCheck:
            if not os.path.exists(dirCtrl.GetValue()):
                dlg = wx.MessageDialog(frame,
                                       _("'%s' does not exist.") % dirCtrl.GetValue(),
                                       _("Find in Directory"),
                                       wx.OK | wx.ICON_EXCLAMATION
                                       )
                dlg.ShowModal()
                dlg.Destroy()

                status = frame.ShowModal()
            elif len(findCtrl.GetValue()) == 0:
                dlg = wx.MessageDialog(frame,
                                       _("'Find what:' cannot be empty."),
                                       _("Find in Directory"),
                                       wx.OK | wx.ICON_EXCLAMATION
                                       )
                dlg.ShowModal()
                dlg.Destroy()

                status = frame.ShowModal()
            else:
                passedCheck = True
    

        # save user choice state for this and other Find Dialog Boxes
        dirString = dirCtrl.GetValue()
        searchSubfolders = subfolderCtrl.IsChecked()
        self.SaveFindDirConfig(dirString, searchSubfolders)
        
        findString = findCtrl.GetValue()
        matchCase = matchCaseCtrl.IsChecked()
        wholeWord = wholeWordCtrl.IsChecked()
        regExpr = regExprCtrl.IsChecked()
        self.SaveFindConfig(findString, wholeWord, matchCase, regExpr)

            
        if status == wx.ID_OK:            
            frame.Destroy()

            messageService = wx.GetApp().GetService(MessageService.MessageService)
            messageService.ShowWindow()

            view = messageService.GetView()
            if view:
                wx.GetApp().GetTopWindow().SetCursor(wx.StockCursor(wx.CURSOR_WAIT))
                view.ClearLines()
                view.SetCallback(self.OnJumpToFoundLine)

                view.AddLines(_("Searching for '%s' in '%s'\n\n") % (findString, dirString))
                
                if os.path.isfile(dirString):
                    try:
                        docFile = file(dirString, 'r')
                        lineNum = 1
                        needToDisplayFilename = True
                        line = docFile.readline()
                        while line:
                            count, foundStart, foundEnd, newText = self.DoFind(findString, None, line, 0, 0, True, matchCase, wholeWord, regExpr)
                            if count != -1:
                                if needToDisplayFilename:
                                    view.AddLines(FILENAME_MARKER + dirString + "\n")
                                    needToDisplayFilename = False
                                line = repr(lineNum).zfill(4) + ":" + line
                                view.AddLines(line)
                            line = docFile.readline()
                            lineNum += 1
                        if not needToDisplayFilename:
                            view.AddLines("\n")
                    except IOError, (code, message):
                        print _("Warning, unable to read file: '%s'.  %s") % (dirString, message)
                else:
                    # do search in files on disk
                    for root, dirs, files in os.walk(dirString):
                        if not searchSubfolders and root != dirString:
                            break
                            
                        for name in files:
                            filename = os.path.join(root, name)
                            try:
                                docFile = file(filename, 'r')
                            except IOError, (code, message):
                                print _("Warning, unable to read file: '%s'.  %s") % (filename, message)
                                continue
        
                            lineNum = 1
                            needToDisplayFilename = True
                            line = docFile.readline()
                            while line:
                                count, foundStart, foundEnd, newText = self.DoFind(findString, None, line, 0, 0, True, matchCase, wholeWord, regExpr)
                                if count != -1:
                                    if needToDisplayFilename:
                                        view.AddLines(FILENAME_MARKER + filename + "\n")
                                        needToDisplayFilename = False
                                    line = repr(lineNum).zfill(4) + ":" + line
                                    view.AddLines(line)
                                line = docFile.readline()
                                lineNum += 1
                            if not needToDisplayFilename:
                                view.AddLines("\n")

                view.AddLines(_("Search completed."))
                wx.GetApp().GetTopWindow().SetCursor(wx.StockCursor(wx.CURSOR_DEFAULT))

            return True
        else:
            frame.Destroy()
            return False
            

    def SaveFindDirConfig(self, dirString, searchSubfolders):
        """ Save search dir patterns and flags to registry.
        
            dirString = search directory
            searchSubfolders = Search subfolders
        """
        config = wx.ConfigBase_Get()
        config.Write(FIND_MATCHDIR, dirString)
        config.WriteInt(FIND_MATCHDIRSUBFOLDERS, searchSubfolders)
    

    def ShowFindAllDialog(self, findString=None):
        config = wx.ConfigBase_Get()

        frame = wx.Dialog(None, -1, _("Find in Project"), size= (320,200))
        borderSizer = wx.BoxSizer(wx.HORIZONTAL)

        contentSizer = wx.BoxSizer(wx.VERTICAL)
        lineSizer = wx.BoxSizer(wx.HORIZONTAL)
        lineSizer.Add(wx.StaticText(frame, -1, _("Find what:")), 0, wx.ALIGN_CENTER | wx.RIGHT, HALF_SPACE)
        if not findString:
            findString = config.Read(FindService.FIND_MATCHPATTERN, "")
        findCtrl = wx.TextCtrl(frame, -1, findString, size=(200,-1))
        lineSizer.Add(findCtrl, 0, wx.LEFT, HALF_SPACE)
        contentSizer.Add(lineSizer, 0, wx.BOTTOM, SPACE)
        wholeWordCtrl = wx.CheckBox(frame, -1, _("Match whole word only"))
        wholeWordCtrl.SetValue(config.ReadInt(FindService.FIND_MATCHWHOLEWORD, False))
        matchCaseCtrl = wx.CheckBox(frame, -1, _("Match case"))
        matchCaseCtrl.SetValue(config.ReadInt(FindService.FIND_MATCHCASE, False))
        regExprCtrl = wx.CheckBox(frame, -1, _("Regular expression"))
        regExprCtrl.SetValue(config.ReadInt(FindService.FIND_MATCHREGEXPR, False))
        contentSizer.Add(wholeWordCtrl, 0, wx.BOTTOM, SPACE)
        contentSizer.Add(matchCaseCtrl, 0, wx.BOTTOM, SPACE)
        contentSizer.Add(regExprCtrl, 0, wx.BOTTOM, SPACE)
        borderSizer.Add(contentSizer, 0, wx.TOP | wx.BOTTOM | wx.LEFT, SPACE)

        buttonSizer = wx.BoxSizer(wx.VERTICAL)
        findBtn = wx.Button(frame, wx.ID_OK, _("Find"))
        findBtn.SetDefault()
        buttonSizer.Add(findBtn, 0, wx.BOTTOM, HALF_SPACE)
        buttonSizer.Add(wx.Button(frame, wx.ID_CANCEL), 0)
        borderSizer.Add(buttonSizer, 0, wx.ALL, SPACE)

        frame.SetSizer(borderSizer)
        frame.Fit()

        status = frame.ShowModal()

        # save user choice state for this and other Find Dialog Boxes
        findString = findCtrl.GetValue()
        matchCase = matchCaseCtrl.IsChecked()
        wholeWord = wholeWordCtrl.IsChecked()
        regExpr = regExprCtrl.IsChecked()
        self.SaveFindConfig(findString, wholeWord, matchCase, regExpr)

        if status == wx.ID_OK:
            frame.Destroy()

            messageService = wx.GetApp().GetService(MessageService.MessageService)
            messageService.ShowWindow()

            view = messageService.GetView()
            if view:
                view.ClearLines()
                view.SetCallback(self.OnJumpToFoundLine)

                projectService = wx.GetApp().GetService(ProjectEditor.ProjectService)
                projectFilenames = projectService.GetFilesFromCurrentProject()

                projView = projectService.GetView()
                if projView:
                    projName = wx.lib.docview.FileNameFromPath(projView.GetDocument().GetFilename())
                    view.AddLines(PROJECT_MARKER + projName + "\n\n")

                # do search in open files first, open files may have been modified and different from disk because it hasn't been saved
                openDocs = wx.GetApp().GetDocumentManager().GetDocuments()
                openDocsInProject = filter(lambda openDoc: openDoc.GetFilename() in projectFilenames, openDocs)
                for openDoc in openDocsInProject:
                    if isinstance(openDoc, ProjectEditor.ProjectDocument):  # don't search project model
                        continue

                    openDocView = openDoc.GetFirstView()
                    # some views don't have a in memory text object to search through such as the PM and the DM
                    # even if they do have a non-text searchable object, how do we display it in the message window?
                    if not hasattr(openDocView, "GetValue"):
                        continue
                    text = openDocView.GetValue()

                    lineNum = 1
                    needToDisplayFilename = True
                    start = 0
                    end = 0
                    count = 0
                    while count != -1:
                        count, foundStart, foundEnd, newText = self.DoFind(findString, None, text, start, end, True, matchCase, wholeWord, regExpr)
                        if count != -1:
                            if needToDisplayFilename:
                                view.AddLines(FILENAME_MARKER + openDoc.GetFilename() + "\n")
                                needToDisplayFilename = False

                            lineNum = openDocView.LineFromPosition(foundStart)
                            line = repr(lineNum).zfill(4) + ":" + openDocView.GetLine(lineNum)
                            view.AddLines(line)

                            start = text.find("\n", foundStart)
                            if start == -1:
                                break
                            end = start
                    if not needToDisplayFilename:
                        view.AddLines("\n")
                openDocNames = map(lambda openDoc: openDoc.GetFilename(), openDocs)

                # do search in closed files, skipping the open ones we already searched
                filenames = filter(lambda filename: filename not in openDocNames, projectFilenames)
                for filename in filenames:
                    try:
                        docFile = file(filename, 'r')
                    except IOError, (code, message):
                        print _("Warning, unable to read file: '%s'.  %s") % (filename, message)
                        continue

                    lineNum = 1
                    needToDisplayFilename = True
                    line = docFile.readline()
                    while line:
                        count, foundStart, foundEnd, newText = self.DoFind(findString, None, line, 0, 0, True, matchCase, wholeWord, regExpr)
                        if count != -1:
                            if needToDisplayFilename:
                                view.AddLines(FILENAME_MARKER + filename + "\n")
                                needToDisplayFilename = False
                            line = repr(lineNum).zfill(4) + ":" + line
                            view.AddLines(line)
                        line = docFile.readline()
                        lineNum += 1
                    if not needToDisplayFilename:
                        view.AddLines("\n")

                view.AddLines(_("Search for '%s' completed.") % findString)

            return True
        else:
            frame.Destroy()
            return False


    def OnJumpToFoundLine(self, event):
        messageService = wx.GetApp().GetService(MessageService.MessageService)
        lineText, pos = messageService.GetView().GetCurrLine()
        if lineText == "\n" or lineText.find(FILENAME_MARKER) != -1 or lineText.find(PROJECT_MARKER) != -1:
            return
        lineEnd = lineText.find(":")
        if lineEnd == -1:
            return
        else:
            lineNum = int(lineText[0:lineEnd])

        text = messageService.GetView().GetText()
        curPos = messageService.GetView().GetCurrentPos()

        startPos = text.rfind(FILENAME_MARKER, 0, curPos)
        endPos = text.find("\n", startPos)
        filename = text[startPos + len(FILENAME_MARKER):endPos]

        foundView = None
        openDocs = wx.GetApp().GetDocumentManager().GetDocuments()
        for openDoc in openDocs:
            if openDoc.GetFilename() == filename:
                foundView = openDoc.GetFirstView()
                break

        if not foundView:
            doc = wx.GetApp().GetDocumentManager().CreateDocument(filename, wx.lib.docview.DOC_SILENT)
            foundView = doc.GetFirstView()

        if foundView:
            foundView.GetFrame().SetFocus()
            foundView.Activate()
            if hasattr(foundView, "GotoLine"):
                foundView.GotoLine(lineNum)
                startPos = foundView.PositionFromLine(lineNum)
                # wxBug:  Need to select in reverse order, (end, start) to put cursor at head of line so positioning is correct
                #         Also, if we use the correct positioning order (start, end), somehow, when we open a edit window for the first
                #         time, we don't see the selection, it is scrolled off screen
                foundView.SetSelection(startPos - 1 + len(lineText[lineEnd:].rstrip("\n")), startPos)
                wx.GetApp().GetService(OutlineService.OutlineService).LoadOutline(foundView, position=startPos)


