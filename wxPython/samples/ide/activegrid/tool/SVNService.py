#----------------------------------------------------------------------------
# Name:         SVNService.py
# Purpose:      Subversion Service for pydocview
#
# Author:       Morgan Hua
#
# Created:      5/13/05
# CVS-ID:       $Id$
# Copyright:    (c) 2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------

import wx
import os.path
import ProjectEditor
import MessageService

import sys        # for errors
import traceback  # for errors

try:
    import pysvn  # python-subversion integration
    SVN_INSTALLED = True
except ImportError:
    SVN_INSTALLED = False

_ = wx.GetTranslation


#----------------------------------------------------------------------------
# Constants
#----------------------------------------------------------------------------
SVN_CONFIG_DIR = "SVNConfigDir"
SVN_REPOSITORY_URL = "SVNRepositoryURL"

SPACE = 10
HALF_SPACE = 5


#----------------------------------------------------------------------------
# Classes
#----------------------------------------------------------------------------

class SVNService(wx.lib.pydocview.DocService):
    SVN_UPDATE_ID = wx.NewId()
    SVN_CHECKIN_ID = wx.NewId()
    SVN_CHECKOUT_ID = wx.NewId()
    SVN_REVERT_ID = wx.NewId()
    SVN_ADD_ID = wx.NewId()
    SVN_DELETE_ID = wx.NewId()
    SVN_COMMAND_LIST = [SVN_UPDATE_ID, SVN_CHECKIN_ID, SVN_CHECKOUT_ID, SVN_REVERT_ID, SVN_ADD_ID, SVN_DELETE_ID]


    def __init__(self):
        self._defaultURL = "svn://"

        global SVN_INSTALLED
        if SVN_INSTALLED:
            config = wx.ConfigBase_Get()
            configDir = config.Read(SVN_CONFIG_DIR, "")

            self._client = pysvn.Client(configDir)
            try:
                self._defaultURL = self._client.info('.').url
            except:
                pass
            self._client.callback_cancel = self.IfCancel
            self._client.callback_notify = self.UpdateStatus
            self._client.callback_get_log_message = self.GetLogMessage
            self._client.callback_get_login = self.GetLogin
            self._client.callback_ssl_server_trust_prompt = self.GetSSLServerTrust
            self._client.callback_ssl_client_cert_password_prompt = self.SSLClientPassword
            self._client.callback_ssl_client_cert_prompt = self.SSLClientCert
            self._client.callback_ssl_server_prompt = self.SSLServerPrompt

    #----------------------------------------------------------------------------
    # pysvn.Client() Callback Methods
    #----------------------------------------------------------------------------

    def IfCancel(self):
        """ return True if user wants to cancel current command """
        return False


    def UpdateStatus(self, eventDict):
        messageService = wx.GetApp().GetService(MessageService.MessageService)
        messageService.ShowWindow()

        view = messageService.GetView()
        view.AddLines(_("%s %s\n") % (eventDict['action'], eventDict['path']))


    def GetLogMessage(self):
        dlg = wx.TextEntryDialog(wx.GetApp().GetTopWindow(),
                _("Comment"),
                _("SVN Log Message"))

        if dlg.ShowModal() == wx.ID_OK:
            retcode = True
            message = dlg.GetValue()
        else:
            retcode = False
            message = _("Cancel Action")

        dlg.Destroy()

        return retcode, message


    def GetLogin(self, realm, username, maySave):
        dlg = wx.Dialog(wx.GetApp().GetTopWindow(), -1, _("SVN Login"))

        sizer = wx.FlexGridSizer(cols = 2, hgap = 5, vgap = 5)
        sizer.Add(wx.StaticText(dlg, -1, _("Username:")), 0, wx.ALIGN_CENTER_VERTICAL)
        usernameTxt = wx.TextCtrl(dlg, -1, username, size = (200, -1))
        sizer.Add(usernameTxt, 0, wx.ALIGN_CENTER_VERTICAL)
        sizer.Add(wx.StaticText(dlg, -1, _("Password:")), 0, wx.ALIGN_CENTER_VERTICAL)
        passwordTxt = wx.TextCtrl(dlg, -1, size=(200, -1), style=wx.TE_PASSWORD)
        sizer.Add(passwordTxt, 0, wx.ALIGN_CENTER_VERTICAL)

        savePasswordCheckbox = wx.CheckBox(dlg, -1, _("Remember Username and Password"))
        if not maySave:
            savePasswordCheckbox.Enable(False)

        buttonSizer = wx.BoxSizer(wx.HORIZONTAL)
        okBtn = wx.Button(dlg, wx.ID_OK)
        okBtn.SetDefault()
        buttonSizer.Add(okBtn, 0, wx.RIGHT, HALF_SPACE)
        buttonSizer.Add(wx.Button(dlg, wx.ID_CANCEL), 0)

        contentSizer = wx.BoxSizer(wx.VERTICAL)
        contentSizer.Add(sizer, 0, wx.LEFT|wx.TOP|wx.RIGHT, SPACE)
        contentSizer.Add(savePasswordCheckbox, 0, wx.TOP|wx.LEFT|wx.BOTTOM, SPACE)
        contentSizer.Add(buttonSizer, 0, wx.ALL|wx.ALIGN_RIGHT, SPACE)

        dlg.SetSizer(contentSizer)
        dlg.Fit()
        dlg.Layout()

        if dlg.ShowModal() == wx.ID_OK:
            retcode = True
            username = usernameTxt.GetValue().strip()
            password = passwordTxt.GetValue()
            save = savePasswordCheckBox.IsChecked()
        else:
            retcode = False
            username = None
            password = None
            save = False

        dlg.Destroy()
        return retcode, username, password, save


    def SSLServerPrompt(self):
        """ Not implemented, as per pysvn documentation """
        return


    def GetSSLServerTrust(self, trustDict):
        dlg = wx.Dialog(wx.GetApp().GetTopWindow(), -1, _("SSL Server Certificate"))

        sizer = wx.FlexGridSizer(cols = 2, hgap = 5, vgap = 5)
        for k in ['hostname', 'valid_from', 'valid_to', 'issuer_dname', 'realm']:
            if trustDict.has_key(k):
                sizer.Add(wx.StaticText(dlg, -1, "%s:" % k), 0, wx.ALIGN_CENTER_VERTICAL)
                sizer.Add(wx.StaticText(dlg, -1, "%s"  % trustDict[k]), 0, wx.ALIGN_CENTER_VERTICAL)

        box = wx.StaticBoxSizer(wx.StaticBox(dlg, -1, _("Certificate Info")), wx.VERTICAL)
        box.Add(sizer, 0, wx.EXPAND)

        certRadio = wx.RadioBox(dlg, -1, _("Certificate"), choices=[_("Accept Always"), _("Accept Once"), _("Reject")], majorDimension=1, style=wx.RA_SPECIFY_COLS)

        buttonSizer = wx.BoxSizer(wx.HORIZONTAL)
        okBtn = wx.Button(dlg, wx.ID_OK)
        okBtn.SetDefault()
        buttonSizer.Add(okBtn, 0, wx.RIGHT, HALF_SPACE)
        buttonSizer.Add(wx.Button(dlg, wx.ID_CANCEL), 0)

        contentSizer = wx.BoxSizer(wx.VERTICAL)
        contentSizer.Add(box, 0, wx.TOP|wx.LEFT|wx.RIGHT|wx.EXPAND, SPACE)
        contentSizer.Add(certRadio, 0, wx.TOP|wx.LEFT|wx.RIGHT|wx.BOTTOM|wx.EXPAND, SPACE)
        contentSizer.Add(buttonSizer, 0, wx.ALL|wx.ALIGN_RIGHT, SPACE)

        dlg.SetSizer(contentSizer)
        dlg.Fit()
        dlg.Layout()

        # default values for reject
        retcode = False
        acceptedFailures = 0
        save = False

        if dlg.ShowModal() == wx.ID_OK:
            cert = certRadio.GetStringSelection()
            if cert == _("Accept Always"):
                retcode = True
                acceptedFailures = trustDict.get('failures')
                save = True
            elif cert == _("Accept Once"):
                retcode = True
                acceptedFailures = trustDict.get('failures')
                save = False

        return retcode, acceptedFailures, save


    def SSLClientPassword(self, realm, maySave):
        dlg = wx.Dialog(wx.GetApp().GetTopWindow(), -1, _("SSL Client Certificate Login"))

        sizer = wx.FlexGridSizer(cols = 2, hgap = 5, vgap = 5)
        sizer.Add(wx.StaticText(dlg, -1, _("Realm:")), 0, wx.ALIGN_CENTER_VERTICAL)
        sizer.Add(wx.StaticText(dlg, -1, realm), 0, wx.ALIGN_CENTER_VERTICAL)
        sizer.Add(wx.StaticText(dlg, -1, _("Password:")), 0, wx.ALIGN_CENTER_VERTICAL)
        passwordTxt = wx.TextCtrl(dlg, -1, size=(200, -1), style=wx.TE_PASSWORD)
        sizer.Add(passwordTxt, 0, wx.ALIGN_CENTER_VERTICAL)

        savePasswordCheckbox = wx.CheckBox(dlg, -1, _("Remember Password"))
        if not maySave:
            savePasswordCheckbox.Enable(False)

        buttonSizer = wx.BoxSizer(wx.HORIZONTAL)
        okBtn = wx.Button(dlg, wx.ID_OK)
        okBtn.SetDefault()
        buttonSizer.Add(okBtn, 0, wx.RIGHT, HALF_SPACE)
        buttonSizer.Add(wx.Button(dlg, wx.ID_CANCEL), 0)

        contentSizer = wx.BoxSizer(wx.VERTICAL)
        contentSizer.Add(sizer, 0, wx.LEFT|wx.TOP|wx.RIGHT, SPACE)
        contentSizer.Add(savePasswordCheckbox, 0, wx.TOP|wx.LEFT|wx.BOTTOM, SPACE)
        contentSizer.Add(buttonSizer, 0, wx.ALL|wx.ALIGN_RIGHT, SPACE)

        dlg.SetSizer(contentSizer)
        dlg.Fit()
        dlg.Layout()

        if dlg.ShowModal() == wx.ID_OK:
            retcode = True
            password = passwordTxt.GetValue()
            save = savePasswordCheckBox.IsChecked()
        else:
            retcode = False
            password = None
            save = False

        dlg.Destroy()
        return retcode, password, save


    def SSLClientCert(self):
        dlg = wx.FileDialog(wx.GetApp().GetTopWindow(),
            message="Choose certificate", defaultDir=os.getcwd(), 
            style=wx.OPEN|wx.CHANGE_DIR
            )
            
        if dlg.ShowModal() == wx.ID_OK:
            retcode = True
            certfile = dlg.GetPath()
        else:
            retcode = False
            certfile = None
            
        dlg.Destroy()
        return retcode, certfile



    #----------------------------------------------------------------------------
    # Service Methods
    #----------------------------------------------------------------------------

    def InstallControls(self, frame, menuBar = None, toolBar = None, statusBar = None, document = None):
        menu = menuBar.GetMenu(menuBar.FindMenu(_("Project")))

        menu.AppendSeparator()

        wx.EVT_MENU(frame, SVNService.SVN_UPDATE_ID, self.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, SVNService.SVN_UPDATE_ID, self.ProcessUpdateUIEvent)
        menu.Append(SVNService.SVN_UPDATE_ID, _("SVN Update"), _("Update file from Subversion"))
        wx.EVT_MENU(frame, SVNService.SVN_CHECKIN_ID, self.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, SVNService.SVN_CHECKIN_ID, self.ProcessUpdateUIEvent)
        menu.Append(SVNService.SVN_CHECKIN_ID, _("SVN Commit..."), _("Commit file changes to Subversion"))
        wx.EVT_MENU(frame, SVNService.SVN_CHECKOUT_ID, self.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, SVNService.SVN_CHECKOUT_ID, self.ProcessUpdateUIEvent)
        menu.Append(SVNService.SVN_CHECKOUT_ID, _("SVN Checkout..."), _("Checkout file from Subversion"))
        wx.EVT_MENU(frame, SVNService.SVN_REVERT_ID, self.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, SVNService.SVN_REVERT_ID, self.ProcessUpdateUIEvent)
        menu.Append(SVNService.SVN_REVERT_ID, _("SVN Revert"), _("Revert file from Subversion"))

        menu.AppendSeparator()

        wx.EVT_MENU(frame, SVNService.SVN_ADD_ID, self.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, SVNService.SVN_ADD_ID, self.ProcessUpdateUIEvent)
        menu.Append(SVNService.SVN_ADD_ID, _("SVN Add"), _("Add file to Subversion"))
        wx.EVT_MENU(frame, SVNService.SVN_DELETE_ID, self.ProcessEvent)
        wx.EVT_UPDATE_UI(frame, SVNService.SVN_DELETE_ID, self.ProcessUpdateUIEvent)
        menu.Append(SVNService.SVN_DELETE_ID, _("SVN Delete"), _("Delete file from Subversion"))


    def ProcessEvent(self, event):

        id = event.GetId()

        if not SVN_INSTALLED:
            if id in SVNService.SVN_COMMAND_LIST:
                wx.MessageBox(_("pysvn not found.  Please install pysvn"), _("Python Subversion"))
                return True
            return False


        if id == SVNService.SVN_UPDATE_ID:
            filenames = self.GetCurrentDocuments()

            wx.GetApp().GetTopWindow().SetCursor(wx.StockCursor(wx.CURSOR_WAIT))

            messageService = wx.GetApp().GetService(MessageService.MessageService)
            messageService.ShowWindow()

            view = messageService.GetView()
            view.ClearLines()
            view.AddLines(_("SVN Update:\n"))

            for filename in filenames:
                view.AddLines("%s\n" % filename)
                try:
                    status = self._client.update(filename)
    
                    if status.number > 0:
                        view.AddLines(_("Updated to revision %s\n") % status.number)

                        openDocs = wx.GetApp().GetDocumentManager().GetDocuments()
                        for doc in openDocs:
                            if doc.GetFilename() == filename:
                                yesNoMsg = wx.MessageDialog(wx.GetApp().GetTopWindow(),
                                                         _("Updated file '%s' is currently open.  Close it?") % os.path.basename(filename),
                                                         _("Close File"),
                                                         wx.YES_NO|wx.ICON_QUESTION)
                                if yesNoMsg.ShowModal() == wx.ID_YES:
                                    doc.DeleteAllViews()
                                break
                    else:
                        view.AddLines(_("Update failed.\n"))
    
                except pysvn.ClientError, e:
                    view.AddLines("%s\n" % str(e))
                    wx.MessageBox(str(e), _("SVN Update"), wx.OK | wx.ICON_EXCLAMATION)
                except:
                    extype, ex, tb = sys.exc_info()
                    view.AddLines("Update failed: (%s) %s\n" % (extype, str(ex)))
                    for line in traceback.format_tb(tb):
                        view.AddLines(line)
    
                    wx.MessageBox(_("Update failed."), _("SVN Update"), wx.OK | wx.ICON_EXCLAMATION)

            wx.GetApp().GetTopWindow().SetCursor(wx.StockCursor(wx.CURSOR_DEFAULT))

            return True

        elif id == SVNService.SVN_CHECKIN_ID:
            filenames = self.GetCurrentDocuments()

            # ask user if dirty files should be saved first
            openDocs = wx.GetApp().GetDocumentManager().GetDocuments()
            for filename in filenames:
                for doc in openDocs:
                    if doc.GetFilename() == filename and doc.IsModified():
                        yesNoMsg = wx.MessageDialog(wx.GetApp().GetTopWindow(),
                                                 _("'%s' has unsaved modifications.  Save it before commit?") % os.path.basename(filename),
                                                 _("SVN Commit"),
                                                 wx.YES_NO|wx.CANCEL|wx.ICON_QUESTION)
                        status = yesNoMsg.ShowModal()
                        if status == wx.ID_YES:
                            doc.Save()
                        elif status == wx.ID_NO:
                            pass
                        else: # elif status == wx.CANCEL:
                            return True
                        break

            shortFilenames = []
            for i, filename in enumerate(filenames):
                shortFilename = os.path.basename(filename)
                shortFilenames.append(shortFilename)

            dlg = wx.Dialog(wx.GetApp().GetTopWindow(), -1, _("SVN Commit"))

            sizer = wx.BoxSizer(wx.VERTICAL)
            sizer.Add(wx.StaticText(dlg, -1, _("Comment:")), 0, wx.ALIGN_CENTER_VERTICAL)
            commentText = wx.TextCtrl(dlg, -1, size=(250,-1), style=wx.TE_MULTILINE)
            sizer.Add(commentText, 1, wx.EXPAND|wx.TOP, HALF_SPACE)

            sizer.Add(wx.StaticText(dlg, -1, _("Files:")), 0, wx.ALIGN_CENTER_VERTICAL|wx.TOP, SPACE)
            fileList = wx.CheckListBox(dlg, -1, choices = shortFilenames)
            for i in range(fileList.GetCount()):
                fileList.Check(i, True)
            sizer.Add(fileList, 0, wx.EXPAND|wx.TOP, HALF_SPACE)

            buttonSizer = wx.BoxSizer(wx.HORIZONTAL)
            okBtn = wx.Button(dlg, wx.ID_OK)
            okBtn.SetDefault()
            buttonSizer.Add(okBtn, 0, wx.RIGHT, HALF_SPACE)
            buttonSizer.Add(wx.Button(dlg, wx.ID_CANCEL), 0)

            contentSizer = wx.BoxSizer(wx.VERTICAL)
            contentSizer.Add(sizer, 0, wx.ALL, SPACE)
            contentSizer.Add(buttonSizer, 0, wx.ALL|wx.ALIGN_RIGHT, SPACE)

            dlg.SetSizer(contentSizer)
            dlg.Fit()
            dlg.Layout()

            if dlg.ShowModal() == wx.ID_OK:
                wx.GetApp().GetTopWindow().SetCursor(wx.StockCursor(wx.CURSOR_WAIT))

                messageService = wx.GetApp().GetService(MessageService.MessageService)
                messageService.ShowWindow()

                view = messageService.GetView()
                view.ClearLines()
                view.AddLines(_("SVN Commit:\n"))

                try:
                    selFilenames = []
                    for i in range(fileList.GetCount()):
                        if fileList.IsChecked(i):
                            selFilenames.append(filenames[i])
                            view.AddLines("%s\n" % filenames[i])
                        
                    if len(selFilenames):
                        comment = commentText.GetValue()
                        status = self._client.checkin(selFilenames, comment)

                        if status is None:
                            view.AddLines(_("Nothing to commit.\n"))
                        elif status.number > 0:
                            view.AddLines(_("Committed as revision %s.\n") % status.number)
                        else:
                            view.AddLines(_("Commit failed.\n"))

                except pysvn.ClientError, e:
                    view.AddLines("%s\n" % str(e))
                    wx.MessageBox(str(e), _("SVN Commit"), wx.OK | wx.ICON_EXCLAMATION)
                except:
                    extype, ex, tb = sys.exc_info()
                    view.AddLines("Commit failed: (%s) %s\n" % (extype, str(ex)))
                    for line in traceback.format_tb(tb):
                        view.AddLines(line)
                    wx.MessageBox(_("Commit failed."), _("SVN Commit"), wx.OK | wx.ICON_EXCLAMATION)

                wx.GetApp().GetTopWindow().SetCursor(wx.StockCursor(wx.CURSOR_DEFAULT))
            dlg.Destroy()
            return True

        elif id == SVNService.SVN_CHECKOUT_ID:
            config = wx.ConfigBase_Get()
            svnUrl = config.Read(SVN_REPOSITORY_URL, self._defaultURL)

            dlg = wx.Dialog(wx.GetApp().GetTopWindow(), -1, _("SVN Checkout"))

            gridSizer = wx.FlexGridSizer(cols = 2, hgap = 5, vgap = 5)
            gridSizer.Add(wx.StaticText(dlg, -1, _("Repository URL:")), 0, wx.ALIGN_CENTER_VERTICAL|wx.RIGHT|wx.TOP, HALF_SPACE)
            svnURLText = wx.TextCtrl(dlg, -1, svnUrl, size = (200, -1))
            svnURLText.SetToolTipString(svnUrl)
            gridSizer.Add(svnURLText, 0)

            gridSizer.Add(wx.StaticText(dlg, -1, _("Checkout to dir:")), 0, wx.ALIGN_CENTER_VERTICAL|wx.RIGHT|wx.TOP, HALF_SPACE)
            localPath = wx.TextCtrl(dlg, -1, size = (200, -1))
            localPath.SetToolTipString(_("Path in local file system where files will be located."))
            findDirButton = wx.Button(dlg, -1, _("Browse..."))

            def OnBrowseButton(event):
                dirDlg = wx.DirDialog(wx.GetApp().GetTopWindow(), _("Choose a directory:"), style=wx.DD_DEFAULT_STYLE)
                dir = localPath.GetValue()
                if len(dir):
                    dirDlg.SetPath(dir)
                if dirDlg.ShowModal() == wx.ID_OK:
                    localPath.SetValue(dirDlg.GetPath())
                    localPath.SetToolTipString(localPath.GetValue())
                    localPath.SetInsertionPointEnd()

                dirDlg.Destroy()
            wx.EVT_BUTTON(findDirButton, -1, OnBrowseButton)

            sizer = wx.BoxSizer(wx.HORIZONTAL)
            sizer.Add(localPath, 1, wx.EXPAND)
            sizer.Add(findDirButton, 0, wx.LEFT, HALF_SPACE)
            gridSizer.Add(sizer, 0)

            buttonSizer = wx.BoxSizer(wx.HORIZONTAL)
            okBtn = wx.Button(dlg, wx.ID_OK)
            okBtn.SetDefault()
            buttonSizer.Add(okBtn, 0, wx.RIGHT, HALF_SPACE)
            buttonSizer.Add(wx.Button(dlg, wx.ID_CANCEL), 0)

            contentSizer = wx.BoxSizer(wx.VERTICAL)
            contentSizer.Add(gridSizer, 0, wx.ALL, SPACE)
            contentSizer.Add(buttonSizer, 0, wx.ALL|wx.ALIGN_RIGHT, SPACE)

            dlg.SetSizer(contentSizer)
            dlg.Fit()
            dlg.Layout()

            if dlg.ShowModal() == wx.ID_OK:
                wx.GetApp().GetTopWindow().SetCursor(wx.StockCursor(wx.CURSOR_WAIT))

                messageService = wx.GetApp().GetService(MessageService.MessageService)
                messageService.ShowWindow()

                view = messageService.GetView()
                view.ClearLines()
                view.AddLines(_("SVN Checkout:\n"))

                svnUrl = svnURLText.GetValue()
                toLocation = localPath.GetValue()
                try:
                    self._client.checkout(svnUrl, toLocation)
                    view.AddLines(_("Checkout completed.\n"))
                except pysvn.ClientError, e:
                    view.AddLines(_("Checkout failed.  %s\n") % str(e))
                    wx.MessageBox(_("Checkout failed.  %s") % str(e), _("SVN Checkout"), wx.OK | wx.ICON_EXCLAMATION)
                except:
                    extype, ex, tb = sys.exc_info()
                    view.AddLines("Checkout failed: (%s) %s\n" % (extype, str(ex)))
                    for line in traceback.format_tb(tb):
                        view.AddLines(line)
                    wx.MessageBox(_("Checkout failed."), _("SVN Checkout"), wx.OK | wx.ICON_EXCLAMATION)

                wx.GetApp().GetTopWindow().SetCursor(wx.StockCursor(wx.CURSOR_DEFAULT))
            dlg.Destroy()
            return True

        elif id == SVNService.SVN_REVERT_ID:
            wx.GetApp().GetTopWindow().SetCursor(wx.StockCursor(wx.CURSOR_WAIT))

            filenames = self.GetCurrentDocuments()

            messageService = wx.GetApp().GetService(MessageService.MessageService)
            messageService.ShowWindow()

            view = messageService.GetView()
            view.ClearLines()
            view.AddLines(_("SVN Revert:\n"))
            for filename in filenames:
                view.AddLines("%s\n" % filename)

            try:
                self._client.revert(filenames)
                view.AddLines(_("Revert completed.\n"))
                
                openDocs = wx.GetApp().GetDocumentManager().GetDocuments()
                for doc in openDocs[:]:   # need to make a copy of the list otherwise ordinality changes as we close the files
                    if doc.GetFilename() in filenames:
                        yesNoMsg = wx.MessageDialog(wx.GetApp().GetTopWindow(),
                                                 _("Reverted file '%s' is currently open.  Close it?") % os.path.basename(doc.GetFilename()),
                                                 _("Close File"),
                                                 wx.YES_NO|wx.ICON_QUESTION)
                        if yesNoMsg.ShowModal() == wx.ID_YES:
                            doc.DeleteAllViews()

            except pysvn.ClientError, e:
                view.AddLines("%s\n" % str(e))
                wx.MessageBox(str(e), _("SVN Revert"), wx.OK | wx.ICON_EXCLAMATION)
            except:
                extype, ex, tb = sys.exc_info()
                view.AddLines("Revert failed: (%s) %s\n" % (extype, str(ex)))
                for line in traceback.format_tb(tb):
                    view.AddLines(line)
                wx.MessageBox(_("Revert failed."), _("SVN Revert"), wx.OK | wx.ICON_EXCLAMATION)

            wx.GetApp().GetTopWindow().SetCursor(wx.StockCursor(wx.CURSOR_DEFAULT))
            return True

        elif id == SVNService.SVN_ADD_ID:
            wx.GetApp().GetTopWindow().SetCursor(wx.StockCursor(wx.CURSOR_WAIT))

            filenames = self.GetCurrentDocuments()

            messageService = wx.GetApp().GetService(MessageService.MessageService)
            messageService.ShowWindow()

            view = messageService.GetView()
            view.ClearLines()
            view.AddLines(_("SVN Add:\n"))
            for filename in filenames:
                view.AddLines("%s\n" % filename)

            try:
                self._client.add(filenames)
                view.AddLines(_("Add completed.\n"))
            except pysvn.ClientError, e:
                view.AddLines("%s\n" % str(e))
                wx.MessageBox(str(e), _("SVN Add"), wx.OK | wx.ICON_EXCLAMATION)
            except:
                extype, ex, tb = sys.exc_info()
                view.AddLines("Add failed: (%s) %s\n" % (extype, str(ex)))
                for line in traceback.format_tb(tb):
                    view.AddLines(line)
                wx.MessageBox(_("Add failed."), _("SVN Add"), wx.OK | wx.ICON_EXCLAMATION)

            wx.GetApp().GetTopWindow().SetCursor(wx.StockCursor(wx.CURSOR_DEFAULT))
            return True

        elif id == SVNService.SVN_DELETE_ID:
            wx.GetApp().GetTopWindow().SetCursor(wx.StockCursor(wx.CURSOR_WAIT))

            filenames = self.GetCurrentDocuments()

            messageService = wx.GetApp().GetService(MessageService.MessageService)
            messageService.ShowWindow()

            view = messageService.GetView()
            view.ClearLines()
            view.AddLines(_("SVN Delete:\n"))
            for filename in filenames:
                view.AddLines("%s\n" % filename)

            try:
                self._client.remove(filenames)
                view.AddLines(_("Delete completed.\n"))
            except pysvn.ClientError, e:
                view.AddLines("%s\n" % str(e))
                wx.MessageBox(str(e), _("SVN Delete"), wx.OK | wx.ICON_EXCLAMATION)
            except:
                extype, ex, tb = sys.exc_info()
                view.AddLines("Delete failed: (%s) %s\n" % (extype, str(ex)))
                for line in traceback.format_tb(tb):
                    view.AddLines(line)
                wx.MessageBox(_("Delete failed."), _("SVN Delete"), wx.OK | wx.ICON_EXCLAMATION)

            wx.GetApp().GetTopWindow().SetCursor(wx.StockCursor(wx.CURSOR_DEFAULT))
            return True

        return False


    def ProcessUpdateUIEvent(self, event):
        id = event.GetId()

        if id in [SVNService.SVN_UPDATE_ID,
                  SVNService.SVN_CHECKIN_ID,
                  SVNService.SVN_REVERT_ID,
                  SVNService.SVN_ADD_ID,
                  SVNService.SVN_DELETE_ID]:
            if self.GetCurrentDocuments():
                event.Enable(True)
            else:
                event.Enable(False)
            return True

        elif id == SVNService.SVN_CHECKOUT_ID:
            event.Enable(True)
            return True

        return False


    def GetCurrentDocuments(self):

        projectService = wx.GetApp().GetService(ProjectEditor.ProjectService)
        if projectService:
            projView = projectService.GetView()

            if projView.HasFocus():
                filenames = projView.GetSelectedFiles()
                if len(filenames):
                    return filenames
                else:
                    return None

        doc = wx.GetApp().GetTopWindow().GetDocumentManager().GetCurrentDocument()
        if doc:
            filenames = [doc.GetFilename()]
        else:
            filenames = None

        return filenames


class SVNOptionsPanel(wx.Panel):


    def __init__(self, parent, id):
        wx.Panel.__init__(self, parent, id)

        config = wx.ConfigBase_Get()
        svnService = wx.GetApp().GetService(SVNService)
        svnUrl = config.Read(SVN_REPOSITORY_URL, svnService._defaultURL)
        configDir = config.Read(SVN_CONFIG_DIR, "")

        borderSizer = wx.BoxSizer(wx.VERTICAL)
        sizer = wx.FlexGridSizer(cols = 2, hgap = 5, vgap = 5)

        sizer.Add(wx.StaticText(self, -1, _("SVN Config Dir:")), 0, wx.ALIGN_CENTER_VERTICAL)

        self._svnConfigDir = wx.TextCtrl(self, -1, configDir, size = (200, -1))
        if configDir == "":
            self._svnConfigDir.SetToolTipString(_("Path Subversion configuration directory."))
        else:
            self._svnConfigDir.SetToolTipString(configDir)
        
        findDirButton = wx.Button(self, -1, _("Browse..."))

        def OnBrowseButton(event):
            dirDlg = wx.DirDialog(self, _("Choose a directory:"), style=wx.DD_DEFAULT_STYLE)
            dir = self._svnConfigDir.GetValue()
            if len(dir):
                dirDlg.SetPath(dir)
            if dirDlg.ShowModal() == wx.ID_OK:
                self._svnConfigDir.SetValue(dirDlg.GetPath())
                self._svnConfigDir.SetToolTipString(self._svnConfigDir.GetValue())
                self._svnConfigDir.SetInsertionPointEnd()

            dirDlg.Destroy()
        wx.EVT_BUTTON(findDirButton, -1, OnBrowseButton)

        hsizer = wx.BoxSizer(wx.HORIZONTAL)
        hsizer.Add(self._svnConfigDir, 1, wx.EXPAND)
        hsizer.Add(findDirButton, 0, wx.LEFT, HALF_SPACE)
        sizer.Add(hsizer, 0)

        sizer.Add(wx.StaticText(self, -1, _("SVN URL:")), 0, wx.ALIGN_CENTER_VERTICAL)
        self._svnURLText = wx.TextCtrl(self, -1, svnUrl, size = (200, -1))
        self._svnURLText.SetToolTipString(svnUrl)
        sizer.Add(self._svnURLText, 0)

        borderSizer.Add(sizer, 0, wx.ALL, SPACE)
        self.SetSizer(borderSizer)
        self.Layout()
        parent.AddPage(self, _("SVN"))


    def OnOK(self, optionsDialog):
        config = wx.ConfigBase_Get()
        config.Write(SVN_CONFIG_DIR, self._svnConfigDir.GetValue())
        config.Write(SVN_REPOSITORY_URL, self._svnURLText.GetValue())

