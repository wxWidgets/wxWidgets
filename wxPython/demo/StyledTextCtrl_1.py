# 
# 11/21/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o wx.TheClipboard.Flush() generates a warning on program exit.
# 

import  wx
import  wx.stc  as  stc

import  images

#----------------------------------------------------------------------

debug = 1


demoText = """\
This editor is provided by a class named wx.StyledTextCtrl.  As
the name suggests, you can define styles that can be applied to
sections of text.  This will typically be used for things like
syntax highlighting code editors, but I'm sure that there are other
applications as well.  A style is a combination of font, point size,
foreground and background colours.  The editor can handle
proportional fonts just as easily as monospaced fonts, and various
styles can use different sized fonts.

There are a few canned language lexers and colourizers included,
(see the next demo) or you can handle the colourization yourself.
If you do you can simply register an event handler and the editor
will let you know when the visible portion of the text needs
styling.

wx.StyledTextEditor also supports setting markers in the margin...




...and indicators within the text.  You can use these for whatever
you want in your application.  Cut, Copy, Paste, Drag and Drop of
text works, as well as virtually unlimited Undo and Redo
capabilities, (right click to try it out.)
"""

if wx.Platform == '__WXMSW__':
    face1 = 'Arial'
    face2 = 'Times New Roman'
    face3 = 'Courier New'
    pb = 10
else:
    face1 = 'Helvetica'
    face2 = 'Times'
    face3 = 'Courier'
    pb = 12


#----------------------------------------------------------------------
# This shows how to catch the Modified event from the wx.StyledTextCtrl

class MySTC(stc.StyledTextCtrl):
    def __init__(self, parent, ID, log):
        stc.StyledTextCtrl.__init__(self, parent, ID)
        self.log = log

        self.Bind(stc.EVT_STC_DO_DROP, self.OnDoDrop)
        self.Bind(stc.EVT_STC_DRAG_OVER, self.OnDragOver)
        self.Bind(stc.EVT_STC_START_DRAG, self.OnStartDrag)
        self.Bind(stc.EVT_STC_MODIFIED, self.OnModified)

        self.Bind(wx.EVT_WINDOW_DESTROY, self.OnDestroy)

    def OnDestroy(self, evt):
        # This is how the clipboard contents can be preserved after
        # the app has exited.
        wx.TheClipboard.Flush()
        evt.Skip()


    def OnStartDrag(self, evt):
        self.log.write("OnStartDrag: %d, %s\n"
                       % (evt.GetDragAllowMove(), evt.GetDragText()))

        if debug and evt.GetPosition() < 250:
            evt.SetDragAllowMove(False)     # you can prevent moving of text (only copy)
            evt.SetDragText("DRAGGED TEXT") # you can change what is dragged
            #evt.SetDragText("")             # or prevent the drag with empty text


    def OnDragOver(self, evt):
        self.log.write(
            "OnDragOver: x,y=(%d, %d)  pos: %d  DragResult: %d\n"
            % (evt.GetX(), evt.GetY(), evt.GetPosition(), evt.GetDragResult())
            )

        if debug and evt.GetPosition() < 250:
            evt.SetDragResult(wx.DragNone)   # prevent dropping at the beginning of the buffer


    def OnDoDrop(self, evt):
        self.log.write("OnDoDrop: x,y=(%d, %d)  pos: %d  DragResult: %d\n"
                       "\ttext: %s\n"
                       % (evt.GetX(), evt.GetY(), evt.GetPosition(), evt.GetDragResult(),
                          evt.GetDragText()))

        if debug and evt.GetPosition() < 500:
            evt.SetDragText("DROPPED TEXT")  # Can change text if needed
            #evt.SetDragResult(wx.DragNone)  # Can also change the drag operation, but it
                                             # is probably better to do it in OnDragOver so
                                             # there is visual feedback

            #evt.SetPosition(25)             # Can also change position, but I'm not sure why
                                             # you would want to...




    def OnModified(self, evt):
        self.log.write("""OnModified
        Mod type:     %s
        At position:  %d
        Lines added:  %d
        Text Length:  %d
        Text:         %s\n""" % ( self.transModType(evt.GetModificationType()),
                                  evt.GetPosition(),
                                  evt.GetLinesAdded(),
                                  evt.GetLength(),
                                  repr(evt.GetText()) ))


    def transModType(self, modType):
        st = ""
        table = [(stc.STC_MOD_INSERTTEXT, "InsertText"),
                 (stc.STC_MOD_DELETETEXT, "DeleteText"),
                 (stc.STC_MOD_CHANGESTYLE, "ChangeStyle"),
                 (stc.STC_MOD_CHANGEFOLD, "ChangeFold"),
                 (stc.STC_PERFORMED_USER, "UserFlag"),
                 (stc.STC_PERFORMED_UNDO, "Undo"),
                 (stc.STC_PERFORMED_REDO, "Redo"),
                 (stc.STC_LASTSTEPINUNDOREDO, "Last-Undo/Redo"),
                 (stc.STC_MOD_CHANGEMARKER, "ChangeMarker"),
                 (stc.STC_MOD_BEFOREINSERT, "B4-Insert"),
                 (stc.STC_MOD_BEFOREDELETE, "B4-Delete")
                 ]

        for flag,text in table:
            if flag & modType:
                st = st + text + " "

        if not st:
            st = 'UNKNOWN'

        return st




#----------------------------------------------------------------------

_USE_PANEL = 1

def runTest(frame, nb, log):
    if not _USE_PANEL:
        ed = p = MySTC(nb, -1, log)

    else:
        p = wx.Panel(nb, -1, style=wx.NO_FULL_REPAINT_ON_RESIZE)
        ed = MySTC(p, -1, log)
        s = wx.BoxSizer(wx.HORIZONTAL)
        s.Add(ed, 1, wx.EXPAND)
        p.SetSizer(s)
        p.SetAutoLayout(True)


    #ed.SetBufferedDraw(False)
    #ed.StyleClearAll()
    #ed.SetScrollWidth(800)
    #ed.SetWrapMode(True)
    #ed.SetUseAntiAliasing(False)    
    #ed.SetViewEOL(True)

    #ed.CmdKeyClear(stc.STC_KEY_BACK,
    #               stc.STC_SCMOD_CTRL)
    #ed.CmdKeyAssign(stc.STC_KEY_BACK,
    #                stc.STC_SCMOD_CTRL,
    #                stc.STC_CMD_DELWORDLEFT)

    ed.SetText(demoText)

    if wx.USE_UNICODE:
        import codecs
        decode = codecs.lookup("utf-8")[1]

        ed.GotoPos(ed.GetLength())
        ed.AddText("\n\nwx.StyledTextCtrl can also do Unicode:\n")
        uniline = ed.GetCurrentLine()
        unitext, l = decode('\xd0\x9f\xd0\xb8\xd1\x82\xd0\xbe\xd0\xbd - '
                            '\xd0\xbb\xd1\x83\xd1\x87\xd1\x88\xd0\xb8\xd0\xb9 '
                            '\xd1\x8f\xd0\xb7\xd1\x8b\xd0\xba \xd0\xbf\xd1\x80\xd0\xbe\xd0\xb3\xd1\x80\xd0\xb0\xd0\xbc\xd0\xbc\xd0\xb8\xd1\x80\xd0\xbe\xd0\xb2\xd0\xb0\xd0\xbd\xd0\xb8\xd1\x8f!\n\n')
        ed.AddText('\tRussian: ')
        ed.AddText(unitext)
        ed.GotoPos(0)
    #else:
    #    #ed.StyleSetFontEncoding(stc.STC_STYLE_DEFAULT, wx.FONTENCODING_KOI8)
    #    #text = u'\u041f\u0438\u0442\u043e\u043d - \u043b\u0443\u0447\u0448\u0438\u0439 \u044f\u0437\u044b\u043a \n\u043f\u0440\u043e\u0433\u0440\u0430\u043c\u043c\u0438\u0440\u043e\u0432\u0430\u043d\u0438\u044f!'
    #    #text = text.encode('koi8-r')
    #    #ed.StyleSetFontEncoding(stc.STC_STYLE_DEFAULT, wx.FONTENCODING_BIG5)
    #    #text = u'Python \u662f\u6700\u597d\u7684\u7de8\u7a0b\u8a9e\u8a00\uff01'
    #    #text = text.encode('big5')
    #    ed.GotoPos(ed.GetLength())
    #    ed.AddText('\n\n' + text)
        
    ed.EmptyUndoBuffer()

    # make some styles
    ed.StyleSetSpec(stc.STC_STYLE_DEFAULT, "size:%d,face:%s" % (pb, face3))
    ed.StyleClearAll()
    ed.StyleSetSpec(1, "size:%d,bold,face:%s,fore:#0000FF" % (pb+2, face1))
    ed.StyleSetSpec(2, "face:%s,italic,fore:#FF0000,size:%d" % (face2, pb))
    ed.StyleSetSpec(3, "face:%s,bold,size:%d" % (face2, pb+2))
    ed.StyleSetSpec(4, "face:%s,size:%d" % (face1, pb-1))

    # Now set some text to those styles...  Normally this would be
    # done in an event handler that happens when text needs displayed.
    ed.StartStyling(98, 0xff)
    ed.SetStyling(6, 1)  # set style for 6 characters using style 1

    ed.StartStyling(190, 0xff)
    ed.SetStyling(20, 2)

    ed.StartStyling(310, 0xff)
    ed.SetStyling(4, 3)
    ed.SetStyling(2, 0)
    ed.SetStyling(10, 4)


    # line numbers in the margin
    ed.SetMarginType(0, stc.STC_MARGIN_NUMBER)
    ed.SetMarginWidth(0, 22)
    ed.StyleSetSpec(stc.STC_STYLE_LINENUMBER, "size:%d,face:%s" % (pb, face1))

    # setup some markers
    ed.SetMarginType(1, stc.STC_MARGIN_SYMBOL)
    ed.MarkerDefine(0, stc.STC_MARK_ROUNDRECT, "#CCFF00", "RED")
    ed.MarkerDefine(1, stc.STC_MARK_CIRCLE, "FOREST GREEN", "SIENNA")
    ed.MarkerDefine(2, stc.STC_MARK_SHORTARROW, "blue", "blue")
    ed.MarkerDefine(3, stc.STC_MARK_ARROW, "#00FF00", "#00FF00")

    # put some markers on some lines
    ed.MarkerAdd(17, 0)
    ed.MarkerAdd(18, 1)
    ed.MarkerAdd(19, 2)
    ed.MarkerAdd(20, 3)
    ed.MarkerAdd(20, 0)


    # and finally, an indicator or two
    ed.IndicatorSetStyle(0, stc.STC_INDIC_SQUIGGLE)
    ed.IndicatorSetForeground(0, wx.RED)
    ed.IndicatorSetStyle(1, stc.STC_INDIC_DIAGONAL)
    ed.IndicatorSetForeground(1, wx.BLUE)
    ed.IndicatorSetStyle(2, stc.STC_INDIC_STRIKE)
    ed.IndicatorSetForeground(2, wx.RED)

    ed.StartStyling(836, stc.STC_INDICS_MASK)
    ed.SetStyling(10, stc.STC_INDIC0_MASK)
    ed.SetStyling(10, stc.STC_INDIC1_MASK)
    ed.SetStyling(10, stc.STC_INDIC2_MASK | stc.STC_INDIC1_MASK)


    # some test stuff...
    if debug:
        print "GetTextLength(): ", ed.GetTextLength(), len(ed.GetText())
        print "GetText(): ", repr(ed.GetText())
        print
        print "GetStyledText(98, 104): ", repr(ed.GetStyledText(98, 104)), len(ed.GetStyledText(98, 104))
        print
        print "GetCurLine(): ", repr(ed.GetCurLine())
        ed.GotoPos(5)
        print "GetCurLine(): ", repr(ed.GetCurLine())
        print
        print "GetLine(1): ", repr(ed.GetLine(1))
        print
        ed.SetSelection(25, 35)
        print "GetSelectedText(): ", repr(ed.GetSelectedText())
        print "GetTextRange(25, 35): ", repr(ed.GetTextRange(25, 35))
        print "FindText(0, max, 'indicators'): ",
        print ed.FindText(0, ed.GetTextLength(), "indicators")
        if wx.USE_UNICODE:
            end = ed.GetLength()
            start = ed.PositionFromLine(uniline)
            print "GetTextRange(%d, %d): " % (start, end),
            print repr(ed.GetTextRange(start, end))


    wx.CallAfter(ed.GotoPos, 0)
    return p



#----------------------------------------------------------------------


overview = """\
<html><body>
Once again, no docs yet.  <b>Sorry.</b>  But <a href="data/stc.h.html">this</a>
and <a href="http://www.scintilla.org/ScintillaDoc.html">this</a> should
be helpful.
</body><html>
"""


if __name__ == '__main__':
    import sys,os
    import run
    run.main(['', os.path.basename(sys.argv[0])] + sys.argv[1:])

