
import pprint, string, os
from wxPython.wx import *
from mimetypes_wdr import *
from Main import opj

#----------------------------------------------------------------------------

# WDR: classes

class MimeTypesTestPanel(wxPanel):
    def __init__(self, parent, id,
        pos = wxPyDefaultPosition, size = wxPyDefaultSize,
        style = wxTAB_TRAVERSAL ):
        wxPanel.__init__(self, parent, id, pos, size, style)

        MakeMimeTypesTestPanel( self, True )

        # WDR: handler declarations for MimeTypesTestPanel
        EVT_LISTBOX(self, ID_LISTBOX, self.OnListbox)
        EVT_BUTTON(self, ID_LOOKUP_BTN, self.OnLookup)

        self.GetInputText().SetValue("wav")
        self.OnLookup()

        mimetypes = wxTheMimeTypesManager.EnumAllFileTypes()
        for mt in mimetypes:
            self.GetListbox().Append(mt)



    # WDR: handler implementations for MimeTypesTestPanel

    def OnListbox(self, event):
        mimetype = event.GetString()
        self.GetInputText().SetValue(mimetype)
        self.GetMimeBtn().SetValue(True)
        self.GetExtensionBtn().SetValue(False)
        self.OnLookup()


    def OnLookup(self, event=None):
        txt = self.GetInputText().GetValue()
        if self.GetMimeBtn().GetValue():
            fileType = wxTheMimeTypesManager.GetFileTypeFromMimeType(txt)
            msg = "Mime type"
        else:
            fileType = wxTheMimeTypesManager.GetFileTypeFromExtension(txt)
            msg = "File extension"
        if fileType is None:
            wxMessageBox(msg + " not found.", "Oops!")
        else:
            self.Update(fileType)



    def Update(self, ft):
        #icon = ft.GetIcon()
        info = ft.GetIconInfo()
        if info is None:
            bmp = MyBitmapsFunc(0)
            ##print bmp.Ok(), bmp.GetWidth(), bmp.GetHeight()
            self.GetIconBmp().SetBitmap(bmp)
            self.GetIconFileTxt().SetValue("")
            self.GetIconIndexTxt().SetValue("")
        else:
            icon, file, idx = info
            #bmp = wxBitmapFromIcon(icon)
            #self.GetIconBmp().SetBitmap(bmp)
            self.GetIconBmp().SetIcon(icon)
            self.GetIconFileTxt().SetValue(file)
            self.GetIconIndexTxt().SetValue(str(idx))

        self.GetMimeTypeTxt().SetValue(str(ft.GetMimeType()))
        self.GetMimeTypesTxt().SetValue(str(ft.GetMimeTypes()))
        self.GetExtensionsTxt().SetValue(str(ft.GetExtensions()))
        self.GetDescriptionTxt().SetValue(str(ft.GetDescription()))

        extList = ft.GetExtensions()
        if extList:
            ext = extList[0]
            if ext[0] == ".": ext = ext[1:]
        else:
            ext = ""
        filename = "SPAM" + "." + ext
        mime = ft.GetMimeType() or ""
        cmd = ft.GetOpenCommand(filename, mime)
        self.GetOpenCmdTxt().SetValue(str(cmd))

        cmd = ft.GetPrintCommand(filename, mime)
        self.GetPrintCmdTxt().SetValue(str(cmd))

        all = ft.GetAllCommands(filename, mime)
        if all is None:
            self.GetAllCmdsTxt().SetValue("")
        else:
            verbs, commands = all
            text = pprint.pformat(map(None, verbs, commands))
            self.GetAllCmdsTxt().SetValue(text)


    # WDR: methods for MimeTypesTestPanel

    def GetListbox(self):
        return wxPyTypeCast( self.FindWindowById(ID_LISTBOX), "wxListBox" )

    def GetIconIndexTxt(self):
        return self.FindWindowById(ID_ICON_INDEX_TXT)

    def GetIconFileTxt(self):
        return self.FindWindowById(ID_ICON_FILE_TXT)

    def GetMimeBtn(self):
        return self.FindWindowById(ID_MIME_BTN)

    def GetExtensionBtn(self):
        return self.FindWindowById(ID_EXTENSION_Btn)

    def GetAllCmdsTxt(self):
        return self.FindWindowById(ID_ALL_CMDS_TXT)

    def GetPrintCmdTxt(self):
        return self.FindWindowById(ID_PRINT_CMD_TXT)

    def GetOpenCmdTxt(self):
        return self.FindWindowById(ID_OPEN_CMD_TXT)

    def GetDescriptionTxt(self):
        return self.FindWindowById(ID_DESCRIPTION_TXT)

    def GetExtensionsTxt(self):
        return self.FindWindowById(ID_EXTENSIONS_TXT)

    def GetMimeTypesTxt(self):
        return self.FindWindowById(ID_MIME_TYPES_TXT)

    def GetMimeTypeTxt(self):
        return self.FindWindowById(ID_MIME_TYPE_TXT)

    def GetIconBmp(self):
        return self.FindWindowById(ID_ICON_BMP)

    def GetInputText(self):
        return self.FindWindowById(ID_INPUT_TEXT)





#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = MimeTypesTestPanel(nb, -1)
    return win


#----------------------------------------------------------------------



overview = """\
"""




import mimetypes_wdr
import images

def MyBitmapsFunc( index ):
    return images.getNoIconBitmap()

mimetypes_wdr.MyBitmapsFunc = MyBitmapsFunc






if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])])
