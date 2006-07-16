#----------------------------------------------------------------------------
# Name:         project.py
# Purpose:      project model for wx.lib.pydocview
#
# Author:       Morgan Hua
#
# Created:      8/25/05
# CVS-ID:       $Id$
# Copyright:    (c) 2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import copy
import os
import os.path
import activegrid.util.xmlutils as xmlutils
import activegrid.util.aglogging as aglogging

# REVIEW 07-Mar-06 stoens@activegrid.com -- Ideally move the pieces required
# to generate the .dpl file out of this module so there's no dependency on wx,
# instead of doing this try/catch (IDE drags in wx).
try:
    from IDE import ACTIVEGRID_BASE_IDE
except:
    ACTIVEGRID_BASE_IDE = False
    
if not ACTIVEGRID_BASE_IDE:
    import activegrid.model.basedocmgr as basedocmgr
    import AppInfo

#----------------------------------------------------------------------------
# Constants
#----------------------------------------------------------------------------
# Always add new versions, never edit the version number
# This allows you to upgrade the file by checking the version number
PROJECT_VERSION_050730 = '10'
PROJECT_VERSION_050826 = '11'


#----------------------------------------------------------------------------
# Classes
#----------------------------------------------------------------------------

class BaseProject(object):

    __xmlname__ = "project"
    __xmlexclude__ = ('fileName', '_projectDir', '_getDocCallback', '_cacheEnabled')
    __xmlattributes__ = ("_homeDir", "version")
    __xmlrename__ = { "_homeDir":"homeDir", "_appInfo":"appInfo" }
    __xmlflattensequence__ = { "_files":("file",) }
    __xmldefaultnamespace__ = xmlutils.AG_NS_URL
    __xmlattrnamespaces__ = { "ag": ["version", "_homeDir"] }


    def __init__(self):
        self.__xmlnamespaces__ = { "ag" : xmlutils.AG_NS_URL }
        self.version = PROJECT_VERSION_050826
        self._files = []
        self._projectDir = None  # default for homeDir, set on load
        self._homeDir = None         # user set homeDir for use in calculating relative path
        self._cacheEnabled = 0
        if not ACTIVEGRID_BASE_IDE:
            self._appInfo = AppInfo.AppInfo()


    def initialize(self):
        for file in self._files:
            file._parentProj = self


    def __copy__(self):
        clone = Project()
        clone._files = [copy.copy(file) for file in self._files]
        clone._projectDir = self._projectDir
        clone._homeDir = self._homeDir
        if not ACTIVEGRID_BASE_IDE:
            clone._appInfo = copy.copy(self._appInfo)
        return clone


    def GetAppInfo(self):
        return self._appInfo


    def AddFile(self, filePath=None, logicalFolder=None, type=None, name=None, file=None):
        """ Usage: self.AddFile(filePath, logicalFolder, type, name)  # used for initial generation of object
                   self.AddFile(file=xyzFile)  # normally used for redo/undo
            Add newly created file object using filePath and logicalFolder or given file object
        """
        if file:
            self._files.append(file)
        else:
            self._files.append(ProjectFile(self, filePath, logicalFolder, type, name, getDocCallback=self._getDocCallback))


    def RemoveFile(self, file):
        self._files.remove(file)


    def FindFile(self, filePath):
        if filePath:
            for file in self._files:
                if file.filePath == filePath:
                    return file

        return None


    def _GetFilePaths(self):
        return [file.filePath for file in self._files]


    filePaths = property(_GetFilePaths)

    def _GetProjectFiles(self):
        return self._files
    projectFiles = property(_GetProjectFiles)


    def _GetLogicalFolders(self):
        folders = []
        for file in self._files:
            if file.logicalFolder and file.logicalFolder not in folders:
                folders.append(file.logicalFolder)
        return folders


    logicalFolders = property(_GetLogicalFolders)


    def _GetPhysicalFolders(self):
        physicalFolders = []
        for file in self._files:
            physicalFolder = file.physicalFolder
            if physicalFolder and physicalFolder not in physicalFolders:
                physicalFolders.append(physicalFolder)
        return physicalFolders


    physicalFolders = property(_GetPhysicalFolders)


    def _GetHomeDir(self):
        if self._homeDir:
            return self._homeDir
        else:
            return self._projectDir


    def _SetHomeDir(self, parentPath):
        self._homeDir = parentPath


    def _IsDefaultHomeDir(self):
        return (self._homeDir == None)


    isDefaultHomeDir = property(_IsDefaultHomeDir)


    homeDir = property(_GetHomeDir, _SetHomeDir)


    def GetRelativeFolders(self):
        relativeFolders = []
        for file in self._files:
            relFolder = file.GetRelativeFolder(self.homeDir)
            if relFolder and relFolder not in relativeFolders:
                relativeFolders.append(relFolder)
        return relativeFolders


    def AbsToRelativePath(self):
        for file in self._files:
            file.AbsToRelativePath(self.homeDir)


    def RelativeToAbsPath(self):
        for file in self._files:
            file.RelativeToAbsPath(self.homeDir)


    def _SetCache(self, enable):
        """
            Only turn this on if your operation assumes files on disk won't change.
            Once your operation is done, turn this back off.
            Nested enables are allowed, only the last disable will disable the cache.
            
            This bypasses the IsDocumentModificationDateCorrect call because the modification date check is too costly, it hits the disk and takes too long.
        """
        if enable:
            if self._cacheEnabled == 0:
                # clear old cache, don't want to accidentally return stale value
                for file in self._files:
                    file.ClearCache()        

            self._cacheEnabled += 1
        else:
            self._cacheEnabled -= 1
            


    def _GetCache(self):
        return (self._cacheEnabled > 0)


    cacheEnabled = property(_GetCache, _SetCache)


    #----------------------------------------------------------------------------
    # BaseDocumentMgr methods
    #----------------------------------------------------------------------------


    def fullPath(self, fileName):
        fileName = super(BaseProject, self).fullPath(fileName)

        if os.path.isabs(fileName):
            absPath = fileName
        elif self.homeDir:
            absPath = os.path.join(self.homeDir, fileName)
        else:
            absPath = os.path.abspath(fileName)
        return os.path.normpath(absPath)


    def documentRefFactory(self, name, fileType, filePath):
        return ProjectFile(self, filePath=self.fullPath(filePath), type=fileType, name=name, getDocCallback=self._getDocCallback)


    def findAllRefs(self):
        return self._files


    def GetXFormsDirectory(self):
        forms = self.findRefsByFileType(basedocmgr.FILE_TYPE_XFORM)
        filePaths = map(lambda form: form.filePath, forms)
        xformdir = os.path.commonprefix(filePaths)
        if not xformdir:
            xformdir = self.homeDir
        return xformdir


    def setRefs(self, files):
        self._files = files


    def findRefsByFileType(self, fileType):
        fileList = []
        for file in self._files:
            if fileType == file.type:
                fileList.append(file)
        return fileList


    def GenerateServiceRefPath(self, wsdlFilePath):
        # HACK: temporary solution to getting wsdlag path from wsdl path.
        import wx
        from WsdlAgEditor import WsdlAgDocument
        ext = WsdlAgDocument.WSDL_AG_EXT
        for template in wx.GetApp().GetDocumentManager().GetTemplates():
            if template.GetDocumentType() == WsdlAgDocument:
                ext = template.GetDefaultExtension()
                break;
        wsdlAgFilePath = os.path.splitext(wsdlFilePath)[0] + ext
        return wsdlAgFilePath


    def SetDocCallback(self, getDocCallback):
        self._getDocCallback = getDocCallback
        for file in self._files:
            file._getDocCallback = getDocCallback


if ACTIVEGRID_BASE_IDE:
    class Project(BaseProject):
        pass
else:
    class Project(BaseProject, basedocmgr.BaseDocumentMgr):
        pass


class ProjectFile(object):
    __xmlname__ = "file"
    __xmlexclude__ = ('_parentProj', '_getDocCallback', '_docCallbackCacheReturnValue', '_docModelCallbackCacheReturnValue', '_doc',)
    __xmlattributes__ = ["filePath", "logicalFolder", "type", "name"]
    __xmldefaultnamespace__ = xmlutils.AG_NS_URL


    def __init__(self, parent=None, filePath=None, logicalFolder=None, type=None, name=None, getDocCallback=None):
        self._parentProj = parent
        self.filePath = filePath
        self.logicalFolder = logicalFolder
        self.type = type
        self.name = name
        self._getDocCallback = getDocCallback
        self._docCallbackCacheReturnValue = None
        self._docModelCallbackCacheReturnValue = None
        self._doc = None


    def _GetDocumentModel(self):
        if (self._docCallbackCacheReturnValue
        and (self._parentProj.cacheEnabled or self._docCallbackCacheReturnValue.IsDocumentModificationDateCorrect())):
            return self._docModelCallbackCacheReturnValue

        if self._getDocCallback:
            self._docCallbackCacheReturnValue, self._docModelCallbackCacheReturnValue = self._getDocCallback(self.filePath)
            return self._docModelCallbackCacheReturnValue

        return None


    document = property(_GetDocumentModel)


    def _GetDocument(self):
        # Return the IDE document wrapper that corresponds to the runtime document model
        if (self._docCallbackCacheReturnValue
        and (self._parentProj.cacheEnabled or self._docCallbackCacheReturnValue.IsDocumentModificationDateCorrect())):
            return self._docCallbackCacheReturnValue

        if self._getDocCallback:
            self._docCallbackCacheReturnValue, self._docModelCallbackCacheReturnValue = self._getDocCallback(self.filePath)
            return self._docCallbackCacheReturnValue

        return None


    ideDocument = property(_GetDocument)


    def ClearCache(self):
        self._docCallbackCacheReturnValue = None
        self._docModelCallbackCacheReturnValue = None


    def _typeEnumeration(self):
        return basedocmgr.FILE_TYPE_LIST


    def _GetPhysicalFolder(self):
        dir = None
        if self.filePath:
            dir = os.path.dirname(self.filePath)
            if os.sep != '/':
                dir = dir.replace(os.sep, '/')  # require '/' as delimiter
        return dir


    physicalFolder = property(_GetPhysicalFolder)


    def GetRelativeFolder(self, parentPath):
        parentPathLen = len(parentPath)

        dir = None
        if self.filePath:
            dir = os.path.dirname(self.filePath)
            if dir.startswith(parentPath + os.sep):
                dir = "." + dir[parentPathLen:]  # convert to relative path
            if os.sep != '/':
                dir = dir.replace(os.sep, '/')  # always save out with '/' as path separator for cross-platform compatibility.
        return dir


    def AbsToRelativePath(self, parentPath):
        """ Used to convert path to relative path for saving (disk format) """
        parentPathLen = len(parentPath)

        if self.filePath.startswith(parentPath + os.sep):
            self.filePath = "." + self.filePath[parentPathLen:]  # convert to relative path
            if os.sep != '/':
                self.filePath = self.filePath.replace(os.sep, '/')  # always save out with '/' as path separator for cross-platform compatibility.
        else:
            pass    # not a decendant of project, use absolute path


    def RelativeToAbsPath(self, parentPath):
        """ Used to convert path to absolute path (for any necessary disk access) """
        if self.filePath.startswith("./"):  # relative to project file
            self.filePath = os.path.normpath(os.path.join(parentPath, self.filePath))  # also converts '/' to os.sep


    #----------------------------------------------------------------------------
    # BaseDocumentMgr methods
    #----------------------------------------------------------------------------

    def _GetDoc(self):
        # HACK: temporary solution.
        import wx
        import wx.lib.docview
        if not self._doc:
            docMgr = wx.GetApp().GetDocumentManager()

            try:
                doc = docMgr.CreateDocument(self.filePath, docMgr.GetFlags()|wx.lib.docview.DOC_SILENT|wx.lib.docview.DOC_OPEN_ONCE|wx.lib.docview.DOC_NO_VIEW)
                if (doc == None):  # already open
                    docs = docMgr.GetDocuments()
                    for d in docs:
                        if d.GetFilename() == self.filePath:
                            doc = d
                            break
                self._doc = doc
            except Exception,e:
                aglogging.reportException(e, stacktrace=True)
                
        return self._doc


    def _GetLocalServiceProcessName(self):
        # HACK: temporary solution to getting process name from wsdlag file.
        doc = self._GetDoc()
        if doc:
            return doc.GetModel().processName
        else:
            return None


    processName = property(_GetLocalServiceProcessName)


    def _GetStateful(self):
        # HACK: temporary solution to getting stateful from wsdlag file.
        return self._GetDoc().GetModel().stateful


    def _SetStateful(self, stateful):
        # HACK: temporary solution to setting stateful from wsdlag file.
        self._GetDoc().GetModel().stateful = stateful


    stateful = property(_GetStateful, _SetStateful)


    def _GetLocalServiceCodeFile(self):
        # HACK: temporary solution to getting class name from wsdlag file.
        return self._GetDoc().GetModel().localServiceCodeFile


    def _SetLocalServiceCodeFile(self, codefile):
        # HACK: temporary solution to setting class name from wsdlag file.
        self._GetDoc().GetModel().localServiceCodeFile = codefile


    localServiceCodeFile = property(_GetLocalServiceCodeFile, _SetLocalServiceCodeFile)


    def _GetLocalServiceClassName(self):
        # HACK: temporary solution to getting class name from wsdlag file.
        return self._GetDoc().GetModel().localServiceClassName


    def _SetLocalServiceClassName(self, className):
        # HACK: temporary solution to setting class name from wsdlag file.
        self._GetDoc().GetModel().localServiceClassName = className


    localServiceClassName = property(_GetLocalServiceClassName, _SetLocalServiceClassName)


    def getServiceParameter(self, message, part):
        return self._GetDoc().GetModel().getServiceParameter(message, part)


# only activate this code if we programatically need to access these values
##    def _GetRssServiceBaseURL(self):
##        return self._GetDoc().GetModel().rssServiceBaseURL
##
##
##    def _SetRssServiceBaseURL(self, baseURL):
##        self._GetDoc().GetModel().rssServiceBaseURL = baseURL
##
##
##    rssServiceBaseURL = property(_GetRssServiceBaseURL, _SetRssServiceBaseURL)
##
##
##    def _GetRssServiceRssVersion(self):
##        return self._GetDoc().GetModel().rssServiceRssVersion
##
##
##    def _SetRssServiceRssVersion(self, rssVersion):
##        self._GetDoc().GetModel().rssServiceRssVersion = rssVersion
##
##
##    rssServiceRssVersion = property(_GetRssServiceRssVersion, _SetRssServiceRssVersion)


    def _GetServiceRefServiceType(self):
        # HACK: temporary solution to getting service type from wsdlag file.
        doc = self._GetDoc()
        if not doc:
            return None
        model = doc.GetModel()
        if hasattr(model, 'serviceType'):
            return model.serviceType
        else:
            return None


    def _SetServiceRefServiceType(self, serviceType):
        # HACK: temporary solution to getting service type from wsdlag file.
        self._GetDoc().GetModel().serviceType = serviceType


    serviceType = property(_GetServiceRefServiceType, _SetServiceRefServiceType)


    def getExternalPackage(self):
        # HACK: temporary solution to getting custom code filename from wsdlag file.
        import activegrid.model.projectmodel as projectmodel
        import wx
        import ProjectEditor

        appInfo = self._GetDoc().GetAppInfo()

        if appInfo.language == None:
            language = wx.ConfigBase_Get().Read(ProjectEditor.APP_LAST_LANGUAGE, projectmodel.LANGUAGE_DEFAULT)
        else:
            language = appInfo.language

        if language == projectmodel.LANGUAGE_PYTHON:
            suffix = ".py"
        elif language == projectmodel.LANGUAGE_PHP:
            suffix = ".php"
        pyFilename = self.name + suffix
        return self._GetDoc().GetAppDocMgr().fullPath(pyFilename)


#----------------------------------------------------------------------------
# Old Classes
#----------------------------------------------------------------------------

class Project_10:
    """ Version 1.0, kept for upgrading to latest version.  Over time, this should be deprecated. """
    __xmlname__ = "project"
    __xmlrename__ = { "_files":"files"}
    __xmlexclude__ = ('fileName',)
    __xmlattributes__ = ["version"]


    def __init__(self):
        self.version = PROJECT_VERSION_050730
        self._files = []


    def initialize(self):
        """ Required method for xmlmarshaller """
        pass


    def upgradeVersion(self):
        currModel = Project()
        for file in self._files:
            currModel._files.append(ProjectFile(currModel, file))
        return currModel


#----------------------------------------------------------------------------
# XML Marshalling Methods
#----------------------------------------------------------------------------

if ACTIVEGRID_BASE_IDE:
    KNOWNTYPES = {"ag:project" : Project, "ag:file" : ProjectFile}
else:
    KNOWNTYPES = {"ag:project" : Project, "ag:file" : ProjectFile,
                  "ag:appInfo" : AppInfo.AppInfo,
                  "ag:deploymentDataSource" : AppInfo.DeploymentDataSource,
                  "ag:dataSourceBinding" : AppInfo.DataSourceBinding}

def load(fileObject):
    version = xmlutils.getAgVersion(fileObject.name)
    # most current versions on top
    if version == PROJECT_VERSION_050826:
        fileObject.seek(0)
        project = xmlutils.load(fileObject.name, knownTypes=KNOWNTYPES, knownNamespaces=xmlutils.KNOWN_NAMESPACES, createGenerics=True)
    elif version == PROJECT_VERSION_050730:
        fileObject.seek(0)
        project = xmlutils.load(fileObject.name, knownTypes={"project" : Project_10}, createGenerics=True)
        project = project.upgradeVersion()
    else:
        # assume it is old version without version number
        fileObject.seek(0)
        project = xmlutils.load(fileObject.name, knownTypes={"project" : Project_10}, createGenerics=True)
        if project:
            project = project.upgradeVersion()
        else:
            print "Project, unknown version:", version
            return None

    if project:
        project._projectDir = os.path.dirname(fileObject.name)
        project.RelativeToAbsPath()

    return project


def save(fileObject, project, productionDeployment=False):
    if not project._projectDir:
        project._projectDir = os.path.dirname(fileObject.name)
    project.AbsToRelativePath()  # temporarily change it to relative paths for saving
    savedHomeDir = project.homeDir
    if productionDeployment:
        # for deployments, we don't want an abs path in homeDir since that
        # would tie the app to the current filesystem. So unset it.
        project.homeDir = None

    xmlutils.save(fileObject.name, project, prettyPrint=True, knownTypes=KNOWNTYPES, knownNamespaces=xmlutils.KNOWN_NAMESPACES)

    if productionDeployment:
        project.homeDir = savedHomeDir

    project.RelativeToAbsPath()  # swap it back to absolute path

