
from wxPython.wx import *
from wxPython.stc import *

#----------------------------------------------------------------------

demoText = """\

This editor is provided by a class named wxStyledTextCtrl.  As
the name suggests, you can define styles that can be applied to
sections of text.  This will typically be used for things like
syntax highlighting code editors, but I'm sure that there are other
applications as well.  A style is a combination of font, point size,
forground and background colours.  The editor can handle
proportional fonts just as easily as monospaced fonts, and various
styles can use different sized fonts.

There are a few canned language lexers and colourizers included,
(see the next demo) or you can handle the colourization yourself.
If you do you can simply register an event handler and the editor
will let you know when the visible portion of the text needs
styling.

wxStyledTextEditor also supports setting markers in the margin...




...and indicators within the text.  You can use these for whatever
you want in your application.  Cut, Copy, Paste, Drag and Drop of
text works, as well as virtually unlimited Undo and Redo
capabilities, (right click to try it out.)

"""

if wxPlatform == '__WXMSW__':
    face1 = 'Arial'
    face2 = 'Times New Roman'
    face3 = 'Courier New'
    pb = 6
else:
    face1 = 'Helvetica'
    face2 = 'Times'
    face3 = 'Courier'
    pb = 10


#----------------------------------------------------------------------
# This shows how to catch the Modified event from the wxStyledTextCtrl

class MySTC(wxStyledTextCtrl):
    def __init__(self, parent, ID, log):
        wxStyledTextCtrl.__init__(self, parent, ID)
        self.log = log

        EVT_STC_MODIFIED(self, ID, self.OnModified)


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
                                  evt.GetText() ))

    def transModType(self, modType):
        st = ""
        table = [(wxSTC_MOD_INSERTTEXT, "InsertText"),
                 (wxSTC_MOD_DELETETEXT, "DeleteText"),
                 (wxSTC_MOD_CHANGESTYLE, "ChangeStyle"),
                 (wxSTC_MOD_CHANGEFOLD, "ChangeFold"),
                 (wxSTC_PERFORMED_USER, "UserFlag"),
                 (wxSTC_PERFORMED_UNDO, "Undo"),
                 (wxSTC_PERFORMED_REDO, "Redo"),
                 (wxSTC_LASTSTEPINUNDOREDO, "Last-Undo/Redo"),
                 (wxSTC_MOD_CHANGEMARKER, "ChangeMarker"),
                 (wxSTC_MOD_BEFOREINSERT, "B4-Insert"),
                 (wxSTC_MOD_BEFOREDELETE, "B4-Delete")
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
        p = wxPanel(nb, -1)
        ed = MySTC(p, -1, log)
        s = wxBoxSizer(wxHORIZONTAL)
        s.Add(ed, 1, wxEXPAND)
        p.SetSizer(s)
        p.SetAutoLayout(true)

    ed.SetText(demoText)
    ed.EmptyUndoBuffer()

    # make some styles
    ed.StyleSetSpec(wxSTC_STYLE_DEFAULT, "size:%d,face:%s" % (pb+2, face3))
    ed.StyleSetSpec(1, "size:%d,bold,face:%s,fore:#0000FF" % (pb+3, face1))
    ed.StyleSetSpec(2, "face:%s,italic,fore:#FF0000,size:%d" % (face2, pb+2))
    ed.StyleSetSpec(3, "face:%s,bold,size:%d" % (face2, pb+3))
    ed.StyleSetSpec(4, "face:%s,size:%d" % (face1, pb))


    # now set some text to those styles...  Normally this would be
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
    ed.SetMarginType(0, wxSTC_MARGIN_NUMBER)
    ed.SetMarginWidth(0, 22)
    ed.StyleSetSpec(wxSTC_STYLE_LINENUMBER, "size:%d,face:%s" % (pb, face1))

    # setup some markers
    ed.SetMarginType(1, wxSTC_MARGIN_SYMBOL)
    ed.MarkerDefine(0, wxSTC_MARK_ROUNDRECT, "#CCFF00", "RED")
    ed.MarkerDefine(1, wxSTC_MARK_CIRCLE, "FOREST GREEN", "SIENNA")
    ed.MarkerDefine(2, wxSTC_MARK_SHORTARROW, "blue", "blue")
    ed.MarkerDefine(3, wxSTC_MARK_ARROW, "#00FF00", "#00FF00")

    # put some markers on some lines
    ed.MarkerAdd(17, 0)
    ed.MarkerAdd(18, 1)
    ed.MarkerAdd(19, 2)
    ed.MarkerAdd(20, 3)
    ed.MarkerAdd(20, 0)


    # and finally, an indicator or two
    ed.IndicatorSetStyle(0, wxSTC_INDIC_SQUIGGLE)
    ed.IndicatorSetForeground(0, wxRED)
    ed.IndicatorSetStyle(1, wxSTC_INDIC_DIAGONAL)
    ed.IndicatorSetForeground(1, wxBLUE)
    ed.IndicatorSetStyle(2, wxSTC_INDIC_STRIKE)
    ed.IndicatorSetForeground(2, wxRED)

    ed.StartStyling(836, wxSTC_INDICS_MASK)
    ed.SetStyling(10, wxSTC_INDIC0_MASK)
    ed.SetStyling(10, wxSTC_INDIC1_MASK)
    ed.SetStyling(10, wxSTC_INDIC2_MASK | wxSTC_INDIC1_MASK)


    return p



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


