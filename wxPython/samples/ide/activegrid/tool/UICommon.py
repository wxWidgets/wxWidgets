#----------------------------------------------------------------------------
# Name:         UICommon.py
# Purpose:      Shared UI stuff
#
# Author:       Matt Fryer
#
# Created:      3/10/05
# CVS-ID:       $Id$
# Copyright:    (c) 2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import os
import os.path
import wx
import ProjectEditor
_ = wx.GetTranslation

def CreateDirectoryControl( parent, fileLabel, dirLabel, fileExtension, startingName="", startingDirectory=""):

    nameControl = wx.TextCtrl(parent, -1, startingName, size=(-1,-1))
    nameLabelText = wx.StaticText(parent, -1, fileLabel)
    dirLabelText = wx.StaticText(parent, -1, dirLabel)
    dirControl = wx.TextCtrl(parent, -1, startingDirectory, size=(-1,-1))
    dirControl.SetToolTipString(startingDirectory)
    button = wx.Button(parent, -1, _("Browse..."), size=(60,-1))
    
    def OnFindDirClick(event): 
        name = ""  
        nameCtrlValue = nameControl.GetValue()
        if nameCtrlValue:
            root, ext = os.path.splitext( nameCtrlValue )
            if ext == '.' + fileExtension:
                name = nameCtrlValue
            else:
                name = _("%s.%s") % (nameCtrlValue, fileExtension)
        path = wx.FileSelector(_("Choose a filename and directory"),
                       "",
                       "%s" % name,
                       wildcard=_("*.%s") % fileExtension ,
                       flags=wx.SAVE,
                       parent=parent)

        if path:
            dir, filename = os.path.split(path)
            dirControl.SetValue(dir)
            dirControl.SetToolTipString(dir)
            nameControl.SetValue(filename)

    parent.Bind(wx.EVT_BUTTON, OnFindDirClick, button)
    
    def Validate(allowOverwriteOnPrompt=False):
        if nameControl.GetValue() == "":
            wx.MessageBox(_("Please provide a filename."), _("Provide a Filename"))            
            return False
        if nameControl.GetValue().find(' ') != -1:
            wx.MessageBox(_("Please provide a filename that does not contains spaces."), _("Spaces in Filename"))            
            return False
        filePath = os.path.join(dirControl.GetValue(), MakeNameEndInExtension(nameControl.GetValue(), "." + fileExtension))
        if os.path.exists(filePath):
            if allowOverwriteOnPrompt:
                res = wx.MessageBox(_("That file already exists. Would you like to overwrite it."), "File Exists", style=wx.YES_NO|wx.NO_DEFAULT)
                return (res == wx.YES)  
            else:                
                wx.MessageBox(_("That file already exists. Please choose a different name."), "File Exists")
                return False                  
        return True    
    HALF_SPACE = 5
    flexGridSizer = wx.FlexGridSizer(cols = 3, vgap = HALF_SPACE, hgap = HALF_SPACE)
    flexGridSizer.AddGrowableCol(1,1)
    flexGridSizer.Add(nameLabelText, 0, wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_LEFT|wx.TOP|wx.RIGHT, HALF_SPACE)
    flexGridSizer.Add(nameControl, 2, flag=wx.ALIGN_CENTER_VERTICAL|wx.EXPAND)
    flexGridSizer.Add(button, flag=wx.ALIGN_RIGHT|wx.LEFT, border=HALF_SPACE)
        
    flexGridSizer.Add(dirLabelText, flag=wx.ALIGN_CENTER_VERTICAL|wx.ALIGN_LEFT|wx.TOP|wx.RIGHT, border=HALF_SPACE)
    flexGridSizer.Add(dirControl, 2, flag=wx.ALIGN_CENTER_VERTICAL|wx.EXPAND, border=HALF_SPACE)
    flexGridSizer.Add(wx.StaticText(parent, -1, ""), 0)
    return nameControl, dirControl, flexGridSizer, Validate

def AddFilesToCurrentProject(paths, save=False):
    projectService = wx.GetApp().GetService(ProjectEditor.ProjectService)
    if projectService:
        projectDocument = projectService.GetCurrentProject()
        if projectDocument:
            files = projectDocument.GetFiles()
            for path in paths:
                if path in files:
                    paths.remove(path)
            if paths:
                projectDocument.GetCommandProcessor().Submit(ProjectEditor.ProjectAddFilesCommand(projectDocument, paths))
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
 
# Lame
def PluralName(name):
    if not name:
        return name
    if name.endswith('us'):
        return name[0:-2] + 'ii'
    elif name.endswith('s'):
        return name
    elif name.endswith('y'):
        return name[0:-1] + 'ies'
    else:
        return name + 's'
           
