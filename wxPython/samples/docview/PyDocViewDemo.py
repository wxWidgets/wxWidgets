#----------------------------------------------------------------------------
# Name:         PyDocViewDemo.py
# Purpose:      Demo of Python extensions to the wxWindows docview framework
#
# Author:       Peter Yared, Morgan Hua
#
# Created:      5/15/03
# CVS-ID:       $Id$
# Copyright:    (c) 2003 ActiveGrid, Inc.
# License:      wxWindows license
#----------------------------------------------------------------------------


import sys
import wx
import wx.lib.docview
import wx.lib.pydocview
import activegrid.tool.TextEditor as TextEditor
import activegrid.tool.FindService as FindService
_ = wx.GetTranslation


#----------------------------------------------------------------------------
# Classes
#----------------------------------------------------------------------------

class TextEditorApplication(wx.lib.pydocview.DocApp):


    def OnInit(self):
        wx.lib.pydocview.DocApp.OnInit(self)

        wx.lib.pydocview.DocApp.ShowSplash(self, "activegrid/tool/images/splash.jpg")

        self.SetAppName(_("wxPython PyDocView Demo"))
        config = wx.Config(self.GetAppName(), style = wx.CONFIG_USE_LOCAL_FILE)

        docManager = wx.lib.docview.DocManager(flags = self.GetDefaultDocManagerFlags())
        self.SetDocumentManager(docManager)

        textTemplate = wx.lib.docview.DocTemplate(docManager,
                                              _("Text"),
                                              "*.text;*.txt",
                                              _("Text"),
                                              _(".txt"),
                                              _("Text Document"),
                                              _("Text View"),
                                              TextEditor.TextDocument,
                                              TextEditor.TextView)
        docManager.AssociateTemplate(textTemplate)

        textService       = self.InstallService(TextEditor.TextService())
        findService       = self.InstallService(FindService.FindService())
        optionsService    = self.InstallService(wx.lib.pydocview.DocOptionsService())
        windowMenuService = self.InstallService(wx.lib.pydocview.WindowMenuService())
        optionsService.AddOptionsPanel(TextEditor.TextOptionsPanel)
        filePropertiesService = self.InstallService(wx.lib.pydocview.FilePropertiesService())
        aboutService         = self.InstallService(wx.lib.pydocview.AboutService())

##        self.SetDefaultIcon(getAppIcon())  # set this for your custom icon

        if docManager.GetFlags() & wx.lib.docview.DOC_MDI:
            frame = wx.lib.pydocview.DocMDIParentFrame(docManager, None, -1, wx.GetApp().GetAppName())
            frame.Show(True)

        wx.lib.pydocview.DocApp.CloseSplash(self)
        
        self.OpenCommandLineArgs()

        if not docManager.GetDocuments() and docManager.GetFlags() & wx.lib.docview.DOC_SDI:
            textTemplate.CreateDocument('', wx.lib.docview.DOC_NEW).OnNewDocument()

        wx.CallAfter(self.ShowTip, wx.GetApp().GetTopWindow(), wx.CreateFileTipProvider("activegrid/tool/data/tips.txt", 0))

        return True


#----------------------------------------------------------------------------
# Main
#----------------------------------------------------------------------------

sys.stdout = sys.stderr

app = TextEditorApplication(redirect = False)
app.MainLoop()
