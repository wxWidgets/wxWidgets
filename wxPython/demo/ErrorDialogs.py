# demo for ErrorDialogs.py
# usual wxWindows license stuff here.
# by Chris Fama, with thanks to Robin Dunn, and others on the wxPython-users
# mailing list.
#
# 11/22/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Updated for wx namespace
# 
# 11/25/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o Looks like we have issues until the library is updated.
#   - Had to rename back to wx* naming conventions
#   - Getting unusual failures in the library itself when that is done.
#

import  sys

import  wx
import  wx.lib.ErrorDialogs as  edlg

_debug = 0

ID_TEXT = 10000
ID_BUTTON_wxPyNonFatalError = 10001
ID_BUTTON_wxPyFatalError = 10002
ID_BUTTON_wxPyFatalErrorDialog = 10003
ID_BUTTON_wxPyNonFatalErrorDialog = 10004
ID_BUTTON_wxPyFatalErrorDialogWithTraceback = 10005
ID_BUTTON_wxPyNonFatalErrorDialogWithTraceback = 10006

def ErrorDialogsDemoPanelFunc( parent, call_fit = True, set_sizer = True ):
    item0 = wx.BoxSizer( wx.VERTICAL )

    item1 = wx.StaticText(
        parent, ID_TEXT, 
        "Please select one of the buttons below for an example using explicit errors..."
        )
        
    item0.AddWindow( item1, 0, wx.ALIGN_CENTRE|wx.ALL, 5 )

    item2 = wx.FlexGridSizer( 0, 2, 0, 0 )

    item3 = wx.Button( parent, ID_BUTTON_wxPyNonFatalError, "wxPyNonFatalError")
    item2.AddWindow( item3, 0, wx.ALIGN_CENTRE|wx.ALL, 5 )

    item4 = wx.Button( parent, ID_BUTTON_wxPyFatalError, "wxPyFatalError")
    item2.AddWindow( item4, 0, wx.ALIGN_CENTRE|wx.ALL, 5 )

    item0.AddSizer( item2, 0, wx.ALIGN_CENTRE|wx.ALL, 5 )

    item5 = wx.StaticText( parent, ID_TEXT, "Please select one of the buttons below for interpreter errors...")
    item0.AddWindow( item5, 0, wx.ALIGN_CENTRE|wx.ALL, 5 )

    item6 = wx.FlexGridSizer( 0, 2, 0, 0 )

    item7 = wx.Button( parent, ID_BUTTON_wxPyFatalErrorDialog, "wxPyFatalErrorDialog")
    item6.AddWindow( item7, 0, wx.ALIGN_CENTRE|wx.ALL, 5 )

    item8 = wx.Button( parent, ID_BUTTON_wxPyNonFatalErrorDialog, "wxPyNonFatalErrorDialog")
    item6.AddWindow( item8, 0, wx.ALIGN_CENTRE|wx.ALL, 5 )

    item9 = wx.Button(
        parent, ID_BUTTON_wxPyFatalErrorDialogWithTraceback, 
        "wxPyFatalErrorDialogWithTraceback"
        )
    item6.AddWindow( item9, 0, wx.ALIGN_CENTRE|wx.ALL, 5 )

    item10 = wx.Button(
        parent, ID_BUTTON_wxPyNonFatalErrorDialogWithTraceback, 
        "wxPyNonFatalErrorDialogWithTraceback"
        )
    item10.SetDefault()
    item6.AddWindow( item10, 0, wx.ALIGN_CENTRE|wx.ALL, 5 )

    item0.AddSizer( item6, 0, wx.ALIGN_CENTRE|wx.ALL, 5 )

    item11 = wx.FlexGridSizer( 0, 2, 0, 0 )

    item0.AddSizer( item11, 0, wx.ALIGN_CENTRE|wx.ALL, 5 )

    if set_sizer == True:
        parent.SetAutoLayout( True )
        parent.SetSizer( item0 )
        if call_fit == True:
            item0.Fit( parent )
            item0.SetSizeHints( parent )

    return item0

# Menu bar functions

# Bitmap functions


# End of generated file

class MyPanel(wx.Panel):
    def __init__(self,parent=None):
        wx.Panel.__init__(self,parent,-1)

        args = (None, -1)
        kwargs = {
            'programname': "sumthing",
            'mailto': "me@sumwear",
            'whendismissed': "from wxPython.wx import * ; wxBell()"
            }

        self.dialogs = map(apply,
                           [edlg.wxPyNonFatalErrorDialogWithTraceback,
                            edlg.wxPyNonFatalErrorDialog,#WithTraceback
                            edlg.wxPyFatalErrorDialogWithTraceback,
                            edlg.wxPyFatalErrorDialog #WithTraceback
                            ],
                           (args,) * 4,
                           (kwargs,) * 4
                           )

        ErrorDialogsDemoPanelFunc(self)

        self.Bind(wx.EVT_BUTTON, self.DoDialog, id=ID_BUTTON_wxPyFatalErrorDialog)
        self.Bind(wx.EVT_BUTTON, self.DoDialog, id=ID_BUTTON_wxPyNonFatalError)
        self.Bind(wx.EVT_BUTTON, self.DoDialog, id=ID_BUTTON_wxPyFatalError)
        self.Bind(wx.EVT_BUTTON, self.DoDialog, id=ID_BUTTON_wxPyFatalErrorDialogWithTraceback)
        self.Bind(wx.EVT_BUTTON, self.DoDialog, id=ID_BUTTON_wxPyNonFatalErrorDialog)
        self.Bind(wx.EVT_BUTTON, self.DoDialog, id=ID_BUTTON_wxPyNonFatalErrorDialogWithTraceback)

    IndexFromID = {
        ID_BUTTON_wxPyFatalErrorDialog: 3,
        ID_BUTTON_wxPyFatalErrorDialogWithTraceback: 2,
        ID_BUTTON_wxPyNonFatalErrorDialog: 1,
        ID_BUTTON_wxPyNonFatalErrorDialogWithTraceback: 0
        }

    def DoDialog(self,event):
        id = event.GetId()

        if id in [ID_BUTTON_wxPyFatalError,ID_BUTTON_wxPyNonFatalError]:
            if id == ID_BUTTON_wxPyFatalError:
                print "%s.DoDialog(): testing explicit wxPyFatalError..."\
                      % self
                edlg.wxPyFatalError(self,"Test Non-fatal error.<p>"
                               "Nearly arbitrary HTML (i.e., that which is"
                               " understood by <B><I>wxHtmlWindow</i></b>)."
                               "<p><table border=\"2\"><tr><td>This</td><td>is</td></tr>"
                               "<tr><td>a</td><td>table</td></tr></table></p>")
            else:
                print "%s.DoDialog(): testing explicit wxPyNonFatalError..."\
                      % self
                edlg.wxPyNonFatalError(self,"Test Non-fatal error.<p>"
                                  "Nearly arbitrary HTML (i.e., that which is"
                                  " understood by <B><I>wxHtmlWindow</i></b>)."
                                  "<p><table border=\"2\"><tr><td>This</td><td>is</td></tr>"
                                  "<tr><td>a</td><td>table</td></tr></table></p>")
        else:
            sys.stderr = self.dialogs[self.IndexFromID[id]]
            print "%s.DoDialog(): testing %s..." % (self,sys.stderr)
            this_will_generate_a_NameError_exception

    def ShutdownDemo(self):
        for d in self.dialogs:
            d.Destroy()



class MyFrame(wx.Frame):
    def __init__(self,parent=None):
        wx.Frame.__init__(self,parent,-1,
                         "Please make a selection...",
                         )
        self.panel = MyPanel(self)
        self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)

    def OnCloseWindow(self,event):
        self.panel.Close()
        self.Destroy()

class MyApp(wx.App):
    def OnInit(self):
        frame = MyFrame()
        frame.Show(True)
        self.SetTopWindow(frame)
        return True

def runTest(pframe, nb, log):
    panel = MyPanel(nb)
    return panel

edlg._debug = 1

if __name__ == "__main__":
    sys.stderr = edlg.wxPyNonWindowingErrorHandler()
    app = MyApp(0)
    app.MainLoop()
    sys.exit()
else:
    overview = edlg.__doc__
