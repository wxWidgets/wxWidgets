
from wxPython.wx import *
from wxPython.stc import *

#----------------------------------------------------------------------

demoText = """\
## This version of the editor has been set up to edit Python source
## code.  Here is a copy of wxPython/demo/Main.py to play with.


"""

#----------------------------------------------------------------------


if wxPlatform == '__WXMSW__':
    faces = { 'times': 'Times New Roman',
              'mono' : 'Courier New',
              'helv' : 'Arial',
              'other': 'Comic Sans MS',
              'size' : 8,
              'size2': 6,
             }
else:
    faces = { 'times': 'Times',
              'mono' : 'Courier',
              'helv' : 'Helvetica',
              'other': 'new century schoolbook',
              'size' : 11,
              'size2': 9,
             }


#----------------------------------------------------------------------

class PythonSTC(wxStyledTextCtrl):
    def __init__(self, parent, ID):
        wxStyledTextCtrl.__init__(self, parent, ID)

        self.SetLexer(wxSTC_LEX_PYTHON)
        self.SetKeywords(0,
                       "and assert break class continue def del elif else except "
                       "exec finally for from global if import in is lambda None "
                       "not or pass print raise return try while")
        self.SetViewWhitespace(false)
        self.SetProperty("fold", "1")
        ##self.SetProperty("tab.timmy.whinge.level", "4")

        # Setup a margin to hold fold markers
        #self.SetFoldFlags(16)  ###  WHAT IS THIS VALUE?  WHAT ARE THE OTHER FLAGS?  DOES IT MATTER?
        self.SetMarginType(2, wxSTC_MARGIN_SYMBOL)
        self.SetMarginMask(2, wxSTC_MASK_FOLDERS)
        self.SetMarginSensitive(2, true)
        self.SetMarginWidth(2, 15)
        self.MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_ARROW, "navy", "navy")
        self.MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_ARROWDOWN, "navy", "navy")


        EVT_STC_UPDATEUI(self,    ID, self.OnUpdateUI)
        EVT_STC_MARGINCLICK(self, ID, self.OnMarginClick)


        # Make some styles,  The lexer defines what each style is used for, we
        # just have to define what each style looks like.  This set is adapted from
        # Scintilla sample property files.

        self.StyleClearAll()

        # Global default styles for all languages
        # Default
        self.StyleSetSpec(32, "face:%(helv)s,size:%(size)d" % faces)
        # Line number
        self.StyleSetSpec(33, "back:#C0C0C0,face:%(helv)s,size:%(size2)d" % faces)
        # Brace highlight
        self.StyleSetSpec(34, "fore:#0000FF,bold")
        # Brace incomplete highlight
        self.StyleSetSpec(35, "fore:#FF0000,bold")
        # Control characters
        self.StyleSetSpec(36, "face:%(other)s" % faces)

        # Python styles
        # White space
        self.StyleSetSpec(0, "fore:#808080")
        # Comment
        self.StyleSetSpec(1, "fore:#007F00,face:%(other)s" % faces)
        # Number
        self.StyleSetSpec(2, "fore:#007F7F")
        # String
        self.StyleSetSpec(3, "fore:#7F007F,italics,face:%(times)s" % faces)
        # Single quoted string
        self.StyleSetSpec(4, "fore:#7F007F,italics,face:%(times)s" % faces)
        # Keyword
        self.StyleSetSpec(5, "fore:#00007F,bold")
        # Triple quotes
        self.StyleSetSpec(6, "fore:#7F0000")
        # Triple double quotes
        self.StyleSetSpec(7, "fore:#7F0000")
        # Class name definition
        self.StyleSetSpec(8, "fore:#0000FF,bold")
        # Function or method name definition
        self.StyleSetSpec(9, "fore:#007F7F,bold")
        # Operators
        self.StyleSetSpec(10, "bold")
        # Identifiers
        #self.StyleSetSpec(11, "bold")#,fore:#FF00FF")
        # Comment-blocks
        self.StyleSetSpec(12, "fore:#7F7F7F")
        # End of line where string is not closed
        self.StyleSetSpec(13, "fore:#000000,face:%(mono)s,back:#E0C0E0,eolfilled" % faces)
        # Matched Operators
        self.StyleSetSpec(34, "fore:#FFFFFF,back:#0000FF,bold")
        self.StyleSetSpec(35, "fore:#000000,back:#FF0000,bold")



    def OnUpdateUI(self, evt):
        # check for matching braces
        braceAtCaret = -1
	braceOpposite = -1
        charBefore = None
        caretPos = self.GetCurrentPos()
        if caretPos > 0:
            charBefore = self.GetCharAt(caretPos - 1)
            styleBefore = self.GetStyleAt(caretPos - 1)

        # check before
        if charBefore and charBefore in "[]{}()" and ord(styleBefore) == 10:
            braceAtCaret = caretPos - 1

        # check after
        if braceAtCaret < 0:
            charAfter = self.GetCharAt(caretPos)
            styleAfter = self.GetStyleAt(caretPos)
            if charAfter and charAfter in "[]{}()" and ord(styleAfter) == 10:
                braceAtCaret = caretPos

        if braceAtCaret >= 0:
            braceOpposite = self.BraceMatch(braceAtCaret)

        if braceAtCaret != -1  and braceOpposite == -1:
            self.BraceBadlight(braceAtCaret)
        else:
            self.BraceHighlight(braceAtCaret, braceOpposite)


    def OnMarginClick(self, evt):
        # fold and unfold as needed
        if evt.GetMargin() == 2:
            if evt.GetShift() and evt.GetControl():
                self.FoldAll()
            else:
                lineClicked = self.GetLineFromPos(evt.GetPosition())
                if self.GetFoldLevel(lineClicked) & wxSTC_FOLDLEVELHEADERFLAG:
                    if evt.GetShift():
                        self.SetFoldExpanded(lineClicked, true)
                        self.Expand(lineClicked, true, true, 1)
                    elif evt.GetControl():
                        if self.GetFoldExpanded(lineClicked):
                            self.SetFoldExpanded(lineClicked, false)
                            self.Expand(lineClicked, false, true, 0)
                        else:
                            self.SetFoldExpanded(lineClicked, true)
                            self.Expand(lineClicked, true, true, 100)
                    else:
                        self.ToggleFold(lineClicked)


    def FoldAll(self):
        lineCount = self.GetLineCount()
        expanding = true

        # find out if we are folding or unfolding
        for lineNum in range(lineCount):
            if self.GetFoldLevel(lineNum) & wxSTC_FOLDLEVELHEADERFLAG:
                expanding = not self.GetFoldExpanded(lineNum)
                break;

        lineNum = 0
        while lineNum < lineCount:
            level = self.GetFoldLevel(lineNum)
            if level & wxSTC_FOLDLEVELHEADERFLAG and \
               (level & wxSTC_FOLDLEVELNUMBERMASK) == wxSTC_FOLDLEVELBASE:

                if expanding:
                    self.SetFoldExpanded(lineNum, true)
                    print lineNum,
                    lineNum = self.Expand(lineNum, true)
                    print lineNum,
                    lineNum = lineNum - 1
                    print lineNum
                else:
                    lastChild = self.GetLastChild(lineNum, -1)
                    self.SetFoldExpanded(lineNum, false)
                    if lastChild > lineNum:
                        self.HideLines(lineNum+1, lastChild)

            lineNum = lineNum + 1



    def Expand(self, line, doExpand, force=false, visLevels=0, level=-1):
        lastChild = self.GetLastChild(line, level)
	line = line + 1
        while line <= lastChild:
            if force:
                if visLevels > 0:
                    self.ShowLines(line, line)
                else:
                    self.HideLines(line, line)
            else:
                if doExpand:
                    self.ShowLines(line, line)

            if level == -1:
                level = self.GetFoldLevel(line)

            if level & wxSTC_FOLDLEVELHEADERFLAG:
                if force:
                    if visLevels > 1:
                        self.SetFoldExpanded(line, true)
                    else:
                        self.SetFoldExpanded(line, false)
                    line = self.Expand(line, doExpand, force, visLevels-1)

                else:
                    if doExpand and self.GetFoldExpanded(line):
                        line = self.Expand(line, true, force, visLevels-1)
                    else:
                        line = self.Expand(line, false, force, visLevels-1)
            else:
                line = line + 1;

        return line


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    ed = PythonSTC(nb, -1)

    ed.SetText(demoText + open('Main.py').read())
    ed.EmptyUndoBuffer()


    # line numbers in the margin
    ed.SetMarginType(1, wxSTC_MARGIN_NUMBER)
    ed.SetMarginWidth(1, 25)

    return ed



#----------------------------------------------------------------------


overview = """\
<html><body>
Once again, no docs yet.  <b>Sorry.</b>  But <a href="data/stc.h">this</a>
and <a href="http://www.scintilla.org/ScintillaDoc.html">this</a> should
be helpful.
</body><html>
"""


if __name__ == '__main__':
    import sys
    app = wxPySimpleApp()
    frame = wxFrame(None, -1, "Tester...", size=(640, 480))
    win = runTest(frame, frame, sys.stdout)
    frame.Show(true)
    app.MainLoop()






#----------------------------------------------------------------------
#----------------------------------------------------------------------

