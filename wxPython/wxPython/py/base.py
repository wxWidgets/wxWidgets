"""Base editor."""

__author__ = "Patrick K. O'Brien <pobrien@orbtech.com>"
__cvsid__ = "$Id$"
__revision__ = "$Revision$"[11:-2]

from wxPython import wx
from wxPython import stc

import keyword
import os
import sys
import time

import dispatcher
from version import VERSION

try:
    True
except NameError:
    True = 1==1
    False = 1==0

if wx.wxPlatform == '__WXMSW__':
    FACES = { 'times'  : 'Times New Roman',
              'mono'   : 'Courier New',
              'helv'   : 'Lucida Console',
              'lucida' : 'Lucida Console',
              'other'  : 'Comic Sans MS',
              'size'   : 10,
              'lnsize' : 9,
              'backcol': '#FFFFFF',
            }
else:  # GTK
    FACES = { 'times'  : 'Times',
              'mono'   : 'Courier',
              'helv'   : 'Helvetica',
              'other'  : 'new century schoolbook',
              'size'   : 12,
              'lnsize' : 10,
              'backcol': '#FFFFFF',
            }


class Editor(stc.wxStyledTextCtrl):
    """Editor based on StyledTextCtrl."""

    revision = __revision__

    def __init__(self, parent, id=-1, pos=wx.wxDefaultPosition,
                 size=wx.wxDefaultSize, style=wx.wxCLIP_CHILDREN | wx.wxSUNKEN_BORDER):
        """Create an Editor instance."""
        stc.wxStyledTextCtrl.__init__(self, parent, id, pos, size, style)
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
        """Configure shell based on user preferences."""
        self.SetMarginType(1, stc.wxSTC_MARGIN_NUMBER)
        self.SetMarginWidth(1, 40)

        self.SetLexer(stc.wxSTC_LEX_PYTHON)
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
        self.AutoCompSetAutoHide(False)
        self.AutoCompStops(' .,;:([)]}\'"\\<>%^&+-=*/|`')
        # Do we want to automatically pop up command argument help?
        self.autoCallTip = True
        self.CallTipSetBackground(wx.wxColour(255, 255, 232))
        self.SetWrapMode(False)
        try:
            self.SetEndAtLastLine(False)
        except AttributeError:
            pass

    def setStyles(self, faces):
        """Configure font size, typeface and color for lexer."""

        # Default style
        self.StyleSetSpec(stc.wxSTC_STYLE_DEFAULT,
                          "face:%(mono)s,size:%(size)d,back:%(backcol)s" % \
                          faces)

        self.StyleClearAll()

        # Built in styles
        self.StyleSetSpec(stc.wxSTC_STYLE_LINENUMBER,
                          "back:#C0C0C0,face:%(mono)s,size:%(lnsize)d" % faces)
        self.StyleSetSpec(stc.wxSTC_STYLE_CONTROLCHAR,
                          "face:%(mono)s" % faces)
        self.StyleSetSpec(stc.wxSTC_STYLE_BRACELIGHT,
                          "fore:#0000FF,back:#FFFF88")
        self.StyleSetSpec(stc.wxSTC_STYLE_BRACEBAD,
                          "fore:#FF0000,back:#FFFF88")

        # Python styles
        self.StyleSetSpec(stc.wxSTC_P_DEFAULT,
                          "face:%(mono)s" % faces)
        self.StyleSetSpec(stc.wxSTC_P_COMMENTLINE,
                          "fore:#007F00,face:%(mono)s" % faces)
        self.StyleSetSpec(stc.wxSTC_P_NUMBER,
                          "")
        self.StyleSetSpec(stc.wxSTC_P_STRING,
                          "fore:#7F007F,face:%(mono)s" % faces)
        self.StyleSetSpec(stc.wxSTC_P_CHARACTER,
                          "fore:#7F007F,face:%(mono)s" % faces)
        self.StyleSetSpec(stc.wxSTC_P_WORD,
                          "fore:#00007F,bold")
        self.StyleSetSpec(stc.wxSTC_P_TRIPLE,
                          "fore:#7F0000")
        self.StyleSetSpec(stc.wxSTC_P_TRIPLEDOUBLE,
                          "fore:#000033,back:#FFFFE8")
        self.StyleSetSpec(stc.wxSTC_P_CLASSNAME,
                          "fore:#0000FF,bold")
        self.StyleSetSpec(stc.wxSTC_P_DEFNAME,
                          "fore:#007F7F,bold")
        self.StyleSetSpec(stc.wxSTC_P_OPERATOR,
                          "")
        self.StyleSetSpec(stc.wxSTC_P_IDENTIFIER,
                          "")
        self.StyleSetSpec(stc.wxSTC_P_COMMENTBLOCK,
                          "fore:#7F7F7F")
        self.StyleSetSpec(stc.wxSTC_P_STRINGEOL,
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
        and styleBefore == stc.wxSTC_P_OPERATOR:
            braceAtCaret = caretPos - 1

        # Check after.
        if braceAtCaret < 0:
            charAfter = self.GetCharAt(caretPos)
            styleAfter = self.GetStyleAt(caretPos)
            if charAfter and chr(charAfter) in '[]{}()' \
            and styleAfter == stc.wxSTC_P_OPERATOR:
                braceAtCaret = caretPos

        if braceAtCaret >= 0:
            braceOpposite = self.BraceMatch(braceAtCaret)

        if braceAtCaret != -1  and braceOpposite == -1:
            self.BraceBadLight(braceAtCaret)
        else:
            self.BraceHighlight(braceAtCaret, braceOpposite)

    def CanCut(self):
        """Return true if text is selected and can be cut."""
        return self.CanCopy()

    def CanCopy(self):
        """Return true if text is selected and can be copied."""
        return self.GetSelectionStart() != self.GetSelectionEnd()

    def CanEdit(self):
        """Return true if editing should succeed."""
        return True
