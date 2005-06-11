#----------------------------------------------------------------------------
# Name:         ExtensionService.py
# Purpose:      Extension Service for IDE
#
# Author:       Peter Yared
#
# Created:      5/23/05
# CVS-ID:       $ID:$
# Copyright:    (c) 2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import wx
import wx.lib.pydocview
import MessageService
import os
import os.path
import pickle

_ = wx.GetTranslation


SPACE = 10
HALF_SPACE = 5


EXTENSIONS_CONFIG_STRING = "Extensions"



# TODO: Redo extensions menu on OK, or provide alert that it won't happen until restart


#----------------------------------------------------------------------------
# Classes
#----------------------------------------------------------------------------

class Extension:
    

    def __init__(self, menuItemName):
        self.menuItemName = menuItemName
        self.id = 0
        self.menuItemDesc = ''
        self.command = ''
        self.commandPreArgs = ''
        self.commandPostArgs = ''
        self.fileExt = None


class ExtensionService(wx.lib.pydocview.DocService):


    def __init__(self):
        self.LoadExtensions()


    def LoadExtensions(self):
        config = wx.ConfigBase_Get()
        pickledExtensions = config.Read(EXTENSIONS_CONFIG_STRING)
        if pickledExtensions:
            try:
                self._extensions = pickle.loads(pickledExtensions.encode('ascii'))
            except:
                tp, val, tb = sys.exc_info()
                traceback.print_exception(tp,val,tb)
                self._extensions = []
        else:
            self._extensions = []
        

    def SaveExtensions(self):
        config = wx.ConfigBase_Get()
        config.Write(EXTENSIONS_CONFIG_STRING, pickle.dumps(self._extensions))


    def GetExtensions(self):
        return self._extensions


    def SetExtensions(self, extensions):
        self._extensions = extensions


    def InstallControls(self, frame, menuBar = None, toolBar = None, statusBar = None, document = None):
        toolsMenuIndex = menuBar.FindMenu(_("&Tools"))
        if toolsMenuIndex > -1:
            toolsMenu = menuBar.GetMenu(toolsMenuIndex)
        else:
            toolsMenu = wx.Menu()
        
        if self._extensions:
            if toolsMenu.GetMenuItems():
                toolsMenu.AppendSeparator()            
            for ext in self._extensions:
                # Append a tool menu item for each extension
                ext.id = wx.NewId()
                toolsMenu.Append(ext.id, ext.menuItemName)
                wx.EVT_MENU(frame, ext.id, frame.ProcessEvent)
                wx.EVT_UPDATE_UI(frame, ext.id, frame.ProcessUpdateUIEvent)

        if toolsMenuIndex == -1:
            formatMenuIndex = menuBar.FindMenu(_("&Format"))
            menuBar.Insert(formatMenuIndex + 1, toolsMenu, _("&Tools"))

    def ProcessEvent(self, event):
        id = event.GetId()
        for extension in self._extensions:
            if id == extension.id:
                self.OnExecuteExtension(extension)
                return True
        return False


    def ProcessUpdateUIEvent(self, event):
        id = event.GetId()
        for extension in self._extensions:
            if id == extension.id:
                if extension.fileExt:
                    doc = wx.GetApp().GetDocumentManager().GetCurrentDocument()
                    if doc and '*' in extension.fileExt:
                        event.Enable(True)
                        return True
                    if doc:
                        for fileExt in extension.fileExt:
                            if fileExt in doc.GetDocumentTemplate().GetFileFilter():
                                event.Enable(True)
                                return True
                    event.Enable(False)
                    return False
        return False


    def OnExecuteExtension(self, extension):
        if extension.fileExt:
            doc = wx.GetApp().GetDocumentManager().GetCurrentDocument()
            if not doc:
                return
            filename = doc.GetFilename()
            ext = os.path.splitext(filename)[1]
            if not '*' in extension.fileExt:
                if not ext or ext[1:] not in extension.fileExt:
                    return
            cmds = [extension.command]
            if extension.commandPreArgs:
                cmds.append(extension.commandPreArgs)
            cmds.append(filename)
            if extension.commandPostArgs:
                cmds.append(extension.commandPostArgs)
            os.spawnv(os.P_NOWAIT, extension.command, cmds)
                      
        else:
            cmd = extension.command
            if extension.commandPreArgs:
                cmd = cmd + ' ' + extension.commandPreArgs
            if extension.commandPostArgs:
                cmd = cmd + ' ' + extension.commandPostArgs
            f = os.popen(cmd)
            messageService = wx.GetApp().GetService(MessageService.MessageService)
            messageService.ShowWindow()
            view = messageService.GetView()
            for line in f.readlines():
                view.AddLines(line)
            view.GetControl().EnsureCaretVisible()
            f.close()
            

class ExtensionOptionsPanel(wx.Panel):


    def __init__(self, parent, id):
        wx.Panel.__init__(self, parent, id)
        
        extOptionsPanelBorderSizer = wx.BoxSizer(wx.HORIZONTAL)
        
        extOptionsPanelSizer = wx.FlexGridSizer(cols=2, hgap=SPACE, vgap=HALF_SPACE)
        
        extCtrlSizer = wx.BoxSizer(wx.VERTICAL)
        extCtrlSizer.Add(wx.StaticText(self, -1, _("Extensions:")), 0)
        self._extListBox = wx.ListBox(self, -1, size=(-1,185), style=wx.LB_SINGLE)
        self.Bind(wx.EVT_LISTBOX, self.OnListBoxSelect, self._extListBox)
        extCtrlSizer.Add(self._extListBox, 1, wx.TOP | wx.BOTTOM | wx.EXPAND, SPACE)        
        buttonSizer = wx.GridSizer(rows=1, hgap=10, vgap=5)
        self._moveUpButton = wx.Button(self, -1, _("Move Up"))
        self.Bind(wx.EVT_BUTTON, self.OnMoveUp, self._moveUpButton)
        buttonSizer.Add(self._moveUpButton, 0)
        self._moveDownButton = wx.Button(self, -1, _("Move Down"))
        self.Bind(wx.EVT_BUTTON, self.OnMoveDown, self._moveDownButton)
        buttonSizer.Add(self._moveDownButton, 0)
        extCtrlSizer.Add(buttonSizer, 0, wx.ALIGN_CENTER | wx.BOTTOM, HALF_SPACE)
        buttonSizer = wx.GridSizer(rows=1, hgap=10, vgap=5)
        self._addButton = wx.Button(self, -1, _("Add"))
        self.Bind(wx.EVT_BUTTON, self.OnAdd, self._addButton)
        buttonSizer.Add(self._addButton, 0)
        self._deleteButton = wx.Button(self, wx.ID_DELETE)
        self.Bind(wx.EVT_BUTTON, self.OnDelete, self._deleteButton)
        buttonSizer.Add(self._deleteButton, 0)
        extCtrlSizer.Add(buttonSizer, 0, wx.ALIGN_CENTER)
        extOptionsPanelSizer.Add(extCtrlSizer, 0)

        self._extDetailPanel = wx.Panel(self)
        staticBox = wx.StaticBox(self._extDetailPanel, label=_("Selected Extension"))
        staticBoxSizer = wx.StaticBoxSizer(staticBox)
        self._extDetailPanel.SetSizer(staticBoxSizer)
        extDetailSizer = wx.FlexGridSizer(cols=1, hgap=5, vgap=3)
        staticBoxSizer.AddSizer(extDetailSizer, 0, wx.ALL, 5)

        extDetailSizer.Add(wx.StaticText(self._extDetailPanel, -1, _("Menu Item Name:")))
        self._menuItemNameTextCtrl = wx.TextCtrl(self._extDetailPanel, -1, size = (-1, -1))
        extDetailSizer.Add(self._menuItemNameTextCtrl, 1, wx.EXPAND)
        self.Bind(wx.EVT_TEXT, self.SaveCurrentItem, self._menuItemNameTextCtrl)        

        extDetailSizer.Add(wx.StaticText(self._extDetailPanel, -1, _("Menu Item Description:")))
        self._menuItemDescTextCtrl = wx.TextCtrl(self._extDetailPanel, -1, size = (-1, -1))
        extDetailSizer.Add(self._menuItemDescTextCtrl, 1, wx.EXPAND)

        extDetailSizer.Add(wx.StaticText(self._extDetailPanel, -1, _("Command Path:")))
        self._commandTextCtrl = wx.TextCtrl(self._extDetailPanel, -1, size = (-1, -1))        
        findFileButton = wx.Button(self._extDetailPanel, -1, _("Browse..."))
        def OnBrowseButton(event):
            fileDlg = wx.FileDialog(self, _("Choose an Executable:"), style=wx.OPEN | wx.HIDE_READONLY)
            path = self._commandTextCtrl.GetValue()
            if path:
                fileDlg.SetPath(path)
            if fileDlg.ShowModal() == wx.ID_OK:
                self._commandTextCtrl.SetValue(fileDlg.GetPath())
                self._commandTextCtrl.SetInsertionPointEnd()
            fileDlg.Destroy()
        wx.EVT_BUTTON(findFileButton, -1, OnBrowseButton)
        hsizer = wx.BoxSizer(wx.HORIZONTAL)
        hsizer.Add(self._commandTextCtrl, 1, wx.EXPAND)
        hsizer.Add(findFileButton, 0, wx.LEFT, HALF_SPACE)
        extDetailSizer.Add(hsizer, 0)

        extDetailSizer.Add(wx.StaticText(self._extDetailPanel, -1, _("Command Pre Arguments:")))
        self._commandPreArgsTextCtrl = wx.TextCtrl(self._extDetailPanel, -1, size = (-1, -1))
        extDetailSizer.Add(self._commandPreArgsTextCtrl, 1, wx.EXPAND)

        extDetailSizer.Add(wx.StaticText(self._extDetailPanel, -1, _("Command Post Arguments:")))
        self._commandPostArgsTextCtrl = wx.TextCtrl(self._extDetailPanel, -1, size = (-1, -1))
        extDetailSizer.Add(self._commandPostArgsTextCtrl, 1, wx.EXPAND)

        extDetailSizer.Add(wx.StaticText(self._extDetailPanel, -1, _("File Extensions (Comma Separated):")))
        self._fileExtTextCtrl = wx.TextCtrl(self._extDetailPanel, -1, size = (-1, -1))
        self._fileExtTextCtrl.SetToolTipString(_("""For example: "txt, text" or "*" for all files"""))
        extDetailSizer.Add(self._fileExtTextCtrl, 1, wx.EXPAND)

        extOptionsPanelSizer.Add(self._extDetailPanel, 0)

        extOptionsPanelBorderSizer.Add(extOptionsPanelSizer, 0, wx.ALL | wx.EXPAND, SPACE)
        self.SetSizer(extOptionsPanelBorderSizer)
        self.Layout()
        parent.AddPage(self, _("Extensions"))
        
        if self.PopulateItems():
            self._extListBox.SetSelection(0)
        self.OnListBoxSelect(None)


    def OnOK(self, optionsDialog):
        self.SaveCurrentItem()
        extensionsService = wx.GetApp().GetService(ExtensionService)
        oldExtensions = extensionsService.GetExtensions()
        extensionsService.SetExtensions(self._extensions)
        extensionsService.SaveExtensions()
        if oldExtensions.__repr__() != self._extensions.__repr__():
            msgTitle = wx.GetApp().GetAppName()
            if not msgTitle:
                msgTitle = _("Document Options")
            wx.MessageBox(_("Extension changes will not appear until the application is restarted."),
                          msgTitle,
                          wx.OK | wx.ICON_INFORMATION,
                          self.GetParent())
        

    def PopulateItems(self):
        extensionsService = wx.GetApp().GetService(ExtensionService)
        import copy
        self._extensions = copy.deepcopy(extensionsService.GetExtensions())
        for extension in self._extensions:
            self._extListBox.Append(extension.menuItemName, extension)
        self._currentItem = None
        self._currentItemIndex = -1
        return len(self._extensions)
      

    def OnListBoxSelect(self, event):
        self.SaveCurrentItem()
        if not self._extListBox.GetSelections():
            self._currentItemIndex = -1
            self._currentItem = None
            self._deleteButton.Enable(False)
            self._moveUpButton.Enable(False)
            self._moveDownButton.Enable(False)
        else:
            self._currentItemIndex = self._extListBox.GetSelection()
            self._currentItem = self._extListBox.GetClientData(self._currentItemIndex)
            self._deleteButton.Enable()
            self._moveUpButton.Enable(self._extListBox.GetCount() > 1 and self._currentItemIndex > 0)
            self._moveDownButton.Enable(self._extListBox.GetCount() > 1 and self._currentItemIndex < self._extListBox.GetCount() - 1)
        self.LoadItem(self._currentItem)


    def SaveCurrentItem(self, event=None):
        extension = self._currentItem
        if extension:
            if extension.menuItemName != self._menuItemNameTextCtrl.GetValue():
                extension.menuItemName = self._menuItemNameTextCtrl.GetValue()
                self._extListBox.SetString(self._currentItemIndex, extension.menuItemName)
            extension.menuItemDesc = self._menuItemDescTextCtrl.GetValue()
            extension.command = self._commandTextCtrl.GetValue()
            extension.commandPreArgs = self._commandPreArgsTextCtrl.GetValue()
            extension.commandPostArgs = self._commandPostArgsTextCtrl.GetValue()
            fileExt = self._fileExtTextCtrl.GetValue().replace(' ','')
            if not fileExt:
                extension.fileExt = None
            else:
                extension.fileExt = fileExt.split(',')
            

    def LoadItem(self, extension):
        if extension:
            self._menuItemDescTextCtrl.SetValue(extension.menuItemDesc or '')
            self._commandTextCtrl.SetValue(extension.command or '')
            self._commandPreArgsTextCtrl.SetValue(extension.commandPreArgs or '')
            self._commandPostArgsTextCtrl.SetValue(extension.commandPostArgs or '')
            if extension.fileExt:
                self._fileExtTextCtrl.SetValue(extension.fileExt.__repr__()[1:-1].replace("'",""))  # Make the list a string, strip the brakcet on either side
            else:
                self._fileExtTextCtrl.SetValue('')
            self._menuItemNameTextCtrl.SetValue(extension.menuItemName or '')  # Do the name last since it triggers the write event that updates the entire item
            self._extDetailPanel.Enable()
        else:
            self._menuItemNameTextCtrl.SetValue('')
            self._menuItemDescTextCtrl.SetValue('')
            self._commandTextCtrl.SetValue('')
            self._commandPreArgsTextCtrl.SetValue('')
            self._commandPostArgsTextCtrl.SetValue('')
            self._fileExtTextCtrl.SetValue('')
            self._extDetailPanel.Enable(False)
            
      
    def OnAdd(self, event):
        self.SaveCurrentItem()
        extensionNames = map(lambda extension: extension.menuItemName, self._extensions)
        name = _("Untitled")
        count = 1
        while name in extensionNames:
            count = count + 1
            name = _("Untitled %s") % count
        extension = Extension(name)
        self._extensions.append(extension)
        self._extListBox.Append(extension.menuItemName, extension)
        self._extListBox.SetSelection(self._extListBox.GetCount() - 1)
        self.OnListBoxSelect(None)
        self._menuItemNameTextCtrl.SetFocus()
        self._menuItemNameTextCtrl.SetSelection(-1, -1)
        

    def OnDelete(self, event):
        self._extListBox.Delete(self._currentItemIndex)
        self._extensions.remove(self._currentItem)    
        self._currentItemIndex = min(self._currentItemIndex, self._extListBox.GetCount() - 1)
        if self._currentItemIndex > -1:
            self._extListBox.SetSelection(self._currentItemIndex)
        self._currentItem = None  # Don't update it since it no longer exists
        self.OnListBoxSelect(None)


    def OnMoveUp(self, event):
        itemAboveString = self._extListBox.GetString(self._currentItemIndex - 1)
        itemAboveData = self._extListBox.GetClientData(self._currentItemIndex - 1)
        self._extListBox.Delete(self._currentItemIndex - 1)
        self._extListBox.Insert(itemAboveString, self._currentItemIndex)
        self._extListBox.SetClientData(self._currentItemIndex, itemAboveData)
        self._currentItemIndex = self._currentItemIndex - 1
        self.OnListBoxSelect(None) # Reset buttons


    def OnMoveDown(self, event):
        itemBelowString = self._extListBox.GetString(self._currentItemIndex + 1)
        itemBelowData = self._extListBox.GetClientData(self._currentItemIndex + 1)
        self._extListBox.Delete(self._currentItemIndex + 1)
        self._extListBox.Insert(itemBelowString, self._currentItemIndex)
        self._extListBox.SetClientData(self._currentItemIndex, itemBelowData)
        self._currentItemIndex = self._currentItemIndex + 1
        self.OnListBoxSelect(None) # Reset buttons
