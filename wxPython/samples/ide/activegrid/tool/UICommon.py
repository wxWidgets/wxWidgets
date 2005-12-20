#----------------------------------------------------------------------------
# Name:         UICommon.py
# Purpose:      Shared UI stuff
#
# Author:       Matt Fryer, Morgan Hua
#
# Created:      3/10/05
# CVS-ID:       $Id$
# Copyright:    (c) 2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import os
import os.path
import wx
import string
import ProjectEditor
import activegrid.util.sysutils as sysutils
import activegrid.util.strutils as strutils
import activegrid.util.appdirs as appdirs
_ = wx.GetTranslation

def CreateDirectoryControl( parent, fileLabel=_("File Name:"), dirLabel=_("Directory"), fileExtension="*", startingName="", startingDirectory=None, choiceDirs=None, appDirDefaultStartDir=False, returnAll=False):

    if not choiceDirs:
        choiceDirs = []
        projectDirs = []

        if appDirDefaultStartDir:
            appDirectory = wx.ConfigBase_Get().Read(ProjectEditor.PROJECT_DIRECTORY_KEY, ProjectEditor.NEW_PROJECT_DIRECTORY_DEFAULT)
        else:
            appDirectory = wx.ConfigBase_Get().Read(ProjectEditor.PROJECT_DIRECTORY_KEY)
        if appDirectory:
            choiceDirs.append(appDirectory)
            if appDirDefaultStartDir and not startingDirectory:
                startingDirectory = appDirectory

        projectService = wx.GetApp().GetService(ProjectEditor.ProjectService)
        if projectService:
            curProjectDoc = projectService.GetCurrentProject()
            if curProjectDoc:
                homeDir = curProjectDoc.GetAppDocMgr().homeDir
                if homeDir and (homeDir not in choiceDirs):
                    choiceDirs.append(homeDir)
                if not startingDirectory:
                    startingDirectory = homeDir
                    
            for projectDoc in projectService.GetOpenProjects():
                if projectDoc == curProjectDoc:
                    continue
                homeDir = projectDoc.GetAppDocMgr().homeDir
                if homeDir and (homeDir not in projectDirs):
                    projectDirs.append(homeDir)
                projectDirs.sort(CaseInsensitiveCompare)
            for projectDir in projectDirs:
                if projectDir not in choiceDirs:
                    choiceDirs.append(projectDir)

        if startingDirectory and (startingDirectory not in choiceDirs):
            choiceDirs.insert(0, startingDirectory)
            
        if os.getcwd() not in choiceDirs:
            choiceDirs.append(os.getcwd())                
        if appdirs.documents_folder not in choiceDirs:
            choiceDirs.append(appdirs.documents_folder) 

    if not startingDirectory:
        startingDirectory = os.getcwd()
        
    nameControl = wx.TextCtrl(parent, -1, startingName, size=(-1,-1))
    nameLabelText = wx.StaticText(parent, -1, fileLabel)
    dirLabelText = wx.StaticText(parent, -1, dirLabel)
    dirControl = wx.ComboBox(parent, -1, startingDirectory, size=(-1,-1), choices=choiceDirs)
    dirControl.SetToolTipString(startingDirectory)
    button = wx.Button(parent, -1, _("Browse..."))
    allControls = [nameControl, nameLabelText, dirLabelText, dirControl, button]
    
    def OnFindDirClick(event): 
        name = ""  
        nameCtrlValue = nameControl.GetValue()
        if nameCtrlValue:
            root, ext = os.path.splitext( nameCtrlValue )
            if ext == '.' + fileExtension:
                name = nameCtrlValue
            else:
                name = _("%s.%s") % (nameCtrlValue, fileExtension)
                
        dlg = wx.FileDialog(parent, _("Choose a filename and directory"),
                       defaultDir = dirControl.GetValue().strip(),
                       defaultFile = name,
                       wildcard= "*.%s" % fileExtension,
                       style=wx.SAVE|wx.CHANGE_DIR)

        if dlg.ShowModal() != wx.ID_OK:
            dlg.Destroy()
            return
        path = dlg.GetPath()
        dlg.Destroy()
        
        if path:
            dir, filename = os.path.split(path)
            if dirControl.FindString(dir) == wx.NOT_FOUND:
                dirControl.Insert(dir, 0)
            dirControl.SetValue(dir)
            dirControl.SetToolTipString(dir)
            nameControl.SetValue(filename)

    parent.Bind(wx.EVT_BUTTON, OnFindDirClick, button)
    
    def Validate(allowOverwriteOnPrompt=False, infoString='', noFirstCharDigit=False):
        projName = nameControl.GetValue().strip()
        if projName == "":
            wx.MessageBox(_("Please provide a %sfile name.") % infoString, _("Provide a File Name"))            
            return False
        if noFirstCharDigit and projName[0].isdigit():
            wx.MessageBox(_("File name cannot start with a number.  Please enter a different name."), _("Invalid File Name"))            
            return False
        if projName.find(' ') != -1:
            wx.MessageBox(_("Please provide a %sfile name that does not contains spaces.") % infoString, _("Spaces in File Name"))            
            return False
        if not os.path.exists(dirControl.GetValue()):
            wx.MessageBox(_("That %sdirectory does not exist. Please choose an existing directory.") % infoString, _("Provide a Valid Directory"))            
            return False
        
        filePath = os.path.join(dirControl.GetValue(), MakeNameEndInExtension(projName, "." + fileExtension))
        if os.path.exists(filePath):
            if allowOverwriteOnPrompt:
                res = wx.MessageBox(_("That %sfile already exists. Would you like to overwrite it.") % infoString, "File Exists", style=wx.YES_NO|wx.NO_DEFAULT)
                return (res == wx.YES)  
            else:                
                wx.MessageBox(_("That %sfile already exists. Please choose a different name.") % infoString, "File Exists")
                return False                  
        return True    
    HALF_SPACE = 5
    flexGridSizer = wx.FlexGridSizer(cols = 3, vgap = HALF_SPACE, hgap = HALF_SPACE)
    flexGridSizer.AddGrowableCol(1,1)
    flexGridSizer.Add(nameLabelText, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_LEFT)
    flexGridSizer.Add(nameControl, 2, flag=wx.ALIGN_CENTER_VERTICAL|wx.EXPAND)
    flexGridSizer.Add(button, flag=wx.ALIGN_RIGHT|wx.LEFT, border=HALF_SPACE)
        
    flexGridSizer.Add(dirLabelText, flag=wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_LEFT)
    flexGridSizer.Add(dirControl, 2, flag=wx.ALIGN_CENTER_VERTICAL|wx.EXPAND)
    flexGridSizer.Add(wx.StaticText(parent, -1, ""), 0)
    if returnAll:
        return nameControl, dirControl, flexGridSizer, Validate, allControls
    else:
        return nameControl, dirControl, flexGridSizer, Validate


def CreateDirectoryOnlyControl( parent, dirLabel=_("Location:"), startingDirectory=None, choiceDirs=None, appDirDefaultStartDir=False):

    if not choiceDirs:
        choiceDirs = []
        projectDirs = []

        if appDirDefaultStartDir:
            appDirectory = wx.ConfigBase_Get().Read(ProjectEditor.PROJECT_DIRECTORY_KEY, ProjectEditor.NEW_PROJECT_DIRECTORY_DEFAULT)
        else:
            appDirectory = wx.ConfigBase_Get().Read(ProjectEditor.PROJECT_DIRECTORY_KEY)
        if appDirectory:
            choiceDirs.append(appDirectory)
            if appDirDefaultStartDir and not startingDirectory:
                startingDirectory = appDirectory

        projectService = wx.GetApp().GetService(ProjectEditor.ProjectService)
        if projectService:
            curProjectDoc = projectService.GetCurrentProject()
            if curProjectDoc:
                homeDir = curProjectDoc.GetAppDocMgr().homeDir
                if homeDir and (homeDir not in choiceDirs):
                    choiceDirs.append(homeDir)
                if not startingDirectory:
                    startingDirectory = homeDir
                    
            for projectDoc in projectService.GetOpenProjects():
                if projectDoc == curProjectDoc:
                    continue
                homeDir = projectDoc.GetAppDocMgr().homeDir
                if homeDir and (homeDir not in projectDirs):
                    projectDirs.append(homeDir)
                projectDirs.sort(CaseInsensitiveCompare)
            for projectDir in projectDirs:
                if projectDir not in choiceDirs:
                    choiceDirs.append(projectDir)

        if startingDirectory and (startingDirectory not in choiceDirs):
            choiceDirs.insert(0, startingDirectory)

        if os.getcwd() not in choiceDirs:
            choiceDirs.append(os.getcwd())                
        if appdirs.documents_folder not in choiceDirs:
            choiceDirs.append(appdirs.documents_folder)                
            

    if not startingDirectory:
        startingDirectory = os.getcwd()
            
    dirLabelText = wx.StaticText(parent, -1, dirLabel)
    dirControl = wx.ComboBox(parent, -1, startingDirectory, size=(-1,-1), choices=choiceDirs)
    dirControl.SetToolTipString(startingDirectory)
    button = wx.Button(parent, -1, _("Browse..."))
    
    def OnFindDirClick(event): 
            dlg = wx.DirDialog(wx.GetApp().GetTopWindow(),
                                _("Choose a directory:"),
                                defaultPath=dirControl.GetValue().strip(),
                                style=wx.DD_DEFAULT_STYLE|wx.DD_NEW_DIR_BUTTON)
            dlg.CenterOnParent()
            if dlg.ShowModal() == wx.ID_OK:
                dir = dlg.GetPath()
                if dirControl.FindString(dir) == wx.NOT_FOUND:
                    dirControl.Insert(dir, 0)
                dirControl.SetValue(dir)
                dirControl.SetToolTipString(dir)
            dlg.Destroy()            

    parent.Bind(wx.EVT_BUTTON, OnFindDirClick, button)
    
    def Validate(allowOverwriteOnPrompt=False):
        dirName = dirControl.GetValue().strip()
        if dirName == "":
            wx.MessageBox(_("Please provide a directory."), _("Provide a Directory"))            
            return False
        if not os.path.exists(dirName):
            wx.MessageBox(_("That directory does not exist. Please choose an existing directory."), _("Provide a Valid Directory"))            
            return False
        return True  
          
    HALF_SPACE = 5
    flexGridSizer = wx.FlexGridSizer(cols = 3, vgap = HALF_SPACE, hgap = HALF_SPACE)
    flexGridSizer.AddGrowableCol(1,1)
    flexGridSizer.Add(dirLabelText, flag=wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_LEFT|wx.RIGHT, border=HALF_SPACE)
    flexGridSizer.Add(dirControl, 2, flag=wx.ALIGN_CENTER_VERTICAL|wx.EXPAND, border=HALF_SPACE)
    flexGridSizer.Add(button, flag=wx.ALIGN_RIGHT|wx.LEFT, border=HALF_SPACE)
        
    return dirControl, flexGridSizer, Validate


def CreateNameOnlyControl( parent, fileLabel, startingName="", startingDirectoryControl=None):

    fileLabelText = wx.StaticText(parent, -1, fileLabel)
    nameControl = wx.TextCtrl(parent, -1, startingName, size=(-1,-1))
        
    def Validate(allowOverwriteOnPrompt=False, noFirstCharDigit=False):
        projName = nameControl.GetValue().strip()
        if projName == "":
            wx.MessageBox(_("Blank name.  Please enter a valid name."), _("Project Name"))            
            return False
        if noFirstCharDigit and projName[0].isdigit():
            wx.MessageBox(_("Name cannot start with a number.  Please enter a valid name."), _("Project Name"))            
            return False
        if projName.find(' ') != -1:
            wx.MessageBox(_("Spaces in name.  Name cannot have spaces.") % infoString, _("Project Name"))            
            return False
        path = os.path.join(startingDirectoryControl.GetValue().strip(), projName)
        if os.path.exists(path):
            if os.path.isdir(path):
                message = _("Project '%s' already exists.  Would you like to overwrite the contents of the project?") % projName
            else: # os.path.isfile(path):
                message = _("'%s' already exists as a file.  Would you like to replace it with the project?") % nameControl.GetValue().strip()
                    
            yesNoMsg = wx.MessageDialog(wx.GetApp().GetTopWindow(),
                          message,
                          _("Project Directory Exists"),
                          wx.YES_NO|wx.ICON_QUESTION
                          )
            yesNoMsg.CenterOnParent()
            status = yesNoMsg.ShowModal()
            yesNoMsg.Destroy()
            if status == wx.ID_NO:
                return False
        return True  
          
    HALF_SPACE = 5
    flexGridSizer = wx.FlexGridSizer(cols = 2, vgap = HALF_SPACE, hgap = HALF_SPACE)
    flexGridSizer.AddGrowableCol(1,1)
    flexGridSizer.Add(fileLabelText, flag=wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_LEFT|wx.TOP|wx.RIGHT, border=HALF_SPACE)
    flexGridSizer.Add(nameControl, 2, flag=wx.ALIGN_CENTER_VERTICAL|wx.EXPAND, border=HALF_SPACE)
        
    return nameControl, flexGridSizer, Validate
    

def GetCurrentProject():
    projectDocument = None
    projectService = wx.GetApp().GetService(ProjectEditor.ProjectService)
    if projectService:
        projectDocument = projectService.GetCurrentProject()
    return projectDocument

def AddFilesToCurrentProject(paths, folderPath=None, types=None, names=None, save=False):
    projectService = wx.GetApp().GetService(ProjectEditor.ProjectService)
    if projectService:
        projectDocument = projectService.GetCurrentProject()
        if projectDocument:
            files = projectDocument.GetFiles()
            for path in paths:
                if path in files:
                    paths.remove(path)
            if paths:
                projectDocument.GetCommandProcessor().Submit(ProjectEditor.ProjectAddFilesCommand(projectDocument, paths, folderPath=folderPath, types=types, names=names))
                if save:
                    projectDocument.OnSaveDocument(projectDocument.GetFilename())

def AddFilesToProject(projectDocument, paths, types=None, names=None, save=False):
    if projectDocument:
        files = projectDocument.GetFiles()
        for path in paths:
            if path in files:
                paths.remove(path)
        if paths:
            projectDocument.GetCommandProcessor().Submit(ProjectEditor.ProjectAddFilesCommand(projectDocument, paths, types=types, names=names))
            if save:
                projectDocument.OnSaveDocument(projectDocument.GetFilename())


def MakeNameEndInExtension(name, extension):
    if not name:
        return name
    root, ext = os.path.splitext(name)
    if ext == extension:
        return name
    else:
        return name + extension
 

def GetPythonExecPath():
    pythonExecPath = wx.ConfigBase_Get().Read("ActiveGridPythonLocation")
    if not pythonExecPath:
        pythonExecPath = sysutils.pythonExecPath
    return pythonExecPath
    

def _DoRemoveRecursive(path, skipFile=None, skipped=False):
    if path == skipFile:
        skipped = True
    elif os.path.isdir(path):
        for file in os.listdir(path):
            file_or_dir = os.path.join(path,file)
            if skipFile == file_or_dir:
                skipped = True
            elif os.path.isdir(file_or_dir) and not os.path.islink(file_or_dir):
                if _DoRemoveRecursive(file_or_dir, skipFile): # it's a directory recursive call to function again
                    skipped = True
            else:
                os.remove(file_or_dir) # it's a file, delete it
        if not skipped:
            os.rmdir(path) # delete the directory here
    else:
        os.remove(path)
        
    return skipped


def RemoveRecursive(path, skipFile=None):
    _DoRemoveRecursive(path, skipFile)    


def CaseInsensitiveCompare(s1, s2):
    """ Method used by sort() to sort values in case insensitive order """
    return strutils.caseInsensitiveCompare(s1, s2)


def GetAnnotation(model, elementName):
    """ Get an object's annotation used for tooltips """
    if hasattr(model, "__xsdcomplextype__"):
        ct = model.__xsdcomplextype__
        if ct:
            el = ct.findElement(elementName)
            if el and el.annotation:
                return el.annotation
            
    return ""


#----------------------------------------------------------------------------
# Methods for finding application level info
#----------------------------------------------------------------------------

def GetProjectForDoc(doc):
    """ Given a document find which project it belongs to.
        Tries to intelligently resolve conflicts if it is in more than one open project.
    """
    projectService = wx.GetApp().GetService(ProjectEditor.ProjectService)

    projectDoc = projectService.FindProjectFromMapping(doc)
    if projectDoc:
        return projectDoc

    projectDoc = projectService.GetCurrentProject()
    if not projectDoc:
        return None
    if projectDoc.IsFileInProject(doc.GetFilename()):
        return projectDoc

    projects = []
    openDocs = wx.GetApp().GetDocumentManager().GetDocuments()
    for openDoc in openDocs:
        if openDoc == projectDoc:
            continue
        if(isinstance(openDoc, ProjectEditor.ProjectDocument)):
            if openDoc.IsFileInProject(doc.GetFilename()):
                projects.append(openDoc)
                
    if projects:
        if len(projects) == 1:
            return projects[0]
        else:
            choices = [os.path.basename(project.GetFilename()) for project in projects]
            dlg = wx.SingleChoiceDialog(wx.GetApp().GetTopWindow(), _("'%s' found in more than one project.\nWhich project should be used for this operation?") % os.path.basename(doc.GetFilename()), _("Select Project"), choices, wx.DEFAULT_DIALOG_STYLE|wx.RESIZE_BORDER|wx.OK|wx.CENTRE)
            dlg.CenterOnParent()
            projectDoc = None
            if dlg.ShowModal() == wx.ID_OK:
                i = dlg.GetSelection()
                projectDoc = projects[i]
            dlg.Destroy()
            return projectDoc

    return None


def GetAppInfoForDoc(doc):
    """ Get the AppInfo for a given document """
    projectDoc = GetProjectForDoc(doc)
    if projectDoc:
        return projectDoc.GetAppInfo()
    return None


def GetAppDocMgrForDoc(doc):
    """ Get the AppDocMgr for a given document """
    projectDoc = GetProjectForDoc(doc)
    if projectDoc:
        return projectDoc.GetModel()
    return None


def GetAppInfoLanguage(doc=None):
    from activegrid.server.deployment import LANGUAGE_DEFAULT
    if doc:
        language = doc.GetAppInfo().language
    else:
        language = None
        
    if not language:
        config = wx.ConfigBase_Get()
        language = config.Read(ProjectEditor.APP_LAST_LANGUAGE, LANGUAGE_DEFAULT)
        
        if doc:
            doc.GetAppInfo().language = language  # once it is selected, it must be set.
        
    return language
