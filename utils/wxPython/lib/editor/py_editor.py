# (C)opyright by Dirk Holtwick, 1999
# ----------------------------------
# holtwick@spirito.de
# http://www.spirito.de/pyde

from editor import *
from string import *
from keyword import *
from tokenizer import *

"""
This module will be loaded by the main
window. It implements some methods that
are typical for Python sources.
"""

class wxPyEditor(wxEditor):

    # ------------------------------------------------------------------

    def __init__(self, parent, id=-1):
        wxEditor.__init__(self, parent, id)
        self.SetFontTab([
            wxNamedColour('black'),
            wxNamedColour('blue'),
            wxNamedColour('red'),
            wxNamedColour('darkgreen'),
            wxNamedColour('brown')
            ])

    # ------------------------------------------------------------------

    def OnUpdateHighlight(self, line = -1):
        if line>=0:
            t   = self.text[line].text
            syn = []

            toks = Tokenizer(t).tokens()
            for type, string, begin, end in toks:
                if type == "KEY":
                    syn.append((begin,  1))
                    syn.append((end,    0))
                elif type == "COMMENT":
                    syn.append((begin,  2))
                elif type == "STRING":
                    syn.append((begin,  3))
                    syn.append((end,    0))
                elif type == "NUMBER":
                    syn.append((begin,  4))
                    syn.append((end,    0))
                elif type == "NAME":
                    if string=="self":
                        syn.append((begin,  4))
                        syn.append((end,    0))
                else:
                    pass
            self.text[line].syntax = syn

    # ------------------------------------------------------------------

    def OnUpdateSyntax(self, line = -1):
        if line>=0:
            """
            tx, syn, m = self.text[line]
            pre = 0
            for i in range(0,len(tx)):
                if tx[i] != " ":
                    pre = i
                    break
            t = tx[pre:]

            t = Tokenizer(t).line()

            t = tx[:pre] + t
            self.text[line] = t, syn, m
            """
            self.OnUpdateHighlight(line)

    # ------------------------------------------------------------------

    def OnTabulator(self, event):
        add = +1
        if event.ShiftDown():
            add = -1
        t = self.GetTextLine(self.cy)
        if strip(t):
            indent = self.GetIndent(t)
#           print indent
            t = t[indent:]
            tabs = indent / self.tabsize
#            for i in range(0,tabs+add):
            t = (" " * 4 * (tabs+add)) + t
            self.SetTextLine(self.cy, t)
        elif add>0:
            self.InsertText("    ")

    # ------------------------------------------------------------------

    def FindQuote(self, lineno, quote_type='"""', direction=1):
        """find line containing the matching quote"""
        l =lineno +direction
        while (l < len(self.text)-1) and (l >= 0):
            if find(self.text[l].text, quote_type) >=0: return l
            l =l +direction
        return None

    def FindNextLine(self, lineno, direction=1):
        """get the next line of code (skipping comment lines and empty lines)"""
        l =lineno +direction
        while (l < len(self.text)-1) and (l >= 0):
            str =lstrip(self.text[l].text)
            if (len(str) >0) and (str[0] !="#"): return l
            l =l +direction
        return None

    def Fold(self):
        l       = self.GetLine(self.cy)
        line    = self.text[l]
        t       = line.text

        # fold ...
        if line.editable:

            # 3*quotes
            qpos =find(t, '"""')
            if qpos >=0: qtype ='"""'
            else:
                qpos =find(t, "'''")
                if qpos >=0: qtype ="'''"

            if (qpos >=0) and (find(t[qpos+3:], qtype) <0):
                closing_quote =self.FindQuote(l, qtype)
                if closing_quote !=None:
                    line.editable = not line.editable
                    l =l +1
                    while l <= closing_quote:
                       self.text[l].visible =self.text[l].visible +1
                       l =l +1

            else:  # try normal fold on leading whitespace
                lim = self.GetIndent(t)
                lnext =self.FindNextLine(l)
                if (lnext !=None) \
                and (self.GetIndent(self.text[lnext].text) >lim):
                    line.editable =FALSE
                    lstart =l +1
                    l =self.FindNextLine(l)
                    while (l !=None) \
                    and (self.GetIndent(self.text[l].text) >lim):
                        l =self.FindNextLine(l)
                    if l ==None:
                        # fold till the end
                        l =len(self.text)
                    for line in self.text[lstart:l]:
                        line.visible =line.visible +1

        # ... or unfold
        else:
            lim = line.visible + 1
            line.editable = not line.editable

            l = l + 1
            line = self.text[l]
            while (l < (len(self.text) -1)) and (line.visible>=lim):
                line.visible = line.visible - 1
                l = l + 1
                line = self.text[l]

    def FoldAll(self):
        self.CalcLines()
        self.cx = 0
        self.cy = len(self.lines) - 1
        prev_indent =0
        # following loop is exited in two cases:
        # when self.cy becomes 0 (topmost level is not folded by FoldAll)
        # or when FindNextLine() returns None (all remaining lines till
        #               the beginning of the text are empty or comments)
        while self.cy:
            t = self.GetTextLine(self.cy)
            # indent-based folding
            indent =self.GetIndent(t)
            if indent <prev_indent:
                self.Fold()
            prev_indent =indent
            # triple-quote folding
            qpos =find(t, '"""')
            if qpos >=0: qtype ='"""'
            else:
                qpos =find(t, "'''")
                if qpos >=0: qtype ="'''"
            if (qpos >=0) and (find(t[qpos+3:], qtype) <0):
                closing_quote =self.FindQuote(self.cy, qtype, -1)
                if closing_quote !=None:
                    # XXX potential bug: unmatched triple quotes
                    self.cy =closing_quote
                    self.Fold()
            self.cy =self.FindNextLine(self.cy, -1)
        if self.cy ==None: self.cy =0

    # ------------------------------------------------------------------

    def OnFold(self):
        self.Fold()

    # ------------------------------------------------------------------

    def OnInit(self):
        #self.FoldAll()
        pass

