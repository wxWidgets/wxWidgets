#----------------------------------------------------------------------------
# Name:         DocViewDemo.py
# Purpose:      Port of the wxWindows docview sample classes
#
# Author:       Peter Yared
#
# Created:      8/1/03
# CVS-ID:       $Id$
# Copyright:    (c) 2003-2005 ActiveGrid, Inc. (Port of wxWindows classes by Julian Smart et al)
# License:      wxWindows license
#----------------------------------------------------------------------------



#----------------------------------------------------------------------
# This isn't the most object oriented code (it is somewhat repetitive,
# but it is true to the wxWindows C++ demos.
#----------------------------------------------------------------------


import wx
import wx.lib.docview
_ = wx.GetTranslation


#----------------------------------------------------------------------
#----------------------------------------------------------------------
# TextEdit document and view classes

class TextEditDocument(wx.lib.docview.Document):


    def OnSaveDocument(self, filename):
        view = self.GetFirstView()
        if not view.GetTextSW().SaveFile(filename):
            return False
        self.Modify(False)
        self.SetDocumentSaved(True)
##        if wx.Platform == "__WXMAC__":
##            fn = wx.Filename(filename)
##            fn.MacSetDefaultTypeAndCreator()
        return True


    def OnOpenDocument(self, filename):
        view = self.GetFirstView()
        if not view.GetTextSW().LoadFile(filename):
            return False
        self.SetFilename(filename, True)
        self.Modify(False)
        self.UpdateAllViews()
        self._savedYet = True
        return True


    def IsModified(self):
        view = self.GetFirstView()
        if view:
            return wx.lib.docview.Document.IsModified(self) or (view.GetTextSW() and view.GetTextSW().IsModified())
        else:
            return wx.lib.docview.Document.IsModified(self)


    def Modify(self, mod):
        view = self.GetFirstView()
        wx.lib.docview.Document.Modify(self, mod)
        if not mod and view and view.GetTextSW():
            view.GetTextSW().DiscardEdits()


class TextEditView(wx.lib.docview.View):


    def OnCreate(self, doc, flags):
        flags = doc.GetDocumentManager().GetFlags()
        if flags & wx.lib.docview.DOC_SDI and doc.GetDocumentManager().GetMaxDocsOpen() == 1:
            self._frame = wx.GetApp().GetMainFrame()
            self.SetFrame(self._frame)
            sizer = wx.BoxSizer()
            self._textsw = MyTextWindow(self, self._frame, wx.DefaultPosition, wx.DefaultSize, wx.TE_MULTILINE)
            sizer.Add(self._textsw, 1, wx.EXPAND, 0)
            self._frame.SetSizer(sizer)
            self._frame.Layout()
            self.Activate(True)
            return True
        elif flags & wx.lib.docview.DOC_MDI:
            self._frame = wx.lib.docview.DocMDIChildFrame(doc, self, wx.GetApp().GetMainFrame(), -1, wx.GetApp().GetAppName(), (10, 10), (300, 300), wx.DEFAULT_FRAME_STYLE)
            self.SetFrame(self._frame)
            sizer = wx.BoxSizer()
            self._textsw = MyTextWindow(self, self._frame, wx.DefaultPosition, wx.DefaultSize, wx.TE_MULTILINE)
            sizer.Add(self._textsw, 1, wx.EXPAND, 0)
            self._frame.SetSizer(sizer)
            self._frame.Layout()
            self._frame.Show(True)
            self.Activate(True)
            return True
        else:  # flags & wx.lib.docview.DOC_SDI
            self._frame = wx.GetApp().CreateChildFrame(doc, self)
            width, height = self._frame.GetClientSize()
            self._textsw = MyTextWindow(self, self._frame, (0, 0), (width, height), wx.TE_MULTILINE)
            self._frame.SetTitle(_("TextEditView"))
            self._frame.Show(True)
            self.Activate(True)
            return True


    # Since ProcessEvent is not virtual, we have to trap the relevant events using this pseudo-ProcessEvent instead of EVT_MENU
    def ProcessEvent(self, event):
        id = event.GetId()
        if id == wx.ID_UNDO:
            self._textsw.Undo()
            return True
        elif id == wx.ID_REDO:
            self._textsw.Redo()
            return True
        else:
            return wx.lib.docview.View.ProcessEvent(self, event)


    def GetTextSW(self):
        return self._textsw


    def OnDraw(self, dc):
        pass


    def OnClose(self, deleteWindow = True):
        if not self.GetDocument().Close():
            return False
        self.Activate(False)
        if deleteWindow:
            if self.GetDocument().GetDocumentManager().GetMaxDocsOpen() == 1 and self.GetDocument().GetDocumentManager().GetFlags() & wx.lib.docview.DOC_SDI:
                if self._textsw:
                    self._textsw.Destroy()
                return True
            else:
                self._frame.Destroy()
                return True
        return True


class MyTextWindow(wx.TextCtrl):


    def __init__(self, view, frame, pos, size, style):
        wx.TextCtrl.__init__(self, frame, -1, "", pos, size, style)
        self._view = view



#----------------------------------------------------------------------
#----------------------------------------------------------------------
# TextEdit Sample Application


class MyApp(wx.PySimpleApp):


    def OnInit(self):


        demoMode = wx.GetSingleChoiceIndex(_("Select the demo mode"),
                                           _("wxPython DocView Demo"),
                                           [_("SDI Single Document"), _("SDI"), _("MDI")])

        if demoMode == 0 or demoMode == 1:
            flags = wx.lib.docview.DOC_SDI
        elif demoMode == 2:
            flags = wx.lib.docview.DOC_MDI
        else:
            return False

        self.SetAppName(_("DocView Demo"))

        docManager = wx.lib.docview.DocManager(flags = flags)
        docManager.AssociateTemplate(wx.lib.docview.DocTemplate(docManager,
                                                         _("Text"),
                                                         "*.text;*.txt",
                                                         _("Text"),
                                                         _(".txt"),
                                                         _("Text Document"),
                                                         _("Text View"),
                                                         TextEditDocument,
                                                         TextEditView))
        #if wx.Platform == "__WXMAC__":
        #     wx.FileName.MacRegisterDefaultTypeAndCreator("txt", 'TEXT', 'WXMA')

        if demoMode == 0:
            docManager.SetMaxDocsOpen(1)

        if demoMode == 2:  # MDI
            self._frame = MyMDIFrame(docManager, None, -1, _("DocView Demo"), (0, 0), (500, 400), wx.DEFAULT_FRAME_STYLE)
        else:  # SDI
            self._frame = MyFrame(docManager, None, -1, _("DocView Demo"), (0, 0), (500, 400), wx.DEFAULT_FRAME_STYLE)

        fileMenu = wx.Menu()
        editMenu = None

        fileMenu.Append(wx.ID_NEW, _("&New..."))
        fileMenu.Append(wx.ID_OPEN, _("&Open..."))

        if demoMode == 2:  # MDI
            fileMenu.Append(wx.ID_CLOSE, _("&Close"))
            fileMenu.AppendSeparator()

        if demoMode == 0 or demoMode == 2:  # Single Doc or MDI
            fileMenu.Append(wx.ID_SAVE, _("&Save"))
            fileMenu.Append(wx.ID_SAVEAS, _("Save &As"))
            fileMenu.AppendSeparator()
            fileMenu.Append(wx.ID_PRINT, _("&Print"))
            fileMenu.Append(wx.ID_PRINT_SETUP, _("Page &Setup"))
            fileMenu.Append(wx.ID_PREVIEW, _("Print Pre&view"))

            editMenu = wx.Menu()
            editMenu.Append(wx.ID_UNDO, _("&Undo"))
            editMenu.Append(wx.ID_REDO, _("&Redo"))

            self._frame.editMenu = editMenu

        fileMenu.AppendSeparator()
        fileMenu.Append(wx.ID_EXIT, _("E&xit"))

        docManager.FileHistoryUseMenu(fileMenu)

        helpMenu = wx.Menu()
        helpMenu.Append(wx.ID_ABOUT, _("&About"))

        menuBar = wx.MenuBar()
        menuBar.Append(fileMenu, _("&File"))
        if editMenu:
            menuBar.Append(editMenu, _("&Edit"))
        menuBar.Append(helpMenu, _("&Help"))

        self._frame.SetMenuBar(menuBar)
        self._frame.Centre(wx.BOTH)
        self._frame.Show(True)

        self.SetTopWindow(self._frame)

        if demoMode == 0:  # Single doc
            docManager.OnFileNew(None)

        return True


    def GetMainFrame(self):
        return self._frame


    def GetDemoMode(self):
        return self._demoMode


    def CreateChildFrame(self, doc, view):
        subframe = wx.lib.docview.DocChildFrame(doc, view, self.GetMainFrame(), -1, wx.GetApp().GetAppName(), (10, 10), (300, 300), wx.DEFAULT_FRAME_STYLE)

        fileMenu = wx.Menu()
        fileMenu.Append(wx.ID_NEW, _("&New"))
        fileMenu.Append(wx.ID_OPEN, _("&Open"))
        fileMenu.Append(wx.ID_CLOSE, _("&Close"))
        fileMenu.AppendSeparator()
        fileMenu.Append(wx.ID_SAVE, _("&Save"))
        fileMenu.Append(wx.ID_SAVEAS, _("Save &As"))
        fileMenu.AppendSeparator()
        fileMenu.Append(wx.ID_PRINT, _("&Print"))
        fileMenu.Append(wx.ID_PRINT_SETUP, _("Page &Setup"))
        fileMenu.Append(wx.ID_PREVIEW, _("Print Pre&view"))

        editMenu = wx.Menu()
        editMenu.Append(wx.ID_UNDO, _("&Undo"))
        editMenu.Append(wx.ID_REDO, _("&Redo"))

        helpMenu = wx.Menu()
        helpMenu.Append(wx.ID_ABOUT, _("&About"))

        menuBar = wx.MenuBar()

        menuBar.Append(fileMenu, _("&File"))
        menuBar.Append(editMenu, _("&Edit"))
        menuBar.Append(helpMenu, _("&Help"))

        subframe.SetMenuBar(menuBar)
        subframe.Centre(wx.BOTH)
        return subframe


class MyFrame(wx.lib.docview.DocParentFrame):


    def __init__(self, manager, frame, id, title, pos = wx.DefaultPosition, size = wx.DefaultSize, style = wx.DEFAULT_FRAME_STYLE, name = "frame"):
        wx.lib.docview.DocParentFrame.__init__(self, manager, frame, id, title, pos, size, style, name)
        self._frame = frame
        wx.EVT_MENU(self, wx.ID_ABOUT, self.OnAbout)


    def OnAbout(self, event):
        wx.MessageBox(wx.GetApp().GetAppName(), _("About DocView"))


    def GetMainFrame(self):
        return self._frame


class MyMDIFrame(wx.lib.docview.DocMDIParentFrame):

    def __init__(self, manager, frame, id, title, pos = wx.DefaultPosition, size = wx.DefaultSize, style = wx.DEFAULT_FRAME_STYLE, name = "frame"):
        wx.lib.docview.DocMDIParentFrame.__init__(self, manager, frame, id, title, pos, size, style, name)
        self._frame = frame
        wx.EVT_MENU(self, wx.ID_ABOUT, self.OnAbout)


    def OnAbout(self, event):
        wx.MessageBox(wx.GetApp().GetAppName(), _("About DocView"))


    def GetMainFrame(self):
        return self._frame


app = MyApp()
app.MainLoop()


