#----------------------------------------------------------------------------
# Name:         docview.py
# Purpose:      Port of the wxWindows docview classes
#
# Author:       Peter Yared
#
# Created:      5/15/03
# CVS-ID:       $Id$
# Copyright:    (c) 2003-2006 ActiveGrid, Inc. (Port of wxWindows classes by Julian Smart et al)
# License:      wxWindows license
#----------------------------------------------------------------------------


import os
import os.path
import shutil
import wx
import sys
_ = wx.GetTranslation


#----------------------------------------------------------------------
# docview globals
#----------------------------------------------------------------------

DOC_SDI = 1
DOC_MDI = 2
DOC_NEW = 4
DOC_SILENT = 8
DOC_OPEN_ONCE = 16
DOC_NO_VIEW = 32
DEFAULT_DOCMAN_FLAGS = DOC_SDI & DOC_OPEN_ONCE

TEMPLATE_VISIBLE = 1
TEMPLATE_INVISIBLE = 2
TEMPLATE_NO_CREATE = (4 | TEMPLATE_VISIBLE)
DEFAULT_TEMPLATE_FLAGS = TEMPLATE_VISIBLE

MAX_FILE_HISTORY = 9


#----------------------------------------------------------------------
# Convenience functions from wxWindows used in docview
#----------------------------------------------------------------------


def FileNameFromPath(path):
    """
    Returns the filename for a full path.
    """
    return os.path.split(path)[1]

def FindExtension(path):
    """
    Returns the extension of a filename for a full path.
    """
    return os.path.splitext(path)[1].lower()

def FileExists(path):
    """
    Returns True if the path exists.
    """
    return os.path.isfile(path)

def PathOnly(path):
    """
    Returns the path of a full path without the filename.
    """
    return os.path.split(path)[0]


#----------------------------------------------------------------------
# Document/View Classes
#----------------------------------------------------------------------


class Document(wx.EvtHandler):
    """
    The document class can be used to model an application's file-based data. It
    is part of the document/view framework supported by wxWindows, and cooperates
    with the wxView, wxDocTemplate and wxDocManager classes.
    
    Note this wxPython version also keeps track of the modification date of the
    document and if it changes on disk outside of the application, we will warn the
    user before saving to avoid clobbering the file.
    """


    def __init__(self, parent=None):
        """
        Constructor.  Define your own default constructor to initialize
        application-specific data.
        """
        wx.EvtHandler.__init__(self)

        self._documentParent = parent
        self._documentTemplate = None
        self._commandProcessor = None
        self._savedYet = False
        self._writeable = True

        self._documentTitle = None
        self._documentFile = None
        self._documentTypeName = None
        self._documentModified = False
        self._documentModificationDate = None
        self._documentViews = []


    def ProcessEvent(self, event):
        """
        Processes an event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        return False


    def GetFilename(self):
        """
        Gets the filename associated with this document, or "" if none is
        associated.
        """
        return self._documentFile


    def GetTitle(self):
        """
        Gets the title for this document. The document title is used for an
        associated frame (if any), and is usually constructed by the framework
        from the filename.
        """
        return self._documentTitle


    def SetTitle(self, title):
        """
        Sets the title for this document. The document title is used for an
        associated frame (if any), and is usually constructed by the framework
        from the filename.
        """
        self._documentTitle = title


    def GetDocumentName(self):
        """
        The document type name given to the wxDocTemplate constructor,
        copied to this document when the document is created. If several
        document templates are created that use the same document type, this
        variable is used in wxDocManager::CreateView to collate a list of
        alternative view types that can be used on this kind of document.
        """
        return self._documentTypeName


    def SetDocumentName(self, name):
        """
        Sets he document type name given to the wxDocTemplate constructor,
        copied to this document when the document is created. If several
        document templates are created that use the same document type, this
        variable is used in wxDocManager::CreateView to collate a list of
        alternative view types that can be used on this kind of document. Do
        not change the value of this variable.
        """
        self._documentTypeName = name


    def GetDocumentSaved(self):
        """
        Returns True if the document has been saved.  This method has been
        added to wxPython and is not in wxWindows.
        """
        return self._savedYet


    def SetDocumentSaved(self, saved=True):
        """
        Sets whether the document has been saved.  This method has been
        added to wxPython and is not in wxWindows.
        """
        self._savedYet = saved


    def GetCommandProcessor(self):
        """
        Returns the command processor associated with this document.
        """
        return self._commandProcessor


    def SetCommandProcessor(self, processor):
        """
        Sets the command processor to be used for this document. The document
        will then be responsible for its deletion. Normally you should not
        call this; override OnCreateCommandProcessor instead.
        """
        self._commandProcessor = processor


    def IsModified(self):
        """
        Returns true if the document has been modified since the last save,
        false otherwise. You may need to override this if your document view
        maintains its own record of being modified (for example if using
        wxTextWindow to view and edit the document).
        """
        return self._documentModified


    def Modify(self, modify):
        """
        Call with true to mark the document as modified since the last save,
        false otherwise. You may need to override this if your document view
        maintains its own record of being modified (for example if using
        xTextWindow to view and edit the document).
        This method has been extended to notify its views that the dirty flag has changed.
        """
        self._documentModified = modify
        self.UpdateAllViews(hint=("modify", self, self._documentModified))


    def SetDocumentModificationDate(self):
        """
        Saves the file's last modification date.
        This is used to check if the file has been modified outside of the application.
        This method has been added to wxPython and is not in wxWindows.
        """
        self._documentModificationDate = os.path.getmtime(self.GetFilename())


    def GetDocumentModificationDate(self):
        """
        Returns the file's modification date when it was loaded from disk.
        This is used to check if the file has been modified outside of the application.        
        This method has been added to wxPython and is not in wxWindows.
        """
        return self._documentModificationDate


    def IsDocumentModificationDateCorrect(self):
        """
        Returns False if the file has been modified outside of the application.
        This method has been added to wxPython and is not in wxWindows.
        """
        if not os.path.exists(self.GetFilename()):  # document must be in memory only and can't be out of date
            return True
        return self._documentModificationDate == os.path.getmtime(self.GetFilename())


    def GetViews(self):
        """
        Returns the list whose elements are the views on the document.
        """
        return self._documentViews


    def GetDocumentTemplate(self):
        """
        Returns the template that created the document.
        """
        return self._documentTemplate


    def SetDocumentTemplate(self, template):
        """
        Sets the template that created the document. Should only be called by
        the framework.
        """
        self._documentTemplate = template


    def DeleteContents(self):
        """
        Deletes the contents of the document.  Override this method as
        necessary.
        """
        return True


    def Destroy(self):
        """
        Destructor. Removes itself from the document manager.
        """
        self.DeleteContents()
        self._documentModificationDate = None
        if self.GetDocumentManager():
            self.GetDocumentManager().RemoveDocument(self)
        wx.EvtHandler.Destroy(self)


    def Close(self):
        """
        Closes the document, by calling OnSaveModified and then (if this true)
        OnCloseDocument. This does not normally delete the document object:
        use DeleteAllViews to do this implicitly.
        """
        if self.OnSaveModified():
            if self.OnCloseDocument():
                return True
            else:
                return False
        else:
            return False


    def OnCloseDocument(self):
        """
        The default implementation calls DeleteContents (an empty
        implementation) sets the modified flag to false. Override this to
        supply additional behaviour when the document is closed with Close.
        """
        self.NotifyClosing()
        self.DeleteContents()
        self.Modify(False)
        return True


    def DeleteAllViews(self):
        """
        Calls wxView.Close and deletes each view. Deleting the final view will
        implicitly delete the document itself, because the wxView destructor
        calls RemoveView. This in turns calls wxDocument::OnChangedViewList,
        whose default implemention is to save and delete the document if no
        views exist.
        """
        manager = self.GetDocumentManager()
        for view in self._documentViews:
            if not view.Close():
                return False
        if self in manager.GetDocuments():
            self.Destroy()
        return True


    def GetFirstView(self):
        """
        A convenience function to get the first view for a document, because
        in many cases a document will only have a single view.
        """
        if len(self._documentViews) == 0:
            return None
        return self._documentViews[0]


    def GetDocumentManager(self):
        """
        Returns the associated document manager.
        """
        if self._documentTemplate:
            return self._documentTemplate.GetDocumentManager()
        return None


    def OnNewDocument(self):
        """
        The default implementation calls OnSaveModified and DeleteContents,
        makes a default title for the document, and notifies the views that
        the filename (in fact, the title) has changed.
        """
        if not self.OnSaveModified() or not self.OnCloseDocument():
            return False
        self.DeleteContents()
        self.Modify(False)
        self.SetDocumentSaved(False)
        name = self.GetDocumentManager().MakeDefaultName()
        self.SetTitle(name)
        self.SetFilename(name, notifyViews = True)


    def Save(self):
        """
        Saves the document by calling OnSaveDocument if there is an associated
        filename, or SaveAs if there is no filename.
        """
        if not self.IsModified():  # and self._savedYet:  This was here, but if it is not modified who cares if it hasn't been saved yet?
            return True

        """ check for file modification outside of application """
        if not self.IsDocumentModificationDateCorrect():
            msgTitle = wx.GetApp().GetAppName()
            if not msgTitle:
                msgTitle = _("Application")
            res = wx.MessageBox(_("'%s' has been modified outside of %s.  Overwrite '%s' with current changes?") % (self.GetPrintableName(), msgTitle, self.GetPrintableName()),
                                msgTitle,
                                wx.YES_NO | wx.CANCEL | wx.ICON_QUESTION,
                                self.GetDocumentWindow())
    
            if res == wx.NO:
                return True
            elif res == wx.YES:
                pass
            else: # elif res == wx.CANCEL:
                return False
        
        if not self._documentFile or not self._savedYet:
            return self.SaveAs()
        return self.OnSaveDocument(self._documentFile)


    def SaveAs(self):
        """
        Prompts the user for a file to save to, and then calls OnSaveDocument.
        """
        docTemplate = self.GetDocumentTemplate()
        if not docTemplate:
            return False

        descr = docTemplate.GetDescription() + _(" (") + docTemplate.GetFileFilter() + _(") |") + docTemplate.GetFileFilter()  # spacing is important, make sure there is no space after the "|", it causes a bug on wx_gtk
        filename = wx.FileSelector(_("Save As"),
                                   docTemplate.GetDirectory(),
                                   FileNameFromPath(self.GetFilename()),
                                   docTemplate.GetDefaultExtension(),
                                   wildcard = descr,
                                   flags = wx.SAVE | wx.OVERWRITE_PROMPT,
                                   parent = self.GetDocumentWindow())
        if filename == "":
            return False

        name, ext = os.path.splitext(filename)
        if ext == "":
            filename += '.' + docTemplate.GetDefaultExtension()

        self.SetFilename(filename)
        self.SetTitle(FileNameFromPath(filename))

        for view in self._documentViews:
            view.OnChangeFilename()

        if not self.OnSaveDocument(filename):
            return False

        if docTemplate.FileMatchesTemplate(filename):
            self.GetDocumentManager().AddFileToHistory(filename)
            
        return True


    def OnSaveDocument(self, filename):
        """
        Constructs an output file for the given filename (which must
        not be empty), and calls SaveObject. If SaveObject returns true, the
        document is set to unmodified; otherwise, an error message box is
        displayed.
        """
        if not filename:
            return False

        msgTitle = wx.GetApp().GetAppName()
        if not msgTitle:
            msgTitle = _("File Error")

        backupFilename = None
        fileObject = None
        copied = False
        try:
            # if current file exists, move it to a safe place temporarily
            if os.path.exists(filename):

                # Check if read-only.
                if not os.access(filename, os.W_OK):
                    wx.MessageBox("Could not save '%s'.  No write permission to overwrite existing file." % FileNameFromPath(filename),
                                  msgTitle,
                                  wx.OK | wx.ICON_EXCLAMATION,
                                  self.GetDocumentWindow())
                    return False

                i = 1
                backupFilename = "%s.bak%s" % (filename, i)
                while os.path.exists(backupFilename):
                    i += 1
                    backupFilename = "%s.bak%s" % (filename, i)
                shutil.copy(filename, backupFilename)
                copied = True

            fileObject = file(filename, 'w')
            self.SaveObject(fileObject)
            fileObject.close()
            fileObject = None
            
            if backupFilename:
                os.remove(backupFilename)
        except:
            # for debugging purposes
            import traceback
            traceback.print_exc()

            if fileObject:
                fileObject.close()  # file is still open, close it, need to do this before removal 

            # save failed, remove copied file
            if backupFilename and copied:
                os.remove(backupFilename)

            wx.MessageBox("Could not save '%s'.  %s" % (FileNameFromPath(filename), sys.exc_value),
                          msgTitle,
                          wx.OK | wx.ICON_EXCLAMATION,
                          self.GetDocumentWindow())
            return False

        self.SetDocumentModificationDate()
        self.SetFilename(filename, True)
        self.Modify(False)
        self.SetDocumentSaved(True)
        #if wx.Platform == '__WXMAC__':  # Not yet implemented in wxPython
        #    wx.FileName(file).MacSetDefaultTypeAndCreator()
        return True


    def OnOpenDocument(self, filename):
        """
        Constructs an input file for the given filename (which must not
        be empty), and calls LoadObject. If LoadObject returns true, the
        document is set to unmodified; otherwise, an error message box is
        displayed. The document's views are notified that the filename has
        changed, to give windows an opportunity to update their titles. All of
        the document's views are then updated.
        """
        if not self.OnSaveModified():
            return False

        msgTitle = wx.GetApp().GetAppName()
        if not msgTitle:
            msgTitle = _("File Error")

        fileObject = file(filename, 'r')
        try:
            self.LoadObject(fileObject)
            fileObject.close()
            fileObject = None
        except:
            # for debugging purposes
            import traceback
            traceback.print_exc()

            if fileObject:
                fileObject.close()  # file is still open, close it 

            wx.MessageBox("Could not open '%s'.  %s" % (FileNameFromPath(filename), sys.exc_value),
                          msgTitle,
                          wx.OK | wx.ICON_EXCLAMATION,
                          self.GetDocumentWindow())
            return False

        self.SetDocumentModificationDate()
        self.SetFilename(filename, True)
        self.Modify(False)
        self.SetDocumentSaved(True)
        self.UpdateAllViews()
        return True


    def LoadObject(self, file):
        """
        Override this function and call it from your own LoadObject before
        loading your own data. LoadObject is called by the framework
        automatically when the document contents need to be loaded.

        Note that the wxPython version simply sends you a Python file object,
        so you can use pickle.
        """
        return True


    def SaveObject(self, file):
        """
        Override this function and call it from your own SaveObject before
        saving your own data. SaveObject is called by the framework
        automatically when the document contents need to be saved.

        Note that the wxPython version simply sends you a Python file object,
        so you can use pickle.
        """
        return True


    def Revert(self):
        """
        Override this function to revert the document to its last saved state.
        """
        return False


    def GetPrintableName(self):
        """
        Copies a suitable document name into the supplied name buffer.
        The default function uses the title, or if there is no title, uses the
        filename; or if no filename, the string 'Untitled'.
        """
        if self._documentTitle:
            return self._documentTitle
        elif self._documentFile:
            return FileNameFromPath(self._documentFile)
        else:
            return _("Untitled")


    def GetDocumentWindow(self):
        """
        Intended to return a suitable window for using as a parent for
        document-related dialog boxes. By default, uses the frame associated
        with the first view.
        """
        if len(self._documentViews) > 0:
            return self._documentViews[0].GetFrame()
        else:
            return wx.GetApp().GetTopWindow()


    def OnCreateCommandProcessor(self):
        """
        Override this function if you want a different (or no) command
        processor to be created when the document is created. By default, it
        returns an instance of wxCommandProcessor.
        """
        return CommandProcessor()


    def OnSaveModified(self):
        """
        If the document has been modified, prompts the user to ask if the
        changes should be changed. If the user replies Yes, the Save function
        is called. If No, the document is marked as unmodified and the
        function succeeds. If Cancel, the function fails.
        """
        if not self.IsModified():
            return True

        """ check for file modification outside of application """
        if not self.IsDocumentModificationDateCorrect():
            msgTitle = wx.GetApp().GetAppName()
            if not msgTitle:
                msgTitle = _("Warning")
            res = wx.MessageBox(_("'%s' has been modified outside of %s.  Overwrite '%s' with current changes?") % (self.GetPrintableName(), msgTitle, self.GetPrintableName()),
                                msgTitle,
                                wx.YES_NO | wx.CANCEL | wx.ICON_QUESTION,
                                self.GetDocumentWindow())
    
            if res == wx.NO:
                self.Modify(False)
                return True
            elif res == wx.YES:
                return wx.lib.docview.Document.Save(self)
            else: # elif res == wx.CANCEL:
                return False

        msgTitle = wx.GetApp().GetAppName()
        if not msgTitle:
            msgTitle = _("Warning")

        res = wx.MessageBox(_("Save changes to '%s'?") % self.GetPrintableName(),
                            msgTitle,
                            wx.YES_NO | wx.CANCEL | wx.ICON_QUESTION,
                            self.GetDocumentWindow())

        if res == wx.NO:
            self.Modify(False)
            return True
        elif res == wx.YES:
            return self.Save()
        else: # elif res == wx.CANCEL:
            return False


    def Draw(context):
        """
        Called by printing framework to draw the view.
        """
        return True


    def AddView(self, view):
        """
        If the view is not already in the list of views, adds the view and
        calls OnChangedViewList.
        """
        if not view in self._documentViews:
            self._documentViews.append(view)
            self.OnChangedViewList()
        return True


    def RemoveView(self, view):
        """
        Removes the view from the document's list of views, and calls
        OnChangedViewList.
        """
        if view in self._documentViews:
            self._documentViews.remove(view)
            self.OnChangedViewList()
        return True


    def OnCreate(self, path, flags):
        """
        The default implementation calls DeleteContents (an empty
        implementation) sets the modified flag to false. Override this to
        supply additional behaviour when the document is opened with Open.
        """
        if flags & DOC_NO_VIEW:
            return True
        return self.GetDocumentTemplate().CreateView(self, flags)


    def OnChangedViewList(self):
        """
        Called when a view is added to or deleted from this document. The
        default implementation saves and deletes the document if no views
        exist (the last one has just been removed).
        """
        if len(self._documentViews) == 0:
            if self.OnSaveModified():
                pass # C version does a delete but Python will garbage collect


    def UpdateAllViews(self, sender = None, hint = None):
        """
        Updates all views. If sender is non-NULL, does not update this view.
        hint represents optional information to allow a view to optimize its
        update.
        """
        for view in self._documentViews:
            if view != sender:
                view.OnUpdate(sender, hint)


    def NotifyClosing(self):
        """
        Notifies the views that the document is going to close.
        """
        for view in self._documentViews:
            view.OnClosingDocument()


    def SetFilename(self, filename, notifyViews = False):
        """
        Sets the filename for this document. Usually called by the framework.
        If notifyViews is true, wxView.OnChangeFilename is called for all
        views.
        """
        self._documentFile = filename
        if notifyViews:
            for view in self._documentViews:
                view.OnChangeFilename()


    def GetWriteable(self):
        """
        Returns true if the document can be written to its accociated file path.
        This method has been added to wxPython and is not in wxWindows.
        """
        if not self._writeable:
            return False 
        if not self._documentFile:  # Doesn't exist, do a save as
            return True
        else:
            return os.access(self._documentFile, os.W_OK)


    def SetWriteable(self, writeable):
        """
        Set to False if the document can not be saved.  This will disable the ID_SAVE_AS
        event and is useful for custom documents that should not be saveable.  The ID_SAVE
        event can be disabled by never Modifying the document.  This method has been added
        to wxPython and is not in wxWindows.
        """
        self._writeable = writeable


class View(wx.EvtHandler):
    """
    The view class can be used to model the viewing and editing component of
    an application's file-based data. It is part of the document/view
    framework supported by wxWindows, and cooperates with the wxDocument,
    wxDocTemplate and wxDocManager classes.
    """

    def __init__(self):
        """
        Constructor. Define your own default constructor to initialize
        application-specific data.
        """
        wx.EvtHandler.__init__(self)
        self._viewDocument = None
        self._viewFrame = None


    def Destroy(self):
        """
        Destructor. Removes itself from the document's list of views.
        """
        if self._viewDocument:
            self._viewDocument.RemoveView(self)
        wx.EvtHandler.Destroy(self)


    def ProcessEvent(self, event):
        """
        Processes an event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        if not self.GetDocument() or not self.GetDocument().ProcessEvent(event):
            return False
        else:
            return True


    def ProcessUpdateUIEvent(self, event):
        """
        Processes a UI event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        return False


    def OnActivateView(self, activate, activeView, deactiveView):
        """
        Called when a view is activated by means of wxView::Activate. The
        default implementation does nothing.
        """
        pass


    def OnClosingDocument(self):
        """
        Override this to clean up the view when the document is being closed.
        The default implementation does nothing.
        """
        pass


    def OnDraw(self, dc):
        """
        Override this to draw the view for the printing framework.  The
        default implementation does nothing.
        """
        pass


    def OnPrint(self, dc, info):
        """
        Override this to print the view for the printing framework.  The
        default implementation calls View.OnDraw.
        """
        self.OnDraw(dc)


    def OnUpdate(self, sender, hint):
        """
        Called when the view should be updated. sender is a pointer to the
        view that sent the update request, or NULL if no single view requested
        the update (for instance, when the document is opened). hint is as yet
        unused but may in future contain application-specific information for
        making updating more efficient.
        """
        if hint:
            if hint[0] == "modify":  # if dirty flag changed, update the view's displayed title
                frame = self.GetFrame()
                if frame and hasattr(frame, "OnTitleIsModified"):
                    frame.OnTitleIsModified()
                    return True
        return False
        

    def OnChangeFilename(self):
        """
        Called when the filename has changed. The default implementation
        constructs a suitable title and sets the title of the view frame (if
        any).
        """
        if self.GetFrame():
            appName = wx.GetApp().GetAppName()
            if not self.GetDocument():
                if appName:
                    title = appName
                else:
                    return
            else:
                if appName and isinstance(self.GetFrame(), DocChildFrame):  # Only need app name in title for SDI
                    title = appName + _(" - ")
                else:
                    title = ''
                self.GetFrame().SetTitle(title + self.GetDocument().GetPrintableName())


    def GetDocument(self):
        """
        Returns the document associated with the view.
        """
        return self._viewDocument


    def SetDocument(self, doc):
        """
        Associates the given document with the view. Normally called by the
        framework.
        """
        self._viewDocument = doc
        if doc:
            doc.AddView(self)


    def GetViewName(self):
        """
        Gets the name associated with the view (passed to the wxDocTemplate
        constructor). Not currently used by the framework.
        """
        return self._viewTypeName


    def SetViewName(self, name):
        """
        Sets the view type name. Should only be called by the framework.
        """
        self._viewTypeName = name


    def Close(self, deleteWindow=True):
        """
        Closes the view by calling OnClose. If deleteWindow is true, this
        function should delete the window associated with the view.
        """
        if self.OnClose(deleteWindow = deleteWindow):
            return True
        else:
            return False


    def Activate(self, activate=True):
        """
        Call this from your view frame's OnActivate member to tell the
        framework which view is currently active. If your windowing system
        doesn't call OnActivate, you may need to call this function from
        OnMenuCommand or any place where you know the view must be active, and
        the framework will need to get the current view.

        The prepackaged view frame wxDocChildFrame calls wxView.Activate from
        its OnActivate member and from its OnMenuCommand member.
        """
        if self.GetDocument() and self.GetDocumentManager():
            self.OnActivateView(activate, self, self.GetDocumentManager().GetCurrentView())
            self.GetDocumentManager().ActivateView(self, activate)


    def OnClose(self, deleteWindow=True):
        """
        Implements closing behaviour. The default implementation calls
        wxDocument.Close to close the associated document. Does not delete the
        view. The application may wish to do some cleaning up operations in
        this function, if a call to wxDocument::Close succeeded. For example,
        if your application's all share the same window, you need to
        disassociate the window from the view and perhaps clear the window. If
        deleteWindow is true, delete the frame associated with the view.
        """
        if self.GetDocument():
            return self.GetDocument().Close()
        else:
            return True


    def OnCreate(self, doc, flags):
        """
        wxDocManager or wxDocument creates a wxView via a wxDocTemplate. Just
        after the wxDocTemplate creates the wxView, it calls wxView::OnCreate.
        In its OnCreate member function, the wxView can create a
        wxDocChildFrame or a derived class. This wxDocChildFrame provides user
        interface elements to view and/or edit the contents of the wxDocument.

        By default, simply returns true. If the function returns false, the
        view will be deleted.
        """
        return True


    def OnCreatePrintout(self):
        """
        Returns a wxPrintout object for the purposes of printing. It should
        create a new object every time it is called; the framework will delete
        objects it creates.

        By default, this function returns an instance of wxDocPrintout, which
        prints and previews one page by calling wxView.OnDraw.

        Override to return an instance of a class other than wxDocPrintout.
        """
        return DocPrintout(self, self.GetDocument().GetPrintableName())


    def GetFrame(self):
        """
        Gets the frame associated with the view (if any). Note that this
        "frame" is not a wxFrame at all in the generic MDI implementation
        which uses the notebook pages instead of the frames and this is why
        this method returns a wxWindow and not a wxFrame.
        """
        return self._viewFrame


    def SetFrame(self, frame):
        """
        Sets the frame associated with this view. The application should call
        this if possible, to tell the view about the frame.  See GetFrame for
        the explanation about the mismatch between the "Frame" in the method
        name and the type of its parameter.
        """
        self._viewFrame = frame


    def GetDocumentManager(self):
        """
        Returns the document manager instance associated with this view.
        """
        if self._viewDocument:
            return self.GetDocument().GetDocumentManager()
        else:
            return None


class DocTemplate(wx.Object):
    """
    The wxDocTemplate class is used to model the relationship between a
    document class and a view class.
    """


    def __init__(self, manager, description, filter, dir, ext, docTypeName, viewTypeName, docType, viewType, flags=DEFAULT_TEMPLATE_FLAGS, icon=None):
        """
        Constructor. Create instances dynamically near the start of your
        application after creating a wxDocManager instance, and before doing
        any document or view operations.

        manager is the document manager object which manages this template.

        description is a short description of what the template is for. This
        string will be displayed in the file filter list of Windows file
        selectors.

        filter is an appropriate file filter such as \*.txt.

        dir is the default directory to use for file selectors.

        ext is the default file extension (such as txt).

        docTypeName is a name that should be unique for a given type of
        document, used for gathering a list of views relevant to a
        particular document.

        viewTypeName is a name that should be unique for a given view.

        docClass is a Python class. If this is not supplied, you will need to
        derive a new wxDocTemplate class and override the CreateDocument
        member to return a new document instance on demand.

        viewClass is a Python class. If this is not supplied, you will need to
        derive a new wxDocTemplate class and override the CreateView member to
        return a new view instance on demand.

        flags is a bit list of the following:
        wx.TEMPLATE_VISIBLE The template may be displayed to the user in
        dialogs.

        wx.TEMPLATE_INVISIBLE The template may not be displayed to the user in
        dialogs.

        wx.DEFAULT_TEMPLATE_FLAGS Defined as wxTEMPLATE_VISIBLE.
        """
        self._docManager = manager
        self._description = description
        self._fileFilter = filter
        self._directory = dir
        self._defaultExt = ext
        self._docTypeName = docTypeName
        self._viewTypeName = viewTypeName
        self._docType = docType
        self._viewType = viewType
        self._flags = flags
        self._icon = icon

        self._docManager.AssociateTemplate(self)


    def GetDefaultExtension(self):
        """
        Returns the default file extension for the document data, as passed to
        the document template constructor.
        """
        return self._defaultExt


    def SetDefaultExtension(self, defaultExt):
        """
        Sets the default file extension.
        """
        self._defaultExt = defaultExt


    def GetDescription(self):
        """
        Returns the text description of this template, as passed to the
        document template constructor.
        """
        return self._description


    def SetDescription(self, description):
        """
        Sets the template description.
        """
        self._description = description


    def GetDirectory(self):
        """
        Returns the default directory, as passed to the document template
        constructor.
        """
        return self._directory


    def SetDirectory(self, dir):
        """
        Sets the default directory.
        """
        self._directory = dir


    def GetDocumentManager(self):
        """
        Returns the document manager instance for which this template was
        created.
        """
        return self._docManager


    def SetDocumentManager(self, manager):
        """
        Sets the document manager instance for which this template was
        created. Should not be called by the application.
        """
        self._docManager = manager


    def GetFileFilter(self):
        """
        Returns the file filter, as passed to the document template
        constructor.
        """
        return self._fileFilter


    def SetFileFilter(self, filter):
        """
        Sets the file filter.
        """
        self._fileFilter = filter


    def GetFlags(self):
        """
        Returns the flags, as passed to the document template constructor.
        (see the constructor description for more details).
        """
        return self._flags


    def SetFlags(self, flags):
        """
        Sets the internal document template flags (see the constructor
        description for more details).
        """
        self._flags = flags


    def GetIcon(self):
        """
        Returns the icon, as passed to the document template
        constructor.  This method has been added to wxPython and is
        not in wxWindows.
        """
        return self._icon


    def SetIcon(self, flags):
        """
        Sets the icon.  This method has been added to wxPython and is not
        in wxWindows.
        """
        self._icon = icon


    def GetDocumentType(self):
        """
        Returns the Python document class, as passed to the document template
        constructor.
        """
        return self._docType


    def GetViewType(self):
        """
        Returns the Python view class, as passed to the document template
        constructor.
        """
        return self._viewType


    def IsVisible(self):
        """
        Returns true if the document template can be shown in user dialogs,
        false otherwise.
        """
        return (self._flags & TEMPLATE_VISIBLE) == TEMPLATE_VISIBLE


    def IsNewable(self):
        """
        Returns true if the document template can be shown in "New" dialogs,
        false otherwise.
        
        This method has been added to wxPython and is not in wxWindows.
        """
        return (self._flags & TEMPLATE_NO_CREATE) != TEMPLATE_NO_CREATE
        

    def GetDocumentName(self):
        """
        Returns the document type name, as passed to the document template
        constructor.
        """
        return self._docTypeName


    def GetViewName(self):
        """
        Returns the view type name, as passed to the document template
        constructor.
        """
        return self._viewTypeName


    def CreateDocument(self, path, flags):
        """
        Creates a new instance of the associated document class. If you have
        not supplied a class to the template constructor, you will need to
        override this function to return an appropriate document instance.
        """
        doc = self._docType()
        doc.SetFilename(path)
        doc.SetDocumentTemplate(self)
        self.GetDocumentManager().AddDocument(doc)
        doc.SetCommandProcessor(doc.OnCreateCommandProcessor())
        if doc.OnCreate(path, flags):
            return doc
        else:
            if doc in self.GetDocumentManager().GetDocuments():
                doc.DeleteAllViews()
            return None


    def CreateView(self, doc, flags):
        """
        Creates a new instance of the associated document view. If you have
        not supplied a class to the template constructor, you will need to
        override this function to return an appropriate view instance.
        """
        view = self._viewType()
        view.SetDocument(doc)
        if view.OnCreate(doc, flags):
            return view
        else:
            view.Destroy()
            return None


    def FileMatchesTemplate(self, path):
        """
        Returns True if the path's extension matches one of this template's
        file filter extensions.
        """
        ext = FindExtension(path)
        if not ext: return False
        
        extList = self.GetFileFilter().replace('*','').split(';')
        return ext in extList


class DocManager(wx.EvtHandler):
    """
    The wxDocManager class is part of the document/view framework supported by
    wxWindows, and cooperates with the wxView, wxDocument and wxDocTemplate
    classes.
    """

    def __init__(self, flags=DEFAULT_DOCMAN_FLAGS, initialize=True):
        """
        Constructor. Create a document manager instance dynamically near the
        start of your application before doing any document or view operations.

        flags is used in the Python version to indicate whether the document
        manager is in DOC_SDI or DOC_MDI mode.

        If initialize is true, the Initialize function will be called to
        create a default history list object. If you derive from wxDocManager,
        you may wish to call the base constructor with false, and then call
        Initialize in your own constructor, to allow your own Initialize or
        OnCreateFileHistory functions to be called.
        """

        wx.EvtHandler.__init__(self)

        self._defaultDocumentNameCounter = 1
        self._flags = flags
        self._currentView = None
        self._lastActiveView = None
        self._maxDocsOpen = 10000
        self._fileHistory = None
        self._templates = []
        self._docs = []
        self._lastDirectory = ""

        if initialize:
            self.Initialize()

        wx.EVT_MENU(self, wx.ID_OPEN, self.OnFileOpen)
        wx.EVT_MENU(self, wx.ID_CLOSE, self.OnFileClose)
        wx.EVT_MENU(self, wx.ID_CLOSE_ALL, self.OnFileCloseAll)
        wx.EVT_MENU(self, wx.ID_REVERT, self.OnFileRevert)
        wx.EVT_MENU(self, wx.ID_NEW, self.OnFileNew)
        wx.EVT_MENU(self, wx.ID_SAVE, self.OnFileSave)
        wx.EVT_MENU(self, wx.ID_SAVEAS, self.OnFileSaveAs)
        wx.EVT_MENU(self, wx.ID_UNDO, self.OnUndo)
        wx.EVT_MENU(self, wx.ID_REDO, self.OnRedo)
        wx.EVT_MENU(self, wx.ID_PRINT, self.OnPrint)
        wx.EVT_MENU(self, wx.ID_PRINT_SETUP, self.OnPrintSetup)
        wx.EVT_MENU(self, wx.ID_PREVIEW, self.OnPreview)

        wx.EVT_UPDATE_UI(self, wx.ID_OPEN, self.OnUpdateFileOpen)
        wx.EVT_UPDATE_UI(self, wx.ID_CLOSE, self.OnUpdateFileClose)
        wx.EVT_UPDATE_UI(self, wx.ID_CLOSE_ALL, self.OnUpdateFileCloseAll)
        wx.EVT_UPDATE_UI(self, wx.ID_REVERT, self.OnUpdateFileRevert)
        wx.EVT_UPDATE_UI(self, wx.ID_NEW, self.OnUpdateFileNew)
        wx.EVT_UPDATE_UI(self, wx.ID_SAVE, self.OnUpdateFileSave)
        wx.EVT_UPDATE_UI(self, wx.ID_SAVEAS, self.OnUpdateFileSaveAs)
        wx.EVT_UPDATE_UI(self, wx.ID_UNDO, self.OnUpdateUndo)
        wx.EVT_UPDATE_UI(self, wx.ID_REDO, self.OnUpdateRedo)
        wx.EVT_UPDATE_UI(self, wx.ID_PRINT, self.OnUpdatePrint)
        wx.EVT_UPDATE_UI(self, wx.ID_PRINT_SETUP, self.OnUpdatePrintSetup)
        wx.EVT_UPDATE_UI(self, wx.ID_PREVIEW, self.OnUpdatePreview)


    def Destroy(self):
        """
        Destructor.
        """
        self.Clear()
        wx.EvtHandler.Destroy(self)


    def GetFlags(self):
        """
        Returns the document manager's flags.  This method has been
        added to wxPython and is not in wxWindows.
        """
        return self._flags


    def CloseDocument(self, doc, force=True):
        """
        Closes the specified document.
        """
        if doc.Close() or force:
            doc.DeleteAllViews()
            if doc in self._docs:
                doc.Destroy()
            return True
        return False


    def CloseDocuments(self, force=True):
        """
        Closes all currently opened documents.
        """
        for document in self._docs[::-1]:  # Close in lifo (reverse) order.  We clone the list to make sure we go through all docs even as they are deleted
            if not self.CloseDocument(document, force):
                return False
            if document:
                document.DeleteAllViews() # Implicitly delete the document when the last view is removed
        return True


    def Clear(self, force=True):
        """
        Closes all currently opened document by callling CloseDocuments and
        clears the document manager's templates.
        """
        if not self.CloseDocuments(force):
            return False
        self._templates = []
        return True


    def Initialize(self):
        """
        Initializes data; currently just calls OnCreateFileHistory. Some data
        cannot always be initialized in the constructor because the programmer
        must be given the opportunity to override functionality. In fact
        Initialize is called from the wxDocManager constructor, but this can
        be vetoed by passing false to the second argument, allowing the
        derived class's constructor to call Initialize, possibly calling a
        different OnCreateFileHistory from the default.

        The bottom line: if you're not deriving from Initialize, forget it and
        construct wxDocManager with no arguments.
        """
        self.OnCreateFileHistory()
        return True


    def OnCreateFileHistory(self):
        """
        A hook to allow a derived class to create a different type of file
        history. Called from Initialize.
        """
        self._fileHistory = wx.FileHistory()


    def OnFileClose(self, event):
        """
        Closes and deletes the currently active document.
        """
        doc = self.GetCurrentDocument()
        if doc:
            doc.DeleteAllViews()
            if doc in self._docs:
                self._docs.remove(doc)


    def OnFileCloseAll(self, event):
        """
        Closes and deletes all the currently opened documents.
        """
        return self.CloseDocuments(force = False)

    
    def OnFileNew(self, event):
        """
        Creates a new document and reads in the selected file.
        """
        self.CreateDocument('', DOC_NEW)


    def OnFileOpen(self, event):
        """
        Creates a new document and reads in the selected file.
        """
        if not self.CreateDocument('', DEFAULT_DOCMAN_FLAGS):
            self.OnOpenFileFailure()


    def OnFileRevert(self, event):
        """
        Reverts the current document by calling wxDocument.Save for the current
        document.
        """
        doc = self.GetCurrentDocument()
        if not doc:
            return
        doc.Revert()


    def OnFileSave(self, event):
        """
        Saves the current document by calling wxDocument.Save for the current
        document.
        """
        doc = self.GetCurrentDocument()
        if not doc:
            return
        doc.Save()


    def OnFileSaveAs(self, event):
        """
        Calls wxDocument.SaveAs for the current document.
        """
        doc = self.GetCurrentDocument()
        if not doc:
            return
        doc.SaveAs()


    def OnPrint(self, event):
        """
        Prints the current document by calling its View's OnCreatePrintout
        method.
        """
        view = self.GetCurrentView()
        if not view:
            return

        printout = view.OnCreatePrintout()
        if printout:
            if not hasattr(self, "printData"):
                self.printData = wx.PrintData()
                self.printData.SetPaperId(wx.PAPER_LETTER)
            self.printData.SetPrintMode(wx.PRINT_MODE_PRINTER)
                
            pdd = wx.PrintDialogData(self.printData)
            printer = wx.Printer(pdd)
            printer.Print(view.GetFrame(), printout)


    def OnPrintSetup(self, event):
        """
        Presents the print setup dialog.
        """
        view = self.GetCurrentView()
        if view:
            parentWin = view.GetFrame()
        else:
            parentWin = wx.GetApp().GetTopWindow()

        if not hasattr(self, "printData"):
            self.printData = wx.PrintData()
            self.printData.SetPaperId(wx.PAPER_LETTER)
            
        data = wx.PrintDialogData(self.printData)
        printDialog = wx.PrintDialog(parentWin, data)
        printDialog.GetPrintDialogData().SetSetupDialog(True)
        printDialog.ShowModal()
        
        # this makes a copy of the wx.PrintData instead of just saving
        # a reference to the one inside the PrintDialogData that will
        # be destroyed when the dialog is destroyed
        self.printData = wx.PrintData(printDialog.GetPrintDialogData().GetPrintData())
        
        printDialog.Destroy()


    def OnPreview(self, event):
        """
        Previews the current document by calling its View's OnCreatePrintout
        method.
        """
        view = self.GetCurrentView()
        if not view:
            return

        printout = view.OnCreatePrintout()
        if printout:
            if not hasattr(self, "printData"):
                self.printData = wx.PrintData()
                self.printData.SetPaperId(wx.PAPER_LETTER)
            self.printData.SetPrintMode(wx.PRINT_MODE_PREVIEW)
                
            data = wx.PrintDialogData(self.printData)
            # Pass two printout objects: for preview, and possible printing.
            preview = wx.PrintPreview(printout, view.OnCreatePrintout(), data)
            if not preview.Ok():
                wx.MessageBox(_("Unable to display print preview."))
                return
            # wxWindows source doesn't use base frame's pos, size, and icon, but did it this way so it would work like MS Office etc.
            mimicFrame =  wx.GetApp().GetTopWindow()
            frame = wx.PreviewFrame(preview, mimicFrame, _("Print Preview"), mimicFrame.GetPosition(), mimicFrame.GetSize())
            frame.SetIcon(mimicFrame.GetIcon())
            frame.SetTitle(_("%s - %s - Preview") % (mimicFrame.GetTitle(), view.GetDocument().GetPrintableName()))
            frame.Initialize()
            frame.Show(True)


    def OnUndo(self, event):
        """
        Issues an Undo command to the current document's command processor.
        """
        doc = self.GetCurrentDocument()
        if not doc:
            return
        if doc.GetCommandProcessor():
            doc.GetCommandProcessor().Undo()


    def OnRedo(self, event):
        """
        Issues a Redo command to the current document's command processor.
        """
        doc = self.GetCurrentDocument()
        if not doc:
            return
        if doc.GetCommandProcessor():
            doc.GetCommandProcessor().Redo()


    def OnUpdateFileOpen(self, event):
        """
        Updates the user interface for the File Open command.
        """
        event.Enable(True)


    def OnUpdateFileClose(self, event):
        """
        Updates the user interface for the File Close command.
        """
        event.Enable(self.GetCurrentDocument() != None)


    def OnUpdateFileCloseAll(self, event):
        """
        Updates the user interface for the File Close All command.
        """
        event.Enable(self.GetCurrentDocument() != None)


    def OnUpdateFileRevert(self, event):
        """
        Updates the user interface for the File Revert command.
        """
        event.Enable(self.GetCurrentDocument() != None)


    def OnUpdateFileNew(self, event):
        """
        Updates the user interface for the File New command.
        """
        return True


    def OnUpdateFileSave(self, event):
        """
        Updates the user interface for the File Save command.
        """
        doc = self.GetCurrentDocument()
        event.Enable(doc != None and doc.IsModified())


    def OnUpdateFileSaveAs(self, event):
        """
        Updates the user interface for the File Save As command.
        """
        event.Enable(self.GetCurrentDocument() != None and self.GetCurrentDocument().GetWriteable())


    def OnUpdateUndo(self, event):
        """
        Updates the user interface for the Undo command.
        """
        doc = self.GetCurrentDocument()
        event.Enable(doc != None and doc.GetCommandProcessor() != None and doc.GetCommandProcessor().CanUndo())
        if doc and doc.GetCommandProcessor():
            doc.GetCommandProcessor().SetMenuStrings()
        else:
            event.SetText(_("&Undo\tCtrl+Z"))


    def OnUpdateRedo(self, event):
        """
        Updates the user interface for the Redo command.
        """
        doc = self.GetCurrentDocument()
        event.Enable(doc != None and doc.GetCommandProcessor() != None and doc.GetCommandProcessor().CanRedo())
        if doc and doc.GetCommandProcessor():
            doc.GetCommandProcessor().SetMenuStrings()
        else:
            event.SetText(_("&Redo\tCtrl+Y"))


    def OnUpdatePrint(self, event):
        """
        Updates the user interface for the Print command.
        """
        event.Enable(self.GetCurrentDocument() != None)


    def OnUpdatePrintSetup(self, event):
        """
        Updates the user interface for the Print Setup command.
        """
        return True


    def OnUpdatePreview(self, event):
        """
        Updates the user interface for the Print Preview command.
        """
        event.Enable(self.GetCurrentDocument() != None)


    def GetCurrentView(self):
        """
        Returns the currently active view.
        """
        if self._currentView:
            return self._currentView
        if len(self._docs) == 1:
            return self._docs[0].GetFirstView()
        return None


    def GetLastActiveView(self):
        """
        Returns the last active view.  This is used in the SDI framework where dialogs can be mistaken for a view
        and causes the framework to deactivete the current view.  This happens when something like a custom dialog box used
        to operate on the current view is shown.
        """
        if len(self._docs) >= 1:
            return self._lastActiveView
        else:
            return None


    def ProcessEvent(self, event):
        """
        Processes an event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        view = self.GetCurrentView()
        if view:
            if view.ProcessEvent(event):
                return True
        id = event.GetId()
        if id == wx.ID_OPEN:
            self.OnFileOpen(event)
            return True
        elif id == wx.ID_CLOSE:
            self.OnFileClose(event)
            return True
        elif id == wx.ID_CLOSE_ALL:
            self.OnFileCloseAll(event)
            return True
        elif id == wx.ID_REVERT:
            self.OnFileRevert(event)
            return True
        elif id == wx.ID_NEW:
            self.OnFileNew(event)
            return True
        elif id == wx.ID_SAVE:
            self.OnFileSave(event)
            return True
        elif id == wx.ID_SAVEAS:
            self.OnFileSaveAs(event)
            return True
        elif id == wx.ID_UNDO:
            self.OnUndo(event)
            return True
        elif id == wx.ID_REDO:
            self.OnRedo(event)
            return True
        elif id == wx.ID_PRINT:
            self.OnPrint(event)
            return True
        elif id == wx.ID_PRINT_SETUP:
            self.OnPrintSetup(event)
            return True
        elif id == wx.ID_PREVIEW:
            self.OnPreview(event)
            return True
        else:
            return False


    def ProcessUpdateUIEvent(self, event):
        """
        Processes a UI event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        id = event.GetId()
        view = self.GetCurrentView()
        if view:
            if view.ProcessUpdateUIEvent(event):
                return True
        if id == wx.ID_OPEN:
            self.OnUpdateFileOpen(event)
            return True
        elif id == wx.ID_CLOSE:
            self.OnUpdateFileClose(event)
            return True
        elif id == wx.ID_CLOSE_ALL:
            self.OnUpdateFileCloseAll(event)
            return True
        elif id == wx.ID_REVERT:
            self.OnUpdateFileRevert(event)
            return True
        elif id == wx.ID_NEW:
            self.OnUpdateFileNew(event)
            return True
        elif id == wx.ID_SAVE:
            self.OnUpdateFileSave(event)
            return True
        elif id == wx.ID_SAVEAS:
            self.OnUpdateFileSaveAs(event)
            return True
        elif id == wx.ID_UNDO:
            self.OnUpdateUndo(event)
            return True
        elif id == wx.ID_REDO:
            self.OnUpdateRedo(event)
            return True
        elif id == wx.ID_PRINT:
            self.OnUpdatePrint(event)
            return True
        elif id == wx.ID_PRINT_SETUP:
            self.OnUpdatePrintSetup(event)
            return True
        elif id == wx.ID_PREVIEW:
            self.OnUpdatePreview(event)
            return True
        else:
            return False


    def CreateDocument(self, path, flags=0):
        """
        Creates a new document in a manner determined by the flags parameter,
        which can be:

        wx.lib.docview.DOC_NEW Creates a fresh document.
        wx.lib.docview.DOC_SILENT Silently loads the given document file.

        If wx.lib.docview.DOC_NEW is present, a new document will be created and returned,
        possibly after asking the user for a template to use if there is more
        than one document template. If wx.lib.docview.DOC_SILENT is present, a new document
        will be created and the given file loaded into it. If neither of these
        flags is present, the user will be presented with a file selector for
        the file to load, and the template to use will be determined by the
        extension (Windows) or by popping up a template choice list (other
        platforms).

        If the maximum number of documents has been reached, this function
        will delete the oldest currently loaded document before creating a new
        one.

        wxPython version supports the document manager's wx.lib.docview.DOC_OPEN_ONCE
        and wx.lib.docview.DOC_NO_VIEW flag.
        
        if wx.lib.docview.DOC_OPEN_ONCE is present, trying to open the same file multiple 
        times will just return the same document.
        if wx.lib.docview.DOC_NO_VIEW is present, opening a file will generate the document,
        but not generate a corresponding view.
        """
        templates = []
        for temp in self._templates:
            if temp.IsVisible():
                templates.append(temp)
        if len(templates) == 0:
            return None

        if len(self.GetDocuments()) >= self._maxDocsOpen:
           doc = self.GetDocuments()[0]
           if not self.CloseDocument(doc, False):
               return None

        if flags & DOC_NEW:
            for temp in templates[:]:
                if not temp.IsNewable():
                    templates.remove(temp)
            if len(templates) == 1:
                temp = templates[0]
            else:
                temp = self.SelectDocumentType(templates)
            if temp:
                newDoc = temp.CreateDocument(path, flags)
                if newDoc:
                    newDoc.SetDocumentName(temp.GetDocumentName())
                    newDoc.SetDocumentTemplate(temp)
                    newDoc.OnNewDocument()
                return newDoc
            else:
                return None

        if path and flags & DOC_SILENT:
            temp = self.FindTemplateForPath(path)
        else:
            temp, path = self.SelectDocumentPath(templates, path, flags)

        # Existing document
        if path and self.GetFlags() & DOC_OPEN_ONCE:
            for document in self._docs:
                if document.GetFilename() and os.path.normcase(document.GetFilename()) == os.path.normcase(path):
                    """ check for file modification outside of application """
                    if not document.IsDocumentModificationDateCorrect():
                        msgTitle = wx.GetApp().GetAppName()
                        if not msgTitle:
                            msgTitle = _("Warning")
                        shortName = document.GetPrintableName()
                        res = wx.MessageBox(_("'%s' has been modified outside of %s.  Reload '%s' from file system?") % (shortName, msgTitle, shortName),
                                            msgTitle,
                                            wx.YES_NO | wx.ICON_QUESTION,
                                            self.FindSuitableParent())
                        if res == wx.YES:
                           if not self.CloseDocument(document, False):
                               wx.MessageBox(_("Couldn't reload '%s'.  Unable to close current '%s'.") % (shortName, shortName))
                               return None
                           return self.CreateDocument(path, flags)
                        elif res == wx.NO:  # don't ask again
                            document.SetDocumentModificationDate()

                    firstView = document.GetFirstView()
                    if not firstView and not (flags & DOC_NO_VIEW):
                        document.GetDocumentTemplate().CreateView(document, flags)
                        document.UpdateAllViews()
                        firstView = document.GetFirstView()
                        
                    if firstView and firstView.GetFrame() and not (flags & DOC_NO_VIEW):
                        firstView.GetFrame().SetFocus()  # Not in wxWindows code but useful nonetheless
                        if hasattr(firstView.GetFrame(), "IsIconized") and firstView.GetFrame().IsIconized():  # Not in wxWindows code but useful nonetheless
                            firstView.GetFrame().Iconize(False)
                    return None

        if temp:
            newDoc = temp.CreateDocument(path, flags)
            if newDoc:
                newDoc.SetDocumentName(temp.GetDocumentName())
                newDoc.SetDocumentTemplate(temp)
                if not newDoc.OnOpenDocument(path):
                    newDoc.DeleteAllViews()  # Implicitly deleted by DeleteAllViews
                    frame = newDoc.GetFirstView().GetFrame()
                    if frame:
                        frame.Destroy() # DeleteAllViews doesn't get rid of the frame, so we'll explicitly destroy it.
                    return None
                self.AddFileToHistory(path)
            return newDoc

        return None


    def CreateView(self, doc, flags=0):
        """
        Creates a new view for the given document. If more than one view is
        allowed for the document (by virtue of multiple templates mentioning
        the same document type), a choice of view is presented to the user.
        """
        templates = []
        for temp in self._templates:
            if temp.IsVisible():
                if temp.GetDocumentName() == doc.GetDocumentName():
                    templates.append(temp)
        if len(templates) == 0:
            return None

        if len(templates) == 1:
            temp = templates[0]
            view = temp.CreateView(doc, flags)
            if view:
                view.SetViewName(temp.GetViewName())
            return view

        temp = SelectViewType(templates)
        if temp:
            view = temp.CreateView(doc, flags)
            if view:
                view.SetViewName(temp.GetViewName())
            return view
        else:
            return None


    def DeleteTemplate(self, template, flags):
        """
        Placeholder, not yet implemented in wxWindows.
        """
        pass


    def FlushDoc(self, doc):
        """
        Placeholder, not yet implemented in wxWindows.
        """
        return False


    def MatchTemplate(self, path):
        """
        Placeholder, not yet implemented in wxWindows.
        """
        return None


    def GetCurrentDocument(self):
        """
        Returns the document associated with the currently active view (if any).
        """
        view = self.GetCurrentView()
        if view:
            return view.GetDocument()
        else:
            return None


    def MakeDefaultName(self):
        """
        Returns a suitable default name. This is implemented by appending an
        integer counter to the string "Untitled" and incrementing the counter.
        """
        name = _("Untitled %d") % self._defaultDocumentNameCounter
        self._defaultDocumentNameCounter = self._defaultDocumentNameCounter + 1
        return name


    def MakeFrameTitle(self):
        """
        Returns a suitable title for a document frame. This is implemented by
        appending the document name to the application name.
        """
        appName = wx.GetApp().GetAppName()
        if not doc:
            title = appName
        else:
            docName = doc.GetPrintableName()
            title = docName + _(" - ") + appName
        return title


    def AddFileToHistory(self, fileName):
        """
        Adds a file to the file history list, if we have a pointer to an
        appropriate file menu.
        """
        if self._fileHistory:
            self._fileHistory.AddFileToHistory(fileName)


    def RemoveFileFromHistory(self, i):
        """
        Removes a file from the file history list, if we have a pointer to an
        appropriate file menu.
        """
        if self._fileHistory:
            self._fileHistory.RemoveFileFromHistory(i)


    def GetFileHistory(self):
        """
        Returns the file history.
        """
        return self._fileHistory


    def GetHistoryFile(self, i):
        """
        Returns the file at index i from the file history.
        """
        if self._fileHistory:
            return self._fileHistory.GetHistoryFile(i)
        else:
            return None


    def FileHistoryUseMenu(self, menu):
        """
        Use this menu for appending recently-visited document filenames, for
        convenient access. Calling this function with a valid menu enables the
        history list functionality.

        Note that you can add multiple menus using this function, to be
        managed by the file history object.
        """
        if self._fileHistory:
            self._fileHistory.UseMenu(menu)


    def FileHistoryRemoveMenu(self, menu):
        """
        Removes the given menu from the list of menus managed by the file
        history object.
        """
        if self._fileHistory:
            self._fileHistory.RemoveMenu(menu)


    def FileHistoryLoad(self, config):
        """
        Loads the file history from a config object.
        """
        if self._fileHistory:
            self._fileHistory.Load(config)


    def FileHistorySave(self, config):
        """
        Saves the file history into a config object. This must be called
        explicitly by the application.
        """
        if self._fileHistory:
            self._fileHistory.Save(config)


    def FileHistoryAddFilesToMenu(self, menu=None):
        """
        Appends the files in the history list, to all menus managed by the
        file history object.

        If menu is specified, appends the files in the history list to the
        given menu only.
        """
        if self._fileHistory:
            if menu:
                self._fileHistory.AddFilesToThisMenu(menu)
            else:
                self._fileHistory.AddFilesToMenu()


    def GetHistoryFilesCount(self):
        """
        Returns the number of files currently stored in the file history.
        """
        if self._fileHistory:
            return self._fileHistory.GetNoHistoryFiles()
        else:
            return 0


    def FindTemplateForPath(self, path):
        """
        Given a path, try to find template that matches the extension. This is
        only an approximate method of finding a template for creating a
        document.
        
        Note this wxPython verson looks for and returns a default template if no specific template is found.
        """
        default = None
        for temp in self._templates:
            if temp.FileMatchesTemplate(path):
                return temp
                
            if "*.*" in temp.GetFileFilter():
                default = temp
        return default


    def FindSuitableParent(self):
        """
        Returns a parent frame or dialog, either the frame with the current
        focus or if there is no current focus the application's top frame.
        """
        parent = wx.GetApp().GetTopWindow()
        focusWindow = wx.Window_FindFocus()
        if focusWindow:
            while focusWindow and not isinstance(focusWindow, wx.Dialog) and not isinstance(focusWindow, wx.Frame):
                focusWindow = focusWindow.GetParent()
            if focusWindow:
                parent = focusWindow
        return parent


    def SelectDocumentPath(self, templates, flags, save):
        """
        Under Windows, pops up a file selector with a list of filters
        corresponding to document templates. The wxDocTemplate corresponding
        to the selected file's extension is returned.

        On other platforms, if there is more than one document template a
        choice list is popped up, followed by a file selector.

        This function is used in wxDocManager.CreateDocument.
        """
        if wx.Platform == "__WXMSW__" or wx.Platform == "__WXGTK__" or wx.Platform == "__WXMAC__":
            descr = ''
            for temp in templates:
                if temp.IsVisible():
                    if len(descr) > 0:
                        descr = descr + _('|')
                    descr = descr + temp.GetDescription() + _(" (") + temp.GetFileFilter() + _(") |") + temp.GetFileFilter()  # spacing is important, make sure there is no space after the "|", it causes a bug on wx_gtk
            descr = _("All|*.*|%s") % descr  # spacing is important, make sure there is no space after the "|", it causes a bug on wx_gtk
        else:
            descr = _("*.*")

        dlg = wx.FileDialog(self.FindSuitableParent(),
                               _("Select a File"),
                               wildcard=descr,
                               style=wx.OPEN|wx.FILE_MUST_EXIST|wx.CHANGE_DIR)
        # dlg.CenterOnParent()  # wxBug: caused crash with wx.FileDialog
        if dlg.ShowModal() == wx.ID_OK:
            path = dlg.GetPath()
        else:
            path = None
        dlg.Destroy()
            
        if path:  
            theTemplate = self.FindTemplateForPath(path)
            return (theTemplate, path)
        
        return (None, None)           


    def OnOpenFileFailure(self):
        """
        Called when there is an error opening a file.
        """
        pass


    def SelectDocumentType(self, temps, sort=False):
        """
        Returns a document template by asking the user (if there is more than
        one template). This function is used in wxDocManager.CreateDocument.

        Parameters

        templates - list of templates from which to choose a desired template.

        sort - If more than one template is passed in in templates, then this
        parameter indicates whether the list of templates that the user will
        have to choose from is sorted or not when shown the choice box dialog.
        Default is false.
        """
        templates = []
        for temp in temps:
            if temp.IsVisible():
                want = True
                for temp2 in templates:
                    if temp.GetDocumentName() == temp2.GetDocumentName() and temp.GetViewName() == temp2.GetViewName():
                        want = False
                        break
                if want:
                    templates.append(temp)

        if len(templates) == 0:
            return None
        elif len(templates) == 1:
            return template[0]

        if sort:
            def tempcmp(a, b):
                return cmp(a.GetDescription(), b.GetDescription())
            templates.sort(tempcmp)

        strings = []
        for temp in templates:
            strings.append(temp.GetDescription())

        res = wx.GetSingleChoiceIndex(_("Select a document type:"),
                                      _("Documents"),
                                      strings,
                                      self.FindSuitableParent())
        if res == -1:
            return None
        return templates[res]


    def SelectViewType(self, temps, sort=False):
        """
        Returns a document template by asking the user (if there is more than one template), displaying a list of valid views. This function is used in wxDocManager::CreateView. The dialog normally will not appear because the array of templates only contains those relevant to the document in question, and often there will only be one such.
        """
        templates = []
        strings = []
        for temp in temps:
            if temp.IsVisible() and temp.GetViewTypeName():
                if temp.GetViewName() not in strings:
                    templates.append(temp)
                    strings.append(temp.GetViewTypeName())

        if len(templates) == 0:
            return None
        elif len(templates) == 1:
            return templates[0]

        if sort:
            def tempcmp(a, b):
                return cmp(a.GetViewTypeName(), b.GetViewTypeName())
            templates.sort(tempcmp)

        res = wx.GetSingleChoiceIndex(_("Select a document view:"),
                                      _("Views"),
                                      strings,
                                      self.FindSuitableParent())
        if res == -1:
            return None
        return templates[res]


    def GetTemplates(self):
        """
        Returns the document manager's template list.  This method has been added to
        wxPython and is not in wxWindows.
        """
        return self._templates


    def AssociateTemplate(self, docTemplate):
        """
        Adds the template to the document manager's template list.
        """
        if docTemplate not in self._templates:
            self._templates.append(docTemplate)


    def DisassociateTemplate(self, docTemplate):
        """
        Removes the template from the list of templates.
        """
        self._templates.remove(docTemplate)


    def AddDocument(self, document):
        """
        Adds the document to the list of documents.
        """
        if document not in self._docs:
            self._docs.append(document)


    def RemoveDocument(self, doc):
        """
        Removes the document from the list of documents.
        """
        if doc in self._docs:
            self._docs.remove(doc)


    def ActivateView(self, view, activate=True, deleting=False):
        """
        Sets the current view.
        """
        if activate:
            self._currentView = view
            self._lastActiveView = view
        else:
            self._currentView = None


    def GetMaxDocsOpen(self):
        """
        Returns the number of documents that can be open simultaneously.
        """
        return self._maxDocsOpen


    def SetMaxDocsOpen(self, maxDocsOpen):
        """
        Sets the maximum number of documents that can be open at a time. By
        default, this is 10,000. If you set it to 1, existing documents will
        be saved and deleted when the user tries to open or create a new one
        (similar to the behaviour of Windows Write, for example). Allowing
        multiple documents gives behaviour more akin to MS Word and other
        Multiple Document Interface applications.
        """
        self._maxDocsOpen = maxDocsOpen


    def GetDocuments(self):
        """
        Returns the list of documents.
        """
        return self._docs


class DocParentFrame(wx.Frame):
    """
    The wxDocParentFrame class provides a default top-level frame for
    applications using the document/view framework. This class can only be
    used for SDI (not MDI) parent frames.

    It cooperates with the wxView, wxDocument, wxDocManager and wxDocTemplates
    classes.
    """

    def __init__(self, manager, frame, id, title, pos=wx.DefaultPosition, size=wx.DefaultSize, style=wx.DEFAULT_FRAME_STYLE, name="frame"):
        """
        Constructor.  Note that the event table must be rebuilt for the
        frame since the EvtHandler is not virtual.
        """
        wx.Frame.__init__(self, frame, id, title, pos, size, style)
        self._docManager = manager

        wx.EVT_CLOSE(self, self.OnCloseWindow)

        wx.EVT_MENU(self, wx.ID_EXIT, self.OnExit)
        wx.EVT_MENU_RANGE(self, wx.ID_FILE1, wx.ID_FILE9, self.OnMRUFile)

        wx.EVT_MENU(self, wx.ID_NEW, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_OPEN, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_CLOSE_ALL, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_CLOSE, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_REVERT, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_SAVE, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_SAVEAS, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_UNDO, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_REDO, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_PRINT, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_PRINT_SETUP, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_PREVIEW, self.ProcessEvent)

        wx.EVT_UPDATE_UI(self, wx.ID_NEW, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_OPEN, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_CLOSE_ALL, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_CLOSE, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_REVERT, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_SAVE, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_SAVEAS, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_UNDO, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_REDO, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_PRINT, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_PRINT_SETUP, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_PREVIEW, self.ProcessUpdateUIEvent)


    def ProcessEvent(self, event):
        """
        Processes an event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        return self._docManager and self._docManager.ProcessEvent(event)


    def ProcessUpdateUIEvent(self, event):
        """
        Processes a UI event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        return self._docManager and self._docManager.ProcessUpdateUIEvent(event)


    def OnExit(self, event):
        """
        Called when File/Exit is chosen and closes the window.
        """
        self.Close()


    def OnMRUFile(self, event):
        """
        Opens the appropriate file when it is selected from the file history
        menu.
        """
        n = event.GetId() - wx.ID_FILE1
        filename = self._docManager.GetHistoryFile(n)
        if filename:
            self._docManager.CreateDocument(filename, DOC_SILENT)
        else:
            self._docManager.RemoveFileFromHistory(n)
            msgTitle = wx.GetApp().GetAppName()
            if not msgTitle:
                msgTitle = _("File Error")
            wx.MessageBox("The file '%s' doesn't exist and couldn't be opened.\nIt has been removed from the most recently used files list" % FileNameFromPath(file),
                          msgTitle,
                          wx.OK | wx.ICON_EXCLAMATION,
                          self)


    def OnCloseWindow(self, event):
        """
        Deletes all views and documents. If no user input cancelled the
        operation, the frame will be destroyed and the application will exit.
        """
        if self._docManager.Clear(not event.CanVeto()):
            self.Destroy()
        else:
            event.Veto()


class DocChildFrame(wx.Frame):
    """
    The wxDocChildFrame class provides a default frame for displaying
    documents on separate windows. This class can only be used for SDI (not
    MDI) child frames.

    The class is part of the document/view framework supported by wxWindows,
    and cooperates with the wxView, wxDocument, wxDocManager and wxDocTemplate
    classes.
    """


    def __init__(self, doc, view, frame, id, title, pos=wx.DefaultPosition, size=wx.DefaultSize, style=wx.DEFAULT_FRAME_STYLE, name="frame"):
        """
        Constructor.  Note that the event table must be rebuilt for the
        frame since the EvtHandler is not virtual.
        """
        wx.Frame.__init__(self, frame, id, title, pos, size, style, name)
        wx.EVT_ACTIVATE(self, self.OnActivate)
        wx.EVT_CLOSE(self, self.OnCloseWindow)
        self._childDocument = doc
        self._childView = view
        if view:
            view.SetFrame(self)

        wx.EVT_MENU(self, wx.ID_NEW, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_OPEN, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_CLOSE_ALL, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_CLOSE, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_REVERT, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_SAVE, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_SAVEAS, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_UNDO, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_REDO, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_PRINT, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_PRINT_SETUP, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_PREVIEW, self.ProcessEvent)

        wx.EVT_UPDATE_UI(self, wx.ID_NEW, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_OPEN, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_CLOSE_ALL, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_CLOSE, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_REVERT, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_SAVE, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_SAVEAS, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_UNDO, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_REDO, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_PRINT, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_PRINT_SETUP, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_PREVIEW, self.ProcessUpdateUIEvent)


    def ProcessEvent(self, event):
        """
        Processes an event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        if self._childView:
            self._childView.Activate(True)
        if not self._childView or not self._childView.ProcessEvent(event):
            # IsInstance not working, but who cares just send all the commands up since this isn't a real ProcessEvent like wxWindows
            # if not isinstance(event, wx.CommandEvent) or not self.GetParent() or not self.GetParent().ProcessEvent(event):
            if not self.GetParent() or not self.GetParent().ProcessEvent(event):
                return False
            else:
                return True
        else:
            return True


    def ProcessUpdateUIEvent(self, event):
        """
        Processes a UI event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        if self.GetParent():
            self.GetParent().ProcessUpdateUIEvent(event)
        else:
            return False


    def OnActivate(self, event):
        """
        Activates the current view.
        """
        # wx.Frame.OnActivate(event)  This is in the wxWindows docview demo but there is no such method in wxPython, so do a Raise() instead
        if self._childView:
            self._childView.Activate(event.GetActive())


    def OnCloseWindow(self, event):
        """
        Closes and deletes the current view and document.
        """
        if self._childView:
            ans = False
            if not event.CanVeto():
                ans = True
            else:
                ans = self._childView.Close(deleteWindow = False)

            if ans:
                self._childView.Activate(False)
                self._childView.Destroy()
                self._childView = None
                if self._childDocument:
                    self._childDocument.Destroy()  # This isn't in the wxWindows codebase but the document needs to be disposed of somehow
                    self._childDocument = None
                self.Destroy()
            else:
                event.Veto()
        else:
            event.Veto()


    def GetDocument(self):
        """
        Returns the document associated with this frame.
        """
        return self._childDocument


    def SetDocument(self, document):
        """
        Sets the document for this frame.
        """
        self._childDocument = document


    def GetView(self):
        """
        Returns the view associated with this frame.
        """
        return self._childView


    def SetView(self, view):
        """
        Sets the view for this frame.
        """
        self._childView = view


class DocMDIParentFrame(wx.MDIParentFrame):
    """
    The wxDocMDIParentFrame class provides a default top-level frame for
    applications using the document/view framework. This class can only be
    used for MDI parent frames.

    It cooperates with the wxView, wxDocument, wxDocManager and wxDocTemplate
    classes.
    """


    def __init__(self, manager, frame, id, title, pos=wx.DefaultPosition, size=wx.DefaultSize, style=wx.DEFAULT_FRAME_STYLE, name="frame"):
        """
        Constructor.  Note that the event table must be rebuilt for the
        frame since the EvtHandler is not virtual.
        """
        wx.MDIParentFrame.__init__(self, frame, id, title, pos, size, style, name)
        self._docManager = manager

        wx.EVT_CLOSE(self, self.OnCloseWindow)

        wx.EVT_MENU(self, wx.ID_EXIT, self.OnExit)
        wx.EVT_MENU_RANGE(self, wx.ID_FILE1, wx.ID_FILE9, self.OnMRUFile)

        wx.EVT_MENU(self, wx.ID_NEW, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_OPEN, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_CLOSE_ALL, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_CLOSE, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_REVERT, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_SAVE, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_SAVEAS, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_UNDO, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_REDO, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_PRINT, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_PRINT_SETUP, self.ProcessEvent)
        wx.EVT_MENU(self, wx.ID_PREVIEW, self.ProcessEvent)

        wx.EVT_UPDATE_UI(self, wx.ID_NEW, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_OPEN, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_CLOSE_ALL, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_CLOSE, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_REVERT, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_SAVE, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_SAVEAS, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_UNDO, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_REDO, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_PRINT, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_PRINT_SETUP, self.ProcessUpdateUIEvent)
        wx.EVT_UPDATE_UI(self, wx.ID_PREVIEW, self.ProcessUpdateUIEvent)


    def ProcessEvent(self, event):
        """
        Processes an event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        return self._docManager and self._docManager.ProcessEvent(event)


    def ProcessUpdateUIEvent(self, event):
        """
        Processes a UI event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        return self._docManager and self._docManager.ProcessUpdateUIEvent(event)


    def OnExit(self, event):
        """
        Called when File/Exit is chosen and closes the window.
        """
        self.Close()


    def OnMRUFile(self, event):
        """
        Opens the appropriate file when it is selected from the file history
        menu.
        """
        n = event.GetId() - wx.ID_FILE1
        filename = self._docManager.GetHistoryFile(n)
        if filename:
            self._docManager.CreateDocument(filename, DOC_SILENT)
        else:
            self._docManager.RemoveFileFromHistory(n)
            msgTitle = wx.GetApp().GetAppName()
            if not msgTitle:
                msgTitle = _("File Error")
            wx.MessageBox("The file '%s' doesn't exist and couldn't be opened.\nIt has been removed from the most recently used files list" % FileNameFromPath(file),
                          msgTitle,
                          wx.OK | wx.ICON_EXCLAMATION,
                          self)


    def OnCloseWindow(self, event):
        """
        Deletes all views and documents. If no user input cancelled the
        operation, the frame will be destroyed and the application will exit.
        """
        if self._docManager.Clear(not event.CanVeto()):
            self.Destroy()
        else:
            event.Veto()


class DocMDIChildFrame(wx.MDIChildFrame):
    """
    The wxDocMDIChildFrame class provides a default frame for displaying
    documents on separate windows. This class can only be used for MDI child
    frames.

    The class is part of the document/view framework supported by wxWindows,
    and cooperates with the wxView, wxDocument, wxDocManager and wxDocTemplate
    classes.
    """


    def __init__(self, doc, view, frame, id, title, pos=wx.DefaultPosition, size=wx.DefaultSize, style=wx.DEFAULT_FRAME_STYLE, name="frame"):
        """
        Constructor.  Note that the event table must be rebuilt for the
        frame since the EvtHandler is not virtual.
        """
        wx.MDIChildFrame.__init__(self, frame, id, title, pos, size, style, name)
        self._childDocument = doc
        self._childView = view
        if view:
            view.SetFrame(self)
        # self.Create(doc, view, frame, id, title, pos, size, style, name)
        self._activeEvent = None
        self._activated = 0
        wx.EVT_ACTIVATE(self, self.OnActivate)
        wx.EVT_CLOSE(self, self.OnCloseWindow)

        if frame:  # wxBug: For some reason the EVT_ACTIVATE event is not getting triggered for the first mdi client window that is opened so we have to do it manually
            mdiChildren = filter(lambda x: isinstance(x, wx.MDIChildFrame), frame.GetChildren())
            if len(mdiChildren) == 1:
                self.Activate()


##    # Couldn't get this to work, but seems to work fine with single stage construction
##    def Create(self, doc, view, frame, id, title, pos, size, style, name):
##        self._childDocument = doc
##        self._childView = view
##        if wx.MDIChildFrame.Create(self, frame, id, title, pos, size, style, name):
##            if view:
##                view.SetFrame(self)
##                return True
##        return False



    def Activate(self):  # Need this in case there are embedded sash windows and such, OnActivate is not getting called
        """
        Activates the current view.
        """
        if self._childView:
            self._childView.Activate(True)


    def ProcessEvent(event):
        """
        Processes an event, searching event tables and calling zero or more
        suitable event handler function(s).  Note that the ProcessEvent
        method is called from the wxPython docview framework directly since
        wxPython does not have a virtual ProcessEvent function.
        """
        if self._activeEvent == event:
            return False

        self._activeEvent = event  # Break recursion loops

        if self._childView:
            self._childView.Activate(True)

        if not self._childView or not self._childView.ProcessEvent(event):
            if not isinstance(event, wx.CommandEvent) or not self.GetParent() or not self.GetParent().ProcessEvent(event):
                ret = False
            else:
                ret = True
        else:
            ret = True

        self._activeEvent = None
        return ret


    def OnActivate(self, event):
        """
        Sets the currently active view to be the frame's view. You may need to
        override (but still call) this function in order to set the keyboard
        focus for your subwindow.
        """
        if self._activated != 0:
            return True
        self._activated += 1
        wx.MDIChildFrame.Activate(self)
        if event.GetActive() and self._childView:
            self._childView.Activate(event.GetActive())
        self._activated = 0


    def OnCloseWindow(self, event):
        """
        Closes and deletes the current view and document.
        """
        if self._childView:
            ans = False
            if not event.CanVeto():
                ans = True
            else:
                ans = self._childView.Close(deleteWindow = False)

            if ans:
                self._childView.Activate(False)
                self._childView.Destroy()
                self._childView = None
                if self._childDocument:  # This isn't in the wxWindows codebase but the document needs to be disposed of somehow
                    self._childDocument.DeleteContents()
                    if self._childDocument.GetDocumentManager():
                        self._childDocument.GetDocumentManager().RemoveDocument(self._childDocument)
                self._childDocument = None
                self.Destroy()
            else:
                event.Veto()
        else:
            event.Veto()


    def GetDocument(self):
        """
        Returns the document associated with this frame.
        """
        return self._childDocument


    def SetDocument(self, document):
        """
        Sets the document for this frame.
        """
        self._childDocument = document


    def GetView(self):
        """
        Returns the view associated with this frame.
        """
        return self._childView


    def SetView(self, view):
        """
        Sets the view for this frame.
        """
        self._childView = view


    def OnTitleIsModified(self):
        """
        Add/remove to the frame's title an indication that the document is dirty.
        If the document is dirty, an '*' is appended to the title
        This method has been added to wxPython and is not in wxWindows.
        """
        title = self.GetTitle()
        if title:
            if self.GetDocument().IsModified():
                if title.endswith("*"):
                    return
                else:
                    title = title + "*"
                    self.SetTitle(title)
            else:
                if title.endswith("*"):
                    title = title[:-1]
                    self.SetTitle(title)                
                else:
                    return


class DocPrintout(wx.Printout):
    """
    DocPrintout is a default Printout that prints the first page of a document
    view.
    """


    def __init__(self, view, title="Printout"):
        """
        Constructor.
        """
        wx.Printout.__init__(self, title)
        self._printoutView = view


    def GetView(self):
        """
        Returns the DocPrintout's view.
        """
        return self._printoutView


    def OnPrintPage(self, page):
        """
        Prints the first page of the view.
        """
        dc = self.GetDC()
        ppiScreenX, ppiScreenY = self.GetPPIScreen()
        ppiPrinterX, ppiPrinterY = self.GetPPIPrinter()
        scale = ppiPrinterX/ppiScreenX
        w, h = dc.GetSize()
        pageWidth, pageHeight = self.GetPageSizePixels()
        overallScale = scale * w / pageWidth
        dc.SetUserScale(overallScale, overallScale)
        if self._printoutView:
            self._printoutView.OnDraw(dc)
        return True


    def HasPage(self, pageNum):
        """
        Indicates that the DocPrintout only has a single page.
        """
        return pageNum == 1


    def GetPageInfo(self):
        """
        Indicates that the DocPrintout only has a single page.
        """
        minPage = 1
        maxPage = 1
        selPageFrom = 1
        selPageTo = 1
        return (minPage, maxPage, selPageFrom, selPageTo)


#----------------------------------------------------------------------
# Command Classes
#----------------------------------------------------------------------

class Command(wx.Object):
    """
    wxCommand is a base class for modelling an application command, which is
    an action usually performed by selecting a menu item, pressing a toolbar
    button or any other means provided by the application to change the data
    or view.
    """


    def __init__(self, canUndo = False, name = None):
        """
        Constructor. wxCommand is an abstract class, so you will need to
        derive a new class and call this constructor from your own constructor.

        canUndo tells the command processor whether this command is undo-able.
        You can achieve the same functionality by overriding the CanUndo member
        function (if for example the criteria for undoability is context-
        dependent).

        name must be supplied for the command processor to display the command
        name in the application's edit menu.
        """
        self._canUndo = canUndo
        self._name = name


    def CanUndo(self):
        """
        Returns true if the command can be undone, false otherwise.
        """
        return self._canUndo


    def GetName(self):
        """
        Returns the command name.
        """
        return self._name


    def Do(self):
        """
        Override this member function to execute the appropriate action when
        called. Return true to indicate that the action has taken place, false
        otherwise. Returning false will indicate to the command processor that
        the action is not undoable and should not be added to the command
        history.
        """
        return True


    def Undo(self):
        """
        Override this member function to un-execute a previous Do. Return true
        to indicate that the action has taken place, false otherwise. Returning
        false will indicate to the command processor that the action is not
        redoable and no change should be made to the command history.

        How you implement this command is totally application dependent, but
        typical strategies include:

        Perform an inverse operation on the last modified piece of data in the
        document. When redone, a copy of data stored in command is pasted back
        or some operation reapplied. This relies on the fact that you know the
        ordering of Undos; the user can never Undo at an arbitrary position in
        he command history.

        Restore the entire document state (perhaps using document
        transactioning). Potentially very inefficient, but possibly easier to
        code if the user interface and data are complex, and an 'inverse
        execute' operation is hard to write.
        """
        return True


class CommandProcessor(wx.Object):
    """
    wxCommandProcessor is a class that maintains a history of wxCommands, with
    undo/redo functionality built-in. Derive a new class from this if you want
    different behaviour.
    """


    def __init__(self, maxCommands=-1):
        """
        Constructor.  maxCommands may be set to a positive integer to limit
        the number of commands stored to it, otherwise (and by default) the
        list of commands can grow arbitrarily.
        """
        self._maxCommands = maxCommands
        self._editMenu = None
        self._undoAccelerator = _("Ctrl+Z")
        self._redoAccelerator = _("Ctrl+Y")
        self.ClearCommands()


    def _GetCurrentCommand(self):
        if len(self._commands) == 0:
            return None
        else:
            return self._commands[-1]


    def _GetCurrentRedoCommand(self):
        if len(self._redoCommands) == 0:
            return None
        else:
            return self._redoCommands[-1]


    def GetMaxCommands(self):
        """
        Returns the maximum number of commands that the command processor
        stores.

        """
        return self._maxCommands


    def GetCommands(self):
        """
        Returns the list of commands.
        """
        return self._commands


    def ClearCommands(self):
        """
        Deletes all the commands in the list and sets the current command
        pointer to None.
        """
        self._commands = []
        self._redoCommands = []


    def GetEditMenu(self):
        """
        Returns the edit menu associated with the command processor.
        """
        return self._editMenu


    def SetEditMenu(self, menu):
        """
        Tells the command processor to update the Undo and Redo items on this
        menu as appropriate. Set this to NULL if the menu is about to be
        destroyed and command operations may still be performed, or the
        command processor may try to access an invalid pointer.
        """
        self._editMenu = menu


    def GetUndoAccelerator(self):
        """
        Returns the string that will be appended to the Undo menu item.
        """
        return self._undoAccelerator


    def SetUndoAccelerator(self, accel):
        """
        Sets the string that will be appended to the Redo menu item.
        """
        self._undoAccelerator = accel


    def GetRedoAccelerator(self):
        """
        Returns the string that will be appended to the Redo menu item.
        """
        return self._redoAccelerator


    def SetRedoAccelerator(self, accel):
        """
        Sets the string that will be appended to the Redo menu item.
        """
        self._redoAccelerator = accel


    def SetEditMenu(self, menu):
        """
        Tells the command processor to update the Undo and Redo items on this
        menu as appropriate. Set this to NULL if the menu is about to be
        destroyed and command operations may still be performed, or the
        command processor may try to access an invalid pointer.
        """
        self._editMenu = menu


    def SetMenuStrings(self):
        """
        Sets the menu labels according to the currently set menu and the
        current command state.
        """
        if self.GetEditMenu() != None:
            undoCommand = self._GetCurrentCommand()
            redoCommand = self._GetCurrentRedoCommand()
            undoItem = self.GetEditMenu().FindItemById(wx.ID_UNDO)
            redoItem = self.GetEditMenu().FindItemById(wx.ID_REDO)
            if self.GetUndoAccelerator():
                undoAccel = '\t' + self.GetUndoAccelerator()
            else:
                undoAccel = ''
            if self.GetRedoAccelerator():
                redoAccel = '\t' + self.GetRedoAccelerator()
            else:
                redoAccel = ''
            if undoCommand and undoItem and undoCommand.CanUndo():
                undoItem.SetText(_("&Undo ") + undoCommand.GetName() + undoAccel)
            #elif undoCommand and not undoCommand.CanUndo():
            #    undoItem.SetText(_("Can't Undo") + undoAccel)
            else:
                undoItem.SetText(_("&Undo" + undoAccel))
            if redoCommand and redoItem:
                redoItem.SetText(_("&Redo ") + redoCommand.GetName() + redoAccel)
            else:
                redoItem.SetText(_("&Redo") + redoAccel)


    def CanUndo(self):
        """
        Returns true if the currently-active command can be undone, false
        otherwise.
        """
        if self._GetCurrentCommand() == None:
            return False
        return self._GetCurrentCommand().CanUndo()


    def CanRedo(self):
        """
        Returns true if the currently-active command can be redone, false
        otherwise.
        """
        return self._GetCurrentRedoCommand() != None


    def Submit(self, command, storeIt=True):
        """
        Submits a new command to the command processor. The command processor
        calls wxCommand::Do to execute the command; if it succeeds, the
        command is stored in the history list, and the associated edit menu
        (if any) updated appropriately. If it fails, the command is deleted
        immediately. Once Submit has been called, the passed command should
        not be deleted directly by the application.

        storeIt indicates whether the successful command should be stored in
        the history list.
        """
        done = command.Do()
        if done:
            del self._redoCommands[:]
            if storeIt:
                self._commands.append(command)
        if self._maxCommands > -1:
            if len(self._commands) > self._maxCommands:
                del self._commands[0]
        return done


    def Redo(self):
        """
        Redoes the command just undone.
        """
        cmd = self._GetCurrentRedoCommand()
        if not cmd:
            return False
        done = cmd.Do()
        if done:
            self._commands.append(self._redoCommands.pop())
        return done


    def Undo(self):
        """
        Undoes the command just executed.
        """
        cmd = self._GetCurrentCommand()
        if not cmd:
            return False
        done = cmd.Undo()
        if done:
            self._redoCommands.append(self._commands.pop())
        return done


