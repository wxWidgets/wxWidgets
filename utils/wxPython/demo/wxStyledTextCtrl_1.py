
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
you want in your application.  Drag and Drop of text works, as well
as virtually unlimited Undo and Redo capabilities, (right click to
try it out.)

"""


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    ed = wxStyledTextCtrl(nb, -1)

    ed.SetText(demoText)

    ed.SetMarginType(0, wxSTC_MARGIN_NUMBER)
    ed.SetMarginWidth(0, 22)
    ed.StyleSetSpec(wxSTC_STYLE_LINENUMBER, "size:6,face:Ariel")

    ed.SetMarginType(1, wxSTC_MARGIN_SYMBOL)
    ed.MarkerDefine(0, wxSTC_MARK_ROUNDRECT, "#DD0FCC", "RED")
    ed.MarkerDefine(1, wxSTC_MARK_CIRCLE, "FOREST GREEN", "SIENNA")
    ed.MarkerDefine(2, wxSTC_MARK_SHORTARROW, "blue", "blue")
    ed.MarkerDefine(3, wxSTC_MARK_ARROW, "#00FF00", "#00FF00")

    ed.MarkerAdd(17, 0)
    ed.MarkerAdd(18, 1)
    ed.MarkerAdd(19, 2)
    ed.MarkerAdd(20, 3)
    ed.MarkerAdd(20, 0)


    return ed



#----------------------------------------------------------------------


overview = """\
"""



if __name__ == '__main__':
    import sys
    app = wxPySimpleApp()
    frame = wxFrame(None, -1, "Tester...", size=(640, 480))
    win = runTest(frame, frame, sys.stdout)
    frame.Show(true)
    app.MainLoop()


