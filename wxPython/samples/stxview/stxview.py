#!/usr/bin/env python
#----------------------------------------------------------------------

import sys, os
import StructuredText
from wxPython.wx import *


USE_WXHTML = 1


if not USE_WXHTML:
    try: # try to load the IE ActiveX control
        from wxPython.lib.activexwrapper import MakeActiveXClass
        import win32com.client.gencache
        browserModule = win32com.client.gencache.EnsureModule(
            "{EAB22AC0-30C1-11CF-A7EB-0000C05BAE0B}", 0, 1, 1)
    except:
        USE_WXHTML = 1

if not USE_WXHTML:
    BrowserClass = MakeActiveXClass(browserModule.WebBrowser)

    class MyHtmlWindow(BrowserClass):
        def SetPage(self, html):
            import tempfile
            filename = tempfile.mktemp('.html')
            f = open(filename, 'w')
            f.write(html)
            f.close()
            self.Navigate(os.path.abspath(filename))
            self.filename = filename

        def OnDocumentComplete(self, pDisp=None, URL=None):
            os.unlink(self.filename)

else:
    from wxPython.html import *
    MyHtmlWindow = wxHtmlWindow



class StxFrame(wxFrame):
    title = "StxViewer"
    def __init__(self, stxFile):
        wxFrame.__init__(self, None, -1, self.title, size=(650, 700),
                         style=wxDEFAULT_FRAME_STYLE|wxNO_FULL_REPAINT_ON_RESIZE)

        ##self.CreateStatusBar()

        menu = wxMenu()
        menu.Append(10, "&Open\tCtrl-O", "Open a Structured Text file")
        EVT_MENU(self, 10, self.OnOpen)
        menu.Append(20, "&Close", "Close the current file")
        EVT_MENU(self, 20, self.OnClose)
        menu.Append(30, "&Save\tCtrl-S", "Save the current file")
        EVT_MENU(self, 30, self.OnSave)
        menu.Append(40, "Save &as", "Save the current file to a new name")
        EVT_MENU(self, 40, self.OnSaveAs)
        menu.Append(45, "Save as &html", "Save the current file as HTML")
        EVT_MENU(self, 45, self.OnSaveAsHTML)
        menu.AppendSeparator()
        menu.Append(50, "&Refresh\tCtrl-R", "Reload the file from disk")
        EVT_MENU(self, 50, self.OnRefresh)
        menu.AppendSeparator()
        menu.Append(60, "E&xit\tCtrl-X", "Close the application")
        EVT_MENU(self, 60, self.OnExit)


        menuBar = wxMenuBar()
        menuBar.Append(menu, "&File")
        self.SetMenuBar(menuBar)


        nb = wxNotebook(self, -1)
        EVT_NOTEBOOK_PAGE_CHANGED(self, -1, self.OnPageChanged)

        self.htmlWin = MyHtmlWindow(nb, -1)
        nb.AddPage(self.htmlWin, "View")

        self.editWin = wxTextCtrl(nb, -1, "", style=wxTE_MULTILINE)
        self.editWin.SetFont(wxFont(10, wxTELETYPE, wxNORMAL, wxNORMAL))
        nb.AddPage(self.editWin, "Edit")

        self.viewHtml = wxTextCtrl(nb, -1, "", style=wxTE_MULTILINE|wxTE_READONLY)
        self.viewHtml.SetFont(wxFont(10, wxTELETYPE, wxNORMAL, wxNORMAL))
        nb.AddPage(self.viewHtml, "HTML")

        self.LoadStxFile(stxFile)


    def LoadStxFile(self, stxFile):
        self.file = stxFile
        if stxFile is not None:
            text = open(stxFile).read()
            self.SetTitle(self.title + ': ' + stxFile)
        else:
            text = ""
            self.SetTitle(self.title)
        self.LoadStxText(text)


    def LoadStxText(self, text):
        # Old ST
        html = str(StructuredText.html_with_references(text))

        # NG Version
        #st = StructuredText.Basic(text)
        #doc = StructuredText.Document(st)
        #html = StructuredText.HTML(doc)

        self.htmlWin.SetPage(html)
        self.editWin.SetValue(text)
        self.viewHtml.SetValue(html)
        self.html = html


    def OnPageChanged(self, evt):
        if evt.GetOldSelection() == 1:  # if it was on the edit page
            text = self.editWin.GetValue()
            self.LoadStxText(text)


    def OnOpen(self, evt):
        dlg = wxFileDialog(self, defaultDir=os.getcwd(),
                           wildcard = "STX files (*.stx)|*.stx|"
                                      "Text files (*.txt)|*.txt|"
                                      "All files (*.*)|*.*",
                           style=wxOPEN)
        if dlg.ShowModal() == wxID_OK:
            self.LoadStxFile(dlg.GetPath())
        dlg.Destroy()



    def OnClose(self, evt):
        self.LoadStxFile(None)


    def OnSave(self, evt):
        if not self.file:
            self.OnSaveAs(evt)
        else:
            text = self.editWin.GetValue()
            open(self.file, 'w').write(text)
            self.LoadStxFile(self.file)


    def OnSaveAs(self, evt):
        dlg = wxFileDialog(self, "Save as...", defaultDir=os.getcwd(),
                           wildcard = "STX files (*.stx)|*.stx|"
                                      "Text files (*.txt)|*.txt|"
                                      "All files (*.*)|*.*",
                           style=wxSAVE)
        if dlg.ShowModal() == wxID_OK:
            file = dlg.GetPath()
            text = self.editWin.GetValue()
            open(file, 'w').write(text)
            self.LoadStxFile(file)
        dlg.Destroy()


    def OnSaveAsHTML(self, evt):
        dlg = wxFileDialog(self, "Save as...", defaultDir=os.getcwd(),
                           wildcard = "HTML files (*.html)|*.html|"
                                      "All files (*.*)|*.*",
                           style=wxSAVE)
        if dlg.ShowModal() == wxID_OK:
            file = dlg.GetPath()
            text = self.editWin.GetValue()
            self.LoadStxText(text)
            open(file, 'w').write(self.html)
        dlg.Destroy()



    def OnRefresh(self, evt):
        self.LoadStxFile(self.file)


    def OnExit(self, evt):
        self.Close(true)





app = wxPySimpleApp()
wxInitAllImageHandlers()

if len(sys.argv) > 1:
    filename = sys.argv[1]
else:
    filename = None

frame = StxFrame(filename)
frame.Show(true)
app.MainLoop()



