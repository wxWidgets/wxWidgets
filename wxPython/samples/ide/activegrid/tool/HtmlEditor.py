#----------------------------------------------------------------------------
# Name:         HtmlEditor.py
# Purpose:      Abstract Code Editor for pydocview tbat uses the Styled Text Control
#
# Author:       Peter Yared
#
# Created:      8/15/04
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
# License:      wxWindows License
#----------------------------------------------------------------------------


import wx
import os.path
import string
import STCTextEditor
import CodeEditor
_ = wx.GetTranslation
        

class HtmlDocument(CodeEditor.CodeDocument):

    pass


class HtmlView(CodeEditor.CodeView):


    def GetCtrlClass(self):
        """ Used in split window to instantiate new instances """
        return HtmlCtrl


    def GetAutoCompleteHint(self):
        pos = self.GetCtrl().GetCurrentPos()
        if pos == 0:
            return None, None
            
        validLetters = string.letters + string.digits + '_!-'
        word = ''
        while (True):
            pos = pos - 1
            if pos < 0:
                break
            char = chr(self.GetCtrl().GetCharAt(pos))
            if char not in validLetters:
                break
            word = char + word
            
        return None, word
        

    def GetAutoCompleteDefaultKeywords(self):
        return HTMLKEYWORDS


##    def _CreateControl(self, parent, id):
##        import wx  # wxBug: When inlining the import of the appropriate html control below, have to specifically import wx for some reason
##        self._notebook = wx.Notebook(parent, wx.NewId(), style = wx.NB_BOTTOM)
##        self._textEditor = HtmlCtrl(self._notebook, id)
##        if wx.Platform =='__WXMSW__':
##            import wxPython.iewin
##            self._browserCtrl = wxPython.iewin.wxIEHtmlWin(self._notebook, -1, style = wx.NO_FULL_REPAINT_ON_RESIZE)
##        else:
##            import wx.html
##            self._browserCtrl = wx.html.HtmlWindow(self._notebook, -1, style = wx.NO_FULL_REPAINT_ON_RESIZE)
##        self._notebook.AddPage(self._textEditor, _("Edit"))
##        self._notebook.AddPage(self._browserCtrl, _("View"))
##        self._insertMode = True
##        wx.EVT_NOTEBOOK_PAGE_CHANGED(self._notebook, self._notebook.GetId(), self.OnNotebookChanging)
##        return self._textEditor
##
##
##    def _CreateSizer(self, frame):
##        sizer = wx.BoxSizer(wx.HORIZONTAL)
##        sizer.Add(self._notebook, 1, wx.EXPAND)
##        frame.SetSizer(sizer)
##        frame.SetAutoLayout(True)
##
##
##    def OnNotebookChanging(self, event):
##        if event.GetSelection() == 0:  # Going to the edit page
##            pass  # self._textEditor.Refresh()
##        elif event.GetSelection() == 1:  # Going to the browser page
##            text = self._textEditor.GetText()
##            if wx.Platform == '__WXMSW__':
##                path = os.path.join(tempfile.gettempdir(), "temp.html")
##                file = open(path, 'w')
##                file.write(text)
##                file.close()
##                self._browserCtrl.Navigate("file://" + path)
##            else:
##                self._browserCtrl.SetPage(text)
##        event.Skip()


class HtmlService(CodeEditor.CodeService):


    def __init__(self):
        CodeEditor.CodeService.__init__(self)


class HtmlCtrl(CodeEditor.CodeCtrl):


    def __init__(self, parent, ID = -1, style = wx.NO_FULL_REPAINT_ON_RESIZE):
        CodeEditor.CodeCtrl.__init__(self, parent, ID, style)
        self.SetLexer(wx.stc.STC_LEX_HTML)
        self.SetProperty("fold.html", "1")

    def GetMatchingBraces(self):
        return "<>[]{}()"

    def CanWordWrap(self):
        return True


    def SetViewDefaults(self):
        CodeEditor.CodeCtrl.SetViewDefaults(self, configPrefix = "Html", hasWordWrap = False, hasTabs = True)


    def GetFontAndColorFromConfig(self):
        return CodeEditor.CodeCtrl.GetFontAndColorFromConfig(self, configPrefix = "Html")


    def UpdateStyles(self):
        CodeEditor.CodeCtrl.UpdateStyles(self)

        if not self.GetFont():
            return

        faces = { 'font' : self.GetFont().GetFaceName(),
                  'size' : self.GetFont().GetPointSize(),
                  'size2': self.GetFont().GetPointSize() - 2,
                  'color' : "%02x%02x%02x" % (self.GetFontColor().Red(), self.GetFontColor().Green(), self.GetFontColor().Blue())
                  }

        # White space
        self.StyleSetSpec(wx.stc.STC_H_DEFAULT, "face:%(font)s,fore:#000000,face:%(font)s,size:%(size)d" % faces)
        # Comment
        self.StyleSetSpec(wx.stc.STC_H_COMMENT, "face:%(font)s,fore:#007F00,italic,face:%(font)s,size:%(size)d" % faces)
        # Number
        self.StyleSetSpec(wx.stc.STC_H_NUMBER, "face:%(font)s,fore:#007F7F,size:%(size)d" % faces)
        # String
        self.StyleSetSpec(wx.stc.STC_H_SINGLESTRING, "face:%(font)s,fore:#7F007F,face:%(font)s,size:%(size)d" % faces)
        self.StyleSetSpec(wx.stc.STC_H_DOUBLESTRING, "face:%(font)s,fore:#7F007F,face:%(font)s,size:%(size)d" % faces)
        # Tag
        self.StyleSetSpec(wx.stc.STC_H_TAG, "face:%(font)s,fore:#00007F,bold,size:%(size)d" % faces)
        # Attributes
        self.StyleSetSpec(wx.stc.STC_H_ATTRIBUTE, "face:%(font)s,fore:#00007F,bold,size:%(size)d" % faces)


class HtmlOptionsPanel(STCTextEditor.TextOptionsPanel):

    def __init__(self, parent, id):
        STCTextEditor.TextOptionsPanel.__init__(self, parent, id, configPrefix = "Html", label = "HTML", hasWordWrap = True, hasTabs = True)


HTMLKEYWORDS = [
        "A", "ABBR", "ACRONYM", "ADDRESS", "APPLET", "AREA", "B", "BASE", "BASEFONT", "BDO", "BIG", "BLOCKQUOTE",
        "BODY", "BR", "BUTTON", "CAPTION", "CENTER", "CITE", "CODE", "COL", "COLGROUP", "DD", "DEL", "DFN", "DIR",
        "DIV", "DL", "DT", "EM", "FIELDSET", "FONT", "FORM", "FRAME", "FRAMESET", "H1", "H2", "H3", "H4", "H5", "H6",
        "HEAD", "HR", "HTML", "I", "IFRAME", "IMG", "INPUT", "INS", "ISINDEX", "KBD", "LABEL", "LEGEND", "LI", "LINK",
        "MAP", "MENU", "META", "NOFRAMES", "NOSCRIPT", "OBJECT", "OL", "OPTGROUP", "OPTION", "P", "PARAM",
        "PRE", "Q", "S", "SAMP", "SCRIPT", "SELECT", "SMALL", "SPAN", "STRIKE", "STRONG", "STYLE", "SUB", "SUP",
        "TABLE", "TBODY", "TD", "TEXTAREA", "TFOOT", "TH", "THEAD", "TITLE", "TR", "TT", "U", "UL", "VAR", "XML",
        "XMLNS", "ACCEPT-CHARSET", "ACCEPT", "ACCESSKEY", "ACTION", "ALIGN", "ALINK", "ALT",
        "ARCHIVE", "AXIS", "BACKGROUND", "BGCOLOR", "BORDER", "CELLPADDING", "CELLSPACING", "CHAR",
        "CHAROFF", "CHARSET", "CHECKED", "CLASS", "CLASSID", "CLEAR", "CODEBASE", "CODETYPE",
        "COLOR", "COLS", "COLSPAN", "COMPACT", "CONTENT", "COORDS", "DATA", "DATAFLD", "DATAFORMATAS",
        "DATAPAGESIZE", "DATASRC", "DATETIME", "DECLARE", "DEFER", "DISABLED", "ENCTYPE",
        "EVENT", "FACE", "FOR", "FRAMEBORDER", "HEADERS", "HEIGHT", "HREF", "HREFLANG", "HSPACE",
        "HTTP-EQUIV", "ID", "ISMAP", "LANG", "LANGUAGE", "LEFTMARGIN", "LONGDESC",
        "MARGINWIDTH", "MARGINHEIGHT", "MAXLENGTH", "MEDIA", "METHOD", "MULTIPLE", "NAME", "NOHREF",
        "NORESIZE", "NOSHADE", "NOWRAP", "ONBLUR", "ONCHANGE", "ONCLICK", "ONDBLCLICK",
        "ONFOCUS", "ONKEYDOWN", "ONKEYPRESS", "ONKEYUP", "ONLOAD", "ONMOUSEDOWN", "ONMOUSEMOVE",
        "ONMOUSEOVER", "ONMOUSEOUT", "ONMOUSEUP", "ONRESET", "ONSELECT", "ONSUBMIT", "ONUNLOAD",
        "PROFILE", "PROMPT", "READONLY", "REL", "REV", "ROWS", "ROWSPAN", "RULES", "SCHEME", "SCOPE",
        "SELECTED", "SHAPE", "SIZE", "SRC", "STANDBY", "START", "SUMMARY", "TABINDEX",
        "TARGET", "TOPMARGIN", "TYPE", "USEMAP", "VALIGN", "VALUE", "VALUETYPE",
        "VERSION", "VLINK", "VSPACE", "WIDTH", "TEXT", "PASSWORD", "CHECKBOX", "RADIO", "SUBMIT", "RESET",
        "FILE", "HIDDEN", "IMAGE", "PUBLIC", "!DOCTYPE",
        "ADD_DATE", "LAST_MODIFIED", "LAST_VISIT"
    ]


#----------------------------------------------------------------------------
# Icon Bitmaps - generated by encode_bitmaps.py
#----------------------------------------------------------------------------
from wx import ImageFromStream, BitmapFromImage
import cStringIO


def getHTMLData():
    return \
'\x89PNG\r\n\x1a\n\x00\x00\x00\rIHDR\x00\x00\x00\x10\x00\x00\x00\x10\x08\x06\
\x00\x00\x00\x1f\xf3\xffa\x00\x00\x00\x04sBIT\x08\x08\x08\x08|\x08d\x88\x00\
\x00\x00\xd3IDAT8\x8dcddbf\xa0\x040\xfe\xbf\xd1\xf3\x9f\x12\x03X\xfe}\xbeI\
\x91\x0b\x98(\xd2=(\x0c`\x90W\xd2\xfc\x0f\x030\xb6\xbc\x92\xe6\x7f\x1d\x03\
\xf3\xffN\xae\xde\xff\xff\xff\xff\xff\xdf\xc0\xd8\xfa\xff\xb5;O\xfe_\xbb\xf3\
\xe4\xbf\x8e\x819\\\xcd\xd7_\xff\xff\xb3<|x\x9dAAY\x0b\xc5\xd0\x07w\xaf1\xd8\
\xdbZ0\xec\xdd\xb5\x85\x81\x81\x81\x81\xe1\xdd\xf3\x1b\x0c\xab\x97\xcef\xe0`\
ca\xf8\xfa\xf1\x19\\\x1d\x17+\x03\x03\x0b\x03\x03\x03Cqq>\xc3\xd3\x17o\x18V,\
]\n\x97de\xe7\x81\xb3\x199\xc4\x18\x0e\x1d:\xc2 "*\xce\xf0\x8f\x11!\x8e\xd3\
\x0b\xd8\xd8\xa7\x8e\xed\xf9\x7f\xf1\xcca\x14o\xca+i\xfeg\xfc{:\x95\xa2\x844\
\xf0\xd1H\xb1\x01\x8c\x94\xe6F\x8a]\x00\x00YXz\xf0\x97\x87\'\x1a\x00\x00\x00\
\x00IEND\xaeB`\x82' 


def getHTMLBitmap():
    return BitmapFromImage(getHTMLImage())

def getHTMLImage():
    stream = cStringIO.StringIO(getHTMLData())
    return ImageFromStream(stream)

def getHTMLIcon():
    return wx.IconFromBitmap(getHTMLBitmap())
