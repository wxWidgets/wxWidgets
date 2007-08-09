"""EditWindow class."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

import wx
from wx import stc

import keyword
import os
import sys
import time

import dispatcher
from version import VERSION


if 'wxMSW' in wx.PlatformInfo:
    FACES = { 'times'     : 'Times New Roman',
              'mono'      : 'Courier New',
              'helv'      : 'Arial',
              'lucida'    : 'Lucida Console',
              'other'     : 'Comic Sans MS',
              'size'      : 10,
              'lnsize'    : 8,
              'backcol'   : '#FFFFFF',
              'calltipbg' : '#FFFFB8',
              'calltipfg' : '#404040',
            }

elif 'wxGTK' in wx.PlatformInfo and 'gtk2' in wx.PlatformInfo:
    FACES = { 'times'     : 'Serif',
              'mono'      : 'Monospace',
              'helv'      : 'Sans',
              'other'     : 'new century schoolbook',
              'size'      : 10,
              'lnsize'    : 9,
              'backcol'   : '#FFFFFF',
              'calltipbg' : '#FFFFB8',
              'calltipfg' : '#404040',
            }

elif 'wxMac' in wx.PlatformInfo:
    FACES = { 'times'     : 'Lucida Grande',
              'mono'      : 'Courier New',
              'helv'      : 'Geneva',
              'other'     : 'new century schoolbook',
              'size'      : 13,
              'lnsize'    : 10,
              'backcol'   : '#FFFFFF',
              'calltipbg' : '#FFFFB8',
              'calltipfg' : '#404040',
            }

else: # GTK1, etc.
    FACES = { 'times'     : 'Times',
              'mono'      : 'Courier',
              'helv'      : 'Helvetica',
              'other'     : 'new century schoolbook',
              'size'      : 12,
              'lnsize'    : 10,
              'backcol'   : '#FFFFFF',
              'calltipbg' : '#FFFFB8',
              'calltipfg' : '#404040',
            }


class EditWindow(stc.StyledTextCtrl):
    """EditWindow based on StyledTextCtrl."""

    revision = __revision__

    def __init__(self, parent, id=-1, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.CLIP_CHILDREN | wx.SUNKEN_BORDER):
        """Create EditWindow instance."""
        stc.StyledTextCtrl.__init__(self, parent, id, pos, size, style)
        self.__config()
        stc.EVT_STC_UPDATEUI(self, id, self.OnUpdateUI)
        dispatcher.connect(receiver=self._fontsizer, signal='FontIncrease')
        dispatcher.connect(receiver=self._fontsizer, signal='FontDecrease')
        dispatcher.connect(receiver=self._fontsizer, signal='FontDefault')

    def _fontsizer(self, signal):
        """Receiver for Font* signals."""
        size = self.GetZoom()
        if signal == 'FontIncrease':
            size += 1
        elif signal == 'FontDecrease':
            size -= 1
        elif signal == 'FontDefault':
            size = 0
        self.SetZoom(size)


    def __config(self):
        self.setDisplayLineNumbers(False)
        
        self.SetLexer(stc.STC_LEX_PYTHON)
        self.SetKeyWords(0, ' '.join(keyword.kwlist))

        self.setStyles(FACES)
        self.SetViewWhiteSpace(False)
        self.SetTabWidth(4)
        self.SetUseTabs(False)
        # Do we want to automatically pop up command completion options?
        self.autoComplete = True
        self.autoCompleteIncludeMagic = True
        self.autoCompleteIncludeSingle = True
        self.autoCompleteIncludeDouble = True
        self.autoCompleteCaseInsensitive = True
        self.AutoCompSetIgnoreCase(self.autoCompleteCaseInsensitive)
        self.autoCompleteAutoHide = False
        self.AutoCompSetAutoHide(self.autoCompleteAutoHide)
        self.AutoCompStops(' .,;:([)]}\'"\\<>%^&+-=*/|`')
        # Do we want to automatically pop up command argument help?
        self.autoCallTip = True
        self.callTipInsert = True
        self.CallTipSetBackground(FACES['calltipbg'])
        self.CallTipSetForeground(FACES['calltipfg'])
        self.SetWrapMode(False)
        try:
            self.SetEndAtLastLine(False)
        except AttributeError:
            pass

    def setDisplayLineNumbers(self, state):
        self.lineNumbers = state
        if state:
            self.SetMarginType(1, stc.STC_MARGIN_NUMBER)
            self.SetMarginWidth(1, 40)
        else:
            # Leave a small margin so the feature hidden lines marker can be seen
            self.SetMarginType(1, 0)
            self.SetMarginWidth(1, 10)
        
    def setStyles(self, faces):
        """Configure font size, typeface and color for lexer."""

        # Default style
        self.StyleSetSpec(stc.STC_STYLE_DEFAULT,
                          "face:%(mono)s,size:%(size)d,back:%(backcol)s" % \
                          faces)

        self.StyleClearAll()
        self.SetSelForeground(True, wx.SystemSettings.GetColour(wx.SYS_COLOUR_HIGHLIGHTTEXT))
        self.SetSelBackground(True, wx.SystemSettings.GetColour(wx.SYS_COLOUR_HIGHLIGHT))
        
        # Built in styles
        self.StyleSetSpec(stc.STC_STYLE_LINENUMBER,
                          "back:#C0C0C0,face:%(mono)s,size:%(lnsize)d" % FACES)
        self.StyleSetSpec(stc.STC_STYLE_CONTROLCHAR,
                          "face:%(mono)s" % faces)
        self.StyleSetSpec(stc.STC_STYLE_BRACELIGHT,
                          "fore:#0000FF,back:#FFFF88")
        self.StyleSetSpec(stc.STC_STYLE_BRACEBAD,
                          "fore:#FF0000,back:#FFFF88")

        # Python styles
        self.StyleSetSpec(stc.STC_P_DEFAULT,
                          "face:%(mono)s" % faces)
        self.StyleSetSpec(stc.STC_P_COMMENTLINE,
                          "fore:#007F00,face:%(mono)s" % faces)
        self.StyleSetSpec(stc.STC_P_NUMBER,
                          "")
        self.StyleSetSpec(stc.STC_P_STRING,
                          "fore:#7F007F,face:%(mono)s" % faces)
        self.StyleSetSpec(stc.STC_P_CHARACTER,
                          "fore:#7F007F,face:%(mono)s" % faces)
        self.StyleSetSpec(stc.STC_P_WORD,
                          "fore:#00007F,bold")
        self.StyleSetSpec(stc.STC_P_TRIPLE,
                          "fore:#7F0000")
        self.StyleSetSpec(stc.STC_P_TRIPLEDOUBLE,
                          "fore:#000033,back:#FFFFE8")
        self.StyleSetSpec(stc.STC_P_CLASSNAME,
                          "fore:#0000FF,bold")
        self.StyleSetSpec(stc.STC_P_DEFNAME,
                          "fore:#007F7F,bold")
        self.StyleSetSpec(stc.STC_P_OPERATOR,
                          "")
        self.StyleSetSpec(stc.STC_P_IDENTIFIER,
                          "")
        self.StyleSetSpec(stc.STC_P_COMMENTBLOCK,
                          "fore:#7F7F7F")
        self.StyleSetSpec(stc.STC_P_STRINGEOL,
                          "fore:#000000,face:%(mono)s,back:#E0C0E0,eolfilled" % faces)

    def OnUpdateUI(self, event):
        """Check for matching braces."""
        # If the auto-complete window is up let it do its thing.
        if self.AutoCompActive() or self.CallTipActive():
            return
        braceAtCaret = -1
        braceOpposite = -1
        charBefore = None
        caretPos = self.GetCurrentPos()
        if caretPos > 0:
            charBefore = self.GetCharAt(caretPos - 1)
            styleBefore = self.GetStyleAt(caretPos - 1)

        # Check before.
        if charBefore and chr(charBefore) in '[]{}()' \
        and styleBefore == stc.STC_P_OPERATOR:
            braceAtCaret = caretPos - 1

        # Check after.
        if braceAtCaret < 0:
            charAfter = self.GetCharAt(caretPos)
            styleAfter = self.GetStyleAt(caretPos)
            if charAfter and chr(charAfter) in '[]{}()' \
            and styleAfter == stc.STC_P_OPERATOR:
                braceAtCaret = caretPos

        if braceAtCaret >= 0:
            braceOpposite = self.BraceMatch(braceAtCaret)

        if braceAtCaret != -1  and braceOpposite == -1:
            self.BraceBadLight(braceAtCaret)
        else:
            self.BraceHighlight(braceAtCaret, braceOpposite)

    def CanCopy(self):
        """Return True if text is selected and can be copied."""
        return self.GetSelectionStart() != self.GetSelectionEnd()

    def CanCut(self):
        """Return True if text is selected and can be cut."""
        return self.CanCopy() and self.CanEdit()

    def CanEdit(self):
        """Return True if editing should succeed."""
        return not self.GetReadOnly()

    def CanPaste(self):
        """Return True if pasting should succeed."""
        return stc.StyledTextCtrl.CanPaste(self) and self.CanEdit()


    def GetLastPosition(self):
        return self.GetLength()

    def GetRange(self, start, end):
        return self.GetTextRange(start, end)

    def GetSelection(self):
        return self.GetAnchor(), self.GetCurrentPos()

    def ShowPosition(self, pos):
        line = self.LineFromPosition(pos)
        #self.EnsureVisible(line)
        self.GotoLine(line)

    def DoFindNext(self, findData, findDlg=None):
        backward = not (findData.GetFlags() & wx.FR_DOWN)
        matchcase = (findData.GetFlags() & wx.FR_MATCHCASE) != 0
        end = self.GetLastPosition()
        textstring = self.GetRange(0, end)
        findstring = findData.GetFindString()
        if not matchcase:
            textstring = textstring.lower()
            findstring = findstring.lower()
        if backward:
            start = self.GetSelection()[0]
            loc = textstring.rfind(findstring, 0, start)
        else:
            start = self.GetSelection()[1]
            loc = textstring.find(findstring, start)

        # if it wasn't found then restart at begining
        if loc == -1 and start != 0:
            if backward:
                start = end
                loc = textstring.rfind(findstring, 0, start)
            else:
                start = 0
                loc = textstring.find(findstring, start)

        # was it still not found?
        if loc == -1:
            dlg = wx.MessageDialog(self, 'Unable to find the search text.',
                          'Not found!',
                          wx.OK | wx.ICON_INFORMATION)
            dlg.ShowModal()
            dlg.Destroy()
        if findDlg:
            if loc == -1:
                wx.CallAfter(findDlg.SetFocus)
                return
            else:
                findDlg.Close()

        # show and select the found text
        self.ShowPosition(loc)
        self.SetSelection(loc, loc + len(findstring))
