#----------------------------------------------------------------------------
# Name:         ProjectEditor.py
# Purpose:      IDE-style Project Editor for wx.lib.pydocview
#
# Author:       Peter Yared, Morgan Hua
#
# Created:      8/15/03
# CVS-ID:       $Id$
# Copyright:    (c) 2003, 2004, 2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import wx.lib.docview
import wx.lib.pydocview
import types
import os
import os.path
import wx
from wxPython.lib.rcsizer import RowColSizer
import time
import Service
import MessageService
import DebuggerService
import sys
import activegrid.util.xmlmarshaller
import UICommon
from IDE import ACTIVEGRID_BASE_IDE
if not ACTIVEGRID_BASE_IDE:
    import ProcessModelEditor

_ = wx.GetTranslation

if wx.Platform == '__WXMSW__':
    _WINDOWS = True
else:
    _WINDOWS = False


#----------------------------------------------------------------------------
# XML Marshalling Methods
#----------------------------------------------------------------------------

def load(fileObject):
    xml = fileObject.read()
    projectModel = activegrid.util.xmlmarshaller.unmarshal(xml)
    return projectModel


def save(fileObject, projectModel):
    xml = activegrid.util.xmlmarshaller.marshal(projectModel, prettyPrint=True)
    fileObject.write(xml)


#----------------------------------------------------------------------------
# Classes
#----------------------------------------------------------------------------

class ProjectModel:
    __xmlname__ = "projectmodel"
    __xmlrename__ = { "_files":"files", "_homepath":"homepath" }

    def __init__(self):
        self._homepath = None
        self._files = []


class ProjectDocument(wx.lib.docview.Document):

    def __init__(self):
        wx.lib.docview.Document.__init__(self)
        self._projectModel = ProjectModel()


    def GetModel(self):
        return self._projectModel


    def OnCreate(self, path, flags):
        projectService = wx.GetApp().GetService(ProjectService)
        if projectService.GetView():
            view = projectService.GetView()
            self.AddView(view)
        else:
            view = self.GetDocumentTemplate().CreateView(self, flags)
            projectService.SetView(view)
        return view


    def LoadObject(self, fileObject):
        self._projectModel = activegrid.tool.ProjectEditor.load(fileObject)
        return True


    def SaveObject(self, fileObject):
        activegrid.tool.ProjectEditor.save(fileObject, self._projectModel)
        return True


    def OnSaveDocument(self, filename):
        self._projectModel._homepath = wx.lib.docview.PathOnly(filename)
        return wx.lib.docview.Document.OnSaveDocument(self, filename)


    def OnOpenDocument(self, filename):
        view = self.GetFirstView()
        frame = view.GetFrame()

        if not os.path.exists(filename):
            wx.GetApp().CloseSplash()
            msgTitle = wx.GetApp().GetAppName()
            if not msgTitle:
                msgTitle = _("File Error")
            wx.MessageBox(_("Could not find '%s'.") % filename,
                          msgTitle,
                          wx.OK | wx.ICON_EXCLAMATION | wx.STAY_ON_TOP,
                          frame)
            return True  # if we return False, the Project View is destroyed, Service windows shouldn't be destroyed

        fileObject = file(filename, 'r')
        try:
            self.LoadObject(fileObject)
        except:
            wx.GetApp().CloseSplash()
            msgTitle = wx.GetApp().GetAppName()
            if not msgTitle:
                msgTitle = _("File Error")
            wx.MessageBox(_("Could not open '%s'.  %s") % (wx.lib.docview.FileNameFromPath(filename), sys.exc_value),
                          msgTitle,
                          wx.OK | wx.ICON_EXCLAMATION | wx.STAY_ON_TOP,
                          frame)
            return True  # if we return False, the Project View is destroyed, Service windows shouldn't be destroyed

        self.Modify(False)

        # if the project file has moved, then ask the user if we should readjust the paths of all the files in the project
        newHomepath = wx.lib.docview.PathOnly(filename)
        if newHomepath != self._projectModel._homepath:
            wx.GetApp().CloseSplash()
            msgTitle = wx.GetApp().GetAppName()
            if not msgTitle:
                msgTitle = _("Project Moved")
            projectService = wx.GetApp().GetService(activegrid.tool.ProjectEditor.ProjectService)
            yesNoMsg = wx.MessageDialog(frame,
                          _("The project file '%s' was moved from:\n    '%s'\nto:\n    '%s'.\n\nWould you like to automatically adjust the project contents accordingly?") % (wx.lib.docview.FileNameFromPath(filename), self._projectModel._homepath, wx.lib.docview.PathOnly(filename)),
                          msgTitle,
                          wx.YES_NO | wx.STAY_ON_TOP
                          )
            if projectService.GetSuppressOpenProjectMessages() or yesNoMsg.ShowModal() == wx.ID_YES:
                if not projectService.GetSuppressOpenProjectMessages():
                    messageService = wx.GetApp().GetService(MessageService.MessageService)
                    messageService.ShowWindow()
                    messageView = messageService.GetView()
                    messageView.ClearLines()
                    messageView.AddLines(_("The project file '%s' was moved from:\n    '%s'\nto:\n    '%s'\n") % (wx.lib.docview.FileNameFromPath(filename), self._projectModel._homepath, wx.lib.docview.PathOnly(filename)))
                    messageView.AddLines(_("Updating file references:\n"))

                for index, filepath in enumerate(self._projectModel._files):
                    if filepath.startswith(self._projectModel._homepath + os.sep):
                        newfile = newHomepath + filepath[len(self._projectModel._homepath):len(filepath)]
                        if os.path.exists(newfile):
                            self._projectModel._files[index] = newfile
                            if not projectService.GetSuppressOpenProjectMessages():
                                messageView.AddLines(_("    Success:    '%s' location changed from '%s' to '%s'\n") % (wx.lib.docview.FileNameFromPath(filepath), wx.lib.docview.PathOnly(filepath), newHomepath))
                            self.Modify(True)
                        else:
                            if not projectService.GetSuppressOpenProjectMessages():
                                messageView.AddLines(_("    Failure:    Couldn't find '%s', file wasn't located at '%s'\n") % (wx.lib.docview.FileNameFromPath(filepath), newHomepath))
                    else:
                        if not projectService.GetSuppressOpenProjectMessages():
                            messageView.AddLines(_(    "    Unmodified: '%s' location wasn't relative to '%s'\n") % (filepath, self._projectModel._homepath))
                self._projectModel._homepath = newHomepath
                if not projectService.GetSuppressOpenProjectMessages():
                    messageView.AddLines(_("Project file updated."))

        self.SetFilename(filename, True)
        view.AddProjectToView(self)
        self.UpdateAllViews()
        self._savedYet = True
        view.Activate(True)
        return True

 
    def AddFile(self, file):
        return self.AddFiles([file])


    def AddFiles(self, files):
        notAlreadyThereFiles = filter(lambda x: x not in self._projectModel._files, files)  # Filter to the files that are not already in the project
        if len(notAlreadyThereFiles) == 0:
            self.UpdateAllViews(hint = ("select", self, files))
            return False
        else:
            self._projectModel._files = self._projectModel._files + notAlreadyThereFiles
            self.UpdateAllViews(hint = ("add", self, notAlreadyThereFiles))
            self.Modify(True)
            return True


    def RemoveFile(self, file):
        return self.RemoveFiles([file])


    def RemoveFiles(self, files):
        for file in files:
            self._projectModel._files.remove(file)
        self.UpdateAllViews(hint = ("remove", self, files))
        self.Modify(True)
        return True


    def RenameFile(self, oldFile, newFile, isProject = False):
        try:
            if oldFile == newFile:
                return False

            # projects don't have to exist yet, so not required to rename old file,
            # but files must exist, so we'll try to rename and allow exceptions to occur if can't.
            if not isProject or (isProject and os.path.exists(oldFile)):
                os.rename(oldFile, newFile)

            if isProject:
                documents = self.GetDocumentManager().GetDocuments()
                for document in documents:
                    if document.GetFilename() == oldFile:  # If the renamed document is open, update it
                        document.SetFilename(newFile)
                        document.SetTitle(wx.lib.docview.FileNameFromPath(newFile))
                        document.UpdateAllViews(hint = ("rename", document, newFile))
            else:
                self.RemoveFile(oldFile)
                self.AddFile(newFile)
                documents = self.GetDocumentManager().GetDocuments()
                for document in documents:
                    if document.GetFilename() == oldFile:  # If the renamed document is open, update it
                        document.SetFilename(newFile, notifyViews = True)
                        document.UpdateAllViews(hint = ("rename", document, newFile))
            return True
        except OSError, (code, message):
            msgTitle = wx.GetApp().GetAppName()
            if not msgTitle:
                msgTitle = _("File Error")
            wx.MessageBox("Could not rename '%s'.  '%s'" % (wx.lib.docview.FileNameFromPath(oldFile), message),
                          msgTitle,
                          wx.OK | wx.ICON_EXCLAMATION,
                          self.GetFirstView().GetFrame())
            return False


    def GetFiles(self):
        return self._projectModel._files


    def IsFileInProject(self, filename):
        return filename in self.GetFiles()


import Wizard

class NewProjectWizard(Wizard.BaseWizard):

    WIZTITLE = _("New Project Wizard")

    def __init__(self, parent):
        self._parent = parent
        self._fullProjectPath = None
        Wizard.BaseWizard.__init__(self, parent, self.WIZTITLE)
        self._projectLocationPage = self.CreateProjectLocation(self)
        wx.wizard.EVT_WIZARD_PAGE_CHANGING(self, self.GetId(), self.OnWizPageChanging)
                
    def CreateProjectLocation(self,wizard):   
        page = Wizard.TitledWizardPage(wizard, _("Project File Location"))

        page.GetSizer().Add(wx.StaticText(page, -1, _("\nSelect the directory and filename for the project.\n\n")))
        self._projectName, self._dirCtrl, sizer, self._fileValidation = UICommon.CreateDirectoryControl(page, _("File Name:"), _("Directory:"), _("agp"), startingDirectory=os.getcwd())
        page.GetSizer().Add(sizer, 1, flag=wx.EXPAND)

        wizard.Layout()
        wizard.FitToPage(page)
        return page    

    def RunWizard(self, existingTables = None, existingRelationships = None):
        status = wx.wizard.Wizard.RunWizard(self, self._projectLocationPage)
        if status:
            docManager = wx.GetApp().GetTopWindow().GetDocumentManager()
            if os.path.exists(self._fullProjectPath):
                # What if the document is already open and we're overwriting it?
                documents = docManager.GetDocuments()
                for document in documents:
                    if document.GetFilename() == self._fullProjectPath:  # If the renamed document is open, update it
                        document.DeleteAllViews()
                        break
                os.remove(self._fullProjectPath)

            for template in docManager.GetTemplates():
                if template.GetDocumentType() == ProjectDocument:
                    doc = template.CreateDocument(self._fullProjectPath, flags = wx.lib.docview.DOC_NEW)
                    doc.OnSaveDocument(self._fullProjectPath) 
                    view = doc.GetFirstView()
                    view.AddProjectToView(doc)
                    break

        self.Destroy()
        return status


    def OnWizPageChanging(self, event):
        if event.GetDirection():  # It's going forwards
            if event.GetPage() == self._projectLocationPage:
                if not self._fileValidation():
                    event.Veto()
                    return 
                self._fullProjectPath = os.path.join(self._dirCtrl.GetValue(),UICommon.MakeNameEndInExtension(self._projectName.GetValue(),'.agp'))
                    
                
    
    def OnShowCreatePages(self):                    
        self.Hide()
        import DataModelEditor
        requestedPos = self.GetPositionTuple()
        projectService = wx.GetApp().GetService(ProjectService)
        projectView = projectService.GetView()

        wiz = DataModelEditor.ImportExportWizard(projectView.GetFrame(), pos=requestedPos)
        if wiz.RunWizard(dontDestroy=True):
           self._schemaName.SetValue(wiz.GetSchemaFileName())
        wiz.Destroy()
        self.Show(True)
         
class ProjectTemplate(wx.lib.docview.DocTemplate):

    def CreateDocument(self, path, flags):
        if path:
            return wx.lib.docview.DocTemplate.CreateDocument(self, path, flags)
        else:
            wiz = NewProjectWizard(wx.GetApp().GetTopWindow())
            wiz.RunWizard()
            wiz.Destroy()
            return None  # never return the doc, otherwise docview will think it is a new file and rename it

class ProjectAddFilesCommand(wx.lib.docview.Command):

    def __init__(self, projectDoc, files):
        wx.lib.docview.Command.__init__(self, canUndo = True)
        self._projectDoc = projectDoc
        self._files = files


    def GetName(self):
        if len(self._files) == 1:
            return _("Add File")
        else:
            return _("Add Files")


    def Do(self):
        return self._projectDoc.AddFiles(self._files)


    def Undo(self):
        return self._projectDoc.RemoveFiles(self._files)


class ProjectRemoveFilesCommand(wx.lib.docview.Command):

    def __init__(self, projectDoc, files):
        wx.lib.docview.Command.__init__(self, canUndo = True)
        self._projectDoc = projectDoc
        self._files = files


    def GetName(self):
        if len(self._files) == 1:
            return _("Remove File")
        else:
            return _("Remove Files")


    def Do(self):
        return self._projectDoc.RemoveFiles(self._files)


    def Undo(self):
        return self._projectDoc.AddFiles(self._files)


class ProjectRenameFileCommand(wx.lib.docview.Command):

    def __init__(self, projectDoc, oldFile, newFile, isProject = False):
        wx.lib.docview.Command.__init__(self, canUndo = True)
        self._projectDoc = projectDoc
        self._oldFile = oldFile
        self._newFile = newFile
        self._isProject = isProject


    def GetName(self):
        return _("Rename File")


    def Do(self):
        return self._projectDoc.RenameFile(self._oldFile, self._newFile, self._isProject)


    def Undo(self):
        return self._projectDoc.RenameFile(self._newFile, self._oldFile, self._isProject)


class ProjectTreeCtrl(wx.TreeCtrl):

    def __init__(self, parent, id, style):
        wx.TreeCtrl.__init__(self, parent, id, style = style)

        templates = wx.GetApp().GetDocumentManager().GetTemplates()
        iconList = wx.ImageList(16, 16, initialCount = len(templates))
        self._iconIndexLookup = []
        for template in templates:
            icon = template.GetIcon()
            if icon:
                if icon.GetHeight() != 16:
                    icon.SetHeight(16)  # wxBug: img2py.py uses EmptyIcon which is 32x32
                if icon.GetWidth() != 16:
                    icon.SetWidth(16)   # wxBug: img2py.py uses EmptyIcon which is 32x32
                iconIndex = iconList.AddIcon(icon)
                self._iconIndexLookup.append((template, iconIndex))
                
        icon = getBlankIcon()
        if icon.GetHeight() != 16:
            icon.SetHeight(16)  # wxBug: img2py.py uses EmptyIcon which is 32x32
        if icon.GetWidth() != 16:
            icon.SetWidth(16)   # wxBug: img2py.py uses EmptyIcon which is 32x32
        self._blankIconIndex = iconList.AddIcon(icon)
        self.AssignImageList(iconList)


    def OnCompareItems(self, item1, item2):
        return cmp(self.GetItemText(item1).lower(), self.GetItemText(item2).lower())


    def AppendItem(self, parent, filepath):
        item = wx.TreeCtrl.AppendItem(self, parent, filepath)

        found = False
        template = wx.GetApp().GetDocumentManager().FindTemplateForPath(filepath)
        if not template and parent == self.GetRootItem(): # If the parent is a root it's a new project
            template = wx.GetApp().GetDocumentManager().FindTemplateForPath('.agp')
        if template:
            for t, iconIndex in self._iconIndexLookup:
                if t is template:
                    self.SetItemImage(item, iconIndex, wx.TreeItemIcon_Normal)
                    self.SetItemImage(item, iconIndex, wx.TreeItemIcon_Expanded)
                    self.SetItemImage(item, iconIndex, wx.TreeItemIcon_Selected)
                    found = True
                    break

        if not found:
            self.SetItemImage(item, self._blankIconIndex, wx.TreeItemIcon_Normal)
            self.SetItemImage(item, self._blankIconIndex, wx.TreeItemIcon_Expanded)
            self.SetItemImage(item, self._blankIconIndex, wx.TreeItemIcon_Selected)

        return item


class ProjectView(wx.lib.docview.View):


    #----------------------------------------------------------------------------
    # Overridden methods
    #----------------------------------------------------------------------------

    def __init__(self, service = None):
        wx.lib.docview.View.__init__(self)
        self._service = service  # not used, but kept to match other Services
        self._lastDirectory = ""
        self._treeCtrl = None
        self._editingSoDontKillFocus = False
        self._checkEditMenu = True


    def Destroy(self):
        projectService = wx.GetApp().GetService(ProjectService)
        if projectService:
            projectService.SetView(None)
        wx.lib.docview.View.Destroy(self)


    def GetDocument(self):
        if not self._treeCtrl:
            return None
            
        items = self._treeCtrl.GetSelections()
        if not items: # No selection, so just return first project
            item = self._treeCtrl.GetFirstVisibleItem()
            if item.IsOk():
                return self._GetItemProject(item)
            else:
                return None
                
        for item in items:
            project = self._GetItemProject(item)
            if project:
                return project
                
        return None


    def GetDocumentManager(self):  # Overshadow this since the superclass uses the view._viewDocument attribute directly, which the project editor doesn't use since it hosts multiple docs
        return wx.GetApp().GetDocumentManager()


    def OnChangeFilename(self):
        if self.GetFrame():
            title = _("Projects")
            if self.GetDocumentManager().GetFlags() & wx.lib.docview.DOC_SDI and wx.GetApp().GetAppName():
                title =  title + " - " + wx.GetApp().GetAppName()
            self.GetFrame().SetTitle(title)
        project = self.GetDocument()
        if project:
            projectItem = self._GetProjectItem(project)
            name = self._treeCtrl.GetItemText(self._GetProjectItem(project))
            name2 = self._MakeProjectName(project)
            if name != name2:
                self._treeCtrl.SetItemText(projectItem, name2)
                self._treeCtrl.SortChildren(self._treeCtrl.GetRootItem())


    def Activate(self, activate = True):
        if not wx.GetApp().IsMDI():
            if activate and not self.IsShown():
                self.Show()

        if self.IsShown():
            wx.lib.docview.View.Activate(self, activate = activate)
            if activate and self._treeCtrl:
                self._treeCtrl.SetFocus()


    def OnCreate(self, doc, flags):
        config = wx.ConfigBase_Get()
        if wx.GetApp().IsMDI():
            self._embeddedWindow = wx.GetApp().GetTopWindow().GetEmbeddedWindow(wx.lib.pydocview.EMBEDDED_WINDOW_TOPLEFT)
            self.SetFrame(self._embeddedWindow)
            frame = self._embeddedWindow
        else:
            self._embeddedWindow = None
            pos = config.ReadInt("ProjectFrameXLoc", -1), config.ReadInt("ProjectFrameYLoc", -1)
            # make sure frame is visible
            screenWidth = wx.SystemSettings.GetMetric(wx.SYS_SCREEN_X)
            screenHeight = wx.SystemSettings.GetMetric(wx.SYS_SCREEN_Y)
            if pos[0] < 0 or pos[0] >= screenWidth or pos[1] < 0 or pos[1] >= screenHeight:
                pos = wx.DefaultPosition

            size = wx.Size(config.ReadInt("ProjectFrameXSize", -1), config.ReadInt("ProjectFrameYSize", -1))

            title = _("Projects")
            if self.GetDocumentManager().GetFlags() & wx.lib.docview.DOC_SDI and wx.GetApp().GetAppName():
                title =  title + " - " + wx.GetApp().GetAppName()

            frame = wx.GetApp().CreateDocumentFrame(self, doc, 0, title = title, pos = pos, size = size)
            if config.ReadInt("ProjectFrameMaximized", False):
                frame.Maximize(True)

        sizer = wx.BoxSizer()
        self._treeCtrl = ProjectTreeCtrl(frame, -1, style = wx.TR_HIDE_ROOT | wx.TR_HAS_BUTTONS | wx.TR_EDIT_LABELS | wx.TR_DEFAULT_STYLE | wx.TR_MULTIPLE)
        self._treeCtrl.AddRoot(_("Projects"))

        if self._embeddedWindow:
            sizer.Add(self._treeCtrl)
            sizer.Fit(frame)
        else:
            sizer.Add(self._treeCtrl, 1, wx.EXPAND, 0)
        frame.SetSizer(sizer)
        frame.Layout()
        self.Activate()

        if wx.GetApp().IsMDI():
            wx.EVT_SET_FOCUS(self._treeCtrl, self.OnFocus)
            wx.EVT_KILL_FOCUS(self._treeCtrl, self.OnKillFocus)

        if self.GetDocumentManager().GetFlags() & wx.lib.docview.DOC_SDI:
            wx.EVT_TREE_ITEM_ACTIVATED(self._treeCtrl, self._treeCtrl.GetId(), self.OnOpenSelectionSDI)
        else:
            wx.EVT_TREE_ITEM_ACTIVATED(self._treeCtrl, self._treeCtrl.GetId(), self.OnOpenSelection)
        wx.EVT_TREE_BEGIN_LABEL_EDIT(self._treeCtrl, self._treeCtrl.GetId(), self.OnBeginLabelEdit)
        wx.EVT_TREE_END_LABEL_EDIT(self._treeCtrl, self._treeCtrl.GetId(), self.OnEndLabelEdit)
        wx.EVT_RIGHT_DOWN(self._treeCtrl, self.OnRightClick)
        wx.EVT_KEY_DOWN(self._treeCtrl, self.OnKeyPressed)
        # wx.EVT_COMMAND_RIGHT_CLICK(self._treeCtrl, self._treeCtrl.GetId(), self.OnRightClick) # wxBug: This isn't working for some reason

        # drag-and-drop support
        dt = ProjectFileDropTarget(self)
        self._treeCtrl.SetDropTarget(dt)

        return True


    def WriteProjectConfig(self):
        frame = self.GetFrame()
        config = wx.ConfigBase_Get()
        if frame and not self._embeddedWindow:
            if not frame.IsMaximized():
                config.WriteInt("ProjectFrameXLoc", frame.GetPositionTuple()[0])
                config.WriteInt("ProjectFrameYLoc", frame.GetPositionTuple()[1])
                config.WriteInt("ProjectFrameXSize", frame.GetSizeTuple()[0])
                config.WriteInt("ProjectFrameYSize", frame.GetSizeTuple()[1])
            config.WriteInt("ProjectFrameMaximized", frame.IsMaximized())

        if config.ReadInt("ProjectSaveDocs", True):
            projectFileNames = []
            projectExpanded = []
            if self._treeCtrl:
                for projectItem in self._GetChildItems(self._treeCtrl.GetRootItem()):
                    project = self._GetItemProject(projectItem)
                    if not project.OnSaveModified():
                        return
                    if project.GetDocumentSaved():  # Might be a new document and "No" selected to save it
                        projectFileNames.append(str(project.GetFilename()))
                        projectExpanded.append(self._treeCtrl.IsExpanded(projectItem))
            config.Write("ProjectSavedDocs", projectFileNames.__repr__())
            config.Write("ProjectExpandedSavedDocs", projectExpanded.__repr__())


    def OnClose(self, deleteWindow = True):
        if self.GetDocumentManager().GetFlags() & wx.lib.docview.DOC_SDI:
            self.WriteProjectConfig()
        project = self.GetDocument()
        if not project:
            return True
        if not self.GetDocument().Close():
            return True
        self.Activate(False)
        if project:
            projectItem = self._GetProjectItem(project)
            if projectItem:
                self._treeCtrl.Delete(projectItem)
        # We don't need to delete the window since it is a floater/embedded
        return True


    def _GetParentFrame(self):
        return wx.GetTopLevelParent(self.GetFrame())


    def OnUpdate(self, sender = None, hint = None):
        wx.lib.docview.View.OnUpdate(self, sender, hint)
        if hint:
            if hint[0] == "add":
                projectItem = self._GetProjectItem(hint[1])
                files = hint[2]
                self._treeCtrl.UnselectAll()
                self._treeCtrl.Expand(projectItem)
                for file in files:
                    item = self._treeCtrl.AppendItem(projectItem, os.path.basename(file))
                    self._treeCtrl.SetPyData(item, file)
                    self._treeCtrl.SelectItem(item)
                    self._treeCtrl.EnsureVisible(item)  # wxBug: Doesn't work
                self._treeCtrl.SortChildren(projectItem)
            elif hint[0] == "remove":
                projectItem = self._GetProjectItem(hint[1])
                files = hint[2]
                children = self._GetChildItems(projectItem)
                for child in children:
                    if self._GetItemFile(child) in files:
                        self._treeCtrl.Delete(child)
            elif hint[0] == "select":
                projectItem = self._GetProjectItem(hint[1])
                files = hint[2]
                self._treeCtrl.UnselectAll()
                children = self._GetChildItems(projectItem)
                for child in children:
                    if self._GetItemFile(child) in files:
                        self._treeCtrl.SelectItem(child)
                        self._treeCtrl.EnsureVisible(child)  # wxBug:  Doesn't work
            elif hint[0] == "rename":
                projectItem = self._GetProjectItem(hint[1])
                self._treeCtrl.SetItemText(projectItem, os.path.basename(hint[2]))


    def ProcessEvent(self, event):
        id = event.GetId()
        if id == ProjectService.ADD_FILES_TO_PROJECT_ID:
            self.OnAddFileToProject(event)
            return True
        elif id == ProjectService.ADD_CURRENT_FILE_TO_PROJECT_ID:
            return False  # Implement this one in the service
        elif id == ProjectService.RENAME_ID:
            self.OnRename(event)
            return True
        elif id == wx.ID_CUT:
            self.OnCut(event)
            return True
        elif id == wx.ID_COPY:
            self.OnCopy(event)
            return True
        elif id == wx.ID_PASTE:
            self.OnPaste(event)
            return True
        elif id == wx.ID_CLEAR or id == ProjectService.REMOVE_FROM_PROJECT:
            self.OnClear(event)
            return True
        elif id == wx.ID_SELECTALL:
            self.OnSelectAll(event)
            return True
        elif id == ProjectService.OPEN_SELECTION_ID:
            self.OnOpenSelection(event)
            return True
        elif id == wx.lib.pydocview.FilePropertiesService.PROPERTIES_ID:
            self.OnProperties(event)
            return True
        else:
            return False

    def ProcessUpdateUIEvent(self, event):
        # Hack: The edit menu is not being set for projects that are preloaded at startup, so make sure it is OK here
        if self._checkEditMenu:
            doc = self.GetDocument()
            if doc and not doc.GetCommandProcessor().GetEditMenu():
                doc.GetCommandProcessor().SetEditMenu(wx.GetApp().GetEditMenu(self._GetParentFrame()))
            self._checkEditMenu = False
        id = event.GetId()
        if id == ProjectService.ADD_FILES_TO_PROJECT_ID:
            event.Enable(self._HasProjectsSelected() or self._HasFilesSelected())
            return True
        elif id == ProjectService.ADD_CURRENT_FILE_TO_PROJECT_ID:
            event.Enable(False)
            return True
        elif id == ProjectService.RENAME_ID:
            event.Enable(self._HasFilesSelected() or self._HasProjectsSelected())
            return True
        elif id == wx.ID_CUT:
            event.Enable(self._AreSelectedItemsFromSameProject())
            return True
        elif id == wx.ID_COPY:
            event.Enable(self._HasFilesSelected())
            return True
        elif id == wx.ID_PASTE:
            event.Enable(self.CanPaste())
            return True
        elif id == wx.ID_CLEAR or id == ProjectService.REMOVE_FROM_PROJECT:
            event.Enable(self._AreSelectedItemsFromSameProject())
            return True
        elif id == wx.ID_SELECTALL:
            event.Enable(self._HasFiles())
            return True
        elif id == ProjectService.OPEN_SELECTION_ID:
            event.Enable(self._HasFilesSelected())
            return True
        elif id == wx.lib.pydocview.FilePropertiesService.PROPERTIES_ID:
            event.Enable(self._HasProjectsSelected() or self._HasFilesSelected())
            return True
        else:
            return False

    #----------------------------------------------------------------------------
    # Display Methods
    #----------------------------------------------------------------------------

    def IsShown(self):
        if not self.GetFrame():
            return False
        return self.GetFrame().IsShown()


    def Hide(self):
        self.Show(False)


    def Show(self, show = True):
        self.GetFrame().Show(show)
        if wx.GetApp().IsMDI():
            mdiParentFrame = wx.GetApp().GetTopWindow()
            mdiParentFrame.ShowEmbeddedWindow(self.GetFrame(), show)


    #----------------------------------------------------------------------------
    # Methods for ProjectDocument and ProjectService to call
    #----------------------------------------------------------------------------

    def SetExpandedProjects(self, expandedProjects):
        self._treeCtrl.UnselectAll()
        firstItem = None
        for i, item in enumerate(self._GetChildItems(self._treeCtrl.GetRootItem())):
            if i == 0:
                firstItem = item
            if expandedProjects[i]:
                self._treeCtrl.Expand(item)
            else:
                self._treeCtrl.Collapse(item)
        # wxBug: This causes a crash, tried using ScrollTo which crashed as well.  Then tried calling it with wx.CallAfter and that crashed as well, with both EnsureVisible and ScrollTo
        # self._treeCtrl.EnsureVisible(self._treeCtrl.GetRootItem())
        # So doing the following massive hack which forces the treectrl to scroll up to the top item
        if firstItem:
            if expandedProjects[i]:
                self._treeCtrl.Collapse(firstItem)
                self._treeCtrl.Expand(firstItem)
            else:
                self._treeCtrl.Expand(firstItem)
                self._treeCtrl.Collapse(firstItem)

    def GetSelectedFile(self):
        for item in self._treeCtrl.GetSelections():
            return self._GetItemFile(item)
            
    def AddProjectToView(self, document):
        rootItem = self._treeCtrl.GetRootItem()
        projectItem = self._treeCtrl.AppendItem(rootItem, self._MakeProjectName(document))
        self._treeCtrl.SetPyData(projectItem, document)
        for file in document.GetFiles():
            fileItem = self._treeCtrl.AppendItem(projectItem, os.path.basename(file))
            self._treeCtrl.SetPyData(fileItem, file)
        self._treeCtrl.SortChildren(rootItem)
        self._treeCtrl.SortChildren(projectItem)
        self._treeCtrl.UnselectAll()
        self._treeCtrl.Expand(projectItem)
        self._treeCtrl.SelectItem(projectItem)
        if self._embeddedWindow:
            document.GetCommandProcessor().SetEditMenu(wx.GetApp().GetEditMenu(self._GetParentFrame()))

    #----------------------------------------------------------------------------
    # Methods for OutlineService
    #----------------------------------------------------------------------------
    def DoLoadOutlineCallback(self, force=False):
        """ Project Editor is a special case for the Outline Service.
            You need to be able to be active in the Project Manager without clearing
            the Outline View.  So we make the Project Editor a client of the Outline
            Service, but we don't load anything in the Outline View, leaving the
            contents of the Outline View alone (e.g. last document's outline view).
        """
        pass

    #----------------------------------------------------------------------------
    # Control events
    #----------------------------------------------------------------------------

    def OnProperties(self, event):
        items = self._treeCtrl.GetSelections()
        if not items:
            return
        item = items[0]
        if self._IsItemProject(item):
            projectPropertiesDialog = ProjectPropertiesDialog(wx.GetApp().GetTopWindow(), self._GetItemProject(item).GetFilename())
            if projectPropertiesDialog.ShowModal() == wx.ID_OK:
                pass  # Handle OK
            projectPropertiesDialog.Destroy()
        elif self._IsItemFile(item):
            filePropertiesService = wx.GetApp().GetService(wx.lib.pydocview.FilePropertiesService)
            filePropertiesService.ShowPropertiesDialog(self._GetItemFile(item))


    def OnAddFileToProject(self, event):
        if wx.Platform == "__WXMSW__" or wx.Platform == "__WXGTK__" or wx.Platform == "__WXMAC__":
            allfilter = ''
            descr = ''
            for temp in self.GetDocumentManager()._templates:
                if temp.IsVisible():
                    if len(descr) > 0:
                        descr = descr + _('|')
                        allfilter = allfilter + _(';')
                    descr = descr + temp.GetDescription() + _(" (") + temp.GetFileFilter() + _(") |") + temp.GetFileFilter()  # spacing is important, make sure there is no space after the "|", it causes a bug on wx_gtk
                    allfilter = allfilter + temp.GetFileFilter()
            descr = _("All") + _(" (") + allfilter + _(") |") + allfilter + _('|') + descr  # spacing is important, make sure there is no space after the "|", it causes a bug on wx_gtk
            descr = descr + _("|") + _("Any (*.*) | *.*")
        else:
            descr = _("*.*")
        if True or _WINDOWS:
            dialog = wx.FileDialog(self.GetFrame(), _("Add Files"), self._lastDirectory, "", descr, wx.OPEN | wx.HIDE_READONLY | wx.MULTIPLE)
            if dialog.ShowModal() != wx.ID_OK:
                return
            paths = dialog.GetPaths()
            dialog.Destroy()
        else:
            paths = wx.FileSelector(_("Add Files"), self._lastDirectory, "", wildcard = descr, flags = wx.OPEN | wx.HIDE_READONLY | wx.MULTIPLE, parent=self.GetFrame())
            if type(paths) == types.StringType:
                paths = [paths]
        if len(paths):
            self._lastDirectory = wx.lib.docview.PathOnly(paths[0])
            self.GetDocument().GetCommandProcessor().Submit(ProjectAddFilesCommand(self.GetDocument(), paths))
        self.Activate(True)  # after add, should put focus on project editor


    def DoAddFilesToProject(self, filenames):
        # method used by Drag-n-Drop to add files to current Project
        self.GetDocument().GetCommandProcessor().Submit(ProjectAddFilesCommand(self.GetDocument(), filenames))


    def DoSelectFiles(self, filenames):
        # method used by Drag-n-Drop to select files in current Project
        for selection in self._treeCtrl.GetSelections():
            self._treeCtrl.SelectItem(selection, False)
        for file in filenames:
            item = self._GetFileItem(longFileName=file)
            if item:
                self._treeCtrl.SelectItem(item, True)
                self._treeCtrl.EnsureVisible(item)


    def DoSelectProject(self, x, y):
        # method used by Drag-n-Drop to set current Project based on cursor position
        item, flag = self._treeCtrl.HitTest((x,y))
        if not item:
            return False

        project = self._GetItemProject(item)
        if not project:
            return False

        projectItem = self._GetProjectItem(project)
        self._treeCtrl.UnselectAll()
        self._treeCtrl.SelectItem(projectItem)
        return True


    def OnFocus(self, event):
        wx.GetApp().GetDocumentManager().ActivateView(self)
        event.Skip()


    def OnKillFocus(self, event):
        # Get the top MDI window and "activate" it since it is already active from the perspective of the MDIParentFrame
        # wxBug: Would be preferable to call OnActivate, but have casting problem, so added Activate method to docview.DocMDIChildFrame
        if not self._editingSoDontKillFocus:  # wxBug: This didn't used to happen, but now when you start to edit an item in a wxTreeCtrl it puts out a KILL_FOCUS event, so we need to detect it
            childFrame = wx.GetApp().GetTopWindow().GetActiveChild()
            if childFrame:
                childFrame.Activate()
        event.Skip()


    def OnRightClick(self, event):
        self.Activate(True)
        if not self._treeCtrl.GetSelections():
            return
        if len(self._treeCtrl.GetSelections()) == 1 and self._IsItemRoot(self._treeCtrl.GetSelections()[0]):
            return  # Don't do a menu if it's just the root item selected
        menu = wx.Menu()
        if self._HasFilesSelected():  # Files context
            menu.Append(ProjectService.OPEN_SELECTION_ID, _("&Open"), _("Opens the selection"))
            menu.Enable(ProjectService.OPEN_SELECTION_ID, True)
            wx.EVT_MENU(self._GetParentFrame(), ProjectService.OPEN_SELECTION_ID, self.OnOpenSelection)
            itemIDs = [None]
            for item in self._treeCtrl.GetSelections():
                if self._IsItemProcessModelFile(item):
                    itemIDs = [None, ProjectService.RUN_SELECTED_PM_ID, None]
                    break
        else:  # Project context
            itemIDs = [wx.ID_CLOSE, wx.ID_SAVE, wx.ID_SAVEAS, None]
        menuBar = self._GetParentFrame().GetMenuBar()
        itemIDs = itemIDs + [wx.lib.pydocview.FilePropertiesService.PROPERTIES_ID, None, ProjectService.ADD_FILES_TO_PROJECT_ID, ProjectService.REMOVE_FROM_PROJECT, None, wx.ID_UNDO, wx.ID_REDO, None, wx.ID_CUT, wx.ID_COPY, wx.ID_PASTE, wx.ID_CLEAR, None, wx.ID_SELECTALL, None, ProjectService.RENAME_ID]
        for itemID in itemIDs:
            if not itemID:
                menu.AppendSeparator()
            else:
                if itemID == ProjectService.RUN_SELECTED_PM_ID:
                    menu.Append(ProjectService.RUN_SELECTED_PM_ID, _("Run Process"))
                    wx.EVT_MENU(self._GetParentFrame(), ProjectService.RUN_SELECTED_PM_ID, self.OnRunSelectedPM)
                elif itemID == ProjectService.REMOVE_FROM_PROJECT:
                    menu.Append(ProjectService.REMOVE_FROM_PROJECT, _("Remove Selected Files from Project"))
                    wx.EVT_MENU(self._GetParentFrame(), ProjectService.REMOVE_FROM_PROJECT, self.OnClear)
                    wx.EVT_UPDATE_UI(self._GetParentFrame(), ProjectService.REMOVE_FROM_PROJECT, self._GetParentFrame().ProcessUpdateUIEvent)
                else:
                    item = menuBar.FindItemById(itemID)
                    if item:
                        menu.Append(itemID, item.GetLabel())
        self._treeCtrl.PopupMenu(menu, wx.Point(event.GetX(), event.GetY()))
        menu.Destroy()

    def OnRunSelectedPM(self, event):
        projectService = wx.GetApp().GetService(ProjectService)
        projectService.OnRunProcessModel(event, runSelected=True)

    def OnRename(self, event):
        if self._treeCtrl.GetSelections():
            self._treeCtrl.EditLabel(self._treeCtrl.GetSelections()[0])


    def OnBeginLabelEdit(self, event):
        self._editingSoDontKillFocus = True
        item = event.GetItem()
        if not self._IsItemFile(item) and not self._IsItemProject(item):
            event.Veto()


    def OnEndLabelEdit(self, event):
        self._editingSoDontKillFocus = False
        item = event.GetItem()
        newName = event.GetLabel()
        if not newName or (not self._IsItemFile(item) and not self._IsItemProject(item)):
            event.Veto()
            return
        if self._IsItemFile(item):
            oldFile = self._GetItemFile(item)
            newFile = os.path.join(os.path.split(oldFile)[0], newName)
            if not self._GetItemProject(item).GetCommandProcessor().Submit(ProjectRenameFileCommand(self.GetDocument(), oldFile, newFile)):
                event.Veto()
                return
            self._treeCtrl.SortChildren(self._treeCtrl.GetItemParent(self._treeCtrl.GetSelections()[0]))
        elif self._IsItemProject(item):
            oldFile = self._GetItemProject(item).GetFilename()
            newFile = os.path.join(os.path.split(oldFile)[0], newName)
            if not self._GetItemProject(item).GetCommandProcessor().Submit(ProjectRenameFileCommand(self.GetDocument(), oldFile, newFile, True)):
                event.Veto()
                return
            self._treeCtrl.SortChildren(self._treeCtrl.GetRootItem())


    def CanPaste(self):
        # wxBug: Should be able to use IsSupported/IsSupportedFormat here
        #fileDataObject = wx.FileDataObject()
        #hasFilesInClipboard = wx.TheClipboard.IsSupportedFormat(wx.FileDataObject)
        if not wx.TheClipboard.IsOpened():
            if wx.TheClipboard.Open():
                fileDataObject = wx.FileDataObject()
                hasFilesInClipboard = wx.TheClipboard.GetData(fileDataObject)
                wx.TheClipboard.Close()
        else:
            hasFilesInClipboard = False
        return hasFilesInClipboard


    def OnCut(self, event):
        if self._AreSelectedItemsFromSameProject():
            self.OnCopy(event)
            self.OnClear(event)


    def OnCopy(self, event):
        fileDataObject = wx.FileDataObject()
        items = self._treeCtrl.GetSelections()
        for item in items:
            if self._IsItemFile(item):
                file = self._treeCtrl.GetPyData(item)
                fileDataObject.AddFile(file)
        if len(fileDataObject.GetFilenames()) > 0 and wx.TheClipboard.Open():
            wx.TheClipboard.SetData(fileDataObject)
            wx.TheClipboard.Close()


    def OnPaste(self, event):
        if wx.TheClipboard.Open():
            fileDataObject = wx.FileDataObject()
            if wx.TheClipboard.GetData(fileDataObject):
                self.GetDocument().GetCommandProcessor().Submit(ProjectAddFilesCommand(self.GetDocument(), fileDataObject.GetFilenames()))
            wx.TheClipboard.Close()


    def OnClear(self, event):
        if self._AreSelectedItemsFromSameProject():
            items = self._treeCtrl.GetSelections()
            files = []
            for item in items:
                if self._IsItemFile(item):
                    files.append(self._GetItemFile(item))
            self.GetDocument().GetCommandProcessor().Submit(ProjectRemoveFilesCommand(self._GetItemProject(items[0]), files))


    def OnKeyPressed(self, event):
        key = event.KeyCode()
        if key == wx.WXK_DELETE:
            self.OnClear(event)
        else:
            event.Skip()


    def OnSelectAll(self, event):
        project = self.GetDocument()
        if project:
            self._treeCtrl.UnselectAll()
            for child in self._GetChildItems(self._GetProjectItem(project)):
                self._treeCtrl.SelectItem(child)


    def OnOpenSelectionSDI(self, event):
        # Do a call after so that the second mouseclick on a doubleclick doesn't reselect the project window
        wx.CallAfter(self.OnOpenSelection, None)


    def OnOpenSelection(self, event):
        doc = None
        try:
            items = self._treeCtrl.GetSelections()
            for item in items:
                if self._IsItemFile(item):
                    filepath = self._GetItemFile(item)
                    if not os.path.exists(filepath):
                        msgTitle = wx.GetApp().GetAppName()
                        if not msgTitle:
                            msgTitle = _("File Not Found")
                        yesNoMsg = wx.MessageDialog(self.GetFrame(),
                                      _("The file '%s' was not found in '%s'.\n\nWould you like to browse for the file?") % (wx.lib.docview.FileNameFromPath(filepath), wx.lib.docview.PathOnly(filepath)),
                                      msgTitle,
                                      wx.YES_NO
                                      )
                        if yesNoMsg.ShowModal() == wx.ID_NO:
                            continue
                        findFile = wx.FileDialog(self.GetFrame(),
                                                 _("Choose a file"),
                                                 wx.lib.docview.PathOnly(filepath),
                                                 wx.lib.docview.FileNameFromPath(filepath),
                                                 style = wx.OPEN
                                                )
                        if findFile.ShowModal() == wx.ID_OK and findFile.GetPath():
                            newpath = findFile.GetPath()
                        else:
                            newpath = None
                        findFile.Destroy()
                        if newpath:
                            # update Project Model with new location
                            self.GetDocument().RemoveFile(filepath)
                            self.GetDocument().AddFile(newpath)
                            filepath = newpath

                    doc = self.GetDocumentManager().CreateDocument(filepath, wx.lib.docview.DOC_SILENT)

        except IOError, (code, message):
            msgTitle = wx.GetApp().GetAppName()
            if not msgTitle:
                msgTitle = _("File Error")
            wx.MessageBox("Could not open '%s'." % wx.lib.docview.FileNameFromPath(filepath),
                          msgTitle,
                          wx.OK | wx.ICON_EXCLAMATION,
                          self.GetFrame())


    #----------------------------------------------------------------------------
    # Convenience methods
    #----------------------------------------------------------------------------

    def _HasFiles(self):
        if not self._treeCtrl:
            return False
        return self._treeCtrl.GetCount() > 1    #  1 item = root item, don't count as having files


    def _HasProjectsSelected(self):
        if not self._treeCtrl:
            return False
        items = self._treeCtrl.GetSelections()
        if not items:
            return False
        for item in items:
            if self._IsItemProject(item):
                return True
        return False


    def _HasFilesSelected(self):
        if not self._treeCtrl:
            return False
        items = self._treeCtrl.GetSelections()
        if not items:
            return False
        for item in items:
            if not self._IsItemFile(item):
                return False
        return True


    def _MakeProjectName(self, project):
        return project.GetPrintableName()


    # Return the tree item for a project
    def _GetProjectItem(self, project):
        children = self._GetChildItems(self._treeCtrl.GetRootItem())
        for child in children:
            if self._treeCtrl.GetPyData(child) == project:
                return child
        return None


    # Returns the project for an item, either for a project item or a file that is part of a project
    def _GetItemProject(self, item):
        if self._IsItemRoot(item):
            return None
        if self._IsItemProject(item):
            return self._treeCtrl.GetPyData(item)
        if self._IsItemFile(item):
            return self._treeCtrl.GetPyData(self._treeCtrl.GetItemParent(item))
        return None


    def _GetItemFile(self, item):
        if self._IsItemFile(item):
            return self._treeCtrl.GetPyData(item)
        else:
            return None


    def _GetFileItem(self, shortFileName = None, longFileName = None):
        """ Returns the tree item for a file given the short (display) or long (fullpath) file name. """
        
        if shortFileName:
            project_children = self._GetChildItems(self._treeCtrl.GetRootItem())
            for child in project_children:
                file_children = self._GetChildItems(child)
                for file_child in file_children:
                    if self._treeCtrl.GetItemText(file_child) == shortFileName:
                        return file_child
            return None
        else:
            project_children = self._GetChildItems(self._treeCtrl.GetRootItem())
            for child in project_children:
                file_children = self._GetChildItems(child)
                for file_child in file_children:
                    if self._treeCtrl.GetPyData(file_child) == longFileName:
                        return file_child
            return None


    def GetFilePathFromTreeName(self, shortFileName):
        """
        Returns the data object given a short (display) file name for a file. The data
        object should be the full path.
        """
        return self._GetItemFile(self._GetFileItem(shortFileName))
    

    def SelectFileInTree(self, shortFileName):
        item = self._GetFileItem(shortFileName)
        if item:
            for selection in self._treeCtrl.GetSelections():
                self._treeCtrl.SelectItem(selection, False)
            self._treeCtrl.SelectItem(item, True)
            self._treeCtrl.EnsureVisible(item)


    def _IsItemRoot(self, item):
        return item == self._treeCtrl.GetRootItem()


    def _IsItemProject(self, item):
        return isinstance(self._treeCtrl.GetPyData(item), ProjectDocument)


    def _IsItemFile(self, item):
        return isinstance(self._treeCtrl.GetPyData(item), types.StringTypes)


    def _IsItemProcessModelFile(self, item):
        if ACTIVEGRID_BASE_IDE:
            return False
            
        if isinstance(self._treeCtrl.GetPyData(item), types.StringTypes):
            filename = self._treeCtrl.GetPyData(item)
            ext = None
            for template in self.GetDocumentManager().GetTemplates():
                if template.GetDocumentType() == ProcessModelEditor.ProcessModelDocument:
                    ext = template.GetDefaultExtension()
                    break;
            if not ext:
                return False

            if filename.endswith(ext):
                return True

        return False


    def _AreSelectedItemsFromSameProject(self):
        if not self._treeCtrl:
            return False
        items = self._treeCtrl.GetSelections()
        if not items:
            return False
        project = self._GetItemProject(items[0])
        if project == None:
            return False
        for item in items:
            if not self._IsItemFile(item):
                return False
            if self._GetItemProject(item) != project:
                return False
        return True


    def _GetChildItems(self, parentItem):
        children = []
        (child, cookie) = self._treeCtrl.GetFirstChild(parentItem)
        while child.IsOk():
            children.append(child)
            (child, cookie) = self._treeCtrl.GetNextChild(parentItem, cookie)
        return children
        
        

class ProjectFileDropTarget(wx.FileDropTarget):

    def __init__(self, view):
        wx.FileDropTarget.__init__(self)
        self._view = view


    def OnDropFiles(self, x, y, filenames):
        if self._view.DoSelectProject(x, y):
            self._view.DoAddFilesToProject(filenames)
            self._view.DoSelectFiles(filenames)
            return True
        return False


    def OnDragOver(self, x, y, default):
        if self._view.DoSelectProject(x,y):
            return wx.DragCopy
        return wx.DragNone


class ProjectPropertiesDialog(wx.Dialog):


    def __init__(self, parent, filename):
        wx.Dialog.__init__(self, parent, -1, _("Project Properties"), size = (310, 330))

        HALF_SPACE = 5
        SPACE = 10

        filePropertiesService = wx.GetApp().GetService(wx.lib.pydocview.FilePropertiesService)

        notebook = wx.Notebook(self, -1)
        tab = wx.Panel(notebook, -1)

        gridSizer = RowColSizer()

        gridSizer.Add(wx.StaticText(tab, -1, _("Filename:")), flag=wx.RIGHT, border=HALF_SPACE, row=0, col=0)
        if os.path.isfile(filename):
            gridSizer.Add(wx.StaticText(tab, -1, os.path.split(filename)[1]), row=0, col=1)

            gridSizer.Add(wx.StaticText(tab, -1, _("Location:")), flag=wx.RIGHT, border=HALF_SPACE, row=1, col=0)
            gridSizer.Add(wx.StaticText(tab, -1, filePropertiesService.chopPath(os.path.split(filename)[0])), flag=wx.BOTTOM, border=SPACE, row=1, col=1)

            gridSizer.Add(wx.StaticText(tab, -1, _("Size:")), flag=wx.RIGHT, border=HALF_SPACE, row=2, col=0)
            gridSizer.Add(wx.StaticText(tab, -1, str(os.path.getsize(filename)) + ' ' + _("bytes")), row=2, col=1)

            lineSizer = wx.BoxSizer(wx.VERTICAL)    # let the line expand horizontally without vertical expansion
            lineSizer.Add(wx.StaticLine(tab, -1, size = (10,-1)), 0, wx.EXPAND)
            gridSizer.Add(lineSizer, flag=wx.EXPAND|wx.ALIGN_CENTER_VERTICAL|wx.TOP, border=HALF_SPACE, row=3, col=0, colspan=2)

            gridSizer.Add(wx.StaticText(tab, -1, _("Created:")), flag=wx.RIGHT, border=HALF_SPACE, row=4, col=0)
            gridSizer.Add(wx.StaticText(tab, -1, time.ctime(os.path.getctime(filename))), row=4, col=1)

            gridSizer.Add(wx.StaticText(tab, -1, _("Modified:")), flag=wx.RIGHT, border=HALF_SPACE, row=5, col=0)
            gridSizer.Add(wx.StaticText(tab, -1, time.ctime(os.path.getmtime(filename))), row=5, col=1)

            gridSizer.Add(wx.StaticText(tab, -1, _("Accessed:")), flag=wx.RIGHT, border=HALF_SPACE, row=6, col=0)
            gridSizer.Add(wx.StaticText(tab, -1, time.ctime(os.path.getatime(filename))), row=6, col=1)

        else:
            gridSizer.Add(wx.StaticText(tab, -1, os.path.split(filename)[1] + ' ' + _("[new project]")), row=0, col=1)

        # add a border around the inside of the tab
        spacerGrid = wx.BoxSizer(wx.VERTICAL)
        spacerGrid.Add(gridSizer, 0, wx.ALL, SPACE);
        tab.SetSizer(spacerGrid)
        notebook.AddPage(tab, _("General"))
        if wx.Platform == "__WXMSW__":
            notebook.SetPageSize((310,200))

        sizer = wx.BoxSizer(wx.VERTICAL)
        sizer.Add(notebook, 0, wx.ALL | wx.EXPAND, SPACE)
        sizer.Add(self.CreateButtonSizer(wx.OK), 0, wx.ALIGN_RIGHT | wx.RIGHT | wx.BOTTOM, HALF_SPACE)

        sizer.Fit(self)
        self.SetDimensions(-1, -1, 310, -1, wx.SIZE_USE_EXISTING)
        self.SetSizer(sizer)
        self.Layout()


class ProjectOptionsPanel(wx.Panel):


    def __init__(self, parent, id):
        wx.Panel.__init__(self, parent, id)
        self._useSashMessageShown = False
        SPACE = 10
        HALF_SPACE = 5
        config = wx.ConfigBase_Get()
        self._projSaveDocsCheckBox = wx.CheckBox(self, -1, _("Remember open projects"))
        self._projSaveDocsCheckBox.SetValue(config.ReadInt("ProjectSaveDocs", True))
        projectBorderSizer = wx.BoxSizer(wx.VERTICAL)
        projectSizer = wx.BoxSizer(wx.VERTICAL)
        projectSizer.Add(self._projSaveDocsCheckBox, 0, wx.ALL, HALF_SPACE)
        self._projShowWelcomeCheckBox = wx.CheckBox(self, -1, _("Show Welcome Dialog"))
        self._projShowWelcomeCheckBox.SetValue(config.ReadInt("RunWelcomeDialog", True))
        projectSizer.Add(self._projShowWelcomeCheckBox, 0, wx.ALL, HALF_SPACE)        
        projectBorderSizer.Add(projectSizer, 0, wx.ALL, SPACE)
        self.SetSizer(projectBorderSizer)
        self.Layout()
        parent.AddPage(self, _("Project"))

    def OnUseSashSelect(self, event):
        if not self._useSashMessageShown:
            msgTitle = wx.GetApp().GetAppName()
            if not msgTitle:
                msgTitle = _("Document Options")
            wx.MessageBox("Project window embedded mode changes will not appear until the application is restarted.",
                          msgTitle,
                          wx.OK | wx.ICON_INFORMATION,
                          self.GetParent())
            self._useSashMessageShown = True


    def OnOK(self, optionsDialog):
        config = wx.ConfigBase_Get()
        config.WriteInt("ProjectSaveDocs", self._projSaveDocsCheckBox.GetValue())
        config.WriteInt("RunWelcomeDialog", self._projShowWelcomeCheckBox.GetValue())


class ProjectService(Service.Service):

    #----------------------------------------------------------------------------
    # Constants
    #----------------------------------------------------------------------------
    SHOW_WINDOW = wx.NewId()  # keep this line for each subclass, need unique ID for each Service
    RUNPM_ID = wx.NewId()
    RUN_SELECTED_PM_ID = wx.NewId()
    RUN_CURRENT_PM_ID = wx.NewId()
    ADD_FILES_TO_PROJECT_ID = wx.NewId()
    ADD_CURRENT_FILE_TO_PROJECT_ID = wx.NewId()
    RENAME_ID = wx.NewId()
    OPEN_SELECTION_ID = wx.NewId()
    REMOVE_FROM_PROJECT = wx.NewId()


    #----------------------------------------------------------------------------
    # Overridden methods
    #----------------------------------------------------------------------------

    def __init__(self, serviceName, embeddedWindowLocation = wx.lib.pydocview.EMBEDDED_WINDOW_LEFT):
        Service.Service.__init__(self, serviceName, embeddedWindowLocation)
        self._runHandlers = []
        self._suppressOpenProjectMessages = False


    def _CreateView(self):
        return ProjectView(self)


    def ShowWindow(self, show = True):
        """ Force showing of saved projects on opening, otherwise empty Project Window is disconcerting for user """
        Service.Service.ShowWindow(self, show)

        if show:
            project = self.GetView().GetDocument()
            if not project:
                self.OpenSavedProjects()


    #----------------------------------------------------------------------------
    # Service specific methods
    #----------------------------------------------------------------------------

    def GetSuppressOpenProjectMessages(self):
        return self._suppressOpenProjectMessages


    def SetSuppressOpenProjectMessages(self, suppressOpenProjectMessages):
        self._suppressOpenProjectMessages = suppressOpenProjectMessages


    def GetRunHandlers(self):
        return self._runHandlers


    def AddRunHandler(self, runHandler):
        self._runHandlers.append(runHandler)


    def RemoveRunHandler(self, runHandler):
        self._runHandlers.remove(runHandler)


    def InstallControls(self, frame, menuBar = None, toolBar = None, statusBar = None, document = None):
        Service.Service.InstallControls(self, frame, menuBar, toolBar, statusBar, document)

        config = wx.ConfigBase_Get()

        projectMenu = wx.Menu()

##            accelTable = wx.AcceleratorTable([
##                eval(_("wx.ACCEL_CTRL, ord('R'), ProjectService.RUN_ID"))
##                ])
##            frame.SetAcceleratorTable(accelTable)
        isProjectDocument = document and document.GetDocumentTemplate().GetDocumentType() == ProjectDocument
        if wx.GetApp().IsMDI() or isProjectDocument:
            if not menuBar.FindItemById(ProjectService.ADD_FILES_TO_PROJECT_ID):
                projectMenu.Append(ProjectService.ADD_FILES_TO_PROJECT_ID, _("&Add Files to Project..."), _("Adds a document to the current project"))
                wx.EVT_MENU(frame, ProjectService.ADD_FILES_TO_PROJECT_ID, frame.ProcessEvent)
                wx.EVT_UPDATE_UI(frame, ProjectService.ADD_FILES_TO_PROJECT_ID, frame.ProcessUpdateUIEvent)
            if not menuBar.FindItemById(ProjectService.ADD_CURRENT_FILE_TO_PROJECT_ID):
                projectMenu.Append(ProjectService.ADD_CURRENT_FILE_TO_PROJECT_ID, _("&Add Active File to Project..."), _("Adds the active document to a project"))
                wx.EVT_MENU(frame, ProjectService.ADD_CURRENT_FILE_TO_PROJECT_ID, frame.ProcessEvent)
                wx.EVT_UPDATE_UI(frame, ProjectService.ADD_CURRENT_FILE_TO_PROJECT_ID, frame.ProcessUpdateUIEvent)
        viewMenuIndex = menuBar.FindMenu(_("&View"))
        menuBar.Insert(viewMenuIndex + 1, projectMenu, _("&Project"))
        editMenu = menuBar.GetMenu(menuBar.FindMenu(_("&Edit")))
        if not menuBar.FindItemById(ProjectService.RENAME_ID):
            editMenu.AppendSeparator()
            editMenu.Append(ProjectService.RENAME_ID, _("&Rename"), _("Renames the active item"))
            wx.EVT_MENU(frame, ProjectService.RENAME_ID, frame.ProcessEvent)
            wx.EVT_UPDATE_UI(frame, ProjectService.RENAME_ID, frame.ProcessUpdateUIEvent)

        return True


    def OnCloseFrame(self, event):
        if not self.GetView():
            return True

        if wx.GetApp().IsMDI():
            # close all non-project documents first
            for document in self.GetDocumentManager().GetDocuments()[:]:  # Cloning list to make sure we go through all docs even as they are deleted
                if document.GetDocumentTemplate().GetDocumentType() != ProjectDocument:
                    if not self.GetDocumentManager().CloseDocument(document, False):
                        return False

            # write project config afterwards because user may change filenames on closing of new documents
            self.GetView().WriteProjectConfig()  # Called onCloseWindow in all of the other services but needed to be factored out for ProjectService since it is called elsewhere

            # close all project documents after closing other documents
            # because user may save a new document with a new name or cancel closing a document
            for document in self.GetDocumentManager().GetDocuments()[:]:  # Cloning list to make sure we go through all docs even as they are deleted
                if document.GetDocumentTemplate().GetDocumentType() == ProjectDocument:
                    if not document.OnSaveModified():
                        return False

        # This is called when any SDI frame is closed, so need to check if message window is closing or some other window
        elif self.GetView() == event.GetEventObject().GetView():
            self.SetView(None)
        return True


    #----------------------------------------------------------------------------
    # Event Processing Methods
    #----------------------------------------------------------------------------

    def ProcessEventBeforeWindows(self, event):
        id = event.GetId()
        if id == wx.ID_CLOSE_ALL:
            self.OnFileCloseAll(event)
            return True
        return False


    def ProcessEvent(self, event):
        if Service.Service.ProcessEvent(self, event):
            return True

        id = event.GetId()
        if id == ProjectService.RUN_SELECTED_PM_ID:
            self.OnRunProcessModel(event, runSelected=True)
            return True
        elif id == ProjectService.RUN_CURRENT_PM_ID:
            self.OnRunProcessModel(event, runCurrentFile=True)
            return True
        elif id == ProjectService.ADD_CURRENT_FILE_TO_PROJECT_ID:
            self.OnAddCurrentFileToProject(event)
            return True
        elif id == wx.lib.pydocview.FilePropertiesService.PROPERTIES_ID:
            if self.GetView():
                return self.GetView().ProcessEvent(event)
            else:
                return False
        else:
            return False


    def ProcessUpdateUIEvent(self, event):
        if Service.Service.ProcessUpdateUIEvent(self, event):
            return True

        id = event.GetId()
        if id == ProjectService.RUNPM_ID or id == ProjectService.RUN_SELECTED_PM_ID or id == ProjectService.RUN_CURRENT_PM_ID:
            event.Enable(self._HasOpenedProjects() and self._HasProcessModel())
            return True
        elif id == ProjectService.ADD_FILES_TO_PROJECT_ID:
            event.Enable(False)
            return True
        elif id == ProjectService.ADD_CURRENT_FILE_TO_PROJECT_ID:
            event.Enable(self._CanAddCurrentFileToProject())
            return True
        elif id == ProjectService.RENAME_ID:
            event.Enable(False)
            return True
        elif id == ProjectService.OPEN_SELECTION_ID:
            event.Enable(False)
            return True
        elif id == wx.lib.pydocview.FilePropertiesService.PROPERTIES_ID:
            if self.GetView():
                return self.GetView().ProcessUpdateUIEvent(event)
            else:
                return False
        else:
            return False


    def OnRunProcessModel(self, event, runSelected=False, runCurrentFile=False):
        project = self.GetView().GetDocument()

        if project:
            ext = None
            for template in self.GetDocumentManager().GetTemplates():
                if template.GetDocumentType() == ProcessModelEditor.ProcessModelDocument:
                    ext = template.GetDefaultExtension()
                    break;
            if not ext:
                return

            files = filter(lambda f: f.endswith(ext), project.GetFiles())
            if not files:
                return

            docs = wx.GetApp().GetDocumentManager().GetDocuments()
            for doc in docs:
                if doc.GetFilename() in files and doc.GetDocumentTemplate().GetDocumentType() == ProcessModelEditor.ProcessModelDocument:
                    if not doc.GetProcessModel().beginProcess:
                        wx.MessageBox(_("Cannot run process.  No begin action found."), _("Run Process"))
                        return

            filesModified = False
            for doc in docs:
                if doc.IsModified():
                    filesModified = True
                    break
            if filesModified:
                frame = self.GetView().GetFrame()                
                yesNoMsg = wx.MessageDialog(frame,
                              _("Files have been modified.  Process may not reflect your current changes.\n\nWould you like to save all files before running?"),
                              _("Run Process"),
                              wx.YES_NO
                              )
                if yesNoMsg.ShowModal() == wx.ID_YES:
                    wx.GetTopLevelParent(frame).OnFileSaveAll(None)
            
            if runCurrentFile:
                fileToRun = self.GetDocumentManager().GetCurrentDocument().GetFilename()
            elif runSelected:
                fileToRun = self.GetView().GetSelectedFile()
            elif len(files) > 1:
                files.sort(lambda a, b: cmp(os.path.basename(a).lower(), os.path.basename(b).lower()))
                strings = map(lambda file: os.path.basename(file), files)
                res = wx.GetSingleChoiceIndex(_("Select a process to run:"),
                                              _("Run"),
                                              strings,
                                              project.GetFirstView()._GetParentFrame())
                if res == -1:
                    return
                fileToRun = files[res]
            else:
                fileToRun = files[0]

            self.RunProcessModel(fileToRun)
            

    def RunProcessModel(self, fileToRun):
        for runHandler in self.GetRunHandlers():
            if runHandler.RunProjectFile(fileToRun):
                return
        os.system('"' + fileToRun + '"')


    def _HasProcessModel(self):
        project = self.GetView().GetDocument()

        if project:
            ext = None
            for template in self.GetDocumentManager().GetTemplates():
                if template.GetDocumentType() == ProcessModelEditor.ProcessModelDocument:
                    ext = template.GetDefaultExtension()
                    break;
            if not ext:
                return False

            files = filter(lambda f: f.endswith(ext), project.GetFiles())
            if not files:
                return False

            if len(files):
                return True

        return False


    def _HasOpenedProjects(self):
        for document in self.GetDocumentManager().GetDocuments():
            if document.GetDocumentTemplate().GetDocumentType() == ProjectDocument:
                return True
        return False


    def _HasCurrentFile(self):
        currentDoc = self.GetDocumentManager().GetCurrentDocument()
        return currentDoc
       
 
    def _CanAddCurrentFileToProject(self):
        currentDoc = self.GetDocumentManager().GetCurrentDocument()
        if not currentDoc:
            return False
        if currentDoc.GetDocumentTemplate().GetDocumentType() == ProjectDocument:
            return False
        if not currentDoc._savedYet:
            return False
        for document in self.GetDocumentManager().GetDocuments():
            if document.GetDocumentTemplate().GetDocumentType() == ProjectDocument:
                return True
        return False  # There are no documents open


    def GetFilesFromCurrentProject(self):
        view = self.GetView()
        if view:
            project = view.GetDocument()
            if project:
                return project.GetFiles()
        return None


    def GetCurrentProject(self):
        view = self.GetView()
        if view:
            return view.GetDocument()
        return None
       

    def FindProjectByFile(self, filename):
        for document in self.GetDocumentManager().GetDocuments():
            if document.GetDocumentTemplate().GetDocumentType() == ProjectDocument:
                if document.GetFilename() == filename:
                    return document
                elif document.IsFileInProject(filename):
                    return document
        return None
        

    def GetCurrentProjectNames(self):
        projects = []
        for document in self.GetDocumentManager().GetDocuments():
            if document.GetDocumentTemplate().GetDocumentType() == ProjectDocument:
                projects.append(document)
        if not projects:
            return
        projects.sort(lambda a, b: cmp(a.GetPrintableName().lower(), b.GetPrintableName().lower()))
        strings = map(lambda project: project.GetPrintableName(), projects)
        return strings
        
    def OnAddCurrentFileToProject(self, event):
        if not self._CanAddCurrentFileToProject():
            return
        projects = []
        for document in self.GetDocumentManager().GetDocuments():
            if document.GetDocumentTemplate().GetDocumentType() == ProjectDocument:
                projects.append(document)
        if not projects:
            return
        projects.sort(lambda a, b: cmp(a.GetPrintableName().lower(), b.GetPrintableName().lower()))
        strings = map(lambda project: project.GetPrintableName(), projects)
        res = wx.GetSingleChoiceIndex(_("Select a project to add the file to:"),
                                      _("Add to Project"),
                                      strings,
                                      self.GetDocumentManager().FindSuitableParent())
        if res == -1:
            return
        file = self.GetDocumentManager().GetCurrentDocument().GetFilename()
        projects[res].GetCommandProcessor().Submit(ProjectAddFilesCommand(projects[res], [file]))
        self.GetView().Activate(True)  # after add, should put focus on project editor


    def OnFileCloseAll(self, event):
        for document in self.GetDocumentManager().GetDocuments()[:]:  # Cloning list to make sure we go through all docs even as they are deleted
            if document.GetDocumentTemplate().GetDocumentType() != ProjectDocument:
                if not self.GetDocumentManager().CloseDocument(document, False):
                    return
                # document.DeleteAllViews() # Implicitly delete the document when the last view is removed


    def OpenSavedProjects(self):
        config = wx.ConfigBase_Get()
        openedDocs = False
        if config.ReadInt("ProjectSaveDocs", True):
            docString = config.Read("ProjectSavedDocs")
            if docString:
                doc = None
                for fileName in eval(docString):
                    if isinstance(fileName, types.StringTypes):
                        if os.path.exists(fileName):
                            doc = self.GetDocumentManager().CreateDocument(fileName, wx.lib.docview.DOC_SILENT)

                if doc:
                    openedDocs = True
                    expandedString = config.Read("ProjectExpandedSavedDocs")
                    if expandedString:
                        view = doc.GetFirstView()
                        view.SetExpandedProjects(eval(expandedString))
        return openedDocs

#----------------------------------------------------------------------------
# Icon Bitmaps - generated by encode_bitmaps.py
#----------------------------------------------------------------------------
from wx import ImageFromStream, BitmapFromImage
from wx import EmptyIcon
import cStringIO


def getProjectData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x00[IDAT8\x8d\xc5\x93\xc1\n\xc00\x08C\x8d\xf6\xff\xffX\xb3Sa-\xf6`;:O\n\
\x12\x1fj\x0059\t\xed\t\xc3\xc9pn\x0b\x88\x88@\rU\x81\xf6.\x18N\xa8aE\x92\rh\
YC\x85\xa4D\x90\x91\xdc%\xf8w\x07+\xd1\xfbW\x98\xc5\x8f\t\x86W\xee\x93+\xbe\
\xc0gn\xdc\x8d\x07\xab"<iG\x8e\xa9\r\x00\x00\x00\x00IEND\xaeB`\x82' 

def getProjectBitmap():
    return BitmapFromImage(getProjectImage())

def getProjectImage():
    stream = cStringIO.StringIO(getProjectData())
    return ImageFromStream(stream)

def getProjectIcon():
    icon = EmptyIcon()
    icon.CopyFromBitmap(getProjectBitmap())
    return icon
    

#----------------------------------------------------------------------------

def getBlankData():
    return \
"\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00 \x00\x00\x00 \x08\x06\x00\
\x00\x00szz\xf4\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\x00\x00\
\x85IDATX\x85\xed\x97\xc9\n\xc0 \x0cD3\xda\xff\xffcMo\x96Z\xc4\xa5\x91\x14:9\
\x8a\xe8\xcb\xd3\xb8\x00!\x8ag\x04\xd7\xd9E\xe4\xa8\x1b4'}3 B\xc4L\x7fs\x03\
\xb3\t<\x0c\x94\x81tN\x04p%\xae9\xe9\xa8\x89m{`\xd4\x84\xfd\x12\xa8\x16{#\
\x10\xdb\xab\xa0\x07a\x0e\x00\xe0\xb6\x1fz\x10\xdf;\x07V\xa3U5\xb5\x8d:\xdc\
\r\x10\x80\x00\x04 \x00\x01\x08@\x80\xe6{\xa0w\x8f[\x85\xbb\x01\xfc\xfeoH\
\x80\x13>\xf9(3zH\x1e\xfb\x00\x00\x00\x00IEND\xaeB`\x82" 


def getBlankBitmap():
    return BitmapFromImage(getBlankImage())

def getBlankImage():
    stream = cStringIO.StringIO(getBlankData())
    return ImageFromStream(stream)

def getBlankIcon():
    icon = EmptyIcon()
    icon.CopyFromBitmap(getBlankBitmap())
    return icon
    
